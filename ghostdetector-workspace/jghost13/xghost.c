// gcc -O2 v4l2_hsv64_nightvision.c -o v4l2_hsv64_nightvision -lX11 -lm -lpthread
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { void *start; size_t length; } Buffer;

typedef struct {
    unsigned short h; // 0..65535 -> 0..360°
    unsigned short s; // 0..65535 -> 0..1
    unsigned short v; // 0..65535 -> 0..1
    unsigned short a; // 0..65535 alpha
} HSV64;

static int clampi(int v, int lo, int hi){ return v<lo?lo:(v>hi?hi:v); }
static unsigned int pack_argb(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
    return ((unsigned int)a<<24)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b);
}

// Numerisch stabile trig-Helfer
static inline double safe_tan(double x) {
    const double LIM = 1.2; // Begrenzung nahe Polstellen
    if (x > LIM) x = LIM;
    if (x < -LIM) x = -LIM;
    return tan(x);
}
static inline double sat_atan(double x, double scale) {
    return atan(x * scale);
}

// ----------------- ESC-Parameter (Hue/Sat only; Value bleibt unverändert) -----------------

typedef struct {
    double hue_base;
    double sat_base;
    double tint_base;

    double hue_amp,  hue_freq;
    double sat_amp,  sat_freq;
    double tint_amp, tint_freq;

    // Ziel: “Night Vision”-Stabilität über Texturmaß (mehr Kanten, weniger Rauschen)
    double target_tex;  // Zielwert für Texturmaß (mittlere Kantenenergie)
    double avg_tex;     // gemessene Textur pro Frame

    double J;           // Kosten (Differenz zu Ziel; mit atan stabilisiert)

    int running;
    pthread_mutex_t mtx;
} OptShared;

static void optshared_init(OptShared *os) {
    os->hue_base  = 10.0;    // Gradäquivalent
    os->sat_base  = 3500.0;  // Sättigungsverstärkung
    os->tint_base = 0.5;     // Tint-Alpha (0.1..0.8 empfohlen)

    os->hue_amp  = 2.0;   os->hue_freq  = 0.9;
    os->sat_amp  = 600.0; os->sat_freq  = 0.7;
    os->tint_amp = 0.08;  os->tint_freq = 0.6;

    os->target_tex = 0.18; // Ziel-Textur (normiert ~0..1)
    os->avg_tex = 0.0;
    os->J = 0.0;

    os->running = 1;
    pthread_mutex_init(&os->mtx, NULL);
}

// ESC-Thread: Lock-in Demodulation gegen Texturmaß (Kantenenergie)
static void* optimizer_thread(void *arg) {
    OptShared *os = (OptShared*)arg;

    double g_hue = 0.0, g_sat = 0.0, g_tint = 0.0;
    const double ema_alpha = 0.05;

    const double lr_hue  = 4e-3;
    const double lr_sat  = 4e-4;
    const double lr_tint = 8e-4;

    const double hue_min = 0.0,  hue_max = 20.0;
    const double sat_min = 500.0, sat_max = 8000.0;
    const double tint_min= 0.1,   tint_max= 0.8;

    struct timespec ts = {0, 10 * 1000 * 1000}; // 100 Hz
    double t = 0.0, dt = 0.01;

    while (1) {
        pthread_mutex_lock(&os->mtx);
        int running = os->running;

        double tex = os->avg_tex;                  // gemessen vom Renderer
        double e_tex = tex - os->target_tex;       // positiv: zu texturreich (u.U. Rauschverstärkung)
        double e_tex_s = sat_atan(e_tex, 3.0);     // stabilisiert Ausreißer
        os->J = e_tex_s * e_tex_s;

        // Dither-Sinus
        double s_hue  = sin(2.0 * M_PI * os->hue_freq  * t);
        double s_sat  = sin(2.0 * M_PI * os->sat_freq  * t);
        double s_tint = sin(2.0 * M_PI * os->tint_freq * t);

        // Lock-in Demodulation
        g_hue  = (1.0 - ema_alpha) * g_hue  + ema_alpha * (os->J * s_hue);
        g_sat  = (1.0 - ema_alpha) * g_sat  + ema_alpha * (os->J * s_sat);
        g_tint = (1.0 - ema_alpha) * g_tint + ema_alpha * (os->J * s_tint);

        // Update (negativer Gradient)
        os->hue_base  -= lr_hue  * g_hue;
        os->sat_base  -= lr_sat  * g_sat;
        os->tint_base -= lr_tint * g_tint;

        // Projektion
        if (os->hue_base  < hue_min)  os->hue_base  = hue_min;
        if (os->hue_base  > hue_max)  os->hue_base  = hue_max;
        if (os->sat_base  < sat_min)  os->sat_base  = sat_min;
        if (os->sat_base  > sat_max)  os->sat_base  = sat_max;
        if (os->tint_base < tint_min) os->tint_base = tint_min;
        if (os->tint_base > tint_max) os->tint_base = tint_max;

        pthread_mutex_unlock(&os->mtx);

        if (!running) break;
        nanosleep(&ts, NULL);
        t += dt;
    }
    return NULL;
}

