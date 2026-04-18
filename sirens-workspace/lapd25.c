
/*
Below is a single-file program that does exactly what you asked: it first tries the fixed device /dev/snd/pcmC0D0p (card 0, device 0) and if that fails it falls back to scanning /dev/snd for other pcm* nodes (optionally preferring playback nodes ending with 'p'). It uses local ALSA ioctl/type definitions so it compiles without <sound/asound.h>, tries ioctl negotiation, and falls back to a raw S16_LE write test when ioctl fails. No libasound.

Save as lapd_all.c and compile:
cc -O2 -std=c11 -Wall -Wextra -o lapd_all lapd_all.c

Usage:
- ./lapd_all           (tries /dev/snd/pcmC0D0p first, then scans)
- ./lapd_all -p        (same, but when scanning prefers nodes ending with 'p' first)
- ./lapd_all -h

Code:
```c

 lapd_all.c
 Try /dev/snd/pcmC0D0p first, then scan /dev/snd for pcm* nodes.
 Use ioctl negotiation (HW_REFINE -> HW_PARAMS -> SW_PARAMS -> PREPARE)
 and fall back to a raw S16_LE write test if ioctl fails.
 No libasound.
 Build: cc -O2 -std=c11 -Wall -Wextra -o lapd_all lapd_all.c
*/

#define _GNU_SOURCE
/* lapd25.c - (brief one-line description) */
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include <getopt.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ---------------- local ALSA ABI (subset) ------------------------------- */
typedef unsigned int    __u32;
typedef int             __s32;
typedef unsigned long   snd_pcm_uframes_t;

/* access types */
#define SNDRV_PCM_ACCESS_MMAP_INTERLEAVED      0
#define SNDRV_PCM_ACCESS_MMAP_NONINTERLEAVED   1
#define SNDRV_PCM_ACCESS_MMAP_COMPLEX          2
#define SNDRV_PCM_ACCESS_RW_INTERLEAVED        3
#define SNDRV_PCM_ACCESS_RW_NONINTERLEAVED     4

/* formats (partial) */
#define SNDRV_PCM_FORMAT_S16_LE     2

typedef struct snd_pcm_hw_params {
    __u32          flags;
    __u32          masks[64];
    __u32          rmask[64];
    __u32          cmasks[64];
    __u32          info;
    __u32          reserved[3];
    unsigned int   access;
    unsigned int   format;
    unsigned int   subformat;
    unsigned int   channels;
    unsigned int   rate_num;
    unsigned int   rate_den;
    unsigned int   period_time;
    snd_pcm_uframes_t period_size;
    unsigned int   period_step;
    unsigned int   periods;
    snd_pcm_uframes_t buffer_time;
    snd_pcm_uframes_t buffer_size;
    unsigned int   back_avail_min;
    unsigned int   sig_bits;
    unsigned int   reserved4[15];
} snd_pcm_hw_params;

typedef struct snd_pcm_sw_params {
    __u32 tstamp_mode;
    __u32 period_step;
    __u32 sleep_min;
    __u32 avail_min;
    __u32 xfer_align;
    __u32 start_threshold;
    __u32 stop_threshold;
    __u32 silence_threshold;
    __u32 silence_size;
    __u32 boundary;
    __u32 reserved[48];
} snd_pcm_sw_params;

/* ioctl encodings (common ALSA encoding) */
#ifndef SNDRV_PCM_IOCTL_HW_PARAMS
#define SNDRV_PCM_IOCTL_HW_PARAMS   _IOWR('A', 0x10, struct snd_pcm_hw_params)
#endif
#ifndef SNDRV_PCM_IOCTL_HW_REFINE
#define SNDRV_PCM_IOCTL_HW_REFINE   _IOWR('A', 0x11, struct snd_pcm_hw_params)
#endif
#ifndef SNDRV_PCM_IOCTL_SW_PARAMS
#define SNDRV_PCM_IOCTL_SW_PARAMS   _IOWR('A', 0x12, struct snd_pcm_sw_params)
#endif
#ifndef SNDRV_PCM_IOCTL_PREPARE
#define SNDRV_PCM_IOCTL_PREPARE     _IOWR('A', 0x01, int)
#endif
#ifndef SNDRV_PCM_IOCTL_DRAIN
#define SNDRV_PCM_IOCTL_DRAIN       _IOWR('A', 0x02, int)
#endif

