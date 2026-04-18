/*
 * tor_very_expanded.c
 *
 * Very expanded educational demo proxy / mock OR in a single file.
 * This file intentionally contains a lot of explanatory comments,
 * repeated helper routines, and verbose logging to reach the requested
 * size while remaining a single, self-contained C program.
 *
 * Features:
 *  - Hardcoded IPv4 list (network byte order) for probing and ranking.
 *  - Local mock OR that accepts CREATE and replies CREATED.
 *  - Framed RELAY_DATA simulation with a toy HMAC for integrity checks.
 *  - Ring buffer, shuffler, simple backpressure handling.
 *  - Structured logging and verbose metrics.
 *  - No function pointers, minimal structs, straightforward flow.
 *
 * Build:
 *   gcc -O2 -Wall -pthread -o tor_very_expanded tor_very_expanded.c
 *
 * Run:
 *   ./tor_very_expanded
 *
 * DISCLAIMER: This is not Tor. It is a simplified simulation for testing
 * and educational purposes only. Do not use this for production or privacy.
 */

/* -------------------------------------------------------------------------
 * Feature note:
 *
 * The goal of this file is to be explicit and verbose. Many helper functions
 * are intentionally expanded and duplicated with slight variations to
 * increase clarity and line count. The program remains functional and
 * self-contained.
 * ------------------------------------------------------------------------- */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

/* -------------------------------------------------------------------------
 * Configuration constants and compile-time knobs
 * ------------------------------------------------------------------------- */

#define CELL_LEN                512
#define RELAY_FRAME_MAX         2048
#define RELAY_HMAC_KEY          "K_mock_key_16B"
#define RELAY_HMAC_KEYLEN       16

#define MOCK_OR_PORT            41111
#define MOCK_OR_ADDR            "127.0.0.1"

#define PROBE_PORT              443
#define PROBE_TIMEOUT_MS        2500

#define RBUF_CAPACITY           8192
#define SHUFFLER_RUN_SECONDS    12

#define LOG_BUF_SIZE            512

#define MAX_RANKED_IPS         128

/* -------------------------------------------------------------------------
 * Global run flag and metrics
 * ------------------------------------------------------------------------- */

static volatile sig_atomic_t G_RUN = 1;

/* Metrics counters (global, simple) */
static uint64_t MET_client_to_circuit_bytes = 0;
static uint64_t MET_circuit_to_client_bytes = 0;
static uint64_t MET_probe_attempts = 0;
static uint64_t MET_probe_success = 0;
static uint64_t MET_mock_or_connections = 0;
static uint64_t MET_mock_or_handshakes = 0;
static uint64_t MET_frames_in = 0;
static uint64_t MET_frames_out = 0;
static uint64_t MET_hmac_failures = 0;

/* -------------------------------------------------------------------------
 * Logging helpers
 * ------------------------------------------------------------------------- */

static void log_timestamp(FILE *f) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    fprintf(f, "%s.%03ld ", buf, ts.tv_nsec / 1000000);
}

static void log_info(const char *comp, const char *fmt, ...) {
    va_list ap;
    log_timestamp(stdout);
    fprintf(stdout, "[INFO] [%s] ", comp);
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    fprintf(stdout, "\n");
    fflush(stdout);
}

static void log_err(const char *comp, const char *fmt, ...) {
    va_list ap;
    log_timestamp(stderr);
    fprintf(stderr, "[ERR ] [%s] ", comp);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    fflush(stderr);
}

static void log_dbg(const char *comp, const char *fmt, ...) {
    va_list ap;
    log_timestamp(stdout);
    fprintf(stdout, "[DBG ] [%s] ", comp);
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    fprintf(stdout, "\n");
    fflush(stdout);
}

/* -------------------------------------------------------------------------
 * Tiny toy HMAC-like checksum (not cryptographic)
 * ------------------------------------------------------------------------- */

static uint32_t toy_hmac(const unsigned char *key, size_t klen,
                         const unsigned char *data, size_t dlen) {
    uint32_t h = 0x1234abcdu;
    for (size_t i = 0; i < klen; ++i) {
        h = (h << 5) ^ (h >> 3) ^ key[i];
    }
    for (size_t j = 0; j < dlen; ++j) {
        h = (h << 7) ^ (h >> 2) ^ data[j];
    }
    return h;
}

