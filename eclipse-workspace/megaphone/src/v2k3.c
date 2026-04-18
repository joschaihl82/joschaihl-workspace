// alsa_live_down8_mod_fixed.c
// Real-time: capture from default ALSA input at 14500 Hz mono S16_LE,
// lowpass + decimate by 8, mix white noise, amplitude-modulate, and play to default ALSA output.
// No command-line parameters; built-in parameters chosen for general use.
// Compile: gcc -O2 alsa_live_down8_mod_fixed.c -o alsa_live_down8_mod_fixed -lasound -lm
// Run: ./alsa_live_down8_mod_fixed

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <time.h>

#define CHANNELS 1
#define FORMAT SND_PCM_FORMAT_S16_LE
#define DECIMATE 8
#define FIR_TAPS 31

// Built-in parameters (tuned for general live monitoring)
static const unsigned int CAPTURE_RATE = 14500;   // in previous range 14400..14600
static const double NOISE_LEVEL = 0.80;          // 0 = none, 1 = noise only
static const double AM_DEPTH = 0.9;             // 0 = none, 1 = full depth
static const double AM_FREQ = 2.0;               // Hz, modulation rate on downsampled stream

// 31-tap FIR coefficients (modest low-pass for decimation)
static const double fir[FIR_TAPS] = {
  -0.001225, -0.001755, -0.001907, -0.000856,  0.001299,  0.003818,  0.005530,
   0.004712, -0.000000, -0.007960, -0.016511, -0.021969, -0.019903, -0.006615,
   0.015228,  0.044030,  0.075383,  0.102582,  0.119720,  0.123069,  0.112274,
   0.089411,  0.058075,  0.023686, -0.006864, -0.024386, -0.023513, -0.003645,
   0.030752,  0.069609,  0.098247
};

static double uniform_rand() {
    return ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
}

static int set_hw_params_capture(snd_pcm_t *pcm, unsigned int rate) {
    snd_pcm_hw_params_t *hw;
    snd_pcm_hw_params_malloc(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, hw, FORMAT);
    snd_pcm_hw_params_set_channels(pcm, hw, CHANNELS);
    unsigned int r = rate;
    snd_pcm_hw_params_set_rate_near(pcm, hw, &r, 0);
    snd_pcm_uframes_t period = 1024;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    snd_pcm_uframes_t buffer = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer);
    int err = snd_pcm_hw_params(pcm, hw);
    snd_pcm_hw_params_free(hw);
    return err;
}

static int set_hw_params_playback(snd_pcm_t *pcm, unsigned int rate) {
    snd_pcm_hw_params_t *hw;
    snd_pcm_hw_params_malloc(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, hw, FORMAT);
    snd_pcm_hw_params_set_channels(pcm, hw, CHANNELS);
    unsigned int r = rate;
    snd_pcm_hw_params_set_rate_near(pcm, hw, &r, 0);
    snd_pcm_uframes_t period = 256;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    snd_pcm_uframes_t buffer = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer);
    int err = snd_pcm_hw_params(pcm, hw);
    snd_pcm_hw_params_free(hw);
    return err;
}

int main(void) {
    srand((unsigned int)time(NULL));

    snd_pcm_t *cap = NULL, *play = NULL;
    int err;

    if ((err = snd_pcm_open(&cap, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "capture open error: %s\n", snd_strerror(err)); return 1;
    }
    if ((err = set_hw_params_capture(cap, CAPTURE_RATE)) < 0) {
        fprintf(stderr, "capture hwparams: %s\n", snd_strerror(err)); snd_pcm_close(cap); return 1;
    }

    unsigned int out_rate = CAPTURE_RATE / DECIMATE;
    if ((err = snd_pcm_open(&play, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "playback open error: %s\n", snd_strerror(err)); snd_pcm_close(cap); return 1;
    }
    if ((err = set_hw_params_playback(play, out_rate)) < 0) {
        fprintf(stderr, "playback hwparams: %s\n", snd_strerror(err)); snd_pcm_close(cap); snd_pcm_close(play); return 1;
    }

    int frames_per_read = 1024 * DECIMATE;
    int16_t *read_buf = malloc(sizeof(int16_t) * frames_per_read * CHANNELS);
    if (!read_buf) { perror("malloc"); goto cleanup; }

    int out_buf_frames = 1024;
    int16_t *out_buf = malloc(sizeof(int16_t) * out_buf_frames * CHANNELS);
    if (!out_buf) { perror("malloc"); goto cleanup; }

    double firbuf[FIR_TAPS];
    memset(firbuf, 0, sizeof(firbuf));
    int firpos = 0;
    long sample_counter = 0;
    double am_phase = 0.0;
    double am_inc = (AM_FREQ > 0.0) ? (2.0 * M_PI * AM_FREQ / (double)out_rate) : 0.0;

    fprintf(stderr, "Capture %u Hz -> output ~%u Hz (decimate %d). Noise %.2f AM depth %.2f Hz %.2f\n",
            CAPTURE_RATE, out_rate, DECIMATE, NOISE_LEVEL, AM_DEPTH, AM_FREQ);

    while (1) {
        int read_frames = snd_pcm_readi(cap, read_buf, frames_per_read);
        if (read_frames == -EPIPE) { snd_pcm_prepare(cap); continue; }
        if (read_frames < 0) { fprintf(stderr, "capture read error: %s\n", snd_strerror(read_frames)); break; }
        int out_i = 0;
        for (int i = 0; i < read_frames; ++i) {
            double x = (double)read_buf[i];
            firbuf[firpos] = x;
            if ((sample_counter % DECIMATE) == 0) {
                double acc = 0.0;
                int idx = firpos;
                for (int t = 0; t < FIR_TAPS; ++t) {
                    acc += fir[t] * firbuf[idx];
                    if (--idx < 0) idx += FIR_TAPS;
                }
                double noise = uniform_rand() * 32767.0;
                double mixed = acc * (1.0 - NOISE_LEVEL) + noise * NOISE_LEVEL;
                if (AM_DEPTH > 0.0 && am_inc > 0.0) {
                    double am_factor = 1.0 + AM_DEPTH * sin(am_phase);
                    mixed *= am_factor;
                    am_phase += am_inc;
                    if (am_phase > 2.0*M_PI) am_phase -= 2.0*M_PI;
                }
                if (mixed > 32767.0) mixed = 32767.0;
                if (mixed < -32768.0) mixed = -32768.0;
                if (out_i < out_buf_frames) {
                    out_buf[out_i++] = (int16_t)lrint(mixed);
                } else {
                    int written = snd_pcm_writei(play, out_buf, out_i);
                    if (written == -EPIPE) { snd_pcm_prepare(play); }
                    else if (written < 0) { fprintf(stderr, "playback write error: %s\n", snd_strerror(written)); }
                    out_i = 0;
                    out_buf[out_i++] = (int16_t)lrint(mixed);
                }
            }
            firpos = (firpos + 1) % FIR_TAPS;
            sample_counter++;
        }
        if (out_i > 0) {
            int written = snd_pcm_writei(play, out_buf, out_i);
            if (written == -EPIPE) { snd_pcm_prepare(play); }
            else if (written < 0) { fprintf(stderr, "playback write error: %s\n", snd_strerror(written)); }
        }
    }

cleanup:
    if (read_buf) free(read_buf);
    if (out_buf) free(out_buf);
    if (cap) snd_pcm_close(cap);
    if (play) snd_pcm_close(play);
    return 0;
}