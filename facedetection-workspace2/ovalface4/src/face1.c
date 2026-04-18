// face_tracker_chanvese.c
// Face tracker + ellipse + Chan-Vese level-set inside ellipse ROI
// Compile: gcc -O2 face_tracker_chanvese.c -o face_tracker -lSDL2 -lm

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <string.h>

#define DEV "/dev/video0"
#define W 640
#define H 480

#define MAX_BUFFERS 4
#define TRACK_SMOOTH 0.80f

// Chan-Vese params (tune for speed/quality)
#define CV_ITER 120
#define CV_DT 0.5f
#define CV_MU 0.2f
#define CV_LAMBDA1 1.0f
#define CV_LAMBDA2 1.0f

// Downscale factor for ROI processing (1.0 = no downscale, 0.5 = half)
#define DOWNSCALE 0.5f

struct buffer { void *start; size_t length; };
static struct buffer buffers[MAX_BUFFERS];
static int fd=-1;

static uint8_t *frame_rgb;
static uint8_t *mask;

// tracker state
static float t_cx=0, t_cy=0;
static float t_a=0, t_b=0;
static float t_angle=0;
static int tracker_init=0;

static inline uint8_t clampi(int v){ if(v<0) return 0; if(v>255) return 255; return v; }

//////////////////// YUYV → RGB ////////////////////

static void yuyv_to_rgb(uint8_t *src,uint8_t *dst){
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x+=2){
            int i=(y*W+x)*2;
            int Y0=src[i+0],U=src[i+1],Y1=src[i+2],V=src[i+3];

            int C=Y0-16,D=U-128,E=V-128;
            int R=(298*C+409*E+128)>>8;
            int G=(298*C-100*D-208*E+128)>>8;
            int B=(298*C+516*D+128)>>8;
            int p=(y*W+x)*3;
            dst[p]=clampi(R); dst[p+1]=clampi(G); dst[p+2]=clampi(B);

            C=Y1-16;
            R=(298*C+409*E+128)>>8;
            G=(298*C-100*D-208*E+128)>>8;
            B=(298*C+516*D+128)>>8;
            p+=3;
            dst[p]=clampi(R); dst[p+1]=clampi(G); dst[p+2]=clampi(B);
        }
    }
}

//////////////////// RGB → HSV (for optional skin mask) ////////////////////

static void rgb_to_hsv(uint8_t r,uint8_t g,uint8_t b,float *h,float *s,float *v){
    float rf=r/255.f,gf=g/255.f,bf=b/255.f;
    float mx=fmaxf(rf,fmaxf(gf,bf));
    float mn=fminf(rf,fminf(gf,bf));
    float d=mx-mn;

    *v=mx;
    *s=(mx==0)?0:d/mx;

    if(d==0){ *h=0; return; }

    if(mx==rf) *h=60*fmodf((gf-bf)/d,6);
    else if(mx==gf) *h=60*((bf-rf)/d+2);
    else *h=60*((rf-gf)/d+4);

    if(*h<0)*h+=360;
}

//////////////////// Skin mask (optional) ////////////////////

static void build_skin_mask(uint8_t *rgb,uint8_t *m){
    for(int i=0;i<W*H;i++){
        int p=i*3;
        float h,s,v;
        rgb_to_hsv(rgb[p],rgb[p+1],rgb[p+2],&h,&s,&v);

        int skin = ((h<45 || h>330) && s>0.15 && v>0.1);
        m[i]=skin?255:0;
    }
}

//////////////////// Largest blob (flood fill) ////////////////////