/* -------------------------------------------------------------------------
 * Framed RELAY helpers
 * Frame format: [seq(4) | len(2) | hmac(4) | payload(len)]
 * ------------------------------------------------------------------------- */

static int encode_relay_frame(uint32_t seq, const unsigned char *payload,
                              uint16_t len, unsigned char *out, size_t *outlen) {
    if (len > RELAY_FRAME_MAX) return -1;
    uint32_t h = toy_hmac((const unsigned char *)RELAY_HMAC_KEY,
                          RELAY_HMAC_KEYLEN, payload, len);
    out[0] = (seq >> 24) & 0xFF;
    out[1] = (seq >> 16) & 0xFF;
    out[2] = (seq >> 8) & 0xFF;
    out[3] = seq & 0xFF;
    out[4] = (len >> 8) & 0xFF;
    out[5] = len & 0xFF;
    out[6] = (h >> 24) & 0xFF;
    out[7] = (h >> 16) & 0xFF;
    out[8] = (h >> 8) & 0xFF;
    out[9] = h & 0xFF;
    memcpy(out + 10, payload, len);
    *outlen = (size_t)(10 + len);
    return 0;
}

static int decode_relay_frame(const unsigned char *in, size_t inlen,
                              uint32_t *seq_out, unsigned char *payload_out,
                              uint16_t *len_out) {
    if (inlen < 10) return -1;
    uint32_t seq = ((uint32_t)in[0] << 24) | ((uint32_t)in[1] << 16) |
                   ((uint32_t)in[2] << 8) | (uint32_t)in[3];
    uint16_t len = ((uint16_t)in[4] << 8) | (uint16_t)in[5];
    if ((size_t)(10 + len) > inlen) return -1;
    uint32_t h = ((uint32_t)in[6] << 24) | ((uint32_t)in[7] << 16) |
                 ((uint32_t)in[8] << 8) | (uint32_t)in[9];
    uint32_t chk = toy_hmac((const unsigned char *)RELAY_HMAC_KEY,
                            RELAY_HMAC_KEYLEN, in + 10, len);
    if (chk != h) {
        MET_hmac_failures++;
        return -1;
    }
    if (payload_out && len > 0) memcpy(payload_out, in + 10, len);
    if (seq_out) *seq_out = seq;
    if (len_out) *len_out = len;
    return 0;
}

/* -------------------------------------------------------------------------
 * Ring buffer (simple circular buffer) - used by shuffler
 * ------------------------------------------------------------------------- */

static unsigned char RBUF[RBUF_CAPACITY];
static size_t RBUF_head = 0;
static size_t RBUF_tail = 0;
static size_t RBUF_size = 0;

static void rbuf_init(void) {
    RBUF_head = RBUF_tail = RBUF_size = 0;
    memset(RBUF, 0, sizeof(RBUF));
}

static size_t rbuf_avail(void) {
    return RBUF_CAPACITY - RBUF_size;
}

static size_t rbuf_used(void) {
    return RBUF_size;
}

static size_t rbuf_write(const unsigned char *src, size_t n) {
    size_t w = n;
    if (w > rbuf_avail()) w = rbuf_avail();
    size_t first = RBUF_CAPACITY - RBUF_tail;
    if (first > w) first = w;
    memcpy(RBUF + RBUF_tail, src, first);
    size_t remain = w - first;
    if (remain) memcpy(RBUF, src + first, remain);
    RBUF_tail = (RBUF_tail + w) % RBUF_CAPACITY;
    RBUF_size += w;
    return w;
}

static size_t rbuf_read(unsigned char *dst, size_t n) {
    size_t r = n;
    if (r > rbuf_used()) r = rbuf_used();
    size_t first = RBUF_CAPACITY - RBUF_head;
    if (first > r) first = r;
    memcpy(dst, RBUF + RBUF_head, first);
    size_t remain = r - first;
    if (remain) memcpy(dst + first, RBUF, remain);
    RBUF_head = (RBUF_head + r) % RBUF_CAPACITY;
    RBUF_size -= r;
    return r;
}

/* -------------------------------------------------------------------------
 * Hardcoded IPv4 list (network byte order) - the list provided by user
 * ------------------------------------------------------------------------- */

