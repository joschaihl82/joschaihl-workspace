// xghost.c
// Gespenster-Filter: V4L2 (YUYV) -> RGB24, Ghost-Blending + Glow -> X11 Fenster
// Features: Resizable Window, Aspect-Fit (Letterboxing), Vollbild-Start, ESC/Q beendet
// Radionullon-Band: 1/2/3 schalten Subbänder, modulieren Effekt; A/Z = Decay, S/X = Glow

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
#include <time.h>

#define SRC_W 640
#define SRC_H 480
#define NUM_BUFFERS 4

typedef struct { void *start; size_t length; } Buffer;

typedef enum {
    BAND_WHISPER_LOW = 0,   // 13.73–13.82 MHz
    BAND_LIMINAL_MID = 1,   // 13.82–13.95 MHz
    BAND_AFTERGLOW_HIGH = 2 // 13.95–14.07 MHz
} NullonBand;

typedef struct {
    NullonBand band;
    double t;            // Zeit (Sekunden) für Modulation
    float base_decay;    // Basis-Decay (per Taste veränderbar)
    float base_glow;     // Basis-Glow (per Taste veränderbar)
    float flicker;       // Flimmer-Amplitude
    float drift;         // langsame zeitliche Drift
} NullonState;

static inline int clamp8(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }

static void yuyv_to_rgb24(uint8_t *src, uint8_t *dst, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x += 2) {
            int i = y * w + x;
            int si = i * 2;
            uint8_t Y0 = src[si + 0], U = src[si + 1], Y1 = src[si + 2], V = src[si + 3];
            int C0 = (int)Y0 - 16, C1 = (int)Y1 - 16, D = (int)U - 128, E = (int)V - 128;
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

// Ghost-Blend mit Radionullon-Modulation und per-Taste änderbaren Basiswerten
static void ghost_blend_nullon(uint8_t *current, uint8_t *prev, uint8_t *out, const NullonState *ns) {
    int w = SRC_W, h = SRC_H;
    float decay = ns->base_decay, glow = ns->base_glow;
    double t = ns->t;

    switch (ns->band) {
        case BAND_WHISPER_LOW:
            decay += 0.01f * (float)sin(t * 0.9);
            glow  += 0.05f * (float)(0.5 + 0.5 * sin(t * 1.7));
            break;
        case BAND_LIMINAL_MID:
            decay += 0.02f * (float)sin(t * 0.5) + 0.01f * (float)sin(t * 3.1);
            glow  += 0.08f * (float)(0.5 + 0.5 * sin(t * 0.8 + 1.2));
            break;
        case BAND_AFTERGLOW_HIGH:
            decay += 0.03f * (float)(0.5 + 0.5 * sin(t * 0.3));
            glow  += 0.06f * (float)sin(t * 0.6);
            break;
    }
    if (decay < 0.80f) decay = 0.80f; if (decay > 0.98f) decay = 0.98f;
    if (glow  < 0.05f) glow  = 0.05f; if (glow  > 0.80f) glow  = 0.80f;

    float flicker_amp = ns->flicker;
    float drift = ns->drift;

    for (int y = 0; y < h; y++) {
        float row_phase = (float)sin(t * 2.0 + y * 0.013 + drift);
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 3;

            float pr = (float)prev[idx + 0], pg = (float)prev[idx + 1], pb = (float)prev[idx + 2];
            float cr = (float)current[idx + 0], cg = (float)current[idx + 1], cb = (float)current[idx + 2];

            float px_phase = (float)sin(t * 3.3 + x * 0.017 + y * 0.011);
            float flick = flicker_amp * (row_phase * 0.5f + px_phase * 0.5f);
            cr = cr * (1.0f + flick * 0.10f);
            cg = cg * (1.0f + flick * 0.08f);
            cb = cb * (1.0f + flick * 0.06f);

            float nr = pr * decay + cr * (1.0f - decay);
            float ng = pg * decay + cg * (1.0f - decay);
            float nb = pb * decay + cb * (1.0f - decay);

            int xm = (x > 0) ? x - 1 : x, xp = (x < w - 1) ? x + 1 : x;
            int ym = (y > 0) ? y - 1 : y, yp = (y < h - 1) ? y + 1 : y;
            int idxL = (y * w + xm) * 3, idxR = (y * w + xp) * 3;
            int idxU = (ym * w + x) * 3, idxD = (yp * w + x) * 3;

            float dx = (float)current[idxR] - (float)current[idxL];
            float dy = (float)current[idxD] - (float)current[idxU];

            float pulse = 1.0f;
            if (ns->band == BAND_LIMINAL_MID) {
                float gate = (float)(0.5 + 0.5 * sin(t * 0.9 + y * 0.02));
                pulse = 0.7f + 0.6f * gate;
            } else if (ns->band == BAND_WHISPER_LOW) {
                float gate = (float)(0.5 + 0.5 * sin(t * 1.4 + x * 0.015));
                pulse = 0.8f + 0.4f * gate;
            } else {
                float gate = (float)(0.5 + 0.5 * sin(t * 0.6));
                pulse = 0.9f + 0.3f * gate;
            }

            float g  = glow * pulse * (fabsf(dx) + fabsf(dy)) * 0.5f;

            int ir = clamp8((int)(nr + g));
            int ig = clamp8((int)(ng + g * 0.7f));
            int ib = clamp8((int)(nb + g * 0.4f));

            if (ns->band == BAND_AFTERGLOW_HIGH) ib = clamp8(ib + 6);
            else if (ns->band == BAND_WHISPER_LOW) ir = clamp8(ir + 2);

            out[idx + 0] = (uint8_t)ir;
            out[idx + 1] = (uint8_t)ig;
            out[idx + 2] = (uint8_t)ib;

            prev[idx + 0] = out[idx + 0];
            prev[idx + 1] = out[idx + 1];
            prev[idx + 2] = out[idx + 2];
        }
    }
}

