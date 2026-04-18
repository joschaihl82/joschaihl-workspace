// mrt.c
// Radionullon MRT-Visualizer: Hochfrequenz-Wischer + X-Ray-Overlay
// Dynamische Selbstanpassung (Online-Optimizer) für Kanten-Gain, Residuum-Schwelle, Overlay-Stärke
// V4L2 + SDL2, Einzeldatei

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

// -----------------------------------------------------------------------------
// Globale Parameter
// -----------------------------------------------------------------------------
#define WIDTH 640
#define HEIGHT 480
#define DEVICE "/dev/video0"

// Bildmetriken / Filter
#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN   1.6f
#define HEATMAP_ALPHA 155

// Balkenkonfiguration
#define BAR_BASE_HEIGHT 20
#define BAR_HEIGHT_AMPL 24
#define BAR_SPACING 90

// Sweep-Zeiten
#define CYCLE_LOW_MS 1200.0f
#define CYCLE_HIGH_MS 220.0f
#define EXPLORATION_PERIOD_MS 5000

// Laufzeit-Variablen
static float cycle_duration_ms = CYCLE_HIGH_MS; // Hochfrequenz bevorzugt
static float mix_weight = 1.0f;                 // nur schnell

// -----------------------------------------------------------------------------
// V4L2 Buffer
// -----------------------------------------------------------------------------
struct buffer { void *start; size_t length; };
static struct buffer *buffers = NULL;
static unsigned int  n_buffers = 0;
static int v4l2_fd = -1;

// Arbeits-Puffer
static uint8_t *gray    = NULL;
static float   *bg      = NULL;
static uint8_t *residue = NULL;
static uint8_t *edge    = NULL;
static uint8_t *flicker = NULL;
static uint8_t *rgb     = NULL;

// SDL
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------
static inline uint8_t clamp_u8(int v) {
  if (v < 0) return 0;
  if (v > 255) return 255;
  return (uint8_t)v;
}
static inline float fclamp(float v, float lo, float hi) {
  return (v < lo) ? lo : (v > hi) ? hi : v;
}

// -----------------------------------------------------------------------------
// Adaptive Optimizer: Runtime-Metriken & State
// -----------------------------------------------------------------------------
typedef struct {
  // EMA-Metriken
  float ema_edge_energy;
  float ema_res_density;
  float ema_vis;
  float ema_flicker;
  // Steuerparameter
  float edge_gain;
  float heatmap_alpha;
  float residue_thresh;
  // Lernraten und Grenzen
  float lr_gain, lr_alpha, lr_thresh;
  float edge_gain_min, edge_gain_max;
  float alpha_min, alpha_max;
  float thresh_min, thresh_max;
} OptState;

static OptState OPT = {
  .ema_edge_energy = 0.0f,
  .ema_res_density = 0.0f,
  .ema_vis         = 0.0f,
  .ema_flicker     = 0.0f,
  .edge_gain       = EDGE_GAIN,
  .heatmap_alpha   = HEATMAP_ALPHA,
  .residue_thresh  = RESIDUE_THRESH,
  .lr_gain = 0.02f, .lr_alpha = 4.0f, .lr_thresh = 0.10f,
  .edge_gain_min = 1.2f, .edge_gain_max = 2.2f,
  .alpha_min = 110.0f, .alpha_max = 200.0f,
  .thresh_min = 12.0f, .thresh_max = 26.0f
};

// Kostenfunktion-Gewichte
typedef struct {
  float w_vis;
  float w_var;
  float w_res;
  float w_reg_a;
  float w_reg_g;
} CostWeights;

static CostWeights CW = {
  .w_vis = 1.0f,
  .w_var = 0.9f,
  .w_res = 0.6f,
  .w_reg_a = 0.0025f,
  .w_reg_g = 0.0040f
};

static const float TGT_VIS = 28.0f;
static const float TGT_RES = 0.08f;
static float EMA_VAR_VIS = 0.0f; // Varianz-EMA

