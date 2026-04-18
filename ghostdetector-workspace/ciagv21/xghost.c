// ghost_enhancer_hist128.c
// Ghost Enhancer v3 with per-pixel running histogram median (MED_BINS=128)
// - v4l2 capture (RGB24 assumed), histogram-based median, separable blur, sobel, etc.
// Compile: gcc -O2 -std=c11 ghost_enhancer_hist128.c -o ghost_enhancer_hist128 -lx11 -lm
// Run: ./ghost_enhancer_hist128 /dev/video0 640 480

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <time.h>

static const int RING_N = 25;            // temporal window
static const int BLUR_RADIUS = 1;        // separable gaussian radius
static const int MIN_BLOB_PX = 64;       // minimum blob size keep
static const int DS = 1;                 // downsample factor
static const int HIST_TARGET_COUNT = 500;
static const float EDGE_WEIGHT = 0.5f;
static const int AGE_MAX = 255;

#define MED_BINS 128
#define CLAMP(v,a,b) ((v)<(a)?(a):((v)>(b)?(b):(v)))

typedef struct {
    int w, h; int stride;
    uint8_t *rgb;
    uint8_t *lum;
    // histogram approach:
    uint16_t *pixel_hist; // size = w*h*MED_BINS
    uint8_t  *ring_vals;  // size = w*h*RING_N (stores last intensity per pixel)
    int       ring_idx;   // current index in ring (0..RING_N-1)
    uint8_t *median;
    uint8_t *blur;
    float   *sobel;
    float   *motion;
    uint8_t *mask;
    uint8_t *morph;
    int *labels;
    uint8_t *age;
    uint8_t *out_rgb;
} framebuf;

static double now_s(){
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec + t.tv_nsec*1e-9;
}

// ---- v4l2 minimal (read) ----
static int v4l2_open_capture(const char *dev, int width, int height, int *out_stride, void **out_buf, size_t *out_len){
    int fd = open(dev, O_RDWR);
    if(fd<0){ perror("open video"); return -1; }
    struct v4l2_format fmt; memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) < 0){ perror("VIDIOC_S_FMT"); close(fd); return -1; }
    int stride = fmt.fmt.pix.width * 3;
    size_t buflen = stride * fmt.fmt.pix.height;
    void *buf = malloc(buflen);
    if(!buf){ close(fd); return -1; }
    *out_stride = stride; *out_buf = buf; *out_len = buflen;
    return fd;
}

static int v4l2_read_frame(int fd, void *buf, size_t buflen){
    ssize_t r = read(fd, buf, buflen);
    if(r<0){ if(errno==EAGAIN) return 0; perror("read"); return -1; }
    return (int)r;
}

// ---- X11 minimal ----
typedef struct { Display *dpy; Window win; int screen; Visual *visual; GC gc; XImage *ximg; } x11disp;
static int x11_init(x11disp *xd, int w, int h){
    xd->dpy = XOpenDisplay(NULL); if(!xd->dpy){ fprintf(stderr,"Cannot open X display\n"); return -1; }
    xd->screen = DefaultScreen(xd->dpy); xd->visual = DefaultVisual(xd->dpy, xd->screen);
    xd->win = XCreateSimpleWindow(xd->dpy, RootWindow(xd->dpy, xd->screen), 10,10, w,h,1, BlackPixel(xd->dpy,xd->screen), WhitePixel(xd->dpy,xd->screen));
    XStoreName(xd->dpy, xd->win, "Ghost Enhancer hist128");
    XSelectInput(xd->dpy, xd->win, ExposureMask | KeyPressMask | StructureNotifyMask);
    xd->gc = DefaultGC(xd->dpy, xd->screen);
    XMapWindow(xd->dpy, xd->win); XFlush(xd->dpy);
    xd->ximg = XCreateImage(xd->dpy, xd->visual, DefaultDepth(xd->dpy, xd->screen), ZPixmap, 0, malloc(w*h*4), w, h, 32, 0);
    if(!xd->ximg){ fprintf(stderr,"XCreateImage failed\n"); return -1; }
    return 0;
}
static void x11_put_frame(x11disp *xd, framebuf *fb){
    int w=fb->w, h=fb->h; uint8_t *dst=(uint8_t*)xd->ximg->data; uint8_t *src=fb->out_rgb;
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        int di=(y*w+x)*4, si=(y*w+x)*3;
        dst[di+0]=src[si+2]; dst[di+1]=src[si+1]; dst[di+2]=src[si+0]; dst[di+3]=0;
    }
    XPutImage(xd->dpy, xd->win, xd->gc, xd->ximg, 0,0, 0,0, w,h); XFlush(xd->dpy);
}

