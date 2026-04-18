// g.c
// CIA GhostViewer v25.0 — HSVA 4.0 Ghost Sepia + Contrast v3.0
// Enhanced: unified cleanup, signal-safe shutdown (self-pipe), MJPEG row reuse,
// MJPEG-dimension safety (skip if mismatch), SDL format/pitch fallback, telemetry.
// Build: gcc g.c -o g -Wall -Wextra -O2 -lSDL2 -ljpeg
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <time.h>
#include <inttypes.h>
#include <limits.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <jpeglib.h>

struct buffer { void *start; size_t length; };

static int xioctl(int fd, int req, void *arg) {
    int r;
    do r = ioctl(fd, req, arg);
    while (r == -1 && errno == EINTR);
    return r;
}

/* ---------- Configurable parameters (tweak or expose via CLI) ---------- */
static const float SEPIA_HUE_DEG = 30.0f;
static const float SEPIA_SAT_MULT = 0.45f;
static const float SEPIA_VAL_MULT = 1.05f;
static const float GHOST_ALPHA = 0.40f;
static const float HSVA_EXPOSURE = 1.0f;

/* Contrast v3.0 */
static const float CONTRAST_EMA_ALPHA = 0.08f;
static const float TARGET_STD_MULT = 1.0f;
static const float CONTRAST_GAIN_MIN = 0.6f;
static const float CONTRAST_GAIN_MAX = 2.0f;
static const float CONTRAST_APPLY_CLAMP = 0.98f;
static const float HISTORY_DECAY = 0.60f; /* used when updating history */

/* Safety caps */
static const int MJPEG_MAX_DIM = 8192;

/* ----------------------------------------------------------------------- */

/* time helpers */
static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* overflow check for size_t multiply */
static int mul_will_overflow_size_t(size_t a, size_t b) {
    if (a == 0 || b == 0) return 0;
    return a > SIZE_MAX / b;
}

/* signal handling via self-pipe */
static int selfpipe_fds[2] = { -1, -1 };
static volatile sig_atomic_t got_signal = 0;
static void sig_handler(int signo) {
    (void)signo;
    got_signal = 1;
    if (selfpipe_fds[1] >= 0) {
        const uint8_t c = 1;
        ssize_t _ = write(selfpipe_fds[1], &c, 1);
        (void)_;
    }
}

/* ---------- RGB/HSV helpers ---------- */
/* R,G,B in 0..255; H in 0..360; S,V in 0..1 */
static void rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b, float *h, float *s, float *v) {
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float mx = rf, mn = rf;
    if (gf > mx) mx = gf;
    if (bf > mx) mx = bf;
    if (gf < mn) mn = gf;
    if (bf < mn) mn = bf;
    float d = mx - mn;
    *v = mx;
    if (mx <= 0.0f) { *s = 0.0f; *h = 0.0f; return; }
    *s = d / mx;
    if (d <= 1e-6f) { *h = 0.0f; return; }
    if (mx == rf) *h = 60.0f * fmodf(((gf - bf) / d), 6.0f);
    else if (mx == gf) *h = 60.0f * (((bf - rf) / d) + 2.0f);
    else *h = 60.0f * (((rf - gf) / d) + 4.0f);
    if (*h < 0.0f) *h += 360.0f;
}

static void hsv_to_rgb(float h, float s, float v, unsigned char *r, unsigned char *g, unsigned char *b) {
    if (s <= 0.0f) {
        int vi = (int)(v * 255.0f + 0.5f);
        if (vi < 0) vi = 0;
        if (vi > 255) vi = 255;
        *r = *g = *b = (unsigned char)vi;
        return;
    }
    float hh = fmodf(h, 360.0f);
    if (hh < 0.0f) hh += 360.0f;
    float hf = hh / 60.0f;
    int i = (int)hf;
    float f = hf - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    float rf, gf, bf;
    switch (i % 6) {
        case 0: rf = v; gf = t; bf = p; break;
        case 1: rf = q; gf = v; bf = p; break;
        case 2: rf = p; gf = v; bf = t; break;
        case 3: rf = p; gf = q; bf = v; break;
        case 4: rf = t; gf = p; bf = v; break;
        default: rf = v; gf = p; bf = q; break;
    }
    int ri = (int)(rf * 255.0f + 0.5f);
    int gi = (int)(gf * 255.0f + 0.5f);
    int bi = (int)(bf * 255.0f + 0.5f);
    if (ri < 0) ri = 0; if (ri > 255) ri = 255;
    if (gi < 0) gi = 0; if (gi > 255) gi = 255;
    if (bi < 0) bi = 0; if (bi > 255) bi = 255;
    *r = (unsigned char)ri; *g = (unsigned char)gi; *b = (unsigned char)bi;
}

