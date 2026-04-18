#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> // WIFEXITED, WEXITSTATUS

// --- ANSI Color Codes (zur besseren Lesbarkeit der Ausgaben) ---
#define COLOR_INFO "\033[94m"
#define COLOR_SUCCESS "\033[92m"
#define COLOR_ERROR "\033[31m"
#define COLOR_ENDC "\033[0m"

// --- Konfiguration ---
const char *LOG_TAG = "[TORSQUID]";
const char *TRANS_PORT = "9040";
const char *DNS_PORT = "53";
// PRIVATE_NETS ist fuer die iptables-Regel erforderlich.
// Wird nun manuell und sicher geparst.
const char *PRIVATE_NETS_STR = "192.168.1.0/24 192.168.0.0/24 10.0.0.0/8 172.16.0.0/12 127.0.0.0/8"; 

// --- Hilfsfunktionen ---

void log_message(const char *color, const char *type, const char *message) {
    printf("%s %s %s%s %s%s\n", LOG_TAG, color, type, COLOR_ENDC, message, COLOR_ENDC);
}

void log_info(const char *message) {
    log_message(COLOR_INFO, "[INFO]", message);
}

void log_success(const char *message) {
    log_message(COLOR_SUCCESS, "[SUCCESS]", message);
}

void log_error(const char *message) {
    log_message(COLOR_ERROR, "[ERROR]", message);
}

/**
 * @brief Fuehrt einen Shell-Befehl aus und ueberprueft den Rueckgabewert.
 * @param cmd Der auszufuehrende Befehl.
 * @return 0 bei Erfolg, ansonsten der Exit-Status des Befehls.
 */
int execute_command(const char *cmd) {
    int status = system(cmd);
    if (status == -1) {
        log_error("system() call failed.");
        return -1;
    }
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        // Befehl fehlgeschlagen
        return WEXITSTATUS(status);
    }
    return 0;
}

/**
 * @brief Hilfsfunktion, um die UID des 'debian-tor' Benutzers zu ermitteln.
 * @return Die UID als dynamisch allozierte Zeichenkette (z.B. "120"). Muss freigegeben werden.
 */
