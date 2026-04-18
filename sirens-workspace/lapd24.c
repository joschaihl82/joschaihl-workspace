/*
 lapd_tinyalsa.c
 Single-file synth + tiny "libasound" shim that plays via aplay pipe.
 No libasound required; tiny API: snd_pcm_open/prepare/writei/drain/close.
 Defaults: wail 10.0 -> 7.5 sec, yelp 0.0 -> 2.5 sec, gain fixed 1.0.
 Compile: cc -O2 -std=c11 -lm -o lapd_tinyalsa lapd_tinyalsa.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SR 44100
#define ITERATIONS 8
#define FADE_MS 4
#define PLAY_CHUNK_SEC 10.0

typedef struct { double f0,f1,rate; } ctx_siren;

/* ---------------- tiny libasound-like shim ----------------
   This is NOT real ALSA. It spawns aplay and feeds raw S16_LE interleaved stereo.
   Functions:
     typedef struct snd_pcm_s snd_pcm_t;
     int snd_pcm_open(snd_pcm_t **p, const char *device_cmd);
     int snd_pcm_prepare(snd_pcm_t *p);
     snd_pcm_sframes_t snd_pcm_writei(snd_pcm_t *p, const void *buf, size_t frames);
     int snd_pcm_drain(snd_pcm_t *p);
     void snd_pcm_close(snd_pcm_t *p);
*/
typedef struct snd_pcm_s {
    FILE *pipe;       /* stdin of child player */
    pid_t  pid;       /* child pid */
    unsigned rate;
    unsigned channels;
} snd_pcm_s;
typedef snd_pcm_s snd_pcm_t;
typedef long snd_pcm_sframes_t;

static const char *DEVICE_CMD = "aplay -f S16_LE -c 2 -r 44100 -t raw -D default";

/* spawn aplay (or other command) and open pipe for writing raw PCM */
static int snd_pcm_open_shim(snd_pcm_t **out, const char *device_cmd, unsigned rate, unsigned channels) {
    if (!out || !device_cmd) return -1;
    /* build command: device_cmd already includes args; use popen for simplicity */
    FILE *p = popen(device_cmd, "w");
    if (!p) {
        fprintf(stderr, "snd_pcm_open_shim: popen(%s) failed: %s\n", device_cmd, strerror(errno));
        return -1;
    }
    /* popen does not give pid portably; leave pid = -1 */
    snd_pcm_t *pcm = calloc(1, sizeof(*pcm));
    if (!pcm) { pclose(p); return -1; }
    pcm->pipe = p;
    pcm->pid = -1;
    pcm->rate = rate;
    pcm->channels = channels;
    *out = pcm;
    fprintf(stderr, "Opened tiny PCM via: %s\n", device_cmd);
    return 0;
}
static int snd_pcm_prepare_shim(snd_pcm_t *p) {
    (void)p; /* no-op for pipe */
    return 0;
}
/* write interleaved stereo s16 frames to the player */
static snd_pcm_sframes_t snd_pcm_writei_shim(snd_pcm_t *p, const void *buf, size_t frames) {
    if (!p || !p->pipe || !buf) return -1;
    size_t samples = frames * p->channels;
    size_t bytes = samples * sizeof(int16_t);
    size_t written = fwrite(buf, 1, bytes, p->pipe);
    if (written != bytes) {
        fprintf(stderr, "snd_pcm_writei_shim: short write %zu != %zu (errno=%d)\n", written, bytes, errno);
        return -1;
    }
    return (snd_pcm_sframes_t)frames;
}
/* flush and close the write pipe, wait for player process to exit if possible */
static int snd_pcm_drain_shim(snd_pcm_t *p) {
    if (!p) return -1;
    if (p->pipe) {
        fflush(p->pipe);
        /* pclose waits for child; use pclose to obtain status */
        int rc = pclose(p->pipe);
        p->pipe = NULL;
        if (rc == -1) {
            fprintf(stderr, "snd_pcm_drain_shim: pclose failed: %s\n", strerror(errno));
            return -1;
        }
        if (WIFEXITED(rc) && WEXITSTATUS(rc) != 0) {
            fprintf(stderr, "player exited with status %d\n", WEXITSTATUS(rc));
        }
    }
    return 0;
}
static void snd_pcm_close_shim(snd_pcm_t *p) {
    if (!p) return;
    if (p->pipe) {
        pclose(p->pipe);
        p->pipe = NULL;
    }
    free(p);
}
/* ---------------- end shim ---------------- */

/* triangle LFO and triangular sweep */
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

