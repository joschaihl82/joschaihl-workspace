/*
 main.c - qgeister1 converted to libxcb + XSHM + FreeType glyph cache text rendering
 (inkl. Debug-Prints, checked SHM attach, depth=32, startup gradient + raw-copy/color-block test)

 Build example:
 gcc -std=gnu11 -O2 -Wall -Wextra -fPIC -I/usr/include/freetype2 -o qgeister main.c \
   -lxcb -lxcb-shm -lxcb-keysyms -lfreetype -lfontconfig -lasound -lm
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <X11/keysym.h>

#include <alsa/asoundlib.h>
#include <linux/videodev2.h>

#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>
#include <xcb/xcb_icccm.h>

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

// -----------------------------------------------------------------------------
// Configuration / constants
// -----------------------------------------------------------------------------
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480

#define SAMPLE_RATE 44100
#define CLICK_DURATION_DENOMINATOR 20
#define CLICK_SAMPLES (SAMPLE_RATE / CLICK_DURATION_DENOMINATOR)
#define NUM_SCALE_NOTES 16

#define INITIAL_GAP 10
#define INFO_BAR_H 100

#define GRAPH_POINTS 150
#define GRAPH_H 80

#define SVH_TO_USVS 277.777777778
#define SVH_TO_USVH 1000000.0

#define BG_LEARN_RATE 0.005f
#define HOT_PIXEL_THRESH 200
#define FLICKER_MEMORY 5
#define GEIGER_SMOOTH_FACTOR 0.3
#define GEIGER_LONG_SMOOTH_FACTOR 0.01

#define SEP_ERROR_FRAMES_DEFAULT 30
#define SEP_ERROR_SECONDS_MIN 1.0

#define DEFAULT_FONT_FAMILY "DejaVu Sans Mono"
#define DEFAULT_FONT_SIZE_PX 14

#define GLYPH_CACHE_BUCKETS 4096

// -----------------------------------------------------------------------------
// Minimal isotope info for display
// -----------------------------------------------------------------------------
typedef struct { const char* name; double RBE; } Isotope;
static const Isotope ISOTOPES[] = {
    {"Cs-137",1.0}, {"Co-60",1.0}, {"I-131",1.0}, {"Sr-90",1.0}, {"U-238",20.0}
};

// -----------------------------------------------------------------------------
// Geiger state, buffers
// -----------------------------------------------------------------------------
typedef struct {
    double currentRad;
    double totalDoseSv;
    double currentFusedUsvS;
    double shortHistory[GRAPH_POINTS];
    double longHistory[GRAPH_POINTS];
    double doseHistory[GRAPH_POINTS];
    long long startTimeSec;
} GeigerStats;

typedef struct {
    float *bg;
    uint8_t *flicker;
    double currentRadSmooth;
    double total_usv_h;
    GeigerStats stats;
} GeigerLogicState;

static GeigerLogicState logic_state;

static uint32_t *original_frame_buffer = NULL; // camera raw (4 bytes/pixel)
static uint32_t *processed_frame_buffer = NULL; // processed before look
static uint32_t *shm_image_buffer = NULL;       // premultiplied ARGB32 for X server (shared mem if used)

// V4L2 mapping
struct V4L2Buffer { void *start; size_t length; };
static int v4l2_fd = -1;
static struct V4L2Buffer *v4l2_buffers = NULL;
static unsigned int v4l2_n_buffers = 0;

// ALSA
static snd_pcm_t *global_pcm_handle = NULL;
static short click_buffers[NUM_SCALE_NOTES][CLICK_SAMPLES];

// Timing
static double last_frame_time_sec = 0.0;

// -----------------------------------------------------------------------------
// XCB + SHM + keysyms + WM atoms
//-----------------------------------------------------------------------------
static xcb_connection_t *xcb_conn = NULL;
static xcb_screen_t *xcb_screen = NULL;
static xcb_window_t xcb_win = 0;
static xcb_gcontext_t xcb_gc = 0;
/* replaced xcb_shm_segment_info_t with explicit fields */
static int shm_shmid = -1;
static void *shm_addr = NULL;
static xcb_shm_seg_t xcb_shmseg = XCB_NONE;
static bool use_shm = false;
static xcb_key_symbols_t *keysyms = NULL;
static xcb_atom_t atom_wm_protocols = 0;
static xcb_atom_t atom_wm_delete = 0;

// -----------------------------------------------------------------------------
// FreeType / Fontconfig glyph cache structures
// -----------------------------------------------------------------------------
typedef struct Glyph {
    uint32_t codepoint;
    int w, h;           // glyph bitmap size
    int left, top;      // bearing
    int advance;        // x advance in pixels
    uint32_t *argb;     // premultiplied ARGB buffer (w*h)
    struct Glyph *next;
} Glyph;

static Glyph *glyph_cache[GLYPH_CACHE_BUCKETS];

static FT_Library ft_lib = NULL;
static FcConfig *fc_cfg = NULL;
static FT_Face global_face = NULL;
static int global_font_size_px = DEFAULT_FONT_SIZE_PX;

// -----------------------------------------------------------------------------
// Sepia learning / detection globals
// -----------------------------------------------------------------------------
static float sepia_strength_global = 0.12f;
static int sepia_mode = 1; // 0 = fixed 0.1, 1 = dynamic
static double sepia_last_update_time = 0.0;
static double sepia_learning_interval = 2.0;
static double sepia_pause_interval = 6.0;
static int sepia_in_pause = 0;
static double sepia_pause_until = 0.0;
static float sepia_candidate = 0.12f;
static float sepia_best = 0.12f;
static double sepia_best_score = -1e308;
static int sepia_epoch_count = 0;
static float sepia_learning_rate_multiplier = 0.25f;
static int sepia_error_active = 0;
static double sepia_error_until_time = 0.0;
static int sepia_error_frames_remaining = 0;

// -----------------------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------------------
static inline uint8_t rgb32_to_grayscale(uint32_t rgb);
static void hsv_to_rgb(float H, float S, float V, float *R, float *G, float *B);

void init_geiger_logic(GeigerLogicState *state);
void cleanup_geiger_logic(GeigerLogicState *state);
void process_radionullon(GeigerLogicState *state, const uint32_t *original_frame, uint32_t *processed_frame, double frame_delta_sec);

// V4L2
static int xioctl(int fh, int request, void *arg);
int v4l2_init(void);
void v4l2_close(void);
int v4l2_read_frame(uint32_t *buffer);

// ALSA
void init_click_buffers(void);
int init_alsa(void);
void play_alsa_click(int index);
void alsa_cleanup(void);