static void blit_aspect_fit_to_ximage(uint8_t *src_rgb, int src_w, int src_h,
                                      XImage *ximg, int win_w, int win_h) {
    double scale = fmin((double)win_w / (double)src_w, (double)win_h / (double)src_h);
    int dst_w = (int)floor(src_w * scale);
    int dst_h = (int)floor(src_h * scale);
    int off_x = (win_w - dst_w) / 2, off_y = (win_h - dst_h) / 2;

    uint8_t *dst = (uint8_t*)ximg->data;
    int bpl = ximg->bytes_per_line;

    // Hintergrund schwarz
    for (int y = 0; y < win_h; y++) {
        uint8_t *row = dst + y * bpl;
        for (int x = 0; x < win_w; x++) {
            int d = x * 4;
            row[d + 0] = 0x00; row[d + 1] = 0x00; row[d + 2] = 0x00; row[d + 3] = 0x00;
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
            row[di + 0] = src_rgb[si + 2]; // B
            row[di + 1] = src_rgb[si + 1]; // G
            row[di + 2] = src_rgb[si + 0]; // R
            row[di + 3] = 0x00;            // X
        }
    }
}

static void request_fullscreen(Display *dpy, Window win) {
    Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    XEvent xev; memset(&xev, 0, sizeof(xev));
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
    struct v4l2_format fmt; memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = w; fmt.fmt.pix.height = h;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return -1; }
    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) { fprintf(stderr, "Gerät unterstützt kein YUYV.\n"); return -1; }
    if (fmt.fmt.pix.width != w || fmt.fmt.pix.height != h) {
        fprintf(stderr, "Warnung: Kamera liefert %ux%u statt %dx%d.\n", fmt.fmt.pix.width, fmt.fmt.pix.height, w, h);
        return -1;
    }
    return 0;
}

static int init_mmap(int fd, Buffer *buffers, int *count) {
    struct v4l2_requestbuffers req; memset(&req, 0, sizeof(req));
    req.count = NUM_BUFFERS; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return -1; }
    if (req.count < 2) { fprintf(stderr, "Nicht genug Buffer.\n"); return -1; }

    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = req.type; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return -1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); return -1; }
    }
    *count = req.count;
    return 0;
}

