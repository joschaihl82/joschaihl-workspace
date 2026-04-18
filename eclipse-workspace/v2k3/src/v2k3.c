/*
 * aec_boost_1812_5_mod.c
 *
 * Echtzeit-Audio mit ALSA:
 * - Boost um center = 14500/8 = 1812.5 Hz +/-10% (FFT+OLA)
 * - Zusätzlich: moduliere einen 1812.5 Hz Carrier mit gefiltertem Zufallsrauschen
 * und addiere das Ergebnis (Modulation) zum Ausgang.
 * - Neu: Automatic Gain Control (AGC), um den Ausgangspegel zu normalisieren.
 *
 * Kompilieren:
 * gcc -O3 aec_boost_agc.c -o boost_agc -lasound -lm
 *
 * Hinweise:
 * - Mono, FLOAT_LE, SAMPLE_RATE = 48000, WIN = 2048, HOP = WIN/2
 * - Einfacher IIR-Filter (first-order HP und LP cascade) als 'Rauschfilter'
 * - Modulationspegel einstellbar via mod_gain_db
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <signal.h>
#include <time.h>

#define SAMPLE_RATE 48000.0f
#define WIN 2048
#define HOP (WIN/2)
#define EPS 1e-12f

static volatile int running = 1;
void sigint_handler(int _) { (void)_; running = 0; }

/* complex */
typedef struct { float re, im; } complexf;

/* simple radix-2 iterative FFT */
void fft_iterative(complexf *buf, int n, int direction) {
    int i, j, k, len;
    j = 0;
    for (i = 1; i < n; i++) {
        int bit = n >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j ^= bit;
        if (i < j) {
            complexf tmp = buf[i]; buf[i] = buf[j]; buf[j] = tmp;
        }
    }
    for (len = 2; len <= n; len <<= 1) {
        double ang = 2.0 * M_PI / (double)len * (direction == 1 ? -1.0 : 1.0);
        float wlen_re = (float)cos(ang), wlen_im = (float)sin(ang);
        for (i = 0; i < n; i += len) {
            float wre = 1.0f, wim = 0.0f;
            for (k = 0; k < len/2; k++) {
                complexf u = buf[i + k];
                complexf t = buf[i + k + len/2];
                complexf v;
                v.re = t.re * wre - t.im * wim;
                v.im = t.re * wim + t.im * wre;
                buf[i + k].re = u.re + v.re;
                buf[i + k].im = u.im + v.im;
                buf[i + k + len/2].re = u.re - v.re;
                buf[i + k + len/2].im = u.im - v.im;
                float tmp_re = wre * wlen_re - wim * wlen_im;
                float tmp_im = wre * wlen_im + wim * wlen_re;
                wre = tmp_re; wim = tmp_im;
            }
        }
    }
    if (direction == -1) {
        for (i = 0; i < n; i++) { buf[i].re /= n; buf[i].im /= n; }
    }
}

void make_hann(float *w, int n) {
    for (int i = 0; i < n; ++i) w[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (n - 1)));
}

/* Simple white-noise generator: returns float in [-1,1] */
static inline float white_noise() {
    /* rand() gives 0..RAND_MAX; scale to [-1,1] */
    return 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;
}

/* Simple cascading HP then LP first-order filters as band-shaper for noise.
   HP: y[n] = a0*x[n] + a1*x[n-1] - b1*y[n-1]
   LP: standard one-pole lowpass.
   We keep state per channel-block.
*/
typedef struct {
    float hp_x1, hp_y1;
    float lp_y1;
    /* coefficients */
    float hp_a0, hp_a1, hp_b1;
    float lp_alpha;
} noise_filter_t;

void noise_filter_init(noise_filter_t *f, float hp_cut, float lp_cut, float fs) {
    memset(f, 0, sizeof(*f));
    /* highpass first-order (bilinear approx) */
    float wc_hp = 2.0f * M_PI * hp_cut;
    float T = 1.0f / fs;
    float K = wc_hp * T / 2.0f;
    /* simple normalized coefficients (one possible stable design) */
    float denom_hp = 1.0f + K;
    f->hp_a0 = 1.0f / denom_hp;
    f->hp_a1 = -1.0f / denom_hp;
    f->hp_b1 = (1.0f - K) / denom_hp;
    /* lowpass one-pole (exponential smoothing) */
    float RC = 1.0f / (2.0f * M_PI * lp_cut);
    float alpha = T / (RC + T);
    f->lp_alpha = alpha;
}

