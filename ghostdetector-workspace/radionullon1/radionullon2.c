// radionullon_viewer.c
// Realtime Radionullon-Ghost Visualizer using V4L2 + SDL2
// Captures YUYV frames, builds a background model, computes anomaly heatmap, overlays & displays.

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
#include <linux/videodev2.h>
#include <time.h>

#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480
#define DEVICE "/dev/video0"

// Tunables for "Radionullon" anomaly visualization
#define BG_LEARN_RATE 0.01f // background EMA factor
#define RESIDUE_THRESH 15   // intensity difference for motion mask
#define FLICKER_THRESH 20   // temporal flicker sensitivity
#define EDGE_GAIN   1.6f  // Sobel edge gain
#define HEATMAP_ALPHA 155  // overlay alpha (0..255)
#define MAX_FRAMES_FLICKER 8  // small temporal window for flicker

// V4L2 mmap buffers
struct buffer {
  void *start;
  size_t length;
};

static struct buffer *buffers = NULL;
static unsigned int  n_buffers = 0;

// Background model and working buffers
static uint8_t *gray   = NULL;
static float *bg    = NULL;
static uint8_t *residue = NULL;
static uint8_t *edge   = NULL;
static uint8_t *flicker = NULL; // simple temporal metric per pixel
static uint8_t *rgb   = NULL; // output RGB888

// Utility clamp
static inline uint8_t clamp_u8(int v) {
  if (v < 0) return 0;
  if (v > 255) return 255;
  return (uint8_t)v;
}

// Convert YUYV to grayscale and RGB
static void yuyv_to_gray_rgb(const uint8_t *in, uint8_t *g, uint8_t *out_rgb) {
  // YUYV: [Y0 U0 Y1 V0] repeating
  // Use Y as grayscale. Build naive RGB from YUV for display.
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x += 2) {
      int idx = (y*WIDTH + x);
      int p = (y*WIDTH + x) * 2; // 2 bytes per pixel average, but 4 per 2 pixels

      uint8_t Y0 = in[p + 0];
      uint8_t U = in[p + 1];
      uint8_t Y1 = in[p + 2];
      uint8_t V = in[p + 3];

      g[idx]  = Y0;
      g[idx + 1] = Y1;

      // YUV to RGB (BT.601 approx)
      int C0 = Y0 - 16;
      int C1 = Y1 - 16;
      int D = U - 128;
      int E = V - 128;

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

// Simple flicker measure: temporal difference accumulation with decay
static void update_flicker(const uint8_t *g, uint8_t *f) {
  static uint8_t prev = 0; // global avg prev frame
  // global average as coarse flicker detector
  uint64_t sum = 0;
  for (int i = 0; i < WIDTH*HEIGHT; i++) sum += g[i];
  uint8_t avg = (uint8_t)(sum / (WIDTH*HEIGHT));

  int delta = abs((int)avg - (int)prev);
  prev = avg;
  // If global flicker is high, softly boost all f
  uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    int v = f[i] + boost;
    f[i] = (uint8_t)((v > 255) ? 255 : v);
    // decay
    if (f[i] > 0) f[i] -= 1;
  }
}

// Anomaly score and heatmap overlay onto RGB
static void overlay_heatmap(uint8_t *rgb, const uint8_t *res, const uint8_t *edge, const uint8_t *flicker) {
  for (int i = 0; i < WIDTH*HEIGHT; i++) {
    // Score from residue + edge + flicker
    int score = (int)res[i] + (int)(edge[i]/2) + (int)(flicker[i]/3);
    if (score < 30) continue; // ignore low score

    // Map score to heat color (blue -> green -> yellow -> red)
    int s = (score > 255) ? 255 : score;
    uint8_t r, g, b;
    if (s < 64) { b = 255; g = s*4; r = 0; }
    else if (s < 128) { b = 255 - (s-64)*4; g = 255; r = 0; }
    else if (s < 192) { b = 0; g = 255; r = (s-128)*4; }
    else { b = 0; g = 255 - (s-192)*4; r = 255; }

    int o = i*3;
    // Alpha blend onto rgb
    uint8_t R0 = rgb[o+0], G0 = rgb[o+1], B0 = rgb[o+2];
    rgb[o+0] = (uint8_t)((R0*(255-HEATMAP_ALPHA) + r*HEATMAP_ALPHA)/255);
    rgb[o+1] = (uint8_t)((G0*(255-HEATMAP_ALPHA) + g*HEATMAP_ALPHA)/255);
    rgb[o+2] = (uint8_t)((B0*(255-HEATMAP_ALPHA) + b*HEATMAP_ALPHA)/255);
  }
}

// Initialize V4L2
static int init_v4l2(int *fd) {
  *fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
  if (*fd < 0) { perror("open"); return -1; }

  struct v4l2_capability cap;
  if (ioctl(*fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); return -1; }
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "Device does not support video capture\n"); return -1;
  }
  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    fprintf(stderr, "Device does not support streaming\n"); return -1;
  }

  // Set format YUYV 640x480
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width   = WIDTH;
  fmt.fmt.pix.height   = HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field   = V4L2_FIELD_NONE;
  if (ioctl(*fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return -1; }

  // Request buffers
  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof(req));
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  if (ioctl(*fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return -1; }
  if (req.count < 2) { fprintf(stderr, "Insufficient buffer memory\n"); return -1; }

  buffers = calloc(req.count, sizeof(*buffers));
  for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = n_buffers;
    if (ioctl(*fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return -1; }

    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start