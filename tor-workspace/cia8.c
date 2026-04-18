#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include <fcntl.h> 

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
    #include <poll.h> // Poll für robustes I/O
#endif

// OpenSSL-Includes
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

// =================================================================
// KONSTANTEN UND STRUKTUREN
// =================================================================

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define SOCKS_PORT 9050
#define CELL_SIZE 512
#define CELL_HEADER_LEN 5
#define RELAY_HDR_LEN 11
#define RELAY_PAYLOAD_LEN (CELL_SIZE - CELL_HEADER_LEN - RELAY_HDR_LEN)
#define SOCKS5_MAX_ADDR_LEN 256
#define IO_TIMEOUT_SEC 10

// Server Simulation (FÜR ECHTEN BETRIEB ERSETZEN!)
#define OR_GUARD_HOST "192.0.2.1"     
#define OR_MIDDLE_HOST "198.51.100.2" 
#define OR_EXIT_HOST "203.0.113.3"    
#define OR_PORT 9001

#define SHA1_DIGEST_LEN 20
#define AES_KEY_LEN 16        
#define IV_LEN 16             
#define CURVE25519_KEY_LEN 32 
#define SHA256_DIGEST_LENGTH 32
#define HKDF_OUTPUT_LEN (2 * SHA1_DIGEST_LEN + 2 * AES_KEY_LEN + 2 * IV_LEN) 
#define MAX_HOPS 3

typedef enum {CMD_CREATE2=1, CMD_CREATED2=2, CMD_RELAY=3, CMD_DESTROY=4} cell_command;
typedef enum {RLY_CMD_EXTEND2=6, RLY_CMD_EXTENDED2=7, RLY_CMD_BEGIN=1, RLY_CMD_DATA=2, RLY_CMD_END=3, RLY_CMD_CONNECTED=4} relay_command;

typedef struct {
    char host[64];
    int port;
    u8 key_fwd[AES_KEY_LEN]; 
    u8 key_back[AES_KEY_LEN]; 
    u8 iv_fwd[IV_LEN]; 
    u8 iv_back[IV_LEN]; 
    u8 hmac_fwd[SHA1_DIGEST_LEN]; 
    u8 hmac_back[SHA1_DIGEST_LEN]; 
    u8 ntor_public_key[CURVE25519_KEY_LEN];
    EVP_CIPHER_CTX *ctx_fwd;
    EVP_CIPHER_CTX *ctx_back;
} Hop_t;

typedef struct {
    u32 circ_id;
    int num_hops;
    int or_fd; // Tor OR Socket FD
    SSL_CTX *ssl_ctx; 
    SSL *ssl_handle; 
    Hop_t path[MAX_HOPS];
} Circuit_t;

Circuit_t G_circuit = {0};
static volatile int running = 1;

// =================================================================
// UTILITY FUNKTIONEN
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

static void aes_ctr_increment(u8* ctr) {
    for (int i = IV_LEN - 1; i >= 0; i--) {
        if (++ctr[i] != 0) break;
    }
}

static int net_set_nonblocking(int fd) {
    #ifndef _WIN32
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) return 0;
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return 0;
    #endif
    return 1;
}

// =================================================================
// NETZWERK & KRYPTO (Teilweise weggelassen, da unverändert)
// =================================================================

static int net_connect(const char* host, int port) {
    int sockfd = -1;
    struct sockaddr_in server_addr;
    struct hostent *server;

    #ifndef _WIN32
    server = gethostbyname(host);
    if (server == NULL) return -1;
    #endif

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;
    
    // Wir setzen keine Timeouts auf Sockets, da wir select/poll verwenden

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
    net_set_nonblocking(sockfd); // Wichtig für die Hauptschleife
    return sockfd;
}

static ssize_t ssl_read_fixed(SSL *ssl, u8 *buf, size_t len) {
    size_t total_received = 0;
    int ret;
    while (total_received < len) {
        ret = SSL_read(ssl, buf + total_received, len - total_received);
        if (ret <= 0) return ret < 0 ? ret : -1;
        total_received += ret;
    }
    return total_received;
}

static ssize_t ssl_write_fixed(SSL *ssl, const u8 *buf, size_t len) {
    size_t total_sent = 0;
    int ret;
    while (total_sent < len) {
        ret = SSL_write(ssl, buf + total_sent, len - total_sent);
        if (ret <= 0) return ret < 0 ? ret : -1;
        total_sent += ret;
    }
    return total_sent;
}

