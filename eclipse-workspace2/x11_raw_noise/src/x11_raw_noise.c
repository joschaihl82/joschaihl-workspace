// raw_x11_noise.c
// Build: gcc -std=c11 -O2 -Wall raw_x11_noise.c -o raw_x11_noise
// Run:   ./raw_x11_noise
//
// Minimal raw X11 client over /tmp/.X11-unix/X0 that creates an 800x600 window
// and repeatedly sends PutImage requests with random pixels.
// This program uses only standard C and POSIX headers (no libX11/libxcb).

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <errno.h>

#define DISPLAY_SOCKET "/tmp/.X11-unix/X0"
#define READ_BUF (16*1024)
#define WIDTH 800
#define HEIGHT 600

// X11 opcodes used
#define X_CreateWindow 1
#define X_MapWindow 8
#define X_OpenFont 45
#define X_CreateGC 55
#define X_PutImage 72

// helper: pad to 4 bytes
static inline int pad4(int n) { return (4 - (n & 3)) & 3; }

// fatal helper
static void die(const char *msg) {
    perror(msg);
    exit(1);
}

// write all helper
static ssize_t write_all(int fd, const void *buf, size_t len) {
    const uint8_t *p = buf;
    size_t left = len;
    while (left) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        left -= (size_t)w;
        p += w;
    }
    return (ssize_t)len;
}

// read exactly helper (returns bytes read or -1)
static ssize_t read_exact(int fd, void *buf, size_t len) {
    uint8_t *p = buf;
    size_t left = len;
    while (left) {
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) return (ssize_t)(len - left);
        left -= (size_t)r;
        p += r;
    }
    return (ssize_t)len;
}

// get next resource id from server-provided base/mask
static uint32_t next_id(uint32_t *base, uint32_t *mask, uint32_t *counter) {
    uint32_t id = (*counter & *mask) | *base;
    (*counter)++;
    return id;
}

// perform X11 initial handshake (Init) and parse minimal setup
// returns 0 on success and fills resource base/mask/root/visual
static int x11_init(int fd, uint32_t *res_base, uint32_t *res_mask,
                    uint32_t *root_window, uint32_t *root_visual) {
    // Build InitRequest (12 bytes). Use little-endian 'l' and protocol major 11.
    uint8_t init_req[12] = { 'l', 0, 11, 0, 0,0,0,0, 0,0,0,0 };
    if (write_all(fd, init_req, sizeof(init_req)) != (ssize_t)sizeof(init_req)) {
        return -1;
    }

    // Read first 8 bytes of setup response
    uint8_t hdr[8];
    if (read_exact(fd, hdr, 8) != 8) return -1;

    if (hdr[0] == 0) {
        // failed - read remaining length (bytes 6..7 give length in 4-byte units)
        uint16_t length_units = (hdr[6] << 8) | hdr[7];
        size_t toread = (size_t)length_units * 4;
        uint8_t *tmp = malloc(toread + 1);
        if (!tmp) return -1;
        read_exact(fd, tmp, toread);
        free(tmp);
        return -1;
    } else if (hdr[0] == 2) {
        // authenticate required
        return -1;
    } else if (hdr[0] == 1) {
        // success: read remaining setup (length in 4-byte units at hdr[6..7])
        uint16_t length_units = (hdr[6] << 8) | hdr[7];
        size_t toread = (size_t)length_units * 4;
        uint8_t *setup = malloc(toread + 1);
        if (!setup) return -1;
        if (read_exact(fd, setup, toread) != (ssize_t)toread) {
            free(setup);
            return -1;
        }

        // parse minimal fields:
        // resource id base at offset 8+4 = 12 in full setup (8 hdr + 4 bytes into setup)
        // But the layout: after the initial 8 bytes we read 'toread' bytes which start at offset 8.
        // ResourceIdBase is at offset 12 from start of full reply => offset 4 into 'setup'.
        if (toread < 24) { free(setup); return -1; }
        uint32_t resource_base = *(uint32_t*)(setup + 4);
        uint32_t resource_mask = *(uint32_t*)(setup + 8);
        // vendor length at offset 16 (2 bytes) and number of formats at offset 21
        uint16_t vendor_len = *(uint16_t*)(setup + 12);
        uint8_t formats = setup[17];
        // compute screens start: 40 + vendor_len + pad + formats*8
        size_t vendor_pad = pad4(vendor_len);
        size_t formats_bytes = (size_t)formats * 8;
        size_t screens_offset = 40 + vendor_len + vendor_pad + formats_bytes;
        if (toread < screens_offset + 32) { free(setup); return -1; }
        uint32_t root = *(uint32_t*)(setup + screens_offset);
        uint32_t root_visual_id = *(uint32_t*)(setup + screens_offset + 32);

        *res_base = resource_base;
        *res_mask = resource_mask;
        *root_window = root;
        *root_visual = root_visual_id;

        free(setup);
        return 0;
    }
    return -1;
}

