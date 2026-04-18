/*
 * main.c - Provisorische C-Implementierung der Tor-Logik
 *
 * Alle Zustände werden über globale Variablen (G_...) verwaltet.
 * Funktionen verwenden atomare Typen als Parameter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h> // Für sleep (Simulation)

// =================================================================
// GLOBALE TYPEN UND KONSTANTEN
// =================================================================

// Hardkodiertes Zertifikat (Platzhalter für die TLS-Verbindung)
#define HARDCODED_DIRECTORY_CERT \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDKzCCAhOgAwIBAgIRAP2s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s\n" \
"MIG+MQswCQYDVQQGEwJVUzESMBAGA1UECAwIQ2FsaWZvcm5pYTEUMBIGA1UECgwL\n" \
"VG9yIFByb2plY3QxETAPBgNVBAsMClJvb3QgQXV0aC4xGTAXBgNVBAMMEFRvciBE\n" \
"aXJlY3RvcnkgQ0ExHjAcBgkqhkiG9w0BCQEWD2RpcmVjdG9yeUB0b3Iub3JnMB4X\n" \
"DTk5MTIzMTIzNTk1OVoxDzANBgNVBAMMB1RPUl9HVUFSRA==\n" \
"-----END CERTIFICATE-----\n"

// Tor Cell Commands (Nur die verwendeten)
#define CELL_CREATE_FAST 5
#define CELL_CREATED_FAST 6
#define CELL_RELAY 3
#define CELL_DESTROY 4

// Tor Relay Commands (Nur die verwendeten)
#define RELAY_BEGIN 1
#define RELAY_DATA 2

#define CELL_NETWORK_SIZE 512
#define CELL_PAYLOAD_SIZE 509
#define DIGEST_LENGTH 4 // Für Dummy-Digest
#define CPATH_KEY_MATERIAL_LEN 40 // Für Dummy-Handshake
#define RELAY_HEADER_SIZE 11
#define CELL_HEADER_SIZE 3

// =================================================================
// GLOBALE ZUSTANDSVARIABLEN (Ersetzen C++ Klassen-Member)
// =================================================================

// Zustand der Circuit-Liste (Simuliert den aktuellen aktiven Circuit)
// Da wir keine structs übergeben dürfen, müssen wir den Zustand global halten.
uint16_t G_circuit_id = 0;
int G_mock_socket_fd = -1; // Simuliert den Dateideskriptor der OR-Verbindung (zum Guard)
int G_stream_id = 0;       // Simuliert die Stream-ID für die Anwendung

// =================================================================
// UTILITY UND HILFSFUNKTIONEN
// =================================================================

// Mimics a log function
void Util_Log(const char* context, const char* message) {
    printf("[%s] %s\n", context, message);
}

// Ersetzt Util::Int16ToArrayBigEndian
void Util_Int16ToArrayBigEndian(unsigned char* a, uint16_t i) {
    a[0] = (unsigned char)((i >> 8) & 0xFF);
    a[1] = (unsigned char)(i & 0xFF);
}

// =================================================================
// CELL-VERPACKUNGS-MOCK
// Ersetzt die C++ Klassen Cell und RelayCell
// =================================================================

/**
 * Erstellt eine Dummy-Tor-Zelle und gibt die simulierte Länge zurück.
 * @param circ_id Circuit ID.
 * @param command Cell Command (e.g., CELL_CREATE_FAST).
 * @param relay_command Relay Command (RELAY_BEGIN, 0 für Non-Relay Cells).
 * @param payload_in Daten-Payload (optional, z.B. Hostname für RELAY_BEGIN).
 * @param payload_len Länge des Payloads.
 * @return Die simulierte Länge der Zelle (oder CELL_NETWORK_SIZE).
 */