char *get_tor_uid() {
    FILE *fp;
    char buffer[16];
    char *tor_uid = NULL;

    fp = popen("id -u debian-tor 2>/dev/null", "r");
    if (fp == NULL) {
        log_error("Failed to run 'id -u debian-tor'. Using fallback '120'.");
        return strdup("120");
    }

    if (fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {
        // Entferne eventuelle Zeilenumbrueche
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        tor_uid = strdup(buffer);
    } else {
        log_info("Tor user 'debian-tor' not found. Using fallback '120'.");
        tor_uid = strdup("120"); 
    }
    pclose(fp);
    return tor_uid;
}


// --- KERFUNKTION: IPTABLES ZURUECKSETZEN ---
void reset_iptables() {
    log_info("Setze iptables Policies auf ACCEPT zurueck.");
    execute_command("iptables -P INPUT ACCEPT");
    execute_command("iptables -P FORWARD ACCEPT");
    execute_command("iptables -P OUTPUT ACCEPT");

    log_info("Flushe (Lösche) alle iptables-Regeln.");
    execute_command("iptables -t nat -F");
    execute_command("iptables -t mangle -F");
    execute_command("iptables -F");
    execute_command("iptables -X"); 
}

/**
 * @brief Iteriert sicher ueber die IP-Netze und fuegt iptables-Regeln hinzu.
 * @param net_string Der String mit den durch Leerzeichen getrennten Netzen.
 * @param chain Die iptables Kette (z.B. "OUTPUT").
 * @param table Die iptables Tabelle (z.B. "-t nat" oder "").
 * @param target Das Ziel-JUMP (z.B. "RETURN" oder "ACCEPT").
 */
void process_private_nets(const char *net_string, const char *chain, const char *table, const char *target) {
    char net[32]; // Buffer fuer ein einzelnes Netz (z.B. "192.168.1.0/24")
    const char *start = net_string;
    const char *end;
    char cmd[256];
    
    // Iteriere, solange wir noch nicht am Ende des Strings sind
    while (*start != '\0') {
        // Überspringe führende Leerzeichen
        while (*start == ' ') {
            start++;
        }
        if (*start == '\0') break; // Ende des Strings erreicht

        // Finde das Ende des aktuellen Netzes
        end = strchr(start, ' ');
        if (end == NULL) {
            end = start + strlen(start); // Letztes Element
        }

        // Kopiere das Netz in den Puffer
        size_t len = end - start;
        if (len >= sizeof(net)) len = sizeof(net) - 1; // Sicherstellen, dass es in den Puffer passt
        strncpy(net, start, len);
        net[len] = '\0';

        // Erzeuge und fuehre den iptables-Befehl aus
        snprintf(cmd, sizeof(cmd), "iptables %s -A %s -d %s -j %s", table, chain, net, target);
        execute_command(cmd);

        // Gehe zum nächsten Element
        start = end;
    }
}


// --- KERNFUNKTION: TOR-PROXY STARTEN ---
void start_torschema() {
    char *tor_uid = get_tor_uid();
    log_info("Starte transparente Tor-Umleitung...");

    // 1. Tor-Dienst starten
    log_info("Starte Tor-Dienst...");
    execute_command("service tor start");
    sleep(3);

    // 2. Initiales FLUSHEN der Regeln (WICHTIG!)
    reset_iptables();

    // 3. Setzen der iptables-Regeln
    log_info("Setze IPTABLES-Regeln fuer transparentes Proxying. ");
    
    char cmd[512]; // Großer Puffer fuer Befehle

    // ----------------------------------------------------
    // NAT TABLE: Traffic-Umleitung
    // ----------------------------------------------------
    
    // 1. Ausschluss des Tor-UID-Verkehrs
    snprintf(cmd, sizeof(cmd), "iptables -t nat -A OUTPUT -m owner --uid-owner %s -j RETURN", tor_uid);
    execute_command(cmd);
    log_info("NAT: Tor-UID ausgeschlossen.");

    // 2. DNS-Umleitung (UDP Port 53)
    snprintf(cmd, sizeof(cmd), "iptables -t nat -A OUTPUT -p udp --dport %s -j REDIRECT --to-ports %s", DNS_PORT, DNS_PORT);
    execute_command(cmd);
    
    // 3. Ausschluss lokaler Netze (Loopback / LAN)
    process_private_nets(PRIVATE_NETS_STR, "OUTPUT", "-t nat", "RETURN");
    log_info("NAT: Lokale Netze ausgeschlossen.");
    
    // 4. Transparente Proxy-Regel: Leite den gesamten verbleibenden TCP-Verkehr um
    snprintf(cmd, sizeof(cmd), "iptables -t nat -A OUTPUT -p tcp --syn -j REDIRECT --to-ports %s", TRANS_PORT);
    execute_command(cmd);
    log_info("NAT: TCP-Verkehr an Tor umgeleitet.");


    // ----------------------------------------------------
    // FILTER TABLE: Leak-Schutz
    // ----------------------------------------------------
    
    // 1. Erlaube etablierte Verbindungen
    execute_command("iptables -A OUTPUT -m state --state ESTABLISHED,RELATED -j ACCEPT");
    
    // 2. Erlaube lokale Netze (Erneute Erlaubnis in der Filter-Tabelle)
    // Wir muessen den PRIVATE_NETS_STR String neu initialisieren, da process_private_nets
    // seine interne Kopie (die geparst wurde) zerlegt hat.
    const char *restart_nets = "192.168.1.0/24 192.168.0.0/24 10.0.0.0/8 172.16.0.0/12 127.0.0.0/8";
    process_private_nets(restart_nets, "OUTPUT", "", "ACCEPT"); // Tabelle weglassen = filter

    // 3. Erlaube Tor-Prozess-Verkehr
    snprintf(cmd, sizeof(cmd), "iptables -A OUTPUT -m owner --uid-owner %s -j ACCEPT", tor_uid);
    execute_command(cmd);

    // 4. Default Drop: BLOCKIERE alles andere (Leak-Schutz)
    log_info("FILTER: Default-Policy auf DROP setzen (Leak-Schutz).");
    execute_command("iptables -P OUTPUT DROP");

    free(tor_uid);
    log_success("Transparenter Tor-Proxy erfolgreich gestartet.");
}

// --- KERNFUNKTION: TOR-PROXY STOPPEN ---
void stop_torschema() {
    log_info("Stoppe transparente Tor-Umleitung...");
    
    // 1. IPTABLES zuruecksetzen (WICHTIG!)
    reset_iptables();
    
    // 2. Tor-Dienst stoppen
    log_info("Stoppe Tor-Dienst.");
    execute_command("service tor stop");

    log_success("Tor-Proxy beendet. iptables zurueckgesetzt.");
}

// --- Hauptprogramm ---
int main(int argc, char *argv[]) {
    if (geteuid() != 0) {
        log_error("Das Programm muss mit Root-Rechten (sudo) ausgefuehrt werden.");
        return 1;
    }

    if (argc < 2) {
        printf("Verwendung: %s {start|stop}\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "start") == 0) {
        start_torschema();
    } else if (strcmp(argv[1], "stop") == 0) {
        stop_torschema();
    } else {
        printf("Unbekannter Befehl: %s\n", argv[1]);
        printf("Verwendung: %s {start|stop}\n", argv[0]);
        return 1;
    }

    return 0;
}
