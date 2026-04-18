/*
 * rawx11_modular2.c
 *
 * Modularized raw X11 client based on the user's example.
 * Creates an 800x600 window and streams white-noise frames via PutImage.
 *
 * Build:
 *   gcc -O2 rawx11_modular2.c -o rawx11_modular2 -g -Wall
 *
 * Run:
 *   DISPLAY=:0 ./rawx11_modular2
 *
 * Notes:
 * - Educational skeleton. For production use parse visuals, handle byte order
 *   precisely, and implement robust error/event handling.
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* Fatal helper */
#define FATAL(msg, ...) do { fprintf(stderr, "Fatal: " msg "\n", ##__VA_ARGS__); exit(EXIT_FAILURE); } while (0)

/* X11 opcodes and masks */
enum {
    OPC_CREATE_WINDOW = 1,
    OPC_MAP_WINDOW    = 8,
    OPC_CREATE_GC     = 55,
    OPC_PUT_IMAGE     = 72,
    OPC_GET_INPUT_FOCUS = 43,

    CW_EVENT_MASK = 1 << 11,
    EM_KEY_PRESS   = 1 << 0,
    EM_BUTTON_PRESS= 1 << 2,
    EM_POINTER_MOTION = 1 << 6,
};

/* Packed structures for handshake */
typedef struct __attribute__((packed)) {
    uint8_t  order;
    uint8_t  pad1;
    uint16_t major_version;
    uint16_t minor_version;
    uint16_t auth_name_len;
    uint16_t auth_data_len;
    uint16_t pad2;
} conn_req_t;

typedef struct __attribute__((packed)) {
    uint8_t  success;
    uint8_t  pad;
    uint16_t major_version;
    uint16_t minor_version;
    uint16_t len; /* length in 4-byte units following this header */
} conn_reply_hdr_t;

/* Minimal view into the success body */
typedef struct __attribute__((packed)) {
    uint32_t release;
    uint32_t id_base;
    uint32_t id_mask;
    uint32_t motion_buffer_size;
    uint16_t vendor_len;
    uint16_t request_max;
    uint8_t  num_screens;
    uint8_t  num_pixmap_formats;
    uint8_t  image_byte_order;
    uint8_t  bitmap_bit_order;
    uint8_t  scanline_unit;
    uint8_t  scanline_pad;
    uint8_t  keycode_min;
    uint8_t  keycode_max;
    uint32_t pad;
    char     vendor_string[1]; /* variable */
} conn_reply_body_t;

/* Screen structure (packed subset) */
typedef struct __attribute__((packed)) {
    uint32_t root_id;
    uint32_t default_colormap;
    uint32_t white_pixel;
    uint32_t black_pixel;
    uint32_t current_input_masks;
    uint16_t width_px;
    uint16_t height_px;
    uint16_t width_mm;
    uint16_t height_mm;
    uint16_t min_maps;
    uint16_t max_maps;
    uint32_t root_visual_id;
    uint8_t  backing_store;
    uint8_t  save_unders;
    uint8_t  root_depth;
    uint8_t  allowed_depths_len;
} screen_t;

/* State container */
typedef struct {
    int sockfd;
    conn_reply_hdr_t hdr;
    conn_reply_body_t *body;
    screen_t *screens;
    uint32_t next_id;
    uint32_t gc_id;
    uint32_t win_id;
} state_t;

/* I/O helpers */
static void write_all_or_die(int fd, const void *buf, size_t n) {
    const uint8_t *p = buf;
    size_t left = n;
    while (left) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            FATAL("write failed: %s", strerror(errno));
        }
        left -= (size_t)w;
        p += w;
    }
}
static void read_all_or_die(int fd, void *buf, size_t n) {
    uint8_t *p = buf;
    size_t left = n;
    while (left) {
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR) continue;
            FATAL("read failed: %s", strerror(errno));
        }
        if (r == 0) FATAL("connection closed while reading");
        left -= (size_t)r;
        p += r;
    }
}

