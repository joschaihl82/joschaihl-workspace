/*
 lapd_conv_reverb.c
 - Realistic LAPD siren + short convolution reverb (vehicle/urban style IRs)
 - Uses libasound (FLOAT_LE stereo, 44.1 kHz)
 - Convolution implemented by direct convolution per block with overlap-add
 - Short built-in IRs (small arrays) representing vehicle/urban reflections
 - Build: gcc -O2 -Wall -o lapd_conv_reverb lapd_conv_reverb.c -lasound -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>

/* ---------- Constants ---------- */
#define SR         44100u
#define CHANNELS   2
#define PERIOD     1024u
#define MAX_IR     512     /* maximum impulse length supported (keep short for real-time) */

/* ---------- Utilities ---------- */
static inline float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
static inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }
static unsigned int xrng_state = 0x89ABCDEFu;
static unsigned int xrng(void) { unsigned int x = xrng_state; x ^= x << 13; x ^= x >> 17; x ^= x << 5; return xrng_state = x; }
static float frand(void) { return (float)(xrng() & 0xFFFF) / 65535.0f * 2.0f - 1.0f; }

/* ---------- Small built-in impulse responses ----------
   Two short IRs intended to emulate close vehicle / urban reflections.
   These are artificial, short (tens to a few hundred taps), and spectrally shaped.
   You can replace them with measured IR arrays (exported as floats).
*/
static const float IR_VEHICLE[] = {
    /* ~80 taps: initial strong reflection + small later echoes */
    1.00f, 0.30f, 0.12f, 0.06f, 0.03f, 0.015f, 0.007f, 0.0035f, 0.0018f, 0.0009f,
    0.0005f, 0.00025f, 0.00012f, 0.00006f, 0.00003f, 0.000015f,
    /* tail with small spaced echoes */
    0.00025f, 0.0004f, 0.0003f, 0.00018f, 0.0001f, 0.00006f, 0.00003f, 0.00002f,
    0.00001f, 0.0f
};
static const int IR_VEH_LEN = (int)(sizeof(IR_VEHICLE)/sizeof(IR_VEHICLE[0]));

static const float IR_URBAN[] = {
    /* ~160 taps: denser early reflections + smoother decay */
    0.85f, 0.42f, 0.26f, 0.15f, 0.09f, 0.055f, 0.035f, 0.022f, 0.014f, 0.009f,
    0.006f, 0.004f, 0.0026f, 0.0019f, 0.0013f, 0.0009f,
    /* moderate tail with small echoes to 160 samples */
    0.0011f,0.00105f,0.0010f,0.00095f,0.0009f,0.00085f,0.0008f,0.00075f,
    0.0007f,0.00065f,0.0006f,0.00055f,0.0005f,0.00045f,0.0004f,0.00035f,
    0.00032f,0.00030f,0.00027f,0.00025f,0.00022f,0.00020f,0.00018f,0.00016f,
    0.00014f,0.00012f,0.00010f,0.00009f,0.00008f,0.00007f,0.00006f,0.00005f,
    0.00004f,0.000035f,0.00003f,0.000028f,0.000025f,0.000022f,0.000020f,0.000018f,
    0.000016f,0.000015f,0.000013f,0.000012f,0.000011f,0.000010f,0.000009f,0.000008f,
    0.000007f,0.000006f,0.0000055f,0.000005f,0.0000045f,0.000004f,0.0000035f,0.000003f,
    0.0000025f,0.000002f,0.0000018f,0.0000016f,0.0000014f,0.0000012f,0.0000010f,0.0f
};
static const int IR_URB_LEN = (int)(sizeof(IR_URBAN)/sizeof(IR_URBAN[0]));

/* ---------- Convolution reverb (simple overlap-add, short IRs) ---------- */
typedef struct {
    const float *ir;      /* pointer to IR taps */
    int ir_len;           /* IR length in samples */
    float *delay;         /* circular delay buffer for overlap-add (per channel) */
    int delay_size;       /* length of delay buffer (>= period + ir_len) */
    int write_pos;        /* position to write next block's input */
} Conv;

