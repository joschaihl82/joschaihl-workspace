#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sched.h> 
#include <time.h> 
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h> 
#include <errno.h>

// --- I. Konfiguration und Konstanten ---
#define IMG_W 640
#define IMG_H 480
#define GRAPH_POINTS 120
#define GRAPH_HEIGHT_INIT 150 
#define INITIAL_WINDOW_HEIGHT (IMG_H + GRAPH_HEIGHT_INIT + 30)

// Logik-Konstanten
#define HOT_PIXEL_THRESH 12
#define GEIGER_NORM_FACTOR 1.5e5 
#define GEIGER_SMOOTH_FACTOR 0.05
#define GEIGER_ALERT_THRESH 5.0
#define BG_LEARN_RATE 0.01 
#define RADIONULLON_THRESH 20

// Audio-Konstanten
#define AUDIO_FREQ 44100
#define AUDIO_FORMAT AUDIO_S16 
#define CLICK_DURATION_S 0.034 
#define CLICK_GAIN 10000.0 
#define V4L2_BUF_COUNT 4 

// Eigene PI Definition
const double PI = 3.14159265358979323846;


// --- II. Eigene Mathematik- und Farb-Implementierungen ---

double my_fmax(double a, double b) { return a > b ? a : b; }
double my_fmin(double a, double b) { return a < b ? a : b; }

// Näherung für Sinus
double my_sin(double x) {
    double x_sq = x * x;
    double x_pow3 = x_sq * x; 
    double x_pow5 = x_pow3 * x_sq; 
    return x - (x_pow3 / 6.0) + (x_pow5 / 120.0);
}

// Näherung für Exponentialfunktion
double my_exp(double x) {
    double x_sq = x * x;
    double x_pow3 = x_sq * x;
    double x_pow4 = x_pow3 * x;
    return 1.0 + x + (x_sq / 2.0) + (x_pow3 / 6.0) + (x_pow4 / 24.0);
}

/**
 * Konvertiert RGB (0-255) zu HSV (H: 0-360, S: 0.0-1.0, V: 0.0-1.0).
 */
void rgb_to_hsv(uint8_t r8, uint8_t g8, uint8_t b8, double *h, double *s, double *v) {
    double r = r8 / 255.0;
    double g = g8 / 255.0;
    double b = b8 / 255.0;

    double max = my_fmax(my_fmax(r, g), b);
    double min = my_fmin(my_fmin(r, g), b);
    double delta = max - min;
    
    *v = max; 

    if (delta == 0.0) { 
        *h = 0.0;
        *s = 0.0;
        return;
    }

    *s = delta / max;

    if (max == r) {
        *h = 60.0 * (g - b) / delta;
    } else if (max == g) {
        *h = 60.0 * (2.0 + (b - r) / delta);
    } else { 
        *h = 60.0 * (4.0 + (r - g) / delta);
    }
    
    if (*h < 0.0) {
        *h += 360.0;
    }
}

/**
 * Konvertiert HSV (H: 0-360, S: 0.0-1.0, V: 0.0-1.0) zu RGB (0-255).
 */
void hsv_to_rgb(double h, double s, double v, uint8_t *r8, uint8_t *g8, uint8_t *b8) {
    if (s == 0.0) {
        *r8 = *g8 = *b8 = (uint8_t)(v * 255.0);
        return;
    }

    double H = h / 60.0;
    int i = (int)H;
    double f = H - i;
    double p = v * (1.0 - s);
    double q = v * (1.0 - s * f);
    double t = v * (1.0 - s * (1.0 - f));
    
    double r, g, b;

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = g = b = v; break;
    }

    *r8 = (uint8_t)(r * 255.0);
    *g8 = (uint8_t)(g * 255.0);
    *b8 = (uint8_t)(b * 255.0);
}


// --- III. Datenstrukturen ---
typedef struct {
    double shortHistory[GRAPH_POINTS];
    double avgHistory[GRAPH_POINTS];
    double longHistory[GRAPH_POINTS];
    double currentRad;
    double avgRad;
    double totalDose;
    bool alertState;
} GeigerStats;