// create a simple window with background pixel and event mask
static uint32_t x11_create_window(int fd, uint32_t id, uint32_t parent,
                                  uint32_t visual, int x, int y, int w, int h) {
    // We'll send CreateWindow request with a few value fields:
    // Format: opcode(1), depth(1), length(2), wid(4), parent(4), x(2), y(2), w(2), h(2),
    // border(2), class(2), visual(4), value-mask(4), value-list...
    uint8_t buf[64];
    memset(buf, 0, sizeof(buf));
    buf[0] = X_CreateWindow;
    buf[1] = 0; // depth = copy from parent (0)
    *(uint16_t*)(buf + 2) = 8 + 2; // request length in 4-byte units (header 8 + 2 words of values)
    *(uint32_t*)(buf + 4) = id;
    *(uint32_t*)(buf + 8) = parent;
    *(int16_t*)(buf + 12) = (int16_t)x;
    *(int16_t*)(buf + 14) = (int16_t)y;
    *(int16_t*)(buf + 16) = (int16_t)w;
    *(int16_t*)(buf + 18) = (int16_t)h;
    *(int16_t*)(buf + 20) = 0; // border width
    *(int16_t*)(buf + 22) = 1; // InputOutput
    *(uint32_t*)(buf + 24) = visual;
    // value mask: background pixel and event mask
    uint32_t value_mask = (1 << 1) | (1 << 11); // BackgroundPixel = bit1, EventMask = bit11
    *(uint32_t*)(buf + 28) = value_mask;
    // values: background pixel (0x000000) and event mask (Exposure | KeyPress)
    *(uint32_t*)(buf + 32) = 0x000000; // black background
    *(uint32_t*)(buf + 36) = (1 << 15) | (1 << 0); // ExposureMask | KeyPressMask

    if (write_all(fd, buf, (8 + 2) * 4) < 0) die("write CreateWindow");
    return id;
}

static void x11_map_window(int fd, uint32_t win) {
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));
    buf[0] = X_MapWindow;
    buf[1] = 0;
    *(uint16_t*)(buf + 2) = 2; // length
    *(uint32_t*)(buf + 4) = win;
    if (write_all(fd, buf, 8) < 0) die("write MapWindow");
}

static void x11_create_gc(int fd, uint32_t gcid, uint32_t drawable) {
    // CreateGC with foreground and background values and no font
    uint8_t buf[28];
    memset(buf, 0, sizeof(buf));
    buf[0] = X_CreateGC;
    buf[1] = 0;
    *(uint16_t*)(buf + 2) = 4 + 3; // header + 3 value words
    *(uint32_t*)(buf + 4) = gcid;
    *(uint32_t*)(buf + 8) = drawable;
    // value mask: FG (bit2), BG (bit3)
    *(uint32_t*)(buf + 12) = (1 << 2) | (1 << 3);
    *(uint32_t*)(buf + 16) = 0x00FFFFFF; // foreground white
    *(uint32_t*)(buf + 20) = 0x00000000; // background black
    if (write_all(fd, buf, (4 + 3) * 4) < 0) die("write CreateGC");
}

