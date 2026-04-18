// face_online_hsv_adaptive.c
// Compile: gcc -O2 face_online_hsv_adaptive.c -o face_online_hsv_adaptive -lSDL2 -lm
// Run on Linux with a V4L2 camera at /dev/video0 and SDL2 installed.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>

/////////////////////// CONFIG ///////////////////////
// Frame/capture
#define DEV "/dev/video0"
#define W 640
#define H 480

// ROI / resize
#define ROI_SIZE 192
#define RES 64
#define PIXELS (RES*RES)

// PCA
#define K 40
#define MAX_CLASSES 128

// Skin model / adaptation
#define ALPHA 0.01f
#define KH 2.0f
#define S_BASE_MIN 0.20f
#define S_MIN_ABS 0.08f
#define BETA 0.5f
#define V_REF 0.5f
#define AREA_MIN 5000
#define STABLE_COM_MAX_MOVE 8
#define STABLE_FRAMES_REQ 3
#define UPDATE_EVERY_N 3

// Quality
#define VAR_GRAY_MIN 50.0f

// Misc
#define MAX_BUFFERS 8
//////////////////////////////////////////////////////

struct buffer { void *start; size_t length; };
static struct buffer *buffers;
static int fd = -1;

// Online PCA / Oja
static float mean_img[PIXELS];
static float U[K][PIXELS];
static int initialized = 0;
static float centroids[MAX_CLASSES][K];
static int class_count[MAX_CLASSES];
static int num_classes = 0;

// Skin model (H and S Gaussians)
typedef struct {
    float mu_h, var_h;
    float mu_s, var_s;
} SkinModel;
static SkinModel skin = { 20.0f, 100.0f, 0.35f, 0.02f };

// buffers
static uint8_t *frame_rgb = NULL;
static uint8_t *mask = NULL;
static uint8_t *gray_res = NULL;

// centroid stability tracking
static int prev_cx = -1, prev_cy = -1;
static int stable_count = 0;
static int stable_update_counter = 0;

// utility
static inline uint8_t clampi(int v){ if(v<0) return 0; if(v>255) return 255; return v; }
static inline int in_bounds(int x,int y){ return x>=0 && x<W && y>=0 && y<H; }

///////////////////// COLOR & MASK /////////////////////
// Convert YUYV -> RGB24 (store in frame_rgb)
static void yuyv_to_rgb_store(uint8_t *yuyv, uint8_t *rgbbuf){
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x+=2){
            int idx = (y*W + x)*2;
            int Y0 = yuyv[idx+0];
            int U  = yuyv[idx+1];
            int Y1 = yuyv[idx+2];
            int V  = yuyv[idx+3];
            int C = Y0 - 16;
            int D = U - 128;
            int E = V - 128;
            int R = (298*C + 409*E + 128) >> 8;
            int G = (298*C - 100*D - 208*E + 128) >> 8;
            int B = (298*C + 516*D + 128) >> 8;
            int p0 = (y*W + x)*3;
            rgbbuf[p0+0] = clampi(R); rgbbuf[p0+1] = clampi(G); rgbbuf[p0+2] = clampi(B);
            C = Y1 - 16;
            R = (298*C + 409*E + 128) >> 8;
            G = (298*C - 100*D - 208*E + 128) >> 8;
            B = (298*C + 516*D + 128) >> 8;
            int p1 = (y*W + x+1)*3;
            rgbbuf[p1+0] = clampi(R); rgbbuf[p1+1] = clampi(G); rgbbuf[p1+2] = clampi(B);
        }
    }
}

// RGB -> HSV for one pixel (H: 0..360, S:0..1, V:0..1)
static void rgb_to_hsv_pixel(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v){
    float rf = r/255.0f, gf = g/255.0f, bf = b/255.0f;
    float mx = fmaxf(rf, fmaxf(gf, bf));
    float mn = fminf(rf, fminf(gf, bf));
    float d = mx - mn;
    *v = mx;
    *s = (mx == 0.0f) ? 0.0f : d / mx;
    if(d == 0.0f) { *h = 0.0f; return; }
    if(mx == rf) *h = 60.0f * fmodf(((gf - bf) / d), 6.0f);
    else if(mx == gf) *h = 60.0f * (((bf - rf) / d) + 2.0f);
    else *h = 60.0f * (((rf - gf) / d) + 4.0f);
    if(*h < 0.0f) *h += 360.0f;
}

