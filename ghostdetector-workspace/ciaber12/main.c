#define _GNU_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// --- System- und Zeit-Header ---
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>       
#include <sys/select.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>

// V4L2-Header
#include <linux/videodev2.h> 

// --- X11 Includes ---
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// --- ALSA-Includes ---
#include <alsa/asoundlib.h> 

// ***************************************************************
// NEUE AUDIO-KONSTANTEN
// ***************************************************************
#define SAMPLE_RATE 44100
#define CLICK_DURATION 0.05 // 50ms
#define CLICK_SAMPLES (int)(SAMPLE_RATE * CLICK_DURATION)

// 16 Noten für 2 A-Moll-Tonleitern (A4 bis A6)
#define NUM_SCALE_NOTES 16 

// Der Start-Sound spielt nun die Tonleiter
#define NUM_STARTUP_CLICKS NUM_SCALE_NOTES 
#define NUM_CLICK_TYPES NUM_SCALE_NOTES 
#define BASE_FREQ_A4 440.0

// Semitone-Schritte für A-Moll (A4 bis A6)
const int SEMITONES[NUM_SCALE_NOTES] = {
    0, 2, 3, 5, 7, 8, 10, // A4, B4, C5, D5, E5, F5, G5
    12, // A5
    14, 15, 17, 19, 20, 22, // B5, C6, D6, E6, F6, G6
    24 // A6
};

// --- Audio Globale Variablen ---
snd_pcm_t* global_pcm_handle = NULL;
short click_buffers[NUM_SCALE_NOTES][CLICK_SAMPLES];

// --- V4L2/X11 Konfiguration ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define GRAPH_POINTS 100 
#define WINDOW_W (IMG_W * 2 + 30)
#define WINDOW_H (IMG_H + 150)
#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN 1.6f
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH 30
#define HOT_PIXEL_THRESH 200
#define GEIGER_SMOOTH_FACTOR 0.3
#define GEIGER_NORM_FACTOR 100.0
#define GEIGER_ALERT_THRESH 5.0

// --- Globale X11 Variablen (Prototypen) ---
Display *display; Window window; GC gc;
XImage *ximage_original; XImage *ximage_processed;

// --- Datenstrukturen und Globale Zustände (Prototypen) ---
struct Buffer { void *start; size_t length; };
typedef struct { double currentRad; double avgRad; double totalDose; int alertState;
    double shortHistory[GRAPH_POINTS]; double avgHistory[GRAPH_POINTS]; double longHistory[GRAPH_POINTS];
    long long startTimeSec; long long currentTimeSec;
} GeigerStats;
typedef struct { float *bg; uint8_t *flicker; float prevGlobalAvg;
    double currentRadSmooth; double totalDoseAcc; int longTermCounter;
    GeigerStats stats;
} GeigerLogicState;

GeigerLogicState logic_state;
uint32_t *original_frame_buffer;
uint32_t *processed_frame_buffer;
static int v4l2_fd = -1;
static struct Buffer *v4l2_buffers = NULL;
static unsigned int v4l2_n_buffers = 0;


// ***************************************************************
// ALSA / SOUND FUNKTIONEN
// ***************************************************************

void generate_click(double duration, short* buffer, double amplitude, double freq, double decay_rate) {
    int samples = duration * SAMPLE_RATE;
    for(int i = 0; i < samples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double decay = exp(-t * decay_rate); 
        buffer[i] = (short)(amplitude * sin(2.0 * M_PI * freq * t) * decay);
    }
}

void init_click_buffers() {
    srand(time(NULL));
    const double base_decay = 800.0; 

    printf("Synthetisiere %d Noten der A-Moll-Tonleiter (A4-A6) als Klick-Sounds...\n", NUM_SCALE_NOTES);

    for (int i = 0; i < NUM_SCALE_NOTES; i++) {
        double freq = BASE_FREQ_A4 * pow(2.0, (double)SEMITONES[i] / 12.0);
        double amplitude = 30000.0 - 500.0 * (i % 7); // Leichte Amplitudenvariation
        generate_click(CLICK_DURATION, click_buffers[i], amplitude, freq, base_decay);
    }
}

