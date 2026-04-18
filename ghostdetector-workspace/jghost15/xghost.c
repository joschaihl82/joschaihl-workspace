// ghost.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------- Config ----------
#define UDP_PORT 5050
#define MAX_HOT 16
#define DEFAULT_W 640
#define DEFAULT_H 480
#define MAX_THREADS 16
#define MASK_DOWNSAMPLE 4   // compute mask at 1/4 resolution (both dims)
#define ATAN_LUT_SIZE 16384 // must be power of two for fast indexing
#define ATAN_LUT_MAX_IN 2.0f // domain mapped to [-ATAN_LUT_MAX_IN, +ATAN_LUT_MAX_IN]

// ---------- Shared types ----------
typedef struct {
    double ambient_uSv;
    int n_hot;
    struct { float x,y; double uSv; } hot[MAX_HOT];
    pthread_mutex_t mtx;
} RadShared;

static RadShared gRad;

typedef struct { void *start; size_t length; } V4LBuf;

// Tile job for workers
typedef struct {
    int x0, y0, w, h;
    int W, H;
    uint8_t *rgb_in;   // in: RGB24 (contiguous)
    uint8_t *rgb_out;  // out: RGB24
    float *V_low;      // low-res Value (shared read-only)
    float *grad_low;   // low-res grad mask (shared read-only)
    float hue_gain, sat_gain;
    float ambientHueDeg, ambientSBoost, ambientAlpha;
    struct { float x,y,s; } hotspots[MAX_HOT];
    int hotc;
    float *atan_lut; int atan_lut_size; float atan_scale;
} TileJob;

// ---------- Global state ----------
static int gW = DEFAULT_W, gH = DEFAULT_H;
static V4LBuf *g_vbufs = NULL; static unsigned int g_vcount = 0; static int g_vfd = -1;
static uint8_t *g_rgb_in = NULL; static uint8_t *g_rgb_out = NULL;
static float *g_V_low = NULL;   // downsampled Value
static float *g_grad_low = NULL; // downsampled grad
static int g_mask_w, g_mask_h;
static pthread_t g_workers[MAX_THREADS];
static int g_worker_count = 0;
static pthread_mutex_t g_job_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_job_cond = PTHREAD_COND_INITIALIZER;
static TileJob *g_jobs = NULL;
static int g_jobs_total = 0;
static int g_jobs_done = 0;
static bool g_stop = false;

// precomputed gaussian kernel (1D)
static float *g_gauss_kernel = NULL; static int g_gauss_radius = 0;

// atan LUT
static float *g_atan_lut = NULL; static int g_atan_lut_size = 0; static float g_atan_lut_scale = 1.0f;

// ---------- Helpers ----------
static double now_sec(void){ struct timespec ts; clock_gettime(CLOCK_REALTIME,&ts); return ts.tv_sec + ts.tv_nsec*1e-9; }
static void die(const char *s){ perror(s); exit(1); }

// ---------- UDP listener (simple CSV) ----------
static void *udp_listener(void *arg){
    RadShared *rs = &gRad;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket udp"); return NULL; }
    struct sockaddr_in addr; memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET; addr.sin_port = htons(UDP_PORT); addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind udp"); close(sock); return NULL; }
    char buf[2048];
    while (!g_stop){
        ssize_t n = recv(sock, buf, sizeof(buf)-1, 0);
        if (n <= 0) { usleep(10000); continue; }
        buf[n]=0;
        double ambient=0.0; int hotc=0; int consumed=0;
        if (sscanf(buf, "%lf,%d%n", &ambient, &hotc, &consumed) >= 1){
            char *p = buf + consumed;
            pthread_mutex_lock(&rs->mtx);
            rs->ambient_uSv = ambient;
            rs->n_hot = 0;
            char *tok = strtok(p, ",");
            while (tok && rs->n_hot < MAX_HOT){
                double a = atof(tok); tok = strtok(NULL,","); if(!tok)break;
                double b = atof(tok); tok = strtok(NULL,","); if(!tok)break;
                double c = atof(tok); tok = strtok(NULL,",");
                int i = rs->n_hot++;
                rs->hot[i].x = (float)a; rs->hot[i].y = (float)b; rs->hot[i].uSv = c;
            }
            pthread_mutex_unlock(&rs->mtx);
        }
    }
    close(sock); return NULL;
}

