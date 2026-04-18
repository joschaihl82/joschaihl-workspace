#define TOR_CLIENT_MONOLITH 1

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/modes.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>

static volatile int section_types_globals = __LINE__;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef unsigned char unc;
typedef unsigned int uni;

#define SOCKS_LISTEN_PORT 9050
#define MAX_FDS 1024
#define MAX_STREAMS_PER_CIRCUIT 256
#define SOCKS_BUFFER_SIZE 8192
#define SHA1_DIGEST_LEN 20
#define SHA256_DIGEST_LEN 32
#define AES_KEY_LEN 16
#define DH_GROUP_MODULUS_LEN 128
#define DH_PUBLIC_KEY_LEN 128

#define CELL_SIZE 514
#define PAYLOAD_SIZE 509
#define POSITION_COMMAND 4
#define PAYLOAD_RELAY_DATA_OFFSET 11
#define DIGEST_LENGTH 4
#define MAX_CIRCUIT_HOPS 3
#define RELAY_HEADER_LEN 11

enum cell_command { command_padding=0, command_create2=10, command_created2=11, command_relay=3, command_destroy=4, command_versions=7 };
enum relay_command { relay_begin=1, relay_data=2, relay_end=3, relay_connected=4, relay_sendme=5, relay_extend2=14, relay_extended2=15 };
enum circuit_state { circuit_disconnected=0, circuit_connected_tcp=1, circuit_connected_tls=2, circuit_waiting_versions=3, circuit_waiting_create_response=4, circuit_established=5, circuit_destroyed=6, circuit_extending=7 };
enum hop_state { hop_created=0, hop_key_established=1, hop_extended=2, hop_failed=3 };
enum socks_stream_state { socks_state_negotiation=0, socks_state_request=1, socks_state_connected=2, socks_state_closing=3 };
enum socks_command { socks_command_connect=1 };

const unc NTOR_CONTEXT_STRING[] = "Tor 1 Key Material";
const unc TOR_DH1024_G[] = { 0x02 };
const char* TOR_GUARD_IP = "127.0.0.1";
const uint16 TOR_GUARD_PORT = 9001;
const char* TOR_GUARD_ID_KEY_HEX = "0101010101010101010101010101010101010101";

static const unc TOR_DH1024_P[DH_GROUP_MODULUS_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
    0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
    0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
    0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x22, 0x44, 0xAD, 0x99, 0xD4, 0x8A, 0x01, 0xFC,
    0xFE, 0x94, 0x0F, 0x0E, 0x6A, 0x45, 0x63, 0x5C, 0xAA, 0x1D, 0x95, 0xDD, 0x7B, 0x24, 0x6A, 0x42,
    0x50, 0x9F, 0x24, 0x16, 0x1D, 0xD1, 0x72, 0x31, 0x4E, 0x05, 0x73, 0xAA, 0x5B, 0x54, 0x06, 0x54,
    0x08, 0x2B, 0xD1, 0x7F, 0x18, 0x39, 0x6A, 0xD2, 0x43, 0x25, 0x3C, 0x8A, 0xFA, 0x12, 0x19, 0x98,
    0x2D, 0xFD, 0x1C, 0x06, 0x0D, 0xAA, 0xB6, 0x4E, 0x67, 0x7E, 0xE3, 0xF2, 0x68, 0xA7, 0x90, 0x63
};

static volatile int section_structs = __LINE__;

struct cell { unc buffer[CELL_SIZE]; size_t index; };
struct circuit_hop {
    SHA_CTX forward_digest; SHA_CTX backward_digest;
    AES_KEY forward_key; AES_KEY backward_key;
    unc forward_iv[AES_KEY_LEN]; unc forward_ec[AES_KEY_LEN]; uni forward_num;
    unc back_iv[AES_KEY_LEN]; unc back_ec[AES_KEY_LEN]; uni back_num;
    enum hop_state state;
    DH* dh_params;
    unc dh_public_key[DH_PUBLIC_KEY_LEN];
    unc dh_shared_secret[DH_GROUP_MODULUS_LEN];
};
struct net_connect {
    int socket_fd; enum circuit_state state;
    SSL_CTX *ssl_context; SSL *ssl_session;
    unc expected_identity_key[SHA1_DIGEST_LEN];
    bool ssl_handshake_done;
    unc read_buffer[CELL_SIZE]; size_t read_len;
};
struct tor_circuit {
    uint32 circuit_id; enum circuit_state state;
    struct net_connect* net_conn;
    struct circuit_hop* path[MAX_CIRCUIT_HOPS]; size_t path_len;
    struct circuit_hop* current_hop; uint32 package_send_window;
    struct stream* streams[MAX_STREAMS_PER_CIRCUIT]; uint16 next_stream_id;
};
struct stream {
    uint16 stream_id; struct tor_circuit* circuit;
    struct socks_client_connection* socks_conn;
    unc buffer_to_tor[SOCKS_BUFFER_SIZE]; size_t buffer_to_tor_len;
    unc buffer_to_socks[SOCKS_BUFFER_SIZE]; size_t buffer_to_socks_len;
    uint32 send_window; uint32 receive_window;
    bool is_open; bool connected_to_target;
    char hostname[256]; uint16 port;
};
struct socks_client_connection {
    int socket_fd; enum socks_stream_state state;
    unc input_buffer[SOCKS_BUFFER_SIZE]; size_t input_len;
    unc output_buffer[SOCKS_BUFFER_SIZE]; size_t output_len; size_t output_sent;
    struct stream* associated_stream;
};
struct tor_client_context {
    int socks_listener_fd; int max_fd;
    struct tor_circuit* active_circuit;
    struct socks_client_connection socks_clients[MAX_FDS];
    size_t num_socks_clients;
    fd_set read_fds; fd_set write_fds;
};

