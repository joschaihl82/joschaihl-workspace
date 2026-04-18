#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <errno.h>
#include <math.h>


// --- I. Konfiguration und Konstanten (Berlin 6 Fixed) ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define GRAPH_POINTS 100
#define BPP 4 // Bytes per Pixel: RGBA für SDL2-Texture

// Core Tunables
#define BG_LEARN_RATE 0.01
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN 1.6
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH 30
#define HOT_PIXEL_THRESH 200
#define GEIGER_SMOOTH_FACTOR 0.3
#define GEIGER_NORM_FACTOR 100.0
#define GEIGER_ALERT_THRESH 5.0 // Dient als Skalierungsgrundlage

#define LONG_EXP_FRAMES 60
#define LONG_EXP_LEARN (1.0 / LONG_EXP_FRAMES)

// CIA Berlin6 Preset (fixed)
const double CIA_BERLIN6_SENS_SCALE = 1.5;
const double CIA_BERLIN6_CLICK_GAIN = 1.4;
const double CIA_BERLIN6_CLICK_MAX = 400.0;
const double CIA_BERLIN6_ALERT_CONFIRM_SEC = 3.0;
const double CIA_BERLIN6_ALERT_HOLD_SEC = 0.8;
const double CIA_BERLIN6_ALERT_THRESHOLD_S_PER_S = (GEIGER_ALERT_THRESH / 3600.0) * 0.7;

// Audio Konstanten
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_PERIOD_FRAMES 256
#define M_PI 3.14159265358979323846

// --- II. Datenstrukturen ---

typedef struct {
    double currentRad; // Sv/s
    double avgRad;     // Sv/s
    double totalDose;  // Sv
    bool alertState;
    uint64_t startTimeMs;
    uint64_t currentTimeMs;
    double shortHistory[GRAPH_POINTS];
    double avgHistory[GRAPH_POINTS];
    double longHistory[GRAPH_POINTS];
} GeigerStats;

typedef struct {
    // Shared state
    double currentRadSmooth; // Sv/s
    double totalDoseAcc;     // Sv
    int longTermCounter;
    
    // Berlin6 state
    double berlin_peak_start_sec;
    bool berlin_alert_active;

    // Internal buffers
    float *bg;
    uint8_t *flicker;
    double prevGlobalAvg;
    double *longExp;
    uint8_t *gray;
    uint8_t *residue;
    uint8_t *edge;
    
} GeigerLogicState;

typedef struct {
    void *start;
    size_t length;
} V4L2Buffer;

typedef struct {
    int fd;
    V4L2Buffer *buffers;
    unsigned int n_buffers;
    struct v4l2_buffer current_v4l2_buf;
} V4L2Context;

typedef struct {
    double activity;
    uint64_t timeMs;
} ClickEvent;

// --- III. Globale und Shared Variablen (Protected by Mutex) ---

// SDL2, ALSA, TTF Globals (Main Thread Only)
SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_Texture *g_texture_original = NULL;
SDL_Texture *g_texture_processed = NULL;
TTF_Font *g_font_small = NULL;
TTF_Font *g_font_mono = NULL;
snd_pcm_t *g_alsa_pcm = NULL;

// Shared Data (Protected by Mutex)
SDL_mutex *g_mutex = NULL;
SDL_cond *g_cond_audio = NULL;
volatile bool g_running = true;
volatile bool g_frame_ready = false;

// Shared/Worker Buffers
uint32_t *g_image_pixels_original = NULL; // RGB32
uint32_t *g_image_pixels_processed = NULL; // RGB32
volatile GeigerStats g_stats;
V4L2Context *g_v4l2_ctx = NULL;

// Audio Queue
#define CLICK_QUEUE_MAX 8192
ClickEvent g_click_queue[CLICK_QUEUE_MAX];
volatile int g_queue_head = 0;
volatile int g_queue_tail = 0;

// --- IV. Hilfsfunktionen ---

// Ersetzt Qt::qBound und Qt::qRgb, Qt::qGray
#define C_BOUND(min, val, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define C_QGRAY(r, g, b) ((uint8_t)((r) * 0.299 + (g) * 0.587 + (b) * 0.114))
#define C_QRGB(r, g, b) (0xFF000000 | ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))
#define C_QRED(rgb) (uint8_t)(((rgb) >> 16) & 0xFF)
#define C_QGREEN(rgb) (uint8_t)(((rgb) >> 8) & 0xFF)
#define C_QBLUE(rgb) (uint8_t)((rgb) & 0xFF)

uint64_t get_current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

double get_current_time_sec() {
    return (double)get_current_time_ms() / 1000.0;
}

// C-Ersatz für QString-Formatierung (vereinfacht, nur Konsolenausgabe)
void format_sv_per_s_c(double value, char *buffer, size_t buf_size) {
    if (isnan(value)) { snprintf(buffer, buf_size, "NaN Sv/s"); return; }
    double absv = fabs(value);
    const char *prefix = "";
    double scaled = value;
    if (absv >= 1.0) { prefix = ""; scaled = value; }
    else if (absv >= 1e-3) { prefix = "m"; scaled = value * 1e3; }
    else if (absv >= 1e-6) { prefix = "µ"; scaled = value * 1e6; }
    else if (absv >= 1e-9) { prefix = "n"; scaled = value * 1e9; }
    else if (absv >= 1e-12) { prefix = "p"; scaled = value * 1e12; }
    else { prefix = "f"; scaled = value * 1e15; }
    snprintf(buffer, buf_size, "%.3f %sSv/s", scaled, prefix);
}

