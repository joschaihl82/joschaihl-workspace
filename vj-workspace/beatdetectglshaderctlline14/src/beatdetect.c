// beatdetect_16band_colorflash.c
// gcc -O2 -o beatdetect_16band_colorflash beatdetect_16band_colorflash.c -lSDL2 -lasound -lGLEW -lGL -lm -lpthread
// 16-band FFT (internal) + per-band HSV colors + logistic brightness -> immediate white flash
// All dynamic memory allocated once at startup; no malloc/alloca/free in main loop.

#define _POSIX_C_SOURCE 200809L
#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Config
#define BUFFERSIZE 64
#define FFT_SIZE 512
#define N_BANDS 512
#define DEFAULT_SR 48000
#define TARGET_DB 24.0f
#define MAX_GAIN 4.0f
#define ATTACK_FAST 0.6f
#define RELEASE_SLOW 0.02f
#define KICK_DEBOUNCE_MS 120
#define KICK_FLASH_MS 80
#define TARGET_FPS 1000
#define FRAME_MS (1000.0f / TARGET_FPS)

// ALSA mixer
static snd_mixer_t *g_mixer = NULL;
static snd_mixer_elem_t *g_selem = NULL;
static long g_vol_min = 0, g_vol_max = 0;

// Ring buffer (float mono)
static float *g_ring = NULL;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;

// Texture samples and waveform vertex buffer (preallocated)
static int tex_size = FFT_SIZE;
static float *tex_samples = NULL;        // length tex_size
static float *wave_verts = NULL;         // length 2 * tex_size (x,y pairs)

// Window
static int win_w = 1280, win_h = 720;

// Limiter state
static float g_limiter_gain = 1.0f;

// ALSA PCM
static snd_pcm_t *g_pcm = NULL;

// Sync
static pthread_mutex_t ring_mutex = PTHREAD_MUTEX_INITIALIZER;

// Onset / kick state
static double running_avg_low = 1e-8;
static const double alpha_avg = 0.08;
static double onset_factor = 3.0; // adjustable with arrow keys
static Uint32 last_kick_ts = 0;
static volatile int kick_flag = 0;
static Uint32 kick_flash_until = 0;

// FFT tables and buffers (static)
static double fft_re[FFT_SIZE];
static double fft_im[FFT_SIZE];
static double tw_re[FFT_SIZE/2];
static double tw_im[FFT_SIZE/2];
static double window_hann[FFT_SIZE];

// Per-band running averages for normalization
static double band_running_avg[N_BANDS];

// Logistic growth parameters (tuneable)
static const double growth_k = 12.0;   // Steilheit
static const double growth_x0 = 0.18*10.0;  // Mitte (normierter Wert)

// Forward declarations
static bool alsa_find_capture_control(const char *name);
static void alsa_set_capture_all(long v);
static void fft_prepare(void);
static void bit_reverse_permute(double *re, double *im);
static void fft_execute(double *re, double *im);
static void compute_magnitude_spectrum(const float *snapshot, double *mag_out);
static void map_bins_to_bands(const double *mag, double *bands_out, unsigned sample_rate);
static void push_to_ring_from_s16_locked(const int16_t *in, size_t n);
static float compute_rms_s16(const int16_t *buf, size_t n);
static float compute_rms_from_ring_locked(size_t n);

// GLSL shaders (fragment shader extended to draw VU strip and support full white flash)
static const char *vertex_shader_src =
"#version 330 core\n"
"layout(location = 0) in vec2 a_pos;\n"
"out vec2 v_uv;\n"
"void main() {\n"
"  v_uv = (a_pos + 1.0) * 0.5;\n" // map [-1,1] -> [0,1]
"  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
"}\n";

