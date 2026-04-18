/*
 lapd_render_and_play.c
 - Renders the LAPD SS2000 A-minor wail (stereo identical channels) to a WAV file,
   then plays the rendered WAV through ALSA immediately after rendering.
 - Usage:
     ./lapd_render_and_play [duration_seconds] [optional: alsa_device]
   Example:
     ./lapd_render_and_play 30 default
 - Build:
     gcc -O2 -Wall -pthread -o lapd_render_and_play lapd_render_and_play.c -lasound -lm
 - Notes:
   * Renders interleaved 16-bit PCM stereo WAV (44100 Hz).
   * Then opens ALSA playback device and streams the WAV file.
   * No external tools required.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <pthread.h>

/* Audio / render settings */
#define SR               44100
#define CHANNELS         2          /* stereo output identical on both channels */
#define PERIOD_FRAMES    1024
#define RING_BLOCKS      32
#define MAX_IR           512

/* Siren / SS2000 A=880 / A-minor settings */
#define WAIL_F_LO        700.0
#define WAIL_F_HI        (880.0 * 2.0) /* 1760 Hz */
#define WAIL_CYCLE       8.0
#define PARTIAL_RATIO    2.0
#define VIB_RATE         5.5f
#define VIB_DEPTH        0.006f
#define JITTER           0.0012f
#define AMP              0.92f

/* Reverb (short mono IR) */
static const float IR_VEHICLE[] = { 1.0f,0.28f,0.11f,0.05f,0.02f,0.01f,0.004f,0.002f,0.0009f,0.0004f,0.0f };
static const int IR_VEHICLE_LEN = (int)(sizeof(IR_VEHICLE)/sizeof(IR_VEHICLE[0]));

/* A minor degrees relative semitones from A */
static const int A_MINOR_DEGREES[] = { 0,2,3,5,7,8,10 };
static const int A_MINOR_DEGREE_COUNT = sizeof(A_MINOR_DEGREES)/sizeof(A_MINOR_DEGREES[0]);
#define A_REF_FREQ 880.0

/* small RNG */
static uint32_t rng_state = 0x89ABCDEFu;
static inline uint32_t xrng(void){ uint32_t x = rng_state; x ^= x<<13; x ^= x>>17; x ^= x<<5; rng_state = x; return x; }
static inline float frand(void){ return (float)(xrng() & 0xFFFF) / 65535.0f * 2.0f - 1.0f; }

/* Simple band-limited oscillator + coloring (kept compact) */
typedef struct { double ph1, ph2; float amp; float vib_rate, vib_amt; float bp_z; } Voice;
static void voice_init(Voice *v, float amp, float vib_rate, float vib_amt){
    v->ph1 = 0.0; v->ph2 = 1.0; v->amp = amp; v->vib_rate = vib_rate; v->vib_amt = vib_amt; v->bp_z = 0.0f;
}
static inline float onepole(float *z, float x, float a){ float y = a * x + (1.0f - a) * (*z); *z = y; return y; }

/* Quantize frequency to nearest A-minor tone (MIDI centered on A_REF_FREQ) */
static inline double note_to_freq(int m){ return A_REF_FREQ * pow(2.0, (m - 69) / 12.0); }
static double quantize_to_A_minor(double freq){
    if (freq <= 0.0) return freq;
    double midi = 69.0 + 12.0 * log2(freq / A_REF_FREQ);
    int center = (int)floor(midi + 0.5);
    double best = 1e9; int bestm = center;
    for (int d = -36; d <= 36; ++d){
        int m = center + d;
        if (m < 0 || m > 127) continue;
        int rel = (m - 69) % 12; if (rel < 0) rel += 12;
        int ok = 0;
        for (int j=0;j<A_MINOR_DEGREE_COUNT;++j) if (rel == (A_MINOR_DEGREES[j] % 12)) { ok = 1; break; }
        if (!ok) continue;
        double f = note_to_freq(m);
        double diff = fabs(f - freq);
        if (diff < best){ best = diff; bestm = m; }
    }
    return note_to_freq(bestm);
}

