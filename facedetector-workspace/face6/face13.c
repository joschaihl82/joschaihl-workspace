// face.c
// Compile: gcc -O2 face.c -o face -lSDL2 -lm
// Requires: Linux + V4L2 camera at /dev/video0 + SDL2

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
// Capture
#define DEV "/dev/video0"
#define W 640
#define H 480

// ROI / resize
#define ROI_SIZE 160
#define RES 64
#define PIXELS (RES*RES)

// PCA
#define K 40
#define MAX_CLASSES 128

// Skin model
#define ALPHA 0.01f
#define KH 2.0f
#define S_BASE_MIN 0.20f
#define S_MIN_ABS 0.08f
#define BETA 0.5f
#define V_REF 0.5f
#define AREA_MIN 3500
#define STABLE_COM_MAX_MOVE 7
#define STABLE_FRAMES_REQ 3
#define UPDATE_EVERY_N 3

// Quality
#define VAR_GRAY_MIN 50.0f

// Edge detection
#define USE_GABOR 1           // set to 0 to disable Gabor reinforcement
#define GABOR_KERNEL_SZ 11
#define GABOR_ORIENTS 3       // 0°, 30°, 60°

#define MAX_BUFFERS 8
//////////////////////////////////////////////////////

struct buffer { void *start; size_t length; };
static struct buffer *buffers;
static int fd = -1;

// Online PCA
static float mean_img[PIXELS];
static float U[K][PIXELS];
static int initialized = 0;
static float centroids[MAX_CLASSES][K];
static int class_count[MAX_CLASSES];
static int num_classes = 0;

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

///////////////////// COLOR /////////////////////
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

///////////////////// SKIN MASK /////////////////////
static void compute_thresholds(float *h_low, float *h_high, float *s_min, float V_mean){
    float sigma_h = sqrtf(fmaxf(skin.var_h, 1e-6f));
    *h_low = skin.mu_h - KH * sigma_h;
    *h_high = skin.mu_h + KH * sigma_h;
    float smin = S_BASE_MIN - BETA * (V_REF - V_mean);
    if(smin < S_MIN_ABS) smin = S_MIN_ABS;
    *s_min = smin;
}

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
            if(skinpix){ area++; Vsum += v; Hsum += h; Ssum += s; }
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

///////////////////// ROI UTILS ////////////////////////
static void mask_center_of_mass(uint8_t *mask_in, int *cx, int *cy){
    long sx=0, sy=0; long cnt=0;
    for(int y=0;y<H;y++) for(int x=0;x<W;x++) if(mask_in[y*W+x]){ sx+=x; sy+=y; cnt++; }
    if(cnt < 50){ *cx = W/2; *cy = H/2; } else { *cx = (int)(sx / cnt); *cy = (int)(sy / cnt); }
}

