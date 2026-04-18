// xnoise_raw.c
// Compile: gcc -o xnoise_raw xnoise_raw.c
// Run: ./xnoise_raw
//
// Uses raw X11 protocol over AF_UNIX socket to create an 800x600 window
// and stream white/black pixel noise by sending PutImage and CopyArea.

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xauth.h>
#include <X11/Xproto.h>

#define WIDTH 800
#define HEIGHT 600
#define FPS 30

#define X_CONNECTION_FAIL 0
#define X_CONNECTION_OKAY 1
#define X_CONNECTION_AUTH 2

#define Write(fd, buf, n) ({                      \
        if (write(fd, buf, n) != (ssize_t)(n)) die("write"); \
    })
#define Read(fd, buf, n) ({                       \
        if (read(fd, buf, n) != (ssize_t)(n)) die("read");   \
    })
#define Malloc(n) ({                              \
        void *p = malloc(n);                      \
        if (!p) die("malloc");                    \
        p;                                        \
    })

void die(const char *msg) {
    perror(msg);
    exit(1);
}

/* Minimal structures used by this program (subset of original) */
struct x_header_prefix {
    uint8_t status;
    uint8_t reason_len;
    uint16_t length;
    uint16_t major;
    uint16_t minor;
    uint16_t pad;
} __attribute__((packed));

struct x_header {
    uint16_t release;
    uint16_t resource_id_base;
    uint16_t resource_id_mask;
    uint32_t motion_buffer;
    uint16_t vendor_len;
    uint8_t num_pixmap_format;
    uint8_t num_screen;
    uint8_t pad[18];
    /* dynamic fields follow in wire protocol */
    char *vendor;
    void *pixmap_formats;
    struct x_screen *roots;
} __attribute__((packed));

struct x_screen {
    uint32_t root;
    uint32_t default_colormap;
    uint32_t white_pixel;
    uint32_t black_pixel;
    uint32_t current_input_masks;
    uint32_t width_in_pixels;
    uint32_t height_in_pixels;
    uint32_t width_in_millimeters;
    uint32_t height_in_millimeters;
    uint32_t min_installed_maps;
    uint32_t max_installed_maps;
    uint32_t root_visual;
    uint8_t backing_stores;
    uint8_t save_unders;
    uint8_t root_depth;
    uint8_t num_depth;
    /* dynamic depths follow */
    struct x_depth *depths;
} __attribute__((packed));

struct x_depth {
    uint8_t depth;
    uint8_t pad[3];
    uint16_t num_visual;
    uint8_t pad2[4];
    struct x_visual *visuals;
} __attribute__((packed));

struct x_visual {
    uint32_t visual_id;
    uint8_t class;
    uint8_t bits_per_rgb;
    uint16_t colormap_entries;
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint8_t pad[4];
} __attribute__((packed));

/* Read 16-bit big-endian value from file (used for .Xauthority parsing) */
uint16_t read_short(FILE *fp) {
    uint8_t buf[2];
    if (fread(buf, 1, 2, fp) != 2) {
        if (feof(fp)) {
            fprintf(stderr, "ERROR: end of file\n");
            exit(1);
        } else {
            die("fread");
        }
    }
    return (buf[0] << 8) | buf[1];
}

