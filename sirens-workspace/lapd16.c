// lapd_wavetable_ss2000.c
// Wavetable-based SS2000-like wail/yelp player (ALSA).
// - Bandlimited wavetable timbre approximates SS2000 wail (harmonics + detune + light LP).
// - Notes: only high A-minor notes (A5..A6 etc.), no notes 110..392 Hz.
// - Global multiplier ramps 1.0 -> 2.0 across TOTAL_BLOCKS (880 -> 1760 Hz).
// - Blocks: 75% wail-block (4 cycles, per-cycle amplitude fades 1.0->0.0), 25% yelp-block (4 cycles full amp).
// - First block: force no yelp.
// - Durations via cycles and rates: WAIL_RATE=0.25Hz, WAIL_CYCLES=1.875 -> 7.5s ; YELP_RATE=0.5Hz, YELP_CYCLES=1.25 -> 2.5s
// - No crossfades, no diagnostic tones.
// Compile with: cc -O2 -std=c11 -lm -lasound -o lapd_wavetable_ss2000 lapd_wavetable_ss2000.c

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <alsa/asoundlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Audio */
#define SR 44100
#define CHANNELS 2

/* Block and behavior */
#define BLOCK_CYCLES 4
#define WAIL_BLOCK_PROB 0.75
#define TOTAL_BLOCKS 32   /* ramp span 1.0 -> 2.0 across these blocks */

/* LFO rates and cycles (cycle-based durations) */
#define WAIL_RATE 0.25
#define YELP_RATE 0.50
#define WAIL_CYCLES 1.875
#define YELP_CYCLES 1.25

/* Wail/yelp structure */
#define YELP_ALTERNATIONS 20

/* Wavetable parameters */
#define WT_SIZE 16384           /* large table for quality */
#define WT_HARMONICS 12        /* number of harmonics to include */
#define DETUNE_CENTS 4.0       /* small detune on upper partials */
#define LP_ALPHA 0.995         /* simple one-pole lowpass when reading (soften) */

/* A-minor high notes only (A5..A6 range and nearby) */
static const double A_MINOR_HIGH[] = {
    880.000,   /* A5 */
    987.767,   /* B5 */
    1046.502,  /* C6 */
    1174.659,  /* D6 */
    1318.510,  /* E6 */
    1396.913,  /* F6 */
    1567.982,  /* G6 */
    1760.000   /* A6 */
};
static const size_t A_MINOR_HIGH_COUNT = sizeof(A_MINOR_HIGH)/sizeof(A_MINOR_HIGH[0]);

/* Yelp base notes (C6, E6) indices inside A_MINOR_HIGH */
#define YELP_IDX_C6 2
#define YELP_IDX_E6 4

/* Control */
static volatile sig_atomic_t keep_running = 1;
static void handle_sigint(int _) { (void)_; keep_running = 0; }

/* Wavetable */
static float *wavetable = NULL;
static size_t wt_size = WT_SIZE;

/* Simple ALSA open helper */
static int open_alsa(snd_pcm_t **handle, unsigned int sr, int channels) {
    int err;
    snd_pcm_t *pcm;
    if ((err = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(err));
        return err;
    }
    if ((err = snd_pcm_set_params(pcm,
                                  SND_PCM_FORMAT_S16_LE,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  channels,
                                  sr,
                                  1,
                                  500000)) < 0) {
        fprintf(stderr, "snd_pcm_set_params: %s\n", snd_strerror(err));
        snd_pcm_close(pcm);
        return err;
    }
    *handle = pcm;
    return 0;
}

/* Bandlimited wavetable construction:
   Sum harmonics with amplitude envelope (1/harmonic), small detune for upper partials,
   and store single-cycle table. */
