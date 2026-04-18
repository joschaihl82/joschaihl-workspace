// beatvis_alsa_gl.c
// gcc -O2 -o beatvis_alsa_gl beatvis_alsa_gl.c -lSDL2 -lasound -lm -lGL
// ALSA-PCM-Capture + ALSA-Mixer (Hardware-Gain, 24 dB Headroom) + SDL2/OpenGL-Visualisierung
// BUFFERSIZE = 512, 2s Ringbuffer, Waveform mittig, Kick -> weißer Screen, Stille -> schwarz.

#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <GL/gl.h>

#define BUFFERSIZE 64
#define DEFAULT_SR 48000

#define TARGET_DB 24.0f
#define MAX_GAIN 4.0f
#define ATTACK_FAST 0.6f
#define RELEASE_SLOW 0.02f

#define LOWPASS_ALPHA 0.02f
#define KICK_RMS_THRESHOLD 0.08f
#define KICK_DEBOUNCE_MS 200

// Ziel-FPS für Visualisierung
#define TARGET_FPS 120
#define FRAME_MS (1000.0f / TARGET_FPS)

// ALSA Mixer
static snd_mixer_t *g_mixer = NULL;
static snd_mixer_elem_t *g_selem = NULL;
static long g_vol_min = 0, g_vol_max = 0;

// Ringbuffer
static float *g_ring = NULL;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;

// Pixelbuffer (eine Spalte pro Pixel)
static float *g_pixels = NULL;
static int g_win_w = 1280;
static int g_win_h = 720;

// Limiter / Kick
static float g_limiter_gain = 1.0f;
static float g_lowpass_state = 0.0f;
static float g_prev_rms = 0.0f;
static Uint32 g_last_kick_ms = 0;
static bool g_kick_state = false;

// HSV -> RGB
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
    *r = rf + m;
    *g = gf + m;
    *b = bf + m;
}

// ALSA Mixer: Capture-Control finden
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

// RMS eines int16-Buffers
static float rms_s16(const int16_t *buf, size_t n) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = buf[i] / 32768.0;
        s += v * v;
    }
    return (float)sqrt(s / (double)n);
}

// Ringbuffer push (int16 -> float)
static void ring_push_s16(const int16_t *in, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        g_ring[g_ring_pos] = in[i] / 32768.0f;
        g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
    }
}

