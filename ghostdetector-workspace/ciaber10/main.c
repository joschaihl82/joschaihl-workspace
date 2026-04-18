#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <alsa/asoundlib.h> 
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
#include <math.h> 

// --- I. Konfiguration und Konstanten ---
#define IMG_W 640
#define IMG_H 480
#define GRAPH_POINTS 120 

// Logik-Konstanten 
#define HOT_PIXEL_THRESH 15     
#define GEIGER_NORM_FACTOR 1.5e5 
#define GEIGER_SMOOTH_FACTOR 0.10 
#define GEIGER_ALERT_THRESH 5.0
#define BG_LEARN_RATE 0.05      
#define MAX_SCORE 100 
#define LONG_TERM_INTERVAL 100  

const int V4L2_BUF_COUNT = 4;
const double PI = 3.14159265358979323846;

// Audio-Konstanten 
#define AUDIO_FREQ 44100
#define CLICK_DURATION_S 0.005 
#define CLICK_GAIN 10000.0 
#define DECAY_FACTOR 150.0      

// GTK3 Statische Farben
const GdkRGBA RGBA_GREEN = { 0x0000 / 65535.0, 0x9600 / 65535.0, 0x0000 / 65535.0, 1.0 };  
const GdkRGBA RGBA_RED   = { 0xFFFF / 65535.0, 0x0000 / 65535.0, 0x0000 / 65535.0, 1.0 };  
const GdkRGBA RGBA_DARK  = { 0x1F1F / 65535.0, 0x1F1F / 65535.0, 0x1F1F / 65535.0, 1.0 };  
const GdkRGBA RGBA_WHITE = { 0xFFFF / 65535.0, 0xFFFF / 65535.0, 0xFFFF / 65535.0, 1.0 };  

// HSVA-Konstanten
#define HSVA_TRANSPARENT_H 0.0
#define HSVA_TRANSPARENT_S 0.0
#define HSVA_TRANSPARENT_V 0.0
#define HSVA_TRANSPARENT_A 0.0


// --- II. Datenstrukturen ---
typedef struct {
    double shortHistory[GRAPH_POINTS]; double avgHistory[GRAPH_POINTS];
    double longHistory[GRAPH_POINTS]; double currentRad;
    double avgRad; double totalDose; bool alertState;
} GeigerStats;

typedef struct {
    float *bg; uint8_t *flicker; double currentRadSmooth;
    double totalDoseAcc; int longTermCounter; uint8_t prevGlobalAvg;
} RadionullonState;

typedef struct { void *start; size_t length; } V4L2Buffer;

typedef struct {
    int fd; V4L2Buffer *buffers; unsigned int n_buffers;
} V4L2Context;


// --- III. Globale Variablen ---
GMutex g_mutex; 
GeigerStats g_stats; 
RadionullonState g_state; 
bool g_running = true;
V4L2Context *g_v4l2_ctx = NULL; 
snd_pcm_t *g_alsa_handle = NULL; 
struct v4l2_buffer current_v4l2_buf; 

// GTK3 Globals
GtkWidget *g_window = NULL;
GtkWidget *g_image = NULL;       
GtkWidget *g_alert_bar = NULL;   
GtkWidget *g_label_rate = NULL; 
GtkWidget *g_label_avg = NULL;
GtkWidget *g_label_dose = NULL;

GdkPixbuf *g_pixbuf_original = NULL;
GdkPixbuf *g_pixbuf_processed = NULL;
GdkPixbuf *g_pixbuf_overlay = NULL; 


// --- IV. Hilfs-Makros und Math-Funktionen ---
#define MY_CLAMP(val) ((val) < 0 ? 0 : ((val) > 255 ? 255 : (val)))
#define C_BOUND(min, val, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