/* Convolution (direct overlap-add for short IR) - mono */
typedef struct { const float *ir; int ir_len; float *delay; int dsize; int wpos; } Conv;
static int conv_init(Conv *c, const float *ir, int ir_len, int block){
    if (ir_len <= 0 || ir_len > MAX_IR) return -1;
    c->ir = ir; c->ir_len = ir_len; c->dsize = block + ir_len;
    c->delay = (float*)calloc(c->dsize, sizeof(float)); if(!c->delay) return -1; c->wpos = 0; return 0;
}
static void conv_free(Conv *c){ if(c->delay) free(c->delay); c->delay = NULL; }
static void conv_block(Conv *c, const float *in, float *out, int N){
    int M = c->ir_len, D = c->dsize, wp = c->wpos;
    for (int n=0;n<N;++n) c->delay[(wp + n) % D] += in[n];
    for (int n=0;n<N;++n){
        float acc = 0.0f; int pos=(wp+n)%D, idx=pos;
        for (int k=0;k<M;++k){ acc += c->delay[idx] * c->ir[k]; if(--idx<0) idx += D; }
        out[n] = acc;
    }
    for (int n=0;n<N;++n) c->delay[(wp + n) % D] = 0.0f;
    c->wpos = (wp + N) % D;
}

/* Simple softclip */
static inline float softclip(float x){ float d=1.6f; x*=d; if(x>1.5f)x=1.5f; if(x<-1.5f)x=-1.5f; float x2=x*x; return (x*(27.0f + x2))/(27.0f + 9.0f*x2); }

/* WAV header helper (PCM16) */
#pragma pack(push,1)
typedef struct { char riff[4]; uint32_t overall_size; char wave[4]; char fmt_chunk_marker[4]; uint32_t length_of_fmt; uint16_t format_type; uint16_t channels; uint32_t sample_rate; uint32_t byterate; uint16_t block_align; uint16_t bits_per_sample; char data_chunk_header[4]; uint32_t data_size; } wav_hdr;
#pragma pack(pop)

static void write_wav_header(FILE *f, int channels, int sample_rate, int bits_per_sample, uint32_t data_bytes){
    wav_hdr h;
    memcpy(h.riff,"RIFF",4);
    h.overall_size = 36 + data_bytes;
    memcpy(h.wave,"WAVE",4);
    memcpy(h.fmt_chunk_marker,"fmt ",4);
    h.length_of_fmt = 16;
    h.format_type = 1; /* PCM */
    h.channels = (uint16_t)channels;
    h.sample_rate = (uint32_t)sample_rate;
    h.bits_per_sample = (uint16_t)bits_per_sample;
    h.byterate = h.sample_rate * h.channels * (h.bits_per_sample/8);
    h.block_align = h.channels * (h.bits_per_sample/8);
    memcpy(h.data_chunk_header,"data",4);
    h.data_size = data_bytes;
    fseek(f,0,SEEK_SET);
    fwrite(&h, sizeof(wav_hdr), 1, f);
}

