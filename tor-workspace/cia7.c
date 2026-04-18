#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include <fcntl.h> // Für F_SETFL

// Bedingte Includes für verschiedene Betriebssysteme
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
    typedef int socklen_t;
    #define INET_ADDRSTRLEN 16
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netdb.h>
    #include <errno.h>
    #include <sys/time.h>
#endif

// OpenSSL-Includes (Kryptografie und SSL/TLS)
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

// =================================================================
// TYPEDEFINITIONEN UND GLOBALE KONSTANTEN
// =================================================================

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define SOCKS_PORT 9050
#define CELL_SIZE 512
#define CELL_HEADER_LEN 5
#define RELAY_HDR_LEN 11
#define RELAY_PAYLOAD_LEN (CELL_SIZE - CELL_HEADER_LEN - RELAY_HDR_LEN)
#define SOCKS5_MAX_ADDR_LEN 256
#define IO_TIMEOUT_SEC 10

// Server Simulation (MÜSSEN FÜR ECHTEN BETRIEB DURCH ECHTE RELAY-DATEN ERSETZT WERDEN)
#define OR_GUARD_HOST "192.0.2.1"     
#define OR_MIDDLE_HOST "198.51.100.2" 
#define OR_EXIT_HOST "203.0.113.3"    
#define OR_PORT 9001

// Kryptographische Konstanten
#define SHA1_DIGEST_LEN 20
#define AES_KEY_LEN 16        
#define IV_LEN 16             
#define CURVE25519_KEY_LEN 32 
#define SHA256_DIGEST_LENGTH 32 // Für HKDF
#define HKDF_OUTPUT_LEN (2 * SHA1_DIGEST_LEN + 2 * AES_KEY_LEN + 2 * IV_LEN) 
#define MAX_HOPS 3

// ... [Cell Commands und Relay Commands Enums bleiben gleich] ...
typedef enum {CMD_CREATE2=1, CMD_CREATED2=2, CMD_RELAY=3, CMD_DESTROY=4} cell_command;
typedef enum {RLY_CMD_EXTEND2=6, RLY_CMD_EXTENDED2=7, RLY_CMD_BEGIN=1, RLY_CMD_DATA=2, RLY_CMD_END=3, RLY_CMD_CONNECTED=4} relay_command;

typedef struct {
    char host[64];
    int port;
    u8 key_fwd[AES_KEY_LEN]; 
    u8 key_back[AES_KEY_LEN]; 
    u8 iv_fwd[IV_LEN]; // Forward IV (wird als Counter verwendet)
    u8 iv_back[IV_LEN]; // Backward IV (wird als Counter verwendet)
    u8 hmac_fwd[SHA1_DIGEST_LEN]; 
    u8 hmac_back[SHA1_DIGEST_LEN]; 
    u8 ntor_public_key[CURVE25519_KEY_LEN];
    EVP_CIPHER_CTX *ctx_fwd;
    EVP_CIPHER_CTX *ctx_back;
} Hop_t;

typedef struct {
    u32 circ_id;
    int num_hops;
    int or_fd; 
    SSL_CTX *ssl_ctx; 
    SSL *ssl_handle; 
    Hop_t path[MAX_HOPS];
} Circuit_t;

Circuit_t G_circuit = {0};
static volatile int running = 1;

// =================================================================
// UTILITY FUNKTIONEN (UTL)
// =================================================================

static void utl_logf(const char* module, const char* level, const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    printf("[%s] %s: %s\n", module, level, buffer);
}

static void utl_u16_to_be(u8* buf, u16 val) {
    buf[0] = (u8)(val >> 8);
    buf[1] = (u8)val;
}

static void utl_u32_to_be(u8* buf, u32 val) {
    buf[0] = (u8)(val >> 24);
    buf[1] = (u8)(val >> 16);
    buf[2] = (u8)(val >> 8);
    buf[3] = (u8)val;
}

/**
 * @brief Inkrementiert einen AES-Counter (Big Endian) um 1.
 * Dies ist die Tor-spezifische Zählerverwaltung.
 */
static void aes_ctr_increment(u8* ctr) {
    for (int i = IV_LEN - 1; i >= 0; i--) {
        if (++ctr[i] != 0) break;
    }
}