// ---------- V4L2 capture (mmap) ----------
static int v4l_open(const char *dev, int *W, int *H, V4LBuf **out_bufs, unsigned int *out_count){
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) return -1;
    struct v4l2_capability cap; if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0){ close(fd); return -1; }
    struct v4l2_format fmt; memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = *W; fmt.fmt.pix.height = *H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0){ close(fd); return -1; }
    *W = fmt.fmt.pix.width; *H = fmt.fmt.pix.height;
    struct v4l2_requestbuffers req; memset(&req,0,sizeof(req));
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0){ close(fd); return -1; }
    V4LBuf *bufs = calloc(req.count, sizeof(V4LBuf));
    for (unsigned int i=0;i<req.count;i++){
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0){ close(fd); return -1; }
        bufs[i].length = buf.length;
        bufs[i].start = mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (bufs[i].start == MAP_FAILED){ close(fd); return -1; }
    }
    for (unsigned int i=0;i<req.count;i++){
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0){ close(fd); return -1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0){ close(fd); return -1; }
    *out_bufs = bufs; *out_count = req.count;
    return fd;
}

// Fast integer YUYV -> RGB24 row
static inline void yuyv_to_rgb24_row(const uint8_t *yuyv, int W, uint8_t *rgb){
    for (int x=0;x<W;x+=2){
        int y0 = yuyv[0], u = yuyv[1], y1 = yuyv[2], v = yuyv[3];
        yuyv += 4;
        int uu = u - 128, vv = v - 128;
        int r0 = y0 + ((1402*vv)>>10);
        int g0 = y0 - ((344*uu + 714*vv)>>10);
        int b0 = y0 + ((1772*uu)>>10);
        int r1 = y1 + ((1402*vv)>>10);
        int g1 = y1 - ((344*uu + 714*vv)>>10);
        int b1 = y1 + ((1772*uu)>>10);
        rgb[0] = (uint8_t)(r0<0?0:(r0>255?255:r0)); rgb[1] = (uint8_t)(g0<0?0:(g0>255?255:g0)); rgb[2] = (uint8_t)(b0<0?0:(b0>255?255:b0));
        rgb[3] = (uint8_t)(r1<0?0:(r1>255?255:r1)); rgb[4] = (uint8_t)(g1<0?0:(g1>255?255:g1)); rgb[5] = (uint8_t)(b1<0?0:(b1>255?255:b1));
        rgb += 6;
    }
}

// ---------- HSV helpers ----------
typedef struct { float h,s,v; } HSVf;
static inline HSVf rgb_to_hsvf(uint8_t r, uint8_t g, uint8_t b){
    float rf = r/255.0f, gf = g/255.0f, bf = b/255.0f;
    float mx = rf>gf? (rf>bf?rf:bf) : (gf>bf?gf:bf);
    float mn = rf<gf? (rf<bf?rf:bf) : (gf<bf?gf:bf);
    float d = mx - mn;
    HSVf out; out.v = mx; out.s = (mx == 0.0f) ? 0.0f : d / mx; out.h = 0.0f;
    if (d > 1e-6f) {
        float hh;
        if (mx == rf) hh = fmodf(((gf - bf) / d), 6.0f);
        else if (mx == gf) hh = ((bf - rf) / d) + 2.0f;
        else hh = ((rf - gf) / d) + 4.0f;
        hh *= 60.0f;
        if (hh < 0.0f) hh += 360.0f;
        out.h = hh;
    }
    return out;
}
static inline void hsvf_to_rgb(const HSVf *hsv, uint8_t *r, uint8_t *g, uint8_t *b){
    float h = hsv->h, s = hsv->s, v = hsv->v;
    if (s <= 1e-6f) {
        uint8_t val = (uint8_t) (fminf(255.0f, fmaxf(0.0f, roundf(v*255.0f))));
        *r = *g = *b = val; return;
    }
    float C = v * s;
    float hh = h / 60.0f;
    float X = C * (1.0f - fabsf(fmodf(hh,2.0f) - 1.0f));
    float m = v - C;
    float rf=0,gf=0,bf=0;
    if (hh < 1.0f) { rf=C; gf=X; bf=0; }
    else if (hh < 2.0f) { rf=X; gf=C; bf=0; }
    else if (hh < 3.0f) { rf=0; gf=C; bf=X; }
    else if (hh < 4.0f) { rf=0; gf=X; bf=C; }
    else if (hh < 5.0f) { rf=X; gf=0; bf=C; }
    else { rf=C; gf=0; bf=X; }
    *r = (uint8_t) (fminf(255.0f, fmaxf(0.0f, roundf((rf + m) * 255.0f))));
    *g = (uint8_t) (fminf(255.0f, fmaxf(0.0f, roundf((gf + m) * 255.0f))));
    *b = (uint8_t) (fminf(255.0f, fmaxf(0.0f, roundf((bf + m) * 255.0f))));
}

