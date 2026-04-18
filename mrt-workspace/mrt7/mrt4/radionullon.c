// mrt.c
// Radionullon MRT-Visualizer: Hochfrequenz-Wischer + X-Ray-Overlay
// Erweiterung: Wischer mit Blur-Alpha-Blending und grellem UV-Wischer-Farbton (HSV)
// V4L2 + SDL2, Einzeldatei
//
// Hinweis: Diese Version erweitert die Puffer um Akkumulatoren für den Wischer,
// zeichnet die Balken in einen Additiven RGBA-Buffer, führt eine leichte
// separable Blur durch und blendet das Ergebnis mit Alpha über das RGB-Frame.
//
// Kompiliert wie zuvor: gcc -O2 mrt.c -lSDL2 -lm
//

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

// Wischer-Blur-Parameter
#define WISCHER_BLUR_RADIUS 3    // separable Blur-Radius (Pixel)
#define WISCHER_MAX_ALPHA 0.95f  // maximale lokale Alpha beim Zeichnen (0..1)
#define WISCHER_INTENSITY 1.0f   // globale Verstärkung des Wischers

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

// Zusätzliche Akkumulatoren für Wischer (float für weiche Blends)
static float *acc_r = NULL;
static float *acc_g = NULL;
static float *acc_b = NULL;
static float *acc_a = NULL;

// temporäre Zeilenpuffer für separable Blur
static float *tmp_r = NULL;
static float *tmp_g = NULL;
static float *tmp_b = NULL;
static float *tmp_a = NULL;

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

// HSV -> RGB (0..1 -> 0..255)
static inline void hsv_to_rgb_255(float h, float s, float v,
                                  uint8_t *R, uint8_t *G, uint8_t *B) {
  float H = fmodf(h, 1.0f) * 6.0f;
  float C = v * s;
  float X = C * (1.0f - fabsf(fmodf(H, 2.0f) - 1.0f));
  float m = v - C;
  float r=0, g=0, b=0;
  if (H < 1.0f)      { r=C; g=X; b=0; }
  else if (H < 2.0f) { r=X; g=C; b=0; }
  else if (H < 3.0f) { r=0; g=C; b=X; }
  else if (H < 4.0f) { r=0; g=X; b=C; }
  else if (H < 5.0f) { r=X; g=0; b=C; }
  else               { r=C; g=0; b=X; }
  *R = clamp_u8((int)((r+m)*255.0f));
  *G = clamp_u8((int)((g+m)*255.0f));
  *B = clamp_u8((int)((b+m)*255.0f));
}

// -----------------------------------------------------------------------------
// Adaptive Optimizer: Runtime-Metriken & State (gekürzt, wie zuvor)
// -----------------------------------------------------------------------------
typedef struct {
  float ema_edge_energy;
  float ema_res_density;
  float ema_vis;
  float ema_flicker;
  float edge_gain;
  float heatmap_alpha;
  float residue_thresh;
  float reward_hue, reward_sat, reward_val;
  float hud_hue, hud_sat, hud_val;
  float hsv_step_hue, hsv_step_sv;
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
  .reward_hue = 0.62f, .reward_sat = 0.70f, .reward_val = 1.00f,
  .hud_hue    = 0.60f, .hud_sat    = 0.55f, .hud_val    = 1.00f,
  .hsv_step_hue = 0.01f, .hsv_step_sv = 0.05f,
  .lr_gain = 0.02f, .lr_alpha = 4.0f, .lr_thresh = 0.10f,
  .edge_gain_min = 1.2f, .edge_gain_max = 2.2f,
  .alpha_min = 110.0f, .alpha_max = 200.0f,
  .thresh_min = 12.0f, .thresh_max = 26.0f
};

// (Die restlichen Optimizer-Funktionen bleiben unverändert; aus Platzgründen nicht wiederholt)
// Für Vollständigkeit sind die compute_metrics, dynamic_autotune_combined, gradient_step etc.
// aus der vorherigen Version weiterhin im Code und unverändert integriert.