static const char *fragment_shader_src =
"#version 330 core\n"
"in vec2 v_uv;\n"
"out vec4 fragColor;\n"
"uniform sampler1D u_samples;\n"
"uniform sampler1D u_bandcols; // RGB colors per band\n"
"uniform float u_vscale;\n"
"uniform float u_gain_display;\n"
"uniform float u_hue;\n"
"uniform int u_kick;\n"
"uniform int u_nbands;\n"
"vec3 hsv2rgb(float h, float s, float v) {\n"
"  float c = v * s; float hh = h * 6.0; float x = c * (1.0 - abs(mod(hh, 2.0) - 1.0));\n"
"  vec3 col;\n"
"  if (hh < 1.0) col = vec3(c, x, 0.0);\n"
"  else if (hh < 2.0) col = vec3(x, c, 0.0);\n"
"  else if (hh < 3.0) col = vec3(0.0, c, x);\n"
"  else if (hh < 4.0) col = vec3(0.0, x, c);\n"
"  else if (hh < 5.0) col = vec3(x, 0.0, c);\n"
"  else col = vec3(c, 0.0, x);\n"
"  return col + vec3(v - c);\n"
"}\n"
"void main() {\n"
"  // Full white flash (highest priority)\n"
"  if (u_kick == 2) { fragColor = vec4(1.0); return; }\n"
"\n"
"  // Draw VU color strip at top: map x->band index\n"
"  if (v_uv.y > 0.92) {\n"
"    float bx = clamp(v_uv.x, 0.0, 0.9999);\n"
"    float idx = bx * float(u_nbands);\n"
"    vec3 col = texture(u_bandcols, (idx + 0.5) / float(u_nbands)).rgb;\n"
"    fragColor = vec4(col, 1.0);\n"
"    return;\n"
"  }\n"
"\n"
"  // Normal waveform rendering below\n"
"  float s = texture(u_samples, v_uv.x).r;\n"
"  float center = 0.5;\n"
"  float y_wave = center - s * u_vscale;\n"
"  float dist = abs(v_uv.y - y_wave);\n"
"  float line_thickness = 1.0 / 600.0;\n"
"  float alpha = smoothstep(line_thickness, 0.0, dist);\n"
"  vec3 wavecol = hsv2rgb(u_hue, 1.0, 1.0);\n"
"  vec3 centercol = wavecol * 0.5;\n"
"  float center_dist = abs(v_uv.y - center);\n"
"  float center_alpha = smoothstep(0.002, 0.0, center_dist);\n"
"  vec3 col = mix(centercol, wavecol, alpha);\n"
"  float final_alpha = max(alpha, center_alpha);\n"
"  vec3 gaincol = vec3(0.0, 1.0, 0.0);\n"
"  if (v_uv.x < 0.15 && v_uv.y < 0.03) {\n"
"    float gx = v_uv.x / 0.15;\n"
"    if (gx < u_gain_display) { col = gaincol; final_alpha = 1.0; }\n"
"  }\n"
"  fragColor = vec4(col, final_alpha);\n"
"}\n";

