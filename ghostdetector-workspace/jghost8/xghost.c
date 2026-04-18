// ghost.c
// Vollbild Gespensterfilter mit FFT-Analyse und Neon-Grün-Boxen
// Abhängigkeiten: X11, math.h, kiss_fft.h (eingebettet)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define WIDTH 640
#define HEIGHT 480
#define VIDEO_DEVICE "/dev/video0"
#define V4L2_BUFFERS 4
#define NEON 0x00FF00FF // Neon-Grün ARGB
#define BOX_SIZE 5
#define FFT_SIZE 512
#define THRESH 50.0f

typedef unsigned char uchar;

typedef struct {
    void *start;
    size_t length;
} V4L2Buffer;

typedef struct {
    int fd;
    V4L2Buffer *buffers;
    unsigned int n_buffers;
} V4L2Data;

typedef struct {
    Display* display;
    int screen;
    Window window;
    GC gc;
    XImage* ximage;
    Visual* visual;
    int depth;
    int bytes_per_pixel;
    int requires_bgrx_conversion;
} XWindowData;

// -------------------
// Minimal KISS FFT (nur inline für 1D, 2D via Zeilen+Spalten)
// -------------------
typedef struct { float r,i; } kiss_fft_cpx;
void kiss_fft_1d(const kiss_fft_cpx* in, kiss_fft_cpx* out, int n){
    for(int k=0;k<n;k++){
        float sum_r=0,sum_i=0;
        for(int t=0;t<n;t++){
            float angle=-2*M_PI*t*k/n;
            sum_r += in[t].r*cos(angle) - in[t].i*sin(angle);
            sum_i += in[t].r*sin(angle) + in[t].i*cos(angle);
        }
        out[k].r=sum_r; out[k].i=sum_i;
    }
}
void fft2d_gray(uchar* gray,int w,int h,float* mag){
    kiss_fft_cpx row_in[FFT_SIZE], row_out[FFT_SIZE];
    kiss_fft_cpx col_in[FFT_SIZE], col_out[FFT_SIZE];
    int nx=FFT_SIZE, ny=FFT_SIZE;
    for(int y=0;y<ny;y++){
        for(int x=0;x<nx;x++){
            int xi = x*w/nx;
            int yi = y*h/ny;
            row_in[x].r = gray[yi*w+xi]; row_in[x].i=0;
        }
        kiss_fft_1d(row_in,row_out,nx);
        for(int x=0;x<nx;x++) mag[y*nx+x]=sqrtf(row_out[x].r*row_out[x].r+row_out[x].i*row_out[x].i);
    }
}

// -------------------
// V4L2-Funktionen
// -------------------
int v4l2_open(V4L2Data* v4l2){
    v4l2->fd=open(VIDEO_DEVICE,O_RDWR|O_NONBLOCK,0);
    if(v4l2->fd<0){perror("open"); return 0;}
    struct v4l2_capability cap;
    if(ioctl(v4l2->fd,VIDIOC_QUERYCAP,&cap)<0){perror("querycap"); return 0;}
    return 1;
}
int v4l2_set_format(V4L2Data* v4l2){
    struct v4l2_format fmt; memset(&fmt,0,sizeof(fmt));
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width=WIDTH; fmt.fmt.pix.height=HEIGHT;
    fmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field=V4L2_FIELD_NONE;
    if(ioctl(v4l2->fd,VIDIOC_S_FMT,&fmt)<0){perror("s_fmt"); return 0;}
    return 1;
}
int v4l2_init_mmap(V4L2Data* v4l2){
    struct v4l2_requestbuffers req; memset(&req,0,sizeof(req));
    req.count=V4L2_BUFFERS; req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory=V4L2_MEMORY_MMAP;
    if(ioctl(v4l2->fd,VIDIOC_REQBUFS,&req)<0){perror("reqbuf"); return 0;}
    v4l2->buffers=calloc(req.count,sizeof(V4L2Buffer));
    v4l2->n_buffers=req.count;
    for(unsigned int i=0;i<v4l2->n_buffers;i++){
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
        buf.type=req.type; buf.memory=V4L2_MEMORY_MMAP; buf.index=i;
        if(ioctl(v4l2->fd,VIDIOC_QUERYBUF,&buf)<0){perror("querybuf"); return 0;}
        v4l2->buffers[i].length=buf.length;
        v4l2->buffers[i].start=mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,v4l2->fd,buf.m.offset);
        if(v4l2->buffers[i].start==MAP_FAILED){perror("mmap"); return 0;}
    }
    return 1;
}
int v4l2_start(V4L2Data* v4l2){
    for(unsigned int i=0;i<v4l2->n_buffers;i++){
        struct v4l2_buffer buf={0}; buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory=V4L2_MEMORY_MMAP; buf.index=i;
        if(ioctl(v4l2->fd,VIDIOC_QBUF,&buf)<0){perror("qbuf"); return 0;}
    }
    enum v4l2_buf_type type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(v4l2->fd,VIDIOC_STREAMON,&type)<0){perror("streamon"); return 0;}
    return 1;
}
uchar* v4l2_read(V4L2Data* v4l2){
    struct v4l2_buffer buf={0}; buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory=V4L2_MEMORY_MMAP;
    if(ioctl(v4l2->fd,VIDIOC_DQBUF,&buf)<0){if(errno==EAGAIN)return NULL; perror("dqbuf"); return NULL;}
    uchar* frame=(uchar*)v4l2->buffers[buf.index].start;
    ioctl(v4l2->fd,VIDIOC_QBUF,&buf);
    return frame;
}
void v4l2_cleanup(V4L2Data* v4l2){
    enum v4l2_buf_type type=V4L2_BUF_TYPE_VIDEO_CAPTURE; ioctl(v4l2->fd,VIDIOC_STREAMOFF,&type);
    for(unsigned int i=0;i<v4l2->n_buffers;i++) munmap(v4l2->buffers[i].start,v4l2->buffers[i].length);
    free(v4l2->buffers); close(v4l2->fd);
}

