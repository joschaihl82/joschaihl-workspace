// lapd_float441.c
// Build:   gcc -O2 -Wall -o lapd_float441 lapd_float441.c
// Run:     ./lapd_float441 [optional: /dev/snd/pcmC?D?p]
// Requires: ALSA kernel UAPI headers: <sound/asound.h>

#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <math.h>
#include <time.h>
#include <sound/asound.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -------------------------------------
// Helpers: bitmask & interval setters
// -------------------------------------
static void mask_clear(struct snd_mask *m) {
    memset(m, 0, sizeof(*m));
}
static void mask_set_bit(struct snd_mask *m, unsigned int bit) {
    unsigned int idx = bit / 32;
    unsigned int off = bit % 32;
    if (idx < (sizeof(m->bits)/sizeof(m->bits[0])))
        m->bits[idx] |= (1U << off);
}
static void interval_set(struct snd_interval *i, unsigned int min, unsigned int max) {
    memset(i, 0, sizeof(*i));
    i->min = min;
    i->max = max;
    i->openmin = 0;
    i->openmax = 0;
    i->integer = 1; // discrete values like rate, channels, period_size
}

// -------------------------------------------------
// Device discovery: first available playback device
// -------------------------------------------------
static char *find_default_playback_device(void) {
    for (int card = 0; card < 8; ++card) {
        char ctlpath[64];
        snprintf(ctlpath, sizeof(ctlpath), "/dev/snd/controlC%d", card);
        int cfd = open(ctlpath, O_RDWR);
        if (cfd < 0) continue;

        struct snd_ctl_card_info cinfo;
        memset(&cinfo, 0, sizeof(cinfo));
        if (ioctl(cfd, SNDRV_CTL_IOCTL_CARD_INFO, &cinfo) < 0) {
            close(cfd);
            continue;
        }

        int dev = -1;
        while (ioctl(cfd, SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, &dev) == 0 && dev >= 0) {
            struct snd_pcm_info pinfo;
            memset(&pinfo, 0, sizeof(pinfo));
            pinfo.device = dev;
            pinfo.subdevice = 0;
            pinfo.stream = SNDRV_PCM_STREAM_PLAYBACK;
            if (ioctl(cfd, SNDRV_CTL_IOCTL_PCM_INFO, &pinfo) == 0) {
                char *path = (char*)malloc(64);
                if (!path) { close(cfd); return NULL; }
                snprintf(path, 64, "/dev/snd/pcmC%dD%dp", card, dev);
                int testfd = open(path, O_RDWR);
                if (testfd >= 0) { close(testfd); close(cfd); return path; }
                free(path);
            }
        }
        close(cfd);
    }
    return NULL;
}

// -------------------------------------
// Minimal libasound-like API (ioctl)
// -------------------------------------
typedef struct {
    int fd;
    unsigned rate;                   // 44100
    unsigned channels;               // 1
    snd_pcm_format_t format;         // FLOAT_LE
    snd_pcm_access_t access;         // RW_INTERLEAVED
    snd_pcm_uframes_t period_size;   // e.g., 1024
} snd_pcm_t;

static int snd_pcm_open(snd_pcm_t **handle, const char *dev, int stream, int mode_flags) {
    (void)stream; (void)mode_flags;
    snd_pcm_t *h = (snd_pcm_t*)calloc(1, sizeof(*h));
    if (!h) return -ENOMEM;
    h->fd = open(dev, O_RDWR);
    if (h->fd < 0) { free(h); return -errno; }
    h->rate = 44100;
    h->channels = 1;
    h->format = SNDRV_PCM_FORMAT_FLOAT_LE;
    h->access = SNDRV_PCM_ACCESS_RW_INTERLEAVED;
    h->period_size = 1024;
    *handle = h;
    return 0;
}