static volatile int section_forward_decls = __LINE__;

void socks_client_close(struct tor_client_context* context, struct socks_client_connection* conn);
void circuit_destroy_stream(struct stream* stream);
void circuit_process_cell(struct tor_circuit* circuit, struct cell* cell);
bool socks_process_client_read(struct socks_client_connection* conn, struct tor_circuit* circuit);
bool socks_process_negotiation(struct socks_client_connection* conn);
bool socks_process_request(struct socks_client_connection* conn, struct tor_circuit* circuit);
void socks_send_success_reply(struct socks_client_connection* conn);
void stream_process_socks_data(struct stream* stream, unc* data, size_t len);

static volatile int section_alloc_free = __LINE__;

struct circuit_hop* hop_new(void) {
    struct circuit_hop* h = (struct circuit_hop*)calloc(1, sizeof(struct circuit_hop));
    if (h) h->state = hop_created;
    return h;
}
void hop_free(struct circuit_hop* hop) {
    if (hop->dh_params) DH_free(hop->dh_params);
    free(hop);
}
struct net_connect* net_connect_new(void) {
    struct net_connect* n = (struct net_connect*)calloc(1, sizeof(struct net_connect));
    if (n) {
        n->socket_fd = -1;
        for(size_t i = 0; i < SHA1_DIGEST_LEN; i++) sscanf(TOR_GUARD_ID_KEY_HEX + 2*i, "%2hhx", &n->expected_identity_key[i]);
    }
    return n;
}
void stream_free(struct stream* stream) {
    if (stream->socks_conn) stream->socks_conn->associated_stream = NULL;
    free(stream);
}
void net_connect_free(struct net_connect* conn) {
    if (conn->ssl_session) SSL_free(conn->ssl_session);
    if (conn->socket_fd >= 0) close(conn->socket_fd);
    free(conn);
}
void circuit_add_hop(struct tor_circuit* c, struct circuit_hop* h) {
    if (c->path_len < MAX_CIRCUIT_HOPS) {
        c->path[c->path_len++] = h;
        c->current_hop = h;
    }
}
struct tor_circuit* circuit_new(uint32 id) {
    struct tor_circuit* c = (struct tor_circuit*)calloc(1, sizeof(struct tor_circuit));
    if (c) { c->circuit_id = id; c->package_send_window = 1000; c->next_stream_id = 1; }
    return c;
}
void circuit_free(struct tor_circuit* circuit) {
    for (size_t i = 0; i < circuit->path_len; i++) hop_free(circuit->path[i]);
    if (circuit->net_conn) net_connect_free(circuit->net_conn);
    for (int i = 0; i < MAX_STREAMS_PER_CIRCUIT; i++) {
        if (circuit->streams[i]) stream_free(circuit->streams[i]);
    }
    free(circuit);
}
struct stream* stream_new(struct tor_circuit* circuit, struct socks_client_connection* socks_conn) {
    uint16 stream_id = 0;
    for (int i = 1; i < MAX_STREAMS_PER_CIRCUIT; i++) {
        if (!circuit->streams[i]) {
            stream_id = circuit->next_stream_id++;
            if (circuit->next_stream_id >= MAX_STREAMS_PER_CIRCUIT) circuit->next_stream_id = 1;

            struct stream* new_stream = (struct stream*)calloc(1, sizeof(struct stream));
            if (!new_stream) return NULL;

            new_stream->stream_id = stream_id;
            new_stream->circuit = circuit;
            new_stream->socks_conn = socks_conn;
            new_stream->send_window = 500;
            new_stream->receive_window = 500;
            new_stream->is_open = true;

            circuit->streams[i] = new_stream;
            socks_conn->associated_stream = new_stream;
            return new_stream;
        }
    }
    return NULL;
}

static volatile int section_utils_crypto = __LINE__;

#define cell_get_payload(C) ((C)->buffer + 7)
#define relay_cell_get_digest_ptr(C) (cell_get_payload(C))

void util_int32_to_array_be(unc* a, uint32 i) { a[0]=(unc)(i>>24); a[1]=(unc)(i>>16); a[2]=(unc)(i>>8); a[3]=(unc)(i); }
uint32 util_array_to_int32_be(const unc* buf) { return (((uint32)buf[0]<<24)|((uint32)buf[1]<<16)|((uint32)buf[2]<<8)|((uint32)buf[3])); }
void util_int16_to_array_be(unc* a, uint16 i) { a[0]=(unc)(i>>8); a[1]=(unc)(i); }
uint16 util_array_to_int16_be(const unc* buf) { return (((uint16)buf[0] << 8) | ((uint16)buf[1])); }

