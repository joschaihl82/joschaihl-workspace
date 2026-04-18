// mrt.c
// MRT-Visualizer: UV Wischer (edge-only alpha, high contrast), face emphasis,
// white reward with strong blur, and automatic reward-quality check.
// Live Wischer HSV control via arrow keys (Shift modifies Value).
// Includes a check_reward_quality() that logs whether softened, blurred white reward
// regions meet target criteria (high luminance, low saturation, reduced local contrast).
//
// Compile with:
//   gcc -O2 mrt.c -o mrt -lSDL2 -lm
//
// Notes:
// - Arrow keys: LEFT/RIGHT adjust Wischer Hue; UP/DOWN adjust Wischer Saturation.
// - Hold Shift + LEFT/RIGHT to adjust Wischer Value (brightness).
// - Toggle face detector with 'T'.
// - ESC to quit.

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
// Configuration
// -----------------------------------------------------------------------------
#define WIDTH 640
#define HEIGHT 480
#define DEVICE "/dev/video0"

#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN   1.6f
#define HEATMAP_ALPHA 155

#define BAR_BASE_HEIGHT 20
#define BAR_HEIGHT_AMPL 24
#define BAR_SPACING 90

#define CYCLE_HIGH_MS 220.0f

// Wischer-Blur
#define WISCHER_BLUR_RADIUS 3
#define WISCHER_MAX_ALPHA 0.95f
#define WISCHER_INTENSITY 1.0f

// Reward blur radius (strong blur for white reward)
#define REWARD_BLUR_RADIUS 9

// Face detection (heuristic)
#define SKIN_MIN_CB 77
#define SKIN_MAX_CB 127
#define SKIN_MIN_CR 133
#define SKIN_MAX_CR 173
#define FACE_MIN_AREA (WIDTH*HEIGHT/200)
#define FACE_MAX_AREA (WIDTH*HEIGHT/2)

// Target visibility used by optimizer/reward condition
#define TGT_VIS 28.0f

// -----------------------------------------------------------------------------
// V4L2 / SDL / Buffers
// -----------------------------------------------------------------------------
struct buffer { void *start; size_t length; };
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static int v4l2_fd = -1;

static uint8_t *gray = NULL;
static float *bg = NULL;
static uint8_t *residue = NULL;
static uint8_t *edge = NULL;
static uint8_t *flicker = NULL;
static uint8_t *rgb = NULL;

// Wischer accumulators
static float *acc_r = NULL, *acc_g = NULL, *acc_b = NULL, *acc_a = NULL;
static float *tmp_r = NULL, *tmp_g = NULL, *tmp_b = NULL, *tmp_a = NULL;

// Reward accumulators (for white-blur reward)
static float *rew_r = NULL, *rew_g = NULL, *rew_b = NULL, *rew_a = NULL;
static float *rew_tmp = NULL;

// Face detection masks
static uint8_t *skin_mask = NULL;
static uint8_t *face_mask = NULL;

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
static inline void hsv_to_rgb_255(float h, float s, float v, uint8_t *R, uint8_t *G, uint8_t *B) {
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
// Optimizer / State (relevant fields)
// -----------------------------------------------------------------------------
typedef struct {
  float ema_edge_energy;
  float ema_res_density;
  float ema_vis;
  float ema_flicker;
  float edge_gain;
  float heatmap_alpha;
  float residue_thresh;
  // wischer HSV (live)
  float wischer_hue;
  float wischer_sat;
  float wischer_val;
  float wischer_step_h;
  float wischer_step_sv;
} OptState;

static OptState OPT = {
  .ema_edge_energy = 0.0f,
  .ema_res_density = 0.0f,
  .ema_vis = 0.0f,
  .ema_flicker = 0.0f,
  .edge_gain = EDGE_GAIN,
  .heatmap_alpha = HEATMAP_ALPHA,
  .residue_thresh = RESIDUE_THRESH,
  .wischer_hue = 0.78f, .wischer_sat = 0.88f, .wischer_val = 0.96f,
  .wischer_step_h = 0.005f, .wischer_step_sv = 0.03f
};

// Global variance metric used in reward condition
static float EMA_VAR_VIS = 0.0f;

// -----------------------------------------------------------------------------
// Frame metrics
// -----------------------------------------------------------------------------
typedef struct {
  float edge_energy;
  float res_density;
  float flicker_mean;
} FrameMetrics;

static FrameMetrics compute_metrics(const uint8_t *res, const uint8_t *edge_map, const uint8_t *flick) {
  const int N = WIDTH * HEIGHT;
  long edge_sum = 0, flick_sum = 0;
  int res_count = 0;
  for (int i = 0; i < N; i++) {
    edge_sum += edge_map[i];
    flick_sum += flick[i];
    if (res[i] > 0) res_count++;
  }
  FrameMetrics m;
  m.edge_energy = (float)edge_sum / (float)N;
  m.res_density = (float)res_count / (float)N;
  m.flicker_mean = (float)flick_sum / (float)N;
  return m;
}

// -----------------------------------------------------------------------------
// Face detector helpers (YCbCr skin thresholds, morph, largest CC)
// -----------------------------------------------------------------------------
static inline void yuyv_to_cbcr(const uint8_t *frame, int p, uint8_t *cb, uint8_t *cr) {
  *cb = frame[p + 1];
  *cr = frame[p + 3];
}

static void compute_skin_mask_from_yuyv(const uint8_t *frame, uint8_t *mask) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x += 2) {
      int p = (y*WIDTH + x) * 2;
      uint8_t cb = frame[p + 1];
      uint8_t cr = frame[p + 3];
      uint8_t val = (cb >= SKIN_MIN_CB && cb <= SKIN_MAX_CB && cr >= SKIN_MIN_CR && cr <= SKIN_MAX_CR) ? 255 : 0;
      mask[y*WIDTH + x] = val;
      mask[y*WIDTH + x + 1] = val;
    }
  }
}

