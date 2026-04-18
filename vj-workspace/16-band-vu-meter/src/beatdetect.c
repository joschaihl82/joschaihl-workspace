// main.c
// 20-band VU meter using PulseAudio (default.monitor) + SDL2
// Bands centered at 1kHz, 2kHz, 3kHz, ... 20kHz (geometric band edges)
// Compile:
// gcc -O2 -o vu20 main.c -lSDL2 -lpulse-simple -lpulse -lm -lpthread

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------- Config ----------------
#define SAMPLE_RATE 44100
#define CHANNELS 2
#define READ_FRAMES 1024            // frames per read from Pulse
#define FFT_SIZE 1024               // must be power of two, >= READ_FRAMES ideally
#define N_BANDS 20                  // 20 bands: 1kHz..20kHz
#define RING_SECONDS 2              // ring buffer length in seconds
#define WINDOW_HANN(n) (0.5*(1.0 - cos(2.0*M_PI*(n)/(FFT_SIZE-1))))

#define SDL_WIN_W 1000
#define SDL_WIN_H 420
#define BAR_SPACING 6
#define PEAK_HOLD_MS 300

// ---------------- Globals ----------------
static volatile int running = 1;
static void sigint_handler(int sig) { (void)sig; running = 0; }

static float *ringbuf = NULL;
static size_t ring_cap = 0;
static size_t ring_pos = 0;
static pthread_mutex_t ring_mutex = PTHREAD_MUTEX_INITIALIZER;

// FFT arrays
static double fft_re[FFT_SIZE];
static double fft_im[FFT_SIZE];
static double tw_re[FFT_SIZE/2];
static double tw_im[FFT_SIZE/2];
static double window_hann[FFT_SIZE];

// band smoothing and peaks
static double band_smooth[N_BANDS];
static double band_peak[N_BANDS];
static uint32_t band_peak_ts[N_BANDS];

// fixed center frequencies (Hz): 1k,2k,...,20k
static const double band_centers[N_BANDS] = {
    1000.0, 2000.0, 3000.0, 4000.0, 5000.0,
    6000.0, 7000.0, 8000.0, 9000.0, 10000.0,
    11000.0, 12000.0, 13000.0, 14000.0, 15000.0,
    16000.0, 17000.0, 18000.0, 19000.0, 20000.0
};

// ---------------- FFT helpers ----------------
static void fft_prepare(void) {
    for (int k = 0; k < FFT_SIZE/2; ++k) {
        double angle = -2.0 * M_PI * k / (double)FFT_SIZE;
        tw_re[k] = cos(angle);
        tw_im[k] = sin(angle);
    }
    for (int n = 0; n < FFT_SIZE; ++n)
        window_hann[n] = WINDOW_HANN(n);
}

static void bit_reverse_permute(double *re, double *im) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < FFT_SIZE; ++i) {
        if (i < j) {
            double tr = re[i]; re[i] = re[j]; re[j] = tr;
            double ti = im[i]; im[i] = im[j]; im[j] = ti;
        }
        unsigned int m = FFT_SIZE >> 1;
        while (m >= 1 && j >= m) { j -= m; m >>= 1; }
        j += m;
    }
}

static void fft_execute(double *re, double *im) {
    bit_reverse_permute(re, im);
    for (int len = 2; len <= FFT_SIZE; len <<= 1) {
        int half = len >> 1;
        int step = FFT_SIZE / len;
        for (int i = 0; i < FFT_SIZE; i += len) {
            for (int j = 0; j < half; ++j) {
                int tw = j * step;
                double tre = tw_re[tw] * re[i + j + half] - tw_im[tw] * im[i + j + half];
                double tim = tw_re[tw] * im[i + j + half] + tw_im[tw] * re[i + j + half];
                double ur = re[i + j];
                double ui = im[i + j];
                re[i + j] = ur + tre;
                im[i + j] = ui + tim;
                re[i + j + half] = ur - tre;
                im[i + j + half] = ui - tim;
            }
        }
    }
}

// ---------------- Ring helpers ----------------
static void push_mono_to_ring(const int16_t *mono, size_t n) {
    pthread_mutex_lock(&ring_mutex);
    for (size_t i = 0; i < n; ++i) {
        ringbuf[ring_pos] = mono[i] / 32768.0f;
        ring_pos = (ring_pos + 1) % ring_cap;
    }
    pthread_mutex_unlock(&ring_mutex);
}

static void snapshot_from_ring(float *out, size_t n) {
    pthread_mutex_lock(&ring_mutex);
    if (n > ring_cap) n = ring_cap;
    size_t start = (ring_pos + ring_cap - n) % ring_cap;
    for (size_t i = 0; i < n; ++i)
        out[i] = ringbuf[(start + i) % ring_cap];
    pthread_mutex_unlock(&ring_mutex);
}

// ---------------- Pulse thread ----------------
typedef struct {
    pa_simple *pa;
    pa_sample_spec ss;
} pa_ctx_t;

