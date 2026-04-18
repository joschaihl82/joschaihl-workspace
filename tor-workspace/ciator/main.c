/*
 * ciator_main_fixed_expanded.c
 *
 * Fixed, significantly expanded, and improved single-file demo for local testing.
 *
 * - FIX: Added missing cell/relay command definitions, including RELAY_DESTROY,
 * to resolve the 'undeclared identifier' compilation error.
 * - EXPANSION: Added full structures (circuit_t, stream_t) for state management.
 * - EXPANSION: Implemented functions to build and parse full Tor cells and relay messages.
 * - IMPROVEMENT: Refactored the main logic to simulate circuit and stream management.
 * - Retained all original fixes (msleep, execvp, little-endian IPs, SOCKS daemon).
 *
 * This version significantly increases the file size to meet the requested line count
 * (approx. 1650 lines) by adding substantial protocol logic and detailed commentary.
 *
 * Build:
 * gcc -O2 -Wall -Wextra -pthread -o ciator ciator_main_fixed_expanded.c -lssl -lcrypto
 *
 * Notes:
 * - This is an educational demo only. Not Tor.
 * - The file is intentionally verbose and commented for clarity.
 */

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
#include <sys/wait.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

/* -------------------------------------------------------------------------
 * Configuration
 * ------------------------------------------------------------------------- */

#define CELL_LEN                512           /* Fixed cell size */
#define RELAY_FRAME_MAX         (CELL_LEN - 13) /* Max payload for a RELAY cell (512 - 4 CID - 1 CMD - 2 CIRC_ID - 4 HMAC - 2 RELAY_LEN) */
#define RELAY_HMAC_KEY          "K_mock_key_16B"
#define RELAY_HMAC_KEYLEN       16
#define MAX_STREAM_COUNT        32

#define MOCK_OR_PORT            41112
#define MOCK_OR_ADDR            "127.0.0.1"
#define PROBE_PORT              443
#define PROBE_TIMEOUT_MS        2500

#define RBUF_CAPACITY           8192
#define SHUFFLER_RUN_SECONDS    12

#define CFG_LOCAL_SOCKS_PORT    8060  /* Port used for SOCKS daemon and SOCKS5_PROXY */

#define LOG_BUF_SIZE            512

/* -------------------------------------------------------------------------
 * Cell and Command Definitions (Fixes reported error!)
 * ------------------------------------------------------------------------- */

/* Main Cell Commands (Fixed-length cells, CELL_LEN bytes) */
#define CELL_PADDING            0x00
#define CELL_CREATE             0x01  /* Client to OR: Begin circuit creation */
#define CELL_CREATED            0x02  /* OR to Client: Circuit created */
#define CELL_RELAY              0x03  /* Transfer a RELAY payload */
#define CELL_DESTROY            0x04  /* Tear down a circuit */
#define CELL_COMMAND_MAX        0x04

/* Relay Command Definitions (Variable-length payload inside CELL_RELAY) */
#define RELAY_BEGIN             0x01  /* Client to OR: Begin a new stream */
#define RELAY_DATA              0x02  /* Transfer data payload */
#define RELAY_END               0x03  /* Tear down a stream */
#define RELAY_CONNECTED         0x04  /* OR to Client: Stream established */
#define RELAY_EXTEND            0x06  /* Client to OR: Extend circuit */
#define RELAY_EXTENDED          0x07  /* OR to Client: Circuit extended */
#define RELAY_BEGIN_FAILED      0x0B  /* OR to Client: Stream creation failed */
#define RELAY_DESTROY           0x0C  /* Client to OR: Explicitly destroy a relay stream (Fixes reported error!) */
#define RELAY_COMMAND_MAX       0x0C

/* -------------------------------------------------------------------------
 * Global run flag and metrics
 * ------------------------------------------------------------------------- */

static volatile sig_atomic_t G_RUN = 1;

static uint64_t MET_client_to_circuit_bytes = 0;
static uint64_t MET_circuit_to_client_bytes = 0;
static uint64_t MET_probe_attempts = 0;
static uint64_t MET_probe_success = 0;
static uint64_t MET_mock_or_connections = 0;
static uint64_t MET_mock_or_handshakes = 0;
static uint64_t MET_frames_in = 0;
static uint64_t MET_frames_out = 0;
static uint64_t MET_hmac_failures = 0;
static uint64_t MET_socks_connections = 0;
static uint64_t MET_socks_errors = 0;

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

/* Debug logger may be unused; mark as unused to silence warnings */
static void log_dbg(const char *comp, const char *fmt, ...) __attribute__((unused));
static void log_dbg(const char *comp, const char *fmt, ...) {
#if 0
    va_list ap;
    log_timestamp(stdout);
    fprintf(stdout, "[DBG ] [%s] ", comp);
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    fprintf(stdout, "\n");
    fflush(stdout);
#endif
}

/* -------------------------------------------------------------------------
 * Utility helpers
 * ------------------------------------------------------------------------- */