/* YUYV -> RGB24 */
static void yuyv_to_rgb24(const unsigned char *yuyv, unsigned char *rgb, int w, int h) {
    const unsigned char *p = yuyv;
    unsigned char *q = rgb;
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; i += 2) {
            int y0 = p[0], u = p[1], y1 = p[2], v = p[3];
            p += 4;
            int c = y0 - 16, d = u - 128, e = v - 128;
            int r = (298 * c + 409 * e + 128) >> 8;
            int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
            int b = (298 * c + 516 * d + 128) >> 8;
            q[0] = (unsigned char)(r < 0 ? 0 : r > 255 ? 255 : r);
            q[1] = (unsigned char)(g < 0 ? 0 : g > 255 ? 255 : g);
            q[2] = (unsigned char)(b < 0 ? 0 : b > 255 ? 255 : b);
            q += 3;
            c = y1 - 16; d = u - 128; e = v - 128;
            r = (298 * c + 409 * e + 128) >> 8;
            g = (298 * c - 100 * d - 208 * e + 128) >> 8;
            b = (298 * c + 516 * d + 128) >> 8;
            q[0] = (unsigned char)(r < 0 ? 0 : r > 255 ? 255 : r);
            q[1] = (unsigned char)(g < 0 ? 0 : g > 255 ? 255 : g);
            q[2] = (unsigned char)(b < 0 ? 0 : b > 255 ? 255 : b);
            q += 3;
        }
    }
}

/* MJPEG decode into rgb buffer; uses a reusable row buffer passed by caller */
static int mjpeg_to_rgb24_reuse(const unsigned char *mjpeg, size_t mjpeg_len, unsigned char *rgb, int expected_w, int expected_h, unsigned char *rowbuf, size_t rowbuf_sz, int *out_w, int *out_h) {
    (void)expected_w; (void)expected_h;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW rowptr[1];
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char*)mjpeg, mjpeg_len);
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) { jpeg_destroy_decompress(&cinfo); return -1; }
    jpeg_start_decompress(&cinfo);
    int w = cinfo.output_width, h = cinfo.output_height, comps = cinfo.output_components;
    if (comps != 3) { jpeg_finish_decompress(&cinfo); jpeg_destroy_decompress(&cinfo); return -1; }
    if (w <= 0 || h <= 0 || w > MJPEG_MAX_DIM || h > MJPEG_MAX_DIM) { jpeg_finish_decompress(&cinfo); jpeg_destroy_decompress(&cinfo); return -1; }
    size_t need = (size_t)w * 3;
    if (need > rowbuf_sz) { jpeg_finish_decompress(&cinfo); jpeg_destroy_decompress(&cinfo); return -1; }
    while (cinfo.output_scanline < cinfo.output_height) {
        rowptr[0] = rowbuf;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
        size_t dst_off = (size_t)(cinfo.output_scanline - 1) * (size_t)w * 3;
        memcpy(rgb + dst_off, rowbuf, (size_t)w * 3);
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    if (out_w) *out_w = w;
    if (out_h) *out_h = h;
    return 0;
}

/* nanosleep helper */
static void sleep_seconds_fraction(double seconds) {
    if (seconds <= 0.0) return;
    time_t sec = (time_t)seconds;
    long nsec = (long)((seconds - (double)sec) * 1e9);
    if (nsec < 0) nsec = 0;
    if (nsec >= 1000000000L) { sec += 1; nsec -= 1000000000L; }
    struct timespec ts = { sec, nsec };
    nanosleep(&ts, NULL);
}