// ---------- Gaussian kernel (precompute) ----------
static void precompute_gauss(float sigma){
    g_gauss_radius = (int)ceilf(3.0f * sigma);
    int len = g_gauss_radius*2 + 1;
    g_gauss_kernel = malloc(len * sizeof(float));
    float sum = 0.0f;
    for (int i=-g_gauss_radius; i<=g_gauss_radius; ++i){
        float v = expf(-(i*i)/(2.0f*sigma*sigma));
        g_gauss_kernel[i + g_gauss_radius] = v; sum += v;
    }
    for (int i=0;i<len;i++) g_gauss_kernel[i] /= sum;
}

// ---------- Separable gaussian on low-res Value (in place) ----------
static void separable_gauss_on_lowres(const float *in, float *out, int Wl, int Hl){
    int r = g_gauss_radius;
    int len = 2*r+1;
    float *tmp = malloc(sizeof(float)*Wl*Hl);
    // horizontal
    for (int y=0;y<Hl;y++){
        for (int x=0;x<Wl;x++){
            float acc = 0.0f;
            for (int k=-r;k<=r;k++){
                int xx = x + k; if (xx<0) xx=0; if (xx>=Wl) xx = Wl-1;
                acc += g_gauss_kernel[k+r] * in[y*Wl + xx];
            }
            tmp[y*Wl + x] = acc;
        }
    }
    // vertical
    for (int x=0;x<Wl;x++){
        for (int y=0;y<Hl;y++){
            float acc = 0.0f;
            for (int k=-r;k<=r;k++){
                int yy = y + k; if (yy<0) yy=0; if (yy>=Hl) yy = Hl-1;
                acc += g_gauss_kernel[k+r] * tmp[yy*Wl + x];
            }
            out[y*Wl + x] = acc;
        }
    }
    free(tmp);
}

// ---------- Sobel on low-res Value ----------
static void sobel_on_lowres(const float *V, int Wl, int Hl, float *grad_out, float *tex_avg){
    float sum = 0.0f;
    for (int y=1;y<Hl-1;y++){
        for (int x=1;x<Wl-1;x++){
            int i = y*Wl + x;
            float v00 = V[(y-1)*Wl + (x-1)], v01 = V[(y-1)*Wl + x], v02 = V[(y-1)*Wl + (x+1)];
            float v10 = V[y*Wl + (x-1)],       v11 = V[i],           v12 = V[y*Wl + (x+1)];
            float v20 = V[(y+1)*Wl + (x-1)],   v21 = V[(y+1)*Wl + x], v22 = V[(y+1)*Wl + (x+1)];
            float gx = (-v00 + v02) + (-2.0f*v10 + 2.0f*v12) + (-v20 + v22);
            float gy = (-v00 - 2.0f*v01 - v02) + (v20 + 2.0f*v21 + v22);
            float g = sqrtf(gx*gx + gy*gy);
            float gn = fminf(1.0f, g * 0.5f);
            grad_out[i] = gn;
            sum += gn;
        }
    }
    *tex_avg = sum / (float)((Wl-2)*(Hl-2));
}

// ---------- Atan LUT ----------
static void precompute_atan_lut(int size, float max_in){
    g_atan_lut_size = size;
    g_atan_lut = malloc(sizeof(float)*size);
    for (int i=0;i<size;i++){
        float x = ((float)i / (float)(size-1)) * 2.0f * max_in - max_in; // -max_in..+max_in
        g_atan_lut[i] = atanf(x);
    }
    g_atan_lut_scale = (size-1) / (2.0f * max_in);
}
static inline float fast_atan(float x){
    // clamp
    if (x <= -ATAN_LUT_MAX_IN) return g_atan_lut[0];
    if (x >= ATAN_LUT_MAX_IN) return g_atan_lut[g_atan_lut_size-1];
    int idx = (int)floorf((x + ATAN_LUT_MAX_IN) * g_atan_lut_scale + 0.5f);
    if (idx < 0) idx = 0; if (idx >= g_atan_lut_size) idx = g_atan_lut_size-1;
    return g_atan_lut[idx];
}

