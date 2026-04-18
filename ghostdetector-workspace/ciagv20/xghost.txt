// gcc -O2 v4l2_ghost_cia_dynamic.c -o v4l2_ghost_cia_dynamic -lX11 -lXft -lm -lpthread
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xft/Xft.h>
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
    unsigned short h;
    unsigned short s;
    unsigned short v;
    unsigned short a;
} HSV64;

static int clampi(int v, int lo, int hi){ return v<lo?lo:(v>hi?hi:v); }
static unsigned int pack_argb(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
    return ((unsigned int)a<<24)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b);
}

// ----------------- OptShared (wie vorher) -----------------
typedef struct {
    double hue_base, sat_base, val_base, tint_base;
    double hue_amp, hue_freq, sat_amp, sat_freq, val_amp, val_freq, tint_amp, tint_freq;
    double target_sat, target_val;
    double avg_sat, avg_val;
    double J;
    int running;
    pthread_mutex_t mtx;
} OptShared;

static void optshared_init(OptShared *os) {
    os->hue_base  = 8.0; os->sat_base  = 2500.0; os->val_base  = 2500.0; os->tint_base = 0.4;
    os->hue_amp  = 2.0;   os->hue_freq  = 0.9;
    os->sat_amp  = 500.0; os->sat_freq  = 0.7;
    os->val_amp  = 500.0; os->val_freq  = 0.5;
    os->tint_amp = 0.08;  os->tint_freq = 0.6;
    os->target_sat = 18000.0; os->target_val = 22000.0;
    os->avg_sat = 0.0; os->avg_val = 0.0; os->J = 0.0;
    os->running = 1; pthread_mutex_init(&os->mtx, NULL);
}

