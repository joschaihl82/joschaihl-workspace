// g.c
// CIA GhostViewer v25.0 — HSVA 4.0 Ghost Sepia effect added (fixed warnings/errors)
// Build: gcc g.c -o g -Wall -Wextra -O2 -lSDL2 -ljpeg
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <time.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <jpeglib.h>
#include <math.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

struct buffer { void *start; size_t length; };

static int xioctl(int fd, int request, void *arg)
{
    int r;
    do r = ioctl(fd, request, arg);
    while (r == -1 && errno == EINTR);
    return r;
}

/* ---------- Effect parameters (HSVA 4.0 Ghost Sepia) ---------- */
/* Sepia HSVA parameters; tweak as needed */
static const float SEPIA_HUE_DEG = 30.0f;      /* target hue in degrees (sepia around 30°) */
static const float SEPIA_SAT_MULT = 0.45f;    /* multiply saturation to desaturate toward sepia */
static const float SEPIA_VAL_MULT = 1.05f;    /* slight brighten */
static const float GHOST_ALPHA = 0.40f;       /* how much previous frame contributes (0..1) */
static const float HSVA_EXPOSURE = 1.0f;      /* overall exposure multiplier (keeps 1.0 by default) */
/* ------------------------------------------------------------- */

static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static int mul_will_overflow_size_t(size_t a, size_t b)
{
    if (a == 0 || b == 0) return 0;
    return a > SIZE_MAX / b;
}

/* RGB <-> HSV helpers (range: R,G,B 0..255; H 0..360; S,V 0..1) */
static void rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b, float *h, float *s, float *v)
{
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float max = rf, min = rf;
    if (gf > max) max = gf;
    if (bf > max) max = bf;
    if (gf < min) min = gf;
    if (bf < min) min = bf;
    float delta = max - min;
    *v = max;
    if (max <= 0.0f) {
        *s = 0.0f;
        *h = 0.0f;
        return;
    }
    *s = delta / max;
    if (delta <= 1e-6f) {
        *h = 0.0f;
        return;
    }
    if (max == rf) {
        *h = 60.0f * (fmodf(((gf - bf) / delta), 6.0f));
    } else if (max == gf) {
        *h = 60.0f * (((bf - rf) / delta) + 2.0f);
    } else {
        *h = 60.0f * (((rf - gf) / delta) + 4.0f);
    }
    if (*h < 0.0f) *h += 360.0f;
}

/* h expected 0..360, s,v 0..1 -> r,g,b 0..255 */
static void hsv_to_rgb(float h, float s, float v, unsigned char *r, unsigned char *g, unsigned char *b)
{
    if (s <= 0.0f) {
        int vi = (int)(v * 255.0f + 0.5f);
        if (vi < 0) vi = 0;
        if (vi > 255) vi = 255;
        *r = *g = *b = (unsigned char)vi;
        return;
    }
    float hh = h;
    if (hh >= 360.0f) hh = fmodf(hh, 360.0f);
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
        case 5: default: rf = v; gf = p; bf = q; break;
    }
    int ri = (int)(rf * 255.0f + 0.5f);
    int gi = (int)(gf * 255.0f + 0.5f);
    int bi = (int)(bf * 255.0f + 0.5f);
    if (ri < 0) ri = 0;
    if (ri > 255) ri = 255;
    if (gi < 0) gi = 0;
    if (gi > 255) gi = 255;
    if (bi < 0) bi = 0;
    if (bi > 255) bi = 255;
    *r = (unsigned char)ri;
    *g = (unsigned char)gi;
    *b = (unsigned char)bi;
}

