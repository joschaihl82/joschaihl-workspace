// facedet_simple.c
// Compile: gcc -O2 facedet_simple.c -o facedet_simple -lSDL2 -lm
// Run on Linux with a V4L2 camera at /dev/video0 and SDL2 installed.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <math.h>

#define DEV "/dev/video0"
#define W 640
#define H 480

// ROI / simple params
#define MIN_FACE_AREA 2000
#define DEFAULT_ROI 160

// V4L2 buffers
struct buffer { void *start; size_t length; };
static struct buffer *buffers;
static int fd = -1;

// frame buffers
static uint8_t *frame_rgb = NULL;
static uint8_t *mask = NULL;

// helpers
static inline uint8_t clampi(int v){ if(v<0) return 0; if(v>255) return 255; return v; }
static inline int in_bounds(int x,int y){ return x>=0 && x<W && y>=0 && y<H; }

// YUYV -> RGB24
static void yuyv_to_rgb(uint8_t *yuyv, uint8_t *rgb){
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x+=2){
            int i = (y*W + x)*2;
            int Y0 = yuyv[i+0], U = yuyv[i+1], Y1 = yuyv[i+2], V = yuyv[i+3];
            int C = Y0 - 16, D = U - 128, E = V - 128;
            int R = (298*C + 409*E + 128) >> 8;
            int G = (298*C - 100*D - 208*E + 128) >> 8;
            int B = (298*C + 516*D + 128) >> 8;
            int p0 = (y*W + x)*3;
            rgb[p0+0]=clampi(R); rgb[p0+1]=clampi(G); rgb[p0+2]=clampi(B);
            C = Y1 - 16;
            R = (298*C + 409*E + 128) >> 8;
            G = (298*C - 100*D - 208*E + 128) >> 8;
            B = (298*C + 516*D + 128) >> 8;
            int p1 = (y*W + x+1)*3;
            rgb[p1+0]=clampi(R); rgb[p1+1]=clampi(G); rgb[p1+2]=clampi(B);
        }
    }
}

// RGB -> HSV (H:0..360, S:0..1, V:0..1)
static void rgb_to_hsv(uint8_t r,uint8_t g,uint8_t b,float *h,float *s,float *v){
    float rf=r/255.0f, gf=g/255.0f, bf=b/255.0f;
    float mx = fmaxf(rf,fmaxf(gf,bf));
    float mn = fminf(rf,fminf(gf,bf));
    float d = mx - mn;
    *v = mx;
    *s = (mx==0.0f)?0.0f:d/mx;
    if(d==0.0f){ *h=0.0f; return; }
    if(mx==rf) *h = 60.0f * fmodf(((gf-bf)/d),6.0f);
    else if(mx==gf) *h = 60.0f * (((bf-rf)/d)+2.0f);
    else *h = 60.0f * (((rf-gf)/d)+4.0f);
    if(*h<0.0f) *h += 360.0f;
}

// simple adaptive skin mask: hue near initial skin hue and S above threshold
static int create_skin_mask(uint8_t *rgb, uint8_t *mask_out, int *minx,int *miny,int *maxx,int *maxy){
    // quick global V mean to adapt S threshold
    double Vsum=0;
    for(int i=0;i<W*H;i++){
        int p=i*3; float h,s,v; rgb_to_hsv(rgb[p],rgb[p+1],rgb[p+2],&h,&s,&v);
        Vsum += v;
    }
    float Vmean = (float)(Vsum/(W*H));
    float smin = 0.18f - 0.4f*(0.5f - Vmean);
    if(smin < 0.08f) smin = 0.08f;

    // skin hue guess around 20..40 deg (tweakable)
    float h_center = 25.0f;
    float h_range = 30.0f;

    int area=0;
    int mx=W, my=H, Mx=-1, My=-1;
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            int p=(y*W+x)*3; float h,s,v; rgb_to_hsv(rgb[p],rgb[p+1],rgb[p+2],&h,&s,&v);
            int skin = 0;
            float dh = fabsf(h - h_center);
            if(dh > 180.0f) dh = 360.0f - dh;
            if(dh <= h_range && s >= smin && v > 0.05f) skin = 1;
            mask_out[y*W + x] = skin ? 255 : 0;
            if(skin){
                area++;
                if(x < mx) mx = x;
                if(y < my) my = y;
                if(x > Mx) Mx = x;
                if(y > My) My = y;
            }
        }
    }
    if(area >= MIN_FACE_AREA && Mx>=0){
        *minx = mx; *miny = my; *maxx = Mx; *maxy = My;
    } else {
        *minx = *miny = *maxx = *maxy = -1;
    }
    return area;
}