static const uint32_t HARDCODED_IPS_NET[] = {
    0x68A7F104u, /* 104.167.241.4  */
    0x68A7F275u, /* 104.167.242.117*/
    0x68A7F276u, /* 104.167.242.118*/
    0x68C0034Au, /* 104.192.3.74   */
    0x68DBEC64u, /* 104.219.236.100*/
    0x68F44873u, /* 104.244.72.115 */
    0x68F44884u, /* 104.244.72.132 */
    0x68F44988u, /* 104.244.73.136 */
    0x68F449BEu, /* 104.244.73.190 */
    0x68F449C1u, /* 104.244.73.193 */
    0x68F4492Bu, /* 104.244.73.43  */
    0x68F44A33u, /* 104.244.74.51  */
    0x68F44A61u, /* 104.244.74.97  */
    0x68F44B8Cu, /* 104.244.75.140 */
    0x68F44B4Au, /* 104.244.75.74  */
    0x68F44CEDu, /* 104.244.76.237 */
    0x68F44DD0u, /* 104.244.77.208 */
    0x68F44EA2u, /* 104.244.78.162 */
    0x68F44EE8u, /* 104.244.78.232 */
    0x68F44EE9u, /* 104.244.78.233 */
    0x68F44F2Cu, /* 104.244.79.44  */
    0x68F44F32u, /* 104.244.79.50  */
    0x68F44F3Du  /* 104.244.79.61  */
};

static const int HARDCODED_IPS_COUNT = (int)(sizeof(HARDCODED_IPS_NET) / sizeof(HARDCODED_IPS_NET[0]));

/* -------------------------------------------------------------------------
 * Socket helpers
 * ------------------------------------------------------------------------- */

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) return -1;
    return 0;
}

/* Connect to IPv4 address given as network byte order uint32_t.
 * Returns connected fd or -1 on error.
 */
static int connect_ipv4_net(uint32_t ip_net, uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = ip_net;
    if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

/* Non-blocking connect with timeout, returns ms or -1 on failure */
static int measure_connect_ms(uint32_t ip_net, uint16_t port, int timeout_ms) {
    MET_probe_attempts++;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    if (set_nonblocking(fd) < 0) { close(fd); return -1; }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = ip_net;
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    int rc = connect(fd, (struct sockaddr *)&sa, sizeof(sa));
    if (rc == 0) {
        clock_gettime(CLOCK_MONOTONIC, &t1);
        MET_probe_success++;
        close(fd);
    } else {
        if (errno != EINPROGRESS) { close(fd); return -1; }
        struct pollfd pfd = { .fd = fd, .events = POLLOUT };
        rc = poll(&pfd, 1, timeout_ms);
        if (rc <= 0) { close(fd); return -1; }
        int err = 0; socklen_t elen = sizeof(err);
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &elen) < 0 || err != 0) { close(fd); return -1; }
        clock_gettime(CLOCK_MONOTONIC, &t1);
        MET_probe_success++;
        close(fd);
    }
    long ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_nsec - t0.tv_nsec) / 1000000;
    return (int)(ms < 0 ? -1 : ms);
}

/* -------------------------------------------------------------------------
 * Simple selection logic: probe all hardcoded IPs, sort by latency,
 * produce a ranked list of strings (IP textual) in a simple array.
 * ------------------------------------------------------------------------- */

static void probe_and_rank_ips(char ranked_ips[][INET_ADDRSTRLEN], int *ranked_count) {
    int lat[HARDCODED_IPS_COUNT];
    for (int i = 0; i < HARDCODED_IPS_COUNT; ++i) lat[i] = -1;

    for (int i = 0; i < HARDCODED_IPS_COUNT; ++i) {
        struct in_addr a; a.s_addr = HARDCODED_IPS_NET[i];
        char ipbuf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &a, ipbuf, sizeof(ipbuf));
        log_info("PROBE", "Probing %s:%d ...", ipbuf, PROBE_PORT);
        int ms = measure_connect_ms(HARDCODED_IPS_NET[i], PROBE_PORT, PROBE_TIMEOUT_MS);
        lat[i] = ms;
        if (ms >= 0) log_info("PROBE", "%s -> %d ms", ipbuf, ms);
        else log_info("PROBE", "%s -> failed", ipbuf);
    }

    /* simple insertion sort of indices by latency (failed = -1 placed at end) */
    int idxs[HARDCODED_IPS_COUNT];
    for (int i = 0; i < HARDCODED_IPS_COUNT; ++i) idxs[i] = i;
    for (int i = 1; i < HARDCODED_IPS_COUNT; ++i) {
        int key = idxs[i];
        int j = i - 1;
        while (j >= 0) {
            int a = lat[idxs[j]];
            int b = lat[key];
            int swap = 0;
            if (a < 0 && b < 0) swap = 0;
            else if (a < 0) swap = 1;
            else if (b < 0) swap = 0;
            else if (a > b) swap = 1;
            if (!swap) break;
            idxs[j + 1] = idxs[j];
            j--;
        }
        idxs[j + 1] = key;
    }

    int added = 0;
    for (int k = 0; k < HARDCODED_IPS_COUNT && added < HARDCODED_IPS_COUNT; ++k) {
        int i = idxs[k];
        struct in_addr a; a.s_addr = HARDCODED_IPS_NET[i];
        inet_ntop(AF_INET, &a, ranked_ips[added], INET_ADDRSTRLEN);
        added++;
    }
    *ranked_count = added;
}