void format_sv_c(double sv, char *buffer, size_t buf_size) {
    if (isnan(sv)) { snprintf(buffer, buf_size, "NaN Sv"); return; }
    double absv = fabs(sv);
    const char *prefix = "";
    double scaled = sv;
    if (absv >= 1.0) { prefix = ""; scaled = sv; }
    else if (absv >= 1e-3) { prefix = "m"; scaled = sv * 1e3; }
    else if (absv >= 1e-6) { prefix = "µ"; scaled = sv * 1e6; }
    else if (absv >= 1e-9) { prefix = "n"; scaled = sv * 1e9; }
    else if (absv >= 1e-12) { prefix = "p"; scaled = sv * 1e12; }
    else { prefix = "f"; scaled = sv * 1e15; }
    snprintf(buffer, buf_size, "%.3f %sSv", scaled, prefix);
}

void rotate_buffer_c(double *buf, double val, int size) {
    memmove(buf, buf + 1, (size - 1) * sizeof(double));
    buf[size - 1] = val;
}

// --- V. V4L2 Capture (Minimal, auf SDL2 RGB32) ---

int v4l2_init_c(V4L2Context **ctx_out) {
    // ... (V4L2-Initialisierung wie in früheren Versionen) ...
    V4L2Context *ctx = (V4L2Context*)calloc(1, sizeof(V4L2Context));
    if (!ctx) return 0;
    ctx->fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
    if (ctx->fd < 0) { free(ctx); return 0; }
    struct v4l2_format fmt; memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W; fmt.fmt.pix.height = IMG_H; fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if (ioctl(ctx->fd, VIDIOC_S_FMT, &fmt) < 0) { close(ctx->fd); free(ctx); return 0; }
    struct v4l2_requestbuffers req; memset(&req, 0, sizeof(req));
    req.count = 2; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(ctx->fd, VIDIOC_REQBUFS, &req) < 0) { close(ctx->fd); free(ctx); return 0; }
    ctx->n_buffers = req.count;
    ctx->buffers = (V4L2Buffer*)calloc(ctx->n_buffers, sizeof(V4L2Buffer));
    for (unsigned int i = 0; i < ctx->n_buffers; ++i) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf)); 
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(ctx->fd, VIDIOC_QUERYBUF, &buf) < 0) { return 0; }
        ctx->buffers[i].length = buf.length;
        ctx->buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, buf.m.offset);
        if (ctx->buffers[i].start == MAP_FAILED) { return 0; }
        if (ioctl(ctx->fd, VIDIOC_QBUF, &buf) < 0) { return 0; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(ctx->fd, VIDIOC_STREAMON, &type) < 0) { return 0; }
    
    *ctx_out = ctx;
    return 1;
}

uint8_t *v4l2_grab_frame_c(V4L2Context *ctx) {
    if (!ctx || ctx->fd < 0) return NULL;
    fd_set fds; FD_ZERO(&fds); FD_SET(ctx->fd, &fds);
    struct timeval tv = {1, 0}; 
    int r = select(ctx->fd + 1, &fds, NULL, NULL, &tv);
    if (r <= 0) return NULL; 
    memset(&ctx->current_v4l2_buf, 0, sizeof(ctx->current_v4l2_buf));
    ctx->current_v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ctx->current_v4l2_buf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(ctx->fd, VIDIOC_DQBUF, &ctx->current_v4l2_buf) < 0) {
        if (errno != EAGAIN) { perror("VIDIOC_DQBUF error"); }
        return NULL;
    }
    return (uint8_t*)ctx->buffers[ctx->current_v4l2_buf.index].start;
}

void v4l2_unqueue_buffer_c(V4L2Context *ctx) { 
    if (ioctl(ctx->fd, VIDIOC_QBUF, &ctx->current_v4l2_buf) < 0) {
        perror("VIDIOC_QBUF error");
    }
}

