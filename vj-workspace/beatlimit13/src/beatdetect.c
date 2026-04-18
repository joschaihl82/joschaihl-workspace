// beatdetect_gpu_fft_optimized.c
// gcc -O2 -o beatdetect_gpu_fft_optimized beatdetect_gpu_fft_optimized.c \
// -lSDL2 -lSDL2_ttf -lasound -lGLEW -lGL -lm -lpthread

#define _POSIX_C_SOURCE 200809L
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <alsa/asoundlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Config
#define BUFFERSIZE 64
#define FFT_SIZE 2048
#define DEFAULT_SR 48000
#define TARGET_DB 56.0f
#define MAX_GAIN 4.0f
#define ATTACK_FAST 0.6f
#define RELEASE_SLOW 0.02f
#define KICK_DEBOUNCE_MS 200

// Limiter parameters
#define LIMITER_THRESHOLD_DB   (-6.0f)
#define LIMITER_RATIO          (8.0f)
#define LIMITER_ATTACK_MS      (5.0f)
#define LIMITER_RELEASE_MS     (80.0f)
#define LIMITER_MAKEUP_DB      (0.0f)

// Font path
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
#define FONT_SIZE 18

#define MAX_ENERGY_GROUPS 64

// Globals
static snd_mixer_t *g_mixer = NULL;
static snd_mixer_elem_t *g_selem = NULL;
static long g_vol_min = 0, g_vol_max = 0;

static float *g_ring = NULL;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;

static int tex_size = FFT_SIZE;
static float *tex_samples = NULL;

static int win_w = 1280, win_h = 720;
static float g_limiter_gain = 1.0f;
static snd_pcm_t *g_pcm = NULL;

static pthread_mutex_t ring_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t param_mutex = PTHREAD_MUTEX_INITIALIZER;

static double onset_factor = 3.0;
static const double alpha_avg = 0.05;

// Forward declarations
static int alsa_find_capture_control(const char *name);
static void alsa_set_capture_all(long v);
static float compute_rms_s16(const int16_t *buf, size_t n);
static void push_to_ring_from_float_locked(const float *in, size_t n);
static float compute_rms_from_ring_locked(size_t n);
static GLuint compile_compute(const char *src);
static GLuint compile_and_link_program(const char *vsrc, const char *fsrc);

// Limiter Struct & Functions
typedef struct {
    float env;
    float gain_smooth;
    float attack_coeff;
    float release_coeff;
    float threshold_lin;
    float ratio;
    float makeup_lin;
} limiter_t;
static limiter_t g_limiter;

static void limiter_init(unsigned sample_rate) {
    float attack_s = LIMITER_ATTACK_MS * 0.001f;
    float release_s = LIMITER_RELEASE_MS * 0.001f;
    g_limiter.attack_coeff = expf(-1.0f / (attack_s * (float)sample_rate + 1e-9f));
    g_limiter.release_coeff = expf(-1.0f / (release_s * (float)sample_rate + 1e-9f));
    g_limiter.env = 0.0f;
    g_limiter.gain_smooth = 1.0f;
    g_limiter.threshold_lin = powf(10.0f, LIMITER_THRESHOLD_DB / 20.0f);
    g_limiter.ratio = LIMITER_RATIO;
    g_limiter.makeup_lin = powf(10.0f, LIMITER_MAKEUP_DB / 20.0f);
}

static void limiter_process_buffer(float *buf, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        float x = buf[i];
        float absx = fabsf(x);
        if (absx > g_limiter.env) g_limiter.env = g_limiter.attack_coeff * g_limiter.env + (1.0f - g_limiter.attack_coeff) * absx;
        else g_limiter.env = g_limiter.release_coeff * g_limiter.env + (1.0f - g_limiter.release_coeff) * absx;
        float env = fmaxf(g_limiter.env, 1e-9f);
        float env_db = 20.0f * log10f(env);
        float thr_db = LIMITER_THRESHOLD_DB;
        float gain_db = 0.0f;
        if (env_db > thr_db) {
            float over_db = env_db - thr_db;
            float reduced_db = over_db * (1.0f - 1.0f / g_limiter.ratio);
            gain_db = -reduced_db;
        }
        float desired_gain = powf(10.0f, gain_db / 20.0f) * g_limiter.makeup_lin;
        if (desired_gain < g_limiter.gain_smooth) g_limiter.gain_smooth = g_limiter.attack_coeff * g_limiter.gain_smooth + (1.0f - g_limiter.attack_coeff) * desired_gain;
        else g_limiter.gain_smooth = g_limiter.release_coeff * g_limiter.gain_smooth + (1.0f - g_limiter.release_coeff) * desired_gain;
        buf[i] = x * g_limiter.gain_smooth;
    }
}

