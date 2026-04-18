/*
 * geigern.c (vollständig, 640x480 @ highest framerate)
 *
 * Features:
 * - V4L2: fordert 640x480 und verhandelt vor dem Streaming die höchste unterstützte Bildrate
 * - Eingabe bevorzugt ARGB32 mit Fallbacks: RGBA32, ABGR32, BGRA32, XRGB32, RGB24, YUYV, MJPEG
 * - Ausgabe immer 32‑Bit ARGB (Alpha=0xFF)
 * - HSV‑Neon pro HOT‑Pixel (jede/r bekommt eigene Farbe via Hash)
 * - Vollständige Isotop‑Namen im Panel
 * - Dynamisches Layout: passt sich Fenstergröße (Resize/Maximize) an
 * - Sofortiges Redraw bei Resize/Expose (auch ohne neues Frame)
 *
 * Build:
 * cc -std=c99 -O2 -march=native geigern.c -o geigern -pthread -ljpeg -lasound -lxcb -lm -lxcb-shm -lxcb-keysyms -lxcb-image
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

// XCB and IPC includes
#include <sys/ipc.h> // For IPC_PRIVATE, IPC_CREAT, IPC_RMID
#include <sys/shm.h> // For shmget, shmat, shmctl
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/shm.h> // Required for xcb_shm_seg_t and SHM functions
#include <xcb/xcb_image.h> // For xcb_image_t, xcb_image_put etc.
#include <xcb/xcb_keysyms.h> // For xcb_key_symbols_t and xcb_key_symbols_get_keysym

#include <alsa/asoundlib.h>
#include <linux/videodev2.h>
#include <jpeglib.h>

#if defined(__x86_64__) || defined(__i386__)
#include <emmintrin.h>
#endif

/* ---------- config ---------- */
#define DEVICE_NODE "/dev/video0"
/* Fixed requested resolution 640x480 */
static int SRC_WIDTH = 640;
static int SRC_HEIGHT = 480;
#define FPS 25

#define Y_HOT_THRESH 200
#define RAD_SCALE 0.00005
#define ALERT_THRESHOLD 0.05
#define ALARM_COOLDOWN_MS 1000

#define ALPHA_MIN 0.05
#define ALPHA_MAX 0.50
#define AEMA_SCALE_FACTOR 1.0

#define NUM_ISOTOPES 20
#define HIGH_RISK_CHANCE_TOTAL 5
#define MEDIUM_RISK_CHANCE_TOTAL 15

#define LD50_LOW_RISK_THRESHOLD 100.0
#define LD50_MEDIUM_RISK_THRESHOLD 50.0
#define LD50_HIGH_RISK_THRESHOLD 10.0
#define LD50_ALARM_FREQ 440.0
#define LD50_ALARM_DURATION 0.5

#define MAX_INTENSITY_FOR_PITCH 0.1
#define BASE_PITCH_HZ 150.0
#define ISOTOPE_FREQ_STEP_HZ 12.0
#define ALARM_FREQ 1000.0
#define ALARM_DURATION 0.05

#define ALSA_DEVICE "default"
#define ALSA_RATE 44100
#define ALSA_CHANNELS 1
#define ALERT_VOLUME 16000.0

#define PANEL_RATIO 0.38
#define PANEL_MIN_W 320
#define PANEL_MAX_W 980

#define FRAME_QUEUE_SIZE 8
#define LSBFirst 0

/* ---------- globals & types ---------- */
static volatile bool running = true;

typedef enum {
    PF_ARGB32,  /* 4 bytes/pixel, A,R,G,B */
    PF_RGBA32,  /* 4 bytes/pixel, R,G,B,A */
    PF_ABGR32,  /* 4 bytes/pixel, A,B,G,R */
    PF_BGRA32,  /* 4 bytes/pixel, B,G,R,A */
    PF_XRGB32,  /* 4 bytes/pixel, X,R,G,B */
    PF_RGB24,   /* 3 bytes/pixel, R,G,B */
    PF_YUYV,    /* 4:2:2 packed YUYV */
    PF_MJPEG
} pixfmt_t;

typedef struct { int width, height; bool simulated; } video_state_t;
static video_state_t vstate;

typedef struct { unsigned char *buf; size_t len; pixfmt_t pf; long long ts_ms; } frame_item_t;
typedef struct {
    frame_item_t items[FRAME_QUEUE_SIZE];
    int head, tail, count;
    pthread_mutex_t mtx;
    pthread_cond_t cv;
} frame_queue_t;
static frame_queue_t fqueue = { .head=0, .tail=0, .count=0, .mtx=PTHREAD_MUTEX_INITIALIZER, .cv=PTHREAD_COND_INITIALIZER };

// Conflict fix: Rename struct to avoid conflict with xcb_image.h
typedef struct {
    int shmid;
    xcb_shm_seg_t shmseg;
} geiger_shm_segment_info_t;

typedef struct {
    xcb_connection_t *conn;
    const xcb_setup_t *setup;
    xcb_screen_t *screen;
    xcb_window_t win;
    xcb_gcontext_t gc_text;
    xcb_gcontext_t gc_blit;
    xcb_pixmap_t back_buffer;
    int win_w, win_h;
    int depth;
    xcb_atom_t atom_wm_protocols;
    xcb_atom_t atom_wm_delete_window;
    xcb_colormap_t colormap;
    geiger_shm_segment_info_t shminfo;
    uint32_t *shm_buffer;
    xcb_key_symbols_t *keysyms;
} xcb_state_t;
static xcb_state_t xstate;

typedef struct {
    double current_rad_value;
    double filtered_rad_value;
    double accumulated_dosage;
    double dynamic_alpha;
    int hot_pixels;
    long long start_time_ms;
    long long last_alert_time_ms;
    bool ld50_exceeded;
    double ld50_threshold;
    double isotope_rad_rate[NUM_ISOTOPES];
    double isotope_accumulated_dosage[NUM_ISOTOPES];
    double isotope_ld50_threshold[NUM_ISOTOPES];
    bool isotope_ld50_exceeded[NUM_ISOTOPES];
    double alert_on_threshold;
    double alert_off_threshold;
    bool alert_active;
    long long alert_hold_ms;
    long long alert_last_change_ms;
    double ld50_on_ratio;
    double ld50_off_ratio;
} geiger_state_t;
static geiger_state_t gstate;

typedef struct { snd_pcm_t *pcm; unsigned int rate; int channels; pthread_mutex_t mtx; } alsa_state_t;
static alsa_state_t astate = { NULL, ALSA_RATE, ALSA_CHANNELS, PTHREAD_MUTEX_INITIALIZER };

/* isotope names */
static const char *isotope_abbr[NUM_ISOTOPES] = {
    "K-40","C-14","H-3","Ra-226","Rn-222","U-238","Po-210",
    "Cs-137","Sr-90","I-131","Tc-99m","Ba-137m","Cl-36","Na-22",
    "Pu-239","Am-241","Co-60","U-235","Th-232","I-129"
};
static const char *isotope_name[NUM_ISOTOPES] = {
    "Potassium-40","Carbon-14","Tritium (H-3)","Radium-226","Radon-222","Uranium-238","Polonium-210",
    "Cesium-137","Strontium-90","Iodine-131","Technetium-99m","Barium-137m","Chlorine-36","Sodium-22",
    "Plutonium-239","Americium-241","Cobalt-60","Uranium-235","Thorium-232","Iodine-129"
};
static int isotope_count[NUM_ISOTOPES];

/* Neon palette (RGB 0-255) */
static const uint8_t neon_rgb[NUM_ISOTOPES][3] = {
    {57, 255, 20},{58, 227, 255},{255, 20, 147},{255, 158, 0},{180, 0, 255},
    {0, 255, 200},{255, 0, 96},{0, 255, 64},{0, 120, 255},{255, 255, 0},
    {255, 0, 200},{120, 255, 180},{200, 100, 255},{255, 90, 20},{0, 255, 128},
    {255, 50, 50},{60, 255, 240},{255, 200, 0},{180, 255, 60},{220, 100, 255}
};
static uint32_t neon_pixel[NUM_ISOTOPES];

/* video buffer (RGB24) for processing */
static uint8_t *video_rgb24 = NULL;
static int video_stride_rgb24 = 0;

/* ---------- layout ---------- */
typedef struct {
    int panel_w;
    int left_w;
    int vid_w, vid_h;
    int video_x, video_y;
    int hud_x, hud_y;
    int panel_x;
} layout_t;
static layout_t layout;
static bool preserve_aspect = true;
static bool need_redraw = true;

