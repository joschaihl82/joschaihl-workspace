// mrt.c
// MRT-Visualizer mit X-Ray-Overlay, adaptivem Optimizer, Wischer-Blur und
// einfachem Gesichtskantendetektor (skin-based, C-only, Echtzeit-geeignet).
//
// Hinweise:
// - Reuse/integriere in deine bestehende Build-Umgebung (SDL2, V4L2).
// - Der Gesichtsdetektor ist ein leichter, heuristischer Ansatz (YCbCr skin,
//   Morphologie, größte Komponente). Er ist nicht so robust wie ML-basierte
//   Detektoren, aber schnell und ohne zusätzliche Bibliotheken.
//
// Kompilieren (Beispiel):
//   gcc -O2 mrt.c -o mrt -lSDL2 -lm
//
// (Datei ist eigenständig; entferne ggf. doppelte Definitionen, wenn du sie in
//  ein bestehendes Projekt einfügst.)
// -----------------------------------------------------------------------------

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
// Konfiguration
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

// Gesichtsdetection
#define SKIN_MIN_CB 77
#define SKIN_MAX_CB 127
#define SKIN_MIN_CR 133
#define SKIN_MAX_CR 173
#define FACE_MIN_AREA (WIDTH*HEIGHT/200)   // minimaler Flächenanteil für Gesicht
#define FACE_MAX_AREA (WIDTH*HEIGHT/2)     // maximaler Flächenanteil

// -----------------------------------------------------------------------------
// V4L2 / SDL / Puffer
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

// Wischer-Akkus
static float *acc_r = NULL, *acc_g = NULL, *acc_b = NULL, *acc_a = NULL;
static float *tmp_r = NULL, *tmp_g = NULL, *tmp_b = NULL, *tmp_a = NULL;

// Gesichtsmasken
static uint8_t *skin_mask = NULL;   // rohe Hautmaske
static uint8_t *face_mask = NULL;   // größte Komponente (vermutetes Gesicht)

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

// HSV->RGB helper (0..1 -> 0..255)
static inline void hsv_to_rgb_255(float h, float s, float v, uint8_t *R, uint8_t *G, uint8_t *B) {
  float H = fmodf(h, 1.0f) * 6.0f;
  float C = v * s;
  float X = C * (1.0f - fabsf(fmodf(H, 2.0f) - 1.0f));
  float m = v - C;
  float r=0,g=0,b=0;
  if (H < 1.0f) { r=C; g=X; b=0; }
  else if (H < 2.0f) { r=X; g=C; b=0; }
  else if (H < 3.0f) { r=0; g=C; b=X; }
  else if (H < 4.0f) { r=0; g=X; b=C; }
  else if (H < 5.0f) { r=X; g=0; b=C; }
  else { r=C; g=0; b=X; }
  *R = clamp_u8((int)((r+m)*255.0f));
  *G = clamp_u8((int)((g+m)*255.0f));
  *B = clamp_u8((int)((b+m)*255.0f));
}

// -----------------------------------------------------------------------------
// Face detector (heuristisch): YUYV->YCbCr skin thresholds, morph, largest CC
// -----------------------------------------------------------------------------

// Convert YUYV pixel at index p (byte offset) to Cb/Cr
static inline void yuyv_to_cbcr(const uint8_t *frame, int p, uint8_t *cb, uint8_t *cr) {
  // Y0 U Y1 V
  // U is Cb, V is Cr
  *cb = frame[p + 1];
  *cr = frame[p + 3];
}

// Create skin mask from raw YUYV frame (fast, per-pixel)
static void compute_skin_mask_from_yuyv(const uint8_t *frame, uint8_t *mask) {
  // mask: 0/255
  int idx = 0;
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x += 2) {
      int p = (y*WIDTH + x) * 2;
      uint8_t cb = frame[p + 1];
      uint8_t cr = frame[p + 3];
      // pixel x
      mask[y*WIDTH + x] = (cb >= SKIN_MIN_CB && cb <= SKIN_MAX_CB && cr >= SKIN_MIN_CR && cr <= SKIN_MAX_CR) ? 255 : 0;
      // pixel x+1: same U/V
      mask[y*WIDTH + x + 1] = mask[y*WIDTH + x];
    }
  }
}