/* unified cleanup state */
struct app_state {
    int fd;
    struct buffer *buffers;
    unsigned int nbuf;
    int started_stream;
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Texture *tex;
    unsigned char *frame_rgb;     /* width*height*3 */
    unsigned char *history_rgb;   /* width*height*3 */
    unsigned char *jpeg_rowbuf;   /* reusable */
    size_t jpeg_rowbuf_sz;
    int tex_bpp; /* bytes per pixel for texture format used */
    int width, height;
    unsigned int pixfmt;
    /* telemetry */
    uint64_t cnt_captured;
    uint64_t cnt_dropped;
    uint64_t cnt_presented;
    uint64_t cnt_mjpeg_mismatch;
    uint64_t cnt_skipped_mjpeg;
};

/* cleanup helper */
static void cleanup_state(struct app_state *s, struct v4l2_requestbuffers *req) {
    if (!s) return;
    if (s->tex) SDL_DestroyTexture(s->tex);
    if (s->ren) SDL_DestroyRenderer(s->ren);
    if (s->win) SDL_DestroyWindow(s->win);
    SDL_Quit();
    if (s->frame_rgb) { free(s->frame_rgb); s->frame_rgb = NULL; }
    if (s->history_rgb) { free(s->history_rgb); s->history_rgb = NULL; }
    if (s->jpeg_rowbuf) { free(s->jpeg_rowbuf); s->jpeg_rowbuf = NULL; }
    if (s->fd >= 0) {
        if (s->started_stream) {
            enum v4l2_buf_type t = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            xioctl(s->fd, VIDIOC_STREAMOFF, &t);
        }
        if (s->buffers && req) {
            for (unsigned int i = 0; i < req->count; ++i) {
                if (s->buffers[i].start && s->buffers[i].length)
                    munmap(s->buffers[i].start, s->buffers[i].length);
            }
        }
        if (s->buffers) free(s->buffers);
        close(s->fd);
        s->fd = -1;
    }
}