/* Apply HSVA 4.0 Ghost Sepia to a single RGB buffer (in-place), blending with history */
static void apply_hsva_ghost_sepia(unsigned char *rgb, unsigned char *history, int w, int h)
{
    size_t pixels = (size_t)w * (size_t)h;
    /* Precompute target hue normalized */
    float target_h = SEPIA_HUE_DEG;
    float sat_mul = SEPIA_SAT_MULT;
    float val_mul = SEPIA_VAL_MULT;
    float ghost_alpha = GHOST_ALPHA;
    for (size_t i = 0; i < pixels; ++i) {
        unsigned char *p = rgb + i * 3;
        unsigned char pr = p[0], pg = p[1], pb = p[2];
        float h, s, v;
        rgb_to_hsv(pr, pg, pb, &h, &s, &v);
        /* Move hue toward sepia target: for "HSVA 4.0" we set hue to target directly */
        h = target_h;
        /* Desaturate and adjust value */
        s *= sat_mul;
        v *= val_mul * HSVA_EXPOSURE;
        if (s < 0.0f) s = 0.0f;
        if (s > 1.0f) s = 1.0f;
        if (v < 0.0f) v = 0.0f;
        if (v > 1.0f) v = 1.0f;
        unsigned char nr, ng, nb;
        hsv_to_rgb(h, s, v, &nr, &ng, &nb);
        /* Ghost blend with history: out = alpha * history + (1-alpha) * current (sepia) */
        if (history) {
            unsigned char hr = history[i * 3 + 0];
            unsigned char hg = history[i * 3 + 1];
            unsigned char hb = history[i * 3 + 2];
            float out_r = ghost_alpha * (float)hr + (1.0f - ghost_alpha) * (float)nr;
            float out_g = ghost_alpha * (float)hg + (1.0f - ghost_alpha) * (float)ng;
            float out_b = ghost_alpha * (float)hb + (1.0f - ghost_alpha) * (float)nb;
            int orr = (int)(out_r + 0.5f);
            int org = (int)(out_g + 0.5f);
            int orb = (int)(out_b + 0.5f);
            if (orr < 0) orr = 0;
            if (orr > 255) orr = 255;
            if (org < 0) org = 0;
            if (org > 255) org = 255;
            if (orb < 0) orb = 0;
            if (orb > 255) orb = 255;
            p[0] = (unsigned char)orr;
            p[1] = (unsigned char)org;
            p[2] = (unsigned char)orb;
        } else {
            p[0] = nr;
            p[1] = ng;
            p[2] = nb;
        }
    }
}

/* YUYV -> RGB24 conversion */
static void yuyv_to_rgb24(const unsigned char *yuyv, unsigned char *rgb, int w, int h)
{
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
            c = y1 - 16;
            d = u - 128;
            e = v - 128;
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

/* MJPEG -> RGB24 via libjpeg with safety checks */
static int mjpeg_to_rgb24(const unsigned char *mjpeg, size_t mjpeg_len, unsigned char *rgb, int expected_w, int expected_h)
{
    (void)expected_w;
    (void)expected_h;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW rowptr[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char*)mjpeg, mjpeg_len);
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&cinfo);
        return -1;
    }
    jpeg_start_decompress(&cinfo);

    int w = cinfo.output_width;
    int h = cinfo.output_height;
    int comps = cinfo.output_components;
    if (comps != 3) {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return -1;
    }

    const int MAX_DIM = 8192;
    if (w <= 0 || h <= 0 || w > MAX_DIM || h > MAX_DIM) {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return -1;
    }

    if (mul_will_overflow_size_t((size_t)w, (size_t)3)) {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return -1;
    }

    unsigned char *row = malloc((size_t)w * 3);
    if (!row) {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return -1;
    }

    while (cinfo.output_scanline < cinfo.output_height) {
        rowptr[0] = row;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
        size_t dst_off = (size_t)(cinfo.output_scanline - 1) * (size_t)w * 3;
        memcpy(rgb + dst_off, row, (size_t)w * 3);
    }

    free(row);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return 0;
}

/* Helper: compute nanosleep timespec from fractional seconds */
static void sleep_seconds_fraction(double seconds)
{
    if (seconds <= 0.0) return;
    time_t sec = (time_t)seconds;
    long nsec = (long)((seconds - (double)sec) * 1e9);
    if (nsec < 0) nsec = 0;
    if (nsec >= 1000000000L) { sec += 1; nsec -= 1000000000L; }
    struct timespec ts = { sec, nsec };
    nanosleep(&ts, NULL);
}

/* Unified cleanup state */
struct app_state {
    int fd;
    struct buffer *buffers;
    unsigned int nbuf;
    int started_stream;
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Texture *tex;
    unsigned char *frame_rgb;
    unsigned char *history_rgb;
};

