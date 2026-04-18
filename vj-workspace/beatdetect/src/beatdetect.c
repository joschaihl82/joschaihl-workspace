// beatvis.c
// Kompiliere mit: gcc -O2 -o beatvis beatvis.c -lSDL2 -lm

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define WINDOW_W 1280
#define WINDOW_H 720

// Audio settings (werden beim Öffnen des Geräts angepasst)
static int g_sample_rate = 48000;
static SDL_AudioFormat g_format = AUDIO_F32SYS;
static int g_channels = 1;
static SDL_AudioDeviceID g_dev = 0;

// Ringbuffer für 2 Sekunden, in Samples
static float *sample_ring = NULL;
static size_t ring_capacity = 0;
static size_t ring_write_pos = 0;

// Visual buffer: eine Spalte pro Pixelbreite (2 Sekunden -> WINDOW_W)
static float *pixel_buffer = NULL;

// Kick detection state
static float lowpass_state = 0.0f;
static float prev_rms = 0.0f;
static Uint32 last_kick_ms = 0;
static bool kick_state = false;

// Parameters (anpassbar)
const float LOWPASS_ALPHA = 0.02f; // für sehr langsamen Tiefpass (Bass)
const float KICK_RMS_THRESHOLD = 0.08f; // Schwelle für Kick (experimentell)
const Uint32 KICK_DEBOUNCE_MS = 200; // minimale Zeit zwischen Kicks

// HSV zu RGB Konvertierung
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

// Audio callback nicht verwendet, wir lesen mit SDL_DequeueAudio
// Hilfsfunktion: schreibe Samples in Ringbuffer (mono float)
static void push_samples_to_ring(const float *samples, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        sample_ring[ring_write_pos] = samples[i];
        ring_write_pos = (ring_write_pos + 1) % ring_capacity;
    }
}

// Berechne RMS eines Fensters von Samples (mono float)
static float compute_rms_window(const float *buf, size_t len) {
    double sum = 0.0;
    for (size_t i = 0; i < len; ++i) {
        double v = buf[i];
        sum += v * v;
    }
    return (float)sqrt(sum / (double)len);
}

// Lese die letzten N Samples aus dem Ringbuffer in ein temporäres Array (älteste zuerst)
static void read_last_samples(float *out, size_t n) {
    size_t start = (ring_write_pos + ring_capacity - n) % ring_capacity;
    for (size_t i = 0; i < n; ++i) {
        out[i] = sample_ring[(start + i) % ring_capacity];
    }
}

// Mappe die letzten 2 Sekunden auf pixel_buffer (neueste ganz rechts)
static void update_pixel_buffer_from_ring() {
    // ring_capacity entspricht 2 Sekunden in Samples
    size_t samples_per_pixel = ring_capacity / WINDOW_W;
    if (samples_per_pixel < 1) samples_per_pixel = 1;
    // Für jede Spalte berechne Mittelwert (neueste rechts)
    size_t start_sample = (ring_write_pos + ring_capacity - ring_capacity) % ring_capacity; // = ring_write_pos - ring_capacity
    // Wir wollen pixel 0 = links (älteste), pixel WINDOW_W-1 = rechts (neueste)
    for (int px = 0; px < WINDOW_W; ++px) {
        size_t sample_index = (start_sample + (size_t)px * samples_per_pixel) % ring_capacity;
        double sum = 0.0;
        for (size_t s = 0; s < samples_per_pixel; ++s) {
            sum += sample_ring[(sample_index + s) % ring_capacity];
        }
        pixel_buffer[px] = (float)(sum / (double)samples_per_pixel);
    }
}

