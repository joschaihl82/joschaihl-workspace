// simple_face_detector.c
// Very simplified HSV skin based face detector

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
#define ROI_SIZE 160

struct buffer { void *start; size_t length; };
static struct buffer *buffers;
static int fd;

static uint8_t *frame_rgb;
static uint8_t *mask;

static inline uint8_t clamp(int v){
    if(v<0) return 0;
    if(v>255) return 255;
    return v;
}

////////////////////////////////////////////////////////////
// YUYV -> RGB
////////////////////////////////////////////////////////////
static void yuyv_to_rgb(uint8_t *src, uint8_t *dst){
    for(int i=0;i<W*H;i+=2){
        int y0=src[0], u=src[1], y1=src[2], v=src[3];
        src+=4;

        int c=y0-16, d=u-128, e=v-128;
        dst[0]=clamp((298*c+409*e+128)>>8);
        dst[1]=clamp((298*c-100*d-208*e+128)>>8);
        dst[2]=clamp((298*c+516*d+128)>>8);

        c=y1-16;
        dst[3]=clamp((298*c+409*e+128)>>8);
        dst[4]=clamp((298*c-100*d-208*e+128)>>8);
        dst[5]=clamp((298*c+516*d+128)>>8);

        dst+=6;
    }
}

////////////////////////////////////////////////////////////
// RGB -> HSV
////////////////////////////////////////////////////////////
static void rgb_to_hsv(uint8_t r,uint8_t g,uint8_t b,
                       float *h,float *s,float *v)
{
    float rf=r/255.0f, gf=g/255.0f, bf=b/255.0f;
    float mx=fmaxf(rf,fmaxf(gf,bf));
    float mn=fminf(rf,fminf(gf,bf));
    float d=mx-mn;

    *v=mx;
    *s = (mx==0)?0:d/mx;

    if(d==0){ *h=0; return; }

    if(mx==rf) *h = 60*fmodf((gf-bf)/d,6);
    else if(mx==gf) *h = 60*((bf-rf)/d + 2);
    else *h = 60*((rf-gf)/d + 4);

    if(*h<0) *h+=360;
}

////////////////////////////////////////////////////////////
// Simple skin mask
////////////////////////////////////////////////////////////
static int create_skin_mask(uint8_t *rgb,uint8_t *mask){
    int area=0;

    for(int i=0;i<W*H;i++){
        float h,s,v;
        rgb_to_hsv(rgb[i*3],rgb[i*3+1],rgb[i*3+2],&h,&s,&v);

        int skin =
            (h>0 && h<40) &&
            (s>0.2f && s<0.8f) &&
            (v>0.2f);

        mask[i] = skin?255:0;
        if(skin) area++;
    }

    return area;
}

////////////////////////////////////////////////////////////
// Center of mask
////////////////////////////////////////////////////////////
static void mask_center(uint8_t *m,int *cx,int *cy){
    long sx=0, sy=0, cnt=0;

    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            if(m[y*W+x]){
                sx+=x; sy+=y; cnt++;
            }
        }
    }

    if(cnt<100){
        *cx=W/2; *cy=H/2;
    }else{
        *cx=sx/cnt;
        *cy=sy/cnt;
    }
}

////////////////////////////////////////////////////////////
// Draw rectangle
////////////////////////////////////////////////////////////
static void draw_rect(uint8_t *rgb,int x,int y,int s){
    for(int i=0;i<s;i++){
        int x1=x+i;
        int y1=y+i;

        if(x1>=0 && x1<W){
            if(y>=0 && y<H){
                int p=(y*W+x1)*3;
                rgb[p]=0; rgb[p+1]=255; rgb[p+2]=0;
            }
            if(y+s>=0 && y+s<H){
                int p=((y+s)*W+x1)*3;
                rgb[p]=0; rgb[p+1]=255; rgb[p+2]=0;
            }
        }

        if(y1>=0 && y1<H){
            if(x>=0 && x<W){
                int p=(y1*W+x)*3;
                rgb[p]=0; rgb[p+1]=255; rgb[p+2]=0;
            }
            if(x+s>=0 && x+s<W){
                int p=(y1*W+x+s)*3;
                rgb[p]=0; rgb[p+1]=255; rgb[p+2]=0;
            }
        }
    }
}