static void roi_to_resized_gray(uint8_t *rgbbuf, int centerx, int centery, uint8_t *out){
    int cx = centerx - ROI_SIZE/2; if(cx < 0) cx = 0; if(cx + ROI_SIZE > W) cx = W - ROI_SIZE;
    int cy = centery - ROI_SIZE/2; if(cy < 0) cy = 0; if(cy + ROI_SIZE > H) cy = H - ROI_SIZE;
    for(int r=0;r<RES;r++){
        for(int c=0;c<RES;c++){
            int srcx = cx + (c*ROI_SIZE)/RES;
            int srcy = cy + (r*ROI_SIZE)/RES;
            int p = (srcy*W + srcx)*3;
            uint8_t Y = (uint8_t)(0.299f*rgbbuf[p] + 0.587f*rgbbuf[p+1] + 0.114f*rgbbuf[p+2]);
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

///////////////////// Oja PCA ////////////////////////
static void oja_update(float *x, float lr){
    if(!initialized){
        for(int i=0;i<PIXELS;i++) mean_img[i] = x[i];
        for(int k=0;k<K;k++) for(int i=0;i<PIXELS;i++) U[k][i] = ((k==0)?1.0f:0.0f);
        initialized = 1; return;
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
    for(int k=0;k<K;k++){ float s=0; for(int i=0;i<PIXELS;i++) s += U[k][i]*(x[i]-mean_img[i]); z[k]=s; }
}
static int classify(float *z){
    if(num_classes==0) return -1;
    int best=-1; float bestd=1e30f;
    for(int c=0;c<num_classes;c++){
        if(class_count[c]==0) continue;
        float d=0; for(int k=0;k<K;k++){ float diff = z[k]-centroids[c][k]; d += diff*diff; }
        if(d<bestd){ bestd=d; best=c; }
    }
    return best;
}
static void update_centroid(int cid, float *z){
    if(class_count[cid]==0){ for(int k=0;k<K;k++) centroids[cid][k]=z[k]; class_count[cid]=1; }
    else { float alpha = 1.0f/(++class_count[cid]); for(int k=0;k<K;k++) centroids[cid][k] = (1-alpha)*centroids[cid][k] + alpha*z[k]; }
}

///////////////////// Drawing ////////////////////////
static void draw_circle_rgb(uint8_t *rgb, int cx, int cy, int r, uint8_t R, uint8_t G, uint8_t B){
    for(int a=-r; a<=r; a++) for(int b=-r; b<=r; b++) if(a*a + b*b <= r*r){
        int x = cx + a, y = cy + b; if(in_bounds(x,y)){ int p=(y*W + x)*3; rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B; }
    }
}
static void draw_ring_rgb(uint8_t *rgb, int cx, int cy, int r_in, int r_out, uint8_t R, uint8_t G, uint8_t B){
    for(int a=-r_out; a<=r_out; a++) for(int b=-r_out; b<=r_out; b++){
        int rr = a*a + b*b;
        if(rr >= r_in*r_in && rr <= r_out*r_out){
            int x = cx + a, y = cy + b; if(in_bounds(x,y)){ int p=(y*W+x)*3; rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B; }
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

///////////////////// Sobel & Canny-light ////////////////////////
static void sobel_mag_Y(const float *Y, float *G, int w, int h){
    for(int y=1;y<h-1;y++){
        for(int x=1;x<w-1;x++){
            float gx =
                -1*Y[(y-1)*w + (x-1)] + 1*Y[(y-1)*w + (x+1)] +
                -2*Y[(y  )*w + (x-1)] + 2*Y[(y  )*w + (x+1)] +
                -1*Y[(y+1)*w + (x-1)] + 1*Y[(y+1)*w + (x+1)];
            float gy =
                -1*Y[(y-1)*w + (x-1)] - 2*Y[(y-1)*w + (x)] - 1*Y[(y-1)*w + (x+1)] +
                 1*Y[(y+1)*w + (x-1)] + 2*Y[(y+1)*w + (x)] + 1*Y[(y+1)*w + (x+1)];
            G[y*w + x] = sqrtf(gx*gx + gy*gy);
        }
    }
}

static void percentile_thresholds(const float *G, int w, int h, float *t_low, float *t_high, float p_low, float p_high){
    // simple histogram-based approx (clamp magnitude to [0,255])
    int hist[256]={0};
    int n=w*h;
    for(int i=0;i<n;i++){
        int v=(int)fminf(255.0f, fmaxf(0.0f, G[i]));
        hist[v]++;
    }
    int cumsum=0, target_low=(int)(p_low*n), target_high=(int)(p_high*n);
    int low_bin=0, high_bin=255;
    for(int b=0;b<256;b++){
        cumsum += hist[b];
        if(cumsum >= target_low){ low_bin = b; break; }
    }
    cumsum=0;
    for(int b=0;b<256;b++){
        cumsum += hist[b];
        if(cumsum >= target_high){ high_bin = b; break; }
    }
    *t_low = (float)low_bin;
    *t_high= (float)high_bin;
}

static void hysteresis_edges(const float *G, uint8_t *edges, int w, int h, float t_low, float t_high){
    // 2-level hysteresis: strong edges -> 255, weak connected to strong -> 255
    memset(edges, 0, w*h);
    // mark strong
    for(int i=0;i<w*h;i++) if(G[i] >= t_high) edges[i] = 255;
    // weak pass
    int changed=1;
    while(changed){
        changed=0;
        for(int y=1;y<h-1;y++){
            for(int x=1;x<w-1;x++){
                int i=y*w+x;
                if(edges[i]) continue;
                if(G[i] >= t_low){
                    // any strong neighbor?
                    int strong=0;
                    for(int dy=-1;dy<=1 && !strong;dy++){
                        for(int dx=-1;dx<=1;dx++){
                            if(dx==0 && dy==0) continue;
                            if(edges[(y+dy)*w + (x+dx)]==255){ strong=1; break; }
                        }
                    }
                    if(strong){ edges[i]=255; changed=1; }
                }
            }
        }
    }
}

///////////////////// Gabor filters (optional) ////////////////////////
#if USE_GABOR
static void gen_gabor(float *K, int sz, float theta, float sigma, float lambda, float gamma, float psi){
    int c = sz/2;
    for(int y=0;y<sz;y++){
        for(int x=0;x<sz;x++){
            float xp = (x - c) * cosf(theta) + (y - c) * sinf(theta);
            float yp = -(x - c) * sinf(theta) + (y - c) * cosf(theta);
            float gauss = expf(-(xp*xp + gamma*gamma*yp*yp)/(2*sigma*sigma));
            float wave  = cosf(2.0f*M_PI*xp/lambda + psi);
            K[y*sz + x] = gauss * wave;
        }
    }
}
static void conv2(const float *img, int w, int h, const float *K, int sz, float *out){
    int c = sz/2;
    for(int y=c; y<h-c; y++){
        for(int x=c; x<w-c; x++){
            float s=0;
            for(int ky=0; ky<sz; ky++){
                for(int kx=0; kx<sz; kx++){
                    int ix = x + kx - c;
                    int iy = y + ky - c;
                    s += img[iy*w + ix] * K[ky*sz + kx];
                }
            }
            out[y*w + x] = s;
        }
    }
}
#endif

///////////////////// Hair band from skin border ////////////////////////
static void skin_border_band(const uint8_t *mask_in, uint8_t *band_out, int w, int h, int radius){
    memset(band_out,0,w*h);
    // dilate skin then subtract original -> outer band
    uint8_t *dil = (uint8_t*)alloca(w*h);
    memset(dil,0,w*h);
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            if(!mask_in[y*w + x]) continue;
            for(int dy=-radius;dy<=radius;dy++){
                for(int dx=-radius;dx<=radius;dx++){
                    int xx=x+dx, yy=y+dy;
                    if(xx>=0 && xx<w && yy>=0 && yy<h) dil[yy*w + xx] = 255;
                }
            }
        }
    }
    for(int i=0;i<w*h;i++){
        if(dil[i] && !mask_in[i]) band_out[i]=255;
    }
}

///////////////////// Eye pupils + eyelid edges ////////////////////////
static void detect_pupils(uint8_t *rgb, int cx, int cy, int *out_x1, int *out_y1, int *out_x2, int *out_y2){
    int rx = cx - ROI_SIZE/2; if(rx < 0) rx = 0; if(rx + ROI_SIZE > W) rx = W - ROI_SIZE;
    int ry = cy - ROI_SIZE/2; if(ry < 0) ry = 0; if(ry + ROI_SIZE > H) ry = H - ROI_SIZE;
    int Rw = ROI_SIZE, Rh = ROI_SIZE;

    float *Y = (float*)alloca(Rw*Rh*sizeof(float));
    for(int yy=0; yy<Rh; yy++){
        int y = ry + yy;
        for(int xx=0; xx<Rw; xx++){
            int x = rx + xx;
            int p = (y*W + x)*3;
            Y[yy*Rw + xx] = 0.299f*rgb[p] + 0.587f*rgb[p+1] + 0.114f*rgb[p+2];
        }
    }
    // blur
    float *Yb = (float*)alloca(Rw*Rh*sizeof(float));
    for(int y=1;y<Rh-1;y++){
        for(int x=1;x<Rw-1;x++){
            float s=0;
            for(int dy=-1;dy<=1;dy++) for(int dx=-1;dx<=1;dx++) s += Y[(y+dy)*Rw + (x+dx)];
            Yb[y*Rw + x] = s/9.0f;
        }
    }
    for(int x=0;x<Rw;x++){ Yb[0*Rw+x]=Y[0*Rw+x]; Yb[(Rh-1)*Rw+x]=Y[(Rh-1)*Rw+x]; }
    for(int y=0;y<Rh;y++){ Yb[y*Rw+0]=Y[y*Rw+0]; Yb[y*Rw+(Rw-1)]=Y[y*Rw+(Rw-1)]; }

    float *G = (float*)alloca(Rw*Rh*sizeof(float));
    sobel_mag_Y(Yb, G, Rw, Rh);

    int eyes_y0 = 0;
    int eyes_y1 = ROI_SIZE/3;

    double sumY=0, sumY2=0; int cnt=0;
    for(int y=eyes_y0; y<eyes_y1; y++) for(int x=0; x<Rw; x++){ float v=Yb[y*Rw+x]; sumY+=v; sumY2+=v*v; cnt++; }
    float meanY = (float)(sumY / (float)cnt);
    float varY = (float)(sumY2 / (float)cnt - meanY*meanY);
    float stdY = sqrtf(fmaxf(varY, 1e-6f));

    float core_thr = meanY - 1.0f*stdY;
    float ring_thr = meanY + 0.6f*stdY;
    float cont_gain = 0.9f;

    int r_core = 3, r_ring_in = 5, r_ring_out = 9, r_contour = 7;

    typedef struct { int x,y; float score; } Cand;
    Cand best1={-1,-1,1e9f}, best2={-1,-1,1e9f};

    for(int y=eyes_y0 + r_ring_out; y<=eyes_y1 - r_ring_out; y++){
        for(int x=r_ring_out; x<=Rw - r_ring_out; x++){
            // core
            double core_sum=0; int core_cnt=0;
            for(int a=-r_core;a<=r_core;a++) for(int b=-r_core;b<=r_core;b++) if(a*a+b*b<=r_core*r_core){ core_sum+=Yb[(y+b)*Rw+(x+a)]; core_cnt++; }
            float core_mean = (float)(core_sum/core_cnt);
            if(core_mean > core_thr) continue;

            // glint suppression
            int glint=0;
            for(int a=-2;a<=2 && !glint;a++) for(int b=-2;b<=2;b++){ if(Yb[(y+b)*Rw+(x+a)] > meanY + 1.5f*stdY){ glint=1; break; } }
            if(glint) continue;

            // ring
            double ring_sum=0; int ring_cnt=0;
            for(int a=-r_ring_out; a<=r_ring_out; a++){
                for(int b=-r_ring_out; b<=r_ring_out; b++){
                    int rr=a*a+b*b;
                    if(rr>=r_ring_in*r_ring_in && rr<=r_ring_out*r_ring_out){ ring_sum+=Yb[(y+b)*Rw+(x+a)]; ring_cnt++; }
                }
            }
            float ring_mean = (float)(ring_sum/ring_cnt);
            if(ring_mean < ring_thr) continue;

            // contour
            double cont_sum=0; int cont_cnt=0;
            for(int t=0;t<360;t+=10){
                float rad=t*(float)M_PI/180.0f;
                int a=(int)roundf(cosf(rad)*r_contour);
                int b=(int)roundf(sinf(rad)*r_contour);
                cont_sum += G[(y+b)*Rw + (x+a)];
                cont_cnt++;
            }
            float cont_mean = (float)(cont_sum/cont_cnt);

            float score = core_mean - cont_gain*cont_mean + (ring_thr - ring_mean);

            if(score < best1.score){ best2=best1; best1.x=x; best1.y=y; best1.score=score; }
            else if(score < best2.score){
                int dx=x-best1.x, dy=y-best1.y;
                if(best1.x<0 || (dx*dx+dy*dy) > (r_ring_out*r_ring_out*3)) { best2.x=x; best2.y=y; best2.score=score; }
            }
        }
    }

    *out_x1 = -1; *out_y1 = -1; *out_x2 = -1; *out_y2 = -1;
    if(best1.x>=0 && best2.x>=0){
        int dy=abs(best1.y-best2.y), dx=abs(best1.x-best2.x);
        int min_dx=r_ring_out*3, max_dx=ROI_SIZE/2;
        if(dy <= r_ring_out*2 && dx >= min_dx && dx <= max_dx){
            *out_x1 = rx + best1.x; *out_y1 = ry + best1.y;
            *out_x2 = rx + best2.x; *out_y2 = ry + best2.y;

            // visualize
            draw_circle_rgb(rgb, *out_x1, *out_y1, r_core, 0,0,0);
            draw_ring_rgb(rgb, *out_x1, *out_y1, r_ring_in, r_ring_out, 255,255,255);
            draw_ring_rgb(rgb, *out_x1, *out_y1, r_ring_out+1, r_ring_out+2, 0,128,255);

            draw_circle_rgb(rgb, *out_x2, *out_y2, r_core, 0,0,0);
            draw_ring_rgb(rgb, *out_x2, *out_y2, r_ring_in, r_ring_out, 255,255,255);
            draw_ring_rgb(rgb, *out_x2, *out_y2, r_ring_out+1, r_ring_out+2, 0,128,255);
        }
    }
}

///////////////////// Mouth detection ////////////////////////
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

    float mouth_s_min = 0.40f;
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
    if(mouth_area > 100 && accum_x1>accum_x0 && accum_y1>accum_y0){
        draw_rect_rgb(rgb, accum_x0, accum_y0, accum_x1, accum_y1, 255,0,255);
    }
}

///////////////////// Face-specific edges: gated Canny ////////////////////////
static void face_edges(uint8_t *rgb, const uint8_t *skin_mask, int cx, int cy){
    // Build luminance Y
    float *Y = (float*)alloca(W*H*sizeof(float));
    for(int i=0;i<W*H;i++){
        int p=i*3;
        Y[i] = 0.299f*rgb[p] + 0.587f*rgb[p+1] + 0.114f*rgb[p+2];
    }
    // Gradient
    float *G = (float*)alloca(W*H*sizeof(float));
    sobel_mag_Y(Y, G, W, H);

    // Hair band around skin
    uint8_t *hair_band = (uint8_t*)alloca(W*H);
    skin_border_band(skin_mask, hair_band, W, H, 6);

    // Adaptive thresholds per region
    float tL_skin, tH_skin, tL_hair, tH_hair;
    percentile_thresholds(G, W, H, &tL_skin, &tH_skin, 0.40f, 0.80f);
    percentile_thresholds(G, W, H, &tL_hair, &tH_hair, 0.50f, 0.95f);

    // Edges result buffer
    uint8_t *edges = (uint8_t*)alloca(W*H);
    memset(edges,0,W*H);

    // Region masks: skin-only and hairband-only
    uint8_t *skin_only = (uint8_t*)alloca(W*H);
    uint8_t *hair_only = (uint8_t*)alloca(W*H);
    for(int i=0;i<W*H;i++){
        skin_only[i] = skin_mask[i] ? 255 : 0;
        hair_only[i] = hair_band[i] ? 255 : 0;
    }

    // Hysteresis per region (apply on masked gradient)
    // For simplicity: run hysteresis on full G but gate edges by mask
    uint8_t *edges_skin = (uint8_t*)alloca(W*H);
    uint8_t *edges_hair = (uint8_t*)alloca(W*H);
    memset(edges_skin,0,W*H); memset(edges_hair,0,W*H);

    hysteresis_edges(G, edges_skin, W, H, tL_skin, tH_skin);
    hysteresis_edges(G, edges_hair, W, H, tL_hair, tH_hair);

    // Gate
    for(int i=0;i<W*H;i++){
        if(skin_only[i] && edges_skin[i]) edges[i]=255;
        if(hair_only[i] && edges_hair[i]) edges[i]=255;
    }

    // Optional Gabor reinforcement in upper ROI (eyes/eyebrows)
#if USE_GABOR
    int rx = cx - ROI_SIZE/2; if(rx < 0) rx = 0; if(rx + ROI_SIZE > W) rx = W - ROI_SIZE;
    int ry = cy - ROI_SIZE/2; if(ry < 0) ry = 0; if(ry + ROI_SIZE > H) ry = H - ROI_SIZE;

    float K[GABOR_ORIENTS][GABOR_KERNEL_SZ*GABOR_KERNEL_SZ];
    float thetas[GABOR_ORIENTS] = {0.0f, 30.0f*(float)M_PI/180.0f, 60.0f*(float)M_PI/180.0f};
    for(int o=0;o<GABOR_ORIENTS;o++) gen_gabor(K[o], GABOR_KERNEL_SZ, thetas[o], 2.5f, 8.0f, 0.5f, 0.0f);

    float *resp = (float*)alloca(W*H*sizeof(float));
    memset(resp,0,W*H*sizeof(float));
    for(int o=0;o<GABOR_ORIENTS;o++){
        float *tmp = (float*)alloca(W*H*sizeof(float));
        memset(tmp,0,W*H*sizeof(float));
        conv2(Y, W, H, K[o], GABOR_KERNEL_SZ, tmp);
        for(int y=ry; y<ry+ROI_SIZE; y++){
            for(int x=rx; x<rx+ROI_SIZE; x++){
                int i=y*W+x;
                if(tmp[i] > resp[i]) resp[i]=tmp[i];
            }
        }
    }
    // Threshold resp and OR with edges inside ROI
    // Simple threshold: mean + 2*std inside ROI
    double rsum=0, rsum2=0; int rcnt=0;
    for(int y=ry; y<ry+ROI_SIZE; y++)
        for(int x=rx; x<rx+ROI_SIZE; x++){ float v=resp[y*W+x]; rsum+=v; rsum2+=v*v; rcnt++; }
    float rmean = (float)(rsum/rcnt);
    float rstd  = (float)sqrt(fmax(0.0, rsum2/rcnt - rmean*rmean));
    float rthr = rmean + 2.0f*rstd;

    for(int y=ry; y<ry+ROI_SIZE; y++){
        for(int x=rx; x<rx+ROI_SIZE; x++){
            int i=y*W+x;
            if(resp[i] > rthr) edges[i]=255;
        }
    }
#endif

    // Draw edges overlay (cyan)
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            if(edges[y*W + x]){
                int p=(y*W + x)*3;
                // blend: set to cyan but keep some original
                rgb[p]   = (uint8_t)fminf(255.0f, 0.4f*rgb[p]   + 0.6f*0);
                rgb[p+1] = (uint8_t)fminf(255.0f, 0.4f*rgb[p+1] + 0.6f*255);
                rgb[p+2] = (uint8_t)fminf(255.0f, 0.4f*rgb[p+2] + 0.6f*255);
            }
        }
    }
}

///////////////////// V4L2 init ////////////////////////
static int v4l2_init(){
    fd = open(DEV, O_RDWR);
    if(fd<0){ perror("open"); return -1; }
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = W; fmt.fmt.pix.height = H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
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

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // bilinear
    SDL_Window *win = SDL_CreateWindow("OnlineFace HSV Adaptive",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
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

        // Skin mask + stats
        float Vmean_mask, Hmean_mask, Smean_mask;
        int area = create_skin_mask_adaptive(frame_rgb, mask, &Vmean_mask, &Hmean_mask, &Smean_mask);

        // COM
        int cx, cy;
        mask_center_of_mass(mask, &cx, &cy);

        // stability check
        int move = (prev_cx<0) ? 0 : abs(cx - prev_cx) + abs(cy - prev_cy);
        if(move <= STABLE_COM_MAX_MOVE && area >= AREA_MIN) stable_count++; else stable_count = 0;
        prev_cx = cx; prev_cy = cy;

        // draw ROI marker
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

        // Pupillen erkennen und visualisieren
        int ex1=-1,ey1=-1,ex2=-1,ey2=-1;
        detect_pupils(frame_rgb, cx, cy, &ex1,&ey1,&ex2,&ey2);

        // Gesichtsspezifische Kanten (Haut/Haar)
        face_edges(frame_rgb, mask, cx, cy);

        // Mund erkennen
        detect_mouth(frame_rgb, cx, cy);

        // ROI -> resized gray for PCA
        roi_to_resized_gray(frame_rgb, cx, cy, gray_res);

        // PCA flow
        if(variance_gray(gray_res) > VAR_GRAY_MIN){
            for(int i=0;i<PIXELS;i++) xvec[i] = (float)gray_res[i];
            oja_update(xvec, 1e-6f);
            project(xvec, z);
            int cid = classify(z);
            if(cid>=0) printf("Recognized class %d (area=%d, Vmean=%.2f)\n", cid, area, Vmean_mask);
        }

        // skin model adaptation
        if(stable_count >= STABLE_FRAMES_REQ){
            stable_update_counter++;
            if(stable_update_counter % UPDATE_EVERY_N == 0){
                float xh = Hmean_mask, xs = Smean_mask;
                float diff_h = xh - skin.mu_h; if(diff_h > 180.0f) diff_h -= 360.0f; if(diff_h < -180.0f) diff_h += 360.0f;
                skin.var_h = (1.0f - ALPHA) * skin.var_h + ALPHA * (diff_h*diff_h);
                skin.mu_h += ALPHA * diff_h;
                float diff_s = xs - skin.mu_s;
                skin.var_s = (1.0f - ALPHA) * skin.var_s + ALPHA * (diff_s*diff_s);
                skin.mu_s += ALPHA * diff_s;
                printf("Skin model updated: muH=%.2f varH=%.2f muS=%.3f varS=%.5f area=%d\n",
                       skin.mu_h, skin.var_h, skin.mu_s, skin.var_s, area);
            }
        } else {
            stable_update_counter = 0;
        }

        // display
        SDL_UpdateTexture(tex, NULL, frame_rgb, W*3);
        int win_w, win_h; SDL_GetWindowSize(win, &win_w, &win_h);
        SDL_Rect dst = {0, 0, win_w, win_h};
        SDL_RenderClear(ren); SDL_RenderCopy(ren, tex, NULL, &dst); SDL_RenderPresent(ren);

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

    SDL_DestroyTexture(tex); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
    free(frame_rgb); free(mask); free(gray_res);
    close(fd);
    return 0;
}