typedef struct { float mean, var; } Stat2;
static Stat2 rolling_var(float x, float ema_mean, float ema_var, float beta) {
  float mean = (1.0f - beta)*ema_mean + beta*x;
  float dev  = x - mean;
  float var  = (1.0f - beta)*ema_var + beta*(dev*dev);
  Stat2 s = { mean, var };
  return s;
}

typedef struct {
  float edge_energy;  // mittlerer Edge-Wert
  float res_density;  // Anteil Pixel mit residuum > 0
  float flicker_mean; // mittlerer Flicker-Wert
} FrameMetrics;

static FrameMetrics compute_metrics(const uint8_t *res, const uint8_t *edge, const uint8_t *flicker) {
  const int N = WIDTH * HEIGHT;
  long edge_sum = 0, flick_sum = 0;
  int res_count = 0;
  for (int i = 0; i < N; i++) {
    edge_sum += edge[i];
    flick_sum += flicker[i];
    if (res[i] > 0) res_count++;
  }
  FrameMetrics m;
  m.edge_energy = (float)edge_sum / (float)N;
  m.res_density = (float)res_count / (float)N;
  m.flicker_mean = (float)flick_sum / (float)N;
  return m;
}

static float compute_cost(float ema_vis, float ema_res, float ema_var_v,
                          float alpha_norm, float gain) {
  float e_vis = TGT_VIS - ema_vis;
  float e_res = TGT_RES - ema_res;
  float J = CW.w_vis*(e_vis*e_vis)
          + CW.w_var*(ema_var_v*ema_var_v)
          + CW.w_res*(e_res*e_res)
          + CW.w_reg_a*(alpha_norm*alpha_norm)
          + CW.w_reg_g*(gain*gain);
  return J;
}

static void gradient_step(uint32_t now_ms,
                          float ema_vis, float ema_res, float ema_var_v,
                          FrameMetrics fm, float vis_score) {
  (void)now_ms; (void)fm; (void)vis_score;

  float alpha0 = OPT.heatmap_alpha;
  float gain0  = OPT.edge_gain;
  float thr0   = OPT.residue_thresh;

  float J0 = compute_cost(ema_vis, ema_res, ema_var_v, alpha0/255.0f, gain0);

  const float d_alpha = 4.0f;
  const float d_gain  = 0.02f;
  const float d_thr   = 0.4f;

  float ema_flick = OPT.ema_flicker;

  float ema_vis_gain_up = ema_vis + 0.12f * (TGT_VIS - ema_vis) * 0.5f;
  float ema_var_gain_up = ema_var_v + 0.06f * fclamp(ema_flick/50.0f, 0.0f, 1.0f);
  float J_gain_up = compute_cost(ema_vis_gain_up, ema_res, ema_var_gain_up, alpha0/255.0f, (gain0 + d_gain));
  float dJ_dGain = (J_gain_up - J0) / d_gain;

  float ema_res_thr_up = fmaxf(0.0f, ema_res - 0.15f * ema_res);
  float ema_vis_thr_up = ema_vis - 0.05f * ema_vis;
  float J_thr_up = compute_cost(ema_vis_thr_up, ema_res_thr_up, ema_var_v, alpha0/255.0f, gain0);
  float dJ_dThr = (J_thr_up - J0) / d_thr;

  float J_alpha_up = compute_cost(ema_vis, ema_res, ema_var_v, (alpha0 + d_alpha)/255.0f, gain0);
  float dJ_dAlpha = (J_alpha_up - J0) / d_alpha;

  const float lr_g = 0.06f;
  const float lr_t = 0.18f;
  const float lr_a = 0.9f;

  OPT.edge_gain      = fclamp(gain0  - lr_g * dJ_dGain,  OPT.edge_gain_min, OPT.edge_gain_max);
  OPT.residue_thresh = fclamp(thr0   - lr_t * dJ_dThr,   OPT.thresh_min,    OPT.thresh_max);
  OPT.heatmap_alpha  = fclamp(alpha0 - lr_a * dJ_dAlpha, OPT.alpha_min,     OPT.alpha_max);

  const float max_d_alpha = 6.0f;
  const float max_d_gain  = 0.06f;
  const float max_d_thr   = 1.2f;
  OPT.heatmap_alpha = fclamp(OPT.heatmap_alpha, alpha0 - max_d_alpha, alpha0 + max_d_alpha);
  OPT.edge_gain     = fclamp(OPT.edge_gain,     gain0  - max_d_gain,  gain0  + max_d_gain);
  OPT.residue_thresh= fclamp(OPT.residue_thresh,thr0   - max_d_thr,   thr0   + max_d_thr);
}