static void *pulse_thread_fn(void *arg) {
    pa_ctx_t *ctx = (pa_ctx_t*)arg;
    int16_t *read_buf = malloc(sizeof(int16_t) * READ_FRAMES * ctx->ss.channels);
    if (!read_buf) return NULL;
    int error;
    size_t bytes = READ_FRAMES * ctx->ss.channels * sizeof(int16_t);

    while (running) {
        if (pa_simple_read(ctx->pa, read_buf, bytes, &error) < 0) {
            fprintf(stderr, "Pulse read error: %s\n", pa_strerror(error));
            SDL_Delay(5);
            continue;
        }
        // convert interleaved stereo -> mono (average)
        int16_t mono[READ_FRAMES];
        for (int i = 0; i < READ_FRAMES; ++i) {
            int32_t s = 0;
            for (int c = 0; c < (int)ctx->ss.channels; ++c)
                s += read_buf[i * ctx->ss.channels + c];
            mono[i] = (int16_t)(s / (int)ctx->ss.channels);
        }
        push_mono_to_ring(mono, READ_FRAMES);
    }

    free(read_buf);
    return NULL;
}

// ---------------- Map FFT bins to fixed center-frequency bands ----------------
static void map_bins_to_fixed_centers(const double *mag, double *bands_out, unsigned sample_rate) {
    // For each center frequency fc, compute band edges as geometric mean with neighbors:
    // edge_lo = sqrt(fc_prev * fc) ; edge_hi = sqrt(fc * fc_next)
    // For first/last band use fc/sqrt(2) and fc*sqrt(2) (clamped to Nyquist)
    double nyquist = (double)sample_rate / 2.0;
    double bin_width = (double) sample_rate / (double) FFT_SIZE;

    for (int b = 0; b < N_BANDS; ++b) bands_out[b] = 0.0;

    for (int b = 0; b < N_BANDS; ++b) {
        double fc = band_centers[b];
        double lo, hi;
        if (b == 0) {
            lo = fc / sqrt(2.0);
        } else {
            lo = sqrt(band_centers[b-1] * fc);
        }
        if (b == N_BANDS - 1) {
            hi = fc * sqrt(2.0);
        } else {
            hi = sqrt(fc * band_centers[b+1]);
        }
        if (lo < 20.0) lo = 20.0;
        if (hi > nyquist) hi = nyquist;
        if (lo >= hi) { bands_out[b] = 0.0; continue; }

        int bin_lo = (int)floor(lo / bin_width);
        int bin_hi = (int)ceil(hi / bin_width);
        if (bin_lo < 0) bin_lo = 0;
        if (bin_hi > FFT_SIZE/2 - 1) bin_hi = FFT_SIZE/2 - 1;
        double sum = 0.0;
        for (int k = bin_lo; k <= bin_hi; ++k) sum += mag[k];
        bands_out[b] = sum;
    }
}

// ---------------- Utility: current ms ----------------
static uint32_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
}

