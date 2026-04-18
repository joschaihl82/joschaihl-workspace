/*
 * tor.c - Vollständige Tor-Client Simulation (Pure C, UNIX-Style, SOCKS5 integriert)
 *
 * Simulierter Fluss:
 * Browser (SOCKS5) -> SOCKS5 Server (Tor Client) -> Guard OR -> Middle OR -> Exit OR
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h> // Für htonl/htons (Netzwerk-Endianness)
#include <sys/socket.h> // Für Socket-Typen

// =================================================================
// GLOBALE KONSTANTEN (KONST)
// =================================================================

#define SOCKS_PORT 9050
#define CELL_SIZE 512
#define CELL_CMD_VERSIONS 7
#define CELL_CMD_NETINFO 8
#define CELL_CMD_CREATE2 1
#define CELL_CMD_CREATED2 2
#define CELL_CMD_RELAY 3
#define CELL_CMD_DESTROY 4
#define RELAY_CMD_BEGIN 1
#define RELAY_CMD_DATA 2
#define RELAY_CMD_EXTEND2 9
#define RELAY_CMD_EXTENDED2 10
#define NTOR_HANDSHAKE_LEN 84 // Dummy-Länge für CREATE2
#define RELAY_HDR_LEN 11      // Relay-Zell-Header-Länge
#define SOCKS5_MAX_ADDR_LEN 256

// Mock-Informationen für den Circuit-Aufbau (Guard, Middle, Exit)
#define OR_GUARD_HOST "192.0.2.1"
#define OR_MIDDLE_HOST "198.51.100.10"
#define OR_EXIT_HOST "203.0.113.20"
#define OR_PORT 9001

// =================================================================
// GLOBALE ZUSTANDSVARIABLEN (G_) - Anstelle von Structs
// =================================================================

volatile int G_run_flag = 1; // 1 = Laufen, 0 = Beenden (Signal-gesteuert)
int G_socks_listen_fd = -1;  // SOCKS5 Listen Socket
int G_socks_client_fd = -1;  // Aktueller SOCKS5 Client Socket (simuliert)
int G_or_fd = -1;            // Guard OR File Descriptor (Simulierter Socket)

uint32_t G_circ_id = 0;   // Aktuelle Circuit ID (4 Bytes)
uint16_t G_stream_id = 0; // Aktuelle Stream ID (SOCKS-Anfrage)

// Circuit Informationen (Dummy, anstelle einer CPATH-Struktur)
char G_path_guard_host[] = OR_GUARD_HOST;
char G_path_middle_host[] = OR_MIDDLE_HOST;
char G_path_exit_host[] = OR_EXIT_HOST;

// =================================================================
// UTILITY-FUNKTIONEN (UTL)
// =================================================================

// utl_log: Protokolliert eine Nachricht
void utl_log(const char* ctx, const char* msg) {
    printf("[%-6s] %s\n", ctx, msg);
}

// utl_u16_to_be: Konvertiert 16-Bit Integer zu Big Endian Array (2 Bytes)
void utl_u16_to_be(unsigned char* a, uint16_t i) {
    uint16_t net_i = htons(i);
    memcpy(a, &net_i, 2);
}

// utl_u32_to_be: Konvertiert 32-Bit Integer zu Big Endian Array (4 Bytes)
void utl_u32_to_be(unsigned char* a, uint32_t i) {
    uint32_t net_i = htonl(i);
    memcpy(a, &net_i, 4);
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
// NETZWERK-MOCK-FUNKTIONEN (NET)
// =================================================================

// Simuliert das Erstellen eines Listening Sockets
int net_bind_listen(int port) {
    utl_log("NET", "Dummy-Socket erstellt, gebunden und lauscht (SIMULIERT).");
    return 10; // Simulierter FD
}

// Simuliert die Annahme einer eingehenden Verbindung
int net_accept(int listen_fd) {
    if (listen_fd < 0) return -1;
    utl_log("NET", "Neue Verbindung akzeptiert (SIMULIERT).");
    return 11; // Simulierter Client FD
}

// net_connect: Simuliert TLS-Verbindung zu einem OR
int net_connect(const char* host, int port) {
    utl_log("NET", "Dummy-TLS-Verbindung (OR) initialisiert.");
    printf("NET: Verbindung zu %s:%d (SIMULIERT)\n", host, port);
    // Nur der Guard OR speichert den FD global
    if (strcmp(host, OR_GUARD_HOST) == 0) {
        G_or_fd = 101;
        return G_or_fd;
    }
    return 102; // Dummy-FD für Middle/Exit Verbindungen (simulierte Hops)
}

// net_write: Simuliert das Senden von Daten/Zellen
void net_write(int fd, const char* buf, size_t len) {
    // printf("NET: %zu Bytes an FD %d gesendet (SIMULIERT)\n", len, fd);
}

// net_read: Simuliert das Empfangen einer Tor-Antwort (Platzhalter)
size_t net_read(int fd, char* buf_out, size_t max_len) {
    // In dieser Simulation gehen wir davon aus, dass Antworten synchron kommen.
    // Ein echter Read müsste warten. Hier wird die erwartete Zellengröße zurückgegeben.
    return CELL_SIZE; 
}

// net_close: Simuliert das Schließen einer Verbindung
void net_close(int fd) {
    if (fd == G_or_fd) G_or_fd = -1;
    if (fd == G_socks_client_fd) G_socks_client_fd = -1;
    // printf("NET: Dummy-Verbindung FD=%d geschlossen.\n", fd);
}

// =================================================================
// TOR-CELL-FUNKTIONEN (CELL)
// =================================================================

// cell_mk: Erstellt eine Dummy-Tor-Zelle im Puffer
size_t cell_mk(char* buf_out, uint32_t circ_id, uint8_t cmd,
               uint16_t rly_cmd, const char* payload, size_t pay_len) {
    unsigned char* dest = (unsigned char*)buf_out;
    size_t off = 0;
    
    // 1. Cell Header (circ_id: 4B, cmd: 1B)
    utl_u32_to_be(dest + off, circ_id);
    off += 4;
    dest[off++] = cmd;

    if (cmd == CELL_CMD_RELAY) {
        // 2. Relay Header (11B): [CMD(2)][RSV(2)][STREAM_ID(2)][DIGEST(4)][LEN(2)]
        utl_u16_to_be(dest + off, rly_cmd); off += 2; // Relay Command
        dest[off++] = 0; dest[off++] = 0;             // Reserviert
        utl_u16_to_be(dest + off, G_stream_id); off += 2; // Stream ID
        
        // Digest (4B) - Dummy für Hmac (in Realität komplexer)
        dest[off++] = 0xDE; dest[off++] = 0xAD; dest[off++] = 0xBE; dest[off++] = 0xEF;
        
        // Länge (2B)
        uint16_t total_pay_len = (uint16_t)pay_len;
        utl_u16_to_be(dest + off, total_pay_len); off += 2;
    }

    // 3. Payload anhängen
    if (payload != NULL && pay_len > 0) {
        size_t copy_len = (off + pay_len > CELL_SIZE) ? (CELL_SIZE - off) : pay_len;
        memcpy(dest + off, payload, copy_len);
        off += copy_len;
    }
    
    // Mit Nullen auffüllen
    memset(dest + off, 0, CELL_SIZE - off);

    return CELL_SIZE; 
}

// cell_write: Erstellt und sendet eine Zelle
void cell_write(int fd, uint32_t circ_id, uint8_t cmd, 
                uint16_t rly_cmd, const char* pay, size_t pay_len) {
    char cell_buf[CELL_SIZE];
    cell_mk(cell_buf, circ_id, cmd, rly_cmd, pay, pay_len);
    net_write(fd, cell_buf, CELL_SIZE);
    // printf("CELL: Zelle gesendet: CircID=%u, Cmd=%d, RlyCmd=%u (Pay: %zu)\n", 
    //        circ_id, cmd, rly_cmd, pay_len);
}

// =================================================================
// TOR-KERN-FUNKTIONEN (TOR) - Circuit-Aufbau
// =================================================================

// tor_send_versions: Sendet die VERSIONS-Zelle
void tor_send_versions(int fd) {
    char versions_payload[6] = {0x00, 0x03, 0x00, 0x04, 0x00, 0x05}; // v3, v4, v5
    cell_write(fd, 0, CELL_CMD_VERSIONS, 0, versions_payload, sizeof(versions_payload));
    utl_log("TOR", "VERSIONS-Zelle gesendet (v3, v4, v5).");
    // Empfange VERSIONS-Antwort (simuliert)
    utl_log("TOR", "VERSIONS-Antwort empfangen (SIMULIERT).");
}

// tor_send_create2: Baut einen Hop im Circuit auf (CREATE2 / CREATED2)
int tor_send_create2(int fd, uint32_t circ_id, const char* host) {
    char ntor_handshake_payload[NTOR_HANDSHAKE_LEN];
    memset(ntor_handshake_payload, 0xAA, NTOR_HANDSHAKE_LEN); // Dummy NTOR Handshake
    
    cell_write(fd, circ_id, CELL_CMD_CREATE2, 0, ntor_handshake_payload, NTOR_HANDSHAKE_LEN);
    utl_log("TOR", "CREATE2 Zelle gesendet (Guard OR).");
    
    // Simuliere NTOR Key Derivation (Key Material ist nun verfügbar)
    
    // Empfange CREATED2 (simuliert)
    char resp_buf[CELL_SIZE];
    net_read(fd, resp_buf, CELL_SIZE);
    printf("TOR: CREATED2 empfangen. Circ %u zu %s ist nun offen. (SIMULIERT)\n", circ_id, host);
    
    return 1;
}

// tor_send_extend2: Baut einen weiteren Hop im Circuit auf (RELAY EXTEND2 / EXTENDED2)
int tor_send_extend2(int fd, uint32_t circ_id, const char* host, int port) {
    char extend_payload[SOCKS5_MAX_ADDR_LEN + 10];
    size_t off = 0;

    // 1. Adresse (IP-Adresse, 4 Bytes)
    uint32_t ip_addr = inet_addr(host);
    utl_u32_to_be((unsigned char*)extend_payload + off, ip_addr); off += 4;
    
    // 2. Port (2 Bytes)
    utl_u16_to_be((unsigned char*)extend_payload + off, port); off += 2;
    
    // 3. Link-Specifier (Mock)
    extend_payload[off++] = 0x00; // Type: IPv4
    extend_payload[off++] = 0x04; // Length: 4
    memcpy(extend_payload + off, &ip_addr, 4); off += 4;

    // 4. NTOR Handshake (Mock)
    extend_payload[off++] = 0x02; // Type: NTOR Handshake
    uint16_t h_len = 84; // Dummy-Handshake-Länge
    utl_u16_to_be((unsigned char*)extend_payload + off, h_len); off += 2;
    memset(extend_payload + off, 0xBB, h_len); off += h_len;
    
    cell_write(fd, circ_id, CELL_CMD_RELAY, RELAY_CMD_EXTEND2, extend_payload, off);
    utl_log("TOR", "RELAY EXTEND2 Zelle gesendet.");
    
    // Empfange RELAY EXTENDED2 (simuliert)
    char resp_buf[CELL_SIZE];
    net_read(fd, resp_buf, CELL_SIZE);
    printf("TOR: RELAY EXTENDED2 empfangen. Circ %u ist zu %s erweitert. (SIMULIERT)\n", circ_id, host);
    
    return 1;
}

// tor_build_circuit: Baut einen kompletten 3-Hop-Circuit auf (Guard, Middle, Exit)
int tor_build_circuit() {
    
    // Starte mit Versions-Handshake
    tor_send_versions(G_or_fd);

    // Wähle eine neue Circuit ID (Tor Client verwendet ungerade)
    G_circ_id = 101; 
    
    // 1. Guard-Hop (CREATE2)
    utl_log("TOR", "Baue Guard Hop auf...");
    if (tor_send_create2(G_or_fd, G_circ_id, G_path_guard_host) == 0) return 0;

    // 2. Middle-Hop (RELAY EXTEND2)
    utl_log("TOR", "Baue Middle Hop auf...");
    if (tor_send_extend2(G_or_fd, G_circ_id, G_path_middle_host, OR_PORT) == 0) return 0;
    
    // 3. Exit-Hop (RELAY EXTEND2)
    utl_log("TOR", "Baue Exit Hop auf...");
    if (tor_send_extend2(G_or_fd, G_circ_id, G_path_exit_host, OR_PORT) == 0) return 0;

    utl_log("TOR", "Circuit-Aufbau abgeschlossen: 3 Hops OPEN.");
    return 1;
}

// tor_conn_stream: Baut einen Stream im Circuit auf (RELAY_BEGIN)
int tor_conn_stream(int fd, const char* host, int port) {
    if (G_circ_id == 0) return 0;
    
    G_stream_id++; // Neue Stream ID

    char hp_pay[SOCKS5_MAX_ADDR_LEN + 10];
    int len = snprintf(hp_pay, sizeof(hp_pay), "%s:%d", host, port);
    
    cell_write(fd, G_circ_id, CELL_CMD_RELAY, RELAY_CMD_BEGIN, hp_pay, len + 1);
    utl_log("TOR", "RELAY_BEGIN gesendet.");
    
    // Empfange RELAY CONNECTED (simuliert)
    char resp_buf[CELL_SIZE];
    net_read(fd, resp_buf, CELL_SIZE);
    printf("TOR: RELAY CONNECTED empfangen. Stream %u ist offen. (SIMULIERT)\n", G_stream_id);
    
    return G_stream_id;
}

// tor_data_exchange: Sendet Daten (RELAY_DATA) und empfängt Antwort
void tor_data_exchange(int fd, int stream_id, const char* req_data, char* resp_buf, size_t max_len) {
    
    // 1. Sende Request (RELAY DATA)
    cell_write(fd, G_circ_id, CELL_CMD_RELAY, RELAY_CMD_DATA, req_data, strlen(req_data));
    utl_log("TOR", "RELAY_DATA (Browser-Request) gesendet.");

    // 2. Empfange Antwort (RELAY DATA)
    char cell_in[CELL_SIZE];
    net_read(fd, cell_in, CELL_SIZE);
    
    // Dummy-Antwort-Payload extrahieren (simuliert)
    const char* dummy_response = "HTTP/1.1 200 OK\r\nContent-Length: 50\r\n\r\nAntwort des Tor Exit-Knotens fuer die Domain zano.org.";
    
    size_t copy_len = strlen(dummy_response);
    if (copy_len > max_len) copy_len = max_len;
    
    strncpy(resp_buf, dummy_response, copy_len);
    resp_buf[copy_len] = '\0';
    
    utl_log("TOR", "RELAY_DATA (Server-Antwort) empfangen und dekodiert.");
}

// tor_destroy: Zerstört den Circuit und schließt die Verbindung
void tor_destroy(int fd) {
    if (G_circ_id != 0 && fd > 0) {
        cell_write(fd, G_circ_id, CELL_CMD_DESTROY, 0, NULL, 0);
        utl_log("TOR", "CELL_CMD_DESTROY gesendet.");
    }
    
    net_close(fd);
    G_circ_id = 0;
    G_stream_id = 0;
    utl_log("TOR", "Sauberes Cleanup (Circuit & Guard-Conn) abgeschlossen.");
}

// =================================================================
// SOCKS5-SERVER-FUNKTIONEN (SOCKS)
// =================================================================

// socks_handle_handshake: Führt SOCKS5 Hello und Auth durch
int socks_handle_handshake(int client_fd) {
    unsigned char buf[SOCKS5_MAX_ADDR_LEN];
    
    // Phase 1: Client HELLO (0x05, NMETHODS, METHODS...)
    size_t read_len = 3; // Dummy-Länge
    // net_read(client_fd, (char*)buf, sizeof(buf)); 
    utl_log("SOCKS", "Client HELLO empfangen (SIMULIERT).");
    
    if (buf[0] != 0x05) return -1; // SOCKS Version muss 5 sein
    
    // Phase 2: Server HELLO (0x05, 0x00 - No Auth Required)
    unsigned char response[] = {0x05, 0x00};
    net_write(client_fd, (char*)response, 2);
    utl_log("SOCKS", "Server HELLO gesendet (NO AUTH).");
    
    return 1;
}

// socks_handle_connect: Führt SOCKS5 CONNECT Request durch
int socks_handle_connect(int client_fd, char* host_out, int* port_out) {
    unsigned char buf[SOCKS5_MAX_ADDR_LEN];
    
    // Phase 3: Client CONNECT Request (0x05, 0x01, 0x00, ADDR_TYPE, ...)
    size_t read_len = 10; // Dummy-Länge (für Beispiel)
    // net_read(client_fd, (char*)buf, sizeof(buf)); 
    
    // Simulierte Daten für zano.org:443
    buf[3] = 0x03; // ADDR_TYPE: Domain Name
    buf[4] = 0x08; // D-Length: 8
    strncpy((char*)buf + 5, "zano.org", 8);
    buf[13] = (443 >> 8) & 0xFF; // Port High
    buf[14] = 443 & 0xFF;        // Port Low
    
    if (buf[0] != 0x05 || buf[1] != 0x01) return -1; // Nur CONNECT unterstützt
    
    if (buf[3] == 0x03) { // ADDR_TYPE: Domain Name
        uint8_t domain_len = buf[4];
        memcpy(host_out, buf + 5, domain_len);
        host_out[domain_len] = '\0';
        
        *port_out = (buf[5 + domain_len] << 8) | buf[5 + domain_len + 1];
    } else {
        // IPV4/IPV6 nicht implementiert
        return -1; 
    }
    
    printf("SOCKS: CONNECT Request empfangen: %s:%d\n", host_out, *port_out);

    // Tor Stream aufbauen
    int s_id = tor_conn_stream(G_or_fd, host_out, *port_out);

    // Phase 4: Server CONNECT Antwort
    unsigned char response_success[] = {
        0x05, 0x01, 0x00, 0x01, // Version, Command (Success), RSV, ATYP (IPv4)
        0x7f, 0x00, 0x00, 0x01, // BND.ADDR (127.0.0.1)
        0x23, 0x5b              // BND.PORT (9051, Dummy)
    };
    
    if (s_id > 0) {
        net_write(client_fd, (char*)response_success, 10);
        utl_log("SOCKS", "CONNECT Response gesendet (SUCCESS).");
        return s_id;
    } else {
        response_success[1] = 0x04; // FAILURE: Host unreachable
        net_write(client_fd, (char*)response_success, 10);
        utl_log("SOCKS", "CONNECT Response gesendet (FAILURE).");
        return 0;
    }
}

// socks_handle_data_exchange: Verarbeitet den HTTP-Datenverkehr
void socks_handle_data_exchange(int client_fd, int stream_id) {
    const char* http_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: zano.org\r\n"
        "Connection: close\r\n\r\n";
    char response_buffer[CELL_SIZE * 2];
    
    // Simuliere, dass der Browser seine HTTP-Anfrage sendet
    // net_read(client_fd, http_request_buffer, sizeof(http_request_buffer)); 
    utl_log("SOCKS", "Browser HTTP-Request empfangen (SIMULIERT).");

    // Sende die Daten über Tor und empfange die Antwort
    tor_data_exchange(G_or_fd, stream_id, http_request, response_buffer, sizeof(response_buffer) - 1);
    
    // Leite die Antwort an den Browser weiter
    net_write(client_fd, response_buffer, strlen(response_buffer));
    utl_log("SOCKS", "Tor-Antwort an Browser (Client) gesendet.");
    
    printf("\n--- Abgeschlossene Transaktion (Browser-Antwort) ---\n");
    printf("--------------------------------------------------\n");
    printf("%s\n", response_buffer);
    printf("--------------------------------------------------\n");
}

// socks_srv_sim_proc: Simuliert die Verarbeitung EINER eingehenden Anfrage
void socks_srv_sim_proc() {
    char target_host[SOCKS5_MAX_ADDR_LEN] = {0};
    int target_port = 0;

    // 1. Simuliere das Akzeptieren einer Verbindung
    int client_fd = net_accept(G_socks_listen_fd);
    if (client_fd < 0) return;
    G_socks_client_fd = client_fd;
    
    // 2. SOCKS5 Handshake
    if (socks_handle_handshake(client_fd) < 0) {
        utl_log("SOCKS", "Handshake fehlgeschlagen.");
        net_close(client_fd);
        return;
    }
    
    // 3. SOCKS5 CONNECT Request
    int stream_id = socks_handle_connect(client_fd, target_host, &target_port);
    if (stream_id > 0) {
        // 4. Datenverkehr
        socks_handle_data_exchange(client_fd, stream_id);
    }
    
    // 5. Stream und Verbindung schließen
    // In einem echten Szenario würde Tor den RELAY END senden, 
    // und der SOCKS-Server würde die Verbindung zum Client schließen.
    net_close(client_fd);
    utl_log("SOCKS", "Client-Verbindung geschlossen.");

}

// =================================================================
// HAUPTPROGRAMM (MAIN)
// =================================================================

int main() {
    printf("--- Tor-Client Simulation (Vollständig, Pure C) ---\n");
    
    // 1. Tor-Initialisierung (Verbindung zum Guard)
    int guard_fd = net_connect(G_path_guard_host, OR_PORT);
    if (guard_fd < 0) {
        utl_log("MAIN", "Verbindung zum Guard fehlgeschlagen. Abbruch.");
        return 1;
    }
    
    // 2. Circuit-Aufbau (3 Hops)
    if (!tor_build_circuit()) {
        utl_log("MAIN", "Circuit-Aufbau fehlgeschlagen. Abbruch.");
        tor_destroy(guard_fd);
        return 1;
    }

    // 3. SOCKS5-Server starten und Signal-Handler einrichten
    G_socks_listen_fd = net_bind_listen(SOCKS_PORT);
    if (G_socks_listen_fd < 0) {
        utl_log("MAIN", "SOCKS5-Server konnte nicht starten. Abbruch.");
        tor_destroy(guard_fd);
        return 1;
    }
    signal(SIGINT, sig_hdl);
    
    printf("\n[MAIN] SOCKS5-Server läuft auf Port %d. Drücken Sie **STRG+C** zum Beenden.\n", SOCKS_PORT);
    
    // 
    
    // 4. Server-Hauptschleife
    while (G_run_flag) {
        utl_log("LOOP", "Warte auf eingehende SOCKS5-Verbindung...");
        
        // Simuliere die Verarbeitung einer eingehenden Browser-Anfrage
        socks_srv_sim_proc(); 
        
        // Kurze Wartezeit, um die Server-Bereitschaft zu simulieren und auf SIGINT zu reagieren.
        // In einem echten Server würde hier `select` oder `poll` verwendet.
        sleep(1); 
    }

    // 5. Cleanup (wird nur nach SIGINT ausgeführt)
    net_close(G_socks_listen_fd);
    tor_destroy(guard_fd);

    printf("\n--- PROGRAMM SAUBER BEENDET ---\n");
    return 0;
}
