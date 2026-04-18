/* render_ss2000.c
   Render: repeated cycles of Wail (7.5s) then Yelp (2.5s), no crossfade,
   SS2000 nominal LFO rates, mono processing -> stereo WAV output.
   Compile: cc -O2 -std=c11 -lm -o render_ss2000 render_ss2000.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Settings */
#define SR 44100
#define ITERATIONS 8            /* Anzahl Zyklen (Wail+Yelp). Anpassen falls gewünscht */
#define WAIL_DUR 7.5           /* Sekunden */
#define YELP_DUR 2.5           /* Sekunden */
#define OUTFILE "siren_sequence.wav"

/* Simple WAV header writer/finalizer */
static int wav_write_header(FILE *f, uint32_t sr, uint16_t ch) {
    if (!f) return -1;
    uint32_t byte_rate = sr * ch * 2;
    uint16_t block_align = ch * 2;
    fwrite("RIFF",1,4,f);
    uint32_t cz = 0; fwrite(&cz,4,1,f);
    fwrite("WAVE",1,4,f);
    fwrite("fmt ",1,4,f);
    uint32_t sub1 = 16; fwrite(&sub1,4,1,f);
    uint16_t af = 1; fwrite(&af,2,1,f);
    fwrite(&ch,2,1,f);
    fwrite(&sr,4,1,f);
    fwrite(&byte_rate,4,1,f);
    fwrite(&block_align,2,1,f);
    uint16_t bps = 16; fwrite(&bps,2,1,f);
    fwrite("data",1,4,f);
    uint32_t ds = 0; fwrite(&ds,4,1,f);
    return 0;
}
static int wav_finalize(FILE *f) {
    if (!f) return -1;
    long len = ftell(f);
    if (len < 0) return -1;
    uint32_t data_sz = (uint32_t)(len - 44);
    uint32_t riff_sz = (uint32_t)(len - 8);
    if (fseek(f, 40, SEEK_SET) != 0) return -1;
    if (fwrite(&data_sz,4,1,f) != 1) return -1;
    if (fseek(f, 4, SEEK_SET) != 0) return -1;
    if (fwrite(&riff_sz,4,1,f) != 1) return -1;
    if (fseek(f, 0, SEEK_END) != 0) return -1;
    return 0;
}

/* Helpers */
static inline double tri_lfo(double t, double rate) {
    double p = fmod(t * rate, 1.0);
    if (p < 0.5) return 4.0 * p - 1.0;
    return 3.0 - 4.0 * p;
}
static inline double sweep_freq_tri(double t, double rate, double f0, double f1) {
    double l = tri_lfo(t, rate);
    double mid = 0.5 * (f0 + f1);
    double half = 0.5 * (f1 - f0);
    return mid + half * l;
}

/* Synthesize mono sine with time-varying freq function into double buffer [-1..1] */
static void synth(double (*freq_fn)(double, void*), void *ctx, double dur, int sr, double *out) {
    double phase = 0.0, t = 0.0, dt = 1.0 / sr;
    size_t frames = (size_t)(dur * sr);
    for (size_t i=0;i<frames;i++) {
        double f = freq_fn(t, ctx);
        double incr = 2.0 * M_PI * f * dt;
        phase += incr;
        if (phase > 2.0*M_PI) phase -= 2.0*M_PI;
        double s = sin(phase) * 0.85; /* amplitude */
        out[i] = s;
        t += dt;
    }
}

/* Contexts and freq functions (SS2000 nominal rates) */
typedef struct { double f0,f1,rate; } ctx_siren;
static double freq_wail(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }
static double freq_yelp(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }

/* Write mono double buffer to stereo int16 WAV (streamed) */
static int write_mono_to_stereo(FILE *f, const double *mono, size_t frames) {
    const size_t BLOCK = 4096;
    int16_t buf[BLOCK * 2];
    size_t written = 0;
    while (written < frames) {
        size_t n = frames - written;
        if (n > BLOCK) n = BLOCK;
        for (size_t i=0;i<n;i++) {
            double s = mono[written + i];
            if (s > 1.0) s = 1.0;
            if (s < -1.0) s = -1.0;
            int16_t v = (int16_t)lrint(s * 32767.0);
            buf[2*i] = v;
            buf[2*i+1] = v;
        }
        if (fwrite(buf, sizeof(int16_t), n * 2, f) != n * 2) return -1;
        written += n;
    }
    return 0;
}

/* Main */
int main(void) {
    const int sr = SR;
    const int iterations = ITERATIONS;

    /* SS2000 nominal sweep ranges and cycle rates (nominal) */
    ctx_siren wctx = {700.0, 1500.0, 12.0/60.0};   /* Wail: 12/min = 0.20 Hz */
    ctx_siren yctx = {700.0, 1500.0, 180.0/60.0};  /* Yelp: 180/min = 3.00 Hz */

    size_t w_frames = (size_t)(WAIL_DUR * sr);
    size_t y_frames = (size_t)(YELP_DUR * sr);

    double *buf_w = malloc(sizeof(double) * w_frames);
    double *buf_y = malloc(sizeof(double) * y_frames);
    if (!buf_w || !buf_y) { fprintf(stderr,"OOM\n"); return 1; }

    /* synth once per type (deterministic) */
    synth(freq_wail, &wctx, WAIL_DUR, sr, buf_w);
    synth(freq_yelp, &yctx, YELP_DUR, sr, buf_y);

    /* open output and write header */
    FILE *out = fopen(OUTFILE, "wb");
    if (!out) { perror("fopen"); return 1; }
    if (wav_write_header(out, sr, 2) != 0) { fclose(out); fprintf(stderr,"WAV header failed\n"); return 1; }

    /* render sequence: for each iteration write wail then yelp (no crossfade) */
    for (int it=0; it<iterations; ++it) {
        if (write_mono_to_stereo(out, buf_w, w_frames) != 0) { fprintf(stderr,"Write error (wail)\n"); goto fail; }
        if (write_mono_to_stereo(out, buf_y, y_frames) != 0) { fprintf(stderr,"Write error (yelp)\n"); goto fail; }
    }

    /* finalize */
    if (wav_finalize(out) != 0) { fprintf(stderr,"WAV finalize failed\n"); goto fail; }
    fclose(out);
    free(buf_w); free(buf_y);
    printf("Rendered %s: %d cycles of %.1fs wail + %.1fs yelp (no crossfade)\n", OUTFILE, iterations, WAIL_DUR, YELP_DUR);
    return 0;

fail:
    fclose(out);
    free(buf_w); free(buf_y);
    return 1;
}


