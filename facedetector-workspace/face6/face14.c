// main.c
// Reines C: v4l2 capture (YUYV 640x480), SDL2 render,
// KLT (single-scale), Kalman (CV 2D), initiale Detektion via Gradientenenergie,
// Re-Detektion via normierter Kreuzkorrelation (NCC) Template-Matching.

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
#include <SDL2/SDL.h>
#include <time.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 480
#define NUM_BUFFERS 4
#define MAX_POINTS 200
#define KLT_WIN 7
#define KLT_MAX_ITERS 12
#define KLT_EPS 0.01f

#define REDETECT_MIN_POINTS 25
#define REDETECT_PERIOD_FRAMES 120
#define SEARCH_MARGIN 80     // NCC Suchfensterradius um erwartete Position
#define TEMPLATE_MAX_W 160   // Max Templategröße zur Begrenzung Rechenaufwand
#define TEMPLATE_MAX_H 160

typedef struct { void* start; size_t length; } Buffer;

typedef struct { int x, y, w, h; } Rect;

typedef struct { float x, y; bool valid; } Point2f;

typedef struct {
    int fd;
    Buffer bufs[NUM_BUFFERS];
    struct v4l2_buffer buf;
} V4L2Ctx;

typedef struct {
    // State: [x, y, vx, vy]
    float x, y, vx, vy;
    // Covariance (4x4, row-major)
    float P[16];
    float Q_pos; // process noise on position
    float R_meas; // measurement noise
} KalmanCV;

typedef struct {
    int w, h;
    uint8_t* data;   // template grayscale
    float mean;      // cached mean
    float std;       // cached std (epsilon floor)
} Template;

// --------- Utility ----------
static uint64_t now_ms() {
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static inline int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
static inline float clampf(float v, float lo, float hi){ return v < lo ? lo : (v > hi ? hi : v); }

// --------- v4l2 ----------
static void v4l2_xioctl(int fd, unsigned long req, void* arg) {
    int r;
    do { r = ioctl(fd, req, arg); } while (r == -1 && errno == EINTR);
    if (r == -1) { perror("ioctl"); exit(EXIT_FAILURE); }
}

static void v4l2_open(V4L2Ctx* ctx, const char* dev) {
    ctx->fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (ctx->fd < 0) { perror("open v4l2"); exit(EXIT_FAILURE); }

    struct v4l2_capability cap = {0};
    v4l2_xioctl(ctx->fd, VIDIOC_QUERYCAP, &cap);
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) || !(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "Device does not support capture/streaming\n"); exit(EXIT_FAILURE);
    }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    v4l2_xioctl(ctx->fd, VIDIOC_S_FMT, &fmt);
    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV || fmt.fmt.pix.width != WIDTH || fmt.fmt.pix.height != HEIGHT) {
        fprintf(stderr, "Device doesn't support requested format/size\n"); exit(EXIT_FAILURE);
    }

    struct v4l2_requestbuffers req = {0};
    req.count = NUM_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    v4l2_xioctl(ctx->fd, VIDIOC_REQBUFS, &req);
    if (req.count < NUM_BUFFERS) { fprintf(stderr, "Insufficient buffer memory\n"); exit(EXIT_FAILURE); }

    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        v4l2_xioctl(ctx->fd, VIDIOC_QUERYBUF, &buf);
        ctx->bufs[i].length = buf.length;
        ctx->bufs[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, buf.m.offset);
        if (ctx->bufs[i].start == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }
    }
    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        v4l2_xioctl(ctx->fd, VIDIOC_QBUF, &buf);
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_xioctl(ctx->fd, VIDIOC_STREAMON, &type);
}

static bool v4l2_dequeue(V4L2Ctx* ctx, void** data, size_t* len) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    int r = ioctl(ctx->fd, VIDIOC_DQBUF, &buf);
    if (r == -1) {
        if (errno == EAGAIN) return false;
        perror("DQBUF"); exit(EXIT_FAILURE);
    }
    *data = ctx->bufs[buf.index].start;
    *len = buf.bytesused;
    ctx->buf = buf;
    return true;
}

static void v4l2_enqueue(V4L2Ctx* ctx) { v4l2_xioctl(ctx->fd, VIDIOC_QBUF, &ctx->buf); }

