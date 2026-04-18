// radionullon_viewer.c
// Realtime Radionullon-Ghost Visualizer using V4L2 + SDL2
// Amalgamiert in einer einzigen Datei.
//
// Modifiziert:
// - Fenster ist resizable und startet maximiert; Inhalt skaliert automatisch.
// - Horizontaler Wischer von unten nach oben mit Alpha-Blending (weiche Kanten).

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
#include <sys/select.h> // Nötig für select()
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

// Tunables for "Radionullon" anomaly visualization
#define BG_LEARN_RATE 0.01f // background EMA factor
#define RESIDUE_THRESH 15   // intensity difference for motion mask
#define FLICKER_THRESH 20   // temporal flicker sensitivity
#define EDGE_GAIN   1.6f    // Sobel edge gain
#define HEATMAP_ALPHA 155   // overlay alpha (0..255)
#define MAX_FRAMES_FLICKER 8

// Konfiguration des Bottom-Wipes
#define WIPE_HEIGHT 80            // Höhe des Wischbandes (px)
#define CYCLE_DURATION_MS 1000    // Dauer eines Durchlaufs (ms)
#define WIPE_MAX_ALPHA 180        // maximale Alpha beim Wischen (0..255)
#define WIPE_R 255                // Wischer-Farbe: Rot
#define WIPE_G 0
#define WIPE_B 0

// V4L2 mmap buffers
struct buffer {
  void *start;
  size_t length;
};

static struct buffer *buffers = NULL;
static unsigned int  n_buffers = 0;
static int v4l2_fd = -1;

// Background model and working buffers
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

// Per-Pixel Alpha-Blend: dst = (dst*(255-a) + src*a) / 255
static inline uint8_t blend_u8(uint8_t dst, uint8_t src, uint8_t alpha) {
  return (uint8_t)((dst * (255 - alpha) + src * alpha) / 255);
}

// -----------------------------------------------------------------------------
// 3. Bildverarbeitungs-Pipeline
// -----------------------------------------------------------------------------

/**
 * @brief Zeichnet einen horizontalen Wischer, der von unten nach oben wandert.
 * - Zeitbasiert (SDL_GetTicks).
 * - Weiche Kanten durch einen Alpha-Gradient über die WIPE_HEIGHT.
 * - Blendet auf bestehendes RGB mittels Alpha-Blending.
 */
static void draw_bottom_wipe_alpha(uint8_t *out_rgb) {
  uint32_t t_ms = SDL_GetTicks();
  float progress = (float)(t_ms % CYCLE_DURATION_MS) / (float)CYCLE_DURATION_MS;

  // Start bei y = HEIGHT (unterhalb) und wandert bis y = -WIPE_HEIGHT (oberhalb komplett raus)
  // Dadurch entstehen sauberer Ein- und Auslauf.
  int band_y_start = HEIGHT - (int)(progress * (HEIGHT + WIPE_HEIGHT));
  int band_y_end   = band_y_start + WIPE_HEIGHT;

  // Clipping
  int draw_y_start = (band_y_start < 0) ? 0 : band_y_start;
  int draw_y_end   = (band_y_end   > HEIGHT) ? HEIGHT : band_y_end;
  if (draw_y_start >= draw_y_end) return;

  // Alpha-Gradient über die Höhe des Bandes (triangular falloff zur Kante)
  for (int y = draw_y_start; y < draw_y_end; y++) {
    int dy = y - band_y_start; // 0 .. WIPE_HEIGHT-1
    // Weicher Verlauf: Alpha steigt zur Bandmitte, fällt zu den Rändern
    // Formel: alpha = max * (1 - |(2*dy/W - 1)|), W=WIPE_HEIGHT
    float rel = (WIPE_HEIGHT > 1) ? (2.0f * (float)dy / (float)WIPE_HEIGHT - 1.0f) : 0.0f;
    float falloff = 1.0f - fabsf(rel);
    int a = (int)(falloff * (float)WIPE_MAX_ALPHA);
    if (a < 0) a = 0; if (a > 255) a = 255;
    uint8_t alpha = (uint8_t)a;

    for (int x = 0; x < WIDTH; x++) {
      int o = (y * WIDTH + x) * 3;
      out_rgb[o + 0] = blend_u8(out_rgb[o + 0], WIPE_R, alpha);
      out_rgb[o + 1] = blend_u8(out_rgb[o + 1], WIPE_G, alpha);
      out_rgb[o + 2] = blend_u8(out_rgb[o + 2], WIPE_B, alpha);
    }
  }
}