/* synth mono sinusoid with time-varying frequency */
static int synth_block(double (*freq_fn)(double, void*), void *ctx, double dur, int sr, float **outbuf, size_t *out_frames) {
    if (dur <= 0.0 || sr <= 0) { *outbuf = NULL; *out_frames = 0; return -1; }
    size_t frames = (size_t)ceil(dur * (double)sr);
    float *buf = calloc(frames, sizeof(float));
    if (!buf) { fprintf(stderr,"OOM synth\n"); *outbuf = NULL; *out_frames = 0; return -1; }
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

/* fade in/out */
static void apply_fade_in_out(float *buf, size_t frames, int sr, unsigned fade_ms) {
    if (!buf || frames == 0 || fade_ms == 0 || sr <= 0) return;
    size_t fade_samples = (size_t)((double)sr * (double)fade_ms / 1000.0);
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

/* frequency wrappers */
static double freq_wail(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }
static double freq_yelp(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }

/* synth into accumulation buffer helpers */
static int synth_into(float **acc_buf, size_t *acc_frames, double (*freq_fn)(double, void*), void *ctx, double dur) {
    float *blk = NULL; size_t blk_frames = 0;
    if (synth_block(freq_fn, ctx, dur, SR, &blk, &blk_frames) != 0) return -1;
    apply_fade_in_out(blk, blk_frames, SR, FADE_MS);
    float *nb = realloc(*acc_buf, (*acc_frames + blk_frames) * sizeof(float));
    if (!nb) { free(blk); return -1; }
    memcpy(nb + *acc_frames, blk, blk_frames * sizeof(float));
    *acc_buf = nb;
    *acc_frames += blk_frames;
    free(blk);
    return 0;
}

/* convert N mono float frames to interleaved s16 stereo in temp buffer and write via shim */
static int write_frames_via_shim(snd_pcm_t *pcm, const float *mono, size_t frames) {
    if (!pcm || !mono) return -1;
    const size_t BLOCK = 4096;
    int16_t outbuf[BLOCK * 2];
    size_t written = 0;
    while (written < frames) {
        size_t n = frames - written;
        if (n > BLOCK) n = BLOCK;
        for (size_t i = 0; i < n; ++i) {
            double s = mono[written + i];
            if (s > 1.0) s = 1.0; else if (s < -1.0) s = -1.0;
            int16_t v = (int16_t)lrint(s * 32767.0);
            outbuf[2*i] = v;
            outbuf[2*i+1] = v;
        }
        if (snd_pcm_writei_shim(pcm, outbuf, n) < 0) return -1;
        written += n;
    }
    return 0;
}

static double lerp(double a, double b, double t) { return a + (b - a) * t; }

int main(void) {
    const double wail_start = 10.0, wail_end = 7.5;
    const double yelp_start = 0.0,  yelp_end  = 2.5;
    const size_t play_chunk_frames = (size_t)ceil(PLAY_CHUNK_SEC * SR);

    snd_pcm_t *pcm = NULL;
    if (snd_pcm_open_shim(&pcm, DEVICE_CMD, SR, 2) != 0) {
        fprintf(stderr, "Failed to open player pipe\n");
        return 1;
    }
    snd_pcm_prepare_shim(pcm);

    ctx_siren wctx = {880.0, 880.0*2.0, 12.0/60.0};
    ctx_siren yctx = {880.0, 880.0*2.0, 180.0/60.0};

    float *acc_buf = NULL;
    size_t acc_frames = 0;

    for (int i = 0; i < ITERATIONS; ++i) {
        double t = (ITERATIONS == 1) ? 0.0 : (double)i / (double)(ITERATIONS - 1);
        double wail_dur = lerp(wail_start, wail_end, t);
        double yelp_dur = lerp(yelp_start, yelp_end, t);

        if (yelp_dur > 0.0) {
            if (synth_into(&acc_buf, &acc_frames, freq_yelp, &yctx, yelp_dur) != 0) break;
        }
        if (wail_dur > 0.0) {
            if (synth_into(&acc_buf, &acc_frames, freq_wail, &wctx, wail_dur) != 0) break;
        }

        while (acc_frames >= play_chunk_frames) {
            if (write_frames_via_shim(pcm, acc_buf, play_chunk_frames) != 0) goto out;
            size_t remain = acc_frames - play_chunk_frames;
            if (remain > 0) memmove(acc_buf, acc_buf + play_chunk_frames, remain * sizeof(float));
            acc_frames = remain;
            float *nb = realloc(acc_buf, acc_frames * sizeof(float));
            if (nb || acc_frames == 0) acc_buf = nb;
        }
    }

    if (acc_frames > 0) {
        if (write_frames_via_shim(pcm, acc_buf, acc_frames) != 0) goto out;
    }

out:
    free(acc_buf);
    snd_pcm_drain_shim(pcm);
    snd_pcm_close_shim(pcm);
    fprintf(stderr, "Done: %d cycles (yelp first) and %d ms fades\n", ITERATIONS, FADE_MS);
    return 0;
}