/* ---------------- program params -------------------------------------- */
#define SR 44100
#define CHANNELS 2
#define FORMAT_REQ SNDRV_PCM_FORMAT_S16_LE
#define TEST_FRAMES 128
#define ITERATIONS 8
#define FADE_MS 4

typedef struct { double f0,f1,rate; } ctx_siren;

/* ---------------- utilities ------------------------------------------- */
static void perrorf(const char *msg) { fprintf(stderr, "%s: %s\n", msg, strerror(errno)); }
static double lerp(double a, double b, double t) { return a + (b - a) * t; }

/* ---------------- try ioctl negotiation ------------------------------- */
static int try_configure_device_ioctl(int fd, snd_pcm_uframes_t *out_period_size, snd_pcm_uframes_t *out_buffer_size) {
    snd_pcm_hw_params hw;
    memset(&hw, 0, sizeof(hw));
    hw.access = SNDRV_PCM_ACCESS_RW_INTERLEAVED;
    hw.format = FORMAT_REQ;
    hw.channels = CHANNELS;
    hw.rate_num = SR;
    hw.rate_den = 1;
    hw.periods = 4;
    hw.period_size = 1024;
    hw.buffer_size = hw.period_size * hw.periods;

    if (ioctl(fd, SNDRV_PCM_IOCTL_HW_REFINE, &hw) < 0) {
        if (errno != ENOTTY && errno != EINVAL) {
            perrorf("HW_REFINE");
            return -1;
        }
    }
    if (ioctl(fd, SNDRV_PCM_IOCTL_HW_PARAMS, &hw) < 0) {
        perrorf("HW_PARAMS");
        return -1;
    }

    snd_pcm_sw_params sw;
    memset(&sw, 0, sizeof(sw));
    sw.tstamp_mode = 0;
    sw.period_step = 1;
    sw.avail_min = hw.period_size ? hw.period_size : 1024;
    sw.start_threshold = hw.buffer_size ? hw.buffer_size / 2 : sw.avail_min;
    sw.stop_threshold = 0;
    sw.xfer_align = 1;

    if (ioctl(fd, SNDRV_PCM_IOCTL_SW_PARAMS, &sw) < 0) {
        perrorf("SW_PARAMS");
        return -1;
    }
    if (ioctl(fd, SNDRV_PCM_IOCTL_PREPARE, 0) < 0) {
        perrorf("PREPARE");
        return -1;
    }

    if (out_period_size) *out_period_size = hw.period_size;
    if (out_buffer_size) *out_buffer_size = hw.buffer_size;
    return 0;
}

/* ---------------- try raw write test ---------------------------------- */
static int try_write_test_raw(int fd) {
    int16_t buf[TEST_FRAMES * CHANNELS];
    memset(buf, 0, sizeof(buf));
    ssize_t w = write(fd, buf, sizeof(buf));
    if (w < 0) { perrorf("raw write test"); return -1; }
    return 0;
}

/* ---------------- synth helpers -------------------------------------- */
static inline double tri_lfo(double t, double rate) {
    double p = fmod(t * rate, 1.0);
    if (p < 0.0) p += 1.0;
    if (p < 0.5) return 4.0 * p - 1.0;
    return 3.0 - 4.0 * p;
}
static inline double sweep_freq_tri(double t, double rate, double f0, double f1) {
    double l = tri_lfo(t, rate);
    double mid = 0.5 * (f0 + f1);
    double half = 0.5 * (f1 - f0);
    return mid + half * l;
}
static double freq_wail(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }
static double freq_yelp(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }

static int synth_block(float **outbuf, size_t *out_frames, double (*freq_fn)(double, void*), void *ctx, double dur, int sr) {
    if (dur <= 0.0) { *outbuf = NULL; *out_frames = 0; return 0; }
    size_t frames = (size_t)ceil(dur * sr);
    float *buf = calloc(frames, sizeof(float));
    if (!buf) { fprintf(stderr,"OOM synth\n"); return -1; }
    double phase = 0.0, t = 0.0, dt = 1.0 / (double)sr;
    for (size_t i = 0; i < frames; ++i) {
        double f = freq_fn(t, ctx);
        double incr = 2.0 * M_PI * f * dt;
        phase += incr;
        if (phase >= 2.0*M_PI || phase < 0.0) phase = fmod(phase, 2.0*M_PI);
        buf[i] = (float)(sin(phase) * 0.85);
        t += dt;
    }
    *outbuf = buf; *out_frames = frames;
    return 0;
}

static void apply_fade_in_out(float *buf, size_t frames, int sr, unsigned fade_ms) {
    if (!buf || frames == 0 || fade_ms == 0 || sr <= 0) return;
    size_t fade_samples = (size_t)((double)sr * fade_ms / 1000.0);
    if (fade_samples == 0) fade_samples = 1;
    if (fade_samples * 2 > frames) fade_samples = frames / 2;
    for (size_t i = 0; i < fade_samples; ++i) {
        double g = (double)(i + 1) / (double)fade_samples;
        buf[i] = (float)(buf[i] * g);
    }
    for (size_t i = 0; i < fade_samples; ++i) {
        double g = (double)(fade_samples - i) / (double)fade_samples;
        buf[frames - fade_samples + i] = (float)(buf[frames - fade_samples + i] * g);
    }
}

static void float_mono_to_interleaved_s16(const float *mono, size_t frames, int16_t *out, unsigned channels) {
    for (size_t i = 0; i < frames; ++i) {
        double s = mono[i];
        if (s > 1.0) s = 1.0; else if (s < -1.0) s = -1.0;
        int16_t v = (int16_t)lrint(s * 32767.0);
        for (unsigned c = 0; c < channels; ++c) out[i*channels + c] = v;
    }
}

static int write_full_chunked(int fd, const int16_t *buf, size_t frames, unsigned channels, size_t chunk_frames) {
    size_t pos = 0;
    while (pos < frames) {
        size_t n = frames - pos;
        if (chunk_frames && n > chunk_frames) n = chunk_frames;
        size_t bytes = n * channels * sizeof(int16_t);
        ssize_t w = write(fd, (const char*)buf + pos * channels * sizeof(int16_t), bytes);
        if (w < 0) {
            if (errno == EAGAIN) { struct timespec ts = {0,1000000}; nanosleep(&ts,NULL); continue; }
            if (errno == EPIPE) { ioctl(fd, SNDRV_PCM_IOCTL_PREPARE, 0); continue; }
            perrorf("write");
            return -1;
        }
        pos += (size_t)(w / (channels * sizeof(int16_t)));
    }
    return 0;
}

/* ---------------- attempt /dev/snd/pcmC0D0p first, then scan ------------- */

static int attempt_path(const char *path, int *out_fd, snd_pcm_uframes_t *out_period, snd_pcm_uframes_t *out_buf) {
    int fd = open(path, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        fprintf(stderr, "open %s failed: %s\n", path, strerror(errno));
        return -1;
    }
    fprintf(stderr, "Opened fixed candidate %s (fd=%d)\n", path, fd);
    snd_pcm_uframes_t period = 0, buffer = 0;
    if (try_configure_device_ioctl(fd, &period, &buffer) == 0) {
        if (try_write_test_raw(fd) == 0) {
            *out_fd = fd; if (out_period) *out_period = period; if (out_buf) *out_buf = buffer;
            return 0;
        }
    } else {
        /* ioctl failed; try raw write fallback */
        if (try_write_test_raw(fd) == 0) {
            *out_fd = fd; if (out_period) *out_period = 1024; if (out_buf) *out_buf = 1024*4;
            return 0;
        }
    }
    close(fd);
    return -1;
}