// =================================================================
// NETZWERK FUNKTIONEN (NET)
// =================================================================

static int net_connect(const char* host, int port) {
    int sockfd = -1;
    struct sockaddr_in server_addr;
    struct hostent *server;

    #ifndef _WIN32
    server = gethostbyname(host);
    if (server == NULL) {
        utl_logf("NET", "ERROR", "Hostname-Auflösung fehlgeschlagen für %s.", host);
        return -1;
    }
    #endif

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    struct timeval tv;
    tv.tv_sec = IO_TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    #ifdef _WIN32
    inet_pton(AF_INET, host, &server_addr.sin_addr);
    #else
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    #endif

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        return -1;
    }
    utl_logf("NET", "INFO", "TCP-Verbindung zu %s:%d hergestellt.", host, port);
    return sockfd;
}

/**
 * @brief Liest Daten von der SSL-Verbindung (blockierend).
 */
static ssize_t ssl_read_fixed(SSL *ssl, u8 *buf, size_t len) {
    size_t total_received = 0;
    int ret;
    while (total_received < len) {
        ret = SSL_read(ssl, buf + total_received, len - total_received);
        if (ret <= 0) {
            utl_logf("SSL", "ERROR", "SSL_read fehlgeschlagen oder EOF. Ret: %d", ret);
            return -1;
        }
        total_received += ret;
    }
    return total_received;
}

/**
 * @brief Schreibt Daten über die SSL-Verbindung (blockierend).
 */
static ssize_t ssl_write_fixed(SSL *ssl, const u8 *buf, size_t len) {
    size_t total_sent = 0;
    int ret;
    while (total_sent < len) {
        ret = SSL_write(ssl, buf + total_sent, len - total_sent);
        if (ret <= 0) {
            utl_logf("SSL", "ERROR", "SSL_write fehlgeschlagen. Ret: %d", ret);
            return -1;
        }
        total_sent += ret;
    }
    return total_sent;
}


// =================================================================
// KRYPTOGRAPHIE MIT OPENSSL (CRYPTO)
// =================================================================

static int crypto_aes_init(Hop_t* hop) {
    hop->ctx_fwd = EVP_CIPHER_CTX_new();
    hop->ctx_back = EVP_CIPHER_CTX_new();
    if (!hop->ctx_fwd || !hop->ctx_back) return 0;
    EVP_EncryptInit_ex(hop->ctx_fwd, EVP_aes_128_ctr(), NULL, NULL, NULL); 
    EVP_DecryptInit_ex(hop->ctx_back, EVP_aes_128_ctr(), NULL, NULL, NULL);
    return 1;
}

/**
 * @brief Führt AES-128 CTR Ver- oder Entschlüsselung durch.
 */
static int crypto_aes_crypt(EVP_CIPHER_CTX *ctx, const u8* key, u8* iv, u8* data, int len) {
    int outlen;
    // Setze Key und den aktuellen Counter (IV). Wichtig: Muss vor jedem Aufruf neu gesetzt werden!
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv)) return 0;
    if (!EVP_EncryptUpdate(ctx, data, &outlen, data, len)) return 0;
    return 1;
}

static void crypto_sha1_hmac(const u8* key, size_t key_len, const u8* data, size_t len, u8* digest_out) {
    HMAC(EVP_sha1(), key, key_len, data, len, digest_out, NULL);
}

static void hkdf_extract(const u8* salt, size_t salt_len, const u8* ikm, size_t ikm_len, u8* prk_out) {
    HMAC(EVP_sha256(), salt, salt_len, ikm, ikm_len, prk_out, NULL);
}

