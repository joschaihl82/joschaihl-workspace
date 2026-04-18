/*
### Kurzbeschreibung
Ich habe den Code so angepasst, dass beim **ersten empfangenen Frame** automatisch ein Template aus der Bildmitte (standardmäßig **64×64**) extrahiert und anschließend für die Template‑Matching‑Erkennung verwendet wird. Du kannst optional weiterhin eine Template‑Datei übergeben; wenn keine Datei angegeben ist, wird das erste Kameraframe als Vorlage genommen.

### Kompilieren

```bash

gcc -O2 -o face face.c `sdl2-config --cflags --libs` -lSDL2_image -lv4l2 -lpthread -lm
```

### Änderungen / Verhalten
- Wenn du `./face template.png` startest, wird `template.png` verwendet (wie vorher).
- Wenn du `./face` ohne Argument startest, wird **das erste Frame** von `/dev/video0` fotografiert: aus der Bildmitte wird ein 64×64‑Crop erzeugt und als Template genutzt.
- Template‑Statistiken (Mittelwert / Standardabweichung) werden berechnet und für NCC verwendet.
- Rest der Pipeline bleibt erhalten (V4L2 MMAP, Threads, DSP‑Vorverarbeitung, NCC‑Matching, NMS, Rendering).

---

### `face.c`
```c
*/
/* face.c
   Erstes Frame als Template fotografieren, falls keine Template-Datei angegeben.
   Compile:
     gcc -O2 -o face face.c `sdl2-config --cflags --libs` -lSDL2_image -lv4l2 -lpthread -lm
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
#define PIX_FMT V4L2_PIX_FMT_YUYV
#define BUFFER_COUNT 4
#define MAX_QUEUE 8

#define LOG(fmt, ...) do{ fprintf(stderr, "[%s] " fmt "\n", __func__, ##__VA_ARGS__); } while(0)

struct v4l2_buffer_info { void *start; size_t length; };

typedef struct { uint8_t *data; int w,h; } frame_t;

typedef struct {
    frame_t *items[MAX_QUEUE];
    int head, tail, count;
    pthread_mutex_t m;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} frame_queue_t;

static void fq_init(frame_queue_t *q){ q->head=q->tail=q->count=0; pthread_mutex_init(&q->m,NULL); pthread_cond_init(&q->not_empty,NULL); pthread_cond_init(&q->not_full,NULL); }
static void fq_push(frame_queue_t *q, frame_t *f){ pthread_mutex_lock(&q->m); while(q->count==MAX_QUEUE) pthread_cond_wait(&q->not_full,&q->m); q->items[q->tail]=f; q->tail=(q->tail+1)%MAX_QUEUE; q->count++; pthread_cond_signal(&q->not_empty); pthread_mutex_unlock(&q->m); }
static frame_t* fq_pop(frame_queue_t *q){ pthread_mutex_lock(&q->m); while(q->count==0) pthread_cond_wait(&q->not_empty,&q->m); frame_t *f = q->items[q->head]; q->head=(q->head+1)%MAX_QUEUE; q->count--; pthread_cond_signal(&q->not_full); pthread_mutex_unlock(&q->m); return f; }

typedef struct { int x,y,w,h; double score; } rect_t;

static double now_ms(){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC,&ts); return ts.tv_sec*1000.0 + ts.tv_nsec/1e6; }

static void yuyv_to_gray(const uint8_t *yuyv, uint8_t *gray, int w, int h){
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x+=2){
            int i = (y*w + x)*2;
            uint8_t Y0 = yuyv[i+0];
            uint8_t Y1 = yuyv[i+2];
            gray[y*w + x] = Y0;
            gray[y*w + x + 1] = Y1;
        }
    }
}

static void gaussian3(const uint8_t *in, uint8_t *out, int w, int h){
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            int sum=0, cnt=0;
            for(int ky=-1;ky<=1;ky++){
                int yy=y+ky; if(yy<0||yy>=h) continue;
                for(int kx=-1;kx<=1;kx++){
                    int xx=x+kx; if(xx<0||xx>=w) continue;
                    int k = (abs(kx)+abs(ky)==2)?1:2;
                    sum += in[yy*w+xx]*k;
                    cnt += k;
                }
            }
            out[y*w+x] = sum/cnt;
        }
    }
}

static void sobel_mag(const uint8_t *in, uint8_t *out, int w, int h){
    for(int y=1;y<h-1;y++){
        for(int x=1;x<w-1;x++){
            int gx = -in[(y-1)*w + (x-1)] -2*in[y*w + (x-1)] - in[(y+1)*w + (x-1)]
                     + in[(y-1)*w + (x+1)] +2*in[y*w + (x+1)] + in[(y+1)*w + (x+1)];
            int gy = -in[(y-1)*w + (x-1)] -2*in[(y-1)*w + x] - in[(y-1)*w + (x+1)]
                     + in[(y+1)*w + (x-1)] +2*in[(y+1)*w + x] + in[(y+1)*w + (x+1)];
            int mag = abs(gx) + abs(gy);
            if(mag>255) mag=255;
            out[y*w+x] = (uint8_t)mag;
        }
    }
    for(int x=0;x<w;x++){ out[x]=0; out[(h-1)*w + x]=0; }
    for(int y=0;y<h;y++){ out[y*w + 0]=0; out[y*w + (w-1)]=0; }
}

static void integral_image(const uint8_t *in, uint32_t *ii, int w, int h){
    for(int y=0;y<=h;y++){
        for(int x=0;x<=w;x++){
            if(y==0 || x==0){ ii[y*(w+1)+x]=0; continue; }
            uint32_t v = in[(y-1)*w + (x-1)];
            ii[y*(w+1)+x] = v + ii[y*(w+1)+(x-1)] + ii[(y-1)*(w+1)+x] - ii[(y-1)*(w+1)+(x-1)];
        }
    }
}

static inline uint32_t ii_sum(const uint32_t *ii, int w, int x1,int y1,int x2,int y2){
    int W = w+1;
    return ii[(y2+1)*W + (x2+1)] - ii[(y1)*W + (x2+1)] - ii[(y2+1)*W + x1] + ii[y1*W + x1];
}

static double ncc_patch(const uint8_t *img, int iw, int ih, const uint8_t *tpl, int tw, int th,
                        const uint32_t *ii_img, const uint32_t *ii_img_sq, double tpl_mean, double tpl_std,
                        int px, int py)
{
    uint32_t sum = ii_sum(ii_img, iw, px, py, px+tw-1, py+th-1);
    uint32_t sumsq = ii_sum(ii_img_sq, iw, px, py, px+tw-1, py+th-1);
    double area = (double)(tw*th);
    double mean = sum / area;
    double var = sumsq/area - mean*mean;
    double std = var>0 ? sqrt(var) : 1e-6;
    double sumIT = 0.0;
    for(int j=0;j<th;j++){
        for(int i=0;i<tw;i++){
            sumIT += img[(py+j)*iw + (px+i)] * tpl[j*tw + i];
        }
    }
    double sumT = tpl_mean * area;
    double num = sumIT - mean*sumT - tpl_mean*sum + area*mean*tpl_mean;
    double denom = (area-1) * std * tpl_std;
    if(denom==0) return 0.0;
    return num / denom;
}

static int nms(rect_t *in, int n, rect_t *out, double iou_thresh){
    int used[n]; memset(used,0,sizeof(used));
    for(int i=0;i<n;i++){
        int best=i;
        for(int j=i+1;j<n;j++) if(in[j].score > in[best].score) best=j;
        if(best!=i){ rect_t tmp=in[i]; in[i]=in[best]; in[best]=tmp; }
    }
    int outc=0;
    for(int i=0;i<n;i++){
        if(used[i]) continue;
        rect_t a = in[i];
        out[outc++]=a;
        for(int j=i+1;j<n;j++){
            if(used[j]) continue;
            rect_t b = in[j];
            int ix1 = fmax(a.x, b.x);
            int iy1 = fmax(a.y, b.y);
            int ix2 = fmin(a.x+a.w, b.x+b.w);
            int iy2 = fmin(a.y+a.h, b.y+b.h);
            int iw = ix2 - ix1;
            int ih = iy2 - iy1;
            double inter = (iw>0 && ih>0) ? (double)(iw*ih) : 0.0;
            double unionA = a.w*a.h + b.w*b.h - inter;
            double iou = unionA>0 ? inter/unionA : 0.0;
            if(iou > iou_thresh) used[j]=1;
        }
    }
    return outc;
}

static struct v4l2_buffer_info vbufs[BUFFER_COUNT];
static int vfd = -1;
static frame_queue_t fq;
static volatile int running = 1;

static uint8_t *tpl = NULL;
static int tpl_w=64, tpl_h=64;
static double tpl_mean=0.0, tpl_std=1.0;
static int tpl_ready = 0; // 0 = not set, 1 = set from file or first frame

static pthread_mutex_t det_m = PTHREAD_MUTEX_INITIALIZER;
typedef struct { rect_t rects[16]; int count; double ts; } detection_t;
static detection_t latest_det = { .count=0, .ts=0 };

static void store_detection(rect_t *rects, int n){
    pthread_mutex_lock(&det_m);
    latest_det.count = n>16?16:n;
    for(int i=0;i<latest_det.count;i++) latest_det.rects[i]=rects[i];
    latest_det.ts = now_ms();
    pthread_mutex_unlock(&det_m);
}

static void errno_exit(const char *s){ LOG("%s error %d, %s", s, errno, strerror(errno)); exit(EXIT_FAILURE); }
static int xioctl(int fd, int request, void *arg){ int r; do r = ioctl(fd, request, arg); while (r == -1 && EINTR == errno); return r; }

static int load_template(const char *path){
    SDL_Surface *s = IMG_Load(path);
    if(!s){ LOG("IMG_Load failed: %s", IMG_GetError()); return -1; }
    SDL_Surface *surf = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGB24, 0);
    SDL_FreeSurface(s);
    if(!surf){ LOG("ConvertSurface failed"); return -1; }
    tpl_w = surf->w; tpl_h = surf->h;
    tpl = malloc(tpl_w*tpl_h);
    uint8_t *pixels = (uint8_t*)surf->pixels;
    for(int y=0;y<tpl_h;y++){
        for(int x=0;x<tpl_w;x++){
            int i = (y*tpl_w + x)*3;
            uint8_t r = pixels[i], g = pixels[i+1], b = pixels[i+2];
            tpl[y*tpl_w + x] = (uint8_t)(0.299*r + 0.587*g + 0.114*b);
        }
    }
    double sum=0, sumsq=0;
    for(int i=0;i<tpl_w*tpl_h;i++){ sum += tpl[i]; sumsq += tpl[i]*tpl[i]; }
    double area = tpl_w*tpl_h;
    tpl_mean = sum/area;
    double var = sumsq/area - tpl_mean*tpl_mean;
    tpl_std = var>0 ? sqrt(var) : 1e-6;
    tpl_ready = 1;
    SDL_FreeSurface(surf);
    LOG("Loaded template %s (%dx%d) mean=%.2f std=%.2f", path, tpl_w, tpl_h, tpl_mean, tpl_std);
    return 0;
}

static void create_template_from_frame(const uint8_t *gray, int w, int h, int tw, int th){
    if(tpl) free(tpl);
    tpl_w = tw; tpl_h = th;
    tpl = malloc(tpl_w*tpl_h);
    int cx = w/2, cy = h/2;
    int sx = cx - tw/2, sy = cy - th/2;
    if(sx<0) sx=0; if(sy<0) sy=0;
    if(sx+tw > w) sx = w - tw;
    if(sy+th > h) sy = h - th;
    for(int y=0;y<th;y++){
        for(int x=0;x<tw;x++){
            tpl[y*tw + x] = gray[(sy+y)*w + (sx+x)];
        }
    }
    double sum=0, sumsq=0;
    for(int i=0;i<tpl_w*tpl_h;i++){ sum += tpl[i]; sumsq += tpl[i]*tpl[i]; }
    double area = tpl_w*tpl_h;
    tpl_mean = sum/area;
    double var = sumsq/area - tpl_mean*tpl_mean;
    tpl_std = var>0 ? sqrt(var) : 1e-6;
    tpl_ready = 1;
    LOG("Created template from first frame at center (%d,%d) size %dx%d mean=%.2f std=%.2f", sx, sy, tpl_w, tpl_h, tpl_mean, tpl_std);
}

static void *processing_thread(void *arg){
    (void)arg;
    uint8_t *blur = malloc(WIDTH*HEIGHT);
    uint8_t *sob = malloc(WIDTH*HEIGHT);
    uint32_t *ii = malloc((WIDTH+1)*(HEIGHT+1)*sizeof(uint32_t));
    uint32_t *ii_sq = malloc((WIDTH+1)*(HEIGHT+1)*sizeof(uint32_t));
    int first_frame_used_for_template = 0;
    while(running){
        frame_t *f = fq_pop(&fq);
        if(!f) continue;
        // If no template provided and not yet created, use this first frame's center crop
        if(!tpl_ready && !first_frame_used_for_template){
            create_template_from_frame(f->data, f->w, f->h, 64, 64);
            first_frame_used_for_template = 1;
        }
        gaussian3(f->data, blur, f->w, f->h);
        sobel_mag(blur, sob, f->w, f->h);
        for(int y=0;y<=f->h;y++) for(int x=0;x<=f->w;x++){ ii[y*(f->w+1)+x]=0; ii_sq[y*(f->w+1)+x]=0; }
        for(int y=0;y<f->h;y++){
            for(int x=0;x<f->w;x++){
                uint32_t v = sob[y*f->w + x];
                ii[(y+1)*(f->w+1) + (x+1)] = v;
                ii_sq[(y+1)*(f->w+1) + (x+1)] = v*v;
            }
        }
        for(int y=1;y<=f->h;y++){
            for(int x=1;x<=f->w;x++){
                ii[y*(f->w+1)+x] += ii[y*(f->w+1)+x-1] + ii[(y-1)*(f->w+1)+x] - ii[(y-1)*(f->w+1)+x-1];
                ii_sq[y*(f->w+1)+x] += ii_sq[y*(f->w+1)+x-1] + ii_sq[(y-1)*(f->w+1)+x] - ii_sq[(y-1)*(f->w+1)+x-1];
            }
        }
        rect_t candidates[256]; int candc=0;
        if(tpl_ready){
            int scales = 3;
            double scale_factors[3] = {1.0, 0.75, 0.5};
            for(int s=0;s<scales;s++){
                double sf = scale_factors[s];
                int sw = (int)round(tpl_w * sf);
                int sh = (int)round(tpl_h * sf);
                if(sw<8 || sh<8) continue;
                int stride = f->w/200 + 4;
                for(int y=0;y<=f->h - sh; y+=stride){
                    for(int x=0;x<=f->w - sw; x+=stride){
                        uint32_t sumEdge = ii_sum(ii, f->w, x, y, x+sw-1, y+sh-1);
                        double meanEdge = sumEdge / (double)(sw*sh);
                        if(meanEdge < 10) continue;
                        uint8_t *tpl_s = malloc(sw*sh);
                        double sum=0, sumsq=0;
                        for(int j=0;j<sh;j++){
                            for(int i=0;i<sw;i++){
                                int srcx = (int)(i / sf);
                                int srcy = (int)(j / sf);
                                if(srcx<0) srcx=0; if(srcx>=tpl_w) srcx=tpl_w-1;
                                if(srcy<0) srcy=0; if(srcy>=tpl_h) srcy=tpl_h-1;
                                uint8_t v = tpl[srcy*tpl_w + srcx];
                                tpl_s[j*sw + i] = v;
                                sum += v; sumsq += v*v;
                            }
                        }
                        double area = sw*sh;
                        double tpl_s_mean = sum/area;
                        double var = sumsq/area - tpl_s_mean*tpl_s_mean;
                        double tpl_s_std = var>0 ? sqrt(var) : 1e-6;
                        double score = ncc_patch(sob, f->w, f->h, tpl_s, sw, sh, ii, ii_sq, tpl_s_mean, tpl_s_std, x, y);
                        free(tpl_s);
                        if(score > 0.45){
                            if(candc < 256){
                                candidates[candc].x = x;
                                candidates[candc].y = y;
                                candidates[candc].w = sw;
                                candidates[candc].h = sh;
                                candidates[candc].score = score;
                                candc++;
                            }
                        }
                    }
                }
            }
        }
        rect_t final[16];
        int finalc = nms(candidates, candc, final, 0.3);
        store_detection(final, finalc);
        free(f->data);
        free(f);
    }
    free(blur); free(sob); free(ii); free(ii_sq);
    return NULL;
}

static struct v4l2_buffer_info vbufs_local[BUFFER_COUNT];

static void *capture_thread(void *arg){
    (void)arg;
    int vfd_local = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
    if(vfd_local < 0) { LOG("Cannot open %s: %s", DEVICE, strerror(errno)); running=0; return NULL; }
    struct v4l2_capability cap;
    if(xioctl(vfd_local, VIDIOC_QUERYCAP, &cap) < 0) errno_exit("VIDIOC_QUERYCAP");
    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) { LOG("Not a video capture device"); running=0; return NULL; }
    if(!(cap.capabilities & V4L2_CAP_STREAMING)) { LOG("Device does not support streaming I/O"); running=0; return NULL; }
    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = PIX_FMT;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if(xioctl(vfd_local, VIDIOC_S_FMT, &fmt) < 0) errno_exit("VIDIOC_S_FMT");
    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if(xioctl(vfd_local, VIDIOC_REQBUFS, &req) < 0) errno_exit("VIDIOC_REQBUFS");
    for(int i=0;i<req.count;i++){
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if(xioctl(vfd_local, VIDIOC_QUERYBUF, &buf) < 0) errno_exit("VIDIOC_QUERYBUF");
        vbufs_local[i].length = buf.length;
        vbufs_local[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, vfd_local, buf.m.offset);
        if(vbufs_local[i].start == MAP_FAILED) errno_exit("mmap");
    }
    for(int i=0;i<req.count;i++){
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if(xioctl(vfd_local, VIDIOC_QBUF, &buf) < 0) errno_exit("VIDIOC_QBUF");
    }
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(xioctl(vfd_local, VIDIOC_STREAMON, &type) < 0) errno_exit("VIDIOC_STREAMON");
    LOG("Capture started on %s (%dx%d)", DEVICE, WIDTH, HEIGHT);
    while(running){
        fd_set fds; FD_ZERO(&fds); FD_SET(vfd_local, &fds);
        struct timeval tv = {2,0};
        int r = select(vfd_local+1, &fds, NULL, NULL, &tv);
        if(r == -1){ if(errno==EINTR) continue; errno_exit("select"); }
        if(r==0){ LOG("select timeout"); continue; }
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if(xioctl(vfd_local, VIDIOC_DQBUF, &buf) < 0){
            if(errno==EAGAIN) continue;
            errno_exit("VIDIOC_DQBUF");
        }
        frame_t *f = malloc(sizeof(frame_t));
        f->w = WIDTH; f->h = HEIGHT;
        f->data = malloc(WIDTH*HEIGHT);
        yuyv_to_gray((uint8_t*)vbufs_local[buf.index].start, f->data, WIDTH, HEIGHT);
        fq_push(&fq, f);
        if(xioctl(vfd_local, VIDIOC_QBUF, &buf) < 0) errno_exit("VIDIOC_QBUF");
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(vfd_local, VIDIOC_STREAMOFF, &type);
    for(int i=0;i<BUFFER_COUNT;i++) munmap(vbufs_local[i].start, vbufs_local[i].length);
    close(vfd_local);
    return NULL;
}

int main(int argc, char **argv){
    fq_init(&fq);
    if(argc>1){
        if(load_template(argv[1])<0){
            LOG("Failed to load template file, will use first frame as template");
            tpl_ready = 0;
        }
    } else {
        tpl_ready = 0; // will create from first frame
    }

    pthread_t cap_thread, proc_thread;
    if(pthread_create(&cap_thread, NULL, capture_thread, NULL)!=0){ LOG("pthread create capture failed"); return 1; }
    if(pthread_create(&proc_thread, NULL, processing_thread, NULL)!=0){ LOG("pthread create proc failed"); return 1; }

    if(SDL_Init(SDL_INIT_VIDEO) != 0){ LOG("SDL_Init: %s", SDL_GetError()); running=0; }
    SDL_Window *win = SDL_CreateWindow("Face DSP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    uint8_t *display = malloc(WIDTH*HEIGHT*3);

    double last_fps_ts = now_ms();
    int frames = 0;

    while(running){
        frame_t *f = NULL;
        pthread_mutex_lock(&fq.m);
        if(fq.count>0){
            int idx = (fq.tail - 1 + MAX_QUEUE) % MAX_QUEUE;
            f = fq.items[idx];
        }
        pthread_mutex_unlock(&fq.m);

        if(f){
            for(int i=0;i<WIDTH*HEIGHT;i++){
                uint8_t v = f->data[i];
                display[3*i+0]=v; display[3*i+1]=v; display[3*i+2]=v;
            }
            SDL_UpdateTexture(tex, NULL, display, WIDTH*3);
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, tex, NULL, NULL);

            pthread_mutex_lock(&det_m);
            for(int i=0;i<latest_det.count;i++){
                rect_t r = latest_det.rects[i];
                SDL_Rect rect = { r.x, r.y, r.w, r.h };
                SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
                SDL_RenderDrawRect(ren, &rect);
            }
            pthread_mutex_unlock(&det_m);

            frames++;
            double t = now_ms();
            if(t - last_fps_ts >= 1000.0){
                double fps = frames * 1000.0 / (t - last_fps_ts);
                char title[128]; snprintf(title, sizeof(title), "Face DSP - FPS: %.1f Detections: %d TemplateReady:%d", fps, latest_det.count, tpl_ready);
                SDL_SetWindowTitle(win, title);
                last_fps_ts = t; frames = 0;
            }

            SDL_RenderPresent(ren);
        }

        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){ running=0; break; }
            if(e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_q) running=0;
            }
        }
        SDL_Delay(10);
    }

    LOG("Shutting down...");
    pthread_join(cap_thread, NULL);
    pthread_join(proc_thread, NULL);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(display);
    if(tpl) free(tpl);
    return 0;
}
/*
```

---

Wenn du möchtest, passe ich die Position/Größe des automatisch extrahierten Templates an (z. B. linke obere Viertel, dynamische Gesichtslokalisierung per heuristischer Hautfarb‑Segmentierung vor dem Crop) oder füge eine kurze visuelle Bestätigung ein (z. B. erstes Frame anzeigen und per Taste bestätigen, bevor das Template gesetzt wird). Welche Variante bevorzugst du?

*/
