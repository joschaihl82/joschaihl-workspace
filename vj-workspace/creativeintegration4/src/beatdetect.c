// main.c
// Amalgamated: photorealistic fractal mountain tunnel shader integrated
// - Replaces audio-driven creative tunnel with photorealistic FBM mountain tunnel
// - Uses uniforms multiple times across shader for richer modulation
// - All dynamic buffers allocated once; no malloc/free in main loop
//
// Compile:
// gcc -O3 -o beatdetect_merged main.c -lSDL2 -lasound -lGLEW -lGL -lm -lpthread

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

// --------------------------- SCALE / Configuration ---------------------------
#define SCALE_FACTOR 5                      // "5x größer" scale factor requested

#define BUFFERSIZE_BASE 64
#define BUFFERSIZE (BUFFERSIZE_BASE * SCALE_FACTOR)   // ALSA read frames per iteration

#define FFT_SIZE 512                         // keep power-of-two for FFT
#define N_BANDS_BASE 256
#define N_BANDS (N_BANDS_BASE * SCALE_FACTOR) // more bands (GPU upload larger)

#define DEFAULT_SR 48000
#define TARGET_DB 24.0f
#define MAX_GAIN 4.0f
#define ATTACK_FAST 0.6f
#define RELEASE_SLOW 0.02f
#define KICK_DEBOUNCE_MS 110
#define FLASH_MS 20
#define TARGET_FPS 120
#define FRAME_MS (1000.0f / TARGET_FPS)

#define INITIAL_WIN_W (1280 * SCALE_FACTOR)
#define INITIAL_WIN_H (720  * SCALE_FACTOR)

// --------------------------- Globals ALSA / Ring ---------------------------
static snd_pcm_t *g_pcm = NULL;
static snd_mixer_t *g_mixer = NULL;
static snd_mixer_elem_t *g_selem = NULL;
static long g_vol_min = 0, g_vol_max = 0;

static float *g_ring = NULL;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;

static int win_w = INITIAL_WIN_W, win_h = INITIAL_WIN_H;
static float g_limiter_gain = 1.0f;

static pthread_mutex_t ring_mutex = PTHREAD_MUTEX_INITIALIZER;

// --------------------------- FFT / Bands ---------------------------
static double fft_re[FFT_SIZE];
static double fft_im[FFT_SIZE];
static double tw_re[FFT_SIZE / 2];
static double tw_im[FFT_SIZE / 2];
static double window_hann[FFT_SIZE];

static double *band_running_avg = NULL; // allocated dynamically for N_BANDS

// --------------------------- Beat / Flash / Colors ---------------------------
static double running_avg_low = 1e-8;
static const double alpha_avg = 0.08;
static double onset_factor = 1.0;

static Uint32 last_kick_ts = 0;
static volatile int kick_flag = 0;

static Uint32 flash_start_ts = 0;
static const double flash_k = 14.0;
static const double flash_x0 = 0.5;
static const int TOTAL_CONTRAST_STEPS = 32;
static int contrast_step = 0;

// --------------------------- HSV helper ---------------------------
typedef struct { double h, s, v; } HSV;
static void hsv_to_rgb_double(const HSV *hsv, float *out_rgb) {
    double h = hsv->h;
    while (h < 0.0) h += 360.0;
    while (h >= 360.0) h -= 360.0;
    double s = hsv->s, v = hsv->v;
    double c = v * s;
    double hh = h / 60.0;
    double x = c * (1.0 - fabs(fmod(hh, 2.0) - 1.0));
    double r=0,g=0,b=0;
    if (hh >= 0 && hh < 1) { r=c; g=x; b=0; }
    else if (hh < 2) { r=x; g=c; b=0; }
    else if (hh < 3) { r=0; g=c; b=x; }
    else if (hh < 4) { r=0; g=x; b=c; }
    else if (hh < 5) { r=x; g=0; b=c; }
    else { r=c; g=0; b=x; }
    double m = v - c;
    out_rgb[0] = (float)(r + m);
    out_rgb[1] = (float)(g + m);
    out_rgb[2] = (float)(b + m);
}
static HSV get_next_contrast_color(int current_step) {
    const double offset = 360.0 / TOTAL_CONTRAST_STEPS;
    HSV new_color;
    double hue = fmod(current_step * (180.0 + offset), 360.0);
    if (hue < 0.0) hue += 360.0;
    new_color.h = hue; new_color.s = 1.0; new_color.v = 1.0;
    return new_color;
}

