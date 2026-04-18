// face_tracker_buck_style_v4l2_sdl.c
// From-scratch face tracking + piecewise-affine warp, using /dev/video0 and SDL2 for display.
// - Captures YUYV from /dev/video0 via V4L2
// - Converts to RGB24, grayscale
// - Initializes template landmarks via Shi-Tomasi inside an ellipse (tracked by simple ellipse fit from skin mask)
// - Tracks points with pyramidal Lucas-Kanade (sparse)
// - RANSAC affine estimation to reject outliers
// - Delaunay triangulation on template landmarks
// - Piecewise-affine warp of the initial template frame into current landmark positions
// - Displays processed output in an SDL2 window
//
// Build:
//   gcc -O2 face_tracker_buck_style_v4l2_sdl.c -o face_tracker -lSDL2 -lm
//
// Notes:
// - This is a single-file, from-scratch implementation intended as a working prototype.
// - For real production use, optimize (ROI, threading, SIMD), add robust error handling, and replace placeholders.
// - The program uses the first successfully captured frame as the "template" image and its detected features as template landmarks.
// - If no face/skin is found, it will keep showing the raw camera feed.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>

#define DEV "/dev/video0"
#define W 640
#define H 480

#define MAX_BUFFERS 4
#define MAX_FEATURES 200
#define LK_PYRAMID_LEVELS 3
#define LK_WIN 9
#define LK_ITERS 10
#define RANSAC_ITERS 200
#define RANSAC_THRESH 6.0f

// ---------- Basic types ----------
typedef struct { float x,y; } vec2;
typedef struct { int a,b,c; } tri_idx;

static inline float clampf(float v,float a,float b){ if(v<a) return a; if(v>b) return b; return v; }
static inline int clampi(int v,int a,int b){ if(v<a) return a; if(v>b) return b; return v; }

// ---------- V4L2 capture helpers ----------
struct buffer { void *start; size_t length; };
static struct buffer buffers[MAX_BUFFERS];
static int fd_v4l = -1;

static int v4l2_init(){
    fd_v4l = open(DEV, O_RDWR);
    if(fd_v4l < 0){ perror("open"); return -1; }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = W;
    fmt.fmt.pix.height = H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if(ioctl(fd_v4l, VIDIOC_S_FMT, &fmt) < 0){ perror("VIDIOC_S_FMT"); return -1; }

    struct v4l2_requestbuffers req = {0};
    req.count = MAX_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if(ioctl(fd_v4l, VIDIOC_REQBUFS, &req) < 0){ perror("VIDIOC_REQBUFS"); return -1; }

    for(int i=0;i<MAX_BUFFERS;i++){
        struct v4l2_buffer buf = {0};
        buf.type = req.type;
        buf.memory = req.memory;
        buf.index = i;
        if(ioctl(fd_v4l, VIDIOC_QUERYBUF, &buf) < 0){ perror("VIDIOC_QUERYBUF"); return -1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd_v4l, buf.m.offset);
        if(buffers[i].start == MAP_FAILED){ perror("mmap"); return -1; }
        if(ioctl(fd_v4l, VIDIOC_QBUF, &buf) < 0){ perror("VIDIOC_QBUF"); return -1; }
    }
    int type = req.type;
    if(ioctl(fd_v4l, VIDIOC_STREAMON, &type) < 0){ perror("VIDIOC_STREAMON"); return -1; }
    return 0;
}

// ---------- Pixel conversions ----------
static inline uint8_t clampi8(int v){ if(v<0) return 0; if(v>255) return 255; return v; }

void yuyv_to_rgb(uint8_t *src, uint8_t *dst){
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x+=2){
            int i = (y*W + x)*2;
            int Y0 = src[i+0], U = src[i+1], Y1 = src[i+2], V = src[i+3];
            int C = Y0 - 16, D = U - 128, E = V - 128;
            int R = (298*C + 409*E + 128) >> 8;
            int G = (298*C - 100*D - 208*E + 128) >> 8;
            int B = (298*C + 516*D + 128) >> 8;
            int p = (y*W + x)*3;
            dst[p+0] = clampi8(R); dst[p+1] = clampi8(G); dst[p+2] = clampi8(B);
            C = Y1 - 16;
            R = (298*C + 409*E + 128) >> 8;
            G = (298*C - 100*D - 208*E + 128) >> 8;
            B = (298*C + 516*D + 128) >> 8;
            p += 3;
            dst[p+0] = clampi8(R); dst[p+1] = clampi8(G); dst[p+2] = clampi8(B);
        }
    }
}

void rgb_to_gray(uint8_t *rgb, uint8_t *gray){
    for(int i=0;i<W*H;i++){
        int p = i*3;
        float r = rgb[p+0], g = rgb[p+1], b = rgb[p+2];
        gray[i] = (uint8_t)clampf(0.299f*r + 0.587f*g + 0.114f*b, 0, 255);
    }
}

// ---------- Simple skin mask (HSV-like approx using RGB) ----------
void build_skin_mask(uint8_t *rgb, uint8_t *mask){
    for(int i=0;i<W*H;i++){
        int p = i*3;
        float r = rgb[p+0]/255.0f, g = rgb[p+1]/255.0f, b = rgb[p+2]/255.0f;
        float mx = fmaxf(r, fmaxf(g,b));
        float mn = fminf(r, fminf(g,b));
        float v = mx;
        float s = (mx==0)?0:(mx-mn)/mx;
        float h = 0;
        if(mx==mn) h=0;
        else if(mx==r) h = 60.0f * fmodf((g-b)/(mx-mn),6.0f);
        else if(mx==g) h = 60.0f * ((b-r)/(mx-mn) + 2.0f);
        else h = 60.0f * ((r-g)/(mx-mn) + 4.0f);
        if(h<0) h+=360.0f;
        int skin = ((h<45.0f || h>330.0f) && s>0.15f && v>0.1f);
        mask[i] = skin ? 255 : 0;
    }
}

