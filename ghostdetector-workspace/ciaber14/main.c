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

// ALSA Header (wird für Audio-Playback benötigt)
#include <alsa/asoundlib.h> 

// V4L2-Header
#include <linux/videodev2.h> 

// --- X11 Includes ---
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
// Xft Header für TrueType Schriftarten
#include <X11/Xft/Xft.h> 

// ***************************************************************
// KONSTANTEN FÜR DOSIMETRIE & KONVERTIERUNG
// ***************************************************************
#define SVH_TO_USVS 277.777777778 // (1 Sv/h) / 3600 * 10^6
#define SVH_TO_USVH 1000000.0

// ***************************************************************
// AUDIO KONSTANTEN (FM-Synthese)
// ***************************************************************
#define SAMPLE_RATE 44100
#define CLICK_DURATION 0.05 
#define CLICK_SAMPLES (int)(SAMPLE_RATE * CLICK_DURATION)
#define NUM_SCALE_NOTES 16 
#define BASE_FREQ_A4 440.0
const int SEMITONES[NUM_SCALE_NOTES] = {
    0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24 
};
// FM-Synthese Parameter
#define FM_CARRIER_MODULATOR_RATIO 1.414 
#define FM_MODULATION_INDEX_BASE 10.0    
#define FM_DECAY_RATE 1200.0             

// Fusionsfaktoren (97% Video / 3% Audio)
#define FUSION_WEIGHT_VIDEO 0.97
#define FUSION_WEIGHT_AUDIO 0.03 

// ***************************************************************
// LOGIK-DEFINITIONEN
// ***************************************************************
#define NUM_ISOTOPES 16 
#define MAX_DETECTIONS_PER_FRAME 200 

typedef struct {
    const char *name;
    double T_HALF_MS;      
    double ENERGY_MEV;     
    double RBE_FACTOR;     
    double ACTIVITY_CPS;   
} Isotope;

static const Isotope ISOTOPES[NUM_ISOTOPES] = {
    {"Pu-239 (Plutonium)", 7.59e+15, 5.15, 20.0, 0.001}, 
    {"Am-241 (Americium)", 1.38e+13, 5.48, 20.0, 0.005}, 
    {"Rn-222 (Radon)", 3.29e+5, 5.49, 20.0, 0.005},      
    {"Ra-226 (Radium)", 5.06e+10, 4.78, 20.0, 0.008},    
    {"Cs-137 (Caesium)", 9.48e+11, 0.662, 1.0, 0.2},   
    {"Co-60 (Kobalt)", 1.66e+11, 1.25, 1.0, 0.1},      
    {"I-131 (Iod)", 6.95e+8, 0.364, 1.0, 0.05},        
    {"Sr-90 (Strontium)", 8.78e+11, 0.546, 1.0, 0.15}, 
    {"U-238 (Uran)", 1.41e+17, 4.20, 20.0, 0.0001},     
    {"Tc-99 (Technetium)", 1.33e+14, 0.294, 1.0, 0.002}, 
    {"C-14 (Kohlenstoff)", 1.794e+14, 0.0495, 1.0, 5.0e-4}, 
    {"K-40 (Kalium)", 4.053e+19, 1.460, 1.0, 2.0e-2},    
    {"Kr-85 (Krypton)", 3.32e+10, 0.514, 1.0, 0.01},    
    {"Xe-133 (Xenon)", 4.54e+7, 0.081, 1.0, 0.05},      
    {"Au-198 (Gold)", 2.30e+5, 0.96, 1.0, 0.08},        
    {"H-3 (Tritium)", 3.892e+11, 0.0057, 1.0, 1.0e-3}   
};

// --- V4L2/X11 Konfiguration ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640 
#define IMG_H 480 
#define GRAPH_POINTS 100 

#define INITIAL_GAP 10
#define ISOTOPE_LIST_WIDTH 250 
#define INITIAL_WINDOW_W (IMG_W * 2 + 4 * INITIAL_GAP + ISOTOPE_LIST_WIDTH)
#define INITIAL_WINDOW_H (IMG_H + 5 * INITIAL_GAP + 70) 