double my_fmax(double a, double b) { return a > b ? a : b; }
double my_fmin(double a, double b) { return a < b ? a : b; }
double my_sin(double x) {
    double x_sq = x * x; double x_pow3 = x_sq * x; double x_pow5 = x_pow3 * x_sq; 
    return x - (x_pow3 / 6.0) + (x_pow5 / 120.0);
}
double my_exp(double x) {
    double x_sq = x * x; double x_pow3 = x_sq * x; double x_pow4 = x_pow3 * x;
    return 1.0 + x + (x_sq / 2.0) + (x_pow3 / 6.0) + (x_pow4 / 24.0);
}

double get_current_time_double() {
    return (double)g_get_monotonic_time() / 1000000.0;
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

// HSVA HILFSFUNKTIONEN
void rgb_to_hsv(uint8_t r8, uint8_t g8, uint8_t b8, double *h, double *s, double *v) {
    double r = r8 / 255.0; double g = g8 / 255.0; double b = b8 / 255.0;
    double max = my_fmax(my_fmax(r, g), b); double min = my_fmin(my_fmin(r, g), b);
    double delta = max - min; *v = max; 
    if (delta == 0.0) { *h = 0.0; *s = 0.0; return; }
    *s = delta / max;
    if (max == r) { *h = 60.0 * (g - b) / delta; } 
    else if (max == g) { *h = 60.0 * (2.0 + (b - r) / delta); } 
    else { *h = 60.0 * (4.0 + (r - g) / delta); }
    if (*h < 0.0) { *h += 360.0; }
}

void hsv_to_rgb(double h, double s, double v, uint8_t *r8, uint8_t *g8, uint8_t *b8) {
    if (s == 0.0) { *r8 = *g8 = *b8 = (uint8_t)(v * 255.0); return; }
    double H = h / 60.0; int i = (int)H; double f = H - i;
    double p = v * (1.0 - s); double q = v * (1.0 - s * f); double t = v * (1.0 - s * (1.0 - f));
    double r, g, b;
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break; case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break; case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break; case 5: r = v; g = p; b = q; break;
        default: r = g = b = v; break;
    }
    *r8 = (uint8_t)(r * 255.0); *g8 = (uint8_t)(g * 255.0); *b8 = (uint8_t)(b * 255.0);
}

void hsva_to_rgba(double h, double s, double v, double a, GdkRGBA *rgba) {
    uint8_t r8, g8, b8;
    hsv_to_rgb(h, s, v, &r8, &g8, &b8);
    
    rgba->red = (double)r8 / 255.0;
    rgba->green = (double)g8 / 255.0;
    rgba->blue = (double)b8 / 255.0;
    rgba->alpha = C_BOUND(0.0, a, 1.0);
}


