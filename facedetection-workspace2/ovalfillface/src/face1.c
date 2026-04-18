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

//////////////////// Skin mask (mit adaptiven Schwellen innerhalb des Ovals) ////////////////////

// Hilfsfunktion: prüft, ob ein Pixel (x,y) innerhalb der Ellipse liegt (rotierte Ellipse)
static int point_in_ellipse(int x, int y, float cx, float cy, float a, float b, float ang){
    float dx = x - cx;
    float dy = y - cy;
    float ca = cosf(-ang);
    float sa = sinf(-ang);
    float xr = dx * ca - dy * sa;
    float yr = dx * sa + dy * ca;
    float val = (xr*xr)/(a*a) + (yr*yr)/(b*b);
    return (val <= 1.0f);
}

// build_skin_mask: wenn tracker_init gesetzt ist, berechne adaptive H/S aus Pixeln innerhalb der Ellipse
static void build_skin_mask(uint8_t *rgb,uint8_t *m){
    const float default_h_center = 25.0f; // Grad
    const float default_h_range = 40.0f;  // +/- Grad
    const float default_s_min = 0.15f;
    const float default_v_min = 0.10f;

    float h_center = default_h_center;
    float h_range = default_h_range;
    float s_min = default_s_min;
    float v_min = default_v_min;

    int adaptive_ok = 0;

    if(tracker_init){
        double sum_sin_h = 0.0, sum_cos_h = 0.0;
        double sum_s = 0.0, sum_v = 0.0;
        double sum_s2 = 0.0;
        int count = 0;

        float a = fmaxf(1.0f, t_a);
        float b = fmaxf(1.0f, t_b);
        float ang = t_angle;
        float cx = t_cx;
        float cy = t_cy;

        int x0 = (int)fmaxf(0.0f, floorf(cx - a));
        int x1 = (int)fminf(W-1.0f, ceilf(cx + a));
        int y0 = (int)fmaxf(0.0f, floorf(cy - b));
        int y1 = (int)fminf(H-1.0f, ceilf(cy + b));

        for(int y=y0; y<=y1; y++){
            for(int x=x0; x<=x1; x++){
                if(!point_in_ellipse(x,y,cx,cy,a,b,ang)) continue;
                int p = (y*W + x)*3;
                float h,s,v;
                rgb_to_hsv(rgb[p], rgb[p+1], rgb[p+2], &h, &s, &v);
                float hr = h * (M_PI/180.0f);
                sum_sin_h += sin(hr);
                sum_cos_h += cos(hr);
                sum_s += s;
                sum_v += v;
                sum_s2 += s*s;
                count++;
            }
        }

        if(count >= 200){
            double mean_sin = sum_sin_h / count;
            double mean_cos = sum_cos_h / count;
            double mean_hr = atan2(mean_sin, mean_cos);
            if(mean_hr < 0) mean_hr += 2*M_PI;
            double mean_h_deg = mean_hr * (180.0/M_PI);

            double R = sqrt(mean_sin*mean_sin + mean_cos*mean_cos);
            double std_hr = 0.0;
            if(R > 1e-6) std_hr = sqrt(fmax(0.0, -2.0 * log(R)));
            double std_h_deg = std_hr * (180.0/M_PI);

            double mean_s = sum_s / count;
            double var_s = (sum_s2 / count) - (mean_s*mean_s);
            double std_s = (var_s > 0.0) ? sqrt(var_s) : 0.0;

            h_center = (float)mean_h_deg;
            h_range = (float)fmax(18.0, 2.5 * std_h_deg);
            s_min = (float)fmax(0.08, mean_s - 0.25f);
            v_min = (float)fmax(0.05, (float)(sum_v / count) - 0.25f);

            adaptive_ok = 1;
        }
    }

    if(!adaptive_ok){
        h_center = default_h_center;
        h_range = default_h_range;
        s_min = default_s_min;
        v_min = default_v_min;
    }

    for(int i=0;i<W*H;i++){
        int p=i*3;
        float h,s,v;
        rgb_to_hsv(rgb[p],rgb[p+1],rgb[p+2],&h,&s,&v);
        float dh = fabsf(h - h_center);
        if(dh > 180.0f) dh = 360.0f - dh;
        int skin = 0;
        if(dh <= h_range && s >= s_min && v >= v_min) skin = 1;
        m[i] = skin ? 255 : 0;
    }
}

//////////////////// Largest blob ////////////////////

static int flood_fill(uint8_t *m,uint8_t *vis,int sx,int sy,
                      double *cx,double *cy,
                      double *cxx,double *cyy,double *cxy)
{
    int stack_size=0;
    static int *stack = NULL;
    static int stack_cap = 0;
    if(!stack){
        stack_cap = W*H/8;
        stack = malloc(stack_cap * 2 * sizeof(int));
        if(!stack) return 0;
    }

    stack[stack_size*2+0]=sx;
    stack[stack_size*2+1]=sy;
    stack_size++;

    vis[sy*W+sx]=1;

    int count=0;
    double mx=0,my=0;

    while(stack_size){
        stack_size--;
        int x = stack[stack_size*2+0];
        int y = stack[stack_size*2+1];

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
                        if(stack_size*2+2 > stack_cap*2){
                            stack_size--;
                        }
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

//////////////////// Draw ellipse (3x3 Pinselbreite) ////////////////////

static void draw_ellipse(uint8_t *rgb){
    const int half = 1; // 3x3 Pinsel
    for(int t=0;t<360;t++){
        double r = t * M_PI / 180.0;
        double ex = t_a * cos(r);
        double ey = t_b * sin(r);

        double rx = ex * cos(t_angle) - ey * sin(t_angle);
        double ry = ex * sin(t_angle) + ey * cos(t_angle);

        int px = (int)round(t_cx + rx);
        int py = (int)round(t_cy + ry);

        for(int dy=-half; dy<=half; dy++){
            int y = py + dy;
            if(y < 0 || y >= H) continue;
            for(int dx=-half; dx<=half; dx++){
                int x = px + dx;
                if(x < 0 || x >= W) continue;
                int p = (y*W + x)*3;
                rgb[p]   = 0;
                rgb[p+1] = 255;
                rgb[p+2] = 0;
            }
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