static int build_wavetable(void) {
    wavetable = malloc(sizeof(float) * wt_size);
    if (!wavetable) return -1;
    for (size_t i = 0; i < wt_size; ++i) wavetable[i] = 0.0f;
    /* fundamental normalized to 1.0 base amplitude; harmonic envelope roll-off */
    for (int h = 1; h <= WT_HARMONICS; ++h) {
        double amp = 1.0 / (double)h; /* simple harmonic envelope */
        /* slight additional roll-off for high partials */
        amp *= 1.0 / (1.0 + 0.12 * (h - 1));
        /* detune upper partials by a few cents to create 'buzzy' texture */
        double det = 0.0;
        if (h > 3) det = pow(2.0, ( (h - 3) * DETUNE_CENTS / 1200.0)); /* multiplicative detune factor */
        for (size_t i = 0; i < wt_size; ++i) {
            double phase = (double)i / (double)wt_size;
            double x = 2.0 * M_PI * h * phase * det;
            wavetable[i] += (float)(amp * sin(x));
        }
    }
    /* normalize */
    double maxv = 1e-12;
    for (size_t i = 0; i < wt_size; ++i) {
        if (fabs(wavetable[i]) > maxv) maxv = fabs(wavetable[i]);
    }
    if (maxv > 0.0) {
        double s = 0.95 / maxv;
        for (size_t i = 0; i < wt_size; ++i) wavetable[i] = (float)(wavetable[i] * s);
    }
    return 0;
}

/* Read wavetable with linear interpolation and simple one-pole lowpass smoothing */
static double wt_read(double phase) {
    /* phase 0..1 */
    double idx = phase * (double)wt_size;
    long i0 = (long)floor(idx) % (long)wt_size;
    long i1 = (i0 + 1) % (long)wt_size;
    double frac = idx - floor(idx);
    double v0 = wavetable[i0];
    double v1 = wavetable[i1];
    double val = v0 + (v1 - v0) * frac;
    /* simple softening */
    static double prev = 0.0;
    prev = LP_ALPHA * prev + (1.0 - LP_ALPHA) * val;
    return prev;
}

/* Synthesize a mono buffer of given frequency (fixed tone using wavetable) */
static int16_t *synth_note_from_wt(double freq, double dur, double amplitude, size_t *out_frames) {
    size_t frames = (size_t)ceil(dur * SR);
    int16_t *buf = malloc(sizeof(int16_t) * frames);
    if (!buf) return NULL;
    double phase = 0.0;
    double phase_inc = freq / (double)SR; /* phase increment per sample (0..1) */
    for (size_t i = 0; i < frames; ++i) {
        double s = wt_read(phase) * amplitude * 0.85;
        if (s > 1.0) s = 1.0;
        if (s < -1.0) s = -1.0;
        buf[i] = (int16_t)lrint(s * 32767.0);
        phase += phase_inc;
        phase -= floor(phase);
    }
    *out_frames = frames;
    return buf;
}

/* Convert mono int16 to interleaved stereo int16 into dst (dst must have frames*2 space) */
static void mono_to_stereo_int16(const int16_t *mono, int16_t *dst, size_t frames) {
    for (size_t i = 0; i < frames; ++i) {
        dst[2*i] = mono[i];
        dst[2*i+1] = mono[i];
    }
}

/* ALSA write helper (handles underrun) */
static int alsa_write_frames(snd_pcm_t *pcm, const int16_t *buf, snd_pcm_uframes_t frames) {
    snd_pcm_sframes_t r = snd_pcm_writei(pcm, buf, frames);
    if (r == -EPIPE) {
        snd_pcm_prepare(pcm);
        r = snd_pcm_writei(pcm, buf, frames);
    }
    if (r < 0) {
        fprintf(stderr, "snd_pcm_writei error: %s\n", snd_strerror((int)r));
        return (int)r;
    }
    return 0;
}