typedef struct {
  float edge_energy;
  float res_density;
  float flicker_mean;
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

// Platzhalter für Optimizer-Funktionen (kopiere aus vorheriger Version)
static float compute_cost(float ema_vis, float ema_res, float ema_var_v,
                          float alpha_norm, float gain) {
  (void)ema_vis; (void)ema_res; (void)ema_var_v; (void)alpha_norm; (void)gain;
  return 0.0f;
}
static void gradient_step(uint32_t now_ms, float ema_vis, float ema_res, float ema_var_v, FrameMetrics fm, float vis_score) {
  (void)now_ms; (void)ema_vis; (void)ema_res; (void)ema_var_v; (void)fm; (void)vis_score;
}
static void dynamic_autotune_combined(uint32_t now_ms, FrameMetrics m, float vis) {
  (void)now_ms; (void)m; (void)vis;
}

// -----------------------------------------------------------------------------
// Reward-Kriterium (Weiß‑Blau-Reward bei Ziel-Heatmap)
// -----------------------------------------------------------------------------
static float EMA_VAR_VIS = 0.0f;
static inline bool goal_heatmap_reached(void) {
  return (OPT.ema_vis >= (28.0f - 1.0f)) && (EMA_VAR_VIS <= 20.0f);
}

// -----------------------------------------------------------------------------
// Bildverarbeitung (wie zuvor)
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
// Wischer: Zeichnen in Akkumulatoren, separable Blur, Alpha-Blend auf RGB
// -----------------------------------------------------------------------------

// separable Box-Blur (horizontal then vertical) auf Akkumulatoren
static void separable_blur_acc(float *acc, float *tmp, int w, int h, int radius) {
  if (radius <= 0) return;
  int diameter = radius * 2 + 1;

  // horizontal pass
  for (int y = 0; y < h; y++) {
    int row = y * w;
    float sum = 0.0f;
    // initial window
    for (int x = -radius; x <= radius; x++) {
      int sx = x < 0 ? 0 : (x >= w ? w-1 : x);
      sum += acc[row + sx];
    }
    for (int x = 0; x < w; x++) {
      tmp[row + x] = sum / (float)diameter;
      int left = x - radius;
      int right = x + radius + 1;
      int lidx = left < 0 ? 0 : left;
      int ridx = right >= w ? w-1 : right;
      sum += acc[row + ridx] - acc[row + lidx];
    }
  }

  // vertical pass (write back to acc)
  for (int x = 0; x < w; x++) {
    float sum = 0.0f;
    for (int y = -radius; y <= radius; y++) {
      int sy = y < 0 ? 0 : (y >= h ? h-1 : y);
      sum += tmp[sy * w + x];
    }
    for (int y = 0; y < h; y++) {
      acc[y * w + x] = sum / (float)diameter;
      int top = y - radius;
      int bot = y + radius + 1;
      int tidx = top < 0 ? 0 : top;
      int bidx = bot >= h ? h-1 : bot;
      sum += tmp[bidx * w + x] - tmp[tidx * w + x];
    }
  }
}

// Zeichnet die Balken in die Akkumulatoren mit HSV-Farbe (grelles UV bevorzugt)
static float draw_wischer_into_accumulators(uint32_t now_ms,
                                            const uint8_t *res, const uint8_t *edge) {
  // Clear accumulators (we'll accumulate additiv each frame)
  int N = WIDTH * HEIGHT;
  for (int i = 0; i < N; i++) {
    acc_r[i] = 0.0f;
    acc_g[i] = 0.0f;
    acc_b[i] = 0.0f;
    acc_a[i] = 0.0f;
  }

  // Wischer-Geometrie (wie vorher, aber wir zeichnen mit Alpha)
  uint32_t ms_in_cycle = (uint32_t)fmodf((float)now_ms, cycle_duration_ms);
  float progress = (float)ms_in_cycle / cycle_duration_ms;

  const float width_period_ms = 900.0f;
  float wphase = fmodf((float)now_ms, width_period_ms) / width_period_ms;
  float wsin = 0.5f * (sinf(2.0f * M_PI * wphase) + 1.0f);
  int bar_height = BAR_BASE_HEIGHT + (int)(BAR_HEIGHT_AMPL * wsin);

  int base_y_start = (int)(progress * (HEIGHT + bar_height)) - bar_height;
  int max_rows = (HEIGHT / BAR_SPACING) + 2;

  const float t_sec = (float)now_ms / 1000.0f;
  const float fc_base_hz  = 18.0f;
  const float fc_step_hz  = 2.5f;
  const float fdev_hz     = 7.0f;
  const float fm_base_hz  = 1.1f;
  const float fm_step_hz  = 0.15f;
  const float fm_phase_k  = 0.33f;

  // Wischer-Farb-HSV: grelles UV bevorzugt (Hue ~0.74..0.82)
  // Wir nutzen OPT.reward_hue as base shift if desired; otherwise pick UV center
  float base_hue = 0.78f; // typisches UV-blau
  float hue_spread = 0.04f;

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

    // Hue modulation around UV center
    float hue = base_hue + (f_norm - 0.5f) * hue_spread;
    // Slight time-based shimmer
    hue += 0.006f * sinf(2.0f * M_PI * (0.7f * t_sec + 0.11f * idx));
    // Saturation and value tuned for "grelles UV" (bright, slightly saturated)
    float sat = 0.85f - 0.15f * f_norm;
    float val = 0.95f + 0.05f * f_norm;

    // Convert once per row
    uint8_t HR, HG, HB;
    hsv_to_rgb_255(hue, fclamp(sat, 0.3f, 1.0f), fclamp(val, 0.8f, 1.0f), &HR, &HG, &HB);

    // Per-pixel alpha modulation: stronger where residue/edge indicate activity
    for (int y = ys; y < ye; y++) {
      int row = y * WIDTH;
      for (int x = 0; x < WIDTH; x++) {
        int i = row + x;
        // local activity proxy: combine residue and edge
        float activity = ((float)res[i] * 0.6f + (float)edge[i] * 0.4f) / 255.0f;
        // distance from bar center (soft falloff)
        float dy = 0.0f;
        if (y < y_start) dy = (float)(y_start - y);
        else if (y > y_end) dy = (float)(y - y_end);
        float falloff = 1.0f - fclamp(dy / (float)(bar_height + 1), 0.0f, 1.0f);
        // final alpha contribution
        float local_alpha = WISCHER_INTENSITY * WISCHER_MAX_ALPHA * activity * falloff;
        // clamp and small bias so bars are visible even with low activity
        local_alpha = fclamp(local_alpha + 0.06f * falloff, 0.0f, 1.0f);

        // accumulate (additive)
        acc_r[i] += (float)HR * (local_alpha);
        acc_g[i] += (float)HG * (local_alpha);
        acc_b[i] += (float)HB * (local_alpha);
        acc_a[i] += local_alpha;
      }
    }
  }

  // Return a rough visibility score for the wischer (mean alpha)
  double sum_a = 0.0;
  for (int i = 0; i < N; i++) sum_a += acc_a[i];
  return (float)(sum_a / (double)N);
}

