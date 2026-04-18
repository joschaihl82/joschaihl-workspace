// xghost.c
// Gespenster-Filter: V4L2 (YUYV) -> RGB24, Ghost-Blending + Glow -> X11 Fenster
// Features: Resizable Window, Aspect-Fit (Letterboxing), Vollbild-Start, ESC/Q beendet

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

#define SRC_W 640
#define SRC_H 480
#define NUM_BUFFERS 4

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
    int w = SRC_W, h = SRC_H;
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

// Einfacher nearest-neighbor-Scaler: RGB24 -> BGRX im Zielrechteck (letterbox)
static void blit_aspect_fit_to_ximage(uint8_t *src_rgb, int src_w, int src_h,
                                      XImage *ximg, int win_w, int win_h) {
    // Ziel: größtmögliches Rechteck im Fenster mit korrektem Verhältnis
    double scale = fmin((double)win_w / (double)src_w, (double)win_h / (double)src_h);
    int dst_w = (int)floor(src_w * scale);
    int dst_h = (int)floor(src_h * scale);
    int off_x = (win_w - dst_w) / 2;
    int off_y = (win_h - dst_h) / 2;

    // Hintergrund schwarz füllen (Letterbox-Balken)
    uint8_t *dst = (uint8_t*)ximg->data;
    int bpl = ximg->bytes_per_line;
    for (int y = 0; y < win_h; y++) {
        uint8_t *row = dst + y * bpl;
        for (int x = 0; x < win_w; x++) {
            int d = x * 4;
            row[d + 0] = 0x00; // B
            row[d + 1] = 0x00; // G
            row[d + 2] = 0x00; // R
            row[d + 3] = 0x00; // X
        }
    }

    if (dst_w <= 0 || dst_h <= 0) return;

    // Skalieren: RGB24 -> BGRX
    for (int y = 0; y < dst_h; y++) {
        int sy = (int)((double)y * src_h / dst_h);
        if (sy >= src_h) sy = src_h - 1;
        uint8_t *row = dst + (off_y + y) * bpl;
        for (int x = 0; x < dst_w; x++) {
            int sx = (int)((double)x * src_w / dst_w);
            if (sx >= src_w) sx = src_w - 1;

            int si = (sy * src_w + sx) * 3;
            int di = (off_x + x) * 4;
            // BGRX im XImage (häufig richtig; bei invertierten Farben R/B tauschen)
            row[di + 0] = src_rgb[si + 2]; // B
            row[di + 1] = src_rgb[si + 1]; // G
            row[di + 2] = src_rgb[si + 0]; // R
            row[di + 3] = 0x00;            // X
        }
    }
}

