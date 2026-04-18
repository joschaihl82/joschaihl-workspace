/*
 lapd.c
 - LAPD-CIA-2000 siren (LAPD base values) with:
   * mono DSP pipeline (two-voice synth) -> stereo output via fractional delay
   * pre-synthesis producer thread filling ring buffer to avoid underruns
   * short convolution reverb (built-in vehicle / urban IRs)
   * probabilistic playback: Wail 75% / Yelp 25%; Hi-Lo disabled by default
   * Yelp slowed and second-sweeper bug fixed (second sweeps track primary)
   * LFO/vibrato sanity checks before start
 - Build:
     gcc -O2 -Wall -pthread -o lapd lapd.c -lasound -lm
 - Run:
     ./lapd [optional ALSA device e.g., hw:0,0 or default]
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdint.h>

/* ---------- Constants (LAPD-CIA-2000 base values) ---------- */
#define SR                  44100u
#define CHANNELS            2
#define PERIOD_FRAMES       1024u
#define RING_BLOCKS         64
#define MAX_IR              512

/* Wail / Yelp / Hi-Lo base values */
#define WAIL_F_LO     700.0
#define WAIL_F_HI    1700.0
#define WAIL_DUR      8.0

/* Yelp: made slightly slower per request */
#define YELP_F_MIN    800.0
#define YELP_F_MAX   2000.0
#define YELP_SEG      0.18
#define YELP_TOTAL    3.8

/* Hi-Lo (not used by default) */
#define HILO_LOW      600.0
#define HILO_HIGH    1400.0
#define HILO_SEG      0.30

/* vibrato */
#define VIB_RATE      5.5f
#define VIB_AMT       0.006f
#define YELP_VIB      12.0f
#define YELP_VIB_AMT  0.015f

/* stereo fractional right delay (samples) */
#define RIGHT_DELAY_SAMPLES 1.2f

/* voices */
#define VOICE1_AMP    0.90f
#define VOICE2_AMP    0.70f
#define SECOND_RATIO  2.0

/* reverb */
#define REVERB_SEND   0.12f
#define REVERB_WET    0.22f

/* LFO sanity */
#define MIN_VIB_RATE  0.5f
#define MAX_VIB_RATE 50.0f

/* ---------- Small built-in IRs ---------- */
static const float IR_VEHICLE[] = {
    1.00f,0.30f,0.12f,0.06f,0.03f,0.015f,0.007f,0.0035f,0.0018f,0.0009f,
    0.0005f,0.00025f,0.00012f,0.00006f,0.00003f,0.000015f,0.00025f,0.0004f,
    0.0003f,0.00018f,0.0001f,0.00006f,0.00003f,0.00002f,0.00001f,0.0f
};
static const int IR_VEH_LEN = (int)(sizeof(IR_VEHICLE)/sizeof(IR_VEHICLE[0]));

static const float IR_URBAN[] = {
    0.85f,0.42f,0.26f,0.15f,0.09f,0.055f,0.035f,0.022f,0.014f,0.009f,
    0.006f,0.004f,0.0026f,0.0019f,0.0013f,0.0009f,0.0011f,0.00105f,
    0.0010f,0.00095f,0.0009f,0.00085f,0.0008f,0.00075f,0.0007f,0.00065f,
    0.0006f,0.00055f,0.0005f,0.00045f,0.0004f,0.00035f,0.00032f,0.00030f,
    0.00027f,0.00025f,0.00022f,0.00020f,0.00018f,0.00016f,0.00014f,0.00012f,
    0.00010f,0.00009f,0.00008f,0.00007f,0.00006f,0.00005f,0.00004f,0.000035f,
    0.00003f,0.000028f,0.000025f,0.000022f,0.000020f,0.000018f,0.000016f,0.000015f,
    0.000013f,0.000012f,0.000011f,0.000010f,0.000009f,0.000008f,0.000007f,0.000006f,
    0.0000055f,0.000005f,0.0000045f,0.000004f,0.0000035f,0.000003f,0.0000025f,0.000002f,
    0.0000018f,0.0000016f,0.0000014f,0.0000012f,0.0000010f,0.0f
};
static const int IR_URB_LEN = (int)(sizeof(IR_URBAN)/sizeof(IR_URBAN[0]));