typedef struct {
    float *bg; 
    uint8_t *flicker; 
    double currentRadSmooth;
    double totalDoseAcc;
    int longTermCounter;
    uint8_t prevGlobalAvg;
} RadionullonState;

typedef struct {
    void *start;
    size_t length;
} V4L2Buffer;

typedef struct {
    int fd;
    V4L2Buffer *buffers;
    unsigned int n_buffers;
} V4L2Context;


// --- IV. Funktionsprototypen ---
void audio_play_click(double freq);
int audio_init();
void audio_cleanup();

V4L2Context* v4l2_init(const char *dev_name, int w, int h);
void v4l2_to_sdl_surface(const uint8_t *yuyv_data, SDL_Surface *dst_surf);
uint8_t *v4l2_grab_frame(V4L2Context *ctx);
void v4l2_unqueue_buffer(V4L2Context *ctx);
void v4l2_cleanup(V4L2Context *ctx);

void radionullon_process(SDL_Surface *original_img, SDL_Surface *processed_img, GeigerStats *stats, RadionullonState *state, double delta_time);
void render_ui(SDL_Renderer *renderer, const GeigerStats *stats);
int worker_thread(void *data);


// --- V. Globale Variablen ---
SDL_mutex *g_mutex = NULL;
GeigerStats g_stats;
RadionullonState g_state; 
SDL_Surface *g_original_surface = NULL;
SDL_Surface *g_processed_surface = NULL;
bool g_running = true;
V4L2Context *g_v4l2_ctx = NULL; 
SDL_AudioDeviceID g_audio_device_id = 0;
struct v4l2_buffer current_v4l2_buf; 
SDL_Window *g_window = NULL; // Hinzugefügt: Globaler Fenster-Pointer


// --- VI. Hilfs-Makros und Utility-Funktionen ---
#define CLAMP(val) ((val) < 0 ? 0 : ((val) > 255 ? 255 : (val)))
#define C_BOUND(min, val, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

double get_current_time_double() {
    return (double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
}

void rotate_buffer(double *buf, double val, int size) {
    memmove(buf, buf + 1, (size - 1) * sizeof(double));
    buf[size - 1] = val;
}

void logic_init(RadionullonState *state) {
    int size = IMG_W * IMG_H;
    state->bg = (float*)calloc(size, sizeof(float));
    state->flicker = (uint8_t*)calloc(size, sizeof(uint8_t));
    for(int i=0; i<size; i++) state->bg[i] = 128.0f;
}


// --- VII. Audio-Engine (mit eigenen Math-Funktionen) ---
int audio_init() { 
    SDL_AudioSpec wanted_spec, obtained_spec;
    SDL_zero(wanted_spec); wanted_spec.freq = AUDIO_FREQ; wanted_spec.format = AUDIO_S16;
    wanted_spec.channels = 1; wanted_spec.samples = 1024;
    g_audio_device_id = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &obtained_spec, 0);
    if (g_audio_device_id == 0) return 0;
    SDL_PauseAudioDevice(g_audio_device_id, 0); return 1;
}

void audio_play_click(double freq) {
    if (g_audio_device_id == 0) return;
    int sample_count = (int)(CLICK_DURATION_S * AUDIO_FREQ);
    int16_t *data = (int16_t *)malloc(sample_count * sizeof(int16_t));
    if (!data) return;
    for (int i = 0; i < sample_count; ++i) {
        double t = (double)i / (double)AUDIO_FREQ;
        
        double angle = 2 * PI * freq * t;
        double decay = -t * 30.0;
        
        while (angle > (2 * PI)) { angle -= (2 * PI); }
        while (angle < 0) { angle += (2 * PI); }
        if (angle > PI) { angle = -(2 * PI - angle); }
        
        data[i] = (int16_t)(my_sin(angle) * my_exp(decay) * CLICK_GAIN);
    }
    SDL_QueueAudio(g_audio_device_id, data, sample_count * sizeof(int16_t));
    free(data);
}
void audio_cleanup() { if (g_audio_device_id != 0) SDL_CloseAudioDevice(g_audio_device_id); }


