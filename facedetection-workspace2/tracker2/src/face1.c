// face_track_simple.c
// Compile: gcc face_track_simple.c -o face_track_simple -lm -lSDL2
// Run: ./face_track_simple /dev/video0 640 480

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
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); exit(1); }

    struct buffer *bufs = calloc(req.count, sizeof(*bufs));
    for (int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); exit(1); }
        bufs[i].length = buf.length;
        bufs[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (bufs[i].start == MAP_FAILED) { perror("mmap"); exit(1); }
    }
    for (int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); exit(1); }
    }
    enum v4l2_buf_type t = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &t) < 0) { perror("VIDIOC_STREAMON"); exit(1); }
    *out_bufs = bufs; *out_n = req.count;
}

int read_frame(int fd, struct buffer *bufs, int nbufs, void **out_ptr, size_t *out_len) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        if (errno == EAGAIN) return 0;
        perror("VIDIOC_DQBUF"); exit(1);
    }
    *out_ptr = bufs[buf.index].start; *out_len = buf.bytesused;
    if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); exit(1); }
    return 1;
}

/* YUYV -> RGB24 */
static inline void clamp255(int *v){ if(*v<0)*v=0; else if(*v>255)*v=255; }
void yuyv_to_rgb(const uint8_t *yuyv, uint8_t *rgb, int w, int h) {
    int idx = 0;
    int total = w*h*2;
    for (int i = 0; i < total; i += 4) {
        int y0 = yuyv[i+0], u = yuyv[i+1], y1 = yuyv[i+2], v = yuyv[i+3];
        int c0 = y0 - 16, c1 = y1 - 16, d = u - 128, e = v - 128;
        int r0 = (298*c0 + 409*e + 128) >> 8;
        int g0 = (298*c0 - 100*d - 208*e + 128) >> 8;
        int b0 = (298*c0 + 516*d + 128) >> 8;
        int r1 = (298*c1 + 409*e + 128) >> 8;
        int g1 = (298*c1 - 100*d - 208*e + 128) >> 8;
        int b1 = (298*c1 + 516*d + 128) >> 8;
        clamp255(&r0); clamp255(&g0); clamp255(&b0);
        clamp255(&r1); clamp255(&g1); clamp255(&b1);
        rgb[idx++] = r0; rgb[idx++] = g0; rgb[idx++] = b0;
        rgb[idx++] = r1; rgb[idx++] = g1; rgb[idx++] = b1;
    }
}

/* Gray conversion */
void rgb_to_gray(const uint8_t *rgb, uint8_t *gray, int w, int h) {
    int n = w*h;
    for (int i = 0; i < n; ++i) {
        int r = rgb[3*i+0], g = rgb[3*i+1], b = rgb[3*i+2];
        gray[i] = (uint8_t)(0.299f*r + 0.587f*g + 0.114f*b);
    }
}

/* Simple gradients */
void compute_grad(const uint8_t *gray, int w, int h, float *gx, float *gy) {
    for (int y = 1; y < h-1; ++y) for (int x = 1; x < w-1; ++x) {
        int i = y*w + x;
        float dx = -gray[i-w-1] - 2*gray[i-1] - gray[i+w-1] + gray[i-w+1] + 2*gray[i+1] + gray[i+w+1];
        float dy = -gray[i-w-1] - 2*gray[i-w] - gray[i-w+1] + gray[i+w-1] + 2*gray[i+w] + gray[i+w+1];
        gx[i] = dx * 0.125f; gy[i] = dy * 0.125f;
    }
}

/* Bilinear sample */
static inline float sample_gray(const uint8_t *img, int w, int h, float x, float y) {
    if (x < 0 || y < 0 || x >= w-1 || y >= h-1) return 0.0f;
    int x0 = (int)floorf(x), y0 = (int)floorf(y);
    float dx = x - x0, dy = y - y0;
    float v00 = img[y0*w + x0], v10 = img[y0*w + x0+1];
    float v01 = img[(y0+1)*w + x0], v11 = img[(y0+1)*w + x0+1];
    return (1-dx)*(1-dy)*v00 + dx*(1-dy)*v10 + (1-dx)*dy*v01 + dx*dy*v11;
}

/* Single-scale LK for many points */
typedef struct { float x,y; } Pt;
void lk_track(const uint8_t *I1, const uint8_t *I2, const float *gx, const float *gy,
              int w, int h, Pt *in_pts, Pt *out_pts, int P, int win) {
    int half = win/2;
    for (int p = 0; p < P; ++p) {
        float px = in_pts[p].x, py = in_pts[p].y;
        float A00=0,A01=0,A11=0,b0=0,b1=0;
        for (int dy = -half; dy <= half; ++dy) for (int dx = -half; dx <= half; ++dx) {
            float x = px + dx, y = py + dy;
            if (x < 1 || y < 1 || x >= w-2 || y >= h-2) continue;
            int ix = (int)floorf(x), iy = (int)floorf(y);
            int idx = iy*w + ix;
            float gxi = gx[idx], gyi = gy[idx];
            float I1v = sample_gray(I1, w, h, x, y);
            float I2v = sample_gray(I2, w, h, x, y);
            float It = I2v - I1v;
            A00 += gxi*gxi; A01 += gxi*gyi; A11 += gyi*gyi;
            b0 += gxi * It; b1 += gyi * It;
        }
        float det = A00*A11 - A01*A01;
        float ux=0, uy=0;
        if (det > 1e-6f) {
            ux = (-b0*A11 + b1*A01) / det;
            uy = (-A00*b1 + A01*b0) / det;
        }
        out_pts[p].x = px + ux; out_pts[p].y = py + uy;
        if (out_pts[p].x < 0) out_pts[p].x = 0;
        if (out_pts[p].y < 0) out_pts[p].y = 0;
        if (out_pts[p].x > w-1) out_pts[p].x = w-1;
        if (out_pts[p].y > h-1) out_pts[p].y = h-1;
    }
}