static int try_all_pcm_devices_pref(int prefer_playback, char *out_path, size_t out_len, int *out_fd, snd_pcm_uframes_t *out_period, snd_pcm_uframes_t *out_buf) {
    /* First try card 0 device 0 as requested */
    const char *first = "/dev/snd/pcmC0D0p";
    if (attempt_path(first, out_fd, out_period, out_buf) == 0) {
        if (out_path && out_len) { strncpy(out_path, first, out_len-1); out_path[out_len-1] = '\0'; }
        return 0;
    }

    /* If first failed, scan /dev/snd as before with preference option */
    const char *dirpath = "/dev/snd";
    DIR *d = opendir(dirpath);
    if (!d) return -1;
    struct dirent *ent;
    char **play = NULL; size_t play_n = 0;
    char **other = NULL; size_t other_n = 0;
    while ((ent = readdir(d)) != NULL) {
        if (strncmp(ent->d_name, "pcm", 3) != 0) continue;
        char full[512];
        if (snprintf(full, sizeof(full), "%s/%s", dirpath, ent->d_name) >= (int)sizeof(full)) continue;
        /* skip the one we already tried */
        if (strcmp(full, first) == 0) continue;
        int is_play = (ent->d_name[strlen(ent->d_name)-1] == 'p');
        char *dup = strdup(full);
        if (!dup) continue;
        if (is_play) { play = realloc(play, (play_n+1)*sizeof(char*)); play[play_n++] = dup; }
        else       { other = realloc(other, (other_n+1)*sizeof(char*)); other[other_n++] = dup; }
    }
    closedir(d);

    /* Try playback-first or directory order depending on prefer_playback */
    if (prefer_playback) {
        for (size_t i=0;i<play_n;++i) {
            int fd = open(play[i], O_RDWR | O_NONBLOCK);
            if (fd < 0) { fprintf(stderr, "open %s failed: %s\n", play[i], strerror(errno)); continue; }
            fprintf(stderr, "Opened candidate %s (fd=%d) playback=1\n", play[i], fd);
            snd_pcm_uframes_t period=0, buffer=0; int ok=0;
            if (try_configure_device_ioctl(fd, &period, &buffer) == 0) ok = (try_write_test_raw(fd)==0);
            else ok = (try_write_test_raw(fd)==0);
            if (ok) { if (out_path) strncpy(out_path, play[i], out_len-1); *out_fd = fd; if (out_period) *out_period = period?period:1024; if (out_buf) *out_buf = buffer?buffer:1024*4; goto success; }
            close(fd);
        }
        for (size_t i=0;i<other_n;++i) {
            int fd = open(other[i], O_RDWR | O_NONBLOCK);
            if (fd < 0) { fprintf(stderr, "open %s failed: %s\n", other[i], strerror(errno)); continue; }
            fprintf(stderr, "Opened candidate %s (fd=%d) playback=0\n", other[i], fd);
            int ok = (try_write_test_raw(fd) == 0);
            if (ok) { if (out_path) strncpy(out_path, other[i], out_len-1); *out_fd = fd; if (out_period) *out_period = 1024; if (out_buf) *out_buf = 1024*4; goto success; }
            close(fd);
        }
    } else {
        /* try others first then play */
        for (size_t i=0;i<other_n;++i) {
            int fd = open(other[i], O_RDWR | O_NONBLOCK);
            if (fd < 0) { fprintf(stderr, "open %s failed: %s\n", other[i], strerror(errno)); continue; }
            fprintf(stderr, "Opened candidate %s (fd=%d) playback=0\n", other[i], fd);
            int ok = (try_write_test_raw(fd) == 0);
            if (ok) { if (out_path) strncpy(out_path, other[i], out_len-1); *out_fd = fd; if (out_period) *out_period = 1024; if (out_buf) *out_buf = 1024*4; goto success; }
            close(fd);
        }
        for (size_t i=0;i<play_n;++i) {
            int fd = open(play[i], O_RDWR | O_NONBLOCK);
            if (fd < 0) { fprintf(stderr, "open %s failed: %s\n", play[i], strerror(errno)); continue; }
            fprintf(stderr, "Opened candidate %s (fd=%d) playback=1\n", play[i], fd);
            snd_pcm_uframes_t period=0, buffer=0; int ok=0;
            if (try_configure_device_ioctl(fd, &period, &buffer) == 0) ok = (try_write_test_raw(fd)==0);
            else ok = (try_write_test_raw(fd)==0);
            if (ok) { if (out_path) strncpy(out_path, play[i], out_len-1); *out_fd = fd; if (out_period) *out_period = period?period:1024; if (out_buf) *out_buf = buffer?buffer:1024*4; goto success; }
            close(fd);
        }
    }

    /* cleanup and fail */
    for (size_t j=0;j<play_n;++j) free(play[j]);
    for (size_t j=0;j<other_n;++j) free(other[j]);
    free(play); free(other);
    return -1;

success:
    for (size_t j=0;j<play_n;++j) free(play[j]);
    for (size_t j=0;j<other_n;++j) free(other[j]);
    free(play); free(other);
    return 0;
}

