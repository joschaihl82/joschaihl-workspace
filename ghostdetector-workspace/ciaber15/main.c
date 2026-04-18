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

// ALSA Header
#include <alsa/asoundlib.h> 

// V4L2-Header
#include <linux/videodev2.h> 

// --- X11 Includes ---
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h> 

// ***************************************************************
// KONSTANTEN & DEFINITIONEN
// ***************************************************************

#define DEVICE_NODE "/dev/video0" // <--- VERWENDET /dev/video0
#define IMG_W 640 
#define IMG_H 480 
#define GRAPH_POINTS 150 
#define GRAPH_H 80       
#define NUM_ISOTOPES 16 

#define INITIAL_GAP 10
#define INFO_BAR_H 100
#define GRAPH_BLOCK_H (GRAPH_H + 5)
#define INITIAL_WINDOW_W (IMG_W + 2 * INITIAL_GAP) 
#define INITIAL_WINDOW_H (IMG_H + 5 * INITIAL_GAP + INFO_BAR_H + (4 * GRAPH_BLOCK_H)) 
#define ISOTOPE_LIST_WIDTH 250

// Dosimetrie
#define SVH_TO_USVS 277.777777778 
#define SVH_TO_USVH 1000000.0

// Audio
#define SAMPLE_RATE 44100
#define CLICK_DURATION_DENOMINATOR 20 
#define CLICK_SAMPLES (SAMPLE_RATE / CLICK_DURATION_DENOMINATOR) 
#define NUM_SCALE_NOTES 16 
#define FM_CARRIER_MODULATOR_RATIO 1.414 
#define FM_MODULATION_INDEX_BASE 10.0    

// Radionullon-Tunables
#define BG_LEARN_RATE 0.005f 
#define HOT_PIXEL_THRESH 200 
#define FLICKER_MEMORY 5     
#define GEIGER_SMOOTH_FACTOR 0.3 
#define GEIGER_LONG_SMOOTH_FACTOR 0.01 
#define GEIGER_ALERT_THRESH 5.0 

// ***************************************************************
// GLOBALE STRUKTUREN
// ***************************************************************

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

typedef struct { 
    double currentRad; double avgRad; double totalDoseSv; 
    double currentAudioUsvS; double currentFusedUsvS; 
    int alertState;
    double shortHistory[GRAPH_POINTS]; double avgHistory[GRAPH_POINTS]; 
    double longHistory[GRAPH_POINTS]; double doseHistory[GRAPH_POINTS]; 
    long long startTimeSec; 
} GeigerStats;

typedef struct GeigerLogicState {
    float *bg; 
    uint8_t *flicker; 
    float prevGlobalAvg;
    double currentRadSmooth; double longTermAvg;
    double totalDoseAcc; int longTermCounter;
    
    long long isotope_count[NUM_ISOTOPES];
    double isotope_total_dose_usvh[NUM_ISOTOPES]; 
    double total_usv_h;                          
    
    GeigerStats stats;
} GeigerLogicState;

struct Buffer { void *start; size_t length; };

// ***************************************************************
// GLOBALE VARIABLEN
// ***************************************************************

GeigerLogicState logic_state;
uint32_t *original_frame_buffer = NULL;
uint32_t *processed_frame_buffer = NULL;
uint32_t *scaled_frame_buffer = NULL;

static int v4l2_fd = -1;
static struct Buffer *v4l2_buffers = NULL; 
static unsigned int v4l2_n_buffers = 0; 

snd_pcm_t* global_pcm_handle = NULL; 
short click_buffers[NUM_SCALE_NOTES][CLICK_SAMPLES];

Display *display = NULL; Window window; GC gc;
XImage *ximage_processed = NULL; 
int current_win_w = 0; int current_win_h = 0;
int scaled_image_w = 0; int scaled_image_h = 0;

XftDraw *global_xft_draw = NULL;
XftFont *global_xft_font = NULL;
XftColor global_xft_color_white; 
XftColor global_xft_color_primary; 
XftColor global_xft_color_secondary; 
XftColor global_xft_color_alert; 

const char *FONT_NAME = "monospace:pixelsize=15:antialias=true:hinting=true";

double last_frame_time_sec = 0.0;

// ***************************************************************
// VORWÄRTSDEKLARATIONEN
// ***************************************************************
void cleanup_geiger_logic(GeigerLogicState *state);
void alsa_cleanup(void);
void v4l2_uninit_device(void);
void v4l2_stop_capturing(void);
void v4l2_close(void);
void x11_cleanup(void);
int x11_init(void); 
void redraw_info_area(Display *d, Window w, GC g, GeigerLogicState *state, int win_w, int win_h); 
void process_radionullon(GeigerLogicState *state, const uint32_t *original_frame, uint32_t *processed_frame, double frame_delta_sec);

