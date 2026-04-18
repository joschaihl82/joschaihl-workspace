// radionullon_viewer.c
// Realtime Radionullon-Ghost Visualizer using V4L2 + SDL2
// Amalgamiert in einer einzigen Datei.
//
// Modifiziert:
// - Fenster resizable, startet maximiert; Inhalt skaliert automatisch.
// - Mehrere horizontale Balken fallen nach unten.
// - Jeder Balken blinkt individuell (eigene Frequenz & Phase) zwischen Blau und Rot.
// - Balkenhöhe pulsiert mit sin().

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
// 1. Definitionen und Globale Variablen
// -----------------------------------------------------------------------------

#define WIDTH 640
#define HEIGHT 480
#define DEVICE "/dev/video0"

// Radionullon-Visualisierung (Anomalie-Metriken)
#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN   1.6f
#define HEATMAP_ALPHA 155

// Konfiguration der fallenden Balken
#define BAR_BASE_HEIGHT 20       // Basis-Höhe eines Balkens (px)
#define BAR_HEIGHT_AMPL 24       // Zusatzhöhe (Amplitude), per sin() (px)
#define BAR_SPACING 90           // Abstand zwischen parallelen Balken (px)

// Farben: Blau und Rot (Strobe)
#define BLUE_R 0
#define BLUE_G 0
#define BLUE_B 255
#define RED_R 255
#define RED_G 0
#define RED_B 0

// Frequenzen (ms pro Durchlauf) für das Fallen
#define CYCLE_LOW_MS 1200.0f     // langsamer Sweep (niederfrequent)
#define CYCLE_HIGH_MS 220.0f     // schneller Sweep (hochfrequent)
#define EXPLORATION_PERIOD_MS 5000 // Sinuswobbler-Periode für die Mischung

// Laufzeit-Variablen für adaptive Mischung
static float cycle_duration_ms = 500.0f;   // effektive Dauer, pro Frame aktualisiert
static float mix_weight = 0.5f;            // 0..1: Anteil Hochfrequenz (1) vs. Niederfrequenz (0)

// V4L2 mmap buffers
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

// SDL Globals
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

// -----------------------------------------------------------------------------
// 2. Utility-Funktionen
// -----------------------------------------------------------------------------

static inline uint8_t clamp_u8(int v) {
  if (v < 0) return 0;
  if (v > 255) return 255;
  return (uint8_t)v;
}

// -----------------------------------------------------------------------------
// 3. Bildverarbeitungs-Pipeline-Grundfunktionen
// -----------------------------------------------------------------------------

// YUYV -> Grau & RGB
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

// Sobel-Kanten
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
      int mag = (int)(EDGE_GAIN * sqrtf((float)(gx*gx + gy*gy)));
      e[i] = clamp_u8(mag);
    }
  }
}

// Hintergrund-EMA & Residuum
static void update_bg_and_residue(const uint8_t *g, float *bg, uint8_t *res) {
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    bg[i] = (1.0f - BG_LEARN_RATE) * bg[i] + BG_LEARN_RATE * (float)g[i];
    int diff = abs((int)g[i] - (int)bg[i]);
    res[i] = (diff > RESIDUE_THRESH) ? (uint8_t)diff : 0;
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

// Heatmap-Overlay
static void overlay_heatmap(uint8_t *rgb, const uint8_t *res, const uint8_t *edge, const uint8_t *flicker) {
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    int score = (int)res[i] + (int)(edge[i]/2) + (int)(flicker[i]/3);
    if (score < 30) continue;

    int s = (score > 255) ? 255 : score;
    uint8_t r, g, b;
    if (s < 64) { b = 255; g = s*4; r = 0; }
    else if (s < 128) { b = 255 - (s-64)*4; g = 255; r = 0; }
    else if (s < 192) { b = 0; g = 255; r = (s-128)*4; }
    else { b = 0; g = 255 - (s-192)*4; r = 255; }

    int o = i*3;
    uint8_t R0 = rgb[o+0], G0 = rgb[o+1], B0 = rgb[o+2];
    rgb[o+0] = (uint8_t)((R0*(255-HEATMAP_ALPHA) + r*HEATMAP_ALPHA)/255);
    rgb[o+1] = (uint8_t)((G0*(255-HEATMAP_ALPHA) + g*HEATMAP_ALPHA)/255);
    rgb[o+2] = (uint8_t)((B0*(255-HEATMAP_ALPHA) + b*HEATMAP_ALPHA)/255);
  }
}