size_t Cell_Create(char* buffer_out, uint16_t circ_id, uint8_t command,
                   uint8_t relay_command, const char* payload_in, size_t payload_len) {
    if (buffer_out == NULL) return 0;
    
    unsigned char* dest = (unsigned char*)buffer_out;
    size_t offset = 0;
    
    // 1. Cell Header (CircID: 2 Bytes, Command: 1 Byte)
    Util_Int16ToArrayBigEndian(dest + offset, circ_id);
    offset += 2;
    dest[offset++] = command;

    if (command == CELL_RELAY) {
        // 2. Relay Header (11 Bytes)
        // [RELAY_COMMAND (2)][RESERVED (2)][STREAM_ID (2)][DIGEST (4)][LENGTH (2)]
        
        // Simuliere Relay Command (2 Bytes)
        Util_Int16ToArrayBigEndian(dest + offset, relay_command);
        offset += 2;
        
        // Reserviert (2 Bytes)
        dest[offset++] = 0; dest[offset++] = 0;
        
        // Stream ID (2 Bytes)
        Util_Int16ToArrayBigEndian(dest + offset, G_stream_id);
        offset += 2;
        
        // Digest (4 Bytes) - Dummy-Digest
        dest[offset++] = 0xDE; dest[offset++] = 0xAD; dest[offset++] = 0xBE; dest[offset++] = 0xEF;
        
        // Länge (2 Bytes)
        uint16_t total_payload_len = (uint16_t)(RELAY_HEADER_SIZE + payload_len);
        Util_Int16ToArrayBigEndian(dest + offset, total_payload_len);
        offset += 2;
    } else {
        // Für Nicht-Relay-Cells (z.B. CREATE_FAST, DESTROY): Nur Payload anhängen
        // In diesem einfachen Mock wird dies ignoriert, da wir nur den Header brauchen.
    }

    // 3. Payload
    if (payload_in != NULL && payload_len > 0) {
        if (offset + payload_len > CELL_NETWORK_SIZE) {
            payload_len = CELL_NETWORK_SIZE - offset; // Kürzen, falls zu lang
        }
        memcpy(dest + offset, payload_in, payload_len);
        offset += payload_len;
    }
    
    // Füllen mit Nullen bis zur vollen Zellenlänge (optional)
    memset(dest + offset, 0, CELL_NETWORK_SIZE - offset);

    return CELL_NETWORK_SIZE; // Tor-Zellen sind immer 512 Bytes lang (bei 2-Byte CircID)
}

// =================================================================
// NETZWERK-MOCK (NETCONNECT ERSATZ)
// =================================================================

// Ersetzt NetConnect::Connect
int NetConnect_Connect(const char* host, int port) {
    Util_Log("NetConnect", "Dummy-TLS-Verbindung initialisiert.");
    printf("NetConnect: Verbindung zu %s:%d (SIMULIERT)\n", host, port);

    // **HARDCODED CERTIFICATE IMPLEMENTIERUNG**
    Util_Log("NetConnect", "Lade hardkodiertes Verzeichnis-Zertifikat für TLS-Validierung...");
    printf("------------------------------------------------------------------------\n");
    printf("%s", HARDCODED_DIRECTORY_CERT);
    printf("------------------------------------------------------------------------\n");
    Util_Log("NetConnect", "Hardkodiertes Zertifikat geladen und für Handshake vorbereitet. (SIMULIERT)");

    // Simuliere einen erfolgreichen Socket-Deskriptor
    G_mock_socket_fd = 101;
    return G_mock_socket_fd;
}

// Ersetzt NetConnect::WriteCell
void NetConnect_WriteCell(int socket_fd, uint16_t circ_id, uint8_t command, 
                          uint8_t relay_command, const char* payload, size_t payload_len) {
    char cell_buffer[CELL_NETWORK_SIZE];
    Cell_Create(cell_buffer, circ_id, command, relay_command, payload, payload_len);
    
    // In einer echten Anwendung: write(socket_fd, cell_buffer, CELL_NETWORK_SIZE);
    printf("NetConnect: Dummy-Zelle gesendet: FD=%d, CircID=%u, Command=%d, RelayCmd=%d (Payload: %zu Bytes)\n",
           socket_fd, circ_id, command, relay_command, payload_len);
}

// Ersetzt NetConnect::Close
void NetConnect_Close(int socket_fd) {
    Util_Log("NetConnect", "Dummy-Verbindung geschlossen.");
    G_mock_socket_fd = -1;
    G_circuit_id = 0;
    G_stream_id = 0;
    // close(socket_fd);
}

// =================================================================
// TOR-KERNFUNKTIONEN (TORLIB ERSATZ)
// =================================================================

// Ersetzt TorLib::Init
int Tor_Init() {
    Util_Log("TorLib", "Initialisierung gestartet. Konsensus wird geladen...");
    
    // 1. Simuliere Konsensus-Download und Auswahl eines Guard-Knotens
    const char* guard_ip = "192.0.2.1";
    int guard_port = 9001;

    // 2. Physische Verbindung zum Guard (MIT HARDCODED CERT)
    int fd = NetConnect_Connect(guard_ip, guard_port);
    if (fd < 0) {
        Util_Log("TorLib", "Fehler beim Verbinden zum Guard.");
        return 0;
    }
    
    // 3. Circuit-Aufbau initiieren
    // Wähle eine neue Circuit ID
    G_circuit_id = 101;
    
    if (Tor_CreateCircuit(fd, G_circuit_id) == 0) {
        Util_Log("TorLib", "Circuit-Aufbau fehlgeschlagen.");
        return 0;
    }
    
    Util_Log("TorLib", "Initialisierung abgeschlossen. Circuit ist OPEN.");
    return 1;
}