// compute adaptive thresholds from skin model and V_mean
static void compute_thresholds(float *h_low, float *h_high, float *s_min, float V_mean){
    float sigma_h = sqrtf(fmaxf(skin.var_h, 1e-6f));
    *h_low = skin.mu_h - KH * sigma_h;
    *h_high = skin.mu_h + KH * sigma_h;
    float smin = S_BASE_MIN - BETA * (V_REF - V_mean);
    if(smin < S_MIN_ABS) smin = S_MIN_ABS;
    *s_min = smin;
}

// create skin mask using adaptive thresholds; returns area and computes V_mean over mask
static int create_skin_mask_adaptive(uint8_t *rgbbuf, uint8_t *mask_out, float *out_Vmean, float *out_Hmean, float *out_Smean){
    float h_low, h_high, s_min;
    double Vsum_all = 0.0;
    for(int i=0;i<W*H;i++){
        int p = i*3;
        float h,s,v; rgb_to_hsv_pixel(rgbbuf[p], rgbbuf[p+1], rgbbuf[p+2], &h, &s, &v);
        Vsum_all += v;
    }
    float V_mean_global = (float)(Vsum_all / (W*H));
    compute_thresholds(&h_low, &h_high, &s_min, V_mean_global);

    int area = 0;
    double Vsum=0.0, Hsum=0.0, Ssum=0.0;
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            int p = (y*W + x)*3;
            float h,s,v; rgb_to_hsv_pixel(rgbbuf[p], rgbbuf[p+1], rgbbuf[p+2], &h, &s, &v);
            int skinpix = 0;
            if(h_low < 0.0f || h_high >= 360.0f){
                float hl = fmodf(h_low + 360.0f, 360.0f);
                float hh = fmodf(h_high, 360.0f);
                if(h >= 0.0f && h <= hh) skinpix = (s >= s_min && v > 0.05f);
                if(h >= hl && h <= 360.0f) skinpix = (s >= s_min && v > 0.05f);
            } else {
                if(h >= h_low && h <= h_high) skinpix = (s >= s_min && v > 0.05f);
            }
            mask_out[y*W + x] = skinpix ? 255 : 0;
            if(skinpix){
                area++;
                Vsum += v; Hsum += h; Ssum += s;
            }
        }
    }
    if(area>0){
        *out_Vmean = (float)(Vsum/area);
        *out_Hmean = (float)(Hsum/area);
        *out_Smean = (float)(Ssum/area);
    } else {
        *out_Vmean = V_mean_global;
        *out_Hmean = skin.mu_h;
        *out_Smean = skin.mu_s;
    }
    return area;
}

///////////////////// ROI & PCA ////////////////////////
static void mask_center_of_mass(uint8_t *mask_in, int *cx, int *cy){
    long sx=0, sy=0; long cnt=0;
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            if(mask_in[y*W + x]){
                sx += x; sy += y; cnt++;
            }
        }
    }
    if(cnt < 50){
        *cx = W/2; *cy = H/2;
    } else {
        *cx = (int)(sx / cnt);
        *cy = (int)(sy / cnt);
    }
}

static void roi_to_resized_gray(uint8_t *rgbbuf, int centerx, int centery, uint8_t *out){
    int cx = centerx - ROI_SIZE/2; if(cx < 0) cx = 0; if(cx + ROI_SIZE > W) cx = W - ROI_SIZE;
    int cy = centery - ROI_SIZE/2; if(cy < 0) cy = 0; if(cy + ROI_SIZE > H) cy = H - ROI_SIZE;
    for(int r=0;r<RES;r++){
        for(int c=0;c<RES;c++){
            int srcx = cx + (c*ROI_SIZE)/RES;
            int srcy = cy + (r*ROI_SIZE)/RES;
            int p = (srcy*W + srcx)*3;
            uint8_t Y = (uint8_t)((0.299f*rgbbuf[p]) + (0.587f*rgbbuf[p+1]) + (0.114f*rgbbuf[p+2]));
            out[r*RES + c] = Y;
        }
    }
}

static float variance_gray(uint8_t *img){
    double m=0, s=0;
    for(int i=0;i<PIXELS;i++) m += img[i];
    m /= PIXELS;
    for(int i=0;i<PIXELS;i++){ double d = img[i]-m; s += d*d; }
    return s/PIXELS;
}