static int flood_fill(uint8_t *m,uint8_t *vis,int sx,int sy,
                      double *cx,double *cy,
                      double *cxx,double *cyy,double *cxy)
{
    int stack_size=0;
    static int *stack = NULL;
    if(!stack) stack = malloc(sizeof(int) * (W*H/8) * 2);
    stack[stack_size*2+0]=sx;
    stack[stack_size*2+1]=sy;
    stack_size++;

    vis[sy*W+sx]=1;

    int count=0;
    double mx=0,my=0;

    while(stack_size){
        stack_size--;
        int x=stack[stack_size*2+0];
        int y=stack[stack_size*2+1];

        mx+=x; my+=y;
        count++;

        for(int dy=-1;dy<=1;dy++){
            for(int dx=-1;dx<=1;dx++){
                int nx=x+dx,ny=y+dy;
                if(nx>=0 && nx<W && ny>=0 && ny<H){
                    int idx=ny*W+nx;
                    if(m[idx] && !vis[idx]){
                        vis[idx]=1;
                        stack[stack_size*2+0]=nx;
                        stack[stack_size*2+1]=ny;
                        stack_size++;
                    }
                }
            }
        }
    }

    if(count<2000) return 0;

    *cx=mx/count;
    *cy=my/count;

    double xx=0,yy=0,xy=0;
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            if(m[y*W+x]){
                double dx=x-*cx;
                double dy=y-*cy;
                xx+=dx*dx;
                yy+=dy*dy;
                xy+=dx*dy;
            }
        }
    }

    *cxx=xx/count;
    *cyy=yy/count;
    *cxy=xy/count;

    return count;
}

//////////////////// Ellipse fitting ////////////////////

static int fit_face(uint8_t *m,
                    float *cx,float *cy,
                    float *a,float *b,float *ang)
{
    static uint8_t *vis=NULL;
    if(!vis) vis=calloc(W*H,1);
    memset(vis,0,W*H);

    int best_area=0;
    double bc=0,br=0,bxx=0,byy=0,bxy=0;

    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            int i=y*W+x;
            if(m[i] && !vis[i]){
                double cx0,cy0,cxx,cyy,cxy;
                int area=flood_fill(m,vis,x,y,&cx0,&cy0,&cxx,&cyy,&cxy);
                if(area>best_area){
                    best_area=area;
                    bc=cx0; br=cy0;
                    bxx=cxx; byy=cyy; bxy=cxy;
                }
            }
        }
    }

    if(best_area==0) return 0;

    double T=bxx+byy;
    double D=bxx*byy-bxy*bxy;

    double l1=T/2+sqrt(fmax(0,T*T/4-D));
    double l2=T/2-sqrt(fmax(0,T*T/4-D));

    *cx=bc;
    *cy=br;
    *a=sqrt(l1)*2.2;
    *b=sqrt(l2)*2.2;
    *ang=0.5*atan2(2*bxy,bxx-byy);

    return 1;
}

//////////////////// Smoothing ////////////////////

static void tracker_update(float cx,float cy,float a,float b,float ang){
    if(!tracker_init){
        t_cx=cx; t_cy=cy; t_a=a; t_b=b; t_angle=ang;
        tracker_init=1;
        return;
    }

    t_cx=t_cx*TRACK_SMOOTH + cx*(1-TRACK_SMOOTH);
    t_cy=t_cy*TRACK_SMOOTH + cy*(1-TRACK_SMOOTH);
    t_a=t_a*TRACK_SMOOTH + a*(1-TRACK_SMOOTH);
    t_b=t_b*TRACK_SMOOTH + b*(1-TRACK_SMOOTH);
    t_angle=t_angle*TRACK_SMOOTH + ang*(1-TRACK_SMOOTH);
}

//////////////////// Antialiased brush with alpha blending ////////////////////

static inline float clampf(float v, float a, float b){
    if(v < a) return a;
    if(v > b) return b;
    return v;
}

static inline void blend_pixel(uint8_t *dst, uint8_t sr, uint8_t sg, uint8_t sb, float alpha){
    float inv = 1.0f - alpha;
    int r = (int)(dst[0]*inv + sr*alpha + 0.5f);
    int g = (int)(dst[1]*inv + sg*alpha + 0.5f);
    int b = (int)(dst[2]*inv + sb*alpha + 0.5f);
    dst[0] = clampi(r);
    dst[1] = clampi(g);
    dst[2] = clampi(b);
}