// --- Radionullon-Tunables ---
#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN 1.6f
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH 30
#define HOT_PIXEL_THRESH 200 // Threshold für die Zählung von Events/Hotspots

#define GEIGER_SMOOTH_FACTOR 0.3 // EMA Glättungsfaktor
#define GEIGER_ALERT_THRESH 5.0 

// ***************************************************************
// GLOBALE VARIABLEN UND STRUKTUREN
// ***************************************************************

// Audio-Playback
snd_pcm_t* global_pcm_handle = NULL; 
short click_buffers[NUM_SCALE_NOTES][CLICK_SAMPLES];

// V4L2/X11
Display *display; Window window; GC gc;
XImage *ximage_original; XImage *ximage_processed;
static int current_win_w = 0;
static int current_win_h = 0;
struct Buffer { void *start; size_t length; };

// Xft-Variablen
XftDraw *global_xft_draw = NULL;
XftFont *global_xft_font = NULL;
XftColor global_xft_color_white; 
// NEUE OPTIMIERTE SCHRIFTDEFINITION: AA=true, Hinting=true
// Die Schriftfamilie 'monospace' wird gewählt, da sie für Daten am besten geeignet ist.
const char *FONT_NAME = "monospace:pixelsize=15:antialias=true:hinting=true";

// Zeit-Tracking für genaue Dosisberechnung
double last_frame_time_sec = 0.0;

typedef struct { 
    double currentRad; 
    double avgRad;     
    double totalDoseSv; 
    double currentAudioUsvS; 
    double currentFusedUsvS; 
    int alertState;
    double shortHistory[GRAPH_POINTS]; 
    double avgHistory[GRAPH_POINTS]; 
    double longHistory[GRAPH_POINTS];
    long long startTimeSec; 
} GeigerStats;

typedef struct { 
    float *bg; 
    uint8_t *flicker; 
    float prevGlobalAvg;
    double currentRadSmooth; 
    double totalDoseAcc; 
    int longTermCounter;
    
    long long isotope_count[NUM_ISOTOPES];
    double isotope_total_dose_usvh[NUM_ISOTOPES]; 
    double total_usv_h;                          
    
    GeigerStats stats;
} GeigerLogicState;

GeigerLogicState logic_state;
uint32_t *original_frame_buffer;
uint32_t *processed_frame_buffer;
static int v4l2_fd = -1;
static struct Buffer *v4l2_buffers = NULL;
static unsigned int v4l2_n_buffers = 0;

// ***************************************************************
// HILFSFUNKTIONEN (FARBE, MATH, etc.)
// ***************************************************************
void hsv_to_rgb(float H, float S, float V, float *R, float *G, float *B) {
    if (S == 0.0f) { *R = *G = *B = V; return; }
    float h = H / 60.0f; int i = (int)floorf(h); float f = h - i; 
    float p = V * (1.0f - S); float q = V * (1.0f - S * f);
    float t = V * (1.0f - S * (1.0f - f));
    switch (i % 6) {
        case 0: *R = V; *G = t; *B = p; break; case 1: *R = q; *G = V; *B = p; break;
        case 2: *R = p; *G = V; *B = t; break; case 3: *R = p; *G = q; *B = V; break;
        case 4: *R = t; *G = p; *B = V; break; case 5: *R = V; *G = p; *B = q; break;
    }
}
static inline uint8_t rgb32_to_grayscale(uint32_t rgb) {
    uint8_t r = (uint8_t)((rgb >> 16) & 0xFF); uint8_t g = (uint8_t)((rgb >> 8) & 0xFF); uint8_t b = (uint8_t)(rgb & 0xFF);
    return (uint8_t)((299 * r + 587 * g + 114 * b) / 1000);
}
void init_geiger_logic(GeigerLogicState *state) {
    int size = IMG_W * IMG_H;
    state->bg = (float *)calloc(size, sizeof(float)); 
    state->flicker = (uint8_t *)calloc(size, sizeof(uint8_t));
    state->prevGlobalAvg = 128.0f; state->currentRadSmooth = 0.0; state->totalDoseAcc = 0.0; state->longTermCounter = 0;
    
    memset(state->isotope_count, 0, sizeof(state->isotope_count));
    memset(state->isotope_total_dose_usvh, 0, sizeof(state->isotope_total_dose_usvh));
    state->total_usv_h = 0.0;

    memset(state->stats.shortHistory, 0, sizeof(state->stats.shortHistory));
    memset(state->stats.avgHistory, 0, sizeof(state->stats.avgHistory));
    memset(state->stats.longHistory, 0, sizeof(state->stats.longHistory));
    state->stats.totalDoseSv = 0.0; 
    
    struct timeval tv; gettimeofday(&tv, NULL); state->stats.startTimeSec = tv.tv_sec;
}
void cleanup_geiger_logic(GeigerLogicState *state) {
    free(state->bg); free(state->flicker);
}