static void v4l2_close(V4L2Ctx* ctx) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_xioctl(ctx->fd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < NUM_BUFFERS; i++) munmap(ctx->bufs[i].start, ctx->bufs[i].length);
    close(ctx->fd);
}

// --------- Pixel Conversion ----------
static void yuyv_to_rgb(uint8_t* yuyv, uint8_t* rgb) {
    for (int i = 0, j = 0; i < WIDTH * HEIGHT * 2; i += 4, j += 6) {
        int Y0 = yuyv[i+0], U = yuyv[i+1], Y1 = yuyv[i+2], V = yuyv[i+3];
        int C0 = Y0 - 16, C1 = Y1 - 16, D = U - 128, E = V - 128;
        int r0 = (298*C0 + 409*E + 128) >> 8;
        int g0 = (298*C0 - 100*D - 208*E + 128) >> 8;
        int b0 = (298*C0 + 516*D + 128) >> 8;
        int r1 = (298*C1 + 409*E + 128) >> 8;
        int g1 = (298*C1 - 100*D - 208*E + 128) >> 8;
        int b1 = (298*C1 + 516*D + 128) >> 8;
        rgb[j+0] = r0 < 0 ? 0 : r0 > 255 ? 255 : r0;
        rgb[j+1] = g0 < 0 ? 0 : g0 > 255 ? 255 : g0;
        rgb[j+2] = b0 < 0 ? 0 : b0 > 255 ? 255 : b0;
        rgb[j+3] = r1 < 0 ? 0 : r1 > 255 ? 255 : r1;
        rgb[j+4] = g1 < 0 ? 0 : g1 > 255 ? 255 : g1;
        rgb[j+5] = b1 < 0 ? 0 : b1 > 255 ? 255 : b1;
    }
}

static void yuyv_to_gray(uint8_t* yuyv, uint8_t* gray) {
    for (int i = 0, j = 0; i < WIDTH * HEIGHT * 2; i += 4) {
        gray[j++] = yuyv[i+0];
        gray[j++] = yuyv[i+2];
    }
}

// --------- Image Helpers ----------
static inline uint8_t get_gray(uint8_t* g, int x, int y) {
    if (x < 0) x = 0; if (x >= WIDTH) x = WIDTH-1;
    if (y < 0) y = 0; if (y >= HEIGHT) y = HEIGHT-1;
    return g[y*WIDTH + x];
}

static void compute_gradients(uint8_t* g, float* Ix, float* Iy) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int xm1 = x-1, xp1 = x+1, ym1 = y-1, yp1 = y+1;
            float gx = (float)get_gray(g, xp1, y) - (float)get_gray(g, xm1, y);
            float gy = (float)get_gray(g, x, yp1) - (float)get_gray(g, x, ym1);
            Ix[y*WIDTH + x] = gx * 0.5f;
            Iy[y*WIDTH + x] = gy * 0.5f;
        }
    }
}

// --------- KLT Feature Init (Shi–Tomasi-ish) ----------
static int init_klt_points(uint8_t* g, Rect roi, Point2f* pts, int max_pts) {
    int count = 0;
    int step = 12;
    for (int yy = roi.y + step; yy < roi.y + roi.h - step; yy += step) {
        for (int xx = roi.x + step; xx < roi.x + roi.w - step; xx += step) {
            if (count >= max_pts) break;
            float Sxx=0, Syy=0, Sxy=0;
            for (int j=-1;j<=1;j++) for(int i=-1;i<=1;i++){
                float gx = (float)get_gray(g, xx+i+1, yy+j) - (float)get_gray(g, xx+i-1, yy+j);
                float gy = (float)get_gray(g, xx+i, yy+j+1) - (float)get_gray(g, xx+i, yy+j-1);
                Sxx += gx*gx; Syy += gy*gy; Sxy += gx*gy;
            }
            float trace = Sxx + Syy;
            float det = Sxx*Syy - Sxy*Sxy;
            float disc = fmaxf(0.0f, trace*trace - 4.0f*det);
            float lambda_min = 0.5f * (trace - sqrtf(disc));
            if (lambda_min > 800.0f) {
                pts[count].x = (float)xx; pts[count].y = (float)yy; pts[count].valid = true;
                count++;
            }
        }
        if (count >= max_pts) break;
    }
    return count;
}