// draw rectangle
static void draw_rect(uint8_t *rgb,int x0,int y0,int x1,int y1,uint8_t R,uint8_t G,uint8_t B){
    if(x0<0) x0=0; if(y0<0) y0=0; if(x1>=W) x1=W-1; if(y1>=H) y1=H-1;
    for(int x=x0;x<=x1;x++){
        int p1=(y0*W + x)*3; rgb[p1]=R; rgb[p1+1]=G; rgb[p1+2]=B;
        int p2=(y1*W + x)*3; rgb[p2]=R; rgb[p2+1]=G; rgb[p2+2]=B;
    }
    for(int y=y0;y<=y1;y++){
        int p1=(y*W + x0)*3; rgb[p1]=R; rgb[p1+1]=G; rgb[p1+2]=B;
        int p2=(y*W + x1)*3; rgb[p2]=R; rgb[p2+1]=G; rgb[p2+2]=B;
    }
}

// draw filled small circle
static void draw_circle(uint8_t *rgb,int cx,int cy,int r,uint8_t R,uint8_t G,uint8_t B){
    for(int dy=-r;dy<=r;dy++){
        for(int dx=-r;dx<=r;dx++){
            if(dx*dx+dy*dy <= r*r){
                int x=cx+dx, y=cy+dy;
                if(in_bounds(x,y)){
                    int p=(y*W + x)*3; rgb[p]=R; rgb[p+1]=G; rgb[p+2]=B;
                }
            }
        }
    }
}

// find two dark eye centers in upper half of face bbox
static int detect_eyes(uint8_t *rgb,int bx0,int by0,int bx1,int by1,int *ex1,int *ey1,int *ex2,int *ey2){
    if(bx0<0) return 0;
    int w = bx1 - bx0 + 1;
    int h = by1 - by0 + 1;
    int top_h = h/2;
    // compute grayscale and find local minima
    int best1x=-1,best1y=-1; float best1v=1e9f;
    int best2x=-1,best2y=-1; float best2v=1e9f;
    for(int y=by0+2; y<by0+top_h-2; y++){
        for(int x=bx0+2; x<bx1-2; x++){
            int p=(y*W + x)*3;
            float Y = 0.299f*rgb[p] + 0.587f*rgb[p+1] + 0.114f*rgb[p+2];
            // local minimum 3x3
            int local=1;
            for(int dy=-1;dy<=1 && local;dy++) for(int dx=-1;dx<=1;dx++){
                if(dx==0 && dy==0) continue;
                int q=(y+dy)*W + (x+dx);
                float Y2 = 0.299f*rgb[q*3] + 0.587f*rgb[q*3+1] + 0.114f*rgb[q*3+2];
                if(Y2 <= Y) { local=0; break; }
            }
            if(!local) continue;
            if(Y < best1v){
                best2x=best1x; best2y=best1y; best2v=best1v;
                best1x=x; best1y=y; best1v=Y;
            } else if(Y < best2v){
                best2x=x; best2y=y; best2v=Y;
            }
        }
    }
    if(best1x>=0 && best2x>=0){
        // ensure horizontal separation
        if(abs(best1x - best2x) > w/6){
            *ex1 = best1x; *ey1 = best1y;
            *ex2 = best2x; *ey2 = best2y;
            return 1;
        }
    }
    return 0;
}

// iris: pick darkest pixel in small neighborhood around eye center
static int detect_iris(uint8_t *rgb,int ex,int ey,int *ix,int *iy){
    if(ex<0) return 0;
    int bestx=-1,besty=-1; int bestv=256;
    for(int dy=-6; dy<=6; dy++){
        for(int dx=-6; dx<=6; dx++){
            int x=ex+dx, y=ey+dy;
            if(!in_bounds(x,y)) continue;
            int p=(y*W + x)*3;
            int Y = (int)(0.299f*rgb[p] + 0.587f*rgb[p+1] + 0.114f*rgb[p+2]);
            if(Y < bestv){ bestv = Y; bestx = x; besty = y; }
        }
    }
    if(bestx>=0){
        *ix = bestx; *iy = besty; return 1;
    }
    return 0;
}

// nose: strongest vertical gradient near center of bbox
static int detect_nose(uint8_t *rgb,int bx0,int by0,int bx1,int by1,int *nx,int *ny){
    if(bx0<0) return 0;
    int cx = (bx0 + bx1)/2;
    int sy = by0 + (by1 - by0)/6;
    int ey = by0 + (by1 - by0)/2;
    float bestv = 0.0f; int bestx=-1,besty=-1;
    for(int y=sy+1;y<ey-1;y++){
        for(int x=cx- (bx1-bx0)/6; x<=cx+(bx1-bx0)/6; x++){
            if(!in_bounds(x,y)) continue;
            int p_up = ((y-1)*W + x)*3;
            int p_dn = ((y+1)*W + x)*3;
            float Yu = 0.299f*rgb[p_up] + 0.587f*rgb[p_up+1] + 0.114f*rgb[p_up+2];
            float Yd = 0.299f*rgb[p_dn] + 0.587f*rgb[p_dn+1] + 0.114f*rgb[p_dn+2];
            float gy = fabsf(Yd - Yu);
            if(gy > bestv){ bestv = gy; bestx = x; besty = y; }
        }
    }
    if(bestx>=0){
        *nx = bestx; *ny = besty; return 1;
    }
    return 0;
}

