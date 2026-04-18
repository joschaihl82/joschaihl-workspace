// xghost.c
// Gespenster-Filter: V4L2 (YUYV) -> RGB24, Ghost-Blending + Glow -> X11 Fenster
// Stabilisiert: korrektes bytes_per_line in XImage, defensive Checks

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 480
#define NUM_BUFFERS 32

typedef struct {
    void *start;
    size_t length;
} Buffer;

static inline int clamp8(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }

// YUYV -> RGB24 (interleaved): src len = w*h*2, dst len = w*h*3
static void yuyv_to_rgb24(uint8_t *src, uint8_t *dst, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x += 2) {
            int i = y * w + x;
            int si = i * 2;

            uint8_t Y0 = src[si + 0];
            uint8_t U  = src[si + 1];
            uint8_t Y1 = src[si + 2];
            uint8_t V  = src[si + 3];

            int C0 = (int)Y0 - 16;
            int C1 = (int)Y1 - 16;
            int D  = (int)U  - 128;
            int E  = (int)V  - 128;

            int R0 = clamp8((298 * C0 + 409 * E + 128) >> 8);
            int G0 = clamp8((298 * C0 - 100 * D - 208 * E + 128) >> 8);
            int B0 = clamp8((298 * C0 + 516 * D + 128) >> 8);

            int R1 = clamp8((298 * C1 + 409 * E + 128) >> 8);
            int G1 = clamp8((298 * C1 - 100 * D - 208 * E + 128) >> 8);
            int B1 = clamp8((298 * C1 + 516 * D + 128) >> 8);

            int di = i * 3;
            dst[di + 0] = (uint8_t)R0; dst[di + 1] = (uint8_t)G0; dst[di + 2] = (uint8_t)B0;
            dst[di + 3] = (uint8_t)R1; dst[di + 4] = (uint8_t)G1; dst[di + 5] = (uint8_t)B1;
        }
    }
}

// Ghost-Blend: prev = prev * decay + current * (1 - decay); einfacher Glow über Gradienten
static void ghost_blend(uint8_t *current, uint8_t *prev, uint8_t *out, float decay, float glow) {
    int w = WIDTH, h = HEIGHT;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 3;

            float pr = (float)prev[idx + 0];
            float pg = (float)prev[idx + 1];
            float pb = (float)prev[idx + 2];

            float cr = (float)current[idx + 0];
            float cg = (float)current[idx + 1];
            float cb = (float)current[idx + 2];

            float nr = pr * decay + cr * (1.0f - decay);
            float ng = pg * decay + cg * (1.0f - decay);
            float nb = pb * decay + cb * (1.0f - decay);

            int xm = (x > 0) ? x - 1 : x;
            int xp = (x < w - 1) ? x + 1 : x;
            int ym = (y > 0) ? y - 1 : y;
            int yp = (y < h - 1) ? y + 1 : y;

            int idxL = (y * w + xm) * 3;
            int idxR = (y * w + xp) * 3;
            int idxU = (ym * w + x) * 3;
            int idxD = (yp * w + x) * 3;

            float dx = (float)current[idxR] - (float)current[idxL];
            float dy = (float)current[idxD] - (float)current[idxU];
            float g  = glow * (fabsf(dx) + fabsf(dy)) * 0.5f;

            int ir = clamp8((int)(nr + g));
            int ig = clamp8((int)(ng + g * 0.7f));
            int ib = clamp8((int)(nb + g * 0.4f));

            out[idx + 0] = (uint8_t)ir;
            out[idx + 1] = (uint8_t)ig;
            out[idx + 2] = (uint8_t)ib;

            prev[idx + 0] = out[idx + 0];
            prev[idx + 1] = out[idx + 1];
            prev[idx + 2] = out[idx + 2];
        }
    }
}

static int set_format(int fd, int w, int h) {
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = w;
    fmt.fmt.pix.height = h;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return -1;
    }
    // Defensive: falls Treiber Größe anpasst, akzeptiere neue Größe
    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        fprintf(stderr, "Gerät unterstützt kein YUYV.\n");
        return -1;
    }
    if (fmt.fmt.pix.width != WIDTH || fmt.fmt.pix.height != HEIGHT) {
        fprintf(stderr, "Warnung: Kamera liefert %ux%u statt %dx%d.\n",
                fmt.fmt.pix.width, fmt.fmt.pix.height, WIDTH, HEIGHT);
        // Für Einfachheit hier abbrechen; alternativ könnte man dynamisch neu allokieren.
        return -1;
    }
    return 0;
}

static int init_mmap(int fd, Buffer *buffers, int *count) {
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = NUM_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return -1;
    }
    if (req.count < 2) {
        fprintf(stderr, "Nicht genug Buffer.\n");
        return -1;
    }

    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = req.type;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            return -1;
        }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            return -1;
        }
    }
    *count = req.count;
    return 0;
}

