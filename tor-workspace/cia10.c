// tor.c
// Minimal SOCKS5 -> TLS Guard -> Target proxy (single-file, C)
// Purpose: allow a browser to use SOCKS5 (127.0.0.1:9050) and proxy traffic
// through a local TLS Guard server (127.0.0.1:7001) running in this process.
//
// Build (Linux):
//   gcc -O2 -Wall tor.c -o tor -lssl -lcrypto -lpthread
//
// Run:
//   ./tor [socks_port] [guard_port]
// e.g. ./tor            (uses socks 9050, guard 7001)
//      ./tor 1080 9001   (uses socks 1080, guard 9001)

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>

static void logf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

/* -------------------- Utilities -------------------- */

static int tcp_connect_ip(const char *ip, int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;
    struct sockaddr_in sa; memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET; sa.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &sa.sin_addr) != 1) { close(s); return -1; }
    if (connect(s, (struct sockaddr*)&sa, sizeof(sa)) != 0) { close(s); return -1; }
    return s;
}

static int resolve_host_ipv4(const char *host, char out[64]) {
    struct addrinfo hints, *res = NULL;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    int rv = getaddrinfo(host, NULL, &hints, &res);
    if (rv != 0) { return 0; }
    struct sockaddr_in *sin = (struct sockaddr_in*)res->ai_addr;
    if (!inet_ntop(AF_INET, &sin->sin_addr, out, 64)) { freeaddrinfo(res); return 0; }
    freeaddrinfo(res);
    return 1;
}

/* -------------------- Self-signed cert generation -------------------- */
/* Generate RSA2048 key + self-signed X509 certificate (returned as X509* and EVP_PKEY*) */
static int make_self_signed_cert(EVP_PKEY **out_pkey, X509 **out_x509) {
    int rc = 0;
    EVP_PKEY *pkey = NULL;
    RSA *rsa = NULL;
    X509 *x = NULL;
    BIGNUM *bn = NULL;

    bn = BN_new();
    if (!bn) goto done;
    if (!BN_set_word(bn, RSA_F4)) goto done;
    rsa = RSA_new();
    if (!rsa) goto done;
    if (!RSA_generate_key_ex(rsa, 2048, bn, NULL)) goto done;

    pkey = EVP_PKEY_new();
    if (!pkey) goto done;
    if (!EVP_PKEY_assign_RSA(pkey, rsa)) goto done;
    rsa = NULL; /* now owned by pkey */

    x = X509_new();
    if (!x) goto done;
    ASN1_INTEGER_set(X509_get_serialNumber(x), (long)time(NULL));
    X509_gmtime_adj(X509_get_notBefore(x), 0);
    X509_gmtime_adj(X509_get_notAfter(x), 31536000L); /* 1 year */
    X509_set_pubkey(x, pkey);

    X509_NAME *name = X509_get_subject_name(x);
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char*)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char*)"MiniTor", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)"localhost", -1, -1, 0);
    X509_set_issuer_name(x, name);

    if (!X509_sign(x, pkey, EVP_sha256())) goto done;

    *out_pkey = pkey; *out_x509 = x;
    pkey = NULL; x = NULL;
    rc = 1;

done:
    if (bn) BN_free(bn);
    if (rsa) RSA_free(rsa);
    if (pkey) EVP_PKEY_free(pkey);
    if (x) X509_free(x);
    return rc;
}

/* -------------------- TLS contexts/helpers -------------------- */

static SSL_CTX *create_server_ctx(EVP_PKEY *pkey, X509 *cert) {
    const SSL_METHOD *m = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(m);
    if (!ctx) return NULL;
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    if (SSL_CTX_use_certificate(ctx, cert) != 1) { SSL_CTX_free(ctx); return NULL; }
    if (SSL_CTX_use_PrivateKey(ctx, pkey) != 1) { SSL_CTX_free(ctx); return NULL; }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL); /* client cert not required */
    return ctx;
}

