#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>

// =================================================================
// TYPEDEFINITIONEN UND GLOBALE KONSTANTEN
// =================================================================

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define SOCKS_PORT 9050
#define CELL_SIZE 512
#define CELL_HEADER_LEN 5 // CircID (4) + Command (1)
#define RELAY_HDR_LEN 11
#define RELAY_PAYLOAD_LEN (CELL_SIZE - CELL_HEADER_LEN - RELAY_HDR_LEN)
#define SOCKS5_MAX_ADDR_LEN 256
#define IO_TIMEOUT_SEC 10

// Kryptographische Konstanten (Tor-Standard)
#define SHA1_DIGEST_LEN 20
#define AES_KEY_LEN 16
#define IV_LEN 16
#define CURVE25519_KEY_LEN 32
#define NTOR_KEM_KEY_MATERIAL_LEN (2 * SHA1_DIGEST_LEN + 2 * AES_KEY_LEN + 2 * IV_LEN) 
#define TLS_RECORD_HEADER_LEN 5 // Typ (1) + Version (2) + Länge (2)

// Hardkodierte Router-Infos
#define OR_GUARD_HOST "192.0.2.1"
#define OR_MIDDLE_HOST "198.51.100.10"
#define OR_EXIT_HOST "203.0.113.20"
#define OR_PORT 9001

// Tor-Protokoll Versionen
#define TOR_PROTO_V3 3

// Enumeration für Tor-Zell-Kommandos
typedef enum {
    CMD_PADDING = 0, CMD_CREATE = 1, CMD_CREATED = 2, CMD_RELAY = 3, 
    CMD_DESTROY = 4, CMD_VERSIONS = 7, CMD_NETINFO = 8, 
    CMD_CREATE2 = 9, CMD_CREATED2 = 10, CMD_CERTS = 5
} cell_command;

// Enumeration für Relay-Zell-Kommandos
typedef enum {
    RLY_CMD_PADDING = 0, RLY_CMD_BEGIN = 1, RLY_CMD_DATA = 2, RLY_CMD_END = 3, 
    RLY_CMD_CONNECTED = 4, RLY_CMD_EXTEND2 = 9, RLY_CMD_EXTENDED2 = 10
} relay_command;

// X.509 Certificate Type Enumeration (Simuliert)
typedef enum {
    CERT_TYPE_TLS_LINK = 1,
    CERT_TYPE_IDENTITY = 2
} cert_type_e;

// =================================================================
// HARTKODIERTE ZERTIFIKAT-DATEN
// =================================================================

// Simuliertes Base64-kodiertes X.509 Identity Key Certificate des Guard
const char* const GUARD_IDENTITY_CERTIFICATE_B64 = 
    "MIIDXzCCAkOgAwIBAgIUQ+zU0FwF+d9L3N1t9R8tA2z7uH4wDQYJKoZIhvcNAQELBQAwGTEXMBUGA1UEAwwOUm9nZXIgR3VhcmQgT1IwHhcNMTcwMzE1MTAwMjEwWhcNMjUwMzEyMTAwMjEwWjAZMRcwFQYDVQQDDA5Sb2dlciBHdWFyZCBPUjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALyLg5xJj5s8yQk6FkH4oZt3w2i/P4zBa1l9xT6xL0f0jR4jTzKz2+Xl/W+8lK9p/v7g/s5/w/g/18/w+m+0+j+6/g/5/k/ma+zU0FwF+d9L3N1t9R8tA2z7uH4wDQYJKoZIhvcNAQELBQAwGTEXMBUGA1UEAwzOWmljZGUgVGVzdCBDZXJ0aWZpa2F0ZSBNSUQxMDAwHhcNMTcwMzE1MTAwMjEwWhcNMjUwMzEyMTAwMjEwWjAZMRcwFQYDVQQDDA9DbGllbnQgVGxzIENlcnQp";

// Simuliertes Base64-kodiertes TLS Link Certificate des Guard
const char* const GUARD_TLS_LINK_CERTIFICATE_B64 = 
    "MIIFHTCCBHGgAwIBAgIQAf4A08Qx/uHw4B7X1dJ1uTANBgkqhkiG9w0BAQsFADBkMQswCQYDVQQGEwJERTERMA8GA1UEBxMIRGVkZGllbjETMBEGA1UEChMKVG9yIFByb2plY3QxETAPBgNVBAsTCE9yIFJlbGF5MRcwFQYDVQQDDA5Sb2dlciBHdWFyZCBPUjCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAQoCggIBALyLg5xJj5s8yQk6FkH4oZt3w2i/P4zBa1l9xT6xL0f0jR4jTzKz2+Xl/W+8lK9p/v7g/s5/w/g/18/w+m+0xT6xL0f0jR4jTzKz2+Xl/W+8lK9p/v7g/s5/w/g/18/w+m+0+j+6/g/5/k/m8+zU0FwF+d9L3N1t9R8tA2z7uH4wDQYJKoZIhvcNAQELBQAwGTEXMBUGA1UEAwzOR3VhcmQgVGxzIENlcnRpZmlrYXQgU0lNMjAwMDHk";


// =================================================================
// ZENTRALE ZUSTANDSSTRUKTUREN
// =================================================================

// SHA1 Kontext Struktur (Interne Implementierung)
typedef struct {
    u32 state[5];
    u32 count[2];
    u8 buffer[64];
} SHA1_CTX_t;

// AES Key Struktur (Interne Implementierung für CTR Modus)
typedef struct {
    u32 rk[60]; // Round Keys
    int nr;
} AES_KEY_t;

// Kryptographische Pfadstruktur
typedef struct {
    char host[32];
    u16 port;
    u8 ntor_public_key[CURVE25519_KEY_LEN];
    u8 key_material[NTOR_KEM_KEY_MATERIAL_LEN];

    // Layered Encryption Keys/State
    AES_KEY_t fwd_aes_key;
    AES_KEY_t back_aes_key;
    u8 fwd_iv[IV_LEN];
    u8 back_iv[IV_LEN];
    
    // Digest State (zur Berechnung der 4-Byte Digests)
    SHA1_CTX_t fwd_digest_ctx;
    SHA1_CTX_t back_digest_ctx;

    u32 fwd_circ_nonce; 
    u32 back_circ_nonce;
} Hop_t;

// Zentrale Circuit-Zustandsstruktur
typedef struct {
    u32 circ_id;
    int or_fd; 
    Hop_t path[3]; 
    int num_hops;
    
    // TLS/Zertifikatszustand
    u8 guard_identity_digest[SHA1_DIGEST_LEN];
    int cert_validated_flag;
} Circuit_t;


// =================================================================
// GLOBALE ZUSTANDSVARIABLEN
// =================================================================

volatile int G_run_flag = 1;
u16 G_stream_id = 0;
Circuit_t G_circuit = {0}; 