static void draw_brush_aa(float cx, float cy, uint8_t *rgb)
{
    const float r = 2.5f;            // Radius = 2.5 -> Durchmesser = 5px
    const float outer = r + 0.5f;    // Antialiasing falloff
    const float outer2 = outer * outer;
    const float denom = (outer - r) > 0.0f ? (outer - r) : 1.0f;

    int x0 = (int)floorf(cx - outer);
    int x1 = (int)ceilf (cx + outer);
    int y0 = (int)floorf(cy - outer);
    int y1 = (int)ceilf (cy + outer);

    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(x1 >= W) x1 = W-1;
    if(y1 >= H) y1 = H-1;

    const uint8_t br = 0, bg = 255, bb = 0; // brush color green

    for(int y = y0; y <= y1; y++){
        for(int x = x0; x <= x1; x++){
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float d2 = dx*dx + dy*dy;
            if(d2 <= outer2){
                float d = sqrtf(d2);
                float alpha;
                if(d <= r) alpha = 1.0f;
                else alpha = clampf((outer - d) / denom, 0.0f, 1.0f);
                int p = (y*W + x) * 3;
                blend_pixel(&rgb[p], br, bg, bb, alpha);
            }
        }
    }
}

static void draw_ellipse(uint8_t *rgb){
    const float step = 1.0f;
    for(float t=0.0f; t<360.0f; t += step){
        double r = t * M_PI / 180.0;
        double ex = t_a * cos(r);
        double ey = t_b * sin(r);

        double rx = ex * cos(t_angle) - ey * sin(t_angle);
        double ry = ex * sin(t_angle) + ey * cos(t_angle);

        float px = (float)(t_cx + rx);
        float py = (float)(t_cy + ry);

        if(px >= -4 && px <= W+4 && py >= -4 && py <= H+4){
            draw_brush_aa(px, py, rgb);
        }
    }
}

//////////////////// Chan-Vese Level-Set (ROI) ////////////////////

// Convert RGB ROI to grayscale float (0..1)
static void rgb_to_gray_float(uint8_t *rgb, float *gray, int w, int h, int stride_rgb){
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            int p_rgb = y*stride_rgb + x*3;
            float r = rgb[p_rgb+0]/255.0f;
            float g = rgb[p_rgb+1]/255.0f;
            float b = rgb[p_rgb+2]/255.0f;
            gray[y*w + x] = 0.299f*r + 0.587f*g + 0.114f*b;
        }
    }
}

// Initialize phi as signed distance to ellipse center/axes (negative inside)
static void init_phi_ellipse(float *phi, int w, int h, float cx, float cy, float a, float b, float ang){
    // cx,cy are in ROI coordinates
    float ca = cosf(ang), sa = sinf(ang);
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            float rx = x - cx;
            float ry = y - cy;
            // rotate back
            float ux =  ca*rx + sa*ry;
            float uy = -sa*rx + ca*ry;
            // normalized distance to ellipse boundary (approx)
            float val = sqrtf((ux*ux)/(a*a + 1e-8f) + (uy*uy)/(b*b + 1e-8f));
            // val < 1 inside
            phi[y*w + x] = val - 1.0f; // negative inside
        }
    }
}

