// nv_nightvision_blue_resizable.c
// Passive nightvision webcam with BLUE comparator, V4L2 capture + X11 XShm display,
// window is resizable / maximizable; image is scaled (nearest neighbor) to the window.
// Compile: gcc -O2 nv_nightvision_blue_resizable.c -o nv_nightvision_blue_resizable -lX11 -lXext -lm
// Run: ./nv_nightvision_blue_resizable
// Press Esc or close the window to exit.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <math.h>
#include <time.h>

#define DEVICE "/dev/video0"
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define PIXFORMAT V4L2_PIX_FMT_YUYV

/* Tunable parameters */
static const int BLUE_THRESHOLD = 120;   /* 0..255 threshold for blue comparator */
static const float GAIN = 2.0f;          /* overall gain for low-light boost */
static const float GAMMA = 1.0f;         /* gamma correction (1.0 = none) */
static const float CONTRAST = 0.10f;     /* contrast factor (0.10 => +10%) */

/* V4L2 mmap buffer structure */
struct buffer {
    void *start;
    size_t length;
};

static int xioctl(int fd, int request, void *arg) {
    int r;
    do { r = ioctl(fd, request, arg); } while (r == -1 && errno == EINTR);
    return r;
}

/* Convert one YUV (Y, U, V) to RGB 0..255 */
static void yuv_to_rgb_pixel(int Y, int U, int V, uint8_t *R, uint8_t *G, uint8_t *B) {
    int C = Y - 16;
    int D = U - 128;
    int E = V - 128;
    int r = (298 * C + 409 * E + 128) >> 8;
    int g = (298 * C - 100 * D - 208 * E + 128) >> 8;
    int b = (298 * C + 516 * D + 128) >> 8;
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    *R = (uint8_t)r;
    *G = (uint8_t)g;
    *B = (uint8_t)b;
}

/* Process a single pixel: apply boost/contrast/gamma and blue comparator mapping to ARGB */
static uint32_t process_pixel_rgb(uint8_t R, uint8_t Gc, uint8_t B) {
    float lum = 0.299f * R + 0.587f * Gc + 0.114f * B;
    float boost = 1.0f + (GAIN - 1.0f) * (1.0f - lum / 255.0f);
    float r = R * boost;
    float g = Gc * boost;
    float b = B * boost;

    r = ((r - 128.0f) * (1.0f + CONTRAST)) + 128.0f;
    g = ((g - 128.0f) * (1.0f + CONTRAST)) + 128.0f;
    b = ((b - 128.0f) * (1.0f + CONTRAST)) + 128.0f;

    if (GAMMA > 0.0f && fabsf(GAMMA - 1.0f) > 1e-6f) {
        r = 255.0f * powf(fmaxf(0.0f, r) / 255.0f, 1.0f / GAMMA);
        g = 255.0f * powf(fmaxf(0.0f, g) / 255.0f, 1.0f / GAMMA);
        b = 255.0f * powf(fmaxf(0.0f, b) / 255.0f, 1.0f / GAMMA);
    }

    uint8_t outR, outG, outB;
    if ((int)b > BLUE_THRESHOLD) {
        float intensity = (b - BLUE_THRESHOLD) / (255.0f - BLUE_THRESHOLD);
        if (intensity < 0.0f) intensity = 0.0f;
        if (intensity > 1.0f) intensity = 1.0f;
        outR = (uint8_t)fminf(255.0f, (0.2f * 255.0f * intensity) + (0.1f * r));
        outG = (uint8_t)fminf(255.0f, (0.9f * 255.0f * intensity) + (0.4f * g));
        outB = (uint8_t)fminf(255.0f, (0.6f * 255.0f * intensity) + (0.3f * b));
    } else {
        outR = (uint8_t)fmaxf(0.0f, r * 0.25f);
        outG = (uint8_t)fmaxf(0.0f, g * 0.5f);
        outB = (uint8_t)fmaxf(0.0f, b * 0.6f);
    }

    uint32_t a = 0xFFu;
    uint32_t pix = (a << 24) | ((uint32_t)outR << 16) | ((uint32_t)outG << 8) | (uint32_t)outB;
    return pix;
}