bool hop_generate_dh_keys(struct circuit_hop* hop) {
    BIGNUM *p, *g;

    hop->dh_params = DH_new();
    if (!hop->dh_params) return false;

    p = BN_bin2bn(TOR_DH1024_P, DH_GROUP_MODULUS_LEN, NULL);
    g = BN_bin2bn(TOR_DH1024_G, 1, NULL);

    if (!p || !g) {
        if (p) BN_free(p); if (g) BN_free(g); DH_free(hop->dh_params); return false;
    }

    if (!DH_set0_pqg(hop->dh_params, p, NULL, g)) {
        BN_free(p); BN_free(g); DH_free(hop->dh_params); return false;
    }

    if (DH_generate_key(hop->dh_params) != 1) {
        DH_free(hop->dh_params); return false;
    }

    const BIGNUM *pub_key = DH_get0_pub_key(hop->dh_params);
    if (!pub_key) { DH_free(hop->dh_params); return false; }

    size_t len = BN_num_bytes(pub_key);
    memset(hop->dh_public_key, 0, DH_PUBLIC_KEY_LEN);
    BN_bn2bin(pub_key, hop->dh_public_key + (DH_PUBLIC_KEY_LEN - len));

    return true;
}
bool hop_compute_shared_secret(struct circuit_hop* hop, const unc* public_key_answer, size_t public_key_len) {
    BIGNUM *remote_pub_key;
    int secret_len;
    if (public_key_len > DH_GROUP_MODULUS_LEN) return false;
    remote_pub_key = BN_bin2bn(public_key_answer, (int)public_key_len, NULL);
    if (!remote_pub_key) return false;
    secret_len = DH_compute_key(hop->dh_shared_secret, remote_pub_key, hop->dh_params);
    BN_free(remote_pub_key);
    if (secret_len != DH_GROUP_MODULUS_LEN) return false;
    return true;
}
void ntor_derive_key_material(const unc* hmac_key, size_t hmac_key_len,
                              const unc* key_hash_input, size_t key_hash_input_len,
                              unc* output, size_t out_len)
{
    const size_t hash_len = SHA256_DIGEST_LEN;
    size_t num_blocks = (out_len + hash_len - 1) / hash_len;
    unc block_data[SHA256_DIGEST_LEN + 1];
    unsigned int output_len;

    HMAC(EVP_sha256(), hmac_key, (int)hmac_key_len, key_hash_input, (int)key_hash_input_len, output, &output_len);

    for (size_t i = 1; i < num_blocks; ++i) {
        memcpy(block_data, output + (i - 1) * hash_len, hash_len);
        block_data[hash_len] = (uint8)(i + 1);
        HMAC(EVP_sha256(), hmac_key, (int)hmac_key_len, block_data, (int)hash_len + 1, output + i * hash_len, &output_len);
    }
}
void hop_set_key_material_from_ntor(struct circuit_hop* hop, const unc* key_material)
{
    const unc* ptr = key_material;

    SHA1_Init(&hop->forward_digest); SHA1_Update(&hop->forward_digest, ptr, SHA1_DIGEST_LEN); ptr += SHA1_DIGEST_LEN;
    SHA1_Init(&hop->backward_digest); SHA1_Update(&hop->backward_digest, ptr, SHA1_DIGEST_LEN); ptr += SHA1_DIGEST_LEN;

    AES_set_encrypt_key(ptr, AES_KEY_LEN * 8, &hop->forward_key); ptr += AES_KEY_LEN;
    AES_set_encrypt_key(ptr, AES_KEY_LEN * 8, &hop->backward_key); ptr += AES_KEY_LEN;

    memset(hop->forward_iv, 0, AES_KEY_LEN); memcpy(hop->forward_iv, ptr, AES_KEY_LEN); ptr += AES_KEY_LEN;
    memset(hop->back_iv, 0, AES_KEY_LEN); memcpy(hop->back_iv, ptr, AES_KEY_LEN);

    memset(hop->forward_ec, 0, AES_KEY_LEN); hop->forward_num = 0;
    memset(hop->back_ec, 0, AES_KEY_LEN); hop->back_num = 0;

    hop->state = hop_key_established;
}
void hop_aes_ctr_encrypt(AES_KEY* key, unc* iv, unc* ec, uni* num, unc* data, size_t data_len)
{
    unsigned char buf[CELL_SIZE];
    CRYPTO_ctr128_encrypt(data, buf, data_len, key, iv, ec, num);
    memcpy(data, buf, data_len);
}
void hop_aes_ctr_decrypt(AES_KEY* key, unc* iv, unc* ec, uni* num, unc* data, size_t data_len)
{
    hop_aes_ctr_encrypt(key, iv, ec, num, data, data_len);
}
void hop_calculate_4byte_digest(SHA_CTX* digest, struct cell* cell, unc* result)
{
    unc full_sha1[SHA1_DIGEST_LEN];
    unc* cell_payload = cell_get_payload(cell);
    SHA_CTX temp;

    memcpy(&temp, digest, sizeof(SHA_CTX));
    SHA1_Update(&temp, cell_payload, PAYLOAD_SIZE);
    SHA1_Final(full_sha1, &temp);

    memcpy(result, full_sha1, DIGEST_LENGTH);
}
void circuit_encrypt_cell_path(struct tor_circuit* circuit, struct cell* cell, bool is_new_cell)
{
    for (int i = (int)circuit->path_len - 1; i >= 0; --i)
    {
        struct circuit_hop* hop = circuit->path[i];

        if (i == (int)circuit->path_len - 1 && is_new_cell && cell->buffer[POSITION_COMMAND] == command_relay)
        {
            unc digest_buf[DIGEST_LENGTH];
            hop_calculate_4byte_digest(&hop->forward_digest, cell, digest_buf);
            memcpy(relay_cell_get_digest_ptr(cell), digest_buf, DIGEST_LENGTH);
            SHA1_Update(&hop->forward_digest, digest_buf, DIGEST_LENGTH);
        }

        hop_aes_ctr_encrypt(&hop->forward_key, hop->forward_iv, hop->forward_ec, &hop->forward_num,
                            cell_get_payload(cell), PAYLOAD_SIZE);
    }
}
int circuit_decrypt_cell(struct tor_circuit* circuit, struct cell* cell)
{
    int found_hop_index = -1;

    for (size_t i = 0; i < circuit->path_len; i++)
    {
        struct circuit_hop* hop = circuit->path[i];

        hop_aes_ctr_decrypt(&hop->backward_key, hop->back_iv, hop->back_ec, &hop->back_num,
                            cell_get_payload(cell), PAYLOAD_SIZE);

        if (cell->buffer[POSITION_COMMAND] == command_relay)
        {
            unc* payload = cell_get_payload(cell);
            unc received_digest[DIGEST_LENGTH];
            unc expected_digest[DIGEST_LENGTH];

            memcpy(received_digest, payload, DIGEST_LENGTH);
            memset(payload, 0, DIGEST_LENGTH);

            hop_calculate_4byte_digest(&hop->backward_digest, cell, expected_digest);

            if (memcmp(received_digest, expected_digest, DIGEST_LENGTH) == 0) {
                SHA1_Update(&hop->backward_digest, received_digest, DIGEST_LENGTH);
                memcpy(payload, received_digest, DIGEST_LENGTH);
                memmove(payload, payload + 3, PAYLOAD_SIZE - 3);
                found_hop_index = (int)i;
                break;
            }
        }
    }
    return found_hop_index;
}

