// ghost.c
// Gespenster-Filter mit X11 + V4L2 in C
// Fullscreen + Neon-Grün 5px-Kästen um „Geister“

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define VIDEO_DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
#define V4L2_BUFFERS 4

typedef unsigned char uchar;

// --- V4L2 Strukturen ---
typedef struct {
    void *start;
    size_t length;
} V4L2Buffer;

typedef struct {
    int fd;
    V4L2Buffer *buffers;
    unsigned int n_buffers;
} V4L2Data;

// --- X11 Strukturen ---
typedef struct {
    Display* display;
    Window window;
    GC gc;
    XImage* ximage;
    Visual* visual;
    int depth;
    int bytes_per_pixel;
    Atom wm_delete_window;
} XWindowData;

// --- V4L2 Funktionen ---
int v4l2_open(V4L2Data* v4l2) {
    v4l2->fd = open(VIDEO_DEVICE, O_RDWR | O_NONBLOCK);
    if (v4l2->fd == -1) { perror("Open video"); return 0; }

    struct v4l2_capability cap;
    if (ioctl(v4l2->fd, VIDIOC_QUERYCAP, &cap) == -1) { perror("VIDIOC_QUERYCAP"); return 0; }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) || !(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "Device not capable\n"); return 0;
    }
    return 1;
}

int v4l2_set_format(V4L2Data* v4l2) {
    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(v4l2->fd, VIDIOC_S_FMT, &fmt) == -1) { perror("VIDIOC_S_FMT"); return 0; }
    return 1;
}

int v4l2_init_mmap(V4L2Data* v4l2) {
    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = V4L2_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if(ioctl(v4l2->fd, VIDIOC_REQBUFS,&req)==-1){ perror("VIDIOC_REQBUFS"); return 0; }
    v4l2->buffers = calloc(req.count,sizeof(V4L2Buffer));
    v4l2->n_buffers = req.count;
    for(unsigned int i=0;i<v4l2->n_buffers;i++){
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory=V4L2_MEMORY_MMAP;
        buf.index=i;
        if(ioctl(v4l2->fd,VIDIOC_QUERYBUF,&buf)==-1){ perror("VIDIOC_QUERYBUF"); return 0; }
        v4l2->buffers[i].length=buf.length;
        v4l2->buffers[i].start=mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,v4l2->fd,buf.m.offset);
        if(v4l2->buffers[i].start==MAP_FAILED){ perror("mmap"); return 0; }
    }
    return 1;
}

int v4l2_start_streaming(V4L2Data* v4l2){
    for(unsigned int i=0;i<v4l2->n_buffers;i++){
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory=V4L2_MEMORY_MMAP;
        buf.index=i;
        if(ioctl(v4l2->fd,VIDIOC_QBUF,&buf)==-1){ perror("VIDIOC_QBUF"); return 0; }
    }
    enum v4l2_buf_type type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(v4l2->fd,VIDIOC_STREAMON,&type)==-1){ perror("VIDIOC_STREAMON"); return 0; }
    return 1;
}

uchar* v4l2_read_frame(V4L2Data* v4l2){
    struct v4l2_buffer buf;
    memset(&buf,0,sizeof(buf));
    buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory=V4L2_MEMORY_MMAP;
    if(ioctl(v4l2->fd,VIDIOC_DQBUF,&buf)==-1){ 
        if(errno==EAGAIN) return NULL;
        perror("VIDIOC_DQBUF"); return NULL;
    }
    uchar* frame=(uchar*)v4l2->buffers[buf.index].start;
    if(ioctl(v4l2->fd,VIDIOC_QBUF,&buf)==-1){ perror("VIDIOC_QBUF"); }
    return frame;
}

void v4l2_cleanup(V4L2Data* v4l2){
    if(v4l2->fd!=-1){
        enum v4l2_buf_type type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(v4l2->fd,VIDIOC_STREAMOFF,&type);
        for(unsigned int i=0;i<v4l2->n_buffers;i++){
            if(v4l2->buffers[i].start!=MAP_FAILED) munmap(v4l2->buffers[i].start,v4l2->buffers[i].length);
        }
        free(v4l2->buffers);
        close(v4l2->fd);
    }
}

// --- YUYV -> BGR ---
void yuyv_to_bgr(const uchar* yuyv, uchar* bgr){
    for(int i=0;i<WIDTH*HEIGHT/2;i++){
        int y1=yuyv[0]; int u=yuyv[1]; int y2=yuyv[2]; int v=yuyv[3];
        int u_p=u-128; int v_p=v-128;
        int r1=y1+1.403*v_p; int g1=y1-0.344*u_p-0.714*v_p; int b1=y1+1.770*u_p;
        int r2=y2+1.403*v_p; int g2=y2-0.344*u_p-0.714*v_p; int b2=y2+1.770*u_p;
        bgr[0]=(uchar)fmax(0,fmin(255,b1)); bgr[1]=(uchar)fmax(0,fmin(255,g1)); bgr[2]=(uchar)fmax(0,fmin(255,r1));
        bgr[3]=(uchar)fmax(0,fmin(255,b2)); bgr[4]=(uchar)fmax(0,fmin(255,g2)); bgr[5]=(uchar)fmax(0,fmin(255,r2));
        bgr+=6; yuyv+=4;
    }
}