// --- VIII. YUYV Konvertierung ---
void v4l2_to_sdl_surface(const uint8_t *yuyv_data, SDL_Surface *dst_surf) {
    const uint8_t *src = yuyv_data;
    
    for (int y = 0; y < IMG_H; y++) {
        Uint32 *dst_line = (Uint32 *)((uint8_t*)dst_surf->pixels + y * dst_surf->pitch);
        
        for (int x = 0; x < IMG_W; x += 2) {
            int Y1 = src[0]; int U = src[1] - 128;
            int Y2 = src[2]; int V = src[3] - 128;
            src += 4;

            int C1 = Y1 - 16;
            int R1 = (298 * C1 + 409 * V + 128) >> 8;
            int G1 = (298 * C1 - 100 * U - 208 * V + 128) >> 8;
            int B1 = (298 * C1 + 516 * U + 128) >> 8;
            
            int C2 = Y2 - 16;
            int R2 = (298 * C2 + 409 * V + 128) >> 8;
            int G2 = (298 * C2 - 100 * U - 208 * V + 128) >> 8;
            int B2 = (298 * C2 + 516 * U + 128) >> 8;

            uint8_t r1 = CLAMP(R1); uint8_t g1 = CLAMP(G1); uint8_t b1 = CLAMP(B1);
            dst_line[0] = SDL_MapRGBA(dst_surf->format, r1, g1, b1, 0xFF);
            
            uint8_t r2 = CLAMP(R2); uint8_t g2 = CLAMP(G2); uint8_t b2 = CLAMP(B2);
            dst_line[1] = SDL_MapRGBA(dst_surf->format, r2, g2, b2, 0xFF);
            
            dst_line += 2;
        }
    }
}


// --- IX. V4L2-Schnittstelle (unverändert) ---
V4L2Context* v4l2_init(const char *dev_name, int w, int h) {
    V4L2Context *ctx = (V4L2Context*)calloc(1, sizeof(V4L2Context));
    if (!ctx) return NULL;
    ctx->fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (ctx->fd < 0) { free(ctx); return NULL; }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = w; fmt.fmt.pix.height = h; fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if (ioctl(ctx->fd, VIDIOC_S_FMT, &fmt) < 0) { close(ctx->fd); free(ctx); return NULL; }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = V4L2_BUF_COUNT; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(ctx->fd, VIDIOC_REQBUFS, &req) < 0) { close(ctx->fd); free(ctx); return NULL; }
    ctx->n_buffers = req.count;
    ctx->buffers = (V4L2Buffer*)calloc(ctx->n_buffers, sizeof(V4L2Buffer));

    for (unsigned int i = 0; i < ctx->n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;

        if (ioctl(ctx->fd, VIDIOC_QUERYBUF, &buf) < 0) { return NULL; }
        ctx->buffers[i].length = buf.length;
        ctx->buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, buf.m.offset);
        if (ctx->buffers[i].start == MAP_FAILED) { return NULL; }

        if (ioctl(ctx->fd, VIDIOC_QBUF, &buf) < 0) { return NULL; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(ctx->fd, VIDIOC_STREAMON, &type) < 0) { return NULL; }

    return ctx;
}

uint8_t *v4l2_grab_frame(V4L2Context *ctx) {
    fd_set fds; FD_ZERO(&fds); FD_SET(ctx->fd, &fds);
    struct timeval tv = {1, 0}; 

    int r = select(ctx->fd + 1, &fds, NULL, NULL, &tv);
    if (r <= 0) return NULL; 

    memset(&current_v4l2_buf, 0, sizeof(current_v4l2_buf));
    current_v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    current_v4l2_buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(ctx->fd, VIDIOC_DQBUF, &current_v4l2_buf) < 0) {
        if (errno != EAGAIN) { perror("VIDIOC_DQBUF error"); }
        return NULL;
    }
    return (uint8_t*)ctx->buffers[current_v4l2_buf.index].start;
}

void v4l2_unqueue_buffer(V4L2Context *ctx) { 
    if (ioctl(ctx->fd, VIDIOC_QBUF, &current_v4l2_buf) < 0) {
        perror("VIDIOC_QBUF error");
    }
}

