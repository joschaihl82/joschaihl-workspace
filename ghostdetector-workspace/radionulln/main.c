// radionullon.c
// Live webcam + SDL2 with spectral "ghost" visual effects.
// Linux + V4L2 required. Compile with: gcc radionullon.c -o radionullon $(sdl2-config --cflags --libs)

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
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480
#define BUFFER_COUNT 4

typedef struct {
    void *start;
    size_t length;
} Buffer;

static int xioctl(int fd, int request, void *arg) {
    int r;
    do { r = ioctl(fd, request, arg); } while (r == -1 && errno == EINTR);
    return r;
}

static void yuyv_to_rgb(uint8_t *yuyv, uint8_t *rgb, int w, int h) {
    for (int i = 0; i < w*h; i += 2) {
        int y0 = yuyv[2*i+0];
        int u  = yuyv[2*i+1];
        int y1 = yuyv[2*i+2];
        int v  = yuyv[2*i+3];

        int c0 = y0 - 16;
        int c1 = y1 - 16;
        int d  = u - 128;
        int e  = v - 128;

        int r0 = (298*c0 + 409*e + 128) >> 8;
        int g0 = (298*c0 - 100*d - 208*e + 128) >> 8;
        int b0 = (298*c0 + 516*d + 128) >> 8;

        int r1 = (298*c1 + 409*e + 128) >> 8;
        int g1 = (298*c1 - 100*d - 208*e + 128) >> 8;
        int b1 = (298*c1 + 516*d + 128) >> 8;

        rgb[3*i+0] = (uint8_t)(r0 < 0 ? 0 : r0 > 255 ? 255 : r0);
        rgb[3*i+1] = (uint8_t)(g0 < 0 ? 0 : g0 > 255 ? 255 : g0);
        rgb[3*i+2] = (uint8_t)(b0 < 0 ? 0 : b0 > 255 ? 255 : b0);

        rgb[3*(i+1)+0] = (uint8_t)(r1 < 0 ? 0 : r1 > 255 ? 255 : r1);
        rgb[3*(i+1)+1] = (uint8_t)(g1 < 0 ? 0 : g1 > 255 ? 255 : g1);
        rgb[3*(i+1)+2] = (uint8_t)(b1 < 0 ? 0 : b1 > 255 ? 255 : b1);
    }
}

// Simple Sobel edge magnitude into a single-channel buffer
static void sobel_edges(uint8_t *rgb, uint8_t *edges, int w, int h) {
    memset(edges, 0, w*h);
    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
            int idx = (y*w + x)*3;
            // luminance approximation
            int Y = (int)(0.2126*rgb[idx+0] + 0.7152*rgb[idx+1] + 0.0722*rgb[idx+2]);

            int xm1 = x-1, xp1 = x+1, ym1 = y-1, yp1 = y+1;
            int idx_l = (y*w + xm1)*3;
            int idx_r = (y*w + xp1)*3;
            int idx_u = (ym1*w + x)*3;
            int idx_d = (yp1*w + x)*3;

            int Yl = (int)(0.2126*rgb[idx_l+0] + 0.7152*rgb[idx_l+1] + 0.0722*rgb[idx_l+2]);
            int Yr = (int)(0.2126*rgb[idx_r+0] + 0.7152*rgb[idx_r+1] + 0.0722*rgb[idx_r+2]);
            int Yu = (int)(0.2126*rgb[idx_u+0] + 0.7152*rgb[idx_u+1] + 0.0722*rgb[idx_u+2]);
            int Yd = (int)(0.2126*rgb[idx_d+0] + 0.7152*rgb[idx_d+1] + 0.0722*rgb[idx_d+2]);

            int gx = Yr - Yl;
            int gy = Yd - Yu;
            int mag = abs(gx) + abs(gy);
            if (mag > 255) mag = 255;
            edges[y*w + x] = (uint8_t)mag;
        }
    }
}