/* Initialize convolution structure for given IR and block size (period) */
static int conv_init(Conv *c, const float *ir, int ir_len, int period) {
    if (ir_len <= 0 || ir_len > MAX_IR) return -1;
    c->ir = ir;
    c->ir_len = ir_len;
    /* choose delay_size = period + ir_len (power-of-two not needed) */
    c->delay_size = period + ir_len;
    c->delay = (float*)calloc(c->delay_size, sizeof(float));
    if (!c->delay) return -1;
    c->write_pos = 0;
    return 0;
}
static void conv_free(Conv *c) {
    if (c->delay) free(c->delay);
    c->delay = NULL;
    c->delay_size = 0;
}

/* Process one block (frame_count samples) of mono input `in` into mono output `out`
   using convolution with IR (overlap-add).
   Both in/out buffers are length frame_count.
   Overlap-add method:
     - write input samples into circular delay buffer at write_pos
     - compute output[n] = sum_{k=0..ir_len-1} delay[write_pos - n + k] * ir[k]
   Implementation here is direct nested loops; optimized for short IR sizes.
*/
static void conv_process_block(Conv *c, const float *in, float *out, int frame_count) {
    int N = frame_count;
    int M = c->ir_len;
    int D = c->delay_size;
    int wp = c->write_pos;

    /* write input into delay buffer */
    for (int n = 0; n < N; ++n) {
        c->delay[(wp + n) % D] += in[n]; /* accumulate to preserve previous overlap contributions */
    }

    /* compute output for current block */
    for (int n = 0; n < N; ++n) {
        float acc = 0.0f;
        /* delay index of sample corresponding to output sample n (the newest sample at position wp + n) */
        int pos = (wp + n) % D;
        /* convolve: acc = sum_{k=0..M-1} delay[pos - k] * ir[k] */
        /* iterate k, reading earlier samples; wrap indices appropriately */
        int idx = pos;
        for (int k = 0; k < M; ++k) {
            /* idx decreases (pos - k) modulo D */
            float d = c->delay[idx];
            acc += d * c->ir[k];
            if (--idx < 0) idx += D;
        }
        out[n] = acc;
    }

    /* clear the used part of delay buffer (we consumed those samples) */
    for (int n = 0; n < N; ++n) {
        int p = (wp + n) % D;
        c->delay[p] = 0.0f;
    }

    /* advance write position */
    c->write_pos = (wp + N) % D;
}

/* ---------- DSP building blocks (voice generation/from previous program) ---------- */
/* We'll keep a compact voice generator: two partials with vibrato, bandpass shaping, mild noise */

/* Simple biquad bandpass (for spectral shaping) */
typedef struct { float b0,b1,b2,a1,a2; float x1,x2,y1,y2; } BQ;
static void bq_init_bandpass(BQ *b, float fc, float q, float sr) {
    float w = 2.0f * M_PI * fc / sr;
    float alpha = sinf(w) / (2.0f * q);
    float cosw = cosf(w);
    float A0 = 1.0f + alpha;
    b->b0 = alpha / A0; b->b1 = 0.0f; b->b2 = -alpha / A0;
    b->a1 = -2.0f * cosw / A0; b->a2 = (1.0f - alpha) / A0;
    b->x1=b->x2=b->y1=b->y2=0.0f;
}
static float bq_process(BQ *b, float x) {
    float y = b->b0*x + b->b1*b->x1 + b->b2*b->x2 - b->a1*b->y1 - b->a2*b->y2;
    b->x2 = b->x1; b->x1 = x; b->y2 = b->y1; b->y1 = y;
    return y;
}

/* Soft clip */
static float softclip(float x, float drive) {
    x *= drive;
    if (x > 1.5f) x = 1.5f;
    if (x < -1.5f) x = -1.5f;
    float x2 = x*x;
    return (x * (27.0f + x2)) / (27.0f + 9.0f * x2);
}

/* Simple compressor (RMS-like envelope) */
typedef struct { float env; float thr; float ratio; float atk_coeff, rel_coeff; } Comp;
static void comp_init(Comp *c, float thr_db, float ratio, float atk_ms, float rel_ms, float sr) {
    c->env = 0.0f;
    c->thr = powf(10.0f, thr_db / 20.0f);
    c->ratio = ratio;
    c->atk_coeff = expf(-1.0f / (0.001f * atk_ms * sr));
    c->rel_coeff = expf(-1.0f / (0.001f * rel_ms * sr));
}
static float comp_process(Comp *c, float in) {
    float absx = fabsf(in);
    if (absx > c->env) c->env = c->atk_coeff * (c->env - absx) + absx;
    else c->env = c->rel_coeff * (c->env - absx) + absx;
    if (c->env <= c->thr) return in;
    float over = c->env / c->thr;
    float gain = powf(over, (1.0f - 1.0f / c->ratio));
    return in / gain;
}