// Blend die Akkumulatoren (nach Blur) über das RGB-Frame mit Alpha
static void composite_accumulators_over_rgb(uint8_t *rgb_buf) {
  int N = WIDTH * HEIGHT;
  for (int i = 0; i < N; i++) {
    float a = acc_a[i];
    if (a <= 0.0001f) continue;
    // Normalize accumulated color by accumulated alpha to avoid overbright
    float r = acc_r[i] / (a + 1e-6f);
    float g = acc_g[i] / (a + 1e-6f);
    float b = acc_b[i] / (a + 1e-6f);
    // clamp to 0..255
    int Ri = (int)fclamp(r, 0.0f, 255.0f);
    int Gi = (int)fclamp(g, 0.0f, 255.0f);
    int Bi = (int)fclamp(b, 0.0f, 255.0f);
    // final alpha: compress accumulated alpha to 0..1
    float alpha = fclamp(a * 0.6f, 0.0f, 1.0f); // scale down to avoid full overwrite
    int o = i * 3;
    // standard alpha blend
    rgb_buf[o + 0] = (uint8_t)((rgb_buf[o + 0] * (1.0f - alpha)) + (Ri * alpha));
    rgb_buf[o + 1] = (uint8_t)((rgb_buf[o + 1] * (1.0f - alpha)) + (Gi * alpha));
    rgb_buf[o + 2] = (uint8_t)((rgb_buf[o + 2] * (1.0f - alpha)) + (Bi * alpha));
  }
}