// --------- KLT Optical Flow (Gauss–Newton, single-scale) ----------
static void klt_track_points(uint8_t* prev, uint8_t* curr, Point2f* pts, int n) {
    float* Ix = (float*)malloc(WIDTH*HEIGHT*sizeof(float));
    float* Iy = (float*)malloc(WIDTH*HEIGHT*sizeof(float));
    compute_gradients(curr, Ix, Iy);

    for (int k = 0; k < n; k++) {
        if (!pts[k].valid) continue;
        float x = pts[k].x, y = pts[k].y;
        float dx = 0.0f, dy = 0.0f;
        bool ok = true;

        for (int iter = 0; iter < KLT_MAX_ITERS; iter++) {
            float Gxx=0, Gxy=0, Gyy=0, bx=0, by=0;
            for (int j = -KLT_WIN; j <= KLT_WIN; j++) {
                for (int i = -KLT_WIN; i <= KLT_WIN; i++) {
                    int xp = (int)(x + i);
                    int yp = (int)(y + j);
                    int xc = (int)(x + dx + i);
                    int yc = (int)(y + dy + j);
                    if (xc < 0 || xc >= WIDTH || yc < 0 || yc >= HEIGHT ||
                        xp < 0 || xp >= WIDTH || yp < 0 || yp >= HEIGHT) { ok = false; break; }
                    float I0 = (float)get_gray(prev, xp, yp);
                    float I1 = (float)get_gray(curr, xc, yc);
                    float It = I1 - I0;
                    float gx = Ix[yc*WIDTH + xc];
                    float gy = Iy[yc*WIDTH + xc];
                    Gxx += gx*gx; Gxy += gx*gy; Gyy += gy*gy;
                    bx += gx*It;  by += gy*It;
                }
                if (!ok) break;
            }
            if (!ok) break;
            float det = Gxx*Gyy - Gxy*Gxy;
            if (det < 1e-6f) { ok = false; break; }
            float invGxx =  Gyy / det;
            float invGxy = -Gxy / det;
            float invGyy =  Gxx / det;
            float ddx = invGxx*bx + invGxy*by;
            float ddy = invGxy*bx + invGyy*by;
            dx += ddx; dy += ddy;
            if (fabsf(ddx) < KLT_EPS && fabsf(ddy) < KLT_EPS) break;
        }
        if (ok) { pts[k].x = x + dx; pts[k].y = y + dy; pts[k].valid = true; }
        else { pts[k].valid = false; }
    }

    free(Ix); free(Iy);
}

// --------- Kalman (constant velocity, 2D) ----------
static void kalman_init(KalmanCV* kf, float x, float y) {
    kf->x = x; kf->y = y; kf->vx = 0.0f; kf->vy = 0.0f;
    memset(kf->P, 0, sizeof(kf->P));
    kf->P[0]=kf->P[5]=kf->P[10]=kf->P[15]=1000.0f;
    kf->Q_pos = 1.0f;
    kf->R_meas = 25.0f;
}

static void kalman_predict(KalmanCV* kf, float dt) {
    kf->x += kf->vx * dt;
    kf->y += kf->vy * dt;

    // propagate covariance (approximate CV model)
    // P00 += 2*dt*P02 + dt^2*P22 + Q; similar for y
    float Pn[16];
    float P00 = kf->P[0] + 2*dt*kf->P[2] + dt*dt*kf->P[10];
    float P01 = kf->P[1] + dt*(kf->P[3] + kf->P[11]);
    float P02 = kf->P[2] + dt*kf->P[10];
    float P03 = kf->P[3] + dt*kf->P[11];

    float P10 = kf->P[4] + dt*(kf->P[6] + kf->P[14]);
    float P11 = kf->P[5] + 2*dt*kf->P[7] + dt*dt*kf->P[15];
    float P12 = kf->P[6] + dt*kf->P[14];
    float P13 = kf->P[7] + dt*kf->P[15];

    float P20 = kf->P[8] + dt*kf->P[0];
    float P21 = kf->P[9] + dt*kf->P[5];
    float P22 = kf->P[10];
    float P23 = kf->P[11];

    float P30 = kf->P[12] + dt*kf->P[0];
    float P31 = kf->P[13] + dt*kf->P[5];
    float P32 = kf->P[14];
    float P33 = kf->P[15];

    Pn[0]=P00; Pn[1]=P01; Pn[2]=P02; Pn[3]=P03;
    Pn[4]=P10; Pn[5]=P11; Pn[6]=P12; Pn[7]=P13;
    Pn[8]=P20; Pn[9]=P21; Pn[10]=P22;Pn[11]=P23;
    Pn[12]=P30;Pn[13]=P31;Pn[14]=P32;Pn[15]=P33;
    memcpy(kf->P, Pn, sizeof(Pn));

    kf->P[0] += kf->Q_pos;
    kf->P[5] += kf->Q_pos;
}