/* draw small circle on rgb buffer */
void draw_circle(uint8_t *rgb, int w, int h, int cx, int cy, int r, uint8_t R, uint8_t G, uint8_t B) {
    for (int dy=-r; dy<=r; ++dy) {
        int y = cy + dy; if (y<0||y>=h) continue;
        for (int dx=-r; dx<=r; ++dx) {
            int x = cx + dx; if (x<0||x>=w) continue;
            if (dx*dx + dy*dy <= r*r) {
                int idx = (y*w + x)*3;
                rgb[idx+0]=R; rgb[idx+1]=G; rgb[idx+2]=B;
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "Usage: %s /dev/video0 width height\n", argv[0]); return 1; }
    const char *dev = argv[1]; int width = atoi(argv[2]), height = atoi(argv[3]);

    int fd = open_device(dev);
    struct buffer *vbufs = NULL; int nbufs = 0;
    init_v4l2(fd, width, height, &vbufs, &nbufs);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    SDL_Window *win = SDL_CreateWindow("Simple Face Track", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

    uint8_t *rgb = malloc(width*height*3);
    uint8_t *gray_prev = malloc(width*height);
    uint8_t *gray_cur  = malloc(width*height);
    float *gx = calloc(width*height, sizeof(float));
    float *gy = calloc(width*height, sizeof(float));

    Pt *pts = NULL, *pts_prev = NULL, *pts_next = NULL;
    int P = 0;
    int initialized = 0;
    int placed = 0;

    SDL_Event e;
    void *frame_ptr = NULL; size_t frame_len = 0;

    printf("Left-click to place points. Press i to initialize, r to reset, q to quit.\n");

    while (1) {
        fd_set fds; FD_ZERO(&fds); FD_SET(fd, &fds);
        struct timeval tv = {2,0};
        int sel = select(fd+1, &fds, NULL, NULL, &tv);
        if (sel <= 0) continue;
        if (!read_frame(fd, vbufs, nbufs, &frame_ptr, &frame_len)) continue;
        yuyv_to_rgb((uint8_t*)frame_ptr, rgb, width, height);
        rgb_to_gray(rgb, gray_cur, width, height);

        if (!initialized) {
            // show frame and allow clicks
            SDL_UpdateTexture(tex, NULL, rgb, width*3);
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, tex, NULL, NULL);
            for (int i = 0; i < placed; ++i) draw_circle(rgb, width, height, (int)pts[i].x, (int)pts[i].y, 4, 255,0,0);
            SDL_UpdateTexture(tex, NULL, rgb, width*3);
            SDL_RenderCopy(ren, tex, NULL, NULL);
            SDL_RenderPresent(ren);

            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) goto done;
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    // allocate or expand arrays
                    P++;
                    pts = realloc(pts, sizeof(Pt)*P);
                    pts_prev = realloc(pts_prev, sizeof(Pt)*P);
                    pts_next = realloc(pts_next, sizeof(Pt)*P);
                    pts[P-1].x = e.button.x; pts[P-1].y = e.button.y;
                    placed++;
                    printf("Placed point %d at (%.1f, %.1f)\n", P, pts[P-1].x, pts[P-1].y);
                }
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_i) {
                        if (P > 0) {
                            initialized = 1;
                            memcpy(gray_prev, gray_cur, width*height);
                            for (int i=0;i<P;++i) pts_prev[i]=pts[i];
                            printf("Initialized with %d points. Tracking started.\n", P);
                        } else printf("Place at least one point before initializing.\n");
                    } else if (e.key.keysym.sym == SDLK_q) goto done;
                    else if (e.key.keysym.sym == SDLK_r) {
                        free(pts); free(pts_prev); free(pts_next);
                        pts = pts_prev = pts_next = NULL; P = 0; placed = 0; initialized = 0;
                        printf("Reset points.\n");
                    }
                }
            }
            continue;
        }

        // tracking
        compute_grad(gray_prev, width, height, gx, gy);
        lk_track(gray_prev, gray_cur, gx, gy, width, height, pts_prev, pts_next, P, 9);

        // print displacements
        printf("displacements:");
        for (int i = 0; i < P; ++i) {
            float dx = pts_next[i].x - pts[i].x;
            float dy = pts_next[i].y - pts[i].y;
            printf(" (%.2f,%.2f)", dx, dy);
            // draw tracked point
            draw_circle(rgb, width, height, (int)pts_next[i].x, (int)pts_next[i].y, 3, 0,255,0);
            // update pts_prev for next iter
            pts_prev[i] = pts_next[i];
        }
        printf("\n");

        SDL_UpdateTexture(tex, NULL, rgb, width*3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        // swap gray buffers
        memcpy(gray_prev, gray_cur, width*height);

        // handle events non-blocking
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) goto done;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_q) goto done;
                if (e.key.keysym.sym == SDLK_r) {
                    free(pts); free(pts_prev); free(pts_next);
                    pts = pts_prev = pts_next = NULL; P = 0; placed = 0; initialized = 0;
                    printf("Reset points.\n");
                }
            }
        }
    }

done:
    // cleanup
    enum v4l2_buf_type t = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMOFF, &t);
    // unmap buffers
    // (omitted for brevity)
    close(fd);

    SDL_DestroyTexture(tex); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
    free(vbufs); free(rgb); free(gray_prev); free(gray_cur); free(gx); free(gy);
    free(pts); free(pts_prev); free(pts_next);
    return 0;
}
