/*
 lapd_amalgamated.c
 - Single-file portable C using libc (no inline asm)
 - ALSA kernel UAPI via <sound/asound.h>
 - 44.1 kHz, FLOAT_LE, stereo interleaved only
 - Includes snd_mask_none helper and a full amalgamated implementation
 - Build: gcc -O2 -Wall -o lapd_amalgamated lapd_amalgamated.c -lm
 - Run:   ./lapd_amalgamated [optional: /dev/snd/pcmC0D0p]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <math.h>
#include <sound/asound.h>

/* --- Utility: integer to decimal into buffer --- */
static int itoa_dec(char *buf, int v) {
    char tmp[16]; int n=0, sign = v<0;
    unsigned int u = sign ? (unsigned int)(-v) : (unsigned int)v;
    if (v==0) { buf[0]='0'; buf[1]=0; return 1; }
    while (u) { tmp[n++] = (char)('0' + (u%10)); u/=10; }
    int i=0; if (sign) buf[i++]='-';
    while (n--) buf[i++]=tmp[n];
    buf[i]=0; return i;
}

/* --- Find first usable playback device (pcmC#D#p) --- */
static char *find_default_playback_device(void)
{
    char ctlpath[64];
    for (int card = 0; card < 8; ++card) {
        snprintf(ctlpath, sizeof(ctlpath), "/dev/snd/controlC%d", card);
        int cfd = open(ctlpath, O_RDWR);
        if (cfd < 0) continue;

        struct snd_ctl_card_info cinfo;
        memset(&cinfo, 0, sizeof(cinfo));
        if (ioctl(cfd, SNDRV_CTL_IOCTL_CARD_INFO, &cinfo) < 0) { close(cfd); continue; }

        int dev = -1;
        while (ioctl(cfd, SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, &dev) == 0 && dev >= 0) {
            struct snd_pcm_info pinfo;
            memset(&pinfo, 0, sizeof(pinfo));
            pinfo.device = dev;
            pinfo.subdevice = 0;
            pinfo.stream = SNDRV_PCM_STREAM_PLAYBACK;
            if (ioctl(cfd, SNDRV_CTL_IOCTL_PCM_INFO, &pinfo) == 0) {
                char *path = malloc(64);
                if (!path) { close(cfd); return NULL; }
                snprintf(path, 64, "/dev/snd/pcmC%dD%dp", card, dev);
                int tfd = open(path, O_RDWR);
                if (tfd >= 0) { close(tfd); close(cfd); return path; }
                free(path);
            }
        }
        close(cfd);
    }
    return NULL;
}

/* --- snd_mask_none helper (explicit request) --- */
static void snd_mask_none(struct snd_mask *m) {
    for (int i = 0; i < (SNDRV_MASK_MAX/32); ++i) m->bits[i] = 0u;
}

/* --- Minimal libasound-like wrapper using PCM ioctl interface (kernel UAPI) --- */
typedef struct {
    int fd;
    unsigned int rate;
    unsigned int channels;
    snd_pcm_format_t format;
    snd_pcm_access_t access;
    snd_pcm_uframes_t period_size;
} snd_pcm_simple_t;

static int snd_pcm_open_simple(snd_pcm_simple_t **ph, const char *devpath)
{
    snd_pcm_simple_t *h = calloc(1, sizeof(*h));
    if (!h) return -ENOMEM;

    int fd = open(devpath, O_RDWR);
    if (fd < 0) { free(h); return -errno; }

    h->fd = fd;
    h->rate = 44100;
    h->channels = 2; /* stereo */
    h->format = SNDRV_PCM_FORMAT_FLOAT_LE;
    h->access = SNDRV_PCM_ACCESS_RW_INTERLEAVED;
    h->period_size = 1024;

    *ph = h;
    return 0;
}

static struct snd_mask *hw_mask(struct snd_pcm_hw_params *p, int idx)
{
    return &p->masks[idx - SNDRV_PCM_HW_PARAM_FIRST_MASK];
}
static struct snd_interval *hw_interval(struct snd_pcm_hw_params *p, int idx)
{
    return &p->intervals[idx - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
}

static int snd_pcm_hw_params_simple(snd_pcm_simple_t *h)
{
    struct snd_pcm_hw_params hw;
    memset(&hw, 0, sizeof(hw));

    if (ioctl(h->fd, SNDRV_PCM_IOCTL_HW_REFINE, &hw) < 0) {
        return -errno;
    }

    /* Access */
    snd_mask_none(hw_mask(&hw, SNDRV_PCM_HW_PARAM_ACCESS));
    /* set the requested access bit */
    {
        unsigned int access = h->access;
        unsigned int idx = access / 32, off = access % 32;
        if (idx < (SNDRV_MASK_MAX/32)) hw_mask(&hw, SNDRV_PCM_HW_PARAM_ACCESS)->bits[idx] |= (1u << off);
        hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_ACCESS);
    }