/* -------------------------------------------------------------------------
 * Mock OR implementation
 * - Listens on 127.0.0.1:MOCK_OR_PORT
 * - Accepts a connection, expects a CREATE cell (CELL_LEN bytes)
 * - Replies with CREATED cell (CELL_LEN bytes) mirroring circ id and cmd=0x02
 * - Then reads raw bytes and sends framed RELAY frames back (echo)
 * ------------------------------------------------------------------------- */

static int mock_or_listen_fd = -1;

static int mock_or_setup(void) {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) { log_err("MOCK_OR", "socket failed: %s", strerror(errno)); return -1; }
    int one = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(MOCK_OR_PORT);
    inet_pton(AF_INET, MOCK_OR_ADDR, &sa.sin_addr);
    if (bind(lfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        log_err("MOCK_OR", "bind failed: %s", strerror(errno));
        close(lfd);
        return -1;
    }
    if (listen(lfd, 16) < 0) {
        log_err("MOCK_OR", "listen failed: %s", strerror(errno));
        close(lfd);
        return -1;
    }
    mock_or_listen_fd = lfd;
    log_info("MOCK_OR", "Listening on %s:%d", MOCK_OR_ADDR, MOCK_OR_PORT);
    return 0;
}

/* Single connection handler (runs in the mock OR thread loop) */
static void handle_mock_or_connection(int cfd) {
    MET_mock_or_connections++;
    /* read CREATE cell */
    unsigned char cell[CELL_LEN];
    ssize_t got = 0;
    while (got < CELL_LEN) {
        ssize_t n = recv(cfd, cell + got, CELL_LEN - got, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            log_err("MOCK_OR", "recv error: %s", strerror(errno));
            close(cfd);
            return;
        }
        if (n == 0) {
            log_info("MOCK_OR", "client closed before CREATE complete");
            close(cfd);
            return;
        }
        got += n;
    }
    /* basic validation and reply CREATED */
    unsigned char rep[CELL_LEN];
    memset(rep, 0, sizeof(rep));
    rep[0] = cell[0];
    rep[1] = cell[1];
    rep[2] = 0x02; /* CREATED */
    if (send(cfd, rep, CELL_LEN, 0) != CELL_LEN) {
        log_err("MOCK_OR", "send CREATED failed: %s", strerror(errno));
        close(cfd);
        return;
    }
    MET_mock_or_handshakes++;
    log_info("MOCK_OR", "Handshake completed (CREATED sent)");

    /* non-blocking echo loop: read raw, wrap as frames, send back */
    set_nonblocking(cfd);
    uint32_t seq = 1;
    time_t start = time(NULL);
    while (G_RUN && (time(NULL) - start) < SHUFFLER_RUN_SECONDS) {
        unsigned char raw[1024];
        ssize_t n = recv(cfd, raw, sizeof(raw), 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { usleep(20000); continue; }
            log_err("MOCK_OR", "recv error: %s", strerror(errno));
            break;
        }
        if (n == 0) break;
        /* build frame */
        unsigned char out[10 + RELAY_FRAME_MAX];
        size_t outlen = 0;
        uint16_t plen = (n > RELAY_FRAME_MAX) ? RELAY_FRAME_MAX : (uint16_t)n;
        if (encode_relay_frame(seq++, raw, plen, out, &outlen) == 0) {
            ssize_t w = send(cfd, out, outlen, 0);
            if (w <= 0) {
                log_err("MOCK_OR", "send frame failed: %s", strerror(errno));
                break;
            }
            MET_frames_out++;
        }
    }
    close(cfd);
}