float noise_filter_process(noise_filter_t *f, float x) {
    /* HP */
    float y_hp = f->hp_a0 * x + f->hp_a1 * f->hp_x1 - f->hp_b1 * f->hp_y1;
    f->hp_x1 = x; f->hp_y1 = y_hp;
    /* LP */
    float y_lp = f->lp_y1 + f->lp_alpha * (y_hp - f->lp_y1);
    f->lp_y1 = y_lp;
    return y_lp;
}

int main(int argc, char **argv) {
    signal(SIGINT, sigint_handler);
    srand((unsigned)time(NULL));

    const float original = 14500.0f;
    const float center = original / 8.0f; /* 1812.5 Hz */
    printf("Using center frequency: %.4f Hz (14500 / 8)\n", center);

    /* band +/-10% */
    const float rel = 0.10f;
    const float lowf = center * (1.0f - rel);
    const float highf = center * (1.0f + rel);

    const float gain_db = 6.0f; /* FFT boost */
    const float linear_gain = powf(10.0f, gain_db / 20.0f);

    /* modulation settings */
    const float mod_gain_db = 6.0f; /* level of added modulation relative to signal (was -6.0f) */
    const float mod_linear = powf(10.0f, mod_gain_db / 20.0f);
    const float carrier_freq = center; /* 1812.5 Hz */

    /* noise filter bands for shaping the modulator (tweak as desired) */
    const float noise_hp_cut = 100.0f;   /* highpass cutoff (was 300.0f) */
    const float noise_lp_cut = 10000.0f;  /* lowpass cutoff (was 3000.0f) */

    /* --- AGC (Automatic Gain Control) Parameters --- */
    const float TARGET_RMS = 0.5f;    // Target RMS value ("green area"), e.g., ~ -6 dBFS. (was 0.15f)
    const float MAX_INPUT_GAIN_DB = 12.0f; // Max gain we can apply
    const float MIN_INPUT_GAIN_DB = -18.0f; // Min gain we can apply
    const float ATTACK_TIME_MS = 25.0f;  // Fast attack for sudden spikes
    const float RELEASE_TIME_MS = 500.0f; // Slower release to avoid "pumping"

    /* AGC state variables */
    float current_input_gain = 1.0f; // linear gain (starts at 0 dB)
    /* --- End AGC Parameters --- */


    /* ALSA setup */
    snd_pcm_t *cap = NULL, *play = NULL;
    snd_pcm_hw_params_t *params;
    int rc;
    const char *cap_dev = "default";
    const char *play_dev = "default";

    rc = snd_pcm_open(&cap, cap_dev, SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) { fprintf(stderr, "Capture open error: %s\n", snd_strerror(rc)); return 1; }
    rc = snd_pcm_open(&play, play_dev, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) { fprintf(stderr, "Playback open error: %s\n", snd_strerror(rc)); snd_pcm_close(cap); return 1; }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(cap, params);
    snd_pcm_hw_params_set_access(cap, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(cap, params, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_channels(cap, params, 1);
    snd_pcm_hw_params_set_rate(cap, params, (unsigned)SAMPLE_RATE, 0);
    snd_pcm_hw_params(cap, params);

    snd_pcm_hw_params_any(play, params);
    snd_pcm_hw_params_set_access(play, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(play, params, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_channels(play, params, 1);
    snd_pcm_hw_params_set_rate(play, params, (unsigned)SAMPLE_RATE, 0);
    snd_pcm_hw_params(play, params);

    /* allocate buffers, windows, etc */
    float *hann = malloc(sizeof(float) * WIN);
    float *norm = malloc(sizeof(float) * WIN);
    make_hann(hann, WIN);
    for (int i = 0; i < WIN; ++i) {
        norm[i] = hann[i] + hann[(i + WIN - HOP) % WIN];
        if (norm[i] < EPS) norm[i] = 1.0f;
    }

    int ring_len = WIN + HOP;
    float *acc = calloc(ring_len, sizeof(float));
    float *wsum = calloc(ring_len, sizeof(float));
    int write_pos = 0, read_pos = 0;

    float *mic_hop = calloc(HOP, sizeof(float));
    float *mic_frame = calloc(WIN, sizeof(float));
    float *proc_frame = calloc(WIN, sizeof(float));
    complexf *spec = malloc(sizeof(complexf) * WIN);

    if (!hann || !norm || !acc || !wsum || !mic_hop || !mic_frame || !proc_frame || !spec) {
        fprintf(stderr, "Allocation failed\n"); goto cleanup;
    }

    /* bins for FFT boost */
    int bin_low = (int)floorf(lowf * (float)WIN / SAMPLE_RATE + 0.5f);
    int bin_high = (int)ceilf(highf * (float)WIN / SAMPLE_RATE - 0.5f);
    if (bin_low < 0) bin_low = 0;
    if (bin_high > WIN/2) bin_high = WIN/2;

    fprintf(stderr, "Boost band: %.1f Hz .. %.1f Hz (bins %d .. %d), gain %.2f dB\n",
            lowf, highf, bin_low, bin_high, gain_db);

    /* Calculate AGC smoothing factors */
    const float HOP_SAMPLES_PER_SEC = SAMPLE_RATE / (float)HOP;
    const float attack_factor = 1.0f - expf(-1.0f / (ATTACK_TIME_MS / 1000.0f * HOP_SAMPLES_PER_SEC));
    const float release_factor = 1.0f - expf(-1.0f / (RELEASE_TIME_MS / 1000.0f * HOP_SAMPLES_PER_SEC));
    fprintf(stderr, "AGC Target RMS: %.3f (%.1f dBFS)\n", TARGET_RMS, 20.0f * log10f(TARGET_RMS));


    /* prepare noise filter state */
    noise_filter_t nf;
    noise_filter_init(&nf, noise_hp_cut, noise_lp_cut, SAMPLE_RATE);

    /* phase accumulator for carrier */
    double phase = 0.0;
    double phase_inc = 2.0 * M_PI * (double)carrier_freq / (double)SAMPLE_RATE;

    /* main loop */
    while (running) {
        rc = snd_pcm_readi(cap, mic_hop, HOP);
        if (rc == -EPIPE) { snd_pcm_prepare(cap); continue; }
        if (rc < 0) { fprintf(stderr, "Capture read error: %s\n", snd_strerror(rc)); break; }

        /* --- AGC: Apply current gain to input signal --- */
        for (int i = 0; i < HOP; ++i) {
            mic_hop[i] *= current_input_gain;
        }

        /* build overlapping frame */
        memmove(mic_frame, mic_frame + HOP, (WIN - HOP) * sizeof(float));
        memcpy(mic_frame + (WIN - HOP), mic_hop, HOP * sizeof(float));

        /* analysis window into complex buffer */
        for (int i = 0; i < WIN; ++i) {
            spec[i].re = mic_frame[i] * hann[i];
            spec[i].im = 0.0f;
        }

        /* forward FFT */
        fft_iterative(spec, WIN, 1);

        /* apply frequency boost */
        for (int k = 0; k <= WIN/2; ++k) {
            float g = 1.0f;
            if (k >= bin_low && k <= bin_high) g = linear_gain;
            spec[k].re *= g; spec[k].im *= g;
            if (k != 0 && k != WIN/2) {
                int k2 = WIN - k;
                spec[k2].re = spec[k].re;
                spec[k2].im = -spec[k].im;
            }
        }

        /* inverse FFT */
        fft_iterative(spec, WIN, -1);

        /* prepare proc_frame (ifft_re / norm) */
        for (int i = 0; i < WIN; ++i) proc_frame[i] = spec[i].re / norm[i];

        /* --- generate modulation vector for this frame (length WIN) ---
           We will create filtered noise per sample and carrier, multiply and then OLA-add
        */
        float *mod_frame = malloc(sizeof(float) * WIN);
        if (!mod_frame) { fprintf(stderr, "alloc mod_frame failed\n"); break; }

        for (int i = 0; i < WIN; ++i) {
            /* generate white noise */
            float wn = white_noise();
            /* filter noise (HP + LP) */
            float nfilt = noise_filter_process(&nf, wn);
            /* carrier sample */
            float carrier = (float)sin(phase);
            phase += phase_inc;
            if (phase >= 2.0 * M_PI) phase -= 2.0 * M_PI;
            /* modulation: amplitude modulation (carrier * filtered_noise) */
            mod_frame[i] = carrier * nfilt * mod_linear;
        }

        /* OLA-add processed (proc_frame) and modulation (mod_frame) into ring */
        for (int i = 0; i < WIN; ++i) {
            int pos = write_pos + i;
            if (pos >= ring_len) pos -= ring_len;
            /* keep both contributions */
            acc[pos] += proc_frame[i] * hann[i] + mod_frame[i] * hann[i];
            wsum[pos] += hann[i];
        }
        write_pos += HOP; if (write_pos >= ring_len) write_pos -= ring_len;

        /* read HOP samples out normalized */
        float out_hop[HOP];
        float rms_sum_sq = 0.0f; // For AGC
        for (int i = 0; i < HOP; ++i) {
            int pos = read_pos + i;
            if (pos >= ring_len) pos -= ring_len;
            float s = acc[pos]; float w = wsum[pos];
            out_hop[i] = (w > EPS) ? (s / w) : 0.0f;
            acc[pos] = 0.0f; wsum[pos] = 0.0f;

            /* AGC: Calculate sum of squares for RMS */
            rms_sum_sq += out_hop[i] * out_hop[i];
        }
        read_pos += HOP; if (read_pos >= ring_len) read_pos -= ring_len;

        free(mod_frame);

        /* --- AGC: Calculate RMS and update gain for *next* frame --- */

        // 1. Calculate the RMS level for this buffer
        float current_rms = sqrtf(rms_sum_sq / (float)HOP);

        // 2. Determine required gain change (in dB)
        float rms_to_db = 20.0f * log10f(current_rms + EPS);
        float target_rms_db = 20.0f * log10f(TARGET_RMS + EPS);
        float gain_needed_db = target_rms_db - rms_to_db;

        // 3. Clamp the required gain to defined limits
        // Note: This logic assumes gain_needed_db is the *total* desired gain.
        // A better approach (implemented here) is to adjust the *current* gain.
        float current_gain_db = 20.0f * log10f(current_input_gain + EPS);
        float new_target_gain_db = current_gain_db + gain_needed_db;

        // Clamp the *target* gain, not the adjustment
        if (new_target_gain_db > MAX_INPUT_GAIN_DB) {
            new_target_gain_db = MAX_INPUT_GAIN_DB;
        } else if (new_target_gain_db < MIN_INPUT_GAIN_DB) {
            new_target_gain_db = MIN_INPUT_GAIN_DB;
        }

        // 4. Convert target gain back to linear
        float new_target_gain = powf(10.0f, new_target_gain_db / 20.0f);

        // 5. Smooth the gain change (Attack/Release)
        float smoothing_factor;
        if (new_target_gain > current_input_gain) {
            // Gain is increasing (Attack)
            smoothing_factor = attack_factor;
        } else {
            // Gain is decreasing (Release)
            smoothing_factor = release_factor;
        }
        
        // Simple one-pole smoothing
        current_input_gain = current_input_gain + smoothing_factor * (new_target_gain - current_input_gain);
        
        /* --- End AGC Logic --- */


        /* write output */
        rc = snd_pcm_writei(play, out_hop, HOP);
        if (rc == -EPIPE) { snd_pcm_prepare(play); continue; }
        if (rc < 0) { fprintf(stderr, "Playback write error: %s\n", snd_strerror(rc)); break; }
    }

cleanup:
    if (cap) snd_pcm_close(cap);
    if (play) snd_pcm_close(play);
    free(hann); free(norm);
    free(acc); free(wsum);
    free(mic_hop); free(mic_frame); free(proc_frame); free(spec);
    fprintf(stderr, "Exiting\n");
    return 0;
}