// Set explicit constraints: 44.1 kHz, FLOAT_LE, mono, interleaved, period_size fixed
static int snd_pcm_hw_params(snd_pcm_t *h) {
    struct snd_pcm_hw_params hw;
    memset(&hw, 0, sizeof(hw));

    // Begin with refine so driver populates capability ranges
    if (ioctl(h->fd, SNDRV_PCM_IOCTL_HW_REFINE, &hw) < 0) return -errno;

    // We will set constraints via masks/intervals and indicate them through cmask
    // Access
    mask_clear(&hw.masks[SNDRV_PCM_HW_PARAM_ACCESS]);
    mask_set_bit(&hw.masks[SNDRV_PCM_HW_PARAM_ACCESS], h->access);
    hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_ACCESS);

    // Format: FLOAT_LE
    mask_clear(&hw.masks[SNDRV_PCM_HW_PARAM_FORMAT]);
    mask_set_bit(&hw.masks[SNDRV_PCM_HW_PARAM_FORMAT], h->format);
    hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_FORMAT);

    // Channels: 1
    interval_set(&hw.intervals[SNDRV_PCM_HW_PARAM_CHANNELS], h->channels, h->channels);
    hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_CHANNELS);

    // Rate: 44100 Hz
    interval_set(&hw.intervals[SNDRV_PCM_HW_PARAM_RATE], h->rate, h->rate);
    hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_RATE);

    // Period size: fixed
    interval_set(&hw.intervals[SNDRV_PCM_HW_PARAM_PERIOD_SIZE], h->period_size, h->period_size);
    hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_PERIOD_SIZE);

    // Buffer size: 4 * period (optional hint)
    snd_pcm_uframes_t buffer_size = h->period_size * 4;
    interval_set(&hw.intervals[SNDRV_PCM_HW_PARAM_BUFFER_SIZE], buffer_size, buffer_size);
    hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_BUFFER_SIZE);

    // Apply definitive parameters
    if (ioctl(h->fd, SNDRV_PCM_IOCTL_HW_PARAMS, &hw) < 0) return -errno;

    return 0;
}

static int snd_pcm_prepare(snd_pcm_t *h) {
    if (ioctl(h->fd, SNDRV_PCM_IOCTL_PREPARE) < 0) return -errno;
    return 0;
}

// Write interleaved frames (float32 little endian)
static ssize_t snd_pcm_writei(snd_pcm_t *h, const void *buffer, size_t frames) {
    struct snd_xferi xfer;
    memset(&xfer, 0, sizeof(xfer));
    xfer.buf = (void*)buffer;
    xfer.frames = frames;
    if (ioctl(h->fd, SNDRV_PCM_IOCTL_WRITEI_FRAMES, &xfer) < 0) {
        if (errno == EPIPE) {
            (void)snd_pcm_prepare(h);
            return 0; // recovered, nothing written this call
        }
        return -errno;
    }
    return xfer.frames;
}

static int snd_pcm_close(snd_pcm_t *h) {
    if (!h) return 0;
    int r = 0;
    if (h->fd >= 0) r = close(h->fd);
    free(h);
    return r;
}

// -------------------------------------
// Synthesis: float32 LE output
// -------------------------------------
static void gen_tone_f(float *buf, size_t frames,
                       double freq, unsigned rate, float amp,
                       double *phase) {
    double inc = 2.0 * M_PI * freq / (double)rate;
    double ph = *phase;
    for (size_t i = 0; i < frames; ++i) {
        buf[i] = (float)(sin(ph) * amp); // -1..+1 scaled by amp
        ph += inc;
        if (ph >= 2.0 * M_PI) ph -= 2.0 * M_PI;
    }
    *phase = ph;
}

static void fade_in_out_f(float *buf, size_t frames, unsigned rate, float ms) {
    size_t n = (size_t)(ms * 0.001f * rate);
    if (n == 0 || n * 2 >= frames) return;
    for (size_t i = 0; i < n; ++i) {
        float g = (float)i / (float)n;
        buf[i] *= g;
    }
    for (size_t i = 0; i < n; ++i) {
        float g = 1.0f - (float)i / (float)n;
        buf[frames - 1 - i] *= g;
    }
}

