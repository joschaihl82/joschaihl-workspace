/*
 lapd_simple.c
 Minimal SS2000-style wail & yelp synth + immediate playback.
 Mono processing; duplicate to stereo when writing/playing.
 No extras, no variants, no CLI params.
 Compile: cc -O2 -std=c11 -lm -o lapd_simple lapd_simple.c
 Run: ./lapd_simple
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* --- Simple streaming WAV writer (16-bit stereo) --- */
static int wav_write_header(FILE *f, uint32_t sr, uint16_t ch) {
    if (!f) return -1;
    uint32_t byte_rate = sr * ch * 2;
    uint16_t block_align = ch * 2;
    fwrite("RIFF",1,4,f);
    uint32_t cz = 0; fwrite(&cz,4,1,f); /* placeholder */
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

/* --- helpers --- */
static inline double clampd(double x, double a, double b) { return x < a ? a : (x > b ? b : x); }
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

/* --- synth block (mono) --- */
static void synth_block(double (*freq_fn)(double, void*), void *ctx,
                        double duration, int sr, int16_t *mono_out, size_t frames)
{
    double phase = 0.0, t = 0.0, dt = 1.0 / sr;
    for (size_t i = 0; i < frames; ++i) {
        double f = freq_fn(t, ctx);
        double env = 1.0;
        double fade = 0.01;
        if (t < fade) env = t / fade;
        if (t > duration - fade) env = (duration - t) / fade;
        env = clampd(env, 0.0, 1.0);
        double incr = 2.0 * M_PI * f * dt;
        phase += incr; if (phase > 2.0*M_PI) phase -= 2.0*M_PI;
        double s = sin(phase) * env;
        double samp = s * 0.85;
        mono_out[i] = (int16_t)(clampd(samp, -1.0, 1.0) * 32767.0);
        t += dt;
    }
}

/* --- contexts and freq functions (SS2000 manual rates) --- */
/* Use manual nominal cycle rates:
   Wail: 12/min = 0.20 Hz
   Yelp: 180/min = 3.00 Hz
   Frequency ranges: use 700-1500 Hz nominal where applicable
*/
typedef struct { double f0,f1,rate,dur; } ctx_siren;
static double freq_wail(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }
static double freq_yelp(double t, void *v) { ctx_siren *c = (ctx_siren*)v; return sweep_freq_tri(t, c->rate, c->f0, c->f1); }

/* --- write mono buffer duplicated to stereo WAV file --- */
static int write_mono_to_stereo_wav(const char *name, const int16_t *mono, size_t frames, int sr) {
    FILE *f = fopen(name, "wb");
    if (!f) return -1;
    if (wav_write_header(f, sr, 2) != 0) { fclose(f); return -1; }
    /* write in blocks to avoid huge memory use */
    size_t block = 4096;
    int16_t buf[4096*2];
    size_t written = 0;
    while (written < frames) {
        size_t n = frames - written;
        if (n > block) n = block;
        for (size_t i=0;i<n;i++) { buf[2*i] = mono[written + i]; buf[2*i+1] = mono[written + i]; }
        if (fwrite(buf, sizeof(int16_t), n * 2, f) != n * 2) { fclose(f); return -1; }
        written += n;
    }
    if (wav_finalize(f) != 0) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

/* --- find playback node under /dev/snd (prefer 'p' nodes) --- */
static int find_playback_node(char *out, size_t len) {
    DIR *d = opendir("/dev/snd");
    if (!d) return -1;
    struct dirent *e;
    char fallback[256] = {0};
    while ((e = readdir(d)) != NULL) {
        if (strncmp(e->d_name, "pcm", 3) != 0) continue;
        size_t L = strlen(e->d_name);
        if (L > 0 && (e->d_name[L-1] == 'p' || e->d_name[L-1] == 'P')) {
            snprintf(out, len, "/dev/snd/%s", e->d_name);
            closedir(d);
            return 0;
        }
        if (fallback[0] == '\0') snprintf(fallback, sizeof(fallback), "/dev/snd/%s", e->d_name);
    }
    closedir(d);
    if (fallback[0] == '\0') return -2;
    strncpy(out, fallback, len-1); out[len-1] = '\0';
    return 0;
}

/* --- minimal WAV loader for 16-bit PCM (reads full file) --- */
typedef struct { uint32_t sr; uint16_t nch; size_t frames; int16_t *data; } wavinfo_t;
static int load_wav16(const char *path, wavinfo_t *w) {
    FILE *f = fopen(path, "rb"); if (!f) return -1;
    uint8_t hdr[44]; if (fread(hdr,1,44,f) != 44) { fclose(f); return -2; }
    if (memcmp(hdr,"RIFF",4)!=0 || memcmp(hdr+8,"WAVE",4)!=0) { fclose(f); return -3; }
    uint16_t audiofmt = *(uint16_t*)(hdr+20);
    uint16_t nch = *(uint16_t*)(hdr+22);
    uint32_t sr = *(uint32_t*)(hdr+24);
    uint16_t bits = *(uint16_t*)(hdr+34);
    uint32_t datasz = *(uint32_t*)(hdr+40);
    if (audiofmt != 1 || bits != 16) { fclose(f); return -4; }
    size_t frames = datasz / (nch * 2);
    int16_t *buf = malloc(datasz);
    if (!buf) { fclose(f); return -5; }
    if (fread(buf, 1, datasz, f) != datasz) { free(buf); fclose(f); return -6; }
    fclose(f);
    w->sr = sr; w->nch = nch; w->frames = frames; w->data = buf;
    return 0;
}
static int16_t *to_mono(const wavinfo_t *w) {
    if (!w || !w->data) return NULL;
    int16_t *mono = malloc(w->frames * sizeof(int16_t));
    if (!mono) return NULL;
    if (w->nch == 1) { memcpy(mono, w->data, w->frames * sizeof(int16_t)); return mono; }
    for (size_t i=0;i<w->frames;i++) {
        int32_t sum = 0;
        for (int c=0;c<w->nch;c++) sum += w->data[i * w->nch + c];
        sum /= (int)w->nch;
        if (sum > INT16_MAX) sum = INT16_MAX;
        if (sum < INT16_MIN) sum = INT16_MIN;
        mono[i] = (int16_t)sum;
    }
    return mono;
}

/* write all bytes blocking */
static int write_all_fd(int fd, const void *buf, size_t bytes) {
    const uint8_t *p = buf; size_t wrote = 0;
    while (wrote < bytes) {
        ssize_t r = write(fd, p + wrote, bytes - wrote);
        if (r > 0) { wrote += (size_t)r; continue; }
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { struct timespec ts = {0,2000000}; nanosleep(&ts,NULL); continue; }
            return -errno;
        }
    }
    return 0;
}

/* crossfade mono tail->head */
static void crossfade_mono(int16_t *out, const int16_t *tail, size_t tail_len, const int16_t *head, size_t head_len, size_t fade_samples) {
    for (size_t i=0;i<fade_samples;i++) {
        double a = (double)(fade_samples - i) / (double)fade_samples;
        double b = (double)i / (double)fade_samples;
        double s = tail[tail_len - fade_samples + i] * a + head[i] * b;
        if (s > 32767.0) s = 32767.0;
        if (s < -32768.0) s = -32768.0;
        out[i] = (int16_t)s;
    }
}
static void mono_to_stereo_interleaved(const int16_t *mono, int16_t *dst, size_t frames) {
    for (size_t i=0;i<frames;i++) { dst[2*i] = mono[i]; dst[2*i+1] = mono[i]; }
}

/* --- playback loop: only wail & yelp, Yelp prob ramps 0->0.25 over 8 iterations --- */
static int play_sequence_after_synth(void) {
    wavinfo_t wail_w = {0}, yelp_w = {0};
    if (load_wav16("wail.wav", &wail_w) != 0) { fprintf(stderr,"Cannot load wail.wav\n"); return -1; }
    if (load_wav16("yelp.wav", &yelp_w) != 0) { fprintf(stderr,"Cannot load yelp.wav\n"); free(wail_w.data); return -1; }
    if (wail_w.sr != yelp_w.sr) { fprintf(stderr,"Sample rates differ\n"); free(wail_w.data); free(yelp_w.data); return -1; }

    int16_t *mono_wail = to_mono(&wail_w);
    int16_t *mono_yelp = to_mono(&yelp_w);
    if (!mono_wail || !mono_yelp) { fprintf(stderr,"OOM\n"); free(wail_w.data); free(yelp_w.data); return -1; }

    char dev[256] = {0};
    if (find_playback_node(dev, sizeof(dev)) != 0) {
        fprintf(stderr,"No /dev/snd playback node found; skipping playback\n");
        free(wail_w.data); free(yelp_w.data); free(mono_wail); free(mono_yelp);
        return 0;
    }
    int fd = open(dev, O_WRONLY | O_NONBLOCK);
    if (fd < 0) { perror("open playback"); free(wail_w.data); free(yelp_w.data); free(mono_wail); free(mono_yelp); return -1; }

    srand((unsigned)time(NULL));
    size_t fade_samples = (size_t)(0.050 * wail_w.sr);
    if (fade_samples < 1) fade_samples = 1;
    const double target_p = 0.25;
    const int ramp_steps = 8;
    const double step = target_p / (double)ramp_steps;

    fprintf(stderr, "Playing sequence (wail / yelp). Yelp prob ramps 0%%->25%% over 8 iterations. Stop with Ctrl+C\n");

    const int16_t *current = mono_wail; size_t current_len = wail_w.frames;
    int iteration = 0;

    while (1) {
        double p_yelp = (iteration <= 0) ? 0.0 : (iteration >= ramp_steps ? target_p : step * (double)iteration);
        double r = (double)rand() / (double)RAND_MAX;
        const int16_t *next = (r < p_yelp) ? mono_yelp : mono_wail;
        size_t next_len = (r < p_yelp) ? yelp_w.frames : wail_w.frames;

        /* play current except last fade_samples */
        size_t play_frames = (current_len > fade_samples) ? (current_len - fade_samples) : current_len;
        int16_t *st = malloc(play_frames * 2 * sizeof(int16_t));
        if (!st) break;
        mono_to_stereo_interleaved(current, st, play_frames);
        if (write_all_fd(fd, st, play_frames * 2 * sizeof(int16_t)) != 0) { free(st); break; }
        free(st);

        /* crossfade */
        if (fade_samples > 0 && current_len >= fade_samples && next_len >= fade_samples) {
            int16_t *fade_m = malloc(fade_samples * sizeof(int16_t));
            if (!fade_m) break;
            crossfade_mono(fade_m, current, current_len, next, next_len, fade_samples);
            int16_t *fade_st = malloc(fade_samples * 2 * sizeof(int16_t));
            if (!fade_st) { free(fade_m); break; }
            mono_to_stereo_interleaved(fade_m, fade_st, fade_samples);
            if (write_all_fd(fd, fade_st, fade_samples * 2 * sizeof(int16_t)) != 0) { free(fade_m); free(fade_st); break; }
            free(fade_m); free(fade_st);
        }

        current = next; current_len = next_len;
        iteration++;
        struct timespec ts = {0, 1000000}; nanosleep(&ts, NULL);
    }

    close(fd);
    free(wail_w.data); free(yelp_w.data); free(mono_wail); free(mono_yelp);
    return 0;
}

/* --- generate only wail & yelp (mono processing, duplicate to stereo) --- */
static int generate_wail_yelp(void) {
    const int sr = 44100;
    /* SS2000 manual nominal rates */
    ctx_siren w = {700.0, 1500.0, (12.0/60.0), 10.0}; /* wail 0.20 Hz */
    ctx_siren y = {700.0, 1500.0, (180.0/60.0), 10.0}; /* yelp 3.00 Hz */

    size_t frames;
    int16_t *mono = NULL;
    int16_t *stereo = NULL;
    int rc = 0;

    /* Wail */
    frames = (size_t)(w.dur * sr);
    mono = malloc(sizeof(int16_t) * frames);
    if (!mono) return -1;
    synth_block((double(*)(double,void*))freq_wail, &w, w.dur, sr, mono, frames);
    if (write_mono_to_stereo_wav("wail.wav", mono, frames, sr) != 0) { rc = -1; goto out; }

    /* Yelp */
    frames = (size_t)(y.dur * sr);
    mono = realloc(mono, sizeof(int16_t) * frames);
    if (!mono) { rc = -1; goto out; }
    synth_block((double(*)(double,void*))freq_yelp, &y, y.dur, sr, mono, frames);
    if (write_mono_to_stereo_wav("yelp.wav", mono, frames, sr) != 0) { rc = -1; goto out; }

out:
    free(mono);
    free(stereo);
    return rc;
}

/* --- main: generate then play (no params) --- */
int main(void) {
    printf("Generating wail.wav and yelp.wav (mono processed, stereo output)...\n");
    if (generate_wail_yelp() != 0) { fprintf(stderr,"Generation failed\n"); return 1; }
    printf("Generation complete — starting playback sequence.\n");
    return play_sequence_after_synth();
}