// ***************************************************************
// GRAFIK-HILFSFUNKTIONEN (unverändert)
// ***************************************************************

static inline uint8_t rgb32_to_grayscale(uint32_t rgb) {
    uint8_t r = (uint8_t)((rgb >> 16) & 0xFF); 
    uint8_t g = (uint8_t)((rgb >> 8) & 0xFF); 
    uint8_t b = (uint8_t)(rgb & 0xFF);
    return (uint8_t)((299 * r + 587 * g + 114 * b) / 1000);
}

void hsv_to_rgb(float H, float S, float V, float *R, float *G, float *B) {
    if (S <= 0.0f) { *R = V; *G = V; *B = V; return; }
    
    if (H >= 360.0f) H = 0.0f;
    H /= 60.0f;
    int i = (int)H;
    float f = H - i;
    float p = V * (1.0f - S);
    float q = V * (1.0f - S * f);
    float t = V * (1.0f - S * (1.0f - f));

    switch(i) {
        case 0: *R = V; *G = t; *B = p; break;
        case 1: *R = q; *G = V; *B = p; break;
        case 2: *R = p; *G = V; *B = t; break;
        case 3: *R = p; *G = q; *B = V; break;
        case 4: *R = t; *G = p; *B = V; break;
        case 5: default: *R = V; *G = p; *B = q; break;
    }
}

static void set_gc_color(Display *d, GC g, int r, int g_val, int b) {
    XColor color;
    char color_str[8];
    snprintf(color_str, 8, "#%02x%02x%02x", r, g_val, b);
    
    XParseColor(d, DefaultColormap(d, DefaultScreen(d)), color_str, &color);
    XAllocColor(d, DefaultColormap(d, DefaultScreen(d)), &color);
    XSetForeground(d, g, color.pixel);
}

static void draw_single_graph(Display *d, Window w, XftDraw *xft_d, XftFont *font, GC g, 
                              int x, int y, int width, int height, 
                              const double *history, double max_val, const char *title, 
                              XftColor *color) {
    set_gc_color(d, g, 50, 50, 50); 
    XFillRectangle(d, w, g, x, y, width, height);
    
    set_gc_color(d, g, 100, 100, 100); 
    for (int i = 1; i < 5; ++i) {
        XDrawLine(d, w, g, x, y + height * i / 5, x + width, y + height * i / 5);
    }
    
    if (max_val < 0.001) max_val = 1.0; 
    
    XftColor *line_color = color;
    set_gc_color(d, g, (int)(line_color->color.red / 256), (int)(line_color->color.green / 256), (int)(line_color->color.blue / 256));

    for (int i = 0; i < GRAPH_POINTS - 1; ++i) {
        int x1 = x + i * width / GRAPH_POINTS;
        int y1 = y + height - (int)(history[i] / max_val * height);
        int x2 = x + (i + 1) * width / GRAPH_POINTS;
        int y2 = y + height - (int)(history[i + 1] / max_val * height);
        
        if (y1 < y) { y1 = y; } if (y1 > y + height) { y1 = y + height; }
        if (y2 < y) { y2 = y; } if (y2 > y + height) { y2 = y + height; }

        XDrawLine(d, w, g, x1, y1, x2, y2);
    }

    XftDrawStringUtf8(xft_d, color, font, x, y - 2, (XftChar8*)title, strlen(title));
    
    char max_val_str[32];
    snprintf(max_val_str, sizeof(max_val_str), "Max: %.2f", max_val);
    
    XGlyphInfo extents;
    XftTextExtents8(d, font, (XftChar8*)max_val_str, strlen(max_val_str), &extents);
    
    XftDrawStringUtf8(xft_d, color, font, x + width - extents.xOff, y - 2, (XftChar8*)max_val_str, strlen(max_val_str));
}


// ***************************************************************
// V4L2 IMPLEMENTIERUNG
// ***************************************************************

static int xioctl(int fh, int request, void *arg) {
    int r;
    do { r = ioctl(fh, request, arg); } 
    while (-1 == r && EINTR == errno);
    return r;
}

void v4l2_stop_capturing(void) {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_fd != -1) {
        if (-1 == xioctl(v4l2_fd, VIDIOC_STREAMOFF, &type)) {
             perror("VIDIOC_STREAMOFF fehlgeschlagen");
        }
    }
}