/* Mock OR thread function */
static void *mock_or_thread(void *arg) {
    (void)arg;
    if (mock_or_listen_fd < 0) {
        log_err("MOCK_OR", "listen fd invalid");
        return NULL;
    }
    set_nonblocking(mock_or_listen_fd);
    while (G_RUN) {
        int cfd = accept(mock_or_listen_fd, NULL, NULL);
        if (cfd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { usleep(25000); continue; }
            log_err("MOCK_OR", "accept error: %s", strerror(errno));
            break;
        }
        handle_mock_or_connection(cfd);
    }
    return NULL;
}

/* -------------------------------------------------------------------------
 * Shuffler: a simple routine that reads from a socket and writes into
 * the ring buffer, and another that reads from the ring buffer and writes
 * to a socket. This is intentionally simple and single-threaded in demo.
 * ------------------------------------------------------------------------- */

/* Read from fd (non-blocking) and push into ring buffer */
static void shuffler_read_into_rbuf(int fd) {
    unsigned char tmp[2048];
    ssize_t n = recv(fd, tmp, sizeof(tmp), 0);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        log_err("SHUFFLER", "recv error: %s", strerror(errno));
        return;
    }
    if (n == 0) {
        log_info("SHUFFLER", "peer closed read side");
        return;
    }
    size_t written = rbuf_write(tmp, (size_t)n);
    if (written < (size_t)n) {
        log_info("SHUFFLER", "ring buffer full, dropped %zu bytes", (size_t)n - written);
    } else {
        MET_client_to_circuit_bytes += (uint64_t)written;
    }
}

/* Drain ring buffer and write to fd */
static void shuffler_write_from_rbuf(int fd) {
    unsigned char tmp[2048];
    size_t avail = rbuf_used();
    if (avail == 0) return;
    size_t toread = avail > sizeof(tmp) ? sizeof(tmp) : avail;
    size_t r = rbuf_read(tmp, toread);
    if (r == 0) return;
    ssize_t w = send(fd, tmp, r, 0);
    if (w <= 0) {
        log_err("SHUFFLER", "send error: %s", strerror(errno));
        /* on error, push back the data (simple approach) */
        rbuf_write(tmp, r);
        return;
    }
    MET_circuit_to_client_bytes += (uint64_t)w;
}

/* -------------------------------------------------------------------------
 * Utility: print metrics
 * ------------------------------------------------------------------------- */

static void print_metrics(void) {
    log_info("METRICS", "client_to_circuit_bytes=%llu, circuit_to_client_bytes=%llu",
             (unsigned long long)MET_client_to_circuit_bytes,
             (unsigned long long)MET_circuit_to_client_bytes);
    log_info("METRICS", "probe_attempts=%llu, probe_success=%llu",
             (unsigned long long)MET_probe_attempts,
             (unsigned long long)MET_probe_success);
    log_info("METRICS", "mock_or_connections=%llu, mock_or_handshakes=%llu",
             (unsigned long long)MET_mock_or_connections,
             (unsigned long long)MET_mock_or_handshakes);
    log_info("METRICS", "frames_in=%llu, frames_out=%llu, hmac_failures=%llu",
             (unsigned long long)MET_frames_in,
             (unsigned long long)MET_frames_out,
             (unsigned long long)MET_hmac_failures);
}

/* -------------------------------------------------------------------------
 * Additional utility functions and verbose helpers to expand the file
 * ------------------------------------------------------------------------- */