/* Render routine: produce interleaved stereo PCM16 to file */
static int render_to_wav(const char *filename, int duration_seconds){
    FILE *wf = fopen(filename,"wb");
    if(!wf){ fprintf(stderr,"Unable to open output WAV '%s'\n", filename); return -1; }
    /* placeholder header (will overwrite after render) */
    uint32_t total_frames = (uint32_t)duration_seconds * SR;
    uint32_t data_bytes = total_frames * CHANNELS * sizeof(int16_t);
    /* write zero header now */
    wav_hdr z = {0}; fwrite(&z, sizeof(z), 1, wf);

    /* prepare DSP objects */
    Voice voice; voice_init(&voice, AMP, VIB_RATE, VIB_DEPTH);
    Conv conv; if(conv_init(&conv, IR_VEHICLE, IR_VEHICLE_LEN, PERIOD_FRAMES) < 0){ fclose(wf); fprintf(stderr,"conv init fail\n"); return -1; }

    float *mono_v = (float*)malloc(PERIOD_FRAMES * sizeof(float));
    float *wet = (float*)malloc(PERIOD_FRAMES * sizeof(float));
    int16_t *outbuf = (int16_t*)malloc((size_t)PERIOD_FRAMES * CHANNELS * sizeof(int16_t));
    if(!mono_v || !wet || !outbuf){ fclose(wf); fprintf(stderr,"alloc fail\n"); conv_free(&conv); return -1; }

    double global_time = 0.0;
    uint32_t frames_written = 0;
    while(frames_written < total_frames){
        int frames_left = (int)total_frames - (int)frames_written;
        int frames = (frames_left >= PERIOD_FRAMES) ? PERIOD_FRAMES : frames_left;

        /* compute block-edge continuous frequencies from triangular sweep */
        double t0 = global_time;
        double t1 = global_time + (double)frames / SR;
        double phase0 = fmod(t0, WAIL_CYCLE) / WAIL_CYCLE;
        double phase1 = fmod(t1, WAIL_CYCLE) / WAIL_CYCLE;
        double tri0 = (phase0 < 0.5) ? (phase0 * 2.0) : (1.0 - (phase0 - 0.5) * 2.0);
        double tri1 = (phase1 < 0.5) ? (phase1 * 2.0) : (1.0 - (phase1 - 0.5) * 2.0);
        double f_start = WAIL_F_LO + (WAIL_F_HI - WAIL_F_LO) * tri0;
        double f_end   = WAIL_F_LO + (WAIL_F_HI - WAIL_F_LO) * tri1;

        /* generate mono block with 2 partials, vibrato, jitter, simple one-pole coloring */
        for(int n=0;n<frames;++n){
            double frac = (double)n / (double)frames;
            double u = 0.5 * (1.0 - cos(M_PI * frac));
            double f_cont = f_start + (f_end - f_start) * u;
            double t = t0 + (double)n / SR;
            double vib = voice.vib_amt * sin(2.0*M_PI * voice.vib_rate * t);
            double jitter = JITTER * ((double)(xrng() & 0x7fff)/32767.0 - 0.5);
            double finst = f_cont * (1.0 + vib + jitter);
            double f2 = finst * PARTIAL_RATIO;
            double inc1 = 2.0*M_PI * finst / SR;
            double inc2 = 2.0*M_PI * f2 / SR;
            float s = (float)(sin(voice.ph1) * 0.82 + sin(voice.ph2) * 0.28);
            voice.ph1 += inc1; if(voice.ph1 > 2.0*M_PI) voice.ph1 -= 2.0*M_PI;
            voice.ph2 += inc2; if(voice.ph2 > 2.0*M_PI) voice.ph2 -= 2.0*M_PI;
            /* one-pole lowpass coloring */
            float a = 0.02f;
            float col = onepole(&voice.bp_z, s, a);
            mono_v[n] = col * voice.amp;
        }

        /* softclip + simple leveler (fast attack slow release) */
        for(int n=0;n<frames;++n) mono_v[n] = softclip(mono_v[n]);
        static float env = 0.0f;
        const float atk = expf(-1.0f / (0.001f * 4.0f * SR));
        const float rel = expf(-1.0f / (0.001f * 80.0f * SR));
        for(int n=0;n<frames;++n){
            float a = fabsf(mono_v[n]);
            if(a > env) env = atk * (env - a) + a; else env = rel * (env - a) + a;
            float gain = (env > 1e-5f) ? (0.6f / env) : 1.0f;
            if(gain > 1.0f) gain = 1.0f;
            mono_v[n] *= gain;
        }

        /* conv send/process (mono) */
        float *send = (float*)malloc(frames * sizeof(float));
        if(!send){ fprintf(stderr,"send alloc fail\n"); break; }
        for(int n=0;n<frames;++n) send[n] = mono_v[n] * 0.12f;
        conv_block(&conv, send, wet, frames);
        free(send);

        /* mix dry/wet and write interleaved stereo PCM16 */
        for(int n=0;n<frames;++n){
            float mono_out = mono_v[n] * (1.0f - 0.20f) + wet[n] * 0.20f;
            if (mono_out > 1.0f) mono_out = 1.0f;
            if (mono_out < -1.0f) mono_out = -1.0f;
            int16_t samp = (int16_t)lrintf(mono_out * 32767.0f);
            outbuf[2*n + 0] = samp;
            outbuf[2*n + 1] = samp;
        }

        /* write to WAV file */
        fwrite(outbuf, sizeof(int16_t), (size_t)frames * CHANNELS, wf);

        frames_written += frames;
        global_time += (double)frames / SR;
    }

    /* finalize header */
    fflush(wf);
    write_wav_header(wf, CHANNELS, SR, 16, frames_written * CHANNELS * sizeof(int16_t));
    fclose(wf);

    /* cleanup */
    free(mono_v); free(wet); free(outbuf);
    conv_free(&conv);
    return 0;
}