// Simuliert den Handshake/Circuit-Aufbau
int Tor_CreateCircuit(int socket_fd, uint16_t circ_id) {
    char dummy_payload[CPATH_KEY_MATERIAL_LEN];
    memset(dummy_payload, 0xBB, CPATH_KEY_MATERIAL_LEN);
    
    // Phase 1: CREATE_FAST Zelle senden
    NetConnect_WriteCell(socket_fd, circ_id, CELL_CREATE_FAST, 0, dummy_payload, CPATH_KEY_MATERIAL_LEN);
    Util_Log("TorLib", "CELL_CREATE_FAST gesendet.");
    
    // Phase 2: Simuliere das Empfangen der CREATED_FAST Zelle
    // In einer echten Anwendung: Lese die Antwort vom Socket und verarbeite sie.
    // Hier wird der Circuit-Zustand direkt auf "Open" gesetzt.
    printf("C-Core: CREATED_FAST empfangen. Circuit %u ist nun OPEN. (SIMULIERT)\n", circ_id);
    
    return 1; // Erfolg
}

// Ersetzt TorLib::Connect (SOCKS Stream-Aufbau)
int Tor_Connect_Stream(int socket_fd, const char* ip, int port) {
    if (G_circuit_id == 0) {
        Util_Log("TorLib", "Kein Circuit verfügbar.");
        return 0;
    }
    
    // 1. Wähle eine neue Stream ID
    G_stream_id = 1;

    // 2. Erstelle den RELAY_BEGIN Payload (Hostname:Port\0)
    char host_port_payload[256];
    int len = snprintf(host_port_payload, sizeof(host_port_payload), "%s:%d", ip, port);
    
    // 3. Sende RELAY_BEGIN Zelle
    NetConnect_WriteCell(socket_fd, G_circuit_id, CELL_RELAY, RELAY_BEGIN, host_port_payload, len + 1);
    Util_Log("TorLib", "RELAY_BEGIN gesendet.");
    
    // 4. Simuliere das Empfangen von RELAY_CONNECTED
    printf("C-Core: RELAY_CONNECTED empfangen. Stream %d ist offen. (SIMULIERT)\n", G_stream_id);
    
    return G_stream_id;
}

// Ersetzt TorLib::Send
void Tor_Send(int socket_fd, int stream_id, const char* data) {
    if (G_circuit_id == 0 || G_stream_id == 0) {
        Util_Log("TorLib", "Kein aktiver Stream zum Senden.");
        return;
    }
    
    // Sende RELAY_DATA Zelle
    NetConnect_WriteCell(socket_fd, G_circuit_id, CELL_RELAY, RELAY_DATA, data, strlen(data));
    Util_Log("TorLib", "RELAY_DATA gesendet.");
}

// Ersetzt TorLib::Receive
void Tor_Receive(char* buffer, size_t max_len) {
    // Simuliere den Empfang von RELAY_DATA mit einer HTTP-Antwort
    const char* dummy_response = "HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\nHello from the Tor mock!";
    
    size_t copy_len = strlen(dummy_response);
    if (copy_len > max_len) {
        copy_len = max_len;
    }
    
    strncpy(buffer, dummy_response, copy_len);
    buffer[copy_len] = '\0';
    
    Util_Log("TorLib", "RELAY_DATA empfangen. Dummy-Daten kopiert.");
}

// Ersetzt TorLib::Close (Circuit/Stream zerstören)
void Tor_Destroy(int socket_fd) {
    if (G_circuit_id != 0) {
        // Sende CELL_DESTROY Zelle
        NetConnect_WriteCell(socket_fd, G_circuit_id, CELL_DESTROY, 0, NULL, 0);
        Util_Log("TorLib", "CELL_DESTROY gesendet.");
    }
    
    NetConnect_Close(socket_fd);
}


// =================================================================
// HAUPTPROGRAMM
// =================================================================

int main() {
    printf("--- Tor-Client Simulation (Pure C, Globals) ---\n");
    
    // 1. Initialisiere den Tor-Client (Enthält Konsensus und Circuit-Aufbau)
    if (!Tor_Init()) {
        Util_Log("main", "Tor-Initialisierung fehlgeschlagen.");
        return 1;
    }

    printf("\n--- SIMULATION EINER BROWSER-ANFRAGE ---\n");

    // 2. Simuliere eine SOCKS-Anfrage (Stream-Aufbau)
    int stream_id = Tor_Connect_Stream(G_mock_socket_fd, "example.com", 80);
    
    if (stream_id > 0) {
        // 3. Sende eine einfache HTTP-Anfrage (RELAY_DATA)
        const char* http_request = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
        Tor_Send(G_mock_socket_fd, stream_id, http_request);

        // 4. Empfange die Antwort (Simulierte RELAY_DATA)
        char response_buffer[512];
        Tor_Receive(response_buffer, sizeof(response_buffer) - 1);
        
        printf("\nBrowser: HTTP-Antwort empfangen:\n");
        printf("------------------------------------------\n");
        printf("%s\n", response_buffer);
        printf("------------------------------------------\n");
    } else {
        Util_Log("main", "Stream-Verbindung fehlgeschlagen.");
    }

    // 5. Schließe die Verbindung (CELL_DESTROY)
    Tor_Destroy(G_mock_socket_fd);

    printf("\n--- SIMULATION BEENDET ---\n");
    return 0;
}