// -----------------------------------------------------------------------------
// 3a. Sichtbarkeits-Score & adaptive Frequenzmischung
// -----------------------------------------------------------------------------

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
      int s = (int)res[i] + ((int)edge[i] / 2);
      sum += (double)s;
      samples++;
    }
  }

  if (samples == 0) return 0.0f;
  return (float)(sum / (double)samples);
}

static void update_cycle_duration(uint32_t now_ms, float vis_score) {
  float phase = (float)(now_ms % EXPLORATION_PERIOD_MS) / (float)EXPLORATION_PERIOD_MS; // 0..1
  float wobble = 0.5f + 0.5f * sinf(2.0f * M_PI * phase); // 0..1

  float env_push = (vis_score > 25.0f) ? 0.15f : -0.10f;
  mix_weight = 0.6f * wobble + 0.4f * mix_weight + env_push;

  if (mix_weight < 0.0f) mix_weight = 0.0f;
  if (mix_weight > 1.0f) mix_weight = 1.0f;

  cycle_duration_ms = (1.0f - mix_weight) * CYCLE_LOW_MS + mix_weight * CYCLE_HIGH_MS;
}

// -----------------------------------------------------------------------------
// 3b. Zeichnen: Balken fallen nach unten, jeder blinkt unterschiedlich,
//               Blau↔Rot-Strobe und sin()-Breitenpuls
// -----------------------------------------------------------------------------

static float draw_falling_horizontal_bars_and_score(uint8_t *out_rgb,
                                                    const uint8_t *res, const uint8_t *edge,
                                                    uint32_t now_ms) {
  // Fortschritt im aktuellen Zyklus
  uint32_t ms_in_cycle = (uint32_t)fmodf((float)now_ms, cycle_duration_ms);
  float progress = (float)ms_in_cycle / cycle_duration_ms;

  // Dynamische Balkenhöhe per sin() (unabhängig vom Fallen)
  const float width_period_ms = 1400.0f;
  float wphase = fmodf((float)now_ms, width_period_ms) / width_period_ms; // 0..1
  float wsin = 0.5f * (sinf(2.0f * M_PI * wphase) + 1.0f);                // 0..1
  int bar_height = BAR_BASE_HEIGHT + (int)(BAR_HEIGHT_AMPL * wsin);

  // Basisband: oben starten (-bar_height) und nach unten fallen bis HEIGHT
  int base_y_start = (int)(progress * (HEIGHT + bar_height)) - bar_height;
  int base_y_end   = base_y_start + bar_height;

  // Anzahl paralleler Balken über die Bildhöhe
  int max_rows = (HEIGHT / BAR_SPACING) + 2;

  // Parameter für individuelle Blinkmuster pro Balken
  const float base_freq_hz = 8.0f;      // Grundfrequenz fürs Blinken
  const float freq_step_hz = 1.3f;      // Frequenzschritt pro Balkenindex
  const float phase_step   = 0.37f;     // Phasenoffset pro Balkenindex (0..1)
  const float jitter_amp   = 0.15f;     // leichte Zeit-basierte Jitter-Amplitude

  // Zeichnen
  for (int r = -max_rows; r <= max_rows; r++) {
    int y_start = base_y_start + r * BAR_SPACING;
    int y_end   = y_start + bar_height;

    int draw_y_start = (y_start < 0) ? 0 : y_start;
    int draw_y_end   = (y_end   > HEIGHT) ? HEIGHT : y_end;
    if (draw_y_start >= draw_y_end) continue;

    // Individuelle Blinkfrequenz und Phase für diesen Balken
    // Index normalisieren (0..N-1)
    int idx = r + max_rows; // 0..(2*max_rows)
    float freq_hz = base_freq_hz + (float)idx * freq_step_hz;
    float phase0  = (float)idx * phase_step; // unterschiedliche Startphase
    // kleiner zeitabhängiger Jitter, damit Muster lebendiger wirken
    float jitter = jitter_amp * sinf(2.0f * M_PI * (0.0007f * (float)now_ms));

    // Blinksignal: sin(2π*(t*freq + phase + jitter)) -> Vorzeichen toggelt Farbe
    float t_sec = (float)now_ms / 1000.0f;
    float sig = sinf(2.0f * M_PI * (t_sec * freq_hz + phase0 + jitter));
    bool use_red = (sig >= 0.0f);

    uint8_t CR = use_red ? RED_R : BLUE_R;
    uint8_t CG = use_red ? RED_G : BLUE_G;
    uint8_t CB = use_red ? RED_B : BLUE_B;

    for (int y = draw_y_start; y < draw_y_end; y++) {
      int o = (y * WIDTH) * 3;
      for (int x = 0; x < WIDTH; x++) {
        int p = o + x * 3;
        out_rgb[p + 0] = CR;
        out_rgb[p + 1] = CG;
        out_rgb[p + 2] = CB;
      }
    }
  }

  // Sichtbarkeit für das Basisband schätzen (repräsentativ für alle parallelen Balken)
  return estimate_visibility_score(res, edge, base_y_start, base_y_end);
}

