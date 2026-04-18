// face_tracker_ellipse.c
// Simple full face tracker using skin mask + ellipse fitting
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
    int stack[W*H/8][2];

    stack[stack_size][0]=sx;
    stack[stack_size++][1]=sy;

    vis[sy*W+sx]=1;

    int count=0;
    double mx=0,my=0;

    while(stack_size){
        int x=stack[--stack_size][0];
        int y=stack[stack_size][1];

        mx+=x; my+=y;
        count++;

        for(int dy=-1;dy<=1;dy++){
            for(int dx=-1;dx<=1;dx++){
                int nx=x+dx,ny=y+dy;
                if(nx>=0 && nx<W && ny>=0 && ny<H){
                    int idx=ny*W+nx;
                    if(m[idx] && !vis[idx]){
                        vis[idx]=1;
                        stack[stack_size][0]=nx;
                        stack[stack_size++][1]=ny;
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

//////////////////// Draw ellipse ////////////////////

static void draw_ellipse(uint8_t *rgb){
    for(int t=0;t<360;t++){
        double r=t*M_PI/180;
        double ex=t_a*cos(r);
        double ey=t_b*sin(r);

        double rx=ex*cos(t_angle)-ey*sin(t_angle);
        double ry=ex*sin(t_angle)+ey*cos(t_angle);

        int px=t_cx+rx;
        int py=t_cy+ry;

        if(px>=0 && px<W && py>=0 && py<H){
            int p=(py*W+px)*3;
            rgb[p]=0; rgb[p+1]=255; rgb[p+2]=0;
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
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win=SDL_CreateWindow("Face Tracker",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
        W,H,SDL_WINDOW_RESIZABLE);

    SDL_Renderer *ren=SDL_CreateRenderer(win,-1,0);
    SDL_Texture *tex=SDL_CreateTexture(ren,
        SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING,W,H);

    frame_rgb=malloc(W*H*3);
    mask=malloc(W*H);

    struct v4l2_buffer buf={0};
    buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory=V4L2_MEMORY_MMAP;

    int running=1;

    while(running){

        ioctl(fd,VIDIOC_DQBUF,&buf);

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

    return 0;
}