int init_alsa() {
    snd_pcm_hw_params_t* params; unsigned int rate = SAMPLE_RATE;
    if(snd_pcm_open(&global_pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "ALSA open error: Kann Audio-Gerät nicht öffnen.\n"); return -1;
    }
    snd_pcm_hw_params_alloca(&params); snd_pcm_hw_params_any(global_pcm_handle, params);
    snd_pcm_hw_params_set_access(global_pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(global_pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(global_pcm_handle, params, 1);
    snd_pcm_hw_params_set_rate_near(global_pcm_handle, params, &rate, NULL);
    snd_pcm_uframes_t frames = 512;
    snd_pcm_hw_params_set_period_size_near(global_pcm_handle, params, &frames, NULL);
    if(snd_pcm_hw_params(global_pcm_handle, params) < 0) {
        fprintf(stderr, "ALSA set params error.\n"); snd_pcm_close(global_pcm_handle);
        global_pcm_handle = NULL; return -1;
    }
    printf("ALSA initialisiert, Rate: %u Hz\n", rate); return 0;
}

void play_alsa_click(int click_index) {
    if (!global_pcm_handle || click_index < 0 || click_index >= NUM_SCALE_NOTES) return;
    if(snd_pcm_writei(global_pcm_handle, click_buffers[click_index], CLICK_SAMPLES) < 0) {
        snd_pcm_prepare(global_pcm_handle);
    }
}

void play_startup_clicks() {
    printf("Spiele zwei A-Moll-Tonleitern (%d Klicks) beim Start...\n", NUM_SCALE_NOTES);
    for (int i = 0; i < NUM_SCALE_NOTES; i++) {
        play_alsa_click(i); 
        usleep(100000); // 100ms Pause
    }
    snd_pcm_drain(global_pcm_handle);
}

void alsa_cleanup() {
    if (global_pcm_handle) {
        snd_pcm_close(global_pcm_handle);
        global_pcm_handle = NULL;
    }
}


// ***************************************************************
// V4L2 UND LOGIK FUNKTIONEN (Unverändert)
// ***************************************************************
static int xioctl(int fh, int request, void *arg) {
    int r; do { r = ioctl(fh, request, arg); } while (-1 == r && EINTR == errno);
    return r;
}
int v4l2_init() {
    struct v4l2_format fmt; struct v4l2_requestbuffers req; enum v4l2_buf_type type;
    v4l2_fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
    if (v4l2_fd == -1) { perror("Fehler bei DEVICE_NODE"); return -1; }
    memset(&fmt, 0, sizeof(fmt)); fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W; fmt.fmt.pix.height = IMG_H; fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if (xioctl(v4l2_fd, VIDIOC_S_FMT, &fmt) == -1) { perror("VIDIOC_S_FMT"); return -1; }
    memset(&req, 0, sizeof(req)); req.count = 2; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(v4l2_fd, VIDIOC_REQBUFS, &req) == -1) { perror("VIDIOC_REQBUFS"); return -1; }
    v4l2_buffers = calloc(req.count, sizeof(*v4l2_buffers));
    for (v4l2_n_buffers = 0; v4l2_n_buffers < req.count; ++v4l2_n_buffers) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = v4l2_n_buffers;
        if (xioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf) == -1) { perror("VIDIOC_QUERYBUF"); return -1; }
        v4l2_buffers[v4l2_n_buffers].length = buf.length;
        v4l2_buffers[v4l2_n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);
        if (MAP_FAILED == v4l2_buffers[v4l2_n_buffers].start) { perror("mmap"); return -1; }
    }
    for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (xioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1) { perror("VIDIOC_QBUF"); return -1; }
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(v4l2_fd, VIDIOC_STREAMON, &type) == -1) { perror("VIDIOC_STREAMON"); return -1; }
    return 0;
}
void yuyv_to_rgb32(const uint8_t *src, uint32_t *dst, int width, int height) {
    for (int y = 0; y < height; y++) {
        const uint8_t *srcLine = src + (y * width * 2); uint32_t *dstLine = dst + (y * width);
        for (int x = 0; x < width; x += 2) {
            int Y1 = srcLine[0]; int U = srcLine[1]; int Y2 = srcLine[2]; int V = srcLine[3]; srcLine += 4;
            int C = Y1 - 16; int D = U - 128; int E = V - 128;
            int R = (298 * C + 409 * E + 128) >> 8; int G = (298 * C - 100 * D - 208 * E + 128) >> 8; int B = (298 * C + 516 * D + 128) >> 8;
            dstLine[0] = (uint32_t)( ((R>255?255:(R<0?0:R)) & 0xFF) << 16 | ((G>255?255:(G<0?0:G)) & 0xFF) << 8 | ((B>255?255:(B<0?0:B)) & 0xFF) );
            C = Y2 - 16;
            R = (298 * C + 409 * E + 128) >> 8; G = (298 * C - 100 * D - 208 * E + 128) >> 8; B = (298 * C + 516 * D + 128) >> 8;
            dstLine[1] = (uint32_t)( ((R>255?255:(R<0?0:R)) & 0xFF) << 16 | ((G>255?255:(G<0?0:G)) & 0xFF) << 8 | ((B>255?255:(B<0?0:B)) & 0xFF) );
            dstLine += 2;
        }
    }
}
int v4l2_read_frame(uint32_t *rgb32_buffer) {
    struct v4l2_buffer buf; 
    memset(&buf, 0, sizeof(buf)); 
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(v4l2_fd, VIDIOC_DQBUF, &buf) == -1) {
        if (errno == EAGAIN) return 1;
        perror("VIDIOC_DQBUF"); return -1;
    }
    uint8_t *src = (uint8_t *)v4l2_buffers[buf.index].start;
    yuyv_to_rgb32(src, rgb32_buffer, IMG_W, IMG_H);
    if (xioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1) {
        perror("VIDIOC_QBUF"); return -1;
    }
    return 0;
}
void v4l2_close() {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE; xioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
        if (v4l2_buffers[i].start) { munmap(v4l2_buffers[i].start, v4l2_buffers[i].length); }
    }
    free(v4l2_buffers);
    if (v4l2_fd != -1) close(v4l2_fd);
}
static inline uint8_t clamp_u8(int val) {
    if (val < 0) return 0; 
    if (val > 255) return 255; 
    return (uint8_t)val;
}
static inline uint8_t rgb32_to_grayscale(uint32_t rgb) {
    uint8_t r = (uint8_t)((rgb >> 16) & 0xFF); uint8_t g = (uint8_t)((rgb >> 8) & 0xFF); uint8_t b = (uint8_t)(rgb & 0xFF);
    return (uint8_t)((299 * r + 587 * g + 114 * b) / 1000);
}
static inline void rotate_buffer(double *buf, int size, double val) {
    for (int i = 0; i < size - 1; ++i) { buf[i] = buf[i + 1]; } buf[size - 1] = val;
}
void init_geiger_logic(GeigerLogicState *state) {
    int size = IMG_W * IMG_H;
    state->bg = (float *)calloc(size, sizeof(float)); state->flicker = (uint8_t *)calloc(size, sizeof(uint8_t));
    state->prevGlobalAvg = 128.0f; state->currentRadSmooth = 0.0; state->totalDoseAcc = 0.0; state->longTermCounter = 0;
    memset(state->stats.shortHistory, 0, sizeof(state->stats.shortHistory));
    memset(state->stats.avgHistory, 0, sizeof(state->stats.avgHistory));
    memset(state->stats.longHistory, 0, sizeof(state->stats.longHistory));
    struct timeval tv; gettimeofday(&tv, NULL); state->stats.startTimeSec = tv.tv_sec;
}
void cleanup_geiger_logic(GeigerLogicState *state) {
    free(state->bg); free(state->flicker);
}
void process_radionullon(GeigerLogicState *state, const uint32_t *img_in, uint32_t *img_out) {
    int size = IMG_W * IMG_H; 
    uint8_t *gray = (uint8_t *)malloc(size); 
    uint8_t *residue = (uint8_t *)malloc(size); 
    uint8_t *edge = (uint8_t *)malloc(size);
    if (!gray || !residue || !edge) return;
    memset(edge, 0, size); memcpy(img_out, img_in, size * sizeof(uint32_t));
    int hotPixels = 0; float globalAvg = 0.0f;
    for (int i = 0; i < size; ++i) { uint8_t g = rgb32_to_grayscale(img_in[i]); gray[i] = g; if (g > HOT_PIXEL_THRESH) { hotPixels++; } }
    for (int i = 0; i < size; i++) { state->bg[i] = (1.0f - BG_LEARN_RATE) * state->bg[i] + BG_LEARN_RATE * (float)gray[i];
        int diff = (int)fabsf((float)gray[i] - state->bg[i]); residue[i] = (uint8_t)((diff > RESIDUE_THRESH) ? (diff > 255 ? 255 : diff) : 0); globalAvg += (float)gray[i]; }
    globalAvg /= size;
    uint8_t currentAvg = clamp_u8((int)globalAvg); int delta = (int)fabsf((float)currentAvg - state->prevGlobalAvg); state->prevGlobalAvg = currentAvg;
    uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
    for (int i = 0; i < size; i++) { state->flicker[i] = clamp_u8((int)state->flicker[i] + boost); if (state->flicker[i] > 0) state->flicker[i] = (uint8_t)(state->flicker[i] - 1); }
    for (int y = 1; y < IMG_H - 1; y++) { for (int x = 1; x < IMG_W - 1; x++) { int i = y * IMG_W + x;
            int gx = -gray[i - IMG_W - 1] - 2 * gray[i - 1] - gray[i + IMG_W - 1] + gray[i - IMG_W + 1] + 2 * gray[i + 1] + gray[i + IMG_W + 1];
            int gy = -gray[i - IMG_W - 1] - 2 * gray[i - IMG_W] - gray[i - IMG_W + 1] + gray[i + IMG_W - 1] + 2 * gray[i + IMG_W] + gray[i + IMG_W + 1];
            int mag = (int)(EDGE_GAIN * sqrtf((float)(gx * gx + gy * gy))); edge[i] = clamp_u8(mag); } }
    for (int i = 0; i < size; ++i) {
        int score = (int)residue[i] + (int)(edge[i] / 2) + (int)(state->flicker[i] / 3);
        if (score < RADIONULLON_THRESH) { uint8_t g = gray[i]; img_out[i] = (uint32_t)(g << 16 | g << 8 | g); continue; }
        int s = (score > 255) ? 255 : score; uint8_t r = 0, g = 0, b = 0;
        if (s < 64) { b = 255; g = s * 4; r = 0; } else if (s < 128) { b = 255 - (s - 64) * 4; g = 255; r = 0; }
        else if (s < 192) { b = 0; g = 255; r = (s - 128) * 4; } else { b = 0; g = 255 - (s - 192) * 4; r = 255; }
        uint32_t original_rgb = img_in[i]; uint8_t R0 = (uint8_t)((original_rgb >> 16) & 0xFF); uint8_t G0 = (uint8_t)((original_rgb >> 8) & 0xFF); uint8_t B0 = (uint8_t)(original_rgb & 0xFF);
        int alpha = HEATMAP_ALPHA;
        uint8_t R_blend = (uint8_t)((R0 * (255 - alpha) + r * alpha) / 255);
        uint8_t G_blend = (uint8_t)((G0 * (255 - alpha) + g * alpha) / 255);
        uint8_t B_blend = (uint8_t)((B0 * (255 - alpha) + b * alpha) / 255);
        img_out[i] = (uint32_t)(R_blend << 16 | G_blend << 8 | B_blend);
    }
    double rawVal = (double)hotPixels / GEIGER_NORM_FACTOR;
    state->currentRadSmooth = (state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);
    state->totalDoseAcc += state->currentRadSmooth * 0.001;
    rotate_buffer(state->stats.shortHistory, GRAPH_POINTS, rawVal);
    rotate_buffer(state->stats.avgHistory, GRAPH_POINTS, state->currentRadSmooth);
    state->longTermCounter++;
    if (state->longTermCounter >= 10) { rotate_buffer(state->stats.longHistory, GRAPH_POINTS, state->currentRadSmooth); state->longTermCounter = 0; }
    struct timeval tv; gettimeofday(&tv, NULL); state->stats.currentTimeSec = tv.tv_sec;
    state->stats.currentRad = rawVal; state->stats.avgRad = state->currentRadSmooth; state->stats.totalDose = state->totalDoseAcc;
    state->stats.alertState = state->currentRadSmooth > GEIGER_ALERT_THRESH;
    free(gray); free(residue); free(edge);
}