/* ---------- Utilities ---------- */
static inline float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
static inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }
static uint32_t xrng_state = 0x89ABCDEFu;
static inline uint32_t xrng(void) { uint32_t x = xrng_state; x ^= x << 13; x ^= x >> 17; x ^= x << 5; xrng_state = x; return x; }
static inline float frand(void) { return (float)(xrng() & 0xFFFF) / 65535.0f * 2.0f - 1.0f; }

/* ---------- DSP building blocks ---------- */
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
static inline float bq_process(BQ *b, float x) {
    float y = b->b0*x + b->b1*b->x1 + b->b2*b->x2 - b->a1*b->y1 - b->a2*b->y2;
    b->x2=b->x1; b->x1=x; b->y2=b->y1; b->y1=y; return y;
}
static inline float softclip(float x, float drive) {
    x *= drive;
    if (x > 1.5f) x = 1.5f;
    if (x < -1.5f) x = -1.5f;
    float x2 = x*x;
    return (x * (27.0f + x2)) / (27.0f + 9.0f * x2);
}
typedef struct { float env; float thr; float ratio; float atk_coeff, rel_coeff; } Comp;
static void comp_init(Comp *c, float thr_db, float ratio, float atk_ms, float rel_ms, float sr) {
    c->env = 0.0f;
    c->thr = powf(10.0f, thr_db / 20.0f);
    c->ratio = ratio;
    c->atk_coeff = expf(-1.0f / (0.001f * atk_ms * sr));
    c->rel_coeff = expf(-1.0f / (0.001f * rel_ms * sr));
}
static inline float comp_process(Comp *c, float in) {
    float absx = fabsf(in);
    if (absx > c->env) c->env = c->atk_coeff * (c->env - absx) + absx;
    else c->env = c->rel_coeff * (c->env - absx) + absx;
    if (c->env <= c->thr) return in;
    float over = c->env / c->thr;
    float gain = powf(over, (1.0f - 1.0f / c->ratio));
    return in / gain;
}

/* ---------- Convolution (direct overlap-add for short IR) ---------- */
typedef struct {
    const float *ir;
    int ir_len;
    float *delay;
    int delay_size;
    int write_pos;
} Conv;
static int conv_init(Conv *c, const float *ir, int ir_len, int period) {
    if (ir_len <= 0 || ir_len > MAX_IR) return -1;
    c->ir = ir; c->ir_len = ir_len;
    c->delay_size = period + ir_len;
    c->delay = (float*)calloc(c->delay_size, sizeof(float));
    if (!c->delay) return -1;
    c->write_pos = 0; return 0;
}
static void conv_free(Conv *c) { if (c->delay) free(c->delay); c->delay = NULL; }
static void conv_process_block(Conv *c, const float *in, float *out, int N) {
    int M = c->ir_len, D = c->delay_size, wp = c->write_pos;
    for (int n=0;n<N;++n) c->delay[(wp + n) % D] += in[n];
    for (int n=0;n<N;++n) {
        float acc = 0.0f;
        int pos = (wp + n) % D;
        int idx = pos;
        for (int k=0;k<M;++k) {
            acc += c->delay[idx] * c->ir[k];
            if (--idx < 0) idx += D;
        }
        out[n] = acc;
    }
    for (int n=0;n<N;++n) c->delay[(wp + n) % D] = 0.0f;
    c->write_pos = (wp + N) % D;
}

