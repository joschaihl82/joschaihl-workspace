// beatvis_alsa.c
// Kompiliere mit: gcc -O2 -o beatvis_alsa beatvis_alsa.c -lSDL2 -lasound -lm
// Kombiniert SDL2 Audio Capture + Visualisierung mit ALSA Mixer basierendem Limiter.
// Falls kein Capture-Regler vorhanden ist, wird Software-Gain als Fallback verwendet.

#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define WINDOW_W 1280
#define WINDOW_H 720

// Audio / Ringbuffer
static int g_sample_rate = 48000;
static SDL_AudioFormat g_format = AUDIO_F32SYS;
static int g_channels = 1;
static SDL_AudioDeviceID g_dev = 0;

static float *sample_ring = NULL;
static size_t ring_capacity = 0;
static size_t ring_write_pos = 0;
static float *pixel_buffer = NULL;

// Kick detection state
static float lowpass_state = 0.0f;
static float prev_rms = 0.0f;
static Uint32 last_kick_ms = 0;
static bool kick_state = false;

// Parameters
const float LOWPASS_ALPHA = 0.02f;
const float KICK_RMS_THRESHOLD = 0.08f;
const Uint32 KICK_DEBOUNCE_MS = 200;

// ALSA mixer
static snd_mixer_t *mixer = NULL;
static snd_mixer_elem_t *selem = NULL;
static long vol_min = 0, vol_max = 0;
static bool has_hw_capture = false;

// Limiter state
static float limiter_gain = 1.0f;
const float TARGET_LEVEL = 0.5f; // 50% headroom
const float MAX_GAIN = 4.0f;
const float ATTACK_FAST = 0.6f;
const float RELEASE_SLOW = 0.02f;

// HSV -> RGB
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

// Ringbuffer helpers
static void push_samples_to_ring(const float *samples, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        sample_ring[ring_write_pos] = samples[i];
        ring_write_pos = (ring_write_pos + 1) % ring_capacity;
    }
}

static float compute_rms_window(const float *buf, size_t len) {
    double sum = 0.0;
    for (size_t i = 0; i < len; ++i) {
        double v = buf[i];
        sum += v * v;
    }
    return (float)sqrt(sum / (double)len);
}

static void read_last_samples(float *out, size_t n) {
    if (n > ring_capacity) n = ring_capacity;
    size_t start = (ring_write_pos + ring_capacity - n) % ring_capacity;
    for (size_t i = 0; i < n; ++i) {
        out[i] = sample_ring[(start + i) % ring_capacity];
    }
}

static void update_pixel_buffer_from_ring() {
    size_t samples_per_pixel = ring_capacity / WINDOW_W;
    if (samples_per_pixel < 1) samples_per_pixel = 1;
    size_t start_sample = (ring_write_pos + ring_capacity - ring_capacity) % ring_capacity;
    for (int px = 0; px < WINDOW_W; ++px) {
        size_t sample_index = (start_sample + (size_t)px * samples_per_pixel) % ring_capacity;
        double sum = 0.0;
        for (size_t s = 0; s < samples_per_pixel; ++s) {
            sum += sample_ring[(sample_index + s) % ring_capacity];
        }
        pixel_buffer[px] = (float)(sum / (double)samples_per_pixel);
    }
}

// Kick detection (wie im Original)
static bool detect_kick() {
    size_t window_ms = 100;
    size_t window_samples = (size_t)((g_sample_rate * window_ms) / 1000);
    if (window_samples < 16) window_samples = 16;
    float *tmp = (float*)malloc(sizeof(float) * window_samples);
    if (!tmp) return false;
    read_last_samples(tmp, window_samples);
    float lp = lowpass_state;
    for (size_t i = 0; i < window_samples; ++i) {
        lp = lp + LOWPASS_ALPHA * (tmp[i] - lp);
    }
    lowpass_state = lp;
    double sum = 0.0;
    for (size_t i = 0; i < window_samples; ++i) {
        double v = tmp[i] - lp;
        sum += v * v;
    }
    float rms = (float)sqrt(sum / (double)window_samples);
    free(tmp);
    Uint32 now = SDL_GetTicks();
    bool kick = false;
    if (rms > KICK_RMS_THRESHOLD && (rms - prev_rms) > 0.01f && (now - last_kick_ms) > KICK_DEBOUNCE_MS) {
        kick = true;
        last_kick_ms = now;
    }
    prev_rms = rms;
    return kick;
}