static void recalc_layout(void) {
    layout.panel_w = (int)lrint(xstate.win_w * PANEL_RATIO);
    if (layout.panel_w < PANEL_MIN_W) layout.panel_w = PANEL_MIN_W;
    if (layout.panel_w > PANEL_MAX_W) layout.panel_w = PANEL_MAX_W;
    if (layout.panel_w > xstate.win_w - 40) layout.panel_w = xstate.win_w - 40;
    if (layout.panel_w < 0) layout.panel_w = 0;

    layout.left_w = xstate.win_w - layout.panel_w;
    if (layout.left_w < 64) layout.left_w = 64;

    double src_aspect = (double)SRC_WIDTH / (double)SRC_HEIGHT;
    layout.vid_w = layout.left_w;
    layout.vid_h = xstate.win_h;

    if (preserve_aspect) {
        double dst_aspect = (double)layout.left_w / (double)xstate.win_h;
        if (dst_aspect > src_aspect) {
            layout.vid_h = xstate.win_h;
            layout.vid_w = (int)lrint(src_aspect * layout.vid_h);
            if (layout.vid_w > layout.left_w) layout.vid_w = layout.left_w;
        } else {
            layout.vid_w = layout.left_w;
            layout.vid_h = (int)lrint(layout.vid_w / src_aspect);
            if (layout.vid_h > xstate.win_h) layout.vid_h = xstate.win_h;
        }
    }

    layout.video_x = (layout.left_w - layout.vid_w) / 2;
    if (layout.video_x < 0) layout.video_x = 0;
    layout.video_y = (xstate.win_h - layout.vid_h) / 2;
    if (layout.video_y < 0) layout.video_y = 0;

    layout.hud_x = layout.video_x + 10;
    layout.hud_y = layout.video_y + 20;

    layout.panel_x = layout.left_w + 10;
}

static void clear_back_buffer(uint32_t pixel) {
    xcb_change_gc(xstate.conn, xstate.gc_blit, XCB_GC_FOREGROUND, &pixel);
    xcb_rectangle_t rect = { .x = 0, .y = 0, .width = (uint16_t)xstate.win_w, .height = (uint16_t)xstate.win_h };
    xcb_poly_fill_rectangle(xstate.conn, xstate.back_buffer, xstate.gc_blit, 1, &rect);
}

/* ---------- utils ---------- */
static double now_seconds(void) { struct timeval tv; gettimeofday(&tv, NULL); return tv.tv_sec + tv.tv_usec*1e-6; }
static void sigint_handler(int s){ (void)s; running = false; }
static void format_time_ms(long long ms, char *buf, size_t len) {
    long long totalSeconds = ms/1000;
    long long totalMinutes = totalSeconds/60;
    long long totalHours = totalMinutes/60;
    long long totalDays = totalHours/24;
    long long currentMS = ms % 1000;
    long long currentSeconds = totalSeconds % 60;
    long long currentMinutes = totalMinutes % 60;
    long long currentHours = totalHours % 24;
    snprintf(buf,len,"%02lld:%02lld:%02lld:%02lld:%03lld", totalDays, currentHours, currentMinutes, currentSeconds, currentMS);
}

/* ---------- ALSA ---------- */
static short *generate_click(double freq, double duration, size_t *frames_out) {
    size_t n = (size_t)(duration * astate.rate);
    if (n == 0) { *frames_out = 0; return NULL; }
    short *buf = malloc(n * sizeof(short));
    if (!buf) return NULL;
    double phase = 0.0, inc = 2.0 * M_PI * freq / (double)astate.rate;
    for (size_t i = 0; i < n; ++i) {
        double t = (double)i / (double)astate.rate;
        double env = exp(-t * 500.0);
        double s = sin(phase) * env;
        buf[i] = (short)(s * ALERT_VOLUME);
        phase += inc;
    }
    *frames_out = n;
    return buf;
}
static bool alsa_init_persistent(void) {
    snd_pcm_t *pcm; snd_pcm_hw_params_t *params;
    if (snd_pcm_open(&pcm, ALSA_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0) { astate.pcm = NULL; return false; }
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, astate.channels);
    unsigned int r = astate.rate; snd_pcm_hw_params_set_rate_near(pcm, params, &r, 0);
    snd_pcm_hw_params(pcm, params); snd_pcm_hw_params_free(params);
    astate.pcm = pcm; pthread_mutex_init(&astate.mtx, NULL);
    return true;
}
static void alsa_close_persistent(void) {
    pthread_mutex_lock(&astate.mtx);
    if (astate.pcm) { snd_pcm_drain(astate.pcm); snd_pcm_close(astate.pcm); astate.pcm = NULL; }
    pthread_mutex_unlock(&astate.mtx);
    pthread_mutex_destroy(&astate.mtx);
}
static void play_click(double freq, double duration) {
    size_t frames; short *buf = generate_click(freq, duration, &frames); if (!buf) return;
    pthread_mutex_lock(&astate.mtx);
    if (astate.pcm) {
        snd_pcm_sframes_t rc = snd_pcm_writei(astate.pcm, buf, frames);
        if (rc < 0) snd_pcm_recover(astate.pcm, rc, 1);
    } else {
        snd_pcm_t *pcm = NULL;
        if (snd_pcm_open(&pcm, ALSA_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) == 0) {
            snd_pcm_hw_params_t *params; snd_pcm_hw_params_malloc(&params); snd_pcm_hw_params_any(pcm, params);
            snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
            snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
            snd_pcm_hw_params_set_channels(pcm, params, astate.channels);
            unsigned int r = astate.rate; snd_pcm_hw_params_set_rate_near(pcm, params, &r, 0);
            snd_pcm_hw_params(pcm, params); snd_pcm_hw_params_free(params);
            snd_pcm_writei(pcm, buf, frames); snd_pcm_drain(pcm); snd_pcm_close(pcm);
        }
    }
    pthread_mutex_unlock(&astate.mtx); free(buf);
}

/* ---------- JPEG decode ---------- */
static bool mjpeg_to_rgb(unsigned char *jpeg_buf, size_t jpeg_len, uint8_t *rgb_buf, int w, int h, int stride) {
    (void)w; (void)h;
    if (jpeg_len == 0 || !jpeg_buf) return false;
    struct jpeg_decompress_struct cinfo; struct jpeg_error_mgr jerr; JSAMPARRAY buffer;
    cinfo.err = jpeg_std_error(&jerr); jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpeg_buf, (unsigned long)jpeg_len);
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) { jpeg_destroy_decompress(&cinfo); return false; }
    jpeg_start_decompress(&cinfo);
    int row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        unsigned char *src = buffer[0];
        unsigned char *dst = rgb_buf + (cinfo.output_scanline - 1) * stride;
        if (cinfo.out_color_space == JCS_RGB && cinfo.output_components >= 3) memcpy(dst, src, (size_t)row_stride);
        else for (JDIMENSION x = 0; x < cinfo.output_width; ++x) {
            if (cinfo.output_components >= 3) {
                dst[3 * x + 0] = src[3 * x + 0];
                dst[3 * x + 1] = src[3 * x + 1];
                dst[3 * x + 2] = src[3 * x + 2];
            } else {
                unsigned char v = src[x];
                dst[3 * x + 0] = dst[3 * x + 1] = dst[3 * x + 2] = v;
            }
        }
    }
    jpeg_finish_decompress(&cinfo); jpeg_destroy_decompress(&cinfo); return true;
}

/* ---------- scaling & hot counting ---------- */
#if defined(__x86_64__) || defined(__i386__)
static inline int sse2_count_hot_rgb24(const uint8_t* src, int pixels, int y_hot_thresh) {
    const __m128 rW = _mm_set1_ps(0.299f), gW = _mm_set1_ps(0.587f), bW = _mm_set1_ps(0.114f), th = _mm_set1_ps((float)y_hot_thresh);
    int hot = 0, i = 0;
    for (; i + 4 <= pixels; i += 4) {
        float r0 = src[3*(i+0)+0], g0 = src[3*(i+0)+1], b0 = src[3*(i+0)+2];
        float r1 = src[3*(i+1)+0], g1 = src[3*(i+1)+1], b1 = src[3*(i+1)+2];
        float r2 = src[3*(i+2)+0], g2 = src[3*(i+2)+1], b2 = src[3*(i+2)+2];
        float r3 = src[3*(i+3)+0], g3 = src[3*(i+3)+1], b3 = src[3*(i+3)+2];
        __m128 r = _mm_set_ps(r3,r2,r1,r0), g = _mm_set_ps(g3,g2,g1,g0), b = _mm_set_ps(b3,b2,b1,b0);
        __m128 y = _mm_add_ps(_mm_add_ps(_mm_mul_ps(r,rW), _mm_mul_ps(g,gW)), _mm_mul_ps(b,bW));
        __m128 mask = _mm_cmpge_ps(y, th);
        int m = _mm_movemask_ps(mask);
        hot += (m & 1) + ((m >> 1) & 1) + ((m >> 2) & 1) + ((m >> 3) & 1);
    }
    for (; i < pixels; ++i) {
        int Y = (int)(src[3*i+0]*0.299f + src[3*i+1]*0.587f + src[3*i+2]*0.114f);
        if (Y >= y_hot_thresh) hot++;
    }
    return hot;
}
#endif