static volatile int section_net_tls = __LINE__;

int system_socket_set_nonblocking(int fd) { return fcntl(fd, F_SETFL, O_NONBLOCK); }
int system_socket_check_connect_status(int fd) {
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) return -1;
    if (error == 0) return 1;
    if (error == EINPROGRESS || error == EALREADY) return 0;
    return -1;
}
int system_socket_connect_nonblocking(int fd, const char* ip, uint16 port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) return -1;

    system_socket_set_nonblocking(fd);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        if (errno == EINPROGRESS) return 0;
        return -1;
    }
    return 1;
}
SSL_CTX *net_connect_tls_setup() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD *method = TLSv1_2_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
    return ctx;
}
bool net_connect_ssl_handshake_nonblocking(struct net_connect* conn) {
    if (conn->ssl_handshake_done) return true;

    int ret = SSL_connect(conn->ssl_session);
    if (ret == 1) {
        conn->ssl_handshake_done = true;
        return true;
    }
    if (ret < 0) {
        int err = SSL_get_error(conn->ssl_session, ret);
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            return false;
        }
        ERR_print_errors_fp(stderr);
        return false;
    }
    return false;
}
int net_connect_send_cell_tls(struct net_connect* conn, struct cell* cell) {
    int ret = SSL_write(conn->ssl_session, cell->buffer, CELL_SIZE);
    if (ret < 0) {
        int err = SSL_get_error(conn->ssl_session, ret);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            ERR_print_errors_fp(stderr);
            return -1;
        }
    }
    return ret;
}
int net_connect_receive_cell_tls(struct net_connect* conn, struct cell* cell) {
    int ret = SSL_read(conn->ssl_session, conn->read_buffer + conn->read_len, CELL_SIZE - conn->read_len);
    if (ret < 0) {
        int err = SSL_get_error(conn->ssl_session, ret);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            ERR_print_errors_fp(stderr);
            return -1;
        }
        return 0;
    }
    if (ret == 0) return -1;

    conn->read_len += ret;

    if (conn->read_len >= CELL_SIZE) {
        memcpy(cell->buffer, conn->read_buffer, CELL_SIZE);
        size_t remaining = conn->read_len - CELL_SIZE;
        if (remaining > 0) {
            memmove(conn->read_buffer, conn->read_buffer + CELL_SIZE, remaining);
        }
        conn->read_len = remaining;
        return CELL_SIZE;
    }
    return 0;
}

static volatile int section_build_cells = __LINE__;