// ***************************************************************
// GEIGER-LOGIK (ISOTOPE)
// ***************************************************************
static void poisson_decay_simulation(int event_count, long long *isotope_count) {
    double total_activity = 0.0;
    for (int i = 0; i < NUM_ISOTOPES; ++i) { total_activity += ISOTOPES[i].ACTIVITY_CPS; }
    if (total_activity == 0.0) return;
    for (int i = 0; i < NUM_ISOTOPES; ++i) { 
        double relative_activity = ISOTOPES[i].ACTIVITY_CPS / total_activity;
        int decays = (int)round(event_count * relative_activity);
        if (decays > 0) { decays += (rand() % 3) - 1; if (decays < 0) decays = 0; }
        isotope_count[i] += decays;
    }
}

static double update_isotopic_dose(GeigerLogicState *state, double frame_delta_sec) {
    double total_usv_h = 0.0;
    for (int i = 0; i < NUM_ISOTOPES; ++i) { 
        const Isotope *iso = &ISOTOPES[i];
        // Skaliert die gezählten Events auf eine Rate, basierend auf der Zeit seit dem letzten Frame
        double current_cps = (frame_delta_sec > 0.0) ? (double)state->isotope_count[i] / frame_delta_sec : 0.0; 
        
        // Berechnung Sv/h Rate
        double sv_h_rate = current_cps * iso->ENERGY_MEV * iso->RBE_FACTOR * 1.0e-5;
        
        state->isotope_total_dose_usvh[i] = sv_h_rate * SVH_TO_USVH;
        total_usv_h += state->isotope_total_dose_usvh[i];
        state->isotope_count[i] = 0; // Reset für den nächsten Frame
    }
    
    state->total_usv_h = total_usv_h;
    
    // Totaldosis-Akkumulation mit tatsächlicher Frame-Zeit
    double frame_time_h = frame_delta_sec / 3600.0; // tatsächliche Frame-Zeit in Stunden
    state->stats.totalDoseSv += (total_usv_h / SVH_TO_USVH) * frame_time_h;
    
    return total_usv_h;
}


