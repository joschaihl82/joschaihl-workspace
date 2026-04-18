/*
 * tor.c - Tor-Client Simulation (Pure C, UNIX-Style, SOCKS5 integriert)
 *
 * Simulierter Fluss:
 * Browser (Mock) -> SOCKS5 Server (Mock) -> Tor Core -> Guard OR (Mock)
 * Der SOCKS5-Server läuft dauerhaft, bis SIGINT (Strg+C) empfangen wird.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h> // Für Signal-Handling

// =================================================================
// GLOBALE KONSTANTEN (KONST)
// =================================================================

#define OR_GUARD_HOST "192.0.2.1"
#define SOCKS_PORT 9050
#define CELL_SIZE 512
#define CELL_CMD_CREATE_FAST 5
#define CELL_CMD_RELAY 3
#define CELL_CMD_DESTROY 4
#define RELAY_CMD_BEGIN 1
#define RELAY_CMD_DATA 2
#define CRYPTO_KEY_LEN 40
#define RELAY_HDR_LEN 11

// Hardkodiertes Verzeichnis-Zertifikat (Platzhalter)
#define OR_DIR_CERT \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDKzCCAhOgAwIBAgIRAP2s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s\n" \
"MIG+MQswCQYDVQQGEwJVUzESMBAGA1UECAwIQ2FsaWZvcm5pYTEUMBIGA1UECgwL\n" \
"VG9yIFByb2plY3QxETAPBgNVBAsMClJvb3QgQXV0aC4xGTAXBgNVBAMMEFRvciBE\n" \
"aXJlY3RvcnkgQ0ExHjAcBgkqhkiG9w0BCQEWD2RpcmVjdG9yeUB0b3Iub3JnMB4X\n" \
"DTk5MTIzMTIzNTk1OVoxDzANBgNVBAMMB1RPUl9HVUFSRA==\n" \
"-----END CERTIFICATE-----\n"

// =================================================================
// GLOBALE ZUSTANDSVARIABLEN (G_)
// =================================================================

uint16_t G_circ_id = 0;   // Aktuelle Circuit ID
int G_or_fd = -1;         // Guard OR File Descriptor (Simulierter Socket)
int G_stream_id = 0;      // Aktuelle Stream ID (SOCKS-Anfrage)
volatile int G_run_flag = 1; // 1 = Laufen, 0 = Beenden (Signal-gesteuert)

// =================================================================
// UTILITY-FUNKTIONEN (UTL)
// =================================================================

// utl_log: Protokolliert eine Nachricht
void utl_log(const char* ctx, const char* msg) {
    printf("[%s] %s\n", ctx, msg);
}

// utl_u16_to_be: Konvertiert 16-Bit Integer zu Big Endian Array (2 Bytes)
void utl_u16_to_be(unsigned char* a, uint16_t i) {
    a[0] = (unsigned char)((i >> 8) & 0xFF);
    a[1] = (unsigned char)(i & 0xFF);
}

// =================================================================
// SIGNAL-HANDLER (SIG)
// =================================================================

// sig_hdl: Behandelt SIGINT (Strg+C) und setzt die Lauf-Flagge
void sig_hdl(int sig) {
    if (sig == SIGINT) {
        utl_log("SIGNAL", "SIGINT (Strg+C) empfangen. Beende Loop...");
        G_run_flag = 0;
    }
}

// =================================================================
// TOR-KERN-FUNKTIONEN (TOR) & NETZWERK-MOCK (NET)
// =================================================================

// net_conn: Simuliert TLS-Verbindung zum Guard-OR
int net_conn(const char* host, int port) {
    utl_log("NET", "Dummy-TLS-Verbindung (OR) initialisiert.");
    printf("NET: Verbindung zu %s:%d (SIMULIERT)\n", host, port);
    G_or_fd = 101; // Simulierter FD
    return G_or_fd;
}

// net_write_cell: Simuliert das Senden einer Tor-Zelle
void net_write_cell(int fd, uint16_t circ_id, uint8_t cmd, 
                    uint8_t rly_cmd, const char* pay, size_t pay_len) {
    // Implementierung weggelassen, da nur die Log-Ausgabe für die Simulation wichtig ist
    printf("NET: Tor-Zelle gesendet: FD=%d, CircID=%u, Cmd=%d, RlyCmd=%d (Payload: %zu Bytes)\n",
           fd, circ_id, cmd, rly_cmd, pay_len);
}

// net_close: Simuliert das Schließen einer Verbindung
void net_close(int fd) {
    utl_log("NET", "Dummy-Verbindung geschlossen.");
    G_or_fd = -1;
}

// tor_mk_circ: Erstellt den Circuit (CREATE_FAST / CREATED_FAST)
int tor_mk_circ(int fd, uint16_t circ_id) {
    char dummy_key[CRYPTO_KEY_LEN];
    memset(dummy_key, 0xBB, CRYPTO_KEY_LEN);
    
    net_write_cell(fd, circ_id, CELL_CMD_CREATE_FAST, 0, dummy_key, CRYPTO_KEY_LEN);
    utl_log("TOR", "CELL_CMD_CREATE_FAST gesendet.");
    printf("TOR: CREATED_FAST empfangen. Circ %u ist nun offen. (SIMULIERT)\n", circ_id);
    return 1;
}

// tor_init: Initialisiert den Tor-Client (Verbindung und Circuit-Aufbau)
int tor_init() {
    utl_log("TOR", "Initialisierung gestartet.");
    
    int fd = net_conn(OR_GUARD_HOST, 9001);
    if (fd < 0) return 0;
    
    G_circ_id = 101;
    if (tor_mk_circ(fd, G_circ_id) == 0) return 0;
    
    utl_log("TOR", "Initialisierung abgeschlossen. Circuit ist OPEN.");
    return 1;
}

// tor_conn_stream: Baut einen Stream im Circuit auf (RELAY_BEGIN)
int tor_conn_stream(int fd, const char* host, int port) {
    if (G_circ_id == 0) return 0;
    
    G_stream_id++; // Neue Stream ID
    char hp_pay[256];
    int len = snprintf(hp_pay, sizeof(hp_pay), "%s:%d", host, port);
    
    net_write_cell(fd, G_circ_id, CELL_CMD_RELAY, RELAY_CMD_BEGIN, hp_pay, len + 1);
    utl_log("TOR", "RELAY_BEGIN gesendet.");
    printf("TOR: RELAY_CONNECTED empfangen. Stream %d ist offen. (SIMULIERT)\n", G_stream_id);
    
    return G_stream_id;
}

// tor_send: Sendet Daten (RELAY_DATA)
void tor_send(int fd, int stream_id, const char* data) {
    if (G_circ_id == 0 || G_stream_id == 0) return;
    net_write_cell(fd, G_circ_id, CELL_CMD_RELAY, RELAY_CMD_DATA, data, strlen(data));
    utl_log("TOR", "RELAY_DATA gesendet.");
}

// tor_recv: Empfängt Daten (Simulierte RELAY_DATA)
void tor_recv(char* buf, size_t max_len) {
    const char* dummy_response = "HTTP/1.1 200 OK\r\nContent-Length: 28\r\n\r\nProxy-Antwort ueber Tor Mock!";
    size_t copy_len = strlen(dummy_response);
    if (copy_len > max_len) copy_len = max_len;
    strncpy(buf, dummy_response, copy_len);
    buf[copy_len] = '\0';
    utl_log("TOR", "RELAY_DATA empfangen. Dummy-Daten kopiert.");
}

// tor_destroy: Zerstört den Circuit und schließt die Verbindung
void tor_destroy(int fd) {
    if (G_circ_id != 0 && fd > 0) {
        net_write_cell(fd, G_circ_id, CELL_CMD_DESTROY, 0, NULL, 0);
        utl_log("TOR", "CELL_CMD_DESTROY gesendet.");
    }
    net_close(fd);
    G_circ_id = 0;
    G_stream_id = 0;
    utl_log("TOR", "Sauberes Cleanup abgeschlossen.");
}


// =================================================================
// SOCKS5-SERVER-SIMULATION (SOCKS)
// =================================================================

// socks_proc_conn: Simuliert SOCKS5-Handshake und CONNECT-Request
int socks_proc_conn(const char* target_host, int target_port) {
    
    utl_log("SOCKS", "Neue Verbindung von Client (Chromium Mock) empfangen.");
    printf("SOCKS: SOCKS5 Handshake/CONNECT abgeschlossen: %s:%d\n", target_host, target_port);

    int s_id = tor_conn_stream(G_or_fd, target_host, target_port);

    if (s_id > 0) {
        printf("SOCKS: SOCKS5 Antwort gesendet (SUCCESS). StreamID: %d.\n", s_id);
        return s_id;
    } else {
        printf("SOCKS: SOCKS5 Antwort gesendet (FAILURE).\n");
        return 0;
    }
}

// socks_forward_data: Simuliert das Weiterleiten von Daten
void socks_forward_data(int s_id, const char* client_req, char* tor_resp_buf, size_t max_len) {
    
    tor_send(G_or_fd, s_id, client_req);
    tor_recv(tor_resp_buf, max_len);
    utl_log("SOCKS", "Leite Tor-Antwort an Client (Chromium Mock) weiter.");
}

// socks_srv_sim_proc: Simuliert die Verarbeitung EINER eingehenden Anfrage
void socks_srv_sim_proc() {
    
    // Simuliere die Zieladresse der Browser-Anfrage
    const char* target_host = "zano.org";
    int target_port = 443;
    const char* http_request = "GET /index.html HTTP/1.1\r\nHost: zano.org\r\nConnection: close\r\n\r\n";
    char response_buffer[512];

    printf("\n[SOCKS LOOP]: Warte auf neue Verbindung (Port %d)... (simuliert)\n", SOCKS_PORT);

    // **************************************************
    // HIER: Simulation einer tatsächlichen Browser-Anfrage
    // **************************************************
    
    // 1. Simuliere eingehende SOCKS-Verbindung
    int stream_id = socks_proc_conn(target_host, target_port);

    if (stream_id > 0) {
        // 2. Simuliere Datenverkehr
        socks_forward_data(stream_id, http_request, response_buffer, sizeof(response_buffer) - 1);

        printf("SOCKS: Anfrage %d abgeschlossen. Browser (Mock) empfing:\n", stream_id);
        printf("------------------------------------------\n");
        printf("%s\n", response_buffer);
        printf("------------------------------------------\n");
        
    } else {
        utl_log("SOCKS", "Fehler: Tor Stream konnte nicht geöffnet werden.");
    }
}

// =================================================================
// HAUPTPROGRAMM (MAIN)
// =================================================================

int main() {
    printf("--- Tor-Client Simulation (Pure C, UNIX-Style) ---\n");
    
    // 1. Initialisiere Tor Core und Circuit
    if (!tor_init()) {
        utl_log("MAIN", "Tor-Initialisierung fehlgeschlagen. Abbruch.");
        return 1;
    }

    // 2. Signal-Handler einrichten
    signal(SIGINT, sig_hdl);
    
    printf("\n--- CHROMIUM START & SOCKS5-KONFIGURATION ---\n");
    printf("MAIN: Starte Chromium mit Proxy-Einstellungen auf 127.0.0.1:%d (SIMULIERT)\n", SOCKS_PORT);
    printf("MAIN: SOCKS5-Server läuft. Drücken Sie **STRG+C** zum Beenden.\n");

    // 3. Server-Hauptschleife
    while (G_run_flag) {
        socks_srv_sim_proc(); 
        // Simuliere, dass der Server nach der Verarbeitung einer Anfrage kurz wartet.
        // Ein echter Server würde hier non-blocking I/O oder Threads/Prozesse nutzen.
        sleep(1); 
    }

    // 4. Cleanup (wird nur nach SIGINT ausgeführt)
    tor_destroy(G_or_fd);

    printf("\n--- PROGRAMM SAUBER BEENDET ---\n");
    return 0;
}
