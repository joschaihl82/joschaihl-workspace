// gcc -O2 v4l2_hsv64_sine_ghost.c -o v4l2_hsv64_sine_ghost -lX11 -lm
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { void *start; size_t length; } Buffer;

typedef struct {
    unsigned short h; // 0..65535 -> 0..360 deg
    unsigned short s; // 0..65535 -> 0..1
    unsigned short v; // 0..65535 -> 0..1
    unsigned short a; // 0..65535 alpha
} HSV64;

static int clampi(int v, int lo, int hi){ return v<lo?lo:(v>hi?hi:v); }
static unsigned int pack_argb(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
    return ((unsigned int)a<<24)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b);
}

// YUYV -> RGB8 (BT.601 approx)
static void yuyv_to_rgb_row(unsigned char *yuyv, int width, unsigned char *rgb) {
    for (int x = 0; x < width; x += 2) {
        int y0 = yuyv[0], u = yuyv[1], y1 = yuyv[2], v = yuyv[3];
        yuyv += 4;
        int uu = u - 128, vv = v - 128;

        int r0 = clampi((int)(y0 + 1.402 * vv), 0, 255);
        int g0 = clampi((int)(y0 - 0.344136 * uu - 0.714136 * vv), 0, 255);
        int b0 = clampi((int)(y0 + 1.772 * uu), 0, 255);

        int r1 = clampi((int)(y1 + 1.402 * vv), 0, 255);
        int g1 = clampi((int)(y1 - 0.344136 * uu - 0.714136 * vv), 0, 255);
        int b1 = clampi((int)(y1 + 1.772 * uu), 0, 255);

        rgb[0]=r0; rgb[1]=g0; rgb[2]=b0; rgb+=3;
        rgb[0]=r1; rgb[1]=g1; rgb[2]=b1; rgb+=3;
    }
}

// RGB8 -> HSV64
static HSV64 rgb8_to_hsv64(unsigned char r8, unsigned char g8, unsigned char b8) {
    double r = r8/255.0, g = g8/255.0, b = b8/255.0;
    double max = fmax(r, fmax(g, b)), min = fmin(r, fmin(g, b));
    double delta = max - min;
    double h = 0.0, s = 0.0, v = max;
    if (delta > 1e-12) {
        s = (max <= 0.0) ? 0.0 : delta / max;
        if (max == r) h = fmod(((g - b) / delta), 6.0);
        else if (max == g) h = ((b - r) / delta) + 2.0;
        else h = ((r - g) / delta) + 4.0;
        h *= 60.0;
        if (h < 0.0) h += 360.0;
    } else {
        s = 0.0; h = 0.0;
    }
    HSV64 out;
    out.h = (unsigned short)clampi((int)llround(h * (65535.0/360.0)), 0, 65535);
    out.s = (unsigned short)clampi((int)llround(s * 65535.0), 0, 65535);
    out.v = (unsigned short)clampi((int)llround(v * 65535.0), 0, 65535);
    out.a = 65535;
    return out;
}

// HSV64 -> RGB8
static void hsv64_to_rgb8(HSV64 hsv, unsigned char *r8, unsigned char *g8, unsigned char *b8) {
    double h = (double)hsv.h * (360.0/65535.0);
    double s = (double)hsv.s / 65535.0;
    double v = (double)hsv.v / 65535.0;

    if (s <= 1e-12) { // achromatisch
        unsigned char val = (unsigned char)clampi((int)llround(v * 255.0), 0, 255);
        *r8 = val; *g8 = val; *b8 = val; return;
    }
    double c = v * s;
    double hh = h / 60.0;
    double x = c * (1.0 - fabs(fmod(hh, 2.0) - 1.0));
    double m = v - c;

    double r=0,g=0,b=0;
    int region = (int)floor(hh);
    switch (region) {
        case 0: r=c; g=x; b=0; break;
        case 1: r=x; g=c; b=0; break;
        case 2: r=0; g=c; b=x; break;
        case 3: r=0; g=x; b=c; break;
        case 4: r=x; g=0; b=c; break;
        default: r=c; g=0; b=x; break;
    }
    *r8 = (unsigned char)clampi((int)llround((r+m) * 255.0), 0, 255);
    *g8 = (unsigned char)clampi((int)llround((g+m) * 255.0), 0, 255);
    *b8 = (unsigned char)clampi((int)llround((b+m) * 255.0), 0, 255);
}