    /* Format */
    snd_mask_none(hw_mask(&hw, SNDRV_PCM_HW_PARAM_FORMAT));
    {
        unsigned int fmt = (unsigned int)h->format;
        unsigned int idx = fmt / 32, off = fmt % 32;
        if (idx < (SNDRV_MASK_MAX/32)) hw_mask(&hw, SNDRV_PCM_HW_PARAM_FORMAT)->bits[idx] |= (1u << off);
        hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_FORMAT);
    }

    /* Subformat STD */
    snd_mask_none(hw_mask(&hw, SNDRV_PCM_HW_PARAM_SUBFORMAT));
    {
        unsigned int sf = SNDRV_PCM_SUBFORMAT_STD;
        unsigned int idx = sf / 32, off = sf % 32;
        if (idx < (SNDRV_MASK_MAX/32)) hw_mask(&hw, SNDRV_PCM_HW_PARAM_SUBFORMAT)->bits[idx] |= (1u << off);
        hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_SUBFORMAT);
    }

    /* Channels */
    {
        struct snd_interval *it = hw_interval(&hw, SNDRV_PCM_HW_PARAM_CHANNELS);
        memset(it, 0, sizeof(*it));
        it->min = h->channels; it->max = h->channels; it->integer = 1;
        hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_CHANNELS);
    }

    /* Rate */
    {
        struct snd_interval *it = hw_interval(&hw, SNDRV_PCM_HW_PARAM_RATE);
        memset(it, 0, sizeof(*it));
        it->min = h->rate; it->max = h->rate; it->integer = 1;
        hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_RATE);
    }

    /* Period size */
    {
        struct snd_interval *it = hw_interval(&hw, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
        memset(it, 0, sizeof(*it));
        it->min = (unsigned)h->period_size; it->max = (unsigned)h->period_size; it->integer = 1;
        hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
    }

    /* Buffer size = 4 * period */
    {
        struct snd_interval *it = hw_interval(&hw, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
        memset(it, 0, sizeof(*it));
        it->min = (unsigned)(h->period_size * 4); it->max = (unsigned)(h->period_size * 4); it->integer = 1;
        hw.cmask |= (1U << SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
    }

    if (ioctl(h->fd, SNDRV_PCM_IOCTL_HW_PARAMS, &hw) < 0) {
        return -errno;
    }
    return 0;
}

static int snd_pcm_prepare_simple(snd_pcm_simple_t *h)
{
    if (ioctl(h->fd, SNDRV_PCM_IOCTL_PREPARE, 0) < 0) return -errno;
    return 0;
}

static ssize_t snd_pcm_writei_simple(snd_pcm_simple_t *h, const void *buf, size_t frames)
{
    struct snd_xferi x;
    memset(&x, 0, sizeof(x));
    x.buf = (void*)buf;
    x.frames = frames;
    if (ioctl(h->fd, SNDRV_PCM_IOCTL_WRITEI_FRAMES, &x) < 0) {
        if (errno == EPIPE) {
            /* underrun -> recover */
            snd_pcm_prepare_simple(h);
            return 0;
        }
        return -errno;
    }
    return (ssize_t)x.frames;
}

static int snd_pcm_close_simple(snd_pcm_simple_t *h)
{
    if (!h) return 0;
    int r = 0;
    if (h->fd >= 0) r = close(h->fd);
    free(h);
    return r;
}

/* --- Synthesis helpers (stereo, interleaved) --- */
static void gen_tone_stereo(float *buf, size_t frames, double fl, double fr, unsigned rate, float amp, double *phl, double *phr)
{
    const double TWO_PI = 6.28318530717958647692;
    double inc_l = TWO_PI * fl / (double)rate;
    double inc_r = TWO_PI * fr / (double)rate;
    double pl = *phl;
    double pr = *phr;
    for (size_t i = 0; i < frames; ++i) {
        buf[2*i + 0] = (float)(sin(pl) * amp);
        buf[2*i + 1] = (float)(sin(pr) * amp);
        pl += inc_l; if (pl >= TWO_PI) pl -= TWO_PI;
        pr += inc_r; if (pr >= TWO_PI) pr -= TWO_PI;
    }
    *phl = pl; *phr = pr;
}

static void fade_in_out_stereo(float *buf, size_t frames, unsigned rate, float ms)
{
    size_t n = (size_t)(ms * 0.001f * (float)rate);
    if (n == 0 || n * 2 >= frames) return;
    for (size_t i = 0; i < n; ++i) {
        float g = (float)i / (float)n;
        buf[2*i + 0] *= g; buf[2*i + 1] *= g;
    }
    for (size_t i = 0; i < n; ++i) {
        float g = 1.0f - (float)i / (float)n;
        buf[2*(frames - 1 - i) + 0] *= g; buf[2*(frames - 1 - i) + 1] *= g;
    }
}

/* --- LAPD siren modes (stereo: slight detune on right) --- */
static void mode_wail(snd_pcm_simple_t *h, float *buf, double *phl, double *phr, double duration_s, float amp)
{
    size_t total = (size_t)(duration_s * h->rate);
    size_t period = h->period_size;
    const double TWO_PI = 6.28318530717958647692;
    for (size_t i = 0; i < total; i += period) {
        double t = (double)i / (double)h->rate;
        double f = 1000.0 + 600.0 * sin((TWO_PI * t) / duration_s); /* ~400..1600 Hz */
        double fl = f;
        double fr = f * 1.01;
        gen_tone_stereo(buf, period, fl, fr, h->rate, amp, phl, phr);
        snd_pcm_writei_simple(h, buf, period);
    }
}

static void mode_yelp(snd_pcm_simple_t *h, float *buf, double *phl, double *phr, double total_s, double seg_s, float amp)
{
    size_t seg_frames = (size_t)(seg_s * h->rate);
    int segments = (int)(total_s / seg_s);
    for (int s = 0; s < segments; ++s) {
        double freq = 600.0 + (rand() % 1001); /* ~600..1600 Hz */
        double fl = freq;
        double fr = freq * 1.02;
        gen_tone_stereo(buf, seg_frames, fl, fr, h->rate, amp, phl, phr);
        fade_in_out_stereo(buf, seg_frames, h->rate, 8.0f);
        snd_pcm_writei_simple(h, buf, seg_frames);
    }
}

static void mode_hilo(snd_pcm_simple_t *h, float *buf, double *phl, double *phr, double total_s, double seg_s, double f_lo, double f_hi, float amp)
{
    size_t seg_frames = (size_t)(seg_s * h->rate);
    int segments = (int)(total_s / seg_s);
    for (int s = 0; s < segments; ++s) {
        double f = (s % 2 == 0) ? f_lo : f_hi;
        double fl = f;
        double fr = f * 1.01;
        gen_tone_stereo(buf, seg_frames, fl, fr, h->rate, amp, phl, phr);
        fade_in_out_stereo(buf, seg_frames, h->rate, 8.0f);
        snd_pcm_writei_simple(h, buf, seg_frames);
    }
}

/* --- main --- */
int main(int argc, char **argv)
{
    srand((unsigned)time(NULL));

    char *devpath = NULL;
    if (argc > 1) {
        devpath = strdup(argv[1]);
    } else {
        devpath = find_default_playback_device();
        if (!devpath) {
            fprintf(stderr, "No playback device found.\n");
            return 1;
        }
    }

    snd_pcm_simple_t *pcm = NULL;
    int rc = snd_pcm_open_simple(&pcm, devpath);
    if (rc < 0) { fprintf(stderr, "snd_pcm_open failed: %s\n", strerror(-rc)); free(devpath); return 1; }

    rc = snd_pcm_hw_params_simple(pcm);
    if (rc < 0) { fprintf(stderr, "snd_pcm_hw_params failed: %s\n", strerror(-rc)); snd_pcm_close_simple(pcm); free(devpath); return 1; }

    rc = snd_pcm_prepare_simple(pcm);
    if (rc < 0) { fprintf(stderr, "snd_pcm_prepare failed: %s\n", strerror(-rc)); snd_pcm_close_simple(pcm); free(devpath); return 1; }

    fprintf(stderr, "Using device %s -- 44100 Hz, FLOAT_LE, %u channels\n", devpath, pcm->channels);

    size_t max_frames = pcm->rate;
    float *buf = malloc(max_frames * pcm->channels * sizeof(float));
    if (!buf) { perror("malloc"); snd_pcm_close_simple(pcm); free(devpath); return 1; }

    double phase_l = 0.0, phase_r = 0.0;
    const float amp = 0.7f;

    for (;;) {
        mode_wail(pcm, buf, &phase_l, &phase_r, 4.0, amp);
        mode_yelp(pcm, buf, &phase_l, &phase_r, 3.6, 0.18, amp);
        mode_hilo(pcm, buf, &phase_l, &phase_r, 4.0, 0.25, 500.0, 1500.0, amp);
    }

    /* unreachable cleanup */
    free(buf);
    snd_pcm_close_simple(pcm);
    free(devpath);
    return 0;
}