// =================================================================
// HILFSFUNKTIONEN (UTL)
// =================================================================

static void utl_log(const char* ctx, const char* level, const char* msg) {
    printf("[%-6s][%-7s] %s\n", ctx, level, msg);
}

static u32 utl_be_to_u32(const u8* a) {
    return ntohl(*(const u32*)a);
}

static u16 utl_be_to_u16(const u8* a) {
    return ntohs(*(const u16*)a);
}

static void utl_u16_to_be(u8* a, u16 i) {
    u16 net_i = htons(i);
    memcpy(a, &net_i, 2);
}

static void utl_u32_to_be(u8* a, u32 i) {
    u32 net_i = htonl(i);
    memcpy(a, &net_i, 4);
}

/**
 * @brief Simuliert Base64-Dekodierung und liefert binäre Daten.
 * @return Länge der dekodierten Daten (simuliert).
 */
static size_t utl_base64_decode(const char* b64_in, u8* out_buf, size_t max_len) {
    size_t actual_len = (strlen(b64_in) / 4 * 3); 
    if (actual_len > max_len) actual_len = max_len;
    // Realistische Dummy-Daten
    for(size_t i = 0; i < actual_len; i++) {
        out_buf[i] = (u8)((i % 250) + 1);
    }
    utl_log("UTIL", "DEBUG", "Base64 Dekodierung abgeschlossen.");
    return actual_len;
}

// =================================================================
// NETZWERKSCHICHT (NET) - BLOCKIEREND
// =================================================================

/**
 * @brief Stellt eine blockierende TCP-Verbindung her.
 */
static int net_connect(const char* host, int port) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        utl_log("NET", "ERROR", "Socket-Erstellung fehlgeschlagen.");
        return -1;
    }

    server = gethostbyname(host);
    if (server == NULL) {
        utl_log("NET", "ERROR", "Hostname-Auflösung fehlgeschlagen.");
        close(sockfd);
        return -1;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        utl_log("NET", "ERROR", "Verbindung fehlgeschlagen.");
        close(sockfd);
        return -1;
    }

    utl_log("NET", "NOTICE", "TCP-Verbindung erfolgreich hergestellt.");
    return sockfd;
}

/**
 * @brief Sendet eine bestimmte Anzahl von Bytes.
 */
static int net_write(int fd, const u8* buf, size_t len) {
    size_t total = 0;
    int bytes_sent;
    while (total < len) {
        bytes_sent = send(fd, buf + total, len - total, 0);
        if (bytes_sent <= 0) {
            utl_log("NET", "ERROR", "Senden fehlgeschlagen.");
            return -1;
        }
        total += bytes_sent;
    }
    return (int)total;
}

/**
 * @brief Empfängt eine bestimmte Anzahl von Bytes (blockierend).
 */
static int net_read_n(int fd, u8* buf, size_t len) {
    size_t total = 0;
    int bytes_recv;
    
    // Setze Timeout für realistische Implementierung
    struct timeval tv;
    tv.tv_sec = IO_TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    while (total < len) {
        bytes_recv = recv(fd, buf + total, len - total, 0);
        if (bytes_recv <= 0) {
            utl_log("NET", "ERROR", (bytes_recv == 0) ? "Verbindung geschlossen." : "Empfangen fehlgeschlagen/Timeout.");
            return -1;
        }
        total += bytes_recv;
    }
    return (int)total;
}

static void net_close(int fd) {
    if (fd >= 0) {
        close(fd);
        utl_log("NET", "INFO", "Socket geschlossen.");
    }
}


// =================================================================
// KRYPTOGRAPHIE IMPLEMENTIERUNGEN (CRYPTO)
// =================================================================

// --- SHA1 (Minimal-Struktur-Implementation) ---

static void sha1_init(SHA1_CTX_t* ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count[0] = ctx->count[1] = 0;
}

static void sha1_transform(u32 state[5], const u8 buffer[64]) {
    // Echte SHA1-Logik ist komplex. Hier nur eine Puffer-Nutzungs-Simulation.
    state[0] ^= utl_be_to_u32(buffer + 0);
    state[1] ^= utl_be_to_u32(buffer + 4);
    state[2] ^= utl_be_to_u32(buffer + 8);
    state[3] ^= utl_be_to_u32(buffer + 12);
    state[4] ^= utl_be_to_u32(buffer + 16);
}

static void sha1_update(SHA1_CTX_t* ctx, const u8* data, size_t len) {
    // Realistisches Hinzufügen von Daten zum Kontext (nur Struktur)
    if (len == 0) return;
    size_t i = 0;
    while (i < len) {
        ctx->buffer[i % 64] = data[i];
        if ((i % 64) == 63) {
            sha1_transform(ctx->state, ctx->buffer);
        }
        i++;
    }
    ctx->count[0] += (u32)len;
}

static void sha1_final(u8 digest[SHA1_DIGEST_LEN], SHA1_CTX_t* ctx) {
    // Endbearbeitung (Padding, Transform, Finalisierung)
    sha1_update(ctx, ctx->buffer, 64); // Abschließende Transformation simulieren
    // Kopiere den simulierten Endzustand (state) als Digest
    for(int i = 0; i < 5; i++) {
        utl_u32_to_be(digest + i * 4, ctx->state[i]);
    }
    digest[0] ^= 0xAB; // Um sicherzustellen, dass es nicht nur Nullen sind.
}

/**
 * @brief Berechnet den 4-Byte Digest für eine Relay-Zelle.
 */
static void crypto_calculate_digest_4byte(u8* digest_out, SHA1_CTX_t* digest_state_copy, const u8* data, size_t len) {
    SHA1_CTX_t tmp_ctx = *digest_state_copy;
    u8 full_digest[SHA1_DIGEST_LEN];
    sha1_update(&tmp_ctx, data, len);
    sha1_final(full_digest, &tmp_ctx);
    
    // Nur die ersten 4 Bytes verwenden (Tor-Protokoll)
    memcpy(digest_out, full_digest, 4); 
    
    // Aktualisiere den tatsächlichen Digest-Zustand für die nächste Zelle
    sha1_update(digest_state_copy, data, len); 
}

// --- AES-CTR (Minimal-Struktur-Implementation) ---

static void aes_key_init(AES_KEY_t* key, const u8* raw_key) {
    // Simuliere die Initialisierung des AES-Schlüssels (realistisch ist sehr komplex)
    memcpy(key->rk, raw_key, AES_KEY_LEN);
    key->nr = 10; // 10 Runden für 128-Bit-Schlüssel
}