// --------------------------- Forward Declarations ---------------------------
static bool alsa_find_capture_control(const char *name);
static void alsa_set_capture_all(long v);
static void fft_prepare(void);
static void bit_reverse_permute(double *re, double *im);
static void fft_execute(double *re, double *im);
static void map_bins_to_bands(const double *mag, double *bands_out, unsigned sample_rate);
static void push_to_ring_from_s16_locked(const int16_t *in, size_t n);
static float compute_rms_s16(const int16_t *buf, size_t n);
static float compute_rms_from_ring_locked(size_t n);

// --------------------------- GL helpers ---------------------------
static GLuint compile_shader(GLenum type, const char *src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        char *log = (char*) malloc(len + 1);
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
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        char *log = (char*) malloc(len + 1);
        glGetProgramInfoLog(prog, len, NULL, log);
        fprintf(stderr, "Program link error: %s\n", log);
        free(log);
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

// --------------------------- ALSA Mixer (patched) ---------------------------
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
    }
    if (snd_mixer_selem_has_capture_switch(g_selem)) {
        int rc = snd_mixer_selem_set_capture_switch_all(g_selem, 1);
        if (rc < 0) fprintf(stderr, "Warning: failed to set capture switch on: %s\n", snd_strerror(rc));
        else fprintf(stderr, "ALSA: capture switch set ON for '%s'.\n", name);
    }
    return true;
}
static void alsa_set_capture_all(long v) {
    if (!g_selem) return;
    if (v < g_vol_min) v = g_vol_min;
    if (v > g_vol_max) v = g_vol_max;
    if (snd_mixer_selem_has_capture_switch(g_selem)) {
        int rc = snd_mixer_selem_set_capture_switch_all(g_selem, 1);
        if (rc < 0) fprintf(stderr, "Warning: failed to re-enable capture switch: %s\n", snd_strerror(rc));
    }
    int rc = snd_mixer_selem_set_capture_volume_all(g_selem, v);
    if (rc < 0) fprintf(stderr, "Warning: failed to set capture volume: %s\n", snd_strerror(rc));
}