void cell_init(struct cell* cell, uint32 circ_id, enum cell_command cmd) {
    memset(cell, 0, sizeof(struct cell));
    util_int32_to_array_be(cell->buffer, circ_id);
    cell->buffer[POSITION_COMMAND] = (unc)cmd;
}
bool tor_connect_send_versions(struct net_connect* conn)
{
    uint16 versions[] = { 0x0005 };
    uint32 payload_len = 2 * sizeof(versions) / sizeof(uint16);
    uint32 total_len = 4 + 1 + 2 + payload_len;

    unc buffer[64];
    util_int32_to_array_be(buffer, total_len);

    util_int32_to_array_be(buffer + 4, 0x00000000);
    buffer[8] = command_versions;
    util_int16_to_array_be(buffer + 9, payload_len);

    util_int16_to_array_be(buffer + 11, versions[0]);

    int ret = SSL_write(conn->ssl_session, buffer, total_len + 4);
    if (ret > 0) {
        conn->state = circuit_waiting_versions;
        return true;
    }
    return false;
}
bool tor_connect_start_circuit(struct tor_circuit* circuit)
{
    if (circuit->path_len != 1 || circuit->path[0]->state != hop_created) return false;

    struct circuit_hop* hop = circuit->path[0];
    if (!hop_generate_dh_keys(hop)) return false;

    struct cell cell;
    cell_init(&cell, circuit->circuit_id, command_create2);

    unc* payload = cell_get_payload(&cell);
    util_int16_to_array_be(payload, 0x0002);

    unc guard_id_key[SHA1_DIGEST_LEN];
    for(size_t i = 0; i < SHA1_DIGEST_LEN; i++) sscanf(TOR_GUARD_ID_KEY_HEX + 2*i, "%2hhx", &guard_id_key[i]);

    unc ntor_public_key[SHA256_DIGEST_LEN];
    memset(ntor_public_key, 0x11, SHA256_DIGEST_LEN);
    unc* dh_pub_key = hop->dh_public_key;

    unc create2_payload[500];
    unc* ptr = create2_payload;

    memcpy(ptr, guard_id_key, SHA1_DIGEST_LEN); ptr += SHA1_DIGEST_LEN;
    memcpy(ptr, ntor_public_key, SHA256_DIGEST_LEN); ptr += SHA256_DIGEST_LEN;
    memcpy(ptr, dh_pub_key, DH_PUBLIC_KEY_LEN); ptr += DH_PUBLIC_KEY_LEN;

    uint16 handshake_len = (uint16)(ptr - create2_payload);
    util_int16_to_array_be(payload + 2, handshake_len);

    memcpy(payload + 4, create2_payload, handshake_len);

    if (net_connect_send_cell_tls(circuit->net_conn, &cell) > 0) {
        circuit->state = circuit_waiting_create_response;
        return true;
    }
    return false;
}
bool circuit_send_relay_begin(struct stream* stream, const char* hostname, uint16 port)
{
    struct tor_circuit* circuit = stream->circuit;
    struct cell cell;
    uint16 hostname_len = (uint16)strlen(hostname);
    uint16 total_data_len = hostname_len + 1 + 2;

    if (total_data_len > PAYLOAD_SIZE - RELAY_HEADER_LEN) return false;

    cell_init(&cell, circuit->circuit_id, command_relay);
    unc* payload = cell_get_payload(&cell);

    payload[4] = relay_begin;
    util_int16_to_array_be(payload + 5, stream->stream_id);
    util_int16_to_array_be(payload + 7, 0x0000);
    util_int16_to_array_be(payload + 9, total_data_len);

    unc* data_ptr = payload + PAYLOAD_RELAY_DATA_OFFSET;

    memcpy(data_ptr, hostname, hostname_len);
    data_ptr += hostname_len;
    *data_ptr++ = 0x00;

    util_int16_to_array_be(data_ptr, port);

    circuit_encrypt_cell_path(circuit, &cell, true);
    return net_connect_send_cell_tls(circuit->net_conn, &cell) > 0;
}
bool circuit_send_relay_data(struct stream* stream)
{
    if (stream->buffer_to_tor_len == 0 || stream->send_window == 0) return false;

    size_t bytes_to_send = stream->buffer_to_tor_len;
    size_t offset = 0;
    bool sent_data = false;

    while (bytes_to_send > 0 && stream->send_window > 0)
    {
        size_t chunk_len = (bytes_to_send > PAYLOAD_SIZE - RELAY_HEADER_LEN) ? (PAYLOAD_SIZE - RELAY_HEADER_LEN) : bytes_to_send;

        struct cell cell;
        struct tor_circuit* circuit = stream->circuit;

        cell_init(&cell, circuit->circuit_id, command_relay);
        unc* payload = cell_get_payload(&cell);

        payload[4] = relay_data;
        util_int16_to_array_be(payload + 5, stream->stream_id);
        util_int16_to_array_be(payload + 7, 0x0000);
        util_int16_to_array_be(payload + 9, (uint16)chunk_len);

        memcpy(payload + PAYLOAD_RELAY_DATA_OFFSET, stream->buffer_to_tor + offset, chunk_len);
        memset(payload + PAYLOAD_RELAY_DATA_OFFSET + chunk_len, 0, PAYLOAD_SIZE - RELAY_HEADER_LEN - chunk_len);

        circuit_encrypt_cell_path(circuit, &cell, true);
        if (net_connect_send_cell_tls(circuit->net_conn, &cell) <= 0) {
            break;
        }

        offset += chunk_len;
        bytes_to_send -= chunk_len;
        stream->send_window--;
        sent_data = true;
    }

    if (offset > 0) {
        size_t remaining_len = stream->buffer_to_tor_len - offset;
        memmove(stream->buffer_to_tor, stream->buffer_to_tor + offset, remaining_len);
        stream->buffer_to_tor_len = remaining_len;
    }

    return sent_data;
}
bool circuit_send_relay_sendme(struct stream* stream)
{
    struct cell cell;
    struct tor_circuit* circuit = stream->circuit;

    cell_init(&cell, circuit->circuit_id, command_relay);
    unc* payload = cell_get_payload(&cell);

    payload[4] = relay_sendme;
    util_int16_to_array_be(payload + 5, stream->stream_id);
    util_int16_to_array_be(payload + 7, 0x0000);
    util_int16_to_array_be(payload + 9, 0x0000);

    circuit_encrypt_cell_path(circuit, &cell, true);
    return net_connect_send_cell_tls(circuit->net_conn, &cell) > 0;
}
bool circuit_send_relay_end(struct stream* stream, uint8 reason)
{
    struct cell cell;
    struct tor_circuit* circuit = stream->circuit;

    cell_init(&cell, circuit->circuit_id, command_relay);
    unc* payload = cell_get_payload(&cell);

    payload[4] = relay_end;
    util_int16_to_array_be(payload + 5, stream->stream_id);
    util_int16_to_array_be(payload + 7, 0x0000);
    util_int16_to_array_be(payload + 9, 0x0001);

    payload[PAYLOAD_RELAY_DATA_OFFSET] = reason;

    circuit_encrypt_cell_path(circuit, &cell, true);
    return net_connect_send_cell_tls(circuit->net_conn, &cell) > 0;
}

static volatile int section_socks_stream = __LINE__;