static void kalman_update(KalmanCV* kf, float mx, float my) {
    // z = [x,y], H = I on position
    float yx = mx - kf->x;
    float yy = my - kf->y;
    float Sx = kf->P[0] + kf->R_meas;
    float Sy = kf->P[5] + kf->R_meas;

    float K00 = kf->P[0] / Sx;
    float K10 = kf->P[4] / Sx;
    float K20 = kf->P[8] / Sx;
    float K30 = kf->P[12]/ Sx;

    float K01 = kf->P[1] / Sy;
    float K11 = kf->P[5] / Sy;
    float K21 = kf->P[9] / Sy;
    float K31 = kf->P[13]/ Sy;

    kf->x  += K00*yx + K01*yy;
    kf->y  += K10*yx + K11*yy;
    kf->vx += K20*yx + K21*yy;
    kf->vy += K30*yx + K31*yy;

    kf->P[0] *= (1.0f - K00);
    kf->P[5] *= (1.0f - K11);
}

// --------- NCC Template Matching ----------
static void template_compute_stats(Template* T) {
    double sum = 0.0, sum2 = 0.0;
    int N = T->w * T->h;
    for (int i=0;i<N;i++){ double v = (double)T->data[i]; sum += v; sum2 += v*v; }
    T->mean = (float)(sum / N);
    double var = (sum2 / N) - (T->mean * T->mean);
    T->std = (float)((var > 1e-6) ? sqrt(var) : 1.0);
}

static void template_from_roi(Template* T, uint8_t* gray, Rect roi) {
    T->w = roi.w; T->h = roi.h;
    if (T->w > TEMPLATE_MAX_W) T->w = TEMPLATE_MAX_W;
    if (T->h > TEMPLATE_MAX_H) T->h = TEMPLATE_MAX_H;
    if (T->data) free(T->data);
    T->data = (uint8_t*)malloc(T->w*T->h);
    for (int j=0;j<T->h;j++){
        int y = clampi(roi.y + j, 0, HEIGHT-1);
        for (int i=0;i<T->w;i++){
            int x = clampi(roi.x + i, 0, WIDTH-1);
            T->data[j*T->w + i] = gray[y*WIDTH + x];
        }
    }
    template_compute_stats(T);
}

// Compute NCC score for image patch at (x,y) top-left with size T->w x T->h
static float ncc_score(uint8_t* gray, Template* T, int x, int y) {
    double sum=0.0, sum2=0.0, cross=0.0;
    int w=T->w, h=T->h;
    for (int j=0;j<h;j++){
        int yy = y + j; if (yy < 0 || yy >= HEIGHT) return -1.0f;
        for (int i=0;i<w;i++){
            int xx = x + i; if (xx < 0 || xx >= WIDTH) return -1.0f;
            double v = (double)gray[yy*WIDTH + xx];
            sum += v; sum2 += v*v;
            double tv = (double)T->data[j*w + i];
            cross += (v) * (tv);
        }
    }
    int N = w*h;
    double mean = sum / N;
    double var = (sum2 / N) - mean*mean;
    double std = (var > 1e-6) ? sqrt(var) : 1.0;
    // NCC = sum((I-mean_I)(T-mean_T)) / (N * std_I * std_T)
    // expand cross term: sum(I*T) - N*mean_I*mean_T
    double numerator = cross - (double)N * mean * T->mean;
    double denom = ((double)N) * std * T->std;
    if (denom <= 1e-9) return -1.0f;
    double ncc = numerator / denom;
    return (float)ncc;
}