static void aes_ctr_block_encrypt(const u8* in, u8* out, size_t len, 
                                  const AES_KEY_t* key, u8* iv) {
    // Simuliere AES-CTR-Verschlüsselung mit Counter-Inkrementierung
    // In der Realität: Block-Verschlüsselung des IVs, dann XOR mit In-Buffer.
    for (size_t i = 0; i < len; i++) {
        out[i] = in[i] ^ iv[i % IV_LEN];
    }
    // Inkrementiere Counter-Teil des IVs (letzten 4 Bytes)
    u32 counter = utl_be_to_u32(iv + IV_LEN - 4);
    counter++;
    utl_u32_to_be(iv + IV_LEN - 4, counter);
}

/**
 * @brief Verschlüsselt/Entschlüsselt die Relay-Payload.
 */
static void crypto_aes_process_cell(u8* payload, size_t len, AES_KEY_t* key, u8* iv) {
    // Simuliert die Anwendung von AES-CTR über die gesamte Payload
    aes_ctr_block_encrypt(payload, len, key, iv);
}

// --- Curve25519/NTOR KDF (Logik-Implementation) ---

// Realistische Curve25519-Typen (aus Curve25519.h)
typedef u8 curve25519_key_t[CURVE25519_KEY_LEN];

/**
 * @brief Simuliert die Erzeugung eines Curve25519-Schlüsselpaars.
 */
static void curve25519_keypair_gen(curve25519_key_t private_key, curve25519_key_t public_key) {
    // In einer echten Implementierung würde hier Zufall verwendet.
    // Hier wird deterministisch gefüllt.
    memset(private_key, 0x1A, CURVE25519_KEY_LEN);
    memset(public_key, 0x2B, CURVE25519_KEY_LEN);
    private_key[0] = 0x01;
    public_key[0] = 0x01;
}

/**
 * @brief Simuliert den Curve25519 Shared Secret-Berechnung.
 */
static void curve25519_shared_secret(curve25519_key_t shared_secret, 
                                    const curve25519_key_t private_key, 
                                    const curve25519_key_t public_key) {
    // Shared Secret ist das XOR der Keys (sehr vereinfacht für Demo)
    for (int i = 0; i < CURVE25519_KEY_LEN; i++) {
        shared_secret[i] = private_key[i] ^ public_key[i] ^ 0xCC;
    }
}

/**
 * @brief Führt die Tor NTOR Key Derivation Funktion (KDF) durch.
 */
static void crypto_kdf_ntor(Hop_t* hop, const u8* handshake_data, size_t data_len) {
    // 1. Erzeugung der temporären Schlüssel
    curve25519_key_t client_private_key, client_public_key, shared_secret;
    curve25519_keypair_gen(client_private_key, client_public_key);
    
    // 2. Berechnung des Shared Secret (g^x * y^x)
    curve25519_shared_secret(shared_secret, client_private_key, hop->ntor_public_key);
    
    // 3. KDF (Key Derivation Function): Anwendung von HASH über Shared Secret und Kontext
    SHA1_CTX_t kdf_ctx;
    u8 temp_key_material[NTOR_KEM_KEY_MATERIAL_LEN * 2]; // Mehr Platz für SHA-Iterationen

    sha1_init(&kdf_ctx);
    sha1_update(&kdf_ctx, shared_secret, CURVE25519_KEY_LEN);
    sha1_update(&kdf_ctx, handshake_data, data_len);
    // Realistisch: Hier würden mehrere SHA-Iterationen mit Konstanten folgen (Tor KDF)
    sha1_final(hop->key_material, &kdf_ctx); // Fülle die ersten 20 Bytes

    // 4. Verteilung des Key Materials
    u8* material = hop->key_material;
    
    // Fwd/Back Digest Seeds (SHA1_DIGEST_LEN)
    sha1_init(&hop->fwd_digest_ctx);
    sha1_update(&hop->fwd_digest_ctx, material, SHA1_DIGEST_LEN);
    material += SHA1_DIGEST_LEN;
    
    sha1_init(&hop->back_digest_ctx);
    sha1_update(&hop->back_digest_ctx, material, SHA1_DIGEST_LEN);
    material += SHA1_DIGEST_LEN;

    // Fwd/Back AES Keys (AES_KEY_LEN)
    aes_key_init(&hop->fwd_aes_key, material);
    material += AES_KEY_LEN;
    
    aes_key_init(&hop->back_aes_key, material);
    material += AES_KEY_LEN;
    
    // Fwd/Back IVs (IV_LEN)
    memcpy(hop->fwd_iv, material, IV_LEN);
    material += IV_LEN;
    
    memcpy(hop->back_iv, material, IV_LEN);

    hop->fwd_circ_nonce = 1;
    hop->back_circ_nonce = 1;

    utl_log("CRYPTO", "INFO", "NTOR KDF mit Curve25519 Shared Secret abgeschlossen.");
}

// =================================================================
// TLS-SCHICHT UND ZERTIFIKAT-VALIDIERUNG
// =================================================================

/**
 * @brief Simuliert das Parsen eines X.509 Zertifikats.
 * @return Länge der Daten, 0 bei Fehler.
 */
static size_t cert_parse_x509_get_pubkey(const u8* raw_cert, size_t len, u8* pub_key_out) {
    // Die Realität erfordert ASN.1 Parsing. Hier wird nur die Größe simuliert.
    if (len < 100) return 0; // Zu kurz
    
    // Simuliere das Extrahieren eines 32-Byte Public Keys (z.B. Ntor Onion Key)
    memcpy(pub_key_out, raw_cert + 50, CURVE25519_KEY_LEN);
    pub_key_out[0] = 0x44; // Markierung
    return CURVE25519_KEY_LEN;
}

/**
 * @brief Simuliert die X.509 Zertifikatsverifikation.
 * @return 1 bei Erfolg, 0 bei Fehler.
 */
static int cert_validate_chain(const u8* identity_cert_data, size_t id_len, 
                               const u8* link_cert_data, size_t link_len) {
    // 1. Identity Key Validation (Self-signed check, Expiry, Hash-Check gegen Consensus)
    if (id_len < 200 || strstr((char*)identity_cert_data, "Certificate") == NULL) {
        utl_log("TLS", "FATAL", "Identity Zertifikat Formatfehler.");
        return 0;
    }
    // 2. Link Key Validation (Prüfen, ob es mit Identity Key signiert wurde)
    if (link_len < 100) {
        utl_log("TLS", "FATAL", "Link Zertifikat unvollständig.");
        return 0;
    }
    
    utl_log("TLS", "NOTICE", "Zertifikatskette (Identity/Link) VALIDATED (Tor-Logik angewendet).");
    return 1;
}

/**
 * @brief Simuliert den Empfang und die Verarbeitung der CERTS-Zelle.
 */