// -----------------------------------------------------------------------------
// 4. V4L2 (Kamera) Initialisierung und Cleanup
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

  printf("V4L2 initialisiert. Streaming gestartet.\n");
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
// 5. SDL (Fenster) Initialisierung und Cleanup
// -----------------------------------------------------------------------------

static int init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return -1;
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  window = SDL_CreateWindow("Radionullon Ghost Viewer",
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

  printf("SDL initialisiert.\n");
  return 0;
}

static void cleanup_sdl() {
  if (texture) SDL_DestroyTexture(texture);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();
}

// -----------------------------------------------------------------------------
// 6. Speicherallokation für Puffer
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
// 7. Hauptprogramm (Main Loop)
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

  printf("Starte Hauptschleife. Drücke ESC oder schließe das Fenster zum Beenden.\n");

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
    } else if (r == 0) {
      fprintf(stderr, "Kamera-Timeout (select)\n");
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

    // 1) YUYV -> Grau & RGB
    yuyv_to_gray_rgb(frame_data, gray, rgb);

    // 2) Hintergrund & Kante (für Score)
    update_bg_and_residue(gray, bg, residue);
    sobel_edge(gray, edge);

    // 3) Balken zeichnen + Score messen (jeder Balken blinkt individuell)
    uint32_t now_ms = SDL_GetTicks();
    float vis_score = draw_falling_horizontal_bars_and_score(rgb, residue, edge, now_ms);

    // 4) Frequenz-Mischung aktualisieren (Sinus + Umweltanpassung)
    update_cycle_duration(now_ms, vis_score);

    // 5) Flackern
    update_flicker(gray, flicker);

    // 6) Heatmap Overlay
    overlay_heatmap(rgb, residue, edge, flicker);

    // 7) Present
    SDL_UpdateTexture(texture, NULL, rgb, WIDTH * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) {
      perror("VIDIOC_QBUF");
      running = false;
    }
  }

  printf("\nBeende...\n");
  stop_v4l2();
  cleanup_v4l2();
  free_buffers();
  cleanup_sdl();

  printf("Aufgeräumt. Tschüss!\n");
  return EXIT_SUCCESS;
}