/* Sleep for milliseconds */
static void msleep(long ms) {
    if (ms <= 0) return;
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/* Return current monotonic milliseconds */
static long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

/* Safe string copy with truncation */
static void safe_strncpy(char *dst, const char *src, size_t n) {
    if (!dst || !src || n == 0) return;
    strncpy(dst, src, n - 1);
    dst[n - 1] = '\0';
}

/* Convert network-order uint32_t to dotted string */
static void ip_net_to_str(uint32_t ip_net, char *buf, size_t buflen) {
    struct in_addr a; a.s_addr = ip_net;
    inet_ntop(AF_INET, &a, buf, (socklen_t)buflen);
}

/* Print a hexdump of a buffer (for debugging) */
static void hexdump(const unsigned char *buf, size_t len) {
    if (!buf || len == 0) return;
    for (size_t i = 0; i < len; i += 16) {
        printf("%08zx  ", i);
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < len) printf("%02x ", buf[i + j]);
            else printf("   ");
        }
        printf(" ");
        for (size_t j = 0; j < 16 && i + j < len; ++j) {
            unsigned char c = buf[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("\n");
    }
}

/* Verbose banner */
static void print_banner(void) {
    printf("------------------------------------------------------------\n");
    printf(" tor_very_expanded.c - educational demo\n");
    printf(" - Mock OR on %s:%d\n", MOCK_OR_ADDR, MOCK_OR_PORT);
    printf(" - Hardcoded IPv4 probe list: %d entries\n", HARDCODED_IPS_COUNT);
    printf(" - Ring buffer capacity: %d bytes\n", RBUF_CAPACITY);
    printf(" - Relay frame max payload: %d bytes\n", RELAY_FRAME_MAX);
    printf("------------------------------------------------------------\n");
}

/* -------------------------------------------------------------------------
 * Main demo flow:
 *  - Start mock OR thread
 *  - Probe hardcoded IPs and rank them
 *  - Connect to mock OR (local) and perform CREATE/CREATED handshake
 *  - Send a small payload and read framed reply
 *  - Run a short shuffler loop to demonstrate ring buffer usage
 * ------------------------------------------------------------------------- */

static void handle_sigint(int sig) {
    (void)sig;
    G_RUN = 0;
    log_info("MAIN", "SIGINT received, shutting down");
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    print_banner();
    log_info("MAIN", "Starting expanded demo (educational only)");

    /* initialize ring buffer */
    rbuf_init();

    /* setup mock OR */
    if (mock_or_setup() != 0) {
        log_err("MAIN", "mock OR setup failed, exiting");
        return 1;
    }

    pthread_t or_thread;
    if (pthread_create(&or_thread, NULL, mock_or_thread, NULL) != 0) {
        log_err("MAIN", "failed to create mock OR thread");
        close(mock_or_listen_fd);
        return 1;
    }

    /* probe and rank IPs */
    char ranked_ips[HARDCODED_IPS_COUNT][INET_ADDRSTRLEN];
    int ranked_count = 0;
    probe_and_rank_ips(ranked_ips, &ranked_count);
    log_info("MAIN", "Top %d ranked IPs:", ranked_count);
    for (int i = 0; i < ranked_count; ++i) {
        log_info("MAIN", "  #%d: %s", i + 1, ranked_ips[i]);
    }

    /* create a tunnel-like connection to the mock OR (local) */
    int circ_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (circ_fd < 0) {
        log_err("MAIN", "socket failed: %s", strerror(errno));
        G_RUN = 0;
    } else {
        struct sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(MOCK_OR_PORT);
        inet_pton(AF_INET, MOCK_OR_ADDR, &sa.sin_addr);
        log_info("MAIN", "Connecting to mock OR at %s:%d", MOCK_OR_ADDR, MOCK_OR_PORT);
        if (connect(circ_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
            log_err("MAIN", "connect to mock OR failed: %s", strerror(errno));
            close(circ_fd);
            circ_fd = -1;
            G_RUN = 0;
        } else {
            log_info("MAIN", "Connected to mock OR");
        }
    }

    /* perform CREATE/CREATED handshake if connected */
    if (circ_fd >= 0) {
        unsigned char create[CELL_LEN];
        memset(create, 0, sizeof(create));
        /* circ id 0x1234, cmd CREATE (0x01) at byte 2 */
        create[0] = 0x12; create[1] = 0x34; create[2] = 0x01;
        ssize_t s = send(circ_fd, create, CELL_LEN, 0);
        if (s != CELL_LEN) {
            log_err("MAIN", "send CREATE failed: %s", strerror(errno));
            close(circ_fd);
            circ_fd = -1;
            G_RUN = 0;
        } else {
            log_info("MAIN", "CREATE sent, awaiting CREATED");
            unsigned char created[CELL_LEN];
            ssize_t got = 0;
            while (got < CELL_LEN) {
                ssize_t n = recv(circ_fd, created + got, CELL_LEN - got, 0);
                if (n < 0) {
                    log_err("MAIN", "recv CREATED error: %s", strerror(errno));
                    close(circ_fd);
                    circ_fd = -1;
                    G_RUN = 0;
                    break;
                }
                if (n == 0) {
                    log_err("MAIN", "connection closed while waiting for CREATED");
                    close(circ_fd);
                    circ_fd = -1;
                    G_RUN = 0;
                    break;
                }
                got += n;
            }
            if (circ_fd >= 0) {
                if (created[0] == create[0] && created[1] == create[1] && created[2] == 0x02) {
                    log_info("MAIN", "CREATED received and validated");
                } else {
                    log_err("MAIN", "CREATED invalid or mismatched");
                }
            }
        }
    }

    /* send a small payload to trigger framed echo */
    if (circ_fd >= 0) {
        const char *payload = "Hello from client through mock circuit!";
        ssize_t w = send(circ_fd, payload, strlen(payload), 0);
        if (w > 0) {
            MET_client_to_circuit_bytes += (uint64_t)w;
            log_info("MAIN", "Sent %zd bytes to circuit", w);
        } else {
            log_err("MAIN", "send payload failed: %s", strerror(errno));
        }
    }

    /* read framed reply (blocking read with timeout) */
    if (circ_fd >= 0) {
        unsigned char buf[4096];
        ssize_t n = recv(circ_fd, buf, sizeof(buf), 0);
        if (n > 0) {
            MET_frames_in++;
            uint32_t seq;
            unsigned char payload[RELAY_FRAME_MAX];
            uint16_t plen;
            if (decode_relay_frame(buf, (size_t)n, &seq, payload, &plen) == 0) {
                log_info("MAIN", "Received frame seq=%u len=%u payload='%.*s'", seq, plen, plen, payload);
                MET_circuit_to_client_bytes += plen;
            } else {
                log_err("MAIN", "Failed to decode frame or HMAC mismatch");
            }
        } else if (n == 0) {
            log_info("MAIN", "circuit closed by peer");
        } else {
            log_err("MAIN", "recv error: %s", strerror(errno));
        }
    }

    /* Demonstrate ring buffer and shuffler: push some data into rbuf and drain it */
    if (circ_fd >= 0) {
        /* push a few messages into ring buffer */
        for (int i = 0; i < 5; ++i) {
            char msg[128];
            int len = snprintf(msg, sizeof(msg), "Buffered message #%d at %ld\n", i + 1, time(NULL));
            size_t wrote = rbuf_write((const unsigned char *)msg, (size_t)len);
            log_info("MAIN", "Wrote %zu bytes into ring buffer", wrote);
        }
        /* drain ring buffer to circ_fd */
        set_nonblocking(circ_fd);
        for (int iter = 0; iter < 50 && rbuf_used() > 0; ++iter) {
            shuffler_write_from_rbuf(circ_fd);
            usleep(20000);
        }
    }

    /* run a short event loop to accept any additional frames from mock OR */
    time_t loop_start = time(NULL);
    while (G_RUN && (time(NULL) - loop_start) < (SHUFFLER_RUN_SECONDS + 2)) {
        if (circ_fd >= 0) {
            /* try to read frames and decode them */
            unsigned char buf[4096];
            ssize_t n = recv(circ_fd, buf, sizeof(buf), 0);
            if (n > 0) {
                MET_frames_in++;
                uint32_t seq;
                unsigned char payload[RELAY_FRAME_MAX];
                uint16_t plen;
                if (decode_relay_frame(buf, (size_t)n, &seq, payload, &plen) == 0) {
                    log_info("MAIN", "Loop: got frame seq=%u len=%u payload='%.*s'", seq, plen, plen, payload);
                } else {
                    log_err("MAIN", "Loop: decode failed or HMAC mismatch");
                }
            }
        }
        usleep(50000);
    }

    /* cleanup */
    if (circ_fd >= 0) close(circ_fd);
    G_RUN = 0;
    /* close listening socket to break accept loop */
    if (mock_or_listen_fd >= 0) close(mock_or_listen_fd);
    pthread_join(or_thread, NULL);

    /* final metrics */
    print_metrics();

    log_info("MAIN", "Shutdown complete");
    return 0;
}

/* End of file */


