// record_96k_float.c
// Capture mono from default ALSA input (best-effort hardware rate ~14500 Hz),
// anti-alias decimate to low-rate, high-quality windowed-sinc resample to 96 kHz float,
// write real-time to ALSA playback (float32) and to a .raw float32 file.
// Compile: gcc -O2 record_96k_float.c -o record_96k_float -lasound -lm
// Run: ./record_96k_float
//
// Notes:
// - Output sample format: 32-bit float little-endian, 96 000 Hz, mono.
// - The program uses a windowed-sinc resampler implemented with a LUT for quality.
// - If your ALSA device does not accept float, ALSA may convert automatically.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <time.h>

#define CAPTURE_RATE      14500u     // desired capture rate (device will choose nearest)
#define DECIMATE          8          // decimation factor applied before resampling
#define LR_RATE           ((double)CAPTURE_RATE / DECIMATE)
#define OUT_RATE          96000u     // final output rate (float32)
#define CHANNELS          1
#define CAP_FORMAT        SND_PCM_FORMAT_S16_LE
#define PLAY_FORMAT       SND_PCM_FORMAT_FLOAT_LE

// FIR anti-alias for decimation by 8 (63 taps example)
#define FIR_TAPS 63
static const double fir[FIR_TAPS] = {
  -0.000438,-0.000648,-0.000906,-0.001182,-0.001435,-0.001616,-0.001672,-0.001547,
  -0.001194,-0.000572, 0.000292, 0.001517, 0.003072, 0.004897, 0.006895, 0.008943,
   0.010880, 0.012518, 0.013653, 0.014088, 0.013670, 0.012300, 0.009955, 0.006708,
   0.002728,-0.001538,-0.005771,-0.009691,-0.012994,-0.015350,-0.016470,-0.016135,
  -0.014227,-0.010782,-0.005995, 0.000000, 0.007436, 0.016164, 0.026006, 0.036737,
   0.048118, 0.059865, 0.071678, 0.083237, 0.094233, 0.104371, 0.113372, 0.121010,
   0.127103, 0.131519, 0.134185, 0.135073, 0.134220, 0.131737, 0.127812, 0.122701,
   0.116724, 0.110241, 0.103619, 0.097254, 0.091516
};

// Resampler LUT settings (quality vs CPU)
#define SINC_HALF_WIDTH 32
#define SINC_LUT_POINTS 4096
#define LUT_STRIDE (2 * SINC_HALF_WIDTH)

// Buffers and blocks
#define READ_BLOCK_FRAMES (2048 * DECIMATE)
#define LR_RING (1<<16)
#define OUT_BLOCK 2048

static double sinc(double x) {
    if (x == 0.0) return 1.0;
    return sin(M_PI * x) / (M_PI * x);
}
static double window_blackman(double x, double halfw) {
    double N = 2.0 * halfw;
    double n = x + halfw;
    double denom = (N - 1.0);
    double a0 = 0.42, a1 = 0.5, a2 = 0.08;
    return a0 - a1 * cos(2.0 * M_PI * n / denom) + a2 * cos(4.0 * M_PI * n / denom);
}

static void build_sinc_lut(double *lut) {
    for (int i = 0; i < SINC_LUT_POINTS; ++i) {
        double frac = (double)i / (double)SINC_LUT_POINTS;
        for (int k = -SINC_HALF_WIDTH; k < SINC_HALF_WIDTH; ++k) {
            double x = (double)k - frac;
            double s = sinc(x);
            double w = window_blackman(x, (double)SINC_HALF_WIDTH);
            lut[i * LUT_STRIDE + (k + SINC_HALF_WIDTH)] = s * w;
        }
    }
}

static double uniform_rand() {
    return ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
}