// Simple morphological opening (erosion then dilation) with box kernel radius r
static void morph_open(uint8_t *mask, uint8_t *tmp, int w, int h, int r) {
  if (r <= 0) return;
  int diameter = r*2 + 1;
  // horizontal erosion -> tmp
  for (int y = 0; y < h; y++) {
    int row = y*w;
    for (int x = 0; x < w; x++) {
      int minv = 255;
      int left = x - r;
      int right = x + r;
      if (left < 0) left = 0;
      if (right >= w) right = w-1;
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
      int top = y - r;
      int bot = y + r;
      if (top < 0) top = 0;
      if (bot >= h) bot = h-1;
      for (int k = top; k <= bot; k++) {
        int v = tmp[k*w + x];
        if (v < minv) minv = v;
      }
      mask[y*w + x] = (uint8_t)minv;
    }
  }
  // dilation (horizontal then vertical) to complete opening
  // horizontal dilation -> tmp
  for (int y = 0; y < h; y++) {
    int row = y*w;
    for (int x = 0; x < w; x++) {
      int maxv = 0;
      int left = x - r;
      int right = x + r;
      if (left < 0) left = 0;
      if (right >= w) right = w-1;
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
      int top = y - r;
      int bot = y + r;
      if (top < 0) top = 0;
      if (bot >= h) bot = h-1;
      for (int k = top; k <= bot; k++) {
        int v = tmp[k*w + x];
        if (v > maxv) maxv = v;
      }
      mask[y*w + x] = (uint8_t)maxv;
    }
  }
}

// Connected components (two-pass) returning label map and largest component mask
// label_map must be int array of size w*h (allocated by caller)
static int extract_largest_component(const uint8_t *bin, uint8_t *out_mask, int *label_map, int w, int h) {
  int N = w*h;
  // init
  for (int i = 0; i < N; i++) label_map[i] = 0;
  int next_label = 1;
  // small union-find arrays (size N/4 approx) but we keep simple parent array sized N/2
  int *parent = (int*)malloc((N/2 + 4) * sizeof(int));
  if (!parent) return 0;
  parent[0] = 0;
  // first pass
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
        // both non-zero: pick min and union
        int a = left, b = up;
        int root_a = a; while (parent[root_a] != root_a) root_a = parent[root_a];
        int root_b = b; while (parent[root_b] != root_b) root_b = parent[root_b];
        if (root_a == root_b) {
          label_map[i] = root_a;
        } else {
          int m = (root_a < root_b) ? root_a : root_b;
          int M = (root_a < root_b) ? root_b : root_a;
          parent[M] = m;
          label_map[i] = m;
        }
      }
    }
  }
  // flatten parent
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
  // count sizes
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
  // find largest valid component within area bounds
  int best_label = 0;
  int best_count = 0;
  for (int l = 1; l < max_label; l++) {
    int c = counts[l];
    if (c > best_count) {
      best_count = c;
      best_label = l;
    }
  }
  // threshold by area
  if (best_count < FACE_MIN_AREA || best_count > FACE_MAX_AREA) {
    // no valid face
    memset(out_mask, 0, N);
    free(parent); free(counts);
    return 0;
  }
  // build out_mask: mark pixels whose root label == best_label
  for (int i = 0; i < N; i++) {
    int L = label_map[i];
    if (L > 0 && parent[L] == best_label) out_mask[i] = 255;
    else out_mask[i] = 0;
  }
  free(parent); free(counts);
  return best_count;
}

// High-level detect face region from YUYV frame (fast heuristic)
static void detect_face_region_from_yuyv(const uint8_t *frame_yuyv, uint8_t *out_face_mask, uint8_t *scratch) {
  // scratch must be at least WIDTH*HEIGHT bytes
  compute_skin_mask_from_yuyv(frame_yuyv, skin_mask);
  // morphological opening to remove noise
  morph_open(skin_mask, scratch, WIDTH, HEIGHT, 2);
  // connected components -> largest component
  // reuse scratch as label_map (int array) by allocating temporary int array
  int *label_map = (int*)malloc(WIDTH * HEIGHT * sizeof(int));
  if (!label_map) {
    memset(out_face_mask, 0, WIDTH*HEIGHT);
    return;
  }
  extract_largest_component(skin_mask, out_face_mask, label_map, WIDTH, HEIGHT);
  free(label_map);
}

