/*
 lapd.c
 Synthesize yelp then wail into a floating accumulation buffer.
 Whenever the buffer holds >= 10 seconds of audio it is played to ALSA.
 At the end any remaining audio is flushed to the device.
 No command-line args. Uses "default" ALSA device with fallback.
 Defaults: wail 10.0 -> 7.5 sec, yelp 0.0 -> 2.5 sec, gain fixed 1.0.
 Compile: cc -O2 -std=c11 -lasound -lm -o lapd lapd.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <alsa/asoundlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SR 44100
#define ITERATIONS 8
#define FADE_MS 4
#define PLAY_CHUNK_SEC 10.0

typedef struct { double f0,f1,rate; } ctx_siren;

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

/* ALSA setup/teardown: prefer "default", then "plughw:0,0" fallback */
static int setup_snd(snd_pcm_t **pcm, unsigned sr) {
    snd_pcm_t *p = NULL;
    snd_pcm_hw_params_t *hw;
    const char *dev_primary = "default";
    int rc = snd_pcm_open(&p, dev_primary, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "snd_pcm_open(%s) failed: %s — trying \"plughw:0,0\"\n", dev_primary, snd_strerror(rc));
        rc = snd_pcm_open(&p, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0);
        if (rc < 0) {
            fprintf(stderr, "snd_pcm_open(plughw:0,0) failed: %s\n", snd_strerror(rc));
            return rc;
        } else {
            fprintf(stderr, "Opened ALSA device: plughw:0,0\n");
        }
    } else {
        fprintf(stderr, "Opened ALSA device: %s\n", dev_primary);
    }

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
    rc = snd_pcm_prepare(p);
    if (rc < 0) { fprintf(stderr,"snd_pcm_prepare: %s\n", snd_strerror(rc)); snd_pcm_close(p); return rc; }
    *pcm = p;
    return 0;
}
static void close_snd(snd_pcm_t *pcm) { if (pcm) { snd_pcm_drain(pcm); snd_pcm_close(pcm); } }

/* write mono float buffer to ALSA as interleaved stereo s16 */
static int alsa_write_frames(snd_pcm_t *pcm, const float *mono, size_t frames) {
    if (!pcm || !mono) return -1;
    const size_t BLOCK = 4096;
    int16_t interleaved[BLOCK * 2];
    size_t written = 0;
    while (written < frames) {
        size_t n = frames - written;
        if (n > BLOCK) n = BLOCK;
        for (size_t i = 0; i < n; ++i) {
            double s = mono[written + i];
            if (s > 1.0) s = 1.0; else if (s < -1.0) s = -1.0;
            int16_t v = (int16_t)lrint(s * 32767.0);
            interleaved[2*i]   = v;
            interleaved[2*i+1] = v;
        }
        snd_pcm_sframes_t r = snd_pcm_writei(pcm, interleaved, (snd_pcm_uframes_t)n);
        if (r == -EPIPE) { fprintf(stderr,"ALSA underrun, preparing\n"); snd_pcm_prepare(pcm); continue; }
        if (r == -ESTRPIPE) { fprintf(stderr,"ALSA suspended, recovering\n"); int rec = snd_pcm_recover(pcm, (int)r, 1); if (rec < 0) { fprintf(stderr,"snd_pcm_recover failed: %s\n", snd_strerror(rec)); return -1; } continue; }
        if (r < 0) { int rec = snd_pcm_recover(pcm, (int)r, 1); if (rec < 0) { fprintf(stderr,"snd_pcm_writei: %s\n", snd_strerror((int)r)); return -1; } continue; }
        written += (size_t)r;
    }
    return 0;
}

/* frequency wrappers */
static double freq_wail(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }
static double freq_yelp(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }

/* players (synthesize into a buffer, gain fixed to 1.0) */
static int synth_yelp_into(float **acc_buf, size_t *acc_frames, ctx_siren *ctx, double dur) {
    float *blk = NULL; size_t blk_frames = 0;
    if (synth_block(freq_yelp, ctx, dur, SR, &blk, &blk_frames) != 0) return -1;
    apply_fade_in_out(blk, blk_frames, SR, FADE_MS);
    float *nb = realloc(*acc_buf, (*acc_frames + blk_frames) * sizeof(float));
    if (!nb) { free(blk); return -1; }
    memcpy(nb + *acc_frames, blk, blk_frames * sizeof(float));
    *acc_buf = nb;
    *acc_frames += blk_frames;
    free(blk);
    return 0;
}
static int synth_wail_into(float **acc_buf, size_t *acc_frames, ctx_siren *ctx, double dur) {
    float *blk = NULL; size_t blk_frames = 0;
    if (synth_block(freq_wail, ctx, dur, SR, &blk, &blk_frames) != 0) return -1;
    apply_fade_in_out(blk, blk_frames, SR, FADE_MS);
    float *nb = realloc(*acc_buf, (*acc_frames + blk_frames) * sizeof(float));
    if (!nb) { free(blk); return -1; }
    memcpy(nb + *acc_frames, blk, blk_frames * sizeof(float));
    *acc_buf = nb;
    *acc_frames += blk_frames;
    free(blk);
    return 0;
}

static double lerp(double a, double b, double t) { return a + (b - a) * t; }

int main(void) {
    const double wail_start = 10.0, wail_end = 7.5;
    const double yelp_start = 0.0,  yelp_end  = 2.5;
    const size_t play_chunk_frames = (size_t)ceil(PLAY_CHUNK_SEC * SR);

    snd_pcm_t *pcm = NULL;
    if (setup_snd(&pcm, SR) != 0) return 1;

    ctx_siren wctx = {880.0, 880.0*2.0, 12.0/60.0};
    ctx_siren yctx = {880.0, 880.0*2.0, 180.0/60.0};

    float *acc_buf = NULL;
    size_t acc_frames = 0;

    for (int i = 0; i < ITERATIONS; ++i) {
        double t = (ITERATIONS == 1) ? 0.0 : (double)i / (double)(ITERATIONS - 1);
        double wail_dur = lerp(wail_start, wail_end, t);
        double yelp_dur = lerp(yelp_start, yelp_end, t);

        /* synth yelp then wail into accumulation buffer */
        if (yelp_dur > 0.0) {
            if (synth_yelp_into(&acc_buf, &acc_frames, &yctx, yelp_dur) != 0) break;
        }
        if (wail_dur > 0.0) {
            if (synth_wail_into(&acc_buf, &acc_frames, &wctx, wail_dur) != 0) break;
        }

        /* when accumulation buffer reaches PLAY_CHUNK_SEC, play it */
        while (acc_frames >= play_chunk_frames) {
            /* play first play_chunk_frames */
            if (alsa_write_frames(pcm, acc_buf, play_chunk_frames) != 0) goto out;
            /* shift remaining frames to buffer start */
            size_t remain = acc_frames - play_chunk_frames;
            if (remain > 0) memmove(acc_buf, acc_buf + play_chunk_frames, remain * sizeof(float));
            acc_frames = remain;
            /* shrink allocation to keep memory reasonable */
            float *nb = realloc(acc_buf, acc_frames * sizeof(float));
            if (nb || acc_frames == 0) acc_buf = nb;
        }
    }

    /* flush remaining audio (may be < 10s) */
    if (acc_frames > 0) {
        if (alsa_write_frames(pcm, acc_buf, acc_frames) != 0) goto out;
    }

out:
    free(acc_buf);
    close_snd(pcm);
    printf("Done: %d cycles (yelp first) and %d ms fades\n", ITERATIONS, FADE_MS);
    return 0;
}