// -------------------------------------
// LAPD modes: Wail (Sweep), Yelp, Hi-Lo
// -------------------------------------
static void mode_wail(snd_pcm_t *h, float *buf, double *phase,
                      double duration_s, float amp) {
    size_t total = (size_t)(duration_s * h->rate);
    size_t period = h->period_size;
    for (size_t i = 0; i < total; i += period) {
        double t = (double)i / (double)h->rate;
        double freq = 1000.0 + 600.0 * sin(2.0 * M_PI * t / duration_s); // ~400..1600 Hz
        gen_tone_f(buf, period, freq, h->rate, amp, phase);
        (void)snd_pcm_writei(h, buf, period);
    }
}

static void mode_yelp(snd_pcm_t *h, float *buf, double *phase,
                      double total_s, double seg_s, float amp) {
    size_t seg_frames = (size_t)(seg_s * h->rate);
    int segments = (int)(total_s / seg_s);
    for (int s = 0; s < segments; ++s) {
        double freq = 600.0 + (rand() % 1001); // ~600..1600 Hz
        gen_tone_f(buf, seg_frames, freq, h->rate, amp, phase);
        fade_in_out_f(buf, seg_frames, h->rate, 8.0f);
        (void)snd_pcm_writei(h, buf, seg_frames);
    }
}

static void mode_hilo(snd_pcm_t *h, float *buf, double *phase,
                      double total_s, double seg_s, double f_lo, double f_hi, float amp) {
    size_t seg_frames = (size_t)(seg_s * h->rate);
    int segments = (int)(total_s / seg_s);
    for (int s = 0; s < segments; ++s) {
        double freq = (s % 2 == 0) ? f_lo : f_hi;
        gen_tone_f(buf, seg_frames, freq, h->rate, amp, phase);
        fade_in_out_f(buf, seg_frames, h->rate, 8.0f);
        (void)snd_pcm_writei(h, buf, seg_frames);
    }
}

// -------------------------------------
// Main: pick device, set 44.1kHz float, play modes
// -------------------------------------
int main(int argc, char **argv) {
    srand((unsigned)time(NULL));

    char *devpath = NULL;
    if (argc > 1) {
        devpath = strdup(argv[1]);
    } else {
        devpath = find_default_playback_device();
        if (!devpath) {
            fprintf(stderr, "Kein Playback-Device gefunden.\n");
            return 1;
        }
    }

    snd_pcm_t *h = NULL;
    int rc = snd_pcm_open(&h, devpath, /*stream*/0, /*mode*/0);
    if (rc < 0) { fprintf(stderr, "snd_pcm_open(%s) fehlgeschlagen: %s\n", devpath, strerror(-rc)); free(devpath); return 1; }

    rc = snd_pcm_hw_params(h);
    if (rc < 0) { fprintf(stderr, "snd_pcm_hw_params fehlgeschlagen: %s\n", strerror(-rc)); free(devpath); snd_pcm_close(h); return 1; }

    rc = snd_pcm_prepare(h);
    if (rc < 0) { fprintf(stderr, "snd_pcm_prepare fehlgeschlagen: %s\n", strerror(-rc)); free(devpath); snd_pcm_close(h); return 1; }

    fprintf(stderr, "Gerät: %s | Rate: %u Hz | Format: FLOAT_LE | Channels: %u\n",
            devpath, h->rate, h->channels);

    size_t max_frames = h->rate; // bis 1 Sekunde
    float *buf = (float*)malloc(max_frames * sizeof(float));
    if (!buf) { perror("malloc"); free(devpath); snd_pcm_close(h); return 1; }

    double phase = 0.0;
    const float amp = 0.7f;

    for (;;) {
        // Wail / Sweep: ~4 s
        mode_wail(h, buf, &phase, 4.0, amp);

        // Yelp: ~3.6 s, 0.18 s Segmente
        mode_yelp(h, buf, &phase, 3.6, 0.18, amp);

        // Hi-Lo: ~4 s, 0.25 s je Ton, 500 Hz <-> 1500 Hz
        mode_hilo(h, buf, &phase, 4.0, 0.25, 500.0, 1500.0, amp);
    }

    free(buf);
    free(devpath);
    snd_pcm_close(h);
    return 0;
}