static bool ncc_redetect(uint8_t* gray, Template* T, Rect* out_roi, int cx_pred, int cy_pred) {
    int w = T->w, h = T->h;
    int best_x = cx_pred - w/2;
    int best_y = cy_pred - h/2;
    float best = -2.0f;

    int xmin = clampi(cx_pred - SEARCH_MARGIN - w/2, 0, WIDTH - w);
    int xmax = clampi(cx_pred + SEARCH_MARGIN - w/2, 0, WIDTH - w);
    int ymin = clampi(cy_pred - SEARCH_MARGIN - h/2, 0, HEIGHT - h);
    int ymax = clampi(cy_pred + SEARCH_MARGIN - h/2, 0, HEIGHT - h);

    // stride for speed
    int step = 4;
    for (int y = ymin; y <= ymax; y += step) {
        for (int x = xmin; x <= xmax; x += step) {
            float s = ncc_score(gray, T, x, y);
            if (s > best) { best = s; best_x = x; best_y = y; }
        }
    }
    if (best < 0.2f) return false; // score too low -> unreliable
    out_roi->x = best_x; out_roi->y = best_y; out_roi->w = w; out_roi->h = h;
    return true;
}

// --------- Initial detection via gradient energy (no cascade) ----------
static bool detect_initial(uint8_t* gray, Rect* roi_out) {
    // Build a coarse gradient energy map and find the window with max energy.
    int winW = WIDTH/4;
    int winH = HEIGHT/3;
    // clamp to max template bounds
    if (winW > TEMPLATE_MAX_W) winW = TEMPLATE_MAX_W;
    if (winH > TEMPLATE_MAX_H) winH = TEMPLATE_MAX_H;

    float* Ix = (float*)malloc(WIDTH*HEIGHT*sizeof(float));
    float* Iy = (float*)malloc(WIDTH*HEIGHT*sizeof(float));
    compute_gradients(gray, Ix, Iy);

    int step = 8;
    float bestE = -1.0f; int bestX = WIDTH/2 - winW/2; int bestY = HEIGHT/2 - winH/2;
    for (int y = 0; y <= HEIGHT - winH; y += step) {
        for (int x = 0; x <= WIDTH - winW; x += step) {
            double E = 0.0;
            for (int j=0;j<winH;j+=4) {
                int yy = y + j;
                for (int i=0;i<winW;i+=4) {
                    int xx = x + i;
                    float gx = Ix[yy*WIDTH + xx];
                    float gy = Iy[yy*WIDTH + xx];
                    E += gx*gx + gy*gy;
                }
            }
            if (E > bestE) { bestE = (float)E; bestX = x; bestY = y; }
        }
    }
    free(Ix); free(Iy);
    roi_out->x = bestX; roi_out->y = bestY; roi_out->w = winW; roi_out->h = winH;
    return true;
}

// --------- Centroid ----------
static bool points_centroid(const Point2f* pts, int n, float* cx, float* cy, int* valid_count) {
    float sx=0, sy=0; int c=0;
    for (int i=0;i<n;i++) if (pts[i].valid) { sx += pts[i].x; sy += pts[i].y; c++; }
    if (valid_count) *valid_count = c;
    if (c == 0) return false;
    *cx = sx / c; *cy = sy / c;
    return true;
}

// --------- Drawing ----------
static void draw_rect(uint8_t* rgb, Rect r, uint8_t R, uint8_t G, uint8_t B) {
    for (int x = r.x; x < r.x + r.w; x++) {
        int y1 = r.y, y2 = r.y + r.h - 1;
        if (x>=0 && x<WIDTH) {
            if (y1>=0 && y1<HEIGHT) { int idx = (y1*WIDTH + x)*3; rgb[idx]=R; rgb[idx+1]=G; rgb[idx+2]=B; }
            if (y2>=0 && y2<HEIGHT) { int idx = (y2*WIDTH + x)*3; rgb[idx]=R; rgb[idx+1]=G; rgb[idx+2]=B; }
        }
    }
    for (int y = r.y; y < r.y + r.h; y++) {
        int x1 = r.x, x2 = r.x + r.w - 1;
        if (y>=0 && y<HEIGHT) {
            if (x1>=0 && x1<WIDTH) { int idx = (y*WIDTH + x1)*3; rgb[idx]=R; rgb[idx+1]=G; rgb[idx+2]=B; }
            if (x2>=0 && x2<WIDTH) { int idx = (y*WIDTH + x2)*3; rgb[idx]=R; rgb[idx+1]=G; rgb[idx+2]=B; }
        }
    }
}