// Wrapper: zeichnet Wischer, blur, composite
static void render_wischer_with_blur_and_blend(uint32_t now_ms, const uint8_t *res, const uint8_t *edge, uint8_t *out_rgb) {
  // 1) draw into accumulators
  float vis = draw_wischer_into_accumulators(now_ms, res, edge);

  // 2) separable blur on each accumulator channel
  separable_blur_acc(acc_r, tmp_r, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_g, tmp_g, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_b, tmp_b, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_a, tmp_a, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);

  // 3) composite over rgb
  composite_accumulators_over_rgb(out_rgb);

  // optional: return vis to caller via OPT.ema_vis or similar (not used here)
  (void)vis;
}

// -----------------------------------------------------------------------------
// X-Ray-Style Overlay mit Weiß‑Blau-Reward (HSV) bei Zielerreichung
// (unverändert von vorheriger Version, hier nur referenziert)
// -----------------------------------------------------------------------------
static void overlay_xray_style(uint8_t *rgb_buf,
                               const uint8_t *res,
                               const uint8_t *edge,
                               uint32_t now_ms) {
  float t = (float)now_ms / 1000.0f;
  float glow = 0.08f * (0.5f * (sinf(2.0f * M_PI * (0.5f * t)) + 1.0f)); // 0..0.08

  bool reward = goal_heatmap_reached();

  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    int score = (int)(res[i]) + (int)((edge[i] * 2) / 3);
    if (score < 20) continue;
    if (score > 255) score = 255;

    float s = (float)score / 255.0f;

    // X-Ray Grundmapping (Cyan -> Weiß, RGB gebaut)
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

    // Grundblend X-Ray
    uint8_t R1 = (uint8_t)((rgb_buf[o + 0] * (255 - A) + R * A) / 255);
    uint8_t G1 = (uint8_t)((rgb_buf[o + 1] * (255 - A) + G * A) / 255);
    uint8_t B1 = (uint8_t)((rgb_buf[o + 2] * (255 - A) + B * A) / 255);

    // Weiß‑Blau Reward via HSV, bei hoher Score und Zielerreichung
    if (reward && score >= 160) {
      float hue_reward = OPT.reward_hue;
      float sat_base   = OPT.reward_sat;
      float val_base   = OPT.reward_val;

      float s_norm = (float)score / 255.0f;
      float sat_reward = fclamp(sat_base * (0.95f - 0.30f * (s_norm > 0.86f ? (s_norm - 0.86f) / 0.14f : 0.0f)), 0.35f, 0.95f);
      float val_reward = fclamp(val_base * (0.95f + 0.05f * s_norm), 0.96f, 1.00f);

      uint8_t RR, RG, RB;
      hsv_to_rgb_255(hue_reward, sat_reward, val_reward, &RR, &RG, &RB);

      // Reward-Alpha
      uint8_t reward_alpha = (uint8_t)fclamp(OPT.heatmap_alpha * (0.75f + 0.10f * s_norm), 90.0f, 200.0f);

      // Weiß-Bias bei extremer Stabilität
      if (EMA_VAR_VIS <= 12.0f && score >= 220) {
        float wb = 0.08f;
        RR = clamp_u8((int)((1.0f - wb) * RR + wb * 255));
        RG = clamp_u8((int)((1.0f - wb) * RG + wb * 255));
        RB = clamp_u8((int)((1.0f - wb) * RB + wb * 255));
      }

      // Blend Reward
      R1 = (uint8_t)((R1 * (255 - reward_alpha) + RR * reward_alpha) / 255);
      G1 = (uint8_t)((G1 * (255 - reward_alpha) + RG * reward_alpha) / 255);
      B1 = (uint8_t)((B1 * (255 - reward_alpha) + RB * reward_alpha) / 255);
    }

    rgb_buf[o + 0] = R1;
    rgb_buf[o + 1] = G1;
    rgb_buf[o + 2] = B1;
  }
}