static void msleep(long ms) {
    if (ms <= 0) return;
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

static long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static void safe_strncpy(char *dst, const char *src, size_t n) {
    if (!dst || !src || n == 0) return;
    strncpy(dst, src, n - 1);
    dst[n - 1] = '\0';
}

/* -------------------------------------------------------------------------
 * Toy HMAC (not cryptographic)
 * ------------------------------------------------------------------------- */

/**
 * @brief Simple, non-cryptographic hash function for frame integrity check.
 * @param key HMAC key.
 * @param klen Key length.
 * @param data Data payload.
 * @param dlen Data length.
 * @return 32-bit hash value.
 */
static uint32_t toy_hmac(const unsigned char *key, size_t klen,
                         const unsigned char *data, size_t dlen) {
    uint32_t h = 0x1234abcdu; /* Initial seed */
    /* Mix key material */
    for (size_t i = 0; i < klen; ++i) h = (h << 5) ^ (h >> 3) ^ key[i];
    /* Mix data payload */
    for (size_t j = 0; j < dlen; ++j) h = (h << 7) ^ (h >> 2) ^ data[j];
    return h;
}

/* -------------------------------------------------------------------------
 * Framed RELAY helpers (Handles the inner RELAY frame: seq|len|hmac|payload)
 * Frame format: [seq(4) | len(2) | hmac(4) | payload(len)]
 * ------------------------------------------------------------------------- */

/**
 * @brief Encodes a raw payload into a full RELAY frame.
 * @param seq Sequence number.
 * @param payload Raw data to frame.
 * @param len Length of the raw data.
 * @param out Output buffer for the frame (must be at least 10 + len bytes).
 * @param outlen Pointer to store the total frame length.
 * @return 0 on success, -1 on failure (e.g., payload too large).
 */
static int encode_relay_frame(uint32_t seq, const unsigned char *payload,
                              uint16_t len, unsigned char *out, size_t *outlen) {
    if (len > RELAY_FRAME_MAX) return -1;

    /* Calculate HMAC over the payload */
    uint32_t h = toy_hmac((const unsigned char *)RELAY_HMAC_KEY,
                          RELAY_HMAC_KEYLEN, payload, len);

    /* 4 bytes: Sequence number (Big-Endian) */
    out[0] = (seq >> 24) & 0xFF;
    out[1] = (seq >> 16) & 0xFF;
    out[2] = (seq >> 8) & 0xFF;
    out[3] = seq & 0xFF;

    /* 2 bytes: Payload length (Big-Endian) */
    out[4] = (len >> 8) & 0xFF;
    out[5] = len & 0xFF;

    /* 4 bytes: HMAC (Big-Endian) */
    out[6] = (h >> 24) & 0xFF;
    out[7] = (h >> 16) & 0xFF;
    out[8] = (h >> 8) & 0xFF;
    out[9] = h & 0xFF;

    /* N bytes: Payload */
    memcpy(out + 10, payload, len);
    *outlen = (size_t)(10 + len);
    return 0;
}

/**
 * @brief Decodes a RELAY frame and validates its HMAC.
 * @param in Input buffer containing the frame.
 * @param inlen Length of the input buffer.
 * @param seq_out Pointer to store the sequence number.
 * @param payload_out Output buffer for the payload.
 * @param len_out Pointer to store the payload length.
 * @return 0 on success, -1 on failure (e.g., too short, length mismatch, HMAC failure).
 */
static int decode_relay_frame(const unsigned char *in, size_t inlen,
                              uint32_t *seq_out, unsigned char *payload_out,
                              uint16_t *len_out) {
    if (inlen < 10) return -1; /* Minimum header size */

    /* Extract header fields (Big-Endian) */
    uint32_t seq = ((uint32_t)in[0] << 24) | ((uint32_t)in[1] << 16) |
                   ((uint32_t)in[2] << 8) | (uint32_t)in[3];
    uint16_t len = ((uint16_t)in[4] << 8) | (uint16_t)in[5];
    uint32_t h = ((uint32_t)in[6] << 24) | ((uint32_t)in[7] << 16) |
                 ((uint32_t)in[8] << 8) | (uint32_t)in[9];

    /* Check if the full frame is present in the buffer */
    if ((size_t)(10 + len) > inlen) return -1;
    if (len > RELAY_FRAME_MAX) return -1;

    /* Validate HMAC */
    uint32_t chk = toy_hmac((const unsigned char *)RELAY_HMAC_KEY,
                            RELAY_HMAC_KEYLEN, in + 10, len);
    if (chk != h) {
        MET_hmac_failures++;
        return -1;
    }

    /* Copy output data */
    if (payload_out && len > 0) memcpy(payload_out, in + 10, len);
    if (seq_out) *seq_out = seq;
    if (len_out) *len_out = len;
    return 0;
}

/* -------------------------------------------------------------------------
 * Ring buffer (simple circular buffer)
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
 * Circuit and Stream State Management Structures
 * ------------------------------------------------------------------------- */

/**
 * @brief Stream state enumeration.
 */
typedef enum {
    STREAM_STATE_NONE = 0,
    STREAM_STATE_NEW,         /* Stream allocation, waiting for SOCKS traffic */
    STREAM_STATE_BEGIN_SENT,  /* Sent RELAY_BEGIN cell */
    STREAM_STATE_CONNECTED,   /* Received RELAY_CONNECTED, ready for DATA */
    STREAM_STATE_OPEN,        /* Data flowing */
    STREAM_STATE_CLOSING,     /* Sent RELAY_END, waiting for END or DESTROY */
} stream_state_t;

/**
 * @brief Structure representing a single stream (e.g., an HTTP connection).
 */
typedef struct {
    uint16_t stream_id;             /* Stream identifier (unique per circuit) */
    int socket_fd;                  /* Associated SOCKS client FD (for a full client) */
    stream_state_t state;           /* Current state of the stream */
    char target_host[256];          /* Destination host for BEGIN cell */
    uint16_t target_port;           /* Destination port */
    uint64_t bytes_out;             /* Data sent over this stream */
    uint64_t bytes_in;              /* Data received over this stream */
} stream_t;

/**
 * @brief Circuit state enumeration.
 */
typedef enum {
    CIRCUIT_STATE_NONE = 0,
    CIRCUIT_STATE_NEW,        /* Socket connected, ready to send CREATE */
    CIRCUIT_STATE_CREATE_SENT,/* Sent CREATE cell */
    CIRCUIT_STATE_CREATED,    /* Received CREATED cell */
    CIRCUIT_STATE_OPEN,       /* Ready for streams/EXTEND cells */
    CIRCUIT_STATE_CLOSING,
} circuit_state_t;

/**
 * @brief Structure representing a single circuit.
 */
typedef struct {
    uint16_t circuit_id;            /* Circuit identifier (unique per connection) */
    circuit_state_t state;          /* Current state of the circuit */
    int socket_fd;                  /* Connection to the first OR (mock_or) */
    uint32_t incoming_seq;          /* Next expected sequence number from OR */
    uint32_t outgoing_seq;          /* Next sequence number to send to OR */
    long last_active_ms;            /* Monotonic timestamp of last activity */
    stream_t streams[MAX_STREAM_COUNT]; /* Array of active streams */
    int stream_count;               /* Number of active streams */
    pthread_mutex_t lock;           /* Mutex for thread safety (if multi-threaded) */
} circuit_t;

static circuit_t G_CIRCUIT;

/* -------------------------------------------------------------------------
 * Stream and Circuit Management Helpers
 * ------------------------------------------------------------------------- */

/**
 * @brief Initializes the global circuit structure.
 * @param fd The socket file descriptor to the OR.
 * @return 0 on success.
 */
static int circuit_init(int fd) {
    memset(&G_CIRCUIT, 0, sizeof(circuit_t));
    G_CIRCUIT.circuit_id = 0x1234; /* Mock ID */
    G_CIRCUIT.socket_fd = fd;
    G_CIRCUIT.state = CIRCUIT_STATE_NEW;
    G_CIRCUIT.incoming_seq = 1;
    G_CIRCUIT.outgoing_seq = 1;
    G_CIRCUIT.last_active_ms = now_ms();
    pthread_mutex_init(&G_CIRCUIT.lock, NULL);
    log_info("CIRCUIT", "Circuit initialized (CID: 0x%04X)", G_CIRCUIT.circuit_id);
    return 0;
}

/**
 * @brief Finds an unused stream ID and initializes a new stream structure.
 * @param host Destination host.
 * @param port Destination port.
 * @return Pointer to the new stream, or NULL if MAX_STREAMS reached.
 */
static stream_t *stream_new(const char *host, uint16_t port) {
    stream_t *stream = NULL;
    uint16_t new_id = 0;

    pthread_mutex_lock(&G_CIRCUIT.lock);

    if (G_CIRCUIT.stream_count >= MAX_STREAM_COUNT) {
        pthread_mutex_unlock(&G_CIRCUIT.lock);
        return NULL;
    }

    /* Simple linear search for first available stream slot and ID */
    for (int i = 0; i < MAX_STREAM_COUNT; ++i) {
        if (G_CIRCUIT.streams[i].state == STREAM_STATE_NONE) {
            new_id = (uint16_t)(i + 1); /* Use 1-indexed ID */
            stream = &G_CIRCUIT.streams[i];
            break;
        }
    }

    if (stream) {
        memset(stream, 0, sizeof(stream_t));
        stream->stream_id = new_id;
        stream->state = STREAM_STATE_NEW;
        safe_strncpy(stream->target_host, host, sizeof(stream->target_host));
        stream->target_port = port;
        G_CIRCUIT.stream_count++;
        log_info("STREAM", "New stream %u created to %s:%u", new_id, host, port);
    }

    pthread_mutex_unlock(&G_CIRCUIT.lock);
    return stream;
}

/**
 * @brief Finds a stream by its ID.
 * @param stream_id ID of the stream to find.
 * @return Pointer to the stream, or NULL if not found.
 */
static stream_t *stream_find(uint16_t stream_id) {
    if (stream_id == 0 || stream_id > MAX_STREAM_COUNT) return NULL;
    stream_t *stream = &G_CIRCUIT.streams[stream_id - 1];
    if (stream->stream_id == stream_id) {
        return stream;
    }
    return NULL;
}

/**
 * @brief Closes and cleans up a stream structure.
 * @param stream Pointer to the stream to close.
 */
static void stream_close(stream_t *stream) {
    if (!stream || stream->state == STREAM_STATE_NONE) return;

    pthread_mutex_lock(&G_CIRCUIT.lock);
    log_info("STREAM", "Closing stream %u. In/Out: %llu/%llu bytes",
             stream->stream_id,
             (unsigned long long)stream->bytes_in,
             (unsigned long long)stream->bytes_out);

    if (stream->socket_fd >= 0) {
        close(stream->socket_fd);
    }
    memset(stream, 0, sizeof(stream_t));
    G_CIRCUIT.stream_count--;
    pthread_mutex_unlock(&G_CIRCUIT.lock);
}

/* -------------------------------------------------------------------------
 * Cell and Relay Message Construction
 * ------------------------------------------------------------------------- */

/**
 * @brief Sends a generic fixed-length cell (e.g., CREATE, CREATED, DESTROY).
 * Cell format: [CID(2) | CMD(1) | PAYLOAD(CELL_LEN-3)]
 * @param cell_cmd The cell command (e.g., CELL_CREATE).
 * @param payload Optional payload (up to CELL_LEN-3).
 * @param payload_len Length of the payload.
 * @return 0 on success, -1 on failure.
 */
static int send_fixed_cell(uint8_t cell_cmd, const unsigned char *payload, size_t payload_len) {
    if (G_CIRCUIT.socket_fd < 0 || G_CIRCUIT.state == CIRCUIT_STATE_CLOSING) return -1;
    if (payload_len > CELL_LEN - 3) return -1;

    unsigned char cell[CELL_LEN];
    memset(cell, 0, CELL_LEN);

    /* 2 bytes: Circuit ID (Big-Endian) */
    cell[0] = (G_CIRCUIT.circuit_id >> 8) & 0xFF;
    cell[1] = G_CIRCUIT.circuit_id & 0xFF;

    /* 1 byte: Command */
    cell[2] = cell_cmd;

    /* N bytes: Payload */
    if (payload && payload_len > 0) {
        memcpy(cell + 3, payload, payload_len);
    }

    ssize_t s = send(G_CIRCUIT.socket_fd, cell, CELL_LEN, 0);
    if (s != CELL_LEN) {
        log_err("PROTO", "Failed to send cell %u: %s", cell_cmd, strerror(errno));
        return -1;
    }
    return 0;
}

/**
 * @brief Sends a RELAY_BEGIN cell to open a new stream.
 * @param stream The stream structure to use.
 * @return 0 on success, -1 on failure.
 */
static int send_relay_begin(stream_t *stream) {
    if (!stream || stream->state != STREAM_STATE_NEW) return -1;
    if (G_CIRCUIT.state != CIRCUIT_STATE_OPEN) {
        log_err("PROTO", "Cannot send BEGIN, circuit not OPEN (state: %u)", G_CIRCUIT.state);
        return -1;
    }

    char target[256 + 6]; /* host:port\0 */
    int len = snprintf(target, sizeof(target), "%s:%u", stream->target_host, stream->target_port);

    unsigned char relay_payload[1 + 2 + 4 + RELAY_FRAME_MAX]; /* Cmd(1)|StreamID(2)|Digest(4)|Frame */
    size_t relay_offset = 0;

    /* 1 byte: Relay Command (RELAY_BEGIN) */
    relay_payload[relay_offset++] = RELAY_BEGIN;

    /* 2 bytes: Stream ID (Big-Endian) */
    relay_payload[relay_offset++] = (stream->stream_id >> 8) & 0xFF;
    relay_payload[relay_offset++] = stream->stream_id & 0xFF;

    /* 4 bytes: Digest (Mock-Tor uses a fixed 0x00000000 for this demo) */
    relay_offset += 4; /* Skip digest for now */

    /* N bytes: BEGIN payload (host:port\0) */
    memcpy(relay_payload + relay_offset, target, len + 1);
    relay_offset += (size_t)(len + 1);

    /* 2 bytes: Payload length (Relay Command + Stream ID + Digest + target) */
    uint16_t relay_len = (uint16_t)(relay_offset - 4); /* Length excludes CID and CMD bytes */
    if (relay_len > RELAY_FRAME_MAX) return -1;

    unsigned char relay_frame[10 + RELAY_FRAME_MAX];
    size_t frame_len = 0;

    /* Encode the inner RELAY frame (seq|len|hmac|payload) */
    if (encode_relay_frame(G_CIRCUIT.outgoing_seq, relay_payload, relay_len, relay_frame, &frame_len) != 0) {
        log_err("PROTO", "Failed to encode RELAY frame");
        return -1;
    }

    /* Now wrap the RELAY frame into a full CELL_RELAY cell */
    unsigned char cell[CELL_LEN];
    memset(cell, 0, CELL_LEN);

    cell[0] = (G_CIRCUIT.circuit_id >> 8) & 0xFF;
    cell[1] = G_CIRCUIT.circuit_id & 0xFF;
    cell[2] = CELL_RELAY;

    memcpy(cell + 3, relay_frame, frame_len);

    ssize_t s = send(G_CIRCUIT.socket_fd, cell, CELL_LEN, 0);
    if (s != CELL_LEN) {
        log_err("PROTO", "Failed to send RELAY_BEGIN cell: %s", strerror(errno));
        return -1;
    }

    G_CIRCUIT.outgoing_seq++;
    stream->state = STREAM_STATE_BEGIN_SENT;
    log_info("PROTO", "Sent RELAY_BEGIN (SID %u) to %s:%u. Out-Seq: %u",
             stream->stream_id, stream->target_host, stream->target_port,
             G_CIRCUIT.outgoing_seq - 1);
    return 0;
}

/**
 * @brief Sends a RELAY_DATA cell with a chunk of data.
 * @param stream The stream structure to use.
 * @param data Data buffer.
 * @param len Length of data.
 * @return 0 on success, -1 on failure.
 */
static int send_relay_data(stream_t *stream, const unsigned char *data, uint16_t len) {
    if (!stream || stream->state != STREAM_STATE_OPEN) return -1;
    if (G_CIRCUIT.state != CIRCUIT_STATE_OPEN) return -1;
    if (len == 0 || len > RELAY_FRAME_MAX - 7) return -1; /* -7 for Relay header */

    unsigned char relay_payload[1 + 2 + 4 + RELAY_FRAME_MAX];
    size_t relay_offset = 0;

    /* 1 byte: Relay Command (RELAY_DATA) */
    relay_payload[relay_offset++] = RELAY_DATA;

    /* 2 bytes: Stream ID (Big-Endian) */
    relay_payload[relay_offset++] = (stream->stream_id >> 8) & 0xFF;
    relay_payload[relay_offset++] = stream->stream_id & 0xFF;

    /* 4 bytes: Digest (Mock-Tor uses a fixed 0x00000000 for this demo) */
    relay_offset += 4;

    /* N bytes: Data payload */
    memcpy(relay_payload + relay_offset, data, len);
    relay_offset += len;

    uint16_t relay_len = (uint16_t)(relay_offset - 4);

    unsigned char relay_frame[10 + RELAY_FRAME_MAX];
    size_t frame_len = 0;

    if (encode_relay_frame(G_CIRCUIT.outgoing_seq, relay_payload, relay_len, relay_frame, &frame_len) != 0) {
        log_err("PROTO", "Failed to encode RELAY_DATA frame (SID %u)", stream->stream_id);
        return -1;
    }

    unsigned char cell[CELL_LEN];
    memset(cell, 0, CELL_LEN);

    cell[0] = (G_CIRCUIT.circuit_id >> 8) & 0xFF;
    cell[1] = G_CIRCUIT.circuit_id & 0xFF;
    cell[2] = CELL_RELAY;

    memcpy(cell + 3, relay_frame, frame_len);

    ssize_t s = send(G_CIRCUIT.socket_fd, cell, CELL_LEN, 0);
    if (s != CELL_LEN) {
        log_err("PROTO", "Failed to send RELAY_DATA cell (SID %u): %s", stream->stream_id, strerror(errno));
        return -1;
    }

    G_CIRCUIT.outgoing_seq++;
    stream->bytes_out += len;
    log_dbg("PROTO", "Sent RELAY_DATA (SID %u, Len %u). Out-Seq: %u",
            stream->stream_id, len, G_CIRCUIT.outgoing_seq - 1);
    return 0;
}

/* -------------------------------------------------------------------------
 * Cell and Relay Message Processing
 * ------------------------------------------------------------------------- */

/**
 * @brief Processes an incoming RELAY cell payload.
 * @param frame_payload The payload of the RELAY frame (Relay Cmd | Stream ID | Digest | Data...).
 * @param frame_len The length of the payload.
 */
static void process_incoming_relay_cell(const unsigned char *frame_payload, uint16_t frame_len) {
    if (frame_len < 7) {
        log_err("PROTO", "Incoming RELAY frame too short (%u bytes)", frame_len);
        return;
    }

    uint8_t relay_cmd = frame_payload[0];
    uint16_t stream_id = (uint16_t)((frame_payload[1] << 8) | frame_payload[2]);
    /* uint32_t digest = ... frame_payload[3] to [6] */
    const unsigned char *data_payload = frame_payload + 7;
    uint16_t data_len = frame_len - 7;

    stream_t *stream = stream_find(stream_id);
    if (!stream) {
        log_err("PROTO", "Received RELAY cell (CMD %u) for unknown stream ID %u", relay_cmd, stream_id);
        /* In a real client, we might send a RELAY_END back */
        return;
    }

    G_CIRCUIT.last_active_ms = now_ms();

    switch (relay_cmd) {
    case RELAY_CONNECTED:
        if (stream->state == STREAM_STATE_BEGIN_SENT) {
            stream->state = STREAM_STATE_OPEN;
            log_info("PROTO", "Stream %u connected! Ready for data.", stream_id);
        } else {
            log_err("PROTO", "RELAY_CONNECTED received for stream %u in unexpected state %u", stream_id, stream->state);
        }
        break;

    case RELAY_DATA:
        if (stream->state == STREAM_STATE_OPEN) {
            /* Simulate processing the data (e.g., writing to SOCKS client socket) */
            log_info("PROTO", "Received RELAY_DATA (SID %u, Len %u). Content: '%.*s'",
                     stream_id, data_len, data_len, data_payload);
            stream->bytes_in += data_len;
        } else {
            log_err("PROTO", "Received RELAY_DATA for stream %u in closed/connecting state %u", stream_id, stream->state);
            /* Send RELAY_END/DESTROY */
        }
        break;

    case RELAY_END:
        log_info("PROTO", "Received RELAY_END for stream %u. Closing stream.", stream_id);
        stream_close(stream);
        break;

    case RELAY_BEGIN_FAILED:
        log_err("PROTO", "Stream %u creation failed (RELAY_BEGIN_FAILED). Closing.", stream_id);
        stream_close(stream);
        break;

    default:
        log_err("PROTO", "Received unhandled RELAY command %u for stream %u", relay_cmd, stream_id);
        break;
    }
}

/**
 * @brief Processes a full incoming cell from the OR connection.
 * @param cell The full 512-byte cell buffer.
 * @return 0 if fully processed, -1 on error.
 */
static int process_incoming_cell(const unsigned char *cell) {
    uint16_t cid = (uint16_t)((cell[0] << 8) | cell[1]);
    uint8_t cmd = cell[2];

    if (cid != G_CIRCUIT.circuit_id) {
        if (cid != 0x0000) { /* Ignore non-circuit (e.g., handshake) or non-CID cells */
            log_err("PROTO", "Received cell for unexpected CID 0x%04X (expected 0x%04X)", cid, G_CIRCUIT.circuit_id);
        }
        return -1;
    }

    G_CIRCUIT.last_active_ms = now_ms();

    switch (cmd) {
    case CELL_CREATED: {
        if (G_CIRCUIT.state == CIRCUIT_STATE_CREATE_SENT) {
            /* For a real Tor client, key material is exchanged here. */
            G_CIRCUIT.state = CIRCUIT_STATE_OPEN;
            log_info("PROTO", "CELL_CREATED received. Circuit 0x%04X is OPEN.", cid);
        } else {
            log_err("PROTO", "CELL_CREATED received in unexpected state %u", G_CIRCUIT.state);
        }
        break;
    }

    case CELL_RELAY: {
        uint32_t seq;
        uint16_t plen;
        unsigned char payload[RELAY_FRAME_MAX];
        const unsigned char *relay_frame = cell + 3;
        size_t frame_len = CELL_LEN - 3;

        if (decode_relay_frame(relay_frame, frame_len, &seq, payload, &plen) == 0) {
            MET_frames_in++;
            if (seq == G_CIRCUIT.incoming_seq) {
                process_incoming_relay_cell(payload, plen);
                G_CIRCUIT.incoming_seq++;
            } else {
                log_err("PROTO", "Received out-of-sequence frame (Got %u, Expected %u)", seq, G_CIRCUIT.incoming_seq);
                /* Drop or queue frame */
            }
        } else {
            log_err("PROTO", "Failed to decode/validate incoming RELAY frame.");
        }
        break;
    }

    case CELL_DESTROY:
        log_info("PROTO", "CELL_DESTROY received. Circuit 0x%04X is closing.", cid);
        G_RUN = 0; /* Shut down the main loop for this demo */
        break;

    default:
        log_dbg("PROTO", "Received unhandled cell command %u", cmd);
        break;
    }
    return 0;
}


/* -------------------------------------------------------------------------
 * Hardcoded IPv4 list in LITTLE-ENDIAN uint32_t values
 * ------------------------------------------------------------------------- */

static const uint32_t HARDCODED_IPS_LE[] = {
    0x04F1A768u, /* 104.167.241.4  -> little-endian */
    0x75F2A768u, /* 104.167.242.117 -> little-endian */
    0x76F2A768u, /* 104.167.242.118 -> little-endian */
    0x4A03C068u, /* 104.192.3.74   -> little-endian */
    0x64ECDB68u, /* 104.219.236.100-> little-endian */
    0x7348F468u, /* 104.244.72.115 -> little-endian */
    0x8448F468u, /* 104.244.72.132 -> little-endian */
    0x8849F468u, /* 104.244.73.136 -> little-endian */
    0xBE49F468u, /* 104.244.73.190 -> little-endian */
    0xC149F468u, /* 104.244.73.193 -> little-endian */
    0x2B49F468u, /* 104.244.73.43  -> little-endian */
    0x334AF468u, /* 104.244.74.51  -> little-endian */
    0x614AF468u, /* 104.244.74.97  -> little-endian */
    0x8C4BF468u, /* 104.244.75.140 -> little-endian */
    0x4A4BF468u, /* 104.244.75.74  -> little-endian */
    0xED4CF468u, /* 104.244.76.237 -> little-endian */
    0xD04DF468u, /* 104.244.77.208 -> little-endian */
    0xA24EF468u, /* 104.244.78.162 -> little-endian */
    0xE84EF468u, /* 104.244.78.232 -> little-endian */
    0xE94EF468u, /* 104.244.78.233 -> little-endian */
    0x2C4FF468u, /* 104.244.79.44  -> little-endian */
    0x324FF468u, /* 104.244.79.50  -> little-endian */
    0x3D4FF468u  /* 104.244.79.61  -> little-endian */
};

static const int HARDCODED_IPS_LE_COUNT = (int)(sizeof(HARDCODED_IPS_LE) / sizeof(HARDCODED_IPS_LE[0]));

/* -------------------------------------------------------------------------
 * Socket helpers
 * ------------------------------------------------------------------------- */

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) return -1;
    return 0;
}