static void rgb24_to_rgb24_and_count(const uint8_t *src, int src_w, int src_h, int src_stride,
                                     uint8_t *dst, int dst_w, int dst_h, int dst_stride, int *hot_pixels_out) {
    int hot = 0;
    for (int y = 0; y < dst_h; ++y) {
        int sy = (y * src_h) / dst_h;
        const uint8_t *srow = src + sy * src_stride;
        uint8_t *drow = dst + y * dst_stride;
#if defined(__x86_64__) || defined(__i386__)
        hot += sse2_count_hot_rgb24(srow, dst_w, Y_HOT_THRESH);
#else
        for (int x = 0; x < dst_w; ++x) {
            int sx = (x * src_w) / dst_w;
            const uint8_t *sp = srow + 3 * sx;
            int Y = (int)(sp[0]*0.299 + sp[1]*0.587 + sp[2]*0.114);
            if (Y >= Y_HOT_THRESH) hot++;
        }
#endif
        for (int x = 0; x < dst_w; ++x) {
            int sx = (x * src_w) / dst_w;
            const uint8_t *sp = srow + 3 * sx;
            uint8_t *dp = drow + 3 * x;
            dp[0] = sp[0]; dp[1] = sp[1]; dp[2] = sp[2];
        }
    }
    if (hot_pixels_out) *hot_pixels_out = hot;
}

static void yuyv_to_rgb24_and_count(const uint8_t *yuyv, int src_w, int src_h,
                                    uint8_t *dst, int dst_w, int dst_h, int dst_stride,
                                    int *hot_pixels_out) {
    int hot = 0;
    for (int y = 0; y < dst_h; ++y) {
        int sy = (y * src_h) / dst_h;
        const uint8_t *srow = yuyv + sy * src_w * 2;
        uint8_t *drow = dst + y * dst_stride;
        for (int x = 0; x < dst_w; ++x) {
            int sx = (x * src_w) / dst_w;
            int pair = (sx / 2) * 4;
            int Y = srow[pair + ((sx & 1) ? 2 : 0)];
            drow[3*x+0] = drow[3*x+1] = drow[3*x+2] = (uint8_t)Y;
            if (Y >= Y_HOT_THRESH) hot++;
        }
    }
    if (hot_pixels_out) *hot_pixels_out = hot;
}

/* 32-bit converters -> RGB24 */
static void argb32_to_rgb24_and_count(const uint8_t *src, int src_w, int src_h,
                                      uint8_t *dst, int dst_w, int dst_h, int dst_stride,
                                      int *hot_pixels_out) {
    int hot = 0;
    for (int y=0;y<dst_h;++y) {
        int sy = (y * src_h) / dst_h;
        const uint8_t *srow = src + sy * src_w * 4;
        uint8_t *drow = dst + y * dst_stride;
        for (int x=0;x<dst_w;++x) {
            int sx = (x * src_w) / dst_w;
            const uint8_t *sp = srow + 4 * sx;
            uint8_t r = sp[1], g = sp[2], b = sp[3]; /* ARGB */
            drow[3*x+0] = r; drow[3*x+1] = g; drow[3*x+2] = b;
            int Y = (int)(r*0.299f + g*0.587f + b*0.114f);
            if (Y >= Y_HOT_THRESH) hot++;
        }
    }
    if (hot_pixels_out) *hot_pixels_out = hot;
}
static void rgba32_to_rgb24_and_count(const uint8_t *src, int src_w, int src_h,
                                      uint8_t *dst, int dst_w, int dst_h, int dst_stride,
                                      int *hot_pixels_out) {
    int hot = 0;
    for (int y=0;y<dst_h;++y) {
        int sy = (y * src_h) / dst_h;
        const uint8_t *srow = src + sy * src_w * 4;
        uint8_t *drow = dst + y * dst_stride;
        for (int x=0;x<dst_w;++x) {
            int sx = (x * src_w) / dst_w;
            const uint8_t *sp = srow + 4 * sx;
            uint8_t r = sp[0], g = sp[1], b = sp[2]; /* RGBA */
            drow[3*x+0] = r; drow[3*x+1] = g; drow[3*x+2] = b;
            int Y = (int)(r*0.299f + g*0.587f + b*0.114f);
            if (Y >= Y_HOT_THRESH) hot++;
        }
    }
    if (hot_pixels_out) *hot_pixels_out = hot;
}
static void abgr32_to_rgb24_and_count(const uint8_t *src, int src_w, int src_h,
                                      uint8_t *dst, int dst_w, int dst_h, int dst_stride,
                                      int *hot_pixels_out) {
    int hot = 0;
    for (int y=0;y<dst_h;++y) {
        int sy = (y * src_h) / dst_h;
        const uint8_t *srow = src + sy * src_w * 4;
        uint8_t *drow = dst + y * dst_stride;
        for (int x=0;x<dst_w;++x) {
            int sx = (x * src_w) / dst_w;
            const uint8_t *sp = srow + 4 * sx;
            uint8_t r = sp[3], g = sp[2], b = sp[1]; /* ABGR */
            drow[3*x+0] = r; drow[3*x+1] = g; drow[3*x+2] = b;
            int Y = (int)(r*0.299f + g*0.587f + b*0.114f);
            if (Y >= Y_HOT_THRESH) hot++;
        }
    }
    if (hot_pixels_out) *hot_pixels_out = hot;
}
static void bgra32_to_rgb24_and_count(const uint8_t *src, int src_w, int src_h,
                                      uint8_t *dst, int dst_w, int dst_h, int dst_stride,
                                      int *hot_pixels_out) {
    int hot = 0;
    for (int y=0;y<dst_h;++y) {
        int sy = (y * src_h) / dst_h;
        const uint8_t *srow = src + sy * src_w * 4;
        uint8_t *drow = dst + y * dst_stride;
        for (int x=0;x<dst_w;++x) {
            int sx = (x * src_w) / dst_w;
            const uint8_t *sp = srow + 4 * sx;
            uint8_t r = sp[2], g = sp[1], b = sp[0]; /* BGRA */
            drow[3*x+0] = r; drow[3*x+1] = g; drow[3*x+2] = b;
            int Y = (int)(r*0.299f + g*0.587f + b*0.114f);
            if (Y >= Y_HOT_THRESH) hot++;
        }
    }
    if (hot_pixels_out) *hot_pixels_out = hot;
}
static void xrgb32_to_rgb24_and_count(const uint8_t *src, int src_w, int src_h,
                                      uint8_t *dst, int dst_w, int dst_h, int dst_stride,
                                      int *hot_pixels_out) {
    int hot = 0;
    for (int y=0;y<dst_h;++y) {
        int sy = (y * src_h) / dst_h;
        const uint8_t *srow = src + sy * src_w * 4;
        uint8_t *drow = dst + y * dst_stride;
        for (int x=0;x<dst_w;++x) {
            int sx = (x * src_w) / dst_w;
            const uint8_t *sp = srow + 4 * sx;
            uint8_t r = sp[1], g = sp[2], b = sp[3]; /* XRGB */
            drow[3*x+0] = r; drow[3*x+1] = g; drow[3*x+2] = b;
            int Y = (int)(r*0.299f + g*0.587f + b*0.114f);
            if (Y >= Y_HOT_THRESH) hot++;
        }
    }
    if (hot_pixels_out) *hot_pixels_out = hot;
}

/* ---------- HSV helpers ---------- */
static inline void rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v) {
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float max = fmaxf(rf, fmaxf(gf, bf)), min = fminf(rf, fminf(gf, bf));
    float d = max - min;
    *v = max;
    *s = (max <= 0.0f) ? 0.0f : (d / max);
    if (d == 0.0f) { *h = 0.0f; return; }
    if (max == rf)      *h = fmodf(((gf - bf) / d), 6.0f);
    else if (max == gf) *h = ((bf - rf) / d) + 2.0f;
    else                *h = ((rf - gf) / d) + 4.0f;
    *h *= 60.0f; if (*h < 0.0f) *h += 360.0f;
}
static inline void hsv_to_rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b) {
    float c = v * s;
    float hh = (h / 60.0f);
    float x = c * (1.0f - fabsf(fmodf(hh, 2.0f) - 1.0f));
    float m = v - c;
    float rf=0, gf=0, bf=0;
    if (hh < 1) { rf=c; gf=x; bf=0; }
    else if (hh < 2) { rf=x; gf=c; bf=0; }
    else if (hh < 3) { rf=0; gf=c; bf=x; }
    else if (hh < 4) { rf=0; gf=x; bf=c; }
    else if (hh < 5) { rf=x; gf=0; bf=c; }
    else             { rf=c; gf=0; bf=x; }
    *r = (uint8_t)lrintf((rf + m) * 255.0f);
    *g = (uint8_t)lrintf((gf + m) * 255.0f);
    *b = (uint8_t)lrintf((bf + m) * 255.0f);
}

/* ---------- xcb color allocation ---------- */
static uint32_t alloc_neon_pixel(uint8_t r, uint8_t g, uint8_t b) {
    xcb_alloc_color_cookie_t c = xcb_alloc_color(xstate.conn, xstate.colormap, (uint16_t)r * 257, (uint16_t)g * 257, (uint16_t)b * 257);
    xcb_alloc_color_reply_t *rply = xcb_alloc_color_reply(xstate.conn, c, NULL);
    uint32_t pixel = (rply) ? rply->pixel : xstate.screen->white_pixel;
    if (rply) free(rply);
    return pixel;
}