static void* optimizer_thread(void *arg) {
    OptShared *os = (OptShared*)arg;
    const double w_sat = 1.0, w_val = 1.0;
    double g_hue = 0.0, g_sat = 0.0, g_val = 0.0, g_tint = 0.0;
    const double ema_alpha = 0.05;
    const double lr_hue  = 5e-3, lr_sat  = 5e-4, lr_val  = 5e-4, lr_tint = 1e-3;
    const double hue_min = 0.0, hue_max = 20.0;
    const double sat_min = 500.0, sat_max = 6000.0;
    const double val_min = 500.0, val_max = 6000.0;
    const double tint_min = 0.1, tint_max = 0.8;
    struct timespec ts = {0, 10 * 1000 * 1000};
    double t = 0.0, dt = 0.01;
    while (1) {
        pthread_mutex_lock(&os->mtx);
        int running = os->running;
        double avg_sat = os->avg_sat;
        double avg_val = os->avg_val;
        double target_sat = os->target_sat;
        double target_val = os->target_val;
        double e_sat = avg_sat - target_sat;
        double e_val = avg_val - target_val;
        os->J = w_sat * e_sat * e_sat + w_val * e_val * e_val;
        double s_hue  = sin(2.0 * M_PI * os->hue_freq  * t);
        double s_sat  = sin(2.0 * M_PI * os->sat_freq  * t);
        double s_val  = sin(2.0 * M_PI * os->val_freq  * t);
        double s_tint = sin(2.0 * M_PI * os->tint_freq * t);
        g_hue  = (1.0 - ema_alpha) * g_hue  + ema_alpha * (os->J * s_hue);
        g_sat  = (1.0 - ema_alpha) * g_sat  + ema_alpha * (os->J * s_sat);
        g_val  = (1.0 - ema_alpha) * g_val  + ema_alpha * (os->J * s_val);
        g_tint = (1.0 - ema_alpha) * g_tint + ema_alpha * (os->J * s_tint);
        os->hue_base  -= lr_hue  * g_hue;
        os->sat_base  -= lr_sat  * g_sat;
        os->val_base  -= lr_val  * g_val;
        os->tint_base -= lr_tint * g_tint;
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

// ----------------- YUV/RGB/HSV helpers -----------------

static void yuyv_to_rgb_row(unsigned char *yuyv, int width, unsigned char *rgb) {
    for (int x = 0; x < width; x += 2) {
        int y0 = yuyv[0], u = yuyv[1], y1 = yuyv[2], v = yuyv[3];
        yuyv += 4;
        int uu = u - 128, vv = v - 128;
        int r0 = clampi((int)llround(y0 + 1.402 * vv), 0, 255);
        int g0 = clampi((int)llround(y0 - 0.344136 * uu - 0.714136 * vv), 0, 255);
        int b0 = clampi((int)llround(y0 + 1.772 * uu), 0, 255);
        int r1 = clampi((int)llround(y1 + 1.402 * vv), 0, 255);
        int g1 = clampi((int)llround(y1 - 0.344136 * uu - 0.714136 * vv), 0, 255);
        int b1 = clampi((int)llround(y1 + 1.772 * uu), 0, 255);
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

static void apply_sepia(unsigned char *r, unsigned char *g, unsigned char *b) {
    double rd = (0.393 * (*r) + 0.769 * (*g) + 0.189 * (*b));
    double gd = (0.349 * (*r) + 0.686 * (*g) + 0.168 * (*b));
    double bd = (0.272 * (*r) + 0.534 * (*g) + 0.131 * (*b));
    double mix = 0.6;
    *r = (unsigned char)clampi((int)llround(mix * rd + (1.0 - mix) * (*r)), 0, 255);
    *g = (unsigned char)clampi((int)llround(mix * gd + (1.0 - mix) * (*g)), 0, 255);
    *b = (unsigned char)clampi((int)llround(mix * bd + (1.0 - mix) * (*b)), 0, 255);
}

// ----------------- PerfParams and adjuster (loop-limiters) -----------------

typedef struct {
    int DS;
    int sobel_padding;
    double ema_alpha_global;
    double hue_saturation_boost;
    double decay_rate;
    int effort_level;
} PerfParams;

static void adjust_perf_params(double fps, PerfParams *pp, double smoothing) {
    if (!pp) return;
    PerfParams target;
    if (fps >= 55.0) {
        target.DS = 1; target.sobel_padding = 1; target.ema_alpha_global = 0.12;
        target.hue_saturation_boost = 1.18; target.decay_rate = 1.0; target.effort_level = 3;
    } else if (fps >= 35.0) {
        target.DS = 1; target.sobel_padding = 1; target.ema_alpha_global = 0.10;
        target.hue_saturation_boost = 1.15; target.decay_rate = 1.0; target.effort_level = 2;
    } else if (fps >= 20.0) {
        target.DS = 2; target.sobel_padding = 2; target.ema_alpha_global = 0.08;
        target.hue_saturation_boost = 1.12; target.decay_rate = 1.2; target.effort_level = 1;
    } else if (fps >= 10.0) {
        target.DS = 3; target.sobel_padding = 2; target.ema_alpha_global = 0.06;
        target.hue_saturation_boost = 1.08; target.decay_rate = 1.5; target.effort_level = 1;
    } else {
        target.DS = 4; target.sobel_padding = 3; target.ema_alpha_global = 0.04;
        target.hue_saturation_boost = 1.05; target.decay_rate = 2.0; target.effort_level = 0;
    }
    double s = smoothing;
    pp->DS = (int)llround((1.0 - s) * pp->DS + s * target.DS);
    pp->sobel_padding = (int)llround((1.0 - s) * pp->sobel_padding + s * target.sobel_padding);
    pp->ema_alpha_global = (1.0 - s) * pp->ema_alpha_global + s * target.ema_alpha_global;
    pp->hue_saturation_boost = (1.0 - s) * pp->hue_saturation_boost + s * target.hue_saturation_boost;
    pp->decay_rate = (1.0 - s) * pp->decay_rate + s * target.decay_rate;
    pp->effort_level = (int)llround((1.0 - s) * pp->effort_level + s * target.effort_level);
    if (pp->DS < 1) pp->DS = 1; if (pp->DS > 8) pp->DS = 8;
    if (pp->sobel_padding < 1) pp->sobel_padding = 1; if (pp->sobel_padding > 4) pp->sobel_padding = 4;
    if (pp->ema_alpha_global < 0.01) pp->ema_alpha_global = 0.01; if (pp->ema_alpha_global > 0.5) pp->ema_alpha_global = 0.5;
    if (pp->hue_saturation_boost < 1.0) pp->hue_saturation_boost = 1.0; if (pp->hue_saturation_boost > 2.0) pp->hue_saturation_boost = 2.0;
    if (pp->decay_rate < 0.1) pp->decay_rate = 0.1; if (pp->decay_rate > 10.0) pp->decay_rate = 10.0;
    if (pp->effort_level < 0) pp->effort_level = 0; if (pp->effort_level > 3) pp->effort_level = 3;
}

// ----------------- Downsampled CIA Ghost Filter (uses PerfParams) -----------------

static void ghost_filter_downsampled(const unsigned char *srcRGB, int W, int H, unsigned int *dstARGB, double t, OptShared *os,
                                     float *edge_mag_buf, double *age_buf, double dt_frame, const PerfParams *pp)
{
    int DS = pp ? pp->DS : 2;
    const int Wd = (W + DS - 1) / DS;
    const int Hd = (H + DS - 1) / DS;
    const double max_age_s = 24.0 * 3600.0;
    double decay_rate = pp ? pp->decay_rate : 1.0;
    double ema_alpha_global = pp ? pp->ema_alpha_global : 0.12;
    double hue_saturation_boost = pp ? pp->hue_saturation_boost : 1.15;
    int effort = pp ? pp->effort_level : 2;

    HSV64 *acc = (HSV64*)malloc(sizeof(HSV64) * W * H);
    float *lum_full = (float*)malloc(sizeof(float) * W * H);
    if (!acc || !lum_full) { free(acc); free(lum_full); return; }
    double sum_sat = 0.0, sum_val = 0.0;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int idx = y*W + x;
            const unsigned char *p = srcRGB + idx*3;
            HSV64 h = rgb8_to_hsv64(p[0], p[1], p[2]);
            acc[idx] = h;
            lum_full[idx] = 0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2];
            sum_sat += (double)h.s;
            sum_val += (double)h.v;
        }
    }

    float *lum_ds = (float*)malloc(sizeof(float) * Wd * Hd);
    if (!lum_ds) { free(acc); free(lum_full); return; }
    for (int yd = 0; yd < Hd; ++yd) {
        for (int xd = 0; xd < Wd; ++xd) {
            double s = 0.0; int cnt = 0;
            int x0 = xd * DS, y0 = yd * DS;
            for (int yy = y0; yy < y0 + DS && yy < H; ++yy) {
                for (int xx = x0; xx < x0 + DS && xx < W; ++xx) {
                    s += lum_full[yy*W + xx]; cnt++;
                }
            }
            lum_ds[yd*Wd + xd] = cnt ? (float)(s / (double)cnt) : 0.0f;
        }
    }

    float *edge_ds = (float*)malloc(sizeof(float) * Wd * Hd);
    if (!edge_ds) { free(acc); free(lum_full); free(lum_ds); return; }
    for (int i = 0; i < Wd*Hd; ++i) edge_ds[i] = 0.0f;
    for (int y = 1; y < Hd-1; ++y) {
        for (int x = 1; x < Wd-1; ++x) {
            int i = y*Wd + x;
            float gx = -lum_ds[(y-1)*Wd + (x-1)] - 2.0f*lum_ds[y*Wd + (x-1)] - lum_ds[(y+1)*Wd + (x-1)]
                       + lum_ds[(y-1)*Wd + (x+1)] + 2.0f*lum_ds[y*Wd + (x+1)] + lum_ds[(y+1)*Wd + (x+1)];
            float gy = -lum_ds[(y-1)*Wd + (x-1)] - 2.0f*lum_ds[(y-1)*Wd + x] - lum_ds[(y-1)*Wd + (x+1)]
                       + lum_ds[(y+1)*Wd + (x-1)] + 2.0f*lum_ds[(y+1)*Wd + x] + lum_ds[(y+1)*Wd + (x+1)];
            edge_ds[i] = sqrtf(gx*gx + gy*gy);
        }
    }

    float maxmag_ds = 1e-6f;
    for (int i = 0; i < Wd*Hd; ++i) if (edge_ds[i] > maxmag_ds) maxmag_ds = edge_ds[i];
    float thr_ds = fmaxf(8.0f, 0.12f * maxmag_ds);

    for (int y = 0; y < H; ++y) {
        double fy = (double)y / (double)DS;
        int y0 = (int)floor(fy); int y1 = y0 + 1; double ty = fy - y0;
        if (y0 < 0) y0 = 0; if (y1 >= Hd) y1 = Hd - 1;
        for (int x = 0; x < W; ++x) {
            double fx = (double)x / (double)DS;
            int x0 = (int)floor(fx); int x1 = x0 + 1; double tx = fx - x0;
            if (x0 < 0) x0 = 0; if (x1 >= Wd) x1 = Wd - 1;
            float v00 = edge_ds[y0*Wd + x0];
            float v10 = edge_ds[y0*Wd + x1];
            float v01 = edge_ds[y1*Wd + x0];
            float v11 = edge_ds[y1*Wd + x1];
            float val = (1.0f - tx) * (1.0f - ty) * v00 + tx * (1.0f - ty) * v10 + (1.0f - tx) * ty * v01 + tx * ty * v11;
            edge_mag_buf[y*W + x] = val;
        }
    }

    float maxmag = 1e-6f;
    for (int i = 0; i < W*H; ++i) if (edge_mag_buf[i] > maxmag) maxmag = edge_mag_buf[i];

    for (int i = 0; i < W*H; ++i) {
        int is_edge = (edge_mag_buf[i] > thr_ds) ? 1 : 0;
        if (is_edge) {
            age_buf[i] += dt_frame;
            if (age_buf[i] > max_age_s) age_buf[i] = max_age_s;
        } else {
            age_buf[i] -= decay_rate * dt_frame;
            if (age_buf[i] < 0.0) age_buf[i] = 0.0;
        }
        HSV64 cur = acc[i];
        if (is_edge) {
            double target_s = clampi((int)llround((double)cur.s * hue_saturation_boost), 0, 65535);
            cur.s = (unsigned short)llround((1.0 - ema_alpha_global) * cur.s + ema_alpha_global * target_s);
            double target_v = clampi((int)llround((double)cur.v * 1.05), 0, 65535);
            cur.v = (unsigned short)llround((1.0 - ema_alpha_global) * cur.v + ema_alpha_global * target_v);
        } else {
            double target_s = 6000.0;
            cur.s = (unsigned short)llround((1.0 - 0.03) * cur.s + 0.03 * target_s);
            double target_v = 4000.0;
            cur.v = (unsigned short)llround((1.0 - 0.03) * cur.v + 0.03 * target_v);
        }
        acc[i] = cur;
    }

    for (int i = 0; i < W*H; ++i) {
        unsigned char r,g,b;
        hsv64_to_rgb8(acc[i], &r, &g, &b);
        apply_sepia(&r, &g, &b);
        if (age_buf[i] > 0.0) {
            double a = age_buf[i]; if (a > max_age_s) a = max_age_s;
            double tn = a / max_age_s;
            double tlog = (tn <= 0.0) ? 0.0 : log10(1.0 + 9.0 * tn);
            double cr = 0.0, cg = 255.0*(1.0 - tlog) + 80.0 * tlog, cb = 255.0 * tlog;
            double inten = (double)edge_mag_buf[i] / (double)(maxmag + 1e-6);
            if (inten > 1.0) inten = 1.0;
            double alpha = 0.30 * inten;
            if (alpha > 1.0) alpha = 1.0;
            if (age_buf[i] < 60.0) {
                double factor = 1.0 + 0.35 * (1.0 - age_buf[i] / 60.0);
                cr *= factor; cg *= factor; cb *= factor;
            }
            r = (unsigned char)clampi((int)llround((1.0 - alpha) * r + alpha * cr), 0, 255);
            g = (unsigned char)clampi((int)llround((1.0 - alpha) * g + alpha * cg), 0, 255);
            b = (unsigned char)clampi((int)llround((1.0 - alpha) * b + alpha * cb), 0, 255);
        }
        dstARGB[i] = pack_argb(0xFF, r, g, b);
    }

    pthread_mutex_lock(&os->mtx);
    os->avg_sat = sum_sat / (double)(W * H);
    os->avg_val = sum_val / (double)(W * H);
    pthread_mutex_unlock(&os->mtx);

    free(acc); free(lum_full); free(lum_ds); free(edge_ds);
}

// ----------------- Main: V4L2 + X11 + Threads + Xft Overlay -----------------

int main(void) {
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

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "XOpenDisplay failed\n"); return 1; }
    int scr = DefaultScreen(dpy);
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 100, 100, W, H, 1,
                                     BlackPixel(dpy, scr), WhitePixel(dpy, scr));
    XStoreName(dpy, win, "Live /dev/video0 • CIA Ghostviewer");
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, NULL);

    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, scr), 24, ZPixmap, 0,
                              malloc(W*H*4), W, H, 32, W*4);
    if (!xi || !xi->data) { fprintf(stderr, "XCreateImage failed\n"); return 1; }

    unsigned char *rgb = malloc(W*H*3);
    unsigned int *argb = (unsigned int*)xi->data;

    float *edge_mag_buf = (float*)malloc(sizeof(float) * W * H);
    double *age_buf = (double*)malloc(sizeof(double) * W * H);
    if (!edge_mag_buf || !age_buf) { fprintf(stderr, "alloc failed\n"); return 1; }
    for (int i = 0; i < W*H; ++i) { edge_mag_buf[i] = 0.0f; age_buf[i] = 0.0; }

    // Xft init
    XftDraw *xft_draw = XftDrawCreate(dpy, win, DefaultVisual(dpy, scr), DefaultColormap(dpy, scr));
    int font_pixels = H / 12; if (font_pixels < 8) font_pixels = 8;
    char fontname[256];
    snprintf(fontname, sizeof(fontname), "monospace-%d", font_pixels);
    XftFont *xft_font = XftFontOpenName(dpy, DefaultScreen(dpy), fontname);
    if (!xft_font) {
        xft_font = XftFontOpenName(dpy, DefaultScreen(dpy), "DejaVu Sans Mono-12");
        if (!xft_font) xft_font = XftFontOpenName(dpy, DefaultScreen(dpy), "monospace-12");
    }
    XftColor xft_white, xft_black;
    XRenderColor xr_white = { .red = 0xffff, .green = 0xffff, .blue = 0xffff, .alpha = 0xffff };
    XRenderColor xr_black = { .red = 0x0000, .green = 0x0000, .blue = 0x0000, .alpha = 0xffff };
    XftColorAllocValue(dpy, DefaultVisual(dpy, scr), DefaultColormap(dpy, scr), &xr_white, &xft_white);
    XftColorAllocValue(dpy, DefaultVisual(dpy, scr), DefaultColormap(dpy, scr), &xr_black, &xft_black);

    struct timespec ts_sleep = {0, 16 * 1000 * 1000};
    double t = 0.0, dt = 0.016;
    int running = 1;

    OptShared os; optshared_init(&os);
    pthread_t opt_th;
    if (pthread_create(&opt_th, NULL, optimizer_thread, &os) != 0) { fprintf(stderr, "pthread_create failed\n"); return 1; }

    // perf params
    PerfParams perf;
    perf.DS = 2; perf.sobel_padding = 2; perf.ema_alpha_global = 0.12;
    perf.hue_saturation_boost = 1.15; perf.decay_rate = 1.0; perf.effort_level = 2;

    double fps_ema = 0.0; const double fps_alpha = 0.12;
    struct timespec last_ts; clock_gettime(CLOCK_MONOTONIC, &last_ts);

    while (running) {
        while (XPending(dpy)) {
            XEvent e; XNextEvent(dpy, &e);
            if (e.type == KeyPress) running = 0;
        }

        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == 0) {
            unsigned char *yuyv = (unsigned char *)buffers[buf.index].start;
            for (int y = 0; y < H; ++y) yuyv_to_rgb_row(yuyv + y * (W*2), W, rgb + y * (W*3));

            // FPS timing
            struct timespec now; clock_gettime(CLOCK_MONOTONIC, &now);
            double elapsed = (now.tv_sec - last_ts.tv_sec) + (now.tv_nsec - last_ts.tv_nsec) * 1e-9;
            last_ts = now;
            double inst_fps = elapsed > 1e-6 ? 1.0 / elapsed : 60.0;
            if (fps_ema <= 0.0) fps_ema = inst_fps;
            else fps_ema = (1.0 - fps_alpha) * fps_ema + fps_alpha * inst_fps;

            adjust_perf_params(fps_ema, &perf, 0.12);

            // call filter with dynamic perf params
            ghost_filter_downsampled(rgb, W, H, argb, t, &os, edge_mag_buf, age_buf, dt, &perf);

            // Build overlay: time + FPS (4 digits) + DS + label
            struct timespec ts_now; clock_gettime(CLOCK_REALTIME, &ts_now);
            time_t sec = ts_now.tv_sec;
            struct tm tmv; localtime_r(&sec, &tmv);
            int ms = (int)(ts_now.tv_nsec / 1000000);
            char timestr[64];
            snprintf(timestr, sizeof(timestr), "%02d:%02d:%02d:%04d", tmv.tm_hour, tmv.tm_min, tmv.tm_sec, ms);
            int fps_display = (int)llround(fps_ema);
            if (fps_display < 0) fps_display = 0; if (fps_display > 9999) fps_display = 9999;
            char fpsbuf[8]; snprintf(fpsbuf, sizeof(fpsbuf), "%04d", fps_display);
            char overlay[256];
            snprintf(overlay, sizeof(overlay), "%s  FPS %s  DS %d  CIA-Ghostviewer", timestr, fpsbuf, perf.DS);

            // measure extents with Xft
            XGlyphInfo ext; FcChar8 *utf8 = (FcChar8*)overlay;
            if (xft_font) XftTextExtentsUtf8(dpy, xft_font, utf8, strlen(overlay), &ext);
            int text_w = xft_font ? ext.xOff : (int)strlen(overlay) * (font_pixels/2);
            int text_h = xft_font ? (xft_font->ascent + xft_font->descent) : font_pixels;
            int pad_x = 8, pad_y = 6;
            int rx = W - text_w - pad_x; if (rx < 0) rx = 0;
            int ry = pad_y;
            int text_x = rx;
            int text_y = ry + (xft_font ? xft_font->ascent : font_pixels);

            // draw background rectangle using X (fast)
            XSetForeground(dpy, gc, BlackPixel(dpy, scr));
            XFillRectangle(dpy, win, gc, rx - 4, ry - 2, text_w + 8, text_h + 4);

            // draw antialiased text with Xft
            if (xft_draw && xft_font) {
                XftDrawStringUtf8(xft_draw, &xft_white, xft_font, text_x, text_y, utf8, strlen(overlay));
            } else {
                XSetForeground(dpy, gc, WhitePixel(dpy, scr));
                XDrawString(dpy, win, gc, text_x, text_y, overlay, strlen(overlay));
            }

            XPutImage(dpy, win, gc, xi, 0, 0, 0, 0, W, H);
            XFlush(dpy);

            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF (requeue)"); break; }

            t += dt; nanosleep(&ts_sleep, NULL);
        } else {
            struct timespec ts2 = {0, 5 * 1000 * 1000};
            nanosleep(&ts2, NULL);
        }
    }

    pthread_mutex_lock(&os.mtx); os.running = 0; pthread_mutex_unlock(&os.mtx);
    pthread_join(opt_th, NULL);

    if (xft_font) XftFontClose(dpy, xft_font);
    if (xft_draw) XftDrawDestroy(xft_draw);
    XftColorFree(dpy, DefaultVisual(dpy, scr), DefaultColormap(dpy, scr), &xft_white);
    XftColorFree(dpy, DefaultVisual(dpy, scr), DefaultColormap(dpy, scr), &xft_black);

    XDestroyImage(xi);
    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE; ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < req.count; ++i) {
        if (buffers[i].start && buffers[i].start != MAP_FAILED)
            munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers); free(rgb); free(edge_mag_buf); free(age_buf);
    close(fd);
    return 0;
}