// ---------- Flood fill & ellipse fit (reuse earlier approach) ----------
int flood_fill_blob(uint8_t *m, uint8_t *vis, int sx, int sy, double *cx, double *cy, double *cxx, double *cyy, double *cxy){
    int Wn=W, Hn=H;
    int maxstack = Wn*Hn/8;
    int *stack = malloc(sizeof(int)*maxstack*2);
    if(!stack) return 0;
    int sp=0;
    stack[sp*2+0]=sx; stack[sp*2+1]=sy; sp++;
    vis[sy*Wn + sx] = 1;
    int count=0;
    double mx=0,my=0;
    while(sp){
        sp--;
        int x = stack[sp*2+0], y = stack[sp*2+1];
        mx += x; my += y; count++;
        for(int dy=-1; dy<=1; dy++){
            for(int dx=-1; dx<=1; dx++){
                int nx = x+dx, ny = y+dy;
                if(nx>=0 && nx<Wn && ny>=0 && ny<Hn){
                    int idx = ny*Wn + nx;
                    if(m[idx] && !vis[idx]){
                        vis[idx]=1;
                        if(sp < maxstack-1){
                            stack[sp*2+0]=nx; stack[sp*2+1]=ny; sp++;
                        }
                    }
                }
            }
        }
    }
    free(stack);
    if(count < 2000) return 0;
    *cx = mx/count; *cy = my/count;
    double xx=0, yy=0, xy=0;
    for(int y=0;y<Hn;y++){
        for(int x=0;x<Wn;x++){
            if(m[y*Wn + x]){
                double dx = x - *cx;
                double dy = y - *cy;
                xx += dx*dx; yy += dy*dy; xy += dx*dy;
            }
        }
    }
    *cxx = xx/count; *cyy = yy/count; *cxy = xy/count;
    return count;
}

int fit_ellipse_from_mask(uint8_t *mask, float *out_cx, float *out_cy, float *out_a, float *out_b, float *out_ang){
    static uint8_t *vis = NULL;
    if(!vis) vis = calloc(W*H,1);
    memset(vis,0,W*H);
    int best_area=0;
    double bc=0, br=0, bxx=0, byy=0, bxy=0;
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            int i = y*W + x;
            if(mask[i] && !vis[i]){
                double cx,cy,cxx,cyy,cxy;
                int area = flood_fill_blob(mask, vis, x, y, &cx, &cy, &cxx, &cyy, &cxy);
                if(area > best_area){
                    best_area = area;
                    bc = cx; br = cy; bxx = cxx; byy = cyy; bxy = cxy;
                }
            }
        }
    }
    if(best_area==0) return 0;
    double T = bxx + byy;
    double D = bxx*byy - bxy*bxy;
    double l1 = T/2 + sqrt(fmax(0, T*T/4 - D));
    double l2 = T/2 - sqrt(fmax(0, T*T/4 - D));
    *out_cx = bc; *out_cy = br;
    *out_a = sqrt(l1) * 2.2f;
    *out_b = sqrt(l2) * 2.2f;
    *out_ang = 0.5f * atan2(2*bxy, bxx - byy);
    return 1;
}

// ---------- Shi-Tomasi corner detection ----------
void compute_gradients(uint8_t *I, float *Ix, float *Iy){
    for(int y=1;y<H-1;y++){
        for(int x=1;x<W-1;x++){
            int p = y*W + x;
            float gx = (I[p+1] - I[p-1]) * 0.5f;
            float gy = (I[p+W] - I[p-W]) * 0.5f;
            Ix[p]=gx; Iy[p]=gy;
        }
    }
    // borders zero
    for(int x=0;x<W;x++){ Ix[x]=Iy[x]=0; Ix[(H-1)*W + x]=Iy[(H-1)*W + x]=0; }
    for(int y=0;y<H;y++){ Ix[y*W + 0]=Iy[y*W + 0]=0; Ix[y*W + (W-1)]=Iy[y*W + (W-1)]=0; }
}

