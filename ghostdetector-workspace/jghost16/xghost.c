// gcc -O2 v4l2_hsv64_nowaves_esc.c -o v4l2_hsv64_nowaves_esc -lX11 -lm -lpthread
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h> // Für KeyCode -> KeySym
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

// Deklaration der globalen Variablen für die dynamische Größe (nur in main genutzt)
static int current_win_W = 640;
static int current_win_H = 480;

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

// ----------------- Gemeinsame Parameter und Metriken (für ESC-Optimierung) -----------------

typedef struct {
    // Basis-Gains (werden optimiert)
    double hue_base;
    double sat_base;
    double val_base;
    double tint_base;

    // Dither-Amplituden und -Frequenzen (sinusförmig)
    double hue_amp, hue_freq;
    double sat_amp, sat_freq;
    double val_amp, val_freq;
    double tint_amp, tint_freq;

    // Zielwerte für mittlere Sättigung und Helligkeit
    double target_sat;    // 0..65535
    double target_val;    // 0..65535

    // Laufzeit-Metriken aus dem Rendering (Main-Thread schreibt)
    double avg_sat;        // Frame-Mittelwert (0..65535)
    double avg_val;        // Frame-Mittelwert (0..65535)

    // Kostenfunktion (Main-Thread aktualisiert grob; Thread LPF-t)
    double J;            // w_sat*(avg_sat-target_sat)^2 + w_val*(avg_val-target_val)^2

    // Laufstatus & Synchronisation
    int running;
    pthread_mutex_t mtx;
} OptShared;

static void optshared_init(OptShared *os) {
    // Startwerte
    os->hue_base  = 8.0;
    os->sat_base  = 2500.0;
    os->val_base  = 5500.0; // ANGEPASST: Höherer Basiswert für Helligkeit
    os->tint_base = 0.4;

    // Dither: kleine sinusförmige Anregung mit verschiedenen Frequenzen
    os->hue_amp  = 2.0;    os->hue_freq  = 0.9;    // Hz
    os->sat_amp  = 500.0; os->sat_freq  = 0.7;
    os->val_amp  = 500.0; os->val_freq  = 0.5;
    os->tint_amp = 0.2;  os->tint_freq = 0.75;

    os->target_sat = 18000.0;
    os->target_val = 22000.0;

    os->avg_sat = 0.0;
    os->avg_val = 0.0;
    os->J = 0.0;

    os->running = 1;
    pthread_mutex_init(&os->mtx, NULL);
}


// ----------------- ESC-Optimizer-Thread: Lock-in-Demodulation mit sinusförmiger Intervallschätzung -----------------

