// camshift_simple.c
// Compile: gcc camshift_simple.c -o camshift_simple -lm -lSDL2
// Run: ./camshift_simple [device] [width] [height]
// Defaults: /dev/video0 640 480

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <sys/select.h>

struct buffer { void *start; size_t length; };

static int xioctl(int fd, int request, void *arg) {
    int r;
    do { r = ioctl(fd, request, arg); } while (r == -1 && errno == EINTR);
    return r;
}

int open_device(const char *devname) {
    int fd = open(devname, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) { perror("open device"); exit(1); }
    return fd;
}

void init_v4l2(int fd, int width, int height, struct buffer **out_bufs, int *out_n) {
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); exit(1); }

    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); exit(1); }

    struct buffer *bufs = calloc(req.count, sizeof(*bufs));
    if (!bufs) { perror("calloc"); exit(1); }

    for (int i = 0; i < (int)req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); exit(1); }
        bufs[i].length = buf.length;
        bufs[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (bufs[i].start == MAP_FAILED) { perror("mmap"); exit(1); }
    }

    for (int i = 0; i < (int)req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); exit(1); }
    }

    enum v4l2_buf_type t = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &t) < 0) { perror("VIDIOC_STREAMON"); exit(1); }

    *out_bufs = bufs;
    *out_n = req.count;
}

int read_frame(int fd, struct buffer *bufs, int nbufs, void **out_ptr, size_t *out_len) {
    (void)nbufs;
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        if (errno == EAGAIN) return 0;
        perror("VIDIOC_DQBUF"); exit(1);
    }
    *out_ptr = bufs[buf.index].start;
    *out_len = buf.bytesused;
    if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); exit(1); }
    return 1;
}

/* Color conversions and helpers */

static inline void clamp255(int *v){ if(*v<0)*v=0; else if(*v>255)*v=255; }

/* YUYV -> RGB24 */
void yuyv_to_rgb(const uint8_t *yuyv, uint8_t *rgb, int w, int h) {
    int idx = 0;
    int total = w*h*2;
    for (int i = 0; i < total; i += 4) {
        int y0 = (int)yuyv[i+0], u = (int)yuyv[i+1], y1 = (int)yuyv[i+2], v = (int)yuyv[i+3];
        int c0 = y0 - 16, c1 = y1 - 16, d = u - 128, e = v - 128;
        int r0 = (298*c0 + 409*e + 128) >> 8;
        int g0 = (298*c0 - 100*d - 208*e + 128) >> 8;
        int b0 = (298*c0 + 516*d + 128) >> 8;
        int r1 = (298*c1 + 409*e + 128) >> 8;
        int g1 = (298*c1 - 100*d - 208*e + 128) >> 8;
        int b1 = (298*c1 + 516*d + 128) >> 8;
        clamp255(&r0); clamp255(&g0); clamp255(&b0);
        clamp255(&r1); clamp255(&g1); clamp255(&b1);
        rgb[idx++] = (uint8_t)r0; rgb[idx++] = (uint8_t)g0; rgb[idx++] = (uint8_t)b0;
        rgb[idx++] = (uint8_t)r1; rgb[idx++] = (uint8_t)g1; rgb[idx++] = (uint8_t)b1;
    }
}

/* RGB -> HSV (H in [0,179] like OpenCV, S,V in [0,255]) */
void rgb_to_hsv_pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t *h, uint8_t *s, uint8_t *v) {
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float mx = fmaxf(rf, fmaxf(gf, bf));
    float mn = fminf(rf, fminf(gf, bf));
    float diff = mx - mn;
    float hh = 0.0f;
    if (diff < 1e-6f) hh = 0.0f;
    else if (mx == rf) hh = fmodf((gf - bf) / diff, 6.0f);
    else if (mx == gf) hh = (bf - rf) / diff + 2.0f;
    else hh = (rf - gf) / diff + 4.0f;
    hh *= 60.0f;
    if (hh < 0) hh += 360.0f;
    *h = (uint8_t)(hh / 2.0f); /* 0..179 */
    *v = (uint8_t)(mx * 255.0f);
    *s = (uint8_t)((mx < 1e-6f) ? 0 : (diff / mx) * 255.0f);
}