// Convert YUYV to grayscale and RGB
static void yuyv_to_gray_rgb(const uint8_t *in, uint8_t *g, uint8_t *out_rgb) {
  // YUYV: [Y0 U0 Y1 V0] repeating
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

      // YUV to RGB (BT.601 approx)
      int C0 = Y0 - 16;
      int C1 = Y1 - 16;
      int D  = U  - 128;
      int E  = V  - 128;

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

// Sobel edge magnitude
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

// Update background model (EMA) and compute residue
static void update_bg_and_residue(const uint8_t *g, float *bg, uint8_t *res) {
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    bg[i] = (1.0f - BG_LEARN_RATE) * bg[i] + BG_LEARN_RATE * (float)g[i];
    int diff = abs((int)g[i] - (int)bg[i]);
    res[i] = (diff > RESIDUE_THRESH) ? (uint8_t)diff : 0;
  }
}

// Simple flicker measure
static void update_flicker(const uint8_t *g, uint8_t *f) {
  static uint8_t prev = 0; // global avg prev frame
  uint64_t sum = 0;
  for (int i = 0; i < WIDTH*HEIGHT; i++) sum += g[i];
  uint8_t avg = (uint8_t)(sum / (WIDTH*HEIGHT));

  int delta = abs((int)avg - (int)prev);
  prev = avg;
  uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    int v = f[i] + boost;
    f[i] = (uint8_t)((v > 255) ? 255 : v);
    if (f[i] > 0) f[i] -= 1; // decay
  }
}

// Anomaly score and heatmap overlay onto RGB
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

  // Queue initial buffers
  for (unsigned int i = 0; i < n_buffers; i++) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = i;
    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) { perror("V4L2: VIDIOC_QBUF"); return -1; }
  }

  // Start streaming
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

  // weichere Skalierung
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

  // Logische Größe: Renderer skaliert Inhalt automatisch auf Fenstergröße
  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

  texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGB24, // Wir konvertieren zu RGB888
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
  bg      = (float*)calloc(size_bytes, sizeof(float)); // Wichtig: calloc für 0.0f
  residue = (uint8_t*)malloc(size_bytes);
  edge    = (uint8_t*)malloc(size_bytes);
  flicker = (uint8_t*)calloc(size_bytes, sizeof(uint8_t));
  rgb     = (uint8_t*)malloc(WIDTH * HEIGHT * 3); // RGB888

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
    // 1. SDL Events (Fenster schließen, ESC-Taste)
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
      }
      // Optional: Fenstergröße-Events (Renderer skaliert bereits via LogicalSize)
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        // Kein zusätzlicher Code nötig.
      }
    }

    // 2. Auf neues Kamera-Frame warten (mit Timeout)
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(v4l2_fd, &fds);
    struct timeval tv = { .tv_sec = 1, .tv_usec = 0 }; // 1 Sekunde Timeout

    int r = select(v4l2_fd + 1, &fds, NULL, NULL, &tv);
    if (r == -1) {
      if (errno == EINTR) continue; // select unterbrochen, weitermachen
      perror("select");
      running = false;
    } else if (r == 0) {
      fprintf(stderr, "Kamera-Timeout (select)\n");
      continue;
    }

    // 3. V4L2 Puffer holen (Dequeue)
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(v4l2_fd, VIDIOC_DQBUF, &buf) < 0) {
      if (errno == EAGAIN) continue; // Nichts da, obwohl select sagte...
      perror("VIDIOC_DQBUF");
      running = false;
      continue;
    }

    // 4. Bildverarbeitung (Die Pipeline)
    uint8_t *frame_data = (uint8_t *)buffers[buf.index].start;

    // Schritt 1: YUYV -> Graustufen UND RGB
    yuyv_to_gray_rgb(frame_data, gray, rgb);

    // Schritt 2: Bottom-Wipe mit Alpha-Blending
    draw_bottom_wipe_alpha(rgb);

    // Schritt 3: Hintergrundmodell aktualisieren
    update_bg_and_residue(gray, bg, residue);

    // Schritt 4: Kanten berechnen
    sobel_edge(gray, edge);

    // Schritt 5: Flackern messen
    update_flicker(gray, flicker);

    // Schritt 6: Heatmap (Effekte) überlagern
    overlay_heatmap(rgb, residue, edge, flicker);

    // 5. Bild an SDL übergeben und anzeigen
    SDL_UpdateTexture(texture, NULL, rgb, WIDTH * 3); // 3 Bytes pro Pixel (RGB)
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // 6. V4L2 Puffer zurückgeben (Queue)
    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) {
      perror("VIDIOC_QBUF");
      running = false;
    }
  } // Ende der Hauptschleife

  // 7. Aufräumen
  printf("\nBeende...\n");
  stop_v4l2();
  cleanup_v4l2();
  free_buffers();
  cleanup_sdl();

  printf("Aufgeräumt. Tschüss!\n");
  return EXIT_SUCCESS;
}