static void morph_open(uint8_t *mask, uint8_t *tmp, int w, int h, int r) {
  if (r <= 0) return;
  // horizontal erosion -> tmp
  for (int y = 0; y < h; y++) {
    int row = y*w;
    for (int x = 0; x < w; x++) {
      int minv = 255;
      int left = x - r; if (left < 0) left = 0;
      int right = x + r; if (right >= w) right = w-1;
      for (int k = left; k <= right; k++) {
        int v = mask[row + k];
        if (v < minv) minv = v;
      }
      tmp[row + x] = (uint8_t)minv;
    }
  }
  // vertical erosion -> mask
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int minv = 255;
      int top = y - r; if (top < 0) top = 0;
      int bot = y + r; if (bot >= h) bot = h-1;
      for (int k = top; k <= bot; k++) {
        int v = tmp[k*w + x];
        if (v < minv) minv = v;
      }
      mask[y*w + x] = (uint8_t)minv;
    }
  }
  // horizontal dilation -> tmp
  for (int y = 0; y < h; y++) {
    int row = y*w;
    for (int x = 0; x < w; x++) {
      int maxv = 0;
      int left = x - r; if (left < 0) left = 0;
      int right = x + r; if (right >= w) right = w-1;
      for (int k = left; k <= right; k++) {
        int v = mask[row + k];
        if (v > maxv) maxv = v;
      }
      tmp[row + x] = (uint8_t)maxv;
    }
  }
  // vertical dilation -> mask
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int maxv = 0;
      int top = y - r; if (top < 0) top = 0;
      int bot = y + r; if (bot >= h) bot = h-1;
      for (int k = top; k <= bot; k++) {
        int v = tmp[k*w + x];
        if (v > maxv) maxv = v;
      }
      mask[y*w + x] = (uint8_t)maxv;
    }
  }
}

// two-pass connected components, returns largest component mask in out_mask
static int extract_largest_component(const uint8_t *bin, uint8_t *out_mask, int *label_map, int w, int h) {
  int N = w*h;
  for (int i = 0; i < N; i++) label_map[i] = 0;
  int next_label = 1;
  int *parent = (int*)malloc((N/2 + 4) * sizeof(int));
  if (!parent) return 0;
  parent[0] = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int i = y*w + x;
      if (bin[i] == 0) continue;
      int left = (x > 0) ? label_map[i-1] : 0;
      int up   = (y > 0) ? label_map[i-w] : 0;
      if (left == 0 && up == 0) {
        label_map[i] = next_label;
        parent[next_label] = next_label;
        next_label++;
      } else if (left != 0 && up == 0) {
        label_map[i] = left;
      } else if (left == 0 && up != 0) {
        label_map[i] = up;
      } else {
        int a = left, b = up;
        int root_a = a; while (parent[root_a] != root_a) root_a = parent[root_a];
        int root_b = b; while (parent[root_b] != root_b) root_b = parent[root_b];
        if (root_a == root_b) label_map[i] = root_a;
        else {
          int m = (root_a < root_b) ? root_a : root_b;
          int M = (root_a < root_b) ? root_b : root_a;
          parent[M] = m;
          label_map[i] = m;
        }
      }
    }
  }
  for (int l = 1; l < next_label; l++) {
    int r = l;
    while (parent[r] != r) r = parent[r];
    int s = l;
    while (parent[s] != s) {
      int t = parent[s];
      parent[s] = r;
      s = t;
    }
  }
  int max_label = next_label;
  int *counts = (int*)calloc(max_label, sizeof(int));
  if (!counts) { free(parent); return 0; }
  for (int i = 0; i < N; i++) {
    int L = label_map[i];
    if (L > 0) {
      int root = parent[L];
      counts[root]++;
    }
  }
  int best_label = 0, best_count = 0;
  for (int l = 1; l < max_label; l++) {
    int c = counts[l];
    if (c > best_count) { best_count = c; best_label = l; }
  }
  if (best_count < FACE_MIN_AREA || best_count > FACE_MAX_AREA) {
    memset(out_mask, 0, N);
    free(parent); free(counts);
    return 0;
  }
  for (int i = 0; i < N; i++) {
    int L = label_map[i];
    if (L > 0 && parent[L] == best_label) out_mask[i] = 255;
    else out_mask[i] = 0;
  }
  free(parent); free(counts);
  return best_count;
}

static void detect_face_region_from_yuyv(const uint8_t *frame_yuyv, uint8_t *out_face_mask, uint8_t *scratch) {
  compute_skin_mask_from_yuyv(frame_yuyv, skin_mask);
  morph_open(skin_mask, scratch, WIDTH, HEIGHT, 2);
  int *label_map = (int*)malloc(WIDTH * HEIGHT * sizeof(int));
  if (!label_map) { memset(out_face_mask, 0, WIDTH*HEIGHT); return; }
  extract_largest_component(skin_mask, out_face_mask, label_map, WIDTH, HEIGHT);
  free(label_map);
}