/* -------------------- Histogram & backprojection -------------------- */

#define H_BINS 180

void build_histogram(const uint8_t *rgb, int w, int h, int rx, int ry, int rw, int rh, float *hist) {
    /* hist must be H_BINS floats, zeroed by caller */
    int area = 0;
    for (int y = ry; y < ry + rh; ++y) {
        if (y < 0 || y >= h) continue;
        for (int x = rx; x < rx + rw; ++x) {
            if (x < 0 || x >= w) continue;
            int idx = (y*w + x)*3;
            uint8_t r = rgb[idx+0], g = rgb[idx+1], b = rgb[idx+2];
            uint8_t hh, ss, vv;
            rgb_to_hsv_pixel(r,g,b,&hh,&ss,&vv);
            /* Optionally mask low saturation/value to avoid background */
            if (ss < 30 || vv < 30) continue;
            hist[hh] += 1.0f;
            area++;
        }
    }
    if (area <= 0) return;
    /* normalize */
    float maxv = 0.0f;
    for (int i=0;i<H_BINS;++i) if (hist[i] > maxv) maxv = hist[i];
    if (maxv <= 0.0f) return;
    for (int i=0;i<H_BINS;++i) hist[i] /= maxv;
}

/* compute backprojection into a float buffer (0..1) */
void compute_backproj(const uint8_t *rgb, int w, int h, const float *hist, float *bp) {
    int n = w*h;
    for (int i=0;i<n;++i) bp[i] = 0.0f;
    for (int y=0;y<h;++y) {
        for (int x=0;x<w;++x) {
            int idx = (y*w + x)*3;
            uint8_t r = rgb[idx+0], g = rgb[idx+1], b = rgb[idx+2];
            uint8_t hh, ss, vv;
            rgb_to_hsv_pixel(r,g,b,&hh,&ss,&vv);
            float v = hist[hh];
            bp[y*w + x] = v;
        }
    }
}

/* -------------------- MeanShift / CamShift core -------------------- */

/* Run mean-shift inside window (rx,ry,rw,rh). Returns new center (cx,cy) and sum of weights. */
void mean_shift_once(const float *bp, int w, int h, int rx, int ry, int rw, int rh, float *out_cx, float *out_cy, float *sumw) {
    double sx = 0.0, sy = 0.0, sw = 0.0;
    int x0 = rx, y0 = ry, x1 = rx + rw, y1 = ry + rh;
    if (x0 < 0) x0 = 0; if (y0 < 0) y0 = 0;
    if (x1 > w) x1 = w; if (y1 > h) y1 = h;
    for (int y = y0; y < y1; ++y) {
        int base = y*w;
        for (int x = x0; x < x1; ++x) {
            float v = bp[base + x];
            if (v <= 0.0f) continue;
            sx += v * x;
            sy += v * y;
            sw += v;
        }
    }
    if (sw <= 1e-6) {
        *out_cx = rx + rw*0.5f;
        *out_cy = ry + rh*0.5f;
        *sumw = 0.0f;
        return;
    }
    *out_cx = (float)(sx / sw);
    *out_cy = (float)(sy / sw);
    *sumw = (float)sw;
}

/* Adapt window size from weight: scale = sqrt(sumw / (rw*rh)) * factor */
void adapt_window_from_weight(int *rw, int *rh, float sumw, float factor) {
    if (sumw <= 0.0f) return;
    float area = (float)(*rw * *rh);
    float scale = sqrtf(sumw / (area + 1e-6f)) * factor;
    if (scale < 0.5f) scale = 0.5f;
    if (scale > 2.5f) scale = 2.5f;
    int neww = (int)( (*rw) * scale );
    int newh = (int)( (*rh) * scale );
    if (neww < 8) neww = 8;
    if (newh < 8) newh = 8;
    *rw = neww; *rh = newh;
}