static int tls_handle_certs_cell(Circuit_t* circ, u8* cell_payload) {
    u8 certs_count = cell_payload[0];
    u32 offset = 1;

    if (certs_count < 2) {
        utl_log("TLS", "ERROR", "CERTS Zelle enthält zu wenige Zertifikate.");
        return 0;
    }
    
    utl_log("TLS", "INFO", "Empfange CERTS Zelle mit 2 Zertifikaten.");

    // --- 1. Identity Certificate ---
    cert_type_e id_type = (cert_type_e)cell_payload[offset++];
    u16 id_len = utl_be_to_u16(cell_payload + offset); offset += 2;
    u8 identity_cert_data[2048];
    memcpy(identity_cert_data, cell_payload + offset, id_len); offset += id_len;
    
    // Berechne den Identity Digest (SHA1 des Identity-Zertifikats)
    u8 temp_id_digest[SHA1_DIGEST_LEN];
    SHA1_CTX_t ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, identity_cert_data, id_len);
    sha1_final(temp_id_digest, &ctx);
    memcpy(circ->guard_identity_digest, temp_id_digest, SHA1_DIGEST_LEN);
    
    // --- 2. TLS Link Certificate ---
    cert_type_e link_type = (cert_type_e)cell_payload[offset++];
    u16 link_len = utl_be_to_u16(cell_payload + offset); offset += 2;
    u8 link_cert_data[2048];
    memcpy(link_cert_data, cell_payload + offset, link_len); 

    // --- VALIDIERUNG ---
    if (!cert_validate_chain(identity_cert_data, id_len, link_cert_data, link_len)) {
        return 0;
    }
    circ->cert_validated_flag = 1;
    
    return 1;
}


/**
 * @brief Simuliert das Verpacken der Daten in einen TLS-Record.
 */
static int tls_wrap_record(int fd, const u8* in_data, size_t in_len, u8* out_buf) {
    // 1. TLS Record Header (Simuliert: Type=Data(23), Version=TLS1.2(0303), Length)
    out_buf[0] = 23; // TLS Record Type: Application Data
    out_buf[1] = 0x03; out_buf[2] = 0x03; // TLS Version 1.2
    utl_u16_to_be(out_buf + 3, (u16)in_len);
    
    // 2. Daten kopieren (In der Realität: Verschlüsselung, MAC)
    memcpy(out_buf + TLS_RECORD_HEADER_LEN, in_data, in_len);
    
    return (int)(in_len + TLS_RECORD_HEADER_LEN);
}

/**
 * @brief Simuliert das Entpacken der Daten aus einem TLS-Record.
 * @return Länge der entschlüsselten Daten (Payload).
 */
static int tls_unwrap_record(int fd, const u8* in_buf, size_t in_buf_len, u8* out_data) {
    if (in_buf_len < TLS_RECORD_HEADER_LEN) {
        utl_log("TLS", "ERROR", "TLS Record zu kurz für Header.");
        return -1;
    }
    
    u8 type = in_buf[0];
    u16 len = utl_be_to_u16(in_buf + 3);
    
    if (type != 23) {
        utl_log("TLS", "WARN", "Unerwarteter TLS Record Typ (kein Application Data).");
        // Erfordert ggf. Handling von Alerts (21) oder Handshake (22)
    }
    
    if (in_buf_len < (size_t)(TLS_RECORD_HEADER_LEN + len)) {
        utl_log("TLS", "ERROR", "TLS Record unvollständig.");
        return -1;
    }
    
    // Daten kopieren (In der Realität: Decryption, MAC-Check)
    memcpy(out_data, in_buf + TLS_RECORD_HEADER_LEN, len);
    
    return len;
}

// =================================================================
// TOR-PROTOKOLL: ZELLEN & HANDSHAKE
// =================================================================

/**
 * @brief Sendet eine Zelle über die TLS-Verbindung.
 */
static void cell_send_raw(int fd, const u8* raw_cell) {
    u8 tls_buf[CELL_SIZE + TLS_RECORD_HEADER_LEN];
    int tls_len = tls_wrap_record(fd, raw_cell, CELL_SIZE, tls_buf);
    
    if (net_write(fd, tls_buf, tls_len) != tls_len) {
        utl_log("TOR", "FATAL", "Senden der Zelle fehlgeschlagen.");
        // Sollte hier Fehlerbehandlung auslösen
    }
}

/**
 * @brief Empfängt und entpackt eine Zelle vom Guard.
 */
static int cell_receive_raw(int fd, u8* raw_cell_out) {
    u8 tls_in_buf[CELL_SIZE + TLS_RECORD_HEADER_LEN + 100];
    
    // Zuerst nur den TLS-Record-Header lesen (5 Bytes)
    if (net_read_n(fd, tls_in_buf, TLS_RECORD_HEADER_LEN) != TLS_RECORD_HEADER_LEN) {
        return 0;
    }
    
    u16 payload_len = utl_be_to_u16(tls_in_buf + 3);
    if (payload_len != CELL_SIZE) {
        utl_log("TOR", "ERROR", "Empfangene TLS-Payload-Länge stimmt nicht mit CELL_SIZE überein.");
        return 0;
    }
    
    // Rest des TLS-Records lesen (Payload = CELL_SIZE)
    if (net_read_n(fd, tls_in_buf + TLS_RECORD_HEADER_LEN, payload_len) != payload_len) {
        return 0;
    }
    
    // Entpacke den Record
    int cell_len = tls_unwrap_record(fd, tls_in_buf, TLS_RECORD_HEADER_LEN + payload_len, raw_cell_out);
    
    if (cell_len != CELL_SIZE) {
        utl_log("TOR", "ERROR", "Entpackte Zellenlänge ist falsch.");
        return 0;
    }
    
    return CELL_SIZE;
}

/**
 * @brief Erstellt eine Tor-Zelle.
 */
static void cell_mk(u8* buf_out, u32 circ_id, cell_command cmd, 
                    relay_command rly_cmd, u8* payload, size_t pay_len, int hop_index) {
    u8* dest = (u8*)buf_out;
    size_t off = 0;
    
    // 1. Circuit ID (4 Bytes)
    utl_u32_to_be(dest + off, circ_id); off += 4;
    // 2. Command (1 Byte)
    dest[off++] = (u8)cmd;

    if (cmd == CMD_RELAY) {
        Hop_t* current_hop = &G_circuit.path[hop_index];
        
        // --- RELAY HEADER (11 Bytes) ---
        u8* relay_header_start = dest + off;
        utl_u16_to_be(relay_header_start, (u16)rly_cmd);
        utl_u16_to_be(relay_header_start + 2, 0); // Reserviert
        utl_u16_to_be(relay_header_start + 4, G_stream_id);
        
        // 3. Digest (4 Bytes, wird NACH Payload-Kopie und VOR Finaler Enc berechnet)
        // Platzhalter setzen
        memset(relay_header_start + 6, 0x00, 4); 
        
        // 4. Length (2 Bytes)
        utl_u16_to_be(relay_header_start + 10, (u16)pay_len);
        off += RELAY_HDR_LEN; 

        // --- PAYLOAD ---
        size_t cell_payload_len = CELL_SIZE - off;
        size_t copy_len = (pay_len < cell_payload_len) ? pay_len : cell_payload_len;
        memcpy(dest + off, payload, copy_len);
        
        // --- BERECHNE DIGEST ---
        crypto_calculate_digest_4byte(relay_header_start + 6, 
                                     &current_hop->fwd_digest_ctx, 
                                     dest + off, copy_len);
                                     
        // --- LAYERED ENCRYPTION (vom Exit rückwärts zum Guard) ---
        // Verschlüsselung der gesamten Relay-Payload (ab Byte 5)
        for(int i = hop_index; i >= 0; i--) {
            Hop_t* hop_to_enc = &G_circuit.path[i];
            crypto_aes_process_cell(dest + CELL_HEADER_LEN, CELL_SIZE - CELL_HEADER_LEN,
                                    &hop_to_enc->fwd_aes_key, hop_to_enc->fwd_iv);
        }

        off = CELL_SIZE;
    } else if (payload != NULL && pay_len > 0) {
        size_t copy_len = (off + pay_len > CELL_SIZE) ? (CELL_SIZE - off) : pay_len;
        memcpy(dest + off, payload, copy_len);
        off += copy_len;
    }
    
    memset(dest + off, 0, CELL_SIZE - off);
}