struct stream* circuit_find_stream(struct tor_circuit* circuit, uint16 stream_id)
{
    if (stream_id == 0) return NULL;
    for (int i = 0; i < MAX_STREAMS_PER_CIRCUIT; i++) {
        if (circuit->streams[i] && circuit->streams[i]->stream_id == stream_id) return circuit->streams[i];
    }
    return NULL;
}
void stream_process_socks_data(struct stream* stream, unc* data, size_t len)
{
    if (!stream->is_open) return;
    size_t copy_len = len;
    if (stream->buffer_to_tor_len + copy_len > SOCKS_BUFFER_SIZE) {
        copy_len = SOCKS_BUFFER_SIZE - stream->buffer_to_tor_len;
    }
    if (copy_len > 0) {
        memcpy(stream->buffer_to_tor + stream->buffer_to_tor_len, data, copy_len);
        stream->buffer_to_tor_len += copy_len;
    }
}
void socks_send_success_reply(struct socks_client_connection* conn)
{
    conn->output_buffer[0] = 0x05;
    conn->output_buffer[1] = 0x00;
    conn->output_buffer[2] = 0x00;
    conn->output_buffer[3] = 0x01;
    memset(conn->output_buffer + 4, 0, 6);
    conn->output_len = 10;
}
bool socks_process_negotiation(struct socks_client_connection* conn) {
    if (conn->input_len < 2) return false;
    uint8 ver = conn->input_buffer[0];
    uint8 nmethods = conn->input_buffer[1];
    if (ver != 0x05) return false;
    if (conn->input_len < 2 + nmethods) return false;
    bool no_auth_found = false;
    for (int i = 0; i < nmethods; i++) {
        if (conn->input_buffer[2 + i] == 0x00) {
            no_auth_found = true; break;
        }
    }
    if (!no_auth_found) return false;
    conn->output_buffer[0] = 0x05; conn->output_buffer[1] = 0x00;
    conn->output_len = 2;
    conn->state = socks_state_request;
    return true;
}
bool socks_process_request(struct socks_client_connection* conn, struct tor_circuit* circuit) {
    if (circuit->state != circuit_established) {
        conn->output_buffer[0] = 0x05; conn->output_buffer[1] = 0x04; conn->output_len = 10; return false;
    }
    if (conn->input_len < 5) return false;
    uint8 ver = conn->input_buffer[0];
    uint8 cmd = conn->input_buffer[1];
    uint8 atyp = conn->input_buffer[3];
    if (ver != 0x05 || cmd != socks_command_connect) return false;
    unc* addr_ptr = conn->input_buffer + 4;
    size_t addr_len = 0;
    if (atyp == 1) addr_len = 4;
    else if (atyp == 3) addr_len = addr_ptr[0] + 1;
    else { conn->output_buffer[0] = 0x05; conn->output_buffer[1] = 0x08; conn->output_len = 10; return false; }

    if (conn->input_len < 4 + addr_len + 2) return false;
    char hostname_buf[256]; uint16 port;

    if (atyp == 3) {
        uint8 domain_len = addr_ptr[0];
        memcpy(hostname_buf, addr_ptr + 1, domain_len);
        hostname_buf[domain_len] = '\0';
    } else if (atyp == 1) {
        inet_ntop(AF_INET, addr_ptr, hostname_buf, sizeof(hostname_buf));
    }
    port = util_array_to_int16_be(addr_ptr + addr_len);

    struct stream* new_stream = stream_new(circuit, conn);
    if (!new_stream) return false;

    strncpy(new_stream->hostname, hostname_buf, sizeof(new_stream->hostname) - 1);
    new_stream->port = port;

    if (!circuit_send_relay_begin(new_stream, hostname_buf, port)) {
        circuit_destroy_stream(new_stream); return false;
    }

    conn->state = socks_state_connected;
    new_stream->connected_to_target = false;

    size_t request_len = 4 + addr_len + 2;
    memmove(conn->input_buffer, conn->input_buffer + request_len, conn->input_len - request_len);
    conn->input_len -= request_len;

    return true;
}
bool socks_process_client_read(struct socks_client_connection* conn, struct tor_circuit* circuit)
{
    bool buffer_consumed = false;
    if (conn->state == socks_state_negotiation) {
        if (socks_process_negotiation(conn)) buffer_consumed = true;
    } else if (conn->state == socks_state_request) {
        if (socks_process_request(conn, circuit)) buffer_consumed = true;
    } else if (conn->state == socks_state_connected && conn->associated_stream) {
        if (conn->input_len > 0) {
            stream_process_socks_data(conn->associated_stream, conn->input_buffer, conn->input_len);
            conn->input_len = 0;
            buffer_consumed = true;
        }
    }
    return buffer_consumed;
}
void circuit_destroy_stream(struct stream* stream)
{
    if (!stream) return;
    if (stream->is_open) {
        circuit_send_relay_end(stream, 0x01);
        stream->is_open = false;
    }
    for (int i = 0; i < MAX_STREAMS_PER_CIRCUIT; i++) {
        if (stream->circuit->streams[i] == stream) {
            stream->circuit->streams[i] = NULL; break;
        }
    }
    if (stream->socks_conn) {
        close(stream->socks_conn->socket_fd);
        stream->socks_conn->socket_fd = -1;
    }
    stream_free(stream);
}
void socks_client_close(struct tor_client_context* context, struct socks_client_connection* conn)
{
    if (conn->socket_fd >= 0) {
        if (conn->associated_stream) {
            circuit_destroy_stream(conn->associated_stream);
            conn->associated_stream = NULL;
        }
        close(conn->socket_fd);
        conn->socket_fd = -1;
        conn->state = socks_state_closing;
        context->num_socks_clients--;
    }
}
int socks_listener_init(struct tor_client_context* context) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    system_socket_set_nonblocking(fd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(SOCKS_LISTEN_PORT);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { close(fd); return -1; }
    if (listen(fd, 100) < 0) { close(fd); return -1; }

    context->socks_listener_fd = fd;
    context->max_fd = (fd > context->max_fd) ? fd : context->max_fd;
    return fd;
}
void socks_accept_connection(struct tor_client_context* context) {
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    client_fd = accept(context->socks_listener_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        return;
    }

    if (client_fd >= MAX_FDS) { close(client_fd); return; }

    struct socks_client_connection* conn = &context->socks_clients[client_fd];
    memset(conn, 0, sizeof(struct socks_client_connection));
    conn->socket_fd = client_fd;
    conn->state = socks_state_negotiation;

    system_socket_set_nonblocking(client_fd);

    context->max_fd = (client_fd > context->max_fd) ? client_fd : context->max_fd;
    context->num_socks_clients++;
}