/* Voice structure */
typedef struct {
    double ph1_l, ph1_r;
    double ph2_l, ph2_r;
    float width;
    float amp;
    float vibrato_rate;
    float vibrato_amt;
    float jitter_amt;
    BQ bp1, bp2;
} Voice;
static void voice_init(Voice *v) {
    v->ph1_l = 0.0; v->ph1_r = 0.31;
    v->ph2_l = 1.0; v->ph2_r = 1.18;
    v->width = 0.25f; v->amp = 0.9f; v->vibrato_rate = 5.6f; v->vibrato_amt = 0.004f; v->jitter_amt = 0.0015f;
    bq_init_bandpass(&v->bp1, 900.0f, 1.4f, SR);
    bq_init_bandpass(&v->bp2, 2000.0f, 0.8f, SR);
}

/* Generate block for a voice into interleaved stereo buffer (frames samples)
   f_start..f_end sweep across the block (smooth interpolation)
   t0 is absolute time offset (seconds) for vibrato phase continuity */
static void voice_generate_block(Voice *v, float *out, int frames, double f_start, double f_end, double t0, double block_dur) {
    for (int n = 0; n < frames; ++n) {
        double tt = (double)n / (double)frames; /* 0..1 across block */
        /* use sinusoidal ease */
        double u = 0.5 * (1.0 - cos(M_PI * tt));
        double f = f_start + (f_end - f_start) * u;
        double local_t = t0 + (double)n / SR;
        double vib = v->vibrato_amt * sin(2.0 * M_PI * v->vibrato_rate * local_t);
        double jitter = v->jitter_amt * ((double)(xrng() & 0xFFFF) / 65535.0 - 0.5);
        double f_inst = f * (1.0 + vib + jitter);
        double f2 = f_inst * 2.0; /* second partial ~ octave */
        double inc1_l = 2.0 * M_PI * f_inst / SR;
        double inc1_r = inc1_l * 1.0015;
        double inc2_l = 2.0 * M_PI * f2 / SR;
        double inc2_r = inc2_l * 1.0015;
        float sL = (float)(sin(v->ph1_l) * 0.75 + sin(v->ph2_l) * 0.35);
        float sR = (float)(sin(v->ph1_r) * 0.75 + sin(v->ph2_r) * 0.35);
        v->ph1_l += inc1_l; if (v->ph1_l > 2.0*M_PI) v->ph1_l -= 2.0*M_PI;
        v->ph1_r += inc1_r; if (v->ph1_r > 2.0*M_PI) v->ph1_r -= 2.0*M_PI;
        v->ph2_l += inc2_l; if (v->ph2_l > 2.0*M_PI) v->ph2_l -= 2.0*M_PI;
        v->ph2_r += inc2_r; if (v->ph2_r > 2.0*M_PI) v->ph2_r -= 2.0*M_PI;
        float fl = bq_process(&v->bp1, sL);
        float fr = bq_process(&v->bp2, sR);
        float noise = 0.015f * frand();
        fl = fl * 0.9f + noise;
        fr = fr * 0.9f + noise;
        float amp = v->amp * (0.6f + 0.4f * (1.0f - fabsf(2.0f*(float)u - 1.0f)));
        /* write stereo with slight width mixing */
        float wl = 0.5f + 0.5f * v->width;
        float wr = 0.5f - 0.5f * v->width;
        out[2*n + 0] = fl * amp * wl + fr * amp * (1.0f - wl);
        out[2*n + 1] = fr * amp * wr + fl * amp * (1.0f - wr);
    }
}