static void hkdf_expand(const u8* prk, size_t prk_len, const u8* info, size_t info_len, u8* okm_out, size_t okm_len) {
    u8 t_buf[SHA256_DIGEST_LENGTH + 1 + 512]; 
    u8 t_output[SHA256_DIGEST_LENGTH];

    size_t offset = 0;
    u8 counter = 0x01;
    
    // T(1) = HMAC(PRK, info || 0x01)
    // T(i) = HMAC(PRK, T(i-1) || info || counter)
    while (offset < okm_len) {
        size_t t_len = 0;
        if (offset > 0) {
            memcpy(t_buf, okm_out + offset - SHA256_DIGEST_LENGTH, SHA256_DIGEST_LENGTH);
            t_len += SHA256_DIGEST_LENGTH;
        }
        
        memcpy(t_buf + t_len, info, info_len);
        t_len += info_len;
        
        t_buf[t_len] = counter;
        t_len += 1;
        
        HMAC(EVP_sha256(), prk, prk_len, t_buf, t_len, t_output, NULL);
        
        size_t copy_len = (okm_len - offset > SHA256_DIGEST_LENGTH) ? SHA256_DIGEST_LENGTH : (okm_len - offset);
        memcpy(okm_out + offset, t_output, copy_len);
        
        offset += copy_len;
        counter++;
    }
}

static void crypto_kdf_ntor(Hop_t* hop, const u8* handshake_data, size_t handshake_len) {
    u8 shared_secret_sim[CURVE25519_KEY_LEN];
    RAND_bytes(shared_secret_sim, CURVE25519_KEY_LEN);
    
    u8 ikm_sim[256]; 
    size_t ikm_len = 0;
    
    memcpy(ikm_sim, shared_secret_sim, CURVE25519_KEY_LEN);
    ikm_len += CURVE25519_KEY_LEN;
    
    memcpy(ikm_sim + ikm_len, handshake_data, handshake_len);
    ikm_len += handshake_len;
    
    const char* salt_str = "Tor:ntor_v1/KDF";
    const char* info_str = "Tor:ntor_v1/KEY_EXPAND";

    u8 prk[SHA256_DIGEST_LENGTH]; 
    hkdf_extract((const u8*)salt_str, strlen(salt_str), ikm_sim, ikm_len, prk);

    u8 okm[HKDF_OUTPUT_LEN];
    hkdf_expand(prk, SHA256_DIGEST_LENGTH, (const u8*)info_str, strlen(info_str), okm, sizeof(okm));

    // OKM Aufteilung
    size_t offset = 0;
    memcpy(hop->hmac_fwd, okm + offset, SHA1_DIGEST_LEN); offset += SHA1_DIGEST_LEN;
    memcpy(hop->hmac_back, okm + offset, SHA1_DIGEST_LEN); offset += SHA1_DIGEST_LEN;
    memcpy(hop->key_fwd, okm + offset, AES_KEY_LEN); offset += AES_KEY_LEN;
    memcpy(hop->key_back, okm + offset, AES_KEY_LEN); offset += AES_KEY_LEN;
    memcpy(hop->iv_fwd, okm + offset, IV_LEN); offset += IV_LEN;
    memcpy(hop->iv_back, okm + offset, IV_LEN); offset += IV_LEN;

    crypto_aes_init(hop);
    utl_logf("CRYPTO", "INFO", "NTOR KDF abgeschlossen. %zu Bytes Schlüsselmaterial.", offset);
}

// =================================================================
// TLS/SSL IMPLEMENTIERUNG (SSL)
// =================================================================

static int tls_connect_and_cert_verify(Circuit_t* circ, const char* host) {
    circ->ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!circ->ssl_ctx) return 0;
    
    SSL_CTX_set_verify(circ->ssl_ctx, SSL_VERIFY_PEER, NULL);
    // Realistisch: SSL_CTX_load_verify_locations(circ->ssl_ctx, "tor_root_certs.pem", NULL);
    
    circ->ssl_handle = SSL_new(circ->ssl_ctx);
    if (!circ->ssl_handle) return 0;
    
    SSL_set_fd(circ->ssl_handle, circ->or_fd);

    if (SSL_connect(circ->ssl_handle) <= 0) {
        utl_logf("SSL", "FATAL", "SSL_connect fehlgeschlagen: %s", ERR_error_string(ERR_get_error(), NULL));
        return 0;
    }
    
    // Zertifikatsprüfung (vereinfacht)
    long verify_result = SSL_get_verify_result(circ->ssl_handle);
    if (verify_result != X509_V_OK) {
        utl_logf("SSL", "WARN", "Zertifikatsprüfung fehlgeschlagen: %s", X509_verify_cert_error_string(verify_result));
    } else {
        utl_logf("SSL", "SUCCESS", "TLS-Verbindung etabliert und Zertifikat simuliert akzeptiert.");
    }
    
    return 1;
}

