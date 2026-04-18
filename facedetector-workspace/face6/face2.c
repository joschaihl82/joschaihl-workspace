// face_online_hsv_adaptive_v2.c
// Compile: gcc -O2 face_online_hsv_adaptive_v2.c -o face_online_hsv_adaptive_v2 -lSDL2 -lm
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
#define W 320
#define H 240

// ROI / resize
#define ROI_SIZE 128
#define RES 64
#define PIXELS (RES*RES)

// PCA
#define K 40
#define MAX_CLASSES 128

// Skin model / adaptation
#define ALPHA 0.01f           // learning rate for mu/var updates
#define KH 2.0f               // H-range multiplier (mu +/- KH*sigma)
#define S_BASE_MIN 0.20f      // base S minimum
#define S_MIN_ABS 0.08f       // absolute lower bound for S
#define BETA 0.5f             // factor for V influence on S_min
#define V_REF 0.5f            // reference V for S adjustment (0..1)
#define AREA_MIN 1500         // minimal skin pixel count to consider update
#define STABLE_COM_MAX_MOVE 6 // max centroid move (pixels) to be considered stable
#define STABLE_FRAMES_REQ 3   // number of consecutive stable frames required
#define UPDATE_EVERY_N 3      // update model every N stable frames

// Enrollment
#define ENROLL_FRAMES 8       // number of stable frames to average for enrollment

// Quality
#define VAR_GRAY_MIN 50.0f

// Persistence
#define MODEL_FILENAME "afrm_model.bin"

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
static SkinModel skin = { 20.0f, 100.0f, 0.35f, 0.02f }; // initial guesses

// buffers
static uint8_t *frame_rgb = NULL;
static uint8_t *mask = NULL;
static uint8_t *gray_res = NULL;
static int *labels = NULL; // for CC labeling
static int *label_area = NULL;
static int *label_sumx = NULL;
static int *label_sumy = NULL;

// centroid stability tracking
static int prev_cx = -1, prev_cy = -1;
static int stable_count = 0;
static int stable_update_counter = 0;

// enrollment buffer
static float enroll_buffer[ENROLL_FRAMES][K];
static int enroll_buf_idx = 0;
static int enrolling = 0;

// utility
static inline uint8_t clampi(int v){ if(v<0) return 0; if(v>255) return 255; return v; }

///////////////////// COLOR & MASK /////////////////////
// Convert YUYV -> RGB24 (store in frame_rgb)
static void yuyv_to_rgb_store(uint8_t *yuyv, uint8_t *rgbbuf){
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x+=2){
            int idx = (y*W + x)*2;
            int Y0 = yuyv[idx+0];
            int Uc  = yuyv[idx+1];
            int Y1 = yuyv[idx+2];
            int Vc  = yuyv[idx+3];
            int C = Y0 - 16;
            int D = Uc - 128;
            int E = Vc - 128;
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

// Connected components (two-pass) on mask (binary 0/255)
// returns label id of largest component and sets cx,cy and area
static int largest_component(uint8_t *mask_in, int *out_cx, int *out_cy, int *out_area){
    int max_labels = W*H/4;
    // allocate label arrays if not yet
    static int allocated = 0;
    if(!allocated){
        labels = calloc(W*H, sizeof(int));
        label_area = calloc(W*H/4 + 10, sizeof(int));
        label_sumx = calloc(W*H/4 + 10, sizeof(int));
        label_sumy = calloc(W*H/4 + 10, sizeof(int));
        allocated = 1;
    }
    int next_label = 1;
    // first pass
    for(int i=0;i<W*H;i++) labels[i]=0;
    // simple union-find not necessary if we do 4-neighborhood with equivalences; but for speed do simple labeling with equivalence table
    int *equiv = calloc(W*H/4 + 10, sizeof(int));
    for(int i=0;i<W*H/4 + 10;i++) equiv[i]=i;
    #define FIND(x) ({ int v=x; while(equiv[v]!=v) v=equiv[v]; v; })
    #define UNION(a,b) { int ra=FIND(a), rb=FIND(b); if(ra!=rb) equiv[rb]=ra; }
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            int idx = y*W + x;
            if(mask_in[idx]==0) continue;
            int left = (x>0) ? labels[idx-1] : 0;
            int up = (y>0) ? labels[idx-W] : 0;
            if(left==0 && up==0){
                labels[idx] = next_label++;
            } else if(left!=0 && up==0){
                labels[idx] = left;
            } else if(left==0 && up!=0){
                labels[idx] = up;
            } else {
                labels[idx] = left;
                if(left != up) UNION(left, up);
            }
        }
    }
    // flatten equivalences
    for(int i=1;i<next_label;i++) equiv[i] = FIND(i);
    // relabel to compact ids
    int *map = calloc(next_label+1, sizeof(int));
    int newlabel = 1;
    for(int i=1;i<next_label;i++){
        if(equiv[i]==i) map[i]=newlabel++;
    }
    // reset accumulators
    int max_possible = newlabel+2;
    for(int i=0;i<max_possible;i++){ if(i < W*H/4 + 10){ label_area[i]=0; label_sumx[i]=0; label_sumy[i]=0; } }
    // second pass: compute sums
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            int idx = y*W + x;
            int lab = labels[idx];
            if(lab==0) continue;
            int ml = map[FIND(lab)];
            labels[idx] = ml;
            label_area[ml] += 1;
            label_sumx[ml] += x;
            label_sumy[ml] += y;
        }
    }
    // find largest
    int best = 0; int best_area = 0;
    for(int i=1;i<max_possible;i++){
        if(label_area[i] > best_area){ best_area = label_area[i]; best = i; }
    }
    if(best==0){
        *out_area = 0; *out_cx = W/2; *out_cy = H/2;
        free(equiv); free(map);
        return 0;
    }
    *out_area = best_area;
    *out_cx = label_sumx[best] / best_area;
    *out_cy = label_sumy[best] / best_area;
    free(equiv); free(map);
    return best;
}