static void* optimizer_thread(void *arg) {
    OptShared *os = (OptShared*)arg;

    // Gewichte der Kosten
    const double w_sat = 1.0, w_val = 1.0;

    // Lock-in-Demodulation (Gradientenschätzung) mit Low-Pass Filter (EMA)
    double g_hue = 0.0, g_sat = 0.0, g_val = 0.0, g_tint = 0.0;
    const double ema_alpha = 0.0002; // LPF-Koeffizient für Demodulation

    // Lernraten
    const double lr_hue  = 5e-3;
    const double lr_sat  = 5e-4;
    const double lr_val  = 5e-4;
    const double lr_tint = 1e-3;

    // Grenzen
    const double hue_min = 40.0,  hue_max = 80.0;
    const double sat_min = 2500.0, sat_max = 6000.0;
    const double val_min = 1500.0, val_max = 6000.0;
    const double tint_min= 0.1,    tint_max= 0.8;

    // Zeitbasis
    struct timespec ts = {0, 10 * 1000 * 1000}; // 100 Hz Update
    double t = 0.0;
    const double dt = 0.01; // passend zu ts

    while (1) {
        // Hole Metriken & Ziel
        pthread_mutex_lock(&os->mtx);
        int running = os->running;
        double avg_sat = os->avg_sat;
        double avg_val = os->avg_val;
        double target_sat = os->target_sat;
        double target_val = os->target_val;

        // Kostenfunktion
        double e_sat = avg_sat - target_sat;
        double e_val = avg_val - target_val;
        os->J = w_sat * e_sat * e_sat + w_val * e_val * e_val;

        // Aktuelle Dither-Signale
        double s_hue  = sin(2.0 * M_PI * os->hue_freq  * t);
        double s_sat  = sin(2.0 * M_PI * os->sat_freq  * t);
        double s_val  = sin(2.0 * M_PI * os->val_freq  * t);
        double s_tint = sin(2.0 * M_PI * os->tint_freq * t);

        // Lock-in Demodulation: J * sin(...) -> Gradient-Schätzung (EMA Low-Pass)
        g_hue  = (1.0 - ema_alpha) * g_hue  + ema_alpha * (os->J * s_hue);
        g_sat  = (1.0 - ema_alpha) * g_sat  + ema_alpha * (os->J * s_sat);
        g_val  = (1.0 - ema_alpha) * g_val  + ema_alpha * (os->J * s_val);
        g_tint = (1.0 - ema_alpha) * g_tint + ema_alpha * (os->J * s_tint);

        // Gradient-Abstieg auf Basis-Gains (Parameterdrift entgegen dem demodulierten Signal)
        os->hue_base  -= lr_hue  * g_hue;
        os->sat_base  -= lr_sat  * g_sat;
        os->val_base  -= lr_val  * g_val;
        os->tint_base -= lr_tint * g_tint;

        // Projektions-Grenzen
        if (os->hue_base  < hue_min)  os->hue_base  = hue_min;
        if (os->hue_base  > hue_max)  os->hue_base  = hue_max;
        if (os->sat_base  < sat_min)  os->sat_base  = sat_min;
        if (os->sat_base  > sat_max)  os->sat_base  = sat_max;
        if (os->val_base  < val_min)  os->val_base  = val_min;
        if (os->val_base  > val_max)  os->val_base  = val_max;
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

        // R = Y + 1.402*V'
        // G = Y - 0.344136*U' - 0.714136*V'
        // B = Y + 1.772*U'
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

// Modifizierte Bilineare Interpolation, um aus dem Quellbild (W/H) einen Pixel an der Zielposition (nx/ny) zu entnehmen
static HSV64 sample_rgb_to_hsv64_bilinear_scaled(const unsigned char *src, int srcW, int srcH, int dstW, int dstH, int dst_x, int dst_y) {
    // Berechne die entsprechenden Koordinaten im Quellbild (src)
    double xf = (double)dst_x * (double)srcW / (double)dstW;
    double yf = (double)dst_y * (double)srcH / (double)dstH;

    if (xf < 0) xf = 0; if (yf < 0) yf = 0;
    if (xf > srcW-1) xf = srcW-1; if (yf > srcH-1) yf = srcH-1;
    int x0 = (int)floor(xf), y0 = (int)floor(yf);
    int x1 = clampi(x0+1, 0, srcW-1), y1 = clampi(y0+1, 0, srcH-1);
    double tx = xf - x0, ty = yf - y0;

    const unsigned char *p00 = src + (y0*srcW + x0)*3;
    const unsigned char *p10 = src + (y0*srcW + x1)*3;
    const unsigned char *p01 = src + (y1*srcW + x0)*3;
    const unsigned char *p11 = src + (y1*srcW + x1)*3;

    // Bilineare Interpolation der RGB-Kanäle
    double r = (1-tx)*(1-ty)*p00[0] + tx*(1-ty)*p10[0] + (1-tx)*ty*p01[0] + tx*ty*p11[0];
    double g = (1-tx)*(1-ty)*p00[1] + tx*(1-ty)*p10[1] + (1-tx)*ty*p01[1] + tx*ty*p11[1];
    double b = (1-tx)*(1-ty)*p00[2] + tx*(1-ty)*p10[2] + (1-tx)*ty*p01[2] + tx*ty*p11[2];

    return rgb8_to_hsv64((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

static HSV64 hsv64_blend_add(HSV64 dst, HSV64 src, unsigned short alpha /*0..65535*/) {
    int dh = (int)src.h - (int)dst.h;
    if (dh > 32768) dh -= 65536;
    if (dh < -32768) dh += 65536;
    int h = dst.h + (int)((alpha / 65535.0) * dh);
    if (h < 0) h += 65536; if (h > 65535) h -= 65536;

    int s = clampi((int)dst.s + (int)((alpha / 65535.0) * src.s), 0, 65535);
    int v = clampi((int)dst.v + (int)((alpha / 65535.0) * src.v), 0, 65535);
    HSV64 out = { (unsigned short)h, (unsigned short)s, (unsigned short)v, 65535 };
    return out;
}

// ----------------- Sinus-Farbmodulation ohne geometrische Verzerrung -----------------

static void ghost_sine_filter_hsv64(const unsigned char *srcRGB, int srcW, int srcH, unsigned int *dstARGB, int dstW, int dstH, double t, OptShared *os) {
    const int layers = 4;
    const double two_pi = 2.0 * M_PI;
    
    // NEU: Konstante Verschiebung zur Erhöhung der Grundhelligkeit
    const double V_SHIFT = 15000.0; 

    // ESC: lese Basis + addiere sinusförmige Dither (Intervallschätzung)
    pthread_mutex_lock(&os->mtx);
    double hue_gain  = os->hue_base  + os->hue_amp  * sin(2.0 * M_PI * os->hue_freq  * t);
    double sat_gain  = os->sat_base  + os->sat_amp  * sin(2.0 * M_PI * os->sat_freq  * t);
    double val_gain  = os->val_base  + os->val_amp  * sin(2.0 * M_PI * os->val_freq  * t);
    double tint_alpha= os->tint_base + os->tint_amp * sin(2.0 * M_PI * os->tint_freq * t);
    pthread_mutex_unlock(&os->mtx);

    // Hintergrund
    HSV64 base = { (unsigned short)llround(220.0 * (65535.0/360.0)),
                   (unsigned short)(7000),
                   (unsigned short)(4000),
                   65535 };

    // **Speicher für Zielbild (acc) basiert jetzt auf dstW * dstH**
    HSV64 *acc = (HSV64*)malloc(sizeof(HSV64) * dstW * dstH);
    for (int i = 0; i < dstW*dstH; ++i) acc[i] = base;

    double sum_sat = 0.0, sum_val = 0.0;

    for (int g = 0; g < layers; ++g) {
        double phase = t * (0.6 + 0.2 * sin(t + g));
        double freqx = 1.2 + 0.2 * g;
        double freqy = 0.8 + 0.2 * g;

        double h_drift_deg = 12.0 * sin(0.7 * t + 0.5 * g);
        unsigned short h_tint = (unsigned short)(( (220.0 + h_drift_deg) * (65535.0/360.0) ));
        unsigned short s_tint = (unsigned short)(8000 + 3000 * fabs(sin(0.4 * t + g)));
        unsigned short v_tint = (unsigned short)(3000 + 3000 * fabs(sin(0.5 * t + 0.7 * g)));

        // Alpha skaliert
        double alpha_scale = tint_alpha; if (alpha_scale < 0.1) alpha_scale = 0.1; if (alpha_scale > 0.8) alpha_scale = 0.8;
        unsigned short alpha = (unsigned short)clampi((int)llround((18000.0 + 12000.0 * fabs(sin(0.3 * t + g))) * alpha_scale), 0, 65535);

        for (int y = 0; y < dstH; ++y) {
            for (int x = 0; x < dstW; ++x) {
                double nx = (double)x / (double)dstW; // Normalisierte Koordinaten basierend auf der FENSTERGRÖSSE
                double ny = (double)y / (double)dstH;

                double s1 = sin(two_pi * (freqx * nx + phase));
                double s2 = sin(two_pi * (freqy * ny + 0.7 * phase + 0.3 * g));
                double s3 = sin(two_pi * (1.7 * freqx * nx + 1.0 * phase + 0.2 * g));
                double comb = s1 + 0.6 * s2 + 0.4 * s3;

                // Bild-Sample wird skaliert (neue Funktion)
                HSV64 sample = sample_rgb_to_hsv64_bilinear_scaled(srcRGB, srcW, srcH, dstW, dstH, x, y);

                // Modulation gemäß aktuellen ESC-Gains
                int h_mod = (int)sample.h + (int)llround((65535.0/360.0) * (hue_gain * comb));
                if (h_mod < 0) h_mod += 65536; if (h_mod > 65535) h_mod -= 65536;

                int s_mod = clampi((int)sample.s + (int)llround(sat_gain * comb), 0, 65535);
                // ANGEPASST: Füge V_SHIFT zur Basishelligkeit hinzu
                int v_mod = clampi((int)sample.v + (int)llround(V_SHIFT + val_gain * sin(0.5 * t + ny * two_pi)), 0, 65535);

                HSV64 mod = { (unsigned short)h_mod, (unsigned short)s_mod, (unsigned short)v_mod, 65535 };
                HSV64 tint = { h_tint, s_tint, v_tint, 65535 };

                acc[y*dstW + x] = hsv64_blend_add(acc[y*dstW + x], mod, alpha);
                acc[y*dstW + x] = hsv64_blend_add(acc[y*dstW + x], tint, (unsigned short)llround(alpha * 0.4));

                sum_sat += acc[y*dstW + x].s;
                sum_val += acc[y*dstW + x].v;
            }
        }
    }

    // Ausgabe in RGB8
    for (int i = 0; i < dstW*dstH; ++i) {
        unsigned char r,g,b; hsv64_to_rgb8(acc[i], &r, &g, &b);
        dstARGB[i] = pack_argb(0xFF, r, g, b);
    }

    // Metriken an Optimizer übergeben (EMA über Frames könnte man hier auch nutzen)
    pthread_mutex_lock(&os->mtx);
    os->avg_sat = sum_sat / (double)(dstW * dstH);
    os->avg_val = sum_val / (double)(dstW * dstH);
    pthread_mutex_unlock(&os->mtx);

    free(acc);
}

// ----------------- Main: V4L2 + X11 + Threads -----------------

// Hilfsfunktion zum Maximieren des Fensters (X11 spezifisch)
static void maximize_window(Display *dpy, Window win) {
    Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom wm_state_maximized_horz = XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom wm_state_maximized_vert = XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_VERT", False);

    if (wm_state != None && wm_state_maximized_horz != None && wm_state_maximized_vert != None) {
        XClientMessageEvent xclient;
        memset(&xclient, 0, sizeof(xclient));
        xclient.type = ClientMessage;
        xclient.window = win;
        xclient.message_type = wm_state;
        xclient.format = 32;
        xclient.data.l[0] = 1; // 1 = _NET_WM_STATE_ADD
        xclient.data.l[1] = wm_state_maximized_horz;
        xclient.data.l[2] = wm_state_maximized_vert;
        xclient.data.l[3] = 0;
        xclient.data.l[4] = 0;
        XSendEvent(dpy, DefaultRootWindow(dpy), False, SubstructureNotifyMask | SubstructureRedirectMask, (XEvent *)&xclient);
    }
}

int main(void) {
    // Ursprüngliche V4L2-Parameter (fest)
    const int V4L2_W = 640, V4L2_H = 480;

    // --------- V4L2-Initialisierung (unverändert) ----------
    const char *dev = "/dev/video0";
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) { perror("open /dev/video0"); return 1; }

    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); return 1; }

    struct v4l2_format fmt; memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = V4L2_W;
    fmt.fmt.pix.height = V4L2_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return 1; }
    // V4L2_W und V4L2_H könnten sich hier ändern, aber wir nehmen die gefragten 640x480 an.
    // Falls die Kamera eine andere Größe wählt:
    // V4L2_W = fmt.fmt.pix.width; V4L2_H = fmt.fmt.pix.height;


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

    // --------- X11-Initialisierung (modifiziert) ----------
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "XOpenDisplay failed\n"); return 1; }
    int scr = DefaultScreen(dpy);
    // Setze anfängliche Fenstergröße auf V4L2-Größe, wird gleich maximiert
    current_win_W = V4L2_W;
    current_win_H = V4L2_H;
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 100, 100, current_win_W, current_win_H, 1,
                                     BlackPixel(dpy, scr), WhitePixel(dpy, scr));
    XStoreName(dpy, win, "Live /dev/video0 + HSV64 (ohne Wellen) + ESC-Optimierung");
    // Wichtig: StructureNotifyMask fängt Resize-Events (ConfigureNotify)
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);
    // 1. Fenster maximieren
    maximize_window(dpy, win);

    GC gc = XCreateGC(dpy, win, 0, NULL);

    // Initialisiere XImage, argb und rgb mit der Anfangsgröße (640x480)
    // Diese werden später bei Resize-Events neu zugewiesen und skaliert.
    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, scr), 24, ZPixmap, 0,
                              malloc(current_win_W*current_win_H*4), current_win_W, current_win_H, 32, current_win_W*4);
    if (!xi || !xi->data) { fprintf(stderr, "XCreateImage failed\n"); return 1; }

    // rgb speichert das unveränderte V4L2-Bild (fest V4L2_W x V4L2_H)
    unsigned char *rgb = malloc(V4L2_W*V4L2_H*3);
    // argb ist das Zielbild im Fenster, dessen Größe sich ändert
    unsigned int *argb = (unsigned int*)xi->data;

    struct timespec ts = {0, 16 * 1000 * 1000}; // ~60 FPS
    double t = 0.0, dt = 0.016;
    int running = 1;

    // --------- ESC-Optimierungsthread starten ----------
    OptShared os; optshared_init(&os);
    pthread_t opt_th;
    if (pthread_create(&opt_th, NULL, optimizer_thread, &os) != 0) {
        fprintf(stderr, "pthread_create failed\n"); return 1;
    }

    while (running) {
        while (XPending(dpy)) {
            XEvent e; XNextEvent(dpy, &e);
            if (e.type == KeyPress) {
                if (XLookupKeysym(&e.xkey, 0) == XK_Escape) {
                    running = 0;
                }
            } else if (e.type == ConfigureNotify) {
                // Fenstergröße hat sich geändert
                XConfigureEvent *c_e = &e.xconfigure;
                if (current_win_W != c_e->width || current_win_H != c_e->height) {
                    current_win_W = c_e->width;
                    current_win_H = c_e->height;

                    // XImage neu erstellen und alten Speicher freigeben
                    XDestroyImage(xi);
                    xi = XCreateImage(dpy, DefaultVisual(dpy, scr), 24, ZPixmap, 0,
                                      malloc(current_win_W*current_win_H*4), current_win_W, current_win_H, 32, current_win_W*4);
                    argb = (unsigned int*)xi->data; // argb Pointer aktualisieren

                    if (!xi || !xi->data) { fprintf(stderr, "XCreateImage failed (resize)\n"); running = 0; }
                }
            }
        }

        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == 0) {
            unsigned char *yuyv = (unsigned char *)buffers[buf.index].start;
            for (int y = 0; y < V4L2_H; ++y)
                yuyv_to_rgb_row(yuyv + y * (V4L2_W*2), V4L2_W, rgb + y * (V4L2_W*3));

            // Filter wird mit V4L2-Größe (src) und aktueller Fenstergröße (dst) aufgerufen
            ghost_sine_filter_hsv64(rgb, V4L2_W, V4L2_H, argb, current_win_W, current_win_H, t, &os);

            // XPutImage füllt das Fenster mit der aktuellen Größe
            XPutImage(dpy, win, gc, xi, 0, 0, 0, 0, current_win_W, current_win_H);
            XFlush(dpy);

            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF (requeue)"); break; }

            t += dt; nanosleep(&ts, NULL);
        } else {
            struct timespec ts2 = {0, 5 * 1000 * 1000};
            nanosleep(&ts2, NULL);
        }
    }

    // --------- Aufräumen und Thread stoppen ----------
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