/* ---------------- main ------------------------------------------------- */

static void usage(const char *argv0) {
    fprintf(stderr, "Usage: %s [options]\n", argv0);
    fprintf(stderr, "  -p, --prefer-playback   try nodes ending with 'p' first when scanning\n");
    fprintf(stderr, "  -h, --help              show this help\n");
}

int main(int argc, char **argv) {
    int prefer_playback = 0;
    static struct option longopts[] = {
        {"prefer-playback", no_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {NULL,0,NULL,0}
    };
    int opt;
    while ((opt = getopt_long(argc, argv, "ph", longopts, NULL)) != -1) {
        if (opt == 'p') prefer_playback = 1;
        else if (opt == 'h') { usage(argv[0]); return 0; }
        else { usage(argv[0]); return 1; }
    }

    char chosen[256] = {0};
    int fd = -1;
    snd_pcm_uframes_t period = 0, bufsize = 0;
    if (try_all_pcm_devices_pref(prefer_playback, chosen, sizeof(chosen), &fd, &period, &bufsize) < 0) {
        fprintf(stderr, "No usable playback device found under /dev/snd\n");
        return 1;
    }
    fprintf(stderr, "Selected device: %s (fd=%d) period=%lu buffer=%lu\n", chosen, fd, (unsigned long)period, (unsigned long)bufsize);

    ctx_siren wctx = {880.0, 880.0*2.0, 12.0/60.0};
    ctx_siren yctx = {880.0, 880.0*2.0, 180.0/60.0};

    for (int iter = 0; iter < ITERATIONS; ++iter) {
        double t = (ITERATIONS == 1) ? 0.0 : (double)iter / (double)(ITERATIONS - 1);
        double wail_dur = lerp(10.0, 7.5, t);
        double yelp_dur = lerp(0.0, 2.5, t);

        if (yelp_dur > 0.0) {
            float *blk = NULL; size_t bframes = 0;
            if (synth_block(&blk, &bframes, freq_yelp, &yctx, yelp_dur, SR) != 0) goto done;
            apply_fade_in_out(blk, bframes, SR, FADE_MS);
            int16_t *tmp = malloc(sizeof(int16_t) * bframes * CHANNELS);
            if (!tmp) { free(blk); goto done; }
            float_mono_to_interleaved_s16(blk, bframes, tmp, CHANNELS);
            if (write_full_chunked(fd, tmp, bframes, CHANNELS, (size_t)(period ? period : 1024)) != 0) { free(tmp); free(blk); goto done; }
            free(tmp); free(blk);
        }

        if (wail_dur > 0.0) {
            float *blk = NULL; size_t bframes = 0;
            if (synth_block(&blk, &bframes, freq_wail, &wctx, wail_dur, SR) != 0) goto done;
            apply_fade_in_out(blk, bframes, SR, FADE_MS);
            int16_t *tmp = malloc(sizeof(int16_t) * bframes * CHANNELS);
            if (!tmp) { free(blk); goto done; }
            float_mono_to_interleaved_s16(blk, bframes, tmp, CHANNELS);
            if (write_full_chunked(fd, tmp, bframes, CHANNELS, (size_t)(period ? period : 1024)) != 0) { free(tmp); free(blk); goto done; }
            free(tmp); free(blk);
        }
    }

done:
    if (fd >= 0) {
        ioctl(fd, SNDRV_PCM_IOCTL_DRAIN, 0);
        close(fd);
    }
    fprintf(stderr, "Finished\n");
    return 0;
}


