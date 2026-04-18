// beatvis_fft.c
// gcc -O2 -o beatvis_fft beatvis_fft.c -lSDL2 -lasound -lGL -lm
// ALSA PCM capture + ALSA mixer control + SDL2/OpenGL visualization
// BUFFERSIZE = 512, Headroom 24 dB, FFT-based kick detection (no FFT lib), FFT_SIZE = 2048

#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFFERSIZE 512
#define FFT_SIZE 2048
#define DEFAULT_SR 48000

#define TARGET_DB 24.0f
#define MAX_GAIN 4.0f
#define ATTACK_FAST 0.6f
#define RELEASE_SLOW 0.02f

#define LOWPASS_ALPHA 0.02f
#define KICK_DEBOUNCE_MS 200

// Visual target FPS
#define TARGET_FPS 120
#define FRAME_MS (1000.0f / TARGET_FPS)

// ALSA mixer globals
static snd_mixer_t *g_mixer = NULL;
static snd_mixer_elem_t *g_selem = NULL;
static long g_vol_min = 0, g_vol_max = 0;

// ring buffer for float samples (2 seconds)
static float *g_ring = NULL;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;

// pixel buffer (one value per pixel column)
static float *g_pixels = NULL;
static int g_win_w = 1280, g_win_h = 720;

// limiter state
static float g_limiter_gain = 1.0f;

// kick detection state
static float g_lowpass_state = 0.0f;
static float g_prev_rms = 0.0f;
static Uint32 g_last_kick_ms = 0;
static bool g_kick_state = false;

// FFT buffers (preallocated)
static double fft_re[FFT_SIZE];
static double fft_im[FFT_SIZE];
static double tw_re[FFT_SIZE/2];
static double tw_im[FFT_SIZE/2];
static double window[FFT_SIZE];

// helper: HSV->RGB (0..1)
static void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b) {
    float c = v * s;
    float hh = h * 6.0f;
    float x = c * (1.0f - fabsf(fmodf(hh, 2.0f) - 1.0f));
    float m = v - c;
    float rf=0, gf=0, bf=0;
    if (hh >= 0 && hh < 1) { rf = c; gf = x; bf = 0; }
    else if (hh < 2) { rf = x; gf = c; bf = 0; }
    else if (hh < 3) { rf = 0; gf = c; bf = x; }
    else if (hh < 4) { rf = 0; gf = x; bf = c; }
    else if (hh < 5) { rf = x; gf = 0; bf = c; }
    else { rf = c; gf = 0; bf = x; }
    *r = rf + m; *g = gf + m; *b = bf + m;
}

// ALSA mixer: find capture control
static bool alsa_find_capture_control(const char *name) {
    snd_mixer_selem_id_t *sid;
    if (snd_mixer_open(&g_mixer, 0) < 0) return false;
    if (snd_mixer_attach(g_mixer, "default") < 0) { snd_mixer_close(g_mixer); g_mixer = NULL; return false; }
    snd_mixer_selem_register(g_mixer, NULL, NULL);
    if (snd_mixer_load(g_mixer) < 0) { snd_mixer_close(g_mixer); g_mixer = NULL; return false; }
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, name);
    g_selem = snd_mixer_find_selem(g_mixer, sid);
    if (!g_selem) return false;
    if (snd_mixer_selem_has_capture_volume(g_selem)) {
        snd_mixer_selem_get_capture_volume_range(g_selem, &g_vol_min, &g_vol_max);
        return true;
    }
    return false;
}

static void alsa_set_capture_all(long v) {
    if (!g_selem) return;
    if (v < g_vol_min) v = g_vol_min;
    if (v > g_vol_max) v = g_vol_max;
    snd_mixer_selem_set_capture_volume_all(g_selem, v);
}

// compute RMS of int16 buffer
static float compute_rms_s16(const int16_t *buf, size_t n) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = buf[i] / 32768.0;
        s += v * v;
    }
    return (float)sqrt(s / (double)n);
}

// push int16 samples into ring as float
static void push_to_ring_from_s16(const int16_t *in, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        g_ring[g_ring_pos] = in[i] / 32768.0f;
        g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
    }
}