static SSL_CTX *create_client_ctx_with_store(X509_STORE *store) {
    const SSL_METHOD *m = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(m);
    if (!ctx) return NULL;
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    if (store) {
        /* SSL_CTX_set_cert_store takes ownership; we up_ref to keep caller's store */
        if (!X509_STORE_up_ref(store)) {
            logf("create_client_ctx_with_store: X509_STORE_up_ref failed");
        }
        SSL_CTX_set_cert_store(ctx, store);
    }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    return ctx;
}

/* tls_wrap: wrap connected TCP fd into SSL* (client side) */
static SSL *tls_wrap(int fd, SSL_CTX *ctx) {
    if (!ctx) return NULL;
    SSL *ssl = SSL_new(ctx);
    if (!ssl) { logf("tls_wrap: SSL_new failed"); return NULL; }
    SSL_set_fd(ssl, fd);
    if (SSL_connect(ssl) != 1) {
        int err = SSL_get_error(ssl, -1);
        logf("tls_wrap: SSL_connect failed (err %d)", err);
        unsigned long e;
        while ((e = ERR_get_error()) != 0) {
            char buf[256]; ERR_error_string_n(e, buf, sizeof(buf));
            logf("  OpenSSL: %s", buf);
        }
        SSL_free(ssl);
        return NULL;
    }
    return ssl;
}

/* tls_wrap_server: helper to accept server-side SSL on connected fd */
static SSL *tls_wrap_server(int fd, SSL_CTX *ctx) {
    if (!ctx) return NULL;
    SSL *ssl = SSL_new(ctx);
    if (!ssl) { logf("tls_wrap_server: SSL_new failed"); return NULL; }
    SSL_set_fd(ssl, fd);
    if (SSL_accept(ssl) != 1) {
        int err = SSL_get_error(ssl, -1);
        logf("tls_wrap_server: SSL_accept failed (err %d)", err);
        unsigned long e;
        while ((e = ERR_get_error()) != 0) {
            char buf[256]; ERR_error_string_n(e, buf, sizeof(buf));
            logf("  OpenSSL: %s", buf);
        }
        SSL_free(ssl);
        return NULL;
    }
    return ssl;
}

/* -------------------- Guard (server) -------------------- */
/* The guard accepts a TLS connection, reads a 4-byte len and payload (host\0port\n),
   connects to the target and proxies bidirectionally. */

typedef struct {
    int listen_fd;
    int listen_port;
    SSL_CTX *srv_ctx;
    EVP_PKEY *srv_pkey;
    X509 *srv_cert;
    pthread_t thread;
} guard_t;

static guard_t GUARD = {0};