// ALSA Helpers
static int alsa_find_capture_control(const char *name) {
    snd_mixer_selem_id_t *sid;
    if (snd_mixer_open(&g_mixer, 0) < 0) return 0;
    if (snd_mixer_attach(g_mixer, "default") < 0) { snd_mixer_close(g_mixer); g_mixer = NULL; return 0; }
    snd_mixer_selem_register(g_mixer, NULL, NULL);
    if (snd_mixer_load(g_mixer) < 0) { snd_mixer_close(g_mixer); g_mixer = NULL; return 0; }
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, name);
    g_selem = snd_mixer_find_selem(g_mixer, sid);
    if (!g_selem) return 0;
    if (snd_mixer_selem_has_capture_volume(g_selem)) {
        snd_mixer_selem_get_capture_volume_range(g_selem, &g_vol_min, &g_vol_max);
        return 1;
    }
    return 0;
}

static void alsa_set_capture_all(long v) {
    if (!g_selem) return;
    if (v < g_vol_min) v = g_vol_min;
    if (v > g_vol_max) v = g_vol_max;
    snd_mixer_selem_set_capture_volume_all(g_selem, v);
}

// Ring Buffers
static void push_to_ring_from_float_locked(const float *in, size_t n) {
    pthread_mutex_lock(&ring_mutex);
    for (size_t i = 0; i < n; ++i) {
        g_ring[g_ring_pos] = in[i];
        g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
    }
    pthread_mutex_unlock(&ring_mutex);
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

// GLSL Sources
static const char *compute_fft_src = R"(
#version 430
layout(local_size_x = 256) in;
layout(std430, binding = 0) buffer BufIn { vec2 data[]; } inBuf;
layout(std430, binding = 1) buffer BufOut { vec2 data[]; } outBuf;
uniform int u_stage;
uniform int u_N;
void main() {
  uint gid = gl_GlobalInvocationID.x;
  int N = u_N;
  int stage = u_stage;
  int m = 1 << (stage + 1);
  int hlf = m >> 1;
  int pairIndex = int(gid);
  int block = pairIndex / hlf;
  int j = pairIndex % hlf;
  int k = block * m + j;
  int idx1 = k;
  int idx2 = k + hlf;
  if (idx2 >= N) return;
  vec2 a = inBuf.data[idx1];
  vec2 b = inBuf.data[idx2];
  float angle = -2.0 * 3.14159265358979323846 * float(j) / float(m);
  float wr = cos(angle);
  float wi = sin(angle);
  vec2 t;
  t.x = b.x * wr - b.y * wi;
  t.y = b.x * wi + b.y * wr;
  outBuf.data[idx1] = a + t;
  outBuf.data[idx2] = a - t;
}
)";

static const char *compute_energy_src = R"(
#version 430
layout(local_size_x = 256) in;
layout(std430, binding = 0) buffer FFTBuf { vec2 data[]; } fftBuf;
layout(std430, binding = 2) buffer EnergyBuf { float energy[]; } energyBuf;
uniform int u_bin_lo;
uniform int u_bin_hi;
uniform int u_N;
shared float ssum[256];
void main() {
  uint gid = gl_GlobalInvocationID.x;
  uint lid = gl_LocalInvocationID.x;
  uint localSize = gl_WorkGroupSize.x;
  float sum = 0.0;
  for (int b = int(gid); b <= u_bin_hi; b += int(gl_NumWorkGroups.x * localSize)) {
    if (b >= u_bin_lo && b <= u_bin_hi) {
      float re = fftBuf.data[b].x;
      float im = fftBuf.data[b].y;
      sum += sqrt(re*re + im*im);
    }
  }
  ssum[lid] = sum;
  memoryBarrierShared();
  barrier();
  for (uint offset = localSize/2; offset > 0; offset >>= 1) {
    if (lid < offset) ssum[lid] += ssum[lid + offset];
    memoryBarrierShared();
    barrier();
  }
  if (lid == 0) {
    energyBuf.energy[gl_WorkGroupID.x] = ssum[0];
  }
}
)";