// --- X11 Init ---
int init_x11(XWindowData* xwin){
    xwin->display=XOpenDisplay(NULL);
    if(!xwin->display){ fprintf(stderr,"Cannot open X display\n"); return 0; }
    int screen=DefaultScreen(xwin->display);
    xwin->visual=DefaultVisual(xwin->display,screen);
    xwin->depth=DefaultDepth(xwin->display,screen);
    xwin->bytes_per_pixel=4;
    xwin->window=XCreateSimpleWindow(xwin->display,RootWindow(xwin->display,screen),0,0,WIDTH,HEIGHT,0,0,0);
    XSelectInput(xwin->display,xwin->window,ExposureMask|KeyPressMask);
    xwin->gc=XCreateGC(xwin->display,xwin->window,0,NULL);
    XMapWindow(xwin->display,xwin->window);
    xwin->ximage=XCreateImage(xwin->display,xwin->visual,xwin->depth,ZPixmap,0,NULL,WIDTH,HEIGHT,32,WIDTH*4);
    xwin->ximage->data=malloc(WIDTH*HEIGHT*4);
    xwin->wm_delete_window=XInternAtom(xwin->display,"WM_DELETE_WINDOW",False);
    XSetWMProtocols(xwin->display,xwin->window,&xwin->wm_delete_window,1);
    return 1;
}

// --- Filter: einfache Schwellenwert-Umrahmung ---
void ghost_filter_box(uchar* bgr){
    int threshold=50;
    for(int y=1;y<HEIGHT-1;y++){
        for(int x=1;x<WIDTH-1;x++){
            int idx=(y*WIDTH+x)*3;
            int gray=(bgr[idx]+bgr[idx+1]+bgr[idx+2])/3;
            if(gray<threshold){
                // 5px Neon-Grün-Umrahmung
                for(int dy=-2;dy<=2;dy++){
                    for(int dx=-2;dx<=2;dx++){
                        int xx=x+dx,yy=y+dy;
                        if(xx<0||xx>=WIDTH||yy<0||yy>=HEIGHT) continue;
                        int ii=(yy*WIDTH+xx)*3;
                        bgr[ii]=0;       // B
                        bgr[ii+1]=255;   // G
                        bgr[ii+2]=0;     // R
                    }
                }
            }
        }
    }
}

void display_frame(XWindowData* xwin, uchar* bgr){
    uchar* dst=(uchar*)xwin->ximage->data;
    for(int i=0;i<WIDTH*HEIGHT;i++){
        dst[i*4+0]=bgr[i*3+0];
        dst[i*4+1]=bgr[i*3+1];
        dst[i*4+2]=bgr[i*3+2];
        dst[i*4+3]=0;
    }
    XPutImage(xwin->display,xwin->window,xwin->gc,xwin->ximage,0,0,0,0,WIDTH,HEIGHT);
    XFlush(xwin->display);
}

// --- Main ---
int main(){
    V4L2Data v4l2={0};
    if(!v4l2_open(&v4l2) || !v4l2_set_format(&v4l2) || !v4l2_init_mmap(&v4l2) || !v4l2_start_streaming(&v4l2)){
        fprintf(stderr,"V4L2 init failed\n"); return 1;
    }

    XWindowData xwin={0};
    if(!init_x11(&xwin)) return 1;

    uchar* frame_raw=(uchar*)malloc(WIDTH*HEIGHT*3);
    if(!frame_raw){ fprintf(stderr,"malloc failed\n"); return 1; }

    int running=1;
    while(running){
        while(XPending(xwin.display)){
            XEvent ev; XNextEvent(xwin.display,&ev);
            if(ev.type==KeyPress){
                KeySym k=XLookupKeysym(&ev.xkey,0);
                if(k==XK_Escape) running=0;
            }
        }
        uchar* yuyv=v4l2_read_frame(&v4l2);
        if(!yuyv){ usleep(10000); continue; }

        yuyv_to_bgr(yuyv,frame_raw);
        ghost_filter_box(frame_raw);
        display_frame(&xwin,frame_raw);
    }

    free(frame_raw);
    XDestroyImage(xwin.ximage);
    XFreeGC(xwin.display,xwin.gc);
    XDestroyWindow(xwin.display,xwin.window);
    XCloseDisplay(xwin.display);
    v4l2_cleanup(&v4l2);
    return 0;
}


