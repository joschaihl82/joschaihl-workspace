// lapd_high_amajor.c
// ALSA player using only A-minor notes at A5 and above (no 110..392 Hz).
// - Notes used: A5, B5, C6, D6, E6, F6, G6, A6 (880 -> 1760 Hz range).
// - Blocks: 75% wail-block (4 cycles, amplitude fades 1.0->0.0), 25% yelp-block (4 cycles full amp).
// - Ramp: across TOTAL_BLOCKS the selection index shifts from lower notes toward higher notes,
//   so playback begins around A5 and ends around A6.
// - No sweeps, no diagnostic tones, no crossfades.
// Compile: cc -O2 -std=c11 -lm -lasound -o lapd_high_amajor lapd_high_amajor.c

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

/* Audio and block config */
#define SR 44100
#define CHANNELS 2
#define BLOCK_CYCLES 4
#define WAIL_BLOCK_PROB 0.75
#define TOTAL_BLOCKS 32   /* ramp span from low to high notes across blocks */

/* Cycle durations (seconds) */
#define WAIL_CYCLE_SEC 7.5
#define YELP_CYCLE_SEC 2.5

/* A-minor notes restricted to A5 and above (no 110..392 Hz) */
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

/* Yelp notes chosen from high A-minor (C6 and E6) */
#define YELP_NOTE1_BASE 1046.502  /* C6 */
#define YELP_NOTE2_BASE 1318.510  /* E6 */

/* Control */
static volatile sig_atomic_t keep_running = 1;
static void handle_sigint(int _) { (void)_; keep_running = 0; }

/* Synthesize a pure sine tone into int16 buffer */
static int16_t *synth_tone(double freq, double dur, double amplitude, int sr, size_t *out_frames) {
    size_t frames = (size_t)ceil(dur * sr);
    int16_t *buf = malloc(sizeof(int16_t) * frames);
    if (!buf) return NULL;
    double phase = 0.0, dt = 1.0 / sr;
    double amp = amplitude * 0.85;
    for (size_t i = 0; i < frames; ++i) {
        double incr = 2.0 * M_PI * freq * dt;
        phase += incr;
        if (phase > 2.0*M_PI) phase -= 2.0*M_PI;
        double s = sin(phase) * amp;
        if (s > 1.0) s = 1.0;
        if (s < -1.0) s = -1.0;
        buf[i] = (int16_t)lrint(s * 32767.0);
    }
    *out_frames = frames;
    return buf;
}

/* Duplicate mono -> interleaved stereo */
static void mono_to_stereo(const int16_t *mono, int16_t *dst, size_t frames) {
    for (size_t i = 0; i < frames; ++i) {
        dst[2*i] = mono[i];
        dst[2*i + 1] = mono[i];
    }
}

/* ALSA open helper */
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

/* Write frames to ALSA, handle underrun */
static int alsa_write_frames(snd_pcm_t *pcm, const int16_t *buf, snd_pcm_uframes_t frames) {
    snd_pcm_sframes_t r = snd_pcm_writei(pcm, buf, frames);
    if (r == -EPIPE) {
        snd_pcm_prepare(pcm);
        r = snd_pcm_writei(pcm, buf, frames);
    }
    if (r < 0) {
        fprintf(stderr, "snd_pcm_writei: %s\n", snd_strerror((int)r));
        return (int)r;
    }
    return 0;
}

