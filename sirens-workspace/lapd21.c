/* play_ss2000_grow.c
   Play yelp first, then wail. Gain increases linearly each cycle.
   Compile: cc -O2 -std=c11 -lasound -lm -o play_ss2000_grow play_ss2000_grow.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <alsa/asoundlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Settings */
#define SR 44100
#define ITERATIONS 8
#define WAIL_DUR 7.5
#define YELP_DUR 2.5
#define FADE_MS 4
#define GAIN_MIN 0.25
#define GAIN_MAX 1.00

typedef struct { double f0,f1,rate; } ctx_siren;

/* ---------------- LFO / sweep / synth ---------------- */
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
static void synth_block(double (*freq_fn)(double, void*), void *ctx, double dur, int sr, double **outbuf, size_t *out_frames) {
    size_t frames = (size_t)(dur * sr);
    double *buf = malloc(sizeof(double) * frames);
    if (!buf) { fprintf(stderr,"OOM synth\n"); *outbuf = NULL; *out_frames = 0; return; }
    double phase = 0.0, t = 0.0, dt = 1.0 / (double)sr;
    for (size_t i = 0; i < frames; ++i) {
        double f = freq_fn(t, ctx);
        double incr = 2.0 * M_PI * f * dt;
        phase += incr;
        while (phase >= 2.0*M_PI) phase -= 2.0*M_PI;
        if (phase < 0.0) phase += 2.0*M_PI;
        buf[i] = sin(phase) * 0.85;
        t += dt;
    }
    *outbuf = buf; *out_frames = frames;
}

/* ---------------- fades ---------------- */
static void apply_fade_in_out(double *buf, size_t frames, int sr, unsigned fade_ms) {
    if (!buf || frames == 0) return;
    size_t fade_samples = (size_t)((double)sr * (double)fade_ms / 1000.0);
    if (fade_samples == 0) return;
    if (fade_samples * 2 > frames) fade_samples = frames / 2;
    for (size_t i = 0; i < fade_samples; ++i) {
        double g = (double)(i + 1) / (double)fade_samples;
        buf[i] *= g;
    }
    for (size_t i = 0; i < fade_samples; ++i) {
        double g = (double)(fade_samples - i) / (double)fade_samples;
        buf[frames - fade_samples + i] *= g;
    }
}

/* ---------------- ALSA helpers ---------------- */
static int setup_snd(snd_pcm_t **pcm, unsigned sr) {
    snd_pcm_t *p = NULL;
    snd_pcm_hw_params_t *hw;
    int rc = snd_pcm_open(&p, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) { fprintf(stderr,"snd_pcm_open: %s\n", snd_strerror(rc)); return rc; }
    snd_pcm_hw_params_alloca(&hw);
    snd_pcm_hw_params_any(p, hw);
    snd_pcm_hw_params_set_access(p, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(p, hw, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(p, hw, 2);
    unsigned int sr_set = sr;
    int dir = 0;
    snd_pcm_hw_params_set_rate_near(p, hw, &sr_set, &dir);
    if (sr_set != (unsigned)sr) {
        fprintf(stderr, "Warning: ALSA returned sample rate %u (requested %u)\n", sr_set, sr);
    }
    snd_pcm_hw_params_set_period_size(p, hw, 1024, 0);
    rc = snd_pcm_hw_params(p, hw);
    if (rc < 0) { fprintf(stderr,"snd_pcm_hw_params: %s\n", snd_strerror(rc)); snd_pcm_close(p); return rc; }
    *pcm = p;
    return 0;
}
static void close_snd(snd_pcm_t *pcm) {
    if (pcm) { snd_pcm_drain(pcm); snd_pcm_close(pcm); }
}
static int alsa_write_frames(snd_pcm_t *pcm, const double *mono, size_t frames) {
    const size_t BLOCK = 4096;
    int16_t buf[BLOCK * 2];
    size_t written = 0;
    while (written < frames) {
        size_t n = frames - written;
        if (n > BLOCK) n = BLOCK;
        for (size_t i = 0; i < n; ++i) {
            double s = mono[written + i];
            if (s > 1.0) s = 1.0; if (s < -1.0) s = -1.0;
            int16_t v = (int16_t)lrint(s * 32767.0);
            buf[2*i] = v; buf[2*i+1] = v;
        }
        snd_pcm_sframes_t r = snd_pcm_writei(pcm, buf, n);
        if (r == -EPIPE) { snd_pcm_prepare(pcm); continue; }
        if (r == -ESTRPIPE) { snd_pcm_recover(pcm, (int)r, 1); continue; }
        if (r < 0) {
            int err = snd_pcm_recover(pcm, (int)r, 1);
            if (err < 0) { fprintf(stderr,"snd_pcm_writei: %s\n", snd_strerror((int)r)); return -1; }
            continue;
        }
        written += (size_t)r;
    }
    return 0;
}

/* ---------------- frequency wrappers ---------------- */
static double freq_wail(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }
static double freq_yelp(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }

/* ---------------- semantic functions (gain parameter) ---------------- */
static int wail(snd_pcm_t *pcm, ctx_siren *ctx, unsigned fade_ms, double gain) {
    double *buf = NULL; size_t frames = 0;
    synth_block(freq_wail, ctx, WAIL_DUR, SR, &buf, &frames);
    if (!buf) return -1;
    /* apply per-iteration gain */
    for (size_t i = 0; i < frames; ++i) buf[i] *= gain;
    apply_fade_in_out(buf, frames, SR, fade_ms);
    int rc = alsa_write_frames(pcm, buf, frames);
    free(buf);
    return rc;
}
static int yelp(snd_pcm_t *pcm, ctx_siren *ctx, unsigned fade_ms, double gain) {
    double *buf = NULL; size_t frames = 0;
    synth_block(freq_yelp, ctx, YELP_DUR, SR, &buf, &frames);
    if (!buf) return -1;
    for (size_t i = 0; i < frames; ++i) buf[i] *= gain;
    apply_fade_in_out(buf, frames, SR, fade_ms);
    int rc = alsa_write_frames(pcm, buf, frames);
    free(buf);
    return rc;
}

/* ---------------- main (yelp first, gain grows) ---------------- */
int main(void) {
    snd_pcm_t *pcm = NULL;
    if (setup_snd(&pcm, SR) != 0) return 1;

    ctx_siren wctx = {880.0, 880.0*2.0, 12.0/60.0};   /* slow wail */
    ctx_siren yctx = {880.0, 880.0*2.0, 180.0/60.0};  /* fast yelp */

    for (int i = 0; i < ITERATIONS; ++i) {
        double t = (ITERATIONS <= 1) ? 0.0 : (double)i / (double)(ITERATIONS - 1);
        /* play yelp first, then wail */
        if (yelp(pcm, &yctx, FADE_MS, 1.0) != 0) break;
        if (wail(pcm, &wctx, FADE_MS, 1.0) != 0) break;
    }

    close_snd(pcm);
    printf("Done: %d cycles (yelp first) with linear gain %.2f->%.2f and %d ms fades\n", ITERATIONS, GAIN_MIN, GAIN_MAX, FADE_MS);
    return 0;
}