// ---- Image ops ----
static void rgb_to_luminance(uint8_t *rgb, uint8_t *lum, int w, int h, int stride_rgb){
    int idx=0;
    for(int y=0;y<h;y++){
        int base = y * stride_rgb;
        for(int x=0;x<w;x++){
            int r = rgb[base + x*3 + 0];
            int g = rgb[base + x*3 + 1];
            int b = rgb[base + x*3 + 2];
            lum[idx++] = (uint8_t)CLAMP((int)((77*r + 150*g + 29*b)>>8), 0, 255);
        }
    }
}

static void make_gaussian_kernel(float *k, int r){
    int len = 2*r+1;
    float sigma = r>0 ? r/2.0f : 0.5f; float sum=0;
    for(int i=0;i<len;i++){ int d=i-r; k[i]=expf(-(d*d)/(2*sigma*sigma)); sum+=k[i]; }
    for(int i=0;i<len;i++) k[i]/=sum;
}
static void separable_blur(uint8_t *in, uint8_t *out, int w, int h, int r){
    int len = 2*r+1; float k[11]; if(len>11) len=11;
    make_gaussian_kernel(k, r);
    uint8_t *tmp = malloc(w*h);
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        float acc=0; for(int j=-r;j<=r;j++){ int sx=CLAMP(x+j,0,w-1); acc+=k[j+r]*in[y*w + sx]; }
        tmp[y*w + x] = (uint8_t)CLAMP((int)(acc+0.5f),0,255);
    }
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        float acc=0; for(int j=-r;j<=r;j++){ int sy=CLAMP(y+j,0,h-1); acc+=k[j+r]*tmp[sy*w + x]; }
        out[y*w + x] = (uint8_t)CLAMP((int)(acc+0.5f),0,255);
    }
    free(tmp);
}

static void sobel_magnitude(uint8_t *in, float *out, int w, int h){
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        int xm1=CLAMP(x-1,0,w-1), xp1=CLAMP(x+1,0,w-1), ym1=CLAMP(y-1,0,h-1), yp1=CLAMP(y+1,0,h-1);
        int a00=in[ym1*w + xm1], a01=in[ym1*w + x], a02=in[ym1*w + xp1];
        int a10=in[y*w + xm1],  a11=in[y*w + x],  a12=in[y*w + xp1];
        int a20=in[yp1*w + xm1], a21=in[yp1*w + x], a22=in[yp1*w + xp1];
        int gx = -a00 + a02 - 2*a10 + 2*a12 - a20 + a22;
        int gy = a00 + 2*a01 + a02 - a20 - 2*a21 - a22;
        out[y*w + x] = sqrtf((float)(gx*gx + gy*gy));
    }
}

static void compute_motion_score(framebuf *fb){
    int px = fb->w * fb->h; float maxdiff=1e-6f, maxedge=1e-6f;
    for(int i=0;i<px;i++){
        float d = fabsf((float)fb->blur[i] - (float)fb->median[i]); if(d>maxdiff) maxdiff=d;
        if(fb->sobel[i] > maxedge) maxedge = fb->sobel[i];
    }
    for(int i=0;i<px;i++){
        float d = fabsf((float)fb->blur[i] - (float)fb->median[i]) / maxdiff;
        float e = fb->sobel[i] / maxedge;
        fb->motion[i] = d + EDGE_WEIGHT * e;
    }
}

static uint8_t adaptive_threshold(framebuf *fb, int target_count){
    int px = fb->w * fb->h; int hist[256]={0};
    for(int i=0;i<px;i++){ int v = CLAMP((int)(fb->motion[i]*255.0f),0,255); hist[v]++; }
    int acc=0;
    for(int t=255;t>=0;t--){ acc+=hist[t]; if(acc>=target_count) return (uint8_t)t; }
    return 255;
}
static void motion_to_mask(framebuf *fb, uint8_t thr){
    int px=fb->w*fb->h; for(int i=0;i<px;i++){ int v=CLAMP((int)(fb->motion[i]*255.0f),0,255); fb->mask[i]=(v>=thr)?255:0; }
}
static void morphology_open(framebuf *fb){
    int w=fb->w, h=fb->h;
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        int minv=255;
        for(int oy=-1;oy<=1;oy++) for(int ox=-1;ox<=1;ox++){ int sx=CLAMP(x+ox,0,w-1), sy=CLAMP(y+oy,0,h-1); int v=fb->mask[sy*w + sx]; if(v<minv) minv=v; }
        fb->morph[y*w + x] = minv;
    }
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        int maxv=0;
        for(int oy=-1;oy<=1;oy++) for(int ox=-1;ox<=1;ox++){ int sx=CLAMP(x+ox,0,w-1), sy=CLAMP(y+oy,0,h-1); int v=fb->morph[sy*w + sx]; if(v>maxv) maxv=v; }
        fb->mask[y*w + x] = maxv;
    }
}