static int v4l2_init_mmap() {
    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    
    if (-1 == xioctl(v4l2_fd, VIDIOC_REQBUFS, &req)) {
        perror("VIDIOC_REQBUFS failed");
        return -1;
    }

    if (req.count < 2) {
        fprintf(stderr, "Unzureichender Pufferspeicher: %d.\n", req.count);
        return -1;
    }

    v4l2_buffers = calloc(req.count, sizeof(*v4l2_buffers));
    if (!v4l2_buffers) {
        fprintf(stderr, "Speicherzuweisung für V4L2-Puffer fehlgeschlagen.\n");
        return -1;
    }

    v4l2_n_buffers = req.count;

    for (unsigned int n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf = {0};

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (-1 == xioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf)) {
            perror("VIDIOC_QUERYBUF fehlgeschlagen");
            return -1;
        }

        v4l2_buffers[n_buffers].length = buf.length;
        v4l2_buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);

        if (MAP_FAILED == v4l2_buffers[n_buffers].start) {
            perror("mmap fehlgeschlagen");
            return -1;
        }
    }
    return 0;
}

static int v4l2_start_capturing() {
    unsigned int i;
    enum v4l2_buf_type type;

    for (i = 0; i < v4l2_n_buffers; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(v4l2_fd, VIDIOC_QBUF, &buf)) {
            perror("VIDIOC_QBUF fehlgeschlagen");
            return -1;
        }
    }
    
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(v4l2_fd, VIDIOC_STREAMON, &type)) {
        perror("VIDIOC_STREAMON fehlgeschlagen");
        return -1;
    }
    return 0;
}

void v4l2_uninit_device() {
    for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
        if (-1 == munmap(v4l2_buffers[i].start, v4l2_buffers[i].length)) {
            perror("munmap fehlgeschlagen");
        }
    }
    free(v4l2_buffers);
    v4l2_buffers = NULL;
    v4l2_n_buffers = 0;
}

int v4l2_init(void) {
    // 1. Gerät öffnen
    v4l2_fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
    if (-1 == v4l2_fd) {
        fprintf(stderr, "Fehler beim Öffnen von %s: %s\n", DEVICE_NODE, strerror(errno));
        return -1;
    }

    // 2. Format setzen
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W;
    fmt.fmt.pix.height = IMG_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32; // <--- RGB32/RV32 Format (32-Bit, 4 Bytes pro Pixel)
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == xioctl(v4l2_fd, VIDIOC_S_FMT, &fmt)) {
        fprintf(stderr, "VIDIOC_S_FMT konnte Format (RGB32) nicht setzen. Versuch fehlgeschlagen.\n");
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }
    
    if (fmt.fmt.pix.width != IMG_W || fmt.fmt.pix.height != IMG_H) {
        fprintf(stderr, "Warnung: Gerät ignoriert Format. Geforderte Größe (%dx%d) != Erhaltene Größe (%dx%d).\n",
                IMG_W, IMG_H, fmt.fmt.pix.width, fmt.fmt.pix.height);
    }
    
    // 3. Speicher-Mapping initialisieren
    if (v4l2_init_mmap() == -1) {
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }

    // 4. Stream starten
    if (v4l2_start_capturing() == -1) {
        v4l2_uninit_device();
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }
    
    size_t buffer_size = IMG_W * IMG_H * sizeof(uint32_t);
    original_frame_buffer = (uint32_t *)malloc(buffer_size);
    processed_frame_buffer = (uint32_t *)malloc(buffer_size);
    if (!original_frame_buffer || !processed_frame_buffer) { 
        fprintf(stderr, "Fehler: Speicherzuweisung Frame-Puffer fehlgeschlagen.\n"); 
        v4l2_stop_capturing(); 
        v4l2_uninit_device(); close(v4l2_fd); v4l2_fd = -1;
        return -1;
    }
    
    return 0;
}

void v4l2_close(void) {
    if (v4l2_fd != -1) {
        v4l2_stop_capturing();
        v4l2_uninit_device();
        close(v4l2_fd);
        v4l2_fd = -1;
    }
    if (original_frame_buffer) { free(original_frame_buffer); original_frame_buffer = NULL; }
    if (processed_frame_buffer) { free(processed_frame_buffer); processed_frame_buffer = NULL; }
}