// ***************************************************************
// X11 FUNKTIONEN (Unverändert)
// ***************************************************************

int x11_init() {
    int screen;
    size_t buffer_size = IMG_W * IMG_H * sizeof(uint32_t);
    original_frame_buffer = (uint32_t *)malloc(buffer_size);
    processed_frame_buffer = (uint32_t *)malloc(buffer_size);
    if (!original_frame_buffer || !processed_frame_buffer) { fprintf(stderr, "Fehler: Speicherzuweisung fehlgeschlagen.\n"); return -1; }
    if (!(display = XOpenDisplay(NULL))) { fprintf(stderr, "Fehler: Kann keine Verbindung zum X-Server herstellen.\n"); return -1; }
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, WINDOW_W, WINDOW_H, 1, BlackPixel(display, screen), BlackPixel(display, screen)); 
    XStoreName(display, window, "qgeister1 (X11/V4L2) | Original vs. Heatmap");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display, window);
    gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, WhitePixel(display, screen)); 
    ximage_original = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, (char*)original_frame_buffer, IMG_W, IMG_H, 32, 0);
    ximage_processed = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, (char*)processed_frame_buffer, IMG_W, IMG_H, 32, 0);
    if (!ximage_original || !ximage_processed) { fprintf(stderr, "Fehler: XCreateImage.\n"); return -1; }
    return 0;
}
void x11_cleanup() {
    if (display) {
        if (ximage_original) ximage_original->data = NULL;
        if (ximage_processed) ximage_processed->data = NULL;
        XDestroyImage(ximage_original);
        XDestroyImage(ximage_processed);
        XFreeGC(display, gc);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }
    free(original_frame_buffer);
    free(processed_frame_buffer);
}
void sigint_handler(int sig) {
    printf("\nBeende Anwendung...\n"); alsa_cleanup(); v4l2_close(); x11_cleanup(); cleanup_geiger_logic(&logic_state); exit(0);
}


