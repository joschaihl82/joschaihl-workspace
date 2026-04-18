/*
 * main.c - Tor-Client Simulation (Pure C, Globals, UNIX-Style Naming)
 *
 * SIMULIERTEN FLUSS:
 * Chromium (Mock) -> SOCKS5 Server (Mock) -> Tor Core (Mock)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h> // Für sleep (Simulation)

// =================================================================
// GLOBALE TYPEN UND KONSTANTEN (KONST)
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

// Hardkodiertes Zertifikat (Platzhalter)
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
int G_or_fd = -1;         // Guard OR File Descriptor (OR = Onion Router)
int G_stream_id = 0;      // Aktuelle Stream ID (SOCKS-Anfrage)

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
// TOR-CELL-FUNKTIONEN (CELL)
// =================================================================

// cell_mk: Erstellt eine Dummy-Tor-Zelle im Puffer
size_t cell_mk(char* buf_out, uint16_t circ_id, uint8_t cmd,
               uint8_t rly_cmd, const char* payload, size_t pay_len) {
    if (buf_out == NULL) return 0;
    
    unsigned char* dest = (unsigned char*)buf_out;
    size_t off = 0;
    
    // Cell Header (circ_id: 2B, cmd: 1B)
    utl_u16_to_be(dest + off, circ_id);
    off += 2;
    dest[off++] = cmd;

    if (cmd == CELL_CMD_RELAY) {
        // Relay Header (11B): [CMD(2)][RSV(2)][STREAM_ID(2)][DIGEST(4)][LEN(2)]
        
        // Relay Command (2B)
        utl_u16_to_be(dest + off, rly_cmd);
        off += 2;
        
        // Reserviert (2B)
        dest[off++] = 0; dest[off++] = 0;
        
        // Stream ID (2B)
        utl_u16_to_be(dest + off, G_stream_id);
        off += 2;
        
        // Digest (4B) - Dummy
        dest[off++] = 0xDE; dest[off++] = 0xAD; dest[off++] = 0xBE; dest[off++] = 0xEF;
        
        // Länge (2B)
        uint16_t total_pay_len = (uint16_t)(11 + pay_len); // 11B Relay Header + Payload
        utl_u16_to_be(dest + off, total_pay_len);
        off += 2;
    }

    // Payload anhängen
    if (payload != NULL && pay_len > 0) {
        size_t copy_len = (off + pay_len > CELL_SIZE) ? (CELL_SIZE - off) : pay_len;
        memcpy(dest + off, payload, copy_len);
        off += copy_len;
    }
    
    // Mit Nullen auffüllen
    memset(dest + off, 0, CELL_SIZE - off);

    return CELL_SIZE; // Tor-Zellen sind immer 512 Bytes
}

// =================================================================
// NETZWERK-FUNKTIONEN (NET)
// =================================================================

// net_conn: Simuliert TLS-Verbindung zum Guard-OR
int net_conn(const char* host, int port) {
    utl_log("NET", "Dummy-TLS-Verbindung (OR) initialisiert.");
    printf("NET: Verbindung zu %s:%d (SIMULIERT)\n", host, port);

    // Hardkodiertes Zertifikat simulieren
    utl_log("NET", "Lade OR-Verzeichnis-Zertifikat für TLS-Validierung...");
    printf("------------------------------------------------------------------------\n");
    printf("%s", OR_DIR_CERT);
    printf("------------------------------------------------------------------------\n");
    
    G_or_fd = 101; // Simulierter FD
    return G_or_fd;
}

// net_write_cell: Simuliert das Senden einer Tor-Zelle
void net_write_cell(int fd, uint16_t circ_id, uint8_t cmd, 
                    uint8_t rly_cmd, const char* pay, size_t pay_len) {
    char cell_buf[CELL_SIZE];
    cell_mk(cell_buf, circ_id, cmd, rly_cmd, pay, pay_len);
    
    printf("NET: Tor-Zelle gesendet: FD=%d, CircID=%u, Cmd=%d, RlyCmd=%d (Payload: %zu Bytes)\n",
           fd, circ_id, cmd, rly_cmd, pay_len);
}

// net_close: Simuliert das Schließen einer Verbindung
void net_close(int fd) {
    utl_log("NET", "Dummy-Verbindung geschlossen.");
    G_or_fd = -1;
}

// =================================================================
// TOR-KERN-FUNKTIONEN (TOR)
// =================================================================

// tor_mk_circ: Erstellt den Circuit (CREATE_FAST / CREATED_FAST)
int tor_mk_circ(int fd, uint16_t circ_id) {
    char dummy_key[CRYPTO_KEY_LEN];
    memset(dummy_key, 0xBB, CRYPTO_KEY_LEN);
    
    // Phase 1: CREATE_FAST Zelle senden
    net_write_cell(fd, circ_id, CELL_CMD_CREATE_FAST, 0, dummy_key, CRYPTO_KEY_LEN);
    utl_log("TOR", "CELL_CMD_CREATE_FAST gesendet.");
    
    // Phase 2: CREATED_FAST empfangen (Simulation)
    printf("TOR: CREATED_FAST empfangen. Circ %u ist nun offen. (SIMULIERT)\n", circ_id);
    
    return 1; // Erfolg
}

// tor_init: Initialisiert den Tor-Client (Verbindung und Circuit-Aufbau)
int tor_init() {
    utl_log("TOR", "Initialisierung gestartet. Konsensus wird geladen...");
    
    // 1. Simuliere Guard-Auswahl und physische Verbindung (TLS)
    int fd = net_conn(OR_GUARD_HOST, 9001);
    if (fd < 0) {
        utl_log("TOR", "Fehler beim Verbinden zum Guard.");
        return 0;
    }
    
    // 2. Circuit-Aufbau initiieren
    G_circ_id = 101;
    if (tor_mk_circ(fd, G_circ_id) == 0) {
        utl_log("TOR", "Circuit-Aufbau fehlgeschlagen.");
        return 0;
    }
    
    utl_log("TOR", "Initialisierung abgeschlossen. Circuit ist OPEN.");
    return 1;
}

// tor_conn_stream: Baut einen Stream im Circuit auf (RELAY_BEGIN)
int tor_conn_stream(int fd, const char* host, int port) {
    if (G_circ_id == 0) {
        utl_log("TOR", "Kein Circuit verfügbar.");
        return 0;
    }
    
    // 1. Wähle eine neue Stream ID
    G_stream_id++;

    // 2. Erstelle den RELAY_BEGIN Payload (Hostname:Port\0)
    char hp_pay[256];
    int len = snprintf(hp_pay, sizeof(hp_pay), "%s:%d", host, port);
    
    // 3. Sende RELAY_BEGIN Zelle
    net_write_cell(fd, G_circ_id, CELL_CMD_RELAY, RELAY_CMD_BEGIN, hp_pay, len + 1);
    utl_log("TOR", "RELAY_BEGIN gesendet.");
    
    // 4. Simuliere das Empfangen von RELAY_CONNECTED
    printf("TOR: RELAY_CONNECTED empfangen. Stream %d ist offen. (SIMULIERT)\n", G_stream_id);
    
    return G_stream_id;
}

// tor_send: Sendet Daten (RELAY_DATA)
void tor_send(int fd, int stream_id, const char* data) {
    if (G_circ_id == 0 || G_stream_id == 0) {
        utl_log("TOR", "Kein aktiver Stream zum Senden.");
        return;
    }
    
    // Sende RELAY_DATA Zelle
    // stream_id wird durch die globale G_stream_id in cell_mk verwendet
    net_write_cell(fd, G_circ_id, CELL_CMD_RELAY, RELAY_CMD_DATA, data, strlen(data));
    utl_log("TOR", "RELAY_DATA gesendet.");
}

// tor_recv: Empfängt Daten (Simulierte RELAY_DATA)
void tor_recv(char* buf, size_t max_len) {
    // Simuliere den Empfang von RELAY_DATA mit einer HTTP-Antwort
    const char* dummy_response = "HTTP/1.1 200 OK\r\nContent-Length: 28\r\n\r\nProxy-Antwort ueber Tor Mock!";
    
    size_t copy_len = strlen(dummy_response);
    if (copy_len > max_len) {
        copy_len = max_len;
    }
    
    strncpy(buf, dummy_response, copy_len);
    buf[copy_len] = '\0';
    
    utl_log("TOR", "RELAY_DATA empfangen. Dummy-Daten kopiert.");
}

// tor_destroy: Zerstört den Circuit und schließt die Verbindung
void tor_destroy(int fd) {
    if (G_circ_id != 0) {
        // Sende CELL_DESTROY Zelle
        net_write_cell(fd, G_circ_id, CELL_CMD_DESTROY, 0, NULL, 0);
        utl_log("TOR", "CELL_CMD_DESTROY gesendet.");
    }
    
    net_close(fd);
    G_circ_id = 0;
    G_stream_id = 0;
}

// =================================================================
// SOCKS5-SERVER-FUNKTIONEN (SOCKS)
// =================================================================

// socks_proc_conn: Simuliert einen SOCKS5-Handshake und CONNECT-Request
int socks_proc_conn(const char* target_host, int target_port) {
    
    utl_log("SOCKS", "Neue Verbindung von Client (Chromium Mock) empfangen.");
    
    // --- Phase 1: Handshake (Auth-Methode) ---
    // C: [VER(1)|NMETHODS(1)|METHODS(N)] -> [0x05 | 0x01 | 0x00] (NO AUTH)
    // S: [VER(1)|METHOD(1)] -> [0x05 | 0x00]
    printf("SOCKS: SOCKS5 Handshake (NO AUTH) abgeschlossen.\n");

    // --- Phase 2: CONNECT-Request ---
    // C: [VER(1)|CMD(1)|RSV(1)|ATYP(1)|DST.ADDR|DST.PORT]
    printf("SOCKS: Client CONNECT-Request empfangen: %s:%d\n", target_host, target_port);

    // 1. Tor Stream aufbauen
    int s_id = tor_conn_stream(G_or_fd, target_host, target_port);

    if (s_id > 0) {
        // 2. Request erfolgreich: Sende SOCKS5-Antwort (SUCCESS)
        // S: [VER(1)|REP(1)|RSV(1)|ATYP(1)|BND.ADDR|BND.PORT] -> [0x05 | 0x00 | 0x00 | 0x01 | 0.0.0.0(4) | 0(2)]
        printf("SOCKS: SOCKS5 Antwort gesendet (SUCCESS). StreamID: %d.\n", s_id);
        return s_id;
    } else {
        // Sende SOCKS5-Antwort (FAILURE)
        printf("SOCKS: SOCKS5 Antwort gesendet (FAILURE).\n");
        return 0;
    }
}

// socks_forward_data: Simuliert das Weiterleiten von Daten vom SOCKS-Client zu Tor
void socks_forward_data(int s_id, const char* client_req, char* tor_resp_buf, size_t max_len) {
    
    // 1. Client-Anfrage über Tor senden
    utl_log("SOCKS", "Leite Client-Daten an Tor weiter...");
    tor_send(G_or_fd, s_id, client_req);
    
    // 2. Tor-Antwort empfangen
    utl_log("SOCKS", "Warte auf Antwort vom Tor-Netzwerk...");
    tor_recv(tor_resp_buf, max_len);
    
    // 3. Tor-Antwort an den Client zurückleiten (Browser)
    utl_log("SOCKS", "Leite Tor-Antwort an Client (Chromium Mock) weiter.");
}

// socks_server_run: Simuliert den Betrieb des SOCKS5-Servers
void socks_server_run() {
    
    // Simuliere die Zieladresse der Browser-Anfrage
    const char* target_host = "zano.org";
    int target_port = 443; // HTTP/S-Verbindung
    const char* http_request = "GET /index.html HTTP/1.1\r\nHost: zano.org\r\nConnection: close\r\n\r\n";
    char response_buffer[512];

    printf("\n--- SOCKS5-SERVER (SIMULIERT) START: Lauscht auf :%d ---\n", SOCKS_PORT);

    // 1. Simuliere eingehende SOCKS-Verbindung vom Browser
    int stream_id = socks_proc_conn(target_host, target_port);

    if (stream_id > 0) {
        // 2. Simuliere Datenverkehr
        socks_forward_data(stream_id, http_request, response_buffer, sizeof(response_buffer) - 1);

        printf("\nSOCKS: Datenfluss abgeschlossen. Browser (Mock) empfing:\n");
        printf("------------------------------------------\n");
        printf("%s\n", response_buffer);
        printf("------------------------------------------\n");
        
    } else {
        utl_log("SOCKS", "Fehler: Tor Stream konnte nicht geöffnet werden.");
    }
}

// =================================================================
// HAUPTPROGRAMM
// =================================================================

int main() {
    printf("--- Tor-Client Simulation (Pure C, UNIX-Style, SOCKS5 Mock) ---\n");
    
    // 1. Initialisiere Tor Core (Guard-Verbindung und Circuit-Aufbau)
    if (!tor_init()) {
        utl_log("MAIN", "Tor-Initialisierung fehlgeschlagen. Abbruch.");
        return 1;
    }

    printf("\n--- CHROMIUM START & SOCKS5-KONFIGURATION ---\n");
    // Simuliere den Start von Chromium, das auf den SOCKS-Proxy verweist
    printf("MAIN: Starte Chromium mit Proxy-Einstellungen auf 127.0.0.1:%d (SIMULIERT)\n", SOCKS_PORT);

    // 2. Starte den SOCKS5-Server-Mock
    socks_server_run();

    // 3. Schließe die Tor-Verbindung (Circuit/OR-Verbindung zerstören)
    tor_destroy(G_or_fd);

    printf("\n--- SIMULATION BEENDET ---\n");
    return 0;
}