// Vollbild anfordern via EWMH (_NET_WM_STATE_FULLSCREEN)
static void request_fullscreen(Display *dpy, Window win) {
    Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.xclient.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 1;
    xev.xclient.data.l[4] = 0;

    XSendEvent(dpy, DefaultRootWindow(dpy), False,
               SubstructureNotifyMask | SubstructureRedirectMask, &xev);
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
    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        fprintf(stderr, "Gerät unterstützt kein YUYV.\n");
        return -1;
    }
    if (fmt.fmt.pix.width != w || fmt.fmt.pix.height != h) {
        fprintf(stderr, "Warnung: Kamera liefert %ux%u statt %dx%d.\n",
                fmt.fmt.pix.width, fmt.fmt.pix.height, w, h);
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
    if (set_format(vfd, SRC_W, SRC_H) < 0) { close(vfd); return 1; }

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

    // Erstellbares, vergrößerbares Fenster
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;
    Window win = XCreateWindow(dpy, root, 100, 100, SRC_W, SRC_H, 0,
                               DefaultDepth(dpy, screen),
                               InputOutput,
                               DefaultVisual(dpy, screen),
                               CWEventMask, &swa);
    XStoreName(dpy, win, "xghost - Gespensterfilter (Fullscreen mit ESC/Q beenden)");
    XMapWindow(dpy, win);

    // Vollbild anfordern
    request_fullscreen(dpy, win);

    int win_w = SRC_W, win_h = SRC_H;
    int depth = DefaultDepth(dpy, screen);
    if (depth < 24) {
        fprintf(stderr, "Unsupported depth %d (benötigt >=24).\n", depth);
        XCloseDisplay(dpy);
        close(vfd);
        return 1;
    }
    Visual *visual = DefaultVisual(dpy, screen);

    // XImage anlegen (wird bei Resize neu erstellt)
    int bpp = 32;
    int bpl = ((win_w * (bpp / 8) + 3) & ~3);
    size_t img_bytes = (size_t)bpl * win_h;
    uint8_t *img_data = (uint8_t*)malloc(img_bytes);
    if (!img_data) {
        fprintf(stderr, "Speicherfehler für XImage-Daten.\n");
        XCloseDisplay(dpy);
        close(vfd);
        return 1;
    }
    XImage *ximg = XCreateImage(dpy, visual, depth, ZPixmap, 0, (char*)img_data,
                                win_w, win_h, 32, bpl);
    if (!ximg || !ximg->data) {
        fprintf(stderr, "XCreateImage fehlgeschlagen.\n");
        free(img_data);
        XCloseDisplay(dpy);
        close(vfd);
        return 1;
    }
    GC gc = DefaultGC(dpy, screen);

    // Framebuffer (RGB24) und Akkumulator
    uint8_t *rgb = (uint8_t*)malloc(SRC_W * SRC_H * 3);
    uint8_t *acc = (uint8_t*)calloc(SRC_W * SRC_H * 3, 1);
    uint8_t *out = (uint8_t*)malloc(SRC_W * SRC_H * 3);
    if (!rgb || !acc || !out) {
        fprintf(stderr, "Speicherfehler.\n");
        if (ximg) XDestroyImage(ximg);
        XCloseDisplay(dpy);
        close(vfd);
        free(rgb); free(acc); free(out);
        return 1;
    }

    float decay = 0.92f;
    float glow  = 0.25f;
    int running = 1;

    // Event- und Capture-Loop
    while (running) {
        // Events: Resize, Key, etc.
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_Escape || ks == XK_q || ks == XK_Q) running = 0;
            } else if (ev.type == ConfigureNotify) {
                // Fenstergröße aktualisieren (bei Resize)
                int nw = ev.xconfigure.width;
                int nh = ev.xconfigure.height;
                if (nw <= 0 || nh <= 0) continue;
                if (nw != win_w || nh != win_h) {
                    win_w = nw; win_h = nh;
                    // XImage neu erstellen
                    if (ximg) XDestroyImage(ximg); // gibt img_data frei
                    bpl = ((win_w * (bpp / 8) + 3) & ~3);
                    img_bytes = (size_t)bpl * win_h;
                    img_data = (uint8_t*)malloc(img_bytes);
                    if (!img_data) {
                        fprintf(stderr, "Speicherfehler für XImage-Daten (Resize).\n");
                        running = 0;
                        break;
                    }
                    ximg = XCreateImage(dpy, visual, depth, ZPixmap, 0, (char*)img_data,
                                        win_w, win_h, 32, bpl);
                    if (!ximg || !ximg->data) {
                        fprintf(stderr, "XCreateImage fehlgeschlagen (Resize).\n");
                        free(img_data);
                        running = 0;
                        break;
                    }
                }
            } else if (ev.type == DestroyNotify) {
                running = 0;
            }
        }
        if (!running) break;

        // Capture
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(vfd, VIDIOC_DQBUF, &buf) < 0) {
            if (errno == EAGAIN) continue;
            perror("VIDIOC_DQBUF");
            break;
        }

        if (buffers[buf.index].length < (size_t)(SRC_W * SRC_H * 2)) {
            fprintf(stderr, "Buffer zu klein: %zu (< %d).\n",
                    buffers[buf.index].length, SRC_W * SRC_H * 2);
            ioctl(vfd, VIDIOC_QBUF, &buf);
            break;
        }

        // YUYV -> RGB
        yuyv_to_rgb24((uint8_t*)buffers[buf.index].start, rgb, SRC_W, SRC_H);

        // Ghost-Blend + Glow
        ghost_blend(rgb, acc, out, decay, glow);

        // Skalieren und blitten in XImage (aspect-fit mit Letterboxing)
        blit_aspect_fit_to_ximage(out, SRC_W, SRC_H, ximg, win_w, win_h);

        // Zeichnen
        XPutImage(dpy, win, gc, ximg, 0, 0, 0, 0, win_w, win_h);
        XFlush(dpy);

        // Buffer zurückgeben
        if (ioctl(vfd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); break; }
    }

    // Cleanup
    if (ximg) XDestroyImage(ximg);
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