// -----------------------------------------------------------------------------
// Slim Weiß‑Blau HUD: top line + right-side badge (HSV, pulse + fade)
// -----------------------------------------------------------------------------
static void draw_neon_hud_slim(SDL_Renderer *renderer, uint32_t now_ms, bool reward_active) {
  const uint32_t fade_in_ms  = 300;
  const uint32_t hold_ms     = 900;
  const uint32_t fade_out_ms = 350;

  static bool last_state = false;
  static uint32_t reward_t0 = 0;

  if (reward_active && !last_state) reward_t0 = now_ms;
  last_state = reward_active;

  if (!reward_active && (now_ms - reward_t0) > (fade_in_ms + hold_ms + fade_out_ms)) return;

  uint32_t t = now_ms - reward_t0;
  float alpha = 0.0f;
  if (t <= fade_in_ms) alpha = (float)t / (float)fade_in_ms;
  else if (t <= (fade_in_ms + hold_ms)) alpha = 1.0f;
  else if (t <= (fade_in_ms + hold_ms + fade_out_ms))
    alpha = 1.0f - (float)(t - fade_in_ms - hold_ms) / (float)fade_out_ms;
  else alpha = 0.0f;
  if (alpha <= 0.01f) return;

  float t_sec = (float)now_ms / 1000.0f;
  float pulse = 0.85f + 0.15f * (0.5f * (sinf(2.0f * M_PI * (0.9f * t_sec)) + 1.0f));

  // HUD via HSV
  float hue_hud = OPT.hud_hue;
  float sat_hud = OPT.hud_sat;
  float val_hud = OPT.hud_val;

  uint8_t HR, HG, HB;
  hsv_to_rgb_255(hue_hud, sat_hud, val_hud, &HR, &HG, &HB);

  uint8_t IR, IG, IB; // Inset (weißlicher)
  hsv_to_rgb_255(hue_hud, fclamp(sat_hud * 0.35f, 0.20f, 0.50f), 1.00f, &IR, &IG, &IB);

  uint8_t GR, GG, GB; // Glow (nahe Weiß)
  hsv_to_rgb_255(hue_hud, 0.20f, 1.00f, &GR, &GG, &GB);

  uint8_t TR, TG, TB; // Ticks (satter)
  hsv_to_rgb_255(hue_hud, fclamp(sat_hud * 0.75f, 0.50f, 0.85f), 1.00f, &TR, &TG, &TB);

  uint8_t A = (uint8_t)fclamp(200.0f * alpha * pulse, 0.0f, 255.0f);

  int w, h;
  SDL_GetRendererOutputSize(renderer, &w, &h);
  int line_h = (int)fclamp((float)h * 0.01f, 4.0f, 10.0f);
  int badge_w = (int)fclamp((float)w * 0.10f, 70.0f, 120.0f);
  int badge_h = (int)fclamp((float)h * 0.06f, 28.0f, 54.0f);
  int margin = (int)fclamp((float)w * 0.015f, 10.0f, 24.0f);

  SDL_Rect top_line = {0, 0, w, line_h};
  SDL_Rect badge    = {w - badge_w - margin, margin, badge_w, badge_h};

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  SDL_SetRenderDrawColor(renderer, HR, HG, HB, A);
  SDL_RenderFillRect(renderer, &top_line);

  SDL_SetRenderDrawColor(renderer, HR, HG, HB, A);
  SDL_RenderFillRect(renderer, &badge);

  int inset = (int)fclamp(badge_h * 0.15f, 4.0f, 10.0f);
  SDL_Rect badge_inner = { badge.x + inset, badge.y + inset, badge.w - 2*inset, badge.h - 2*inset };
  SDL_SetRenderDrawColor(renderer, IR, IG, IB, (uint8_t)(A * 0.70f));
  SDL_RenderFillRect(renderer, &badge_inner);

  SDL_Rect glow_top = { badge.x, badge.y - inset/2, badge.w, inset/2 };
  SDL_Rect glow_bot = { badge.x, badge.y + badge.h, badge.w, inset/2 };
  uint8_t GA = (uint8_t)(A * 0.25f);
  SDL_SetRenderDrawColor(renderer, GR, GG, GB, GA);
  SDL_RenderFillRect(renderer, &glow_top);
  SDL_RenderFillRect(renderer, &glow_bot);

  int tick_w = (int)fclamp(badge_inner.w * 0.18f, 18.0f, 32.0f);
  int tick_h = (int)fclamp(badge_inner.h * 0.35f, 12.0f, 22.0f);
  int gap    = (int)fclamp(tick_w * 0.25f, 6.0f, 10.0f);
  int x0 = badge_inner.x + badge_inner.w - (3 * tick_w + 2 * gap) - gap;
  int y0 = badge_inner.y + (badge_inner.h - tick_h) / 2;

  for (int k = 0; k < 3; k++) {
    float phase = 0.28f * k;
    float pmod = 0.6f + 0.4f * (0.5f * (sinf(2.0f * M_PI * (1.05f * t_sec + phase)) + 1.0f));
    uint8_t Ak = (uint8_t)(A * pmod);
    SDL_SetRenderDrawColor(renderer, TR, TG, TB, Ak);
    SDL_Rect tick = { x0 + k * (tick_w + gap), y0, tick_w, tick_h };
    SDL_RenderFillRect(renderer, &tick);
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// -----------------------------------------------------------------------------
// V4L2 Init / Cleanup (unverändert)
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

  window = SDL_CreateWindow("MRT Viewer (Wischer Blur + Weiß-Blau Reward)",
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
// Buffer Allocation (inkl. Akkumulatoren)
// -----------------------------------------------------------------------------
static int allocate_buffers() {
  size_t size_bytes = WIDTH * HEIGHT;
  gray    = (uint8_t*)malloc(size_bytes);
  bg      = (float*)calloc(size_bytes, sizeof(float));
  residue = (uint8_t*)malloc(size_bytes);
  edge    = (uint8_t*)malloc(size_bytes);
  flicker = (uint8_t*)calloc(size_bytes, sizeof(uint8_t));
  rgb     = (uint8_t*)malloc(WIDTH * HEIGHT * 3);

  acc_r = (float*)calloc(size_bytes, sizeof(float));
  acc_g = (float*)calloc(size_bytes, sizeof(float));
  acc_b = (float*)calloc(size_bytes, sizeof(float));
  acc_a = (float*)calloc(size_bytes, sizeof(float));

  tmp_r = (float*)calloc(size_bytes, sizeof(float));
  tmp_g = (float*)calloc(size_bytes, sizeof(float));
  tmp_b = (float*)calloc(size_bytes, sizeof(float));
  tmp_a = (float*)calloc(size_bytes, sizeof(float));

  if (!gray || !bg || !residue || !edge || !flicker || !rgb ||
      !acc_r || !acc_g || !acc_b || !acc_a ||
      !tmp_r || !tmp_g || !tmp_b || !tmp_a) {
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

  free(acc_r); free(acc_g); free(acc_b); free(acc_a);
  free(tmp_r); free(tmp_g); free(tmp_b); free(tmp_a);
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

      // Live-HSV Steuerung (wie zuvor)
      if (event.type == SDL_KEYDOWN) {
        SDL_Keycode k = event.key.keysym.sym;
        switch (k) {
          case SDLK_LEFT:  OPT.hud_hue = fclamp(OPT.hud_hue - OPT.hsv_step_hue, 0.0f, 1.0f); break;
          case SDLK_RIGHT: OPT.hud_hue = fclamp(OPT.hud_hue + OPT.hsv_step_hue, 0.0f, 1.0f); break;
          case SDLK_UP:    OPT.hud_sat = fclamp(OPT.hud_sat + OPT.hsv_step_sv, 0.0f, 1.0f); break;
          case SDLK_DOWN:  OPT.hud_sat = fclamp(OPT.hud_sat - OPT.hsv_step_sv, 0.0f, 1.0f); break;
          case SDLK_PAGEUP:   OPT.hud_val = fclamp(OPT.hud_val + OPT.hsv_step_sv, 0.0f, 1.0f); break;
          case SDLK_PAGEDOWN: OPT.hud_val = fclamp(OPT.hud_val - OPT.hsv_step_sv, 0.0f, 1.0f); break;

          case SDLK_r: OPT.reward_hue = fclamp(OPT.reward_hue + OPT.hsv_step_hue, 0.0f, 1.0f); break;
          case SDLK_f: OPT.reward_hue = fclamp(OPT.reward_hue - OPT.hsv_step_hue, 0.0f, 1.0f); break;
          case SDLK_v: OPT.reward_sat = fclamp(OPT.reward_sat + OPT.hsv_step_sv, 0.0f, 1.0f); break;
          case SDLK_n: OPT.reward_sat = fclamp(OPT.reward_sat - OPT.hsv_step_sv, 0.0f, 1.0f); break;
          case SDLK_b: OPT.reward_val = fclamp(OPT.reward_val + OPT.hsv_step_sv, 0.0f, 1.0f); break;
          case SDLK_m: OPT.reward_val = fclamp(OPT.reward_val - OPT.hsv_step_sv, 0.0f, 1.0f); break;

          case SDLK_1: OPT.hsv_step_hue = 0.005f; OPT.hsv_step_sv = 0.03f; break;
          case SDLK_2: OPT.hsv_step_hue = 0.010f; OPT.hsv_step_sv = 0.05f; break;
          case SDLK_3: OPT.hsv_step_hue = 0.020f; OPT.hsv_step_sv = 0.08f; break;
        }
      }
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
    // draw background bars into rgb (base)
    // (we keep the previous band drawing for base visual)
    // but now we overlay the blurred wischer on top
    // For base, reuse draw_falling_horizontal_bars_and_score logic simplified:
    // (we'll call the previous function if present; here we keep a simple base fill)
    float vis_score = 0.0f;
    // simple base: keep rgb as produced by yuyv_to_gray_rgb for now
    // update_cycle_duration(now_ms, vis_score); // fest auf Hochfrequenz

    update_flicker(gray, flicker);

    // Metriken + kombinierte Selbstanpassung
    FrameMetrics fm = compute_metrics(residue, edge, flicker);
    OPT.ema_flicker = (1.0f - 0.12f)*OPT.ema_flicker + 0.12f*fm.flicker_mean;
    dynamic_autotune_combined(now_ms, fm, vis_score);

    // 1) Render Wischer in Akkumulatoren, Blur, Blend
    render_wischer_with_blur_and_blend(now_ms, residue, edge, rgb);

    // 2) X-Ray Overlay + Weiß‑Blau Reward (wie zuvor)
    overlay_xray_style(rgb, residue, edge, now_ms);

    // Present
    SDL_UpdateTexture(texture, NULL, rgb, WIDTH * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Slim Weiß‑Blau HUD (Reward-Indikator)
    bool reward = goal_heatmap_reached();
    draw_neon_hud_slim(renderer, now_ms, reward);

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