/* ---------- Main program ---------- */
int main(int argc, char **argv) {
    const char *device = "default";
    if (argc > 1) device = argv[1];

    /* Open ALSA */
    snd_pcm_t *pcm;
    int rc = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) { fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(rc)); return 1; }

    /* hw params */
    snd_pcm_hw_params_t *hw;
    snd_pcm_hw_params_alloca(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_channels(pcm, hw, CHANNELS);
    unsigned int rate = SR;
    snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0);
    snd_pcm_uframes_t period = PERIOD;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    snd_pcm_uframes_t bufsize = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &bufsize);
    rc = snd_pcm_hw_params(pcm, hw);
    if (rc < 0) { fprintf(stderr, "snd_pcm_hw_params: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); return 1; }
    rc = snd_pcm_prepare(pcm);
    if (rc < 0) { fprintf(stderr, "snd_pcm_prepare: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); return 1; }

    /* allocate audio buffers */
    int frames_block = (int)period;
    float *mix_buf = (float*)malloc(frames_block * CHANNELS * sizeof(float));
    float *v1_buf  = (float*)malloc(frames_block * CHANNELS * sizeof(float));
    float *v2_buf  = (float*)malloc(frames_block * CHANNELS * sizeof(float));
    if (!mix_buf || !v1_buf || !v2_buf) { fprintf(stderr, "malloc failed\n"); return 1; }

    /* initialize voices */
    Voice v1, v2; voice_init(&v1); voice_init(&v2);
    v1.width = 0.32f; v1.amp = 0.95f; v1.vibrato_rate = 5.2f; v1.vibrato_amt = 0.005f;
    v2.width = 0.18f; v2.amp = 0.78f; v2.vibrato_rate = 7.2f; v2.vibrato_amt = 0.012f;

    /* init compressor */
    Comp comp; comp_init(&comp, -6.0f, 4.0f, 8.0f, 120.0f, SR);

    /* init convolution reverbs (two IRs, one for each channel or blended) */
    Conv convL, convR;
    /* choose IRs and init conv structures. For stereo variation, use different IRs for L/R or same */
    if (conv_init(&convL, IR_VEHICLE, IR_VEH_LEN, frames_block) < 0) { fprintf(stderr,"conv init fail\n"); return 1; }
    if (conv_init(&convR, IR_URBAN, IR_URB_LEN, frames_block) < 0) { fprintf(stderr,"conv init fail\n"); conv_free(&convL); return 1; }

    /* reverb send/wet settings */
    const float reverb_send = 0.12f;
    const float reverb_wet = 0.22f;

    fprintf(stderr, "Playing LAPD siren with convolution reverb (stereo, 44.1kHz FLOAT_LE)\n");

    double global_time = 0.0;
    srand((unsigned)time(NULL));

    for (;;) {
        /* choose pattern sequence: Wail -> Yelp -> HiLo (durations and sweep ranges tuned) */

        /* 1) Wail: 8s slow up/down (we process frame blocks) */
        double wail_dur = 8.0;
        int wail_frames = (int)(wail_dur * SR);
        int done = 0;
        while (done < wail_frames) {
            int frames = (done + frames_block <= wail_frames) ? frames_block : (wail_frames - done);

            /* voice 1 sweep center: triangular up-down */
            double block_center_time = global_time + (double)frames / SR * 0.5;
            double tri = fmod(block_center_time, wail_dur);
            double prog = (tri < wail_dur*0.5) ? (tri / (wail_dur*0.5)) : (1.0 - (tri - wail_dur*0.5) / (wail_dur*0.5));
            prog = clampf((float)prog, 0.0f, 1.0f);
            double f1_center = lerpf(420.0, 1500.0, prog);
            double f1_start = f1_center * 0.98, f1_end = f1_center * 1.02;

            /* voice 2: faster subtle movement */
            double f2_center = lerpf(600.0, 1600.0, 0.5 + 0.5 * sin(global_time * 0.25));
            double f2_start = f2_center * 0.96, f2_end = f2_center * 1.04;

            /* generate voices */
            voice_generate_block(&v1, v1_buf, frames, f1_start, f1_end, global_time, (double)frames / SR);
            voice_generate_block(&v2, v2_buf, frames, f2_start, f2_end, global_time, (double)frames / SR);

            /* mix voices and apply mild processing */
            for (int n = 0; n < frames; ++n) {
                float L = v1_buf[2*n + 0] + v2_buf[2*n + 0];
                float R = v1_buf[2*n + 1] + v2_buf[2*n + 1];

                /* soft clip */
                L = softclip(L, 1.5f);
                R = softclip(R, 1.5f);

                /* compression (mono detector) */
                float m = 0.5f * (fabsf(L) + fabsf(R));
                float outmono = comp_process(&comp, m);
                float gain = m > 1e-9f ? outmono / m : 1.0f;
                L *= gain; R *= gain;

                /* dry->wet send for convolution */
                /* store dry mix in mix_buf for writing later after adding wet */
                mix_buf[2*n + 0] = L;
                mix_buf[2*n + 1] = R;
            }

            /* Convolution reverb: process L and R separately (mono-to-mono conv),
               fed by dry mix scaled by send level. Then mix wet back into dry. */
            /* prepare mono buffers for conv input/output */
            float *sendL = (float*)malloc(sizeof(float) * frames);
            float *sendR = (float*)malloc(sizeof(float) * frames);
            float *wetL  = (float*)malloc(sizeof(float) * frames);
            float *wetR  = (float*)malloc(sizeof(float) * frames);
            if (!sendL || !sendR || !wetL || !wetR) {
                fprintf(stderr,"malloc fail conv\n"); goto cleanup;
            }
            for (int n=0;n<frames;++n) {
                sendL[n] = mix_buf[2*n + 0] * reverb_send;
                sendR[n] = mix_buf[2*n + 1] * reverb_send;
            }
            conv_process_block(&convL, sendL, wetL, frames);
            conv_process_block(&convR, sendR, wetR, frames);

            /* Add wet back with mixing */
            for (int n=0;n<frames;++n) {
                float outL = mix_buf[2*n + 0] * (1.0f - reverb_wet) + wetL[n] * reverb_wet;
                float outR = mix_buf[2*n + 1] * (1.0f - reverb_wet) + wetR[n] * reverb_wet;
                mix_buf[2*n + 0] = outL;
                mix_buf[2*n + 1] = outR;
            }

            free(sendL); free(sendR); free(wetL); free(wetR);

            /* write block to ALSA */
            snd_pcm_sframes_t w = snd_pcm_writei(pcm, mix_buf, frames);
            if (w < 0) { w = snd_pcm_recover(pcm, (int)w, 0); if (w < 0) { fprintf(stderr, "write error: %s\n", snd_strerror((int)w)); goto cleanup; } }

            done += frames;
            global_time += (double)frames / SR;
        } /* end wail */

        /* 2) Yelp: rapid bursts ~3.6s (shorter sweeps) */
        double yelp_total = 3.6;
        int yelp_frames = (int)(yelp_total * SR);
        done = 0;
        while (done < yelp_frames) {
            int frames = (done + frames_block <= yelp_frames) ? frames_block : (yelp_frames - done);

            /* choose random center per block for variety */
            double f1_start = 600.0 + (xrng() & 511);
            double f1_end   = f1_start * 1.2;
            double f2_start = 900.0 + (xrng() & 511);
            double f2_end   = f2_start * 1.15;

            voice_generate_block(&v1, v1_buf, frames, f1_start, f1_end, global_time, (double)frames / SR);
            voice_generate_block(&v2, v2_buf, frames, f2_start, f2_end, global_time, (double)frames / SR);

            for (int n=0;n<frames;++n) {
                float L = v1_buf[2*n + 0] + 0.6f * v2_buf[2*n + 0];
                float R = v1_buf[2*n + 1] + 0.6f * v2_buf[2*n + 1];
                L = softclip(L, 1.8f); R = softclip(R, 1.8f);
                float m = 0.5f*(fabsf(L)+fabsf(R));
                float outmono = comp_process(&comp, m);
                float gain = m > 1e-9f ? outmono / m : 1.0f;
                L *= gain; R *= gain;
                mix_buf[2*n+0] = L; mix_buf[2*n+1] = R;
            }

            /* create sends and conv as for Wail */
            float *sendL = (float*)malloc(sizeof(float) * frames);
            float *sendR = (float*)malloc(sizeof(float) * frames);
            float *wetL  = (float*)malloc(sizeof(float) * frames);
            float *wetR  = (float*)malloc(sizeof(float) * frames);
            if (!sendL || !sendR || !wetL || !wetR) { fprintf(stderr,"malloc fail conv\n"); goto cleanup; }
            for (int n=0;n<frames;++n) { sendL[n] = mix_buf[2*n+0] * reverb_send * 1.2f; sendR[n] = mix_buf[2*n+1] * reverb_send * 1.2f; }
            conv_process_block(&convL, sendL, wetL, frames);
            conv_process_block(&convR, sendR, wetR, frames);
            for (int n=0;n<frames;++n) {
                float outL = mix_buf[2*n+0] * (1.0f - reverb_wet) + wetL[n] * reverb_wet;
                float outR = mix_buf[2*n+1] * (1.0f - reverb_wet) + wetR[n] * reverb_wet;
                mix_buf[2*n+0] = outL; mix_buf[2*n+1] = outR;
            }
            free(sendL); free(sendR); free(wetL); free(wetR);

            snd_pcm_sframes_t w = snd_pcm_writei(pcm, mix_buf, frames);
            if (w < 0) { w = snd_pcm_recover(pcm, (int)w, 0); if (w < 0) { fprintf(stderr,"write err: %s\n", snd_strerror((int)w)); goto cleanup; } }

            done += frames;
            global_time += (double)frames / SR;
        }

        /* 3) Hi-Lo alternating tones ~4s */
        double hilo_total = 4.0;
        int hilo_frames = (int)(hilo_total * SR);
        done = 0;
        while (done < hilo_frames) {
            int frames = (done + frames_block <= hilo_frames) ? frames_block : (hilo_frames - done);

            /* determine target frequency depending on segment index (alternating) */
            int seg_idx = ((done / frames_block) % 2);
            double f_target = seg_idx ? 1400.0 : 500.0;
            double f_start = f_target * 0.995; double f_end = f_target * 1.005;

            voice_generate_block(&v1, v1_buf, frames, f_start, f_end, global_time, (double)frames / SR);
            voice_generate_block(&v2, v2_buf, frames, f_start*1.9, f_end*1.9, global_time, (double)frames / SR);

            for (int n=0;n<frames;++n) {
                float L = v1_buf[2*n + 0] + 0.2f * v2_buf[2*n + 0];
                float R = v1_buf[2*n + 1] + 0.2f * v2_buf[2*n + 1];
                L = softclip(L, 1.6f); R = softclip(R, 1.6f);
                float m = 0.5f*(fabsf(L)+fabsf(R));
                float outmono = comp_process(&comp, m);
                float gain = m > 1e-9f ? outmono / m : 1.0f;
                L *= gain; R *= gain;
                mix_buf[2*n+0] = L; mix_buf[2*n+1] = R;
            }

            /* conv send */
            float *sendL = (float*)malloc(sizeof(float) * frames);
            float *sendR = (float*)malloc(sizeof(float) * frames);
            float *wetL  = (float*)malloc(sizeof(float) * frames);
            float *wetR  = (float*)malloc(sizeof(float) * frames);
            if (!sendL || !sendR || !wetL || !wetR) { fprintf(stderr,"malloc fail conv\n"); goto cleanup; }
            for (int n=0;n<frames;++n) { sendL[n] = mix_buf[2*n+0] * reverb_send * 0.9f; sendR[n] = mix_buf[2*n+1] * reverb_send * 0.9f; }
            conv_process_block(&convL, sendL, wetL, frames);
            conv_process_block(&convR, sendR, wetR, frames);
            for (int n=0;n<frames;++n) {
                float outL = mix_buf[2*n+0] * (1.0f - reverb_wet) + wetL[n] * reverb_wet;
                float outR = mix_buf[2*n+1] * (1.0f - reverb_wet) + wetR[n] * reverb_wet;
                mix_buf[2*n+0] = outL; mix_buf[2*n+1] = outR;
            }
            free(sendL); free(sendR); free(wetL); free(wetR);

            snd_pcm_sframes_t w = snd_pcm_writei(pcm, mix_buf, frames);
            if (w < 0) { w = snd_pcm_recover(pcm, (int)w, 0); if (w < 0) { fprintf(stderr,"write err: %s\n", snd_strerror((int)w)); goto cleanup; } }

            done += frames;
            global_time += (double)frames / SR;
        } /* end Hi-Lo */
    } /* end main loop */

cleanup:
    conv_free(&convL);
    conv_free(&convR);
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    return 0;
}