// two-pass CCL (as before)
static int connected_components(framebuf *fb, int min_px){
    int w=fb->w, h=fb->h, px=w*h;
    int *labels = fb->labels; memset(labels,0,sizeof(int)*px);
    int next_label = 1;
    int maxlabels = px/2 + 10;
    int *uf_parent = malloc(sizeof(int)*(maxlabels+1));
    for(int i=0;i<=maxlabels;i++) uf_parent[i]=i;
    int findp_impl(int a){ while(uf_parent[a]!=a) a=uf_parent[a]; return a; }
    void unionp_impl(int a, int b){ int pa=findp_impl(a), pb=findp_impl(b); if(pa!=pb) uf_parent[pb]=pa; }
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        int idx=y*w+x; if(fb->mask[idx]==0) continue;
        int label=0;
        if(x>0 && fb->mask[idx-1]) label = labels[idx-1];
        if(y>0 && fb->mask[idx-w]){
            if(label==0) label = labels[idx-w];
            else if(labels[idx-w] && labels[idx-w] != label) unionp_impl(label, labels[idx-w]);
        }
        if(label==0) label = next_label++;
        labels[idx] = label;
    }
    int *map = calloc(next_label+1, sizeof(int));
    for(int i=0;i<px;i++){ if(labels[i]==0) continue; int p=findp_impl(labels[i]); labels[i]=p; map[p]++; }
    int *compact = malloc(sizeof(int)*(next_label+1)); memset(compact,0,sizeof(int)*(next_label+1));
    int newlabel=1;
    for(int l=1;l<next_label;l++) if(map[l] >= min_px) compact[l]=newlabel++;
    for(int i=0;i<px;i++){ int l=labels[i]; labels[i] = (l>0 && compact[l]>0) ? compact[l] : 0; }
    free(uf_parent); free(map); free(compact);
    return newlabel;
}

// render & age update (same visual approach)
static void render_and_update_age(framebuf *fb){
    int w=fb->w, h=fb->h, px=w*h;
    for(int i=0;i<px;i++){
        uint8_t L = fb->blur[i];
        int r = CLAMP((int)(L * 1.2f),0,255);
        int g = CLAMP((int)(L * 0.95f),0,255);
        int b = CLAMP((int)(L * 0.7f),0,255);
        fb->out_rgb[i*3+0]=r; fb->out_rgb[i*3+1]=g; fb->out_rgb[i*3+2]=b;
    }
    for(int i=0;i<px;i++){
        if(fb->labels[i]) fb->age[i] = CLAMP(fb->age[i] + 16, 0, AGE_MAX);
        else if(fb->age[i] > 0) fb->age[i] = CLAMP(fb->age[i] - 4, 0, AGE_MAX);
    }
    for(int i=0;i<px;i++){
        if(fb->labels[i]){
            float agef = (float)fb->age[i] / (float)AGE_MAX;
            float edge = CLAMP(fb->sobel[i] / 128.0f, 0.0f, 1.0f);
            float alpha = CLAMP(0.2f + 0.8f * agef * edge, 0.0f, 1.0f);
            uint8_t hr = CLAMP((int)(80 + 175*agef), 0, 255);
            uint8_t hg = CLAMP((int)(200 + 55*agef), 0, 255);
            uint8_t hb = CLAMP((int)(200 + 55*agef), 0, 255);
            int ir = fb->out_rgb[i*3+0], ig = fb->out_rgb[i*3+1], ib = fb->out_rgb[i*3+2];
            fb->out_rgb[i*3+0] = CLAMP((int)((1-alpha)*ir + alpha*hr),0,255);
            fb->out_rgb[i*3+1] = CLAMP((int)((1-alpha)*ig + alpha*hg),0,255);
            fb->out_rgb[i*3+2] = CLAMP((int)((1-alpha)*ib + alpha*hb),0,255);
        }
    }
    int maxlabel=0; for(int i=0;i<px;i++) if(fb->labels[i] > maxlabel) maxlabel = fb->labels[i];
    if(maxlabel==0) return;
    int *minx = malloc(sizeof(int)*(maxlabel+1)); int *miny = malloc(sizeof(int)*(maxlabel+1));
    int *maxx = malloc(sizeof(int)*(maxlabel+1)); int *maxy = malloc(sizeof(int)*(maxlabel+1));
    for(int l=0;l<=maxlabel;l++){ minx[l]=fb->w; miny[l]=fb->h; maxx[l]=0; maxy[l]=0; }
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){ int idx=y*w+x; int l=fb->labels[idx]; if(l){ if(x<minx[l]) minx[l]=x; if(y<miny[l]) miny[l]=y; if(x>maxx[l]) maxx[l]=x; if(y>maxy[l]) maxy[l]=y; } }
    for(int l=1;l<=maxlabel;l++){
        if(minx[l]>maxx[l]||miny[l]>maxy[l]) continue;
        for(int x=minx[l]; x<=maxx[l]; x++){ int t=miny[l]*fb->w + x; int b=maxy[l]*fb->w + x; fb->out_rgb[t*3+0]=255; fb->out_rgb[t*3+1]=255; fb->out_rgb[t*3+2]=0; fb->out_rgb[b*3+0]=255; fb->out_rgb[b*3+1]=255; fb->out_rgb[b*3+2]=0; }
        for(int y=miny[l]; y<=maxy[l]; y++){ int le=y*fb->w + minx[l]; int ri=y*fb->w + maxx[l]; fb->out_rgb[le*3+0]=255; fb->out_rgb[le*3+1]=255; fb->out_rgb[le*3+2]=0; fb->out_rgb[ri*3+0]=255; fb->out_rgb[ri*3+1]=255; fb->out_rgb[ri*3+2]=0; }
    }
    free(minx); free(miny); free(maxx); free(maxy);
}