// RMS aus Ringbuffer
static float rms_from_ring(size_t n) {
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

// Pixelbuffer aus Ringbuffer (2s -> g_win_w)
static void update_pixels(void) {
    if (!g_pixels) return;
    size_t spp = g_ring_cap / (size_t)g_win_w;
    if (spp < 1) spp = 1;
    size_t start = (g_ring_pos + g_ring_cap - g_ring_cap) % g_ring_cap;
    for (int x = 0; x < g_win_w; ++x) {
        size_t idx = (start + (size_t)x * spp) % g_ring_cap;
        double sum = 0.0;
        for (size_t k = 0; k < spp; ++k) {
            sum += g_ring[(idx + k) % g_ring_cap];
        }
        g_pixels[x] = (float)(sum / (double)spp);
    }
}

// Kick-Erkennung
static bool detect_kick(unsigned sample_rate) {
    size_t win = (sample_rate * 100) / 1000;
    if (win < 16) win = 16;
    size_t start = (g_ring_pos + g_ring_cap - win) % g_ring_cap;
    float lp = g_lowpass_state;
    for (size_t i = 0; i < win; ++i) {
        float v = g_ring[(start + i) % g_ring_cap];
        lp = lp + LOWPASS_ALPHA * (v - lp);
    }
    g_lowpass_state = lp;
    double s = 0.0;
    for (size_t i = 0; i < win; ++i) {
        double v = g_ring[(start + i) % g_ring_cap] - lp;
        s += v * v;
    }
    float r = (float)sqrt(s / (double)win);
    Uint32 now = SDL_GetTicks();
    bool kick = false;
    if (r > KICK_RMS_THRESHOLD && (r - g_prev_rms) > 0.01f && (now - g_last_kick_ms) > KICK_DEBOUNCE_MS) {
        kick = true;
        g_last_kick_ms = now;
    }
    g_prev_rms = r;
    return kick;
}

// OpenGL Setup für aktuelles Fenster
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
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_Window *win = SDL_CreateWindow(
        "Beat Visualizer ALSA + OpenGL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        g_win_w, g_win_h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!win) {
        fprintf(stderr, "Window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_MaximizeWindow(win);
    SDL_GetWindowSize(win, &g_win_w, &g_win_h);

    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) {
        fprintf(stderr, "GL Context: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    SDL_GL_SetSwapInterval(0); // kein VSync

    gl_setup_view(g_win_w, g_win_h);

    // ALSA Mixer
    if (!alsa_find_capture_control("Capture")) {
        if (!alsa_find_capture_control("Mic")) {
            fprintf(stderr, "Kein ALSA Capture/Mic Control gefunden.\n");
            SDL_GL_DeleteContext(ctx);
            SDL_DestroyWindow(win);
            SDL_Quit();
            return 2;
        }
    }

    // ALSA PCM
    snd_pcm_t *pcm = NULL;
    int err = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(err));
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 3;
    }

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
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 4;
    }
    snd_pcm_hw_params_free(hw);
    snd_pcm_prepare(pcm);

    snd_pcm_uframes_t actual_period;
    snd_pcm_get_params(pcm, &buffer, &actual_period);
    fprintf(stderr, "ALSA: rate=%u period=%lu buffer=%lu\n",
            rate, (unsigned long)actual_period, (unsigned long)buffer);

    // Ringbuffer 2s
    g_ring_cap = (size_t)rate * 2;
    g_ring = (float*)calloc(g_ring_cap, sizeof(float));
    if (!g_ring) {
        fprintf(stderr, "Out of memory ring\n");
        snd_pcm_close(pcm);
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    g_pixels = (float*)calloc((size_t)g_win_w, sizeof(float));
    if (!g_pixels) {
        free(g_ring);
        snd_pcm_close(pcm);
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    int16_t *read_buf = (int16_t*)malloc(sizeof(int16_t) * BUFFERSIZE);
    if (!read_buf) {
        free(g_pixels);
        free(g_ring);
        snd_pcm_close(pcm);
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // RMS-Fenster
    size_t rms100 = (rate * 100) / 1000; if (rms100 < 16) rms100 = 16;
    size_t rms200 = (rate * 200) / 1000; if (rms200 < 16) rms200 = 16;
    size_t rms500 = (rate * 500) / 1000; if (rms500 < 16) rms500 = 16;

    // Anfangs-HW-Gain
    long initial = g_vol_min + (g_vol_max - g_vol_min) / 2;
    alsa_set_capture_all(initial);

    bool running = true;
    SDL_Event ev;
    Uint32 last_frame = SDL_GetTicks();
    Uint32 last_dbg = 0;

    while (running) {
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

        // ALSA lesen (blocking)
        snd_pcm_sframes_t frames = snd_pcm_readi(pcm, read_buf, BUFFERSIZE);
        if (frames == -EPIPE) {
            snd_pcm_prepare(pcm);
            continue;
        } else if (frames < 0) {
            frames = snd_pcm_recover(pcm, frames, 0);
            if (frames < 0) {
                fprintf(stderr, "snd_pcm_readi: %s\n", snd_strerror((int)frames));
                break;
            }
            continue;
        } else if (frames == 0) {
            SDL_Delay(1);
            continue;
        }

        // RMS Chunk
        float chunk_rms = rms_s16(read_buf, (size_t)frames);

        // Limiter (24 dB Headroom)
        float desired_gain = TARGET_LEVEL / fmaxf(chunk_rms, 1e-12f);
        if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;
        if (desired_gain > g_limiter_gain)
            g_limiter_gain = ATTACK_FAST * desired_gain + (1.0f - ATTACK_FAST) * g_limiter_gain;
        else
            g_limiter_gain = RELEASE_SLOW * desired_gain + (1.0f - RELEASE_SLOW) * g_limiter_gain;

        long setv;
        if (g_limiter_gain >= 1.0f) setv = g_vol_max;
        else {
            float gmap = fmaxf(0.0f, g_limiter_gain);
            setv = g_vol_min + (long)((g_vol_max - g_vol_min) * gmap + 0.5f);
        }
        alsa_set_capture_all(setv);

        // in Ringbuffer
        ring_push_s16(read_buf, (size_t)frames);

        // Pixelbuffer aktualisieren
        update_pixels();

        // Kick
        bool kick = detect_kick(rate);

        // Hintergrundfarbe
        float bg_r=0, bg_g=0, bg_b=0;
        float rms200v = rms_from_ring(rms200);
        if (kick) {
            bg_r = bg_g = bg_b = 1.0f;
            g_kick_state = true;
        } else {
            if (rms200v < 0.005f) {
                bg_r = bg_g = bg_b = 0.0f;
            } else {
                float v = fminf(0.31f, 0.08f + rms200v * 1.6f); // ~0..80/255
                bg_r = bg_g = bg_b = v;
            }
            g_kick_state = false;
        }

        // Clear
        glClearColor(bg_r, bg_g, bg_b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Waveform-Farbe aus HSV (500ms RMS)
        float rms500v = rms_from_ring(rms500);
        float hue = fminf(0.7f, rms500v * 5.0f);
        float wr, wg, wb;
        hsv_to_rgb(hue, 1.0f, 1.0f, &wr, &wg, &wb);

        int cy = g_win_h / 2;
        float vscale = (float)(g_win_h / 2) * 0.9f;

        glLoadIdentity();

        // Mittelachse
        glColor3f(wr*0.5f, wg*0.5f, wb*0.5f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, (float)cy);
        glVertex2f((float)g_win_w, (float)cy);
        glEnd();

        // Waveform
        glColor3f(wr, wg, wb);
        glBegin(GL_LINE_STRIP);
        for (int x = 0; x < g_win_w; ++x) {
            float v = g_pixels[x];
            float y = (float)cy - v * vscale;
            glVertex2f((float)x, y);
        }
        glEnd();

        // Kick-Rahmen
        if (g_kick_state) {
            glColor3f(1.0f, 1.0f, 1.0f);
            float x0 = g_win_w * 0.125f;
            float y0 = g_win_h * 0.125f;
            float x1 = g_win_w * 0.875f;
            float y1 = g_win_h * 0.875f;
            glBegin(GL_LINE_LOOP);
            glVertex2f(x0, y0);
            glVertex2f(x1, y0);
            glVertex2f(x1, y1);
            glVertex2f(x0, y1);
            glEnd();
        }

        // Gain-Balken (links oben)
        float disp_gain = g_limiter_gain;
        if (disp_gain > 1.0f) disp_gain = 1.0f;
        float barw = 200.0f * disp_gain;
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(10.0f, 10.0f);
        glVertex2f(10.0f + barw, 10.0f);
        glVertex2f(10.0f + barw, 18.0f);
        glVertex2f(10.0f, 18.0f);
        glEnd();

        SDL_GL_SwapWindow(win);

        Uint32 now = SDL_GetTicks();
        if (now - last_dbg > 1000) {
            float lat_ms = (float)actual_period / (float)rate * 1000.0f;
            fprintf(stderr, "rate=%u period=%lu buffer=%lu latency≈%.2fms gain=%.3f\n",
                    rate, (unsigned long)actual_period, (unsigned long)buffer, lat_ms, g_limiter_gain);
            last_dbg = now;
        }

        Uint32 frame_end = SDL_GetTicks();
        float elapsed = (float)(frame_end - last_frame);
        if (elapsed < FRAME_MS) SDL_Delay((Uint32)fmaxf(1.0f, FRAME_MS - elapsed));
        last_frame = SDL_GetTicks();
    }

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