static void dynamic_autotune_combined(uint32_t now_ms, FrameMetrics m, float vis) {
  static float ema_edge=0, ema_res=0, ema_vis=0, ema_flick=0;
  const float ema = 0.12f;
  ema_edge  = (1-ema)*ema_edge  + ema*m.edge_energy;
  ema_res   = (1-ema)*ema_res   + ema*m.res_density;
  ema_vis   = (1-ema)*ema_vis   + ema*vis;
  ema_flick = (1-ema)*ema_flick + ema*m.flicker_mean;

  Stat2 st = rolling_var(vis, ema_vis, EMA_VAR_VIS, 0.15f);
  EMA_VAR_VIS = st.var;

  float err_edge = (38.0f - ema_edge);
  float err_res  = (0.08f - ema_res);
  float err_vis  = (28.0f - ema_vis);

  float flick_damp = 1.0f - fclamp(ema_flick/50.0f, 0.0f, 0.6f);
  float pump_damp  = 1.0f - fclamp(EMA_VAR_VIS/80.0f, 0.0f, 0.5f);

  OPT.edge_gain += OPT.lr_gain * err_edge * flick_damp * pump_damp;
  OPT.edge_gain = fclamp(OPT.edge_gain, OPT.edge_gain_min, OPT.edge_gain_max);

  OPT.residue_thresh += OPT.lr_thresh * (-err_res) * pump_damp;
  OPT.residue_thresh = fclamp(OPT.residue_thresh, OPT.thresh_min, OPT.thresh_max);

  float alpha_step = OPT.lr_alpha * (0.7f*err_vis + 0.3f*err_edge*0.2f) * pump_damp;
  alpha_step = fclamp(alpha_step, -6.0f, 6.0f);
  OPT.heatmap_alpha = fclamp(OPT.heatmap_alpha + alpha_step, OPT.alpha_min, OPT.alpha_max);

  float wobble = 0.3f * sinf(2.0f * M_PI * (0.2f * ((float)now_ms / 1000.0f)));
  OPT.edge_gain = fclamp(OPT.edge_gain + 0.01f*wobble, OPT.edge_gain_min, OPT.edge_gain_max);

  gradient_step(now_ms, ema_vis, ema_res, EMA_VAR_VIS, m, vis);
}

// -----------------------------------------------------------------------------
// Bildverarbeitung
// -----------------------------------------------------------------------------
static void yuyv_to_gray_rgb(const uint8_t *in, uint8_t *g, uint8_t *out_rgb) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x += 2) {
      int idx = (y*WIDTH + x);
      int p = (y*WIDTH + x) * 2;

      uint8_t Y0 = in[p + 0];
      uint8_t U  = in[p + 1];
      uint8_t Y1 = in[p + 2];
      uint8_t V  = in[p + 3];

      g[idx]     = Y0;
      g[idx + 1] = Y1;

      int C0 = Y0 - 16, C1 = Y1 - 16;
      int D  = U  - 128, E  = V  - 128;

      int R0 = (298*C0 + 409*E + 128) >> 8;
      int G0 = (298*C0 - 100*D - 208*E + 128) >> 8;
      int B0 = (298*C0 + 516*D + 128) >> 8;

      int R1 = (298*C1 + 409*E + 128) >> 8;
      int G1 = (298*C1 - 100*D - 208*E + 128) >> 8;
      int B1 = (298*C1 + 516*D + 128) >> 8;

      int o = (idx * 3);
      out_rgb[o + 0] = clamp_u8(R0);
      out_rgb[o + 1] = clamp_u8(G0);
      out_rgb[o + 2] = clamp_u8(B0);
      out_rgb[o + 3] = clamp_u8(R1);
      out_rgb[o + 4] = clamp_u8(G1);
      out_rgb[o + 5] = clamp_u8(B1);
    }
  }
}