// ---- Histogram median maintenance (MED_BINS=128) ----
// pixel_hist: uint16_t [px * MED_BINS], ring_vals: uint8_t [px * RING_N]

static void update_hist_and_compute_median(framebuf *fb){
    int w=fb->w, h=fb->h, px=w*h;
    uint16_t *hist = fb->pixel_hist;
    uint8_t *rvals = fb->ring_vals;
    int ridx = fb->ring_idx;
    int half = (RING_N + 1) / 2;
    // If MED_BINS != 128, quantization mapping must be applied. Here MED_BINS=128 -> shift by 1 (256->128)
    for(int i=0;i<px;i++){
        uint8_t oldv = rvals[i * RING_N + ridx];
        uint8_t newv = fb->lum[i];
        // Map 0-255 -> 0-(MED_BINS-1)
        int oldbin = oldv >> 1;
        int newbin = newv >> 1;
        uint16_t *hp = &hist[i * MED_BINS];
        // decrement old bin (only if ring was initialized; harmless if count goes negative guard)
        if(hp[oldbin] > 0) hp[oldbin]--;
        // store new and increment
        rvals[i * RING_N + ridx] = newv;
        hp[newbin]++;
        // find median bin by cumulative sum
        int acc = 0; int median_bin = 0;
        for(int b=0;b<MED_BINS;b++){
            acc += hp[b];
            if(acc >= half){ median_bin = b; break; }
        }
        // map median_bin back to 0-255 value (use bin center)
        fb->median[i] = (uint8_t)CLAMP((median_bin * 2 + 1), 0, 255);
    }
    fb->ring_idx = (ridx + 1) % RING_N;
}