// =================================================================
// TOR-PROTOKOLL: ZELLEN & RELAYS (TOR)
// =================================================================

static int cell_send_raw(SSL *ssl, const u8* cell_buf) {
    return ssl_write_fixed(ssl, cell_buf, CELL_SIZE) == CELL_SIZE;
}

static int cell_receive_raw(SSL *ssl, u8* cell_buf) {
    return ssl_read_fixed(ssl, cell_buf, CELL_SIZE) == CELL_SIZE;
}

static void cell_mk(u8* buf, u32 circ_id, cell_command cmd, relay_command rly_cmd, 
                    const u8* payload, size_t payload_len, int hop_index) {
    
    memset(buf, 0, CELL_SIZE); 
    utl_u32_to_be(buf, circ_id); 
    buf[4] = (u8)cmd;           

    u8* cell_payload = buf + CELL_HEADER_LEN;
    
    if (cmd == CMD_RELAY) {
        
        utl_u16_to_be(cell_payload + 0, (u16)rly_cmd);
        utl_u16_to_be(cell_payload + 2, 0); 
        utl_u16_to_be(cell_payload + 4, 0); 
        utl_u16_to_be(cell_payload + 6, (u16)payload_len); 
        
        memcpy(cell_payload + RELAY_HDR_LEN, payload, payload_len);
        
        Hop_t* target_hop = &G_circuit.path[hop_index]; 
        u8 hmac_digest[SHA1_DIGEST_LEN];
        
        crypto_sha1_hmac(target_hop->hmac_fwd, AES_KEY_LEN, cell_payload, RELAY_HDR_LEN - 4 + payload_len, hmac_digest);
        memcpy(cell_payload + 8, hmac_digest, 4); 

        // Verschlüsselung (Layered Encryption)
        for (int i = G_circuit.num_hops - 1; i >= 0; i--) {
             Hop_t* hop = &G_circuit.path[i];
             crypto_aes_crypt(hop->ctx_fwd, hop->key_fwd, hop->iv_fwd, cell_payload, CELL_SIZE - CELL_HEADER_LEN);
             // ZÄHLER-UPDATE: Nach jeder Zelle, die verschlüsselt wurde
             aes_ctr_increment(hop->iv_fwd); 
        }

    } else if (cmd == CMD_CREATE2) {
        memcpy(cell_payload, payload, payload_len);
    } 
}

static int cell_decrypt_and_unwrap(u8* cell_buf, Circuit_t* circ) {
    u8* cell_payload = cell_buf + CELL_HEADER_LEN;
    
    // Entschlüsselung (Layered Decryption)
    for (int i = 0; i < circ->num_hops; i++) {
        Hop_t* hop = &circ->path[i];
        crypto_aes_crypt(hop->ctx_back, hop->key_back, hop->iv_back, cell_payload, CELL_SIZE - CELL_HEADER_LEN);
        // ZÄHLER-UPDATE: Nach jeder Zelle, die entschlüsselt wurde
        aes_ctr_increment(hop->iv_back);
    }
    
    cell_command cmd = (cell_command)cell_buf[4];
    
    if (cmd == CMD_RELAY) {
        u8 hmac_received[4];
        memcpy(hmac_received, cell_payload + 8, 4);
        memset(cell_payload + 8, 0, 4); 

        u16 rly_cmd = (u16)((cell_payload[0] << 8) | cell_payload[1]);
        u16 rly_len = (u16)((cell_payload[6] << 8) | cell_payload[7]);

        Hop_t* innermost_hop = &circ->path[circ->num_hops - 1];
        u8 hmac_digest[SHA1_DIGEST_LEN];
        
        crypto_sha1_hmac(innermost_hop->hmac_back, AES_KEY_LEN, cell_payload, RELAY_HDR_LEN - 4 + rly_len, hmac_digest);

        if (memcmp(hmac_received, hmac_digest, 4) != 0) {
            utl_logf("TOR", "ERROR", "HMAC-Prüfung fehlgeschlagen. Zelle verworfen.");
            return 0;
        }

        memcpy(cell_payload + 8, hmac_received, 4);
        return 1;
    } else if (cmd == CMD_CREATED2) {
        return 1;
    }
    
    return 0;
}

