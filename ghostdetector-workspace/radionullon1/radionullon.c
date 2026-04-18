// radionullon.c
// Amalgamierte Realtime "Radionullon" Ghost Visualizer
// - V4L2 YUYV capture (mmap, poll)
// - lightweight analysis: background EMA, residue, Sobel edges, flicker
// - SDL2 rendering: window maximized, resizable, video always stretched (verzerrt)
// - renderer created WITHOUT VSYNC for maximum throughput

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <time.h>
#include <math.h>
#include <poll.h>

#include <SDL2/SDL.h>

#define WIDTH  640
#define HEIGHT 480
#define DEVICE "/dev/video0"

// Tunables for "Radionullon" anomaly visualization
#define BG_LEARN_RATE   0.01f
#define RESIDUE_THRESH  15
#define FLICKER_THRESH  20
#define EDGE_GAIN       1.6f
#define HEATMAP_ALPHA   155

// V4L2 mmap buffers
struct buffer { void *start; size_t length; };
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;

// Working buffers
static uint8_t *gray    = NULL;
static float   *bg      = NULL;
static uint8_t *residue = NULL;
static uint8_t *edge    = NULL;
static uint8_t *flicker = NULL;
static uint8_t *rgb     = NULL;

// Utility clamp
static inline uint8_t clamp_u8(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (uint8_t)v;
}

// Convert YUYV to grayscale and RGB
static void yuyv_to_gray_rgb(const uint8_t *in, uint8_t *g, uint8_t *out_rgb) {
    // YUYV: [Y0 U0 Y1 V0] repeating
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x += 2) {
            int idx = (y*WIDTH + x);
            int p   = (y*WIDTH + x) * 2;

            uint8_t Y0 = in[p + 0];
            uint8_t U  = in[p + 1];
            uint8_t Y1 = in[p + 2];
            uint8_t V  = in[p + 3];

            g[idx]     = Y0;
            g[idx + 1] = Y1;

            int C0 = Y0 - 16;
            int C1 = Y1 - 16;
            int D  = U  - 128;
            int E  = V  - 128;

            int R0 = (298*C0 + 409*E + 128) >> 8;
            int G0 = (298*C0 - 100*D - 208*E + 128) >> 8;
            int B0 = (298*C0 + 516*D + 128) >> 8;

            int R1 = (298*C1 + 409*E + 128) >> 8;
            int G1 = (298*C1 - 100*D - 208*E + 128) >> 8;
            int B1 = (298*C1 + 516*D + 128) >> 8;

            int o = (idx * 3);
            out_rgb[o + 0] = clamp_u8(R0);
            out_rgb[o + 1] = clamp_u8(G0);
            out_rgb[o + 2] = clamp_u8(B0);
            out_rgb[o + 3] = clamp_u8(R1);
            out_rgb[o + 4] = clamp_u8(G1);
            out_rgb[o + 5] = clamp_u8(B1);
        }
    }
}

// Sobel edge magnitude
static void sobel_edge(const uint8_t *g, uint8_t *e) {
    memset(e, 0, WIDTH*HEIGHT);
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            int i = y*WIDTH + x;
            int gx =
                -g[i - WIDTH - 1] - 2*g[i - 1] - g[i + WIDTH - 1] +
                 g[i - WIDTH + 1] + 2*g[i + 1] + g[i + WIDTH + 1];
            int gy =
                -g[i - WIDTH - 1] - 2*g[i - WIDTH] - g[i - WIDTH + 1] +
                 g[i + WIDTH - 1] + 2*g[i + WIDTH] + g[i + WIDTH + 1];
            int mag = (int)(EDGE_GAIN * sqrtf((float)(gx*gx + gy*gy)));
            e[i] = clamp_u8(mag);
        }
    }
}

// Update background and residue
static void update_bg_and_residue(const uint8_t *g, float *bgm, uint8_t *res) {
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        bgm[i] = (1.0f - BG_LEARN_RATE) * bgm[i] + BG_LEARN_RATE * (float)g[i];
        int diff = abs((int)g[i] - (int)bgm[i]);
        res[i] = (diff > RESIDUE_THRESH) ? (uint8_t)diff : 0;
    }
}

// Simple flicker measure
static void update_flicker(const uint8_t *g, uint8_t *f) {
    static uint8_t prev = 0;
    uint64_t sum = 0;
    for (int i = 0; i < WIDTH*HEIGHT; i++) sum += g[i];
    uint8_t avg = (uint8_t)(sum / (WIDTH*HEIGHT));
    int delta = abs((int)avg - (int)prev);
    prev = avg;
    uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        int v = f[i] + boost;
        f[i] = (uint8_t)((v > 255) ? 255 : v);
        if (f[i] > 0) f[i] -= 1;
    }
}

// Overlay heatmap onto RGB
static void overlay_heatmap(uint8_t *rgb, const uint8_t *res, const uint8_t *edge, const uint8_t *flicker) {
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        int score = (int)res[i] + (int)(edge[i]/2) + (int)(flicker[i]/3);
        if (score < 30) continue;

        int s = (score > 255) ? 255 : score;
        uint8_t r, g, b;
        if (s < 64) { b = 255; g = s*4; r = 0; }
        else if (s < 128) { b = 255 - (s-64)*4; g = 255; r = 0; }
        else if (s < 192) { b = 0; g = 255; r = (s-128)*4; }
        else { b = 0; g = 255 - (s-192)*4; r = 255; }

        int o = i*3;
        uint8_t R0 = rgb[o+0], G0 = rgb[o+1], B0 = rgb[o+2];
        rgb[o+0] = (uint8_t)((R0*(255-HEATMAP_ALPHA) + r*HEATMAP_ALPHA)/255);
        rgb[o+1] = (uint8_t)((G0*(255-HEATMAP_ALPHA) + g*HEATMAP_ALPHA)/255);
        rgb[o+2] = (uint8_t)((B0*(255-HEATMAP_ALPHA) + b*HEATMAP_ALPHA)/255);
    }
}

