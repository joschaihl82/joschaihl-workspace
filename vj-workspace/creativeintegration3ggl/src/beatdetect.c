// main.c
// Merged, patched and scaled-up version (5x) of the previous programs.
// - Ensures ALSA capture switch is enabled and kept enabled
// - Uses an audio-driven raymarch fragment shader (creative tunnel)
// - Scales key resources by SCALE_FACTOR (buffers, bands, ring capacity, window size)
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

// --------------------------- Shaders ---------------------------
// Vertex shader (fullscreen quad)
static const char *fullscreen_vs = "#version 330 core\n"
"layout(location = 0) in vec2 a_pos;\n"
"void main() { gl_Position = vec4(a_pos, 0.0, 1.0); }\n";

// Creative raymarch fragment shader (audio-driven). Uses all shared uniforms.
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
"const float FOV = 0.45;\n"
"const float MarchDumping = 0.92;\n"
"const float FarBase = 20.0;\n"
"const int MaxSteps = 36;\n"
"const float CameraSpeedBase = 12.0;\n"
"const float TunnelRadiusBase = 0.9;\n"
"const float TunnelFreqA = 0.18;\n"
"const float TunnelFreqB = 0.25;\n"
"const float TunnelAmpA = 3.2;\n"
"const float TunnelAmpB = 2.4;\n"
"const vec3 BaseColor = vec3(0.08, 0.66, 1.0);\n"
"#define M_NONE -1.0\n"
"#define M_TUNNEL 1.0\n"
"float audioPattern(vec3 p, float bass, float mid, float treb, float gain, float hue) {\n"
"    float sx = clamp((p.x * 0.5 + 0.5), 0.0, 0.999);\n"
"    float sy = clamp((p.y * 0.5 + 0.5), 0.0, 0.999);\n"
"    float s1 = texture(u_samples, sx).r;\n"
"    float s2 = texture(u_samples, sy).r;\n"
"    float a = (0.6 * bass + 0.3 * mid + 0.1 * treb) * (0.5 + 0.5 * s1);\n"
"    float b = (0.2 * bass + 0.7 * mid + 0.1 * treb) * (0.5 + 0.5 * s2);\n"
"    float c = (0.3 * gain + 0.2 * hue) * (0.5 + 0.5 * s1 * s2);\n"
"    float tosc = sin(iTime * (1.0 + 0.6 * bass) + p.z * (0.5 + 0.5 * mid));\n"
"    return clamp(a * 0.8 + b * 0.5 + c * 0.4 + 0.25 * tosc, -1.0, 1.0);\n"
"}\n"
"vec2 pathLayer(float z, float layerOffset, float mid, float treb, float hue) {\n"
"    float ampA = TunnelAmpA * (1.0 + 0.25 * mid) * (1.0 + 0.15 * layerOffset);\n"
"    float ampB = TunnelAmpB * (1.0 + 0.25 * treb) * (1.0 + 0.12 * layerOffset);\n"
"    float fA = TunnelFreqA * (1.0 + 0.05 * hue) * (1.0 + 0.02 * layerOffset);\n"
"    float fB = TunnelFreqB * (1.0 + 0.05 * hue) * (1.0 + 0.02 * layerOffset);\n"
"    return vec2(ampA * sin(z * fA + layerOffset * 0.7), ampB * cos(z * fB + layerOffset * 0.9));\n"
"}\n"
"float distToTunnelLayer(vec3 p, float layerOffset, float bass, float mid, float treb, float gain, float hue, float baseRadius) {\n"
"    vec2 center = pathLayer(p.z, layerOffset, mid, treb, hue);\n"
"    float d2 = length(p.xy - center) - baseRadius * (1.0 + 0.12 * layerOffset);\n"
"    float pat = audioPattern(p + vec3(layerOffset, layerOffset, layerOffset * 0.3), bass, mid, treb, gain, hue);\n"
"    float detail = 0.18 * pat * (0.5 + 0.5 * mid) * (1.0 + 0.6 * treb) * (1.0 + 0.4 * layerOffset);\n"
"    return d2 + detail;\n"
"}\n"
"vec2 mapScene(vec3 p, float bass, float mid, float treb, float gain, float hue) {\n"
"    float baseRadius = TunnelRadiusBase * (1.0 - 0.25 * bass);\n"
"    float d = 1e6;\n"
"    for (int L = 0; L < 4; ++L) {\n"
"        float lo = float(L);\n"
"        float dl = distToTunnelLayer(p, lo, bass, mid, treb, gain, hue, baseRadius);\n"
"        float k = 0.25 + 0.6 * (1.0 - mid);\n"
"        float h = clamp(0.5 + 0.5 * (d - dl) / k, 0.0, 1.0);\n"
"        d = mix(dl, d, h) - k * h * (1.0 - h);\n"
"    }\n"
"    float bubbleMin = 1e6;\n"
"    for (int b = 0; b < 6; ++b) {\n"
"        float t = iTime * (0.6 + 0.4 * gain) + float(b) * 1.2;\n"
"        vec2 c = pathLayer(t, float(b) * 0.3, mid, treb, hue);\n"
"        float zpos = t + 0.5 * (0.2 * bass + 0.1 * mid);\n"
"        vec3 bc = vec3(c.x, c.y, zpos);\n"
"        float samplePos = fract(0.1 * float(b) + 0.05 * u_band_bass);\n"
"        float s = texture(u_samples, samplePos).r;\n"
"        float br = 0.18 + 0.12 * (0.5 + 0.5 * s) + 0.08 * u_band_bass;\n"
"        float sd = length(p - bc) - br * (1.0 + 0.6 * u_band_mid);\n"
"        bubbleMin = min(bubbleMin, sd);\n"
"    }\n"
"    float sceneD = min(d, bubbleMin);\n"
"    return vec2(sceneD, M_TUNNEL);\n"
"}\n"
"float softshadow(vec3 ro, vec3 rd, float mint, float tmax, float bass, float mid, float treb, float gain, float hue) {\n"
"    float res = 1.0;\n"
"    float t = mint;\n"
"    for (int i = 0; i < 4; ++i) {\n"
"        float h = mapScene(ro + rd * t, bass, mid, treb, gain, hue).x;\n"
"        res = min(res, 5.0 * h / max(0.001, t));\n"
"        t += clamp(h, 0.04, 0.14);\n"
"        if (h < 0.002 || t > tmax) break;\n"
"    }\n"
"    return clamp(res, 0.0, 1.0);\n"
"}\n"
"vec3 calcNormal(vec3 pos, float bass, float mid, float treb, float gain, float hue) {\n"
"    vec2 eps = vec2(0.004, 0.0);\n"
"    vec3 nor = vec3(\n"
"        mapScene(pos + eps.xyy, bass, mid, treb, gain, hue).x - mapScene(pos - eps.xyy, bass, mid, treb, gain, hue).x,\n"
"        mapScene(pos + eps.yxy, bass, mid, treb, gain, hue).x - mapScene(pos - eps.yxy, bass, mid, treb, gain, hue).x,\n"
"        mapScene(pos + eps.yyx, bass, mid, treb, gain, hue).x - mapScene(pos - eps.yyx, bass, mid, treb, gain, hue).x\n"
"    );\n"
"    return normalize(nor);\n"
"}\n"
"vec2 castRay(vec3 ro, vec3 rd, float bass, float mid, float treb, float gain, float hue) {\n"
"    float t = 0.0;\n"
"    float tmax = FarBase * (1.0 + 0.25 * treb);\n"
"    float m = M_NONE;\n"
"    for (int i = 0; i < MaxSteps; ++i) {\n"
"        vec3 pos = ro + rd * t;\n"
"        vec2 res = mapScene(pos, bass, mid, treb, gain, hue);\n"
"        float dist = res.x;\n"
"        if (dist < 0.001) { m = res.y; break; }\n"
"        float stepMul = MarchDumping * (1.0 + 0.6 * bass) * (1.0 + 0.4 * gain) * (1.0 + 0.2 * mid);\n"
"        t += max(0.001, dist) * stepMul;\n"
"        if (t > tmax) break;\n"
"    }\n"
"    if (t > tmax) m = M_NONE;\n"
"    return vec2(t, m);\n"
"}\n"
"vec3 render(vec3 ro, vec3 rd, float bass, float mid, float treb, float gain, float hue) {\n"
"    vec3 col = vec3(0.0);\n"
"    vec2 res = castRay(ro, rd, bass, mid, treb, gain, hue);\n"
"    float t = res.x;\n"
"    float m = res.y;\n"
"    if (m > -0.5) {\n"
"        vec3 pos = ro + t * rd;\n"
"        vec3 nor = calcNormal(pos, bass, mid, treb, gain, hue);\n"
"        float bandIdx = fract(t * (0.1 + 0.05 * bass)) * float(u_nbands);\n"
"        vec3 bandcol = texture(u_bandcols, (bandIdx + 0.5) / float(u_nbands)).rgb;\n"
"        vec3 hueShift = vec3(0.5 + 0.5 * sin(hue * 6.2831853 + 0.0), 0.5 + 0.5 * sin(hue * 6.2831853 + 2.0), 0.5 + 0.5 * sin(hue * 6.2831853 + 4.0));\n"
"        vec3 base = mix(BaseColor * (1.0 + 0.6 * mid), bandcol * (0.6 + 0.8 * bass), 0.6);\n"
"        base *= (0.9 + 0.2 * hueShift);\n"
"        float depthOsc = sin(t * (0.5 + 0.3 * treb) + iTime * (0.2 + 0.6 * gain));\n"
"        col = base + depthOsc * (0.2 + 0.3 * mid);\n"
"        vec3 lig = -rd;\n"
"        float dif = clamp(dot(nor, lig), 0.0, 1.0);\n"
"        dif *= softshadow(pos, lig, 0.03, 1.8, bass, mid, treb, gain, hue);\n"
"        vec3 brdf = vec3(0.0);\n"
"        brdf += 0.9 * dif * vec3(1.00, 0.90, 0.60);\n"
"        float spe = pow(clamp(dot(reflect(rd, nor), lig), 0.0, 1.0), 16.0);\n"
"        brdf += 0.9 * spe * vec3(1.00, 0.90, 0.60) * dif;\n"
"        nor = normalize(nor - normalize(pos) * 0.12);\n"
"        spe = pow(clamp(dot(reflect(rd, nor), lig), 0.0, 1.0), 16.0);\n"
"        brdf += 1.2 * spe * vec3(1.00, 0.90, 0.60) * dif;\n"
"        brdf += 0.3 * (0.4 + 0.6 * mid) * vec3(0.50, 0.70, 1.00);\n"
"        col = col * brdf;\n"
"        col = mix(col, vec3(0.0), 1.0 - exp(-0.08 * t));\n"
"    }\n"
"    return clamp(col, 0.0, 1.0);\n"
"}\n"
"void main() {\n"
"    if (u_flash_brightness > 0.001) {\n"
"        vec3 flash = u_flash_color * (1.0 + 0.8 * u_band_bass) * (0.6 + 0.4 * u_gain_display);\n"
"        float alpha = u_flash_brightness * (0.8 + 0.4 * u_band_mid);\n"
"        fragColor = vec4(flash, alpha);\n"
"        return;\n"
"    }\n"
"    vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
"    vec2 coord = 2.0 * uv - 1.0;\n"
"    coord.x *= iResolution.x / iResolution.y;\n"
"    float s_bass = texture(u_samples, 0.02).r;\n"
"    float s_mid  = texture(u_samples, 0.25).r;\n"
"    float s_treb = texture(u_samples, 0.75).r;\n"
"    float bass = clamp(u_band_bass, 0.0, 8.0);\n"
"    float mid  = clamp(u_band_mid, 0.0, 8.0);\n"
"    float treb = clamp(u_band_treble, 0.0, 8.0);\n"
"    float gain = clamp(u_gain_display, 0.0, 2.0);\n"
"    float hue = clamp(u_hue, 0.0, 1.0);\n"
"    float cameraSpeed = CameraSpeedBase * (1.0 + 1.2 * bass * gain + 0.6 * mid * s_mid);\n"
"    float timeMod = iTime * cameraSpeed + 4.0 * 60.0 * (1.0 + 0.1 * s_bass);\n"
"    vec3 ro = vec3(pathLayer(timeMod, 0.0, mid, treb, hue).xy, timeMod);\n"
"    vec3 target = vec3(pathLayer(timeMod + 0.5, 0.0, mid, treb, hue).xy, timeMod + 0.5);\n"
"    vec3 dir = normalize(target - ro);\n"
"    vec3 up = vec3(-0.9309864, -0.33987653, 0.1332234);\n"
"    vec3 upOrtho = normalize(up - dot(dir, up) * dir);\n"
"    vec3 right = normalize(cross(dir, upOrtho));\n"
"    vec3 rd = normalize(dir + coord.x * right + coord.y * upOrtho);\n"
"    vec3 col = render(ro, rd, bass, mid, treb, gain, hue);\n"
"    vec3 bandAvg = texture(u_bandcols, 0.5).rgb;\n"
"    col = mix(col, bandAvg * (0.8 + 0.4 * bass), 0.12 * gain * (0.5 + 0.5 * hue));\n"
"    if (u_kick > 0) col += vec3(0.6 * s_bass * (1.0 + 0.8 * u_band_bass));\n"
"    col = pow(col * (1.0 + 0.08 * gain + 0.06 * mid), vec3(0.4545));\n"
"    fragColor = vec4(col, 1.0);\n"
"}\n";

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

    SDL_Window *win = SDL_CreateWindow("BeatDetect Merged 5x",
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
    double bands[N_BANDS]; // stack may be large; N_BANDS scaled but still reasonable for typical systems
    // If N_BANDS is too large for stack, consider malloc; here we assume it's acceptable.

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

        // RMS chunk
        float chunk_rms = compute_rms_s16(read_buf, (size_t) frames);

        // limiter / auto-gain
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

        // push into ring
        push_to_ring_from_s16_locked(read_buf, (size_t) frames);

        // snapshot for FFT
        pthread_mutex_lock(&ring_mutex);
        size_t start = (g_ring_pos + g_ring_cap - (size_t) FFT_SIZE) % g_ring_cap;
        for (int i = 0; i < FFT_SIZE; ++i) {
            double v = g_ring[(start + i) % g_ring_cap];
            fft_re[i] = v * window_hann[i];
            fft_im[i] = 0.0;
            tex_samples[i] = (float) v;
        }
        pthread_mutex_unlock(&ring_mutex);

        // FFT + map bins->bands
        fft_execute(fft_re, fft_im);
        int half = FFT_SIZE / 2;
        for (int b = 0; b < half; ++b) mag[b] = sqrt(fft_re[b] * fft_re[b] + fft_im[b] * fft_im[b]);
        map_bins_to_bands(mag, bands, rate);

        // per-band normalization + color mapping
        double bass_sum = 0.0, mid_sum = 0.0, treb_sum = 0.0;
        int bass_count = 0, mid_count = 0, treb_count = 0;
        for (int b = 0; b < N_BANDS; ++b) {
            band_running_avg[b] = 0.05 * bands[b] + 0.95 * band_running_avg[b];
            double norm = bands[b] / (band_running_avg[b] + 1e-12);
            double bright = 1.0 / (1.0 + exp(-12.0 * (norm - 1.8))); // reuse logistic
            if (bright < 0.0) bright = 0.0; if (bright > 1.0) bright = 1.0;
            if (b == 0) {
                bandcols[3*b + 0] = (float) bright;
                bandcols[3*b + 1] = (float) bright;
                bandcols[3*b + 2] = (float) bright;
            } else {
                double hue = (double)(b - 1) / (double)(N_BANDS - 1) * 0.85;
                HSV tmp = { hue * 360.0, 1.0, bright };
                float rgb[3];
                hsv_to_rgb_double(&tmp, rgb);
                bandcols[3*b + 0] = rgb[0];
                bandcols[3*b + 1] = rgb[1];
                bandcols[3*b + 2] = rgb[2];
            }
            // accumulate band summaries
            if (b < N_BANDS / 8) { bass_sum += norm; bass_count++; }
            else if (b < (N_BANDS * 5) / 8 && b >= (N_BANDS * 3) / 8) { mid_sum += norm; mid_count++; }
            else if (b >= (N_BANDS * 3) / 4) { treb_sum += norm; treb_count++; }
        }
        float band_bass = (bass_count > 0) ? (float)(bass_sum / bass_count) : 0.0f;
        float band_mid  = (mid_count > 0) ? (float)(mid_sum / mid_count) : 0.0f;
        float band_treble = (treb_count > 0) ? (float)(treb_sum / treb_count) : 0.0f;

        // kick detection (low bands)
        double low_sum = 0.0;
        int low_bands = 4 * SCALE_FACTOR; // scaled low-band window
        if (low_bands > N_BANDS) low_bands = N_BANDS;
        for (int i = 0; i < low_bands; ++i) low_sum += bands[i];
        low_sum /= (double) low_bands;
        running_avg_low = alpha_avg * low_sum + (1.0 - alpha_avg) * running_avg_low;
        Uint32 now = SDL_GetTicks();
        bool detected = false;
        if (low_sum > running_avg_low * onset_factor && low_sum > 1e-8) {
            if ((now - last_kick_ts) > KICK_DEBOUNCE_MS) {
                detected = true;
                last_kick_ts = now;
                flash_start_ts = now;
            }
        }
        if (detected) {
            contrast_step = (contrast_step + 1) % TOTAL_CONTRAST_STEPS;
            HSV bg_hsv = get_next_contrast_color(contrast_step);
            float bg_rgb2[3]; hsv_to_rgb_double(&bg_hsv, bg_rgb2);
            glClearColor(bg_rgb2[0], bg_rgb2[1], bg_rgb2[2], 1.0f);
        }

        // flash brightness logistic decay
        float flash_brightness = 0.0f;
        if (flash_start_ts != 0) {
            Uint32 nowf = SDL_GetTicks();
            double elapsed = (double)(nowf - flash_start_ts);
            if (elapsed < 0.0) elapsed = 0.0;
            double t = elapsed / (double) FLASH_MS;
            if (t <= 1.0) {
                double b = 1.0 / (1.0 + exp(flash_k * (t - flash_x0)));
                if (b < 0.0) b = 0.0; if (b > 1.0) b = 1.0;
                flash_brightness = (float) b;
                kick_flag = 2;
            } else {
                flash_brightness = 0.0f;
                flash_start_ts = 0;
                kick_flag = 0;
            }
        } else { kick_flag = 0; flash_brightness = 0.0f; }

        // upload textures
        pthread_mutex_lock(&ring_mutex);
        start = (g_ring_pos + g_ring_cap - (size_t) FFT_SIZE) % g_ring_cap;
        for (int i = 0; i < FFT_SIZE; ++i) tex_samples[i] = g_ring[(start + i) % g_ring_cap];
        pthread_mutex_unlock(&ring_mutex);

        glBindTexture(GL_TEXTURE_1D, tex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, FFT_SIZE, GL_RED, GL_FLOAT, tex_samples);

        glBindTexture(GL_TEXTURE_1D, bandtex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, N_BANDS, GL_RGB, GL_FLOAT, bandcols);

        // render pass
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(prog);

        float tsec = (float) SDL_GetTicks() * 0.001f;
        glUniform1f(loc_iTime, tsec);
        glUniform2f(loc_iResolution, (float)win_w, (float)win_h);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tex);
        glUniform1i(loc_u_samples, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, bandtex);
        glUniform1i(loc_u_bandcols, 1);

        glUniform1f(loc_u_vscale, 0.4f);
        glUniform1f(loc_u_gain_display, fminf(1.0f, g_limiter_gain));
        float hue_norm = (float)contrast_step / (float)TOTAL_CONTRAST_STEPS;
        glUniform1f(loc_u_hue, hue_norm);
        glUniform1i(loc_u_kick, kick_flag);
        glUniform1i(loc_u_nbands, N_BANDS);
        glUniform1f(loc_u_flash_brightness, flash_brightness);
        glUniform3f(loc_u_flash_color, 1.0f, 1.0f, 1.0f);

        glUniform1f(loc_band_bass, band_bass);
        glUniform1f(loc_band_mid, band_mid);
        glUniform1f(loc_band_treble, band_treble);

        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glUseProgram(0);

        SDL_GL_SwapWindow(win);

        // frame pacing
        Uint32 nowf = SDL_GetTicks();
        Uint32 elapsed = nowf - last_frame;
        if (elapsed < FRAME_MS) SDL_Delay((Uint32)(FRAME_MS - elapsed));
        last_frame = SDL_GetTicks();
    }

    // cleanup
cleanup:
    if (g_pcm) { snd_pcm_close(g_pcm); g_pcm = NULL; }
    if (g_mixer) { snd_mixer_close(g_mixer); g_mixer = NULL; }
    if (g_ring) { free(g_ring); g_ring = NULL; }
    if (band_running_avg) { free(band_running_avg); band_running_avg = NULL; }
    if (tex_samples) { free(tex_samples); tex_samples = NULL; }
    if (bandcols) { free(bandcols); bandcols = NULL; }
    if (read_buf) { free(read_buf); read_buf = NULL; }
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