// Simple explicit Chan-Vese update (not optimized). img in [0..1], phi signed distance approx
static void chan_vese(float *img, int w, int h, float *phi, uint8_t *out_mask,
                      int iterations, float dt, float mu, float lambda1, float lambda2)
{
    int N = w*h;
    float *phi_new = malloc(sizeof(float)*N);
    if(!phi_new) return;

    for(int it=0; it<iterations; ++it){
        // compute means c1 (inside) and c2 (outside)
        double sum_in=0, sum_out=0; int cnt_in=0, cnt_out=0;
        for(int i=0;i<N;i++){
            if(phi[i] <= 0){ sum_in += img[i]; cnt_in++; }
            else { sum_out += img[i]; cnt_out++; }
        }
        float c1 = cnt_in? (float)(sum_in/cnt_in) : 0.0f;
        float c2 = cnt_out? (float)(sum_out/cnt_out) : 0.0f;

        // update phi (skip border pixels)
        for(int y=1;y<h-1;y++){
            for(int x=1;x<w-1;x++){
                int i = y*w + x;
                float ph = phi[i];
                // central differences
                float phix = (phi[i+1]-phi[i-1])*0.5f;
                float phiy = (phi[i+w]-phi[i-w])*0.5f;
                float phixx = phi[i+1] - 2*phi[i] + phi[i-1];
                float phiyy = phi[i+w] - 2*phi[i] + phi[i-w];
                float phixy = (phi[i+w+1] - phi[i+w-1] - phi[i-w+1] + phi[i-w-1]) * 0.25f;
                float grad2 = phix*phix + phiy*phiy + 1e-8f;
                // curvature (approx)
                float num = phixx*phiy*phiy - 2*phix*phiy*phixy + phiyy*phix*phix;
                float kappa = num / (grad2*sqrtf(grad2) + 1e-12f);
                // data term (Chan-Vese)
                float data = -lambda1*(img[i]-c1)*(img[i]-c1) + lambda2*(img[i]-c2)*(img[i]-c2);
                // evolution
                phi_new[i] = phi[i] + dt*(mu * kappa + data);
            }
        }
        // copy boundaries and swap
        for(int x=0;x<w;x++){
            phi_new[x] = phi[x];
            phi_new[(h-1)*w + x] = phi[(h-1)*w + x];
        }
        for(int y=0;y<h;y++){
            phi_new[y*w + 0] = phi[y*w + 0];
            phi_new[y*w + (w-1)] = phi[y*w + (w-1)];
        }
        // swap
        memcpy(phi, phi_new, sizeof(float)*N);
    }

    // produce mask
    for(int i=0;i<N;i++) out_mask[i] = (phi[i] <= 0) ? 255 : 0;
    free(phi_new);
}

//////////////////// ROI helpers ////////////////////

// clamp helper
static inline int clampi_i(int v, int a, int b){ if(v<a) return a; if(v>b) return b; return v; }

// copy ROI from full frame into buffer (RGB24)
static void copy_roi_rgb(uint8_t *dst, int dst_w, int dst_h, uint8_t *src, int src_w, int src_h, int sx, int sy){
    // dst stride = dst_w*3
    for(int y=0;y<dst_h;y++){
        int syy = sy + y;
        if(syy < 0 || syy >= src_h) {
            memset(dst + y*dst_w*3, 0, dst_w*3);
            continue;
        }
        for(int x=0;x<dst_w;x++){
            int sxx = sx + x;
            int dstp = (y*dst_w + x)*3;
            if(sxx < 0 || sxx >= src_w){
                dst[dstp]=dst[dstp+1]=dst[dstp+2]=0;
            } else {
                int srcp = (syy*src_w + sxx)*3;
                dst[dstp+0] = src[srcp+0];
                dst[dstp+1] = src[srcp+1];
                dst[dstp+2] = src[srcp+2];
            }
        }
    }
}

// paste mask (binary 0/255) back into full-frame mask at position (sx,sy)
static void paste_mask(uint8_t *fullmask, int full_w, int full_h, uint8_t *mask, int mw, int mh, int sx, int sy){
    for(int y=0;y<mh;y++){
        int fy = sy + y;
        if(fy < 0 || fy >= full_h) continue;
        for(int x=0;x<mw;x++){
            int fx = sx + x;
            if(fx < 0 || fx >= full_w) continue;
            fullmask[fy*full_w + fx] = mask[y*mw + x];
        }
    }
}

//////////////////// V4L2 ////////////////////