int shi_tomasi(uint8_t *I, vec2 *out_pts, int max_pts, int win, float quality, uint8_t *mask){
    float *Ix = malloc(sizeof(float)*W*H);
    float *Iy = malloc(sizeof(float)*W*H);
    memset(Ix,0,sizeof(float)*W*H); memset(Iy,0,sizeof(float)*W*H);
    compute_gradients(I,Ix,Iy);
    float *score = malloc(sizeof(float)*W*H);
    memset(score,0,sizeof(float)*W*H);
    int half = win/2;
    for(int y=half;y<H-half;y++){
        for(int x=half;x<W-half;x++){
            if(mask && !mask[y*W + x]) { score[y*W + x]=0; continue; }
            float sxx=0, syy=0, sxy=0;
            for(int wy=-half; wy<=half; wy++){
                for(int wx=-half; wx<=half; wx++){
                    int p = (y+wy)*W + (x+wx);
                    float gx = Ix[p], gy = Iy[p];
                    sxx += gx*gx; syy += gy*gy; sxy += gx*gy;
                }
            }
            float T = sxx + syy;
            float D = sxx*syy - sxy*sxy;
            float val;
            float disc = T*T - 4*D;
            if(disc <= 0) val = T*0.5f;
            else val = 0.5f*(T - sqrtf(disc));
            score[y*W + x] = val;
        }
    }
    // threshold and NMS greedy
    float maxs = 0;
    for(int i=0;i<W*H;i++) if(score[i] > maxs) maxs = score[i];
    float thresh = maxs * quality;
    int found=0;
    int nms = win;
    for(;;){
        float best = thresh; int bx=-1, by=-1;
        for(int y=half;y<H-half;y++){
            for(int x=half;x<W-half;x++){
                float s = score[y*W + x];
                if(s > best){ best = s; bx = x; by = y; }
            }
        }
        if(bx<0) break;
        out_pts[found].x = bx; out_pts[found].y = by; found++;
        if(found >= max_pts) break;
        for(int yy=by-nms; yy<=by+nms; yy++){
            if(yy<0 || yy>=H) continue;
            for(int xx=bx-nms; xx<=bx+nms; xx++){
                if(xx<0 || xx>=W) continue;
                score[yy*W + xx] = 0;
            }
        }
    }
    free(Ix); free(Iy); free(score);
    return found;
}

// ---------- Simple pyramidal LK (sparse) ----------
typedef struct {
    uint8_t *data;
    int w,h;
} PyrLevel;

typedef struct {
    PyrLevel levels[LK_PYRAMID_LEVELS];
    int nlevels;
} Pyramid;

void build_pyramid_from_gray(uint8_t *gray, Pyramid *P, int levels){
    P->nlevels = levels;
    int w = W, h = H;
    P->levels[0].w = w; P->levels[0].h = h;
    P->levels[0].data = malloc(w*h);
    memcpy(P->levels[0].data, gray, w*h);
    for(int L=1; L<levels; L++){
        int pw = P->levels[L-1].w, ph = P->levels[L-1].h;
        int nw = pw/2, nh = ph/2;
        if(nw<8) nw=8; if(nh<8) nh=8;
        P->levels[L].w = nw; P->levels[L].h = nh;
        P->levels[L].data = malloc(nw*nh);
        // simple box downsample with 2x2 average
        for(int y=0;y<nh;y++){
            for(int x=0;x<nw;x++){
                int sx = x*2, sy = y*2;
                int sum = 0;
                for(int yy=0; yy<2; yy++) for(int xx=0; xx<2; xx++){
                    int ix = clampi(sx+xx,0,pw-1), iy = clampi(sy+yy,0,ph-1);
                    sum += P->levels[L-1].data[iy*pw + ix];
                }
                P->levels[L].data[y*nw + x] = (uint8_t)(sum/4);
            }
        }
    }
}

void free_pyramid(Pyramid *P){
    for(int L=0; L<P->nlevels; L++) free(P->levels[L].data);
}

static inline float sample_gray_p(const PyrLevel *L, float x, float y){
    int Wl = L->w, Hl = L->h;
    if(x<0) x=0; if(y<0) y=0; if(x>Wl-1) x=Wl-1; if(y>Hl-1) y=Hl-1;
    int x0 = (int)floorf(x), y0 = (int)floorf(y);
    int x1 = clampi(x0+1,0,Wl-1), y1 = clampi(y0+1,0,Hl-1);
    float dx = x - x0, dy = y - y0;
    float v00 = L->data[y0*Wl + x0];
    float v10 = L->data[y0*Wl + x1];
    float v01 = L->data[y1*Wl + x0];
    float v11 = L->data[y1*Wl + x1];
    return v00*(1-dx)*(1-dy) + v10*dx*(1-dy) + v01*(1-dx)*dy + v11*dx*dy;
}

int lk_single_point(const PyrLevel *I1, const PyrLevel *I2, float x, float y, float *dx, float *dy, int win, int max_iters){
    float u=0, v=0;
    int half = win/2;
    for(int iter=0; iter<max_iters; iter++){
        float A11=0,A12=0,A22=0,b1=0,b2=0;
        for(int wy=-half; wy<=half; wy++){
            for(int wx=-half; wx<=half; wx++){
                float px = x + wx;
                float py = y + wy;
                float I1v = sample_gray_p(I1, px, py);
                float I2v = sample_gray_p(I2, px + u, py + v);
                float gx = (sample_gray_p(I1, px+1, py) - sample_gray_p(I1, px-1, py)) * 0.5f;
                float gy = (sample_gray_p(I1, px, py+1) - sample_gray_p(I1, px, py-1)) * 0.5f;
                float It = I2v - I1v;
                A11 += gx*gx; A12 += gx*gy; A22 += gy*gy;
                b1 += gx * It; b2 += gy * It;
            }
        }
        float det = A11*A22 - A12*A12;
        if(fabs(det) < 1e-6f) break;
        float inv11 = A22/det, inv12 = -A12/det, inv22 = A11/det;
        float du = -(inv11*b1 + inv12*b2);
        float dv = -(inv12*b1 + inv22*b2);
        u += du; v += dv;
        if(sqrtf(du*du + dv*dv) < 0.01f) break;
    }
    *dx = u; *dy = v;
    float fx = x + *dx, fy = y + *dy;
    if(fx < 0 || fy < 0 || fx >= I2->w || fy >= I2->h) return 0;
    return 1;
}