void v4l2_cleanup(V4L2Context *ctx) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(ctx->fd, VIDIOC_STREAMOFF, &type);
    
    for (unsigned int i = 0; i < ctx->n_buffers; ++i) {
        if (ctx->buffers[i].start) {
            munmap(ctx->buffers[i].start, ctx->buffers[i].length);
        }
    }
    free(ctx->buffers);
    close(ctx->fd);
    free(ctx);
}


// --- X. Radionullon-Kernlogik (HSV-BASIERT) ---
void radionullon_process(SDL_Surface *original_img, SDL_Surface *processed_img, GeigerStats *stats, RadionullonState *state, double delta_time) {
    int hotPixels = 0;
    
    if (SDL_BlitSurface(original_img, NULL, processed_img, NULL) != 0) return;

    for (int y = 0; y < IMG_H; y++) {
        Uint32 *src_pixel = (Uint32 *)((uint8_t*)original_img->pixels + y * original_img->pitch);
        Uint32 *dst_pixel = (Uint32 *)((uint8_t*)processed_img->pixels + y * processed_img->pitch);
        
        for (int x = 0; x < IMG_W; x++) {
            int idx = y * IMG_W + x;
            uint8_t r, g, b;
            SDL_GetRGB(src_pixel[x], original_img->format, &r, &g, &b);
            
            double h, s, v;
            rgb_to_hsv(r, g, b, &h, &s, &v);
            
            uint8_t brightness = (uint8_t)(v * 255.0);
            
            if (brightness > HOT_PIXEL_THRESH) hotPixels++;

            state->bg[idx] = state->bg[idx] * (1.0f - BG_LEARN_RATE) + (float)brightness * BG_LEARN_RATE;
            uint8_t residue = (uint8_t)C_BOUND(0, abs((int)brightness - (int)state->bg[idx]), 255);
            
            int score = (int)residue; 
            
            if (score > RADIONULLON_THRESH) {
                // Hot Pixel: Manipuliere HSV direkt (Reinrot)
                double h_new = 0.0;     
                double s_new = 1.0;     
                double v_new = v;       
                
                uint8_t r_final, g_final, b_final;
                hsv_to_rgb(h_new, s_new, v_new, &r_final, &g_final, &b_final);
                
                dst_pixel[x] = SDL_MapRGB(processed_img->format, r_final, g_final, b_final);
            }
        }
    }
    
    double rawVal = (double)hotPixels / GEIGER_NORM_FACTOR;
    state->currentRadSmooth = (state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);
    
    stats->currentRad = rawVal;
    stats->avgRad = state->currentRadSmooth;
    stats->alertState = state->currentRadSmooth > GEIGER_ALERT_THRESH;

    state->totalDoseAcc += stats->avgRad * delta_time; 
    stats->totalDose = state->totalDoseAcc;

    if (stats->currentRad > 2.0) { audio_play_click(440.0 + (stats->currentRad * 100.0)); }
    rotate_buffer(stats->shortHistory, rawVal, GRAPH_POINTS);
    rotate_buffer(stats->avgHistory, state->currentRadSmooth, GRAPH_POINTS);
}


// --- XI. SDL Rendering (Dynamische Größen mit Globalem Fenster) ---
void draw_graph(SDL_Renderer *renderer, const double *history, SDL_Color color, double max_val, int window_width, int graph_height, int img_h) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_Point points[GRAPH_POINTS];
    double x_step = (double)window_width / (GRAPH_POINTS - 1);
    
    for (int i = 0; i < GRAPH_POINTS; i++) {
        int h = (int)((history[i] / max_val) * graph_height);
        h = C_BOUND(0, h, graph_height); 
        points[i].x = (int)(i * x_step);
        points[i].y = img_h + graph_height + 10 - h; 
    }
    SDL_RenderDrawLines(renderer, points, GRAPH_POINTS);
}