// compute RMS from ring last n samples
static float compute_rms_from_ring(size_t n) {
    if (n == 0) return 0.0f;
    if (n > g_ring_cap) n = g_ring_cap;
    size_t start = (g_ring_pos + g_ring_cap - n) % g_ring_cap;
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = g_ring[(start + i) % g_ring_cap];
        s += v * v;
    }
    return (float)sqrt(s / (double)n);
}

// update pixel buffer mapping last 2s -> g_win_w columns
static void update_pixel_buffer() {
    if (!g_pixels) return;
    size_t spp = g_ring_cap / (size_t)g_win_w;
    if (spp < 1) spp = 1;
    size_t start_sample = (g_ring_pos + g_ring_cap - g_ring_cap) % g_ring_cap;
    for (int px = 0; px < g_win_w; ++px) {
        size_t sample_index = (start_sample + (size_t)px * spp) % g_ring_cap;
        double sum = 0.0;
        for (size_t s = 0; s < spp; ++s) {
            sum += g_ring[(sample_index + s) % g_ring_cap];
        }
        g_pixels[px] = (float)(sum / (double)spp);
    }
}

// prepare FFT twiddles and window (Hann)
static void fft_prepare() {
    for (int k = 0; k < FFT_SIZE/2; ++k) {
        double angle = -2.0 * M_PI * k / (double)FFT_SIZE;
        tw_re[k] = cos(angle);
        tw_im[k] = sin(angle);
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        // Hann window
        window[n] = 0.5 * (1.0 - cos(2.0 * M_PI * n / (FFT_SIZE - 1)));
    }
}

// bit reversal permutation
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

// iterative Cooley-Tukey radix-2 FFT (in-place), forward transform
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

// compute magnitude spectrum from ring last FFT_SIZE samples
// returns energy sum in freq range [f_lo, f_hi]
static double fft_compute_bass_energy(unsigned sample_rate, double f_lo, double f_hi) {
    // copy last FFT_SIZE samples into fft_re, apply window
    size_t start = (g_ring_pos + g_ring_cap - FFT_SIZE) % g_ring_cap;
    for (int n = 0; n < FFT_SIZE; ++n) {
        double v = g_ring[(start + n) % g_ring_cap];
        fft_re[n] = v * window[n];
        fft_im[n] = 0.0;
    }
    // execute FFT
    fft_execute(fft_re, fft_im);
    // compute magnitude and sum bins in range
    double bin_width = (double)sample_rate / (double)FFT_SIZE;
    int bin_lo = (int)floor(f_lo / bin_width);
    int bin_hi = (int)ceil(f_hi / bin_width);
    if (bin_lo < 0) bin_lo = 0;
    if (bin_hi > FFT_SIZE/2 - 1) bin_hi = FFT_SIZE/2 - 1;
    double sum = 0.0;
    for (int b = bin_lo; b <= bin_hi; ++b) {
        double re = fft_re[b];
        double im = fft_im[b];
        double mag = sqrt(re*re + im*im);
        sum += mag;
    }
    return sum;
}

// detect kick using FFT-based bass energy onset detection
static bool detect_kick_fft(unsigned sample_rate, double *running_avg, double alpha_avg) {
    // compute bass energy in 20..150 Hz
    double energy = fft_compute_bass_energy(sample_rate, 20.0, 150.0);
    // update running average (exponential)
    *running_avg = alpha_avg * energy + (1.0 - alpha_avg) * (*running_avg);
    // onset if energy significantly above average
    bool kick = false;
    if (energy > (*running_avg) * 3.0 && energy > 1e-6) { // factor 3 threshold (tunable)
        Uint32 now = SDL_GetTicks();
        if ((now - g_last_kick_ms) > KICK_DEBOUNCE_MS) {
            kick = true;
            g_last_kick_ms = now;
        }
    }
    return kick;
}