// -----------------------------------------------------------------------------
// Sobel edge (uses OPT.edge_gain-like constant)
// -----------------------------------------------------------------------------
static float EDGE_GAIN_F = EDGE_GAIN;
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
      int mag = (int)(EDGE_GAIN_F * sqrtf((float)(gx*gx + gy*gy)));
      e[i] = clamp_u8(mag);
    }
  }
}

// -----------------------------------------------------------------------------
// Wischer: accumulators, separable blur, composite (as in previous version)
// -----------------------------------------------------------------------------
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

// draw wischer into accumulators; boost where face_mask present
static void draw_wischer_accumulators(uint32_t now_ms, const uint8_t *res, const uint8_t *edge_map, const uint8_t *frame_yuyv) {
  int N = WIDTH*HEIGHT;
  // clear accumulators
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

  // UV hue center (grelles UV)
  float base_hue = 0.78f;
  float hue_spread = 0.04f;

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
    hue += 0.006f * sinf(2.0f * M_PI * (0.7f * t_sec + 0.11f * idx));
    float sat = 0.85f - 0.15f * f_norm;
    float val = 0.95f + 0.05f * f_norm;
    uint8_t HR, HG, HB;
    hsv_to_rgb_255(hue, fclamp(sat,0.3f,1.0f), fclamp(val,0.8f,1.0f), &HR, &HG, &HB);

    for (int y = ys; y < ye; y++) {
      int row = y * WIDTH;
      for (int x = 0; x < WIDTH; x++) {
        int i = row + x;
        float activity = ((float)res[i] * 0.6f + (float)edge_map[i] * 0.4f) / 255.0f;
        float dy = 0.0f;
        if (y < y_start) dy = (float)(y_start - y);
        else if (y > y_end) dy = (float)(y - y_end);
        float falloff = 1.0f - fclamp(dy / (float)(bar_height + 1), 0.0f, 1.0f);
        float local_alpha = WISCHER_INTENSITY * WISCHER_MAX_ALPHA * activity * falloff;
        local_alpha = fclamp(local_alpha + 0.06f * falloff, 0.0f, 1.0f);

        // face boost: if face_mask present, amplify alpha and slightly shift hue to whiter-blue
        if (face_mask[i]) {
          local_alpha = fclamp(local_alpha * 1.6f, 0.0f, 1.0f);
          // blend HR/HG/HB slightly towards white-blue
          HR = clamp_u8((int)(HR*0.85f + 255*0.15f));
          HG = clamp_u8((int)(HG*0.88f + 255*0.12f));
          HB = clamp_u8((int)(HB*0.92f + 255*0.08f));
        }

        acc_r[i] += (float)HR * local_alpha;
        acc_g[i] += (float)HG * local_alpha;
        acc_b[i] += (float)HB * local_alpha;
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

// wrapper: draw -> blur -> composite
static void render_wischer(uint32_t now_ms, const uint8_t *res, const uint8_t *edge_map, const uint8_t *frame_yuyv) {
  draw_wischer_accumulators(now_ms, res, edge_map, frame_yuyv);
  separable_blur_acc(acc_r, tmp_r, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_g, tmp_g, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_b, tmp_b, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  separable_blur_acc(acc_a, tmp_a, WIDTH, HEIGHT, WISCHER_BLUR_RADIUS);
  composite_accumulators_over_rgb(rgb);
}

// -----------------------------------------------------------------------------
// Simple X-Ray overlay (uses face_mask to emphasize face edges)
// -----------------------------------------------------------------------------
static void overlay_xray_style(uint8_t *rgb_buf, const uint8_t *res, const uint8_t *edge_map, uint32_t now_ms) {
  float t = (float)now_ms / 1000.0f;
  float glow = 0.08f * (0.5f * (sinf(2.0f * M_PI * (0.5f * t)) + 1.0f));
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
    uint8_t R = clamp_u8((int)(r * 255.0f));
    uint8_t G = clamp_u8((int)(g * 255.0f));
    uint8_t B = clamp_u8((int)(b * 255.0f));
    int o = i*3;
    uint8_t A = (uint8_t)fclamp(HEATMAP_ALPHA, 0.0f, 255.0f);
    uint8_t R1 = (uint8_t)((rgb_buf[o+0] * (255 - A) + R * A) / 255);
    uint8_t G1 = (uint8_t)((rgb_buf[o+1] * (255 - A) + G * A) / 255);
    uint8_t B1 = (uint8_t)((rgb_buf[o+2] * (255 - A) + B * A) / 255);

    // If face_mask present, emphasize edges inside face region
    if (face_mask[i]) {
      // boost brightness slightly and shift to cooler white-blue
      R1 = clamp_u8((int)(R1 * 0.88f + 255 * 0.12f));
      G1 = clamp_u8((int)(G1 * 0.90f + 255 * 0.10f));
      B1 = clamp_u8((int)(B1 * 0.94f + 255 * 0.06f));
    }

    rgb_buf[o+0] = R1;
    rgb_buf[o+1] = G1;
    rgb_buf[o+2] = B1;
  }
}

// -----------------------------------------------------------------------------
// V4L2 / SDL init / cleanup (simplified, similar to previous versions)
// -----------------------------------------------------------------------------
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
    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) { perror("QBUF"); return -1; }
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(v4l2_fd, VIDIOC_STREAMON, &type) < 0) { perror("STREAMON"); return -1; }
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

// SDL init/cleanup
static int init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) { fprintf(stderr,"SDL_Init: %s\n",SDL_GetError()); return -1; }
  window = SDL_CreateWindow("MRT Viewer - Face Edge Wischer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
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
// -----------------------------------------------------------------------------
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

  skin_mask = (uint8_t*)malloc(N);
  face_mask = (uint8_t*)malloc(N);

  if (!gray || !bg || !residue || !edge || !flicker || !rgb ||
      !acc_r || !acc_g || !acc_b || !acc_a ||
      !tmp_r || !tmp_g || !tmp_b || !tmp_a ||
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
  free(skin_mask); free(face_mask);
}

// -----------------------------------------------------------------------------
// Simple YUYV->gray + RGB conversion (fast)
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

// Background update & residue
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
// -----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  if (init_sdl() != 0) { cleanup_sdl(); return EXIT_FAILURE; }
  if (allocate_buffers() != 0) { free_buffers(); cleanup_sdl(); return EXIT_FAILURE; }
  if (init_v4l2() != 0) { cleanup_v4l2(); free_buffers(); cleanup_sdl(); return EXIT_FAILURE; }

  bool running = true;
  SDL_Event event;
  bool face_detector_enabled = true; // Tastendruck T toggles

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
        if (event.key.keysym.sym == SDLK_t) face_detector_enabled = !face_detector_enabled;
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

    // face detection (from raw YUYV frame) -> face_mask
    if (face_detector_enabled) {
      // use tmp_a as scratch for morph_open
      detect_face_region_from_yuyv(frame, face_mask, tmp_a ? (uint8_t*)tmp_a : skin_mask);
    } else {
      memset(face_mask, 0, WIDTH*HEIGHT);
    }

    // render wischer (uses face_mask to emphasize)
    render_wischer(SDL_GetTicks(), residue, edge, frame);

    // overlay X-Ray (also uses face_mask)
    overlay_xray_style(rgb, residue, edge, SDL_GetTicks());

    // present
    SDL_UpdateTexture(texture, NULL, rgb, WIDTH * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // small HUD: show whether face detector is on (simple rectangle)
    if (face_detector_enabled) {
      SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(renderer, 200, 230, 255, 160);
      SDL_Rect rct = {8, 8, 18, 18};
      SDL_RenderFillRect(renderer, &rct);
      SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    SDL_RenderPresent(renderer);

    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) { perror("QBUF"); running = false; }
  }

  stop_v4l2();
  cleanup_v4l2();
  free_buffers();
  cleanup_sdl();
  return EXIT_SUCCESS;
}