static volatile int section_tor_cell_processing = __LINE__;

void relay_process_cell(struct tor_circuit* circuit, struct cell* cell, int hop_index)
{
    unc* payload = cell_get_payload(cell);
    enum relay_command cmd = (enum relay_command)payload[3];
    uint16 stream_id = util_array_to_int16_be(payload + 4);
    uint16 data_len = util_array_to_int16_be(payload + 9);
    unc* data = payload + PAYLOAD_RELAY_DATA_OFFSET - 7;

    struct stream* stream = circuit_find_stream(circuit, stream_id);

    if (cmd == relay_extended2 && circuit->state == circuit_extending)
    {
        struct circuit_hop* new_hop = circuit->path[circuit->path_len - 1];
        if (new_hop->state == hop_created) {
            new_hop->state = hop_extended;
            circuit->state = circuit_established;
        }
        return;
    }

    if (!stream) return;

    switch (cmd) {
        case relay_connected:
            stream->connected_to_target = true;
            if (stream->socks_conn) socks_send_success_reply(stream->socks_conn);
            break;

        case relay_data:
            if (data_len > 0) {
                if (stream->socks_conn && stream->socks_conn->output_len + data_len <= SOCKS_BUFFER_SIZE) {
                     memcpy(stream->socks_conn->output_buffer + stream->socks_conn->output_len, data, data_len);
                     stream->socks_conn->output_len += data_len;
                }
                stream->receive_window -= 1;
                if (stream->receive_window < 10) {
                    circuit_send_relay_sendme(stream);
                    stream->receive_window += 100;
                }
            }
            break;

        case relay_end:
            circuit_destroy_stream(stream);
            break;

        case relay_sendme:
            stream->send_window += 100;
            break;

        default: break;
    }
}
void circuit_process_cell(struct tor_circuit* circuit, struct cell* cell)
{
    uint32 circ_id = util_array_to_int32_be(cell->buffer);
    enum cell_command cmd = (enum cell_command)cell->buffer[POSITION_COMMAND];

    if (circ_id != circuit->circuit_id) return;

    switch (cmd) {
        case command_created2:
            if (circuit->state == circuit_waiting_create_response && circuit->path_len == 1) {
                struct circuit_hop* entry_hop = circuit->path[0];
                unc* payload = cell_get_payload(cell);

                unc key_material[104];
                memset(key_material, 0x55, 104);
                hop_set_key_material_from_ntor(entry_hop, key_material);

                entry_hop->state = hop_key_established;
                circuit->state = circuit_established;
            }
            break;

        case command_relay: {
            int hop_index = circuit_decrypt_cell(circuit, cell);
            if (hop_index >= 0) relay_process_cell(circuit, cell, hop_index);
            break;
        }

        case command_destroy:
            circuit->state = circuit_destroyed;
            for (int i = 0; i < MAX_STREAMS_PER_CIRCUIT; i++) {
                if (circuit->streams[i]) circuit_destroy_stream(circuit->streams[i]);
            }
            break;

        default: break;
    }
}

static volatile int section_main_loop = __LINE__;