// OpenGL view setup
static void gl_setup_view(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)w, (double)h, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(void) {
    const float TARGET_LEVEL = powf(10.0f, -TARGET_DB / 20.0f);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // create OpenGL window, maximized
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_Window *win = SDL_CreateWindow("Beat Visualizer FFT", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       g_win_w, g_win_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win) { fprintf(stderr, "Window error: %s\n", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_MaximizeWindow(win);
    SDL_GetWindowSize(win, &g_win_w, &g_win_h);

    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) { fprintf(stderr, "GL context error: %s\n", SDL_GetError()); SDL_DestroyWindow(win); SDL_Quit(); return 1; }
    SDL_GL_SetSwapInterval(0); // no vsync for max FPS
    gl_setup_view(g_win_w, g_win_h);

    // find ALSA capture control
    if (!alsa_find_capture_control("Capture")) {
        if (!alsa_find_capture_control("Mic")) {
            fprintf(stderr, "No ALSA capture control found. Exiting.\n");
            SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
            return 2;
        }
    }

    // open ALSA PCM capture
    snd_pcm_t *pcm = NULL;
    int err = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(err));
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
        return 3;
    }

    // configure HW params
    snd_pcm_hw_params_t *hw;
    snd_pcm_hw_params_malloc(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
    unsigned int rate = DEFAULT_SR;
    snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0);
    snd_pcm_hw_params_set_channels(pcm, hw, 1);
    snd_pcm_uframes_t period = BUFFERSIZE;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    snd_pcm_uframes_t buffer = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer);
    if ((err = snd_pcm_hw_params(pcm, hw)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params: %s\n", snd_strerror(err));
        snd_pcm_hw_params_free(hw);
        snd_pcm_close(pcm);
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
        return 4;
    }
    snd_pcm_hw_params_free(hw);
    snd_pcm_prepare(pcm);

    snd_pcm_uframes_t actual_period;
    snd_pcm_get_params(pcm, &buffer, &actual_period);
    fprintf(stderr, "ALSA opened: rate=%u period=%lu buffer=%lu\n", rate, (unsigned long)actual_period, (unsigned long)buffer);

    // allocate ring buffer (2 seconds)
    g_ring_cap = (size_t)rate * 2;
    g_ring = (float*)calloc(g_ring_cap, sizeof(float));
    if (!g_ring) { fprintf(stderr, "Out of memory\n"); snd_pcm_close(pcm); if (g_mixer) snd_mixer_close(g_mixer); SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // allocate pixel buffer
    g_pixels = (float*)calloc((size_t)g_win_w, sizeof(float));
    if (!g_pixels) { free(g_ring); snd_pcm_close(pcm); if (g_mixer) snd_mixer_close(g_mixer); SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // allocate read buffer
    int16_t *read_buf = (int16_t*)malloc(sizeof(int16_t) * BUFFERSIZE);
    if (!read_buf) { free(g_pixels); free(g_ring); snd_pcm_close(pcm); if (g_mixer) snd_mixer_close(g_mixer); SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // prepare FFT twiddles and window
    fft_prepare();

    // precompute RMS windows
    size_t rms100 = (rate * 100) / 1000; if (rms100 < 16) rms100 = 16;
    size_t rms200 = (rate * 200) / 1000; if (rms200 < 16) rms200 = 16;
    size_t rms500 = (rate * 500) / 1000; if (rms500 < 16) rms500 = 16;

    // initial hardware volume mid
    long initial = g_vol_min + (g_vol_max - g_vol_min) / 2;
    alsa_set_capture_all(initial);

    // running average for FFT energy
    double running_avg = 1e-8;
    const double alpha_avg = 0.05; // smoothing for running average

    bool running = true;
    SDL_Event ev;
    Uint32 last_frame = SDL_GetTicks();
    Uint32 last_dbg = 0;

    while (running) {
        // handle events
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) running = false;
            else if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                g_win_w = ev.window.data1;
                g_win_h = ev.window.data2;
                float *tmp = (float*)realloc(g_pixels, sizeof(float) * (size_t)g_win_w);
                if (tmp) g_pixels = tmp;
                gl_setup_view(g_win_w, g_win_h);
            }
        }

        // read from ALSA (blocking read of BUFFERSIZE frames)
        snd_pcm_sframes_t frames = snd_pcm_readi(pcm, read_buf, BUFFERSIZE);
        if (frames == -EPIPE) {
            snd_pcm_prepare(pcm);
            continue;
        } else if (frames < 0) {
            frames = snd_pcm_recover(pcm, frames, 0);
            if (frames < 0) {
                fprintf(stderr, "snd_pcm_readi failed: %s\n", snd_strerror((int)frames));
                break;
            }
            continue;
        } else if (frames == 0) {
            SDL_Delay(1);
            continue;
        }

        // compute RMS of chunk
        float chunk_rms = compute_rms_s16(read_buf, (size_t)frames);

        // desired gain for 24 dB headroom
        float desired_gain = TARGET_LEVEL / fmaxf(chunk_rms, 1e-12f);
        if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;

        // smoothing
        if (desired_gain > g_limiter_gain) g_limiter_gain = ATTACK_FAST * desired_gain + (1.0f - ATTACK_FAST) * g_limiter_gain;
        else g_limiter_gain = RELEASE_SLOW * desired_gain + (1.0f - RELEASE_SLOW) * g_limiter_gain;

        // map to hardware control
        long setv;
        if (g_limiter_gain >= 1.0f) setv = g_vol_max;
        else {
            float gmap = fmaxf(0.0f, g_limiter_gain);
            setv = g_vol_min + (long)((g_vol_max - g_vol_min) * gmap + 0.5f);
        }
        alsa_set_capture_all(setv);

        // push samples into ring
        push_to_ring_from_s16(read_buf, (size_t)frames);

        // update pixel buffer
        update_pixel_buffer();

        // FFT-based kick detection
        bool kick = detect_kick_fft(rate, &running_avg, alpha_avg);

        // background color
        float rms200v = compute_rms_from_ring(rms200);
        float bg_r = 0.0f, bg_g = 0.0f, bg_b = 0.0f;
        if (kick) {
            bg_r = bg_g = bg_b = 1.0f;
            g_kick_state = true;
        } else {
            if (rms200v < 0.005f) { bg_r = bg_g = bg_b = 0.0f; }
            else {
                float v = fminf(0.31f, 0.08f + rms200v * 1.6f);
                bg_r = bg_g = bg_b = v;
            }
            g_kick_state = false;
        }

        // render with OpenGL
        glClearColor(bg_r, bg_g, bg_b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // waveform color via HSV (500ms RMS)
        float rms500v = compute_rms_from_ring(rms500);
        float hue = fminf(0.7f, rms500v * 5.0f);
        float wr, wg, wb;
        hsv_to_rgb(hue, 1.0f, 1.0f, &wr, &wg, &wb);

        int cy = g_win_h / 2;
        float vscale = (float)(g_win_h / 2) * 0.9f;

        // center line
        glColor3f(wr*0.5f, wg*0.5f, wb*0.5f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, (float)cy);
        glVertex2f((float)g_win_w, (float)cy);
        glEnd();

        // waveform (line strip)
        glColor3f(wr, wg, wb);
        glBegin(GL_LINE_STRIP);
        for (int x = 0; x < g_win_w; ++x) {
            float v = g_pixels[x];
            float y = (float)cy - v * vscale;
            glVertex2f((float)x, y);
        }
        glEnd();

        // kick frame
        if (g_kick_state) {
            glColor3f(1.0f, 1.0f, 1.0f);
            float x0 = g_win_w * 0.125f, y0 = g_win_h * 0.125f;
            float x1 = g_win_w * 0.875f, y1 = g_win_h * 0.875f;
            glBegin(GL_LINE_LOOP);
            glVertex2f(x0, y0); glVertex2f(x1, y0); glVertex2f(x1, y1); glVertex2f(x0, y1);
            glEnd();
        }

        // gain bar
        float disp_gain = g_limiter_gain; if (disp_gain > 1.0f) disp_gain = 1.0f;
        float barw = 200.0f * disp_gain;
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(10.0f, 10.0f); glVertex2f(10.0f + barw, 10.0f);
        glVertex2f(10.0f + barw, 18.0f); glVertex2f(10.0f, 18.0f);
        glEnd();

        SDL_GL_SwapWindow(win);

        // debug print once per second
        Uint32 now = SDL_GetTicks();
        if (now - last_dbg > 1000) {
            fprintf(stderr, "rate=%u FFT=%d limiter_gain=%.3f running_avg=%.6f\n", rate, FFT_SIZE, g_limiter_gain, running_avg);
            last_dbg = now;
        }

        // frame timing cap
        Uint32 frame_end = SDL_GetTicks();
        float elapsed = (float)(frame_end - last_frame);
        if (elapsed < FRAME_MS) SDL_Delay((Uint32)fmaxf(1.0f, FRAME_MS - elapsed));
        last_frame = SDL_GetTicks();
    }

    // cleanup
    free(read_buf);
    free(g_pixels);
    free(g_ring);
    snd_pcm_close(pcm);
    if (g_mixer) snd_mixer_close(g_mixer);
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