/* ---------- xcb init/close ---------- */
static bool xcb_init(int initial_w, int initial_h) {
    xstate.conn = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(xstate.conn)) { fprintf(stderr,"xcb_connect failed\n"); return false; }
    xstate.setup = xcb_get_setup(xstate.conn);
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(xstate.setup);
    xstate.screen = it.data;
    xstate.win_w = initial_w; xstate.win_h = initial_h;
    xstate.depth = xstate.screen->root_depth;
    xstate.win = xcb_generate_id(xstate.conn);

    xstate.colormap = xstate.screen->default_colormap;
    uint32_t mask = XCB_CW_EVENT_MASK | XCB_CW_BACK_PIXEL;
    uint32_t values[2] = {
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_KEY_PRESS,
        xstate.screen->black_pixel
    };
    xcb_create_window(xstate.conn, xstate.depth, xstate.win, xstate.screen->root, 0,0, xstate.win_w, xstate.win_h, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, xstate.screen->root_visual, mask, values);
    xcb_change_property(xstate.conn, XCB_PROP_MODE_REPLACE, xstate.win, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen("geigern"), "geigern");

    xstate.gc_blit = xcb_generate_id(xstate.conn); xcb_create_gc(xstate.conn, xstate.gc_blit, xstate.win, 0, NULL);
    xstate.gc_text = xcb_generate_id(xstate.conn); xcb_create_gc(xstate.conn, xstate.gc_text, xstate.win, 0, NULL);
    
    // Initialize keysyms
    xstate.keysyms = xcb_key_symbols_alloc(xstate.conn);

    /* WM_DELETE_WINDOW */
    xcb_intern_atom_cookie_t c1 = xcb_intern_atom(xstate.conn, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_cookie_t c2 = xcb_intern_atom(xstate.conn, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t *r1 = xcb_intern_atom_reply(xstate.conn, c1, NULL);
    xcb_intern_atom_reply_t *r2 = xcb_intern_atom_reply(xstate.conn, c2, NULL);
    if (r1) { xstate.atom_wm_protocols = r1->atom; free(r1); } else xstate.atom_wm_protocols = XCB_NONE;
    if (r2) { xstate.atom_wm_delete_window = r2->atom; free(r2); } else xstate.atom_wm_delete_window = XCB_NONE;
    if (xstate.atom_wm_protocols && xstate.atom_wm_delete_window) {
        xcb_change_property(xstate.conn, XCB_PROP_MODE_REPLACE, xstate.win,
                            xstate.atom_wm_protocols, XCB_ATOM_ATOM, 32, 1, &xstate.atom_wm_delete_window);
    }

    xcb_map_window(xstate.conn, xstate.win);
    xcb_flush(xstate.conn);

    xstate.back_buffer = xcb_generate_id(xstate.conn);
    xcb_create_pixmap(xstate.conn, xstate.depth, xstate.back_buffer, xstate.win, xstate.win_w, xstate.win_h);
    clear_back_buffer(xstate.screen->black_pixel);

    for (int i = 0; i < NUM_ISOTOPES; ++i) neon_pixel[i] = alloc_neon_pixel(neon_rgb[i][0], neon_rgb[i][1], neon_rgb[i][2]);

    recalc_layout();
    
    // --- SHM INITIALIZATION LOGIC ---
    xstate.shminfo.shmid = -1;
    xstate.shm_buffer = NULL;
    
    // Prüfen, ob SHM verfügbar ist
    xcb_shm_query_version_cookie_t shm_cookie = xcb_shm_query_version(xstate.conn);
    xcb_shm_query_version_reply_t *shm_reply = xcb_shm_query_version_reply(xstate.conn, shm_cookie, NULL);
    if (shm_reply && shm_reply->shared_pixmaps && shm_reply->major_version > 1) {
        free(shm_reply);
        
        // Versuche SHM zu initialisieren
        size_t size = (size_t)xstate.win_w * (size_t)xstate.win_h * 4; // ARGB32
        
        xstate.shminfo.shmseg = xcb_generate_id(xstate.conn);
        xstate.shminfo.shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);

        if (xstate.shminfo.shmid >= 0) {
            xstate.shm_buffer = (uint32_t*)shmat(xstate.shminfo.shmid, 0, 0);

            if (xstate.shm_buffer != (uint32_t*)-1) {
                // Attach-Anfrage an den X-Server senden
                xcb_shm_attach(xstate.conn, xstate.shminfo.shmseg, xstate.shminfo.shmid, false);
                // Markiert SHM-Segment zum Löschen nach Detach
                shmctl(xstate.shminfo.shmid, IPC_RMID, 0); 
            } else {
                xstate.shminfo.shmid = -1; // shmat failed
                xstate.shm_buffer = NULL;
            }
        }
    } else if (shm_reply) {
        free(shm_reply);
    }
    // --- END SHM INITIALIZATION LOGIC ---
    
    return true;
}

static void xcb_recreate_buffers(int w, int h) {
    // --- SHM Cleanup on Resize ---
    if (xstate.shminfo.shmid >= 0) {
        xcb_shm_detach(xstate.conn, xstate.shminfo.shmseg);
        if (xstate.shm_buffer) shmdt(xstate.shm_buffer);
        xstate.shminfo.shmid = -1;
        xstate.shm_buffer = NULL;
    }
    
    if (xstate.back_buffer) { xcb_free_pixmap(xstate.conn, xstate.back_buffer); xstate.back_buffer = 0; }
    xstate.back_buffer = xcb_generate_id(xstate.conn);
    xcb_create_pixmap(xstate.conn, xstate.depth, xstate.back_buffer, xstate.win, w, h);
    xstate.win_w = w; xstate.win_h = h;
    recalc_layout();
    clear_back_buffer(xstate.screen->black_pixel);
    need_redraw = true;
    
    // --- SHM Re-creation on Resize ---
    size_t size = (size_t)w * (size_t)h * 4; // ARGB32
    
    xcb_shm_query_version_cookie_t shm_cookie = xcb_shm_query_version(xstate.conn);
    xcb_shm_query_version_reply_t *shm_reply = xcb_shm_query_version_reply(xstate.conn, shm_cookie, NULL);
    if (shm_reply && shm_reply->shared_pixmaps && shm_reply->major_version > 1) {
        free(shm_reply);

        xstate.shminfo.shmseg = xcb_generate_id(xstate.conn);
        xstate.shminfo.shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);

        if (xstate.shminfo.shmid >= 0) {
            xstate.shm_buffer = (uint32_t*)shmat(xstate.shminfo.shmid, 0, 0);
            
            if (xstate.shm_buffer != (uint32_t*)-1) {
                // Attach-Anfrage an den X-Server senden
                xcb_shm_attach(xstate.conn, xstate.shminfo.shmseg, xstate.shminfo.shmid, false);
                // Markiert SHM-Segment zum Löschen nach Detach
                shmctl(xstate.shminfo.shmid, IPC_RMID, 0);
            } else {
                xstate.shminfo.shmid = -1;
                xstate.shm_buffer = NULL;
            }
        }
    } else if (shm_reply) {
        free(shm_reply);
    }
    // --- END SHM Re-creation LOGIC ---
}

static void xcb_close(void) {
    // --- SHM Cleanup on Close ---
    if (xstate.shminfo.shmid >= 0) {
        xcb_shm_detach(xstate.conn, xstate.shminfo.shmseg);
        if (xstate.shm_buffer) shmdt(xstate.shm_buffer);
        xstate.shminfo.shmid = -1;
    }
    // Free keysyms
    if (xstate.keysyms) { xcb_key_symbols_free(xstate.keysyms); xstate.keysyms = NULL; }
    // --- END SHM Cleanup ---
    
    if (video_rgb24) { free(video_rgb24); video_rgb24 = NULL; }
    if (xstate.back_buffer) { xcb_free_pixmap(xstate.conn, xstate.back_buffer); xstate.back_buffer = 0; }
    if (xstate.gc_text) { xcb_free_gc(xstate.conn, xstate.gc_text); xstate.gc_text = 0; }
    if (xstate.gc_blit) { xcb_free_gc(xstate.conn, xstate.gc_blit); xstate.gc_blit = 0; }
    if (xstate.conn) { xcb_disconnect(xstate.conn); xstate.conn = NULL; }
}

/* ---------- frame queue ---------- */
static void queue_push_frame(unsigned char *cpy, size_t len, pixfmt_t pf, long long ts_ms) { pthread_mutex_lock(&fqueue.mtx); if (fqueue.count < FRAME_QUEUE_SIZE) { int idx = fqueue.head; fqueue.items[idx].buf = cpy; fqueue.items[idx].len = len; fqueue.items[idx].pf = pf; fqueue.items[idx].ts_ms = ts_ms; fqueue.head = (fqueue.head + 1) % FRAME_QUEUE_SIZE; fqueue.count++; pthread_cond_signal(&fqueue.cv); } else { free(cpy); } pthread_mutex_unlock(&fqueue.mtx); }
static frame_item_t queue_pop_frame(void) {
    frame_item_t item = { NULL, 0, 0, 0 };
    pthread_mutex_lock(&fqueue.mtx);
    while (fqueue.count == 0 && running) pthread_cond_wait(&fqueue.cv, &fqueue.mtx);
    if (fqueue.count > 0) {
        int idx = fqueue.tail;
        item = fqueue.items[idx];
        fqueue.tail = (fqueue.tail + 1) % FRAME_QUEUE_SIZE;
        fqueue.count--;
    }
    pthread_mutex_unlock(&fqueue.mtx);
    return item;
}