// send PutImage ZPixmap with 32-bit pixels (B G R A)
static void x11_put_image(int fd, uint32_t drawable, uint32_t gc,
                          int width, int height, const uint8_t *pixels, int depth) {
    // format = 2 (ZPixmap)
    int format = 2;
    int bpp = 4; // bytes per pixel we send
    int image_bytes = width * height * bpp;
    int request_length = 6 + ((image_bytes + 3) / 4);
    // build header
    size_t header_bytes = 6 * 4;
    uint8_t *buf = malloc((size_t)request_length * 4);
    if (!buf) die("malloc putimage");
    memset(buf, 0, (size_t)request_length * 4);
    buf[0] = X_PutImage;
    buf[1] = (uint8_t)format;
    *(uint16_t*)(buf + 2) = (uint16_t)request_length;
    *(uint32_t*)(buf + 4) = drawable;
    *(uint32_t*)(buf + 8) = gc;
    *(uint16_t*)(buf + 12) = (uint16_t)width;
    *(uint16_t*)(buf + 14) = (uint16_t)height;
    *(int16_t*)(buf + 16) = 0; // dst-x
    *(int16_t*)(buf + 18) = 0; // dst-y
    buf[20] = (uint8_t)depth; // depth
    // copy pixels into buffer at offset 24
    memcpy(buf + 24, pixels, (size_t)image_bytes);
    // write
    if (write_all(fd, buf, (size_t)request_length * 4) < 0) {
        free(buf);
        die("write PutImage");
    }
    free(buf);
}

int main(void) {
    srand((unsigned)time(NULL));

    // open unix domain socket to X0
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) die("socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, DISPLAY_SOCKET, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) die("connect");

    uint32_t res_base = 0, res_mask = 0, root = 0, root_visual = 0;
    if (x11_init(fd, &res_base, &res_mask, &root, &root_visual) != 0) {
        fprintf(stderr, "X init failed or requires auth\n");
        close(fd);
        return 1;
    }

    uint32_t id_counter = 1;
    uint32_t win = next_id(&res_base, &res_mask, &id_counter);
    uint32_t gc = next_id(&res_base, &res_mask, &id_counter);

    // create and map window at 100,100 size WIDTHxHEIGHT
    x11_create_window(fd, win, root, root_visual, 100, 100, WIDTH, HEIGHT);
    x11_map_window(fd, win);

    // create a GC
    x11_create_gc(fd, gc, win);

    // allocate pixel buffer (B G R A)
    size_t pixels_size = (size_t)WIDTH * (size_t)HEIGHT * 4;
    uint8_t *pixels = malloc(pixels_size);
    if (!pixels) die("malloc pixels");

    // poll for events and continuously send PutImage
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    while (1) {
        // non-blocking poll with short timeout so we can update continuously
        int ret = poll(&pfd, 1, 10);
        if (ret > 0) {
            if (pfd.revents & (POLLHUP | POLLERR)) break;
            if (pfd.revents & POLLIN) {
                // read one event (X11 events are 32 bytes typically; read available bytes)
                uint8_t ev[32];
                ssize_t r = read(fd, ev, sizeof(ev));
                if (r <= 0) break;
                // handle simple events: KeyPress (2) -> exit
                uint8_t code = ev[0];
                if (code == 2) break;
                // ignore others
            }
        }

        // fill pixels with random noise (B G R A)
        for (size_t i = 0; i < (size_t)WIDTH * (size_t)HEIGHT; ++i) {
            size_t idx = i * 4;
            pixels[idx + 0] = (uint8_t)(rand() & 0xFF); // B
            pixels[idx + 1] = (uint8_t)(rand() & 0xFF); // G
            pixels[idx + 2] = (uint8_t)(rand() & 0xFF); // R
            pixels[idx + 3] = 0xFF;                     // padding/alpha
        }

        // send PutImage (depth 24)
        x11_put_image(fd, win, gc, WIDTH, HEIGHT, pixels, 24);

        // throttle ~60 FPS
        struct timespec ts = {0, 16000000};
        nanosleep(&ts, NULL);
    }

    free(pixels);
    close(fd);
    return 0;
}