static int tor_initial_handshake(SSL *ssl) {
    u8 versions_cell[CELL_SIZE];
    memset(versions_cell, 0, CELL_SIZE);
    utl_u32_to_be(versions_cell, 0); 
    versions_cell[4] = 7; 
    utl_u16_to_be(versions_cell + 5, 3); 
    
    if (cell_send_raw(ssl, versions_cell) == 0) return 0;
    
    u8 resp_cell[CELL_SIZE];
    if (cell_receive_raw(ssl, resp_cell) != CELL_SIZE) return 0;
    if (resp_cell[4] != 7) return 0; 

    G_circuit.circ_id = 0xAA01BB02;
    return 1;
}

static int tor_build_hop(Circuit_t* circ, int index, const char* host, int port, cell_command cmd) {
    Hop_t* current_hop = &circ->path[index];
    strncpy(current_hop->host, host, sizeof(current_hop->host) - 1);
    current_hop->host[sizeof(current_hop->host) - 1] = '\0';
    current_hop->port = port;
    
    u8 ntor_handshake_payload[200]; 
    u8 cell_buf[CELL_SIZE]; 
    u8 client_ephemeral_pubkey[CURVE25519_KEY_LEN];
    u8 client_ephemeral_privkey[CURVE25519_KEY_LEN];

    RAND_bytes(client_ephemeral_privkey, CURVE25519_KEY_LEN); 
    RAND_bytes(client_ephemeral_pubkey, CURVE25519_KEY_LEN);
    
    memset(current_hop->ntor_public_key, 0x3C, CURVE25519_KEY_LEN); 
    
    memcpy(ntor_handshake_payload + 0, client_ephemeral_pubkey, CURVE25519_KEY_LEN);
    memset(ntor_handshake_payload + 32, 0x1A, SHA1_DIGEST_LEN);
    memcpy(ntor_handshake_payload + 32 + SHA1_DIGEST_LEN, current_hop->ntor_public_key, CURVE25519_KEY_LEN);
    
    u8* payload = ntor_handshake_payload;
    size_t payload_len = CURVE25519_KEY_LEN + SHA1_DIGEST_LEN + CURVE25519_KEY_LEN; 

    if (cmd == CMD_CREATE2) {
        cell_mk(cell_buf, circ->circ_id, CMD_CREATE2, 0, payload, payload_len, index);
        if (!cell_send_raw(circ->ssl_handle, cell_buf)) return 0;
        utl_logf("TOR", "INFO", "CREATE2 Zelle gesendet (Guard OR).");
    } else if (cmd == CMD_RELAY) {
        u8 extend_payload[200];
        extend_payload[0] = 4;
        utl_u32_to_be(extend_payload + 1, inet_addr(host));
        utl_u16_to_be(extend_payload + 5, (u16)port);
        memset(extend_payload + 7, 0xAE, SHA1_DIGEST_LEN);
        memset(extend_payload + 7 + SHA1_DIGEST_LEN, 0xBF, CURVE25519_KEY_LEN);
        memcpy(extend_payload + 7 + SHA1_DIGEST_LEN + CURVE25519_KEY_LEN, payload, payload_len);
        
        cell_mk(cell_buf, circ->circ_id, CMD_RELAY, RLY_CMD_EXTEND2, extend_payload, sizeof(extend_payload), index - 1);
        if (!cell_send_raw(circ->ssl_handle, cell_buf)) return 0;
        utl_logf("TOR", "INFO", "RELAY EXTEND2 Zelle gesendet.");
    }

    u8 resp_cell[CELL_SIZE];
    if (cell_receive_raw(circ->ssl_handle, resp_cell) != CELL_SIZE) return 0;
    
    if (!cell_decrypt_and_unwrap(resp_cell, circ)) return 0; 

    crypto_kdf_ntor(current_hop, ntor_handshake_payload, payload_len);
    
    circ->num_hops = index + 1;
    utl_logf("TOR", "SUCCESS", "Hop %d (%s) erfolgreich eingerichtet.", circ->num_hops, host);
    
    return 1;
}