void tor_main_loop_prepare(struct tor_client_context* context)
{
    FD_ZERO(&context->read_fds);
    FD_ZERO(&context->write_fds);

    if (context->socks_listener_fd >= 0) FD_SET(context->socks_listener_fd, &context->read_fds);

    struct net_connect* tor_conn = context->active_circuit->net_conn;
    if (tor_conn->socket_fd >= 0) {
        FD_SET(tor_conn->socket_fd, &context->read_fds);
        if (tor_conn->ssl_handshake_done || context->active_circuit->state == circuit_connected_tcp) {
            FD_SET(tor_conn->socket_fd, &context->write_fds);
        }
    }

    for (int i = 0; i <= context->max_fd; i++) {
        struct socks_client_connection* conn = &context->socks_clients[i];
        if (conn->socket_fd == i && conn->socket_fd >= 0) {
            FD_SET(conn->socket_fd, &context->read_fds);
            if (conn->output_len > conn->output_sent) {
                FD_SET(conn->socket_fd, &context->write_fds);
            } else if (conn->associated_stream && conn->associated_stream->buffer_to_tor_len > 0) {
                (void)0;
            }
        }
    }
}
void tor_main_loop_process(struct tor_client_context* context)
{
    struct net_connect* tor_conn = context->active_circuit->net_conn;
    struct tor_circuit* circuit = context->active_circuit;

    if (tor_conn->socket_fd >= 0) {
        if (circuit->state == circuit_connected_tcp && FD_ISSET(tor_conn->socket_fd, &context->write_fds)) {
            int status = system_socket_check_connect_status(tor_conn->socket_fd);
            if (status == 1) {
                tor_conn->ssl_session = SSL_new(tor_conn->ssl_context);
                SSL_set_fd(tor_conn->ssl_session, tor_conn->socket_fd);
                circuit->state = circuit_connected_tls;
            } else if (status == -1) { circuit->state = circuit_disconnected; }
        }

        if (circuit->state == circuit_connected_tls) {
            if (net_connect_ssl_handshake_nonblocking(tor_conn)) {
                tor_connect_send_versions(tor_conn);
            } else if (!tor_conn->ssl_handshake_done) {
                (void)0;
            } else { circuit->state = circuit_disconnected; }
        }

        if (circuit->state == circuit_waiting_versions) {
            if (FD_ISSET(tor_conn->socket_fd, &context->read_fds)) {
                struct cell incoming_cell;
                (void)incoming_cell;
                tor_connect_start_circuit(circuit);
            }
        }

        if (FD_ISSET(tor_conn->socket_fd, &context->read_fds) && circuit->state >= circuit_waiting_create_response) {
            struct cell incoming_cell;
            if (net_connect_receive_cell_tls(tor_conn, &incoming_cell) == CELL_SIZE) {
                circuit_process_cell(circuit, &incoming_cell);
            }
        }
    }

    if (FD_ISSET(context->socks_listener_fd, &context->read_fds)) { socks_accept_connection(context); }

    for (int i = 0; i <= context->max_fd; i++) {
        struct socks_client_connection* conn = &context->socks_clients[i];
        if (conn->socket_fd != i || conn->socket_fd < 0) continue;

        if (FD_ISSET(conn->socket_fd, &context->read_fds)) {
            ssize_t bytes_read = read(conn->socket_fd, conn->input_buffer + conn->input_len, SOCKS_BUFFER_SIZE - conn->input_len);
            if (bytes_read > 0) {
                conn->input_len += bytes_read;
                socks_process_client_read(conn, circuit);
            } else if (bytes_read == 0 || (errno != EWOULDBLOCK && errno != EAGAIN)) {
                socks_client_close(context, conn);
            }
        }

        if (FD_ISSET(conn->socket_fd, &context->write_fds) && conn->output_len > conn->output_sent) {
            ssize_t bytes_written = write(conn->socket_fd, conn->output_buffer + conn->output_sent, conn->output_len - conn->output_sent);
            if (bytes_written > 0) {
                conn->output_sent += bytes_written;
                if (conn->output_sent == conn->output_len) {
                    conn->output_len = 0; conn->output_sent = 0;
                }
            } else if (bytes_written < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
                socks_client_close(context, conn);
            }
        }

        if (conn->associated_stream && conn->associated_stream->buffer_to_tor_len > 0 && circuit->state == circuit_established) {
            circuit_send_relay_data(conn->associated_stream);
        }
    }
}

static volatile int section_main_func = __LINE__;

int main()
{
    struct tor_client_context context;
    memset(&context, 0, sizeof(context));
    context.max_fd = 0;

    SSL_CTX *global_ssl_context = net_connect_tls_setup();
    if (!global_ssl_context) { fprintf(stderr, "[FATAL] Globale TLS-Initialisierung fehlgeschlagen.\n"); return 1; }

    if (socks_listener_init(&context) < 0) {
        fprintf(stderr, "[FATAL] Konnte SOCKS5 Listener nicht starten.\n");
        SSL_CTX_free(global_ssl_context);
        return 1;
    }

    struct tor_circuit* circuit = circuit_new(0x10000);
    circuit->net_conn = net_connect_new();
    circuit->net_conn->ssl_context = global_ssl_context;
    context.active_circuit = circuit;

    circuit->net_conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (circuit->net_conn->socket_fd < 0) {
        fprintf(stderr, "[FATAL] Tor Socket konnte nicht erstellt werden.\n");
        return 1;
    }
    context.max_fd = (circuit->net_conn->socket_fd > context->max_fd) ? circuit->net_conn->socket_fd : context->max_fd;

    int connect_status = system_socket_connect_nonblocking(circuit->net_conn->socket_fd, TOR_GUARD_IP, TOR_GUARD_PORT);
    if (connect_status == 0) {
        circuit->state = circuit_connected_tcp;
    } else if (connect_status == 1) {
        circuit->state = circuit_connected_tcp;
    } else {
        fprintf(stderr, "[FATAL] Tor Connect fehlgeschlagen.\n");
        return 1;
    }

    circuit_add_hop(circuit, hop_new());

    printf("[MAIN] Tor Client gestartet. Lausche auf SOCKS5-Anfragen auf Port %d.\n", SOCKS_LISTEN_PORT);

    while (circuit->state != circuit_destroyed && circuit->state != circuit_disconnected)
    {
        tor_main_loop_prepare(&context);

        struct timeval timeout = (struct timeval){1, 0};
        int activity = select(context.max_fd + 1, &context.read_fds, &context.write_fds, NULL, &timeout);

        if (activity < 0 && (errno != EINTR)) { perror("[MAIN] Select Fehler"); break; }
        if (activity > 0) { tor_main_loop_process(&context); }
    }

    circuit_free(circuit);
    SSL_CTX_free(global_ssl_context);
    EVP_cleanup();

    return 0;
}

