// beatvis_alsa_only.c
// gcc -O2 -o beatvis_alsa_only beatvis_alsa_only.c -lSDL2 -lasound -lm
// ALSA PCM capture only + ALSA mixer control + SDL2 rendering
// BUFFERSIZE = 512, Headroom = 24 dB, hardware-only gain, dynamic window scaling.

#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFFERSIZE 256

// Visual target FPS
#define TARGET_FPS 120
#define FRAME_MS (1000.0f / TARGET_FPS)

// Default desired sample rate
#define DEFAULT_SR 48000

// Headroom
const float TARGET_DB = 24.0f;

// Limiter params
const float MAX_GAIN = 4.0f;
const float ATTACK_FAST = 0.6f;
const float RELEASE_SLOW = 0.02f;

// Kick detection params
const float LOWPASS_ALPHA = 0.02f;
const float KICK_RMS_THRESHOLD = 0.08f;
const unsigned KICK_DEBOUNCE_MS = 200;

// Globals for ALSA mixer
static snd_mixer_t *mixer = NULL;
static snd_mixer_elem_t *selem = NULL;
static long vol_min = 0, vol_max = 0;

// Ringbuffer (float mono)
static float *ring = NULL;
static size_t ring_cap = 0;
static size_t ring_pos = 0;

// Pixel buffer (one value per pixel column) allocated dynamically to window width
static float *pixel_buf = NULL;
static int win_w = 1280, win_h = 720;

// State
static float limiter_gain = 1.0f;
static float lowpass_state = 0.0f;
static float prev_rms = 0.0f;
static Uint32 last_kick_ms = 0;
static bool kick_state = false;

// Utility: HSV -> RGB
static void hsv_to_rgb(float h, float s, float v, Uint8 *r, Uint8 *g, Uint8 *b) {
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
    *r = (Uint8)fminf(255.0f, (rf + m) * 255.0f);
    *g = (Uint8)fminf(255.0f, (gf + m) * 255.0f);
    *b = (Uint8)fminf(255.0f, (bf + m) * 255.0f);
}

// ALSA mixer: find capture control by name
static bool alsa_find_capture_control(const char *name) {
    snd_mixer_selem_id_t *sid;
    if (snd_mixer_open(&mixer, 0) < 0) return false;
    if (snd_mixer_attach(mixer, "default") < 0) { snd_mixer_close(mixer); mixer = NULL; return false; }
    snd_mixer_selem_register(mixer, NULL, NULL);
    if (snd_mixer_load(mixer) < 0) { snd_mixer_close(mixer); mixer = NULL; return false; }
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, name);
    selem = snd_mixer_find_selem(mixer, sid);
    if (!selem) return false;
    if (snd_mixer_selem_has_capture_volume(selem)) {
        snd_mixer_selem_get_capture_volume_range(selem, &vol_min, &vol_max);
        return true;
    }
    return false;
}

static void alsa_set_capture_all(long v) {
    if (!selem) return;
    if (v < vol_min) v = vol_min;
    if (v > vol_max) v = vol_max;
    snd_mixer_selem_set_capture_volume_all(selem, v);
}

// compute RMS of int16 buffer converted to float [-1..1]
static float compute_rms_s16(const int16_t *buf, size_t n) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = buf[i] / 32768.0;
        s += v * v;
    }
    return (float)sqrt(s / (double)n);
}

// push float samples into ring
static void push_to_ring_from_float(const float *in, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        ring[ring_pos] = in[i];
        ring_pos = (ring_pos + 1) % ring_cap;
    }
}

// push int16 samples converted to float into ring
static void push_to_ring_from_s16(const int16_t *in, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        ring[ring_pos] = in[i] / 32768.0f;
        ring_pos = (ring_pos + 1) % ring_cap;
    }
}

// compute RMS from ring last n samples
static float compute_rms_from_ring(size_t n) {
    if (n == 0) return 0.0f;
    if (n > ring_cap) n = ring_cap;
    size_t start = (ring_pos + ring_cap - n) % ring_cap;
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = ring[(start + i) % ring_cap];
        s += v * v;
    }
    return (float)sqrt(s / (double)n);
}

// update pixel buffer mapping last 2s -> win_w columns
static void update_pixel_buffer() {
    if (!pixel_buf) return;
    size_t samples_per_pixel = ring_cap / (size_t)win_w;
    if (samples_per_pixel < 1) samples_per_pixel = 1;
    size_t start_sample = (ring_pos + ring_cap - ring_cap) % ring_cap;
    for (int px = 0; px < win_w; ++px) {
        size_t idx = (start_sample + (size_t)px * samples_per_pixel) % ring_cap;
        double sum = 0.0;
        for (size_t s = 0; s < samples_per_pixel; ++s) {
            sum += ring[(idx + s) % ring_cap];
        }
        pixel_buf[px] = (float)(sum / (double)samples_per_pixel);
    }
}