// Bilineares Sampling aus RGB8-Buffer (interleaved), gibt HSV64
static HSV64 sample_rgb_to_hsv64_bilinear(const unsigned char *src, int W, int H, double xf, double yf) {
    if (xf < 0) xf = 0; if (yf < 0) yf = 0;
    if (xf > W-1) xf = W-1; if (yf > H-1) yf = H-1;
    int x0 = (int)floor(xf), y0 = (int)floor(yf);
    int x1 = clampi(x0+1, 0, W-1), y1 = clampi(y0+1, 0, H-1);
    double tx = xf - x0, ty = yf - y0;

    const unsigned char *p00 = src + (y0*W + x0)*3;
    const unsigned char *p10 = src + (y0*W + x1)*3;
    const unsigned char *p01 = src + (y1*W + x0)*3;
    const unsigned char *p11 = src + (y1*W + x1)*3;

    double r = (1-tx)*(1-ty)*p00[0] + tx*(1-ty)*p10[0] + (1-tx)*ty*p01[0] + tx*ty*p11[0];
    double g = (1-tx)*(1-ty)*p00[1] + tx*(1-ty)*p10[1] + (1-tx)*ty*p01[1] + tx*ty*p11[1];
    double b = (1-tx)*(1-ty)*p00[2] + tx*(1-ty)*p10[2] + (1-tx)*ty*p01[2] + tx*ty*p11[2];

    return rgb8_to_hsv64((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

// Additives Blend in HSV64 (V anheben, S/H leicht mischen), mit Alpha 16‑bit
static HSV64 hsv64_blend_add(HSV64 dst, HSV64 src, unsigned short alpha /*0..65535*/) {
    // H: zirkulär – leichte Verschiebung Richtung src abhängig von alpha
    int dh = (int)src.h - (int)dst.h;
    // kürzeste Richtung auf Kreis
    if (dh > 32768) dh -= 65536;
    if (dh < -32768) dh += 65536;
    int h = dst.h + (int)((alpha / 65535.0) * dh);
    if (h < 0) h += 65536; if (h > 65535) h -= 65536;

    // S, V: additive/linear Mischung
    int s = clampi((int)dst.s + (int)((alpha / 65535.0) * src.s), 0, 65535);
    int v = clampi((int)dst.v + (int)((alpha / 65535.0) * src.v), 0, 65535);
    HSV64 out = { (unsigned short)h, (unsigned short)s, (unsigned short)v, 65535 };
    return out;
}

// Sinus-Gespensterfilter in HSV64
static void ghost_sine_filter_hsv64(const unsigned char *srcRGB, int W, int H, unsigned int *dstARGB, double t) {
    const int layers = 7;
    const double two_pi = 2.0 * M_PI;

    // Hintergrund: dunkles, leicht gesättigtes Blau in HSV
    HSV64 base = { (unsigned short)llround(240.0 * (65535.0/360.0)), (unsigned short)(10000), (unsigned short)(3000), 65535 };

    // Arbeitspuffer HSV64
    HSV64 *acc = (HSV64*)malloc(sizeof(HSV64) * W * H);
    for (int i = 0; i < W*H; ++i) acc[i] = base;

    for (int g = 0; g < layers; ++g) {
        double phase = t * (0.6 + 0.2 * sin(t + g));
        double damp  = 0.55 + 0.45 * fabs(sin(0.35 * t + 0.9 * g));
        double amp   = (H * 0.06) * (1.0 + 0.15 * g) * damp;
        double freqx = 1.5 + 0.3 * g;
        double freqy = 0.9 + 0.2 * g;

        // Tint in HSV: Hue driftet sinusförmig
        double h_drift_deg = 20.0 * sin(0.7 * t + 0.5 * g);
        unsigned short h_tint = (unsigned short)(( (240.0 + h_drift_deg) * (65535.0/360.0) ));
        unsigned short s_tint = (unsigned short)(10000 + 5000 * fabs(sin(0.4 * t + g)));
        unsigned short v_tint = (unsigned short)(2000 + 4000 * fabs(sin(0.5 * t + 0.7 * g)));

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                double nx = (double)x / (double)W;
                double ny = (double)y / (double)H;

                double s1 = sin(two_pi * (freqx * nx + phase));
                double s2 = sin(two_pi * (freqy * ny + 0.7 * phase + 0.3 * g));
                double s3 = sin(two_pi * (1.9 * freqx * nx + 1.1 * phase + 0.2 * g));
                double comb = s1 + 0.6 * s2 + 0.4 * s3;

                double dy = amp * comb + 0.03 * H * sin(0.2 * t + 0.05 * x + 0.3 * g);
                double dx = 0.02 * W * sin(0.15 * t + 0.04 * y + 0.2 * g);

                double sx = x + dx;
                double sy = y + dy;

                HSV64 sample = sample_rgb_to_hsv64_bilinear(srcRGB, W, H, sx, sy);

                // Sinusbasierte Modulation von H, S, V
                int h_mod = (int)sample.h + (int)llround((65535.0/360.0) * (10.0 * sin(0.8 * t + 0.3 * nx + 0.4 * ny)));
                if (h_mod < 0) h_mod += 65536; if (h_mod > 65535) h_mod -= 65536;

                int s_mod = clampi((int)sample.s + (int)llround(3000.0 * sin(0.6 * t + nx * two_pi)), 0, 65535);
                int v_mod = clampi((int)sample.v + (int)llround(4000.0 * sin(0.5 * t + ny * two_pi)), 0, 65535);

                HSV64 mod = { (unsigned short)h_mod, (unsigned short)s_mod, (unsigned short)v_mod, 65535 };

                // Tint anwenden (leichte Aufsättigung/Val-Zugabe)
                HSV64 tint = { h_tint, s_tint, v_tint, 65535 };

                // Alpha pro Schicht aus Sinus
                unsigned short alpha = (unsigned short)llround(30000.0 + 20000.0 * fabs(sin(0.3 * t + g)));

                // Blends: erst mod, dann tint
                HSV64 blended = hsv64_blend_add(acc[y*W + x], mod, alpha);
                blended = hsv64_blend_add(blended, tint, (unsigned short)llround(alpha * 0.4));
                acc[y*W + x] = blended;
            }
        }
    }

    // HSV64 -> RGB8 und in ARGB packen
    for (int i = 0; i < W*H; ++i) {
        unsigned char r,g,b; hsv64_to_rgb8(acc[i], &r, &g, &b);
        dstARGB[i] = pack_argb(0xFF, r, g, b);
    }
    free(acc);
}

int main(void) {
    // --------- V4L2 ----------
    const char *dev = "/dev/video0";
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) { perror("open /dev/video0"); return 1; }

    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); return 1; }

    int W = 640, H = 480;
    struct v4l2_format fmt; memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = W;
    fmt.fmt.pix.height = H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return 1; }
    W = fmt.fmt.pix.width; H = fmt.fmt.pix.height;

    struct v4l2_requestbuffers req; memset(&req, 0, sizeof(req));
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return 1; }
    if (req.count < 2) { fprintf(stderr, "Insufficient buffer memory\n"); return 1; }

    Buffer *buffers = calloc(req.count, sizeof(Buffer));
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return 1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); return 1; }
    }
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return 1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return 1; }

    // --------- X11 ----------
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "XOpenDisplay failed\n"); return 1; }
    int scr = DefaultScreen(dpy);
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 100, 100, W, H, 1,
                                     BlackPixel(dpy, scr), WhitePixel(dpy, scr));
    XStoreName(dpy, win, "Live /dev/video0 + HSV64 Gespensterfilter (sin)");
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, NULL);

    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, scr), 24, ZPixmap, 0,
                              malloc(W*H*4), W, H, 32, W*4);
    if (!xi || !xi->data) { fprintf(stderr, "XCreateImage failed\n"); return 1; }

    unsigned char *rgb = malloc(W*H*3);
    unsigned int *argb = (unsigned int*)xi->data;

    struct timespec ts = {0, 16 * 1000 * 1000}; // ~60 FPS
    double t = 0.0, dt = 0.016;
    int running = 1;

    while (running) {
        while (XPending(dpy)) {
            XEvent e; XNextEvent(dpy, &e);
            if (e.type == KeyPress) running = 0;
        }

        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == 0) {
            unsigned char *yuyv = (unsigned char *)buffers[buf.index].start;
            for (int y = 0; y < H; ++y)
                yuyv_to_rgb_row(yuyv + y * (W*2), W, rgb + y * (W*3));

            ghost_sine_filter_hsv64(rgb, W, H, argb, t);

            XPutImage(dpy, win, gc, xi, 0, 0, 0, 0, W, H);
            XFlush(dpy);

            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF (requeue)"); break; }

            t += dt; nanosleep(&ts, NULL);
        } else {
            struct timespec ts2 = {0, 5 * 1000 * 1000};
            nanosleep(&ts2, NULL);
        }
    }

    XDestroyImage(xi);
    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < req.count; ++i) {
        if (buffers[i].start && buffers[i].start != MAP_FAILED)
            munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers); free(rgb);
    close(fd);
    return 0;
}