int lk_pyr_track(Pyramid *Pprev, Pyramid *Pcur, vec2 *pts_prev, vec2 *pts_cur, int n){
    int L = Pprev->nlevels;
    for(int i=0;i<n;i++) pts_cur[i] = pts_prev[i];
    for(int lvl = L-1; lvl>=0; lvl--){
        const PyrLevel *I1 = &Pprev->levels[lvl];
        const PyrLevel *I2 = &Pcur->levels[lvl];
        float scale = 1.0f / (1<<lvl);
        for(int i=0;i<n;i++){
            if(pts_cur[i].x < -9000) continue;
            float x = pts_cur[i].x * scale;
            float y = pts_cur[i].y * scale;
            float dx=0, dy=0;
            int ok = lk_single_point(I1, I2, x, y, &dx, &dy, LK_WIN, LK_ITERS);
            if(!ok){ pts_cur[i].x = -99999; pts_cur[i].y = -99999; continue; }
            pts_cur[i].x = (x + dx) / scale;
            pts_cur[i].y = (y + dy) / scale;
        }
    }
    int valid=0;
    for(int i=0;i<n;i++) if(pts_cur[i].x > -1000) valid++;
    return valid;
}

// ---------- RANSAC affine ----------
void estimate_affine_3(const vec2 *p1, const vec2 *p2, float A[2][3]){
    // Solve 6x6 linear system for 3 point correspondences
    float X[6][6] = {0}, B[6] = {0};
    for(int i=0;i<3;i++){
        float x = p1[i].x, y = p1[i].y;
        int r = i*2;
        X[r][0] = x; X[r][1] = y; X[r][2] = 1; X[r][3]=0; X[r][4]=0; X[r][5]=0; B[r]=p2[i].x;
        X[r+1][0]=0; X[r+1][1]=0; X[r+1][2]=0; X[r+1][3]=x; X[r+1][4]=y; X[r+1][5]=1; B[r+1]=p2[i].y;
    }
    int N=6;
    float M[6][7];
    for(int i=0;i<N;i++){ for(int j=0;j<N;j++) M[i][j]=X[i][j]; M[i][N]=B[i]; }
    for(int i=0;i<N;i++){
        int piv=i;
        for(int r=i+1;r<N;r++) if(fabs(M[r][i])>fabs(M[piv][i])) piv=r;
        if(piv!=i) for(int c=i;c<=N;c++){ float t=M[i][c]; M[i][c]=M[piv][c]; M[piv][c]=t; }
        float diag = M[i][i];
        if(fabs(diag) < 1e-12) continue;
        for(int c=i;c<=N;c++) M[i][c] /= diag;
        for(int r=0;r<N;r++) if(r!=i){
            float f = M[r][i];
            for(int c=i;c<=N;c++) M[r][c] -= f * M[i][c];
        }
    }
    float sol[6];
    for(int i=0;i<N;i++) sol[i] = M[i][N];
    A[0][0]=sol[0]; A[0][1]=sol[1]; A[0][2]=sol[2];
    A[1][0]=sol[3]; A[1][1]=sol[4]; A[1][2]=sol[5];
}

int ransac_affine(vec2 *src, vec2 *dst, int n, float Aout[2][3], uint8_t *inlier_mask){
    if(n < 3) return 0;
    int best_in = 0;
    float bestA[2][3];
    srand((unsigned)time(NULL));
    for(int it=0; it<RANSAC_ITERS; it++){
        int i0 = rand()%n, i1 = rand()%n, i2 = rand()%n;
        if(i1==i0 || i2==i0 || i2==i1) continue;
        vec2 s[3] = { src[i0], src[i1], src[i2] };
        vec2 d[3] = { dst[i0], dst[i1], dst[i2] };
        float A[2][3];
        estimate_affine_3(s,d,A);
        int cnt=0;
        for(int i=0;i<n;i++){
            float x = src[i].x, y = src[i].y;
            float rx = A[0][0]*x + A[0][1]*y + A[0][2];
            float ry = A[1][0]*x + A[1][1]*y + A[1][2];
            float err = hypotf(rx - dst[i].x, ry - dst[i].y);
            if(err <= RANSAC_THRESH) cnt++;
        }
        if(cnt > best_in){
            best_in = cnt;
            memcpy(bestA, A, sizeof(bestA));
        }
    }
    if(best_in == 0) return 0;
    for(int i=0;i<n;i++){
        float x = src[i].x, y = src[i].y;
        float rx = bestA[0][0]*x + bestA[0][1]*y + bestA[0][2];
        float ry = bestA[1][0]*x + bestA[1][1]*y + bestA[1][2];
        float err = hypotf(rx - dst[i].x, ry - dst[i].y);
        inlier_mask[i] = (err <= RANSAC_THRESH) ? 1 : 0;
    }
    memcpy(Aout, bestA, sizeof(bestA));
    return best_in;
}

// ---------- Delaunay triangulation (Bowyer-Watson simplified) ----------
typedef struct { float x,y; } P2;
typedef struct { int a,b,c; } Tri;

static float circum_x(const P2 *a, const P2 *b, const P2 *c){
    float A = b->x - a->x, B = b->y - a->y;
    float C = c->x - a->x, D = c->y - a->y;
    float E = A*(a->x + b->x) + B*(a->y + b->y);
    float F = C*(a->x + c->x) + D*(a->y + c->y);
    float G = 2.0f*(A*(c->y - b->y) - B*(c->x - b->x));
    if(fabs(G) < 1e-12) return 0;
    return (D*E - B*F) / G;
}
static float circum_y(const P2 *a, const P2 *b, const P2 *c){
    float A = b->x - a->x, B = b->y - a->y;
    float C = c->x - a->x, D = c->y - a->y;
    float E = A*(a->x + b->x) + B*(a->y + b->y);
    float F = C*(a->x + c->x) + D*(a->y + c->y);
    float G = 2.0f*(A*(c->y - b->y) - B*(c->x - b->x));
    if(fabs(G) < 1e-12) return 0;
    return (A*F - C*E) / G;
}