// Sobel-Kanten (mit dynamischem Gain)
static void sobel_edge(const uint8_t *g, uint8_t *e) {
  memset(e, 0, WIDTH*HEIGHT);
  for (int y = 1; y < HEIGHT-1; y++) {
    for (int x = 1; x < WIDTH-1; x++) {
      int i = y*WIDTH + x;
      int gx =
        -g[i - WIDTH - 1] - 2*g[i - 1] - g[i + WIDTH - 1] +
         g[i - WIDTH + 1] + 2*g[i + 1] + g[i + WIDTH + 1];
      int gy =
        -g[i - WIDTH - 1] - 2*g[i - WIDTH] - g[i - WIDTH + 1] +
         g[i + WIDTH - 1] + 2*g[i + WIDTH] + g[i + WIDTH + 1];
      int mag = (int)(OPT.edge_gain * sqrtf((float)(gx*gx + gy*gy)));
      e[i] = clamp_u8(mag);
    }
  }
}

// Hintergrund-EMA & Residuum (mit dynamischer Schwelle)
static void update_bg_and_residue(const uint8_t *g, float *bg, uint8_t *res) {
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    bg[i] = (1.0f - BG_LEARN_RATE) * bg[i] + BG_LEARN_RATE * (float)g[i];
    int diff = abs((int)g[i] - (int)bg[i]);
    res[i] = (diff > (int)OPT.residue_thresh) ? (uint8_t)diff : 0;
  }
}

// Globales Flackern
static void update_flicker(const uint8_t *g, uint8_t *f) {
  static uint8_t prev = 0;
  uint64_t sum = 0;
  for (int i = 0; i < WIDTH*HEIGHT; i++) sum += g[i];
  uint8_t avg = (uint8_t)(sum / (WIDTH*HEIGHT));

  int delta = abs((int)avg - (int)prev);
  prev = avg;
  uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    int v = f[i] + boost;
    f[i] = (uint8_t)((v > 255) ? 255 : v);
    if (f[i] > 0) f[i] -= 1;
  }
}

// Sichtbarkeits-Score für Band
static float estimate_visibility_score(const uint8_t *res, const uint8_t *edge,
                                       int band_y_start, int band_y_end) {
  int ys = (band_y_start < 0) ? 0 : band_y_start;
  int ye = (band_y_end > HEIGHT) ? HEIGHT : band_y_end;
  if (ys >= ye) return 0.0f;

  double sum = 0.0;
  int samples = 0;

  for (int y = ys; y < ye; y += 2) {
    for (int x = 0; x < WIDTH; x += 2) {
      int i = y * WIDTH + x;
      int s = (int)res[i] + (int)((edge[i] * 2) / 3); // HF-betont
      sum += (double)s;
      samples++;
    }
  }

  if (samples == 0) return 0.0f;
  return (float)(sum / (double)samples);
}

// adaptive Sweep-Mischung (hier minimal, HF bleibt aktiv)
static void update_cycle_duration(uint32_t now_ms, float vis_score) {
  (void)now_ms; (void)vis_score;
  cycle_duration_ms = CYCLE_HIGH_MS; // nur schnell
}