void v4l2_cleanup_c(V4L2Context *ctx) {
    if (!ctx) return;
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

// YUYV -> RGB32 (SDL2 Format)
void yuyv_to_rgb32_c(const uint8_t *yuyv_data, uint32_t *dst_rgb32) {
    const uint8_t *src = yuyv_data;
    uint32_t *dst = dst_rgb32;

    for (int i = 0; i < IMG_W * IMG_H / 2; i++) {
        int Y1 = src[0]; int U = src[1] - 128;
        int Y2 = src[2]; int V = src[3] - 128;
        src += 4;

        int C1 = Y1 - 16;
        int R1 = (298 * C1 + 409 * V + 128) >> 8;
        int G1 = (298 * C1 - 100 * U - 208 * V + 128) >> 8;
        int B1 = (298 * C1 + 516 * U + 128) >> 8;
        
        uint8_t r1 = C_BOUND(0, R1, 255);
        uint8_t g1 = C_BOUND(0, G1, 255);
        uint8_t b1 = C_BOUND(0, B1, 255);
        dst[0] = C_QRGB(r1, g1, b1);

        int C2 = Y2 - 16;
        int R2 = (298 * C2 + 409 * V + 128) >> 8;
        int G2 = (298 * C2 - 100 * U - 208 * V + 128) >> 8;
        int B2 = (298 * C2 + 516 * U + 128) >> 8;
        
        uint8_t r2 = C_BOUND(0, R2, 255);
        uint8_t g2 = C_BOUND(0, G2, 255);
        uint8_t b2 = C_BOUND(0, B2, 255);
        dst[1] = C_QRGB(r2, g2, b2);
        
        dst += 2;
    }
}


// --- VI. GeigerLogic (Radionullon/Berlin6 Core) ---

void geiger_logic_init(GeigerLogicState *state) {
    int size = IMG_W * IMG_H;
    state->bg = (float*)calloc(size, sizeof(float));
    state->flicker = (uint8_t*)calloc(size, sizeof(uint8_t));
    state->longExp = (double*)calloc(size, sizeof(double));
    state->gray = (uint8_t*)malloc(size * sizeof(uint8_t));
    state->residue = (uint8_t*)malloc(size * sizeof(uint8_t));
    state->edge = (uint8_t*)malloc(size * sizeof(uint8_t));
    
    state->currentRadSmooth = 0.0;
    state->totalDoseAcc = 0.0;
    state->longTermCounter = 0;
    state->prevGlobalAvg = 128.0;
    state->berlin_peak_start_sec = -1.0;
    state->berlin_alert_active = false;
}

void geiger_logic_cleanup(GeigerLogicState *state) {
    free(state->bg);
    free(state->flicker);
    free(state->longExp);
    free(state->gray);
    free(state->residue);
    free(state->edge);
}

// Ersetzt Qt::qAbs, Qt::qSqrt
#define C_ABS(x) fabs(x)
#define C_SQRT(x) sqrt(x)

void geiger_logic_process(uint32_t *src_rgb32, uint32_t *dst_rgb32, GeigerLogicState *state, double frame_sec, GeigerStats *stats) {
    int size = IMG_W * IMG_H;
    int hotPixels = 0;
    
    // 1. Gray und Hot Pixels
    for (int i = 0; i < size; ++i) {
        uint32_t rgb = src_rgb32[i];
        uint8_t g = C_QGRAY(C_QRED(rgb), C_QGREEN(rgb), C_QBLUE(rgb));
        state->gray[i] = g;
        
        // Originalbild in das Ausgabebild kopieren (wird später überschrieben)
        dst_rgb32[i] = rgb;

        if (g > HOT_PIXEL_THRESH) hotPixels++;
    }

    // 2. Long exposure (EMA)
    for (int i = 0; i < size; ++i) {
        state->longExp[i] = (1.0 - LONG_EXP_LEARN) * state->longExp[i] + LONG_EXP_LEARN * (double)state->gray[i];
    }

    // 3. BG und Residue
    double globalAvg = 0.0;
    for (int i = 0; i < size; ++i) {
        state->bg[i] = (1.0 - BG_LEARN_RATE) * state->bg[i] + BG_LEARN_RATE * (double)state->gray[i];
        double combinedBg = 0.7 * state->bg[i] + 0.3 * state->longExp[i];
        int diff = abs((int)state->gray[i] - (int)round(combinedBg));
        state->residue[i] = (diff > RESIDUE_THRESH) ? (uint8_t)C_BOUND(0, diff, 255) : 0;
        globalAvg += (double)state->gray[i];
    }
    globalAvg /= (double)size;

    // 4. Flicker
    double currentAvg = C_BOUND(0.0, globalAvg, 255.0);
    int delta = (int)C_ABS(currentAvg - state->prevGlobalAvg);
    state->prevGlobalAvg = currentAvg;
    int boost = (delta > FLICKER_THRESH) ? 20 : 1;
    for (int i = 0; i < size; i++) {
        state->flicker[i] = (uint8_t)C_BOUND(0, (int)state->flicker[i] + boost, 255);
        if (state->flicker[i] > 0) state->flicker[i] = (uint8_t)C_BOUND(0, (int)state->flicker[i] - 1, 255);
    }

    // 5. Sobel Edges
    memset(state->edge, 0, size * sizeof(uint8_t));
    for (int y = 1; y < IMG_H-1; y++) {
        for (int x = 1; x < IMG_W-1; x++) {
            int i = y*IMG_W + x;
            int gx = -state->gray[i - IMG_W - 1] - 2*state->gray[i - 1] - state->gray[i + IMG_W - 1] +
                     state->gray[i - IMG_W + 1] + 2*state->gray[i + 1] + state->gray[i + IMG_W + 1];
            int gy = -state->gray[i - IMG_W - 1] - 2*state->gray[i - IMG_W] - state->gray[i - IMG_W + 1] +
                     state->gray[i + IMG_W - 1] + 2*state->gray[i + IMG_W] + state->gray[i + IMG_W + 1];
            int mag = (int)(EDGE_GAIN * C_SQRT((double)(gx*gx + gy*gy)));
            state->edge[i] = (uint8_t)C_BOUND(0, mag, 255);
        }
    }

    // 6. Heatmap Overlay
    for (int i = 0; i < size; ++i) {
        int score = (int)state->residue[i] + (int)(state->edge[i]/2) + (int)(state->flicker[i]/3);

        if (score < RADIONULLON_THRESH) {
            int le = C_BOUND(0, (int)round(state->longExp[i]), 255);
            int mixed = (int)((0.7 * (double)state->gray[i]) + (0.3 * (double)le));
            dst_rgb32[i] = C_QRGB(mixed, mixed, mixed); // Graustufen-Grau für Niedrig-Score
            continue;
        }

        int s = C_BOUND(0, score, 255);
        uint8_t r,g,b;
        if (s < 64) { b = 255; g = s*4; r = 0; }
        else if (s < 128) { b = 255 - (s-64)*4; g = 255; r = 0; }
        else if (s < 192) { b = 0; g = 255; r = (s-128)*4; }
        else { b = 0; g = 255 - (s-192)*4; r = 255; }

        uint8_t R0 = C_QRED(dst_rgb32[i]), G0 = C_QGREEN(dst_rgb32[i]), B0 = C_QBLUE(dst_rgb32[i]);
        int alpha = HEATMAP_ALPHA;
        uint8_t R_blend = (R0*(255-alpha) + r*alpha)/255;
        uint8_t G_blend = (G0*(255-alpha) + g*alpha)/255;
        uint8_t B_blend = (B0*(255-alpha) + b*alpha)/255;
        dst_rgb32[i] = C_QRGB(R_blend, G_blend, B_blend);
    }

    // 7. Geiger-Werte und Smoothing (Berlin6-Skalierung)
    double rawVal_per_h = ((double)hotPixels / GEIGER_NORM_FACTOR) * CIA_BERLIN6_SENS_SCALE;
    double rawVal = rawVal_per_h / 3600.0; // Sv/s

    state->currentRadSmooth = (state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);

    // 8. Dose-Akkumulation
    state->totalDoseAcc += state->currentRadSmooth * frame_sec; // Sv

    // 9. Histories
    rotate_buffer_c(stats->shortHistory, rawVal, GRAPH_POINTS);
    rotate_buffer_c(stats->avgHistory, state->currentRadSmooth, GRAPH_POINTS);
    state->longTermCounter++;
    if (state->longTermCounter >= 10) {
        rotate_buffer_c(stats->longHistory, state->currentRadSmooth, GRAPH_POINTS);
        state->longTermCounter = 0;
    }

    // 10. Berlin6 Alert Hysteresis
    double now_sec = get_current_time_sec();
    if (state->currentRadSmooth > CIA_BERLIN6_ALERT_THRESHOLD_S_PER_S) {
        if (state->berlin_peak_start_sec < 0.0) state->berlin_peak_start_sec = now_sec;
        if ((now_sec - state->berlin_peak_start_sec) >= CIA_BERLIN6_ALERT_CONFIRM_SEC) {
            state->berlin_alert_active = true;
        }
    } else {
        if (state->berlin_alert_active && (now_sec - state->berlin_peak_start_sec) < (CIA_BERLIN6_ALERT_CONFIRM_SEC + CIA_BERLIN6_ALERT_HOLD_SEC)) {
            // Keep alert briefly
        } else {
            state->berlin_alert_active = false;
            state->berlin_peak_start_sec = -1.0;
        }
    }

    // 11. Stats-Update (lokal)
    stats->currentRad = rawVal;
    stats->avgRad = state->currentRadSmooth;
    stats->totalDose = state->totalDoseAcc;
    stats->alertState = state->berlin_alert_active;
    stats->currentTimeMs = get_current_time_ms();
    
    // 12. Audio Request
    if (rawVal > 0.0) {
        if (SDL_LockMutex(g_mutex) == 0) {
            int next_tail = (g_queue_tail + 1) % CLICK_QUEUE_MAX;
            if (next_tail != g_queue_head) { // Queue isn't full
                g_click_queue[g_queue_tail].activity = rawVal;
                g_click_queue[g_queue_tail].timeMs = stats->currentTimeMs;
                g_queue_tail = next_tail;
                SDL_CondSignal(g_cond_audio);
            }
            SDL_UnlockMutex(g_mutex);
        }
    }
}

// 13. CSV Logging
void log_to_csv(const GeigerStats *stats) {
    static FILE *csv_file = NULL;
    static bool header_written = false;
    
    if (csv_file == NULL) {
        csv_file = fopen("berlin6_log.csv", "a");
        if (csv_file == NULL) {
            fprintf(stderr, "Konnte berlin6_log.csv nicht öffnen zum Schreiben.\n");
            return;
        }
    }
    
    if (!header_written) {
        fseek(csv_file, 0, SEEK_END);
        if (ftell(csv_file) == 0) {
            fprintf(csv_file, "ts_ms,raw_Sv_per_s,avg_Sv_per_s,total_Sv,alert\n");
        }
        header_written = true;
    }

    fprintf(csv_file, "%" PRIu64 ",%.8g,%.8g,%.12g,%d\n", 
            stats->currentTimeMs, 
            stats->currentRad, 
            stats->avgRad, 
            stats->totalDose, 
            stats->alertState ? 1 : 0);
    fflush(csv_file);
}

void csv_cleanup() {
    static FILE *csv_file = NULL;
    if (csv_file != NULL) {
        fclose(csv_file);
        csv_file = NULL;
    }
}

// --- VII. Audio Engine (ALSA Thread) ---

static int audio_thread(void *data) {
    (void)data;
    
    // Audio Parameter
    const double carrierA = 1400.0;
    const double carrierB = 3200.0;
    const double clickLenMs = 3.8;
    const double envTau = 0.0022;
    const double noiseLevel = 0.16;
    const double baseAmp = 0.95;

    double lpState = 0.0;
    double prevOut = 0.0;
    const double rcLp = 1.0 / (2.0 * M_PI * 9000.0);
    const double dt = 1.0 / AUDIO_SAMPLE_RATE;
    const double alphaLp = dt / (rcLp + dt);
    const double hpCoef = 0.995;
    
    typedef struct { int pos; int len; double ampVar; double pitchVar; int id; } ActiveClick;
    // C++ deque durch statisches Array ersetzt (vereinfacht)
    ActiveClick activeClicks[64]; 
    int num_active_clicks = 0;
    int nextClickId = 1;
    
    int16_t outBuf[AUDIO_PERIOD_FRAMES];
    int click_len_frames = (int)(clickLenMs * 0.001 * AUDIO_SAMPLE_RATE);
    
    while (g_running) {
        // 1. Warte auf Signal oder Timeout
        if (SDL_LockMutex(g_mutex) == 0) {
            if (g_queue_head == g_queue_tail) {
                SDL_CondWaitTimeout(g_cond_audio, g_mutex, 5); 
            }
            SDL_UnlockMutex(g_mutex);
        }

        // 2. Events aus der Queue verarbeiten
        if (SDL_LockMutex(g_mutex) == 0) {
            while (g_queue_head != g_queue_tail) {
                ClickEvent ev = g_click_queue[g_queue_head];
                g_queue_head = (g_queue_head + 1) % CLICK_QUEUE_MAX;
                
                // --- Berlin6 Click Mapping ---
                double activityPerHour = ev.activity * 3600.0;
                double baseClicks = 0.9 * activityPerHour + 0.5;
                double scaled = baseClicks * CIA_BERLIN6_CLICK_GAIN;
                double soft = sqrt(scaled);
                double clicksPerSecond_est = fmin(CIA_BERLIN6_CLICK_MAX, soft * soft);
                if (clicksPerSecond_est < 1.0) clicksPerSecond_est = 1.0;

                double expectation = clicksPerSecond_est * 0.02; // 20ms window
                int injectCount = (int)floor(expectation);
                double fractional = expectation - injectCount;
                if (((double)rand() / RAND_MAX) < fractional) injectCount++;
                if (injectCount < 1) injectCount = 1;

                for (int i = 0; i < injectCount; ++i) {
                    if (num_active_clicks < 64) {
                        ActiveClick *ac = &activeClicks[num_active_clicks++];
                        ac->pos = rand() % click_len_frames;
                        ac->len = click_len_frames;
                        uint32_t seed = (uint32_t)((ev.timeMs & 0xffffffff) ^ (nextClickId * 1664525u) ^ (i * 374761393u));
                        ac->ampVar = 0.80 + (double)(seed & 0xFF) / 255.0 * 0.6;
                        ac->pitchVar = 1.0 + ((((double)((seed >> 8) & 0xFF) / 255.0) - 0.5) * 0.06);
                        ac->id = nextClickId++;
                    }
                }
            }
            SDL_UnlockMutex(g_mutex);
        }

        // 3. Synthese des Blocks
        for (int n = 0; n < AUDIO_PERIOD_FRAMES; ++n) {
            double s = 0.0;
            
            for (int i = 0; i < num_active_clicks; ) {
                ActiveClick *ac = &activeClicks[i];
                if (ac->pos >= ac->len) {
                    // Ersetze gelöschten Click durch den letzten in der Liste
                    activeClicks[i] = activeClicks[--num_active_clicks]; 
                    continue;
                }
                
                double tt = (double)ac->pos / AUDIO_SAMPLE_RATE;
                double env = exp(-tt / envTau);
                env *= (1.0 - exp(-40.0 * tt));

                double c1 = sin(2.0 * M_PI * carrierA * ac->pitchVar * tt);
                double c2 = 0.55 * sin(2.0 * M_PI * carrierB * (1.0 + (ac->id & 7) * 0.0018) * tt);

                // Pseudozufallszahl (vereinfachter C-Ersatz für den C++-Operator)
                double pseudoNoise = (((rand() * 1103515245u + 12345u) >> 16) & 0x7FFF) / (double)0x7FFF * 2.0 - 1.0;
                s += baseAmp * ac->ampVar * env * (0.88 * c1 + 0.48 * c2 + noiseLevel * pseudoNoise);

                ac->pos++;
                i++;
            }

            double bg_noise = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * 0.0009;
            s += bg_noise;

            lpState = lpState + alphaLp * (s - lpState);
            double banded = lpState - hpCoef * prevOut;
            prevOut = lpState;
            double sample = banded;

            sample = C_BOUND(-1.0, sample, 1.0);
            outBuf[n] = (int16_t)(sample * 30000.0);
        }

        // 4. ALSA Schreiben
        int frames = AUDIO_PERIOD_FRAMES;
        int written = 0;
        while (written < frames) {
            int rc = snd_pcm_writei(g_alsa_pcm, outBuf + written, frames - written);
            if (rc == -EPIPE) {
                snd_pcm_prepare(g_alsa_pcm);
                continue;
            } else if (rc < 0) {
                fprintf(stderr, "ALSA write error: %s\n", snd_strerror(rc));
                break;
            } else {
                written += rc;
            }
        }
    }
    return 0;
}

int audio_init_c() {
    int err;
    if ((err = snd_pcm_open(&g_alsa_pcm, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "ALSA-Fehler: Kann PCM-Gerät nicht öffnen: %s\n", snd_strerror(err));
        return 0;
    }
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(g_alsa_pcm, params);
    
    snd_pcm_hw_params_set_access(g_alsa_pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(g_alsa_pcm, params, SND_PCM_FORMAT_S16_LE); 
    snd_pcm_hw_params_set_channels(g_alsa_pcm, params, 1); 
    unsigned int rate = AUDIO_SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(g_alsa_pcm, params, &rate, 0);
    snd_pcm_uframes_t frames = AUDIO_PERIOD_FRAMES;
    snd_pcm_hw_params_set_period_size_near(g_alsa_pcm, params, &frames, 0);
    
    if ((err = snd_pcm_hw_params(g_alsa_pcm, params)) < 0) {
        fprintf(stderr, "ALSA-Fehler: Kann Hardware-Parameter nicht setzen: %s\n", snd_strerror(err));
        snd_pcm_close(g_alsa_pcm);
        g_alsa_pcm = NULL;
        return 0;
    }
    snd_pcm_prepare(g_alsa_pcm);
    return 1;
}

void audio_cleanup_c() {
    if (g_alsa_pcm) {
        snd_pcm_drain(g_alsa_pcm);
        snd_pcm_close(g_alsa_pcm);
        g_alsa_pcm = NULL;
    }
}

// --- VIII. Worker Thread (V4L2 + Logic) ---

static int worker_thread(void *data) {
    (void)data;
    
    V4L2Context *v4l2_ctx = NULL;
    if (!v4l2_init_c(&v4l2_ctx)) {
        fprintf(stderr, "FATAL: V4L2 device %s failed.\n", DEVICE_NODE);
        g_running = false; return -1;
    }
    
    GeigerLogicState state;
    geiger_logic_init(&state);
    
    uint8_t *yuyv_data_buffer = (uint8_t*)malloc(IMG_W * IMG_H * 2);
    
    uint64_t last_frame_ms = get_current_time_ms();
    
    while (g_running) {
        uint8_t *yuyv_data = v4l2_grab_frame_c(v4l2_ctx);
        if (yuyv_data) {
            uint64_t now_ms = get_current_time_ms();
            double frame_sec = (double)(now_ms - last_frame_ms) / 1000.0;
            last_frame_ms = now_ms;
            
            // 1. Konvertierung YUYV -> RGB32 (Original-Puffer)
            yuyv_to_rgb32_c(yuyv_data, g_image_pixels_original);
            
            // 2. Logic & Processing. Ergebnisse in g_stats und g_image_pixels_processed
            geiger_logic_process(g_image_pixels_original, g_image_pixels_processed, &state, frame_sec, (GeigerStats*)&g_stats);

            // 3. CSV Logging
            log_to_csv(&g_stats);

            // 4. Buffer freigeben
            v4l2_unqueue_buffer_c(v4l2_ctx); 

            // 5. Signal an Main Thread (Frame bereit)
            if (SDL_LockMutex(g_mutex) == 0) {
                g_frame_ready = true;
                SDL_UnlockMutex(g_mutex);
            }
        } else {
            SDL_Delay(5); // Bei Fehler kurz warten
        }
    }
    
    geiger_logic_cleanup(&state);
    v4l2_cleanup_c(v4l2_ctx);
    free(yuyv_data_buffer);
    
    return 0;
}

// --- IX. SDL Rendering Helpers ---

SDL_Texture *render_text(const char *text, SDL_Color color, TTF_Font *font, SDL_Renderer *renderer) {
    if (!font || !renderer || !text) return NULL;
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return NULL;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void draw_graph_sdl(SDL_Renderer *renderer, const SDL_Rect *r, const double *data, int size, SDL_Color col, double alert_thresh) {
    if (size < 2) return;
    
    // Hintergrund
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
    SDL_RenderFillRect(renderer, r);
    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 160);
    SDL_RenderDrawRect(renderer, r);

    double maxVal = 0.001;
    for (int i = 0; i < size; ++i) maxVal = fmax(maxVal, data[i]);
    maxVal = fmax(maxVal, alert_thresh * 1.5); // Feste obere Skala für Konsistenz

    // Alert-Linie (Sv/s)
    double thresh_norm = fmin(alert_thresh / maxVal, 1.0);
    int thresh_y = r->y + r->h - (int)(thresh_norm * r->h);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
    SDL_RenderDrawLine(renderer, r->x, thresh_y, r->x + r->w, thresh_y);

    // Graph füllen (simplifizierter Ansatz ohne Path, nur Linien)
    double stepX = (double)r->w / (size - 1);
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    
    for (int i = 0; i < size - 1; ++i) {
        double valNorm1 = fmin(data[i] / maxVal, 1.0);
        double valNorm2 = fmin(data[i+1] / maxVal, 1.0);
        
        int x1 = r->x + (int)(i * stepX);
        int y1 = r->y + r->h - (int)(valNorm1 * r->h);
        int x2 = r->x + (int)((i + 1) * stepX);
        int y2 = r->y + r->h - (int)(valNorm2 * r->h);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void sdl_render_loop() {
    // Lokale Kopie der Stats, da sie shared sind
    GeigerStats stats_copy;
    bool new_frame_available = false;

    if (SDL_LockMutex(g_mutex) == 0) {
        if (g_frame_ready) {
            stats_copy = g_stats; // Atomarer Kopierversuch der ganzen Struktur
            g_frame_ready = false;
            new_frame_available = true;
        }
        SDL_UnlockMutex(g_mutex);
    }

    if (new_frame_available) {
        // Texturen aktualisieren
        SDL_UpdateTexture(g_texture_original, NULL, g_image_pixels_original, IMG_W * BPP);
        SDL_UpdateTexture(g_texture_processed, NULL, g_image_pixels_processed, IMG_W * BPP);
    }
    
    // --- Rendering ---
    
    int win_w, win_h;
    SDL_GetWindowSize(g_window, &win_w, &win_h);
    
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);

    int gap = 5;
    int panelW = (win_w - 3*gap) / 2;
    int panelH = win_h - 130; // Platz für Graphen und Text

    SDL_Rect leftImgRect = {gap, gap, panelW, panelH};
    SDL_Rect rightImgRect = {2*gap + panelW, gap, panelW, panelH};
    
    // Bilder rendern
    if (new_frame_available) {
        SDL_RenderCopy(g_renderer, g_texture_original, NULL, &leftImgRect);
        SDL_RenderCopy(g_renderer, g_texture_processed, NULL, &rightImgRect);
    }

    // --- HUD Text ---
    
    char buffer[64];
    SDL_Color color_white = {255, 255, 255, 255};
    SDL_Color color_green = {0, 255, 0, 255};
    SDL_Color color_red = {255, 0, 0, 255};

    int textX = 20;
    int textY = 28;
    
    // AKTUELL
    format_sv_per_s_c(stats_copy.currentRad, buffer, sizeof(buffer));
    char current_rad_text[128];
    snprintf(current_rad_text, sizeof(current_rad_text), "AKTUELL: %s", buffer);
    SDL_Texture *tex_curr = render_text(current_rad_text, stats_copy.alertState ? color_red : color_green, g_font_mono, g_renderer);
    if (tex_curr) {
        int w, h; SDL_QueryTexture(tex_curr, NULL, NULL, &w, &h);
        SDL_Rect dst_rect = {textX, textY, w, h};
        SDL_RenderCopy(g_renderer, tex_curr, NULL, &dst_rect);
        SDL_DestroyTexture(tex_curr);
    }
    
    // Ø MITTEL
    textY += 22;
    format_sv_per_s_c(stats_copy.avgRad, buffer, sizeof(buffer));
    char avg_rad_text[128];
    snprintf(avg_rad_text, sizeof(avg_rad_text), "Ø MITTEL: %s", buffer);
    SDL_Texture *tex_avg = render_text(avg_rad_text, color_white, g_font_small, g_renderer);
    if (tex_avg) {
        int w, h; SDL_QueryTexture(tex_avg, NULL, NULL, &w, &h);
        SDL_Rect dst_rect = {textX, textY, w, h};
        SDL_RenderCopy(g_renderer, tex_avg, NULL, &dst_rect);
        SDL_DestroyTexture(tex_avg);
    }

    // GESAMT DOSE
    textY += 20;
    format_sv_c(stats_copy.totalDose, buffer, sizeof(buffer));
    char total_dose_text[128];
    snprintf(total_dose_text, sizeof(total_dose_text), "GESAMT DOSE: %s", buffer);
    SDL_Texture *tex_dose = render_text(total_dose_text, color_white, g_font_small, g_renderer);
    if (tex_dose) {
        int w, h; SDL_QueryTexture(tex_dose, NULL, NULL, &w, &h);
        SDL_Rect dst_rect = {textX, textY, w, h};
        SDL_RenderCopy(g_renderer, tex_dose, NULL, &dst_rect);
        SDL_DestroyTexture(tex_dose);
    }

    // --- Graphen ---
    
    int graphH = 100;
    int margin = 10;
    int totalW = win_w - (4 * margin);
    int singleGraphW = (totalW - 20) / 3;
    int yPos = win_h - graphH - margin;

    SDL_Rect r1 = {margin, yPos, singleGraphW, graphH};
    SDL_Color col_long = {0, 100, 255, 200};
    draw_graph_sdl(g_renderer, &r1, stats_copy.longHistory, GRAPH_POINTS, col_long, CIA_BERLIN6_ALERT_THRESHOLD_S_PER_S);

    SDL_Rect r2 = {margin*2 + singleGraphW, yPos, singleGraphW, graphH};
    SDL_Color col_avg = {255, 200, 0, 200};
    draw_graph_sdl(g_renderer, &r2, stats_copy.avgHistory, GRAPH_POINTS, col_avg, CIA_BERLIN6_ALERT_THRESHOLD_S_PER_S);

    SDL_Rect r3 = {margin*3 + singleGraphW*2, yPos, singleGraphW, graphH};
    SDL_Color col_short = stats_copy.alertState ? (SDL_Color){255, 0, 0, 255} : (SDL_Color){0, 255, 0, 200};
    draw_graph_sdl(g_renderer, &r3, stats_copy.shortHistory, GRAPH_POINTS, col_short, CIA_BERLIN6_ALERT_THRESHOLD_S_PER_S);

    // Berlin6 visual alert (blinking red frame)
    if (stats_copy.alertState) {
        uint64_t ms = get_current_time_ms();
        uint8_t alpha = (uint8_t)(128 + 127 * sin((double)ms / 200.0));
        SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, alpha);
        SDL_Rect border = {0, 0, win_w, win_h};
        for (int i = 0; i < 6; ++i) { // 6 Pixel dicker Rahmen
            SDL_RenderDrawRect(g_renderer, &border);
            border.x += 1; border.y += 1; border.w -= 2; border.h -= 2;
        }
    }

    SDL_RenderPresent(g_renderer);
}

// --- X. Main Function ---

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    
    // Initialisierung
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL konnte nicht initialisiert werden: %s\n", SDL_GetError()); return 1;
    }
    if (TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf konnte nicht initialisiert werden: %s\n", TTF_GetError()); SDL_Quit(); return 1;
    }
    
    g_font_small = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 12);
    g_font_mono = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 18);
    if (!g_font_small || !g_font_mono) {
        fprintf(stderr, "WARN: Konnte Schriftart nicht laden. Text-Rendering wird fehlschlagen.\n");
    }

    g_window = SDL_CreateWindow("Radionullon Geiger (CIA Berlin 6 - C/SDL2)",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                IMG_W * 2 + 3 * 5, IMG_H + 130, // 2 Panels + UI Platz
                                SDL_WINDOW_RESIZABLE);
    if (!g_window) { fprintf(stderr, "Fenster konnte nicht erstellt werden: %s\n", SDL_GetError()); goto cleanup; }

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer) { fprintf(stderr, "Renderer konnte nicht erstellt werden: %s\n", SDL_GetError()); goto cleanup; }
    
    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    // Texturen
    g_texture_original = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, IMG_W, IMG_H);
    g_texture_processed = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, IMG_W, IMG_H);
    if (!g_texture_original || !g_texture_processed) { fprintf(stderr, "Texturen konnten nicht erstellt werden.\n"); goto cleanup; }

    // Puffer
    g_image_pixels_original = (uint32_t*)calloc(IMG_W * IMG_H, sizeof(uint32_t));
    g_image_pixels_processed = (uint32_t*)calloc(IMG_W * IMG_H, sizeof(uint32_t));
    if (!g_image_pixels_original || !g_image_pixels_processed) { fprintf(stderr, "Puffer konnten nicht alloziert werden.\n"); goto cleanup; }
    
    // Mutex/Condition Variable
    g_mutex = SDL_CreateMutex();
    g_cond_audio = SDL_CreateCond();
    if (!g_mutex || !g_cond_audio) { fprintf(stderr, "Mutex/Cond konnten nicht erstellt werden.\n"); goto cleanup; }

    // Audio
    if (!audio_init_c()) { fprintf(stderr, "WARN: ALSA Audio Init failed. No sound will be played.\n"); }

    // Threads starten
    SDL_Thread *worker_t = SDL_CreateThread(worker_thread, "RadionullonWorker", NULL);
    SDL_Thread *audio_t = NULL;
    if (g_alsa_pcm) {
        audio_t = SDL_CreateThread(audio_thread, "AudioEngine", NULL);
    }
    
    if (!worker_t) { fprintf(stderr, "Worker-Thread konnte nicht gestartet werden: %s\n", SDL_GetError()); g_running = false; }
    if (g_alsa_pcm && !audio_t) { fprintf(stderr, "Audio-Thread konnte nicht gestartet werden: %s\n", SDL_GetError()); }

    // Haupt-Loop
    SDL_Event event;
    while (g_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                g_running = false;
            }
        }
        
        sdl_render_loop();
        SDL_Delay(10); 
    }

cleanup:
    // Threads beenden
    if (audio_t) SDL_WaitThread(audio_t, NULL);
    if (worker_t) SDL_WaitThread(worker_t, NULL);

    // Aufräumen
    csv_cleanup();
    audio_cleanup_c();

    if (g_mutex) SDL_DestroyMutex(g_mutex);
    if (g_cond_audio) SDL_DestroyCond(g_cond_audio);

    if (g_image_pixels_original) free(g_image_pixels_original);
    if (g_image_pixels_processed) free(g_image_pixels_processed);
    
    if (g_texture_original) SDL_DestroyTexture(g_texture_original);
    if (g_texture_processed) SDL_DestroyTexture(g_texture_processed);
    if (g_renderer) SDL_DestroyRenderer(g_renderer);
    if (g_window) SDL_DestroyWindow(g_window);
    
    if (g_font_small) TTF_CloseFont(g_font_small);
    if (g_font_mono) TTF_CloseFont(g_font_mono);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
