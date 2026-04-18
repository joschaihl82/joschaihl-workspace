/*
 play_seq.c
 Play wail/yelp with 75% / 25% probability, mono processing, stereo output.
 Usage:
   cc -O2 -std=c11 -lm -o play_seq play_seq.c
   ./play_seq [playback_node]   # e.g. ./play_seq /dev/snd/pcmC0D0p
 If no playback_node given, program auto-selects a /dev/snd/pcm* node (prefers 'p' nodes).
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

/* Minimal WAV loader for 16-bit PCM (stereo or mono) */
typedef struct {
    uint32_t samplerate;
    uint16_t channels;
    uint16_t bits;
    size_t frames;
    int16_t *data; /* interleaved if channels>1 */
} wav_t;

static int read_wav16(const char *path, wav_t *w) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    uint8_t hdr[44];
    if (fread(hdr,1,44,f) != 44) { fclose(f); return -2; }
    /* check "RIFF" and "WAVE" */
    if (memcmp(hdr,"RIFF",4)!=0 || memcmp(hdr+8,"WAVE",4)!=0) { fclose(f); return -3; }
    uint16_t audiofmt = *(uint16_t*)(hdr+20);
    uint16_t nch = *(uint16_t*)(hdr+22);
    uint32_t sr = *(uint32_t*)(hdr+24);
    uint16_t bits = *(uint16_t*)(hdr+34);
    uint32_t datasz = *(uint32_t*)(hdr+40);
    if (audiofmt != 1 || (bits != 16 && bits != 24 && bits != 32 && bits != 8)) {
        fclose(f); return -4;
    }
    size_t frames = datasz / (nch * (bits/8));
    int16_t *buf = malloc(frames * nch * sizeof(int16_t));
    if (!buf) { fclose(f); return -5; }
    if (fread(buf, nch * (bits/8), frames, f) != frames) { free(buf); fclose(f); return -6; }
    fclose(f);
    w->samplerate = sr;
    w->channels = nch;
    w->bits = bits;
    w->frames = frames;
    w->data = buf;
    return 0;
}

/* convert interleaved to mono (averaging channels), result is newly allocated array of int16_t */
static int16_t *to_mono(const wav_t *w) {
    if (!w || !w->data) return NULL;
    int16_t *mono = malloc(w->frames * sizeof(int16_t));
    if (!mono) return NULL;
    if (w->channels == 1) {
        memcpy(mono, w->data, w->frames * sizeof(int16_t));
        return mono;
    }
    for (size_t i=0;i<w->frames;i++) {
        int32_t sum = 0;
        for (int c=0;c<w->channels;c++) {
            sum += w->data[i * w->channels + c];
        }
        sum /= (int)w->channels;
        if (sum > INT16_MAX) sum = INT16_MAX;
        if (sum < INT16_MIN) sum = INT16_MIN;
        mono[i] = (int16_t)sum;
    }
    return mono;
}

/* find playback node under /dev/snd (prefer nodes ending with 'p') */
static int find_playback_node(char *out, size_t len) {
    DIR *d = opendir("/dev/snd");
    if (!d) return -1;
    struct dirent *e;
    char fallback[MAX_PATH] = {0};
    while ((e = readdir(d)) != NULL) {
        if (strncmp(e->d_name, "pcm", 3) != 0) continue;
        size_t L = strlen(e->d_name);
        if (L > 0 && (e->d_name[L-1] == 'p' || e->d_name[L-1] == 'P')) {
            snprintf(out, len, "/dev/snd/%s", e->d_name);
            closedir(d);
            return 0;
        }
        if (fallback[0] == '\0') snprintf(fallback, sizeof(fallback), "/dev/snd/%s", e->d_name);
    }
    closedir(d);
    if (fallback[0] == '\0') return -2;
    strncpy(out, fallback, len-1);
    out[len-1] = '\0';
    return 0;
}

/* write stereo interleaved 16-bit buffer to fd (blocking) */
static int write_all(int fd, const void *buf, size_t bytes) {
    const uint8_t *p = buf;
    size_t wrote = 0;
    while (wrote < bytes) {
        ssize_t r = write(fd, p + wrote, bytes - wrote);
        if (r > 0) { wrote += (size_t)r; continue; }
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { struct timespec ts = {0,2000000}; nanosleep(&ts,NULL); continue; }
            return -errno;
        }
    }
    return 0;
}

/* crossfade: mix tail of prev and head of next (mono int16 arrays), length in samples */
static void crossfade_mono(int16_t *out, const int16_t *prev, size_t prev_len,
                           const int16_t *next, size_t next_len, size_t fade_samples)
{
    /* out must have at least fade_samples samples; prev points to last fade_samples of previous,
       next points to first fade_samples of next. We write into out[0..fade_samples-1] */
    for (size_t i=0;i<fade_samples;i++) {
        double a = (double)(fade_samples - i) / (double)fade_samples; /* prev weight */
        double b = (double)i / (double)fade_samples;                 /* next weight */
        double s = prev[prev_len - fade_samples + i] * a + next[i] * b;
        if (s > 32767.0) s = 32767.0;
        if (s < -32768.0) s = -32768.0;
        out[i] = (int16_t) (s);
    }
}

/* duplicate mono -> stereo interleaved into dst (dst must be frames*2 length) */
static void mono_to_stereo_interleaved(const int16_t *mono, int16_t *dst, size_t frames) {
    for (size_t i=0;i<frames;i++) { dst[2*i] = mono[i]; dst[2*i+1] = mono[i]; }
}