// ---------------- Main ----------------
int main(int argc, char *argv[]) {
    const char *device = "default.monitor";

    // parse CLI: --device <name>
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--device") == 0 && i + 1 < argc) {
            device = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [--device <pulse_source>]\n", argv[0]);
            printf("Defaults: device=default.monitor\n");
            return 0;
        }
    }

    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    // prepare FFT twiddles and window
    fft_prepare();

    // allocate ring buffer
    ring_cap = (size_t)SAMPLE_RATE * RING_SECONDS;
    ringbuf = calloc(ring_cap, sizeof(float));
    if (!ringbuf) { fprintf(stderr, "OOM ring\n"); return 1; }

    // PulseAudio setup
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = SAMPLE_RATE;
    ss.channels = CHANNELS;
    int pa_error = 0;
    pa_simple *pa = pa_simple_new(NULL, "vu20", PA_STREAM_RECORD, device, "vu-record", &ss, NULL, NULL, &pa_error);
    if (!pa) {
        fprintf(stderr, "PulseAudio error: %s\n", pa_strerror(pa_error));
        free(ringbuf);
        return 2;
    }

    // start pulse thread
    pa_ctx_t ctx = { .pa = pa, .ss = ss };
    pthread_t pa_thread;
    if (pthread_create(&pa_thread, NULL, pulse_thread_fn, &ctx) != 0) {
        fprintf(stderr, "Failed to create pulse thread\n");
        pa_simple_free(pa);
        free(ringbuf);
        return 3;
    }

    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        running = 0;
    }

    SDL_Window *win = SDL_CreateWindow("20-Band VU Meter (1kHz..20kHz)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SDL_WIN_W, SDL_WIN_H, SDL_WINDOW_SHOWN);
    if (!win) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); running = 0; }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) { fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError()); running = 0; }

    // initialize smoothing/peaks
    for (int i = 0; i < N_BANDS; ++i) { band_smooth[i] = 0.0; band_peak[i] = 0.0; band_peak_ts[i] = 0; }

    // temp buffers
    float snapshot[FFT_SIZE];
    double mag[FFT_SIZE/2];
    double bands[N_BANDS];

    uint32_t last_render = now_ms();

    while (running) {
        // handle SDL events
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
            }
        }

        // take snapshot for FFT (copy latest FFT_SIZE samples)
        pthread_mutex_lock(&ring_mutex);
        size_t start = (ring_pos + ring_cap - FFT_SIZE) % ring_cap;
        for (int i = 0; i < FFT_SIZE; ++i) snapshot[i] = ringbuf[(start + i) % ring_cap];
        pthread_mutex_unlock(&ring_mutex);

        // prepare FFT arrays
        for (int i = 0; i < FFT_SIZE; ++i) {
            fft_re[i] = snapshot[i] * window_hann[i];
            fft_im[i] = 0.0;
        }
        fft_execute(fft_re, fft_im);

        // magnitude
        int half = FFT_SIZE/2;
        for (int b = 0; b < half; ++b) mag[b] = sqrt(fft_re[b]*fft_re[b] + fft_im[b]*fft_im[b]);

        // map to fixed-center bands
        map_bins_to_fixed_centers(mag, bands, SAMPLE_RATE);

        // normalize & smooth bands (simple running average + log scaling)
        for (int b = 0; b < N_BANDS; ++b) {
            double val = bands[b];
            // convert to dB-like scale
            double db = 20.0 * log10(val + 1e-9);
            // map db range [-100 .. 0] to [0..1]
            double norm = (db + 100.0) / 100.0;
            if (norm < 0.0) norm = 0.0;
            if (norm > 1.0) norm = 1.0;
            // smoothing (attack fast, release slower)
            double alpha_attack = 0.6;
            double alpha_release = 0.08;
            if (norm > band_smooth[b])
                band_smooth[b] = alpha_attack * norm + (1.0 - alpha_attack) * band_smooth[b];
            else
                band_smooth[b] = alpha_release * norm + (1.0 - alpha_release) * band_smooth[b];

            // peak hold
            uint32_t tnow = now_ms();
            if (band_smooth[b] > band_peak[b]) {
                band_peak[b] = band_smooth[b];
                band_peak_ts[b] = tnow;
            } else {
                if (tnow - band_peak_ts[b] > PEAK_HOLD_MS) {
                    band_peak[b] *= 0.98; // slow decay
                    if (band_peak[b] < 0.0) band_peak[b] = 0.0;
                }
            }
        }

        // render
        SDL_SetRenderDrawColor(ren, 18, 18, 18, 255);
        SDL_RenderClear(ren);

        int total_width = SDL_WIN_W;
        int bar_w = (total_width - (N_BANDS + 1) * BAR_SPACING) / N_BANDS;
        if (bar_w < 4) bar_w = 4;
        int x = BAR_SPACING;

        for (int b = 0; b < N_BANDS; ++b) {
            float v = (float)band_smooth[b];
            float peak = (float)band_peak[b];
            int bar_h = (int)(v * (SDL_WIN_H - 60));
            int peak_y = SDL_WIN_H - 30 - (int)(peak * (SDL_WIN_H - 60));

            // color gradient: low bands green -> mid yellow -> high red
            float t = (float)b / (float)(N_BANDS - 1);
            Uint8 r = (Uint8)(fminf(1.0f, 2.0f * t) * 255);
            Uint8 g = (Uint8)(fminf(1.0f, 2.0f * (1.0f - fabsf(t - 0.5f))) * 255);
            Uint8 bl = (Uint8)(fminf(1.0f, 2.0f * (1.0f - t)) * 255);

            // draw bar background
            SDL_Rect bg = { x, 20, bar_w, SDL_WIN_H - 60 };
            SDL_SetRenderDrawColor(ren, 36, 36, 36, 255);
            SDL_RenderFillRect(ren, &bg);

            // draw filled portion
            SDL_Rect fill = { x, SDL_WIN_H - 30 - bar_h, bar_w, bar_h };
            SDL_SetRenderDrawColor(ren, r, g, bl, 255);
            SDL_RenderFillRect(ren, &fill);

            // draw peak line
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 200);
            SDL_Rect peak_rect = { x, peak_y, bar_w, 2 };
            SDL_RenderFillRect(ren, &peak_rect);

            // draw center frequency label (small rectangles as markers)
            // (no text rendering to keep dependencies minimal)
            SDL_SetRenderDrawColor(ren, 200, 200, 200, 200);
            SDL_Rect label = { x + bar_w/2 - 1, SDL_WIN_H - 18, 2, 8 };
            SDL_RenderFillRect(ren, &label);

            x += bar_w + BAR_SPACING;
        }

        // draw top legend: centers
        // (no text; draw small ticks at top corresponding to band centers)
        SDL_RenderPresent(ren);

        // cap render rate ~60Hz
        uint32_t now = now_ms();
        uint32_t dt = now - last_render;
        if (dt < 16) SDL_Delay(16 - dt);
        last_render = now;
    }

    // cleanup
    running = 0;
    pthread_join(pa_thread, NULL);
    pa_simple_free(pa);

    if (ren) SDL_DestroyRenderer(ren);
    if (win) SDL_DestroyWindow(win);
    SDL_Quit();

    free(ringbuf);
    return 0;
}