static const char *vertex_src = R"(
#version 330 core
layout(location = 0) in vec2 a_pos;
out vec2 v_uv;
void main() { v_uv = (a_pos + 1.0) * 0.5; gl_Position = vec4(a_pos, 0.0, 1.0); }
)";

static const char *fragment_src = R"(
#version 330 core
in vec2 v_uv;
out vec4 fragColor;
uniform sampler1D u_samples;
uniform float u_vscale;
uniform float u_gain_display;
uniform float u_hue;
uniform int u_kick;
vec3 hsv2rgb(float h, float s, float v) {
  float c = v * s;
  float hh = h * 6.0;
  float x = c * (1.0 - abs(mod(hh, 2.0) - 1.0));
  vec3 col;
  if (hh < 1.0) col = vec3(c, x, 0.0);
  else if (hh < 2.0) col = vec3(x, c, 0.0);
  else if (hh < 3.0) col = vec3(0.0, c, x);
  else if (hh < 4.0) col = vec3(0.0, x, c);
  else if (hh < 5.0) col = vec3(x, 0.0, c);
  else col = vec3(c, 0.0, x);
  return col + vec3(v - c);
}
void main(){
  if (u_kick==1) { fragColor = vec4(1.0); return; }
  float s = texture(u_samples, v_uv.x).r;
  float center = 0.5;
  float y = center - s * u_vscale;
  float d = abs(v_uv.y - y);
  float t = smoothstep(1.0/600.0, 0.0, d);
  vec3 col = hsv2rgb(u_hue, 1.0, 1.0);
  vec3 c = mix(col * 0.5, col, t);
  float ca = max(t, smoothstep(0.002, 0.0, abs(v_uv.y - center)));
  vec3 gaincol = vec3(0.0, 1.0, 0.0);
  if (v_uv.x < 0.15 && v_uv.y < 0.03) {
    float gx = v_uv.x / 0.15;
    if (gx < u_gain_display) { c = gaincol; ca = 1.0; }
  }
  fragColor = vec4(c, ca);
}
)";

static const char *ov_vs = R"(
#version 330 core
layout(location=0) in vec2 a_pos;
layout(location=2) in vec2 a_uv;
out vec2 v_uv;
void main(){ v_uv = a_uv; gl_Position = vec4(a_pos,0.0,1.0); }
)";

static const char *ov_fs = R"(
#version 330 core
in vec2 v_uv; out vec4 fragColor; uniform sampler2D u_tex; void main(){ fragColor = texture(u_tex, v_uv); }
)";

static const char *bresenham_aa_comp_src = R"(
#version 430
layout(local_size_x = 256) in;
layout(std430, binding = 0) buffer SamplesBuf { vec2 samples[]; };
layout(rgba8, binding = 0) uniform image2D outImg;
uniform int u_tex_size;
uniform int u_win_w;
uniform int u_win_h;
uniform float u_vscale_px;
uniform float u_y_center_px;
uniform vec4 u_color;

bool inBounds(ivec2 p) { return (p.x >= 0 && p.x < u_win_w && p.y >= 0 && p.y < u_win_h); }

void writeMaxAlpha(ivec2 p, float a, vec3 rgb) {
  if (!inBounds(p)) return;
  vec4 old = imageLoad(outImg, p);
  float oldA = old.a;
  float newA = max(oldA, a * u_color.a);
  vec3 newRGB;
  if (newA <= 0.0) newRGB = vec3(0.0);
  else newRGB = mix(old.rgb, rgb * (a * u_color.a), step(oldA, newA));
  imageStore(outImg, p, vec4(newRGB, newA));
}