static int tor_build_circuit(Circuit_t* circ) {
    if (!tor_build_hop(circ, 0, OR_GUARD_HOST, OR_PORT, CMD_CREATE2)) return 0;
    if (!tor_build_hop(circ, 1, OR_MIDDLE_HOST, OR_PORT, CMD_RELAY)) return 0;
    if (!tor_build_hop(circ, 2, OR_EXIT_HOST, OR_PORT, CMD_RELAY)) return 0;
    return 1;
}

static void tor_destroy(Circuit_t* circ) {
    if (circ->ssl_handle) {
        u8 destroy_cell[CELL_SIZE];
        memset(destroy_cell, 0, CELL_SIZE);
        utl_u32_to_be(destroy_cell, circ->circ_id);
        destroy_cell[4] = CMD_DESTROY;
        destroy_cell[5] = 1; 
        cell_send_raw(circ->ssl_handle, destroy_cell);
        SSL_shutdown(circ->ssl_handle);
    }
    if (circ->ssl_handle) SSL_free(circ->ssl_handle);
    if (circ->ssl_ctx) SSL_CTX_free(circ->ssl_ctx);
    if (circ->or_fd > 0) close(circ->or_fd);

    for(int i = 0; i < circ->num_hops; i++) {
        if (circ->path[i].ctx_fwd) EVP_CIPHER_CTX_free(circ->path[i].ctx_fwd);
        if (circ->path[i].ctx_back) EVP_CIPHER_CTX_free(circ->path[i].ctx_back);
    }
}

static int tor_send_relay_cell(Circuit_t* circ, relay_command rly_cmd, const u8* payload, size_t len) {
    if (circ->num_hops == 0) return 0;
    u8 cell_buf[CELL_SIZE];
    cell_mk(cell_buf, circ->circ_id, CMD_RELAY, rly_cmd, payload, len, circ->num_hops - 1);
    return cell_send_raw(circ->ssl_handle, cell_buf);
}

// =================================================================
// SOCKS5 SERVER (SOCKS)
// =================================================================

static int net_receive_fixed(int fd, u8* buf, size_t len) {
    size_t total_received = 0;
    while (total_received < len) {
        ssize_t received = recv(fd, (char*)buf + total_received, len - total_received, 0);
        if (received <= 0) return received; 
        total_received += received;
    }
    return total_received;
}

static ssize_t net_send(int fd, const u8* buf, size_t len) {
    return send(fd, (const char*)buf, len, 0);
}