/* Entry point */
int main(int argc, char **argv) {
    const char *dev_name = "/dev/video0";
    double slew_seconds = 2.0;
    if (argc > 1) dev_name = argv[1];
    if (argc > 2) slew_seconds = atof(argv[2]);

    struct app_state st;
    memset(&st, 0, sizeof(st));
    st.fd = -1;

    /* setup self-pipe and signal handlers */
    if (pipe(selfpipe_fds) < 0) {
        fprintf(stderr, "self-pipe pipe failed: %s\n", strerror(errno));
        return 1;
    }
    /* make read end non-blocking */
    int fl = fcntl(selfpipe_fds[0], F_GETFL, 0);
    fcntl(selfpipe_fds[0], F_SETFL, fl | O_NONBLOCK);
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    int fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) { fprintf(stderr, "open %s failed: %s\n", dev_name, strerror(errno)); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    st.fd = fd;

    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) { fprintf(stderr, "VIDIOC_QUERYCAP failed: %s\n", strerror(errno)); cleanup_state(&st, NULL); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }

    /* negotiate format: try YUYV then MJPEG */
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
        if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
            fprintf(stderr, "VIDIOC_S_FMT failed (YUYV/MJPEG): %s\n", strerror(errno));
            cleanup_state(&st, NULL); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1;
        }
    }
    st.width = fmt.fmt.pix.width;
    st.height = fmt.fmt.pix.height;
    st.pixfmt = fmt.fmt.pix.pixelformat;

    if ((st.width & 1) != 0) {
        fprintf(stderr, "Negotiated width %d is odd; YUYV requires even width. Aborting.\n", st.width);
        cleanup_state(&st, NULL); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1;
    }

    /* determine camera FPS robustly */
    double cam_fps = 30.0;
    struct v4l2_streamparm sp;
    memset(&sp, 0, sizeof(sp));
    sp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_G_PARM, &sp) == 0) {
        unsigned int den = 0, num = 0;
        den = sp.parm.capture.timeperframe.denominator;
        num = sp.parm.capture.timeperframe.numerator;
        if (den && num) cam_fps = (double)den / (double)num;
    }

    double initial_fps = cam_fps > 15.0 ? 5.0 : (cam_fps > 5.0 ? 2.0 : cam_fps);
    if (initial_fps <= 0.0) initial_fps = 1.0;

    /* request mmap buffers */
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        fprintf(stderr, "VIDIOC_REQBUFS failed: %s\n", strerror(errno));
        cleanup_state(&st, NULL); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1;
    }
    st.nbuf = req.count;
    st.buffers = calloc(req.count, sizeof(struct buffer));
    if (!st.buffers) { fprintf(stderr, "calloc buffers failed\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer b;
        memset(&b, 0, sizeof(b));
        b.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        b.memory = V4L2_MEMORY_MMAP;
        b.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &b) < 0) { fprintf(stderr, "VIDIOC_QUERYBUF failed\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
        st.buffers[i].length = b.length;
        st.buffers[i].start = mmap(NULL, b.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, b.m.offset);
        if (st.buffers[i].start == MAP_FAILED) { fprintf(stderr, "mmap failed\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    }
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer b;
        memset(&b, 0, sizeof(b));
        b.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        b.memory = V4L2_MEMORY_MMAP;
        b.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &b) < 0) { fprintf(stderr, "VIDIOC_QBUF failed\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    }
    enum v4l2_buf_type bt = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &bt) < 0) { fprintf(stderr, "VIDIOC_STREAMON failed\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    st.started_stream = 1;

    /* SDL init */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError()); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    st.win = SDL_CreateWindow("CIA GhostViewer v25.0 - HSVA 4.0 Sepia + Contrast v3.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, st.width, st.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!st.win) { fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError()); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    st.ren = SDL_CreateRenderer(st.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!st.ren) { st.ren = SDL_CreateRenderer(st.win, -1, 0); if (!st.ren) { fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError()); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; } }

    /* create texture with fallback formats; determine bpp */
    Uint32 texfmt_candidates[] = { SDL_PIXELFORMAT_RGB24, SDL_PIXELFORMAT_RGB888, SDL_PIXELFORMAT_ARGB8888 };
    Uint32 chosen_fmt = 0;
    for (size_t i = 0; i < sizeof(texfmt_candidates)/sizeof(texfmt_candidates[0]); ++i) {
        Uint32 fmt_try = texfmt_candidates[i];
        SDL_Texture *t = SDL_CreateTexture(st.ren, fmt_try, SDL_TEXTUREACCESS_STREAMING, st.width, st.height);
        if (t) {
            st.tex = t;
            chosen_fmt = fmt_try;
            break;
        }
    }
    if (!st.tex) { fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError()); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    int bpp = 3;
    if (chosen_fmt == SDL_PIXELFORMAT_RGB24) bpp = 3;
    else if (chosen_fmt == SDL_PIXELFORMAT_RGB888) bpp = 3;
    else bpp = 4;
    st.tex_bpp = bpp;

    /* allocate frame buffers with overflow checks */
    size_t pixels = (size_t)st.width * (size_t)st.height;
    if (mul_will_overflow_size_t(pixels, (size_t)3)) { fprintf(stderr, "frame size overflow\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    st.frame_rgb = malloc(pixels * 3);
    st.history_rgb = malloc(pixels * 3);
    if (!st.frame_rgb || !st.history_rgb) { fprintf(stderr, "alloc frame/history failed\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }
    memset(st.history_rgb, 0, pixels * 3);

    /* allocate jpeg row buffer once to max width */
    st.jpeg_rowbuf_sz = (size_t)st.width * 3;
    if (st.jpeg_rowbuf_sz < 4096) st.jpeg_rowbuf_sz = 4096;
    st.jpeg_rowbuf = malloc(st.jpeg_rowbuf_sz);
    if (!st.jpeg_rowbuf) { fprintf(stderr, "alloc jpeg rowbuf failed\n"); cleanup_state(&st, &req); close(selfpipe_fds[0]); close(selfpipe_fds[1]); return 1; }

    /* timing / slew */
    double start_time = now_seconds();
    double slew_t = slew_seconds > 0.0 ? slew_seconds : 0.0;
    double target_fps = cam_fps;
    double cur_fps = initial_fps;
    double last_present_time = now_seconds();
    double present_interval = 1.0 / (cur_fps > 0.0 ? cur_fps : 1.0);

    /* contrast EMA state */
    float ema_mean = 0.0f, ema_std = 0.0f, initial_frame_std = 0.01f;
    int ema_init = 0;

    /* main loop */
    int quit = 0;
    while (!quit) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(st.fd, &fds);
        FD_SET(selfpipe_fds[0], &fds);
        int nf = (st.fd > selfpipe_fds[0] ? st.fd : selfpipe_fds[0]) + 1;
        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 100000; /* 100 ms */
        int r = select(nf, &fds, NULL, NULL, &tv);
        if (r < 0) {
            if (errno == EINTR) continue;
            fprintf(stderr, "select failed: %s\n", strerror(errno));
            break;
        }
        if (FD_ISSET(selfpipe_fds[0], &fds)) {
            uint8_t buf[64];
            while (read(selfpipe_fds[0], buf, sizeof(buf)) > 0) {}
            if (got_signal) { quit = 1; }
        }
        if (!FD_ISSET(st.fd, &fds)) {
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) { quit = 1; break; }
                if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_q) { quit = 1; break; }
            }
            continue;
        }

        /* dequeue one frame */
        struct v4l2_buffer cur;
        memset(&cur, 0, sizeof(cur));
        cur.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        cur.memory = V4L2_MEMORY_MMAP;
        if (xioctl(st.fd, VIDIOC_DQBUF, &cur) < 0) {
            if (errno == EAGAIN || errno == EINTR) continue;
            fprintf(stderr, "VIDIOC_DQBUF failed: %s\n", strerror(errno));
            break;
        }
        st.cnt_captured++;

        /* aggressive drain: keep only newest, requeue dropped frames */
        while (1) {
            struct v4l2_buffer tmp;
            memset(&tmp, 0, sizeof(tmp));
            tmp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            tmp.memory = V4L2_MEMORY_MMAP;
            if (xioctl(st.fd, VIDIOC_DQBUF, &tmp) < 0) {
                if (errno == EAGAIN) break;
                if (errno == EINTR) continue;
                fprintf(stderr, "VIDIOC_DQBUF (drain) failed: %s\n", strerror(errno));
                goto out;
            } else {
                struct v4l2_buffer re;
                memset(&re, 0, sizeof(re));
                re.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                re.memory = V4L2_MEMORY_MMAP;
                re.index = cur.index;
                if (xioctl(st.fd, VIDIOC_QBUF, &re) < 0) {
                    fprintf(stderr, "VIDIOC_QBUF (drop) failed: %s\n", strerror(errno));
                } else {
                    st.cnt_dropped++;
                }
                cur = tmp;
            }
        }

        /* decode/copy into frame_rgb */
        int skip_present = 0;
        size_t pixels_count = (size_t)st.width * (size_t)st.height;
        if (st.pixfmt == V4L2_PIX_FMT_YUYV) {
            yuyv_to_rgb24((const unsigned char*)st.buffers[cur.index].start, st.frame_rgb, st.width, st.height);
        } else if (st.pixfmt == V4L2_PIX_FMT_MJPEG) {
            int jpeg_w = 0, jpeg_h = 0;
            if (mjpeg_to_rgb24_reuse((const unsigned char*)st.buffers[cur.index].start, cur.bytesused, st.frame_rgb, st.width, st.height, st.jpeg_rowbuf, st.jpeg_rowbuf_sz, &jpeg_w, &jpeg_h) != 0) {
                fprintf(stderr, "MJPEG decode failed or unsafe; skipping frame\n");
                skip_present = 1;
                st.cnt_skipped_mjpeg++;
            } else {
                if (jpeg_w != st.width || jpeg_h != st.height) {
                    st.cnt_mjpeg_mismatch++;
                    skip_present = 1;
                }
            }
        } else {
            memset(st.frame_rgb, 0, pixels_count * 3);
        }

        if (!skip_present) {
            /* Contrast v3.0: compute luminance mean/std for this frame */
            double sum = 0.0, sumsq = 0.0;
            for (size_t i = 0; i < pixels_count; ++i) {
                unsigned char *p = st.frame_rgb + i * 3;
                double L = (0.2126 * p[0] + 0.7152 * p[1] + 0.0722 * p[2]) / 255.0;
                sum += L; sumsq += L * L;
            }
            double mean = sum / (double)pixels_count;
            double var = sumsq / (double)pixels_count - mean * mean;
            if (var < 0.0) var = 0.0;
            double stddev = sqrt(var);

            if (!ema_init) {
                ema_mean = (float)mean;
                ema_std = (float)(stddev > 1e-6 ? stddev : initial_frame_std);
                initial_frame_std = (float)(ema_std > 1e-6 ? ema_std : 0.01f);
                ema_init = 1;
            } else {
                ema_mean = (1.0f - CONTRAST_EMA_ALPHA) * ema_mean + CONTRAST_EMA_ALPHA * (float)mean;
                ema_std  = (1.0f - CONTRAST_EMA_ALPHA) * ema_std  + CONTRAST_EMA_ALPHA * (float)stddev;
            }

            float target_std = TARGET_STD_MULT * initial_frame_std;
            float gain = 1.0f;
            if (ema_std > 1e-6f) gain = target_std / ema_std;
            if (gain < CONTRAST_GAIN_MIN) gain = CONTRAST_GAIN_MIN;
            if (gain > CONTRAST_GAIN_MAX) gain = CONTRAST_GAIN_MAX;

            /* Apply contrast by scaling RGB around ema_mean luminance */
            const double eps = 1e-6;
            for (size_t i = 0; i < pixels_count; ++i) {
                unsigned char *p = st.frame_rgb + i * 3;
                double r = p[0] / 255.0;
                double g = p[1] / 255.0;
                double b = p[2] / 255.0;
                double L = 0.2126 * r + 0.7152 * g + 0.0722 * b;
                double delta = L - (double)ema_mean;
                double scaled_delta = delta * (double)gain;
                double L_new = (double)ema_mean + scaled_delta;
                double m = 1.0;
                if (L > eps) m = L_new / L;
                else m = gain;
                double rr = r * m, gg = g * m, bb = b * m;
                double maxv = rr;
                if (gg > maxv) maxv = gg;
                if (bb > maxv) maxv = bb;
                if (maxv > CONTRAST_APPLY_CLAMP) {
                    double scale_back = CONTRAST_APPLY_CLAMP / maxv;
                    m *= scale_back;
                    rr = r * m; gg = g * m; bb = b * m;
                }
                int ri = (int)(rr * 255.0 + 0.5), gi = (int)(gg * 255.0 + 0.5), bi = (int)(bb * 255.0 + 0.5);
                if (ri < 0) ri = 0; if (ri > 255) ri = 255;
                if (gi < 0) gi = 0; if (gi > 255) gi = 255;
                if (bi < 0) bi = 0; if (bi > 255) bi = 255;
                p[0] = (unsigned char)ri; p[1] = (unsigned char)gi; p[2] = (unsigned char)bi;
            }

            /* HSVA Sepia + ghost blending + history update */
            for (size_t i = 0; i < pixels_count; ++i) {
                unsigned char *p = st.frame_rgb + i * 3;
                unsigned char pr = p[0], pg = p[1], pb = p[2];
                float h, s, v;
                rgb_to_hsv(pr, pg, pb, &h, &s, &v);
                h = SEPIA_HUE_DEG;
                s *= SEPIA_SAT_MULT;
                v *= SEPIA_VAL_MULT * HSVA_EXPOSURE;
                if (s < 0.0f) s = 0.0f;
                if (s > 1.0f) s = 1.0f;
                if (v < 0.0f) v = 0.0f;
                if (v > 1.0f) v = 1.0f;
                unsigned char nr, ng, nb;
                hsv_to_rgb(h, s, v, &nr, &ng, &nb);
                unsigned char *his = st.history_rgb + i * 3;
                float out_r = GHOST_ALPHA * (float)his[0] + (1.0f - GHOST_ALPHA) * (float)nr;
                float out_g = GHOST_ALPHA * (float)his[1] + (1.0f - GHOST_ALPHA) * (float)ng;
                float out_b = GHOST_ALPHA * (float)his[2] + (1.0f - GHOST_ALPHA) * (float)nb;
                int orr = (int)(out_r + 0.5f);
                int org = (int)(out_g + 0.5f);
                int orb = (int)(out_b + 0.5f);
                if (orr < 0) orr = 0; if (orr > 255) orr = 255;
                if (org < 0) org = 0; if (org > 255) org = 255;
                if (orb < 0) orb = 0; if (orb > 255) orb = 255;
                p[0] = (unsigned char)orr; p[1] = (unsigned char)org; p[2] = (unsigned char)orb;
                his[0] = (unsigned char)(HISTORY_DECAY * (float)his[0] + (1.0f - HISTORY_DECAY) * (float)nr + 0.5f);
                his[1] = (unsigned char)(HISTORY_DECAY * (float)his[1] + (1.0f - HISTORY_DECAY) * (float)ng + 0.5f);
                his[2] = (unsigned char)(HISTORY_DECAY * (float)his[2] + (1.0f - HISTORY_DECAY) * (float)nb + 0.5f);
            }

            /* compute slewed FPS and sleep if needed */
            double tnow = now_seconds();
            double elapsed = tnow - start_time;
            if (slew_t <= 0.0) cur_fps = target_fps;
            else {
                double frac = elapsed / slew_t;
                if (frac < 0.0) frac = 0.0;
                if (frac > 1.0) frac = 1.0;
                cur_fps = initial_fps + (target_fps - initial_fps) * frac;
            }
            if (cur_fps < 0.001) cur_fps = 0.001;
            present_interval = 1.0 / cur_fps;
            double next_present_time = last_present_time + present_interval;
            double nowt2 = now_seconds();
            if (next_present_time > nowt2) { sleep_seconds_fraction(next_present_time - nowt2); last_present_time = next_present_time; }
            else last_present_time = nowt2;

            /* upload to texture: need correct pitch */
            int pitch = st.width * 3;
            if (st.tex_bpp == 4) pitch = st.width * 4;
            if (st.tex_bpp == 3) {
                if (SDL_UpdateTexture(st.tex, NULL, st.frame_rgb, pitch) != 0) {
                    fprintf(stderr, "SDL_UpdateTexture failed (3bpp): %s\n", SDL_GetError());
                }
            } else {
                size_t out_sz = pixels_count * 4;
                unsigned char *tmp = malloc(out_sz);
                if (tmp) {
                    unsigned char *dst = tmp;
                    unsigned char *src = st.frame_rgb;
                    for (size_t i = 0; i < pixels_count; ++i) {
                        dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 0xFF;
                        dst += 4; src += 3;
                    }
                    if (SDL_UpdateTexture(st.tex, NULL, tmp, st.width * 4) != 0) {
                        fprintf(stderr, "SDL_UpdateTexture failed (4bpp): %s\n", SDL_GetError());
                    }
                    free(tmp);
                } else {
                    fprintf(stderr, "alloc tmp RGBA failed\n");
                }
            }

            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) { quit = 1; break; }
                if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_q) { quit = 1; break; }
            }
            SDL_RenderClear(st.ren);
            SDL_RenderCopy(st.ren, st.tex, NULL, NULL);
            SDL_RenderPresent(st.ren);

            st.cnt_presented++;
        } else {
            st.cnt_dropped++;
        }

        /* requeue buffer */
        struct v4l2_buffer re;
        memset(&re, 0, sizeof(re));
        re.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        re.memory = V4L2_MEMORY_MMAP;
        re.index = cur.index;
        if (xioctl(st.fd, VIDIOC_QBUF, &re) < 0) {
            fprintf(stderr, "VIDIOC_QBUF requeue failed: %s\n", strerror(errno));
            break;
        }
    }

out:
    fprintf(stderr, "Captured: %" PRIu64 ", Dropped: %" PRIu64 ", Presented: %" PRIu64 ", MJPEG mismatches: %" PRIu64 ", Skipped MJPEG decode: %" PRIu64 "\n",
            st.cnt_captured, st.cnt_dropped, st.cnt_presented, st.cnt_mjpeg_mismatch, st.cnt_skipped_mjpeg);

    cleanup_state(&st, &req);
    close(selfpipe_fds[0]); close(selfpipe_fds[1]);
    return 0;
}