void plotAA(int x, int y, float coverage, vec3 rgb) { writeMaxAlpha(ivec2(x,y), coverage, rgb); }

void drawWuLine(ivec2 a, ivec2 b, vec3 rgb) {
  int dx = abs(b.x - a.x);
  int dy = abs(b.y - a.y);
  if (dx == 0 && dy == 0) { plotAA(a.x, a.y, 1.0, rgb); return; }
  if (dx >= dy) {
    if (a.x > b.x) { ivec2 t = a; a = b; b = t; }
    float gradient = (b.y - a.y) / float(b.x - a.x);
    float y = float(a.y);
    for (int x = a.x; x <= b.x; ++x) {
      int yi = int(floor(y));
      float frac = y - float(yi);
      plotAA(x, yi, 1.0 - frac, rgb);
      plotAA(x, yi + 1, frac, rgb);
      y += gradient;
    }
  } else {
    if (a.y > b.y) { ivec2 t = a; a = b; b = t; }
    float gradient = (b.x - a.x) / float(b.y - a.y);
    float x = float(a.x);
    for (int y = a.y; y <= b.y; ++y) {
      int xi = int(floor(x));
      float frac = x - float(xi);
      plotAA(xi, y, 1.0 - frac, rgb);
      plotAA(xi + 1, y, frac, rgb);
      x += gradient;
    }
  }
}

void main() {
  uint gid = gl_GlobalInvocationID.x;
  int i = int(gid);
  if (i < 0 || i >= u_tex_size - 1) return;
  float s0 = samples[i].x;
  float s1 = samples[i+1].x;
  int x0 = int(round(float(u_win_w - 1) * (float(i) / float(u_tex_size - 1))));
  int x1 = int(round(float(u_win_w - 1) * (float(i+1) / float(u_tex_size - 1))));
  int y0 = int(round(u_y_center_px - s0 * u_vscale_px));
  int y1 = int(round(u_y_center_px - s1 * u_vscale_px));
  drawWuLine(ivec2(x0, y0), ivec2(x1, y1), u_color.rgb);
}
)";

// Shader compilers
static GLuint compile_and_link_program(const char *vsrc, const char *fsrc) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, NULL);
    glCompileShader(fs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 2, "a_uv");
    glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);
    return prog;
}
static GLuint compile_compute(const char *src) {
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(cs, 1, &src, NULL);
    glCompileShader(cs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, cs);
    glLinkProgram(prog);
    glDeleteShader(cs);
    return prog;
}