static void *guard_accept_loop(void *arg) {
    (void)arg;
    while (1) {
        struct sockaddr_in peer; socklen_t plen = sizeof(peer);
        int cfd = accept(GUARD.listen_fd, (struct sockaddr*)&peer, &plen);
        if (cfd < 0) { if (errno==EINTR) continue; logf("guard accept error: %s", strerror(errno)); break; }
        SSL *ssl = tls_wrap_server(cfd, GUARD.srv_ctx);
        if (!ssl) { close(cfd); continue; }

        /* read 4-byte len (network order) */
        uint32_t nnet; int rr = SSL_read(ssl, &nnet, sizeof(nnet));
        if (rr != sizeof(nnet)) { logf("guard: read len failed"); SSL_shutdown(ssl); SSL_free(ssl); close(cfd); continue; }
        uint32_t n = ntohl(nnet);
        if (n == 0 || n > 65536) { logf("guard: bad len %u", n); SSL_shutdown(ssl); SSL_free(ssl); close(cfd); continue; }

        unsigned char *buf = malloc(n+1);
        size_t got = 0;
        while (got < n) {
            int r = SSL_read(ssl, (char*)buf + got, (int)(n - got));
            if (r <= 0) break;
            got += r;
        }
        if (got != n) { logf("guard: incomplete payload %zu/%u", got, n); free(buf); SSL_shutdown(ssl); SSL_free(ssl); close(cfd); continue; }
        buf[n] = 0;

        /* parse payload: host\0port\n */
        char host[256]; int port = 0;
        char *portptr = memchr(buf, 0, n);
        if (!portptr) { logf("guard: bad payload (no null)"); free(buf); SSL_shutdown(ssl); SSL_free(ssl); close(cfd); continue; }
        strncpy(host, (char*)buf, sizeof(host)-1);
        port = atoi(portptr + 1);

        logf("guard: connect to %s:%d", host, port);

        /* resolve and connect */
        char ip[64];
        int target_sock = -1;
        if (resolve_host_ipv4(host, ip)) target_sock = tcp_connect_ip(ip, port);
        else target_sock = tcp_connect_ip(host, port);
        if (target_sock < 0) { logf("guard: target connect failed"); free(buf); SSL_shutdown(ssl); SSL_free(ssl); close(cfd); continue; }

        /* proxy loop: ssl <-> target_sock */
        fd_set rfds;
        int maxfd = (target_sock > cfd ? target_sock : cfd) + 1;
        while (1) {
            FD_ZERO(&rfds);
            FD_SET(target_sock, &rfds);
            FD_SET(cfd, &rfds);
            int sel = select(maxfd, &rfds, NULL, NULL, NULL);
            if (sel <= 0) break;
            if (FD_ISSET(target_sock, &rfds)) {
                char tmp[4096];
                int r = recv(target_sock, tmp, sizeof(tmp), 0);
                if (r <= 0) break;
                int written = 0;
                while (written < r) {
                    int w = SSL_write(ssl, tmp + written, r - written);
                    if (w <= 0) goto guard_cleanup;
                    written += w;
                }
            }
            if (FD_ISSET(cfd, &rfds)) {
                char tmp[4096];
                int r = SSL_read(ssl, tmp, sizeof(tmp));
                if (r <= 0) break;
                int written = 0;
                while (written < r) {
                    int w = send(target_sock, tmp + written, r - written, 0);
                    if (w <= 0) goto guard_cleanup;
                    written += w;
                }
            }
        }

    guard_cleanup:
        close(target_sock);
        free(buf);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(cfd);
    }
    return NULL;
}

static int guard_init(int port) {
    EVP_PKEY *pkey = NULL; X509 *cert = NULL;
    if (!make_self_signed_cert(&pkey, &cert)) { logf("guard: cert gen failed"); return 0; }
    GUARD.srv_pkey = pkey; GUARD.srv_cert = cert;
    GUARD.srv_ctx = create_server_ctx(pkey, cert);
    if (!GUARD.srv_ctx) { logf("guard: create_server_ctx failed"); return 0; }

    int l = socket(AF_INET, SOCK_STREAM, 0);
    if (l < 0) { logf("guard: socket failed"); return 0; }
    int yes = 1; setsockopt(l, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in sa; memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET; sa.sin_port = htons(port); sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(l, (struct sockaddr*)&sa, sizeof(sa)) != 0) { logf("guard: bind failed: %s", strerror(errno)); close(l); return 0; }
    if (listen(l, 8) != 0) { logf("guard: listen failed"); close(l); return 0; }
    GUARD.listen_fd = l; GUARD.listen_port = port;

    if (pthread_create(&GUARD.thread, NULL, guard_accept_loop, NULL) != 0) {
        logf("guard: pthread_create failed"); close(l); return 0;
    }
    pthread_detach(GUARD.thread);
    logf("guard: listening on 127.0.0.1:%d", port);
    return 1;
}

/* -------------------- Client side forwarder -------------------- */
/* Forwarder creates TLS client to guard, sends length-prefixed target and proxies. */

struct forward_args {
    int client_fd;
    char target_host[256];
    int target_port;
    X509 *guard_cert;      /* pointer to guard cert (we trust this in-memory) */
    char guard_host[64];
    int guard_port;
};