Tri *delaunay(P2 *pts, int npts, int *out_ntri){
    float minx=pts[0].x, miny=pts[0].y, maxx=minx, maxy=miny;
    for(int i=1;i<npts;i++){ if(pts[i].x<minx) minx=pts[i].x; if(pts[i].y<miny) miny=pts[i].y; if(pts[i].x>maxx) maxx=pts[i].x; if(pts[i].y>maxy) maxy=pts[i].y; }
    float dx = maxx - minx, dy = maxy - miny;
    float dmax = fmaxf(dx,dy);
    P2 p1 = { minx - 10*dmax, miny - dmax };
    P2 p2 = { minx + 0.5f*dmax, maxy + 10*dmax };
    P2 p3 = { maxx + 10*dmax, miny - dmax };
    // dynamic triangle list
    Tri *tris = malloc(sizeof(Tri)*(npts*8 + 10));
    int ntri = 0;
    // super triangle stored with negative indices -1,-2,-3 encoded as -1,-2,-3
    tris[ntri].a = -1; tris[ntri].b = -2; tris[ntri].c = -3; ntri++;
    // store circumcenters separately in arrays
    float *circx = malloc(sizeof(float)*(npts*8 + 10));
    float *circy = malloc(sizeof(float)*(npts*8 + 10));
    circx[0] = circum_x(&p1,&p2,&p3); circy[0] = circum_y(&p1,&p2,&p3);
    for(int i=0;i<npts;i++){
        P2 pi = pts[i];
        int *bad = malloc(sizeof(int)*ntri);
        int nbad=0;
        for(int t=0;t<ntri;t++){
            float cx = circx[t], cy = circy[t];
            float dx = cx - pi.x, dy = cy - pi.y;
            // approximate radius squared by distance to one vertex (we don't store r2; use large margin)
            if(dx*dx + dy*dy <= 1e12f){ // conservative: mark many triangles; we'll filter later
                // compute circumcircle precisely for triangle vertices
                // get triangle vertices
                int ia = tris[t].a, ib = tris[t].b, ic = tris[t].c;
                P2 pa = (ia>=0)? pts[ia] : (ia==-1? p1 : (ia==-2? p2 : p3));
                P2 pb = (ib>=0)? pts[ib] : (ib==-1? p1 : (ib==-2? p2 : p3));
                P2 pc = (ic>=0)? pts[ic] : (ic==-1? p1 : (ic==-2? p2 : p3));
                float cx2 = circum_x(&pa,&pb,&pc), cy2 = circum_y(&pa,&pb,&pc);
                float r2 = (cx2 - pa.x)*(cx2 - pa.x) + (cy2 - pa.y)*(cy2 - pa.y);
                float dx2 = cx2 - pi.x, dy2 = cy2 - pi.y;
                if(dx2*dx2 + dy2*dy2 <= r2 + 1e-6f){
                    bad[nbad++] = t;
                }
            }
        }
        // collect boundary edges
        typedef struct { int a,b; } Edge;
        Edge *edges = malloc(sizeof(Edge)*nbad*3);
        int nedges=0;
        for(int bi=0; bi<nbad; bi++){
            Tri T = tris[bad[bi]];
            int idxs[3] = {T.a, T.b, T.c};
            for(int e=0;e<3;e++){
                int a = idxs[e], b = idxs[(e+1)%3];
                int shared=0;
                for(int bj=0; bj<nbad; bj++){
                    if(bj==bi) continue;
                    Tri T2 = tris[bad[bj]];
                    int idxs2[3] = {T2.a, T2.b, T2.c};
                    for(int e2=0;e2<3;e2++){
                        int a2 = idxs2[e2], b2 = idxs2[(e2+1)%3];
                        if((a==a2 && b==b2) || (a==b2 && b==a2)){ shared=1; break; }
                    }
                    if(shared) break;
                }
                if(!shared){ edges[nedges].a = a; edges[nedges].b = b; nedges++; }
            }
        }
        // remove bad triangles by marking
        for(int bi=0; bi<nbad; bi++){
            int t = bad[bi];
            tris[t].a = tris[t].b = tris[t].c = -9999;
        }
        // add new triangles from edges to point i
        for(int e=0;e<nedges;e++){
            Tri nt; nt.a = edges[e].a; nt.b = edges[e].b; nt.c = i;
            // compute circumcenter for nt
            P2 pa = (nt.a>=0)? pts[nt.a] : (nt.a==-1? p1 : (nt.a==-2? p2 : p3));
            P2 pb = (nt.b>=0)? pts[nt.b] : (nt.b==-1? p1 : (nt.b==-2? p2 : p3));
            P2 pc = (nt.c>=0)? pts[nt.c] : (nt.c==-1? p1 : (nt.c==-2? p2 : p3));
            tris[ntri] = nt;
            circx[ntri] = circum_x(&pa,&pb,&pc);
            circy[ntri] = circum_y(&pa,&pb,&pc);
            ntri++;
        }
        free(bad); free(edges);
        // compact
        int write=0;
        for(int t=0;t<ntri;t++){
            if(tris[t].a == -9999) continue;
            if(write!=t){ tris[write]=tris[t]; circx[write]=circx[t]; circy[write]=circy[t]; }
            write++;
        }
        ntri = write;
    }
    // remove triangles that reference super-triangle vertices (-1,-2,-3)
    int write=0;
    for(int t=0;t<ntri;t++){
        Tri T = tris[t];
        if(T.a<0 || T.b<0 || T.c<0) continue;
        tris[write++] = T;
    }
    ntri = write;
    Tri *out = malloc(sizeof(Tri)*ntri);
    memcpy(out, tris, sizeof(Tri)*ntri);
    free(tris); free(circx); free(circy);
    *out_ntri = ntri;
    return out;
}