/* Convert YUYV frame to ARGB32 applying blue comparator */
static void yuyv_to_argb32_bluecmp(const uint8_t *yuyv, uint32_t *out, int w, int h) {
    int idx = 0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; x += 2) {
            uint8_t Y0 = yuyv[idx++];
            uint8_t U  = yuyv[idx++];
            uint8_t Y1 = yuyv[idx++];
            uint8_t V  = yuyv[idx++];

            uint8_t R0, G0, B0, R1, G1, B1;
            yuv_to_rgb_pixel(Y0, U, V, &R0, &G0, &B0);
            yuv_to_rgb_pixel(Y1, U, V, &R1, &G1, &B1);

            *out++ = process_pixel_rgb(R0, G0, B0);
            *out++ = process_pixel_rgb(R1, G1, B1);
        }
    }
}

/* Helper: create or recreate XShm XImage sized w x h; previous image/shm (if any) detached and freed */
static int create_shm_image(Display *dpy, Visual *vis, int depth,
                            XShmSegmentInfo *shminfo, XImage **ximg,
                            int w, int h) {
    if (*ximg) {
        XShmDetach(dpy, shminfo);
        XDestroyImage(*ximg);
        if (shminfo->shmaddr && shminfo->shmid >= 0) {
            shmdt(shminfo->shmaddr);
            shmctl(shminfo->shmid, IPC_RMID, 0);
        }
        memset(shminfo, 0, sizeof(*shminfo));
        *ximg = NULL;
    }

    int bytes_per_pixel = 4;
    size_t img_size = (size_t)w * (size_t)h * bytes_per_pixel;
    shminfo->shmid = shmget(IPC_PRIVATE, img_size, IPC_CREAT | 0600);
    if (shminfo->shmid < 0) { perror("shmget"); return -1; }
    shminfo->shmaddr = (char *)shmat(shminfo->shmid, 0, 0);
    if (shminfo->shmaddr == (char *)-1) { perror("shmat"); shmctl(shminfo->shmid, IPC_RMID, 0); return -1; }
    shminfo->readOnly = False;

    *ximg = XShmCreateImage(dpy, vis, depth, ZPixmap, shminfo->shmaddr, shminfo, w, h);
    if (!*ximg) { fprintf(stderr, "XShmCreateImage failed\n"); shmdt(shminfo->shmaddr); shmctl(shminfo->shmid, IPC_RMID, 0); return -1; }

    if (!XShmAttach(dpy, shminfo)) { fprintf(stderr, "XShmAttach failed\n"); XDestroyImage(*ximg); shmdt(shminfo->shmaddr); shmctl(shminfo->shmid, IPC_RMID, 0); *ximg = NULL; return -1; }
    XSync(dpy, False);
    return 0;
}

/* Nearest-neighbor scale source ARGB (sw x sh) into dest (dw x dh) */
static void scale_nearest_argb32(const uint32_t *src, int sw, int sh, uint32_t *dst, int dw, int dh) {
    if (sw == dw && sh == dh) {
        memcpy(dst, src, (size_t)sw * sh * sizeof(uint32_t));
        return;
    }
    for (int y = 0; y < dh; ++y) {
        int sy = (int)((long long)y * sh / dh);
        if (sy >= sh) sy = sh - 1;
        for (int x = 0; x < dw; ++x) {
            int sx = (int)((long long)x * sw / dw);
            if (sx >= sw) sx = sw - 1;
            dst[y * dw + x] = src[sy * sw + sx];
        }
    }
}