int main(void) {
    const float TARGET_LEVEL = powf(10.0f, -TARGET_DB / 20.0f);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return 1;
    if (TTF_Init() != 0) { SDL_Quit(); return 1; }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *win = SDL_CreateWindow("BeatDetect GPU FFT - Optimized", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win) return 1;
    SDL_MaximizeWindow(win);
    SDL_GetWindowSize(win, &win_w, &win_h);

    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    glewInit();
    SDL_GL_SetSwapInterval(0); // Off to not fight ALSA blocking

    TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!font) font = TTF_OpenFont("DejaVuSans.ttf", FONT_SIZE);

    if (!alsa_find_capture_control("Capture") && !alsa_find_capture_control("Mic")) return 2;

    snd_pcm_open(&g_pcm, "default", SND_PCM_STREAM_CAPTURE, 0);
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
    snd_pcm_hw_params(g_pcm, hw);
    snd_pcm_hw_params_free(hw);
    snd_pcm_prepare(g_pcm);

    limiter_init(rate);

    g_ring_cap = (size_t)rate * 2;
    g_ring = (float*)calloc(g_ring_cap, sizeof(float));
    tex_samples = (float*)calloc(tex_size, sizeof(float));
    float *proc_buf = (float*)malloc(sizeof(float) * BUFFERSIZE);
    int16_t *read_buf = (int16_t*)malloc(sizeof(int16_t) * BUFFERSIZE);

    GLuint fft_prog = compile_compute(compute_fft_src);
    GLuint energy_prog = compile_compute(compute_energy_src);
    GLuint render_prog = compile_and_link_program(vertex_src, fragment_src);

    GLuint ssbo_a, ssbo_b, ssbo_energy;
    glGenBuffers(1, &ssbo_a);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_a);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * tex_size, NULL, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &ssbo_b);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_b);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * tex_size, NULL, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &ssbo_energy);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_energy);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * MAX_ENERGY_GROUPS, NULL, GL_DYNAMIC_DRAW);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, tex_size, 0, GL_RED, GL_FLOAT, tex_samples);

    GLuint quad_vao, quad_vbo;
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    float quad[] = { -1.0f,-1.0f,  1.0f,-1.0f,  -1.0f,1.0f,  1.0f,1.0f };
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint overlay_vao, overlay_vbo;
    glGenVertexArrays(1, &overlay_vao);
    glGenBuffers(1, &overlay_vbo);
    glBindVertexArray(overlay_vao);
    glBindBuffer(GL_ARRAY_BUFFER, overlay_vbo);
    float overlay_verts[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(overlay_verts), overlay_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2));

    GLuint overlay_prog = compile_and_link_program(ov_vs, ov_fs);

    GLuint text_tex;
    glGenTextures(1, &text_tex);
    int text_w = 0, text_h = 0;
    double last_rendered_onset = -1.0; // Text Caching Variable

    GLuint bres_prog = compile_compute(bresenham_aa_comp_src);
    GLint loc_tex_size = glGetUniformLocation(bres_prog, "u_tex_size");
    GLint loc_win_w = glGetUniformLocation(bres_prog, "u_win_w");
    GLint loc_win_h = glGetUniformLocation(bres_prog, "u_win_h");
    GLint loc_vscale_px = glGetUniformLocation(bres_prog, "u_vscale_px");
    GLint loc_y_center_px = glGetUniformLocation(bres_prog, "u_y_center_px");
    GLint loc_color = glGetUniformLocation(bres_prog, "u_color");

    GLuint wave_tex2D;
    glGenTextures(1, &wave_tex2D);
    glBindTexture(GL_TEXTURE_2D, wave_tex2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, win_w, win_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint loc_vscale = glGetUniformLocation(render_prog, "u_vscale");
    GLint loc_gain = glGetUniformLocation(render_prog, "u_gain_display");
    GLint loc_hue = glGetUniformLocation(render_prog, "u_hue");
    GLint loc_kick = glGetUniformLocation(render_prog, "u_kick");
    GLint loc_samples = glGetUniformLocation(render_prog, "u_samples");

    GLubyte *clearbuf = NULL;
    size_t clearbuf_size = 0;

    // Two-timescale onset detection state (persistent)
    static double long_avg = 1e-8;
    static double short_avg = 1e-8;
    const double alpha_long = 0.01;   // slow background estimate
    const double alpha_short = 0.3;   // fast, responsive energy estimate

    bool running = true;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE) running = false;
                else if (ev.key.keysym.sym == SDLK_UP) {
                    pthread_mutex_lock(&param_mutex);
                    onset_factor = fmax(1.0, onset_factor - 0.1);
                    pthread_mutex_unlock(&param_mutex);
                } else if (ev.key.keysym.sym == SDLK_DOWN) {
                    pthread_mutex_lock(&param_mutex);
                    onset_factor = fmin(10.0, onset_factor + 0.1);
                    pthread_mutex_unlock(&param_mutex);
                }
            } else if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                win_w = ev.window.data1; win_h = ev.window.data2;
                glViewport(0,0,win_w,win_h);
                glBindTexture(GL_TEXTURE_2D, wave_tex2D);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, win_w, win_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            }
        }

        snd_pcm_sframes_t frames = snd_pcm_readi(g_pcm, read_buf, BUFFERSIZE);
        if (frames == -EPIPE) { snd_pcm_prepare(g_pcm); continue; }
        else if (frames < 0) { snd_pcm_recover(g_pcm, frames, 0); continue; }
        else if (frames == 0) { SDL_Delay(1); continue; }

        for (int i = 0; i < frames; ++i) proc_buf[i] = read_buf[i] / 32768.0f;
        limiter_process_buffer(proc_buf, (size_t)frames);

        float chunk_rms = 0.0f;
        double s = 0.0;
        for (int i = 0; i < frames; ++i) s += proc_buf[i] * proc_buf[i];
        chunk_rms = (float)sqrt(s / (double)frames);

        float desired_gain = TARGET_LEVEL / fmaxf(chunk_rms, 1e-12f);
        if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;
        g_limiter_gain = (desired_gain > g_limiter_gain) ?
            (ATTACK_FAST * desired_gain + (1.0f - ATTACK_FAST) * g_limiter_gain) :
            (RELEASE_SLOW * desired_gain + (1.0f - RELEASE_SLOW) * g_limiter_gain);

        alsa_set_capture_all(g_limiter_gain >= 1.0f ? g_vol_max :
            g_vol_min + (long)((g_vol_max - g_vol_min) * fmaxf(0.0f, g_limiter_gain) + 0.5f));

        push_to_ring_from_float_locked(proc_buf, (size_t)frames);

        pthread_mutex_lock(&ring_mutex);
        size_t start = (g_ring_pos + g_ring_cap - (size_t)tex_size) % g_ring_cap;
        for (int i = 0; i < tex_size; ++i) tex_samples[i] = g_ring[(start + i) % g_ring_cap];
        pthread_mutex_unlock(&ring_mutex);

        glBindTexture(GL_TEXTURE_1D, tex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, tex_size, GL_RED, GL_FLOAT, tex_samples);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_a);
        float *fp = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*2*tex_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (fp) {
            for (int i = 0; i < tex_size; ++i) { fp[2*i+0] = tex_samples[i]; fp[2*i+1] = 0.0f; }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        int groups_for_energy = fmin(8, MAX_ENERGY_GROUPS);
        float zeros[MAX_ENERGY_GROUPS] = {0};
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_energy);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * groups_for_energy, zeros);

        int stages = (int)log2((double)tex_size);
        GLuint cur_in = ssbo_a, cur_out = ssbo_b;
        for (int stage = 0; stage < stages; ++stage) {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cur_in);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cur_out);
            glUseProgram(fft_prog);
            glUniform1i(glGetUniformLocation(fft_prog, "u_stage"), stage);
            glUniform1i(glGetUniformLocation(fft_prog, "u_N"), tex_size);
            glDispatchCompute(fmax(1, (tex_size / 2 + 255) / 256), 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            GLuint tmp = cur_in; cur_in = cur_out; cur_out = tmp;
        }

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cur_in);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_energy);
        glUseProgram(energy_prog);
        double bin_width = (double)rate / (double)tex_size;
        glUniform1i(glGetUniformLocation(energy_prog, "u_bin_lo"), fmax(0, (int)floor(20.0 / bin_width)));
        glUniform1i(glGetUniformLocation(energy_prog, "u_bin_hi"), fmin(tex_size/2 - 1, (int)ceil(150.0 / bin_width)));
        glUniform1i(glGetUniformLocation(energy_prog, "u_N"), tex_size);
        glDispatchCompute(groups_for_energy, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_energy);
        float energy_val = 0.0f;
        float *emap2 = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * groups_for_energy, GL_MAP_READ_BIT);
        if (emap2) {
            for (int i = 0; i < groups_for_energy; ++i) energy_val += emap2[i];
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        // --- Two-timescale onset detection (short vs long average) ---
        pthread_mutex_lock(&param_mutex);
        double local_onset = onset_factor;
        pthread_mutex_unlock(&param_mutex);

        // Update short and long averages
        short_avg = alpha_short * energy_val + (1.0 - alpha_short) * short_avg;
        long_avg  = alpha_long  * energy_val + (1.0 - alpha_long)  * long_avg;

        int kick = 0;
        static Uint32 last_kick_ms = 0;
        // Trigger when short-term energy significantly exceeds long-term background
        if (short_avg > long_avg * local_onset && energy_val > 1e-8) {
            Uint32 now = SDL_GetTicks();
            if ((now - last_kick_ms) > KICK_DEBOUNCE_MS) {
                kick = 1;
                last_kick_ms = now;
                // Slightly reduce the long-term average after a detected kick so the detector
                // doesn't remain desensitized by a sustained high background.
                long_avg *= 0.9;
                if (long_avg < 1e-8) long_avg = 1e-8;
            }
        }

#ifdef GL_VERSION_4_4
        GLfloat clearColor[4] = {0.0f,0.0f,0.0f,0.0f};
        glClearTexImage(wave_tex2D, 0, GL_RGBA, GL_FLOAT, clearColor);
#else
        size_t clearSize = (size_t)win_w * (size_t)win_h * 4;
        if (clearbuf_size < clearSize) { free(clearbuf); clearbuf = (GLubyte*)calloc(clearSize, 1); clearbuf_size = clearSize; }
        glBindTexture(GL_TEXTURE_2D, wave_tex2D);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, win_w, win_h, GL_RGBA, GL_UNSIGNED_BYTE, clearbuf);