// -----------------------------------------------------------------------------
// Sobel edge
static void sobel_edge_compute(const uint8_t *g, uint8_t *e) {
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

// -----------------------------------------------------------------------------
// Separable blur for accumulators
static void separable_blur_acc(float *acc, float *tmp, int w, int h, int radius) {
  if (radius <= 0) return;
  int diameter = radius*2 + 1;
  // horizontal
  for (int y = 0; y < h; y++) {
    int row = y*w;
    float sum = 0.0f;
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
  // vertical
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

// -----------------------------------------------------------------------------
// Draw wischer into accumulators (UV-focused, green suppressed), alpha only at edges
// -----------------------------------------------------------------------------
static void draw_wischer_accumulators(uint32_t now_ms, const uint8_t *res, const uint8_t *edge_map, const uint8_t *frame_yuyv) {
  int N = WIDTH*HEIGHT;
  for (int i = 0; i < N; i++) { acc_r[i]=acc_g[i]=acc_b[i]=acc_a[i]=0.0f; }

  uint32_t ms_in_cycle = (uint32_t)fmodf((float)now_ms, CYCLE_HIGH_MS);
  float progress = (float)ms_in_cycle / CYCLE_HIGH_MS;
  const float width_period_ms = 900.0f;
  float wphase = fmodf((float)now_ms, width_period_ms) / width_period_ms;
  float wsin = 0.5f * (sinf(2.0f * M_PI * wphase) + 1.0f);
  int bar_height = BAR_BASE_HEIGHT + (int)(BAR_HEIGHT_AMPL * wsin);
  int base_y_start = (int)(progress * (HEIGHT + bar_height)) - bar_height;
  int max_rows = (HEIGHT / BAR_SPACING) + 2;
  const float t_sec = (float)now_ms / 1000.0f;

  float base_hue = OPT.wischer_hue;
  float hue_spread = 0.03f;

  for (int r = -max_rows; r <= max_rows; r++) {
    int y_start = base_y_start + r * BAR_SPACING;
    int y_end = y_start + bar_height;
    int ys = (y_start < 0) ? 0 : y_start;
    int ye = (y_end > HEIGHT) ? HEIGHT : y_end;
    if (ys >= ye) continue;

    int idx = r + max_rows;
    float fc = 18.0f + 2.5f * (float)idx;
    float fm = 1.1f + 0.15f * (float)idx;
    float phi = 0.33f * (float)idx;
    float fm_phase = sinf(2.0f * M_PI * (fm * t_sec + phi));
    float freq_hz = fc + 7.0f * fm_phase;
    float f_min = fc - 7.0f, f_max = fc + 7.0f;
    float f_norm = (freq_hz - f_min) / (f_max - f_min);
    if (f_norm < 0.0f) f_norm = 0.0f;
    if (f_norm > 1.0f) f_norm = 1.0f;

    float hue = base_hue + (f_norm - 0.5f) * hue_spread;
    hue += 0.004f * sinf(2.0f * M_PI * (0.9f * t_sec + 0.13f * idx));

    float sat = OPT.wischer_sat - 0.12f * f_norm;
    float val = OPT.wischer_val + 0.04f * f_norm;

    uint8_t HR, HG, HB;
    hsv_to_rgb_255(hue, fclamp(sat, 0.3f, 1.0f), fclamp(val, 0.85f, 1.0f), &HR, &HG, &HB);

    // suppress green strongly, boost blue slightly, damp red a bit
    float g_scale = 0.18f;
    float b_boost  = 1.12f;
    float r_boost  = 0.90f;
    uint8_t HR2 = clamp_u8((int)fclamp((float)HR * r_boost, 0.0f, 255.0f));
    uint8_t HG2 = clamp_u8((int)fclamp((float)HG * g_scale, 0.0f, 255.0f));
    uint8_t HB2 = clamp_u8((int)fclamp((float)HB * b_boost, 0.0f, 255.0f));

    for (int y = ys; y < ye; y++) {
      int row = y * WIDTH;
      for (int x = 0; x < WIDTH; x++) {
        int i = row + x;
        // activity proxy
        float activity = ((float)res[i] * 0.6f + (float)edge_map[i] * 0.4f) / 255.0f;
        // edge-only alpha: use normalized edge strength as primary alpha driver
        float edge_norm = (float)edge_map[i] / 255.0f;
        // distance falloff for bar shape
        float dy = 0.0f;
        if (y < y_start) dy = (float)(y_start - y);
        else if (y > y_end) dy = (float)(y - y_end);
        float falloff = 1.0f - fclamp(dy / (float)(bar_height + 1), 0.0f, 1.0f);
        // final alpha: edge-weighted, so alpha concentrated at edges
        float local_alpha = WISCHER_INTENSITY * WISCHER_MAX_ALPHA * edge_norm * activity * falloff;
        local_alpha = fclamp(local_alpha + 0.02f * falloff, 0.0f, 1.0f);

        // face boost: stronger alpha and slight whitening
        uint8_t rcol = HR2, gcol = HG2, bcol = HB2;
        if (face_mask[i]) {
          local_alpha = fclamp(local_alpha * 1.8f, 0.0f, 1.0f);
          rcol = clamp_u8((int)(rcol * 0.80f + 255 * 0.20f));
          gcol = clamp_u8((int)(gcol * 0.82f + 255 * 0.18f));
          bcol = clamp_u8((int)(bcol * 0.88f + 255 * 0.12f));
        }

        // increase contrast of wischer colors (stronger vividness)
        float contrast = 1.25f; // >1 increases contrast
        int r_con = (int)fclamp(128.0f + (rcol - 128.0f) * contrast, 0.0f, 255.0f);
        int g_con = (int)fclamp(128.0f + (gcol - 128.0f) * contrast, 0.0f, 255.0f);
        int b_con = (int)fclamp(128.0f + (bcol - 128.0f) * contrast, 0.0f, 255.0f);

        acc_r[i] += (float)r_con * local_alpha;
        acc_g[i] += (float)g_con * local_alpha;
        acc_b[i] += (float)b_con * local_alpha;
        acc_a[i] += local_alpha;
      }
    }
  }
}

// composite accumulators over rgb
static void composite_accumulators_over_rgb(uint8_t *rgb_buf) {
  int N = WIDTH*HEIGHT;
  for (int i = 0; i < N; i++) {
    float a = acc_a[i];
    if (a <= 1e-6f) continue;
    float r = acc_r[i] / (a + 1e-6f);
    float g = acc_g[i] / (a + 1e-6f);
    float b = acc_b[i] / (a + 1e-6f);
    int Ri = (int)fclamp(r, 0.0f, 255.0f);
    int Gi = (int)fclamp(g, 0.0f, 255.0f);
    int Bi = (int)fclamp(b, 0.0f, 255.0f);
    float alpha = fclamp(a * 0.6f, 0.0f, 1.0f);
    int o = i*3;
    rgb_buf[o+0] = (uint8_t)((rgb_buf[o+0] * (1.0f - alpha)) + (Ri * alpha));
    rgb_buf[o+1] = (uint8_t)((rgb_buf[o+1] * (1.0f - alpha)) + (Gi * alpha));
    rgb_buf[o+2] = (uint8_t)((rgb_buf[o+2] * (1.0f - alpha)) + (Bi * alpha));
  }
}

// -----------------------------------------------------------------------------
// Reward: detect bright/whiteish areas and apply low-contrast, strong blur reward
// -----------------------------------------------------------------------------
static void prepare_reward_accumulators(const uint8_t *res, const uint8_t *edge_map) {
  int N = WIDTH*HEIGHT;
  // clear reward accumulators
  for (int i = 0; i < N; i++) { rew_r[i]=rew_g[i]=rew_b[i]=rew_a[i]=0.0f; }

  // Build reward map: high combined score -> stronger reward
  for (int i = 0; i < N; i++) {
    int score = (int)res[i] + (int)((edge_map[i] * 2) / 3);
    if (score < 160) continue; // only high scores
    float s_norm = (float)score / 255.0f;
    // reward color: near white-blue (HSV around 0.60, low sat)
    float hue = 0.60f;
    float sat = 0.30f; // low sat -> whitish
    float val = 1.00f;
    uint8_t RR, RG, RB;
    hsv_to_rgb_255(hue, sat, val, &RR, &RG, &RB);
    // stronger reward for higher scores
    float alpha = fclamp((s_norm - 0.62f) * 1.8f, 0.0f, 1.0f); // starts around score ~160
    // accumulate
    rew_r[i] += (float)RR * alpha;
    rew_g[i] += (float)RG * alpha;
    rew_b[i] += (float)RB * alpha;
    rew_a[i] += alpha;
  }

  // Strong blur to make reward soft and glowing
  separable_blur_acc(rew_r, rew_tmp, WIDTH, HEIGHT, REWARD_BLUR_RADIUS);
  separable_blur_acc(rew_g, rew_tmp, WIDTH, HEIGHT, REWARD_BLUR_RADIUS);
  separable_blur_acc(rew_b, rew_tmp, WIDTH, HEIGHT, REWARD_BLUR_RADIUS);
  separable_blur_acc(rew_a, rew_tmp, WIDTH, HEIGHT, REWARD_BLUR_RADIUS);
}

// Composite reward over rgb with reduced contrast (less contrast where reward applies)
static void composite_reward_over_rgb(uint8_t *rgb_buf) {
  int N = WIDTH*HEIGHT;
  for (int i = 0; i < N; i++) {
    float a = rew_a[i];
    if (a <= 1e-6f) continue;
    float r = rew_r[i] / (a + 1e-6f);
    float g = rew_g[i] / (a + 1e-6f);
    float b = rew_b[i] / (a + 1e-6f);
    int Ri = (int)fclamp(r, 0.0f, 255.0f);
    int Gi = (int)fclamp(g, 0.0f, 255.0f);
    int Bi = (int)fclamp(b, 0.0f, 255.0f);
    float alpha = fclamp(a * 0.7f, 0.0f, 1.0f); // fairly strong
    int o = i*3;
    // reduce contrast of underlying pixel before blending: move towards mid-gray
    float baseR = (float)rgb_buf[o+0];
    float baseG = (float)rgb_buf[o+1];
    float baseB = (float)rgb_buf[o+2];
    float contrast_reduce = 0.55f; // 0..1, lower -> less contrast
    float mid = 128.0f;
    float lowR = mid + (baseR - mid) * contrast_reduce;
    float lowG = mid + (baseG - mid) * contrast_reduce;
    float lowB = mid + (baseB - mid) * contrast_reduce;
    // blend reward color over low-contrast base
    rgb_buf[o+0] = (uint8_t)fclamp((lowR * (1.0f - alpha) + Ri * alpha), 0.0f, 255.0f);
    rgb_buf[o+1] = (uint8_t)fclamp((lowG * (1.0f - alpha) + Gi * alpha), 0.0f, 255.0f);
    rgb_buf[o+2] = (uint8_t)fclamp((lowB * (1.0f - alpha) + Bi * alpha), 0.0f, 255.0f);
  }
}

// -----------------------------------------------------------------------------
// Reward quality checks (automated metrics)
// -----------------------------------------------------------------------------

// RGB -> Luminance (0..255)
static inline float rgb_luma(uint8_t r, uint8_t g, uint8_t b) {
  return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

// local 3x3 stddev of luminance
static float local_stddev_luma_buf(const uint8_t *rgb_buf, int idx) {
  int x = idx % WIDTH;
  int y = idx / WIDTH;
  float vals[9]; int cnt = 0;
  for (int yy = y-1; yy <= y+1; ++yy) {
    if (yy < 0 || yy >= HEIGHT) continue;
    for (int xx = x-1; xx <= x+1; ++xx) {
      if (xx < 0 || xx >= WIDTH) continue;
      int j = (yy * WIDTH + xx) * 3;
      vals[cnt++] = rgb_luma(rgb_buf[j], rgb_buf[j+1], rgb_buf[j+2]);
    }
  }
  if (cnt == 0) return 0.0f;
  float mean = 0.0f;
  for (int i=0;i<cnt;i++) mean += vals[i];
  mean /= (float)cnt;
  float var = 0.0f;
  for (int i=0;i<cnt;i++) { float d = vals[i]-mean; var += d*d; }
  var /= (float)cnt;
  return sqrtf(var);
}

// Check reward quality: returns 1 if OK, 0 otherwise; logs metrics
static int check_reward_quality(const uint8_t *rgb_buf,
                                const float *rew_r, const float *rew_g, const float *rew_b, const float *rew_a) {
  int N = WIDTH * HEIGHT;
  double sum_luma = 0.0, sum_sat = 0.0;
  double sum_std_before = 0.0, sum_std_after = 0.0;
  int count = 0;

  // create a temporary blurred copy of rgb for "after" metrics (simple box blur 5x5)
  uint8_t *tmp = (uint8_t*)malloc(N*3);
  if (!tmp) return 0;
  for (int y=0;y<HEIGHT;y++) {
    for (int x=0;x<WIDTH;x++) {
      int rsum=0, gsum=0, bsum=0, cnt=0;
      for (int yy=y-2; yy<=y+2; yy++) {
        if (yy<0||yy>=HEIGHT) continue;
        for (int xx=x-2; xx<=x+2; xx++) {
          if (xx<0||xx>=WIDTH) continue;
          int j=(yy*WIDTH+xx)*3;
          rsum += rgb_buf[j]; gsum += rgb_buf[j+1]; bsum += rgb_buf[j+2];
          cnt++;
        }
      }
      int o=(y*WIDTH+x)*3;
      tmp[o+0] = clamp_u8(rsum/cnt);
      tmp[o+1] = clamp_u8(gsum/cnt);
      tmp[o+2] = clamp_u8(bsum/cnt);
    }
  }

  for (int i=0;i<N;i++) {
    if (rew_a[i] <= 1e-6f) continue;
    int o = i*3;
    float R = (float)rgb_buf[o+0]/255.0f;
    float G = (float)rgb_buf[o+1]/255.0f;
    float B = (float)rgb_buf[o+2]/255.0f;
    float mx = fmaxf(R, fmaxf(G,B));
    float mn = fminf(R, fminf(G,B));
    float sat = (mx <= 0.0f) ? 0.0f : (mx - mn) / mx;
    float luma = rgb_luma(rgb_buf[o+0], rgb_buf[o+1], rgb_buf[o+2]);

    sum_luma += luma;
    sum_sat += sat;
    float std_before = local_stddev_luma_buf(rgb_buf, i);
    float std_after = local_stddev_luma_buf(tmp, i);
    sum_std_before += std_before;
    sum_std_after += std_after;
    count++;
  }

  free(tmp);
  if (count == 0) {
    printf("[reward-check] no reward pixels found\n");
    return 0;
  }

  double mean_luma = sum_luma / (double)count;
  double mean_sat  = sum_sat  / (double)count;
  double mean_std_before = sum_std_before / (double)count;
  double mean_std_after  = sum_std_after / (double)count;

  int ok = 1;
  if (mean_luma < 220.0) ok = 0;            // not bright enough
  if (mean_sat  > 0.35)  ok = 0;            // too saturated (not white)
  if (mean_std_after > mean_std_before * 0.7) ok = 0; // blur didn't reduce contrast enough

  printf("[reward-check] count=%d mean_luma=%.1f mean_sat=%.3f std_before=%.2f std_after=%.2f => %s\n",
         count, mean_luma, mean_sat, mean_std_before, mean_std_after, ok ? "OK" : "FAIL");

  return ok;
}

// -----------------------------------------------------------------------------
// Wrapper: render wischer, then reward, and run check
static void render_wischer_and_reward(uint32_t now_ms, const uint8_t *res, const uint8_t *edge_map, const uint8_t *frame_yuyv) {
  draw_wischer_accumulators(now_ms, res, edge_map, frame_yuyv);
  separable_blur_acc(acc_r, tmp_r, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_g, tmp_g, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_b, tmp_b, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_a, tmp_a, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  composite_accumulators_over_rgb(rgb);

  // prepare and composite reward (white-ish, low contrast, strong blur)
  prepare_reward_accumulators(res, edge_map);

  // run quality check (logs to stdout)
  check_reward_quality(rgb, rew_r, rew_g, rew_b, rew_a);

  composite_reward_over_rgb(rgb);
}

// -----------------------------------------------------------------------------
// X-Ray overlay (keeps face emphasis)
// -----------------------------------------------------------------------------
static void overlay_xray_style(uint8_t *rgb_buf, const uint8_t *res, const uint8_t *edge_map, uint32_t now_ms) {
  float t = (float)now_ms / 1000.0f;
  float glow = 0.08f * (0.5f * (sinf(2.0f * M_PI * (0.5f * t)) + 1.0f));
  bool reward = (OPT.ema_vis >= (TGT_VIS - 1.0f)) && (EMA_VAR_VIS <= 20.0f);
  uint8_t A = (uint8_t)fclamp(OPT.heatmap_alpha, 0.0f, 255.0f);

  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    int score = (int)res[i] + (int)((edge_map[i] * 2) / 3);
    if (score < 20) continue;
    if (score > 255) score = 255;
    float s = (float)score / 255.0f;
    float val = fminf(1.0f, 0.85f + 0.15f * s + glow);
    float sat_drop = 0.5f * (1.0f - s);
    float r = val * (0.3f + 0.7f * s);
    float g = val * (1.0f - 0.2f * sat_drop);
    float b = val * (1.0f - 0.1f * sat_drop);
    uint8_t XR = clamp_u8((int)(r * 255.0f));
    uint8_t XG = clamp_u8((int)(g * 255.0f));
    uint8_t XB = clamp_u8((int)(b * 255.0f));
    int o = i*3;
    uint8_t Rb = (uint8_t)((rgb_buf[o+0] * (255 - A) + XR * A) / 255);
    uint8_t Gb = (uint8_t)((rgb_buf[o+1] * (255 - A) + XG * A) / 255);
    uint8_t Bb = (uint8_t)((rgb_buf[o+2] * (255 - A) + XB * A) / 255);

    if (reward && score >= 160) {
      // reward color via OPT.wischer as fallback
      uint8_t RR, RG, RBc;
      hsv_to_rgb_255(OPT.wischer_hue, 0.30f, 1.00f, &RR, &RG, &RBc);
      if (score >= 220) {
        RR = clamp_u8((int)(RR * 0.88f + 255 * 0.12f));
        RG = clamp_u8((int)(RG * 0.90f + 255 * 0.10f));
        RBc = clamp_u8((int)(RBc * 0.94f + 255 * 0.06f));
      }
      float s_norm = (float)score / 255.0f;
      uint8_t reward_alpha = (uint8_t)fclamp(OPT.heatmap_alpha * (0.65f + 0.35f * s_norm), 60.0f, 230.0f);
      float mid = 128.0f;
      float contrast_reduce = 0.45f;
      float lowR = mid + ((float)Rb - mid) * contrast_reduce;
      float lowG = mid + ((float)Gb - mid) * contrast_reduce;
      float lowB = mid + ((float)Bb - mid) * contrast_reduce;
      uint8_t Rr = (uint8_t)fclamp((lowR * (255 - reward_alpha) + (float)RR * reward_alpha) / 255.0f, 0.0f, 255.0f);
      uint8_t Gr = (uint8_t)fclamp((lowG * (255 - reward_alpha) + (float)RG * reward_alpha) / 255.0f, 0.0f, 255.0f);
      uint8_t Br = (uint8_t)fclamp((lowB * (255 - reward_alpha) + (float)RBc * reward_alpha) / 255.0f, 0.0f, 255.0f);

      // small neighborhood blur mix
      int sx = i % WIDTH;
      int sy = i / WIDTH;
      int rsum = 0, gsum = 0, bsum = 0, cnt = 0;
      for (int yy = sy - 1; yy <= sy + 1; ++yy) {
        if (yy < 0 || yy >= HEIGHT) continue;
        for (int xx = sx - 1; xx <= sx + 1; ++xx) {
          if (xx < 0 || xx >= WIDTH) continue;
          int jj = (yy * WIDTH + xx) * 3;
          rsum += rgb[jj + 0];
          gsum += rgb[jj + 1];
          bsum += rgb[jj + 2];
          ++cnt;
        }
      }
      if (cnt > 0) {
        float navgR = (float)rsum / (float)cnt;
        float navgG = (float)gsum / (float)cnt;
        float navgB = (float)bsum / (float)cnt;
        float blur_mix = 0.22f * fclamp(s_norm, 0.0f, 1.0f);
        Rr = clamp_u8((int)fclamp((1.0f - blur_mix) * Rr + blur_mix * navgR, 0.0f, 255.0f));
        Gr = clamp_u8((int)fclamp((1.0f - blur_mix) * Gr + blur_mix * navgG, 0.0f, 255.0f));
        Br = clamp_u8((int)fclamp((1.0f - blur_mix) * Br + blur_mix * navgB, 0.0f, 255.0f));
      }

      if (face_mask && face_mask[i]) {
        Rr = clamp_u8((int)(Rr * 0.92f + 255 * 0.08f));
        Gr = clamp_u8((int)(Gr * 0.94f + 255 * 0.06f));
        Br = clamp_u8((int)(Br * 0.96f + 255 * 0.04f));
      }

      rgb_buf[o + 0] = Rr;
      rgb_buf[o + 1] = Gr;
      rgb_buf[o + 2] = Br;
    } else {
      rgb_buf[o + 0] = Rb;
      rgb_buf[o + 1] = Gb;
      rgb_buf[o + 2] = Bb;
    }
  }
}

// -----------------------------------------------------------------------------
// Simple HUD: small indicator and wischer color box
static void draw_hud(SDL_Renderer *renderer, uint32_t now_ms, bool face_detector_on) {
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  if (face_detector_on) {
    SDL_SetRenderDrawColor(renderer, 200, 230, 255, 160);
    SDL_Rect rct = {8, 8, 18, 18};
    SDL_RenderFillRect(renderer, &rct);
  }
  uint8_t WR, WG, WB;
  hsv_to_rgb_255(OPT.wischer_hue, OPT.wischer_sat, OPT.wischer_val, &WR, &WG, &WB);
  SDL_Rect box = { 36, 8, 44, 18 };
  SDL_SetRenderDrawColor(renderer, WR, WG, WB, 220);
  SDL_RenderFillRect(renderer, &box);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// -----------------------------------------------------------------------------
// V4L2 / SDL init / cleanup
static int init_v4l2() {
  v4l2_fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
  if (v4l2_fd < 0) { perror("V4L2: open"); return -1; }
  struct v4l2_capability cap;
  if (ioctl(v4l2_fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); return -1; }
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) { fprintf(stderr,"No capture\n"); return -1; }
  if (!(cap.capabilities & V4L2_CAP_STREAMING)) { fprintf(stderr,"No streaming\n"); return -1; }
  struct v4l2_format fmt; memset(&fmt,0,sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = WIDTH; fmt.fmt.pix.height = HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  if (ioctl(v4l2_fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return -1; }
  struct v4l2_requestbuffers req; memset(&req,0,sizeof(req));
  req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
  if (ioctl(v4l2_fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return -1; }
  buffers = calloc(req.count, sizeof(*buffers));
  for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
    struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = n_buffers;
    if (ioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return -1; }
    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);
    if (buffers[n_buffers].start == MAP_FAILED) { perror("mmap"); return -1; }
  }
  for (unsigned int i = 0; i < n_buffers; i++) {
    struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return -1; }
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(v4l2_fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return -1; }
  return 0;
}
static void stop_v4l2() {
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (v4l2_fd >= 0) ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
}
static void cleanup_v4l2() {
  if (buffers) {
    for (unsigned int i = 0; i < n_buffers; i++) if (buffers[i].start) munmap(buffers[i].start, buffers[i].length);
    free(buffers); buffers = NULL;
  }
  if (v4l2_fd >= 0) close(v4l2_fd);
  v4l2_fd = -1;
}

static int init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) { fprintf(stderr,"SDL_Init: %s\n",SDL_GetError()); return -1; }
  window = SDL_CreateWindow("MRT Viewer - UV Wischer (edge-only alpha, white reward)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
  if (!window) { fprintf(stderr,"SDL_CreateWindow: %s\n",SDL_GetError()); return -1; }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) { fprintf(stderr,"SDL_CreateRenderer: %s\n",SDL_GetError()); return -1; }
  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (!texture) { fprintf(stderr,"SDL_CreateTexture: %s\n",SDL_GetError()); return -1; }
  return 0;
}
static void cleanup_sdl() {
  if (texture) SDL_DestroyTexture(texture);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();
}

// -----------------------------------------------------------------------------
// Buffer allocation
static int allocate_buffers() {
  size_t N = WIDTH * HEIGHT;
  gray = (uint8_t*)malloc(N);
  bg = (float*)calloc(N, sizeof(float));
  residue = (uint8_t*)malloc(N);
  edge = (uint8_t*)malloc(N);
  flicker = (uint8_t*)calloc(N, sizeof(uint8_t));
  rgb = (uint8_t*)malloc(N * 3);

  acc_r = (float*)calloc(N, sizeof(float));
  acc_g = (float*)calloc(N, sizeof(float));
  acc_b = (float*)calloc(N, sizeof(float));
  acc_a = (float*)calloc(N, sizeof(float));
  tmp_r = (float*)calloc(N, sizeof(float));
  tmp_g = (float*)calloc(N, sizeof(float));
  tmp_b = (float*)calloc(N, sizeof(float));
  tmp_a = (float*)calloc(N, sizeof(float));

  rew_r = (float*)calloc(N, sizeof(float));
  rew_g = (float*)calloc(N, sizeof(float));
  rew_b = (float*)calloc(N, sizeof(float));
  rew_a = (float*)calloc(N, sizeof(float));
  rew_tmp = (float*)calloc(N, sizeof(float));

  skin_mask = (uint8_t*)malloc(N);
  face_mask = (uint8_t*)malloc(N);

  if (!gray || !bg || !residue || !edge || !flicker || !rgb ||
      !acc_r || !acc_g || !acc_b || !acc_a ||
      !tmp_r || !tmp_g || !tmp_b || !tmp_a ||
      !rew_r || !rew_g || !rew_b || !rew_a || !rew_tmp ||
      !skin_mask || !face_mask) {
    fprintf(stderr,"Allocation failed\n");
    return -1;
  }
  memset(face_mask, 0, N);
  return 0;
}
static void free_buffers() {
  free(gray); free(bg); free(residue); free(edge); free(flicker); free(rgb);
  free(acc_r); free(acc_g); free(acc_b); free(acc_a);
  free(tmp_r); free(tmp_g); free(tmp_b); free(tmp_a);
  free(rew_r); free(rew_g); free(rew_b); free(rew_a); free(rew_tmp);
  free(skin_mask); free(face_mask);
}

// -----------------------------------------------------------------------------
// YUYV -> gray + RGB
static void yuyv_to_gray_rgb(const uint8_t *in, uint8_t *g, uint8_t *out_rgb) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x += 2) {
      int idx = (y*WIDTH + x);
      int p = (y*WIDTH + x) * 2;
      uint8_t Y0 = in[p + 0];
      uint8_t U  = in[p + 1];
      uint8_t Y1 = in[p + 2];
      uint8_t V  = in[p + 3];
      g[idx] = Y0; g[idx+1] = Y1;
      int C0 = Y0 - 16, C1 = Y1 - 16;
      int D = U - 128, E = V - 128;
      int R0 = (298*C0 + 409*E + 128) >> 8;
      int G0 = (298*C0 - 100*D - 208*E + 128) >> 8;
      int B0 = (298*C0 + 516*D + 128) >> 8;
      int R1 = (298*C1 + 409*E + 128) >> 8;
      int G1 = (298*C1 - 100*D - 208*E + 128) >> 8;
      int B1 = (298*C1 + 516*D + 128) >> 8;
      int o = idx * 3;
      out_rgb[o+0] = clamp_u8(R0); out_rgb[o+1] = clamp_u8(G0); out_rgb[o+2] = clamp_u8(B0);
      out_rgb[o+3] = clamp_u8(R1); out_rgb[o+4] = clamp_u8(G1); out_rgb[o+5] = clamp_u8(B1);
    }
  }
}