// -------------------
// YUYV -> BGRX
// -------------------
void yuyv_to_bgrx(uchar* yuyv, uchar* bgrx){
    for(int i=0;i<WIDTH*HEIGHT/2;i++){
        int y1=yuyv[i*4+0], u=yuyv[i*4+1], y2=yuyv[i*4+2], v=yuyv[i*4+3];
        int u1=u-128,v1=v-128;
        int r=y1+1.403*v1, g=y1-0.344*u1-0.714*v1, b=y1+1.770*u1;
        bgrx[i*8+0]=fmin(fmax(b,0),255);
        bgrx[i*8+1]=fmin(fmax(g,0),255);
        bgrx[i*8+2]=fmin(fmax(r,0),255);
        bgrx[i*8+3]=0;
        r=y2+1.403*v1; g=y2-0.344*u1-0.714*v1; b=y2+1.770*u1;
        bgrx[i*8+4]=fmin(fmax(b,0),255);
        bgrx[i*8+5]=fmin(fmax(g,0),255);
        bgrx[i*8+6]=fmin(fmax(r,0),255);
        bgrx[i*8+7]=0;
    }
}

// -------------------
// Maske erzeugen via FFT
// -------------------
void make_mask(uchar* bgrx, uchar* mask){
    uchar gray[WIDTH*HEIGHT];
    for(int i=0;i<WIDTH*HEIGHT;i++)
        gray[i]=(bgrx[i*4+0]+bgrx[i*4+1]+bgrx[i*4+2])/3;
    float mag[FFT_SIZE*FFT_SIZE];
    fft2d_gray(gray,WIDTH,HEIGHT,mag);
    for(int i=0;i<FFT_SIZE*FFT_SIZE;i++)
        mask[i]=(mag[i]>THRESH)?255:0;
}

// -------------------
// X11 Initialisierung
// -------------------
int init_x11(XWindowData* xwin){
    xwin->display=XOpenDisplay(NULL);
    if(!xwin->display) return 0;
    xwin->screen=DefaultScreen(xwin->display);
    xwin->visual=DefaultVisual(xwin->display,xwin->screen);
    xwin->depth=DefaultDepth(xwin->display,xwin->screen);
    xwin->bytes_per_pixel=4; xwin->requires_bgrx_conversion=1;
    xwin->window=RootWindow(xwin->display,xwin->screen);
    xwin->gc=XCreateGC(xwin->display,xwin->window,0,NULL);
    return 1;
}

// -------------------
// Main
// -------------------
int main(){
    V4L2Data v4l2={0};
    if(!v4l2_open(&v4l2)||!v4l2_set_format(&v4l2)||!v4l2_init_mmap(&v4l2)||!v4l2_start(&v4l2)){
        fprintf(stderr,"V4L2 Init fehlgeschlagen\n"); return 1;
    }

    XWindowData xwin={0};
    if(!init_x11(&xwin)){fprintf(stderr,"X11 Init fehlgeschlagen\n"); return 1;}

    uchar* bgrx=malloc(WIDTH*HEIGHT*4);
    uchar* mask=malloc(FFT_SIZE*FFT_SIZE);

    printf("ESC zum Beenden\n");
    while(1){
        uchar* frame=v4l2_read(&v4l2);
        if(!frame){usleep(10000); continue;}
        yuyv_to_bgrx(frame,bgrx);
        make_mask(bgrx,mask);

        // Neon-Grün-Boxen
        for(int y=0;y<FFT_SIZE;y++){
            for(int x=0;x<FFT_SIZE;x++){
                if(mask[y*FFT_SIZE+x]){
                    for(int dy=0;dy<BOX_SIZE;dy++)
                        for(int dx=0;dx<BOX_SIZE;dx++){
                            int xi=x*WIDTH/FFT_SIZE+dx;
                            int yi=y*HEIGHT/FFT_SIZE+dy;
                            if(xi<WIDTH && yi<HEIGHT){
                                int idx=(yi*WIDTH+xi)*4;
                                bgrx[idx+0]=0; bgrx[idx+1]=255; bgrx[idx+2]=0; bgrx[idx+3]=0;
                            }
                        }
                }
            }
        }

        // Display (root window)
        XImage* img=XCreateImage(xwin.display,xwin.visual,xwin.depth,ZPixmap,0,(char*)bgrx,WIDTH,HEIGHT,32,WIDTH*4);
        XPutImage(xwin.display,xwin.window,xwin.gc,img,0,0,0,0,WIDTH,HEIGHT);
        XDestroyImage(img);

        // ESC check
        if(XPending(xwin.display)){
            XEvent e; XNextEvent(xwin.display,&e);
            if(e.type==KeyPress && XLookupKeysym(&e.xkey,0)==XK_Escape) break;
        }
    }

    free(bgrx); free(mask);
    v4l2_cleanup(&v4l2);
    return 0;
}