// ---------- Tile worker ----------
static void *worker_thread(void *arg){
    int id = (intptr_t)arg;
    while (!g_stop){
        pthread_mutex_lock(&g_job_mtx);
        while (g_jobs_done >= g_jobs_total && !g_stop) pthread_cond_wait(&g_job_cond, &g_job_mtx);
        if (g_stop){ pthread_mutex_unlock(&g_job_mtx); break; }
        // fetch job if available
        TileJob job = g_jobs[g_jobs_done]; // read-only copy
        g_jobs_done++;
        pthread_mutex_unlock(&g_job_mtx);

        // process job tile
        int x0 = job.x0, y0 = job.y0, w = job.w, h = job.h;
        int W = job.W, H = job.H;
        for (int yy = y0; yy < y0 + h; ++yy){
            for (int xx = x0; xx < x0 + w; ++xx){
                int i = yy*W + xx;
                // load RGB
                int idx = i*3;
                uint8_t R = job.rgb_in[idx+0], G = job.rgb_in[idx+1], B = job.rgb_in[idx+2];
                HSVf px = rgb_to_hsvf(R,G,B);
                // compute comb (low-cost sin/cosh-ish shaping)
                float nx = (float)xx / (float)W, ny = (float)yy / (float)H;
                float comb = sinf(2.0f*M_PI*(1.2f*nx + 0.6f*job.hue_gain*0.05f)) + 0.6f * sinf(2.0f*M_PI*(0.8f*ny + 0.3f*job.hue_gain*0.03f));
                float tc = tanhf(0.6f * comb) * 1.2f;
                float mixc = 0.7f * comb + 0.3f * tc;
                // sample low-res grad (bilinear from low-res arrays)
                float fx = (float)xx / (float)W * (float)(g_mask_w - 1);
                float fy = (float)yy / (float)H * (float)(g_mask_h - 1);
                int lx = (int)fx, ly = (int)fy;
                float tx = fx - lx, ty = fy - ly;
                int lx1 = (lx+1<g_mask_w)?lx+1:lx;
                int ly1 = (ly+1<g_mask_h)?ly+1:ly;
                float v00 = job.V_low[ly*g_mask_w + lx];
                float v10 = job.V_low[ly*g_mask_w + lx1];
                float v01 = job.V_low[ly1*g_mask_w + lx];
                float v11 = job.V_low[ly1*g_mask_w + lx1];
                float Vb = (1-tx)*(1-ty)*v00 + tx*(1-ty)*v10 + (1-tx)*ty*v01 + tx*ty*v11;
                float g00 = job.grad_low[ly*g_mask_w + lx];
                float g10 = job.grad_low[ly*g_mask_w + lx1];
                float g01 = job.grad_low[ly1*g_mask_w + lx];
                float g11 = job.grad_low[ly1*g_mask_w + lx1];
                float grad = (1-tx)*(1-ty)*g00 + tx*(1-ty)*g10 + (1-tx)*ty*g01 + tx*ty*g11;
                // hue and sat modulation
                float hueShift = job.hue_gain * fast_atan(mixc); // degrees
                float newh = px.h + hueShift; if (newh < 0.0f) newh += 360.0f; if (newh >= 360.0f) newh -= 360.0f;
                float s_boost = job.sat_gain * mixc * grad;
                float news = px.s + s_boost; if (news > 1.0f) news = 1.0f;
                px.h = newh; px.s = news;
                // ambient tint blend (preserve V)
                float tint_alpha = job.ambientAlpha * grad;
                float dh = job.ambientHueDeg - px.h; if (dh > 180.0f) dh -= 360.0f; if (dh < -180.0f) dh += 360.0f;
                px.h = px.h + dh * tint_alpha;
                if (px.h < 0.0f) px.h += 360.0f; if (px.h >= 360.0f) px.h -= 360.0f;
                px.s = fminf(1.0f, px.s + job.ambientSBoost * tint_alpha);
                // hotspots
                for (int hi=0; hi<job.hotc; ++hi){
                    float cx = job.hotspots[hi].x, cy = job.hotspots[hi].y; float strength = job.hotspots[hi].s;
                    double dx = (xx / (double)W) - cx, dy = (yy / (double)H) - cy;
                    double r2 = dx*dx + dy*dy;
                    double radius = 0.12 + 0.08 * fmin(1.0, log1p(strength));
                    double fall = exp(-r2 / (radius*radius));
                    float a_hot = (float)(0.6 * fall * fmin(1.0, strength/5.0));
                    if (a_hot > 1e-5f){
                        float dh2 = job.ambientHueDeg - px.h; if (dh2 > 180.0f) dh2 -= 360.0f; if (dh2 < -180.0f) dh2 += 360.0f;
                        px.h = px.h + dh2 * a_hot; if (px.h < 0.0f) px.h += 360.0f; if (px.h >= 360.0f) px.h -= 360.0f;
                        px.s = fminf(1.0f, px.s + job.ambientSBoost * a_hot);
                    }
                }
                // preserve original V (we have Vb ~ smoothed value but requirement is to not change output brightness)
                // Original V is approximated from input rgb to HSV conversion earlier.
                // hsvf_to_rgb will use px.v which we set to original input value
                uint8_t r8,g8,b8;
                hsvf_to_rgb(&px, &r8, &g8, &b8);
                int outidx = idx;
                job.rgb_out[outidx+0] = r8; job.rgb_out[outidx+1] = g8; job.rgb_out[outidx+2] = b8;
            }
        }
        // job finished; loop to next
    }
    return NULL;
}