/* -------------------- Drawing helpers -------------------- */

void draw_rect(uint8_t *rgb, int w, int h, int rx, int ry, int rw, int rh, uint8_t R, uint8_t G, uint8_t B) {
    int x0 = rx, y0 = ry, x1 = rx + rw - 1, y1 = ry + rh - 1;
    if (x0 < 0) x0 = 0; if (y0 < 0) y0 = 0;
    if (x1 >= w) x1 = w-1; if (y1 >= h) y1 = h-1;
    for (int x = x0; x <= x1; ++x) {
        int idx1 = (y0*w + x)*3;
        int idx2 = (y1*w + x)*3;
        rgb[idx1+0]=R; rgb[idx1+1]=G; rgb[idx1+2]=B;
        rgb[idx2+0]=R; rgb[idx2+1]=G; rgb[idx2+2]=B;
    }
    for (int y = y0; y <= y1; ++y) {
        int idx1 = (y*w + x0)*3;
        int idx2 = (y*w + x1)*3;
        rgb[idx1+0]=R; rgb[idx1+1]=G; rgb[idx1+2]=B;
        rgb[idx2+0]=R; rgb[idx2+1]=G; rgb[idx2+2]=B;
    }
}

/* -------------------- Main -------------------- */

int main(int argc, char **argv) {
    const char *default_dev = "/dev/video0";
    int default_w = 640, default_h = 480;

    const char *dev = default_dev;
    int width = default_w, height = default_h;

    if (argc >= 2) dev = argv[1];
    if (argc >= 3) width = atoi(argv[2]);
    if (argc >= 4) height = atoi(argv[3]);

    int fd = open_device(dev);
    struct buffer *vbufs = NULL;
    int nbufs = 0;
    init_v4l2(fd, width, height, &vbufs, &nbufs);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("CamShift Simple",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING, width, height);

    uint8_t *rgb = malloc(width*height*3);
    float *bp = calloc(width*height, sizeof(float));
    float *hist = calloc(H_BINS, sizeof(float));
    if (!rgb || !bp || !hist) { fprintf(stderr, "alloc fail\n"); return 1; }

    /* ROI selection state */
    int selecting = 0;
    int sel_x0 = 0, sel_y0 = 0, sel_x1 = 0, sel_y1 = 0;
    int roi_rx = 0, roi_ry = 0, roi_rw = 0, roi_rh = 0;
    int tracking = 0;

    SDL_Event e;
    void *frame_ptr = NULL;
    size_t frame_len = 0;

    printf("CamShift-style tracker. Draw ROI with mouse, press i to init, r reset, q quit\n");

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        struct timeval tv = {2,0};
        if (select(fd+1, &fds, NULL, NULL, &tv) <= 0) continue;
        if (!read_frame(fd, vbufs, nbufs, &frame_ptr, &frame_len)) continue;

        yuyv_to_rgb((uint8_t*)frame_ptr, rgb, width, height);

        /* if selecting, draw selection rectangle on copy */
        if (selecting) {
            int rx = sel_x0 < sel_x1 ? sel_x0 : sel_x1;
            int ry = sel_y0 < sel_y1 ? sel_y0 : sel_y1;
            int rw = abs(sel_x1 - sel_x0);
            int rh = abs(sel_y1 - sel_y0);
            draw_rect(rgb, width, height, rx, ry, rw>0?rw:1, rh>0?rh:1, 255, 0, 0);
        }

        /* if tracking, draw current ROI */
        if (tracking) {
            draw_rect(rgb, width, height, roi_rx, roi_ry, roi_rw, roi_rh, 0, 255, 0);
        }

        SDL_UpdateTexture(tex, NULL, rgb, width*3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        /* handle events non-blocking */
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) goto done;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_q) goto done;
                if (e.key.keysym.sym == SDLK_r) {
                    tracking = 0;
                    memset(hist, 0, sizeof(float)*H_BINS);
                }
                if (e.key.keysym.sym == SDLK_i) {
                    /* initialize histogram from selected ROI */
                    if (sel_x0 == sel_x1 || sel_y0 == sel_y1) {
                        /* nothing selected */
                    } else {
                        int rx = sel_x0 < sel_x1 ? sel_x0 : sel_x1;
                        int ry = sel_y0 < sel_y1 ? sel_y0 : sel_y1;
                        int rw = abs(sel_x1 - sel_x0);
                        int rh = abs(sel_y1 - sel_y0);
                        if (rw < 4) rw = 4; if (rh < 4) rh = 4;
                        if (rx < 0) rx = 0; if (ry < 0) ry = 0;
                        if (rx+rw > width) rw = width - rx;
                        if (ry+rh > height) rh = height - ry;
                        memset(hist, 0, sizeof(float)*H_BINS);
                        build_histogram(rgb, width, height, rx, ry, rw, rh, hist);
                        /* if histogram is empty, do not start tracking */
                        float maxh = 0.0f;
                        for (int i=0;i<H_BINS;++i) if (hist[i] > maxh) maxh = hist[i];
                        if (maxh > 0.0f) {
                            roi_rx = rx; roi_ry = ry; roi_rw = rw; roi_rh = rh;
                            tracking = 1;
                        }
                    }
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                selecting = 1;
                sel_x0 = e.button.x; sel_y0 = e.button.y;
                sel_x1 = sel_x0; sel_y1 = sel_y0;
            }
            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                selecting = 0;
                sel_x1 = e.button.x; sel_y1 = e.button.y;
            }
            if (e.type == SDL_MOUSEMOTION && selecting) {
                sel_x1 = e.motion.x; sel_y1 = e.motion.y;
            }
        }

        /* tracking step */
        if (tracking) {
            /* compute backprojection */
            compute_backproj(rgb, width, height, hist, bp);

            /* mean-shift iterations */
            float cx = roi_rx + roi_rw*0.5f;
            float cy = roi_ry + roi_rh*0.5f;
            int rw = roi_rw, rh = roi_rh;
            int max_iter = 10;
            float shift_eps = 0.5f;
            float last_sumw = 0.0f;
            for (int it=0; it<max_iter; ++it) {
                int rx = (int)roundf(cx - rw*0.5f);
                int ry = (int)roundf(cy - rh*0.5f);
                float new_cx, new_cy, sumw;
                mean_shift_once(bp, width, height, rx, ry, rw, rh, &new_cx, &new_cy, &sumw);
                float dx = new_cx - cx;
                float dy = new_cy - cy;
                cx = new_cx; cy = new_cy;
                last_sumw = sumw;
                if (sqrtf(dx*dx + dy*dy) < shift_eps) break;
            }
            /* adapt window size modestly */
            adapt_window_from_weight(&rw, &rh, last_sumw, 1.0f);

            /* clamp and update roi */
            int new_rx = (int)roundf(cx - rw*0.5f);
            int new_ry = (int)roundf(cy - rh*0.5f);
            if (new_rx < 0) new_rx = 0;
            if (new_ry < 0) new_ry = 0;
            if (new_rx + rw > width) rw = width - new_rx;
            if (new_ry + rh > height) rh = height - new_ry;
            roi_rx = new_rx; roi_ry = new_ry; roi_rw = rw; roi_rh = rh;
        }
    }

done:
    {
        enum v4l2_buf_type t = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        xioctl(fd, VIDIOC_STREAMOFF, &t);
    }
    close(fd);

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    free(vbufs);
    free(rgb);
    free(bp);
    free(hist);

    return 0;
}