// ---------- Piecewise-affine warp ----------
void estimate_affine_from_tri(const vec2 *src_pts, const vec2 *dst_pts, const Tri *tri, float M[3][3]){
    // compute 3x3 affine matrix mapping src->dst (dst = M * [src;1])
    float X[6][6] = {0}, B[6] = {0};
    for(int i=0;i<3;i++){
        float sx = src_pts[(i==0?tri->a:(i==1?tri->b:tri->c))].x;
        float sy = src_pts[(i==0?tri->a:(i==1?tri->b:tri->c))].y;
        float dx = dst_pts[(i==0?tri->a:(i==1?tri->b:tri->c))].x;
        float dy = dst_pts[(i==0?tri->a:(i==1?tri->b:tri->c))].y;
        int r = i*2;
        X[r][0]=sx; X[r][1]=sy; X[r][2]=1; X[r][3]=0; X[r][4]=0; X[r][5]=0; B[r]=dx;
        X[r+1][0]=0; X[r+1][1]=0; X[r+1][2]=0; X[r+1][3]=sx; X[r+1][4]=sy; X[r+1][5]=1; B[r+1]=dy;
    }
    int N=6;
    float Msys[6][7];
    for(int i=0;i<N;i++){ for(int j=0;j<N;j++) Msys[i][j]=X[i][j]; Msys[i][N]=B[i]; }
    for(int i=0;i<N;i++){
        int piv=i;
        for(int r=i+1;r<N;r++) if(fabs(Msys[r][i])>fabs(Msys[piv][i])) piv=r;
        if(piv!=i) for(int c=i;c<=N;c++){ float t=Msys[i][c]; Msys[i][c]=Msys[piv][c]; Msys[piv][c]=t; }
        float diag = Msys[i][i];
        if(fabs(diag) < 1e-12) continue;
        for(int c=i;c<=N;c++) Msys[i][c] /= diag;
        for(int r=0;r<N;r++) if(r!=i){
            float f = Msys[r][i];
            for(int c=i;c<=N;c++) Msys[r][c] -= f * Msys[i][c];
        }
    }
    float sol[6];
    for(int i=0;i<N;i++) sol[i] = Msys[i][N];
    // fill 3x3 matrix
    M[0][0]=sol[0]; M[0][1]=sol[1]; M[0][2]=sol[2];
    M[1][0]=sol[3]; M[1][1]=sol[4]; M[1][2]=sol[5];
    M[2][0]=0; M[2][1]=0; M[2][2]=1;
}

static inline void bilinear_sample_rgb(uint8_t *src, int sw, int sh, float sx, float sy, uint8_t *out_rgb){
    if(sx < 0 || sy < 0 || sx >= sw-1 || sy >= sh-1){ out_rgb[0]=out_rgb[1]=out_rgb[2]=0; return; }
    int x0 = (int)floorf(sx), y0 = (int)floorf(sy);
    int x1 = x0+1, y1 = y0+1;
    float dx = sx - x0, dy = sy - y0;
    int p00 = (y0*sw + x0)*3, p10 = (y0*sw + x1)*3, p01 = (y1*sw + x0)*3, p11 = (y1*sw + x1)*3;
    for(int c=0;c<3;c++){
        float v00 = src[p00 + c], v10 = src[p10 + c], v01 = src[p01 + c], v11 = src[p11 + c];
        float v = v00*(1-dx)*(1-dy) + v10*dx*(1-dy) + v01*(1-dx)*dy + v11*dx*dy;
        out_rgb[c] = (uint8_t)clampf(v,0,255);
    }
}

