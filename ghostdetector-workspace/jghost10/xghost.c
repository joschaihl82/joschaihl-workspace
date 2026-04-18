// ghost_cam_amalgam_fixed2.c
// Änderungen gegenüber vorheriger Version:
// - renamed Region -> MyRegion to avoid X11 Region conflict
// - ensured bufs/nbufs visible in cleanup and used consistently
// - added defensive checks on buffer indices and lengths
// Build:
//   gcc -O2 -Wall -Wextra -std=gnu11 -o ghost_cam_amalgam_fixed2 ghost_cam_amalgam_fixed2.c -lX11 -lm
// Run:
//   ./ghost_cam_amalgam_fixed2 [/dev/video0]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <errno.h>

#define WIDTH 640
#define HEIGHT 480
#define BUFFER_COUNT 4
#define MAX_HISTORY 256
#define MIN_REGION_AREA 200
#define MAX_REGIONS 128
#define IOU_MATCH_THRESH 0.15
#define DFT_REPORT_INTERVAL 60
#define STALE_FRAMES 150

typedef struct { void *start; size_t length; } Buffer;
typedef struct { int x0,y0,x1,y1,area; } BBox;

// renamed to avoid conflict with X11 Region typedef
typedef struct {
    int x, y, w, h;
    double hist[MAX_HISTORY];
    int hist_pos;
    int hist_count;
    int id;
    int last_seen_frame;
} MyRegion;

// ---------- helpers ----------
static inline void yuyv_to_rgb_pixel(uint8_t y, uint8_t u, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
    int C = (int)y - 16;
    int D = (int)u - 128;
    int E = (int)v - 128;
    int R = (298 * C + 409 * E + 128) >> 8;
    int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
    int B = (298 * C + 516 * D + 128) >> 8;
    *r = (uint8_t) (R < 0 ? 0 : (R > 255 ? 255 : R));
    *g = (uint8_t) (G < 0 ? 0 : (G > 255 ? 255 : G));
    *b = (uint8_t) (B < 0 ? 0 : (B > 255 ? 255 : B));
}

static inline void rgb_to_hsv_pixel(uint8_t r, uint8_t g, uint8_t b, double *h, double *s, double *v) {
    double rd=r/255.0, gd=g/255.0, bd=b/255.0;
    double mx = fmax(rd, fmax(gd, bd));
    double mn = fmin(rd, fmin(gd, bd));
    double d = mx - mn;
    *v = mx;
    *s = (mx <= 0.0) ? 0.0 : d / mx;
    if (d == 0.0) { *h = 0.0; return; }
    if (mx == rd) *h = 60.0 * fmod(((gd - bd) / d), 6.0);
    else if (mx == gd) *h = 60.0 * (((bd - rd) / d) + 2.0);
    else *h = 60.0 * (((rd - gd) / d) + 4.0);
    if (*h < 0.0) *h += 360.0;
}

// small morphology (3x3 open/close)
static void morph_open_close(uint8_t *mask, int w, int h) {
    uint8_t *tmp = malloc((size_t)w*h);
    if (!tmp) return;
    // erosion 3x3
    for (int y=0;y<h;y++) for (int x=0;x<w;x++) {
        uint8_t m = 255;
        for (int j=-1;j<=1;j++) for (int i=-1;i<=1;i++) {
            int nx=x+i, ny=y+j;
            if (nx<0||ny<0||nx>=w||ny>=h) { m = 0; continue; }
            uint8_t v = mask[ny*w+nx];
            if (v < m) m = v;
        }
        tmp[y*w+x] = m;
    }
    // dilation 3x3
    for (int y=0;y<h;y++) for (int x=0;x<w;x++) {
        uint8_t m = 0;
        for (int j=-1;j<=1;j++) for (int i=-1;i<=1;i++) {
            int nx=x+i, ny=y+j;
            if (nx<0||ny<0||nx>=w||ny>=h) continue;
            uint8_t v = tmp[ny*w+nx];
            if (v > m) m = v;
        }
        mask[y*w+x] = m;
    }
    // close: dilation then erosion (3x3)
    for (int y=0;y<h;y++) for (int x=0;x<w;x++) {
        uint8_t m = 0;
        for (int j=-1;j<=1;j++) for (int i=-1;i<=1;i++) {
            int nx=x+i, ny=y+j;
            if (nx<0||ny<0||nx>=w||ny>=h) continue;
            uint8_t v = mask[ny*w+nx];
            if (v > m) m = v;
        }
        tmp[y*w+x] = m;
    }
    for (int y=0;y<h;y++) for (int x=0;x<w;x++) {
        uint8_t m = 255;
        for (int j=-1;j<=1;j++) for (int i=-1;i<=1;i++) {
            int nx=x+i, ny=y+j;
            if (nx<0||ny<0||nx>=w||ny>=h) { m = 0; continue; }
            uint8_t v = tmp[ny*w+nx];
            if (v < m) m = v;
        }
        mask[y*w+x] = m;
    }
    free(tmp);
}