/* Non-blocking connect with timeout, returns ms or -1 on failure.
 * Accepts ip as little-endian uint32_t (as stored above). We assign
 * ip_net = ip_le (the literal is already byte-swapped).
 */
static int measure_connect_ms_le(uint32_t ip_le, uint16_t port, int timeout_ms) {
    MET_probe_attempts++;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    if (set_nonblocking(fd) < 0) { close(fd); return -1; }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = ip_le;
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
 * Probe and rank IPs (using little-endian array)
 * ------------------------------------------------------------------------- */

static void probe_and_rank_ips_le(char ranked_ips[][INET_ADDRSTRLEN], int *ranked_count) {
    int lat[HARDCODED_IPS_LE_COUNT];
    for (int i = 0; i < HARDCODED_IPS_LE_COUNT; ++i) lat[i] = -1;

    for (int i = 0; i < HARDCODED_IPS_LE_COUNT; ++i) {
        uint32_t ip_le = HARDCODED_IPS_LE[i];
        struct in_addr a;
        a.s_addr = ip_le;
        char ipbuf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &a, ipbuf, sizeof(ipbuf));
        log_info("PROBE", "Probing %s:%d ...", ipbuf, PROBE_PORT);
        int ms = measure_connect_ms_le(ip_le, PROBE_PORT, PROBE_TIMEOUT_MS);
        lat[i] = ms;
        if (ms >= 0) log_info("PROBE", "%s -> %d ms", ipbuf, ms);
        else log_info("PROBE", "%s -> failed", ipbuf);
    }

    int idxs[HARDCODED_IPS_LE_COUNT];
    for (int i = 0; i < HARDCODED_IPS_LE_COUNT; ++i) idxs[i] = i;
    for (int i = 1; i < HARDCODED_IPS_LE_COUNT; ++i) {
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
    for (int k = 0; k < HARDCODED_IPS_LE_COUNT && added < HARDCODED_IPS_LE_COUNT; ++k) {
        int i = idxs[k];
        struct in_addr a; a.s_addr = HARDCODED_IPS_LE[i];
        inet_ntop(AF_INET, &a, ranked_ips[added], INET_ADDRSTRLEN);
        added++;
    }
    *ranked_count = added;
}

/* -------------------------------------------------------------------------
 * Mock OR (local entry node) implementation
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

/**
 * @brief Handles a single connection to the Mock OR.
 * Performs the CREATE/CREATED handshake and then simulates traffic.
 * @param cfd The client socket file descriptor.
 */
static void handle_mock_or_connection(int cfd) {
    MET_mock_or_connections++;
    unsigned char cell[CELL_LEN];
    ssize_t got = 0;
    /* 1. Expect a CREATE cell (CELL_LEN bytes) */
    while (got < CELL_LEN) {
        ssize_t n = recv(cfd, cell + got, CELL_LEN - got, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            log_err("MOCK_OR", "recv CREATE error: %s", strerror(errno));
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

    uint16_t cid = (uint16_t)((cell[0] << 8) | cell[1]);
    uint8_t cmd = cell[2];

    if (cmd != CELL_CREATE) {
        log_err("MOCK_OR", "Received cell command 0x%02X, expected CREATE (0x%02X)", cmd, CELL_CREATE);
        close(cfd);
        return;
    }

    /* 2. Respond with a CREATED cell */
    unsigned char rep[CELL_LEN];
    memset(rep, 0, sizeof(rep));
    rep[0] = cell[0]; /* Keep same CID */
    rep[1] = cell[1];
    rep[2] = CELL_CREATED; /* CREATED command */
    if (send(cfd, rep, CELL_LEN, 0) != CELL_LEN) {
        log_err("MOCK_OR", "send CREATED failed: %s", strerror(errno));
        close(cfd);
        return;
    }
    MET_mock_or_handshakes++;
    log_info("MOCK_OR", "Handshake completed (CREATED sent for CID 0x%04X)", cid);

    set_nonblocking(cfd);
    uint32_t seq = 1;
    time_t start = time(NULL);
    /* 3. Main loop: simulate receiving RELAY cells and sending back framed data */
    while (G_RUN && (time(NULL) - start) < SHUFFLER_RUN_SECONDS) {
        unsigned char in_cell[CELL_LEN];
        ssize_t n_in = recv(cfd, in_cell, CELL_LEN, 0);

        if (n_in == CELL_LEN) {
            /* Process incoming cell (for a full mock OR, this would handle BEGIN, DATA, etc.) */
            uint16_t in_cid = (uint16_t)((in_cell[0] << 8) | in_cell[1]);
            uint8_t in_cmd = in_cell[2];

            if (in_cmd == CELL_RELAY) {
                log_info("MOCK_OR", "Received CELL_RELAY from client (CID 0x%04X).", in_cid);
                /* Simulate a response by sending back a RELAY cell with DATA */
                unsigned char raw[1024];
                snprintf((char*)raw, sizeof(raw), "Mock OR response to RELAY cell (CID 0x%04X, Time %ld)", in_cid, time(NULL));
                ssize_t n_raw = strlen((char*)raw);

                unsigned char out[10 + RELAY_FRAME_MAX];
                size_t outlen = 0;
                uint16_t plen = (n_raw > RELAY_FRAME_MAX) ? RELAY_FRAME_MAX : (uint16_t)n_raw;

                if (encode_relay_frame(seq++, raw, plen, out, &outlen) == 0) {
                    unsigned char out_cell[CELL_LEN];
                    memset(out_cell, 0, CELL_LEN);
                    out_cell[0] = (cid >> 8) & 0xFF;
                    out_cell[1] = cid & 0xFF;
                    out_cell[2] = CELL_RELAY;
                    memcpy(out_cell + 3, out, outlen);

                    ssize_t w = send(cfd, out_cell, CELL_LEN, 0);
                    if (w <= 0) {
                        log_err("MOCK_OR", "send frame failed: %s", strerror(errno));
                        break;
                    }
                    MET_frames_out++;
                }
            } else if (in_cmd == CELL_DESTROY) {
                log_info("MOCK_OR", "Received CELL_DESTROY. Closing connection.");
                break;
            }

        } else if (n_in < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { msleep(20); continue; }
            log_err("MOCK_OR", "recv error: %s", strerror(errno));
            break;
        } else if (n_in == 0) {
            log_info("MOCK_OR", "client closed connection");
            break;
        } else {
            log_err("MOCK_OR", "Partial cell received (%zd bytes). Closing.", n_in);
            break;
        }
    }
    close(cfd);
}

static void *mock_or_thread(void *arg) {
    (void)arg;
    if (mock_or_listen_fd < 0) {
        log_err("MOCK_OR", "listen fd invalid");
        return NULL;
    }
    set_nonblocking(mock_or_listen_fd);
    while (G_RUN) {
        struct sockaddr_in cli; socklen_t cl = sizeof(cli);
        int cfd = accept(mock_or_listen_fd, (struct sockaddr*)&cli, &cl);
        if (cfd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { msleep(25); continue; }
            if (errno == EINVAL || errno == EBADF) break; /* fd closed */
            log_err("MOCK_OR", "accept error: %s", strerror(errno));
            break;
        }
        /* Handle connection in a new thread/fork if OR were production-grade.
           For this simple demo, handle sequentially and quickly. */
        handle_mock_or_connection(cfd);
    }
    return NULL;
}

/* -------------------------------------------------------------------------
 * Shuffler helpers (read into ring buffer, write from ring buffer)
 * ------------------------------------------------------------------------- */

/* This helper may be unused in some flows; mark as possibly unused to avoid warnings */
static void shuffler_read_into_rbuf(int fd) __attribute__((unused));
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
        /* In a real scenario, this data should be put back into the ring buffer,
           but for simplicity in this demo, we drop it to avoid infinite loops. */
        return;
    }
    MET_circuit_to_client_bytes += (uint64_t)w;
}

/* -------------------------------------------------------------------------
 * Metrics printing
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
    log_info("METRICS", "socks_connections=%llu, socks_errors=%llu",
             (unsigned long long)MET_socks_connections,
             (unsigned long long)MET_socks_errors);
}

/* -------------------------------------------------------------------------
 * Start Chromium with SOCKS5_PROXY env var (fork + exec)
 * Returns PID of Chromium child or -1 on error.
 * ------------------------------------------------------------------------- */

static pid_t start_chromium_with_socks(void) {
    pid_t pid = fork();
    if (pid < 0) {
        log_err("CHROMIUM", "fork() failed: %s", strerror(errno));
        return -1;
    } else if (pid == 0) {
        /* Child: set environment variable and exec a candidate browser */
        char proxy_val[64];
        snprintf(proxy_val, sizeof(proxy_val), "localhost:%d", CFG_LOCAL_SOCKS_PORT);
        /* set SOCKS5_PROXY environment variable to "localhost:PORT" */
        setenv("SOCKS5_PROXY", proxy_val, 1);
        /* Also set ALL_PROXY for some tools */
        setenv("ALL_PROXY", proxy_val, 0);

        char proxy_arg[64];
        snprintf(proxy_arg, sizeof(proxy_arg), "--proxy-server=socks5://localhost:%d", CFG_LOCAL_SOCKS_PORT);

        /* Try common browser executables */
        const char *cands[] = { "chromium", "chromium-browser", "google-chrome", "chrome", NULL };
        for (int i = 0; cands[i] != NULL; ++i) {
            char *argv[3];
            argv[0] = (char *)cands[i];
            argv[1] = proxy_arg;
            argv[2] = NULL;
            execvp(cands[i], argv);
            /* If execvp returns, it failed; try next candidate */
        }
        perror("[CHROMIUM] execvp failed: Could not find/run browser executable");
        _exit(127);
    } else {
        log_info("CHROMIUM", "Chromium forked (pid=%d)", (int)pid);
        return pid;
    }
}

/* -------------------------------------------------------------------------
 * Simple SOCKS5 daemon started via fork() (background)
 * - NOTE: This SOCKS daemon acts as a standard proxy, NOT a circuit client.
 * In a full implementation, this logic would be merged with circuit_t/stream_t management.
 * ------------------------------------------------------------------------- */

static pid_t start_socks_daemon(uint16_t listen_port) {
    pid_t pid = fork();
    if (pid < 0) {
        log_err("SOCKS", "fork failed: %s", strerror(errno));
        return -1;
    }
    if (pid > 0) {
        log_info("SOCKS", "Started background SOCKS daemon (pid=%d) on port %u", (int)pid, listen_port);
        return pid;
    }

    /* Child: become a simple daemon */
    if (setsid() < 0) { /* ignore */ }

    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) {
        perror("[SOCKS] socket");
        _exit(1);
    }
    int one = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(listen_port);
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(lfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        perror("[SOCKS] bind");
        close(lfd);
        _exit(1);
    }
    if (listen(lfd, 32) < 0) {
        perror("[SOCKS] listen");
        close(lfd);
        _exit(1);
    }
    log_info("SOCKS", "Daemon listening on 127.0.0.1:%u", listen_port);

    while (1) {
        struct sockaddr_in cli; socklen_t cl = sizeof(cli);
        int cfd = accept(lfd, (struct sockaddr*)&cli, &cl);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            perror("[SOCKS] accept");
            break;
        }

        pid_t cpid = fork();
        if (cpid < 0) {
            perror("[SOCKS] fork child");
            close(cfd);
            continue;
        }
        if (cpid > 0) {
            close(cfd);
            while (waitpid(-1, NULL, WNOHANG) > 0) {}
            continue;
        }

        /* Child: handle one client (standard SOCKS5 proxying) */
        close(lfd);

        MET_socks_connections++;

        unsigned char buf[4096];
        ssize_t n = recv(cfd, buf, 2, 0);
        if (n != 2 || buf[0] != 0x05) { close(cfd); _exit(0); }
        int nmethods = (int)buf[1];
        if (nmethods <= 0 || nmethods > 255) { close(cfd); _exit(0); }
        n = recv(cfd, buf, nmethods, 0);
        if (n <= 0) { close(cfd); _exit(0); }
        unsigned char resp[2] = {0x05, 0xFF};
        for (int i = 0; i < n; ++i) {
            if (buf[i] == 0x00) { resp[1] = 0x00; break; }
        }
        send(cfd, resp, 2, 0);
        if (resp[1] != 0x00) { close(cfd); _exit(0); }

        n = recv(cfd, buf, 4, 0);
        if (n != 4 || buf[0] != 0x05) { close(cfd); _exit(0); }
        unsigned char cmd = buf[1];
        unsigned char atyp = buf[3];
        char dest_host[256] = {0};
        uint16_t dest_port = 0;
        if (cmd != 0x01) {
            unsigned char rep_fail[10] = {0x05, 0x07, 0x00, 0x01};
            send(cfd, rep_fail, 10, 0);
            close(cfd); _exit(0);
        }
        if (atyp == 0x01) {
            n = recv(cfd, buf, 6, 0);
            if (n != 6) { close(cfd); _exit(0); }
            snprintf(dest_host, sizeof(dest_host), "%u.%u.%u.%u", (unsigned)buf[0], (unsigned)buf[1], (unsigned)buf[2], (unsigned)buf[3]);
            dest_port = (uint16_t)((buf[4] << 8) | buf[5]);
        } else if (atyp == 0x03) {
            n = recv(cfd, buf, 1, 0);
            if (n != 1) { close(cfd); _exit(0); }
            int dlen = buf[0];
            if (dlen <= 0 || dlen > 255) { close(cfd); _exit(0); }
            n = recv(cfd, (unsigned char*)dest_host, dlen + 2, 0);
            if (n != dlen + 2) { close(cfd); _exit(0); }
            char domain[256];
            memcpy(domain, dest_host, dlen);
            domain[dlen] = '\0';
            dest_port = (uint16_t)(((unsigned char)dest_host[dlen] << 8) | (unsigned char)dest_host[dlen+1]);
            safe_strncpy(dest_host, domain, sizeof(dest_host));
        } else {
            unsigned char rep_fail[10] = {0x05, 0x08, 0x00, 0x01};
            send(cfd, rep_fail, 10, 0);
            close(cfd); _exit(0);
        }

        int remote_fd = -1;
        struct addrinfo hints, *res = NULL;
        char portstr[16];
        snprintf(portstr, sizeof(portstr), "%u", dest_port);
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_UNSPEC;
        int gai = getaddrinfo(dest_host, portstr, &hints, &res);
        if (gai != 0 || res == NULL) {
            unsigned char rep_fail[10] = {0x05, 0x04, 0x00, 0x01};
            send(cfd, rep_fail, 10, 0);
            if (res) freeaddrinfo(res);
            close(cfd);
            _exit(0);
        }
        struct addrinfo *rp;
        for (rp = res; rp != NULL; rp = rp->ai_next) {
            remote_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (remote_fd < 0) continue;
            if (connect(remote_fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
            close(remote_fd);
            remote_fd = -1;
        }
        freeaddrinfo(res);
        if (remote_fd < 0) {
            unsigned char rep_fail[10] = {0x05, 0x05, 0x00, 0x01};
            send(cfd, rep_fail, 10, 0);
            close(cfd);
            _exit(0);
        }

        unsigned char rep_ok[10] = {0x05, 0x00, 0x00, 0x01, 0,0,0,0, 0,0};
        send(cfd, rep_ok, 10, 0);

        set_nonblocking(cfd);
        set_nonblocking(remote_fd);
        fd_set rfds;
        int maxfd = (cfd > remote_fd) ? cfd : remote_fd;
        while (1) {
            FD_ZERO(&rfds);
            FD_SET(cfd, &rfds);
            FD_SET(remote_fd, &rfds);
            struct timeval tv = { .tv_sec = 5, .tv_usec = 0 };
            int sel = select(maxfd + 1, &rfds, NULL, NULL, &tv);
            if (sel < 0) {
                if (errno == EINTR) continue;
                break;
            } else if (sel == 0) {
                continue;
            }
            if (FD_ISSET(cfd, &rfds)) {
                ssize_t r = recv(cfd, buf, sizeof(buf), 0);
                if (r <= 0) break;
                ssize_t s2 = send(remote_fd, buf, r, 0);
                if (s2 <= 0) break;
            }
            if (FD_ISSET(remote_fd, &rfds)) {
                ssize_t r = recv(remote_fd, buf, sizeof(buf), 0);
                if (r <= 0) break;
                ssize_t s2 = send(cfd, buf, r, 0);
                if (s2 <= 0) break;
            }
        }

        close(remote_fd);
        close(cfd);
        _exit(0);
    }

    close(lfd);
    _exit(0);
    return 0;
}

/* -------------------------------------------------------------------------
 * Banner
 * ------------------------------------------------------------------------- */

static void print_banner(void) {
    printf("------------------------------------------------------------\n");
    printf(" ciator_main_fixed_expanded.c - educational demo (expanded)\n");
    printf(" - Mock OR on %s:%d\n", MOCK_OR_ADDR, MOCK_OR_PORT);
    printf(" - Hardcoded IPv4 probe list: %d entries\n", HARDCODED_IPS_LE_COUNT);
    printf(" - Ring buffer capacity: %d bytes\n", RBUF_CAPACITY);
    printf(" - Relay frame max payload: %d bytes\n", RELAY_FRAME_MAX);
    printf(" - SOCKS daemon will be started on 127.0.0.1:%d\n", CFG_LOCAL_SOCKS_PORT);
    printf(" - Chromium will be attempted with SOCKS5_PROXY=localhost:%d\n", CFG_LOCAL_SOCKS_PORT);
    printf(" - Expanded to include basic Circuit/Stream management structures and logic.\n");
    printf("------------------------------------------------------------\n");
}

/* -------------------------------------------------------------------------
 * Signal handler
 * ------------------------------------------------------------------------- */

static void handle_sigint(int sig) {
    (void)sig;
    G_RUN = 0;
    log_info("MAIN", "SIGINT received, shutting down");
}

/* -------------------------------------------------------------------------
 * Main
 * ------------------------------------------------------------------------- */

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
    /* Ignore SIGPIPE for socket operations */
    signal(SIGPIPE, SIG_IGN);

    print_banner();
    log_info("MAIN", "Starting fixed and expanded demo (educational only)");

    rbuf_init();

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

    /* Start SOCKS daemon in background */
    pid_t socks_pid = start_socks_daemon(CFG_LOCAL_SOCKS_PORT);
    if (socks_pid < 0) {
        log_err("MAIN", "Failed to start SOCKS daemon");
    } else {
        log_info("MAIN", "SOCKS daemon started (pid=%d)", (int)socks_pid);
    }

    /* 1. Probe and Rank IPs */
    char ranked_ips[HARDCODED_IPS_LE_COUNT][INET_ADDRSTRLEN];
    int ranked_count = 0;
    probe_and_rank_ips_le(ranked_ips, &ranked_count);
    log_info("MAIN", "Top %d ranked IPs:", ranked_count);
    for (int i = 0; i < ranked_count; ++i) {
        log_info("MAIN", "  #%d: %s", i + 1, ranked_ips[i]);
    }

    /* 2. Connect to Mock OR and establish circuit */
    int circ_fd = -1;
    circ_fd = socket(AF_INET, SOCK_STREAM, 0);
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
            log_info("MAIN", "Connected to mock OR. Initializing circuit.");
            circuit_init(circ_fd);
        }
    }

    if (G_CIRCUIT.socket_fd >= 0) {
        /* Send CREATE cell */
        if (send_fixed_cell(CELL_CREATE, NULL, 0) == 0) {
            G_CIRCUIT.state = CIRCUIT_STATE_CREATE_SENT;
            log_info("MAIN", "CREATE sent, awaiting CREATED");
            unsigned char created[CELL_LEN];
            ssize_t got = 0;
            /* Blocking read for CREATED response */
            while (got < CELL_LEN) {
                ssize_t n = recv(G_CIRCUIT.socket_fd, created + got, CELL_LEN - got, 0);
                if (n < 0) {
                    log_err("MAIN", "recv CREATED error: %s", strerror(errno));
                    G_RUN = 0; break;
                }
                if (n == 0) {
                    log_err("MAIN", "connection closed while waiting for CREATED");
                    G_RUN = 0; break;
                }
                got += n;
            }
            if (G_RUN) {
                process_incoming_cell(created);
            }
        } else {
            log_err("MAIN", "send CREATE failed.");
            G_RUN = 0;
        }
    }

    /* 3. Simulate Stream setup and data transfer */
    stream_t *stream1 = NULL;
    if (G_CIRCUIT.state == CIRCUIT_STATE_OPEN) {
        log_info("MAIN", "Circuit is OPEN. Starting stream simulation.");
        stream1 = stream_new("www.example.com", 80);
        if (stream1) {
            send_relay_begin(stream1);
        }
    }

    /* 4. Main Client Polling Loop (Circuit only) */
    set_nonblocking(G_CIRCUIT.socket_fd);
    time_t loop_start = time(NULL);
    while (G_RUN && (time(NULL) - loop_start) < (SHUFFLER_RUN_SECONDS + 2)) {
        if (G_CIRCUIT.socket_fd >= 0) {
            unsigned char buf[CELL_LEN];
            /* Try to read a full cell */
            ssize_t n = recv(G_CIRCUIT.socket_fd, buf, CELL_LEN, 0);
            if (n == CELL_LEN) {
                process_incoming_cell(buf);
            } else if (n > 0) {
                log_err("MAIN", "Partial cell received (%zd bytes). Discarding.", n);
            } else if (n < 0) {
                if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
                    log_err("MAIN", "Circuit recv error: %s", strerror(errno));
                    G_RUN = 0;
                }
            } else { /* n == 0 */
                log_info("MAIN", "Circuit closed by peer");
                G_RUN = 0;
            }
        }

        /* Simulate sending data once stream is open */
        if (stream1 && stream1->state == STREAM_STATE_OPEN) {
            const char *data = "GET / HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n";
            uint16_t len = (uint16_t)strlen(data);
            send_relay_data(stream1, (const unsigned char*)data, len);
            stream1->state = STREAM_STATE_CLOSING; /* Only send once for demo */
        }

        msleep(50);
    }

    /* 5. Start Chromium and wait for it to exit */
    log_info("MAIN", "Circuit simulation finished. Starting browser test.");
    if (stream1) stream_close(stream1); /* Clean up simulated stream */

    pid_t chromepid = start_chromium_with_socks();
    if (chromepid > 0) {
        int status = 0;
        log_info("CHROMIUM", "Waiting for Chromium (pid=%d) to exit...", (int)chromepid);
        if (waitpid(chromepid, &status, 0) == chromepid) {
            if (WIFEXITED(status)) {
                log_info("CHROMIUM", "Chromium exited with code %d", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                log_info("CHROMIUM", "Chromium killed by signal %d", WTERMSIG(status));
            } else {
                log_info("CHROMIUM", "Chromium terminated (status=%d)", status);
            }
        } else {
            log_err("CHROMIUM", "waitpid failed: %s", strerror(errno));
        }
    } else {
        log_err("CHROMIUM", "Chromium was not started (chromepid <= 0)");
    }

    /* 6. Stop SOCKS daemon (only after Chromium exited) */
    if (socks_pid > 0) {
        log_info("SOCKS", "Stopping SOCKS daemon (pid=%d) after Chromium exit", (int)socks_pid);
        kill(socks_pid, SIGTERM);
        long start = now_ms();
        int waited = 0;
        while (now_ms() - start < 5000) {
            pid_t w = waitpid(socks_pid, NULL, WNOHANG);
            if (w == socks_pid) { waited = 1; break; }
            msleep(100);
        }
        if (!waited) {
            log_info("SOCKS", "SOCKS daemon did not exit, sending SIGKILL");
            kill(socks_pid, SIGKILL);
            waitpid(socks_pid, NULL, 0);
        } else {
            log_info("SOCKS", "SOCKS daemon exited cleanly");
        }
    }

    /* 7. Final Shutdown */
    if (G_CIRCUIT.socket_fd >= 0) close(G_CIRCUIT.socket_fd);
    G_RUN = 0;
    if (mock_or_listen_fd >= 0) close(mock_or_listen_fd);
    pthread_join(or_thread, NULL);
    pthread_mutex_destroy(&G_CIRCUIT.lock);

    print_metrics();

    log_info("MAIN", "Shutdown complete");
    return 0;
}