/* main sequence player */
int main(int argc, char **argv) {
    const char *wail_file = "wail.wav";
    const char *yelp_file = "yelp.wav";
    const char *playdev = NULL;
    if (argc >= 2) playdev = argv[1];

    char auto_dev[MAX_PATH] = {0};
    if (!playdev) {
        if (find_playback_node(auto_dev, sizeof(auto_dev)) == 0) playdev = auto_dev;
        else {
            fprintf(stderr, "No playback node found under /dev/snd. Provide device path as argument.\n");
            return 1;
        }
    }

    /* open playback node */
    int fd = open(playdev, O_WRONLY | O_NONBLOCK);
    if (fd < 0) { perror("open playback node"); return 1; }
    fprintf(stderr, "Using playback node: %s\n", playdev);

    /* load WAVs */
    wav_t wail = {0}, yelp = {0};
    if (read_wav16(wail_file, &wail) != 0) { fprintf(stderr, "Failed to read %s\n", wail_file); close(fd); return 1; }
    if (read_wav16(yelp_file, &yelp) != 0) { fprintf(stderr, "Failed to read %s\n", yelp_file); close(fd); return 1; }

    /* require same sample rate and 16-bit */
    if (wail.samplerate != yelp.samplerate) { fprintf(stderr, "Sample rates differ\n"); close(fd); return 1; }
    if (wail.bits != 16 || yelp.bits != 16) { fprintf(stderr, "Only 16-bit PCM supported\n"); close(fd); return 1; }
    uint32_t sr = wail.samplerate;

    /* convert to mono */
    int16_t *mono_wail = to_mono(&wail);
    int16_t *mono_yelp = to_mono(&yelp);
    if (!mono_wail || !mono_yelp) { fprintf(stderr, "OOM converting to mono\n"); close(fd); return 1; }

    /* free original interleaved buffers */
    free(wail.data); free(yelp.data);

    /* prepare stereo buffer for writing (we will allocate per-play length) */
    srand((unsigned)time(NULL));

    /* probabilities: wail 75%, yelp 25% */
    const double p_wail = 0.75;
    const double p_yelp = 0.25;

    /* crossfade length: 50 ms */
    size_t fade_samples = (size_t)(0.050 * sr);
    if (fade_samples < 1) fade_samples = 1;

    fprintf(stderr, "Starting sequence: Wail 75%%, Yelp 25%%. Crossfade %.1f ms\n", (double)fade_samples * 1000.0 / sr);

    /* play loop: choose clip, write, then choose next, crossfade between them */
    int16_t *stereo_buf = NULL;
    int16_t *fade_buf = malloc(fade_samples * sizeof(int16_t));
    if (!fade_buf) { fprintf(stderr, "OOM\n"); close(fd); return 1; }

    /* We'll loop indefinitely until interrupted */
    const int16_t *current = NULL;
    size_t current_len = 0;
    const int16_t *next = NULL;
    size_t next_len = 0;

    /* initial pick */
    double r = (double)rand() / RAND_MAX;
    if (r < p_wail) { current = mono_wail; current_len = wail.frames; }
    else { current = mono_yelp; current_len = yelp.frames; }

    while (1) {
        /* pick next clip */
        r = (double)rand() / RAND_MAX;
        if (r < p_wail) { next = mono_wail; next_len = wail.frames; }
        else { next = mono_yelp; next_len = yelp.frames; }

        /* play current except last fade_samples (we will crossfade) */
        size_t play_frames = (current_len > fade_samples) ? (current_len - fade_samples) : current_len;
        /* allocate stereo buffer */
        stereo_buf = malloc(play_frames * 2 * sizeof(int16_t));
        if (!stereo_buf) { fprintf(stderr, "OOM\n"); break; }
        mono_to_stereo_interleaved(current, stereo_buf, play_frames);
        int rc = write_all(fd, stereo_buf, play_frames * 2 * sizeof(int16_t));
        free(stereo_buf);
        if (rc != 0) { fprintf(stderr, "Write error: %s\n", strerror(-rc)); break; }

        /* prepare crossfade between tail of current and head of next */
        if (fade_samples > 0 && current_len >= fade_samples && next_len >= fade_samples) {
            crossfade_mono(fade_buf, current, current_len, next, next_len, fade_samples);
            /* duplicate to stereo and write */
            stereo_buf = malloc(fade_samples * 2 * sizeof(int16_t));
            if (!stereo_buf) { fprintf(stderr, "OOM\n"); break; }
            mono_to_stereo_interleaved(fade_buf, stereo_buf, fade_samples);
            rc = write_all(fd, stereo_buf, fade_samples * 2 * sizeof(int16_t));
            free(stereo_buf);
            if (rc != 0) { fprintf(stderr, "Write error: %s\n", strerror(-rc)); break; }
        } else {
            /* if no fade possible, just write next head silence or small gap (optional) */
        }

        /* advance: next becomes current */
        current = next;
        current_len = next_len;

        /* small sleep to allow device to catch up if needed (not strictly necessary) */
        struct timespec ts = {0, 1000000}; /* 1 ms */
        nanosleep(&ts, NULL);
    }

    free(fade_buf);
    free(mono_wail);
    free(mono_yelp);
    close(fd);
    return 0;
}