/* Play WAV via ALSA (reads PCM16 interleaved stereo) */
static int play_wav_alsa(const char *filename, const char *device){
    FILE *f = fopen(filename,"rb");
    if(!f){ fprintf(stderr,"Unable to open WAV '%s'\n", filename); return -1; }
    /* read header */
    wav_hdr hdr;
    if (fread(&hdr, sizeof(hdr), 1, f) != 1){ fclose(f); fprintf(stderr,"Invalid WAV header\n"); return -1; }
    if (hdr.format_type != 1 || hdr.bits_per_sample != 16){ fclose(f); fprintf(stderr,"Unsupported WAV format (need PCM16)\n"); return -1; }

    unsigned int rate = hdr.sample_rate;
    unsigned short channels = hdr.channels;
    snd_pcm_t *pcm;
    int rc = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0){ fprintf(stderr,"snd_pcm_open: %s\n", snd_strerror(rc)); fclose(f); return -1; }

    snd_pcm_hw_params_t *hw; snd_pcm_hw_params_alloca(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, hw, channels);
    snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0);
    snd_pcm_uframes_t period = PERIOD_FRAMES;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    snd_pcm_uframes_t bufsize = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &bufsize);
    rc = snd_pcm_hw_params(pcm, hw);
    if (rc < 0){ fprintf(stderr,"snd_pcm_hw_params: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); fclose(f); return -1; }
    rc = snd_pcm_prepare(pcm);
    if (rc < 0){ fprintf(stderr,"snd_pcm_prepare: %s\n", snd_strerror(rc)); snd_pcm_close(pcm); fclose(f); return -1; }

    /* stream data */
    const int BUF_FRAMES = PERIOD_FRAMES;
    int16_t *buf = (int16_t*)malloc((size_t)BUF_FRAMES * channels * sizeof(int16_t));
    if(!buf){ fprintf(stderr,"alloc fail\n"); snd_pcm_close(pcm); fclose(f); return -1; }

    /* data chunk begins immediately after header (we already read header) */
    /* ensure file pointer is at start of data chunk (wav_hdr already read) */

    while (!feof(f)) {
        size_t r = fread(buf, sizeof(int16_t), (size_t)BUF_FRAMES * channels, f);
        if (r == 0) break;
        snd_pcm_sframes_t frames = (snd_pcm_sframes_t)(r / channels);
        int16_t *ptr = buf;
        while (frames > 0) {
            snd_pcm_sframes_t written = snd_pcm_writei(pcm, ptr, frames);
            if (written < 0) {
                written = snd_pcm_recover(pcm, (int)written, 0);
                if (written < 0) { fprintf(stderr,"snd_pcm_writei error: %s\n", snd_strerror((int)written)); free(buf); snd_pcm_close(pcm); fclose(f); return -1; }
            }
            ptr += written * channels;
            frames -= written;
        }
    }

    free(buf);
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(f);
    return 0;
}

/* one-pole helper used in render (keeps static state per voice) */
static inline float onepole(float *z, float x, float a){
    float y = a * x + (1.0f - a) * (*z);
    *z = y; return y;
}

/* Entry point: render then play */
int main(int argc, char **argv){
    int duration = 30; /* default seconds */
    const char *dev = "default";
    if (argc >= 2) duration = atoi(argv[1]) > 0 ? atoi(argv[1]) : duration;
    if (argc >= 3) dev = argv[2];

    const char *outname = "lapd_render.wav";
    fprintf(stderr, "Rendering %d seconds to '%s'...\n", duration, outname);
    if (render_to_wav(outname, duration) != 0){ fprintf(stderr, "Render failed\n"); return 1; }
    fprintf(stderr, "Render complete. Playing '%s' on ALSA device '%s'...\n", outname, dev);
    if (play_wav_alsa(outname, dev) != 0){ fprintf(stderr, "Playback failed\n"); return 1; }
    fprintf(stderr, "Playback finished.\n");
    return 0;
}