// ---------- Build jobs (tiling) ----------
static void build_jobs_for_frame(int W, int H, int tile_w, int tile_h, float hue_gain, float sat_gain,
                                 float ambientHueDeg, float ambientSBoost, float ambientAlpha, int hotc){
    // prepare job array once per frame; g_jobs_total and g_jobs_done used for synchronization
    int nx = (W + tile_w - 1) / tile_w;
    int ny = (H + tile_h - 1) / tile_h;
    int total = nx * ny;
    // reuse buffer if not large enough
    if (!g_jobs || total > g_jobs_total) {
        free(g_jobs);
        g_jobs = calloc(total, sizeof(TileJob));
    }
    g_jobs_total = total;
    g_jobs_done = 0;
    int j = 0;
    for (int ty=0; ty<ny; ++ty){
        for (int tx=0; tx<nx; ++tx){
            int x0 = tx*tile_w; int y0 = ty*tile_h;
            int w = ((x0 + tile_w) > W) ? (W - x0) : tile_w;
            int h = ((y0 + tile_h) > H) ? (H - y0) : tile_h;
            g_jobs[j].x0 = x0; g_jobs[j].y0 = y0; g_jobs[j].w = w; g_jobs[j].h = h;
            g_jobs[j].W = W; g_jobs[j].H = H;
            g_jobs[j].rgb_in = g_rgb_in; g_jobs[j].rgb_out = g_rgb_out;
            g_jobs[j].V_low = g_V_low; g_jobs[j].grad_low = g_grad_low;
            g_jobs[j].hue_gain = hue_gain; g_jobs[j].sat_gain = sat_gain;
            g_jobs[j].ambientHueDeg = ambientHueDeg; g_jobs[j].ambientSBoost = ambientSBoost; g_jobs[j].ambientAlpha = ambientAlpha;
            // copy hotspots under lock
            pthread_mutex_lock(&gRad.mtx);
            int copyc = (hotc < gRad.n_hot) ? hotc : gRad.n_hot;
            for (int hi=0; hi<copyc; ++hi){
                g_jobs[j].hotspots[hi].x = gRad.hot[hi].x;
                g_jobs[j].hotspots[hi].y = gRad.hot[hi].y;
                g_jobs[j].hotspots[hi].s = (float)gRad.hot[hi].uSv;
            }
            g_jobs[j].hotc = copyc;
            pthread_mutex_unlock(&gRad.mtx);
            g_jobs[j].atan_lut = g_atan_lut; g_jobs[j].atan_lut_size = g_atan_lut_size; g_jobs[j].atan_scale = g_atan_lut_scale;
            ++j;
        }
    }
}