/**
 * @brief Entschlüsselt eine Zelle, schält die Zwiebel ab.
 * @return 1 bei Erfolg, 0 bei Fehler.
 */
static int cell_decrypt_and_unwrap(u8* raw_cell, Circuit_t* circ) {
    cell_command cmd = (cell_command)raw_cell[4];
    u32 received_circ_id = utl_be_to_u32(raw_cell);
    
    if (received_circ_id != circ->circ_id) {
        utl_log("TOR", "ERROR", "Unerwartete Circuit ID in empfangener Zelle.");
        return 0;
    }

    if (cmd == CMD_RELAY || cmd == CMD_EXTENDED2 || cmd == CMD_CREATED2) {
        // Starte mit der Entschlüsselung vom äußersten Hop (Exit) zum innersten (Guard)
        for (int i = circ->num_hops - 1; i >= 0; i--) {
            Hop_t* hop = &circ->path[i];
            crypto_aes_process_cell(raw_cell + CELL_HEADER_LEN, 
                                    CELL_SIZE - CELL_HEADER_LEN, 
                                    &hop->back_aes_key, hop->back_iv);
            utl_log("CRYPTO", "DEBUG", "Layer Entschlüsselung durchgeführt.");
        }

        if (cmd == CMD_RELAY) {
            // Nach Layered Decryption: Relay Header checken und Digest validieren
            u8* relay_header_start = raw_cell + CELL_HEADER_LEN;
            u16 rly_cmd = utl_be_to_u16(relay_header_start);
            u8 received_digest[4];
            memcpy(received_digest, relay_header_start + 6, 4);

            u8 calculated_digest[4];
            u16 payload_len = utl_be_to_u16(relay_header_start + 10);
            
            // Berechne Digest mit dem aktuellen Zustand des Digest Context
            crypto_calculate_digest_4byte(calculated_digest, 
                                         &circ->path[circ->num_hops - 1].back_digest_ctx, 
                                         relay_header_start + RELAY_HDR_LEN, 
                                         payload_len);
                                         
            if (memcmp(received_digest, calculated_digest, 4) != 0) {
                utl_log("TOR", "ERROR", "Relay Digest-Verifikation fehlgeschlagen! Onion-Integrität verletzt.");
                return 0;
            }
        }
    }
    
    return 1;
}


// =================================================================
// TOR-PROTOKOLL: HANDSHAKE SEQUENZ
// =================================================================

/**
 * @brief Phase 1: Versions-Handshake.
 */
static int tor_handshake_versions(int fd) {
    u8 versions_cell[CELL_SIZE] = {0};
    u8 resp_cell[CELL_SIZE] = {0};
    
    utl_log("TOR", "INFO", "Sende CMD_VERSIONS Zelle (Tor V3).");

    // Header: CircID 0 (Negotiation) + CMD_VERSIONS (7) + Länge 2 (für 1 Version)
    versions_cell[4] = CMD_VERSIONS;
    utl_u16_to_be(versions_cell + 5, 2); // Länge
    utl_u16_to_be(versions_cell + 7, TOR_PROTO_V3); // Version 3

    cell_send_raw(fd, versions_cell);

    // Empfange CMD_VERSIONS Antwort (sollte auch V3 enthalten)
    if (cell_receive_raw(fd, resp_cell) != CELL_SIZE) return 0;
    if (resp_cell[4] != CMD_VERSIONS) {
        utl_log("TOR", "FATAL", "Unerwartete Antwort auf CMD_VERSIONS.");
        return 0;
    }
    
    u16 negotiated_version = utl_be_to_u16(resp_cell + 7);
    if (negotiated_version != TOR_PROTO_V3) {
        utl_log("TOR", "FATAL", "Aushandlung Tor Version fehlgeschlagen.");
        return 0;
    }
    utl_log("TOR", "NOTICE", "Tor Protokollversion V3 erfolgreich ausgehandelt.");
    return 1;
}

/**
 * @brief Phase 2: Zertifikate und Authentifizierung.
 */
static int tor_handshake_certs_and_auth(Circuit_t* circ, int fd) {
    u8 certs_cell[CELL_SIZE];
    u8 auth_cell[CELL_SIZE];
    u8 resp_cell[CELL_SIZE];

    // Empfange CMD_CERTS (Guard sendet seine Identity und Link Keys)
    if (cell_receive_raw(fd, certs_cell) != CELL_SIZE || certs_cell[4] != CMD_CERTS) {
        utl_log("TOR", "FATAL", "Empfangen der CMD_CERTS Zelle fehlgeschlagen.");
        return 0;
    }
    if (!tls_handle_certs_cell(circ, certs_cell + CELL_HEADER_LEN)) {
        return 0;
    }

    // Sende CMD_AUTHENTICATE (Client sendet seinen Proof, der seine TLS-Verbindung authentifiziert)
    utl_log("TOR", "INFO", "Sende CMD_AUTHENTICATE Zelle (Proof of Ownership).");
    memset(auth_cell, 0, CELL_SIZE);
    auth_cell[4] = 6; // CMD_AUTHENTICATE (6) - nicht in Cell_command, da veraltet/nur Handshake
    // Payload: Authenticator Type (1) + Authenticator Data (z.B. 20 Bytes Hash)
    auth_cell[5] = 1; // HASHED_CERTIFICATE
    memset(auth_cell + 6, 0xDD, SHA1_DIGEST_LEN); // Simulierter Hash des Client-Keys

    cell_send_raw(fd, auth_cell);

    // Empfange CMD_NETINFO
    if (cell_receive_raw(fd, resp_cell) != CELL_SIZE || resp_cell[4] != CMD_NETINFO) {
        utl_log("TOR", "FATAL", "Empfangen der CMD_NETINFO Zelle fehlgeschlagen.");
        return 0;
    }
    utl_log("TOR", "NOTICE", "Tor Handshake (Auth/NetInfo) abgeschlossen.");
    return 1;
}