// -----------------------------------------------------------------------------
// Hochfrequenz-Wischer mit IR↔UV-Frequenz-Mapping (visuell), Balken in RGB
// -----------------------------------------------------------------------------
static float draw_falling_horizontal_bars_and_score(uint8_t *out_rgb,
                                                    const uint8_t *res, const uint8_t *edge,
                                                    uint32_t now_ms) {
  cycle_duration_ms = CYCLE_HIGH_MS;

  uint32_t ms_in_cycle = (uint32_t)fmodf((float)now_ms, cycle_duration_ms);
  float progress = (float)ms_in_cycle / cycle_duration_ms;

  const float width_period_ms = 900.0f; // schnellerer Puls
  float wphase = fmodf((float)now_ms, width_period_ms) / width_period_ms;
  float wsin = 0.5f * (sinf(2.0f * M_PI * wphase) + 1.0f);
  int bar_height = BAR_BASE_HEIGHT + (int)(BAR_HEIGHT_AMPL * wsin);

  int base_y_start = (int)(progress * (HEIGHT + bar_height)) - bar_height;
  int base_y_end   = base_y_start + bar_height;
  int max_rows = (HEIGHT / BAR_SPACING) + 2;

  const float t_sec = (float)now_ms / 1000.0f;
  const float fc_base_hz  = 18.0f;
  const float fc_step_hz  = 2.5f;
  const float fdev_hz     = 7.0f;
  const float fm_base_hz  = 1.1f;
  const float fm_step_hz  = 0.15f;
  const float fm_phase_k  = 0.33f;

  // IR↔UV Hue-Bereich (visuell, Bildschirm)
  const float hue_ir = 0.00f;  // Rot
  const float hue_uv = 0.80f;  // Violett

  const float base_sat = 0.85f;
  const float base_val = 0.95f;

  for (int r = -max_rows; r <= max_rows; r++) {
    int y_start = base_y_start + r * BAR_SPACING;
    int y_end   = y_start + bar_height;
    int ys = (y_start < 0) ? 0 : y_start;
    int ye = (y_end   > HEIGHT) ? HEIGHT : y_end;
    if (ys >= ye) continue;

    int idx = r + max_rows;

    float fc = fc_base_hz + fc_step_hz * (float)idx;
    float fm = fm_base_hz + fm_step_hz * (float)idx;
    float phi = fm_phase_k * (float)idx;
    float fm_phase = sinf(2.0f * M_PI * (fm * t_sec + phi));
    float freq_hz = fc + fdev_hz * fm_phase;

    float f_min = fc - fdev_hz;
    float f_max = fc + fdev_hz;
    float f_norm = (freq_hz - f_min) / (f_max - f_min);
    if (f_norm < 0.0f) f_norm = 0.0f;
    if (f_norm > 1.0f) f_norm = 1.0f;

    float hue = hue_ir + (hue_uv - hue_ir) * f_norm;

    // Hue->RGB (schnell, ohne HSV-Helper): Näherung via sektorbasierte Mischung
    // Wir nutzen eine einfache Kurve: Rot->Magenta->Violett
    float rC = 1.0f - f_norm*0.2f; // Rot sinkt leicht
    float bC = 0.6f + 0.4f*f_norm; // Blau steigt
    float gC = 0.3f + 0.2f*(1.0f - fabsf(0.5f - f_norm)*2.0f); // leichtes Grünplateau

    float sat = base_sat - 0.10f * (0.5f * (cosf(2.0f * M_PI * (0.9f * t_sec + 0.07f * idx)) + 1.0f));
    float val = base_val - 0.08f * (0.5f * (sinf(2.0f * M_PI * (1.3f * t_sec + 0.05f * idx)) + 1.0f));

    float Rf = fclamp(val * (rC * sat + (1.0f - sat)), 0.0f, 1.0f);
    float Gf = fclamp(val * (gC * sat + (1.0f - sat)), 0.0f, 1.0f);
    float Bf = fclamp(val * (bC * sat + (1.0f - sat)), 0.0f, 1.0f);

    uint8_t R = clamp_u8((int)(Rf * 255.0f));
    uint8_t G = clamp_u8((int)(Gf * 255.0f));
    uint8_t B = clamp_u8((int)(Bf * 255.0f));

    for (int y = ys; y < ye; y++) {
      int o = (y * WIDTH) * 3;
      for (int x = 0; x < WIDTH; x++) {
        int p = o + x * 3;
        out_rgb[p + 0] = R;
        out_rgb[p + 1] = G;
        out_rgb[p + 2] = B;
      }
    }
  }

  return estimate_visibility_score(res, edge, base_y_start, base_y_end);
}