// GL helpers (unchanged)
static GLuint compile_shader(GLenum type, const char *src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    GLint ok = 0; glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        char *log = (char*)malloc(len + 1);
        glGetShaderInfoLog(sh, len, NULL, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
        free(log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

static GLuint link_program(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glBindAttribLocation(prog, 0, "a_pos");
    glLinkProgram(prog);
    GLint ok = 0; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        char *log = (char*)malloc(len + 1);
        glGetProgramInfoLog(prog, len, NULL, log);
        fprintf(stderr, "Program link error: %s\n", log);
        free(log);
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

// ALSA mixer helpers (unchanged)
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

// FFT prepare
static void fft_prepare(void) {
    for (int k = 0; k < FFT_SIZE/2; ++k) {
        double angle = -2.0 * M_PI * k / (double)FFT_SIZE;
        tw_re[k] = cos(angle);
        tw_im[k] = sin(angle);
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        window_hann[n] = 0.5 * (1.0 - cos(2.0 * M_PI * n / (FFT_SIZE - 1)));
    }
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

// compute magnitude spectrum from snapshot (size FFT_SIZE)
static void compute_magnitude_spectrum(const float *snapshot, double *mag_out) {
    for (int n = 0; n < FFT_SIZE; ++n) {
        double v = snapshot[n];
        fft_re[n] = v * window_hann[n];
        fft_im[n] = 0.0;
    }
    fft_execute(fft_re, fft_im);
    int half = FFT_SIZE / 2;
    for (int b = 0; b < half; ++b) {
        double re = fft_re[b];
        double im = fft_im[b];
        mag_out[b] = sqrt(re*re + im*im);
    }
}

// Map linear bins to N_BANDS logarithmic bands (focus on low end)
static void map_bins_to_bands(const double *mag, double *bands_out, unsigned sample_rate) {
    double f_min = 20.0;
    double f_max = sample_rate / 2.0;
    double log_min = log(f_min);
    double log_max = log(f_max);
    double bin_width = (double)sample_rate / (double)FFT_SIZE;
    for (int b = 0; b < N_BANDS; ++b) bands_out[b] = 0.0;
    for (int b = 0; b < N_BANDS; ++b) {
        double f_lo = exp(log_min + (double)b * (log_max - log_min) / (double)N_BANDS);
        double f_hi = exp(log_min + (double)(b+1) * (log_max - log_min) / (double)N_BANDS);
        int bin_lo = (int)floor(f_lo / bin_width);
        int bin_hi = (int)ceil(f_hi / bin_width);
        if (bin_lo < 0) bin_lo = 0;
        if (bin_hi > FFT_SIZE/2 - 1) bin_hi = FFT_SIZE/2 - 1;
        double sum = 0.0;
        for (int k = bin_lo; k <= bin_hi; ++k) sum += mag[k];
        bands_out[b] = sum;
    }
}

// Ring buffer helpers
static void push_to_ring_from_s16_locked(const int16_t *in, size_t n) {
    pthread_mutex_lock(&ring_mutex);
    for (size_t i = 0; i < n; ++i) {
        g_ring[g_ring_pos] = in[i] / 32768.0f;
        g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
    }
    pthread_mutex_unlock(&ring_mutex);
}

static float compute_rms_s16(const int16_t *buf, size_t n) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = buf[i] / 32768.0;
        s += v * v;
    }
    return (float)sqrt(s / (double)n);
}

static float compute_rms_from_ring_locked(size_t n) {
    pthread_mutex_lock(&ring_mutex);
    if (n == 0) { pthread_mutex_unlock(&ring_mutex); return 0.0f; }
    if (n > g_ring_cap) n = g_ring_cap;
    size_t start = (g_ring_pos + g_ring_cap - n) % g_ring_cap;
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = g_ring[(start + i) % g_ring_cap];
        s += v * v;
    }
    pthread_mutex_unlock(&ring_mutex);
    return (float)sqrt(s / (double)n);
}

// HSV -> RGB helper on CPU
static void hsv_to_rgb(double h, double s, double v, float *out_rgb) {
    double c = v * s;
    double hh = h * 6.0;
    double x = c * (1.0 - fabs(fmod(hh, 2.0) - 1.0));
    double r=0,g=0,b=0;
    if (hh < 1.0) { r=c; g=x; b=0; }
    else if (hh < 2.0) { r=x; g=c; b=0; }
    else if (hh < 3.0) { r=0; g=c; b=x; }
    else if (hh < 4.0) { r=0; g=x; b=c; }
    else if (hh < 5.0) { r=x; g=0; b=c; }
    else { r=c; g=0; b=x; }
    out_rgb[0] = (float)(r + (v - c));
    out_rgb[1] = (float)(g + (v - c));
    out_rgb[2] = (float)(b + (v - c));
}

int main(void) {
    const float TARGET_LEVEL = powf(10.0f, -TARGET_DB / 20.0f);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // OpenGL 3.3 Core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *win = SDL_CreateWindow("BeatDetect 16-band ColorFlash", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       win_w, win_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win) { fprintf(stderr, "Window error: %s\n", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_MaximizeWindow(win);
    SDL_GetWindowSize(win, &win_w, &win_h);

    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) { fprintf(stderr, "GL context error: %s\n", SDL_GetError()); SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        fprintf(stderr, "GLEW init failed: %s\n", glewGetErrorString(glew_status));
        SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit(); return 1;
    }
    SDL_GL_SetSwapInterval(0);

    // ALSA mixer
    if (!alsa_find_capture_control("Capture")) {
        if (!alsa_find_capture_control("Mic")) {
            fprintf(stderr, "No ALSA capture control found. Exiting.\n");
            SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
            return 2;
        }
    }

    // open ALSA PCM
    int err = snd_pcm_open(&g_pcm, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(err));
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
        return 3;
    }

    // configure HW params
    snd_pcm_hw_params_t *hw;
    snd_pcm_hw_params_malloc(&hw);
    snd_pcm_hw_params_any(g_pcm, hw);
    snd_pcm_hw_params_set_access(g_pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(g_pcm, hw, SND_PCM_FORMAT_S16_LE);
    unsigned int rate = DEFAULT_SR;
    snd_pcm_hw_params_set_rate_near(g_pcm, hw, &rate, 0);
    snd_pcm_hw_params_set_channels(g_pcm, hw, 1);
    snd_pcm_uframes_t period = BUFFERSIZE;
    snd_pcm_hw_params_set_period_size_near(g_pcm, hw, &period, 0);
    snd_pcm_uframes_t buffer = period * 4;
    snd_pcm_hw_params_set_buffer_size_near(g_pcm, hw, &buffer);
    if ((err = snd_pcm_hw_params(g_pcm, hw)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params: %s\n", snd_strerror(err));
        snd_pcm_hw_params_free(hw);
        snd_pcm_close(g_pcm);
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
        return 4;
    }
    snd_pcm_hw_params_free(hw);
    snd_pcm_prepare(g_pcm);

    snd_pcm_uframes_t actual_period;
    snd_pcm_get_params(g_pcm, &buffer, &actual_period);
    fprintf(stderr, "ALSA opened: rate=%u period=%lu buffer=%lu\n", rate, (unsigned long)actual_period, (unsigned long)buffer);

    // allocate buffers once
    g_ring_cap = (size_t)rate * 2;
    g_ring = (float*)malloc(sizeof(float) * g_ring_cap);
    if (!g_ring) { fprintf(stderr, "OOM ring\n"); goto cleanup_early; }
    memset(g_ring, 0, sizeof(float) * g_ring_cap);

    tex_size = FFT_SIZE;
    tex_samples = (float*)malloc(sizeof(float) * tex_size);
    if (!tex_samples) { fprintf(stderr, "OOM tex\n"); goto cleanup_early; }
    memset(tex_samples, 0, sizeof(float) * tex_size);

    wave_verts = (float*)malloc(sizeof(float) * 2 * tex_size);
    if (!wave_verts) { fprintf(stderr, "OOM wave\n"); goto cleanup_early; }
    memset(wave_verts, 0, sizeof(float) * 2 * tex_size);

    int16_t *read_buf = (int16_t*)malloc(sizeof(int16_t) * BUFFERSIZE);
    if (!read_buf) { fprintf(stderr, "OOM read\n"); goto cleanup_early; }

    // prepare FFT tables
    fft_prepare();

    // initial per-band running averages
    for (int i = 0; i < N_BANDS; ++i) band_running_avg[i] = 1e-8;

    // initial hardware volume mid
    long initial = g_vol_min + (g_vol_max - g_vol_min) / 2;
    alsa_set_capture_all(initial);

    // compile shaders
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (!vs || !fs) { fprintf(stderr, "Shader compile failed\n"); goto cleanup_early; }
    GLuint prog = link_program(vs, fs);
    if (!prog) { fprintf(stderr, "Shader link failed\n"); goto cleanup_early; }

    // uniform locations
    GLint loc_u_samples = glGetUniformLocation(prog, "u_samples");
    GLint loc_u_bandcols = glGetUniformLocation(prog, "u_bandcols");
    GLint loc_u_vscale = glGetUniformLocation(prog, "u_vscale");
    GLint loc_u_gain_display = glGetUniformLocation(prog, "u_gain_display");
    GLint loc_u_hue = glGetUniformLocation(prog, "u_hue");
    GLint loc_u_kick = glGetUniformLocation(prog, "u_kick");
    GLint loc_u_nbands = glGetUniformLocation(prog, "u_nbands");

    // create 1D texture for samples
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, tex_size, 0, GL_RED, GL_FLOAT, tex_samples);

    // create 1D texture for band colors (RGB)
    GLuint bandtex;
    glGenTextures(1, &bandtex);
    glBindTexture(GL_TEXTURE_1D, bandtex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // allocate storage: N_BANDS RGB floats
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, N_BANDS, 0, GL_RGB, GL_FLOAT, NULL);

    // full-screen quad VAO/VBO
    GLuint quad_vao, quad_vbo;
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    float quad[] = { -1.0f,-1.0f,  1.0f,-1.0f,  -1.0f,1.0f,  1.0f,1.0f };
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray(0);

    // waveform VAO/VBO (preallocated)
    GLuint wave_vao, wave_vbo;
    glGenVertexArrays(1, &wave_vao);
    glGenBuffers(1, &wave_vbo);
    glBindVertexArray(wave_vao);
    glBindBuffer(GL_ARRAY_BUFFER, wave_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * tex_size, NULL, GL_DYNAMIC_DRAW); // storage reserved once
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray(0);

    // temporary arrays for FFT/bands and band color upload
    double mag[FFT_SIZE/2];
    double bands[N_BANDS];
    float bandcols[N_BANDS * 3];

    // main loop
    Uint32 last_frame = SDL_GetTicks();
    Uint32 last_dbg = 0;
    bool running = true;
    SDL_Event ev;

    fprintf(stderr, "16-band FFT + per-band color VU enabled. Onset factor initial = %.2f\n", onset_factor);

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE) running = false;
                else if (ev.key.keysym.sym == SDLK_UP) {
                    onset_factor -= 0.1; if (onset_factor < 1.0) onset_factor = 1.0;
                    fprintf(stderr, "Onset factor -> %.2f (finer)\n", onset_factor);
                } else if (ev.key.keysym.sym == SDLK_DOWN) {
                    onset_factor += 0.1; if (onset_factor > 10.0) onset_factor = 10.0;
                    fprintf(stderr, "Onset factor -> %.2f (coarser)\n", onset_factor);
                }
            } else if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                win_w = ev.window.data1; win_h = ev.window.data2;
                glViewport(0, 0, win_w, win_h);
            }
        }

        // read ALSA (blocking)
        snd_pcm_sframes_t frames = snd_pcm_readi(g_pcm, read_buf, BUFFERSIZE);
        if (frames == -EPIPE) { snd_pcm_prepare(g_pcm); continue; }
        else if (frames < 0) { frames = snd_pcm_recover(g_pcm, frames, 0); if (frames < 0) break; continue; }
        else if (frames == 0) { SDL_Delay(1); continue; }

        // RMS chunk (instant energy)
        float chunk_rms = compute_rms_s16(read_buf, (size_t)frames);

        // desired gain (limiter)
        float desired_gain = TARGET_LEVEL / fmaxf(chunk_rms, 1e-12f);
        if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;
        if (desired_gain > g_limiter_gain) g_limiter_gain = ATTACK_FAST * desired_gain + (1.0f - ATTACK_FAST) * g_limiter_gain;
        else g_limiter_gain = RELEASE_SLOW * desired_gain + (1.0f - RELEASE_SLOW) * g_limiter_gain;

        long setv;
        if (g_limiter_gain >= 1.0f) setv = g_vol_max;
        else {
            float gmap = fmaxf(0.0f, g_limiter_gain);
            setv = g_vol_min + (long)((g_vol_max - g_vol_min) * gmap + 0.5f);
        }
        alsa_set_capture_all(setv);

        // push into ring (thread-safe)
        push_to_ring_from_s16_locked(read_buf, (size_t)frames);

        // copy last FFT_SIZE samples into snapshot (locked)
        float snapshot[FFT_SIZE];
        pthread_mutex_lock(&ring_mutex);
        size_t start = (g_ring_pos + g_ring_cap - (size_t)FFT_SIZE) % g_ring_cap;
        for (int i = 0; i < FFT_SIZE; ++i) snapshot[i] = g_ring[(start + i) % g_ring_cap];
        pthread_mutex_unlock(&ring_mutex);

        // --- FFT and 16-band mapping (synchronous, immediate) ---
        compute_magnitude_spectrum(snapshot, mag);
        map_bins_to_bands(mag, bands, rate);

        // Update per-band running averages and compute normalized band values
        for (int b = 0; b < N_BANDS; ++b) {
            // small smoothing for each band
            band_running_avg[b] = 0.05 * bands[b] + 0.95 * band_running_avg[b];
            double norm = bands[b] / (band_running_avg[b] + 1e-12);
            // logistic brightness
            double bright = 1.0 / (1.0 + exp(-growth_k * (norm - growth_x0)));
            // ensure very bright mapping for strong signals
            if (bright < 0.0) bright = 0.0;
            if (bright > 1.0) bright = 1.0;
            // assign color: band 0 = white scaled by bright; others HSV rainbow
            if (b == 0) {
                bandcols[3*b + 0] = (float)bright;
                bandcols[3*b + 1] = (float)bright;
                bandcols[3*b + 2] = (float)bright;
            } else {
                // hue: iterate across 0..0.85 (rainbow), band 1..N_BANDS-1
                double hue = (double)(b - 1) / (double)(N_BANDS - 1) * 0.85;
                float rgb[3];
                hsv_to_rgb(hue, 1.0, (double)bright, rgb);
                bandcols[3*b + 0] = rgb[0];
                bandcols[3*b + 1] = rgb[1];
                bandcols[3*b + 2] = rgb[2];
            }
        }

        // immediate kick detection based on lowest bands sum
        double low_sum = 0.0;
        int low_bands = 4;
        for (int i = 0; i < low_bands; ++i) low_sum += bands[i];
        low_sum /= (double)low_bands;
        running_avg_low = alpha_avg * low_sum + (1.0 - alpha_avg) * running_avg_low;
        Uint32 now = SDL_GetTicks();
        bool detected = false;
        if (low_sum > running_avg_low * onset_factor && low_sum > 1e-8) {
            if ((now - last_kick_ts) > KICK_DEBOUNCE_MS) {
                detected = true;
                last_kick_ts = now;
                kick_flash_until = now + KICK_FLASH_MS;
            }
        }
        if (now < kick_flash_until) kick_flag = 2;
        else if (detected) kick_flag = 1;
        else kick_flag = 0;
        // --------------------------------------------------------

        // copy last tex_size samples into tex_samples (locked)
        pthread_mutex_lock(&ring_mutex);
        start = (g_ring_pos + g_ring_cap - (size_t)tex_size) % g_ring_cap;
        for (int i = 0; i < tex_size; ++i) tex_samples[i] = g_ring[(start + i) % g_ring_cap];
        pthread_mutex_unlock(&ring_mutex);

        // upload 1D texture (no allocation)
        glBindTexture(GL_TEXTURE_1D, tex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, tex_size, GL_RED, GL_FLOAT, tex_samples);

        // upload band color texture
        glBindTexture(GL_TEXTURE_1D, bandtex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, N_BANDS, GL_RGB, GL_FLOAT, bandcols);

        // build waveform vertex array (NO allocation)
        float vscale_ndc = 0.4f * 2.0f;
        float center_ndc = 0.0f;
        for (int i = 0; i < tex_size; ++i) {
            float xf = (float)i / (float)(tex_size - 1);
            float x_ndc = xf * 2.0f - 1.0f;
            float s = tex_samples[i];
            float y_ndc = center_ndc - s * vscale_ndc;
            wave_verts[2*i + 0] = x_ndc;
            wave_verts[2*i + 1] = y_ndc;
        }

        // upload waveform VBO (no allocation)
        glBindBuffer(GL_ARRAY_BUFFER, wave_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * tex_size, wave_verts);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // compute hue from 500ms RMS (visual)
        float rms500 = compute_rms_from_ring_locked((size_t)((rate * 500) / 1000));
        float hue = fminf(0.7f, rms500 * 5.0f);

        // render
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tex);
        glUniform1i(loc_u_samples, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, bandtex);
        glUniform1i(loc_u_bandcols, 1);

        glUniform1f(loc_u_vscale, 0.4f);
        glUniform1f(loc_u_gain_display, fminf(1.0f, g_limiter_gain));
        glUniform1f(loc_u_hue, hue);
        glUniform1i(loc_u_kick, kick_flag);
        glUniform1i(loc_u_nbands, N_BANDS);

        // draw full-screen quad
        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        // draw continuous waveform line on top
        glLineWidth(2.0f);
        glBindVertexArray(wave_vao);
        glDrawArrays(GL_LINE_STRIP, 0, tex_size);
        glBindVertexArray(0);

        glUseProgram(0);
        SDL_GL_SwapWindow(win);

        // debug once per second
        if (now - last_dbg > 1000) {
            fprintf(stderr, "rate=%u tex=%d limiter_gain=%.3f onset=%.2f kick=%d low=%.6f avg=%.6f\n",
                    rate, tex_size, g_limiter_gain, onset_factor, kick_flag, low_sum, running_avg_low);
            last_dbg = now;
        }

        // frame cap
        Uint32 frame_end = SDL_GetTicks();
        float elapsed = (float)(frame_end - last_frame);
        if (elapsed < FRAME_MS) SDL_Delay((Uint32)fmaxf(1.0f, FRAME_MS - elapsed));
        last_frame = SDL_GetTicks();
    }

cleanup_early:
    if (g_pcm) snd_pcm_close(g_pcm);
    if (g_mixer) snd_mixer_close(g_mixer);
    if (tex_samples) free(tex_samples);
    if (wave_verts) free(wave_verts);
    if (g_ring) free(g_ring);
    if (read_buf) free(read_buf);

    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