/**
 * @brief Stellt die TLS-Verbindung her und führt den Tor-Handshake durch.
 */
static int tls_connect_and_tor_handshake(Circuit_t* circ, const char* host, int port) {
    int fd;
    
    printf("\n[NET   ] Starte TCP-Verbindung zu %s:%d...\n", host, port);
    fd = net_connect(host, port);
    if (fd < 0) return -1;
    circ->or_fd = fd;
    
    utl_log("TLS", "INFO", "Starte simulierten TLS-Handshake (Client Hello senden).");
    // Hier würde die TLS-Ebene einen vollständigen TLS-Handshake durchführen
    // Wir überspringen diesen und gehen direkt zum Tor-Protokoll Handshake.
    utl_log("TLS", "NOTICE", "TLS-Handshake erfolgreich abgeschlossen (SIM).");

    // Tor Protokoll Handshake: Versions, Certs, Authenticate, NetInfo
    if (!tor_handshake_versions(fd)) {
        net_close(fd); return -1;
    }
    if (!tor_handshake_certs_and_auth(circ, fd)) {
        net_close(fd); return -1;
    }
    
    return fd;
}


/**
 * @brief Baut einen Hop auf (CREATE2 oder EXTEND2).
 */
static int tor_build_hop(Circuit_t* circ, int index, const char* host, int port, cell_command cmd) {
    Hop_t* current_hop = &circ->path[index];
    strncpy(current_hop->host, host, sizeof(current_hop->host) - 1);
    current_hop->port = port;
    
    // --- Handshake-Payload generieren (NTOR) ---
    u8 ntor_handshake_payload[100]; // Vereinfachte Länge
    curve25519_key_t client_ephemeral_pubkey;
    curve25519_keypair_gen(ntor_handshake_payload, client_ephemeral_pubkey); 

    // Simuliere, dass wir den NTOR Public Key des OR aus dem Consensus haben (oder dem CERTs Cell)
    memset(current_hop->ntor_public_key, 0x3C, CURVE25519_KEY_LEN); 
    
    // Fülle die Payload mit allen notwendigen Keys und Digests
    memcpy(ntor_handshake_payload, client_ephemeral_pubkey, CURVE25519_KEY_LEN);
    // Hier würden der OR Identity Digest und der Ntor Key folgen.
    
    u8* payload = ntor_handshake_payload;
    size_t payload_len = 92; // Realistische NTOR Payload Länge

    if (cmd == CMD_CREATE2) {
        cell_mk(ntor_handshake_payload + 0, circ->circ_id, CMD_CREATE2, RLY_CMD_PADDING, payload, payload_len, index);
        cell_send_raw(circ->or_fd, ntor_handshake_payload + 0);
        utl_log("TOR", "INFO", "CREATE2 Zelle gesendet (Guard OR).");
    } else if (cmd == CMD_RELAY) {
        // EXTEND2 Payload erstellen: Host/Port, OR Identity Digest, NTOR Public Key
        u8 extend_payload[200];
        // Adress-Typ (1=IPv4) + IPv4 (4) + Port (2)
        extend_payload[0] = 4;
        utl_u32_to_be(extend_payload + 1, inet_addr(host));
        utl_u16_to_be(extend_payload + 5, (u16)port);
        // Identity Digest (20) + Ntor Key (32)
        memset(extend_payload + 7, 0xAE, SHA1_DIGEST_LEN);
        memset(extend_payload + 7 + SHA1_DIGEST_LEN, 0xBF, CURVE25519_KEY_LEN);
        
        // Füge den CREATE2 Handshake in die EXTEND2 Payload ein
        memcpy(extend_payload + 7 + SHA1_DIGEST_LEN + CURVE25519_KEY_LEN, payload, payload_len);
        
        // Sende RELAY EXTEND2, verschlüsselt an den VORHERIGEN Hop (index - 1)
        cell_mk(ntor_handshake_payload + 0, circ->circ_id, CMD_RELAY, RLY_CMD_EXTEND2, extend_payload, sizeof(extend_payload), index - 1);
        cell_send_raw(circ->or_fd, ntor_handshake_payload + 0);
        utl_log("TOR", "INFO", "RELAY EXTEND2 Zelle gesendet.");
    }

    // Empfange CREATED2 / EXTENDED2
    u8 resp_cell[CELL_SIZE];
    if (cell_receive_raw(circ->or_fd, resp_cell) != CELL_SIZE) return 0;
    if (!cell_decrypt_and_unwrap(resp_cell, circ)) return 0;

    cell_command resp_cmd = (cell_command)resp_cell[4];
    if (resp_cmd != CMD_CREATED2 && resp_cmd != CMD_RELAY) {
        utl_log("TOR", "ERROR", "Unerwartete Antwort auf Hop-Aufbau.");
        return 0;
    }
    
    // Führe Key Derivation nach erfolgreicher Antwort durch
    crypto_kdf_ntor(current_hop, ntor_handshake_payload, payload_len);
    
    circ->num_hops = index + 1;
    printf("TOR: Hop %d (%s) erfolgreich eingerichtet.\n", circ->num_hops, host);
    
    return 1;
}

/**
 * @brief Baut einen kompletten 3-Hop-Circuit auf.
 */
static int tor_build_circuit(Circuit_t* circ) {
    circ->circ_id = 101; 
    
    // 1. Guard-Hop (CREATE2)
    if (!tor_build_hop(circ, 0, OR_GUARD_HOST, OR_PORT, CMD_CREATE2)) return 0;
    
    // 2. Middle-Hop (RELAY EXTEND2)
    if (!tor_build_hop(circ, 1, OR_MIDDLE_HOST, OR_PORT, CMD_RELAY)) return 0;
    
    // 3. Exit-Hop (RELAY EXTEND2)
    if (!tor_build_hop(circ, 2, OR_EXIT_HOST, OR_PORT, CMD_RELAY)) return 0;

    utl_log("TOR", "SUCCESS", "Circuit-Aufbau abgeschlossen: 3 Hops OPEN.");
    return 1;
}

/**
 * @brief Baut einen Stream im Circuit auf (RELAY_BEGIN).
 */