static void set_nullon_band(NullonState *ns, NullonBand band) {
    ns->band = band; ns->t = 0.0;
    switch (band) {
        case BAND_WHISPER_LOW:
            ns->base_decay = 0.93f; ns->base_glow = 0.18f; ns->flicker = 0.20f; ns->drift = 0.15f; break;
        case BAND_LIMINAL_MID:
            ns->base_decay = 0.92f; ns->base_glow = 0.22f; ns->flicker = 0.28f; ns->drift = 0.35f; break;
        case BAND_AFTERGLOW_HIGH:
            ns->base_decay = 0.95f; ns->base_glow = 0.20f; ns->flicker = 0.16f; ns->drift = 0.08f; break;
    }
}

static void adjust_decay(NullonState *ns, float delta) {
    ns->base_decay += delta;
    if (ns->base_decay < 0.80f) ns->base_decay = 0.80f;
    if (ns->base_decay > 0.98f) ns->base_decay = 0.98f;
}

static void adjust_glow(NullonState *ns, float delta) {
    ns->base_glow += delta;
    if (ns->base_glow < 0.05f) ns->base_glow = 0.05f;
    if (ns->base_glow > 0.80f) ns->base_glow = 0.80f;
}

int main(void) {
    int vfd = open("/dev/video0", O_RDWR);
    if (vfd < 0) { perror("open /dev/video0"); return 1; }
    if (set_format(vfd, SRC_W, SRC_H) < 0) { close(vfd); return 1; }

    Buffer buffers[NUM_BUFFERS]; memset(buffers, 0, sizeof(buffers));
    int buf_count = 0;
    if (init_mmap(vfd, buffers, &buf_count) < 0) { close(vfd); return 1; }

    for (int i = 0; i < buf_count; i++) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(vfd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return 1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(vfd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return 1; }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Kann X11 Display nicht öffnen.\n"); return 1; }
    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);

    XSetWindowAttributes swa; swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;
    Window win = XCreateWindow(dpy, root, 100, 100, SRC_W, SRC_H, 0,
                               DefaultDepth(dpy, screen), InputOutput, DefaultVisual(dpy, screen),
                               CWEventMask, &swa);
    XStoreName(dpy, win, "xghost - Radionullon");
    XMapWindow(dpy, win);
    request_fullscreen(dpy, win);

    int win_w = SRC_W, win_h = SRC_H;
    int depth = DefaultDepth(dpy, screen);
    if (depth < 24) { fprintf(stderr, "Unsupported depth %d (>=24 nötig).\n", depth); XCloseDisplay(dpy); close(vfd); return 1; }
    Visual *visual = DefaultVisual(dpy, screen);
    GC gc = DefaultGC(dpy, screen);

    int bpp = 32;
    int bpl = ((win_w * (bpp / 8) + 3) & ~3);
    size_t img_bytes = (size_t)bpl * win_h;
    uint8_t *img_data = (uint8_t*)malloc(img_bytes);
    if (!img_data) { fprintf(stderr, "Speicherfehler für XImage-Daten.\n"); XCloseDisplay(dpy); close(vfd); return 1; }
    XImage *ximg = XCreateImage(dpy, visual, depth, ZPixmap, 0, (char*)img_data, win_w, win_h, 32, bpl);
    if (!ximg || !ximg->data) { fprintf(stderr, "XCreateImage fehlgeschlagen.\n"); free(img_data); XCloseDisplay(dpy); close(vfd); return 1; }

    uint8_t *rgb = (uint8_t*)malloc(SRC_W * SRC_H * 3);
    uint8_t *acc = (uint8_t*)calloc(SRC_W * SRC_H * 3, 1);
    uint8_t *out = (uint8_t*)malloc(SRC_W * SRC_H * 3);
    if (!rgb || !acc || !out) { fprintf(stderr, "Speicherfehler.\n"); if (ximg) XDestroyImage(ximg); XCloseDisplay(dpy); close(vfd); free(rgb); free(acc); free(out); return 1; }

    NullonState ns; set_nullon_band(&ns, BAND_WHISPER_LOW);
    struct timespec ts_prev; clock_gettime(CLOCK_MONOTONIC, &ts_prev);

    int running = 1;
    while (running) {
        struct timespec ts_now; clock_gettime(CLOCK_MONOTONIC, &ts_now);
        double dt = (ts_now.tv_sec - ts_prev.tv_sec) + (ts_now.tv_nsec - ts_prev.tv_nsec) / 1e9;
        if (dt < 0) dt = 0; ns.t += dt; ts_prev = ts_now;

        while (XPending(dpy)) {
            XEvent ev; XNextEvent(dpy, &ev);
            if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_Escape || ks == XK_q || ks == XK_Q) running = 0;
                else if (ks == XK_1) set_nullon_band(&ns, BAND_WHISPER_LOW);
                else if (ks == XK_2) set_nullon_band(&ns, BAND_LIMINAL_MID);
                else if (ks == XK_3) set_nullon_band(&ns, BAND_AFTERGLOW_HIGH);
                else if (ks == XK_a || ks == XK_A) adjust_decay(&ns, +0.01f);
                else if (ks == XK_z || ks == XK_Z) adjust_decay(&ns, -0.01f);
                else if (ks == XK_s || ks == XK_S) adjust_glow(&ns, +0.02f);
                else if (ks == XK_x || ks == XK_X) adjust_glow(&ns, -0.02f);
            } else if (ev.type == ConfigureNotify) {
                int nw = ev.xconfigure.width, nh = ev.xconfigure.height;
                if (nw > 0 && nh > 0 && (nw != win_w || nh != win_h)) {
                    win_w = nw; win_h = nh;
                    if (ximg) XDestroyImage(ximg);
                    bpl = ((win_w * (bpp / 8) + 3) & ~3);
                    img_bytes = (size_t)bpl * win_h;
                    img_data = (uint8_t*)malloc(img_bytes);
                    if (!img_data) { fprintf(stderr, "Speicherfehler für XImage-Daten (Resize).\n"); running = 0; break; }
                    ximg = XCreateImage(dpy, visual, depth, ZPixmap, 0, (char*)img_data, win_w, win_h, 32, bpl);
                    if (!ximg || !ximg->data) { fprintf(stderr, "XCreateImage fehlgeschlagen (Resize).\n"); free(img_data); running = 0; break; }
                }
            } else if (ev.type == DestroyNotify) {
                running = 0;
            }
        }
        if (!running) break;

        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(vfd, VIDIOC_DQBUF, &buf) < 0) { if (errno == EAGAIN) continue; perror("VIDIOC_DQBUF"); break; }
        if (buffers[buf.index].length < (size_t)(SRC_W * SRC_H * 2)) {
            fprintf(stderr, "Buffer zu klein: %zu (< %d).\n", buffers[buf.index].length, SRC_W * SRC_H * 2);
            ioctl(vfd, VIDIOC_QBUF, &buf); break;
        }

        yuyv_to_rgb24((uint8_t*)buffers[buf.index].start, rgb, SRC_W, SRC_H);
        ghost_blend_nullon(rgb, acc, out, &ns);
        blit_aspect_fit_to_ximage(out, SRC_W, SRC_H, ximg, win_w, win_h);

        XPutImage(dpy, win, gc, ximg, 0, 0, 0, 0, win_w, win_h);

        // Overlay: Bandname und Werte
        char overlay[160];
        const char *band_name = (ns.band == BAND_WHISPER_LOW) ? "Whisper low 13.73–13.82 MHz"
                               : (ns.band == BAND_LIMINAL_MID) ? "Liminal mid 13.82–13.95 MHz"
                               : "Afterglow high 13.95–14.07 MHz";
        snprintf(overlay, sizeof(overlay), "Radionullon: %s  Decay=%.3f  Glow=%.3f  [ESC/Q quit | 1/2/3 band | A/Z decay | S/X glow]",
                 band_name, ns.base_decay, ns.base_glow);
        XSetForeground(dpy, gc, WhitePixel(dpy, screen));
        XDrawString(dpy, win, gc, 16, 24, overlay, (int)strlen(overlay));
        XFlush(dpy);

        if (ioctl(vfd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); break; }
    }

    if (ximg) XDestroyImage(ximg);
    XCloseDisplay(dpy);
    enum v4l2_buf_type type2 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(vfd, VIDIOC_STREAMOFF, &type2);
    for (int i = 0; i < buf_count; i++) munmap(buffers[i].start, buffers[i].length);
    close(vfd);
    free(rgb); free(acc); free(out);
    return 0;
}