// ALSA mixer open/find
static bool alsa_mixer_open_find(const char *card, const char *name) {
    snd_mixer_selem_id_t *sid;
    if (snd_mixer_open(&mixer, 0) < 0) return false;
    if (snd_mixer_attach(mixer, card ? card : "default") < 0) { snd_mixer_close(mixer); mixer = NULL; return false; }
    snd_mixer_selem_register(mixer, NULL, NULL);
    if (snd_mixer_load(mixer) < 0) { snd_mixer_close(mixer); mixer = NULL; return false; }
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, name);
    selem = snd_mixer_find_selem(mixer, sid);
    if (!selem) return false;
    if (snd_mixer_selem_has_capture_volume(selem)) {
        snd_mixer_selem_get_capture_volume_range(selem, &vol_min, &vol_max);
        has_hw_capture = true;
    } else has_hw_capture = false;
    return true;
}

static void alsa_set_capture_volume_all(long v) {
    if (!selem) return;
    if (v < vol_min) v = vol_min;
    if (v > vol_max) v = vol_max;
    snd_mixer_selem_set_capture_volume_all(selem, v);
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init Fehler: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Beat Visualizer ALSA Limiter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    if (!win) { fprintf(stderr, "Window Fehler: %s\n", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) { fprintf(stderr, "Renderer Fehler: %s\n", SDL_GetError()); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    // Versuche ALSA Mixer "Capture" oder "Mic"
    if (!alsa_mixer_open_find(NULL, "Capture")) {
        if (!alsa_mixer_open_find(NULL, "Mic")) {
            // kein HW-Capture-Regler gefunden; Fallback auf Software-Gain
            has_hw_capture = false;
            if (mixer) { snd_mixer_close(mixer); mixer = NULL; }
            selem = NULL;
        }
    }

    // SDL Audio Capture
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 48000;
    want.format = AUDIO_F32SYS;
    want.channels = 1;
    want.samples = 1024;
    want.callback = NULL;
    const char *devname = SDL_GetAudioDeviceName(0, SDL_TRUE);
    if (!devname) devname = NULL;
    g_dev = SDL_OpenAudioDevice(devname, SDL_TRUE, &want, &have, 0);
    if (g_dev == 0) {
        fprintf(stderr, "Konnte Audioaufnahme nicht öffnen: %s\n", SDL_GetError());
        if (mixer) { snd_mixer_close(mixer); mixer = NULL; }
        SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
        return 1;
    }
    g_sample_rate = have.freq;
    g_format = have.format;
    g_channels = have.channels;

    // Ringbuffer 2 Sekunden
    ring_capacity = (size_t)g_sample_rate * 2;
    sample_ring = (float*)calloc(ring_capacity, sizeof(float));
    if (!sample_ring) { fprintf(stderr, "Speicherfehler\n"); SDL_CloseAudioDevice(g_dev); if (mixer) snd_mixer_close(mixer); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit(); return 1; }
    pixel_buffer = (float*)calloc(WINDOW_W, sizeof(float));
    if (!pixel_buffer) { free(sample_ring); SDL_CloseAudioDevice(g_dev); if (mixer) snd_mixer_close(mixer); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    SDL_PauseAudioDevice(g_dev, 0);

    const int CHUNK = 2048;
    float *audio_chunk = (float*)malloc(sizeof(float) * CHUNK);
    if (!audio_chunk) { fprintf(stderr, "Speicherfehler chunk\n"); SDL_PauseAudioDevice(g_dev,1); SDL_CloseAudioDevice(g_dev); free(sample_ring); free(pixel_buffer); if (mixer) snd_mixer_close(mixer); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    bool running = true;
    SDL_Event e;
    const int center_y = WINDOW_H / 2;
    const float vertical_scale = (float)(WINDOW_H / 2) * 0.9f;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        // Lese verfügbare Samples
        Uint32 bytes_available = SDL_GetQueuedAudioSize(g_dev);
        if (bytes_available > 0) {
            size_t samples_available = bytes_available / sizeof(float);
            while (samples_available > 0) {
                size_t to_read = samples_available > CHUNK ? CHUNK : samples_available;
                Uint32 bytes_to_read = (Uint32)(to_read * sizeof(float));
                Uint32 got = SDL_DequeueAudio(g_dev, audio_chunk, bytes_to_read);
                if (got == 0) break;
                size_t got_samples = got / sizeof(float);

                // Messen RMS über Chunk
                float rms = compute_rms_window(audio_chunk, got_samples);
                float desired_gain = TARGET_LEVEL / fmaxf(rms, 1e-6f);
                if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;

                // Asymmetrisches Smoothing
                if (desired_gain > limiter_gain) limiter_gain = ATTACK_FAST * desired_gain + (1.0f - ATTACK_FAST) * limiter_gain;
                else limiter_gain = RELEASE_SLOW * desired_gain + (1.0f - RELEASE_SLOW) * limiter_gain;

                // Hardware-Regler wenn vorhanden
                if (has_hw_capture && selem) {
                    float gmap = fminf(limiter_gain, 1.0f); // map >1 auf max
                    long setv = vol_min + (long)((vol_max - vol_min) * gmap);
                    alsa_set_capture_volume_all(setv);
                } else {
                    // Software-Gain Fallback (in-place)
                    for (size_t i = 0; i < got_samples; ++i) {
                        float s = audio_chunk[i] * limiter_gain;
                        if (s > 1.0f) s = 1.0f;
                        if (s < -1.0f) s = -1.0f;
                        audio_chunk[i] = s;
                    }
                }

                // Push in ringbuffer (für Visualisierung)
                push_samples_to_ring(audio_chunk, got_samples);
                samples_available -= got_samples;
            }
        }

        // Update visualization buffers
        update_pixel_buffer_from_ring();
        bool kick = detect_kick();

        // Hintergrundfarbe: Kick -> weiß, Stille -> schwarz, sonst dunkles Grau
        Uint8 bg_r=0, bg_g=0, bg_b=0;
        if (kick) {
            bg_r = bg_g = bg_b = 255;
            kick_state = true;
        } else {
            size_t rms_ms = 200;
            size_t rms_samples = (size_t)((g_sample_rate * rms_ms) / 1000);
            if (rms_samples < 16) rms_samples = 16;
            float *tmp = (float*)malloc(sizeof(float) * rms_samples);
            read_last_samples(tmp, rms_samples);
            float rms = compute_rms_window(tmp, rms_samples);
            free(tmp);
            if (rms < 0.005f) bg_r = bg_g = bg_b = 0;
            else {
                int v = (int)fminf(80.0f, 20.0f + rms * 400.0f);
                bg_r = bg_g = bg_b = (Uint8)v;
            }
            kick_state = false;
        }

        SDL_SetRenderDrawColor(ren, bg_r, bg_g, bg_b, 255);
        SDL_RenderClear(ren);

        // Waveform-Farbe via HSV (Hue abhängig von RMS 500ms)
        size_t rms_ms2 = 500;
        size_t rms_samples2 = (size_t)((g_sample_rate * rms_ms2) / 1000);
        if (rms_samples2 < 16) rms_samples2 = 16;
        float *tmp2 = (float*)malloc(sizeof(float) * rms_samples2);
        read_last_samples(tmp2, rms_samples2);
        float rms2 = compute_rms_window(tmp2, rms_samples2);
        free(tmp2);
        float hue = fminf(0.7f, rms2 * 5.0f);
        Uint8 wr, wg, wb;
        hsv_to_rgb(hue, 1.0f, 1.0f, &wr, &wg, &wb);

        // Mittelachse
        SDL_SetRenderDrawColor(ren, wr/2, wg/2, wb/2, 255);
        SDL_RenderDrawLine(ren, 0, center_y, WINDOW_W, center_y);

        // Waveform: älteste links, neueste rechts
        SDL_SetRenderDrawColor(ren, wr, wg, wb, 255);
        for (int x = 1; x < WINDOW_W; ++x) {
            float v1 = pixel_buffer[x-1];
            float v2 = pixel_buffer[x];
            int y1 = center_y - (int)(v1 * vertical_scale);
            int y2 = center_y - (int)(v2 * vertical_scale);
            SDL_RenderDrawLine(ren, x-1, y1, x, y2);
        }

        // Kick-Rahmen wenn getriggert
        if (kick_state) {
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_Rect r = { WINDOW_W/8, WINDOW_H/8, WINDOW_W*3/4, WINDOW_H*3/4 };
            SDL_RenderDrawRect(ren, &r);
        }

        // Anzeige aktueller Limiter-Gain (kleine Leiste oben links)
        int barw = (int)(fminf(limiter_gain, 1.0f) * 200.0f);
        SDL_Rect gainrect = {10, 10, barw, 8};
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
        SDL_RenderFillRect(ren, &gainrect);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    // Cleanup
    SDL_PauseAudioDevice(g_dev, 1);
    SDL_CloseAudioDevice(g_dev);
    if (mixer) { snd_mixer_close(mixer); mixer = NULL; }
    free(audio_chunk);
    free(sample_ring);
    free(pixel_buffer);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