static int socks_handshake(int client_fd) {
    u8 request[512], response[10];
    // Phase 1: Authentication
    if (net_receive_fixed(client_fd, request, 2) != 2) return 0;
    if (request[0] != 0x05) return 0; 
    if (net_receive_fixed(client_fd, request + 2, request[1]) != request[1]) return 0; 
    response[0] = 0x05; response[1] = 0x00; // SOCKS5, NO AUTH
    if (net_send(client_fd, response, 2) != 2) return 0;
    
    // Phase 2: Request
    if (net_receive_fixed(client_fd, request, 4) != 4) return 0;
    if (request[1] != 0x01) { // Nur CONNECT (0x01)
        response[1] = 0x07; net_send(client_fd, response, 2); return 0;
    }
    
    u8 atype = request[3];
    char host_str[SOCKS5_MAX_ADDR_LEN];
    u16 port;
    u8 addr_buf[SOCKS5_MAX_ADDR_LEN + 2];
    size_t addr_len = 0;
    
    if (atype == 0x01) { // IPv4
        addr_len = 4;
    } else if (atype == 0x03) { // Domain Name
        if (net_receive_fixed(client_fd, addr_buf, 1) != 1) return 0;
        addr_len = addr_buf[0];
        if (net_receive_fixed(client_fd, addr_buf + 1, addr_len) != addr_len) return 0;
        addr_len++;
    } else { return 0; }
    
    if (net_receive_fixed(client_fd, addr_buf + addr_len, 2) != 2) return 0;

    if (atype == 0x03) {
        memcpy(host_str, addr_buf + 1, addr_len - 1);
        host_str[addr_len - 1] = '\0';
        port = (u16)((addr_buf[addr_len] << 8) | addr_buf[addr_len + 1]);
    } else { 
        inet_ntop(AF_INET, addr_buf, host_str, sizeof(host_str));
        port = (u16)((addr_buf[4] << 8) | addr_buf[5]);
    }

    utl_logf("SOCKS", "INFO", "SOCKS5-Anfrage: %s:%u", host_str, port);

    // Tor Relay BEGIN Cell senden
    u8 begin_payload[SOCKS5_MAX_ADDR_LEN + 10];
    size_t begin_len = snprintf((char*)begin_payload, sizeof(begin_payload), "%s:%u", host_str, port);
    
    if (tor_send_relay_cell(&G_circuit, RLY_CMD_BEGIN, begin_payload, begin_len + 1) == 0) return 0;

    // Empfange Relay CONNECTED (Tor Antwort)
    u8 resp_cell[CELL_SIZE];
    if (cell_receive_raw(G_circuit.ssl_handle, resp_cell) != CELL_SIZE) return 0;
    if (!cell_decrypt_and_unwrap(resp_cell, &G_circuit)) return 0;

    // SOCKS5 Success Response an Client senden
    memset(response, 0, 10);
    response[0] = 0x05; 
    response[1] = 0x00; // Success
    response[3] = 0x01; // ATYP: IPv4
    if (net_send(client_fd, response, 10) != 10) return 0;
    utl_logf("SOCKS", "SUCCESS", "SOCKS5-Tunnel zu %s:%u offen.", host_str, port);

    return 1;
}

static void socks_data_loop(int client_fd) {
    u8 client_buf[RELAY_PAYLOAD_LEN];
    u8 tor_resp_cell[CELL_SIZE];
    u8 rly_payload[CELL_SIZE - CELL_HEADER_LEN];

    // Setze SOCKS Client Socket auf Non-Blocking für Select/Poll
    #ifndef _WIN32
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    #endif

    while (running) {
        fd_set read_fds;
        struct timeval timeout;
        
        FD_ZERO(&read_fds);
        FD_SET(client_fd, &read_fds);
        
        // Tor-Socket ist implizit Teil der SSL-Session und wird über SSL_read/write gehandhabt.
        // Das Polling hier dient nur dem SOCKS-Client.

        timeout.tv_sec = 1; 
        timeout.tv_usec = 0;

        // 1. Warte auf Daten vom SOCKS-Client
        if (select(client_fd + 1, &read_fds, NULL, NULL, &timeout) > 0) {
            if (FD_ISSET(client_fd, &read_fds)) {
                ssize_t received = recv(client_fd, (char*)client_buf, sizeof(client_buf), 0);
                
                if (received > 0) {
                    utl_logf("SOCKS", "DATA", "Sende %zu Bytes vom Client in Relay DATA Zelle.", received);
                    if (!tor_send_relay_cell(&G_circuit, RLY_CMD_DATA, client_buf, received)) break;
                } else {
                    utl_logf("SOCKS", "INFO", "SOCKS-Client getrennt oder Fehler (%zd).", received);
                    break;
                }
            }
        }
        
        // 2. Prüfe auf Daten vom Tor-Netzwerk (non-blocking SSL_read ist schwierig, hier wird blockiert versucht)
        int ssl_ret = SSL_read(G_circuit.ssl_handle, (char*)tor_resp_cell, CELL_SIZE);
        if (ssl_ret == CELL_SIZE) {
            
            if (cell_decrypt_and_unwrap(tor_resp_cell, &G_circuit)) {
                memcpy(rly_payload, tor_resp_cell + CELL_HEADER_LEN, CELL_SIZE - CELL_HEADER_LEN);
                u16 rly_cmd = (u16)((rly_payload[0] << 8) | rly_payload[1]);
                u16 rly_len = (u16)((rly_payload[6] << 8) | rly_payload[7]);
                
                if (rly_cmd == RLY_CMD_DATA) {
                    utl_logf("SOCKS", "DATA", "Empfange %u Bytes als Relay DATA von Tor. Sende zum Client.", rly_len);
                    if (net_send(client_fd, rly_payload + RELAY_HDR_LEN, rly_len) <= 0) break;
                } else if (rly_cmd == RLY_CMD_END) {
                    utl_logf("SOCKS", "INFO", "Relay END empfangen. Stream geschlossen.");
                    break;
                }
            }
        } else if (ssl_ret > 0) {
            // Unvollständige Zelle (sollte bei fixed read nicht passieren)
            utl_logf("SOCKS", "WARN", "Unvollständige Zelle empfangen: %d Bytes", ssl_ret);
        } else if (ssl_ret < 0) {
            // Nur kritische Fehler führen zum Abbruch
            int err = SSL_get_error(G_circuit.ssl_handle, ssl_ret);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                 utl_logf("SOCKS", "ERROR", "SSL-Fehler im Data Loop: %d", err);
                 break;
            }
        }
    }
    
    // Cleanup: Schließe den Stream am Exit-Relay (Relay END)
    tor_send_relay_cell(&G_circuit, RLY_CMD_END, NULL, 0);
    close(client_fd);
}

