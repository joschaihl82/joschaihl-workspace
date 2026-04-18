/*
lapd_conv_presynth.c
- Realistic LAPD siren + short convolution reverb (vehicle/urban IRs)
- Pre-synthesis producer thread fills a ring of audio blocks to avoid underruns
- Uses libasound (FLOAT_LE stereo, 44.1 kHz)
- Build: gcc -O2 -Wall -pthread -o lapd_conv_presynth lapd_conv_presynth.c -lasound -lm
- Run:   ./lapd_conv_presynth [optional: device e.g., default or hw:0,0]
Notes:
- Keep IRs short (embedded) for real-time direct convolution.
- Producer generates blocks continuously and wakes consumer when enough data.
- Consumer writes to ALSA from ring; if ring empty it waits (shouldn't under normal load).
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <pthread.h>

/* ---------- Config ---------- */
#define SR          44100u
#define CHANNELS    2
#define PERIOD      1024u               /* frames per block */
#define RING_SIZE   64                  /* number of blocks in ring (must be > latency) */
#define MAX_IR      512

/* ---------- Small IRs (short vehicle/urban approximations) ---------- */
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
static unsigned int xrng_state = 0x89ABCDEFu;
static unsigned int xrng(void) { unsigned int x = xrng_state; x ^= x << 13; x ^= x >> 17; x ^= x << 5; return xrng_state = x; }
static float frand(void) { return (float)(xrng() & 0xFFFF) / 65535.0f * 2.0f - 1.0f; }

/* ---------- Biquad bandpass ---------- */
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