// ... [crypto_aes_crypt, crypto_sha1_hmac, hkdf, crypto_kdf_ntor bleiben gleich] ...

static int crypto_aes_crypt(EVP_CIPHER_CTX *ctx, const u8* key, u8* iv, u8* data, int len) {
    int outlen;
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv)) return 0;
    if (!EVP_EncryptUpdate(ctx, data, &outlen, data, len)) return 0;
    return 1;
}

static void crypto_sha1_hmac(const u8* key, size_t key_len, const u8* data, size_t len, u8* digest_out) {
    HMAC(EVP_sha1(), key, key_len, data, len, digest_out, NULL);
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
             aes_ctr_increment(hop->iv_fwd); // Wichtig: Zähler inkrementieren
        }
    } 
}

static int cell_decrypt_and_unwrap(u8* cell_buf, Circuit_t* circ) {
    u8* cell_payload = cell_buf + CELL_HEADER_LEN;
    
    // Entschlüsselung (Layered Decryption)
    for (int i = 0; i < circ->num_hops; i++) {
        Hop_t* hop = &circ->path[i];
        crypto_aes_crypt(hop->ctx_back, hop->key_back, hop->iv_back, cell_payload, CELL_SIZE - CELL_HEADER_LEN);
        aes_ctr_increment(hop->iv_back); // Wichtig: Zähler inkrementieren
    }
    
    cell_command cmd = (cell_command)cell_buf[4];
    
    if (cmd == CMD_RELAY) {
        u8 hmac_received[4];
        memcpy(hmac_received, cell_payload + 8, 4);
        memset(cell_payload + 8, 0, 4); 

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

// ... [tor_initial_handshake, tor_build_hop, tor_build_circuit bleiben gleich] ...
static int tor_send_relay_cell(Circuit_t* circ, relay_command rly_cmd, const u8* payload, size_t len) {
    if (circ->num_hops == 0) return 0;
    u8 cell_buf[CELL_SIZE];
    cell_mk(cell_buf, circ->circ_id, CMD_RELAY, rly_cmd, payload, len, circ->num_hops - 1);
    return ssl_write_fixed(circ->ssl_handle, cell_buf, CELL_SIZE) == CELL_SIZE;
}


// =================================================================
// SOCKS5 SERVER & I/O-MULTIPLEXING
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
    if (net_receive_fixed(client_fd, request, 2) != 2 || request[0] != 0x05) return 0;
    if (net_receive_fixed(client_fd, request + 2, request[1]) != request[1]) return 0; 
    response[0] = 0x05; response[1] = 0x00; 
    if (net_send(client_fd, response, 2) != 2) return 0;
    
    if (net_receive_fixed(client_fd, request, 4) != 4 || request[1] != 0x01) return 0;
    
    u8 atype = request[3];
    char host_str[SOCKS5_MAX_ADDR_LEN];
    u16 port;
    u8 addr_buf[SOCKS5_MAX_ADDR_LEN + 2];
    size_t addr_len = 0;
    
    if (atype == 0x03) { 
        if (net_receive_fixed(client_fd, addr_buf, 1) != 1) return 0;
        addr_len = addr_buf[0];
        if (net_receive_fixed(client_fd, addr_buf + 1, addr_len) != addr_len) return 0;
        addr_len++;
    } else { return 0; } // Nur Domain Name (0x03) wird unterstützt
    
    if (net_receive_fixed(client_fd, addr_buf + addr_len, 2) != 2) return 0;

    memcpy(host_str, addr_buf + 1, addr_len - 1);
    host_str[addr_len - 1] = '\0';
    port = (u16)((addr_buf[addr_len] << 8) | addr_buf[addr_len + 1]);

    utl_logf("SOCKS", "INFO", "SOCKS5-Anfrage: %s:%u", host_str, port);

    // Tor Relay BEGIN Cell senden
    u8 begin_payload[SOCKS5_MAX_ADDR_LEN + 10];
    size_t begin_len = snprintf((char*)begin_payload, sizeof(begin_payload), "%s:%u", host_str, port);
    
    if (!tor_send_relay_cell(&G_circuit, RLY_CMD_BEGIN, begin_payload, begin_len + 1)) return 0;

    // Empfange Relay CONNECTED (Tor Antwort)
    u8 resp_cell[CELL_SIZE];
    if (ssl_read_fixed(G_circuit.ssl_handle, resp_cell, CELL_SIZE) != CELL_SIZE) return 0;
    if (!cell_decrypt_and_unwrap(resp_cell, &G_circuit)) return 0;
    
    u8* rly_payload = resp_cell + CELL_HEADER_LEN;
    u16 rly_cmd = (u16)((rly_payload[0] << 8) | rly_payload[1]);
    if (rly_cmd != RLY_CMD_CONNECTED) return 0;

    // SOCKS5 Success Response an Client senden
    memset(response, 0, 10);
    response[0] = 0x05; 
    response[1] = 0x00; // Success
    response[3] = 0x01; // ATYP: IPv4 (Dummy)
    if (net_send(client_fd, response, 10) != 10) return 0;
    utl_logf("SOCKS", "SUCCESS", "SOCKS5-Tunnel zu %s:%u offen.", host_str, port);

    return 1;
}

/**
 * @brief Hauptschleife zur Weiterleitung des Datenverkehrs zwischen SOCKS-Client und Tor-Netzwerk.
 */
static void socks_data_loop(int client_fd) {
    int tor_fd = G_circuit.or_fd; 
    net_set_nonblocking(client_fd);

    u8 tor_recv_buf[CELL_SIZE]; 
    size_t tor_recv_offset = 0; 
    int connection_active = 1;

    utl_logf("SOCKS", "DATA", "Starte Datenweiterleitung. Drücken Sie Ctrl+C, um den Server zu beenden.");

    while (running && connection_active) {
        struct pollfd fds[2];
        fds[0].fd = client_fd;
        fds[0].events = POLLIN;
        fds[1].fd = tor_fd;
        fds[1].events = POLLIN;

        int ret = poll(fds, 2, 1000); // 1000ms Timeout

        if (ret < 0) {
            if (errno == EINTR) continue;
            utl_logf("SOCKS", "ERROR", "Poll-Fehler.");
            break;
        }
        
        if (ret == 0) continue; 

        // 1. Daten vom Tor-Netzwerk empfangen (Tor -> Client)
        if (fds[1].revents & POLLIN) {
            ssize_t ssl_ret = SSL_read(G_circuit.ssl_handle, (char*)tor_recv_buf + tor_recv_offset, CELL_SIZE - tor_recv_offset);

            if (ssl_ret > 0) {
                tor_recv_offset += ssl_ret;
            } else if (ssl_ret == 0) {
                utl_logf("TOR", "INFO", "Tor-Verbindung geschlossen (EOF).");
                connection_active = 0; break;
            } else {
                int err = SSL_get_error(G_circuit.ssl_handle, ssl_ret);
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    utl_logf("TOR", "ERROR", "Kritischer SSL_read Fehler: %d", err);
                    connection_active = 0; break;
                }
            }
            
            // Verarbeite volle Zellen
            while (tor_recv_offset >= CELL_SIZE) {
                if (cell_decrypt_and_unwrap(tor_recv_buf, &G_circuit)) {
                    u8* rly_payload = tor_recv_buf + CELL_HEADER_LEN;
                    u16 rly_cmd = (u16)((rly_payload[0] << 8) | rly_payload[1]);
                    u16 rly_len = (u16)((rly_payload[6] << 8) | rly_payload[7]);

                    if (rly_cmd == RLY_CMD_DATA) {
                        if (net_send(client_fd, rly_payload + RELAY_HDR_LEN, rly_len) <= 0) {
                            connection_active = 0; break;
                        }
                    } else if (rly_cmd == RLY_CMD_END) {
                        utl_logf("SOCKS", "INFO", "Relay END empfangen. Stream geschlossen.");
                        connection_active = 0; break;
                    }
                } else {
                    utl_logf("SOCKS", "ERROR", "Fehler beim Entschlüsseln/HMAC-Prüfung.");
                    connection_active = 0; break;
                }
                
                memmove(tor_recv_buf, tor_recv_buf + CELL_SIZE, tor_recv_offset - CELL_SIZE);
                tor_recv_offset -= CELL_SIZE;
            }
        }
        
        // 2. Daten vom SOCKS-Client empfangen (Client -> Tor)
        if (fds[0].revents & POLLIN) {
            u8 client_buf[RELAY_PAYLOAD_LEN];
            ssize_t received = recv(client_fd, (char*)client_buf, sizeof(client_buf), 0);
            
            if (received > 0) {
                if (!tor_send_relay_cell(&G_circuit, RLY_CMD_DATA, client_buf, received)) {
                    utl_logf("TOR", "ERROR", "Fehler beim Senden der Relay-Zelle.");
                    connection_active = 0; break;
                }
            } else if (received == 0) {
                utl_logf("SOCKS", "INFO", "SOCKS-Client geschlossen (EOF).");
                connection_active = 0; break; 
            } else { 
                if (errno != EWOULDBLOCK && errno != EAGAIN) {
                    utl_logf("SOCKS", "ERROR", "SOCKS-Client Lesefehler.");
                    connection_active = 0; break;
                }
            }
        }
    }
    
    // Cleanup: Sende abschließende Relay END Zelle und schließe Socket
    if (running) {
        tor_send_relay_cell(&G_circuit, RLY_CMD_END, NULL, 0);
    }
    close(client_fd);
    utl_logf("SOCKS", "INFO", "SOCKS-Stream-Verbindung beendet.");
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
        close(listen_fd); return -1;
    }
    if (listen(listen_fd, 5) < 0) {
        close(listen_fd); return -1;
    }
    net_set_nonblocking(listen_fd);
    utl_logf("SOCKS", "SUCCESS", "SOCKS5-Server gestartet auf 127.0.0.1:%d.", port);
    return listen_fd;
}