#endif

        glBindImageTexture(0, wave_tex2D, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_a);

        glUseProgram(bres_prog);
        glUniform1i(loc_tex_size, tex_size);
        glUniform1i(loc_win_w, win_w);
        glUniform1i(loc_win_h, win_h);
        glUniform1f(loc_vscale_px, (float)win_h * 0.4f);
        glUniform1f(loc_y_center_px, (float)win_h * 0.5f);
        glUniform4f(loc_color, 1.0f, 1.0f, 1.0f, 1.0f);
        glDispatchCompute(fmax(1, (tex_size - 1 + 255) / 256), 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(render_prog);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tex);
        glUniform1i(loc_samples, 0);
        glUniform1f(loc_vscale, 0.4f);
        glUniform1f(loc_gain, fminf(1.0f, g_limiter_gain));
        glUniform1f(loc_hue, fminf(0.7f, compute_rms_from_ring_locked((size_t)((rate * 500) / 1000)) * 5.0f));
        glUniform1i(loc_kick, kick);

        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(overlay_prog);
        glBindTexture(GL_TEXTURE_2D, wave_tex2D);
        glBindVertexArray(overlay_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Optimiertes Text-Rendering (Caching!)
        if (font) {
            if (local_onset != last_rendered_onset) {
                last_rendered_onset = local_onset;
                char buf[64];
                snprintf(buf, sizeof(buf), "Onset: %.2f", local_onset);
                SDL_Color white = { 255, 255, 255, 255 };
                SDL_Surface *surf = TTF_RenderUTF8_Blended(font, buf, white);
                if (surf) {
                    SDL_Surface *conv = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_ABGR8888, 0);
                    if (conv) {
                        glBindTexture(GL_TEXTURE_2D, text_tex);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, conv->w, conv->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, conv->pixels);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        text_w = conv->w; text_h = conv->h;
                        SDL_FreeSurface(conv);
                    }
                    SDL_FreeSurface(surf);
                }
            }

            if (text_w > 0) {
                float sx = (float)text_w / (float)win_w;
                float sy = (float)text_h / (float)win_h;
                float txt_verts[] = {
                    1.0f - sx * 2.0f, -1.0f,               0.0f, 0.0f,
                    1.0f,             -1.0f,               1.0f, 0.0f,
                    1.0f - sx * 2.0f, -1.0f + sy * 2.0f,   0.0f, 1.0f,
                    1.0f,             -1.0f + sy * 2.0f,   1.0f, 1.0f
                };
                glBindBuffer(GL_ARRAY_BUFFER, overlay_vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(txt_verts), txt_verts);
                glBindTexture(GL_TEXTURE_2D, text_tex);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(overlay_verts), overlay_verts);
            }
        }

        glDisable(GL_BLEND);
        SDL_GL_SwapWindow(win);
    }

    // Cleanup (basic)
    if (g_pcm) { snd_pcm_close(g_pcm); g_pcm = NULL; }
    if (g_mixer) { snd_mixer_close(g_mixer); g_mixer = NULL; }
    if (g_ring) free(g_ring);
    if (tex_samples) free(tex_samples);
    if (proc_buf) free(proc_buf);
    if (read_buf) free(read_buf);

    // Note: GL resources and SDL/TTF cleanup omitted for brevity; add as needed.

    return 0;
}