// detect kick using last ~100ms RMS with simple lowpass
static bool detect_kick(size_t sample_rate) {
    size_t window_samples = (sample_rate * 100) / 1000;
    if (window_samples < 16) window_samples = 16;
    size_t start = (ring_pos + ring_cap - window_samples) % ring_cap;
    float lp = lowpass_state;
    for (size_t i = 0; i < window_samples; ++i) {
        float v = ring[(start + i) % ring_cap];
        lp = lp + LOWPASS_ALPHA * (v - lp);
    }
    lowpass_state = lp;
    double sum = 0.0;
    for (size_t i = 0; i < window_samples; ++i) {
        double v = ring[(start + i) % ring_cap] - lp;
        sum += v * v;
    }
    float rms = (float)sqrt(sum / (double)window_samples);
    Uint32 now = SDL_GetTicks();
    bool kick = false;
    if (rms > KICK_RMS_THRESHOLD && (rms - prev_rms) > 0.01f && (now - last_kick_ms) > KICK_DEBOUNCE_MS) {
        kick = true;
        last_kick_ms = now;
    }
    prev_rms = rms;
    return kick;
}

int main(void) {
    // compute target linear level from 24 dB headroom
    const float TARGET_LEVEL = powf(10.0f, -TARGET_DB / 20.0f);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create resizable window and maximize
    SDL_Window *win = SDL_CreateWindow("Beat Visualizer ALSA Only", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, SDL_WINDOW_RESIZABLE);
    if (!win) { fprintf(stderr, "Window error: %s\n", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_MaximizeWindow(win);
    SDL_GetWindowSize(win, &win_w, &win_h);

    // Create renderer (no VSYNC to allow high FPS; change if needed)
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) { fprintf(stderr, "Renderer error: %s\n", SDL_GetError()); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // find ALSA capture control
    if (!alsa_find_capture_control("Capture")) {
        if (!alsa_find_capture_control("Mic")) {
            fprintf(stderr, "No ALSA capture control found. Exiting (no software fallback).\n");
            SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
            return 2;
        }
    }

    // Open ALSA PCM capture device (default)
    snd_pcm_t *pcm = NULL;
    const char *pcm_name = "default";
    int err = snd_pcm_open(&pcm, pcm_name, SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_open error: %s\n", snd_strerror(err));
        if (mixer) snd_mixer_close(mixer);
        SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
        return 3;
    }

    // Set HW params
    snd_pcm_hw_params_t *hw;
    snd_pcm_hw_params_malloc(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    // use 16-bit signed little endian (widely supported)
    snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
    unsigned int rate = DEFAULT_SR;
    snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0);
    snd_pcm_hw_params_set_channels(pcm, hw, 1);
    // set period size to BUFFERSIZE frames
    snd_pcm_uframes_t period = BUFFERSIZE;
    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
    // set buffer size to 4 * period (or nearest)
    snd_pcm_uframes_t buffer = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer);
    if ((err = snd_pcm_hw_params(pcm, hw)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params error: %s\n", snd_strerror(err));
        snd_pcm_hw_params_free(hw);
        snd_pcm_close(pcm);
        if (mixer) snd_mixer_close(mixer);
        SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
        return 4;
    }
    snd_pcm_hw_params_free(hw);

    // prepare PCM
    if ((err = snd_pcm_prepare(pcm)) < 0) {
        fprintf(stderr, "snd_pcm_prepare error: %s\n", snd_strerror(err));
    }

    // actual frames per period (may differ)
    snd_pcm_uframes_t actual_period;
    snd_pcm_get_params(pcm, &buffer, &actual_period);
    fprintf(stderr, "ALSA opened: rate=%u, period=%lu, buffer=%lu\n", rate, (unsigned long)actual_period, (unsigned long)buffer);

    // allocate ring buffer for 2 seconds
    ring_cap = (size_t)rate * 2;
    ring = (float*)calloc(ring_cap, sizeof(float));
    if (!ring) { fprintf(stderr, "Out of memory ring\n"); snd_pcm_close(pcm); if (mixer) snd_mixer_close(mixer); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // allocate pixel buffer for current window width
    pixel_buf = (float*)calloc(win_w > 0 ? win_w : 1280, sizeof(float));
    if (!pixel_buf) { free(ring); snd_pcm_close(pcm); if (mixer) snd_mixer_close(mixer); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // allocate ALSA read buffer (int16)
    int16_t *read_buf = (int16_t*)malloc(sizeof(int16_t) * BUFFERSIZE);
    if (!read_buf) { free(pixel_buf); free(ring); snd_pcm_close(pcm); if (mixer) snd_mixer_close(mixer); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // precompute RMS window sizes
    size_t rms100 = (rate * 100) / 1000; if (rms100 < 16) rms100 = 16;
    size_t rms200 = (rate * 200) / 1000; if (rms200 < 16) rms200 = 16;
    size_t rms500 = (rate * 500) / 1000; if (rms500 < 16) rms500 = 16;

    // initial hardware volume mid
    long initial = vol_min + (vol_max - vol_min) / 2;
    alsa_set_capture_all(initial);

    Uint32 last_frame = SDL_GetTicks();
    Uint32 last_dbg = 0;
    bool running = true;
    SDL_Event ev;

    while (running) {
        // handle events (resize, quit)
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) running = false;
            else if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                win_w = ev.window.data1;
                win_h = ev.window.data2;
                // reallocate pixel buffer if width changed
                float *tmp = (float*)realloc(pixel_buf, sizeof(float) * (size_t)win_w);
                if (tmp) pixel_buf = tmp;
            }
        }

        // read from ALSA (blocking read of period frames)
        snd_pcm_sframes_t frames = snd_pcm_readi(pcm, read_buf, BUFFERSIZE);
        if (frames == -EPIPE) {
            // overrun
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
            // no data
            SDL_Delay(1);
            continue;
        }

        // compute RMS on captured frames
        float chunk_rms = compute_rms_s16(read_buf, (size_t)frames);

        // compute desired gain to keep peak ~ TARGET_LEVEL (24 dB headroom)
        float desired_gain = TARGET_LEVEL / fmaxf(chunk_rms, 1e-12f);
        if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;

        // smoothing
        if (desired_gain > limiter_gain) limiter_gain = ATTACK_FAST * desired_gain + (1.0f - ATTACK_FAST) * limiter_gain;
        else limiter_gain = RELEASE_SLOW * desired_gain + (1.0f - RELEASE_SLOW) * limiter_gain;

        // map limiter_gain to hardware control (no software gain)
        long setv;
        if (limiter_gain >= 1.0f) setv = vol_max;
        else {
            float gmap = fmaxf(0.0f, limiter_gain); // 0..1
            setv = vol_min + (long)((vol_max - vol_min) * gmap + 0.5f);
        }
        alsa_set_capture_all(setv);

        // push samples into ring (convert to float)
        push_to_ring_from_s16(read_buf, (size_t)frames);

        // update pixel buffer mapping last 2s -> win_w
        update_pixel_buffer();

        // detect kick
        bool kick = detect_kick(rate);

        // render
        // background: white on kick, black on silence, else grey proportional to RMS200
        float rms200 = compute_rms_from_ring(rms200);
        Uint8 bg_r = 0, bg_g = 0, bg_b = 0;
        if (kick) {
            bg_r = bg_g = bg_b = 255;
            kick_state = true;
        } else {
            if (rms200 < 0.005f) { bg_r = bg_g = bg_b = 0; }
            else {
                int v = (int)fminf(80.0f, 20.0f + rms200 * 400.0f);
                bg_r = bg_g = bg_b = (Uint8)v;
            }
            kick_state = false;
        }

        SDL_SetRenderDrawColor(ren, bg_r, bg_g, bg_b, 255);
        SDL_RenderClear(ren);

        // waveform color via HSV (hue from 500ms RMS)
        float rms500v = compute_rms_from_ring(rms500);
        float hue = fminf(0.7f, rms500v * 5.0f);
        Uint8 wr, wg, wb;
        hsv_to_rgb(hue, 1.0f, 1.0f, &wr, &wg, &wb);

        // draw center line
        int center_y = win_h / 2;
        SDL_SetRenderDrawColor(ren, wr/2, wg/2, wb/2, 255);
        SDL_RenderDrawLine(ren, 0, center_y, win_w, center_y);

        // draw waveform (map pixel_buf to current window width/height)
        SDL_SetRenderDrawColor(ren, wr, wg, wb, 255);
        float vscale = (float)(win_h / 2) * 0.9f;
        for (int x = 1; x < win_w; ++x) {
            float v1 = pixel_buf[x-1];
            float v2 = pixel_buf[x];
            int y1 = center_y - (int)(v1 * vscale);
            int y2 = center_y - (int)(v2 * vscale);
            SDL_RenderDrawLine(ren, x-1, y1, x, y2);
        }

        // optional kick frame
        if (kick_state) {
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_Rect r = { win_w/8, win_h/8, win_w*3/4, win_h*3/4 };
            SDL_RenderDrawRect(ren, &r);
        }

        // hardware gain indicator
        float display_gain = limiter_gain;
        if (display_gain > 1.0f) display_gain = 1.0f;
        int barw = (int)(display_gain * 200.0f);
        SDL_Rect gainrect = {10, 10, barw, 8};
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
        SDL_RenderFillRect(ren, &gainrect);

        SDL_RenderPresent(ren);

        // debug print once per second
        Uint32 now = SDL_GetTicks();
        if (now - last_dbg > 1000) {
            float actual_latency_ms = (float)actual_period / (float)rate * 1000.0f;
            fprintf(stderr, "rate=%u period=%lu buffer=%lu latency≈%.2fms limiter_gain=%.3f\n",
                    rate, (unsigned long)actual_period, (unsigned long)buffer, actual_latency_ms, limiter_gain);
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
    free(pixel_buf);
    free(ring);
    snd_pcm_close(pcm);
    if (mixer) snd_mixer_close(mixer);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