int main(void) {
    int fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        perror("open " DEVICE);
        return 1;
    }

    /* Query capabilities */
    struct v4l2_capability capcap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &capcap) < 0) {
        perror("VIDIOC_QUERYCAP");
        close(fd);
        return 1;
    }

    /* Set desired format (driver may adjust) */
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = DEFAULT_WIDTH;
    fmt.fmt.pix.height = DEFAULT_HEIGHT;
    fmt.fmt.pix.pixelformat = PIXFORMAT;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

    /* Actual camera frame size chosen by driver */
    int cam_w = (int)fmt.fmt.pix.width;
    int cam_h = (int)fmt.fmt.pix.height;

    /* Request mmap buffers */
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        close(fd);
        return 1;
    }
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory\n");
        close(fd);
        return 1;
    }

    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) { perror("calloc"); close(fd); return 1; }
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); close(fd); return 1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); close(fd); return 1; }
    }

    /* Queue buffers */
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); close(fd); return 1; }
    }

    /* Start streaming */
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); close(fd); return 1; }

    /* Open X display and create a resizable window (initially same as camera) */
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Cannot open X display\n"); goto vl_end; }
    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);

    XSetWindowAttributes attrs;
    attrs.background_pixel = BlackPixel(dpy, scr);
    attrs.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    XVisualInfo vinfo;
    Visual *vis = NULL;
    int depth = DefaultDepth(dpy, scr);
    if (XMatchVisualInfo(dpy, scr, 24, TrueColor, &vinfo)) {
        vis = vinfo.visual;
        depth = vinfo.depth;
    } else {
        vis = DefaultVisual(dpy, scr);
        depth = DefaultDepth(dpy, scr);
    }

    /* Create resizable window (no strict size hints) */
    Window win = XCreateWindow(dpy, root,
                               0, 0, (unsigned int)cam_w, (unsigned int)cam_h, 0,
                               depth,                 /* depth */
                               InputOutput,           /* class */
                               vis,                   /* visual */
                               CWBackPixel | CWEventMask, /* valuemask */
                               &attrs);               /* attributes */
    XStoreName(dpy, win, "Nightvision Blue Comparator (Resizable)");
    XMapWindow(dpy, win);

    /* Prepare XShm image; we create an XImage matching the current window size and recreate on resize */
    XShmSegmentInfo shminfo;
    memset(&shminfo, 0, sizeof(shminfo));
    XImage *ximg = NULL;
    int win_w = cam_w, win_h = cam_h;
    if (create_shm_image(dpy, vis, depth, &shminfo, &ximg, win_w, win_h) < 0) goto x_end;

    /* Buffers */
    uint32_t *convbuf = malloc((size_t)cam_w * (size_t)cam_h * sizeof(uint32_t));
    if (!convbuf) { perror("malloc"); goto x_end; }
    /* temporary buffer for scaled image (matches XImage size) */
    uint32_t *scaledbuf = malloc((size_t)win_w * (size_t)win_h * sizeof(uint32_t));
    if (!scaledbuf) { perror("malloc"); goto x_end; }

    /* Main capture/display loop */
    fd_set fds;
    struct timeval tv;
    int maxfd = fd;
    while (1) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        int r = select(maxfd + 1, &fds, NULL, NULL, &tv);
        if (r < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        } else if (r == 0) {
            continue;
        }
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            if (errno == EAGAIN) continue;
            perror("VIDIOC_DQBUF");
            break;
        }

        const uint8_t *data = (const uint8_t *)buffers[buf.index].start;
        yuyv_to_argb32_bluecmp(data, convbuf, cam_w, cam_h);

        /* Check for window resize events (non-blocking peek) */
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == ConfigureNotify) {
                XConfigureEvent ce = ev.xconfigure;
                if (ce.width != win_w || ce.height != win_h) {
                    /* recreate XImage and scaled buffer to new size */
                    win_w = ce.width > 1 ? ce.width : 1;
                    win_h = ce.height > 1 ? ce.height : 1;
                    if (create_shm_image(dpy, vis, depth, &shminfo, &ximg, win_w, win_h) < 0) {
                        fprintf(stderr, "Failed to recreate shm image on resize\n");
                        goto endloop;
                    }
                    free(scaledbuf);
                    scaledbuf = malloc((size_t)win_w * (size_t)win_h * sizeof(uint32_t));
                    if (!scaledbuf) { perror("malloc"); goto endloop; }
                }
            } else if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_Escape) goto endloop;
            } else if (ev.type == DestroyNotify) {
                goto endloop;
            }
        }

        /* Scale convbuf (camera resolution) into scaledbuf (window resolution) */
        scale_nearest_argb32(convbuf, cam_w, cam_h, scaledbuf, win_w, win_h);

        /* Copy scaledbuf into X shared memory image and display */
        memcpy(ximg->data, scaledbuf, (size_t)win_w * (size_t)win_h * 4);
        XShmPutImage(dpy, win, DefaultGC(dpy, scr), ximg,
                     0, 0, 0, 0, win_w, win_h, False);
        XFlush(dpy);

        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); break; }
    }

endloop:
    free(convbuf);
    free(scaledbuf);

x_end:
    if (ximg) {
        XShmDetach(dpy, &shminfo);
        XDestroyImage(ximg);
    }
    if (shminfo.shmaddr && shminfo.shmid >= 0) {
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);
    }
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

vl_end:
    /* Stop streaming and cleanup V4L2 buffers */
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < req.count; ++i) {
        if (buffers[i].start && buffers[i].length) munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    close(fd);
    return 0;
}