// mouth: red/saturated region in lower third of bbox
static int detect_mouth(uint8_t *rgb,int bx0,int by0,int bx1,int by1,int *mx0,int *my0,int *mx1,int *my1){
    if(bx0<0) return 0;
    int sx = bx0 + (bx1-bx0)/6;
    int ex = bx1 - (bx1-bx0)/6;
    int sy = by0 + 2*(by1-by0)/3;
    int ey = by1;
    int ax0=W, ay0=H, ax1=-1, ay1=-1, area=0;
    for(int y=sy;y<=ey;y++){
        for(int x=sx;x<=ex;x++){
            int p=(y*W + x)*3; float h,s,v; rgb_to_hsv(rgb[p],rgb[p+1],rgb[p+2],&h,&s,&v);
            int is_red = (h <= 25.0f) || (h >= 335.0f);
            if(is_red && s >= 0.35f && v <= 0.9f){
                area++;
                if(x < ax0) ax0 = x;
                if(y < ay0) ay0 = y;
                if(x > ax1) ax1 = x;
                if(y > ay1) ay1 = y;
            }
        }
    }
    if(area > 80 && ax1>ax0){
        *mx0 = ax0; *my0 = ay0; *mx1 = ax1; *my1 = ay1;
        return 1;
    }
    return 0;
}

// V4L2 init (simple)
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

int main(){
    if(v4l2_init()<0) return 1;
    if(SDL_Init(SDL_INIT_VIDEO)!=0){ fprintf(stderr,"SDL init: %s\n",SDL_GetError()); return 1; }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_Window *win = SDL_CreateWindow("FaceDet Simple", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, W, H);

    frame_rgb = malloc(W*H*3);
    mask = malloc(W*H);

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;

    int running = 1;
    while(running){
        if(ioctl(fd, VIDIOC_DQBUF, &buf) < 0){ perror("DQBUF"); break; }
        uint8_t *yuyv = buffers[buf.index].start;
        yuyv_to_rgb(yuyv, frame_rgb);

        int minx,miny,maxx,maxy;
        int area = create_skin_mask(frame_rgb, mask, &minx,&miny,&maxx,&maxy);

        // draw face bbox or fallback ROI
        if(minx>=0){
            // pad bbox a bit
            int pad = 8;
            int bx0 = minx - pad, by0 = miny - pad, bx1 = maxx + pad, by1 = maxy + pad;
            if(bx0<0) bx0=0; if(by0<0) by0=0; if(bx1>=W) bx1=W-1; if(by1>=H) by1=H-1;
            draw_rect(frame_rgb, bx0, by0, bx1, by1, 0, 255, 0);

            // eyes
            int ex1,ey1,ex2,ey2;
            if(detect_eyes(frame_rgb,bx0,by0,bx1,by1,&ex1,&ey1,&ex2,&ey2)){
                draw_circle(frame_rgb, ex1, ey1, 5, 0,128,255);
                draw_circle(frame_rgb, ex2, ey2, 5, 0,128,255);
                int ix,iy;
                if(detect_iris(frame_rgb, ex1, ey1, &ix,&iy)) draw_circle(frame_rgb, ix, iy, 3, 255,255,255);
                if(detect_iris(frame_rgb, ex2, ey2, &ix,&iy)) draw_circle(frame_rgb, ix, iy, 3, 255,255,255);
            }

            // nose
            int nx,ny;
            if(detect_nose(frame_rgb,bx0,by0,bx1,by1,&nx,&ny)) draw_circle(frame_rgb, nx, ny, 4, 255,200,0);

            // mouth
            int mx0,my0,mx1,my1;
            if(detect_mouth(frame_rgb,bx0,by0,bx1,by1,&mx0,&my0,&mx1,&my1)){
                draw_rect(frame_rgb, mx0,my0,mx1,my1, 255,0,255);
            }
        } else {
            // fallback: draw centered ROI
            int cx = W/2, cy = H/2;
            int r = DEFAULT_ROI/2;
            draw_rect(frame_rgb, cx-r, cy-r, cx+r, cy+r, 0,255,0);
        }

        SDL_UpdateTexture(tex, NULL, frame_rgb, W*3);
        int ww, hh; SDL_GetWindowSize(win, &ww, &hh);
        SDL_Rect dst = {0,0,ww,hh};
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, &dst);
        SDL_RenderPresent(ren);

        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) running=0;
            if(e.type==SDL_KEYDOWN){
                if(e.key.keysym.sym=='q') running=0;
            }
        }

        if(ioctl(fd, VIDIOC_QBUF, &buf) < 0){ perror("QBUF"); break; }
    }

    SDL_DestroyTexture(tex); SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
    if(frame_rgb) free(frame_rgb);
    if(mask) free(mask);
    close(fd);
    return 0;
}