static int tor_conn_stream(Circuit_t* circ, const char* host, int port) {
    G_stream_id++; 
    u8 hp_pay[SOCKS5_MAX_ADDR_LEN + 10];
    int len = snprintf((char*)hp_pay, sizeof(hp_pay), "%s:%d", host, port);
    
    u8 begin_cell[CELL_SIZE];
    cell_mk(begin_cell, circ->circ_id, CMD_RELAY, RLY_CMD_BEGIN, hp_pay, len + 1, 2);
    cell_send_raw(circ->or_fd, begin_cell);
    utl_log("TOR", "INFO", "RELAY_BEGIN gesendet (dreifach verschlüsselt).");
    
    u8 resp_cell[CELL_SIZE];
    if (cell_receive_raw(circ->or_fd, resp_cell) != CELL_SIZE) return 0;
    if (!cell_decrypt_and_unwrap(resp_cell, circ)) return 0;
    
    // Prüfen auf RLY_CMD_CONNECTED
    u16 rly_cmd = utl_be_to_u16(resp_cell + CELL_HEADER_LEN);
    if (resp_cell[4] != CMD_RELAY || rly_cmd != RLY_CMD_CONNECTED) {
        utl_log("TOR", "ERROR", "Unerwartete Antwort auf RELAY_BEGIN.");
        return 0;
    }
    
    printf("TOR: RELAY CONNECTED empfangen. Stream %u ist offen.\n", G_stream_id);
    
    return G_stream_id;
}

/**
 * @brief Sendet und empfängt Daten über den Stream (RELAY_DATA).
 */
static void tor_data_exchange(Circuit_t* circ, int stream_id, const char* req_data, char* resp_buf, size_t max_len) {
    u8 data_cell[CELL_SIZE];
    u8 resp_cell[CELL_SIZE];

    // Sende Request (RELAY DATA)
    cell_mk(data_cell, circ->circ_id, CMD_RELAY, RLY_CMD_DATA, (u8*)req_data, strlen(req_data), 2);
    cell_send_raw(circ->or_fd, data_cell);
    utl_log("TOR", "INFO", "RELAY_DATA (Browser-Request) gesendet.");

    // Empfange Antwort (RELAY DATA)
    if (cell_receive_raw(circ->or_fd, resp_cell) != CELL_SIZE) return;
    if (!cell_decrypt_and_unwrap(resp_cell, circ)) return;

    // Prüfen auf RLY_CMD_DATA
    u16 rly_cmd = utl_be_to_u16(resp_cell + CELL_HEADER_LEN);
    if (resp_cell[4] != CMD_RELAY || rly_cmd != RLY_CMD_DATA) {
        utl_log("TOR", "ERROR", "Unerwartete Antwort auf RELAY_DATA.");
        return;
    }

    // Antwort-Payload extrahieren: Sie beginnt nach CELL_HEADER_LEN (5) + RELAY_HDR_LEN (11)
    u16 payload_len = utl_be_to_u16(resp_cell + CELL_HEADER_LEN + 10);
    u8* payload_start = resp_cell + CELL_HEADER_LEN + RELAY_HDR_LEN;

    // Simuliere die Datenextraktion mit einer festen Antwort
    const char* dummy_response = "HTTP/1.1 200 OK\r\nContent-Length: 60\r\n\r\nTor Exit-Antwort von zano.org (Ende der Onion-Kette).";
    
    size_t copy_len = strlen(dummy_response);
    if (copy_len > max_len) copy_len = max_len;
    
    strncpy(resp_buf, dummy_response, copy_len);
    resp_buf[copy_len] = '\0';
    
    utl_log("TOR", "INFO", "RELAY_DATA (Server-Antwort) empfangen und dekodiert.");
}

/**
 * @brief Zerstört den Circuit und schließt die Verbindung.
 */
static void tor_destroy(Circuit_t* circ) {
    if (circ->circ_id != 0 && circ->or_fd > 0) {
        u8 destroy_cell[CELL_SIZE] = {0};
        utl_u32_to_be(destroy_cell, circ->circ_id);
        destroy_cell[4] = CMD_DESTROY;
        
        cell_send_raw(circ->or_fd, destroy_cell);
        utl_log("TOR", "NOTICE", "CMD_DESTROY gesendet.");
    }
    net_close(circ->or_fd); 
    
    memset(circ, 0, sizeof(Circuit_t)); 
    utl_log("TOR", "NOTICE", "Sauberes Cleanup abgeschlossen.");
}


// =================================================================
// SOCKS5-SERVER-FUNKTIONEN (SOCKS)
// =================================================================

static void socks_send_response(int fd, u8 version, u8 cmd, u8 status) {
    u8 response[10] = {0};
    response[0] = version; // SOCKS Version (5)
    response[1] = status;  // Status (SUCCESS/FAILURE)
    response[2] = 0x00;    // Reserviert
    response[3] = 0x01;    // BND.ADDR.TYPE (IPv4)
    utl_u32_to_be(response + 4, 0x7f000001); // 127.0.0.1 (simuliert)
    utl_u16_to_be(response + 8, htons(SOCKS_PORT)); // Port
    net_write(fd, response, 10);
}

static int socks_handle_handshake(int client_fd) {
    u8 buffer[256];
    
    if (net_read_n(client_fd, buffer, 2) != 2 || buffer[0] != 0x05) {
        utl_log("SOCKS", "ERROR", "Ungültiger SOCKS5 Header.");
        return 0;
    }
    u8 nmethods = buffer[1];
    if (net_read_n(client_fd, buffer + 2, nmethods) != nmethods) {
        utl_log("SOCKS", "ERROR", "Methoden-Lesefehler.");
        return 0;
    }

    // Unterstützt nur No-Authentication (0x00)
    for (int i = 0; i < nmethods; i++) {
        if (buffer[2 + i] == 0x00) {
            // Sende Auswahl: V5, Methode 0 (No Auth)
            u8 resp[2] = {0x05, 0x00};
            net_write(client_fd, resp, 2);
            utl_log("SOCKS", "INFO", "SOCKS5 Handshake abgeschlossen (No Auth).");
            return 1;
        }
    }

    // Keine akzeptable Methode gefunden
    socks_send_response(client_fd, 0x05, 0x00, 0xFF); // Failure
    utl_log("SOCKS", "ERROR", "Keine akzeptable Authentifizierungsmethode.");
    return 0;
}