static int set_hw_params_capture(snd_pcm_t *pcm, unsigned int rate) {
    snd_pcm_hw_params_t *hw;
    snd_pcm_hw_params_malloc(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, hw, CAP_FORMAT);
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
    snd_pcm_hw_params_set_format(pcm, hw, PLAY_FORMAT);
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

int main(void) {
    srand((unsigned int)time(NULL));

    snd_pcm_t *cap = NULL, *play = NULL;
    int err;

    if ((err = snd_pcm_open(&cap, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "capture open: %s\n", snd_strerror(err)); return 1;
    }
    if ((err = set_hw_params_capture(cap, CAPTURE_RATE)) < 0) {
        fprintf(stderr, "capture hwparams: %s\n", snd_strerror(err)); snd_pcm_close(cap); return 1;
    }
    if ((err = snd_pcm_open(&play, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "playback open: %s\n", snd_strerror(err)); snd_pcm_close(cap); return 1;
    }
    if ((err = set_hw_params_playback(play, OUT_RATE)) < 0) {
        fprintf(stderr, "playback hwparams: %s\n", snd_strerror(err)); snd_pcm_close(cap); snd_pcm_close(play); return 1;
    }

    // open output file
    FILE *outf = fopen("out_96k_f32.raw", "wb");
    if (!outf) { perror("fopen"); goto cleanup; }

    // buffers
    int16_t *read_buf = malloc(sizeof(int16_t) * READ_BLOCK_FRAMES);
    if (!read_buf) { perror("malloc"); goto cleanup; }

    double firbuf[FIR_TAPS]; memset(firbuf,0,sizeof(firbuf));
    int firpos = 0;
    long sample_counter = 0;

    double *lr_ring = calloc(LR_RING, sizeof(double));
    if (!lr_ring) { perror("calloc"); goto cleanup; }
    long lr_total = 0;
    int lr_w = 0;

    double *sinc_lut = malloc(sizeof(double) * SINC_LUT_POINTS * LUT_STRIDE);
    if (!sinc_lut) { perror("malloc"); goto cleanup; }
    build_sinc_lut(sinc_lut);

    float *out_block = malloc(sizeof(float) * OUT_BLOCK);
    if (!out_block) { perror("malloc"); goto cleanup; }

    // resampler state
    double lr_rate = LR_RATE;
    double ratio = lr_rate / (double)OUT_RATE; // low-rate samples per one output sample
    double resamp_pos = 0.0; // in low-rate sample coordinates, where next output sample will be interpolated from

    fprintf(stderr, "Recording: CAP %u -> decimate %d -> LR %.6f -> OUT %u Hz float32\n",
            CAPTURE_RATE, DECIMATE, lr_rate, OUT_RATE);

    // main loop
    while (1) {
        int read_frames = snd_pcm_readi(cap, read_buf, READ_BLOCK_FRAMES);
        if (read_frames == -EPIPE) { snd_pcm_prepare(cap); continue; }
        if (read_frames < 0) { fprintf(stderr, "capture read error: %s\n", snd_strerror(read_frames)); break; }

        // produce low-rate decimated samples
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
                // small noise mix (optional, keep low)
                const double NOISE_LEVEL = 0.00;
                double noise = uniform_rand() * 32767.0;
                double mixed = acc * (1.0 - NOISE_LEVEL) + noise * NOISE_LEVEL;
                if (mixed > 32767.0) mixed = 32767.0;
                if (mixed < -32768.0) mixed = -32768.0;
                lr_ring[lr_w] = mixed / 32768.0;
                lr_w = (lr_w + 1) % LR_RING;
                lr_total++;
            }
            firpos = (firpos + 1) % FIR_TAPS;
            sample_counter++;
        }

        // generate output samples while enough low-rate samples available
        while (1) {
            long need_index = (long)floor(resamp_pos);
            if (need_index + SINC_HALF_WIDTH >= lr_total) break; // wait for more low-rate samples

            // synthesize OUT_BLOCK samples (or fewer if you prefer streaming smaller blocks)
            for (int n = 0; n < OUT_BLOCK; ++n) {
                double pos = resamp_pos + (double)n * ratio;
                long idx0 = (long)floor(pos);
                double frac = pos - (double)idx0;
                int lut_idx = (int)(frac * (double)SINC_LUT_POINTS);
                if (lut_idx >= SINC_LUT_POINTS) lut_idx = SINC_LUT_POINTS - 1;

                double outv = 0.0;
                for (int k = -SINC_HALF_WIDTH; k < SINC_HALF_WIDTH; ++k) {
                    long sample_index = idx0 + k;
                    long stored_start = lr_total - (lr_total < LR_RING ? lr_total : LR_RING);
                    long abs_pos = sample_index - stored_start;
                    if (abs_pos < 0 || abs_pos >= LR_RING) continue;
                    int ring_idx = (int)(((lr_w - (lr_total - stored_start)) + abs_pos) % LR_RING);
                    if (ring_idx < 0) ring_idx += LR_RING;
                    double sample = lr_ring[ring_idx];
                    double w = sinc_lut[lut_idx * LUT_STRIDE + (k + SINC_HALF_WIDTH)];
                    outv += sample * w;
                }
                out_block[n] = (float)outv;
            }

            // write out_block to playback and file
            // playback: may accept fewer frames, handle underrun
            int to_write = OUT_BLOCK;
            int written = snd_pcm_writei(play, out_block, to_write);
            if (written == -EPIPE) { snd_pcm_prepare(play); }
            else if (written < 0) { fprintf(stderr, "playback write error: %s\n", snd_strerror(written)); }

            // file write: float32 little-endian
            if (fwrite(out_block, sizeof(float), OUT_BLOCK, outf) != OUT_BLOCK) {
                perror("fwrite"); goto cleanup;
            }

            // advance resamp_pos by OUT_BLOCK * ratio
            resamp_pos += (double)OUT_BLOCK * ratio;
        }
    }

cleanup:
    if (read_buf) free(read_buf);
    if (lr_ring) free(lr_ring);
    if (sinc_lut) free(sinc_lut);
    if (out_block) free(out_block);
    if (outf) fclose(outf);
    if (cap) snd_pcm_close(cap);
    if (play) snd_pcm_close(play);
    return 0;
}