static int extract_bboxes(uint8_t *mask, int w, int h, BBox *out, int max_out) {
    uint8_t *vis = calloc((size_t)w*h,1);
    if (!vis) return 0;
    int *stackx = malloc((size_t)w*h*sizeof(int));
    int *stacky = malloc((size_t)w*h*sizeof(int));
    if (!stackx || !stacky) { free(vis); free(stackx); free(stacky); return 0; }
    int found = 0;
    for (int y=0;y<h;y++) for (int x=0;x<w;x++) {
        int idx = y*w + x;
        if (!mask[idx] || vis[idx]) continue;
        int sp=0;
        stackx[sp]=x; stacky[sp]=y; sp++;
        int x0=x,x1=x,y0=y,y1=y, area=0;
        while (sp>0) {
            sp--; int cx=stackx[sp], cy=stacky[sp];
            if (cx<0||cy<0||cx>=w||cy>=h) continue;
            int cidx = cy*w + cx;
            if (vis[cidx]) continue;
            if (!mask[cidx]) continue;
            vis[cidx]=1; area++;
            if (cx<x0) x0=cx; if (cx>x1) x1=cx;
            if (cy<y0) y0=cy; if (cy>y1) y1=cy;
            const int dx[4]={1,-1,0,0}, dy[4]={0,0,1,-1};
            for (int k=0;k<4;k++) {
                int nx=cx+dx[k], ny=cy+dy[k];
                if (nx<0||ny<0||nx>=w||ny>=h) continue;
                int nidx = ny*w + nx;
                if (!vis[nidx] && mask[nidx]) {
                    stackx[sp]=nx; stacky[sp]=ny; sp++;
                }
            }
        }
        if (area >= MIN_REGION_AREA && found < max_out) {
            out[found].x0 = x0; out[found].y0 = y0;
            out[found].x1 = x1; out[found].y1 = y1;
            out[found].area = area;
            found++;
        }
    }
    free(stackx); free(stacky); free(vis);
    return found;
}

static double bbox_iou(const BBox *a, const BBox *b) {
    int ix0 = a->x0 > b->x0 ? a->x0 : b->x0;
    int iy0 = a->y0 > b->y0 ? a->y0 : b->y0;
    int ix1 = a->x1 < b->x1 ? a->x1 : b->x1;
    int iy1 = a->y1 < b->y1 ? a->y1 : b->y1;
    if (ix1 < ix0 || iy1 < iy0) return 0.0;
    int inter = (ix1 - ix0 + 1) * (iy1 - iy0 + 1);
    int areaA = (a->x1 - a->x0 + 1) * (a->y1 - a->y0 + 1);
    int areaB = (b->x1 - b->x0 + 1) * (b->y1 - b->y0 + 1);
    return (double)inter / (double)(areaA + areaB - inter);
}

static void dft_peak(const double *x, int n, double fps, double *out_f, double *out_p) {
    if (n < 8) { *out_f = 0.0; *out_p = 0.0; return; }
    int N=n, half=N/2;
    double best_p=0.0, best_f=0.0;
    for (int k=1;k<=half;k++) {
        double re=0.0, im=0.0;
        for (int t=0;t<N;t++) {
            double ang = 2.0*M_PI*k*t/N;
            re += x[t]*cos(ang);
            im -= x[t]*sin(ang);
        }
        double p = sqrt(re*re + im*im);
        double freq = (double)k * fps / (double)N;
        if (p > best_p) { best_p = p; best_f = freq; }
    }
    *out_f = best_f; *out_p = best_p;
}