static int v4l2_init(){
    fd=open(DEV,O_RDWR);
    if(fd<0){ perror("open"); return -1; }

    struct v4l2_format fmt={0};
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width=W;
    fmt.fmt.pix.height=H;
    fmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;

    if(ioctl(fd,VIDIOC_S_FMT,&fmt)<0) return -1;

    struct v4l2_requestbuffers req={0};
    req.count=MAX_BUFFERS;
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory=V4L2_MEMORY_MMAP;

    ioctl(fd,VIDIOC_REQBUFS,&req);

    for(int i=0;i<MAX_BUFFERS;i++){
        struct v4l2_buffer buf={0};
        buf.type=req.type;
        buf.memory=req.memory;
        buf.index=i;

        ioctl(fd,VIDIOC_QUERYBUF,&buf);

        buffers[i].length=buf.length;
        buffers[i].start=mmap(NULL,buf.length,
            PROT_READ|PROT_WRITE,MAP_SHARED,fd,buf.m.offset);

        ioctl(fd,VIDIOC_QBUF,&buf);
    }

    int type=req.type;
    ioctl(fd,VIDIOC_STREAMON,&type);
    return 0;
}

//////////////////// MAIN ////////////////////

int main(){

    if(v4l2_init()<0) return 1;
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win=SDL_CreateWindow("Face Tracker ChanVese",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
        W,H,SDL_WINDOW_RESIZABLE);

    if(!win){
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *ren=SDL_CreateRenderer(win,-1,0);
    if(!ren){
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Texture *tex=SDL_CreateTexture(ren,
        SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING,W,H);

    frame_rgb=malloc(W*H*3);
    mask=malloc(W*H);
    if(!frame_rgb || !mask){
        fprintf(stderr,"malloc failed\n");
        return 1;
    }
    memset(mask,0,W*H);

    struct v4l2_buffer buf={0};
    buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory=V4L2_MEMORY_MMAP;

    int running=1;

    // temporary buffers for ROI processing (allocated once)
    int max_roi_w = (int)ceilf((t_a>t_b?t_a:t_b)*2 + 20);
    max_roi_w = 512; // safe upper bound
    int max_roi_h = 512;
    uint8_t *roi_rgb = malloc(max_roi_w * max_roi_h * 3);
    float *roi_gray = malloc(sizeof(float) * max_roi_w * max_roi_h);
    float *phi = malloc(sizeof(float) * max_roi_w * max_roi_h);
    uint8_t *roi_mask = malloc(max_roi_w * max_roi_h);
    if(!roi_rgb || !roi_gray || !phi || !roi_mask){
        fprintf(stderr,"roi malloc failed\n");
        return 1;
    }

    while(running){

        if(ioctl(fd,VIDIOC_DQBUF,&buf) < 0) break;

        yuyv_to_rgb(buffers[buf.index].start,frame_rgb);
        build_skin_mask(frame_rgb,mask);

        float cx,cy,a,b,ang;

        if(fit_face(mask,&cx,&cy,&a,&b,&ang))
            tracker_update(cx,cy,a,b,ang);

        // If tracker initialized, run Chan-Vese inside ellipse ROI
        if(tracker_init){
            // compute bounding box of ellipse in full-res
            int bbw = (int)ceilf(t_a*1.2f) * 2 + 10;
            int bbh = (int)ceilf(t_b*1.2f) * 2 + 10;
            // apply downscale
            float ds = DOWNSCALE;
            int rw = clampi_i((int)floorf(bbw * ds), 16, max_roi_w);
            int rh = clampi_i((int)floorf(bbh * ds), 16, max_roi_h);

            // top-left of ROI in full frame coordinates
            int sx = (int)floorf(t_cx - bbw/2.0f);
            int sy = (int)floorf(t_cy - bbh/2.0f);

            // copy ROI (full-res) then downscale into roi_rgb
            // simple nearest downscale for speed
            uint8_t *tmp_full = malloc(bbw * bbh * 3);
            if(tmp_full){
                copy_roi_rgb(tmp_full, bbw, bbh, frame_rgb, W, H, sx, sy);
                // downscale to rw x rh
                for(int y=0;y<rh;y++){
                    for(int x=0;x<rw;x++){
                        int srcx = (int)((float)x / (float)rw * (float)bbw);
                        int srcy = (int)((float)y / (float)rh * (float)bbh);
                        int sp = (srcy*bbw + srcx)*3;
                        int dp = (y*rw + x)*3;
                        roi_rgb[dp+0] = tmp_full[sp+0];
                        roi_rgb[dp+1] = tmp_full[sp+1];
                        roi_rgb[dp+2] = tmp_full[sp+2];
                    }
                }
                free(tmp_full);

                // grayscale float
                rgb_to_gray_float(roi_rgb, roi_gray, rw, rh, rw);

                // initialize phi from ellipse parameters scaled to ROI
                // ellipse center in ROI coords:
                float roi_cx = (float)rw * 0.5f;
                float roi_cy = (float)rh * 0.5f;
                // scale axes to downscale
                float roi_a = (t_a * ds) * 0.5f * (float)bbw / (float)rw * 1.0f;
                float roi_b = (t_b * ds) * 0.5f * (float)bbh / (float)rh * 1.0f;
                // angle remains same
                init_phi_ellipse(phi, rw, rh, roi_cx, roi_cy, fmaxf(roi_a,1.0f), fmaxf(roi_b,1.0f), t_angle);

                // run Chan-Vese
                chan_vese(roi_gray, rw, rh, phi, roi_mask, CV_ITER, CV_DT, CV_MU, CV_LAMBDA1, CV_LAMBDA2);

                // upsample mask back to bbw x bbh and paste into full-frame mask
                // simple nearest upsample and paste
                uint8_t *mask_full_roi = malloc(bbw * bbh);
                if(mask_full_roi){
                    for(int y=0;y<bbh;y++){
                        for(int x=0;x<bbw;x++){
                            int sx2 = (int)((float)x / (float)bbw * (float)rw);
                            int sy2 = (int)((float)y / (float)bbh * (float)rh);
                            sx2 = clampi_i(sx2,0,rw-1);
                            sy2 = clampi_i(sy2,0,rh-1);
                            mask_full_roi[y*bbw + x] = roi_mask[sy2*rw + sx2];
                        }
                    }
                    // paste into global mask (overwrite)
                    paste_mask(mask, W, H, mask_full_roi, bbw, bbh, sx, sy);
                    free(mask_full_roi);
                }
            }

            // Optionally: fill the detected face area (mask) with semi-transparent green
            for(int y=0;y<H;y++){
                for(int x=0;x<W;x++){
                    if(mask[y*W + x]){
                        int p = (y*W + x)*3;
                        // alpha blend green (0,200,0) with alpha 0.35
                        float alpha = 0.35f;
                        frame_rgb[p+0] = clampi((int)(frame_rgb[p+0]*(1-alpha) + 0*alpha + 0.5f));
                        frame_rgb[p+1] = clampi((int)(frame_rgb[p+1]*(1-alpha) + 200*alpha + 0.5f));
                        frame_rgb[p+2] = clampi((int)(frame_rgb[p+2]*(1-alpha) + 0*alpha + 0.5f));
                    }
                }
            }
        }

        // draw ellipse outline on top (antialiased brush)
        if(tracker_init) draw_ellipse(frame_rgb);

        SDL_UpdateTexture(tex,NULL,frame_rgb,W*3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren,tex,NULL,NULL);
        SDL_RenderPresent(ren);

        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) running=0;
        }

        ioctl(fd,VIDIOC_QBUF,&buf);
    }

    // cleanup
    if(frame_rgb) free(frame_rgb);
    if(mask) free(mask);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    if(fd>=0) close(fd);

    free(roi_rgb);
    free(roi_gray);
    free(phi);
    free(roi_mask);

    return 0;
}