// ***************************************************************
// V4L2 LOGIK
// ***************************************************************
static int xioctl(int fh, int request, void *arg) {
    int r; do { r = ioctl(fh, request, arg); } while (-1 == r && EINTR == errno); return r;
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
int v4l2_read_frame(uint32_t *rgb32_buffer) {
    struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(v4l2_fd, VIDIOC_DQBUF, &buf) == -1) {
        if (errno == EAGAIN) return 1; perror("VIDIOC_DQBUF"); return -1;
    }
    uint8_t *src = (uint8_t *)v4l2_buffers[buf.index].start;
    yuyv_to_rgb32(src, rgb32_buffer, IMG_W, IMG_H);
    if (xioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1) { perror("VIDIOC_QBUF"); return -1; }
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


// ***************************************************************
// ALSA LOGIK (Playback FM-Synthese)
// ***************************************************************
void generate_fm_click(double duration, short* buffer, double amplitude, double carrier_freq, double decay_rate) {
    int samples = duration * SAMPLE_RATE;
    double fm_ratio = FM_CARRIER_MODULATOR_RATIO;
    double modulator_freq = carrier_freq * fm_ratio;
    double modulation_index = FM_MODULATION_INDEX_BASE / pow(carrier_freq / BASE_FREQ_A4, 0.2); 
    if (modulation_index < 0.5) modulation_index = 0.5;

    for(int i = 0; i < samples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double decay = exp(-t * decay_rate); 
        double phase_modulation = modulation_index * sin(2.0 * M_PI * modulator_freq * t);
        double signal = sin(2.0 * M_PI * carrier_freq * t + phase_modulation);
        buffer[i] = (short)(amplitude * signal * decay);
    }
}
void init_click_buffers() {
    srand(time(NULL)); 
    for (int i = 0; i < NUM_SCALE_NOTES; i++) {
        double carrier_freq = BASE_FREQ_A4 * pow(2.0, (double)SEMITONES[i] / 12.0);
        double amplitude = 30000.0 - 500.0 * (i % 7); 
        generate_fm_click(CLICK_DURATION, click_buffers[i], amplitude, carrier_freq, FM_DECAY_RATE);
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
        fprintf(stderr, "ALSA set params error.\n"); snd_pcm_close(global_pcm_handle); global_pcm_handle = NULL; return -1;
    }
    printf("ALSA Playback initialisiert.\n"); return 0;
}
void play_alsa_click(int click_index) {
    if (!global_pcm_handle || click_index < 0 || click_index >= NUM_SCALE_NOTES) return;
    if(snd_pcm_writei(global_pcm_handle, click_buffers[click_index], CLICK_SAMPLES) < 0) {
        snd_pcm_prepare(global_pcm_handle); 
    }
}
void play_startup_clicks() {
    for (int i = 0; i < NUM_SCALE_NOTES; i++) {
        play_alsa_click(i); 
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
// X11/XFT LOGIK
// ***************************************************************
void xft_draw_text(const char *text, int x, int y, const char *color_name) {
    if (!global_xft_draw || !global_xft_font || !display) return;
    
    XftColor draw_color;
    Colormap colormap = DefaultColormap(display, DefaultScreen(display));
    Visual *visual = DefaultVisual(display, DefaultScreen(display));
    
    if (XftColorAllocName(display, visual, colormap, color_name, &draw_color)) {
        XftDrawStringUtf8(global_xft_draw, &draw_color, global_xft_font, 
                          x, y, 
                          (XftChar8*)text, strlen(text));
        XftColorFree(display, visual, colormap, &draw_color);
    } else {
        XftDrawStringUtf8(global_xft_draw, &global_xft_color_white, global_xft_font, 
                          x, y, 
                          (XftChar8*)text, strlen(text));
    }
}

int x11_init() {
    int screen;
    size_t buffer_size = IMG_W * IMG_H * sizeof(uint32_t);
    original_frame_buffer = (uint32_t *)malloc(buffer_size);
    processed_frame_buffer = (uint32_t *)malloc(buffer_size);
    if (!original_frame_buffer || !processed_frame_buffer) { fprintf(stderr, "Fehler: Speicherzuweisung fehlgeschlagen.\n"); return -1; }
    if (!(display = XOpenDisplay(NULL))) { fprintf(stderr, "Fehler: Kann keine Verbindung zum X-Server herstellen.\n"); return -1; }
    screen = DefaultScreen(display);
    
    current_win_w = INITIAL_WINDOW_W;
    current_win_h = INITIAL_WINDOW_H;
    
    Visual *visual = DefaultVisual(display, screen);
    Colormap colormap = DefaultColormap(display, screen);
    
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 
                                 10, 10, current_win_w, current_win_h, 1, 
                                 BlackPixel(display, screen), BlackPixel(display, screen)); 
    XStoreName(display, window, "qgeister1 (X11/V4L2) | Top 16 Isotope | 97%/3% Fusion (Xft)");
    
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    
    XSizeHints *hints = XAllocSizeHints();
    if (hints) {
        hints->flags = PMinSize;
        hints->min_width = IMG_W * 2 + 4 * INITIAL_GAP + ISOTOPE_LIST_WIDTH;
        hints->min_height = IMG_H + 5 * INITIAL_GAP + 70; 
        XSetWMNormalHints(display, window, hints);
        XFree(hints);
    }
    
    XMapWindow(display, window);
    gc = XCreateGC(display, window, 0, NULL);

    ximage_original = XCreateImage(display, visual, DefaultDepth(display, screen), ZPixmap, 0, (char*)original_frame_buffer, IMG_W, IMG_H, 32, 0);
    ximage_processed = XCreateImage(display, visual, DefaultDepth(display, screen), ZPixmap, 0, (char*)processed_frame_buffer, IMG_W, IMG_H, 32, 0);
    if (!ximage_original || !ximage_processed) { fprintf(stderr, "Fehler: XCreateImage.\n"); return -1; }

    // Xft Initialisierung
    global_xft_draw = XftDrawCreate(display, window, visual, colormap);
    if (!global_xft_draw) { fprintf(stderr, "Fehler: XftDrawCreate fehlgeschlagen.\n"); return -1; }
    
    // Optimierte Schriftart-Initialisierung
    global_xft_font = XftFontOpenName(display, screen, FONT_NAME);
    if (!global_xft_font) { 
        // Fallback, falls 'monospace' oder die AA/Hinting-Parameter nicht funktionieren
        global_xft_font = XftFontOpenName(display, screen, "fixed");
        if (!global_xft_font) {
            fprintf(stderr, "Schwerwiegender Fehler: Fallback-Schrift konnte nicht geladen werden.\n");
            return -1;
        }
    }
    
    if (!XftColorAllocName(display, visual, colormap, "white", &global_xft_color_white)) {
        fprintf(stderr, "Warnung: XftColorAllocName für 'white' fehlgeschlagen.\n"); 
    }
    
    return 0;
}

void x11_cleanup() {
    if (display) {
        if (global_xft_draw) XftDrawDestroy(global_xft_draw);
        if (global_xft_font) XftFontClose(display, global_xft_font);
        if (global_xft_color_white.pixel != 0) { 
             XftColorFree(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &global_xft_color_white);
        }
        
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

// ***************************************************************
// HAUPT-VERARBEITUNG UND FUSION
// ***************************************************************

void process_radionullon(GeigerLogicState *state, const uint32_t *img_in, uint32_t *img_out, double frame_delta_sec) {
    int size = IMG_W * IMG_H; 
    uint8_t *gray = (uint8_t *)malloc(size); 
    uint8_t *residue = (uint8_t *)malloc(size); 
    uint8_t *edge = (uint8_t *)malloc(size);
    
    if (!gray || !residue || !edge) return;
    
    // ... (Logik zur Kantenerkennung, Hintergrundsubtraktion, Flicker-Analyse) ...
    memset(edge, 0, size); 
    // Kopiere Original-Frame in den Output-Buffer, um die Graustufen-Konvertierung zu überschreiben
    memcpy(img_out, img_in, size * sizeof(uint32_t));
    
    int hotPixels = 0; float globalAvg = 0.0f;
    for (int i = 0; i < size; ++i) { 
        uint8_t g = rgb32_to_grayscale(img_in[i]); 
        gray[i] = g; 
        if (g > HOT_PIXEL_THRESH) { hotPixels++; } 
        
        // Hintergrundmodell (EMA)
        state->bg[i] = (1.0f - BG_LEARN_RATE) * state->bg[i] + BG_LEARN_RATE * (float)gray[i];
        int diff = (int)fabsf((float)gray[i] - state->bg[i]); 
        // Residuen: Pixel, die stark vom Hintergrund abweichen
        const int RESIDUE_THRESH_SQR = RESIDUE_THRESH * RESIDUE_THRESH;
        residue[i] = (uint8_t)((diff * diff > RESIDUE_THRESH_SQR) ? (diff > 255 ? 255 : diff) : 0); 
        globalAvg += (float)gray[i]; 
    }
    globalAvg /= size;
    uint8_t currentAvg = (uint8_t)globalAvg; //clamp_u8((int)globalAvg);
    int delta = (int)fabsf((float)currentAvg - state->prevGlobalAvg); 
    state->prevGlobalAvg = currentAvg;
    uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
    for (int i = 0; i < size; i++) { 
        state->flicker[i] = (uint8_t)(((int)state->flicker[i] + boost) > 255 ? 255 : (state->flicker[i] + boost)); 
        if (state->flicker[i] > 0) state->flicker[i] = (uint8_t)(state->flicker[i] - 1); 
    }
    
    // Sobel/Edge Detection
    for (int y = 1; y < IMG_H - 1; y++) { 
        for (int x = 1; x < IMG_W - 1; x++) { 
            int i = y * IMG_W + x;
            int gx = -gray[i - IMG_W - 1] - 2 * gray[i - 1] - gray[i + IMG_W - 1] + gray[i - IMG_W + 1] + 2 * gray[i + 1] + gray[i + IMG_W + 1];
            int gy = -gray[i - IMG_W - 1] - 2 * gray[i - IMG_W] - gray[i - IMG_W + 1] + gray[i + IMG_W - 1] + 2 * gray[i + IMG_W] + gray[i + IMG_W + 1];
            int mag = (int)(EDGE_GAIN * sqrtf((float)(gx * gx + gy * gy))); 
            edge[i] = (uint8_t)(mag > 255 ? 255 : mag); 
        } 
    }
            
    // --- Heatmap Generierung ---
    for (int i = 0; i < size; ++i) {
        int score = (int)residue[i] + (int)(edge[i] / 2) + (int)(state->flicker[i] / 3);
        if (score < RADIONULLON_THRESH) { 
            uint8_t g = gray[i]; 
            img_out[i] = (uint32_t)(g << 16 | g << 8 | g); 
            continue; 
        }
        int s_clamped = (score > 255) ? 255 : score; 
        float H_norm = 1.0f - ((float)s_clamped / 255.0f); 
        float R_float, G_float, B_float; 
        hsv_to_rgb(H_norm * 240.0f, 1.0f, 1.0f, &R_float, &G_float, &B_float);
        
        // Alpha-Mischung mit Original-Farbe
        uint8_t R_blend = (uint8_t)((((img_in[i] >> 16) & 0xFF) * (255 - HEATMAP_ALPHA) + (uint8_t)(R_float * 255.0f) * HEATMAP_ALPHA) / 255);
        uint8_t G_blend = (uint8_t)((((img_in[i] >> 8) & 0xFF) * (255 - HEATMAP_ALPHA) + (uint8_t)(G_float * 255.0f) * HEATMAP_ALPHA) / 255);
        uint8_t B_blend = (uint8_t)(((img_in[i] & 0xFF) * (255 - HEATMAP_ALPHA) + (uint8_t)(B_float * 255.0f) * HEATMAP_ALPHA) / 255);
        img_out[i] = (uint32_t)(R_blend << 16 | G_blend << 8 | B_blend);
    }
    
    // --- Video-Dosimetrie ---
    int event_count_scaled = (int)((double)hotPixels * 0.5); 
    poisson_decay_simulation(event_count_scaled, state->isotope_count);
    
    // Verwende frame_delta_sec für genaue Dosisberechnung
    double total_usv_h_new = update_isotopic_dose(state, frame_delta_sec); 
    double video_usv_s = total_usv_h_new / 3600.0;
    double rawVal = total_usv_h_new / SVH_TO_USVS; 
    
    // EMA-Glättung der Rate
    state->currentRadSmooth = (state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);
    
    // --- FUSION (97% Video / 3% Audio) ---
    double audio_usv_s = 0.0; 
    
    // Fusioniere: Fused = (Video * 0.97) + (Audio * 0.03)
    double fused_usv_s = (video_usv_s * FUSION_WEIGHT_VIDEO) + (audio_usv_s * FUSION_WEIGHT_AUDIO);
    
    // Aktualisiere Stats
    state->stats.currentAudioUsvS = audio_usv_s; 
    state->stats.currentFusedUsvS = fused_usv_s;
    state->stats.currentRad = fused_usv_s * (SVH_TO_USVS / 3600.0); // Zurück in Sv/h für die Anzeige
    state->stats.avgRad = state->currentRadSmooth;
    state->stats.alertState = state->stats.currentRad > GEIGER_ALERT_THRESH;

    free(gray); free(residue); free(edge);
}

void redraw_text_area(Display *display, Window window, GC gc, const GeigerLogicState *state, int w, int h) {
    const int gap = INITIAL_GAP;
    const int video_x_start = gap;
    const int video_y_start = gap;

    double video_usv_s = state->total_usv_h / 3600.0;
    double total_usv = state->stats.totalDoseSv * SVH_TO_USVH; 
    
    char stats_line1[128];
    char stats_line2[128];
    char isotope_line[128];
    
    // Textbereich löschen
    XClearArea(display, window, 
               video_x_start, 
               video_y_start + IMG_H + gap, 
               w - 2 * gap,                           
               h - (video_y_start + IMG_H + gap),     
               False);

    // Zeile 1: Fusionierte Rate (Hauptergebnis)
    xft_draw_text("--- GEWICHTETE DOSIMETRIE (97% Video / 3% Audio) ---", video_x_start, video_y_start + IMG_H + 30, "white");
    
    const char *stats1_color = state->stats.currentFusedUsvS * 3600.0 > 1.0 ? "red" : "lime"; // Alert bei > 1 µSv/h
    sprintf(stats_line1, "FUSIONIERT (97%%/3%%): %.3f µSv/s (%.2f µSv/h)", 
            state->stats.currentFusedUsvS, state->stats.currentFusedUsvS * 3600.0);
    xft_draw_text(stats_line1, video_x_start, video_y_start + IMG_H + 50, stats1_color);
            
    // Zeile 2: Einzelwerte und Gesamtdosis
    sprintf(stats_line2, "VIDEO (Rohwert): %.3f µSv/s | AUDIO (Statischer 0%%-Wert): %.3f µSv/s", 
            video_usv_s, state->stats.currentAudioUsvS);
    xft_draw_text(stats_line2, video_x_start, video_y_start + IMG_H + 70, "cyan");
    
    sprintf(isotope_line, "TOTALDOSIS (Akku): %.3f µSv", total_usv);
    xft_draw_text(isotope_line, video_x_start, video_y_start + IMG_H + 90, "yellow");


    // Isotopenliste anzeigen
    int isotope_x_start = video_x_start + IMG_W * 2 + 2 * gap;
    int isotope_y = video_y_start + 30;
    
    xft_draw_text("Top 16 Isotope (µSv/h):", isotope_x_start, video_y_start, "white");
    
    for (int i = 0; i < NUM_ISOTOPES; ++i) {
        const char *iso_color = state->isotope_total_dose_usvh[i] > 1.0 ? "red" : "gray";
        sprintf(isotope_line, "%-20s: %.4f", ISOTOPES[i].name, state->isotope_total_dose_usvh[i]);
        xft_draw_text(isotope_line, isotope_x_start, isotope_y, iso_color);
        isotope_y += 15; 
    }
}

void sigint_handler(int sig) {
    printf("\nBeende Anwendung...\n"); 
    alsa_cleanup(); 
    v4l2_close(); 
    x11_cleanup(); 
    cleanup_geiger_logic(&logic_state); 
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    
    // 1. Initialisierung der Logik und Assets
    init_click_buffers(); 
    init_geiger_logic(&logic_state); 

    // 2. Hardware/Display-Initialisierung
    if (v4l2_init() != 0) { cleanup_geiger_logic(&logic_state); return 1; }
    if (x11_init() != 0) { v4l2_close(); cleanup_geiger_logic(&logic_state); return 1; }
    
    // Bildschirm sofort löschen (schwarz malen)
    XClearWindow(display, window); 
    XFlush(display);

    // 3. Audio-Initialisierung (Playback)
    if (init_alsa() == 0) {
        play_startup_clicks(); 
    } else {
        fprintf(stderr, "Warnung: Audio-Playback nicht möglich. Kein Ticken.\n");
    }

    int running = 1;
    XEvent event;
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    const int gap = INITIAL_GAP;
    const int video_x_start = gap;
    const int video_y_start = gap;
    
    // 4. Haupt-Loop
    while (running) {
        fd_set fds; FD_ZERO(&fds); FD_SET(v4l2_fd, &fds);
        struct timeval tv = { 0, 33000 }; 
        int r = select(v4l2_fd + 1, &fds, NULL, NULL, &tv);

        // --- Zeitmessung für Dosisberechnung ---
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        double current_time_sec = (double)tv_now.tv_sec + (double)tv_now.tv_usec / 1000000.0;
        double frame_delta_sec = 1.0 / 30.0; // Fallback: 30 FPS
        
        if (last_frame_time_sec > 0.0) {
            frame_delta_sec = current_time_sec - last_frame_time_sec;
            if (frame_delta_sec <= 0.0) frame_delta_sec = 1.0 / 30.0; 
        }
        last_frame_time_sec = current_time_sec; // Zeit des aktuellen Frames speichern
        // -------------------------------------------------------------------


        if (r > 0 && FD_ISSET(v4l2_fd, &fds)) {
            if (v4l2_read_frame(original_frame_buffer) == 0) {
                
                // Aufruf mit frame_delta_sec für genaue Dosisberechnung
                process_radionullon(&logic_state, original_frame_buffer, processed_frame_buffer, frame_delta_sec); 

                // Akustisches Ticken basierend auf der FUSIONIERTEN Rate
                if (logic_state.stats.currentFusedUsvS * 3600.0 > 0.1 && global_pcm_handle != NULL) { 
                    int click_index = (int)(logic_state.stats.currentFusedUsvS * 3600.0 * 100.0) % NUM_SCALE_NOTES;
                    if (click_index < 0) click_index = 0;
                    play_alsa_click(click_index); 
                }
                
                XPutImage(display, window, gc, ximage_original, 0, 0, video_x_start, video_y_start, IMG_W, IMG_H);
                XPutImage(display, window, gc, ximage_processed, 0, 0, video_x_start + IMG_W + gap, video_y_start, IMG_W, IMG_H);
                
                redraw_text_area(display, window, gc, &logic_state, current_win_w, current_win_h);

                XFlush(display);
            }
        } else if (r == -1) {
            perror("select"); break;
        }

        // X11-Events verarbeiten
        while (XPending(display)) {
            XNextEvent(display, &event);
            
            if (event.type == ConfigureNotify) {
                if (event.xconfigure.width != current_win_w || event.xconfigure.height != current_win_h) {
                    current_win_w = event.xconfigure.width;
                    current_win_h = event.xconfigure.height;
                    // Neuzeichnen nur des Textbereichs (Effizienz)
                    redraw_text_area(display, window, gc, &logic_state, current_win_w, current_win_h);
                }
            }
            if (event.type == Expose) {
                XPutImage(display, window, gc, ximage_original, 0, 0, video_x_start, video_y_start, IMG_W, IMG_H);
                XPutImage(display, window, gc, ximage_processed, 0, 0, video_x_start + IMG_W + gap, video_y_start, IMG_W, IMG_H);
                redraw_text_area(display, window, gc, &logic_state, current_win_w, current_win_h);
            } else if (event.type == KeyPress) {
                char buffer[1]; KeySym key; XLookupString(&event.xkey, buffer, sizeof(buffer), &key, NULL);
                if (key == XK_Escape) running = 0;
            } else if (event.type == ClientMessage) {
                if ((Atom)event.xclient.data.l[0] == wm_delete_window) running = 0;
            }
        }
    }

    // 5. Cleanup
    sigint_handler(0);
    return 0;
}