// Oja update
static void oja_update(float *x, float lr){
    if(!initialized){
        for(int i=0;i<PIXELS;i++) mean_img[i] = x[i];
        for(int k=0;k<K;k++) for(int i=0;i<PIXELS;i++) U[k][i] = ((k==0)?1.0f:0.0f);
        initialized = 1;
        return;
    }
    for(int i=0;i<PIXELS;i++) mean_img[i] = 0.999f*mean_img[i] + 0.001f*x[i];
    float v[PIXELS];
    for(int i=0;i<PIXELS;i++) v[i] = x[i] - mean_img[i];
    for(int k=0;k<K;k++){
        float y = 0;
        for(int i=0;i<PIXELS;i++) y += U[k][i]*v[i];
        for(int i=0;i<PIXELS;i++) U[k][i] += lr * y * (v[i] - y * U[k][i]);
        float norm=0; for(int i=0;i<PIXELS;i++) norm += U[k][i]*U[k][i];
        norm = sqrtf(norm)+1e-9f;
        for(int i=0;i<PIXELS;i++) U[k][i] /= norm;
    }
}

static void project(float *x, float *z){
    for(int k=0;k<K;k++){
        float s=0;
        for(int i=0;i<PIXELS;i++) s += U[k][i]*(x[i]-mean_img[i]);
        z[k]=s;
    }
}

static int classify(float *z){
    if(num_classes==0) return -1;
    int best=-1; float bestd=1e30f;
    for(int c=0;c<num_classes;c++){
        if(class_count[c]==0) continue;
        float d=0;
        for(int k=0;k<K;k++){ float diff = z[k]-centroids[c][k]; d += diff*diff; }
        if(d<bestd){ bestd=d; best=c; }
    }
    return best;
}

static void update_centroid(int cid, float *z){
    if(class_count[cid]==0){
        for(int k=0;k<K;k++) centroids[cid][k]=z[k];
        class_count[cid]=1;
    } else {
        float alpha = 1.0f/(++class_count[cid]);
        for(int k=0;k<K;k++) centroids[cid][k] = (1-alpha)*centroids[cid][k] + alpha*z[k];
    }
}

///////////////////// Drawing helpers //////////////////////
static void draw_circle_rgb(uint8_t *rgb, int cx, int cy, int r, uint8_t R, uint8_t G, uint8_t B){
    for(int a=-r; a<=r; a++){
        for(int b=-r; b<=r; b++){
            if(a*a + b*b <= r*r){
                int x = cx + a, y = cy + b;
                if(in_bounds(x,y)){
                    int p=(y*W + x)*3;
                    rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B;
                }
            }
        }
    }
}
static void draw_ring_rgb(uint8_t *rgb, int cx, int cy, int r_in, int r_out, uint8_t R, uint8_t G, uint8_t B){
    for(int a=-r_out; a<=r_out; a++){
        for(int b=-r_out; b<=r_out; b++){
            int rr = a*a + b*b;
            if(rr >= r_in*r_in && rr <= r_out*r_out){
                int x = cx + a, y = cy + b;
                if(in_bounds(x,y)){
                    int p=(y*W + x)*3;
                    rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B;
                }
            }
        }
    }
}
static void draw_rect_rgb(uint8_t *rgb, int x0,int y0,int x1,int y1, uint8_t R,uint8_t G,uint8_t B){
    for(int x=x0; x<=x1; x++){
        if(in_bounds(x,y0)){ int p=(y0*W+x)*3; rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B; }
        if(in_bounds(x,y1)){ int p=(y1*W+x)*3; rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B; }
    }
    for(int y=y0; y<=y1; y++){
        if(in_bounds(x0,y)){ int p=(y*W+x0)*3; rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B; }
        if(in_bounds(x1,y)){ int p=(y*W+x1)*3; rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B; }
    }
}