// ----------------- YUV/RGB/HSV Hilfsfunktionen -----------------

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
    } else { s = 0.0; h = 0.0; }
    HSV64 out;
    out.h = (unsigned short)clampi((int)llround(h * (65535.0/360.0)), 0, 65535);
    out.s = (unsigned short)clampi((int)llround(s * 65535.0), 0, 65535);
    out.v = (unsigned short)clampi((int)llround(v * 65535.0), 0, 65535);
    out.a = 65535;
    return out;
}

static void hsv64_to_rgb8(HSV64 hsv, unsigned char *r8, unsigned char *g8, unsigned char *b8) {
    double h = (double)hsv.h * (360.0/65535.0);
    double s = (double)hsv.s / 65535.0;
    double v = (double)hsv.v / 65535.0;
    if (s <= 1e-12) {
        unsigned char val = (unsigned char)clampi((int)llround(v * 255.0), 0, 255);
        *r8 = val; *g8 = val; *b8 = val; return;
    }
    double c = v * s, hh = h / 60.0;
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

// Blend-Variante: Value (Helligkeit) unverändert lassen
static HSV64 hsv64_blend_preserve_v(HSV64 dst, HSV64 src, unsigned short alpha /*0..65535*/) {
    int dh = (int)src.h - (int)dst.h;
    if (dh > 32768) dh -= 65536;
    if (dh < -32768) dh += 65536;
    int h = dst.h + (int)((alpha / 65535.0) * dh);
    if (h < 0) h += 65536; if (h > 65535) h -= 65536;

    int s = clampi((int)dst.s + (int)((alpha / 65535.0) * src.s), 0, 65535);
    // v bleibt exakt wie dst.v
    HSV64 out = { (unsigned short)h, (unsigned short)s, dst.v, 65535 };
    return out;
}

// ----------------- Night-Vision Filter (ohne Helligkeitsänderung) -----------------

// Sobel-Kanten auf Value zur Texturmessung (kein Output-Eingriff auf V)
static void sobel_on_value(const unsigned short *V, int W, int H, double *tex_out, double *grad_norm /*W*H*/) {
    // Normierung: V in [0..65535] -> [0..1]
    const double inv = 1.0 / 65535.0;
    double sum = 0.0;
    for (int y = 1; y < H-1; ++y) {
        for (int x = 1; x < W-1; ++x) {
            int i = y*W + x;
            double v00 = V[(y-1)*W + (x-1)]*inv, v01 = V[(y-1)*W + x]*inv,     v02 = V[(y-1)*W + (x+1)]*inv;
            double v10 = V[y*W + (x-1)]*inv,     v11 = V[i]*inv,               v12 = V[y*W + (x+1)]*inv;
            double v20 = V[(y+1)*W + (x-1)]*inv, v21 = V[(y+1)*W + x]*inv,     v22 = V[(y+1)*W + (x+1)]*inv;

            double gx = (-1*v00 + 0*v01 + 1*v02) + (-2*v10 + 0*v11 + 2*v12) + (-1*v20 + 0*v21 + 1*v22);
            double gy = (-1*v00 - 2*v01 - 1*v02) + ( 0*v10 + 0*v11 + 0*v12) + ( 1*v20 + 2*v21 + 1*v22);
            double g  = sqrt(gx*gx + gy*gy); // 0..~8
            grad_norm[i] = fmin(1.0, g / 4.0); // grobe Normierung
            sum += grad_norm[i];
        }
    }
    *tex_out = sum / ((double)(W-2)*(double)(H-2)); // mittlere Kantenenergie ~0..1
}

static void ghost_nightvision_filter_hsv64(const unsigned char *srcRGB, int W, int H, unsigned int *dstARGB, double t, OptShared *os) {
    const int layers = 3;
    const double two_pi = 2.0 * M_PI;

    // ESC: Basis + sinusförmige Dither (Hue/Sat/Tint), Value wird nie verändert
    pthread_mutex_lock(&os->mtx);
    double hue_gain   = os->hue_base  + os->hue_amp  * sin(2.0 * M_PI * os->hue_freq  * t);
    double sat_gain   = os->sat_base  + os->sat_amp  * sin(2.0 * M_PI * os->sat_freq  * t);
    double tint_alpha = os->tint_base + os->tint_amp * sin(2.0 * M_PI * os->tint_freq * t);
    pthread_mutex_unlock(&os->mtx);

    // Puffer
    HSV64 *img = (HSV64*)malloc(sizeof(HSV64) * W * H);
    unsigned short *Vbuf = (unsigned short*)malloc(sizeof(unsigned short) * W * H);
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            HSV64 s = sample_rgb_to_hsv64_bilinear(srcRGB, W, H, (double)x, (double)y);
            img[y*W + x] = s;
            Vbuf[y*W + x] = s.v; // Original-Value sichern
        }
    }

    // Texturmaß: Sobel auf Value (nur zur Maskenbildung)
    double *grad = (double*)malloc(sizeof(double) * W * H);
    double tex = 0.0;
    sobel_on_value(Vbuf, W, H, &tex, grad);

    // Night-Vision: Grünphosphor-Tint, Hue/Sat-Modulation basierend auf Kantenmaske (Value bleibt)
    double sum_tex = tex; // für ESC-Metrik
    for (int g = 0; g < layers; ++g) {
        double phase = t * (0.5 + 0.15 * sin(t + g));
        double freqx = 1.0 + 0.15 * g;
        double freqy = 0.8 + 0.15 * g;

        // Ziel-Hue: klassisches Nachtsichtgrün (~120°)
        unsigned short h_green = (unsigned short)llround(120.0 * (65535.0/360.0));
        // Schicht-Tint-Sättigung/Val: S beeinflussen (Val bleibt bei Blend preserved)
        unsigned short s_tint = (unsigned short)(10000 + 5000 * fabs(sin(0.3 * t + g)));
        unsigned short v_tint = 0; // Value nicht ändern

        // Alpha skaliert
        double alpha_scale = tint_alpha; if (alpha_scale < 0.1) alpha_scale = 0.1; if (alpha_scale > 0.8) alpha_scale = 0.8;
        unsigned short alpha = (unsigned short)clampi((int)llround(20000.0 * alpha_scale), 0, 65535);

        for (int y = 1; y < H-1; ++y) {
            for (int x = 1; x < W-1; ++x) {
                int idx = y*W + x;
                HSV64 s = img[idx];

                // Sinus-Anteile zur sanften Variation der Hue-Drift (ohne Wellen/Geometrie)
                double nx = (double)x / (double)W;
                double ny = (double)y / (double)H;
                double s1 = sin(two_pi * (freqx * nx + phase));
                double s2 = sin(two_pi * (freqy * ny + 0.7 * phase + 0.3 * g));
                double s3 = sin(two_pi * (1.5 * freqx * nx + 0.9 * phase + 0.2 * g));
                double s_comb = s1 + 0.5 * s2 + 0.3 * s3;

                // Kantenmaske aus Sobel-Gradienten, mit arctan stabilisiert
                double edge = sat_atan(grad[idx], 1.5); // 0..~1, geglättet

                // Saturation-Boost nichtlinear: Mischung aus sin und tan
                double sat_boost = sat_gain * (0.6 * s_comb + 0.4 * safe_tan(0.5 * s_comb)) * edge;

                // Hue Richtung grün verschieben, abhängig von Kante und sin
                int dh = (int)h_green - (int)s.h;
                if (dh > 32768) dh -= 65536;
                if (dh < -32768) dh += 65536;
                int h_mod = s.h + (int)llround((65535.0/360.0) * (hue_gain * sat_atan(edge * s_comb, 1.0)));
                if (h_mod < 0) h_mod += 65536; if (h_mod > 65535) h_mod -= 65536;

                // Neue Sättigung (geboostet), geclamped
                int s_mod = clampi((int)s.s + (int)llround(sat_boost), 0, 65535);

                HSV64 mod = { (unsigned short)h_mod, (unsigned short)s_mod, s.v, 65535 }; // Value exakt unverändert
                HSV64 tint = { h_green, s_tint, (unsigned short)v_tint, 65535 };

                // Blend, das Value bewahrt
                HSV64 blended = hsv64_blend_preserve_v(s, mod, alpha);
                blended = hsv64_blend_preserve_v(blended, tint, (unsigned short)llround(alpha * 0.35));
                img[idx] = blended;
            }
        }
    }

    // Ausgabe: Value bleibt aus Original, Hue/Sat modifiziert
    for (int i = 0; i < W*H; ++i) {
        img[i].v = Vbuf[i]; // Sicherheit: Value unverändert
        unsigned char r,g,b; hsv64_to_rgb8(img[i], &r, &g, &b);
        dstARGB[i] = pack_argb(0xFF, r, g, b);
    }

    // ESC-Metrik
    pthread_mutex_lock(&os->mtx);
    os->avg_tex = sum_tex;
    pthread_mutex_unlock(&os->mtx);

    free(img); free(Vbuf); free(grad);
}

// ----------------- Main: V4L2 + X11 + ESC-Thread -----------------

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
    XStoreName(dpy, win, "Live /dev/video0 + HSV64 Night Vision (Value unverändert) + ESC");
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

    // --------- ESC-Thread ----------
    OptShared os; optshared_init(&os);
    pthread_t opt_th;
    if (pthread_create(&opt_th, NULL, optimizer_thread, &os) != 0) {
        fprintf(stderr, "pthread_create failed\n"); return 1;
    }

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

            ghost_nightvision_filter_hsv64(rgb, W, H, argb, t, &os);

            XPutImage(dpy, win, gc, xi, 0, 0, 0, 0, W, H);
            XFlush(dpy);

            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF (requeue)"); break; }

            t += dt; nanosleep(&ts, NULL);
        } else {
            struct timespec ts2 = {0, 5 * 1000 * 1000};
            nanosleep(&ts2, NULL);
        }
    }

    // --------- Cleanup ----------
    pthread_mutex_lock(&os.mtx);
    os.running = 0;
    pthread_mutex_unlock(&os.mtx);
    pthread_join(opt_th, NULL);

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