/* ---------- Mono voice generator ---------- */
typedef struct {
    double ph1, ph2;
    float amp, vibrato_rate, vibrato_amt, jitter_amt;
    BQ bp1, bp2;
} VoiceMono;
static void voice_mono_init(VoiceMono *v, float amp, float vib_rate, float vib_amt) {
    v->ph1 = 0.0; v->ph2 = 1.0;
    v->amp = amp; v->vibrato_rate = vib_rate; v->vibrato_amt = vib_amt;
    v->jitter_amt = 0.0015f;
    bq_init_bandpass(&v->bp1, 900.0f, 1.4f, SR);
    bq_init_bandpass(&v->bp2, 2000.0f, 0.8f, SR);
}
static void voice_mono_generate(VoiceMono *v, float *out, int frames, double f_start, double f_end, double t0) {
    for (int n=0;n<frames;++n) {
        double tt = (double)n / (double)frames;
        double u = 0.5 * (1.0 - cos(M_PI * tt));
        double f = f_start + (f_end - f_start) * u;
        double local_t = t0 + (double)n / SR;
        double vib = v->vibrato_amt * sin(2.0 * M_PI * v->vibrato_rate * local_t);
        double jitter = v->jitter_amt * ((double)(xrng() & 0xFFFF)/65535.0 - 0.5);
        double f_inst = f * (1.0 + vib + jitter);
        double f2 = f_inst * SECOND_RATIO;
        double inc1 = 2.0*M_PI * f_inst / SR;
        double inc2 = 2.0*M_PI * f2 / SR;
        float s = (float)(sin(v->ph1) * 0.75 + sin(v->ph2) * 0.35);
        v->ph1 += inc1; if (v->ph1 > 2.0*M_PI) v->ph1 -= 2.0*M_PI;
        v->ph2 += inc2; if (v->ph2 > 2.0*M_PI) v->ph2 -= 2.0*M_PI;
        float shaped = bq_process(&v->bp1, s);
        float noise = 0.015f * frand();
        shaped = shaped * 0.92f + noise;
        float amp = v->amp * (0.6f + 0.4f * (1.0f - fabsf(2.0f*(float)u - 1.0f)));
        out[n] = shaped * amp;
    }
}

/* ---------- Fractional delay for right channel ---------- */
typedef struct {
    float *buf;
    int size;
    int pos;
} DelayLine;
static DelayLine right_delay;
static int delay_init(DelayLine *d, int max_delay_samples) {
    d->size = max_delay_samples + PERIOD_FRAMES + 8;
    d->buf = (float*)calloc(d->size, sizeof(float));
    if (!d->buf) return -1;
    d->pos = 0;
    return 0;
}
static void delay_free(DelayLine *d) { if (d->buf) free(d->buf); d->buf = NULL; }
static inline void delay_write(DelayLine *d, float x) {
    d->buf[d->pos++] = x;
    if (d->pos >= d->size) d->pos = 0;
}
static inline float delay_read_frac(DelayLine *d, float delay_samples) {
    float read_pos = (float)d->pos - delay_samples;
    while (read_pos < 0.0f) read_pos += d->size;
    while (read_pos >= d->size) read_pos -= d->size;
    int i0 = (int)floorf(read_pos);
    int i1 = i0 + 1; if (i1 >= d->size) i1 -= d->size;
    float frac = read_pos - (float)i0;
    float s0 = d->buf[i0];
    float s1 = d->buf[i1];
    return s0 * (1.0f - frac) + s1 * frac;
}

/* ---------- Ring buffer for blocks ---------- */
typedef struct {
    float *blocks;
    int capacity;
    int frames_per_block;
    int head;
    int tail;
    int count;
    pthread_mutex_t mtx;
    pthread_cond_t nonempty;
    pthread_cond_t nonfull;
    int stop;
} BlockRing;
static BlockRing ring;
static int ring_init(BlockRing *r, int capacity, int frames_per_block) {
    r->capacity = capacity; r->frames_per_block = frames_per_block;
    r->blocks = (float*)calloc((size_t)capacity * frames_per_block * CHANNELS, sizeof(float));
    if (!r->blocks) return -1;
    r->head = r->tail = r->count = 0;
    pthread_mutex_init(&r->mtx, NULL);
    pthread_cond_init(&r->nonempty, NULL);
    pthread_cond_init(&r->nonfull, NULL);
    r->stop = 0;
    return 0;
}
static void ring_free(BlockRing *r) {
    if (!r) return;
    free(r->blocks); r->blocks=NULL;
    pthread_mutex_destroy(&r->mtx);
    pthread_cond_destroy(&r->nonempty);
    pthread_cond_destroy(&r->nonfull);
}
static inline float *ring_write_ptr(BlockRing *r) { return r->blocks + (size_t)r->tail * r->frames_per_block * CHANNELS; }
static inline float *ring_read_ptr(BlockRing *r) { return r->blocks + (size_t)r->head * r->frames_per_block * CHANNELS; }