/* Read a simple MIT-MAGIC-COOKIE-1 from ~/.Xauthority (heuristic) */
static int read_xauth_cookie(uint8_t out[16]) {
    char path[1024];
    const char *home = getenv("HOME");
    if (!home) home = "";
    snprintf(path, sizeof(path), "%s/.Xauthority", home);
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    /* Simple heuristic: read file into buffer and take last 16 bytes if present */
    uint8_t buf[8192];
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    if (n < 16) return -1;
    memcpy(out, buf + n - 16, 16);
    return 0;
}

/* Connect to /tmp/.X11-unix/X0 (simple) */
static int connect_xsocket(void) {
    int s = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (s < 0) FATAL("socket: %s", strerror(errno));
    struct sockaddr_un sa;
    memset(&sa, 0, sizeof(sa));
    sa.sun_family = AF_UNIX;
    const char *path = "/tmp/.X11-unix/X0";
    if (strlen(path) >= sizeof(sa.sun_path)) FATAL("socket path too long");
    strcpy(sa.sun_path, path);
    if (connect(s, (struct sockaddr *)&sa, sizeof(sa)) < 0) FATAL("connect: %s", strerror(errno));
    return s;
}

/* Send setup request with auth */
static void send_setup_request(int sockfd) {
    conn_req_t req;
    memset(&req, 0, sizeof(req));
    req.order = 'l'; /* little-endian */
    req.major_version = 11;
    req.minor_version = 0;
    /* auth name "MIT-MAGIC-COOKIE-1" length 18, auth data 16 */
    req.auth_name_len = 18;
    req.auth_data_len = 16;
    write_all_or_die(sockfd, &req, sizeof(req));

    /* auth name padded to 4 bytes (18 -> 20) */
    const char auth_name_padded[20] = "MIT-MAGIC-COOKIE-1\0\0";
    write_all_or_die(sockfd, auth_name_padded, sizeof(auth_name_padded));

    uint8_t cookie[16];
    if (read_xauth_cookie(cookie) != 0) {
        FATAL("could not read Xauthority cookie");
    }
    write_all_or_die(sockfd, cookie, sizeof(cookie));
}

/* Parse connection reply and set up state pointers */
static void read_connection_reply(state_t *st) {
    read_all_or_die(st->sockfd, &st->hdr, sizeof(st->hdr));
    if (st->hdr.success == 0) FATAL("X server connection failed");
    if (st->hdr.len == 0) FATAL("connection reply length zero");

    size_t body_bytes = (size_t)st->hdr.len * 4;
    st->body = malloc(body_bytes);
    if (!st->body) FATAL("malloc failed");
    read_all_or_die(st->sockfd, st->body, body_bytes);

    /* vendor string length and padding */
    size_t vendor_len = st->body->vendor_len;
    size_t vendor_pad = (vendor_len + 3) & ~3;
    uint8_t *base = (uint8_t *)st->body;
    uint8_t *pfmt_ptr = base + offsetof(conn_reply_body_t, vendor_string) + vendor_pad;

    /* pixmap formats count */
    int pf_count = st->body->num_pixmap_formats;
    size_t pf_bytes = pf_count * sizeof(uint8_t) * 8; /* each pixmap_format is 8 bytes in protocol; we don't define struct here */
    /* For safety, we won't parse pixmap formats in detail; instead find first screen after them */
    uint8_t *screens_ptr = pfmt_ptr + pf_count * 8;
    st->screens = (screen_t *)screens_ptr;

    /* set next resource id */
    st->next_id = st->body->id_base;
    if (st->next_id == 0) FATAL("invalid id_base");
}

/* Generate a new resource id */
static uint32_t alloc_id(state_t *st) {
    return st->next_id++;
}