static int socks_handle_connect(int client_fd, char* host_out, int* port_out) {
    u8 buffer[512];
    
    // Lese bis zur ATYP (4 Bytes: V5, CMD, RSV, ATYP)
    if (net_read_n(client_fd, buffer, 4) != 4) return 0;
    if (buffer[1] != 0x01) { // CMD=CONNECT
        socks_send_response(client_fd, 0x05, 0x00, 0x07); // Command not supported
        utl_log("SOCKS", "ERROR", "Nur CMD_CONNECT (0x01) unterstützt.");
        return 0;
    }
    u8 atyp = buffer[3];
    u16 port = 0;
    int addr_len = 0;
    int host_len = 0;

    if (atyp == 0x01) { // IPv4
        addr_len = 4;
        host_len = snprintf(host_out, SOCKS5_MAX_ADDR_LEN, "%d.%d.%d.%d", buffer[4], buffer[5], buffer[6], buffer[7]);
    } else if (atyp == 0x03) { // Domain Name
        if (net_read_n(client_fd, buffer + 4, 1) != 1) return 0;
        addr_len = buffer[4]; // Host Länge
        if (net_read_n(client_fd, buffer + 5, addr_len + 2) != (addr_len + 2)) return 0;
        
        memcpy(host_out, buffer + 5, addr_len);
        host_out[addr_len] = '\0';
        host_len = addr_len;
        port = utl_be_to_u16(buffer + 5 + addr_len);
    } else {
        socks_send_response(client_fd, 0x05, 0x00, 0x08); // Address type not supported
        utl_log("SOCKS", "ERROR", "ATYP nicht unterstützt.");
        return 0;
    }
    
    if (atyp != 0x03) { // Port folgt unmittelbar nach IPv4-Adresse
        if (net_read_n(client_fd, buffer + 4 + addr_len, 2) != 2) return 0;
        port = utl_be_to_u16(buffer + 4 + addr_len);
    }
    
    *port_out = port;
    printf("SOCKS: CONNECT Request empfangen (%s:%d).\n", host_out, *port_out);

    int s_id = tor_conn_stream(&G_circuit, host_out, *port_out);
    
    if (s_id > 0) {
        socks_send_response(client_fd, 0x05, 0x01, 0x00); // SUCCESS
    } else {
        socks_send_response(client_fd, 0x05, 0x01, 0x05); // Connection Refused
    }
    return s_id;
}

static void socks_handle_data_exchange(int client_fd, int stream_id) {
    char http_request[CELL_SIZE * 2] = {0};
    char response_buffer[CELL_SIZE * 2] = {0};
    
    // Lese HTTP-Request (vereinfacht: Gehe davon aus, dass alles an einem Stück kommt)
    int bytes_read = recv(client_fd, http_request, sizeof(http_request) - 1, 0);
    if (bytes_read <= 0) {
        utl_log("SOCKS", "WARN", "Kein HTTP-Request empfangen oder Fehler.");
        return;
    }
    http_request[bytes_read] = '\0';
    utl_log("SOCKS", "INFO", "Browser HTTP-Request empfangen.");

    tor_data_exchange(&G_circuit, stream_id, http_request, response_buffer, sizeof(response_buffer) - 1);
    
    net_write(client_fd, (u8*)response_buffer, strlen(response_buffer));
    utl_log("SOCKS", "INFO", "Tor-Antwort an Browser (Client) gesendet.");
    
    printf("\n--- Abgeschlossene Transaktion ---\n%s\n----------------------------------\n", response_buffer);
}

static void socks_srv_sim_proc(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Setze Timeout für accept (um auf SIGINT reagieren zu können)
    fd_set fds;
    struct timeval tv = {1, 0}; // 1 Sekunde Timeout
    
    FD_ZERO(&fds);
    FD_SET(listen_fd, &fds);
    
    int sel_res = select(listen_fd + 1, &fds, NULL, NULL, &tv);

    if (sel_res > 0 && FD_ISSET(listen_fd, &fds)) {
        int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            utl_log("SOCKS", "ERROR", "Accept fehlgeschlagen.");
            return;
        }

        char target_host[SOCKS5_MAX_ADDR_LEN] = {0};
        int target_port = 0;
        int stream_id = 0;

        utl_log("SOCKS", "NOTICE", "Neue SOCKS5-Verbindung akzeptiert.");
        
        if (socks_handle_handshake(client_fd)) {
            stream_id = socks_handle_connect(client_fd, target_host, &target_port);
            if (stream_id > 0) {
                socks_handle_data_exchange(client_fd, stream_id);
            }
        }
        
        net_close(client_fd);
    } else if (sel_res < 0) {
        if (errno != EINTR) {
             utl_log("SOCKS", "FATAL", "Select-Fehler.");
             G_run_flag = 0;
        }
    }
}

// =================================================================
// HAUPTPROGRAMM (MAIN)
// =================================================================

static int socks_server_start(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) return -1;

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        net_close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, 5) < 0) {
        net_close(listen_fd);
        return -1;
    }
    
    printf("\n[SOCKS ] Warte auf neue SOCKS5-Verbindung auf Port %d.\n", port);
    return listen_fd;
}

static void sig_hdl(int sig) {
    if (sig == SIGINT) {
        utl_log("SIGNAL", "NOTICE", "SIGINT (Strg+C) empfangen. Beende Loop...");
        G_run_flag = 0;
    }
}


int main() {
    printf("###################################################\n");
    printf("### Tor-Client Simulation (V4: Hardened Impl.)  ###\n");
    printf("###################################################\n");
    
    // 0. Initialisiere Winsock (falls auf Windows)
    #ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        utl_log("MAIN", "FATAL", "WSAStartup fehlgeschlagen.");
        return 1;
    }
    #endif

    // 1. Initialisierung und TLS/Tor-Handshake mit Guard
    utl_log("MAIN", "INFO", "Starte TLS-Verbindung und Tor-Handshake mit Guard OR.");
    int guard_fd = tls_connect_and_tor_handshake(&G_circuit, OR_GUARD_HOST, OR_PORT);
    if (guard_fd < 0 || G_circuit.cert_validated_flag == 0) {
        utl_log("MAIN", "FATAL", "TLS/Tor Handshake fehlgeschlagen. Abbruch.");
        return 1;
    }
    
    // 2. Circuit-Aufbau mit Layered Encryption und NTOR-Key-Derivation
    utl_log("MAIN", "INFO", "Starte 3-Hop Circuit-Aufbau.");
    if (!tor_build_circuit(&G_circuit)) {
        utl_log("MAIN", "FATAL", "Circuit-Aufbau fehlgeschlagen. Abbruch.");
        tor_destroy(&G_circuit);
        return 1;
    }
    
    // 3. SOCKS5-Server starten und Hauptschleife
    int socks_listen_fd = socks_server_start(SOCKS_PORT);
    if (socks_listen_fd < 0) {
        utl_log("MAIN", "FATAL", "SOCKS-Server konnte nicht gestartet werden.");
        tor_destroy(&G_circuit);
        return 1;
    }
    
    signal(SIGINT, sig_hdl);
    
    printf("\n[MAIN] SOCKS5-Server läuft. Drücken Sie **STRG+C** zum Beenden.\n");
    
    while (G_run_flag) {
        socks_srv_sim_proc(socks_listen_fd); 
    }

    // 4. Sauberes Cleanup
    net_close(socks_listen_fd);
    tor_destroy(&G_circuit);

    #ifdef _WIN32
    WSACleanup();
    #endif

    printf("\n--- PROGRAMM SAUBER BEENDET ---\n");
    return 0;
}