// --------------------------- FFT Implementation ---------------------------
static void fft_prepare(void) {
    for (int k = 0; k < FFT_SIZE / 2; ++k) {
        double angle = -2.0 * M_PI * k / (double) FFT_SIZE;
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
static void map_bins_to_bands(const double *mag, double *bands_out, unsigned sample_rate) {
    double f_min = 20.0;
    double f_max = sample_rate / 2.0;
    double log_min = log(f_min);
    double log_max = log(f_max);
    double bin_width = (double) sample_rate / (double) FFT_SIZE;
    for (int b = 0; b < N_BANDS; ++b) bands_out[b] = 0.0;
    for (int b = 0; b < N_BANDS; ++b) {
        double f_lo = exp(log_min + (double) b * (log_max - log_min) / (double) N_BANDS);
        double f_hi = exp(log_min + (double) (b + 1) * (log_max - log_min) / (double) N_BANDS);
        int bin_lo = (int) floor(f_lo / bin_width);
        int bin_hi = (int) ceil(f_hi / bin_width);
        if (bin_lo < 0) bin_lo = 0;
        if (bin_hi > FFT_SIZE / 2 - 1) bin_hi = FFT_SIZE / 2 - 1;
        double sum = 0.0;
        for (int k = bin_lo; k <= bin_hi; ++k) sum += mag[k];
        bands_out[b] = sum;
    }
}

// --------------------------- Ring helpers ---------------------------
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
    return (float) sqrt(s / (double) n);
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
    return (float) sqrt(s / (double) n);
}

// --------------------------- Fragment Shader (FBM mountain tunnel) ---------------------------
static const char *ray_fs = "#version 330 core\n"
"uniform float iTime;\n"
"uniform vec2 iResolution;\n"
"uniform sampler1D u_samples;\n"
"uniform sampler1D u_bandcols;\n"
"uniform float u_vscale;\n"
"uniform float u_gain_display;\n"
"uniform float u_hue;\n"
"uniform int u_kick;\n"
"uniform int u_nbands;\n"
"uniform float u_flash_brightness;\n"
"uniform vec3 u_flash_color;\n"
"uniform float u_band_bass;\n"
"uniform float u_band_mid;\n"
"uniform float u_band_treble;\n"
"out vec4 fragColor;\n"
"\n"
// Hash / pseudo-random
"uint wang_hash(uint seed) { seed = (seed ^ 61u) ^ (seed >> 16u); seed *= 9u; seed = seed ^ (seed >> 4u); seed *= 0x27d4eb2du; seed = seed ^ (seed >> 15u); return seed; }\n"
"float rnd(vec3 p) { uint h = wang_hash(floatBitsToUint(p.x*15731.0 + p.y*789221.0 + p.z*1376312589.0)); return float(h) / 4294967296.0; }\n"
"vec3 fade(vec3 t){ return t*t*(3.0-2.0*t); }\n"
"float gradNoise(vec3 p) {\n"
"    vec3 ip = floor(p);\n"
"    vec3 fp = fract(p);\n"
"    vec3 w = fade(fp);\n"
"    float n000 = rnd(ip + vec3(0.0,0.0,0.0));\n"
"    float n100 = rnd(ip + vec3(1.0,0.0,0.0));\n"
"    float n010 = rnd(ip + vec3(0.0,1.0,0.0));\n"
"    float n110 = rnd(ip + vec3(1.0,1.0,0.0));\n"
"    float n001 = rnd(ip + vec3(0.0,0.0,1.0));\n"
"    float n101 = rnd(ip + vec3(1.0,0.0,1.0));\n"
"    float n011 = rnd(ip + vec3(0.0,1.0,1.0));\n"
"    float n111 = rnd(ip + vec3(1.0,1.0,1.0));\n"
"    float nx00 = mix(n000, n100, w.x);\n"
"    float nx10 = mix(n010, n110, w.x);\n"
"    float nx01 = mix(n001, n101, w.x);\n"
"    float nx11 = mix(n011, n111, w.x);\n"
"    float nxy0 = mix(nx00, nx10, w.y);\n"
"    float nxy1 = mix(nx01, nx11, w.y);\n"
"    return mix(nxy0, nxy1, w.z);\n"
"}\n"
"float fbm(vec3 p) {\n"
"    float sum = 0.0;\n"
"    float amp = 0.6;\n"
"    float freq = 1.0;\n"
"    // use u_band_mid and u_band_treble to modulate octaves and amplitude\n"
"    float mid = clamp(u_band_mid, 0.0, 6.0);\n"
"    float treb = clamp(u_band_treble, 0.0, 6.0);\n"
"    int OCT = 5 + int(clamp(mid*0.2, 0.0, 3.0));\n"
"    for (int i=0;i<OCT;i++){\n"
"        sum += amp * gradNoise(p * freq + vec3(0.0, iTime*0.01*i));\n"
"        freq *= 2.0 + 0.05*treb;\n"
"        amp *= 0.5;\n"
"    }\n"
"    return sum;\n"
"}\n"
"\n"
"float mapScene(vec3 p, out int mat) {\n"
"    // use bass to open/close tunnel radius in multiple places\n"
"    float bass = clamp(u_band_bass, 0.0, 6.0);\n"
"    float mid  = clamp(u_band_mid, 0.0, 6.0);\n"
"    float treb = clamp(u_band_treble, 0.0, 6.0);\n"
"    // centerline wobble uses samples texture and fbm\n"
"    float s0 = texture(u_samples, 0.02).r;\n"
"    float s1 = texture(u_samples, 0.25).r;\n"
"    float s2 = texture(u_samples, 0.75).r;\n"
"    vec2 center = vec2( sin(p.z*0.08 + iTime*0.02 + s0*2.0)*0.8, cos(p.z*0.06 + iTime*0.015 + s1*1.5)*0.6 );\n"
"    // mountain height from fbm, modulated by mid/treb and samples\n"
"    vec3 q = p * vec3(0.9, 0.7, 1.0);\n"
"    q += vec3( fbm(vec3(p.z*0.12,0.0,iTime*0.05))*0.8, fbm(vec3(0.0,p.z*0.09,iTime*0.04))*0.6, 0.0 );\n"
"    float height = fbm(q * (1.0 + 0.25*mid)) * (1.0 + 0.5*treb) + 0.25*s2;\n"
"    // radial distance from centerline\n"
"    float r = length(p.xy - center);\n"
"    float tunnelRadius = 0.9 + 0.35 * (1.0 - exp(-bass*0.6));\n"
"    // add micro-detail using samples and fbm\n"
"    float micro = 0.08 * fbm(vec3(p.xy*6.0, p.z*0.5 + s1*2.0));\n"
"    float rock = height - (r - tunnelRadius);\n"
"    float d = rock + micro;\n"
"    mat = 1;\n"
"    return d;\n"
"}\n"
"\n"
"vec3 calcNormal(vec3 p) {\n"
"    float eps = 0.006;\n"
"    int dummy;\n"
"    float dx = mapScene(p + vec3(eps,0,0), dummy) - mapScene(p - vec3(eps,0,0), dummy);\n"
"    float dy = mapScene(p + vec3(0,eps,0), dummy) - mapScene(p - vec3(0,eps,0), dummy);\n"
"    float dz = mapScene(p + vec3(0,0,eps), dummy) - mapScene(p - vec3(0,0,eps), dummy);\n"
"    return normalize(vec3(dx,dy,dz));\n"
"}\n"
"\n"
"float softAO(vec3 p, vec3 n) {\n"
"    float ao = 0.0;\n"
"    float sca = 1.0;\n"
"    int dummy;\n"
"    for (int i=1;i<=5;i++){\n"
"        float hr = 0.02 * float(i);\n"
"        float dd = mapScene(p + n * hr, dummy);\n"
"        ao += (hr - dd) * sca;\n"
"        sca *= 0.6;\n"
"    }\n"
"    return clamp(1.0 - 3.0*ao, 0.0, 1.0);\n"
"}\n"
"\n"
"vec3 shade(vec3 pos, vec3 ro, vec3 rd, vec3 n, vec3 baseCol) {\n"
"    vec3 lightDir = normalize(vec3(-0.6, 0.8, 0.4));\n"
"    float diff = max(dot(n, lightDir), 0.0);\n"
"    float spec = pow(max(dot(reflect(-lightDir, n), -rd), 0.0), 32.0);\n"
"    float ao = softAO(pos, n);\n"
"    vec3 bandAvg = texture(u_bandcols, 0.5).rgb;\n"
"    // use u_hue and u_gain_display multiple times to tint and brighten\n"
"    float hueMix = clamp(u_hue * 0.8 + 0.2 * bandAvg.r, 0.0, 1.0);\n"
"    float gainBoost = 1.0 + 0.25 * u_gain_display;\n"
"    vec3 col = baseCol * (0.12 + 0.9*diff) * ao * gainBoost;\n"
"    col += vec3(1.0)*0.6*spec*diff* (0.6 + 0.4 * u_gain_display);\n"
"    col = mix(col, col * (0.6 + 0.8*bandAvg), 0.08 + 0.02*u_hue);\n"
"    // subtle hue shift using u_hue again\n"
"    col *= (0.95 + 0.1 * hueMix);\n"
"    return clamp(col, 0.0, 1.0);\n"
"}\n"
"\n"
"void main(){\n"
"    // flash override uses u_flash_brightness and u_flash_color multiple times\n"
"    if (u_flash_brightness > 0.001) {\n"
"        vec3 flash = u_flash_color * (1.0 + 0.6 * u_band_bass) * (0.6 + 0.4 * u_gain_display);\n"
"        float alpha = clamp(u_flash_brightness * (0.8 + 0.4 * u_band_mid), 0.0, 1.0);\n"
"        fragColor = vec4(flash, alpha);\n"
"        return;\n"
"    }\n"
"    vec2 uv = (gl_FragCoord.xy / iResolution.xy) * 2.0 - 1.0;\n"
"    uv.x *= iResolution.x / iResolution.y;\n"
"    // camera\n"
"    float t = iTime * (1.0 + 0.6 * u_band_bass);\n"
"    vec3 camPos = vec3( sin(t*0.06 + u_hue*2.0)*0.8, cos(t*0.045 + u_hue*1.5)*0.6, t );\n"
"    vec3 target = vec3( sin((t+0.8)*0.06 + u_hue*2.0)*0.8, cos((t+0.8)*0.045 + u_hue*1.5)*0.6, t + 1.2 );\n"
"    vec3 forward = normalize(target - camPos);\n"
"    vec3 up = vec3(0.0,1.0,0.0);\n"
"    vec3 right = normalize(cross(forward, up));\n"
"    vec3 camUp = cross(right, forward);\n"
"    vec3 rd = normalize(forward + uv.x*right*0.9 + uv.y*camUp*0.6);\n"
"\n"
"    // raymarch\n"
"    float totalT = 0.0;\n"
"    int mat = 0;\n"
"    float d = 1e6;\n"
"    const int MAX_STEPS = 72;\n"
"    const float MAX_DIST = 120.0;\n"
"    for (int i=0;i<MAX_STEPS;i++){\n"
"        vec3 pos = camPos + rd * totalT;\n"
"        d = mapScene(pos, mat);\n"
"        if (d < 0.001) break;\n"
"        totalT += max(0.002, d*0.65);\n"
"        if (totalT > MAX_DIST) break;\n"
"    }\n"
"    vec3 color = vec3(0.02, 0.03, 0.05);\n"
"    if (d < 0.001 && totalT < MAX_DIST) {\n"
"        vec3 pos = camPos + rd * totalT;\n"
"        vec3 n = calcNormal(pos);\n"
"        float rockTone = 0.4 + 0.6 * fbm(pos*0.8 + vec3(0.0,iTime*0.02,0.0));\n"
"        vec3 baseCol = mix(vec3(0.35,0.32,0.28), vec3(0.55,0.5,0.45), rockTone);\n"
"        float kick = u_kick > 0 ? 1.0 : 0.0;\n"
"        baseCol = mix(baseCol, baseCol + 0.12*vec3(0.9,0.85,0.8), 0.2*kick);\n"
"        color = shade(pos, camPos, rd, n, baseCol);\n"
"        float scatter = exp(-0.06*totalT) * (0.6 + 0.4*u_band_mid);\n"
"        color += vec3(0.08,0.06,0.04) * scatter;\n"
"    } else {\n"
"        float horizon = fbm(vec3(rd.x*0.6, rd.y*0.6, iTime*0.01));\n"
"        color = mix(color, vec3(0.12,0.14,0.18) + 0.6*horizon, 0.35 + 0.1*u_hue);\n"
"    }\n"
"    color = pow(clamp(color, 0.0, 1.0), vec3(0.4545));\n"
"    fragColor = vec4(color, 1.0);\n"
"}\n";

// --------------------------- Main ---------------------------
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

    SDL_Window *win = SDL_CreateWindow("BeatDetect Merged 5x - Mountain Tunnel",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ALSA mixer: try to find capture control and enable capture switch
    if (!alsa_find_capture_control("Capture")) {
        if (!alsa_find_capture_control("Mic")) {
            fprintf(stderr, "Warning: No ALSA capture control found. Program will continue but microphone may be muted by system.\n");
        }
    }

    // open ALSA PCM
    int err = snd_pcm_open(&g_pcm, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(err));
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
        return 2;
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
        snd_pcm_hw_params_free(hw); snd_pcm_close(g_pcm);
        if (g_mixer) snd_mixer_close(g_mixer);
        SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit();
        return 3;
    }
    snd_pcm_hw_params_free(hw);
    snd_pcm_prepare(g_pcm);

    snd_pcm_uframes_t actual_period;
    snd_pcm_get_params(g_pcm, &buffer, &actual_period);
    fprintf(stderr, "ALSA opened: rate=%u period=%lu buffer=%lu\n", rate, (unsigned long) actual_period, (unsigned long) buffer);

    // allocate ring and band arrays (scaled)
    g_ring_cap = (size_t) rate * 2 * SCALE_FACTOR; // 5x larger ring capacity
    g_ring = (float*) malloc(sizeof(float) * g_ring_cap);
    if (!g_ring) { fprintf(stderr, "OOM ring\n"); goto cleanup; }
    memset(g_ring, 0, sizeof(float) * g_ring_cap);

    band_running_avg = (double*) malloc(sizeof(double) * N_BANDS);
    if (!band_running_avg) { fprintf(stderr, "OOM band_running_avg\n"); goto cleanup; }
    for (int i = 0; i < N_BANDS; ++i) band_running_avg[i] = 1e-8;

    float *tex_samples = (float*) malloc(sizeof(float) * FFT_SIZE);
    if (!tex_samples) { fprintf(stderr, "OOM tex_samples\n"); goto cleanup; }
    memset(tex_samples, 0, sizeof(float) * FFT_SIZE);

    float *bandcols = (float*) malloc(sizeof(float) * N_BANDS * 3);
    if (!bandcols) { fprintf(stderr, "OOM bandcols\n"); goto cleanup; }
    memset(bandcols, 0, sizeof(float) * N_BANDS * 3);

    int16_t *read_buf = (int16_t*) malloc(sizeof(int16_t) * BUFFERSIZE);
    if (!read_buf) { fprintf(stderr, "OOM read_buf\n"); goto cleanup; }

    // prepare FFT twiddles and window
    fft_prepare();

    // compile shaders
    const char *fullscreen_vs = "#version 330 core\n"
    "layout(location = 0) in vec2 a_pos;\n"
    "void main() { gl_Position = vec4(a_pos, 0.0, 1.0); }\n";
    GLuint vs = compile_shader(GL_VERTEX_SHADER, fullscreen_vs);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, ray_fs);
    if (!vs || !fs) { fprintf(stderr, "Shader compile failed\n"); goto cleanup; }
    GLuint prog = link_program(vs, fs);
    if (!prog) { fprintf(stderr, "Shader link failed\n"); goto cleanup; }

    // uniform locations
    GLint loc_iTime = glGetUniformLocation(prog, "iTime");
    GLint loc_iResolution = glGetUniformLocation(prog, "iResolution");
    GLint loc_u_samples = glGetUniformLocation(prog, "u_samples");
    GLint loc_u_bandcols = glGetUniformLocation(prog, "u_bandcols");
    GLint loc_u_vscale = glGetUniformLocation(prog, "u_vscale");
    GLint loc_u_gain_display = glGetUniformLocation(prog, "u_gain_display");
    GLint loc_u_hue = glGetUniformLocation(prog, "u_hue");
    GLint loc_u_kick = glGetUniformLocation(prog, "u_kick");
    GLint loc_u_nbands = glGetUniformLocation(prog, "u_nbands");
    GLint loc_u_flash_brightness = glGetUniformLocation(prog, "u_flash_brightness");
    GLint loc_u_flash_color = glGetUniformLocation(prog, "u_flash_color");
    GLint loc_band_bass = glGetUniformLocation(prog, "u_band_bass");
    GLint loc_band_mid = glGetUniformLocation(prog, "u_band_mid");
    GLint loc_band_treble = glGetUniformLocation(prog, "u_band_treble");

    // create textures
    GLuint tex, bandtex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, FFT_SIZE, 0, GL_RED, GL_FLOAT, tex_samples);

    glGenTextures(1, &bandtex);
    glBindTexture(GL_TEXTURE_1D, bandtex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, N_BANDS, 0, GL_RGB, GL_FLOAT, bandcols);

    // full-screen quad
    GLuint quad_vao, quad_vbo;
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    float quad[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray(0);

    // temp arrays for FFT and mapping
    double mag[FFT_SIZE / 2];
    double *bands = (double*) malloc(sizeof(double) * N_BANDS);
    if (!bands) { fprintf(stderr, "OOM bands\n"); goto cleanup; }

    // initial background color
    HSV initial_hsv = get_next_contrast_color(contrast_step);
    float bg_rgb[3];
    hsv_to_rgb_double(&initial_hsv, bg_rgb);
    glClearColor(bg_rgb[0], bg_rgb[1], bg_rgb[2], 1.0f);

    // main loop
    Uint32 last_frame = SDL_GetTicks();
    bool running = true;
    SDL_Event ev;

    fprintf(stderr, "Start: merged program (scaled x%d). N_BANDS=%d BUFFERSIZE=%d ring_cap=%zu\n", SCALE_FACTOR, N_BANDS, BUFFERSIZE, g_ring_cap);

    double start_time = (double)SDL_GetTicks() * 0.001;
    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE) running = false;
                else if (ev.key.keysym.sym == SDLK_UP) {
                    onset_factor -= 0.1; if (onset_factor < 1.0) onset_factor = 1.0;
                    fprintf(stderr, "Onset factor -> %.2f\n", onset_factor);
                } else if (ev.key.keysym.sym == SDLK_DOWN) {
                    onset_factor += 0.1; if (onset_factor > 10.0) onset_factor = 10.0;
                    fprintf(stderr, "Onset factor -> %.2f\n", onset_factor);
                } else if (ev.key.keysym.sym == SDLK_c) {
                    contrast_step = (contrast_step + 1) % TOTAL_CONTRAST_STEPS;
                    HSV h = get_next_contrast_color(contrast_step);
                    hsv_to_rgb_double(&h, bg_rgb);
                    glClearColor(bg_rgb[0], bg_rgb[1], bg_rgb[2], 1.0f);
                }
            } else if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                win_w = ev.window.data1; win_h = ev.window.data2;
                glViewport(0, 0, win_w, win_h);
            }
        }

        // read ALSA
        snd_pcm_sframes_t frames = snd_pcm_readi(g_pcm, read_buf, BUFFERSIZE);
        if (frames == -EPIPE) { snd_pcm_prepare(g_pcm); continue; }
        else if (frames < 0) { frames = snd_pcm_recover(g_pcm, frames, 0); if (frames < 0) break; continue; }
        else if (frames == 0) { SDL_Delay(1); continue; }

        // push to ring
        push_to_ring_from_s16_locked(read_buf, (size_t)frames);

        // RMS chunk
        float chunk_rms = compute_rms_s16(read_buf, (size_t) frames);

        // limiter / auto-gain
        float desired_gain = TARGET_LEVEL / fmaxf(chunk_rms, 1e-12f);
        if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;
        // smooth limiter
        if (desired_gain > g_limiter_gain) {
            g_limiter_gain = g_limiter_gain * (1.0f - ATTACK_FAST) + desired_gain * ATTACK_FAST;
        } else {
            g_limiter_gain = g_limiter_gain * (1.0f - RELEASE_SLOW) + desired_gain * RELEASE_SLOW;
        }

        // kick detection (simple)
        float peak = chunk_rms * g_limiter_gain;
        Uint32 now = SDL_GetTicks();
        if (peak > 0.12f * onset_factor && (now - last_kick_ts) > KICK_DEBOUNCE_MS) {
            kick_flag = 1;
            last_kick_ts = now;
            flash_start_ts = now;
            contrast_step = (contrast_step + 1) % TOTAL_CONTRAST_STEPS;
        } else {
            // decay kick flag after short time
            if (kick_flag && (now - last_kick_ts) > KICK_DEBOUNCE_MS) kick_flag = 0;
        }

        // prepare FFT input: take most recent FFT_SIZE samples from ring
        pthread_mutex_lock(&ring_mutex);
        size_t avail = g_ring_cap;
        size_t start = (g_ring_pos + g_ring_cap - FFT_SIZE) % g_ring_cap;
        for (int i = 0; i < FFT_SIZE; ++i) {
            fft_re[i] = (double) g_ring[(start + i) % g_ring_cap] * window_hann[i];
            fft_im[i] = 0.0;
        }
        pthread_mutex_unlock(&ring_mutex);

        // execute FFT
        fft_execute(fft_re, fft_im);
        // compute magnitude (only first half)
        for (int k = 0; k < FFT_SIZE/2; ++k) {
            mag[k] = sqrt(fft_re[k]*fft_re[k] + fft_im[k]*fft_im[k]);
        }

        // map bins to bands
        map_bins_to_bands(mag, bands, rate);

        // update running averages per band (attack/release)
        for (int b = 0; b < N_BANDS; ++b) {
            double v = bands[b];
            double prev = band_running_avg[b];
            double a = 0.15; // smoothing
            band_running_avg[b] = prev * (1.0 - a) + v * a;
        }

        // compute bandcols (RGB) from running avg using HSV mapping
        for (int b = 0; b < N_BANDS; ++b) {
            double val = band_running_avg[b];
            // normalize roughly
            double nv = val * 0.5;
            if (nv < 1e-8) nv = 1e-8;
            // hue influenced by global u_hue (we'll set u_hue below), but create a local hue ramp
            HSV h;
            h.h = fmod((double)b / (double)N_BANDS * 360.0 + contrast_step * 11.0, 360.0);
            h.s = 0.8;
            h.v = fmin(1.0, 0.2 + nv * 6.0);
            float rgb[3];
            hsv_to_rgb_double(&h, rgb);
            bandcols[b*3 + 0] = rgb[0];
            bandcols[b*3 + 1] = rgb[1];
            bandcols[b*3 + 2] = rgb[2];
        }

        // update textures
        // fill tex_samples from low->high freq sampling of bands (for shader convenience)
        // sample N points across bands into tex_samples
        for (int i = 0; i < FFT_SIZE; ++i) {
            int idx = (int)((double)i / (double)FFT_SIZE * (double)N_BANDS);
            if (idx < 0) idx = 0;
            if (idx >= N_BANDS) idx = N_BANDS - 1;
            // use band running avg magnitude as sample
            tex_samples[i] = (float) fmin(1.0, band_running_avg[idx] * 4.0);
        }

        glBindTexture(GL_TEXTURE_1D, tex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, FFT_SIZE, GL_RED, GL_FLOAT, tex_samples);

        glBindTexture(GL_TEXTURE_1D, bandtex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, N_BANDS, GL_RGB, GL_FLOAT, bandcols);

        // render
        glViewport(0, 0, win_w, win_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prog);
        double cur_time = (double)SDL_GetTicks() * 0.001;
        float iTime = (float)(cur_time - start_time);
        glUniform1f(loc_iTime, iTime);
        glUniform2f(loc_iResolution, (float)win_w, (float)win_h);

        // bind textures to units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tex);
        glUniform1i(loc_u_samples, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, bandtex);
        glUniform1i(loc_u_bandcols, 1);

        // set other uniforms (use multiple times in shader)
        glUniform1f(loc_u_vscale, 1.0f + 0.5f * g_limiter_gain);
        glUniform1f(loc_u_gain_display, fminf(2.0f, g_limiter_gain));
        // hue derived from running average low and contrast step
        float hue = fmodf((float)contrast_step / (float)TOTAL_CONTRAST_STEPS + 0.1f * (float)(running_avg_low), 1.0f);
        glUniform1f(loc_u_hue, hue);
        glUniform1i(loc_u_kick, kick_flag ? 1 : 0);
        glUniform1i(loc_u_nbands, N_BANDS);

        // flash brightness/color
        float flash_brightness = 0.0f;
        if (flash_start_ts) {
            Uint32 dt = SDL_GetTicks() - flash_start_ts;
            if (dt < FLASH_MS) flash_brightness = 1.0f - (float)dt / (float)FLASH_MS;
            else flash_start_ts = 0;
        }
        glUniform1f(loc_u_flash_brightness, flash_brightness);
        // flash color from contrast color
        HSV cur_hsv = get_next_contrast_color(contrast_step);
        float flash_rgb[3];
        hsv_to_rgb_double(&cur_hsv, flash_rgb);
        glUniform3f(loc_u_flash_color, flash_rgb[0], flash_rgb[1], flash_rgb[2]);

        // band aggregates for shader (bass/mid/treble)
        // compute simple aggregates from band_running_avg
        double bass_sum = 0.0, mid_sum = 0.0, treb_sum = 0.0;
        int bass_end = N_BANDS / 8;
        int mid_start = N_BANDS / 8; int mid_end = N_BANDS / 2;
        int treb_start = N_BANDS / 2;
        for (int b = 0; b < N_BANDS; ++b) {
            if (b < bass_end) bass_sum += band_running_avg[b];
            else if (b < mid_end) mid_sum += band_running_avg[b];
            else treb_sum += band_running_avg[b];
        }
        float band_bass = (float)(bass_sum / (double)fmax(1.0, (double)bass_end));
        float band_mid = (float)(mid_sum / (double)fmax(1.0, (double)(mid_end - mid_start)));
        float band_treble = (float)(treb_sum / (double)fmax(1.0, (double)(N_BANDS - treb_start)));
        // scale and clamp
        band_bass = fminf(8.0f, band_bass * 4.0f);
        band_mid = fminf(8.0f, band_mid * 4.0f);
        band_treble = fminf(8.0f, band_treble * 4.0f);

        glUniform1f(loc_band_bass, band_bass);
        glUniform1f(loc_band_mid, band_mid);
        glUniform1f(loc_band_treble, band_treble);

        // draw quad
        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(win);

        // frame limiter
        Uint32 now_ms = SDL_GetTicks();
        Uint32 elapsed = now_ms - last_frame;
        if (elapsed < FRAME_MS) SDL_Delay((Uint32)(FRAME_MS - elapsed));
        last_frame = SDL_GetTicks();

        // update running_avg_low slowly
        running_avg_low = running_avg_low * (1.0 - alpha_avg) + (double)chunk_rms * alpha_avg;
    }

    // cleanup
cleanup:
    if (bands) free(bands);
    if (read_buf) free(read_buf);
    if (bandcols) free(bandcols);
    if (tex_samples) free(tex_samples);
    if (band_running_avg) free(band_running_avg);
    if (g_ring) free(g_ring);

    if (g_pcm) { snd_pcm_close(g_pcm); g_pcm = NULL; }
    if (g_mixer) { snd_mixer_close(g_mixer); g_mixer = NULL; }

    if (prog) glDeleteProgram(prog);
    if (vs) glDeleteShader(vs);
    if (fs) glDeleteShader(fs);
    if (quad_vbo) glDeleteBuffers(1, &quad_vbo);
    if (quad_vao) glDeleteVertexArrays(1, &quad_vao);
    if (tex) glDeleteTextures(1, &tex);
    if (bandtex) glDeleteTextures(1, &bandtex);

    if (ctx) SDL_GL_DeleteContext(ctx);
    if (win) SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
