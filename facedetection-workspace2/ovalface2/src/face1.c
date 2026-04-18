// face_tracker_ellipse.c
// Simple full face tracker using skin mask + ellipse fitting
// Draws ellipse outline with an antialiased brush (diameter 5px) using alpha blending
// Compile: gcc -O2 face_tracker_ellipse.c -o face_tracker -lSDL2 -lm

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

#define DEV "/dev/video0"
#define W 640
#define H 480

#define MAX_BUFFERS 4
#define TRACK_SMOOTH 0.80f

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

//////////////////// RGB → HSV ////////////////////

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

//////////////////// Skin mask ////////////////////

static void build_skin_mask(uint8_t *rgb,uint8_t *m){
    for(int i=0;i<W*H;i++){
        int p=i*3;
        float h,s,v;
        rgb_to_hsv(rgb[p],rgb[p+1],rgb[p+2],&h,&s,&v);

        // simple universal skin thresholds
        int skin =
            ((h<45 || h>330) && s>0.15 && v>0.1);

        m[i]=skin?255:0;
    }
}

//////////////////// Largest blob ////////////////////

static int flood_fill(uint8_t *m,uint8_t *vis,int sx,int sy,
                      double *cx,double *cy,
                      double *cxx,double *cyy,double *cxy)
{
    int stack_size=0;
    static int *stack = NULL;
    if(!stack) stack = malloc(sizeof(int) * (W*H/8) * 2); // fallback heap stack
    // store pairs as consecutive ints
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

    // covariance
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
    // alpha in [0,1], perform linear alpha blending: out = src*alpha + dst*(1-alpha)
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

//////////////////// Draw ellipse using antialiased brush ////////////////////

static void draw_ellipse(uint8_t *rgb){
    // step in degrees; smaller step = denser brush stamps
    const float step = 1.0f; // 1 degree gives smooth result with AA brush
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

    SDL_Window *win=SDL_CreateWindow("Face Tracker",
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

    struct v4l2_buffer buf={0};
    buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory=V4L2_MEMORY_MMAP;

    int running=1;

    while(running){

        if(ioctl(fd,VIDIOC_DQBUF,&buf) < 0) break;

        yuyv_to_rgb(buffers[buf.index].start,frame_rgb);
        build_skin_mask(frame_rgb,mask);

        float cx,cy,a,b,ang;

        if(fit_face(mask,&cx,&cy,&a,&b,&ang))
            tracker_update(cx,cy,a,b,ang);

        if(tracker_init)
            draw_ellipse(frame_rgb);

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

    // cleanup (best effort)
    if(frame_rgb) free(frame_rgb);
    if(mask) free(mask);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    if(fd>=0) close(fd);

    return 0;
}