// -----------------------------------------------------------------------------
// X-Ray-Style Overlay (rein visuell): kaltes Cyan->Weiß, kantengewichtet
// -----------------------------------------------------------------------------
static void overlay_xray_style(uint8_t *rgb_buf,
                               const uint8_t *res,
                               const uint8_t *edge,
                               uint32_t now_ms) {
  float t = (float)now_ms / 1000.0f;
  float glow = 0.08f * (0.5f * (sinf(2.0f * M_PI * (0.5f * t)) + 1.0f)); // 0..0.08

  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    int score = (int)(res[i]) + (int)((edge[i] * 2) / 3);
    if (score < 20) continue;
    if (score > 255) score = 255;

    float s = (float)score / 255.0f;

    // Cyan->Weiß Mapping, höherer Score → weißlicher, heller
    float val = fminf(1.0f, 0.85f + 0.15f * s + glow);
    float sat_drop = 0.5f * (1.0f - s);

    float r = val * (0.3f + 0.7f * s);
    float g = val * (1.0f - 0.2f * sat_drop);
    float b = val * (1.0f - 0.1f * sat_drop);

    uint8_t R = clamp_u8((int)(r * 255.0f));
    uint8_t G = clamp_u8((int)(g * 255.0f));
    uint8_t B = clamp_u8((int)(b * 255.0f));

    int o = i * 3;
    uint8_t A = (uint8_t)fclamp(OPT.heatmap_alpha, 0.0f, 255.0f);
    rgb_buf[o + 0] = (uint8_t)((rgb_buf[o + 0] * (255 - A) + R * A) / 255);
    rgb_buf[o + 1] = (uint8_t)((rgb_buf[o + 1] * (255 - A) + G * A) / 255);
    rgb_buf[o + 2] = (uint8_t)((rgb_buf[o + 2] * (255 - A) + B * A) / 255);
  }
}

// -----------------------------------------------------------------------------
// V4L2 Init / Cleanup
// -----------------------------------------------------------------------------
static int init_v4l2() {
  v4l2_fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
  if (v4l2_fd < 0) { perror("V4L2: open"); return -1; }

  struct v4l2_capability cap;
  if (ioctl(v4l2_fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("V4L2: VIDIOC_QUERYCAP"); return -1; }
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "Device does not support video capture\n"); return -1;
  }
  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    fprintf(stderr, "Device does not support streaming\n"); return -1;
  }

  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width       = WIDTH;
  fmt.fmt.pix.height      = HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field       = V4L2_FIELD_NONE;
  if (ioctl(v4l2_fd, VIDIOC_S_FMT, &fmt) < 0) { perror("V4L2: VIDIOC_S_FMT"); return -1; }

  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof(req));
  req.count  = 4;
  req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  if (ioctl(v4l2_fd, VIDIOC_REQBUFS, &req) < 0) { perror("V4L2: VIDIOC_REQBUFS"); return -1; }
  if (req.count < 2) { fprintf(stderr, "Insufficient buffer memory\n"); return -1; }

  buffers = calloc(req.count, sizeof(*buffers));
  for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = n_buffers;
    if (ioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("V4L2: VIDIOC_QUERYBUF"); return -1; }

    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start  = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);
    if (buffers[n_buffers].start == MAP_FAILED) { perror("V4L2: mmap"); return -1; }
  }

  for (unsigned int i = 0; i < n_buffers; i++) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = i;
    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) { perror("V4L2: VIDIOC_QBUF"); return -1; }
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(v4l2_fd, VIDIOC_STREAMON, &type) < 0) { perror("V4L2: VIDIOC_STREAMON"); return -1; }

  return 0;
}

static void stop_v4l2() {
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (v4l2_fd >= 0) {
    ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
  }
}

static void cleanup_v4l2() {
  if (buffers) {
    for (unsigned int i = 0; i < n_buffers; i++) {
      if (buffers[i].start) {
        munmap(buffers[i].start, buffers[i].length);
      }
    }
    free(buffers);
    buffers = NULL;
  }
  if (v4l2_fd >= 0) {
    close(v4l2_fd);
    v4l2_fd = -1;
  }
}