// create skin mask using adaptive thresholds; returns area and computes V_mean over mask
static int create_skin_mask_adaptive(uint8_t *rgbbuf, uint8_t *mask_out, float *out_Vmean, float *out_Hmean, float *out_Smean){
    float h_low, h_high, s_min;
    // compute global V mean quickly (over whole image) to adapt S threshold
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
// resize ROI nearest neighbor to RESxRES grayscale
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

// variance gray
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

// project
static void project(float *x, float *z){
    for(int k=0;k<K;k++){
        float s=0;
        for(int i=0;i<PIXELS;i++) s += U[k][i]*(x[i]-mean_img[i]);
        z[k]=s;
    }
}

// classify nearest centroid
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

// update centroid online
static void update_centroid(int cid, float *z){
    if(class_count[cid]==0){
        for(int k=0;k<K;k++) centroids[cid][k]=z[k];
        class_count[cid]=1;
    } else {
        float alpha = 1.0f/(++class_count[cid]);
        for(int k=0;k<K;k++) centroids[cid][k] = (1-alpha)*centroids[cid][k] + alpha*z[k];
    }
}

///////////////////// Persistence ////////////////////////
static int save_model(const char *fname){
    FILE *f = fopen(fname,"wb");
    if(!f) return -1;
    // write skin
    fwrite(&skin, sizeof(SkinModel), 1, f);
    // write initialized flag
    fwrite(&initialized, sizeof(int), 1, f);
    // write mean_img
    fwrite(mean_img, sizeof(float), PIXELS, f);
    // write U
    for(int k=0;k<K;k++) fwrite(U[k], sizeof(float), PIXELS, f);
    // write classes
    fwrite(&num_classes, sizeof(int), 1, f);
    fwrite(class_count, sizeof(int), MAX_CLASSES, f);
    for(int c=0;c<MAX_CLASSES;c++){
        fwrite(centroids[c], sizeof(float), K, f);
    }
    fclose(f);
    return 0;
}

static int load_model(const char *fname){
    FILE *f = fopen(fname,"rb");
    if(!f) return -1;
    fread(&skin, sizeof(SkinModel), 1, f);
    fread(&initialized, sizeof(int), 1, f);
    fread(mean_img, sizeof(float), PIXELS, f);
    for(int k=0;k<K;k++) fread(U[k], sizeof(float), PIXELS, f);
    fread(&num_classes, sizeof(int), 1, f);
    fread(class_count, sizeof(int), MAX_CLASSES, f);
    for(int c=0;c<MAX_CLASSES;c++) fread(centroids[c], sizeof(float), K, f);
    fclose(f);
    return 0;
}

///////////////////// V4L2 init ////////////////////////
int v4l2_init(){
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
        ioctl(fd, VIDIOC_QUERYBUF, &buf);
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        ioctl(fd, VIDIOC_QBUF, &buf);
    }
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);
    return 0;
}