// Initialize V4L2 (mmap)
static int init_v4l2(int *fd) {
    *fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (*fd < 0) { perror("open"); return -1; }

    struct v4l2_capability cap;
    if (ioctl(*fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); close(*fd); return -1; }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) { fprintf(stderr, "Device does not support video capture\n"); close(*fd); return -1; }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) { fprintf(stderr, "Device does not support streaming\n"); close(*fd); return -1; }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = WIDTH;
    fmt.fmt.pix.height      = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_NONE;
    if (ioctl(*fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); close(*fd); return -1; }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(*fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); close(*fd); return -1; }
    if (req.count < 2) { fprintf(stderr, "Insufficient buffer memory\n"); close(*fd); return -1; }

    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) { fprintf(stderr, "calloc buffers failed\n"); close(*fd); return -1; }

    for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = n_buffers;
        if (ioctl(*fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return -1; }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, buf.m.offset);
        if (buffers[n_buffers].start == MAP_FAILED) { perror("mmap"); return -1; }
    }

    for (unsigned int i = 0; i < n_buffers; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;
        if (ioctl(*fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return -1; }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(*fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return -1; }

    return 0;
}

static void cleanup_v4l2(int fd) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < n_buffers; i++) {
        if (buffers && buffers[i].start) munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    close(fd);
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    // Create maximized, resizable window (will start maximized)
    SDL_Window *win = SDL_CreateWindow("Radionullon Visualizer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
    if (!win) { fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError()); SDL_Quit(); return 1; }

    // NO VSYNC for maximum throughput
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) { fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError()); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (!tex) { fprintf(stderr, "SDL_CreateTexture error: %s\n", SDL_GetError()); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // Alloc working buffers
    gray    = malloc(WIDTH*HEIGHT);
    bg      = malloc(WIDTH*HEIGHT*sizeof(float));
    residue = malloc(WIDTH*HEIGHT);
    edge    = malloc(WIDTH*HEIGHT);
    flicker = malloc(WIDTH*HEIGHT);
    rgb     = malloc(WIDTH*HEIGHT*3);
    if (!gray || !bg || !residue || !edge || !flicker || !rgb) {
        fprintf(stderr, "Out of memory\n");
        SDL_DestroyTexture(tex); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
        return 1;
    }
    memset(bg, 0, WIDTH*HEIGHT*sizeof(float));
    memset(flicker, 0, WIDTH*HEIGHT);

    // Init V4L2
    int fd;
    if (init_v4l2(&fd) != 0) {
        fprintf(stderr, "Failed to initialize V4L2\n");
        free(gray); free(bg); free(residue); free(edge); free(flicker); free(rgb);
        SDL_DestroyTexture(tex); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
        return 1;
    }

    // Destination rect fills the window (we will stretch/distort)
    SDL_Rect dst_rect = {0,0,WIDTH,HEIGHT};
    int win_w = WIDTH, win_h = HEIGHT;
    SDL_GetWindowSize(win, &win_w, &win_h);
    dst_rect.w = win_w; dst_rect.h = win_h; dst_rect.x = 0; dst_rect.y = 0;

    bool quit = false;
    bool reset_bg = true;

    // poll fd for camera frames
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN | POLLPRI;

    while (!quit) {
        // wait for a frame (block up to 200ms; adjust to taste; -1 = indefinite)
        int pret = poll(&pfd, 1, 200);
        if (pret > 0) {
            if (pfd.revents & (POLLIN | POLLPRI)) {
                struct v4l2_buffer buf;
                memset(&buf, 0, sizeof(buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;

                if (ioctl(fd, VIDIOC_DQBUF, &buf) == 0) {
                    uint8_t *frame = (uint8_t*)buffers[buf.index].start;

                    // Convert + analysis
                    yuyv_to_gray_rgb(frame, gray, rgb);
                    if (reset_bg) {
                        for (int i = 0; i < WIDTH*HEIGHT; i++) bg[i] = (float)gray[i];
                        reset_bg = false;
                    }
                    update_bg_and_residue(gray, bg, residue);
                    sobel_edge(gray, edge);
                    update_flicker(gray, flicker);
                    overlay_heatmap(rgb, residue, edge, flicker);

                    // Update texture and render stretched to window
                    SDL_UpdateTexture(tex, NULL, rgb, WIDTH * 3);
                    SDL_RenderClear(ren);
                    SDL_RenderCopy(ren, tex, NULL, &dst_rect);
                    SDL_RenderPresent(ren);

                    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) perror("VIDIOC_QBUF");
                }
            }
        } else if (pret == 0) {
            // timeout: continue to process UI events (keeps UI responsive)
        } else {
            perror("poll");
            break;
        }

        // Process SDL events (resize, input)
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                if (e.key.keysym.sym == SDLK_b) reset_bg = true;
            } else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED ||
                    e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    dst_rect.w = e.window.data1;
                    dst_rect.h = e.window.data2;
                    dst_rect.x = 0;
                    dst_rect.y = 0;
                }
            }
        }
    }

    cleanup_v4l2(fd);
    free(gray); free(bg); free(residue); free(edge); free(flicker); free(rgb);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