void warp_piecewise_affine(uint8_t *src_rgb, int sw, int sh, uint8_t *dst_rgb, int dw, int dh,
                           vec2 *src_pts, vec2 *dst_pts, Tri *tris, int ntri)
{
    // clear dst
    for(int i=0;i<dw*dh*3;i++) dst_rgb[i]=0;
    // for each triangle, compute affine mapping from src->dst, then for each pixel in dst triangle map back
    for(int t=0;t<ntri;t++){
        Tri tri = tris[t];
        // bounding box in dst
        float minx = fminf(fminf(dst_pts[tri.a].x, dst_pts[tri.b].x), dst_pts[tri.c].x);
        float miny = fminf(fminf(dst_pts[tri.a].y, dst_pts[tri.b].y), dst_pts[tri.c].y);
        float maxx = fmaxf(fmaxf(dst_pts[tri.a].x, dst_pts[tri.b].x), dst_pts[tri.c].x);
        float maxy = fmaxf(fmaxf(dst_pts[tri.a].y, dst_pts[tri.b].y), dst_pts[tri.c].y);
        int x0 = clampi((int)floorf(minx), 0, dw-1);
        int y0 = clampi((int)floorf(miny), 0, dh-1);
        int x1 = clampi((int)ceilf(maxx), 0, dw-1);
        int y1 = clampi((int)ceilf(maxy), 0, dh-1);
        // compute affine matrix M mapping src->dst: dst = M * [src;1]
        float M[3][3];
        estimate_affine_from_tri(src_pts, dst_pts, &tri, M);
        // invert M to map dst->src
        float det = M[0][0]*(M[1][1]*M[2][2]-M[1][2]*M[2][1]) - M[0][1]*(M[1][0]*M[2][2]-M[1][2]*M[2][0]) + M[0][2]*(M[1][0]*M[2][1]-M[1][1]*M[2][0]);
        if(fabs(det) < 1e-12) continue;
        float inv[3][3];
        inv[0][0] =  (M[1][1]*M[2][2]-M[1][2]*M[2][1]) / det;
        inv[0][1] = -(M[0][1]*M[2][2]-M[0][2]*M[2][1]) / det;
        inv[0][2] =  (M[0][1]*M[1][2]-M[0][2]*M[1][1]) / det;
        inv[1][0] = -(M[1][0]*M[2][2]-M[1][2]*M[2][0]) / det;
        inv[1][1] =  (M[0][0]*M[2][2]-M[0][2]*M[2][0]) / det;
        inv[1][2] = -(M[0][0]*M[1][2]-M[0][2]*M[1][0]) / det;
        inv[2][0] =  (M[1][0]*M[2][1]-M[1][1]*M[2][0]) / det;
        inv[2][1] = -(M[0][0]*M[2][1]-M[0][1]*M[2][0]) / det;
        inv[2][2] =  (M[0][0]*M[1][1]-M[0][1]*M[1][0]) / det;
        // barycentric denom for triangle test
        float xA = dst_pts[tri.a].x, yA = dst_pts[tri.a].y;
        float xB = dst_pts[tri.b].x, yB = dst_pts[tri.b].y;
        float xC = dst_pts[tri.c].x, yC = dst_pts[tri.c].y;
        float denom = (yB - yC)*(xA - xC) + (xC - xB)*(yA - yC);
        if(fabs(denom) < 1e-9) continue;
        for(int y=y0;y<=y1;y++){
            for(int x=x0;x<=x1;x++){
                float w1 = ((yB - yC)*(x - xC) + (xC - xB)*(y - yC)) / denom;
                float w2 = ((yC - yA)*(x - xC) + (xA - xC)*(y - yC)) / denom;
                float w3 = 1.0f - w1 - w2;
                if(w1 < -1e-4f || w2 < -1e-4f || w3 < -1e-4f) continue;
                // map (x,y,1) via inv to source coords
                float sx = inv[0][0]*x + inv[0][1]*y + inv[0][2];
                float sy = inv[1][0]*x + inv[1][1]*y + inv[1][2];
                uint8_t sample[3];
                bilinear_sample_rgb(src_rgb, sw, sh, sx, sy, sample);
                int dp = (y*dw + x)*3;
                dst_rgb[dp+0] = sample[0];
                dst_rgb[dp+1] = sample[1];
                dst_rgb[dp+2] = sample[2];
            }
        }
    }
}