int main(void) {
    signal(SIGINT, handle_sigint);

    if (build_wavetable() != 0) {
        fprintf(stderr, "Wavetable build failed\n");
        return 1;
    }

    /* compute per-note durations for wail (split cycle across notes count) */
    const size_t notes_per_wail = A_MINOR_HIGH_COUNT;
    double w_note_dur = (WAIL_CYCLES / WAIL_RATE) / (double)notes_per_wail; /* seconds per note */
    double y_sub_dur = (YELP_CYCLES / YELP_RATE) / (double)YELP_ALTERNATIONS;

    /* open ALSA */
    snd_pcm_t *pcm = NULL;
    if (open_alsa(&pcm, SR, CHANNELS) < 0) {
        free(wavetable);
        return 1;
    }

    /* stereo buffer large enough for longest chunk */
    size_t max_frames = (size_t)ceil(w_note_dur * SR);
    size_t y_sub_frames = (size_t)ceil(y_sub_dur * SR);
    if (y_sub_frames > max_frames) max_frames = y_sub_frames;
    int16_t *stereo_buf = malloc(sizeof(int16_t) * max_frames * 2);
    if (!stereo_buf) { snd_pcm_close(pcm); free(wavetable); return 1; }

    srand((unsigned)time(NULL));

    /* main block loop: ramp multiplier from 1.0 -> 2.0 across TOTAL_BLOCKS */
    for (int block = 0; block < TOTAL_BLOCKS && keep_running; ++block) {
        double tblock = (TOTAL_BLOCKS <= 1) ? 1.0 : (double)block / (double)(TOTAL_BLOCKS - 1);
        double multiplier = 1.0 + tblock * 1.0; /* 1.0 -> 2.0 */

        /* choose block type; first block must not play yelp at all (force wail) */
        double r = (double)rand() / (double)RAND_MAX;
        int choose_wail_block = (block == 0) ? 1 : (r < WAIL_BLOCK_PROB);

        if (choose_wail_block) {
            /* play BLOCK_CYCLES of wail cycles; per-cycle amplitude fades 1.0 -> 0.0 */
            for (int cyc = 0; cyc < BLOCK_CYCLES && keep_running; ++cyc) {
                double amp_cycle = 1.0;
                if (BLOCK_CYCLES > 1) amp_cycle = 1.0 - ((double)cyc / (double)(BLOCK_CYCLES - 1));
                for (size_t n = 0; n < notes_per_wail && keep_running; ++n) {
                    double freq = A_MINOR_HIGH[n] * multiplier;
                    size_t frames = 0;
                    int16_t *mono = synth_note_from_wt(freq, w_note_dur, amp_cycle, &frames);
                    if (!mono) { keep_running = 0; break; }
                    mono_to_stereo_int16(mono, stereo_buf, frames);
                    free(mono);
                    if (alsa_write_frames(pcm, stereo_buf, (snd_pcm_uframes_t)frames) != 0) { keep_running = 0; break; }
                }
            }
        } else {
            /* play BLOCK_CYCLES of yelp cycles; each cycle alternates YELP_ALTERNATIONS times between two notes */
            for (int cyc = 0; cyc < BLOCK_CYCLES && keep_running; ++cyc) {
                for (int a = 0; a < YELP_ALTERNATIONS && keep_running; ++a) {
                    int base_idx = (a & 1) ? YELP_IDX_E6 : YELP_IDX_C6;
                    int idx = base_idx;
                    /* bias toward higher indices as multiplier ramps (simple bias) */
                    double bias = tblock * (double)(A_MINOR_HIGH_COUNT - 1);
                    idx = (int)round((double)idx + bias);
                    if (idx < 0) idx = 0;
                    if (idx >= (int)A_MINOR_HIGH_COUNT) idx = (int)A_MINOR_HIGH_COUNT - 1;
                    double freq = A_MINOR_HIGH[idx] * multiplier;
                    size_t frames = 0;
                    int16_t *mono = synth_note_from_wt(freq, y_sub_dur, 1.0, &frames);
                    if (!mono) { keep_running = 0; break; }
                    mono_to_stereo_int16(mono, stereo_buf, frames);
                    free(mono);
                    if (alsa_write_frames(pcm, stereo_buf, (snd_pcm_uframes_t)frames) != 0) { keep_running = 0; break; }
                }
            }
        }
    }

    /* optional final closing tone at final multiplier (A6 = 1760) */
    if (keep_running) {
        double final_freq = 880.0 * 2.0;
        double close_dur = 1.0;
        size_t frames = 0;
        int16_t *mono = synth_note_from_wt(final_freq, close_dur, 1.0, &frames);
        if (mono) {
            mono_to_stereo_int16(mono, stereo_buf, frames);
            alsa_write_frames(pcm, stereo_buf, (snd_pcm_uframes_t)frames);
            free(mono);
        }
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(stereo_buf);
    free(wavetable);
    return 0;
}