///////////////////// Augen- und Mund-Erkennung (radiales Muster) //////////////////////
// Sucht zwei nahe Kreise:
// - Zentrum (r <= r_core): dunkel (schwarz)
// - Ring (r_ring_in..r_ring_out): hell (weiß)
// - Kontur: starker Gradientenring (Sobel)
// Zusätzlich wird horizontale Nähe und ähnliche y-Position gefordert, sowie plausible Augenabstand.
static void detect_eye_circles(uint8_t *rgb, int cx, int cy){
    int rx = cx - ROI_SIZE/2; if(rx < 0) rx = 0; if(rx + ROI_SIZE > W) rx = W - ROI_SIZE;
    int ry = cy - ROI_SIZE/2; if(ry < 0) ry = 0; if(ry + ROI_SIZE > H) ry = H - ROI_SIZE;

    int roi_x0 = rx, roi_y0 = ry;
    int roi_x1 = rx + ROI_SIZE - 1;
    int roi_y1 = ry + ROI_SIZE - 1;

    int Rw = ROI_SIZE, Rh = ROI_SIZE;

    // Grayscale + gradient magnitude
    float *Y = (float*)alloca(Rw*Rh*sizeof(float));
    float *Gmag = (float*)alloca(Rw*Rh*sizeof(float));

    for(int yy=0; yy<Rh; yy++){
        int y = roi_y0 + yy;
        for(int xx=0; xx<Rw; xx++){
            int x = roi_x0 + xx;
            int p = (y*W + x)*3;
            float yv = 0.299f*rgb[p] + 0.587f*rgb[p+1] + 0.114f*rgb[p+2];
            Y[yy*Rw + xx] = yv;
        }
    }
    for(int yy=1; yy<Rh-1; yy++){
        for(int xx=1; xx<Rw-1; xx++){
            float gx =
                -1*Y[(yy-1)*Rw + (xx-1)] + 1*Y[(yy-1)*Rw + (xx+1)] +
                -2*Y[(yy  )*Rw + (xx-1)] + 2*Y[(yy  )*Rw + (xx+1)] +
                -1*Y[(yy+1)*Rw + (xx-1)] + 1*Y[(yy+1)*Rw + (xx+1)];
            float gy =
                -1*Y[(yy-1)*Rw + (xx-1)] - 2*Y[(yy-1)*Rw + (xx)] - 1*Y[(yy-1)*Rw + (xx+1)] +
                 1*Y[(yy+1)*Rw + (xx-1)] + 2*Y[(yy+1)*Rw + (xx)] + 1*Y[(yy+1)*Rw + (xx+1)];
            Gmag[yy*Rw + xx] = sqrtf(gx*gx + gy*gy);
        }
    }

    // Augenregion: oberes Drittel der ROI
    int eyes_y0 = roi_y0;
    int eyes_y1 = roi_y0 + ROI_SIZE/3;

    // globale Mittelwerte für Schwellen
    double sumY=0; int cntY=0;
    for(int i=0;i<Rw*Rh;i++){ sumY += Y[i]; cntY++; }
    float meanY = (float)(sumY/cntY);
    float core_dark_thr = meanY * 0.5f;    // Kern deutlich dunkler als Durchschnitt
    float ring_bright_thr = fminf(255.0f, meanY * 1.15f); // Ring heller als Durchschnitt

    // Augenradius-Set (in Pixeln)
    int r_core = 3;            // schwarzer Kern
    int r_ring_in = 5;         // beginn heller Ring
    int r_ring_out = 8;        // Ende heller Ring
    int r_contour = 6;         // Konturprüfung nahe Ringmitte

    typedef struct { int x,y; float score; } EyeCand;
    EyeCand best1 = { -1,-1,1e9f }, best2 = { -1,-1,1e9f };

    // Kandidaten-Suche: prüfe Kreisprofil
    for(int y=eyes_y0 + r_ring_out; y<=eyes_y1 - r_ring_out; y++){
        for(int x=roi_x0 + r_ring_out; x<=roi_x1 - r_ring_out; x++){
            int rx_local = x - roi_x0;
            int ry_local = y - roi_y0;

            // Kernmittel
            double core_sum=0; int core_cnt=0;
            for(int a=-r_core; a<=r_core; a++){
                for(int b=-r_core; b<=r_core; b++){
                    if(a*a + b*b <= r_core*r_core){
                        core_sum += Y[(ry_local+b)*Rw + (rx_local+a)];
                        core_cnt++;
                    }
                }
            }
            float core_mean = (core_cnt>0)? (float)(core_sum/core_cnt) : 255.0f;
            if(core_mean > core_dark_thr) continue; // Kern muss dunkel sein

            // Ringmittel (heller Bereich)
            double ring_sum=0; int ring_cnt=0;
            for(int a=-r_ring_out; a<=r_ring_out; a++){
                for(int b=-r_ring_out; b<=r_ring_out; b++){
                    int rr = a*a + b*b;
                    if(rr >= r_ring_in*r_ring_in && rr <= r_ring_out*r_ring_out){
                        ring_sum += Y[(ry_local+b)*Rw + (rx_local+a)];
                        ring_cnt++;
                    }
                }
            }
            float ring_mean = (ring_cnt>0)? (float)(ring_sum/ring_cnt) : 0.0f;
            if(ring_mean < ring_bright_thr) continue; // Ring muss heller sein

            // Konturprüfung: starker Gradient auf Ringradius
            double cont_sum=0; int cont_cnt=0;
            for(int t=0; t<360; t+=10){
                float rad = t * (float)M_PI / 180.0f;
                int a = (int)roundf(cosf(rad) * r_contour);
                int b = (int)roundf(sinf(rad) * r_contour);
                cont_sum += Gmag[(ry_local+b)*Rw + (rx_local+a)];
                cont_cnt++;
            }
            float cont_mean = (cont_cnt>0)? (float)(cont_sum/cont_cnt) : 0.0f;
            // Score: niedriger ist besser (dunkler Kern, hoher Kontur-Gradient, heller Ring)
            float score = core_mean - 0.7f*cont_mean + (ring_bright_thr - ring_mean);

            if(score < best1.score){
                best2 = best1;
                best1.x = x; best1.y = y; best1.score = score;
            } else if(score < best2.score){
                int dx = x - best1.x, dy = y - best1.y;
                // nicht zu nahe am ersten
                if(best1.x<0 || (dx*dx + dy*dy) > (r_ring_out*r_ring_out*4)) {
                    best2.x = x; best2.y = y; best2.score = score;
                }
            }
        }
    }

    // Plausibilitätsprüfung: Augen nahe beieinander, ähnliche y-Position, horizontaler Abstand sinnvoll
    if(best1.x>=0 && best2.x>=0){
        int dy = abs(best1.y - best2.y);
        int dx = abs(best1.x - best2.x);
        int min_dx = r_ring_out*3;
        int max_dx = ROI_SIZE/2;
        if(dy <= r_ring_out*2 && dx >= min_dx && dx <= max_dx){
            // Visualisierung: schwarzer Kern, weißer Ring, zusätzlicher dünner Konturrand
            draw_circle_rgb(rgb, best1.x, best1.y, r_core, 0, 0, 0);          // schwarzer Kern
            draw_ring_rgb(rgb, best1.x, best1.y, r_ring_in, r_ring_out, 255, 255, 255); // weißer Ring
            draw_ring_rgb(rgb, best1.x, best1.y, r_ring_out+1, r_ring_out+2, 0, 128, 255); // Kontur farbig

            draw_circle_rgb(rgb, best2.x, best2.y, r_core, 0, 0, 0);
            draw_ring_rgb(rgb, best2.x, best2.y, r_ring_in, r_ring_out, 255, 255, 255);
            draw_ring_rgb(rgb, best2.x, best2.y, r_ring_out+1, r_ring_out+2, 0, 128, 255);
        }
    }
}