/* Create a simple GC */
static void create_gc(state_t *st) {
    st->gc_id = alloc_id(st);
    uint32_t packet[4];
    /* packet[0]: opcode | (length << 16) */
    packet[0] = (OPC_CREATE_GC) | (4u << 16);
    packet[1] = st->gc_id;
    packet[2] = st->screens[0].root_id; /* drawable */
    packet[3] = 0; /* value-mask = 0 */
    write_all_or_die(st->sockfd, packet, sizeof(packet));
}

/* Create window with event mask value provided */
static void create_window(state_t *st, uint16_t w, uint16_t h, uint32_t parent) {
    st->win_id = alloc_id(st);
    /* We'll build a packet of 9 words (like your example) */
    const int words = 9;
    uint32_t *packet = calloc(words, 4);
    if (!packet) FATAL("calloc failed");
    packet[0] = (OPC_CREATE_WINDOW) | ((uint32_t)words << 16);
    packet[1] = st->win_id;
    packet[2] = parent;
    packet[3] = 0; /* x,y packed as 32-bit zero */
    packet[4] = ((uint32_t)w) | ((uint32_t)h << 16);
    packet[5] = 0; /* border width and class */
    packet[6] = 0; /* visual */
    packet[7] = CW_EVENT_MASK;
    packet[8] = EM_KEY_PRESS | EM_BUTTON_PRESS | EM_POINTER_MOTION;
    write_all_or_die(st->sockfd, packet, words * 4);
    free(packet);
}

/* Map window */
static void map_window(state_t *st) {
    uint32_t packet[2];
    packet[0] = (OPC_MAP_WINDOW) | (2u << 16);
    packet[1] = st->win_id;
    write_all_or_die(st->sockfd, packet, sizeof(packet));
}

/* Build and send PutImage ZPixmap with 32bpp data.
   width and height are the image size; data is 4 bytes per pixel (native uint32 pixels).
   This function constructs a packet where:
     - packet[0] = opcode | (format<<8) | (length<<16)
     - packet[1] = drawable
     - packet[2] = gc
     - packet[3] = width | (height<<16)
     - packet[4] = dst_x | (dst_y<<16)
     - packet[5] = leftPad | (depth<<8)
     - pixel data follows as 32-bit words
*/
static void put_image_32(state_t *st, uint32_t drawable, uint32_t gc,
                         uint32_t width, uint32_t height, uint32_t *pixels) {
    /* words after word0: header words 1..5 (5 words) + pixel words (width*height) */
    uint32_t data_words = width * height;
    uint32_t words_after0 = 5 + data_words;
    uint32_t total_words = 1 + words_after0;
    size_t total_bytes = (size_t)total_words * 4;

    uint32_t *packet = malloc(total_bytes);
    if (!packet) FATAL("malloc failed for PutImage packet");

    /* format = 2 (ZPixmap) placed in byte 1 (<<8) */
    uint32_t format_byte = (2u << 8);
    packet[0] = OPC_PUT_IMAGE | format_byte | (words_after0 << 16);
    packet[1] = drawable;
    packet[2] = gc;
    packet[3] = ((uint32_t)width) | ((uint32_t)height << 16);
    packet[4] = 0; /* dst x,y */
    packet[5] = (24u << 8); /* leftPad=0, depth=24 (in byte1 of word5) */

    /* copy pixel data (assume pixels are 0x00RRGGBB) */
    memcpy(&packet[6], pixels, data_words * 4);

    write_all_or_die(st->sockfd, packet, total_bytes);
    free(packet);
}

/* Generate a frame of white noise into a preallocated pixel buffer (32-bit words).
   Each pixel is 0x00RRGGBB (alpha zero). */
static void generate_noise_frame_u32(uint32_t *buf, uint32_t width, uint32_t height) {
    uint32_t total = width * height;
    for (uint32_t i = 0; i < total; ++i) {
        uint32_t rgb = (uint32_t)(rand() & 0x00FFFFFF);
        buf[i] = rgb;
    }
}