// --- V. Audio-Engine (ALSA) ---
int audio_init() {
    int err;
    if ((err = snd_pcm_open(&g_alsa_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "ALSA-Fehler: Kann PCM-Gerät nicht öffnen: %s\n", snd_strerror(err));
        return 0;
    }
    
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(g_alsa_handle, params);
    
    snd_pcm_hw_params_set_access(g_alsa_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(g_alsa_handle, params, SND_PCM_FORMAT_S16_LE); 
    snd_pcm_hw_params_set_channels(g_alsa_handle, params, 1); 
    unsigned int rate = AUDIO_FREQ;
    snd_pcm_hw_params_set_rate_near(g_alsa_handle, params, &rate, 0);
    
    if ((err = snd_pcm_hw_params(g_alsa_handle, params)) < 0) {
        fprintf(stderr, "ALSA-Fehler: Kann Hardware-Parameter nicht setzen: %s\n", snd_strerror(err));
        snd_pcm_close(g_alsa_handle);
        g_alsa_handle = NULL;
        return 0;
    }
    
    snd_pcm_uframes_t buffer_size = AUDIO_FREQ / 10; 
    snd_pcm_hw_params_set_buffer_size_near(g_alsa_handle, params, &buffer_size);

    return 1;
}

void audio_play_click(double freq) {
    if (g_alsa_handle == NULL) return;
    
    int sample_count = (int)(CLICK_DURATION_S * AUDIO_FREQ);
    int16_t *data = (int16_t *)malloc(sample_count * sizeof(int16_t));
    if (!data) return;
    
    for (int i = 0; i < sample_count; ++i) {
        double t = (double)i / (double)AUDIO_FREQ;
        double angle = 2 * PI * freq * t;
        double decay = -t * DECAY_FACTOR;
        while (angle > (2 * PI)) { angle -= (2 * PI); }
        while (angle < 0) { angle += (2 * PI); }
        if (angle > PI) { angle = -(2 * PI - angle); }
        data[i] = (int16_t)(my_sin(angle) * my_exp(decay) * CLICK_GAIN);
    }
    
    int frames_to_write = sample_count;
    int err = snd_pcm_writei(g_alsa_handle, data, frames_to_write);
    
    if (err < 0) {
        if (err == -EPIPE) {
            snd_pcm_prepare(g_alsa_handle);
            snd_pcm_writei(g_alsa_handle, data, frames_to_write);
        } else {
            fprintf(stderr, "ALSA-Fehler beim Schreiben: %s\n", snd_strerror(err));
        }
    }
    
    free(data);
}

void audio_cleanup() { 
    if (g_alsa_handle != NULL) {
        snd_pcm_drain(g_alsa_handle);
        snd_pcm_close(g_alsa_handle);
        g_alsa_handle = NULL;
    }
}


// --- VI. V4L2-Schnittstelle ---
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

void yuyv_to_pixbuf(const uint8_t *yuyv_data, GdkPixbuf *dst_pixbuf) {
    guchar *pixels = gdk_pixbuf_get_pixels(dst_pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(dst_pixbuf);
    const uint8_t *src = yuyv_data;

    for (int y = 0; y < IMG_H; y++) {
        guchar *dst_line = pixels + y * rowstride;
        for (int x = 0; x < IMG_W; x += 2) {
            int Y1 = src[0]; int U = src[1] - 128;
            int Y2 = src[2]; int V = src[3] - 128;
            src += 4;

            int C1 = Y1 - 16;
            int R1 = (298 * C1 + 409 * V + 128) >> 8;
            int G1 = (298 * C1 - 100 * U - 208 * V + 128) >> 8;
            int B1 = (298 * C1 + 516 * U + 128) >> 8;
            dst_line[0] = MY_CLAMP(R1); dst_line[1] = MY_CLAMP(G1); dst_line[2] = MY_CLAMP(B1);
            
            int C2 = Y2 - 16;
            int R2 = (298 * C2 + 409 * V + 128) >> 8;
            int G2 = (298 * C2 - 100 * U - 208 * V + 128) >> 8;
            int B2 = (298 * C2 + 516 * U + 128) >> 8;
            dst_line[3] = MY_CLAMP(R2); dst_line[4] = MY_CLAMP(G2); dst_line[5] = MY_CLAMP(B2);
            
            dst_line += 6;
        }
    }
}


// --- VII. Radionullon-Kernlogik ---

void radionullon_process_gtk(GdkPixbuf *original_pb, GdkPixbuf *processed_pb, GeigerStats *stats, RadionullonState *state, double delta_time) {
    int hotPixels = 0;
    
    gdk_pixbuf_copy_area(original_pb, 0, 0, IMG_W, IMG_H, processed_pb, 0, 0);

    const double HUE_MIN = 0.0;     
    const double HUE_MAX = 240.0;   
    const double GAMMA = 0.8; 

    guchar *src_pixels = gdk_pixbuf_get_pixels(original_pb);
    guchar *dst_pixels = gdk_pixbuf_get_pixels(processed_pb);
    int rowstride = gdk_pixbuf_get_rowstride(original_pb);
    
    state->longTermCounter++;
    if (state->longTermCounter >= LONG_TERM_INTERVAL) {
        rotate_buffer(stats->longHistory, stats->avgRad, GRAPH_POINTS);
        state->longTermCounter = 0;
    }
    
    for (int y = 0; y < IMG_H; y++) {
        guchar *src_line = src_pixels + y * rowstride;
        guchar *dst_line = dst_pixels + y * gdk_pixbuf_get_rowstride(processed_pb); 
        
        for (int x = 0; x < IMG_W; x++) {
            int idx = y * IMG_W + x;
            uint8_t r = src_line[x*3];
            uint8_t g = src_line[x*3 + 1];
            uint8_t b = src_line[x*3 + 2];
            
            double h, s, v;
            rgb_to_hsv(r, g, b, &h, &s, &v);
            uint8_t brightness = (uint8_t)(v * 255.0);
            
            v = pow(v, GAMMA); 
            if (brightness > HOT_PIXEL_THRESH) hotPixels++;

            state->bg[idx] = state->bg[idx] * (1.0f - BG_LEARN_RATE) + (float)brightness * BG_LEARN_RATE;
            uint8_t residue = (uint8_t)C_BOUND(0, abs((int)brightness - (int)state->bg[idx]), 255);
            int score = (int)residue; 
            
            if (score > 10) { 
                state->flicker[idx] = 255; 
            } else if (state->flicker[idx] > 0) {
                state->flicker[idx] = (uint8_t)my_fmax(0, state->flicker[idx] - 2); 
            }
            
            double normalized_score = (double)C_BOUND(0, score, MAX_SCORE) / MAX_SCORE;
            double h_new = (1.0 - normalized_score) * HUE_MAX + normalized_score * HUE_MIN;
            
            double s_new = my_fmin(1.0, 0.7 + 0.3 * normalized_score); 
            double v_new = v;
                
            uint8_t r_final, g_final, b_final;
            hsv_to_rgb(h_new, s_new, v_new, &r_final, &g_final, &b_final);
            
            dst_line[x*4] = r_final;
            dst_line[x*4 + 1] = g_final;
            dst_line[x*4 + 2] = b_final;
            dst_line[x*4 + 3] = 255; 
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


// --- VIII. Manuelles Pixel-Zeichnen (Ohne Cairo) ---

void draw_line(guchar *pixels, int rowstride, int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    int dx = abs(x1 - x0); int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0); int sy = y0 < y1 ? 1 : -1; 
    int err = dx + dy;  
    
    while (true) {
        if (x0 >= 0 && x0 < IMG_W && y0 >= 0 && y0 < IMG_H) {
            int pixel_idx = y0 * rowstride + x0 * 4;
            double alpha = (double)a / 255.0;
            
            pixels[pixel_idx + 0] = (uint8_t)(pixels[pixel_idx + 0] * (1.0 - alpha) + r * alpha);
            pixels[pixel_idx + 1] = (uint8_t)(pixels[pixel_idx + 1] * (1.0 - alpha) + g * alpha);
            pixels[pixel_idx + 2] = (uint8_t)(pixels[pixel_idx + 2] * (1.0 - alpha) + b * alpha);
            pixels[pixel_idx + 3] = a; 
        }
        
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } 
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw_overlay_manual(GdkPixbuf *overlay_pb, GeigerStats *stats, RadionullonState *state) {
    guchar *pixels = gdk_pixbuf_get_pixels(overlay_pb);
    int rowstride = gdk_pixbuf_get_rowstride(overlay_pb);
    
    gdk_pixbuf_fill(overlay_pb, 0x00000000); 

    // --- 2. Ghostcounter (Flicker Map) ---
    if (state->flicker) {
        for (int y = 0; y < IMG_H; y++) {
            for (int x = 0; x < IMG_W; x++) {
                int idx = y * IMG_W + x;
                uint8_t alpha = state->flicker[idx]; 
                
                if (alpha > 0) {
                    int pixel_idx = y * rowstride + x * 4;
                    
                    pixels[pixel_idx + 0] = 255; 
                    pixels[pixel_idx + 1] = 0;   
                    pixels[pixel_idx + 2] = 0;   
                    pixels[pixel_idx + 3] = alpha; 
                }
            }
        }
    }


    // --- 3. Graphen-Zeichnen (Unterer Rand) ---
    const int GRAPH_H = 120; 
    const int GRAPH_Y_START = IMG_H - GRAPH_H; 
    
    // Hintergrund des Graphen
    for (int y = GRAPH_Y_START; y < IMG_H; y++) {
        for (int x = 0; x < IMG_W; x++) {
            int pixel_idx = y * rowstride + x * 4;
            pixels[pixel_idx + 0] = 0x1F; pixels[pixel_idx + 1] = 0x1F; 
            pixels[pixel_idx + 2] = 0x1F; pixels[pixel_idx + 3] = 128; 
        }
    }
    
    const double max_val = GEIGER_ALERT_THRESH * 1.5; 
    const double x_scale = (double)IMG_W / GRAPH_POINTS;
    const double y_scale = (double)GRAPH_H / max_val;

    #define GET_Y_POS(val) (GRAPH_Y_START + GRAPH_H - (int)((val) * y_scale))

    // Hilfslinie: Alert Threshold
    int alert_y = GET_Y_POS(GEIGER_ALERT_THRESH);
    for(int x = 0; x < IMG_W; x++) {
        if (alert_y >= GRAPH_Y_START && alert_y < IMG_H) {
             int pixel_idx = alert_y * rowstride + x * 4;
             pixels[pixel_idx + 0] = 255; pixels[pixel_idx + 1] = 0; 
             pixels[pixel_idx + 2] = 0; pixels[pixel_idx + 3] = 255;
        }
    }
    
    // Funktion zum Zeichnen der Graphen
    // HINWEIS: 'int' wurde entfernt, um Neudefinition zu verhindern.
    #define PLOT_GRAPH(history, r, g, b, a) \
        prev_x = 0; prev_y = GET_Y_POS(history[0]); \
        for (int i = 1; i < GRAPH_POINTS; i++) { \
            int curr_x = (int)(i * x_scale); \
            int curr_y = GET_Y_POS(history[i]); \
            draw_line(pixels, rowstride, prev_x, prev_y, curr_x, curr_y, r, g, b, a); \
            prev_x = curr_x; prev_y = curr_y; \
        }

    // FIX: Deklariere die Variablen einmalig VOR dem ersten Makro-Aufruf.
    int prev_x, prev_y; 

    // Graphen zeichnen (Short, Avg, Long)
    PLOT_GRAPH(stats->shortHistory, 255, 255, 255, 128); // Weiß
    PLOT_GRAPH(stats->avgHistory, 0, 255, 0, 200);   // Grün
    PLOT_GRAPH(stats->longHistory, 0, 128, 255, 200);  // Blau
}


// --- IX. GTK3 Rendering ---

static gboolean refresh_gui(gpointer user_data) {
    if (!g_window) return FALSE;

    g_mutex_lock(&g_mutex);
    
    if (g_pixbuf_overlay) {
        draw_overlay_manual(g_pixbuf_overlay, &g_stats, &g_state);
    }
    
    if (g_pixbuf_processed && g_pixbuf_overlay && g_image) {
        
        gdk_pixbuf_composite(g_pixbuf_overlay, g_pixbuf_processed,
                            0, 0, IMG_W, IMG_H,
                            0, 0, 1.0, 1.0,
                            GDK_INTERP_NEAREST, 255); 
        
        GtkAllocation allocation;
        gtk_widget_get_allocation(g_image, &allocation);
        int target_width = allocation.width;
        int target_height = allocation.height;

        if (target_width > 0 && target_height > 0) {
            GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(
                g_pixbuf_processed, 
                target_width, 
                target_height, 
                GDK_INTERP_BILINEAR 
            );
            
            if (scaled_pixbuf) {
                gtk_image_set_from_pixbuf(GTK_IMAGE(g_image), scaled_pixbuf);
                g_object_unref(scaled_pixbuf); 
            }
        } else {
             gtk_image_set_from_pixbuf(GTK_IMAGE(g_image), g_pixbuf_processed);
        }
    }
    
    char text_buffer[64];
    snprintf(text_buffer, sizeof(text_buffer), "Rate: %.2f CPM", g_stats.currentRad * 60.0);
    gtk_label_set_text(GTK_LABEL(g_label_rate), text_buffer); 
    
    snprintf(text_buffer, sizeof(text_buffer), "Avg: %.2f CPM", g_stats.avgRad * 60.0);
    gtk_label_set_text(GTK_LABEL(g_label_avg), text_buffer); 
    
    snprintf(text_buffer, sizeof(text_buffer), "Dose: %.4f mSv", g_stats.totalDose / 1000.0);
    gtk_label_set_text(GTK_LABEL(g_label_dose), text_buffer); 
    
    if (g_alert_bar) {
        const GdkRGBA *color;
        if (g_stats.alertState) {
            color = &RGBA_RED;
        } else {
            color = &RGBA_GREEN;
        }
        // DEPRECATED WARNINGS: Verwendung von override_color ist hier für die Funktion beibehalten.
        gtk_widget_override_color(g_alert_bar, GTK_STATE_FLAG_NORMAL, color); 
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(g_alert_bar), 1.0); 
    }

    g_mutex_unlock(&g_mutex);
    return TRUE; 
}


// --- X. Worker-Thread ---
static gpointer worker_thread_gtk(gpointer data) {
    g_v4l2_ctx = v4l2_init("/dev/video0", IMG_W, IMG_H); 
    if (!g_v4l2_ctx) { 
        fprintf(stderr, "FATAL: V4L2 device /dev/video0 failed. Check permissions.\n");
        g_running = false; 
        return NULL; 
    }
    logic_init(&g_state);
    
    g_pixbuf_original = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, IMG_W, IMG_H);
    g_pixbuf_processed = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, IMG_W, IMG_H); 
    g_pixbuf_overlay = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, IMG_W, IMG_H); 
    
    if (!g_pixbuf_original || !g_pixbuf_processed || !g_pixbuf_overlay) {
        fprintf(stderr, "FATAL: Failed to create GdkPixbuf.\n");
        g_running = false; 
        return NULL;
    }

    double last_time = get_current_time_double();
    while (g_running) {
        double current_time = get_current_time_double();
        double delta_time = current_time - last_time;
        last_time = current_time;
        
        uint8_t *yuyv_data = v4l2_grab_frame(g_v4l2_ctx);
        if (yuyv_data) {
            yuyv_to_pixbuf(yuyv_data, g_pixbuf_original); 

            g_mutex_lock(&g_mutex);
            radionullon_process_gtk(g_pixbuf_original, g_pixbuf_processed, &g_stats, &g_state, delta_time);
            g_mutex_unlock(&g_mutex);
            
            v4l2_unqueue_buffer(g_v4l2_ctx); 
        }
        
        sched_yield(); 
    }
    v4l2_cleanup(g_v4l2_ctx);
    
    if (g_pixbuf_original) g_object_unref(g_pixbuf_original);
    if (g_pixbuf_processed) g_object_unref(g_pixbuf_processed);
    if (g_pixbuf_overlay) g_object_unref(g_pixbuf_overlay);
    
    return NULL;
}


// --- XI. Hauptfunktion (GUI-Setup) ---

static void activate(GtkApplication *app, gpointer user_data) {
    
    g_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(g_window), "Radionullon Geiger (V11.1 - Fix)");
    gtk_window_set_default_size(GTK_WINDOW(g_window), 800, 600); 

    GdkRGBA rgba_transparent;
    hsva_to_rgba(HSVA_TRANSPARENT_H, HSVA_TRANSPARENT_S, HSVA_TRANSPARENT_V, HSVA_TRANSPARENT_A, &rgba_transparent);
    
    // --- GUI LAYOUT ---
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    // DEPRECATED WARNINGS
    gtk_widget_override_background_color(main_box, GTK_STATE_FLAG_NORMAL, &RGBA_DARK);

    // 1. Text-Labels
    g_label_rate = gtk_label_new("Rate: 0.00 CPM");
    g_label_avg = gtk_label_new("Avg: 0.00 CPM");
    g_label_dose = gtk_label_new("Dose: 0.0000 mSv");
    
    // DEPRECATED WARNINGS
    gtk_widget_override_color(g_label_rate, GTK_STATE_FLAG_NORMAL, &RGBA_WHITE);
    gtk_widget_override_background_color(g_label_rate, GTK_STATE_FLAG_NORMAL, &rgba_transparent);
    
    // DEPRECATED WARNINGS
    gtk_widget_override_color(g_label_avg, GTK_STATE_FLAG_NORMAL, &RGBA_WHITE);
    gtk_widget_override_background_color(g_label_avg, GTK_STATE_FLAG_NORMAL, &rgba_transparent);

    // DEPRECATED WARNINGS
    gtk_widget_override_color(g_label_dose, GTK_STATE_FLAG_NORMAL, &RGBA_WHITE);
    gtk_widget_override_background_color(g_label_dose, GTK_STATE_FLAG_NORMAL, &rgba_transparent);
    
    // 2. Video-Widget (GtkImage) - Basis des Overlays
    g_image = gtk_image_new();
    gtk_widget_set_hexpand(g_image, TRUE); 
    gtk_widget_set_vexpand(g_image, TRUE); 
    
    // 3. Overlay
    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(overlay), g_image); 

    // Labels als Overlay
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), g_label_rate);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), g_label_avg);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), g_label_dose);
    
    // Statische Positionierung der Labels
    gtk_widget_set_halign(g_label_rate, GTK_ALIGN_START);
    gtk_widget_set_valign(g_label_rate, GTK_ALIGN_START);
    gtk_widget_set_margin_start(g_label_rate, 10);
    gtk_widget_set_margin_top(g_label_rate, 10);
    
    gtk_widget_set_halign(g_label_avg, GTK_ALIGN_START);
    gtk_widget_set_valign(g_label_avg, GTK_ALIGN_START);
    gtk_widget_set_margin_start(g_label_avg, 10);
    gtk_widget_set_margin_top(g_label_avg, 40);

    gtk_widget_set_halign(g_label_dose, GTK_ALIGN_END);
    gtk_widget_set_valign(g_label_dose, GTK_ALIGN_START);
    gtk_widget_set_margin_end(g_label_dose, 10);
    gtk_widget_set_margin_top(g_label_dose, 10);
    
    gtk_box_pack_start(GTK_BOX(main_box), overlay, TRUE, TRUE, 0);

    // 4. Alert Bar
    g_alert_bar = gtk_progress_bar_new();
    gtk_widget_set_hexpand(g_alert_bar, TRUE);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(g_alert_bar), FALSE);
    // DEPRECATED WARNINGS
    gtk_widget_override_background_color(g_alert_bar, GTK_STATE_FLAG_NORMAL, &RGBA_DARK);
    
    gtk_box_pack_start(GTK_BOX(main_box), g_alert_bar, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(g_window), main_box);
    
    // --- Laufzeit-Initialisierung ---
    g_mutex_init(&g_mutex);
    
    if (!audio_init()) { 
        fprintf(stderr, "WARN: ALSA Audio Init failed. No sound will be played.\n");
    }
    
    memset(&g_stats, 0, sizeof(g_stats));
    
    g_thread_new("RadionullonWorker", worker_thread_gtk, NULL);
    g_timeout_add(1000 / 30, refresh_gui, NULL); 

    gtk_widget_show_all(g_window);
}

int main(int argc, char *argv[]) {
    
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("com.radionullon.gtk3.static", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_running = FALSE;
    g_mutex_clear(&g_mutex);
    
    audio_cleanup(); 
    
    g_object_unref(app);
    return status;
}