static X509_STORE *build_trust_from_cert(X509 *cert) {
    X509_STORE *store = X509_STORE_new();
    if (!store) return NULL;
    if (X509_STORE_add_cert(store, cert) != 1) {
        /* still return store; OpenSSL may allow verify with it */
        logf("build_trust_from_cert: add_cert failed (maybe dup)");
    }
    return store;
}

static void *forward_thread(void *v) {
    struct forward_args *fa = (struct forward_args*)v;
    X509_STORE *trust = build_trust_from_cert(fa->guard_cert);
    if (!trust) { logf("forward: trust store build failed"); close(fa->client_fd); free(fa); return NULL; }
    SSL_CTX *cctx = create_client_ctx_with_store(trust);
    if (!cctx) { logf("forward: create client ctx failed"); X509_STORE_free(trust); close(fa->client_fd); free(fa); return NULL; }

    /* resolve guard host */
    char ip[64];
    if (!resolve_host_ipv4(fa->guard_host, ip)) strncpy(ip, fa->guard_host, sizeof(ip)-1);

    int sock = tcp_connect_ip(ip, fa->guard_port);
    if (sock < 0) { logf("forward: connect to guard failed"); SSL_CTX_free(cctx); X509_STORE_free(trust); close(fa->client_fd); free(fa); return NULL; }

    SSL *ssl = tls_wrap(sock, cctx);
    if (!ssl) { logf("forward: TLS connect failed"); close(sock); SSL_CTX_free(cctx); X509_STORE_free(trust); close(fa->client_fd); free(fa); return NULL; }

    long vr = SSL_get_verify_result(ssl);
    if (vr != X509_V_OK) { logf("forward: verify result not OK: %ld", vr); SSL_shutdown(ssl); SSL_free(ssl); close(sock); SSL_CTX_free(cctx); X509_STORE_free(trust); close(fa->client_fd); free(fa); return NULL; }

    /* send length-prefixed payload: host\0port\n */
    char payload[512];
    int plen = snprintf(payload, sizeof(payload), "%s%c%d\n", fa->target_host, '\0', fa->target_port);
    uint32_t nnet = htonl((uint32_t)plen);
    if (SSL_write(ssl, &nnet, sizeof(nnet)) <= 0) { logf("forward: send len failed"); goto cleanup; }
    if (SSL_write(ssl, payload, plen) <= 0) { logf("forward: send payload failed"); goto cleanup; }

    /* proxy loop: client_fd <-> ssl */
    int client = fa->client_fd;
    int guard_sock = sock;
    fd_set rfds;
    int maxfd = (client > guard_sock ? client : guard_sock) + 1;
    char buf[4096];

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(client, &rfds);
        FD_SET(guard_sock, &rfds);
        int sel = select(maxfd, &rfds, NULL, NULL, NULL);
        if (sel <= 0) break;
        if (FD_ISSET(client, &rfds)) {
            int r = recv(client, buf, sizeof(buf), 0);
            if (r <= 0) break;
            int written = 0;
            while (written < r) {
                int w = SSL_write(ssl, buf + written, r - written);
                if (w <= 0) goto cleanup;
                written += w;
            }
        }
        if (FD_ISSET(guard_sock, &rfds)) {
            int r = SSL_read(ssl, buf, sizeof(buf));
            if (r <= 0) break;
            int written = 0;
            while (written < r) {
                int w = send(client, buf + written, r - written, 0);
                if (w <= 0) goto cleanup;
                written += w;
            }
        }
    }

cleanup:
    SSL_shutdown(ssl); SSL_free(ssl); close(sock);
    SSL_CTX_free(cctx); X509_STORE_free(trust);
    close(client);
    free(fa);
    return NULL;
}

/* -------------------- SOCKS5 handling -------------------- */
/* minimal SOCKS5: NO AUTH, only CONNECT; supports domain & IPv4 */