int main(void) {
    signal(SIGINT, handle_sigint);

    /* Precompute per-note durations for a wail cycle: split WAIL_CYCLE_SEC equally across chosen notes */
    const size_t notes_per_wail = A_MINOR_HIGH_COUNT;
    double w_note_dur = WAIL_CYCLE_SEC / (double)notes_per_wail;

    /* For yelp, alternate short sub-notes across the yelp cycle */
    const int y_alternations = 20;
    double y_sub_dur = YELP_CYCLE_SEC / (double)y_alternations;

    /* Prepare stereo buffer large enough for the longest chunk */
    size_t max_frames = (size_t)ceil(w_note_dur * SR);
    size_t y_sub_frames = (size_t)ceil(y_sub_dur * SR);
    if (y_sub_frames > max_frames) max_frames = y_sub_frames;
    int16_t *stereo_buf = malloc(sizeof(int16_t) * max_frames * 2);
    if (!stereo_buf) return 1;

    snd_pcm_t *pcm = NULL;
    if (open_alsa(&pcm, SR, CHANNELS) < 0) { free(stereo_buf); return 1; }

    srand((unsigned)time(NULL));

    /* Ramp selection index across TOTAL_BLOCKS from low index -> high index */
    for (int block = 0; block < TOTAL_BLOCKS && keep_running; ++block) {
        double t = (TOTAL_BLOCKS <= 1) ? 0.0 : (double)block / (double)(TOTAL_BLOCKS - 1);
        /* compute a base index shift (0..(count-1)) to bias toward higher notes as t increases */
        double idx_shift = t * (double)(A_MINOR_HIGH_COUNT - 1);

        double r = (double)rand() / (double)RAND_MAX;
        int choose_wail_block = (r < WAIL_BLOCK_PROB) ? 1 : 0;

        if (choose_wail_block) {
            /* Play BLOCK_CYCLES wail cycles; each cycle plays A-minor high notes in order.
               Apply amplitude fade across cycles: 1.0 -> 0.0.
               For each note, pick a note index offset by idx_shift (rounded) to move upward over blocks. */
            for (int cycle = 0; cycle < BLOCK_CYCLES && keep_running; ++cycle) {
                double scale = 1.0;
                if (BLOCK_CYCLES > 1) scale = 1.0 - ((double)cycle / (double)(BLOCK_CYCLES - 1));
                for (size_t n = 0; n < notes_per_wail && keep_running; ++n) {
                    /* compute target index: base n plus shift, clamp to range */
                    int idx = (int)round((double)n + idx_shift);
                    if (idx < 0) idx = 0;
                    if (idx >= (int)A_MINOR_HIGH_COUNT) idx = (int)A_MINOR_HIGH_COUNT - 1;
                    double freq = A_MINOR_HIGH[idx];
                    size_t frames = 0;
                    int16_t *mono = synth_tone(freq, w_note_dur, scale, SR, &frames);
                    if (!mono) { keep_running = 0; break; }
                    mono_to_stereo(mono, stereo_buf, frames);
                    free(mono);
                    if (alsa_write_frames(pcm, stereo_buf, (snd_pcm_uframes_t)frames) != 0) { keep_running = 0; break; }
                }
            }
        } else {
            /* Play BLOCK_CYCLES yelp cycles; each cycle alternates y_alternations times between two high A-minor notes.
               Apply idx_shift to both base yelp notes. */
            for (int cycle = 0; cycle < BLOCK_CYCLES && keep_running; ++cycle) {
                for (int a = 0; a < y_alternations && keep_running; ++a) {
                    /* choose base yelp note index (C6 ~ idx 2, E6 ~ idx 4 in A_MINOR_HIGH) */
                    int base_idx = (a & 1) ? 4 : 2; /* E6 or C6 */
                    int idx = (int)round((double)base_idx + idx_shift);
                    if (idx < 0) idx = 0;
                    if (idx >= (int)A_MINOR_HIGH_COUNT) idx = (int)A_MINOR_HIGH_COUNT - 1;
                    double freq = A_MINOR_HIGH[idx];
                    size_t frames = 0;
                    int16_t *mono = synth_tone(freq, y_sub_dur, 1.0, SR, &frames);
                    if (!mono) { keep_running = 0; break; }
                    mono_to_stereo(mono, stereo_buf, frames);
                    free(mono);
                    if (alsa_write_frames(pcm, stereo_buf, (snd_pcm_uframes_t)frames) != 0) { keep_running = 0; break; }
                }
            }
        }
    }

    /* final closing tone at A6 (1760 Hz) if still running */
    if (keep_running) {
        double final_freq = 1760.0;
        double close_dur = 1.0;
        size_t frames = 0;
        int16_t *mono = synth_tone(final_freq, close_dur, 1.0, SR, &frames);
        if (mono) {
            mono_to_stereo(mono, stereo_buf, frames);
            alsa_write_frames(pcm, stereo_buf, (snd_pcm_uframes_t)frames);
            free(mono);
        }
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(stereo_buf);
    return 0;
}