////////////////////////////////////////////////////////////
// Very simple eyes + mouth
////////////////////////////////////////////////////////////
static void detect_features(uint8_t *rgb,int cx,int cy){
    int rx=cx-ROI_SIZE/2;
    int ry=cy-ROI_SIZE/2;

    int eye_y = ry + ROI_SIZE/3;
    int mouth_y = ry + 2*ROI_SIZE/3;

    // draw two simple eye markers
    for(int i=-20;i<=20;i++){
        int x=cx+i;

        if(x>0 && x<W && eye_y>0 && eye_y<H){
            int p=(eye_y*W+x)*3;
            rgb[p]=255; rgb[p+1]=0; rgb[p+2]=0;
        }
    }

    // mouth line
    for(int i=-25;i<=25;i++){
        int x=cx+i;

        if(x>0 && x<W && mouth_y>0 && mouth_y<H){
            int p=(mouth_y*W+x)*3;
            rgb[p]=255; rgb[p+1]=0; rgb[p+2]=255;
        }
    }
}

////////////////////////////////////////////////////////////
// V4L2 init
////////////////////////////////////////////////////////////
int v4l2_init(){
    fd=open(DEV,O_RDWR);
    if(fd<0){ perror("open"); return -1; }

    struct v4l2_format fmt={0};
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width=W;
    fmt.fmt.pix.height=H;
    fmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;

    if(ioctl(fd,VIDIOC_S_FMT,&fmt)<0){ perror("fmt"); return -1; }

    struct v4l2_requestbuffers req={0};
    req.count=4;
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory=V4L2_MEMORY_MMAP;

    ioctl(fd,VIDIOC_REQBUFS,&req);

    buffers=calloc(req.count,sizeof(*buffers));

    for(int i=0;i<req.count;i++){
        struct v4l2_buffer buf={0};
        buf.type=req.type;
        buf.memory=req.memory;
        buf.index=i;

        ioctl(fd,VIDIOC_QUERYBUF,&buf);

        buffers[i].length=buf.length;
        buffers[i].start=mmap(NULL,buf.length,
                              PROT_READ|PROT_WRITE,
                              MAP_SHARED,fd,buf.m.offset);

        ioctl(fd,VIDIOC_QBUF,&buf);
    }

    int type=req.type;
    ioctl(fd,VIDIOC_STREAMON,&type);
    return 0;
}

////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////
int main(){
    v4l2_init();

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win=SDL_CreateWindow(
        "Simple Face Detector",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        W,H,0);

    SDL_Renderer *ren=SDL_CreateRenderer(win,-1,0);
    SDL_Texture *tex=SDL_CreateTexture(
        ren,SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,W,H);

    frame_rgb=malloc(W*H*3);
    mask=malloc(W*H);

    struct v4l2_buffer buf={0};
    buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory=V4L2_MEMORY_MMAP;

    int run=1;

    while(run){
        ioctl(fd,VIDIOC_DQBUF,&buf);

        yuyv_to_rgb(buffers[buf.index].start,frame_rgb);

        create_skin_mask(frame_rgb,mask);

        int cx,cy;
        mask_center(mask,&cx,&cy);

        draw_rect(frame_rgb,cx-ROI_SIZE/2,cy-ROI_SIZE/2,ROI_SIZE);
        detect_features(frame_rgb,cx,cy);

        SDL_UpdateTexture(tex,NULL,frame_rgb,W*3);
        SDL_RenderCopy(ren,tex,NULL,NULL);
        SDL_RenderPresent(ren);

        SDL_Event e;
        while(SDL_PollEvent(&e))
            if(e.type==SDL_QUIT) run=0;

        ioctl(fd,VIDIOC_QBUF,&buf);
    }

    return 0;
}