// ---------- Main ----------
int main(int argc, char **argv){
    const char *dev = "/dev/video0";
    int W = DEFAULT_W, H = DEFAULT_H;
    if (argc >= 3){ W = atoi(argv[1]); H = atoi(argv[2]); }
    gW = W; gH = H;

    // init rad shared
    memset(&gRad, 0, sizeof(gRad)); pthread_mutex_init(&gRad.mtx, NULL);

    // start UDP listener
    pthread_t udp_th; if (pthread_create(&udp_th, NULL, udp_listener, NULL) != 0) die("pthread_create udp");

    // open v4l2
    if ((g_vfd = v4l_open(dev, &W, &H, &g_vbufs, &g_vcount)) < 0) die("v4l_open");
    // allocate persistent RGB buffers
    size_t rgb_bytes = W * H * 3;
    g_rgb_in = malloc(rgb_bytes); g_rgb_out = malloc(rgb_bytes);
    if (!g_rgb_in || !g_rgb_out) die("malloc rgb");

    // precompute low-res mask dims
    g_mask_w = (W + MASK_DOWNSAMPLE - 1) / MASK_DOWNSAMPLE;
    g_mask_h = (H + MASK_DOWNSAMPLE - 1) / MASK_DOWNSAMPLE;
    g_V_low = calloc(g_mask_w * g_mask_h, sizeof(float));
    g_grad_low = calloc(g_mask_w * g_mask_h, sizeof(float));
    if (!g_V_low || !g_grad_low) die("malloc lowres");

    // precompute gaussian kernel (sigma tuned)
    precompute_gauss(1.6f);

    // precompute atan LUT
    precompute_atan_lut(ATAN_LUT_SIZE, ATAN_LUT_MAX_IN);

    // create worker threads
    int hw_cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
    g_worker_count = hw_cores > 0 ? hw_cores : 4;
    if (g_worker_count > MAX_THREADS) g_worker_count = MAX_THREADS;
    for (int i=0;i<g_worker_count;i++){
        if (pthread_create(&g_workers[i], NULL, worker_thread, (void*)(intptr_t)i) != 0) die("pthread_create worker");
    }

    // X11 setup
    Display *dpy = XOpenDisplay(NULL); if (!dpy) die("XOpenDisplay");
    int scr = DefaultScreen(dpy); Window root = RootWindow(dpy, scr);
    XSetWindowAttributes swa; swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;
    Window win = XCreateSimpleWindow(dpy, root, 100, 100, W, H, 1, BlackPixel(dpy,scr), WhitePixel(dpy,scr));
    XStoreName(dpy, win, "ghost (optimized CPU) - NightVision + Radio Overlay");
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win); GC gc = XCreateGC(dpy, win, 0, NULL);
    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, scr), 24, ZPixmap, 0, (char*)malloc(W*H*4), W, H, 32, W*4);
    if (!xi || !xi->data) die("XCreateImage");

    // Main capture+process loop
    double t = 0.0, dt = 1.0/60.0;
    int running = 1; XEvent ev;
    while (running){
        while (XPending(dpy)){
            XNextEvent(dpy, &ev);
            if (ev.type == KeyPress) running = 0;
        }
        // dequeue frame
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(g_vfd, VIDIOC_DQBUF, &buf) == 0){
            uint8_t *yuyv = (uint8_t*)g_vbufs[buf.index].start;
            // YUYV -> RGB24 (fast)
            for (int y=0;y<H;y++){
                yuyv_to_rgb24_row(yuyv + y*(W*2), W, g_rgb_in + y*(W*3));
            }

            // Build low-res Value image (downsample)
            for (int ly=0; ly<g_mask_h; ++ly){
                for (int lx=0; lx<g_mask_w; ++lx){
                    // box average over block
                    int sx = lx * MASK_DOWNSAMPLE, sy = ly * MASK_DOWNSAMPLE;
                    int ex = sx + MASK_DOWNSAMPLE; int ey = sy + MASK_DOWNSAMPLE;
                    if (ex > W) ex = W; if (ey > H) ey = H;
                    float sumv = 0.0f; int cnt = 0;
                    for (int yy = sy; yy < ey; ++yy){
                        for (int xx = sx; xx < ex; ++xx){
                            int idx = (yy*W + xx)*3;
                            // compute luminance ~ Value
                            float v = (0.299f * g_rgb_in[idx+0] + 0.587f * g_rgb_in[idx+1] + 0.114f * g_rgb_in[idx+2]) / 255.0f;
                            sumv += v; cnt++;
                        }
                    }
                    g_V_low[ly*g_mask_w + lx] = (cnt>0) ? (sumv / cnt) : 0.0f;
                }
            }

            // smooth low-res value with separable gaussian
            separable_gauss_on_lowres(g_V_low, g_V_low, g_mask_w, g_mask_h);

            // compute sobel on low-res value to get grad mask
            float tex_avg = 0.0f;
            sobel_on_lowres(g_V_low, g_mask_w, g_mask_h, g_grad_low, &tex_avg);

            // compute ESC (simple sinusoidal gains)
            float hue_base = 10.0f; float sat_base = 0.35f;
            float hue_amp = 2.0f; float sat_amp = 0.12f;
            float hue_gain = hue_base + hue_amp * sinf(0.9f * t);
            float sat_gain = sat_base + sat_amp * sinf(0.7f * t);

            // radiation overlay params
            float ambientHueDeg = 110.0f, ambientSBoost = 0.18f, ambientAlpha = 0.45f;
            pthread_mutex_lock(&gRad.mtx);
            double ambient = gRad.ambient_uSv;
            int hotc = gRad.n_hot;
            if (ambient > 0.0) {
                double L = log1p(ambient);
                ambientHueDeg = (float)(110.0 + 60.0 * fmin(1.0, L / 2.0));
                ambientSBoost = (float)(0.25 * fmin(1.0, L / 1.5));
                ambientAlpha = (float)(0.6 * fmin(1.0, L / 2.0));
            }
            // copy hotspots into job template area under lock
            struct { float x,y,s; } hotcpy[MAX_HOT];
            int copyc = hotc > MAX_HOT ? MAX_HOT : hotc;
            for (int i=0;i<copyc;i++){ hotcpy[i].x = gRad.hot[i].x; hotcpy[i].y = gRad.hot[i].y; hotcpy[i].s = (float)gRad.hot[i].uSv; }
            pthread_mutex_unlock(&gRad.mtx);

            // build tile jobs
            int tile_w = 64, tile_h = 64; // tuned tile size; can be tuned per CPU
            build_jobs_for_frame(W, H, tile_w, tile_h, hue_gain, sat_gain, ambientHueDeg, ambientSBoost, ambientAlpha, copyc);
            // fill hotspot copies into each job
            for (int j=0;j<g_jobs_total;j++){
                g_jobs[j].hotc = copyc;
                for (int hi=0; hi<copyc; ++hi){
                    g_jobs[j].hotspots[hi].x = hotcpy[hi].x; g_jobs[j].hotspots[hi].y = hotcpy[hi].y; g_jobs[j].hotspots[hi].s = hotcpy[hi].s;
                }
            }

            // signal workers that jobs are ready (g_jobs_done starts at 0)
            pthread_mutex_lock(&g_job_mtx);
            g_jobs_done = 0; // workers will read g_jobs from 0..g_jobs_total-1
            pthread_cond_broadcast(&g_job_cond);
            pthread_mutex_unlock(&g_job_mtx);

            // busy-wait for completion with small sleep; alternative: use a counter+cond (kept simple)
            while (1){
                pthread_mutex_lock(&g_job_mtx);
                int done = g_jobs_done;
                pthread_mutex_unlock(&g_job_mtx);
                if (done >= g_jobs_total) break;
                usleep(100); // short sleep to yield CPU
            }

            // copy output RGB into XImage buffer
            uint32_t *dst = (uint32_t*)xi->data;
            for (int i=0;i<W*H;i++){
                int idx = i*3;
                uint8_t r = g_rgb_out[idx+0], g = g_rgb_out[idx+1], b = g_rgb_out[idx+2];
                dst[i] = (0xFF<<24) | (r<<16) | (g<<8) | b;
            }
            XPutImage(dpy, win, gc, xi, 0,0, 0,0, W, H); XFlush(dpy);

            // requeue buffer
            if (ioctl(g_vfd, VIDIOC_QBUF, &buf) < 0) perror("VIDIOC_QBUF");
            t += dt;
        } else {
            // no frame yet
            usleep(2000);
        }
    }

    // cleanup (not reached in typical usage)
    g_stop = true;
    pthread_cond_broadcast(&g_job_cond);
    for (int i=0;i<g_worker_count;i++) pthread_join(g_workers[i], NULL);
    free(g_rgb_in); free(g_rgb_out);
    free(g_vbufs);
    free(g_V_low); free(g_grad_low);
    free(g_gauss_kernel); free(g_atan_lut); free(g_jobs);
    XDestroyImage(xi); XFreeGC(dpy, gc); XDestroyWindow(dpy, win); XCloseDisplay(dpy);
    return 0;
}