/* ---------- Globals ---------- */
static snd_pcm_t *pcm = NULL;
static VoiceMono vm1, vm2;
static Comp comp;
static Conv convL, convR;

/* ---------- LFO sanity ---------- */
static int check_lfo_sanity(float vib_rate, unsigned period_frames, unsigned sr) {
    if (!(vib_rate >= MIN_VIB_RATE && vib_rate <= MAX_VIB_RATE)) return 0;
    float nyq_limit = (float)sr * 0.25f;
    if (vib_rate >= nyq_limit) return 0;
    return 1;
}

/* ---------- Producer thread: mono synth -> conv -> stereo block -> ring ---------- */
static void *producer(void *arg) {
    (void)arg;
    int frames_block = PERIOD_FRAMES;
    float *mono_v1 = (float*)malloc(frames_block * sizeof(float));
    float *mono_v2 = (float*)malloc(frames_block * sizeof(float));
    float *mono_mix = (float*)malloc(frames_block * sizeof(float));
    float *conv_outL = (float*)malloc(frames_block * sizeof(float));
    float *conv_outR = (float*)malloc(frames_block * sizeof(float));
    float *stereo_block = (float*)malloc(frames_block * CHANNELS * sizeof(float));
    if (!mono_v1 || !mono_v2 || !mono_mix || !conv_outL || !conv_outR || !stereo_block) {
        fprintf(stderr,"producer malloc fail\n");
        ring.stop = 1; pthread_cond_signal(&ring.nonempty); return NULL;
    }

    if (delay_init(&right_delay, (int)ceilf(RIGHT_DELAY_SAMPLES) + 4) < 0) {
        fprintf(stderr,"delay init fail\n"); ring.stop = 1; pthread_cond_signal(&ring.nonempty); return NULL;
    }

    double global_time = 0.0;
    int state = 0; /* 0=Wail, 1=Yelp */
    int state_blocks = 0;
    int last_was_yelp = 0;

    for (;;) {
        pthread_mutex_lock(&ring.mtx);
        while (ring.count >= ring.capacity && !ring.stop) pthread_cond_wait(&ring.nonfull, &ring.mtx);
        if (ring.stop) { pthread_mutex_unlock(&ring.mtx); break; }
        float *out_ptr = ring_write_ptr(&ring);
        pthread_mutex_unlock(&ring.mtx);

        /* If cycle boundary, pick next cycle: Yelp 25%, Wail 75% */
        if (state_blocks <= 0) {
            float r = (float)(xrng() & 0xFFFF) / 65535.0f;
            if (!last_was_yelp && r < 0.25f) {
                state = 1; /* Yelp */
                state_blocks = (int)((YELP_TOTAL * SR) / frames_block);
                last_was_yelp = 1;
            } else {
                state = 0; /* Wail */
                state_blocks = (int)((WAIL_DUR * SR) / frames_block);
                last_was_yelp = 0;
            }
        }

        if (state == 0) {
            /* Wail: slow triangular up-down */
            double center_time = global_time + (double)frames_block / SR * 0.5;
            double tri = fmod(center_time, WAIL_DUR);
            double prog = (tri < WAIL_DUR*0.5) ? (tri / (WAIL_DUR*0.5)) : (1.0 - (tri - WAIL_DUR*0.5) / (WAIL_DUR*0.5));
            prog = clampf((float)prog, 0.0f, 1.0f);
            double f_center = lerpf(WAIL_F_LO, WAIL_F_HI, prog);
            double f1s = f_center * 0.98, f1e = f_center * 1.02;
            double f2_center = lerpf(600.0, 1600.0, 0.5 + 0.5 * sin(global_time * 0.25));
            double f2s = f2_center * 0.96, f2e = f2_center * 1.04;
            voice_mono_generate(&vm1, mono_v1, frames_block, f1s, f1e, global_time);
            voice_mono_generate(&vm2, mono_v2, frames_block, f2s, f2e, global_time);
        } else {
            /* Yelp: slower; second sweeper follows proportionally */
            double center = YELP_F_MIN + (xrng() % (int)(YELP_F_MAX - YELP_F_MIN + 1));
            double f1s = center * 0.85, f1e = center * 1.25;
            double f2s = f1s * SECOND_RATIO * 0.95;
            double f2e = f1e * SECOND_RATIO * 1.05;
            /* temporary vibrato overrides */
            float save1_vib = vm1.vibrato_amt, save1_rate = vm1.vibrato_rate;
            float save2_vib = vm2.vibrato_amt, save2_rate = vm2.vibrato_rate;
            vm1.vibrato_amt = VIB_AMT; vm1.vibrato_rate = VIB_RATE;
            vm2.vibrato_amt = YELP_VIB_AMT; vm2.vibrato_rate = YELP_VIB;
            voice_mono_generate(&vm1, mono_v1, frames_block, f1s, f1e, global_time);
            voice_mono_generate(&vm2, mono_v2, frames_block, f2s, f2e, global_time);
            vm1.vibrato_amt = save1_vib; vm1.vibrato_rate = save1_rate;
            vm2.vibrato_amt = save2_vib; vm2.vibrato_rate = save2_rate;
        }

        /* mix mono voices, saturate and compress */
        for (int n=0;n<frames_block;++n) {
            float m = mono_v1[n] * VOICE1_AMP + mono_v2[n] * VOICE2_AMP;
            float clip = softclip(m, 1.6f);
            float env_in = fabsf(clip);
            float outm = comp_process(&comp, env_in);
            float gain = (env_in > 1e-9f) ? (outm / env_in) : 1.0f;
            mono_mix[n] = clip * gain;
        }

        /* convolve mono send into left & right IR engines */
        float *send = (float*)malloc(frames_block * sizeof(float));
        if (!send) { fprintf(stderr,"alloc send fail\n"); ring.stop = 1; break; }
        for (int n=0;n<frames_block;++n) send[n] = mono_mix[n] * REVERB_SEND;
        conv_process_block(&convL, send, conv_outL, frames_block);
        conv_process_block(&convR, send, conv_outR, frames_block);
        free(send);

        /* produce stereo block: left uses mono directly; right uses fractional delay of mono */
        for (int n=0;n<frames_block;++n) {
            float left_sample = mono_mix[n] * (1.0f - REVERB_WET) + conv_outL[n] * REVERB_WET;
            delay_write(&right_delay, mono_mix[n]);
            float delayed = delay_read_frac(&right_delay, RIGHT_DELAY_SAMPLES);
            float right_sample = delayed * (1.0f - REVERB_WET) + conv_outR[n] * REVERB_WET;
            stereo_block[2*n + 0] = left_sample;
            stereo_block[2*n + 1] = right_sample;
        }

        /* copy into ring */
        memcpy(out_ptr, stereo_block, frames_block * CHANNELS * sizeof(float));

        global_time += (double)frames_block / SR;
        state_blocks--;
        if (state_blocks <= 0) {
            /* nothing here; next loop will choose probabilistically */
        }

        pthread_mutex_lock(&ring.mtx);
        ring.tail = (ring.tail + 1) % ring.capacity;
        ring.count++;
        pthread_cond_signal(&ring.nonempty);
        pthread_mutex_unlock(&ring.mtx);
    }

    delay_free(&right_delay);
    free(mono_v1); free(mono_v2); free(mono_mix); free(conv_outL); free(conv_outR); free(stereo_block);
    return NULL;
}