///////////////////// MAIN ////////////////////////
int main(){
    if(v4l2_init()<0) return 1;
    if(SDL_Init(SDL_INIT_VIDEO)!=0){ fprintf(stderr,"SDL init: %s\n",SDL_GetError()); return 1; }
    SDL_Window *win = SDL_CreateWindow("AFRM Adaptive v2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
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
        ioctl(fd, VIDIOC_DQBUF, &buf);
        uint8_t *yuyv = buffers[buf.index].start;
        yuyv_to_rgb_store(yuyv, frame_rgb);

        // adaptive mask + stats
        float Vmean_mask, Hmean_mask, Smean_mask;
        int area = create_skin_mask_adaptive(frame_rgb, mask, &Vmean_mask, &Hmean_mask, &Smean_mask);

        // largest component
        int cx, cy, comp_area;
        largest_component(mask, &cx, &cy, &comp_area);

        // stability check
        int move = (prev_cx<0) ? 0 : abs(cx - prev_cx) + abs(cy - prev_cy);
        if(move <= STABLE_COM_MAX_MOVE && comp_area >= AREA_MIN) stable_count++; else stable_count = 0;
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

        // ROI -> resized gray
        roi_to_resized_gray(frame_rgb, cx, cy, gray_res);

        // quality check
        if(variance_gray(gray_res) > VAR_GRAY_MIN){
            for(int i=0;i<PIXELS;i++) xvec[i] = (float)gray_res[i];
            // update PCA always
            oja_update(xvec, 1e-6f);
            project(xvec, z);
            int cid = classify(z);
            if(cid>=0) printf("Recognized class %d (comp_area=%d, Vmean=%.2f)\n", cid, comp_area, Vmean_mask);
            // enrollment buffer
            if(enrolling){
                // store z into enroll buffer
                for(int k=0;k<K;k++) enroll_buffer[enroll_buf_idx][k] = z[k];
                enroll_buf_idx++;
                if(enroll_buf_idx >= ENROLL_FRAMES){
                    // average and create new centroid
                    float avg[K]; for(int k=0;k<K;k++) avg[k]=0.0f;
                    for(int f=0;f<ENROLL_FRAMES;f++) for(int k=0;k<K;k++) avg[k] += enroll_buffer[f][k];
                    for(int k=0;k<K;k++) avg[k] /= ENROLL_FRAMES;
                    int id = num_classes++;
                    if(id >= MAX_CLASSES){ printf("Max classes reached\n"); num_classes--; }
                    else {
                        for(int k=0;k<K;k++) centroids[id][k]=avg[k];
                        class_count[id]=ENROLL_FRAMES; // initial count
                        printf("Auto-enrolled class %d (from %d frames)\n", id, ENROLL_FRAMES);
                    }
                    enrolling = 0; enroll_buf_idx = 0;
                }
            }
        }

        // Adaptive skin model update: only when stable_count >= STABLE_FRAMES_REQ and comp_area >= AREA_MIN
        if(stable_count >= STABLE_FRAMES_REQ && comp_area >= AREA_MIN){
            stable_update_counter++;
            if(stable_update_counter % UPDATE_EVERY_N == 0){
                // update skin.mu and var using Hmean_mask and Smean_mask
                float old_mu_h = skin.mu_h;
                float old_mu_s = skin.mu_s;
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
                skin.mu_h = new_mu_h; skin.var_h = new_var_h; skin.mu_s = new_mu_s; skin.var_s = new_var_s;
                printf("Skin model updated: muH=%.2f varH=%.2f muS=%.3f varS=%.5f area=%d\n",
                       skin.mu_h, skin.var_h, skin.mu_s, skin.var_s, comp_area);
            }
        } else {
            stable_update_counter = 0;
        }

        // draw simple overlay bars for H,S,V
        // compute small bar positions
        int barx = 8, bary = 8, barw = 120, barh = 10;
        // compute current H,S,V display values from skin model
        float sigma_h = sqrtf(fmaxf(skin.var_h, 1e-6f));
        float hlow = skin.mu_h - KH*sigma_h;
        float hhigh = skin.mu_h + KH*sigma_h;
        // draw background rectangle
        for(int yy=0; yy<40; yy++){
            for(int xx=0; xx<barw+16; xx++){
                int px = (bary+yy)*W + (barx+xx);
                if(px>=0 && px < W*H){
                    int p = px*3;
                    // dim background
                    frame_rgb[p+0] = (frame_rgb[p+0]*200)/255;
                    frame_rgb[p+1] = (frame_rgb[p+1]*200)/255;
                    frame_rgb[p+2] = (frame_rgb[p+2]*200)/255;
                }
            }
        }
        // draw H bar (map mu_h 0..360 to 0..barw)
        int hx = (int)((skin.mu_h/360.0f) * barw);
        if(hx<0) hx=0; if(hx>barw) hx=barw;
        for(int xx=0; xx<hx; xx++){
            int px = (bary+2)*W + (barx+xx);
            int p = px*3; frame_rgb[p]=255; frame_rgb[p+1]=200; frame_rgb[p+2]=0;
        }
        // draw S bar
        int sx = (int)((skin.mu_s/1.0f) * barw);
        for(int xx=0; xx<sx; xx++){
            int px = (bary+14)*W + (barx+xx);
            int p = px*3; frame_rgb[p]=0; frame_rgb[p+1]=200; frame_rgb[p+2]=0;
        }
        // draw V mean indicator (from last mask Vmean)
        int vx = (int)((Vmean_mask/1.0f) * barw);
        for(int xx=0; xx<vx; xx++){
            int px = (bary+26)*W + (barx+xx);
            int p = px*3; frame_rgb[p]=0; frame_rgb[p+1]=150; frame_rgb[p+2]=255;
        }
        // draw small status LED: green if stable_count >= STABLE_FRAMES_REQ
        int ledx = barx + barw + 8, ledy = bary;
        int led = (stable_count >= STABLE_FRAMES_REQ && comp_area >= AREA_MIN) ? 1 : 0;
        for(int yy=0; yy<8; yy++){
            for(int xx=0; xx<8; xx++){
                int px = (ledy+yy)*W + (ledx+xx);
                int p = px*3;
                if(led){ frame_rgb[p]=0; frame_rgb[p+1]=255; frame_rgb[p+2]=0; }
                else { frame_rgb[p]=80; frame_rgb[p+1]=80; frame_rgb[p+2]=80; }
            }
        }

        SDL_UpdateTexture(tex, NULL, frame_rgb, W*3);
        SDL_RenderClear(ren); SDL_RenderCopy(ren, tex, NULL, NULL); SDL_RenderPresent(ren);

        // events
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) running=0;
            if(e.type==SDL_KEYDOWN){
                if(e.key.keysym.sym=='q') running=0;
                if(e.key.keysym.sym=='e'){
                    if(!enrolling){
                        enrolling = 1; enroll_buf_idx = 0;
                        printf("Enrollment started: capturing %d stable frames\n", ENROLL_FRAMES);
                    } else {
                        enrolling = 0; enroll_buf_idx = 0;
                        printf("Enrollment cancelled\n");
                    }
                }
                if(e.key.keysym.sym=='s'){
                    if(save_model(MODEL_FILENAME)==0) printf("Model saved to %s\n", MODEL_FILENAME);
                    else printf("Save failed\n");
                }
                if(e.key.keysym.sym=='l'){
                    if(load_model(MODEL_FILENAME)==0) printf("Model loaded from %s\n", MODEL_FILENAME);
                    else printf("Load failed\n");
                }
            }
        }

        ioctl(fd, VIDIOC_QBUF, &buf);
    }

    // cleanup
    SDL_DestroyTexture(tex); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
    if(frame_rgb) free(frame_rgb);
    if(mask) free(mask);
    if(gray_res) free(gray_res);
    if(labels) free(labels);
    if(label_area) free(label_area);
    if(label_sumx) free(label_sumx);
    if(label_sumy) free(label_sumy);
    close(fd);
    return 0;
}