// Einfacher Mund: rote, gesättigte Region unten
static void detect_mouth(uint8_t *rgb, int cx, int cy){
    int rx = cx - ROI_SIZE/2; if(rx < 0) rx = 0; if(rx + ROI_SIZE > W) rx = W - ROI_SIZE;
    int ry = cy - ROI_SIZE/2; if(ry < 0) ry = 0; if(ry + ROI_SIZE > H) ry = H - ROI_SIZE;

    int roi_x0 = rx, roi_y0 = ry;
    int roi_x1 = rx + ROI_SIZE - 1;
    int roi_y1 = ry + ROI_SIZE - 1;

    int mouth_y0 = roi_y0 + (2*ROI_SIZE)/3;
    int mouth_y1 = roi_y1;
    int mx0 = roi_x0 + ROI_SIZE/6;
    int mx1 = roi_x1 - ROI_SIZE/6;

    float mouth_s_min = 0.38f;
    float mouth_v_max = 0.85f;
    float mouth_red_band = 25.0f;

    int accum_x0=1e9, accum_y0=1e9, accum_x1=-1, accum_y1=-1, mouth_area=0;
    for(int y=mouth_y0; y<=mouth_y1; y++){
        for(int x=mx0; x<=mx1; x++){
            int p=(y*W+x)*3;
            float h,s,v; rgb_to_hsv_pixel(rgb[p], rgb[p+1], rgb[p+2], &h, &s, &v);
            int is_red = (h <= mouth_red_band) || (h >= 360.0f - mouth_red_band);
            if(is_red && s >= mouth_s_min && v <= mouth_v_max){
                mouth_area++;
                if(x<accum_x0) accum_x0=x;
                if(y<accum_y0) accum_y0=y;
                if(x>accum_x1) accum_x1=x;
                if(y>accum_y1) accum_y1=y;
            }
        }
    }
    if(mouth_area > 120 && accum_x1>accum_x0 && accum_y1>accum_y0){
        draw_rect_rgb(rgb, accum_x0, accum_y0, accum_x1, accum_y1, 255, 0, 255);
    }
}