// ---- Main ----
int main(int argc, char **argv){
    if(argc < 4){ fprintf(stderr,"Usage: %s /dev/videoN WIDTH HEIGHT\n", argv[0]); return 1; }
    const char *dev = argv[1]; int W = atoi(argv[2]), H = atoi(argv[3]);
    int stride_rgb; void *capbuf; size_t caplen;
    int fd = v4l2_open_capture(dev, W, H, &stride_rgb, &capbuf, &caplen);
    if(fd<0) return 1;
    framebuf fb; memset(&fb,0,sizeof(fb));
    fb.w = W/DS; fb.h = H/DS; fb.stride = fb.w * 3;
    int px = fb.w * fb.h;
    fb.rgb = malloc(W * H * 3);
    fb.lum = malloc(px);
    // allocate histogram structures
    fb.pixel_hist = calloc((size_t)px * MED_BINS, sizeof(uint16_t)); // zeroed
    fb.ring_vals = malloc((size_t)px * RING_N); memset(fb.ring_vals, 0, (size_t)px * RING_N);
    fb.ring_idx = 0;
    fb.median = malloc(px);
    fb.blur = malloc(px);
    fb.sobel = malloc(sizeof(float) * px);
    fb.motion = malloc(sizeof(float) * px);
    fb.mask = malloc(px);
    fb.morph = malloc(px);
    fb.labels = malloc(sizeof(int) * px);
    fb.age = malloc(px); memset(fb.age,0,px);
    fb.out_rgb = malloc(px * 3);

    x11disp xd; if(x11_init(&xd, fb.w, fb.h) != 0) return 1;
    printf("Starting capture %s %dx%d -> processing %dx%d (MED_BINS=%d, RING_N=%d)\n", dev, W, H, fb.w, fb.h, MED_BINS, RING_N);
    double t0 = now_s(); int frames=0;

    // Warmup: read RING_N frames to initialize histograms more smoothly (optional)
    int warm = 0;
    while(warm < RING_N){
        int r = v4l2_read_frame(fd, capbuf, caplen);
        if(r <= 0){ continue; }
        uint8_t *src = capbuf;
        for(int y=0;y<fb.h;y++) for(int x=0;x<fb.w;x++){ int sx=x*DS, sy=y*DS, si=(sy*W + sx)*3, di=(y*fb.w + x)*3; fb.rgb[di+0]=src[si+0]; fb.rgb[di+1]=src[si+1]; fb.rgb[di+2]=src[si+2]; }
        rgb_to_luminance(fb.rgb, fb.lum, fb.w, fb.h, fb.stride);
        // populate ring slot ridx = warm
        for(int i=0;i<px;i++){
            uint8_t v = fb.lum[i];
            fb.ring_vals[i * RING_N + warm] = v;
            int bin = v >> 1;
            fb.pixel_hist[i * MED_BINS + bin]++;
        }
        warm++;
    }
    // after warmup, compute initial median from hist
    for(int i=0;i<px;i++){
        uint16_t *hp = &fb.pixel_hist[i * MED_BINS];
        int acc=0, half = (RING_N + 1)/2, median_bin=0;
        for(int b=0;b<MED_BINS;b++){ acc += hp[b]; if(acc >= half){ median_bin=b; break; } }
        fb.median[i] = (uint8_t)CLAMP((median_bin*2 + 1), 0, 255);
    }

    while(1){
        int r = v4l2_read_frame(fd, capbuf, caplen);
        if(r <= 0){ continue; }
        uint8_t *src = capbuf;
        for(int y=0;y<fb.h;y++) for(int x=0;x<fb.w;x++){ int sx=x*DS, sy=y*DS, si=(sy*W + sx)*3, di=(y*fb.w + x)*3; fb.rgb[di+0]=src[si+0]; fb.rgb[di+1]=src[si+1]; fb.rgb[di+2]=src[si+2]; }
        rgb_to_luminance(fb.rgb, fb.lum, fb.w, fb.h, fb.stride);

        // update histogram & compute median (MED_BINS quantizes 0-255 -> 0-127)
        update_hist_and_compute_median(&fb);

        // blur current luminance, sobel, motion, thr, morphology, CCL, render
        separable_blur(fb.lum, fb.blur, fb.w, fb.h, BLUR_RADIUS);
        sobel_magnitude(fb.blur, fb.sobel, fb.w, fb.h);
        compute_motion_score(&fb);
        uint8_t thr = adaptive_threshold(&fb, HIST_TARGET_COUNT);
        motion_to_mask(&fb, thr);
        morphology_open(&fb);
        connected_components(&fb, MIN_BLOB_PX);
        render_and_update_age(&fb);
        x11_put_frame(&xd, &fb);

        frames++;
        if(frames % 30 == 0){ double t = now_s(); double fps = frames / (t - t0); printf("FPS: %.2f\n", fps); }

        while(XPending(xd.dpy)){ XEvent ev; XNextEvent(xd.dpy, &ev); if(ev.type == KeyPress){ printf("KeyPress: exiting\n"); goto cleanup; } }
    }

cleanup:
    close(fd);
    free(capbuf);
    //XDestroyImage(xd.ximg);
    XDestroyWindow(xd.dpy, xd.win); XCloseDisplay(xd.dpy);
    free(fb.rgb); free(fb.lum);
    free(fb.pixel_hist); free(fb.ring_vals); free(fb.median);
    free(fb.blur); free(fb.sobel); free(fb.motion); free(fb.mask); free(fb.morph);
    free(fb.labels); free(fb.age); free(fb.out_rgb);
    return 0;
}