// Background & residue
static void update_bg_and_residue(const uint8_t *g_in, float *bg_buf, uint8_t *res) {
  int N = WIDTH*HEIGHT;
  for (int i = 0; i < N; i++) {
    bg_buf[i] = (1.0f - BG_LEARN_RATE) * bg_buf[i] + BG_LEARN_RATE * (float)g_in[i];
    int diff = abs((int)g_in[i] - (int)bg_buf[i]);
    res[i] = (diff > RESIDUE_THRESH) ? (uint8_t)diff : 0;
  }
}

// flicker
static void update_flicker(const uint8_t *g, uint8_t *f) {
  static uint8_t prev = 0;
  uint64_t sum = 0;
  int N = WIDTH*HEIGHT;
  for (int i = 0; i < N; i++) sum += g[i];
  uint8_t avg = (uint8_t)(sum / N);
  int delta = abs((int)avg - (int)prev);
  prev = avg;
  uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
  for (int i = 0; i < N; i++) {
    int v = f[i] + boost;
    f[i] = (uint8_t)((v > 255) ? 255 : v);
    if (f[i] > 0) f[i] -= 1;
  }
}

// -----------------------------------------------------------------------------
// Main
int main(int argc, char *argv[]) {
  if (init_sdl() != 0) { cleanup_sdl(); return EXIT_FAILURE; }
  if (allocate_buffers() != 0) { free_buffers(); cleanup_sdl(); return EXIT_FAILURE; }
  if (init_v4l2() != 0) { cleanup_v4l2(); free_buffers(); cleanup_sdl(); return EXIT_FAILURE; }

  bool running = true;
  SDL_Event event;
  bool face_detector_enabled = true;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;
      if (event.type == SDL_KEYDOWN) {
        SDL_Keycode k = event.key.keysym.sym;
        if (k == SDLK_ESCAPE) { running = false; break; }
        if (k == SDLK_t) { face_detector_enabled = !face_detector_enabled; }

        // Wischer controls: arrow keys
        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        bool shift = keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT];

        if (k == SDLK_LEFT) {
          if (shift) OPT.wischer_val = fclamp(OPT.wischer_val - OPT.wischer_step_sv, 0.0f, 1.0f);
          else OPT.wischer_hue = fmodf(OPT.wischer_hue - OPT.wischer_step_h + 1.0f, 1.0f);
        } else if (k == SDLK_RIGHT) {
          if (shift) OPT.wischer_val = fclamp(OPT.wischer_val + OPT.wischer_step_sv, 0.0f, 1.0f);
          else OPT.wischer_hue = fmodf(OPT.wischer_hue + OPT.wischer_step_h, 1.0f);
        } else if (k == SDLK_UP) {
          OPT.wischer_sat = fclamp(OPT.wischer_sat + OPT.wischer_step_sv, 0.0f, 1.0f);
        } else if (k == SDLK_DOWN) {
          OPT.wischer_sat = fclamp(OPT.wischer_sat - OPT.wischer_step_sv, 0.0f, 1.0f);
        }

        if (k == SDLK_1) { OPT.wischer_step_h = 0.0025f; OPT.wischer_step_sv = 0.015f; }
        if (k == SDLK_2) { OPT.wischer_step_h = 0.005f; OPT.wischer_step_sv = 0.03f; }
        if (k == SDLK_3) { OPT.wischer_step_h = 0.01f; OPT.wischer_step_sv = 0.06f; }
      }
    }

    fd_set fds; FD_ZERO(&fds); FD_SET(v4l2_fd, &fds);
    struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };
    int r = select(v4l2_fd + 1, &fds, NULL, NULL, &tv);
    if (r <= 0) continue;

    struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(v4l2_fd, VIDIOC_DQBUF, &buf) < 0) continue;
    uint8_t *frame = (uint8_t*)buffers[buf.index].start;

    // pipeline
    yuyv_to_gray_rgb(frame, gray, rgb);
    update_bg_and_residue(gray, bg, residue);
    sobel_edge_compute(gray, edge);

    if (face_detector_enabled) {
      detect_face_region_from_yuyv(frame, face_mask, (uint8_t*)tmp_a);
    } else {
      memset(face_mask, 0, WIDTH*HEIGHT);
    }

    update_flicker(gray, flicker);

    // render wischer + reward (includes reward-quality check)
    render_wischer_and_reward(SDL_GetTicks(), residue, edge, frame);

    // overlay X-Ray (keeps face emphasis)
    overlay_xray_style(rgb, residue, edge, SDL_GetTicks());

    // present
    SDL_UpdateTexture(texture, NULL, rgb, WIDTH * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    draw_hud(renderer, SDL_GetTicks(), face_detector_enabled);

    SDL_RenderPresent(renderer);

    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) { perror("QBUF"); running = false; }
  }

  stop_v4l2();
  cleanup_v4l2();
  free_buffers();
  cleanup_sdl();
  return EXIT_SUCCESS;
}