///////////////////// V4L2 init ////////////////////////
int v4l2_init(){
    fd = open(DEV, O_RDWR);
    if(fd<0){ perror("open"); return -1; }
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = W; fmt.fmt.pix.height = H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // ggf. V4L2_PIX_FMT_MJPEG, falls 800x600 nicht unterstützt
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) < 0){ perror("VIDIOC_S_FMT"); return -1; }
    struct v4l2_requestbuffers req = {0};
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if(ioctl(fd, VIDIOC_REQBUFS, &req) < 0){ perror("REQBUFS"); return -1; }
    buffers = calloc(req.count, sizeof(*buffers));
    for(int i=0;i<req.count;i++){
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if(ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0){ perror("QUERYBUF"); return -1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if(buffers[i].start == MAP_FAILED){ perror("mmap"); return -1; }
        if(ioctl(fd, VIDIOC_QBUF, &buf) < 0){ perror("QBUF"); return -1; }
    }
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){ perror("STREAMON"); return -1; }
    return 0;
}

///////////////////// MAIN ////////////////////////
int main(){
    if(v4l2_init()<0) return 1;
    if(SDL_Init(SDL_INIT_VIDEO)!=0){ fprintf(stderr,"SDL init: %s\n",SDL_GetError()); return 1; }

    // Bilineares Stretching für weiches Skalieren
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    // Fenster (resizable + maximiert, rendert Texture in Fenstergröße)
    SDL_Window *win = SDL_CreateWindow(
        "OnlineFace HSV Adaptive",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        W, H,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );
    SDL_Renderer *ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, W, H);

    frame_rgb = malloc(W*H*3);
    mask = malloc(W*H);
    gray_res = malloc(PIXELS);

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;

    float xvec[PIXELS]; float z[K];

    int running=1;
    while(running){
        if(ioctl(fd, VIDIOC_DQBUF, &buf) < 0){ perror("DQBUF"); break; }
        uint8_t *yuyv = buffers[buf.index].start;
        yuyv_to_rgb_store(yuyv, frame_rgb);

        // adaptive mask + stats
        float Vmean_mask, Hmean_mask, Smean_mask;
        int area = create_skin_mask_adaptive(frame_rgb, mask, &Vmean_mask, &Hmean_mask, &Smean_mask);

        // COM
        int cx, cy;
        mask_center_of_mass(mask, &cx, &cy);

        // stability check
        int move = (prev_cx<0) ? 0 : abs(cx - prev_cx) + abs(cy - prev_cy);
        if(move <= STABLE_COM_MAX_MOVE && area >= AREA_MIN) stable_count++; else stable_count = 0;
        prev_cx = cx; prev_cy = cy;

        // draw markers (cross + ROI)
        for(int dy=-1;dy<=1;dy++){
            int yy = cy+dy; if(yy>=0 && yy<H){
                for(int dx=-5;dx<=5;dx++){
                    int xx = cx+dx; if(xx>=0 && xx<W){
                        int p=(yy*W+xx)*3; frame_rgb[p]=255; frame_rgb[p+1]=0; frame_rgb[p+2]=0;
                    }
                }
            }
        }
        int rx = cx - ROI_SIZE/2, ry = cy - ROI_SIZE/2;
        for(int x=rx; x<rx+ROI_SIZE; x++){
            if(x<0||x>=W) continue;
            if(ry>=0 && ry<H){ int p=(ry*W+x)*3; frame_rgb[p]=0; frame_rgb[p+1]=255; frame_rgb[p+2]=0; }
            if(ry+ROI_SIZE-1>=0 && ry+ROI_SIZE-1<H){ int p=((ry+ROI_SIZE-1)*W+x)*3; frame_rgb[p]=0; frame_rgb[p+1]=255; frame_rgb[p+2]=0; }
        }
        for(int y=ry; y<ry+ROI_SIZE; y++){
            if(y<0||y>=H) continue;
            if(rx>=0 && rx<W){ int p=(y*W+rx)*3; frame_rgb[p]=0; frame_rgb[p+1]=255; frame_rgb[p+2]=0; }
            if(rx+ROI_SIZE-1>=0 && rx+ROI_SIZE-1<W){ int p=(y*W+rx+ROI_SIZE-1)*3; frame_rgb[p]=0; frame_rgb[p+1]=255; frame_rgb[p+2]=0; }
        }

        // Augen (kreisförmiges Muster) und Mund zeichnen
        detect_eye_circles(frame_rgb, cx, cy);
        detect_mouth(frame_rgb, cx, cy);

        // ROI -> resized gray
        roi_to_resized_gray(frame_rgb, cx, cy, gray_res);

        // quality check
        if(variance_gray(gray_res) > VAR_GRAY_MIN){
            for(int i=0;i<PIXELS;i++) xvec[i] = (float)gray_res[i];
            oja_update(xvec, 1e-6f);
            project(xvec, z);
            int cid = classify(z);
            if(cid>=0) printf("Recognized class %d (area=%d, Vmean=%.2f)\n", cid, area, Vmean_mask);
        }

        // Adaptive skin model update
        if(stable_count >= STABLE_FRAMES_REQ){
            stable_update_counter++;
            if(stable_update_counter % UPDATE_EVERY_N == 0){
                float xh = Hmean_mask;
                float xs = Smean_mask;
                float diff_h = xh - skin.mu_h;
                if(diff_h > 180.0f) diff_h -= 360.0f;
                if(diff_h < -180.0f) diff_h += 360.0f;
                float new_mu_h = skin.mu_h + ALPHA * diff_h;
                float new_var_h = (1.0f - ALPHA) * skin.var_h + ALPHA * (diff_h*diff_h);
                float diff_s = xs - skin.mu_s;
                float new_mu_s = skin.mu_s + ALPHA * diff_s;
                float new_var_s = (1.0f - ALPHA) * skin.var_s + ALPHA * (diff_s*diff_s);
                skin.mu_h = new_mu_h;
                skin.var_h = new_var_h;
                skin.mu_s = new_mu_s;
                skin.var_s = new_var_s;
                printf("Skin model updated: muH=%.2f varH=%.2f muS=%.3f varS=%.5f area=%d\n",
                       skin.mu_h, skin.var_h, skin.mu_s, skin.var_s, area);
            }
        } else {
            stable_update_counter = 0;
        }

        // display with stretch to window size (bilinear filtering active)
        SDL_UpdateTexture(tex, NULL, frame_rgb, W*3);
        int win_w, win_h;
        SDL_GetWindowSize(win, &win_w, &win_h);
        SDL_Rect dst = {0, 0, win_w, win_h};
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, &dst); // stretch to fill window with smoothing
        SDL_RenderPresent(ren);

        // events
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) running=0;
            if(e.type==SDL_KEYDOWN){
                if(e.key.keysym.sym=='q') running=0;
                if(e.key.keysym.sym=='e'){
                    project(xvec, z);
                    int id = num_classes++;
                    if(id >= MAX_CLASSES){ printf("Max classes reached\n"); num_classes--; }
                    else { update_centroid(id, z); printf("Enrolled class %d\n", id); }
                }
            }
        }

        if(ioctl(fd, VIDIOC_QBUF, &buf) < 0){ perror("QBUF"); break; }
    }

    // cleanup
    SDL_DestroyTexture(tex); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
    if(frame_rgb) free(frame_rgb);
    if(mask) free(mask);
    if(gray_res) free(gray_res);
    close(fd);
    return 0;
}