static void socks_handle_client(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            utl_logf("SOCKS", "ERROR", "Accept fehlgeschlagen.");
        }
        return;
    }
    
    utl_logf("SOCKS", "INFO", "SOCKS-Client verbunden.");

    if (socks_handshake(client_fd)) {
        socks_data_loop(client_fd);
    } else {
        close(client_fd);
        utl_logf("SOCKS", "ERROR", "SOCKS-Handshake fehlgeschlagen.");
    }
}

// =================================================================
// HAUPTPROGRAMM
// =================================================================

void signal_handler(int signum) {
    running = 0;
    utl_logf("MAIN", "SIGNAL", "Shutdown-Signal (Ctrl+C) erhalten. Beende...");
}

int main(void) {
    
    // ... [Windows/OpenSSL Initialisierung] ...
    
    #ifdef _WIN32
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) return 1;
    #endif

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    signal(SIGINT, signal_handler);
    
    utl_logf("MAIN", "INFO", "Starte Tor-Simulation mit OpenSSL-Kryptografie.");
    
    // 1. Initialisierung und Circuit-Aufbau
    G_circuit.or_fd = net_connect(OR_GUARD_HOST, OR_PORT);
    if (G_circuit.or_fd < 0) goto cleanup;

    // Führen Sie hier tor_initial_handshake und tor_build_circuit aus (Code aus Platzgründen weggelassen,
    // aber die Logik ist in der vorherigen Antwort enthalten und sollte hier aufgerufen werden).
    // ... tls_connect_and_cert_verify(&G_circuit, OR_GUARD_HOST); ...
    // ... tor_initial_handshake(G_circuit.ssl_handle); ...
    // ... tor_build_circuit(&G_circuit); ...

    // SIMULIERE ERFOLGREICHEN AUFBAU FÜR LAUFFÄHIGKEIT
    utl_logf("MAIN", "WARN", "TLS-Handshake und Circuit-Aufbau werden simuliert übersprungen/angenommen.");
    G_circuit.ssl_handle = SSL_new(SSL_CTX_new(TLS_client_method())); // Dummy SSL Handle
    SSL_set_fd(G_circuit.ssl_handle, G_circuit.or_fd);
    G_circuit.circ_id = 0xAA01BB02;
    G_circuit.num_hops = 3; 
    // Hier müsste eine echte crypto_kdf_ntor für alle 3 Hops aufgerufen werden, um die Keys zu setzen.
    // Da dies ein Simulator ist, wird angenommen, dass die Keys gesetzt wurden.

    // 2. SOCKS5-Server starten
    int socks_listen_fd = socks_server_start(SOCKS_PORT);
    if (socks_listen_fd < 0) goto cleanup;
    
    // 3. Hauptschleife (akzeptiert neue Verbindungen)
    while (running) {
        struct pollfd fds[1];
        fds[0].fd = socks_listen_fd;
        fds[0].events = POLLIN;

        if (poll(fds, 1, 1000) > 0) { // 1000ms Timeout
            if (fds[0].revents & POLLIN) {
                socks_handle_client(socks_listen_fd);
            }
        }
    }
    
    if (socks_listen_fd > 0) close(socks_listen_fd);

cleanup:
    // Aufräumen (wird durch tor_destroy erledigt)
    tor_destroy(&G_circuit);
    
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    utl_logf("MAIN", "SUCCESS", "Programm beendet.");
    return 0;
}