void render_ui(SDL_Renderer *renderer, const GeigerStats *stats) {
    int win_w, win_h;
    
    // FIX: Nutze den globalen Fenster-Pointer g_window anstelle von SDL_GetRenderWindow(renderer)
    if (g_window) {
        SDL_GetWindowSize(g_window, &win_w, &win_h);
    } else {
        // Fallback, sollte nicht eintreten
        return;
    }

    int graph_y_start = IMG_H + 10;
    int graph_h = win_h - graph_y_start - 20; 
    if (graph_h < 0) graph_h = 0;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_LockMutex(g_mutex);

    if (g_processed_surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, g_processed_surface);
        SDL_Rect dst_rect = {0, 0, IMG_W, IMG_H}; 
        if (texture) {
            SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
            SDL_DestroyTexture(texture);
        }
    }
    
    SDL_Rect graph_area = {0, graph_y_start, win_w, graph_h};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); 
    SDL_RenderFillRect(renderer, &graph_area);

    const double MAX_GEIGER_VALUE = 40.0;
    draw_graph(renderer, stats->shortHistory, (SDL_Color){255, 50, 50, 255}, MAX_GEIGER_VALUE, win_w, graph_h, IMG_H);
    draw_graph(renderer, stats->avgHistory, (SDL_Color){50, 255, 50, 255}, MAX_GEIGER_VALUE, win_w, graph_h, IMG_H);
    draw_graph(renderer, stats->longHistory, (SDL_Color){50, 50, 255, 255}, MAX_GEIGER_VALUE, win_w, graph_h, IMG_H);
    
    SDL_Rect alert_bar = {0, win_h - 10, win_w, 10};
    if (stats->alertState) { SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); } 
    else { SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255); }
    SDL_RenderFillRect(renderer, &alert_bar);
    
    SDL_UnlockMutex(g_mutex);
    SDL_RenderPresent(renderer);
}


// --- XII. Worker-Thread (unverändert) ---
int worker_thread(void *data) {
    g_v4l2_ctx = v4l2_init("/dev/video0", IMG_W, IMG_H); 
    if (!g_v4l2_ctx) { g_running = false; return -1; }
    logic_init(&g_state);

    g_original_surface = SDL_CreateRGBSurface(0, IMG_W, IMG_H, 32, 0xFF0000, 0x00FF00, 0x0000FF, 0xFF000000);
    g_processed_surface = SDL_CreateRGBSurface(0, IMG_W, IMG_H, 32, 0xFF0000, 0x00FF00, 0x0000FF, 0xFF000000);
    
    double last_time = get_current_time_double();

    while (g_running) {
        
        double current_time = get_current_time_double();
        double delta_time = current_time - last_time;
        last_time = current_time;
        
        uint8_t *yuyv_data = v4l2_grab_frame(g_v4l2_ctx);
        
        if (!yuyv_data) { 
            sched_yield(); 
            continue; 
        }

        v4l2_to_sdl_surface(yuyv_data, g_original_surface);

        SDL_LockMutex(g_mutex);
        radionullon_process(g_original_surface, g_processed_surface, &g_stats, &g_state, delta_time);
        SDL_UnlockMutex(g_mutex);
        
        v4l2_unqueue_buffer(g_v4l2_ctx); 
        
        sched_yield(); 
    }
    
    v4l2_cleanup(g_v4l2_ctx);
    return 0;
}


// --- XIII. Hauptfunktion (main) ---
int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { return -1; }
    if (!audio_init()) { fprintf(stderr, "Audio Init failed.\n"); }
    memset(&g_stats, 0, sizeof(g_stats));
    
    // Speichere den Pointer global, um ihn in render_ui nutzen zu können.
    g_window = SDL_CreateWindow("Radionullon Geiger (HSV)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, IMG_W, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (!g_window || !renderer) { goto cleanup; }
    
    g_mutex = SDL_CreateMutex();
    SDL_Thread *thread = SDL_CreateThread(worker_thread, "RadionullonWorker", NULL);
    if (!g_mutex || !thread) { goto cleanup; }
    
    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                g_running = false; quit = true;
            }
        }
        render_ui(renderer, &g_stats);
    }
    
    SDL_WaitThread(thread, NULL);
    
cleanup:
    audio_cleanup();
    
    if (g_state.bg) { free(g_state.bg); } 
    if (g_state.flicker) { free(g_state.flicker); }
    
    if (g_mutex) SDL_DestroyMutex(g_mutex);
    if (g_processed_surface) SDL_FreeSurface(g_processed_surface);
    if (g_original_surface) SDL_FreeSurface(g_original_surface);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (g_window) SDL_DestroyWindow(g_window);
    SDL_Quit();
    return 0;
}