int v4l2_read_frame(uint32_t *buffer) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(v4l2_fd, VIDIOC_DQBUF, &buf)) {
        if (errno == EAGAIN) return -1; 
        perror("VIDIOC_DQBUF fehlgeschlagen");
        return -1;
    }
    
    size_t data_len = buf.bytesused;
    size_t expected_len = IMG_W * IMG_H * 4; 
    
    if (data_len > expected_len) data_len = expected_len;
    
    memcpy(buffer, v4l2_buffers[buf.index].start, data_len);

    if (-1 == xioctl(v4l2_fd, VIDIOC_QBUF, &buf)) {
        perror("VIDIOC_QBUF fehlgeschlagen");
        return -1;
    }
    
    return 0; 
}


// ***************************************************************
// ALSA IMPLEMENTIERUNG (unverändert)
// ***************************************************************

static void generate_fm_sample(short *buffer, double freq, double amp_decay, double fm_idx) {
    double phase_c = 0.0;
    double phase_m = 0.0;
    double c_freq = freq;
    double m_freq = freq * FM_CARRIER_MODULATOR_RATIO;
    double amp = 1.0;

    for (int i = 0; i < CLICK_SAMPLES; ++i) {
        double t = (double)i / SAMPLE_RATE;
        amp = exp(-t * amp_decay);
        
        phase_m += 2.0 * M_PI * m_freq / SAMPLE_RATE;
        if (phase_m > 2.0 * M_PI) phase_m -= 2.0 * M_PI;

        double mod_signal = fm_idx * sin(phase_m);
        phase_c += 2.0 * M_PI * c_freq / SAMPLE_RATE + mod_signal / SAMPLE_RATE;
        if (phase_c > 2.0 * M_PI) phase_c -= 2.0 * M_PI;

        double sample = sin(phase_c) * amp;

        buffer[i] = (short)(sample * 32767.0 * 0.5); 
    }
}

void init_click_buffers(void) {
    const int SEMITONES[NUM_SCALE_NOTES] = {
        0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26 
    };
    const double BASE_FREQ_A4 = 440.0;
    const double DECAY_RATE = 1500.0;
    const double FM_IDX_DECAY = 1.0; 

    for (int i = 0; i < NUM_SCALE_NOTES; ++i) {
        double freq = BASE_FREQ_A4 * pow(2.0, (double)SEMITONES[i] / 12.0);
        double fm_idx = FM_MODULATION_INDEX_BASE - (i * FM_IDX_DECAY);
        if (fm_idx < 1.0) fm_idx = 1.0;
        
        generate_fm_sample(click_buffers[i], freq, DECAY_RATE, fm_idx);
    }
}