// XCB
int xcb_init_display_and_shm(void);
void xcb_cleanup(void);
void xcb_put_shm_image(void);
void xcb_draw_text_to_framebuffer(uint32_t *frame, int fb_w, int x, int y, const char *utf8);

// Font/glyph cache
static FT_Face load_face_fc(const char *pattern, int size_px);
static Glyph *glyph_cache_lookup_or_create(FT_Face face, uint32_t codepoint, uint32_t text_color_argb);
static void free_glyph_cache(void);

// Sepia learning helpers
static int detect_color_space_error(const uint8_t *src_rgba, int w, int h);
static double compute_image_quality_proxy(const uint8_t *rgba_buf, int w, int h);
static void sepia_learning_update(const uint8_t *raw_rgba_src, int w, int h, double current_time_sec);

// Conversion + look
static void convert_camera_to_shm_premultiplied_with_coldwar(const void *cam_buf, uint32_t *ximage_buf, int w, int h, float sepia_override);

// -----------------------------------------------------------------------------
// Implementations
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Geiger logic (similar to original)
// -----------------------------------------------------------------------------
void init_geiger_logic(GeigerLogicState *state) {
    int size = IMG_W * IMG_H;
    state->bg = (float*)calloc(size, sizeof(float));
    state->flicker = (uint8_t*)calloc(size, sizeof(uint8_t));
    for (int i = 0; i < size; ++i) state->bg[i] = 128.0f;
    state->currentRadSmooth = 0.0;
    state->total_usv_h = 0.0;
    memset(&state->stats, 0, sizeof(state->stats));
    struct timeval tv; gettimeofday(&tv, NULL);
    state->stats.startTimeSec = tv.tv_sec;
}

void cleanup_geiger_logic(GeigerLogicState *state) {
    if (state->bg) free(state->bg);
    if (state->flicker) free(state->flicker);
}

// process_radionullon: background subtraction, hot pixel, heatmap, dose calc
void process_radionullon(GeigerLogicState *state, const uint32_t *original_frame, uint32_t *processed_frame, double frame_delta_sec) {
    int size = IMG_W * IMG_H;
    int detections_count = 0;
    for (int i = 0; i < size; ++i) {
        uint8_t current_gs = rgb32_to_grayscale(original_frame[i]);
        float bg_val = state->bg[i];
        int residue = (int)current_gs - (int)bg_val;
        state->bg[i] = bg_val * (1.0f - BG_LEARN_RATE) + current_gs * BG_LEARN_RATE;
        if (residue > HOT_PIXEL_THRESH) {
            if (state->flicker[i] < FLICKER_MEMORY) state->flicker[i]++;
            else {
                if (state->flicker[i] == FLICKER_MEMORY) detections_count++;
                state->flicker[i] = FLICKER_MEMORY;
            }
        } else {
            if (state->flicker[i] > 0) state->flicker[i]--;
        }
        if (state->flicker[i] > 0) {
            float r_hsv, g_hsv, b_hsv;
            float flicker_ratio = (float)state->flicker[i] / FLICKER_MEMORY;
            float hue = (1.0f - flicker_ratio) * 120.0f;
            hsv_to_rgb(hue, 1.0f, 1.0f, &r_hsv, &g_hsv, &b_hsv);
            uint8_t red = (uint8_t)(r_hsv * 255.0f);
            uint8_t green = (uint8_t)(g_hsv * 255.0f);
            uint8_t blue = (uint8_t)(b_hsv * 255.0f);
            uint32_t pix = original_frame[i];
            uint8_t orig_r = (pix >> 16) & 0xFF;
            uint8_t orig_g = (pix >> 8) & 0xFF;
            uint8_t orig_b = (pix & 0xFF);
            uint8_t final_r = (orig_r / 2) + (red / 2);
            uint8_t final_g = (orig_g / 2) + (green / 2);
            uint8_t final_b = (orig_b / 2) + (blue / 2);
            processed_frame[i] = (0xFFu << 24) | (final_r << 16) | (final_g << 8) | final_b;
        } else {
            processed_frame[i] = original_frame[i];
        }
    }
    double cps = (double)detections_count / frame_delta_sec;
    double current_usvh = cps / 100.0;
    if (current_usvh < 0.05) current_usvh = 0.05;
    state->currentRadSmooth = state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR) + current_usvh * GEIGER_SMOOTH_FACTOR;
    state->total_usv_h = state->currentRadSmooth;
    double usvs_video = state->total_usv_h / 3600.0;
    state->stats.currentFusedUsvS = usvs_video * 0.97;
    state->stats.totalDoseSv += state->stats.currentFusedUsvS * frame_delta_sec;
    for (int i = 0; i < GRAPH_POINTS - 1; ++i) {
        state->stats.shortHistory[i] = state->stats.shortHistory[i+1];
        state->stats.longHistory[i] = state->stats.longHistory[i+1];
        state->stats.doseHistory[i] = state->stats.doseHistory[i+1];
    }
    state->stats.shortHistory[GRAPH_POINTS-1] = state->currentRadSmooth;
    state->stats.longHistory[GRAPH_POINTS-1] = state->stats.longHistory[GRAPH_POINTS-2] * (1.0 - GEIGER_LONG_SMOOTH_FACTOR) + state->currentRadSmooth * GEIGER_LONG_SMOOTH_FACTOR;
    state->stats.doseHistory[GRAPH_POINTS-1] = state->stats.totalDoseSv * SVH_TO_USVH;
}

// -----------------------------------------------------------------------------
// V4L2 helper functions (mmap)
static int xioctl(int fh, int request, void *arg) {
    int r;
    do { r = ioctl(fh, request, arg); } while (r == -1 && errno == EINTR);
    return r;
}

static int v4l2_init_mmap() {
    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(v4l2_fd, VIDIOC_REQBUFS, &req)) { perror("VIDIOC_REQBUFS"); return -1; }
    if (req.count < 2) { fprintf(stderr,"Insufficient V4L2 buffers\n"); return -1; }
    v4l2_buffers = calloc(req.count, sizeof(*v4l2_buffers));
    if (!v4l2_buffers) return -1;
    v4l2_n_buffers = req.count;
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (-1 == xioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf)) { perror("VIDIOC_QUERYBUF"); return -1; }
        v4l2_buffers[i].length = buf.length;
        v4l2_buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);
        if (v4l2_buffers[i].start == MAP_FAILED) { perror("mmap"); return -1; }
    }
    return 0;
}