int main(int argc, char **argv) {
    const char *dev = (argc>1)? argv[1] : "/dev/video0";
    int fd = open(dev, O_RDWR | O_CLOEXEC);
    if (fd < 0) { perror("open"); return 1; }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); close(fd); return 1; }

    struct v4l2_requestbuffers req = {0};
    req.count = BUFFER_COUNT; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); close(fd); return 1; }
    if (req.count < 2) { fprintf(stderr,"Not enough buffers\n"); close(fd); return 1; }

    Buffer bufs[BUFFER_COUNT];
    unsigned int nbufs = (req.count < BUFFER_COUNT) ? req.count : BUFFER_COUNT;
    for (unsigned int i=0;i<nbufs;i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); close(fd); return 1; }
        bufs[i].length = buf.length;
        bufs[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (bufs[i].start == MAP_FAILED) { perror("mmap"); close(fd); return 1; }
    }
    for (unsigned int i=0;i<nbufs;i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); close(fd); return 1; }
    }
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); close(fd); return 1; }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr,"XOpenDisplay failed\n"); goto cleanup_stream; }
    int scr = DefaultScreen(dpy);
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy,scr), 0,0, WIDTH, HEIGHT, 1, BlackPixel(dpy,scr), WhitePixel(dpy,scr));
    XSelectInput(dpy, win, ExposureMask | KeyPressMask);
    XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, NULL);
    Visual *vis = DefaultVisual(dpy, scr);
    XImage *xim = XCreateImage(dpy, vis, DefaultDepth(dpy,scr), ZPixmap, 0, malloc((size_t)WIDTH*HEIGHT*4), WIDTH, HEIGHT, 32, 0);
    if (!xim || !xim->data) { fprintf(stderr,"XImage failed\n"); goto cleanup_x; }

    uint8_t *mask = calloc((size_t)WIDTH*HEIGHT,1);
    if (!mask) { fprintf(stderr,"OOM\n"); goto cleanup_x; }

    MyRegion regions[MAX_REGIONS];
    int region_count = 0;
    int next_id = 1;
    int frame_idx = 0;
    double fps = 30.0;

    struct v4l2_buffer vbuf;
    memset(&vbuf,0,sizeof(vbuf));

    for (;;) {
        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; vbuf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &vbuf) < 0) {
            if (errno == EAGAIN) { usleep(1000); continue; }
            perror("VIDIOC_DQBUF"); break;
        }
        if ((unsigned)vbuf.index >= nbufs) { fprintf(stderr,"bad buf index %u\n", vbuf.index); break; }
        uint8_t *frame = (uint8_t*)bufs[vbuf.index].start;
        size_t frame_bytes = bufs[vbuf.index].length;

        // sample medians
        double hvals[64], svals[64], vvals[64]; int vi=0;
        for (int yy=HEIGHT/16; yy<HEIGHT && vi<64; yy+=HEIGHT/8) {
            for (int xx=WIDTH/16; xx<WIDTH && vi<64; xx+=WIDTH/8) {
                int off = (yy*WIDTH + (xx/2)) * 2;
                if ((size_t)off+3 >= frame_bytes) continue;
                uint8_t y0 = frame[off + (xx%2==0?0:2)];
                uint8_t u  = frame[off + 1];
                uint8_t v  = frame[off + 3];
                uint8_t r,g,b; yuyv_to_rgb_pixel(y0,u,v,&r,&g,&b);
                double H,S,Vv; rgb_to_hsv_pixel(r,g,b,&H,&S,&Vv);
                hvals[vi]=H; svals[vi]=S; vvals[vi]=Vv; vi++;
            }
        }
        int n = vi?vi:1;
        for (int i=0;i<n-1;i++) for (int j=i+1;j<n;j++) if (hvals[i]>hvals[j]) { double t=hvals[i]; hvals[i]=hvals[j]; hvals[j]=t; }
        for (int i=0;i<n-1;i++) for (int j=i+1;j<n;j++) if (svals[i]>svals[j]) { double t=svals[i]; svals[i]=svals[j]; svals[j]=t; }
        for (int i=0;i<n-1;i++) for (int j=i+1;j<n;j++) if (vvals[i]>vvals[j]) { double t=vvals[i]; vvals[i]=vvals[j]; vvals[j]=t; }
        double h_med = hvals[n/2], s_med = svals[n/2], v_med = vvals[n/2];
        double k_h=40.0, k_s=0.25, k_v=0.20;

        // build mask
        for (int y=0;y<HEIGHT;y++) for (int x=0;x<WIDTH;x++) {
            int pix = y*WIDTH + x;
            int off = (y*WIDTH + (x/2)) * 2;
            if ((size_t)off+3 >= frame_bytes) { mask[pix]=0; continue; }
            uint8_t y0 = frame[off + (x%2==0?0:2)];
            uint8_t u  = frame[off + 1];
            uint8_t v  = frame[off + 3];
            uint8_t r,g,b; yuyv_to_rgb_pixel(y0,u,v,&r,&g,&b);
            double H,S,Vv; rgb_to_hsv_pixel(r,g,b,&H,&S,&Vv);
            int is_anom = 0;
            double dh = fabs(H - h_med); if (dh > 180.0) dh = 360.0 - dh;
            if (dh > k_h) is_anom = 1;
            if (fabs(S - s_med) > k_s) is_anom = 1;
            if (fabs(Vv - v_med) > k_v) is_anom = 1;
            mask[pix] = is_anom ? 255 : 0;
        }

        morph_open_close(mask, WIDTH, HEIGHT);

        BBox boxes[64];
        int found = extract_bboxes(mask, WIDTH, HEIGHT, boxes, 64);

        int assigned_box[64]; for (int i=0;i<64;i++) assigned_box[i]=-1;
        int assigned_reg[MAX_REGIONS]; for (int i=0;i<MAX_REGIONS;i++) assigned_reg[i]=-1;

        for (int r=0;r<region_count;r++) {
            double best_iou=0.0; int best_b=-1;
            BBox regbox = { regions[r].x, regions[r].y, regions[r].x + regions[r].w - 1, regions[r].y + regions[r].h - 1, 0 };
            for (int b=0;b<found;b++) {
                if (assigned_box[b] != -1) continue;
                double iou = bbox_iou(&boxes[b], &regbox);
                if (iou > best_iou) { best_iou = iou; best_b = b; }
            }
            if (best_b != -1 && best_iou >= IOU_MATCH_THRESH) { assigned_box[best_b] = r; assigned_reg[r] = best_b; }
        }

        for (int b=0;b<found;b++) {
            if (assigned_box[b] != -1) continue;
            if (region_count >= MAX_REGIONS) break;
            int x0 = boxes[b].x0, y0 = boxes[b].y0;
            int w = boxes[b].x1 - boxes[b].x0 + 1;
            int h = boxes[b].y1 - boxes[b].y0 + 1;
            MyRegion *nr = &regions[region_count];
            nr->x = x0; nr->y = y0; nr->w = w; nr->h = h;
            nr->hist_pos = 0; nr->hist_count = 0;
            nr->id = next_id++; nr->last_seen_frame = frame_idx;
            region_count++; assigned_box[b] = region_count - 1;
        }

        for (int r=0;r<region_count;r++) {
            int b = assigned_reg[r];
            if (b == -1) continue;
            int x = boxes[b].x0, y = boxes[b].y0;
            int w = boxes[b].x1 - boxes[b].x0 + 1;
            int h = boxes[b].y1 - boxes[b].y0 + 1;
            long sum=0; long cnt=0;
            int step_y = (h>30)?2:1; int step_x = (w>30)?2:1;
            for (int yy=y; yy<y+h; yy+=step_y) {
                if (yy<0||yy>=HEIGHT) continue;
                for (int xx=x; xx<x+w; xx+=step_x) {
                    if (xx<0||xx>=WIDTH) continue;
                    int off = (yy*WIDTH + (xx/2)) * 2;
                    if ((size_t)off+3 >= frame_bytes) continue;
                    uint8_t y0 = frame[off + (xx%2==0?0:2)];
                    sum += y0; cnt++;
                }
            }
            double mean = cnt ? ((double)sum / (double)cnt) : 0.0;
            MyRegion *reg = &regions[r];
            reg->hist[reg->hist_pos] = mean;
            reg->hist_pos = (reg->hist_pos + 1) % MAX_HISTORY;
            if (reg->hist_count < MAX_HISTORY) reg->hist_count++;
            reg->last_seen_frame = frame_idx;
            reg->x = boxes[b].x0; reg->y = boxes[b].y0; reg->w = boxes[b].x1 - boxes[b].x0 + 1; reg->h = boxes[b].y1 - boxes[b].y0 + 1;
        }

        int write_i=0;
        for (int r=0;r<region_count;r++) {
            if (frame_idx - regions[r].last_seen_frame > STALE_FRAMES) { } else {
                if (write_i != r) regions[write_i] = regions[r];
                write_i++;
            }
        }
        region_count = write_i;

        if ((frame_idx % DFT_REPORT_INTERVAL) == 0 && frame_idx > 0) {
            printf("Frame %d: Regions=%d\n", frame_idx, region_count);
            for (int r=0;r<region_count;r++) {
                MyRegion *reg = &regions[r];
                int n = reg->hist_count;
                if (n == 0) { printf("  ID %d: no history\n", reg->id); continue; }
                double tmp[MAX_HISTORY];
                int start = (reg->hist_pos + MAX_HISTORY - reg->hist_count) % MAX_HISTORY;
                for (int i=0;i<n;i++) tmp[i] = reg->hist[(start + i) % MAX_HISTORY];
                double freq, power; dft_peak(tmp, n, fps, &freq, &power);
                printf("  ID %d bbox=(%d,%d,%dx%d) samples=%d -> freq=%.2fHz power=%.2f\n",
                       reg->id, reg->x, reg->y, reg->w, reg->h, n, freq, power);
            }
            fflush(stdout);
        }

        // X display of mask + boxes
        unsigned char *data = (unsigned char*)xim->data;
        for (int y=0;y<HEIGHT;y++) for (int x=0;x<WIDTH;x++) {
            int p = y*WIDTH + x; uint8_t v = mask[p]; int pos=(y*WIDTH + x)*4;
            data[pos+0]=v; data[pos+1]=v; data[pos+2]=v; data[pos+3]=0;
        }
        for (int r=0;r<region_count;r++) {
            MyRegion *reg = &regions[r];
            int x0=reg->x, y0=reg->y, x1=reg->x+reg->w-1, y1=reg->y+reg->h-1;
            if (x0<0) x0=0; if (y0<0) y0=0; if (x1>=WIDTH) x1=WIDTH-1; if (y1>=HEIGHT) y1=HEIGHT-1;
            for (int x=x0;x<=x1;x++) { int pos; pos=(y0*WIDTH + x)*4; data[pos+2]=255; data[pos+1]=0; data[pos+0]=0;
                                      pos=(y1*WIDTH + x)*4; data[pos+2]=255; data[pos+1]=0; data[pos+0]=0; }
            for (int y=y0;y<=y1;y++) { int pos; pos=(y*WIDTH + x0)*4; data[pos+2]=255; data[pos+1]=0; data[pos+0]=0;
                                      pos=(y*WIDTH + x1)*4; data[pos+2]=255; data[pos+1]=0; data[pos+0]=0; }
        }
        XPutImage(dpy, win, gc, xim, 0,0,0,0, WIDTH, HEIGHT);
        XFlush(dpy);

        if (ioctl(fd, VIDIOC_QBUF, &vbuf) < 0) { perror("VIDIOC_QBUF"); break; }

        while (XPending(dpy)) { XEvent ev; XNextEvent(dpy,&ev); if (ev.type==KeyPress) goto finish; }

        frame_idx++;
    }

finish:
    printf("Exiting...\n");
    if (xim) XDestroyImage(xim);
cleanup_x:
    if (dpy) { if (gc) XFreeGC(dpy,gc); if (win) XDestroyWindow(dpy,win); XCloseDisplay(dpy); }
cleanup_stream:
    if (fd >= 0) {
        int t = V4L2_BUF_TYPE_VIDEO_CAPTURE; ioctl(fd, VIDIOC_STREAMOFF, &t);
        for (unsigned int i=0;i<nbufs;i++) {
            if (bufs[i].start && bufs[i].length) munmap(bufs[i].start, bufs[i].length);
        }
        close(fd);
    }
    if (mask) free(mask);
    return 0;
}