/* ---------- V4L2 helpers ---------- */
static pixfmt_t fourcc_to_pixfmt(uint32_t fourcc) {
    switch (fourcc) {
#ifdef V4L2_PIX_FMT_ARGB32
        case V4L2_PIX_FMT_ARGB32: return PF_ARGB32;
#endif
#ifdef V4L2_PIX_FMT_RGBA32
        case V4L2_PIX_FMT_RGBA32: return PF_RGBA32;
#endif
#ifdef V4L2_PIX_FMT_ABGR32
        case V4L2_PIX_FMT_ABGR32: return PF_ABGR32;
#endif
#ifdef V4L2_PIX_FMT_BGRA32
        case V4L2_PIX_FMT_BGRA32: return PF_BGRA32;
#endif
#ifdef V4L2_PIX_FMT_RGB32
        case V4L2_PIX_FMT_RGB32: return PF_XRGB32;
#endif
        case V4L2_PIX_FMT_RGB24: return PF_RGB24;
        case V4L2_PIX_FMT_YUYV: return PF_YUYV;
        case V4L2_PIX_FMT_MJPEG: return PF_MJPEG;
        default: return PF_RGB24; /* Fallback, should not happen */
    }
}
static bool try_set_fmt_fixed(int fd, uint32_t fourcc) {
    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = (unsigned int)SRC_WIDTH;
    fmt.fmt.pix.height = (unsigned int)SRC_HEIGHT;
    fmt.fmt.pix.pixelformat = fourcc;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) return false;
    if (fmt.fmt.pix.width != (unsigned int)SRC_WIDTH || fmt.fmt.pix.height != (unsigned int)SRC_HEIGHT) return false;
    return true;
}
static unsigned int pick_highest_fps(int fd, uint32_t fourcc) {
    unsigned int best_num = 1; unsigned int best_den = 1; /* frame interval = num/den seconds, fps = den/num */
#ifdef VIDIOC_ENUM_FRAMEINTERVALS
    struct v4l2_frmivalenum fiv;
    memset(&fiv,0,sizeof(fiv));
    fiv.pixel_format = fourcc;
    fiv.width = SRC_WIDTH;
    fiv.height = SRC_HEIGHT;
    for (fiv.index = 0; ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fiv) == 0; fiv.index++) {
        if (fiv.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
            unsigned int num = fiv.discrete.numerator;
            unsigned int den = fiv.discrete.denominator;
            if (num == 0 || den == 0) continue;
            /* pick larger fps => larger den/num */
            if ((double)den/(double)num > (double)best_den/(double)best_num) {
                best_num = num;
                best_den = den;
            }
        } else if (fiv.type == V4L2_FRMIVAL_TYPE_STEPWISE || fiv.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) {
            /* stepwise: min numerator/min denominator gives fastest frame interval? For safety pick min numerator / max denominator? */
            unsigned int num = fiv.stepwise.min.numerator;
            unsigned int den = fiv.stepwise.min.denominator;
            if (num == 0 || den == 0) continue;
            if ((double)den/(double)num > (double)best_den/(double)best_num) {
                best_num = num;
                best_den = den;
            }
        }
        memset(&fiv,0,sizeof(fiv));
        fiv.pixel_format = fourcc;
        fiv.width = SRC_WIDTH;
        fiv.height = SRC_HEIGHT;
    }
#endif
    if (best_num != 1 || best_den != 1) {
        double fps = (double)best_den / (double)best_num;
        if (fps < 1.0) fps = 1.0;
        return (unsigned int)floor(fps + 0.5);
    }
    /* Fallback: try common high fps candidates */
    unsigned int candidates[] = {120, 90, 60, 30, 25, 15};
    for (size_t i = 0; i < sizeof(candidates)/sizeof(candidates[0]); ++i) {
        unsigned int fps = candidates[i];
        struct v4l2_streamparm sparm;
        memset(&sparm,0,sizeof(sparm));
        sparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        sparm.parm.capture.timeperframe.numerator = 1;
        sparm.parm.capture.timeperframe.denominator = fps;
        if (ioctl(fd, VIDIOC_S_PARM, &sparm) == 0) return fps;
    }
    return FPS;
}