static int v4l2_start_capturing() {
    for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (-1 == xioctl(v4l2_fd, VIDIOC_QBUF, &buf)) { perror("VIDIOC_QBUF"); return -1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(v4l2_fd, VIDIOC_STREAMON, &type)) { perror("VIDIOC_STREAMON"); return -1; }
    return 0;
}

int v4l2_init(void) {
    v4l2_fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
    if (v4l2_fd == -1) { fprintf(stderr,"Cannot open %s: %s\n", DEVICE_NODE, strerror(errno)); return -1; }
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W;
    fmt.fmt.pix.height = IMG_H;
#ifdef V4L2_PIX_FMT_RGBA32
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGBA32;
#else
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
#endif
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (-1 == xioctl(v4l2_fd, VIDIOC_S_FMT, &fmt)) {
        fprintf(stderr,"VIDIOC_S_FMT failed\n"); close(v4l2_fd); v4l2_fd=-1; return -1;
    }
    if (v4l2_init_mmap() == -1) { close(v4l2_fd); v4l2_fd=-1; return -1; }
    if (v4l2_start_capturing() == -1) { v4l2_close(); return -1; }

    size_t sz = IMG_W * IMG_H * 4;
    original_frame_buffer = malloc(sz);
    processed_frame_buffer = malloc(sz);
    if (!original_frame_buffer || !processed_frame_buffer) { fprintf(stderr,"Buffer alloc failed\n"); return -1; }
    return 0;
}

void v4l2_close(void) {
    if (v4l2_fd != -1) {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        xioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
        for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
            munmap(v4l2_buffers[i].start, v4l2_buffers[i].length);
        }
        free(v4l2_buffers); v4l2_buffers = NULL; v4l2_n_buffers = 0;
        close(v4l2_fd); v4l2_fd = -1;
    }
    if (original_frame_buffer) { free(original_frame_buffer); original_frame_buffer = NULL; }
    if (processed_frame_buffer) { free(processed_frame_buffer); processed_frame_buffer = NULL; }
    if (shm_image_buffer && !use_shm) { free(shm_image_buffer); shm_image_buffer = NULL; }
}

int v4l2_read_frame(uint32_t *buffer) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(v4l2_fd, VIDIOC_DQBUF, &buf)) {
        if (errno == EAGAIN) return -1;
        perror("VIDIOC_DQBUF"); return -1;
    }
    size_t data_len = buf.bytesused;
    size_t expected = IMG_W * IMG_H * 4;
    if (data_len > expected) data_len = expected;
    memcpy(buffer, v4l2_buffers[buf.index].start, data_len);
    if (-1 == xioctl(v4l2_fd, VIDIOC_QBUF, &buf)) { perror("VIDIOC_QBUF"); return -1; }
    return 0;
}

// -----------------------------------------------------------------------------
// ALSA and click buffers
// -----------------------------------------------------------------------------
static void generate_fm_sample(short *buffer, double freq, double amp_decay, double fm_idx) {
    double phase_c = 0.0, phase_m = 0.0;
    double c_freq = freq, m_freq = freq * 1.414;
    for (int i = 0; i < CLICK_SAMPLES; ++i) {
        double t = (double)i / SAMPLE_RATE;
        double amp = exp(-t * amp_decay);
        phase_m += 2.0 * M_PI * m_freq / SAMPLE_RATE;
        if (phase_m > 2.0*M_PI) phase_m -= 2.0*M_PI;
        double mod = fm_idx * sin(phase_m);
        phase_c += 2.0 * M_PI * c_freq / SAMPLE_RATE + mod / SAMPLE_RATE;
        if (phase_c > 2.0*M_PI) phase_c -= 2.0*M_PI;
        double sample = sin(phase_c) * amp;
        buffer[i] = (short)(sample * 32767.0 * 0.5);
    }
}

void init_click_buffers(void) {
    const int SEMITONES[NUM_SCALE_NOTES] = {0,2,3,5,7,8,10,12,14,15,17,19,20,22,24,26};
    const double BASE = 440.0, DECAY = 1500.0;
    for (int i = 0; i < NUM_SCALE_NOTES; ++i) {
        double freq = BASE * pow(2.0, (double)SEMITONES[i] / 12.0);
        double fm_idx = 10.0 - i;
        if (fm_idx < 1.0) fm_idx = 1.0;
        generate_fm_sample(click_buffers[i], freq, DECAY, fm_idx);
    }
}