int init_alsa(void) {
    int err;
    const char *pcm_name = "default";
    snd_pcm_hw_params_t *params;

    if ((err = snd_pcm_open(&global_pcm_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "ALSA: Playback open failed: %s\n", snd_strerror(err));
        global_pcm_handle = NULL;
        return -1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(global_pcm_handle, params);

    snd_pcm_hw_params_set_access(global_pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(global_pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(global_pcm_handle, params, 1);
    
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(global_pcm_handle, params, &rate, 0);

    if ((err = snd_pcm_hw_params(global_pcm_handle, params)) < 0) {
        fprintf(stderr, "ALSA: Parameter set failed: %s\n", snd_strerror(err));
        snd_pcm_close(global_pcm_handle);
        global_pcm_handle = NULL;
        return -1;
    }
    
    return 0;
}

void play_alsa_click(int index) {
    if (global_pcm_handle == NULL || index < 0 || index >= NUM_SCALE_NOTES) return;
    
    short *buffer = click_buffers[index];
    int frames = CLICK_SAMPLES;
    int err;

    err = snd_pcm_writei(global_pcm_handle, buffer, frames);
    if (err < 0) {
        if (err == -EPIPE) { 
            snd_pcm_prepare(global_pcm_handle);
            err = snd_pcm_writei(global_pcm_handle, buffer, frames);
        }
    }
}

void alsa_cleanup(void) {
    if (global_pcm_handle) {
        snd_pcm_drain(global_pcm_handle);
        snd_pcm_close(global_pcm_handle);
        global_pcm_handle = NULL;
    }
}


// ***************************************************************
// GEIGER-LOGIK (Radionullon-Verarbeitung)
// ***************************************************************

void init_geiger_logic(GeigerLogicState *state) {
    int size = IMG_W * IMG_H;
    state->bg = (float *)calloc(size, sizeof(float)); 
    state->flicker = (uint8_t *)calloc(size, sizeof(uint8_t));
    for (int i = 0; i < size; ++i) state->bg[i] = 128.0f; 
    
    state->prevGlobalAvg = 128.0f; 
    state->currentRadSmooth = 0.0; 
    state->longTermAvg = 0.0;
    state->totalDoseAcc = 0.0; 
    state->longTermCounter = 0;
    
    memset(state->isotope_count, 0, sizeof(state->isotope_count));
    memset(state->isotope_total_dose_usvh, 0, sizeof(state->isotope_total_dose_usvh));
    state->total_usv_h = 0.0;

    memset(state->stats.shortHistory, 0, sizeof(state->stats.shortHistory));
    memset(state->stats.avgHistory, 0, sizeof(state->stats.avgHistory));
    memset(state->stats.longHistory, 0, sizeof(state->stats.longHistory));
    memset(state->stats.doseHistory, 0, sizeof(state->stats.doseHistory));
    state->stats.totalDoseSv = 0.0; 
    
    struct timeval tv; gettimeofday(&tv, NULL); state->stats.startTimeSec = tv.tv_sec;
}

void cleanup_geiger_logic(GeigerLogicState *state) {
    if (state->bg) free(state->bg); 
    if (state->flicker) free(state->flicker);
}

void process_radionullon(GeigerLogicState *state, const uint32_t *original_frame, uint32_t *processed_frame, double frame_delta_sec) {
    int size = IMG_W * IMG_H;
    int detections_count = 0;

    for (int i = 0; i < size; ++i) {
        uint8_t current_gs = rgb32_to_grayscale(original_frame[i]);
        float bg_val = state->bg[i];
        
        // --- 1. Background Subtraction und Update ---
        int residue = (int)current_gs - (int)bg_val;
        
        state->bg[i] = bg_val * (1.0f - BG_LEARN_RATE) + current_gs * BG_LEARN_RATE;
        
        // --- 2. Hot Pixel Detection ---
        if (residue > HOT_PIXEL_THRESH) {
            if (state->flicker[i] < FLICKER_MEMORY) {
                 state->flicker[i]++;
            } else {
                if (state->flicker[i] == FLICKER_MEMORY) {
                    detections_count++;
                }
                state->flicker[i] = FLICKER_MEMORY; 
            }
        } else {
            if (state->flicker[i] > 0) {
                state->flicker[i]--;
            }
        }
        
        // --- 3. Visualisierung (Heatmap) ---
        if (state->flicker[i] > 0) {
             float r_hsv, g_hsv, b_hsv;
             float flicker_ratio = (float)state->flicker[i] / FLICKER_MEMORY;
             float hue = (1.0f - flicker_ratio) * 120.0f; 
             float saturation = 1.0f;
             float value = 1.0f;

             hsv_to_rgb(hue, saturation, value, &r_hsv, &g_hsv, &b_hsv);
             
             uint8_t red = (uint8_t)(r_hsv * 255.0f);
             uint8_t green = (uint8_t)(g_hsv * 255.0f);
             uint8_t blue = (uint8_t)(b_hsv * 255.0f);
             
             uint32_t original_pix = original_frame[i];
             uint8_t orig_r = (uint8_t)((original_pix >> 16) & 0xFF);
             uint8_t orig_g = (uint8_t)((original_pix >> 8) & 0xFF);
             uint8_t orig_b = (uint8_t)(original_pix & 0xFF);

             uint8_t final_r = (orig_r / 2) + (red / 2);
             uint8_t final_g = (orig_g / 2) + (green / 2);
             uint8_t final_b = (orig_b / 2) + (blue / 2);

             processed_frame[i] = (0xFF << 24) | (final_r << 16) | (final_g << 8) | final_b; 

        } else {
            processed_frame[i] = original_frame[i];
        }
    }
    
    // --- 4. Dosisberechnung und History-Update ---
    double cps = (double)detections_count / frame_delta_sec;
    double current_usvh = cps / 100.0; 
    
    if (current_usvh < 0.05) current_usvh = 0.05; 

    state->currentRadSmooth = state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR) + current_usvh * GEIGER_SMOOTH_FACTOR;
    state->total_usv_h = state->currentRadSmooth;
    
    double usvs_video = state->total_usv_h / 3600.0;
    state->stats.currentFusedUsvS = usvs_video * 0.97;
    
    state->stats.totalDoseSv += state->stats.currentFusedUsvS * frame_delta_sec;

    for (int i = 0; i < GRAPH_POINTS - 1; ++i) {
        state->stats.shortHistory[i] = state->stats.shortHistory[i + 1];
        state->stats.avgHistory[i] = state->stats.avgHistory[i + 1];
        state->stats.longHistory[i] = state->stats.longHistory[i + 1];
        state->stats.doseHistory[i] = state->stats.doseHistory[i + 1];
    }
    state->stats.shortHistory[GRAPH_POINTS - 1] = state->currentRadSmooth;
    state->stats.avgHistory[GRAPH_POINTS - 1] = state->stats.avgHistory[GRAPH_POINTS - 2] * (1.0 - GEIGER_SMOOTH_FACTOR) + state->currentRadSmooth * GEIGER_SMOOTH_FACTOR;
    state->stats.longHistory[GRAPH_POINTS - 1] = state->stats.longHistory[GRAPH_POINTS - 2] * (1.0 - GEIGER_LONG_SMOOTH_FACTOR) + state->currentRadSmooth * GEIGER_LONG_SMOOTH_FACTOR;
    state->stats.doseHistory[GRAPH_POINTS - 1] = state->stats.totalDoseSv * SVH_TO_USVH;

    // Fiktive Isotopen-Akkumulation (Verwendung von ISOTOPES)
    state->isotope_total_dose_usvh[4] = state->currentRadSmooth * 0.5 * ISOTOPES[4].RBE_FACTOR; 
    state->isotope_total_dose_usvh[5] = state->currentRadSmooth * 0.4 * ISOTOPES[5].RBE_FACTOR;
    state->isotope_total_dose_usvh[6] = state->currentRadSmooth * 0.1 * ISOTOPES[6].RBE_FACTOR;
    state->isotope_total_dose_usvh[7] = state->currentRadSmooth * 0.05 * ISOTOPES[7].RBE_FACTOR;
    state->isotope_total_dose_usvh[8] = state->currentRadSmooth * 0.02 * ISOTOPES[8].RBE_FACTOR;
}


// ***************************************************************
// X11 IMPLEMENTIERUNG (unverändert)
// ***************************************************************

void redraw_info_area(Display *d, Window w, GC g, GeigerLogicState *state, int win_w, int win_h) {
    int x_start = INITIAL_GAP;
    int y_start_info = INITIAL_GAP + IMG_H;
    int y_start_graphs = y_start_info + INFO_BAR_H + INITIAL_GAP;
    
    int graph_width = win_w - ISOTOPE_LIST_WIDTH - 3 * INITIAL_GAP; 
    int isotope_x_start = x_start + graph_width + 2 * INITIAL_GAP;
    int graph_y = y_start_graphs;
    
    // --- 1. Hintergrund löschen ---
    set_gc_color(d, g, 0, 0, 0); 
    XFillRectangle(d, w, g, 0, y_start_info, win_w, win_h - y_start_info);

    // --- 2. Statistik-Tabelle ---
    char line[128];
    
    XftColor* rad_color = (state->total_usv_h > 1.0) ? &global_xft_color_alert : &global_xft_color_white;

    snprintf(line, sizeof(line), "CURRENT (µSv/h): %.3f", state->total_usv_h);
    XftDrawStringUtf8(global_xft_draw, rad_color, global_xft_font, x_start, y_start_info + 20, (XftChar8*)line, strlen(line));

    double total_usv = state->stats.totalDoseSv * SVH_TO_USVH;
    snprintf(line, sizeof(line), "TOTAL DOSE (µSv): %.6f", total_usv);
    XftDrawStringUtf8(global_xft_draw, &global_xft_color_primary, global_xft_font, x_start, y_start_info + 45, (XftChar8*)line, strlen(line));
    
    snprintf(line, sizeof(line), "FPS: 30 | V4L2-Src: OK | ALSA-Sink: %s", (global_pcm_handle != NULL) ? "OK" : "ERR");
    XftDrawStringUtf8(global_xft_draw, &global_xft_color_secondary, global_xft_font, x_start, y_start_info + 70, (XftChar8*)line, strlen(line));

    // --- 3. Graphen zeichnen ---
    double max_short = 0.0;
    for (int i = 0; i < GRAPH_POINTS; ++i) {
        if (state->stats.shortHistory[i] > max_short) max_short = state->stats.shortHistory[i];
    }
    if (max_short < 0.1) max_short = 0.1; 

    draw_single_graph(d, w, global_xft_draw, global_xft_font, g, x_start, graph_y, graph_width, GRAPH_H,
                      state->stats.shortHistory, max_short * 1.2, "Current µSv/h", &global_xft_color_white);
    graph_y += GRAPH_BLOCK_H;

    draw_single_graph(d, w, global_xft_draw, global_xft_font, g, x_start, graph_y, graph_width, GRAPH_H,
                      state->stats.longHistory, max_short * 1.2, "Long-Term Avg", &global_xft_color_primary);
    graph_y += GRAPH_BLOCK_H;
    
    double max_dose = state->stats.doseHistory[GRAPH_POINTS - 1] * 1.2;
    if (max_dose < 0.1) max_dose = 0.1;
    draw_single_graph(d, w, global_xft_draw, global_xft_font, g, x_start, graph_y, graph_width, GRAPH_H,
                      state->stats.doseHistory, max_dose, "Total Dose (µSv)", &global_xft_color_secondary);
    graph_y += GRAPH_BLOCK_H;
    
    // --- 4. Isotopen-Liste (Verwendung von ISOTOPES) ---
    int list_y = y_start_graphs;
    int line_h = 20; 

    XftDrawStringUtf8(global_xft_draw, &global_xft_color_alert, global_xft_font, isotope_x_start, list_y - 2, (XftChar8*)"TOP ISOTOPES (Sim.)", strlen("TOP ISOTOPES (Sim.)"));
    list_y += line_h;

    for (int i = 4; i < 9; ++i) { 
        XftColor* iso_color = (state->isotope_total_dose_usvh[i] > 0.01) ? &global_xft_color_alert : &global_xft_color_white;

        snprintf(line, sizeof(line), "%-20s %.3f µSv/h", 
                 ISOTOPES[i].name, 
                 state->isotope_total_dose_usvh[i]);
        
        XftDrawStringUtf8(global_xft_draw, iso_color, global_xft_font, isotope_x_start, list_y, (XftChar8*)line, strlen(line));
        list_y += line_h;
    }
}

int x11_init(void) {
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Fehler: Kann Display nicht öffnen.\n");
        return -1;
    }

    int screen = DefaultScreen(display);
    
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 0, 0, INITIAL_WINDOW_W, INITIAL_WINDOW_H,
                                 1, BlackPixel(display, screen), WhitePixel(display, screen));
    
    current_win_w = INITIAL_WINDOW_W;
    current_win_h = INITIAL_WINDOW_H;
    
    XStoreName(display, window, "qgeister1 - Radionullon Counter");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    gc = XCreateGC(display, window, 0, NULL);
    
    scaled_image_w = IMG_W; 
    scaled_image_h = IMG_H;
    
    size_t scaled_buffer_size = scaled_image_w * scaled_image_h * sizeof(uint32_t);
    scaled_frame_buffer = (uint32_t*)malloc(scaled_buffer_size);
    if (!scaled_frame_buffer) {
        fprintf(stderr, "Speicherzuweisung für skalierten Puffer fehlgeschlagen.\n");
        XCloseDisplay(display);
        display = NULL;
        return -1;
    }
    
    ximage_processed = XCreateImage(display, DefaultVisual(display, screen), 
                                    DefaultDepth(display, screen), ZPixmap, 0, 
                                    (char*)scaled_frame_buffer, scaled_image_w, scaled_image_h, 32, 0);
    if (!ximage_processed) {
        fprintf(stderr, "Fehler: XCreateImage fehlgeschlagen.\n");
        XCloseDisplay(display);
        free(scaled_frame_buffer);
        display = NULL;
        return -1;
    }

    global_xft_draw = XftDrawCreate(display, window, DefaultVisual(display, screen), DefaultColormap(display, screen));
    global_xft_font = XftFontOpenName(display, screen, FONT_NAME);
    if (!global_xft_font) {
        fprintf(stderr, "Fehler: Kann Xft-Schriftart '%s' nicht laden.\n", FONT_NAME);
    }
    
    XftColorAllocName(display, DefaultVisual(display, screen), DefaultColormap(display, screen), "white", &global_xft_color_white);
    XftColorAllocName(display, DefaultVisual(display, screen), DefaultColormap(display, screen), "lime green", &global_xft_color_primary);
    XftColorAllocName(display, DefaultVisual(display, screen), DefaultColormap(display, screen), "orange", &global_xft_color_secondary);
    XftColorAllocName(display, DefaultVisual(display, screen), DefaultColormap(display, screen), "red", &global_xft_color_alert);
    
    XMapWindow(display, window);
    return 0;
}

void x11_cleanup(void) {
    if (global_xft_font) XftFontClose(display, global_xft_font);
    if (global_xft_draw) XftDrawDestroy(global_xft_draw);
    
    if (ximage_processed) {
        ximage_processed->data = NULL; 
        XDestroyImage(ximage_processed);
    }
    if (scaled_frame_buffer) free(scaled_frame_buffer);

    if (gc) XFreeGC(display, gc);
    if (window) XDestroyWindow(display, window);
    if (display) XCloseDisplay(display);
}


// ***************************************************************
// MAIN LOOP & HANDLER
// ***************************************************************

void sigint_handler(int sig) {
    (void)sig; 
    printf("\nBeende Anwendung...\n"); 
    alsa_cleanup(); 
    v4l2_close(); 
    x11_cleanup(); 
    cleanup_geiger_logic(&logic_state); 
    exit(0);
}

int main(int argc, char *argv[]) {
    (void)argc; 
    (void)argv;
    
    signal(SIGINT, sigint_handler);
    
    // 1. Initialisierung der Logik und Assets
    init_click_buffers(); 
    init_geiger_logic(&logic_state); 

    // 2. Hardware/Display-Initialisierung
    if (v4l2_init() != 0) { 
        fprintf(stderr, "V4L2 Initialisierung fehlgeschlagen. Programm beendet.\n");
        cleanup_geiger_logic(&logic_state); 
        return 1; 
    }
    if (x11_init() != 0) {
        v4l2_close(); cleanup_geiger_logic(&logic_state); return 1; 
    }
    
    XClearWindow(display, window); 
    XFlush(display);

    // 3. Audio-Initialisierung
    if (init_alsa() == 0) {
        play_alsa_click(0); 
    } else {
        fprintf(stderr, "Warnung: Audio-Playback nicht möglich. Kein Ticken.\n");
    }

    int running = 1;
    XEvent event;
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    // 4. Haupt-Loop
    while (running) {
        fd_set fds; FD_ZERO(&fds); FD_SET(v4l2_fd, &fds);
        struct timeval tv = { 0, 33000 }; 
        int r = select(v4l2_fd + 1, &fds, NULL, NULL, &tv);

        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        double current_time_sec = (double)tv_now.tv_sec + (double)tv_now.tv_usec / 1000000.0;
        double frame_delta_sec = 1.0 / 30.0; 
        
        if (last_frame_time_sec > 0.0) {
            frame_delta_sec = current_time_sec - last_frame_time_sec;
            if (frame_delta_sec <= 0.0) frame_delta_sec = 1.0 / 30.0; 
        }
        last_frame_time_sec = current_time_sec; 
        
        // --- V4L2 Frame-Verarbeitung ---
        if (r > 0 && FD_ISSET(v4l2_fd, &fds)) {
            if (v4l2_read_frame(original_frame_buffer) == 0) { 
                
                process_radionullon(&logic_state, original_frame_buffer, processed_frame_buffer, frame_delta_sec); 

                // Kopiere den verarbeiteten Puffer in den XImage-Puffer (640x480)
                memcpy(scaled_frame_buffer, processed_frame_buffer, IMG_W * IMG_H * sizeof(uint32_t));

                // Akustisches Ticken 
                if (logic_state.stats.currentFusedUsvS * 3600.0 > 0.1 && global_pcm_handle != NULL) { 
                    int click_index = (int)(logic_state.stats.currentFusedUsvS * 3600.0 * 100.0) % NUM_SCALE_NOTES;
                    if (click_index < 0) click_index = 0;
                    play_alsa_click(click_index); 
                }
                
                XPutImage(display, window, gc, ximage_processed, 
                          0, 0, INITIAL_GAP, INITIAL_GAP, IMG_W, IMG_H);

                redraw_info_area(display, window, gc, &logic_state, current_win_w, current_win_h);

                XFlush(display);
            }
        } else if (r == -1) {
            if (errno != EINTR) { 
                perror("select"); 
                running = 0;
            }
        }
        // --- X11 Event Handling ---
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == ConfigureNotify) {
                 XConfigureEvent *xc = &event.xconfigure;
                 current_win_w = xc->width;
                 current_win_h = xc->height;
                 
                 redraw_info_area(display, window, gc, &logic_state, current_win_w, current_win_h);

            } else if (event.type == Expose) {
                XPutImage(display, window, gc, ximage_processed, 
                          0, 0, INITIAL_GAP, INITIAL_GAP, IMG_W, IMG_H);
                redraw_info_area(display, window, gc, &logic_state, current_win_w, current_win_h);

            } else if (event.type == KeyPress) {
                char buffer[1]; KeySym key;
                XLookupString(&event.xkey, buffer, sizeof(buffer), &key, NULL);
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