static int socks_server_start(int port) {
    int listen_fd;
    struct sockaddr_in server_addr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) return -1;

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    server_addr.sin_port = htons(port);
    
    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(listen_fd);
        utl_logf("SOCKS", "FATAL", "Bind fehlgeschlagen (127.0.0.1:%d). Port belegt?", port);
        return -1;
    }

    if (listen(listen_fd, 5) < 0) {
        close(listen_fd);
        return -1;
    }

    utl_logf("SOCKS", "SUCCESS", "SOCKS5-Server gestartet auf 127.0.0.1:%d.", port);
    return listen_fd;
}

static void socks_handle_client(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        utl_logf("SOCKS", "ERROR", "Accept fehlgeschlagen.");
        return;
    }
    
    utl_logf("SOCKS", "INFO", "SOCKS-Client verbunden.");

    if (socks_handshake(client_fd)) {
        socks_data_loop(client_fd);
    } else {
        close(client_fd);
    }
}

// =================================================================
// HAUPTPROGRAMM
// =================================================================

void signal_handler(int signum) {
    running = 0;
    utl_logf("MAIN", "SIGNAL", "Shutdown-Signal erhalten. Beende...");
}

int main(void) {
    
    #ifdef _WIN32
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) return 1;
    #endif

    // Initialisiere OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    signal(SIGINT, signal_handler);
    
    utl_logf("MAIN", "INFO", "Starte Tor-Simulation mit OpenSSL-Kryptografie.");

    // 1. Initialisierung und TLS/Tor-Handshake mit Guard
    G_circuit.or_fd = net_connect(OR_GUARD_HOST, OR_PORT);
    if (G_circuit.or_fd < 0) goto cleanup;

    if (!tls_connect_and_cert_verify(&G_circuit, OR_GUARD_HOST)) goto cleanup;
    
    if (!tor_initial_handshake(G_circuit.ssl_handle)) goto cleanup;

    // 2. Circuit-Aufbau
    utl_logf("MAIN", "INFO", "Starte 3-Hop Circuit-Aufbau...");
    if (!tor_build_circuit(&G_circuit)) goto cleanup;
    
    // 3. SOCKS5-Server starten und Hauptschleife
    int socks_listen_fd = socks_server_start(SOCKS_PORT);
    if (socks_listen_fd < 0) goto cleanup;
    
    while (running) {
        fd_set read_fds;
        struct timeval timeout;
        
        FD_ZERO(&read_fds);
        FD_SET(socks_listen_fd, &read_fds);
        
        timeout.tv_sec = 1; 
        timeout.tv_usec = 0;
        
        if (select(socks_listen_fd + 1, &read_fds, NULL, NULL, &timeout) > 0) {
            if (FD_ISSET(socks_listen_fd, &read_fds)) {
                socks_handle_client(socks_listen_fd);
            }
        }
    }
    
    if (socks_listen_fd > 0) close(socks_listen_fd);

cleanup:
    tor_destroy(&G_circuit);
    
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    utl_logf("MAIN", "SUCCESS", "Programm beendet.");
    return 0;
}