/* Read MIT-MAGIC-COOKIE-1 from ~/.Xauthority (FamilyLocal entry) */
int read_magic_cookie(char *dest, int dest_size) {
    int ret = 0;
    char xauth_fname[256];
    snprintf(xauth_fname, sizeof xauth_fname, "%s/.Xauthority", getenv("HOME"));
    FILE *fp = fopen(xauth_fname, "rb");
    if (!fp) {
        fprintf(stderr, "ERROR: can't open Xauthority file\n");
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    ssize_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    while (ftell(fp) < fsize) {
        uint16_t family = read_short(fp);
        uint16_t addr_len = read_short(fp);
        fseek(fp, addr_len, SEEK_CUR);
        uint16_t num_len = read_short(fp);
        fseek(fp, num_len, SEEK_CUR);
        uint16_t name_len = read_short(fp);
        fseek(fp, name_len, SEEK_CUR);
        uint16_t data_len = read_short(fp);
        if (family == FamilyLocal) {
            if (data_len > dest_size) {
                fclose(fp);
                return 0;
            }
            fread(dest, 1, data_len, fp);
            ret = 1;
            break;
        } else {
            fseek(fp, data_len, SEEK_CUR);
        }
    }
    fclose(fp);
    return ret;
}

/* Connect and perform X protocol handshake; fill x_conn with server info */
void x_connect(int xfd, struct x_header *x_conn) {
    struct {
        char byte_order;
        char pad_1;
        uint16_t version_major;
        uint16_t version_minor;
        uint16_t len_auth_proto_name;
        uint16_t len_auth_proto_data;
        uint16_t pad_2;
        char auth_proto_name[20];
        char auth_proto_data[16];
    } __attribute__((packed)) header = {
        .byte_order = 'l', /* little-endian client */
        .version_major = X_PROTOCOL,
        .version_minor = X_PROTOCOL_REVISION,
        .len_auth_proto_name = 18,
        .len_auth_proto_data = 16,
        .auth_proto_name = "MIT-MAGIC-COOKIE-1",
    };

    static_assert(sizeof(header) == 48, "wrong size for header");

    if (!read_magic_cookie(header.auth_proto_data, 16)) {
        fprintf(stderr, "ERROR: can't read cookie from Xauthority file\n");
        exit(1);
    }

    Write(xfd, &header, sizeof header);

    struct x_header_prefix x_conn_prefix;
    Read(xfd, &x_conn_prefix, sizeof x_conn_prefix);

    if (x_conn_prefix.status != X_CONNECTION_OKAY) {
        char *reason = Malloc(x_conn_prefix.length * 4);
        Read(xfd, reason, x_conn_prefix.length * 4);
        fprintf(stderr, "ERROR: can't connect to X server (%d): %.*s\n", x_conn_prefix.status,
                x_conn_prefix.reason_len, reason);
        free(reason);
        exit(1);
    }

    /* read fixed 32 bytes of initial header (we only use a few fields) */
    uint8_t fixed[32];
    Read(xfd, fixed, 32);

    /* parse a few fields we need (resource id base and mask, vendor len, screens) */
    /* offsets based on X11 initial connection reply */
    uint32_t release = *(uint32_t *)(fixed + 0);
    uint32_t rid_base = *(uint32_t *)(fixed + 4);
    uint32_t rid_mask = *(uint32_t *)(fixed + 8);
    uint8_t vendor_len = *(uint8_t *)(fixed + 12);
    uint8_t num_pixmap_format = *(uint8_t *)(fixed + 13);
    uint8_t num_screen = *(uint8_t *)(fixed + 14);

    /* fill x_conn minimal fields */
    x_conn->resource_id_base = (uint16_t)(rid_base & 0xffff);
    x_conn->resource_id_mask = (uint16_t)(rid_mask & 0xffff);
    x_conn->vendor_len = vendor_len;
    x_conn->num_pixmap_format = num_pixmap_format;
    x_conn->num_screen = num_screen;

    /* read vendor string (padded to 4) */
    int vendor_len_padded = (vendor_len + 3) / 4 * 4;
    x_conn->vendor = Malloc(vendor_len_padded + 1);
    Read(xfd, x_conn->vendor, vendor_len_padded);
    x_conn->vendor[vendor_len] = '\0';

    /* skip pixmap formats */
    int px_fmt_len = num_pixmap_format * 8;
    if (px_fmt_len)
        x_conn->pixmap_formats = Malloc(px_fmt_len), Read(xfd, x_conn->pixmap_formats, px_fmt_len);

    /* read first screen only (we assume screen 0) */
    /* The screen structure is variable; for simplicity read a chunk and extract root, white_pixel, root_visual */
    /* We'll read 40 bytes which covers the fixed part for typical servers */
    uint8_t screen_buf[40];
    Read(xfd, screen_buf, sizeof screen_buf);

    uint32_t root = *(uint32_t *)(screen_buf + 0);
    uint32_t default_colormap = *(uint32_t *)(screen_buf + 4);
    uint32_t white_pixel = *(uint32_t *)(screen_buf + 8);
    uint32_t black_pixel = *(uint32_t *)(screen_buf + 12);
    uint32_t root_visual = *(uint32_t *)(screen_buf + 24);
    uint8_t root_depth = *(uint8_t *)(screen_buf + 28);
    uint8_t num_depth = *(uint8_t *)(screen_buf + 29);

    x_conn->roots = Malloc(sizeof(struct x_screen));
    x_conn->roots[0].root = root;
    x_conn->roots[0].white_pixel = white_pixel;
    x_conn->roots[0].black_pixel = black_pixel;
    x_conn->roots[0].root_visual = root_visual;
    x_conn->roots[0].root_depth = root_depth;
    x_conn->roots[0].num_depth = num_depth;
}

/* Create an 800x600 window */
uint32_t x_create_window(int xfd, struct x_header *x_conn) {
    uint32_t wid = (uint32_t)(x_conn->resource_id_base + 1);
    struct x_screen *screen = &x_conn->roots[0];

    struct {
        uint8_t opcode;
        uint8_t depth;
        uint16_t req_len;
        uint32_t wid;
        uint32_t parent;
        int16_t x, y;
        uint16_t width, height;
        uint16_t border_width;
        uint16_t window_class;
        uint32_t visual;
        uint32_t value_mask;
        uint32_t values[2];
    } __attribute__((packed)) req_create_window = {
        .opcode = X_CreateWindow,
        .depth = CopyFromParent,
        .req_len = 10,
        .wid = wid,
        .parent = screen->root,
        .x = 0, .y = 0,
        .width = WIDTH, .height = HEIGHT,
        .border_width = 0,
        .window_class = InputOutput,
        .visual = screen->root_visual,
        .value_mask = CWBackPixel | CWEventMask,
        .values = { screen->white_pixel, PointerMotionMask | KeyReleaseMask | ExposureMask | StructureNotifyMask }
    };

    Write(xfd, &req_create_window, sizeof req_create_window);
    return wid;
}

/* Map window */
void x_map_window(int xfd, uint32_t wid) {
    struct {
        uint8_t opcode;
        uint8_t pad;
        uint16_t req_len;
        uint32_t window;
    } __attribute__((packed)) req_map = {
        .opcode = X_MapWindow,
        .req_len = 2,
        .window = wid,
    };
    Write(xfd, &req_map, sizeof req_map);
}

/* Create a pixmap resource */
uint32_t x_create_pixmap(int xfd, struct x_header *x_conn, uint32_t drawable, uint16_t width, uint16_t height, uint8_t depth) {
    uint32_t pid = x_conn->resource_id_base + 2;
    struct {
        uint8_t opcode;
        uint8_t pad;
        uint16_t req_len;
        uint32_t pid;
        uint32_t drawable;
        uint16_t width;
        uint16_t height;
    } __attribute__((packed)) req = {
        .opcode = X_CreatePixmap,
        .req_len = 4,
        .pid = pid,
        .drawable = drawable,
        .width = width,
        .height = height
    };
    /* depth is the first byte after opcode in CreatePixmap request; set it manually */
    uint8_t buf[16];
    memcpy(buf, &req, sizeof req);
    buf[1] = depth;
    Write(xfd, buf, sizeof req);
    return pid;
}

/* CopyArea: copy pixmap to window */
void x_copy_area(int xfd, uint32_t src, uint32_t dst, uint32_t gc, uint16_t width, uint16_t height) {
    struct {
        uint8_t opcode;
        uint8_t pad;
        uint16_t req_len;
        uint32_t src;
        uint32_t dst;
        uint32_t gc;
        int16_t src_x, src_y;
        int16_t dst_x, dst_y;
        uint16_t width, height;
    } __attribute__((packed)) req = {
        .opcode = X_CopyArea,
        .req_len = 8,
        .src = src,
        .dst = dst,
        .gc = gc,
        .src_x = 0, .src_y = 0,
        .dst_x = 0, .dst_y = 0,
        .width = width, .height = height
    };
    Write(xfd, &req, sizeof req);
}

/* Create a simple GC (graphics context) */
uint32_t x_create_gc(int xfd, struct x_header *x_conn, uint32_t drawable) {
    uint32_t gcid = x_conn->resource_id_base + 3;
    struct {
        uint8_t opcode;
        uint8_t pad;
        uint16_t req_len;
        uint32_t gc;
        uint32_t drawable;
        uint32_t value_mask;
        uint32_t values[1];
    } __attribute__((packed)) req = {
        .opcode = X_CreateGC,
        .req_len = 6,
        .gc = gcid,
        .drawable = drawable,
        .value_mask = 0,
        .values = { 0 }
    };
    Write(xfd, &req, sizeof req);
    return gcid;
}

/* Send PutImage (ZPixmap) with raw pixel data (32-bit per pixel, native endianness) */
void x_put_image(int xfd, uint32_t drawable, uint8_t depth, uint16_t width, uint16_t height, const void *data, size_t data_bytes) {
    /* PutImage request header is 16 bytes; data follows and must be padded to 4 bytes */
    size_t padded = (data_bytes + 3) / 4 * 4;
    uint16_t req_len = (uint16_t)((16 + padded) / 4);

    /* Build header */
    uint8_t header[16];
    memset(header, 0, sizeof header);
    header[0] = X_PutImage;       /* opcode 72 */
    header[1] = 2;               /* format = ZPixmap (2) */
    header[2] = req_len >> 8;
    header[3] = req_len & 0xff;
    /* next fields in little-endian client mode; we will write as little-endian directly */
    /* but to be consistent with earlier code which writes struct directly, we craft a packed struct */
    struct {
        uint8_t opcode;
        uint8_t format;
        uint16_t req_len;
        uint32_t drawable;
        int16_t x, y;
        uint16_t width, height;
        uint8_t left_pad;
        uint8_t depth;
        uint16_t pad;
    } __attribute__((packed)) req = {
        .opcode = X_PutImage,
        .format = 2, /* ZPixmap */
        .req_len = req_len,
        .drawable = drawable,
        .x = 0, .y = 0,
        .width = width, .height = height,
        .left_pad = 0,
        .depth = depth,
        .pad = 0
    };

    /* write header struct */
    Write(xfd, &req, sizeof req);

    /* write pixel data padded to 4 bytes */
    Write(xfd, data, data_bytes);
    if (padded > data_bytes) {
        uint8_t pad[3] = {0,0,0};
        Write(xfd, pad, padded - data_bytes);
    }
}

/* Read and handle events; return 1 to exit loop */
int x_read_event(int xfd) {
    uint8_t header[32];
    ssize_t n = read(xfd, header, sizeof header);
    if (n == -1) die("read");
    if (n == 0) return 1;

    uint8_t type = header[0];
    if (type == X_Error) {
        fprintf(stderr, "X protocol error: code=%u\n", header[1]);
        return 1;
    } else if (type == KeyRelease) {
        /* keycode in header[1]; X keycodes are offset from linux; Escape often keycode 9 in earlier example */
        if (header[1] == 9) return 1;
    } else if (type == ClientMessage) {
        return 1;
    }
    /* ignore other events */
    return 0;
}

int main(void) {
    int xfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (xfd == -1) die("socket");
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = "/tmp/.X11-unix/X0",
    };
    if (connect(xfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
        die("connect");

    struct x_header x_conn;
    memset(&x_conn, 0, sizeof x_conn);
    x_connect(xfd, &x_conn);

    /* create window */
    uint32_t win = x_create_window(xfd, &x_conn);
    x_map_window(xfd, win);

    /* create pixmap and gc */
    uint8_t depth = x_conn.roots[0].root_depth ? x_conn.roots[0].root_depth : 24;
    uint32_t pixmap = x_create_pixmap(xfd, &x_conn, win, WIDTH, HEIGHT, depth);
    uint32_t gc = x_create_gc(xfd, &x_conn, win);

    /* allocate pixel buffer: use 32-bit per pixel (native endianness) */
    const int bpp = 4;
    size_t buf_bytes = (size_t)WIDTH * HEIGHT * bpp;
    uint8_t *buf = Malloc(buf_bytes);

    srand((unsigned)time(NULL));
    const useconds_t frame_delay = 1000000 / FPS;

    /* Main loop: generate random white/black pixels, PutImage to pixmap, CopyArea to window */
    int running = 1;
    while (running) {
        /* generate noise: white = 0x00FFFFFF, black = 0x00000000 (native endianness assumed little) */
        uint32_t *p32 = (uint32_t *)buf;
        size_t total = (size_t)WIDTH * HEIGHT;
        for (size_t i = 0; i < total; ++i) {
            if (rand() & 1)
                p32[i] = 0x00FFFFFFu;
            else
                p32[i] = 0x00000000u;
        }

        /* PutImage into pixmap (ZPixmap) */
        x_put_image(xfd, pixmap, depth, WIDTH, HEIGHT, buf, buf_bytes);

        /* Copy pixmap to window */
        x_copy_area(xfd, pixmap, win, gc, WIDTH, HEIGHT);

        /* flush by reading any pending events non-blocking */
        /* use select with timeout to allow event processing and frame pacing */
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(xfd, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = frame_delay;

        int sel = select(xfd + 1, &rfds, NULL, NULL, &tv);
        if (sel > 0 && FD_ISSET(xfd, &rfds)) {
            /* read and handle one event; if it signals exit, break */
            if (x_read_event(xfd)) running = 0;
        }

        /* continue loop; frame pacing handled by select timeout */
    }

    free(buf);
    close(xfd);
    return 0;
}