int init_alsa(void) {
    int err;
    const char *pcm = "default";
    snd_pcm_hw_params_t *params;
    if ((err = snd_pcm_open(&global_pcm_handle, pcm, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr,"ALSA open failed: %s\n", snd_strerror(err)); global_pcm_handle = NULL; return -1;
    }
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(global_pcm_handle, params);
    snd_pcm_hw_params_set_access(global_pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(global_pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(global_pcm_handle, params, 1);
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(global_pcm_handle, params, &rate, 0);
    if ((err = snd_pcm_hw_params(global_pcm_handle, params)) < 0) {
        fprintf(stderr,"ALSA params failed: %s\n", snd_strerror(err)); snd_pcm_close(global_pcm_handle); global_pcm_handle = NULL; return -1;
    }
    return 0;
}

void play_alsa_click(int index) {
    if (!global_pcm_handle || index < 0 || index >= NUM_SCALE_NOTES) return;
    short *buf = click_buffers[index];
    int frames = CLICK_SAMPLES;
    int err = snd_pcm_writei(global_pcm_handle, buf, frames);
    if (err < 0) {
        if (err == -EPIPE) { snd_pcm_prepare(global_pcm_handle); snd_pcm_writei(global_pcm_handle, buf, frames); }
    }
}

void alsa_cleanup(void) {
    if (global_pcm_handle) { snd_pcm_drain(global_pcm_handle); snd_pcm_close(global_pcm_handle); global_pcm_handle = NULL; }
}

// -----------------------------------------------------------------------------
// XCB + SHM init / cleanup / put
// -----------------------------------------------------------------------------
int xcb_init_display_and_shm(void) {
    int screen_num;
    xcb_conn = xcb_connect(NULL, &screen_num);
    if (xcb_connection_has_error(xcb_conn)) { fprintf(stderr,"Cannot open XCB connection\n"); return -1; }
    const xcb_setup_t *setup = xcb_get_setup(xcb_conn);
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    for (int i = 0; i < screen_num; ++i) xcb_screen_next(&it);
    xcb_screen = it.data;

    xcb_win = xcb_generate_id(xcb_conn);
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t vals[2] = { xcb_screen->white_pixel,
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY };

    xcb_create_window(xcb_conn, XCB_COPY_FROM_PARENT, xcb_win, xcb_screen->root,
                      0, 0, IMG_W + 2*INITIAL_GAP, IMG_H + 2*INITIAL_GAP + INFO_BAR_H, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_screen->root_visual,
                      mask, vals);

    xcb_intern_atom_cookie_t c = xcb_intern_atom(xcb_conn, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *pr = xcb_intern_atom_reply(xcb_conn, c, NULL);
    xcb_intern_atom_cookie_t c2 = xcb_intern_atom(xcb_conn, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t *dm = xcb_intern_atom_reply(xcb_conn, c2, NULL);
    if (pr && dm) {
        atom_wm_protocols = pr->atom;
        atom_wm_delete = dm->atom;
        xcb_change_property(xcb_conn, XCB_PROP_MODE_REPLACE, xcb_win, atom_wm_protocols, XCB_ATOM_ATOM, 32, 1, &atom_wm_delete);
    }
    free(pr); free(dm);

    xcb_map_window(xcb_conn, xcb_win);
    xcb_flush(xcb_conn);

    xcb_gc = xcb_generate_id(xcb_conn);
    uint32_t gcvals[] = { xcb_screen->black_pixel, 0 };
    xcb_create_gc(xcb_conn, xcb_gc, xcb_win, XCB_GC_FOREGROUND, gcvals);

    xcb_shm_query_version_cookie_t shm_cookie = xcb_shm_query_version(xcb_conn);
    xcb_shm_query_version_reply_t *shm_reply = xcb_shm_query_version_reply(xcb_conn, shm_cookie, NULL);
    if (shm_reply) { use_shm = true; free(shm_reply); } else use_shm = false;

    size_t shm_bytes = IMG_W * IMG_H * 4;
    if (use_shm) {
        /* allocate and attach shared memory */
        int shmid = shmget(IPC_PRIVATE, shm_bytes, IPC_CREAT | 0600);
        if (shmid < 0) {
            perror("shmget");
            use_shm = false;
        } else {
            void *addr = shmat(shmid, 0, 0);
            if (addr == (void*)-1) {
                perror("shmat");
                shmctl(shmid, IPC_RMID, 0);
                use_shm = false;
            } else {
                /* remember shmid/addr and try XCB SHM attach with checked request */
                shm_shmid = shmid;
                shm_addr = addr;
                xcb_shmseg = xcb_generate_id(xcb_conn);
                xcb_void_cookie_t ck = xcb_shm_attach_checked(xcb_conn, xcb_shmseg, shmid, 0);
                xcb_generic_error_t *err = xcb_request_check(xcb_conn, ck);
                if (err) {
                    fprintf(stderr, "xcb_shm_attach failed: error_code=%d\n", err->error_code);
                    free(err);
                    /* detach and remove sysv shm because server refused attach */
                    shmdt(addr);
                    shmctl(shmid, IPC_RMID, 0);
                    shm_shmid = -1;
                    shm_addr = NULL;
                    xcb_shmseg = XCB_NONE;
                    use_shm = false;
                } else {
                    /* success */
                    use_shm = true;
                    shm_image_buffer = (uint32_t*)shm_addr;
                    fprintf(stderr, "XCB SHM attached (shmid=%d)\n", shmid);
                }
            }
        }
    }
    if (!use_shm) {
        shm_image_buffer = malloc(shm_bytes);
        if (!shm_image_buffer) { fprintf(stderr,"shm_image_buffer alloc fail\n"); return -1; }
        fprintf(stderr, "Falling back to malloc-backed buffer for image upload\n");
    } else {
        fprintf(stderr, "SHM attached and will be used for image upload\n");
    }

    keysyms = xcb_key_symbols_alloc(xcb_conn);
    return 0;
}

void xcb_cleanup(void) {
    if (keysyms) { xcb_key_symbols_free(keysyms); keysyms = NULL; }
    if (use_shm && xcb_shmseg != XCB_NONE) {
        xcb_shm_detach(xcb_conn, xcb_shmseg);
        xcb_flush(xcb_conn);
        if (shm_addr) { shmdt(shm_addr); shm_addr = NULL; }
        if (shm_shmid >= 0) { shmctl(shm_shmid, IPC_RMID, 0); shm_shmid = -1; }
    } else if (shm_image_buffer) {
        free(shm_image_buffer);
        shm_image_buffer = NULL;
    }
    if (xcb_conn) { xcb_disconnect(xcb_conn); xcb_conn = NULL; }
}

// Upload SHM buffer to window (premultiplied ARGB32)
void xcb_put_shm_image(void) {
    if (!xcb_conn || !shm_image_buffer) return;
    /* Use 32-bit depth for ARGB32/premultiplied buffers */
    const uint8_t depth = 32;
    if (use_shm && xcb_shmseg != XCB_NONE) {
        /* checked variant to surface X errors */
        xcb_void_cookie_t ck = xcb_shm_put_image_checked(xcb_conn, xcb_win, xcb_gc,
                          IMG_W, IMG_H,
                          INITIAL_GAP, INITIAL_GAP, 0, 0, IMG_W, IMG_H,
                          depth, XCB_IMAGE_FORMAT_Z_PIXMAP,
                          0, xcb_shmseg, 0);
        xcb_generic_error_t *err = xcb_request_check(xcb_conn, ck);
        if (err) {
            fprintf(stderr, "DEBUG: xcb_shm_put_image failed: code=%d\n", err->error_code);
            free(err);
        }
        xcb_flush(xcb_conn);
    } else {
        xcb_put_image(xcb_conn, XCB_IMAGE_FORMAT_Z_PIXMAP, xcb_win, xcb_gc,
                      IMG_W, IMG_H, INITIAL_GAP, INITIAL_GAP, 0, depth,
                      IMG_W * IMG_H * 4, (const void*)shm_image_buffer);
        xcb_flush(xcb_conn);
    }
}

// -----------------------------------------------------------------------------
// Color-space detection & sepia learning helpers
// -----------------------------------------------------------------------------
static int detect_color_space_error(const uint8_t *src_rgba, int w, int h) {
    const int SAMPLE_MAX = 2048;
    int pixels = w * h;
    int step = 1;
    int samples = pixels;
    if (pixels > SAMPLE_MAX) { step = pixels / SAMPLE_MAX; samples = SAMPLE_MAX; }
    int suspicious_count = 0;
    int counted = 0;
    for (int i = 0; i < pixels && counted < samples; i += step, ++counted) {
        const uint8_t *p = src_rgba + i*4;
        uint8_t a = p[3];
        if (a == 0) { suspicious_count++; continue; }
        uint8_t c0 = p[0], c2 = p[2];
        if ((c0 == 0 && c2 == 255) || (c0 == 255 && c2 == 0)) { suspicious_count++; continue; }
    }
    double frac = (double)suspicious_count / (double)(samples ? samples : 1);
    return frac > 0.06 ? 1 : 0;
}

static double compute_image_quality_proxy(const uint8_t *rgba_buf, int w, int h) {
    const int SAMPLE_MAX = 1024;
    int pixels = w * h;
    int step = 1;
    int samples = pixels;
    if (pixels > SAMPLE_MAX) { step = pixels / SAMPLE_MAX; samples = SAMPLE_MAX; }
    double sum = 0.0, sum2 = 0.0;
    int counted = 0;
    for (int i = 0; i < pixels && counted < samples; i += step, ++counted) {
        int idx = i * 4;
        double r = rgba_buf[idx + 0] / 255.0;
        double g = rgba_buf[idx + 1] / 255.0;
        double b = rgba_buf[idx + 2] / 255.0;
        double lum = 0.299*r + 0.587*g + 0.114*b;
        sum += lum; sum2 += lum*lum;
    }
    double mean = sum / (samples ? samples : 1);
    double var = (sum2 / (samples ? samples : 1)) - mean*mean;
    if (var < 0.0) var = 0.0;
    return var;
}

static void sepia_learning_update(const uint8_t *raw_rgba_src, int w, int h, double current_time_sec) {
    if (sepia_mode == 0) { sepia_strength_global = 0.1f; return; }

    if (!sepia_error_active) {
        if (detect_color_space_error(raw_rgba_src, w, h)) {
            sepia_error_active = 1;
            sepia_error_frames_remaining = SEP_ERROR_FRAMES_DEFAULT;
            sepia_error_until_time = current_time_sec + SEP_ERROR_SECONDS_MIN;
            sepia_learning_rate_multiplier = 1.0f;
            sepia_epoch_count = 0;
        }
    } else {
        if (sepia_error_frames_remaining > 0) sepia_error_frames_remaining--;
        if (current_time_sec >= sepia_error_until_time || sepia_error_frames_remaining <= 0) {
            sepia_error_active = 0;
            sepia_learning_rate_multiplier = 0.25f;
            sepia_last_update_time = current_time_sec;
        } else {
            sepia_strength_global = sepia_best_score > -1e307 ? sepia_best : sepia_candidate;
            return;
        }
    }

    if (sepia_in_pause) {
        if (current_time_sec >= sepia_pause_until) {
            sepia_in_pause = 0;
            sepia_last_update_time = current_time_sec;
        } else {
            sepia_strength_global = sepia_best_score > -1e307 ? sepia_best : sepia_candidate;
            return;
        }
    }

    if (sepia_last_update_time <= 0.0) sepia_last_update_time = current_time_sec;
    double effective_interval = sepia_learning_interval / (sepia_learning_rate_multiplier > 0.0f ? sepia_learning_rate_multiplier : 1.0f);
    double elapsed = current_time_sec - sepia_last_update_time;
    if (elapsed >= effective_interval) {
        float candidate = (float)(sin((double)sepia_epoch_count) * 0.1 + 0.1);
        if (candidate < 0.0f) candidate = 0.0f;
        if (candidate > 1.0f) candidate = 1.0f;
        double base_score = compute_image_quality_proxy(raw_rgba_src, w, h);
        double candidate_score = base_score * (1.0 + candidate * 0.2) - fabs(candidate - 0.12) * 0.01;
        if (candidate_score > sepia_best_score) {
            sepia_best_score = candidate_score;
            sepia_best = candidate;
        }
        sepia_candidate = candidate;
        sepia_strength_global = sepia_candidate;
        sepia_epoch_count++;
        sepia_last_update_time = current_time_sec;
        if ((sepia_epoch_count % 6) == 0) {
            sepia_in_pause = 1;
            sepia_pause_until = current_time_sec + sepia_pause_interval;
            sepia_strength_global = sepia_best;
        }
    } else {
        sepia_strength_global = sepia_best_score > -1e307 ? sepia_best : sepia_candidate;
    }
}

// -----------------------------------------------------------------------------
// Convert camera buffer (RGBA or BGRA) to premultiplied ARGB32 and apply Cold-War sepia look.
// -----------------------------------------------------------------------------
static void convert_camera_to_shm_premultiplied_with_coldwar(const void *cam_buf_void, uint32_t *ximage_buf, int w, int h, float sepia_override) {
    const uint8_t *src = (const uint8_t*)cam_buf_void;
    int pixels = w * h;
    static int ordering = 0; // 0=unknown,1=RGBA,2=BGRA

    /* DEBUG: sample src pixel before conversion */
    if (pixels > 0) {
        uint32_t s0 = ((const uint32_t*)cam_buf_void)[0];
        fprintf(stderr, "DEBUG: convert start src0=0x%08x ordering=%d\n", s0, ordering);
    }

    if (ordering == 0) {
        const int SAMPLE_MAX = 1024;
        int step = 1;
        int samples = pixels;
        if (pixels > SAMPLE_MAX) { step = pixels / SAMPLE_MAX; samples = SAMPLE_MAX; }
        long long sum = 0;
        int counted = 0;
        for (int i = 0; i < pixels && counted < samples; i += step, ++counted) {
            uint8_t r0 = src[4*i + 0];
            uint8_t b0 = src[4*i + 2];
            sum += (int)r0 - (int)b0;
        }
        double avg = (double)sum / (samples ? samples : 1);
        ordering = (avg > 0.0) ? 1 : 2;
    }

    const float desaturate_amount = 0.55f;
    const float sepia_strength_base = sepia_override;
    const float teal_strength = 0.10f;
    const float contrast_factor = 1.08f;
    const float gamma = 0.95f;

    for (int i = 0; i < pixels; ++i) {
        uint8_t r,g,b,a;
        if (ordering == 1) {
            r = src[4*i + 0]; g = src[4*i + 1]; b = src[4*i + 2]; a = src[4*i + 3];
        } else {
            b = src[4*i + 0]; g = src[4*i + 1]; r = src[4*i + 2]; a = src[4*i + 3];
        }
        if (a == 0) a = 0xFF;

        float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
        float lum = 0.299f*rf + 0.587f*gf + 0.114f*bf;
        rf = rf * (1.0f - desaturate_amount) + lum * desaturate_amount;
        gf = gf * (1.0f - desaturate_amount) + lum * desaturate_amount;
        bf = bf * (1.0f - desaturate_amount) + lum * desaturate_amount;

        float shadow_weight = (1.0f - lum);
        float highlight_weight = lum;

        rf = rf * (1.0f - teal_strength * shadow_weight) + (rf * 0.88f) * (teal_strength * shadow_weight);
        gf = gf * (1.0f - teal_strength * shadow_weight) + (gf * 1.02f) * (teal_strength * shadow_weight);
        bf = bf * (1.0f - teal_strength * shadow_weight) + (bf * 1.10f) * (teal_strength * shadow_weight);

        float sr = (rf * 0.393f + gf * 0.769f + bf * 0.189f);
        float sg = (rf * 0.349f + gf * 0.686f + bf * 0.168f);
        float sb = (rf * 0.272f + gf * 0.534f + bf * 0.131f);

        rf = rf * (1.0f - sepia_strength_base * highlight_weight) + sr * (sepia_strength_base * highlight_weight);
        gf = gf * (1.0f - sepia_strength_base * highlight_weight) + sg * (sepia_strength_base * highlight_weight);
        bf = bf * (1.0f - sepia_strength_base * highlight_weight) + sb * (sepia_strength_base * highlight_weight);

        rf = ((rf - 0.5f) * contrast_factor) + 0.5f;
        gf = ((gf - 0.5f) * contrast_factor) + 0.5f;
        bf = ((bf - 0.5f) * contrast_factor) + 0.5f;

        if (rf < 0.0f) rf = 0.0f;
        if (gf < 0.0f) gf = 0.0f;
        if (bf < 0.0f) bf = 0.0f;

        if (rf > 1.0f) rf = 1.0f;
        if (gf > 1.0f) gf = 1.0f;
        if (bf > 1.0f) bf = 1.0f;

        rf = powf(rf, gamma); gf = powf(gf, gamma); bf = powf(bf, gamma);

        float mid = 0.5f;
        float lift = 0.01f * (1.0f - fabsf(lum - mid)/mid);
        rf += lift; gf += lift; bf += lift;

        if (rf < 0.0f) rf = 0.0f;
        if (gf < 0.0f) gf = 0.0f;
        if (bf < 0.0f) bf = 0.0f;

        if (rf > 1.0f) rf = 1.0f;
        if (gf > 1.0f) gf = 1.0f;
        if (bf > 1.0f) bf = 1.0f;

        uint8_t out_r = (uint8_t)(rf * 255.0f + 0.5f);
        uint8_t out_g = (uint8_t)(gf * 255.0f + 0.5f);
        uint8_t out_b = (uint8_t)(bf * 255.0f + 0.5f);

        float af = (float)a / 255.0f;
        uint8_t prem_r = (uint8_t)(out_r * af + 0.5f);
        uint8_t prem_g = (uint8_t)(out_g * af + 0.5f);
        uint8_t prem_b = (uint8_t)(out_b * af + 0.5f);

        ximage_buf[i] = ((uint32_t)a << 24) | ((uint32_t)prem_r << 16) | ((uint32_t)prem_g << 8) | (uint32_t)prem_b;
    }

    /* DEBUG: sample dst pixel after conversion */
    if (pixels > 0) {
        fprintf(stderr, "DEBUG: convert done dst0=0x%08x sepia=%.3f\n", ximage_buf[0], sepia_override);
    }
}

// -----------------------------------------------------------------------------
// FreeType + Fontconfig helpers and glyph cache
// -----------------------------------------------------------------------------
static FT_Face load_face_fc(const char *pattern, int size_px) {
    if (!ft_lib) return NULL;
    FcPattern *pat = FcNameParse((const FcChar8*)pattern);
    if (!pat) return NULL;
    FcConfigSubstitute(NULL, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);
    FcResult result;
    FcPattern *font = FcFontMatch(NULL, pat, &result);
    if (!font) { FcPatternDestroy(pat); return NULL; }
    FcChar8 *file = NULL;
    if (FcPatternGetString(font, FC_FILE, 0, &file) != FcResultMatch) { FcPatternDestroy(font); FcPatternDestroy(pat); return NULL; }
    FT_Face face = NULL;
    if (FT_New_Face(ft_lib, (const char*)file, 0, &face) != 0) { FcPatternDestroy(font); FcPatternDestroy(pat); return NULL; }
    FT_Set_Pixel_Sizes(face, 0, size_px);
    FcPatternDestroy(font);
    FcPatternDestroy(pat);
    return face;
}

static inline uint32_t glyph_cache_hash(uint32_t codepoint) {
    return (codepoint * 2654435761u) & (GLYPH_CACHE_BUCKETS - 1);
}

static Glyph *glyph_cache_create(FT_Face face, uint32_t codepoint, uint32_t text_color_argb) {
    if (!face) return NULL;
    if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER)) return NULL;
    FT_GlyphSlot slot = face->glyph;
    FT_Bitmap *bm = &slot->bitmap;
    int w = bm->width, h = bm->rows;
    Glyph *g = calloc(1, sizeof(Glyph));
    if (!g) return NULL;
    g->codepoint = codepoint;
    g->w = w; g->h = h;
    g->left = slot->bitmap_left;
    g->top = slot->bitmap_top;
    g->advance = slot->advance.x >> 6;
    g->argb = NULL;
    if (w > 0 && h > 0) {
        g->argb = malloc(w * h * 4);
        if (!g->argb) { free(g); return NULL; }
        uint8_t tc_a = (text_color_argb >> 24) & 0xFF;
        uint8_t tc_r = (text_color_argb >> 16) & 0xFF;
        uint8_t tc_g = (text_color_argb >> 8) & 0xFF;
        uint8_t tc_b = (text_color_argb) & 0xFF;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                uint8_t glyph_a = bm->buffer[y * bm->pitch + x];
                float glyph_af = glyph_a / 255.0f;
                float overall_af = (tc_a / 255.0f) * glyph_af;
                uint8_t out_a = (uint8_t)(overall_af * 255.0f + 0.5f);
                uint8_t out_r = (uint8_t)(tc_r * overall_af + 0.5f);
                uint8_t out_g = (uint8_t)(tc_g * overall_af + 0.5f);
                uint8_t out_b = (uint8_t)(tc_b * overall_af + 0.5f);
                g->argb[y * w + x] = ((uint32_t)out_a << 24) | ((uint32_t)out_r << 16) | ((uint32_t)out_g << 8) | out_b;
            }
        }
    }
    return g;
}

static Glyph *glyph_cache_lookup_or_create(FT_Face face, uint32_t codepoint, uint32_t text_color_argb) {
    uint32_t idx = glyph_cache_hash(codepoint);
    Glyph *iter = glyph_cache[idx];
    while (iter) { if (iter->codepoint == codepoint) return iter; iter = iter->next; }
    Glyph *g = glyph_cache_create(face, codepoint, text_color_argb);
    if (!g) return NULL;
    g->next = glyph_cache[idx];
    glyph_cache[idx] = g;
    return g;
}

static void free_glyph_cache(void) {
    for (int i = 0; i < GLYPH_CACHE_BUCKETS; ++i) {
        Glyph *g = glyph_cache[i];
        while (g) {
            Glyph *n = g->next;
            if (g->argb) free(g->argb);
            free(g);
            g = n;
        }
        glyph_cache[i] = NULL;
    }
}

static inline void blit_premultiplied(uint32_t *dst, int dst_w, int dx, int dy, Glyph *g) {
    if (!g || g->w <= 0 || g->h <= 0) return;
    int gw = g->w, gh = g->h;
    for (int gy = 0; gy < gh; ++gy) {
        int dst_y = dy + gy;
        if (dst_y < 0 || dst_y >= IMG_H) continue;
        uint32_t *dst_row = dst + dst_y * dst_w + dx;
        uint32_t *src_row = g->argb + gy * gw;
        for (int gx = 0; gx < gw; ++gx) {
            int dst_x = dx + gx;
            if (dst_x < 0 || dst_x >= dst_w) continue;
            uint32_t s = src_row[gx];
            uint8_t sa = (s >> 24) & 0xFF;
            if (sa == 0) continue;
            if (sa == 255) { dst_row[gx] = s; continue; }
            uint8_t sr = (s >> 16) & 0xFF;
            uint8_t sg = (s >> 8) & 0xFF;
            uint8_t sb = s & 0xFF;
            uint32_t d = dst_row[gx];
            uint8_t dr = (d >> 16) & 0xFF;
            uint8_t dg = (d >> 8) & 0xFF;
            uint8_t db = d & 0xFF;
            uint8_t da = (d >> 24) & 0xFF;
            uint8_t out_r = (uint8_t)(sr + ((dr * (255 - sa) + 127) / 255));
            uint8_t out_g = (uint8_t)(sg + ((dg * (255 - sa) + 127) / 255));
            uint8_t out_b = (uint8_t)(sb + ((db * (255 - sa) + 127) / 255));
            uint8_t out_a = (uint8_t)(sa + ((da * (255 - sa) + 127) / 255));
            dst_row[gx] = ((uint32_t)out_a << 24) | ((uint32_t)out_r << 16) | ((uint32_t)out_g << 8) | out_b;
        }
    }
}

void xcb_draw_text_to_framebuffer(uint32_t *frame, int fb_w, int x, int y_baseline, const char *utf8) {
    if (!frame || !utf8 || !global_face) return;
    int pen_x = x;
    FT_UInt previous = 0;
    const unsigned char *p = (const unsigned char*)utf8;
    while (*p) {
        uint32_t ch;
        if (*p < 0x80) { ch = *p++; }
        else {
            if ((*p & 0xE0) == 0xC0) { ch = ((*p & 0x1F) << 6) | (p[1] & 0x3F); p += 2; }
            else if ((*p & 0xF0) == 0xE0) { ch = ((*p & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F); p += 3; }
            else { ch = '?'; ++p; }
        }
        FT_UInt glyph_index = FT_Get_Char_Index(global_face, ch);
        if (previous && glyph_index) {
            FT_Vector delta;
            FT_Get_Kerning(global_face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
            pen_x += delta.x >> 6;
        }
        Glyph *g = glyph_cache_lookup_or_create(global_face, ch, 0xFFFFFFFFu);
        if (g) {
            int dx = pen_x + g->left;
            int dy = y_baseline - g->top;
            blit_premultiplied(frame, fb_w, dx, dy, g);
            pen_x += g->advance;
        }
        previous = glyph_index;
    }
}

// -----------------------------------------------------------------------------
// Signal handler & cleanup
// -----------------------------------------------------------------------------
void sigint_handler(int sig) {
    (void)sig;
    fprintf(stderr, "Exiting...\n");
    alsa_cleanup();
    v4l2_close();
    xcb_cleanup();
    cleanup_geiger_logic(&logic_state);
    free_glyph_cache();
    if (global_face) { FT_Done_Face(global_face); global_face = NULL; }
    if (ft_lib) { FT_Done_FreeType(ft_lib); ft_lib = NULL; }
    FcFini();
    exit(0);
}

// -----------------------------------------------------------------------------
// Debug: startup gradient
// -----------------------------------------------------------------------------
static void draw_startup_debug_pattern(void) {
    if (!shm_image_buffer || !xcb_conn) return;
    for (int y = 0; y < IMG_H; ++y) {
        for (int x = 0; x < IMG_W; ++x) {
            uint8_t r = (uint8_t)(x * 255 / (IMG_W > 1 ? IMG_W - 1 : 1));
            uint8_t g = (uint8_t)(y * 255 / (IMG_H > 1 ? IMG_H - 1 : 1));
            uint8_t b = (uint8_t)((x + y) & 0xFF);
            uint32_t pix = (0xFFu << 24) | (r << 16) | (g << 8) | b; // opaque ARGB
            shm_image_buffer[y * IMG_W + x] = pix;
        }
    }

    if (use_shm && xcb_shmseg != XCB_NONE) {
        xcb_shm_put_image(xcb_conn, xcb_win, xcb_gc,
                          IMG_W, IMG_H,
                          0, 0, 0, 0, IMG_W, IMG_H,
                          32, XCB_IMAGE_FORMAT_Z_PIXMAP,
                          0, xcb_shmseg, 0);
    } else {
        xcb_put_image(xcb_conn, XCB_IMAGE_FORMAT_Z_PIXMAP, xcb_win, xcb_gc,
                      IMG_W, IMG_H, 0, 0, 0, 32,
                      IMG_W * IMG_H * 4, (const void*)shm_image_buffer);
    }
    xcb_flush(xcb_conn);
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    signal(SIGINT, sigint_handler);

    init_click_buffers();
    init_geiger_logic(&logic_state);

    if (FT_Init_FreeType(&ft_lib)) { fprintf(stderr,"FT_Init_FreeType failed\n"); }
    FcInit();
    global_face = load_face_fc(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE_PX);
    global_font_size_px = DEFAULT_FONT_SIZE_PX;
    if (!global_face) { fprintf(stderr,"Failed to load font '%s' size %d\n", DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE_PX); }

    if (v4l2_init() != 0) { fprintf(stderr,"V4L2 init failed\n"); cleanup_geiger_logic(&logic_state); return 1; }
    if (xcb_init_display_and_shm() != 0) { v4l2_close(); cleanup_geiger_logic(&logic_state); return 1; }

    // Debug: show startup gradient immediately
    draw_startup_debug_pattern();

    // >>> DEBUG: one-time startup raw-copy / color-blocks test
    if (original_frame_buffer && shm_image_buffer) {
        bool src_nonzero = false;
        for (int i = 0; i < 16; ++i) { if (original_frame_buffer[i] != 0) { src_nonzero = true; break; } }
        if (src_nonzero) {
            memcpy(shm_image_buffer, original_frame_buffer, IMG_W * IMG_H * 4);
            fprintf(stderr, "DEBUG: startup raw-copy from original_frame_buffer -> shm_image_buffer\n");
            xcb_put_shm_image();
            xcb_flush(xcb_conn);
            sleep(1);
        } else {
            for (int y = 0; y < IMG_H; ++y) {
                for (int x = 0; x < IMG_W; ++x) {
                    uint32_t col;
                    if (x < IMG_W/3) col = 0xFFFF0000u;
                    else if (x < 2*IMG_W/3) col = 0xFF00FF00u;
                    else col = 0xFF0000FFu;
                    shm_image_buffer[y*IMG_W + x] = col;
                }
            }
            fprintf(stderr, "DEBUG: startup color-blocks drawn\n");
            xcb_put_shm_image();
            xcb_flush(xcb_conn);
            sleep(1);
        }
    }
    // <<< end debug test

    if (!shm_image_buffer) {
        fprintf(stderr,"No shm image buffer available\n");
        v4l2_close(); xcb_cleanup(); cleanup_geiger_logic(&logic_state); return 1;
    }

    if (init_alsa() == 0) play_alsa_click(0);
    else fprintf(stderr,"Audio init failed (no clicking)\n");

    int running = 1;
    while (running) {
        fd_set fds; FD_ZERO(&fds); FD_SET(v4l2_fd, &fds);
        struct timeval tv = {0, 33000};
        int sel = select(v4l2_fd + 1, &fds, NULL, NULL, &tv);

        struct timeval tv_now; gettimeofday(&tv_now, NULL);
        double current_time_sec = tv_now.tv_sec + tv_now.tv_usec / 1000000.0;
        double frame_delta_sec = 1.0 / 30.0;
        if (last_frame_time_sec > 0.0) {
            frame_delta_sec = current_time_sec - last_frame_time_sec;
            if (frame_delta_sec <= 0.0) frame_delta_sec = 1.0/30.0;
        }
        last_frame_time_sec = current_time_sec;

        if (sel > 0 && FD_ISSET(v4l2_fd, &fds)) {
            if (v4l2_read_frame(original_frame_buffer) == 0) {
                /* DEBUG: log a few sample pixels from camera source */
                fprintf(stderr, "DEBUG: v4l2 frame sample: src[0]=0x%08x src[1]=0x%08x src[mid]=0x%08x\n",
                        original_frame_buffer[0], original_frame_buffer[1], original_frame_buffer[(IMG_H/2)*IMG_W + (IMG_W/2)]);

                process_radionullon(&logic_state, original_frame_buffer, processed_frame_buffer, frame_delta_sec);

                sepia_learning_update((const uint8_t*)processed_frame_buffer, IMG_W, IMG_H, current_time_sec);

                convert_camera_to_shm_premultiplied_with_coldwar(processed_frame_buffer, shm_image_buffer, IMG_W, IMG_H, sepia_strength_global);

                char line[256];
                snprintf(line, sizeof(line), "CURRENT (uSv/h): %.3f", logic_state.total_usv_h);
                xcb_draw_text_to_framebuffer(shm_image_buffer, IMG_W, INITIAL_GAP, IMG_H + INITIAL_GAP + 16, line);
                snprintf(line, sizeof(line), "TOTAL DOSE (uSv): %.6f", logic_state.stats.totalDoseSv * SVH_TO_USVH);
                xcb_draw_text_to_framebuffer(shm_image_buffer, IMG_W, INITIAL_GAP, IMG_H + INITIAL_GAP + 32, line);
                snprintf(line, sizeof(line), "SEPIA: %.3f MODE:%s M:%.2f ERR:%d",
                         sepia_strength_global, (sepia_mode==0)?"FIX":"DYN", sepia_learning_rate_multiplier, sepia_error_active ? sepia_error_frames_remaining : 0);
                xcb_draw_text_to_framebuffer(shm_image_buffer, IMG_W, INITIAL_GAP + 300, IMG_H + INITIAL_GAP + 16, line);

                xcb_put_shm_image();

                if (logic_state.stats.currentFusedUsvS * 3600.0 > 0.1 && global_pcm_handle != NULL) {
                    int click_index = (int)(logic_state.stats.currentFusedUsvS * 3600.0 * 100.0) % NUM_SCALE_NOTES;
                    if (click_index < 0) click_index = 0;
                    play_alsa_click(click_index);
                }
            }
        } else if (sel == -1) {
            if (errno != EINTR) { perror("select"); running = 0; }
        }

        xcb_generic_event_t *ev;
        while ((ev = xcb_poll_for_event(xcb_conn))) {
            uint8_t r = ev->response_type & ~0x80;
            if (r == XCB_EXPOSE) {
                // nothing special; redraw per frame
            } else if (r == XCB_KEY_PRESS) {
                xcb_key_press_event_t *kp = (xcb_key_press_event_t*)ev;
                xcb_keysym_t ks = xcb_key_symbols_get_keysym(keysyms, kp->detail, 0);
                if (ks == XK_Escape) running = 0;
                else if (ks == XK_d || ks == XK_D) {
                    sepia_mode = (sepia_mode == 0) ? 1 : 0;
                    if (sepia_mode == 1) { sepia_epoch_count = 0; sepia_best_score = -1e308; sepia_in_pause = 0; sepia_last_update_time = 0.0; }
                } else if (ks == XK_r || ks == XK_R) {
                    sepia_strength_global = 0.12f; sepia_best = sepia_strength_global; sepia_best_score = -1e308;
                } else if (ks == XK_1) sepia_learning_rate_multiplier = 1.0f;
                else if (ks == XK_2) sepia_learning_rate_multiplier = 0.25f;
            } else if (r == XCB_CONFIGURE_NOTIFY) {
                // handle resize if desired
            } else if (r == XCB_CLIENT_MESSAGE) {
                xcb_client_message_event_t *cm = (xcb_client_message_event_t*)ev;
                if (cm->data.data32[0] == atom_wm_delete) running = 0;
            }
            free(ev);
        }
    }

    sigint_handler(0);
    return 0;
}