static void *socks_client_handler(void *arg) {
    int client_fd = *(int*)arg; free(arg);
    unsigned char buf[512];
    int r = recv(client_fd, buf, sizeof(buf), 0);
    if (r <= 1) { close(client_fd); return NULL; }
    if (buf[0] != 0x05) { close(client_fd); return NULL; }
    unsigned char method_reply[2] = {0x05, 0x00}; /* no auth */
    send(client_fd, method_reply, 2, 0);

    r = recv(client_fd, buf, sizeof(buf), 0);
    if (r < 7) { close(client_fd); return NULL; }
    if (buf[0] != 0x05 || buf[1] != 0x01) { close(client_fd); return NULL; } /* only CONNECT */
    unsigned char atyp = buf[3];
    char target_host[256] = {0};
    int target_port = 0;
    if (atyp == 0x01) { /* IPv4 */
        if (r < 10) { close(client_fd); return NULL; }
        snprintf(target_host, sizeof(target_host), "%u.%u.%u.%u",
                 (unsigned char)buf[4], (unsigned char)buf[5], (unsigned char)buf[6], (unsigned char)buf[7]);
        target_port = ((unsigned char)buf[8] << 8) | (unsigned char)buf[9];
    } else if (atyp == 0x03) { /* domain */
        int len = (unsigned char)buf[4];
        if (r < 5 + len + 2) { close(client_fd); return NULL; }
        memcpy(target_host, &buf[5], len); target_host[len] = 0;
        target_port = ((unsigned char)buf[5+len] << 8) | (unsigned char)buf[5+len+1];
    } else {
        close(client_fd); return NULL; /* unsupported */
    }

    logf("SOCKS CONNECT to %s:%d", target_host, target_port);

    /* send success reply to client */
    unsigned char rep[10] = {0x05, 0x00, 0x00, 0x01, 0,0,0,0, 0,0};
    send(client_fd, rep, sizeof(rep), 0);

    /* prepare forward args and spawn forwarder */
    struct forward_args *fa = calloc(1, sizeof(*fa));
    if (!fa) { close(client_fd); return NULL; }
    fa->client_fd = client_fd;
    strncpy(fa->target_host, target_host, sizeof(fa->target_host)-1);
    fa->target_port = target_port;
    strncpy(fa->guard_host, "127.0.0.1", sizeof(fa->guard_host)-1);
    fa->guard_port = GUARD.listen_port;
    fa->guard_cert = GUARD.srv_cert; /* trust runtime guard cert */

    pthread_t th;
    pthread_create(&th, NULL, forward_thread, fa);
    pthread_detach(th);

    /* forward_thread owns client_fd now */
    return NULL;
}

static int start_socks_listener(int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;
    int yes = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in sa; memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET; sa.sin_port = htons(port); sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(s, (struct sockaddr*)&sa, sizeof(sa)) != 0) { close(s); return -1; }
    if (listen(s, 64) != 0) { close(s); return -1; }
    logf("SOCKS listening on 127.0.0.1:%d", port);
    while (1) {
        struct sockaddr_in peer; socklen_t plen = sizeof(peer);
        int c = accept(s, (struct sockaddr*)&peer, &plen);
        if (c < 0) { if (errno==EINTR) continue; logf("accept error: %s", strerror(errno)); break; }
        int *pfd = malloc(sizeof(int)); *pfd = c;
        pthread_t th; pthread_create(&th, NULL, socks_client_handler, pfd);
        pthread_detach(th);
    }
    close(s);
    return 0;
}

/* -------------------- MAIN -------------------- */

int main(int argc, char **argv) {
    int socks_port = 9050;
    int guard_port = 7001;
    if (argc > 1) socks_port = atoi(argv[1]);
    if (argc > 2) guard_port = atoi(argv[2]);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_crypto_strings();

    if (!guard_init(guard_port)) {
        logf("failed to init guard");
        return 1;
    }

    /* now start socks listener (blocking) */
    start_socks_listener(socks_port);

    return 0;
}