/* ---------- V4L2 thread ---------- */
typedef struct { void *start; size_t length; } buffer_t;
static void *v4l2_thread(void *arg) {
    (void)arg;
    int fd = -1;
    bool streaming = false;
    buffer_t *mbufs = NULL;
    int nbuf = 0;
    
    struct v4l2_format fmt;

    if (vstate.simulated) { fprintf(stderr,"Simulated V4L2 thread running.\n"); } else {
        fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
        if (fd < 0) { fprintf(stderr,"open %s failed: %s -> simulated\n", DEVICE_NODE, strerror(errno)); } else {
            struct v4l2_capability cap;
            if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
                fprintf(stderr,"VIDIOC_QUERYCAP failed -> simulated\n"); close(fd); fd=-1; vstate.simulated=true;
            } else {
                uint32_t candidates[] = {
#ifdef V4L2_PIX_FMT_ARGB32
                    V4L2_PIX_FMT_ARGB32,
#endif
#ifdef V4L2_PIX_FMT_RGBA32
                    V4L2_PIX_FMT_RGBA32,
#endif
#ifdef V4L2_PIX_FMT_ABGR32
                    V4L2_PIX_FMT_ABGR32,
#endif
#ifdef V4L2_PIX_FMT_BGRA32
                    V4L2_PIX_FMT_BGRA32,
#endif
#ifdef V4L2_PIX_FMT_RGB32
                    V4L2_PIX_FMT_RGB32,
#endif
                    V4L2_PIX_FMT_RGB24,
                    V4L2_PIX_FMT_YUYV,
                    V4L2_PIX_FMT_MJPEG
                };
                bool ok = false; 
                unsigned int chosen_fps = 30;
                for (size_t i=0;i<sizeof(candidates)/sizeof(candidates[0]);++i) {
                    uint32_t fourcc = candidates[i];
                    if (!fourcc) continue;
                    if (try_set_fmt_fixed(fd, fourcc)) {
                        /* pick highest fps */
                        chosen_fps = pick_highest_fps(fd, fourcc);
                        /* try to set it */
                        struct v4l2_streamparm sparm;
                        memset(&sparm,0,sizeof(sparm));
                        sparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        sparm.parm.capture.timeperframe.numerator = 1;
                        sparm.parm.capture.timeperframe.denominator = chosen_fps;
                        ioctl(fd, VIDIOC_S_PARM, &sparm); /* may fail, but continue */
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    fprintf(stderr,"VIDIOC_S_FMT (640x480) failed -> simulated\n"); close(fd); fd=-1; vstate.simulated=true;
                } else {
                    // Get actual format set by driver
                    memset(&fmt,0,sizeof(fmt));
                    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0) {
                        fprintf(stderr,"VIDIOC_G_FMT failed: %s -> simulated\n", strerror(errno)); close(fd); fd=-1; vstate.simulated=true;
                    } else {
                        vstate.width = (int)fmt.fmt.pix.width;
                        vstate.height = (int)fmt.fmt.pix.height;
                        struct v4l2_requestbuffers req;
                        memset(&req,0,sizeof(req));
                        req.count = 4;
                        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        req.memory = V4L2_MEMORY_MMAP;
                        if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
                            fprintf(stderr,"VIDIOC_REQBUFS failed: %s -> simulated\n", strerror(errno)); close(fd); fd=-1; vstate.simulated=true;
                        } else {
                            mbufs = calloc(req.count, sizeof(buffer_t));
                            if (!mbufs) {
                                fprintf(stderr,"calloc failed -> simulated\n"); close(fd); fd=-1; vstate.simulated=true;
                            } else {
                                for (unsigned i=0;i<req.count;++i) {
                                    struct v4l2_buffer buf;
                                    memset(&buf,0,sizeof(buf));
                                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                    buf.memory = V4L2_MEMORY_MMAP;
                                    buf.index = i;
                                    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
                                        fprintf(stderr,"VIDIOC_QUERYBUF failed: %s -> simulated\n", strerror(errno));
                                        for (unsigned j=0;j<i;++j) if (mbufs[j].start) munmap(mbufs[j].start, mbufs[j].length);
                                        free(mbufs); mbufs = NULL; close(fd); fd = -1; vstate.simulated=true; break;
                                    }
                                    mbufs[i].length = buf.length;
                                    mbufs[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
                                    if (mbufs[i].start == MAP_FAILED) {
                                        fprintf(stderr,"mmap failed: %s -> simulated\n", strerror(errno));
                                        for (unsigned j=0;j<i;++j) if (mbufs[j].start) munmap(mbufs[j].start, mbufs[j].length);
                                        free(mbufs); mbufs = NULL; close(fd); fd = -1; vstate.simulated=true; break;
                                    }
                                }
                                if (vstate.simulated == false) {
                                    for (unsigned i=0;i<req.count;++i) {
                                        struct v4l2_buffer buf;
                                        memset(&buf,0,sizeof(buf));
                                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                        buf.memory = V4L2_MEMORY_MMAP;
                                        buf.index = i;
                                        ioctl(fd, VIDIOC_QBUF, &buf);
                                    }
                                    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
                                        fprintf(stderr,"VIDIOC_STREAMON failed: %s -> simulated\n", strerror(errno));
                                        for (unsigned i=0;i<req.count;++i) if (mbufs[i].start) munmap(mbufs[i].start, mbufs[i].length);
                                        free(mbufs); mbufs = NULL; close(fd); fd = -1; vstate.simulated=true;
                                    } else {
                                        streaming = true;
                                        nbuf = (int)req.count;
                                        vstate.simulated = false;
                                        fprintf(stderr,"V4L2 streaming %dx%d @ %u FPS\n", SRC_WIDTH, SRC_HEIGHT, pick_highest_fps(fd, fmt.fmt.pix.pixelformat));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    srand((unsigned)time(NULL) ^ getpid());
    while (running) {
        if (streaming && fd >= 0 && mbufs) {
            fd_set fds; FD_ZERO(&fds); FD_SET(fd,&fds);
            struct timeval tv; tv.tv_sec = 2; tv.tv_usec = 0;
            int r = select(fd+1,&fds,NULL,NULL,&tv);
            if (r <= 0) { if (r == 0 || errno == EINTR) continue; break; }

            struct v4l2_buffer buf;
            memset(&buf,0,sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) { if (errno == EAGAIN) continue; break; }
            if (buf.index >= (unsigned)nbuf) { ioctl(fd, VIDIOC_QBUF, &buf); continue; }
            size_t copy_len = buf.bytesused;
            if (copy_len == 0) { ioctl(fd, VIDIOC_QBUF, &buf); continue; }
            long long ts_ms = (long long)(now_seconds() * 1000.0);
            unsigned char *cpy = malloc(copy_len);
            if (cpy) memcpy(cpy, mbufs[buf.index].start, copy_len);
            ioctl(fd, VIDIOC_QBUF, &buf);
            
            if (cpy) queue_push_frame(cpy, copy_len, fourcc_to_pixfmt(fmt.fmt.pix.pixelformat), ts_ms);
        } else {
            /* simulation: just produce black frame */
            long long ts_ms = (long long)(now_seconds() * 1000.0);
            int frame_size = SRC_WIDTH * SRC_HEIGHT * 4;
            unsigned char *cpy = malloc((size_t)frame_size);
            if (cpy) {
                memset(cpy, 0, (size_t)frame_size);
                queue_push_frame(cpy, (size_t)frame_size, PF_ARGB32, ts_ms);
            }
            usleep(1000000 / FPS);
        }
    }

    /* cleanup */
    if (streaming && fd >= 0) { enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE; ioctl(fd, VIDIOC_STREAMOFF, &type); }
    if (mbufs) { for (int i=0;i<nbuf;++i) if (mbufs[i].start) munmap(mbufs[i].start, mbufs[i].length); free(mbufs); }
    if (fd >= 0) close(fd);
    return NULL;
}

/* ---------- text helper ---------- */
static inline void draw_text_with_gc(int x, int y, const char *s, xcb_gcontext_t gc) {
    size_t n = strlen(s);
    if (n > 255) n = 255;
    xcb_image_text_8(xstate.conn, (uint8_t)n, xstate.back_buffer, gc, x, y, s);
}

/* ---------- main redraw logic ---------- */
static void xcb_redraw(void) {
    static double last_frame_time_sec = 0.0;
    static double last_rad_update_sec = 0.0;
    
    // ... (frame retrieval logic) ...
    frame_item_t frame = queue_pop_frame();
    bool got_frame = (frame.buf != NULL);
    unsigned char *local_buf = frame.buf;
    size_t local_len = frame.len;
    pixfmt_t local_pf = frame.pf;

    /* recalc layout each iteration */
    recalc_layout();
    double now_sec = now_seconds();
    double frame_delta_sec = now_sec - last_frame_time_sec;
    if (frame_delta_sec < 0.0) frame_delta_sec = 0.0;
    if (frame_delta_sec > 0.25) frame_delta_sec = 0.25;
    last_frame_time_sec = now_sec;
    long long current_time_ms = (long long)(now_sec * 1000.0);
    int vid_w = layout.vid_w;
    int vid_h = layout.vid_h;
    int video_x = layout.video_x;
    int video_y = layout.video_y;
    int hot = gstate.hot_pixels;
    
    if (got_frame && local_buf && local_len > 0) {
        /* update processing buffer sizes for new layout */
        int required_stride_rgb24 = vid_w * 3;
        if (video_rgb24 == NULL || video_stride_rgb24 != required_stride_rgb24) {
            if (video_rgb24) { free(video_rgb24); video_rgb24 = NULL; }
            video_stride_rgb24 = required_stride_rgb24;
            video_rgb24 = malloc((size_t)video_stride_rgb24 * (size_t)vid_h);
            if (!video_rgb24) { fprintf(stderr,"malloc video_rgb24 failed\n"); video_stride_rgb24 = 0; }
        }

        /* convert input to RGB24 and count hot */
        if (video_rgb24) {
            if (local_pf == PF_ARGB32) {
                argb32_to_rgb24_and_count(local_buf, vstate.width, vstate.height, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            } else if (local_pf == PF_RGBA32) {
                rgba32_to_rgb24_and_count(local_buf, vstate.width, vstate.height, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            } else if (local_pf == PF_ABGR32) {
                abgr32_to_rgb24_and_count(local_buf, vstate.width, vstate.height, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            } else if (local_pf == PF_BGRA32) {
                bgra32_to_rgb24_and_count(local_buf, vstate.width, vstate.height, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            } else if (local_pf == PF_XRGB32) {
                xrgb32_to_rgb24_and_count(local_buf, vstate.width, vstate.height, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            } else if (local_pf == PF_RGB24) {
                rgb24_to_rgb24_and_count(local_buf, vstate.width, vstate.height, video_stride_rgb24, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            } else if (local_pf == PF_YUYV) {
                yuyv_to_rgb24_and_count(local_buf, vstate.width, vstate.height, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            } else if (local_pf == PF_MJPEG) {
                if (!mjpeg_to_rgb(local_buf, local_len, video_rgb24, vid_w, vid_h, video_stride_rgb24)) {
                    memset(video_rgb24, 0, (size_t)video_stride_rgb24 * (size_t)vid_h);
                }
                rgb24_to_rgb24_and_count(video_rgb24, vid_w, vid_h, video_stride_rgb24, video_rgb24, vid_w, vid_h, video_stride_rgb24, &hot);
            }
        }
    }
    
    if (local_buf) free(local_buf);

    // --- Geiger Counter Logic ---
    gstate.hot_pixels = hot;

    /* radiation calculation */
    double measuredRadValue = (double)hot * RAD_SCALE / frame_delta_sec;
    double radValue = measuredRadValue;
    if (gstate.filtered_rad_value == 0.0) gstate.filtered_rad_value = measuredRadValue;
    
    double changeRate = 1.0;
    if (gstate.filtered_rad_value > measuredRadValue) changeRate = AEMA_SCALE_FACTOR * 0.9; else changeRate = AEMA_SCALE_FACTOR;
    gstate.dynamic_alpha = fmin(ALPHA_MAX, ALPHA_MIN + gstate.filtered_rad_value * changeRate);
    radValue = (measuredRadValue * gstate.dynamic_alpha) + (gstate.filtered_rad_value * (1.0 - gstate.dynamic_alpha));
    
    gstate.filtered_rad_value = radValue;
    gstate.accumulated_dosage += radValue * frame_delta_sec;

    int chance = rand()%100;
    int isotopeId;
    if (chance < HIGH_RISK_CHANCE_TOTAL) isotopeId = 14 + (rand()%6);
    else if (chance < (HIGH_RISK_CHANCE_TOTAL + MEDIUM_RISK_CHANCE_TOTAL)) isotopeId = 7 + (rand()%7);
    else isotopeId = rand()%7;
    isotope_count[isotopeId]++;
    
    long long total_detections=0;
    for (int i=0;i<NUM_ISOTOPES;++i) total_detections += isotope_count[i];
    
    if (total_detections > 0) {
        double per = gstate.filtered_rad_value / (double)total_detections;
        for (int i=0;i<NUM_ISOTOPES;++i) {
            if (isotope_count[i]>0) {
                gstate.isotope_rad_rate[i] = per * isotope_count[i];
                gstate.isotope_accumulated_dosage[i] += gstate.isotope_rad_rate[i] * frame_delta_sec;
            } else gstate.isotope_rad_rate[i]=0.0;
        }
    }
    
    for (int i=0;i<NUM_ISOTOPES;++i) {
        double iso_ld50 = gstate.isotope_ld50_threshold[i];
        double iso_dose = gstate.isotope_accumulated_dosage[i];
        double iso_ratio = (iso_ld50>0.0) ? (iso_dose/iso_ld50) : 0.0;
        if (!gstate.isotope_ld50_exceeded[i] && iso_ratio >= gstate.ld50_on_ratio) {
            gstate.isotope_ld50_exceeded[i]=true;
            play_click(LD50_ALARM_FREQ + i*7.0, LD50_ALARM_DURATION);
        } else if (gstate.isotope_ld50_exceeded[i] && iso_ratio <= gstate.ld50_off_ratio) {
            gstate.isotope_ld50_exceeded[i]=false;
        }
    }
    
    double lowest_ld50_threshold = LD50_LOW_RISK_THRESHOLD;
    for (int i=0;i<NUM_ISOTOPES;++i) if (isotope_count[i] > 0 && gstate.isotope_ld50_threshold[i] < lowest_ld50_threshold) lowest_ld50_threshold = gstate.isotope_ld50_threshold[i];
    
    // Global LD50 check
    double current_ld50_ratio_check = (lowest_ld50_threshold > 0.0) ? (gstate.accumulated_dosage / lowest_ld50_threshold) : 0.0;
    if (!gstate.ld50_exceeded && current_ld50_ratio_check >= gstate.ld50_on_ratio) {
        gstate.ld50_exceeded = true;
        play_click(LD50_ALARM_FREQ, LD50_ALARM_DURATION * 3.0);
    } else if (gstate.ld50_exceeded && current_ld50_ratio_check <= gstate.ld50_off_ratio) {
        gstate.ld50_exceeded = false;
    }
    
    /* audio alert logic (click) */
    if (now_sec - last_rad_update_sec > 0.0) {
        int rate = (int)lrint(gstate.filtered_rad_value / ALERT_THRESHOLD * 1000.0);
        if (rate > 0) {
            double interval_sec = 1.0 / (double)rate;
            if (interval_sec < 0.01) interval_sec = 0.01;
            double next_alert_sec = last_rad_update_sec + interval_sec;
            if (now_sec >= next_alert_sec) {
                last_rad_update_sec = now_sec;
                double freq = BASE_PITCH_HZ + (gstate.filtered_rad_value / MAX_INTENSITY_FOR_PITCH) * ISOTOPE_FREQ_STEP_HZ;
                if (freq > 2000.0) freq = 2000.0;
                play_click(freq, ALARM_DURATION);
            }
        } else {
            last_rad_update_sec = now_sec;
        }
    } else {
        last_rad_update_sec = now_sec;
    }
    
    /* visual alert (flashing background) */
    bool should_be_active = gstate.filtered_rad_value >= gstate.alert_on_threshold;
    if (gstate.alert_active != should_be_active) {
        if (current_time_ms - gstate.alert_last_change_ms > gstate.alert_hold_ms) {
            gstate.alert_active = should_be_active;
            gstate.alert_last_change_ms = current_time_ms;
        }
    }
    
    uint32_t bg_pixel = xstate.screen->black_pixel;
    if (gstate.alert_active && (current_time_ms/100)%2 == 0) {
        bg_pixel = 0xAA0000;
    }

    /* redraw background */
    clear_back_buffer(bg_pixel);

    int tmp32_stride = vid_w * 4;
    uint8_t *tmp32 = NULL;
    
    // Check SHM availability first
    if (video_rgb24 && video_stride_rgb24 == vid_w * 3 && xstate.shminfo.shmid >= 0) { // use SHM if available
        tmp32 = (uint8_t*)xstate.shm_buffer;
    } else if (video_rgb24 && video_stride_rgb24 == vid_w * 3) {
        tmp32 = malloc((size_t)tmp32_stride * (size_t)vid_h);
    }
    
    if (tmp32) {
        bool lsb = (xcb_get_setup(xstate.conn)->image_byte_order == LSBFirst);
        
        /* initialize ARGB from RGB24 */
        for (int y=0;y<vid_h;++y) {
            const uint8_t *srow = video_rgb24 + y * video_stride_rgb24;
            uint8_t *drow = tmp32 + y * tmp32_stride;
            for (int x=0;x<vid_w;++x) {
                uint8_t r=srow[3*x+0], g=srow[3*x+1], b=srow[3*x+2], a=0xFF;
                if (lsb) {
                    uint32_t outpx = ((uint32_t)a<<24) | ((uint32_t)r<<16) | ((uint32_t)g<<8) | (uint32_t)b;
                    memcpy(drow + 4*x, &outpx, 4);
                } else {
                    drow[4*x+0]=a; drow[4*x+1]=r; drow[4*x+2]=g; drow[4*x+3]=b;
                }
            }
        }
        
        /* luminance */
        uint8_t *gray = malloc((size_t)vid_h * (size_t)vid_w);
        if (gray) {
            for (int y=0;y<vid_h;++y) {
                const uint8_t *rrow = video_rgb24 + y * video_stride_rgb24;
                uint8_t *grow = gray + y * vid_w;
                for (int x=0;x<vid_w;++x) {
                    uint8_t r = rrow[3*x+0], g = rrow[3*x+1], b = rrow[3*x+2];
                    grow[x] = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b + 0.5f);
                }
            }
            const float max_alpha = 0.6f;
            
            /* draw hot pixels */
            for (int y=0;y<vid_h;++y) {
                uint8_t *g_row = gray + y * vid_w;
                uint8_t *row_tmp32 = tmp32 + y * tmp32_stride;
                for (int x=0;x<vid_w;++x) {
                    if (g_row[x] >= Y_HOT_THRESH) {
                        float h, s, v;
                        uint8_t r_old, g_old, b_old;
                        
                        // Extract color components based on byte order for HSV conversion
                        if (lsb) { r_old = row_tmp32[4*x+1]; g_old = row_tmp32[4*x+2]; b_old = row_tmp32[4*x+3]; }
                        else     { r_old = row_tmp32[4*x+1]; g_old = row_tmp32[4*x+2]; b_old = row_tmp32[4*x+3]; }
                        
                        rgb_to_hsv(r_old, g_old, b_old, &h, &s, &v);
                        
                        float noise = (float)rand()/(float)RAND_MAX;
                        int isotope_idx = (int)(noise * NUM_ISOTOPES);
                        
                        uint8_t nr=neon_rgb[isotope_idx][0], ng=neon_rgb[isotope_idx][1], nb=neon_rgb[isotope_idx][2];
                        
                        float alpha = fminf(max_alpha, (g_row[x] - Y_HOT_THRESH) / (255.0f - Y_HOT_THRESH));
                        
                        uint8_t orr=r_old, ogg=g_old, obb=b_old;
                        float fr = (1.0f - alpha) * (float)orr + alpha * (float)nr;
                        float fg = (1.0f - alpha) * (float)ogg + alpha * (float)ng;
                        float fb = (1.0f - alpha) * (float)obb + alpha * (float)nb;

                        uint8_t dr = (uint8_t)fminf(255.0f, fr + 0.5f);
                        uint8_t dg = (uint8_t)fminf(255.0f, fg + 0.5f);
                        uint8_t db = (uint8_t)fminf(255.0f, fb + 0.5f);
                        uint8_t da = 0xFF;

                        if (lsb) {
                            uint32_t outpx = ((uint32_t)da<<24) | ((uint32_t)dr<<16) | ((uint32_t)dg<<8) | (uint32_t)db;
                            memcpy(row_tmp32 + 4*x, &outpx, 4);
                        } else {
                            row_tmp32[4*x+0] = da;
                            row_tmp32[4*x+1] = dr;
                            row_tmp32[4*x+2] = dg;
                            row_tmp32[4*x+3] = db;
                        }
                    }
                }
            }
            free(gray);
        }
        
        /* blit to back buffer */
        if (xstate.shminfo.shmid >= 0) {
            // SHM-Variante: 13 Argumente (ohne depth/format) oder 16 Argumente (mit depth/format/send_event)
            // Hier die 16-Argumente-Version von xcb_shm_put_image, um die Kompatibilität zu maximieren.
            xcb_shm_put_image(
                xstate.conn,
                xstate.back_buffer,
                xstate.gc_blit,
                (uint16_t)vid_w,            // total_width
                (uint16_t)vid_h,            // total_height
                (uint16_t)0,                // src_x
                (uint16_t)0,                // src_y
                (uint16_t)vid_w,            // src_width
                (uint16_t)vid_h,            // src_height
                (int16_t)video_x,           // dst_x
                (int16_t)video_y,           // dst_y
                (uint8_t)xstate.depth,      // depth (uint8_t)
                XCB_IMAGE_FORMAT_Z_PIXMAP,  // format
                (uint8_t)0,                 // send_event (false)
                xstate.shminfo.shmseg,
                (uint32_t)0                 // offset
            );
        } else {
            // Fallback für Nicht-SHM: Nutzt xcb_image_t und xcb_image_put
            xcb_image_t *img = xcb_image_create_native(xstate.conn, vid_w, vid_h, XCB_IMAGE_FORMAT_Z_PIXMAP,
                                                      xstate.depth, NULL, (size_t)tmp32_stride * (size_t)vid_h, (uint8_t*)tmp32);
            if (img) {
                // FIX: xcb_image_put mit 8 Argumenten (wie im Header /usr/include/xcb/xcb_image.h:347)
                xcb_image_put(xstate.conn, xstate.back_buffer, xstate.gc_blit,
                              img,
                              (int16_t)video_x,      // dst_x
                              (int16_t)video_y,      // dst_y
                              (int16_t)0,            // src_x (added argument)
                              (int16_t)0);           // src_y (added argument)
                xcb_image_destroy(img);
            }
        }

        if (xstate.shminfo.shmid < 0 && tmp32) free(tmp32); // Free if not SHM buffer
    } else {
        need_redraw = false; // Cannot redraw without buffer, don't waste CPU
    }

    /* draw HUD (text and rectangles) */
    char buf[256];
    int line_h = 24;
    int hud_x = layout.hud_x;
    int hud_y = layout.hud_y;
    int panel_x = layout.panel_x;

    xcb_change_gc(xstate.conn, xstate.gc_text, XCB_GC_FOREGROUND, &xstate.screen->white_pixel);

    /* Main Radiation Info */
    snprintf(buf,sizeof(buf),"Rad (raw): %.4f", measuredRadValue); draw_text_with_gc(hud_x, hud_y, buf, xstate.gc_text);
    hud_y += line_h;
    snprintf(buf,sizeof(buf),"Rad (filt): %.4f", gstate.filtered_rad_value); draw_text_with_gc(hud_x, hud_y, buf, xstate.gc_text);
    hud_y += line_h;
    snprintf(buf,sizeof(buf),"Dose (acc): %.3f", gstate.accumulated_dosage); draw_text_with_gc(hud_x, hud_y, buf, xstate.gc_text);
    hud_y += line_h;
    
    /* Global LD50 Status */
    double current_ld50_ratio = (lowest_ld50_threshold > 0.0) ? (gstate.accumulated_dosage / lowest_ld50_threshold) : 0.0;
    snprintf(buf,sizeof(buf),"LD50-Ratio: %5.2f%% / %5.2f", current_ld50_ratio * 100.0, lowest_ld50_threshold);
    draw_text_with_gc(hud_x, hud_y, buf, xstate.gc_text);
    hud_y += line_h;
    
    if (gstate.ld50_exceeded) {
        snprintf(buf,sizeof(buf),"CRITICAL LD50 EXCEEDED");
        draw_text_with_gc(xstate.win_w/2 - (int)strlen(buf)*4, hud_y, buf, xstate.gc_text);
        hud_y += line_h;
    }

    /* Uptime */
    char time_buf[64];
    format_time_ms(current_time_ms - gstate.start_time_ms, time_buf, sizeof(time_buf));
    snprintf(buf,sizeof(buf),"Uptime: %s", time_buf);
    draw_text_with_gc(hud_x, hud_y, buf, xstate.gc_text);
    hud_y += line_h;
    
    /* V4L2 Status */
    snprintf(buf,sizeof(buf),"Source: %s, %dx%d", vstate.simulated ? "SIM" : DEVICE_NODE, vstate.width, vstate.height);
    draw_text_with_gc(hud_x, hud_y, buf, xstate.gc_text);

    /* Panel Header */
    int list_y = layout.hud_y + 160;
    snprintf(buf,sizeof(buf), "Isotop | Name                    | LD50  | Rate  | Dose     | Ratio");
    draw_text_with_gc(panel_x, list_y, buf, xstate.gc_text);
    list_y += line_h;
    
    /* Panel List */
    int available_h = xstate.win_h - list_y;
    int to_draw = available_h / line_h;
    if (to_draw > NUM_ISOTOPES) to_draw = NUM_ISOTOPES;

    for (int i=0;i<to_draw;++i) {
        double ld = gstate.isotope_ld50_threshold[i];
        double rate = gstate.isotope_rad_rate[i];
        double dose = gstate.isotope_accumulated_dosage[i];
        double ratio = (ld>0.0) ? (dose/ld) : 0.0;

        uint32_t neon_fg = neon_pixel[i];
        xcb_change_gc(xstate.conn, xstate.gc_text, XCB_GC_FOREGROUND, &neon_fg);

        int y = list_y + i * line_h;
        if (isotope_count[i] == 0) {
            snprintf(buf,sizeof(buf), "%-6s | %-22s | %5.1f |   --   |    --    |  --",
                     isotope_abbr[i], isotope_name[i], ld);
        } else {
            snprintf(buf,sizeof(buf), "%-6s | %-22s | %5.1f | %6.3f | %8.3f | %5.2f",
                     isotope_abbr[i], isotope_name[i], ld, rate, dose, ratio);
        }
        draw_text_with_gc(panel_x, y, buf, xstate.gc_text);
    }
    xcb_change_gc(xstate.conn, xstate.gc_text, XCB_GC_FOREGROUND, &xstate.screen->white_pixel);

    /* copy back buffer to window */
    xcb_copy_area(xstate.conn, xstate.back_buffer, xstate.win, xstate.gc_blit, 0, 0, 0, 0, (uint16_t)xstate.win_w, (uint16_t)xstate.win_h);
    xcb_flush(xstate.conn);
    
    if (!got_frame) usleep(10000);
}

/* ---------- event loop ---------- */
static void xcb_handle_events(void) {
    xcb_generic_event_t *event;
    while ((event = xcb_poll_for_event(xstate.conn))) {
        switch (event->response_type & ~0x80) {
            case XCB_EXPOSE: {
                xcb_expose_event_t *e = (xcb_expose_event_t*)event;
                if (e->count == 0) need_redraw = true;
                break;
            }
            case XCB_CONFIGURE_NOTIFY: {
                xcb_configure_notify_event_t *e = (xcb_configure_notify_event_t*)event;
                if (e->width != (uint16_t)xstate.win_w || e->height != (uint16_t)xstate.win_h) {
                    xcb_recreate_buffers((int)e->width, (int)e->height);
                    need_redraw = true;
                }
                break;
            }
            case XCB_KEY_PRESS: {
                xcb_key_press_event_t *e = (xcb_key_press_event_t*)event;
                if (e->detail && xstate.keysyms) {
                    // FIX: Argument 'col' (column) wurde hinzugefügt
                    xcb_keysym_t keysym = xcb_key_symbols_get_keysym(xstate.keysyms, e->detail, 0);
                    if (keysym == 0x71 /* 'q' */ || keysym == 0xff1b /* Escape Keysym */) {
                        running = false;
                    } else if (keysym == 0x72 /* 'r' */) {
                        preserve_aspect = !preserve_aspect;
                        xcb_recreate_buffers(xstate.win_w, xstate.win_h);
                        need_redraw = true;
                    }
                }
                break;
            }
            case XCB_CLIENT_MESSAGE: {
                xcb_client_message_event_t *e = (xcb_client_message_event_t*)event;
                if (e->data.data32[0] == xstate.atom_wm_delete_window) running = false;
                break;
            }
        }
        free(event);
    }
}

/* ---------- main ---------- */
int main(int argc, char **argv) {
    (void)argc; (void)argv;
    signal(SIGINT, sigint_handler);
    srand((unsigned)time(NULL));
    vstate.width = SRC_WIDTH; vstate.height = SRC_HEIGHT; vstate.simulated = true;
    
    memset(&gstate, 0, sizeof(gstate));
    gstate.start_time_ms = (long long)(now_seconds() * 1000.0);
    gstate.dynamic_alpha = ALPHA_MIN;
    gstate.ld50_exceeded = false;
    gstate.ld50_threshold = LD50_LOW_RISK_THRESHOLD;
    
    for (int i=0;i<NUM_ISOTOPES;++i) {
        isotope_count[i]=0;
        gstate.isotope_ld50_exceeded[i]=false;
        if (i>=14) gstate.isotope_ld50_threshold[i]=LD50_HIGH_RISK_THRESHOLD;
        else if (i>=7) gstate.isotope_ld50_threshold[i]=LD50_MEDIUM_RISK_THRESHOLD;
        else gstate.isotope_ld50_threshold[i]=LD50_LOW_RISK_THRESHOLD;
    }
    
    gstate.alert_on_threshold = ALERT_THRESHOLD;
    gstate.alert_off_threshold = ALERT_THRESHOLD * 0.8;
    gstate.alert_active = false;
    gstate.alert_hold_ms = 1500;
    gstate.alert_last_change_ms = gstate.start_time_ms;
    gstate.ld50_on_ratio = 1.0;
    gstate.ld50_off_ratio = 0.95;

    alsa_init_persistent();
    
    int initial_w = SRC_WIDTH + (int)lrint(SRC_WIDTH * PANEL_RATIO);
    int initial_h = SRC_HEIGHT;

    if (!xcb_init(initial_w, initial_h)) {
        running = false;
    } else {
        pthread_t v4l2_tid;
        pthread_create(&v4l2_tid, NULL, v4l2_thread, NULL);
        
        while (running) {
            xcb_handle_events();
            xcb_redraw();
        }
        
        pthread_join(v4l2_tid, NULL);
    }

    alsa_close_persistent();
    xcb_close();
    
    return 0;
}