// Apply spectral effects: edge glow + persistence + eerie color bands
static void ghost_filter(uint8_t *rgb, uint8_t *prev, uint8_t *edges, int w, int h,
                         float persist, float edge_intensity) {
    for (int i = 0; i < w*h; i++) {
        int idx = 3*i;
        // persistence blend
        int r = (int)(persist*prev[idx+0] + (1.0f - persist)*rgb[idx+0]);
        int g = (int)(persist*prev[idx+1] + (1.0f - persist)*rgb[idx+1]);
        int b = (int)(persist*prev[idx+2] + (1.0f - persist)*rgb[idx+2]);

        // eerie color band mapping based on luminance
        int Y = (int)(0.2126*r + 0.7152*g + 0.0722*b);
        if (Y > 180) { // spectral flare
            r = (r + 30) > 255 ? 255 : (r + 30);
            g = (g + 5)  > 255 ? 255 : (g + 5);
            b = (b + 60) > 255 ? 255 : (b + 60);
        } else if (Y < 60) { // deep ecto-shadow
            r = (r / 2);
            g = (g / 4);
            b = (b / 2) + 20;
        }

        // edge glow overlay
        int glow = (int)(edge_intensity * edges[i]);
        r = r + glow; g = g + glow/2; b = b + glow;
        if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;

        prev[idx+0] = (uint8_t)r;
        prev[idx+1] = (uint8_t)g;
        prev[idx+2] = (uint8_t)b;
    }
}

int main(int argc, char **argv) {
    const char *devname = (argc > 1) ? argv[1] : "/dev/video0";
    int fd = open(devname, O_RDWR | O_NONBLOCK);
    if (fd < 0) { perror("open video"); return 1; }

    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) { perror("VIDIOC_QUERYCAP"); return 1; }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) { fprintf(stderr, "No capture capability\n"); return 1; }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) { fprintf(stderr, "No streaming capability\n"); return 1; }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) == -1) { perror("VIDIOC_S_FMT"); return 1; }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) == -1) { perror("VIDIOC_REQBUFS"); return 1; }
    if (req.count < 2) { fprintf(stderr, "Insufficient buffer memory\n"); return 1; }

    Buffer buffers[BUFFER_COUNT];
    for (unsigned int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) { perror("VIDIOC_QUERYBUF"); return 1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); return 1; }
    }

    for (unsigned int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) == -1) { perror("VIDIOC_QBUF"); return 1; }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) == -1) { perror("VIDIOC_STREAMON"); return 1; }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    SDL_Window *win = SDL_CreateWindow("Radionullon Ghost Filter",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) { fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError()); return 1; }
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24,
                                         SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (!tex) { fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError()); return 1; }

    uint8_t *rgb = (uint8_t*)malloc(WIDTH*HEIGHT*3);
    uint8_t *post = (uint8_t*)malloc(WIDTH*HEIGHT*3);
    uint8_t *edges = (uint8_t*)malloc(WIDTH*HEIGHT);
    memset(post, 0, WIDTH*HEIGHT*3);

    int quit = 0;
    float persist = 0.80f;
    float edge_intensity = 1.2f;
    int show_edges = 1;
    int show_persist = 1;

    while (!quit) {
        // Dequeue frame
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            if (errno == EAGAIN) { SDL_Delay(1); }
            else { perror("VIDIOC_DQBUF"); break; }
        } else {
            // Convert YUYV -> RGB
            yuyv_to_rgb((uint8_t*)buffers[buf.index].start, rgb, WIDTH, HEIGHT);

            // Compute edges
            if (show_edges) sobel_edges(rgb, edges, WIDTH, HEIGHT);
            else memset(edges, 0, WIDTH*HEIGHT);

            // Apply ghost filter
            ghost_filter(rgb, post, edges, WIDTH, HEIGHT,
                         show_persist ? persist : 0.0f,
                         edge_intensity);

            // Upload texture and render
            SDL_UpdateTexture(tex, NULL, post, WIDTH*3);
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, tex, NULL, NULL);
            SDL_RenderPresent(ren);

            // Requeue buffer
            if (xioctl(fd, VIDIOC_QBUF, &buf) == -1) { perror("VIDIOC_QBUF"); break; }
        }

        // Handle events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: quit = 1; break;
                    case SDLK_e: show_edges = !show_edges; break;
                    case SDLK_p: show_persist = !show_persist; break;
                    case SDLK_UP: persist = (persist < 0.98f) ? persist + 0.02f : 0.98f; break;
                    case SDLK_DOWN: persist = (persist > 0.0f) ? persist - 0.02f : 0.0f; break;
                    case SDLK_RIGHT: edge_intensity = edge_intensity + 0.1f; break;
                    case SDLK_LEFT: edge_intensity = (edge_intensity > 0.1f) ? edge_intensity - 0.1f : 0.1f; break;
                }
            }
        }
    }

    // Cleanup
    free(rgb); free(post); free(edges);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < req.count; i++) {
        munmap(buffers[i].start, buffers[i].length);
    }
    close(fd);
    return 0;
}