int main(void) {
    // Video öffnen
    int vfd = open("/dev/video0", O_RDWR);
    if (vfd < 0) { perror("open /dev/video0"); return 1; }
    if (set_format(vfd, WIDTH, HEIGHT) < 0) { close(vfd); return 1; }

    Buffer buffers[NUM_BUFFERS];
    memset(buffers, 0, sizeof(buffers));
    int buf_count = 0;
    if (init_mmap(vfd, buffers, &buf_count) < 0) { close(vfd); return 1; }

    // Buffer enqueuen
    for (int i = 0; i < buf_count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(vfd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return 1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(vfd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return 1; }

    // X11 Setup
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Kann X11 Display nicht öffnen.\n"); return 1; }
    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    Window win = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HEIGHT, 1,
                                     BlackPixel(dpy, screen), WhitePixel(dpy, screen));
    XStoreName(dpy, win, "xghost - Gespensterfilter");
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);

    int depth = DefaultDepth(dpy, screen);
    if (depth < 24) {
        fprintf(stderr, "Unsupported depth %d (benötigt >=24).\n", depth);
        XCloseDisplay(dpy);
        close(vfd);
        return 1;
    }
    Visual *visual = DefaultVisual(dpy, screen);

    // XImage mit korrekter bytes_per_line (WIDTH*4) und ausreichender Datenlänge
    int bpp = 32;
    int bpl = ((WIDTH * (bpp / 8) + 3) & ~3); // 32-bit aligned; meist WIDTH*4
    size_t img_bytes = (size_t)bpl * HEIGHT;

    uint8_t *img_data = (uint8_t*)malloc(img_bytes);
    if (!img_data) {
        fprintf(stderr, "Speicherfehler für XImage-Daten.\n");
        XCloseDisplay(dpy);
        close(vfd);
        return 1;
    }

    XImage *ximg = XCreateImage(dpy, visual, depth, ZPixmap, 0,
                                (char*)img_data, WIDTH, HEIGHT, 32, bpl);
    if (!ximg || !ximg->data) {
        fprintf(stderr, "XCreateImage fehlgeschlagen.\n");
        free(img_data);
        XCloseDisplay(dpy);
        close(vfd);
        return 1;
    }
    GC gc = DefaultGC(dpy, screen);

    // Framebuffer (RGB24) und Akkumulator
    uint8_t *rgb = (uint8_t*)malloc(WIDTH * HEIGHT * 3);
    uint8_t *acc = (uint8_t*)calloc(WIDTH * HEIGHT * 3, 1);
    uint8_t *out = (uint8_t*)malloc(WIDTH * HEIGHT * 3);
    if (!rgb || !acc || !out) {
        fprintf(stderr, "Speicherfehler.\n");
        if (ximg) XDestroyImage(ximg); // gibt img_data frei
        XCloseDisplay(dpy);
        close(vfd);
        free(rgb); free(acc); free(out);
        return 1;
    }

    float decay = 0.92f; // höher = längeres Nachleuchten
    float glow  = 0.25f; // Kantenverstärkung
    int running = 1;

    // Event- und Capture-Loop
    while (running) {
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_Escape || ks == XK_q || ks == XK_Q) running = 0;
            } else if (ev.type == DestroyNotify) {
                running = 0;
            }
        }

        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(vfd, VIDIOC_DQBUF, &buf) < 0) {
            if (errno == EAGAIN) continue;
            perror("VIDIOC_DQBUF");
            break;
        }

        // Sicherheitscheck: Länge ausreichend?
        if (buffers[buf.index].length < (size_t)(WIDTH * HEIGHT * 2)) {
            fprintf(stderr, "Buffer zu klein: %zu (< %d).\n",
                    buffers[buf.index].length, WIDTH * HEIGHT * 2);
            // Re-Queue und sauber abbrechen
            ioctl(vfd, VIDIOC_QBUF, &buf);
            break;
        }

        // YUYV -> RGB
        yuyv_to_rgb24((uint8_t*)buffers[buf.index].start, rgb, WIDTH, HEIGHT);

        // Ghost-Blend + Glow
        ghost_blend(rgb, acc, out, decay, glow);

        // RGB24 -> XImage (BGRX häufig richtig, bei verdrehten Farben R/B tauschen)
        uint8_t *dst = (uint8_t*)ximg->data;
        int x_bpl = ximg->bytes_per_line; // Server/Lib kann alignment vorgeben
        for (int y = 0; y < HEIGHT; y++) {
            uint8_t *row = dst + y * x_bpl;
            for (int x = 0; x < WIDTH; x++) {
                int i = y * WIDTH + x;
                int o = i * 3;
                int d = x * 4;
                row[d + 0] = out[o + 2]; // B
                row[d + 1] = out[o + 1]; // G
                row[d + 2] = out[o + 0]; // R
                row[d + 3] = 0x00;       // Padding
            }
        }

        // Zeichnen
        XPutImage(dpy, win, gc, ximg, 0, 0, 0, 0, WIDTH, HEIGHT);
        XFlush(dpy);

        // Buffer zurückgeben
        if (ioctl(vfd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); break; }
    }

    // Cleanup
    if (ximg) XDestroyImage(ximg); // gibt img_data frei
    XCloseDisplay(dpy);

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(vfd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < buf_count; i++) {
        munmap(buffers[i].start, buffers[i].length);
    }
    close(vfd);
    free(rgb); free(acc); free(out);
    return 0;
}