static void draw_points(uint8_t* rgb, const Point2f* pts, int n, uint8_t R, uint8_t G, uint8_t B) {
    for (int i=0;i<n;i++) if (pts[i].valid) {
        int x = (int)pts[i].x, y = (int)pts[i].y;
        for (int dy=-1;dy<=1;dy++) for(int dx=-1;dx<=1;dx++){
            int xx = x+dx, yy=y+dy;
            if (xx>=0 && xx<WIDTH && yy>=0 && yy<HEIGHT) {
                int idx = (yy*WIDTH + xx)*3;
                rgb[idx]=R; rgb[idx+1]=G; rgb[idx+2]=B;
            }
        }
    }
}

// --------- Main ----------
int main(int argc, char** argv) {
    const char* dev = "/dev/video0";

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1;
    }
    SDL_Window* win = SDL_CreateWindow("KLT + Kalman + NCC (v4l2/SDL2, pure C)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture* tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    V4L2Ctx cam = {0};
    v4l2_open(&cam, dev);

    uint8_t* rgb = (uint8_t*)malloc(WIDTH*HEIGHT*3);
    uint8_t* gray = (uint8_t*)malloc(WIDTH*HEIGHT);
    uint8_t* prev_gray = (uint8_t*)malloc(WIDTH*HEIGHT);

    Rect roi = { WIDTH/4, HEIGHT/4, WIDTH/2, HEIGHT/2 };
    Point2f pts[MAX_POINTS]; int npts = 0;
    KalmanCV kf; kalman_init(&kf, WIDTH/2.0f, HEIGHT/2.0f);

    Template templ = {0}; templ.data = NULL;

    bool has_init = false;
    int frame_count = 0;
    uint64_t last_ms = now_ms();

    bool running = true;
    while (running) {
        SDL_Event e; while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        void* data = NULL; size_t len = 0;
        if (!v4l2_dequeue(&cam, &data, &len)) { SDL_Delay(1); continue; }

        yuyv_to_rgb((uint8_t*)data, rgb);
        yuyv_to_gray((uint8_t*)data, gray);

        if (!has_init) {
            detect_initial(gray, &roi);
            template_from_roi(&templ, gray, roi);
            npts = init_klt_points(gray, roi, pts, MAX_POINTS);
            float cx = roi.x + roi.w/2.0f, cy = roi.y + roi.h/2.0f;
            kalman_init(&kf, cx, cy);
            has_init = true;
            memcpy(prev_gray, gray, WIDTH*HEIGHT);
        } else {
            // Track points
            klt_track_points(prev_gray, gray, pts, npts);

            float cx, cy; int valid;
            bool ok = points_centroid(pts, npts, &cx, &cy, &valid);

            uint64_t now = now_ms();
            float dt = (float)(now - last_ms) / 1000.0f;
            if (dt < 0.0001f) dt = 0.016f;
            last_ms = now;

            kalman_predict(&kf, dt);
            if (ok) kalman_update(&kf, cx, cy);

            // Update ROI center from Kalman
            float kx = kf.x, ky = kf.y;
            roi.x = (int)(kx - roi.w/2);
            roi.y = (int)(ky - roi.h/2);

            // Re-Detection: too few points or periodic
            bool need_redetect = (valid < REDETECT_MIN_POINTS) || (frame_count % REDETECT_PERIOD_FRAMES == 0);
            if (need_redetect && templ.data) {
                Rect new_roi;
                int pred_cx = (int)kx;
                int pred_cy = (int)ky;
                if (ncc_redetect(gray, &templ, &new_roi, pred_cx, pred_cy)) {
                    roi = new_roi;
                    // refresh template occasionally
                    template_from_roi(&templ, gray, roi);
                    npts = init_klt_points(gray, roi, pts, MAX_POINTS);
                    float cx0 = roi.x + roi.w/2.0f, cy0 = roi.y + roi.h/2.0f;
                    kalman_init(&kf, cx0, cy0);
                } else {
                    // fallback: if no NCC match, shrink search threshold next time
                }
            }

            memcpy(prev_gray, gray, WIDTH*HEIGHT);
        }

        // Draw overlays
        draw_rect(rgb, roi, 255, 0, 0);
        draw_points(rgb, pts, npts, 0, 255, 0);

        SDL_UpdateTexture(tex, NULL, rgb, WIDTH*3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        v4l2_enqueue(&cam);
        frame_count++;
    }

    v4l2_close(&cam);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    if (templ.data) free(templ.data);
    free(rgb); free(gray); free(prev_gray);
    return 0;
}