static void cleanup_state(struct app_state *s, struct v4l2_requestbuffers *req)
{
    if (!s) return;
    if (s->tex) SDL_DestroyTexture(s->tex);
    if (s->ren) SDL_DestroyRenderer(s->ren);
    if (s->win) SDL_DestroyWindow(s->win);
    SDL_Quit();
    if (s->frame_rgb) { free(s->frame_rgb); s->frame_rgb = NULL; }
    if (s->history_rgb) { free(s->history_rgb); s->history_rgb = NULL; }
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

/* Main program */
int main(int argc, char **argv)
{
    const char *dev_name = "/dev/video0";
    double slew_seconds = 2.0;
    if (argc > 1) dev_name = argv[1];
    if (argc > 2) slew_seconds = atof(argv[2]);

    struct app_state state;
    memset(&state, 0, sizeof(state));
    state.fd = -1;

    int rc = 1;
    int fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", dev_name, strerror(errno));
        return 1;
    }
    state.fd = fd;

    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        fprintf(stderr, "%s is not a V4L2 device or cannot query capabilities\n", dev_name);
        cleanup_state(&state, NULL);
        return 1;
    }

    /* choose format: prefer YUYV, fallback to MJPEG */
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
            fprintf(stderr, "Failed to set format YUYV/MJPEG: %s\n", strerror(errno));
            cleanup_state(&state, NULL);
            return 1;
        }
    }

    int width = fmt.fmt.pix.width;
    int height = fmt.fmt.pix.height;
    unsigned int pixfmt = fmt.fmt.pix.pixelformat;

    /* get camera framerate if possible */
    double cam_fps = 30.0;
    struct v4l2_streamparm streamparm;
    memset(&streamparm, 0, sizeof(streamparm));
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_G_PARM, &streamparm) == 0) {
        if (streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
            unsigned int den = streamparm.parm.capture.timeperframe.denominator;
            unsigned int num = streamparm.parm.capture.timeperframe.numerator;
            if (den && num) cam_fps = (double)den / (double)num;
        }
    }

    /* determine initial FPS */
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
        cleanup_state(&state, NULL);
        return 1;
    }

    state.nbuf = req.count;
    state.buffers = calloc(req.count, sizeof(struct buffer));
    if (!state.buffers) {
        fprintf(stderr, "Out of memory allocating buffers\n");
        cleanup_state(&state, &req);
        return 1;
    }

    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            fprintf(stderr, "VIDIOC_QUERYBUF failed: %s\n", strerror(errno));
            cleanup_state(&state, &req);
            return 1;
        }
        state.buffers[i].length = buf.length;
        state.buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (state.buffers[i].start == MAP_FAILED) {
            fprintf(stderr, "mmap failed: %s\n", strerror(errno));
            cleanup_state(&state, &req);
            return 1;
        }
    }

    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            fprintf(stderr, "VIDIOC_QBUF failed: %s\n", strerror(errno));
            cleanup_state(&state, &req);
            return 1;
        }
    }

    enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &buf_type) < 0) {
        fprintf(stderr, "VIDIOC_STREAMON failed: %s\n", strerror(errno));
        cleanup_state(&state, &req);
        return 1;
    }
    state.started_stream = 1;

    /* prepare SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        cleanup_state(&state, &req);
        return 1;
    }

    state.win = SDL_CreateWindow("CIA GhostViewer v25.0 - HSVA 4.0 Ghost Sepia", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!state.win) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        cleanup_state(&state, &req);
        return 1;
    }

    state.ren = SDL_CreateRenderer(state.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!state.ren) {
        state.ren = SDL_CreateRenderer(state.win, -1, 0);
        if (!state.ren) {
            fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
            cleanup_state(&state, &req);
            return 1;
        }
    }

    Uint32 tex_format = SDL_PIXELFORMAT_RGB24;
    state.tex = SDL_CreateTexture(state.ren, tex_format, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!state.tex) {
        tex_format = SDL_PIXELFORMAT_RGB888;
        state.tex = SDL_CreateTexture(state.ren, tex_format, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (!state.tex) {
            fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
            cleanup_state(&state, &req);
            return 1;
        }
    }

    /* allocate frame buffer safely */
    size_t pixels = (size_t)width * (size_t)height;
    if (mul_will_overflow_size_t(pixels, 3)) {
        fprintf(stderr, "Requested frame size too large\n");
        cleanup_state(&state, &req);
        return 1;
    }
    state.frame_rgb = malloc(pixels * 3);
    if (!state.frame_rgb) {
        fprintf(stderr, "Out of memory for frame_rgb\n");
        cleanup_state(&state, &req);
        return 1;
    }
    /* history buffer for ghosting */
    state.history_rgb = malloc(pixels * 3);
    if (!state.history_rgb) {
        fprintf(stderr, "Out of memory for history_rgb\n");
        cleanup_state(&state, &req);
        return 1;
    }
    /* start history black */
    memset(state.history_rgb, 0, pixels * 3);

    /* timing / slew state */
    double start_time = now_seconds();
    double slew_t = slew_seconds > 0.0 ? slew_seconds : 0.0;
    double target_fps = cam_fps;
    double cur_fps = initial_fps;
    double last_present_time = now_seconds();
    double present_interval = 1.0 / (cur_fps > 0.0 ? cur_fps : 1.0);

    /* main loop */
    int quit = 0;
    while (!quit) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        int sel = select(fd + 1, &fds, NULL, NULL, &tv);
        if (sel < 0) {
            if (errno == EINTR) continue;
            fprintf(stderr, "select failed: %s\n", strerror(errno));
            break;
        } else if (sel == 0) {
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) { quit = 1; break; }
                if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_q) { quit = 1; break; }
            }
            continue;
        }

        struct v4l2_buffer current;
        memset(&current, 0, sizeof(current));
        current.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        current.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd, VIDIOC_DQBUF, &current) < 0) {
            if (errno == EAGAIN) continue;
            if (errno == EINTR) continue;
            fprintf(stderr, "VIDIOC_DQBUF failed: %s\n", strerror(errno));
            break;
        }

        /* drain loop */
        while (1) {
            struct v4l2_buffer tmp;
            memset(&tmp, 0, sizeof(tmp));
            tmp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            tmp.memory = V4L2_MEMORY_MMAP;
            if (xioctl(fd, VIDIOC_DQBUF, &tmp) < 0) {
                if (errno == EAGAIN) break;
                if (errno == EINTR) continue;
                fprintf(stderr, "VIDIOC_DQBUF (drain) failed: %s\n", strerror(errno));
                goto out;
            } else {
                struct v4l2_buffer re;
                memset(&re, 0, sizeof(re));
                re.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                re.memory = V4L2_MEMORY_MMAP;
                re.index = current.index;
                if (xioctl(fd, VIDIOC_QBUF, &re) < 0) {
                    fprintf(stderr, "VIDIOC_QBUF (drop) failed: %s\n", strerror(errno));
                }
                current = tmp;
            }
        }

        /* decode */
        if (pixfmt == V4L2_PIX_FMT_YUYV) {
            yuyv_to_rgb24((const unsigned char *)state.buffers[current.index].start, state.frame_rgb, width, height);
        } else if (pixfmt == V4L2_PIX_FMT_MJPEG) {
            if (mjpeg_to_rgb24((const unsigned char *)state.buffers[current.index].start, current.bytesused, state.frame_rgb, width, height) != 0) {
                memset(state.frame_rgb, 0, pixels * 3);
            }
        } else {
            memset(state.frame_rgb, 0, pixels * 3);
        }

        /* Apply HSVA 4.0 Ghost Sepia: transform then blend with history */
        apply_hsva_ghost_sepia(state.frame_rgb, state.history_rgb, width, height);

        /* Update history: new history = blend(old_history, current) using GHOST_ALPHA */
        for (size_t i = 0; i < pixels; ++i) {
            unsigned char *cur = state.frame_rgb + i * 3;
            unsigned char *his = state.history_rgb + i * 3;
            float h0 = GHOST_ALPHA * (float)his[0] + (1.0f - GHOST_ALPHA) * (float)cur[0];
            float h1 = GHOST_ALPHA * (float)his[1] + (1.0f - GHOST_ALPHA) * (float)cur[1];
            float h2 = GHOST_ALPHA * (float)his[2] + (1.0f - GHOST_ALPHA) * (float)cur[2];
            int ih0 = (int)(h0 + 0.5f);
            int ih1 = (int)(h1 + 0.5f);
            int ih2 = (int)(h2 + 0.5f);
            if (ih0 < 0) ih0 = 0; if (ih0 > 255) ih0 = 255;
            if (ih1 < 0) ih1 = 0; if (ih1 > 255) ih1 = 255;
            if (ih2 < 0) ih2 = 0; if (ih2 > 255) ih2 = 255;
            his[0] = (unsigned char)ih0;
            his[1] = (unsigned char)ih1;
            his[2] = (unsigned char)ih2;
        }

        /* compute slewed FPS */
        double tnow = now_seconds();
        double elapsed = tnow - start_time;
        if (slew_t <= 0.0) {
            cur_fps = target_fps;
        } else {
            double frac = elapsed / slew_t;
            if (frac < 0.0) frac = 0.0;
            if (frac > 1.0) frac = 1.0;
            cur_fps = initial_fps + (target_fps - initial_fps) * frac;
        }
        if (cur_fps < 0.001) cur_fps = 0.001;
        present_interval = 1.0 / cur_fps;

        double next_present_time = last_present_time + present_interval;
        double nowt2 = now_seconds();
        if (next_present_time > nowt2) {
            sleep_seconds_fraction(next_present_time - nowt2);
            last_present_time = next_present_time;
        } else {
            last_present_time = nowt2;
        }

        /* present */
        if (SDL_UpdateTexture(state.tex, NULL, state.frame_rgb, width * 3) != 0) {
            fprintf(stderr, "SDL_UpdateTexture failed: %s\n", SDL_GetError());
        }
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { quit = 1; break; }
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_q) { quit = 1; break; }
        }
        SDL_RenderClear(state.ren);
        SDL_RenderCopy(state.ren, state.tex, NULL, NULL);
        SDL_RenderPresent(state.ren);

        /* requeue */
        struct v4l2_buffer requeue;
        memset(&requeue, 0, sizeof(requeue));
        requeue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        requeue.memory = V4L2_MEMORY_MMAP;
        requeue.index = current.index;
        if (xioctl(fd, VIDIOC_QBUF, &requeue) < 0) {
            fprintf(stderr, "VIDIOC_QBUF requeue failed: %s\n", strerror(errno));
            break;
        }
    }

out:
    rc = 0;
    cleanup_state(&state, &req);
    return rc ? rc : 0;
}