/* Handle incoming event buffer (32 bytes or more). Basic dispatch for a few events. */
static void handle_event(state_t *st, uint8_t *ev, ssize_t len) {
    if (len < 1) return;
    uint8_t type = ev[0];
    switch (type) {
        case 2: { /* KeyPress */
            uint8_t keycode = ev[1];
            fprintf(stderr, "KeyPress keycode=%u\n", keycode);
            if (keycode == 9) { /* Escape heuristic */
                fprintf(stderr, "Escape pressed, exiting\n");
                exit(0);
            }
            break;
        }
        case 4: { /* ButtonPress */
            uint8_t button = ev[1];
            uint16_t x = *(uint16_t *)(ev + 8);
            uint16_t y = *(uint16_t *)(ev + 10);
            fprintf(stderr, "ButtonPress button=%u at (%u,%u)\n", button, x, y);
            break;
        }
        case 6: { /* MotionNotify */
            uint16_t x = *(uint16_t *)(ev + 8);
            uint16_t y = *(uint16_t *)(ev + 10);
            /* quiet by default */
            break;
        }
        case 12: { /* Expose */
            fprintf(stderr, "Expose event: redrawing\n");
            /* On expose we redraw once synchronously */
            uint32_t *pixels = malloc(800u * 600u * 4u);
            if (!pixels) break;
            generate_noise_frame_u32(pixels, 800, 600);
            put_image_32(st, st->win_id, st->gc_id, 800, 600, pixels);
            free(pixels);
            break;
        }
        default:
            /* ignore other events */
            break;
    }
}

/* x11_flush: send GetInputFocus and wait for its reply while dispatching events.
   This mirrors the example's approach to ensure the server processed prior requests. */
static void x11_flush_and_dispatch(state_t *st) {
    uint32_t req = OPC_GET_INPUT_FOCUS | (1u << 16);
    write_all_or_die(st->sockfd, &req, 4);

    uint8_t buf[32];
    while (1) {
        read_all_or_die(st->sockfd, buf, sizeof(buf));
        uint8_t rtype = buf[0];
        if (rtype == 1) {
            /* reply to GetInputFocus */
            return;
        } else if (rtype == 0) {
            fprintf(stderr, "Received X error packet\n");
            /* continue waiting */
        } else {
            handle_event(st, buf, sizeof(buf));
        }
    }
}

/* Main program flow */
int main(void) {
    srand((unsigned)time(NULL));

    state_t st;
    memset(&st, 0, sizeof(st));

    st.sockfd = connect_xsocket();
    send_setup_request(st.sockfd);
    read_connection_reply(&st);

    /* Create GC, window, map */
    create_gc(&st);
    create_window(&st, 800, 600, st.screens[0].root_id);
    map_window(&st);

    /* Prepare a reusable pixel buffer for frames */
    uint32_t width = 800, height = 600;
    size_t pixels_count = (size_t)width * (size_t)height;
    uint32_t *pixels = malloc(pixels_count * 4);
    if (!pixels) FATAL("malloc pixels failed");

    /* Event loop: select on socket with short timeout to periodically refresh */
    while (1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(st.sockfd, &rfds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 16000; /* ~60 FPS pacing */

        int r = select(st.sockfd + 1, &rfds, NULL, NULL, &tv);
        if (r < 0) {
            if (errno == EINTR) continue;
            FATAL("select failed: %s", strerror(errno));
        } else if (r == 0) {
            /* timeout: generate and send a new frame */
            generate_noise_frame_u32(pixels, width, height);
            put_image_32(&st, st.win_id, st.gc_id, width, height, pixels);
            /* ensure server processed it and dispatch events while waiting */
            x11_flush_and_dispatch(&st);
            continue;
        } else {
            if (FD_ISSET(st.sockfd, &rfds)) {
                uint8_t evbuf[32];
                ssize_t n = read(st.sockfd, evbuf, sizeof(evbuf));
                if (n <= 0) FATAL("read failed or connection closed");
                handle_event(&st, evbuf, n);
            }
        }
    }

    free(pixels);
    close(st.sockfd);
    return 0;
}