// ***************************************************************
// MAIN FUNKTION
// ***************************************************************

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);

    // 1. Initialisierung Audio-Puffer (Tonleiter-Klicks)
    init_click_buffers();

    // 2. Initialisierung Geiger/V4L2/X11
    init_geiger_logic(&logic_state);
    if (v4l2_init() != 0) { cleanup_geiger_logic(&logic_state); return 1; }
    if (x11_init() != 0) { v4l2_close(); cleanup_geiger_logic(&logic_state); return 1; }
    
    // 3. Initialisierung ALSA und Start-Tonleiter
    if (init_alsa() != 0) {
        fprintf(stderr, "Warnung: Audio-Wiedergabe nicht möglich. Fortfahren ohne Ton.\n");
    } else {
        play_startup_clicks();
    }
    
    int running = 1;
    XEvent event;
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    const int gap = 10;
    const int video_x_start = gap;
    const int video_y_start = gap;
    
    // Hauptloop: Verarbeitet Events und Frames
    while (running) {
        fd_set fds; FD_ZERO(&fds); FD_SET(v4l2_fd, &fds);
        struct timeval tv = { 0, 33000 }; 
        int r = select(v4l2_fd + 1, &fds, NULL, NULL, &tv);

        if (r > 0 && FD_ISSET(v4l2_fd, &fds)) {
            if (v4l2_read_frame(original_frame_buffer) == 0) {
                
                process_radionullon(&logic_state, original_frame_buffer, processed_frame_buffer);

                // Klick-Sound bei ausreichend hohem Zählwert abspielen (verwendet den ersten Puffer/Note A4)
                if (logic_state.stats.currentRad > 0.5 && global_pcm_handle != NULL) {
                    play_alsa_click(0); // Spielt den A4-Klick für Geiger-Ereignisse
                }
                
                // Grafik aktualisieren
                XPutImage(display, window, gc, ximage_original, 0, 0, video_x_start, video_y_start, IMG_W, IMG_H);
                XPutImage(display, window, gc, ximage_processed, 0, 0, video_x_start + IMG_W + gap, video_y_start, IMG_W, IMG_H);
                
                char stats_text[128];
                sprintf(stats_text, "AKTUELL: %.2f Sv/h | Ø MITTEL: %.2f Sv/h", logic_state.stats.currentRad, logic_state.stats.avgRad);
                
                XClearArea(display, window, video_x_start, video_y_start + IMG_H + 10, WINDOW_W - 2 * gap, WINDOW_H - IMG_H - 10, False);
                XDrawString(display, window, gc, video_x_start, video_y_start + IMG_H + 30, stats_text, strlen(stats_text));
                XFlush(display);
            }
        } else if (r == -1) {
            perror("select"); break;
        }

        // X11-Events verarbeiten
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == Expose) {
                XPutImage(display, window, gc, ximage_original, 0, 0, video_x_start, video_y_start, IMG_W, IMG_H);
                XPutImage(display, window, gc, ximage_processed, 0, 0, video_x_start + IMG_W + gap, video_y_start, IMG_W, IMG_H);
                char stats_text[128];
                sprintf(stats_text, "AKTUELL: %.2f Sv/h | Ø MITTEL: %.2f Sv/h", logic_state.stats.currentRad, logic_state.stats.avgRad);
                XClearArea(display, window, video_x_start, video_y_start + IMG_H + 10, WINDOW_W - 2 * gap, WINDOW_H - IMG_H - 10, False);
                XDrawString(display, window, gc, video_x_start, video_y_start + IMG_H + 30, stats_text, strlen(stats_text));
            } else if (event.type == KeyPress) {
                char buffer[1]; KeySym key; XLookupString(&event.xkey, buffer, sizeof(buffer), &key, NULL);
                if (key == XK_Escape) running = 0;
            } else if (event.type == ClientMessage) {
                if ((Atom)event.xclient.data.l[0] == wm_delete_window) running = 0;
            }
        }
    }

    // Sauberes Beenden
    alsa_cleanup(); v4l2_close(); x11_cleanup(); cleanup_geiger_logic(&logic_state);
    return 0;
}