// -----------------------------------------------------------------------------
// SDL Init / Cleanup
// -----------------------------------------------------------------------------
static int init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return -1;
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  window = SDL_CreateWindow("MRT Viewer (X-Ray adaptive)",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    WIDTH, HEIGHT,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

  if (!window) {
    fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
    return -1;
  }

  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

  texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGB24,
    SDL_TEXTUREACCESS_STREAMING,
    WIDTH, HEIGHT);

  if (!texture) {
    fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
    return -1;
  }

  return 0;
}

static void cleanup_sdl() {
  if (texture) SDL_DestroyTexture(texture);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();
}

// -----------------------------------------------------------------------------
// Buffer Allocation
// -----------------------------------------------------------------------------
static int allocate_buffers() {
  size_t size_bytes = WIDTH * HEIGHT;
  gray    = (uint8_t*)malloc(size_bytes);
  bg      = (float*)calloc(size_bytes, sizeof(float));
  residue = (uint8_t*)malloc(size_bytes);
  edge    = (uint8_t*)malloc(size_bytes);
  flicker = (uint8_t*)calloc(size_bytes, sizeof(uint8_t));
  rgb     = (uint8_t*)malloc(WIDTH * HEIGHT * 3);

  if (!gray || !bg || !residue || !edge || !flicker || !rgb) {
    fprintf(stderr, "Speicherzuweisung für Puffer fehlgeschlagen.\n");
    return -1;
  }
  return 0;
}

static void free_buffers() {
  free(gray);
  free(bg);
  free(residue);
  free(edge);
  free(flicker);
  free(rgb);
}

// -----------------------------------------------------------------------------
// Main Loop
// -----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  if (init_sdl() != 0) {
    cleanup_sdl();
    return EXIT_FAILURE;
  }
  if (allocate_buffers() != 0) {
    free_buffers();
    cleanup_sdl();
    return EXIT_FAILURE;
  }
  if (init_v4l2() != 0) {
    stop_v4l2();
    cleanup_v4l2();
    free_buffers();
    cleanup_sdl();
    return EXIT_FAILURE;
  }

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(v4l2_fd, &fds);
    struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };

    int r = select(v4l2_fd + 1, &fds, NULL, NULL, &tv);
    if (r == -1) {
      if (errno == EINTR) continue;
      perror("select");
      running = false;
      continue;
    } else if (r == 0) {
      continue;
    }

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(v4l2_fd, VIDIOC_DQBUF, &buf) < 0) {
      if (errno == EAGAIN) continue;
      perror("VIDIOC_DQBUF");
      running = false;
      continue;
    }

    uint8_t *frame_data = (uint8_t *)buffers[buf.index].start;

    // Pipeline
    yuyv_to_gray_rgb(frame_data, gray, rgb);
    update_bg_and_residue(gray, bg, residue);
    sobel_edge(gray, edge);

    uint32_t now_ms = SDL_GetTicks();
    float vis_score = draw_falling_horizontal_bars_and_score(rgb, residue, edge, now_ms);
    update_cycle_duration(now_ms, vis_score); // hier: fest auf Hochfrequenz

    update_flicker(gray, flicker);

    // Metriken + kombinierte Selbstanpassung
    FrameMetrics fm = compute_metrics(residue, edge, flicker);
    OPT.ema_flicker = (1.0f - 0.12f)*OPT.ema_flicker + 0.12f*fm.flicker_mean;
    dynamic_autotune_combined(now_ms, fm, vis_score);

    // X-Ray Overlay
    overlay_xray_style(rgb, residue, edge, now_ms);

    SDL_UpdateTexture(texture, NULL, rgb, WIDTH * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) {
      perror("VIDIOC_QBUF");
      running = false;
    }
  }

  stop_v4l2();
  cleanup_v4l2();
  free_buffers();
  cleanup_sdl();

  return EXIT_SUCCESS;
}