/* ---------- Main: setup, start producer, consumer writes to ALSA ---------- */
int main(int argc, char **argv) {
    const char *device = "default";
    if (argc > 1) device = argv[1];

    /* LFO sanity checks */
    if (!check_lfo_sanity(VIB_RATE, PERIOD_FRAMES, SR)) {
        fprintf(stderr, "Vibrato rate (%g Hz) out of sane range\n", (double)VIB_RATE);
        return 1;
    }
    if (!check_lfo_sanity(YELP_VIB, PERIOD_FRAMES, SR)) {
        fprintf(stderr, "Yelp vibrato rate (%g Hz) out of sane range\n", (double)YELP_VIB);
        return 1;
    }

    /* open ALSA */
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
    snd_pcm_uframes_t period = PERIOD_FRAMES;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    snd_pcm_uframes_t bufsize = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &bufsize);
    rc = snd_pcm_hw_params(pcm, hw);
    if (rc < 0) { fprintf(stderr, "snd_pcm_hw_params: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); return 1; }
    rc = snd_pcm_prepare(pcm);
    if (rc < 0) { fprintf(stderr, "snd_pcm_prepare: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); return 1; }

    /* init DSP */
    voice_mono_init(&vm1, VOICE1_AMP, VIB_RATE, VIB_AMT);
    voice_mono_init(&vm2, VOICE2_AMP, YELP_VIB, YELP_VIB_AMT);
    comp_init(&comp, -6.0f, 4.0f, 8.0f, 120.0f, SR);
    if (conv_init(&convL, IR_VEHICLE, IR_VEH_LEN, PERIOD_FRAMES) < 0) { fprintf(stderr,"convL init fail\n"); goto fail; }
    if (conv_init(&convR, IR_URBAN, IR_URB_LEN, PERIOD_FRAMES) < 0) { fprintf(stderr,"convR init fail\n"); conv_free(&convL); goto fail; }

    if (ring_init(&ring, RING_BLOCKS, PERIOD_FRAMES) < 0) { fprintf(stderr,"ring init fail\n"); goto fail_conv; }

    /* start producer thread */
    pthread_t prod;
    if (pthread_create(&prod, NULL, producer, NULL) != 0) { fprintf(stderr,"producer create fail\n"); goto fail_ring; }

    /* prefill ring (quarter) to avoid initial underrun */
    int prefill = RING_BLOCKS / 4;
    pthread_mutex_lock(&ring.mtx);
    while (ring.count < prefill && !ring.stop) pthread_cond_wait(&ring.nonempty, &ring.mtx);
    pthread_mutex_unlock(&ring.mtx);

    fprintf(stderr, "Playing LAPD-CIA-2000 siren (mono processing -> stereo out)\n");

    /* consumer loop */
    float *local_block = (float*)malloc(PERIOD_FRAMES * CHANNELS * sizeof(float));
    if (!local_block) { fprintf(stderr,"consumer malloc fail\n"); goto stop_prod; }

    for (;;) {
        pthread_mutex_lock(&ring.mtx);
        while (ring.count == 0 && !ring.stop) pthread_cond_wait(&ring.nonempty, &ring.mtx);
        if (ring.stop && ring.count == 0) { pthread_mutex_unlock(&ring.mtx); break; }
        float *rp = ring_read_ptr(&ring);
        memcpy(local_block, rp, PERIOD_FRAMES * CHANNELS * sizeof(float));
        ring.head = (ring.head + 1) % ring.capacity;
        ring.count--;
        pthread_cond_signal(&ring.nonfull);
        pthread_mutex_unlock(&ring.mtx);

        snd_pcm_sframes_t w = snd_pcm_writei(pcm, local_block, PERIOD_FRAMES);
        if (w < 0) {
            w = snd_pcm_recover(pcm, (int)w, 0);
            if (w < 0) { fprintf(stderr,"snd_pcm_writei error: %s\n", snd_strerror((int)w)); break; }
        }
    }

    free(local_block);

stop_prod:
    pthread_mutex_lock(&ring.mtx); ring.stop = 1; pthread_cond_signal(&ring.nonfull); pthread_cond_signal(&ring.nonempty); pthread_mutex_unlock(&ring.mtx);
    pthread_join(prod, NULL);

    ring_free(&ring);
    conv_free(&convL); conv_free(&convR);
    snd_pcm_drain(pcm);
fail_ring:
fail_conv:
    ;
fail:
    snd_pcm_close(pcm);
    return 0;
}