// ---------- Main application ----------
int main(int argc, char **argv){
    if(v4l2_init() < 0) return 1;
    if(SDL_Init(SDL_INIT_VIDEO) != 0){ fprintf(stderr,"SDL_Init: %s\n", SDL_GetError()); return 1; }
    SDL_Window *win = SDL_CreateWindow("Face Tracker Buck-style (no OpenCV)",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
    if(!win){ fprintf(stderr,"SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, W, H);

    uint8_t *frame_rgb = malloc(W*H*3);
    uint8_t *gray = malloc(W*H);
    uint8_t *mask = malloc(W*H);
    uint8_t *vis = calloc(W*H,1);

    // template storage (first good frame)
    uint8_t *templ_rgb = NULL;
    uint8_t *templ_gray = NULL;
    vec2 templ_pts[MAX_FEATURES];
    int templ_npts = 0;
    Tri *tris = NULL;
    int ntri = 0;

    // tracking state
    vec2 prev_pts[MAX_FEATURES], cur_pts[MAX_FEATURES];
    int npts = 0;
    Pyramid Pprev, Pcur;
    int have_prev_gray = 0;

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    int running = 1;
    while(running){
        if(ioctl(fd_v4l, VIDIOC_DQBUF, &buf) < 0) break;
        yuyv_to_rgb(buffers[buf.index].start, frame_rgb);
        rgb_to_gray(frame_rgb, gray);
        build_skin_mask(frame_rgb, mask);

        // fit ellipse to skin mask to get ROI center/axes
        float cx,cy,a,b,ang;
        int have_ellipse = fit_ellipse_from_mask(mask, &cx, &cy, &a, &b, &ang);

        // initialize template on first good frame: detect features inside ellipse
        if(!templ_rgb && have_ellipse){
            templ_rgb = malloc(W*H*3);
            templ_gray = malloc(W*H);
            memcpy(templ_rgb, frame_rgb, W*H*3);
            memcpy(templ_gray, gray, W*H);
            // build mask for detection: only inside ellipse
            uint8_t *ellmask = malloc(W*H);
            for(int y=0;y<H;y++){
                for(int x=0;x<W;x++){
                    float rx = x - cx, ry = y - cy;
                    float ux =  cosf(ang)*rx + sinf(ang)*ry;
                    float uy = -sinf(ang)*rx + cosf(ang)*ry;
                    float val = (ux*ux)/(a*a + 1e-6f) + (uy*uy)/(b*b + 1e-6f);
                    ellmask[y*W + x] = (val <= 1.0f) ? 255 : 0;
                }
            }
            templ_npts = shi_tomasi(templ_gray, templ_pts, MAX_FEATURES, 9, 0.01f, ellmask);
            free(ellmask);
            if(templ_npts < 10){
                // fallback: detect without mask
                templ_npts = shi_tomasi(templ_gray, templ_pts, MAX_FEATURES, 9, 0.01f, NULL);
            }
            if(templ_npts > 3){
                // triangulate template points
                P2 *p2 = malloc(sizeof(P2)*templ_npts);
                for(int i=0;i<templ_npts;i++){ p2[i].x = templ_pts[i].x; p2[i].y = templ_pts[i].y; }
                tris = delaunay(p2, templ_npts, &ntri);
                free(p2);
                // initialize tracking points
                npts = templ_npts;
                for(int i=0;i<npts;i++) prev_pts[i] = templ_pts[i];
                // build prev pyramid
                build_pyramid_from_gray(templ_gray, &Pprev, LK_PYRAMID_LEVELS);
                have_prev_gray = 1;
                printf("Template initialized with %d features, %d triangles\n", npts, ntri);
            } else {
                free(templ_rgb); templ_rgb = NULL;
                free(templ_gray); templ_gray = NULL;
            }
        }

        // if we have template and previous pyramid, track features
        if(templ_rgb && have_prev_gray && npts>0){
            // build current pyramid
            build_pyramid_from_gray(gray, &Pcur, LK_PYRAMID_LEVELS);
            // track
            for(int i=0;i<npts;i++) prev_pts[i] = prev_pts[i]; // prev_pts already set
            int valid = lk_pyr_track(&Pprev, &Pcur, prev_pts, cur_pts, npts);
            // build arrays for ransac
            vec2 src[MAX_FEATURES], dst[MAX_FEATURES];
            int m = 0;
            for(int i=0;i<npts;i++){
                if(cur_pts[i].x > -1000){
                    src[m] = templ_pts[i];
                    dst[m] = cur_pts[i];
                    m++;
                }
            }
            uint8_t inlier_mask[MAX_FEATURES];
            memset(inlier_mask,0,sizeof(inlier_mask));
            float A[2][3];
            int nin = ransac_affine(src, dst, m, A, inlier_mask);
            // compute current landmarks by applying A to template landmarks
            vec2 cur_landmarks[MAX_FEATURES];
            for(int i=0;i<templ_npts;i++){
                float x = templ_pts[i].x, y = templ_pts[i].y;
                cur_landmarks[i].x = A[0][0]*x + A[0][1]*y + A[0][2];
                cur_landmarks[i].y = A[1][0]*x + A[1][1]*y + A[1][2];
            }
            // warp template image to current landmarks
            uint8_t *warped = malloc(W*H*3);
            warp_piecewise_affine(templ_rgb, W, H, warped, W, H, templ_pts, cur_landmarks, tris, ntri);
            // composite: blend warped over current frame
            for(int i=0;i<W*H;i++){
                int p = i*3;
                // simple alpha blend
                float alpha = 0.8f;
                frame_rgb[p+0] = (uint8_t)clampf(frame_rgb[p+0]*(1-alpha) + warped[p+0]*alpha, 0, 255);
                frame_rgb[p+1] = (uint8_t)clampf(frame_rgb[p+1]*(1-alpha) + warped[p+1]*alpha, 0, 255);
                frame_rgb[p+2] = (uint8_t)clampf(frame_rgb[p+2]*(1-alpha) + warped[p+2]*alpha, 0, 255);
            }
            free(warped);
            // prepare for next frame: set prev pyramid = current pyramid, prev_pts = cur_pts
            free_pyramid(&Pprev);
            Pprev = Pcur; // shallow copy of levels pointers is fine; we won't free Pcur separately
            for(int i=0;i<npts;i++) prev_pts[i] = cur_pts[i];
            have_prev_gray = 1;
        }

        // draw ellipse outline (simple green circle stamps along ellipse)
        if(have_ellipse){
            for(int t=0;t<360;t+=4){
                double rads = t * M_PI / 180.0;
                double ex = a * cos(rads);
                double ey = b * sin(rads);
                double rx = ex * cos(ang) - ey * sin(ang);
                double ry = ex * sin(ang) + ey * cos(ang);
                int px = (int)round(cx + rx);
                int py = (int)round(cy + ry);
                for(int yy=-2; yy<=2; yy++){
                    for(int xx=-2; xx<=2; xx++){
                        int x = px + xx, y = py + yy;
                        if(x>=0 && x<W && y>=0 && y<H){
                            int p = (y*W + x)*3;
                            frame_rgb[p+0]=0; frame_rgb[p+1]=255; frame_rgb[p+2]=0;
                        }
                    }
                }
            }
        }

        SDL_UpdateTexture(tex, NULL, frame_rgb, W*3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        // event handling
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_q) running = 0;
            }
        }

        if(ioctl(fd_v4l, VIDIOC_QBUF, &buf) < 0) break;
    }

    // cleanup
    if(fd_v4l>=0) close(fd_v4l);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(frame_rgb); free(gray); free(mask); free(vis);
    if(templ_rgb) free(templ_rgb);
    if(templ_gray) free(templ_gray);
    if(tris) free(tris);
    // free pyramid levels if any
    if(have_prev_gray){
        for(int L=0; L<Pprev.nlevels; L++) free(Pprev.levels[L].data);
    }
    return 0;
}