/* ---------- Soft clip and compressor (simple) ---------- */
static float softclip(float x, float drive) {
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
static float comp_process(Comp *c, float in) {
    float absx = fabsf(in);
    if (absx > c->env) c->env = c->atk_coeff * (c->env - absx) + absx;
    else c->env = c->rel_coeff * (c->env - absx) + absx;
    if (c->env <= c->thr) return in;
    float over = c->env / c->thr;
    float gain = powf(over, (1.0f - 1.0f / c->ratio));
    return in / gain;
}

/* ---------- Convolution (short IR) ---------- */
typedef struct {
    const float *ir;
    int ir_len;
    float *delay;   /* circular buffer for overlap-add */
    int delay_size;
    int write_pos;
} Conv;
static int conv_init(Conv *c, const float *ir, int ir_len, int period) {
    if (ir_len <= 0 || ir_len > MAX_IR) return -1;
    c->ir = ir; c->ir_len = ir_len;
    c->delay_size = period + ir_len;
    c->delay = (float*)calloc(c->delay_size, sizeof(float));
    if (!c->delay) return -1;
    c->write_pos = 0;
    return 0;
}
static void conv_free(Conv *c) { if (c->delay) free(c->delay); c->delay=NULL; }
static void conv_process_block(Conv *c, const float *in, float *out, int N) {
    int M = c->ir_len, D = c->delay_size, wp = c->write_pos;
    /* accumulate input into delay */
    for (int n=0;n<N;++n) c->delay[(wp + n) % D] += in[n];
    /* compute output via convolution (direct) */
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
    /* clear used delay samples */
    for (int n=0;n<N;++n) c->delay[(wp + n) % D] = 0.0f;
    c->write_pos = (wp + N) % D;
}

/* ---------- Voice generator (compact, two partials) ---------- */
typedef struct {
    double ph1_l, ph1_r, ph2_l, ph2_r;
    float width, amp, vibrato_rate, vibrato_amt, jitter_amt;
    BQ bp1, bp2;
} Voice;
static void voice_init(Voice *v) {
    v->ph1_l=0.0; v->ph1_r=0.31; v->ph2_l=1.0; v->ph2_r=1.18;
    v->width=0.25f; v->amp=0.9f; v->vibrato_rate=5.6f; v->vibrato_amt=0.004f; v->jitter_amt=0.0015f;
    bq_init_bandpass(&v->bp1, 900.0f, 1.4f, SR);
    bq_init_bandpass(&v->bp2, 2000.0f, 0.8f, SR);
}
static void voice_generate_block(Voice *v, float *out, int frames, double f_start, double f_end, double t0) {
    for (int n=0;n<frames;++n) {
        double tt = (double)n / (double)frames;
        double u = 0.5 * (1.0 - cos(M_PI * tt));
        double f = f_start + (f_end - f_start) * u;
        double local_t = t0 + (double)n / SR;
        double vib = v->vibrato_amt * sin(2.0*M_PI * v->vibrato_rate * local_t);
        double jitter = v->jitter_amt * ((double)(xrng() & 0xFFFF)/65535.0 - 0.5);
        double f_inst = f * (1.0 + vib + jitter);
        double f2 = f_inst * 2.0;
        double inc1_l = 2.0*M_PI * f_inst / SR;
        double inc1_r = inc1_l * 1.0015;
        double inc2_l = 2.0*M_PI * f2 / SR;
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
        fl = fl * 0.9f + noise; fr = fr * 0.9f + noise;
        float amp = v->amp * (0.6f + 0.4f * (1.0f - fabsf(2.0f*(float)u - 1.0f)));
        float wl = 0.5f + 0.5f * v->width, wr = 0.5f - 0.5f * v->width;
        out[2*n + 0] = fl * amp * wl + fr * amp * (1.0f - wl);
        out[2*n + 1] = fr * amp * wr + fl * amp * (1.0f - wr);
    }
}

/* ---------- Ring buffer for blocks ---------- */
typedef struct {
    float *blocks;          /* contiguous memory: RING_SIZE * PERIOD * CHANNELS */
    int capacity;           /* number of blocks (RING_SIZE) */
    int frames_per_block;   /* PERIOD */
    int head;               /* next block to read by consumer */
    int tail;               /* next block to write by producer */
    int count;              /* blocks filled */
    pthread_mutex_t mtx;
    pthread_cond_t  nonempty;
    pthread_cond_t  nonfull;
    int stop;
} BlockRing;

static int ring_init(BlockRing *r, int capacity, int frames_per_block) {
    r->capacity = capacity;
    r->frames_per_block = frames_per_block;
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
    free(r->blocks); r->blocks = NULL;
    pthread_mutex_destroy(&r->mtx);
    pthread_cond_destroy(&r->nonempty);
    pthread_cond_destroy(&r->nonfull);
}
static float* ring_get_write_ptr(BlockRing *r) {
    return r->blocks + (size_t)r->tail * r->frames_per_block * CHANNELS;
}
static float* ring_get_read_ptr(BlockRing *r) {
    return r->blocks + (size_t)r->head * r->frames_per_block * CHANNELS;
}

/* ---------- Global shared state ---------- */
static BlockRing ring;
static snd_pcm_t *pcm = NULL;

/* DSP objects used by producer */
static Voice v1, v2;
static Comp comp;
static Conv convL, convR;
static const float reverb_send = 0.12f;
static const float reverb_wet = 0.22f;

/* ---------- Producer thread: continuously synthesize blocks and fill ring ---------- */
static void* producer_thread(void *arg) {
    (void)arg;
    double global_time = 0.0;
    int sr = SR;
    int frames_block = PERIOD;

    float *v1_buf = (float*)malloc(frames_block * CHANNELS * sizeof(float));
    float *v2_buf = (float*)malloc(frames_block * CHANNELS * sizeof(float));
    float *mix_buf = (float*)malloc(frames_block * CHANNELS * sizeof(float));
    float *sendL = (float*)malloc(frames_block * sizeof(float));
    float *sendR = (float*)malloc(frames_block * sizeof(float));
    float *wetL = (float*)malloc(frames_block * sizeof(float));
    float *wetR = (float*)malloc(frames_block * sizeof(float));
    if (!v1_buf || !v2_buf || !mix_buf || !sendL || !sendR || !wetL || !wetR) {
        fprintf(stderr, "producer malloc fail\n"); ring.stop = 1; pthread_cond_signal(&ring.nonempty); return NULL;
    }

    srand((unsigned)time(NULL));

    for (;;) {
        /* stop requested? */
        pthread_mutex_lock(&ring.mtx);
        while (ring.count >= ring.capacity && !ring.stop) {
            pthread_cond_wait(&ring.nonfull, &ring.mtx);
        }
        if (ring.stop) { pthread_mutex_unlock(&ring.mtx); break; }
        /* get write pointer */
        float *out_block = ring_get_write_ptr(&ring);
        pthread_mutex_unlock(&ring.mtx);

        /* Generate a block according to current pattern sequence.
           We'll follow the same Wail->Yelp->HiLo cycle as before but produce continuously:
           For simplicity, generate blocks in a repeating sequence pattern with durations measured in blocks.
        */

        /* Here we implement a simple state machine for patterns */
        static int state = 0; /* 0=wail,1=yelp,2=hilo */
        static int state_remaining_blocks = 0;
        if (state_remaining_blocks <= 0) {
            if (state == 0) { /* start Wail: 8s */
                state = 0; state_remaining_blocks = (int)((8.0 * SR) / frames_block);
            } else if (state == 1) { /* start Yelp: 3.6s */
                state = 1; state_remaining_blocks = (int)((3.6 * SR) / frames_block);
            } else { /* HiLo: 4s */
                state = 2; state_remaining_blocks = (int)((4.0 * SR) / frames_block);
            }
            /* advance to next after setting */
            /* rotate state sequence wail->yelp->hilo */
            static int seq = 0;
            seq = (seq + 1) % 3;
            state = seq;
            if (state == 0) state_remaining_blocks = (int)((8.0 * SR) / frames_block);
            if (state == 1) state_remaining_blocks = (int)((3.6 * SR) / frames_block);
            if (state == 2) state_remaining_blocks = (int)((4.0 * SR) / frames_block);
        }

        /* per-block generation based on state */
        int frames = frames_block;
        if (state == 0) {
            /* Wail: triangular slow sweep */
            double block_center_time = global_time + (double)frames / SR * 0.5;
            double tri = fmod(block_center_time, 8.0);
            double prog = (tri < 4.0) ? (tri / 4.0) : (1.0 - (tri - 4.0) / 4.0);
            prog = clampf((float)prog, 0.0f, 1.0f);
            double f1_center = lerpf(420.0, 1500.0, prog);
            double f1_start = f1_center * 0.98, f1_end = f1_center * 1.02;
            double f2_center = lerpf(600.0, 1600.0, 0.5 + 0.5 * sin(global_time * 0.25));
            double f2_start = f2_center * 0.96, f2_end = f2_center * 1.04;
            voice_generate_block(&v1, v1_buf, frames, f1_start, f1_end, global_time);
            voice_generate_block(&v2, v2_buf, frames, f2_start, f2_end, global_time);
        } else if (state == 1) {
            /* Yelp: faster, randomized centers */
            double f1_start = 600.0 + (xrng() & 511);
            double f1_end   = f1_start * 1.2;
            double f2_start = 900.0 + (xrng() & 511);
            double f2_end   = f2_start * 1.15;
            voice_generate_block(&v1, v1_buf, frames, f1_start, f1_end, global_time);
            voice_generate_block(&v2, v2_buf, frames, f2_start, f2_end, global_time);
        } else {
            /* Hi-Lo: alternate segments by block index */
            static int hilo_toggle = 0;
            double f_target = hilo_toggle ? 1400.0 : 500.0;
            hilo_toggle ^= 1;
            double f_start = f_target * 0.995, f_end = f_target * 1.005;
            voice_generate_block(&v1, v1_buf, frames, f_start, f_end, global_time);
            voice_generate_block(&v2, v2_buf, frames, f_start*1.9, f_end*1.9, global_time);
        }

        /* mix voices, saturate, compress, build sends */
        for (int n=0;n<frames;++n) {
            float L = v1_buf[2*n + 0] + v2_buf[2*n + 0];
            float R = v1_buf[2*n + 1] + v2_buf[2*n + 1];
            L = softclip(L, 1.6f); R = softclip(R, 1.6f);
            float m = 0.5f * (fabsf(L) + fabsf(R));
            float outmono = comp_process(&comp, m);
            float gain = m > 1e-9f ? outmono / m : 1.0f;
            L *= gain; R *= gain;
            mix_buf[2*n + 0] = L;
            mix_buf[2*n + 1] = R;
            sendL[n] = mix_buf[2*n + 0] * reverb_send;
            sendR[n] = mix_buf[2*n + 1] * reverb_send;
        }

        /* convolution per channel */
        conv_process_block(&convL, sendL, wetL, frames);
        conv_process_block(&convR, sendR, wetR, frames);

        /* mix wet and write into out_block */
        for (int n=0;n<frames;++n) {
            float outL = mix_buf[2*n + 0] * (1.0f - reverb_wet) + wetL[n] * reverb_wet;
            float outR = mix_buf[2*n + 1] * (1.0f - reverb_wet) + wetR[n] * reverb_wet;
            out_block[2*n + 0] = outL;
            out_block[2*n + 1] = outR;
        }

        /* advance global time and state */
        global_time += (double)frames / SR;
        state_remaining_blocks--;

        /* commit: update ring indexes under lock and signal consumer */
        pthread_mutex_lock(&ring.mtx);
        ring.tail = (ring.tail + 1) % ring.capacity;
        ring.count++;
        pthread_cond_signal(&ring.nonempty);
        pthread_mutex_unlock(&ring.mtx);
    } /* loop */

    free(v1_buf); free(v2_buf); free(mix_buf); free(sendL); free(sendR); free(wetL); free(wetR);
    return NULL;
}

/* ---------- Consumer (main) writes blocks to ALSA from ring ---------- */
int main(int argc, char **argv) {
    const char *device = "default";
    if (argc > 1) device = argv[1];

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
    snd_pcm_uframes_t period = PERIOD;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    snd_pcm_uframes_t bufsize = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &bufsize);
    rc = snd_pcm_hw_params(pcm, hw);
    if (rc < 0) { fprintf(stderr, "snd_pcm_hw_params: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); return 1; }
    rc = snd_pcm_prepare(pcm);
    if (rc < 0) { fprintf(stderr, "snd_pcm_prepare: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); return 1; }

    /* init DSP objects */
    voice_init(&v1); voice_init(&v2);
    v1.width = 0.32f; v1.amp = 0.95f; v1.vibrato_amt = 0.005f;
    v2.width = 0.18f; v2.amp = 0.78f; v2.vibrato_amt = 0.012f;
    comp_init(&comp, -6.0f, 4.0f, 8.0f, 120.0f, SR);
    if (conv_init(&convL, IR_VEHICLE, IR_VEH_LEN, PERIOD) < 0) { fprintf(stderr,"convL init fail\n"); goto fail; }
    if (conv_init(&convR, IR_URBAN, IR_URB_LEN, PERIOD) < 0) { fprintf(stderr,"convR init fail\n"); conv_free(&convL); goto fail; }

    /* init ring buffer */
    if (ring_init(&ring, RING_SIZE, PERIOD) < 0) { fprintf(stderr,"ring init fail\n"); goto fail_conv; }

    /* pre-fill ring to target prebuffer blocks to avoid immediate underrun */
    int prefill_blocks = RING_SIZE / 4; /* produce a quarter of ring before starting consumer */
    pthread_t prod;
    if (pthread_create(&prod, NULL, producer_thread, NULL) != 0) { fprintf(stderr,"thread create fail\n"); goto fail_ring; }

    /* wait until at least prefill_blocks are available or producer signals stop */
    pthread_mutex_lock(&ring.mtx);
    while (ring.count < prefill_blocks && !ring.stop) {
        pthread_cond_wait(&ring.nonempty, &ring.mtx);
    }
    pthread_mutex_unlock(&ring.mtx);

    fprintf(stderr, "Starting playback with prefilled %d blocks (period=%u frames)\n", ring.count, PERIOD);

    /* consumer main loop: read blocks and write to ALSA */
    for (;;) {
        pthread_mutex_lock(&ring.mtx);
        while (ring.count == 0 && !ring.stop) {
            pthread_cond_wait(&ring.nonempty, &ring.mtx);
        }
        if (ring.stop && ring.count == 0) { pthread_mutex_unlock(&ring.mtx); break; }
        /* copy block locally to avoid keeping lock while writing to ALSA */
        float *read_ptr = ring_get_read_ptr(&ring);
        float *local_block = (float*)malloc(PERIOD * CHANNELS * sizeof(float));
        if (!local_block) { pthread_mutex_unlock(&ring.mtx); fprintf(stderr,"malloc fail consumer\n"); break; }
        memcpy(local_block, read_ptr, PERIOD * CHANNELS * sizeof(float));
        ring.head = (ring.head + 1) % ring.capacity;
        ring.count--;
        pthread_cond_signal(&ring.nonfull);
        pthread_mutex_unlock(&ring.mtx);

        /* write to ALSA */
        snd_pcm_sframes_t w = snd_pcm_writei(pcm, local_block, PERIOD);
        if (w < 0) {
            w = snd_pcm_recover(pcm, (int)w, 0);
            if (w < 0) { fprintf(stderr,"snd_pcm_writei error: %s\n", snd_strerror((int)w)); free(local_block); break; }
        }
        free(local_block);
    }

    /* stop producer and cleanup */
    pthread_mutex_lock(&ring.mtx); ring.stop = 1; pthread_cond_signal(&ring.nonfull); pthread_cond_signal(&ring.nonempty); pthread_mutex_unlock(&ring.mtx);
    pthread_join(prod, NULL);

    /* drain and cleanup */
    snd_pcm_drain(pcm);

fail_ring:
    ring_free(&ring);
fail_conv:
    conv_free(&convL); conv_free(&convR);
fail:
    snd_pcm_close(pcm);
    return 0;
}