// Kick Erkennung: einfacher Bass RMS über letzten 100 ms
static bool detect_kick() {
    size_t window_ms = 100;
    size_t window_samples = (size_t)((g_sample_rate * window_ms) / 1000);
    if (window_samples < 16) window_samples = 16;
    float *tmp = (float*)malloc(sizeof(float) * window_samples);
    if (!tmp) return false;
    read_last_samples(tmp, window_samples);
    // Tiefpass (IIR) auf tmp und RMS
    float lp = lowpass_state;
    for (size_t i = 0; i < window_samples; ++i) {
        lp = lp + LOWPASS_ALPHA * (tmp[i] - lp);
    }
    lowpass_state = lp;
    // RMS der tiefpassgeglätteten Werte
    double sum = 0.0;
    for (size_t i = 0; i < window_samples; ++i) {
        double v = tmp[i] - lp;
        sum += v * v;
    }
    float rms = (float)sqrt(sum / (double)window_samples);
    free(tmp);
    Uint32 now = SDL_GetTicks();
    bool kick = false;
    // Trigger wenn RMS über Schwelle und Anstieg
    if (rms > KICK_RMS_THRESHOLD && (rms - prev_rms) > 0.01f && (now - last_kick_ms) > KICK_DEBOUNCE_MS) {
        kick = true;
        last_kick_ms = now;
    }
    prev_rms = rms;
    return kick;
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init Fehler: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Beat Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "Window Fehler: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        fprintf(stderr, "Renderer Fehler: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Öffne Standardaufnahmegerät
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 48000;
    want.format = AUDIO_F32SYS;
    want.channels = 1;
    want.samples = 1024;
    want.callback = NULL; // wir verwenden Pull mit SDL_DequeueAudio

    const char *devname = SDL_GetAudioDeviceName(0, SDL_TRUE);
    if (!devname) devname = NULL;
    g_dev = SDL_OpenAudioDevice(devname, SDL_TRUE, &want, &have, 0);
    if (g_dev == 0) {
        fprintf(stderr, "Konnte Audioaufnahme nicht öffnen: %s\n", SDL_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // Übernehme tatsächliche Parameter
    g_sample_rate = have.freq;
    g_format = have.format;
    g_channels = have.channels;

    // Ringbuffer für 2 Sekunden
    ring_capacity = (size_t)g_sample_rate * 2; // 2 Sekunden
    sample_ring = (float*)calloc(ring_capacity, sizeof(float));
    if (!sample_ring) {
        fprintf(stderr, "Speicherfehler\n");
        SDL_CloseAudioDevice(g_dev);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    pixel_buffer = (float*)calloc(WINDOW_W, sizeof(float));
    if (!pixel_buffer) {
        free(sample_ring);
        SDL_CloseAudioDevice(g_dev);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_PauseAudioDevice(g_dev, 0); // Aufnahme starten

    bool running = true;
    SDL_Event e;
    const int center_y = WINDOW_H / 2;
    const float vertical_scale = (float)(WINDOW_H / 2) * 0.9f; // Skalierung des Signals

    // temporärer Puffer für Dequeue
    const int CHUNK = 2048;
    float *audio_chunk = (float*)malloc(sizeof(float) * CHUNK);
    if (!audio_chunk) { running = false; }

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
            }
        }

        // Lese verfügbare Samples
        Uint32 bytes_available = SDL_GetQueuedAudioSize(g_dev);
        if (bytes_available > 0) {
            // Anzahl Samples (float mono)
            size_t samples_available = bytes_available / sizeof(float);
            // Lese in CHUNK-Blöcken
            while (samples_available > 0) {
                size_t to_read = samples_available > CHUNK ? CHUNK : samples_available;
                Uint32 bytes_to_read = (Uint32)(to_read * sizeof(float));
                // SDL_DequeueAudio liefert Bytes in das Ziel
                Uint32 got = SDL_DequeueAudio(g_dev, audio_chunk, bytes_to_read);
                if (got == 0) break;
                size_t got_samples = got / sizeof(float);
                // Falls Aufnahme mehrere Kanäle liefert, hier Mono-Annahme; SDL liefert mono wenn have.channels==1
                push_samples_to_ring(audio_chunk, got_samples);
                samples_available -= got_samples;
            }
        }

        // Update pixel buffer (2 Sekunden -> WINDOW_W)
        update_pixel_buffer_from_ring();

        // Kick detection
        bool kick = detect_kick();
        Uint8 bg_r=0, bg_g=0, bg_b=0;
        if (kick) {
            // Bildschirm komplett weiß
            bg_r = bg_g = bg_b = 255;
            kick_state = true;
        } else {
            // Wenn keine Lautstärke (sehr niedrige RMS) dann schwarz, sonst dunkles Grau
            // Berechne kurze RMS über 200 ms
            size_t rms_ms = 200;
            size_t rms_samples = (size_t)((g_sample_rate * rms_ms) / 1000);
            if (rms_samples < 16) rms_samples = 16;
            float *tmp = (float*)malloc(sizeof(float) * rms_samples);
            read_last_samples(tmp, rms_samples);
            float rms = compute_rms_window(tmp, rms_samples);
            free(tmp);
            if (rms < 0.005f) {
                bg_r = bg_g = bg_b = 0;
            } else {
                // dunkles Grau proportional zur Lautstärke
                int v = (int)fminf(80.0f, 20.0f + rms * 400.0f);
                bg_r = bg_g = bg_b = (Uint8)v;
            }
            kick_state = false;
        }

        // Render Hintergrund
        SDL_SetRenderDrawColor(ren, bg_r, bg_g, bg_b, 255);
        SDL_RenderClear(ren);

        // Waveform-Farbe aus HSV: Hue abhängig von RMS der letzten 500ms
        size_t rms_ms2 = 500;
        size_t rms_samples2 = (size_t)((g_sample_rate * rms_ms2) / 1000);
        if (rms_samples2 < 16) rms_samples2 = 16;
        float *tmp2 = (float*)malloc(sizeof(float) * rms_samples2);
        read_last_samples(tmp2, rms_samples2);
        float rms2 = compute_rms_window(tmp2, rms_samples2);
        free(tmp2);
        // Mappe rms2 auf Hue 0..0.7 (rot->blau)
        float hue = fminf(0.7f, rms2 * 5.0f);
        float sat = 1.0f;
        float val = 1.0f;
        Uint8 wr, wg, wb;
        hsv_to_rgb(hue, sat, val, &wr, &wg, &wb);

        // Zeichne horizontale Mittelachse
        SDL_SetRenderDrawColor(ren, wr/2, wg/2, wb/2, 255);
        SDL_RenderDrawLine(ren, 0, center_y, WINDOW_W, center_y);

        // Zeichne Waveform: neueste ganz rechts, ältere links
        SDL_SetRenderDrawColor(ren, wr, wg, wb, 255);
        for (int x = 1; x < WINDOW_W; ++x) {
            float v1 = pixel_buffer[x-1];
            float v2 = pixel_buffer[x];
            int y1 = center_y - (int)(v1 * vertical_scale);
            int y2 = center_y - (int)(v2 * vertical_scale);
            SDL_RenderDrawLine(ren, x-1, y1, x, y2);
            // Optional: Fülle von center bis y für stärkeren visuellen Effekt
            // SDL_RenderDrawLine(ren, x, center_y, x, y2);
        }

        // Wenn Kick aktiv, zeichne zusätzlich einen kurzen Flash-Ring (optional)
        if (kick_state) {
            // kleiner weißer Rahmen
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_Rect r = { WINDOW_W/8, WINDOW_H/8, WINDOW_W*3/4, WINDOW_H*3/4 };
            SDL_RenderDrawRect(ren, &r);
        }

        SDL_RenderPresent(ren);

        SDL_Delay(16); // ~60 FPS
    }

    // Aufräumen
    SDL_PauseAudioDevice(g_dev, 1);
    SDL_CloseAudioDevice(g_dev);
    free(audio_chunk);
    free(sample_ring);
    free(pixel_buffer);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
