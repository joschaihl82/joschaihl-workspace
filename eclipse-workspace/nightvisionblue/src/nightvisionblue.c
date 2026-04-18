// nv_nightvision_blue.c
// Passive nightvision webcam with BLUE comparator, X11 display via XShm
// Compile: gcc -O2 nv_nightvision_blue.c -o nv_nightvision_blue -lX11 -lXext -lm
// Run: ./nv_nightvision_blue

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
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>

#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
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
    *R = (uint8_t)r; *G = (uint8_t)g; *B = (uint8_t)b;
}

/* Process a single pixel: apply boost/contrast/gamma and blue comparator mapping to ARGB */
static uint32_t process_pixel_rgb(uint8_t R, uint8_t Gc, uint8_t B) {
    /* brightness boost for low-light */
    float lum = 0.299f * R + 0.587f * Gc + 0.114f * B;
    float boost = 1.0f + (GAIN - 1.0f) * (1.0f - lum / 255.0f);
    float r = R * boost;
    float g = Gc * boost;
    float b = B * boost;

    /* contrast */
    r = ((r - 128.0f) * (1.0f + CONTRAST)) + 128.0f;
    g = ((g - 128.0f) * (1.0f + CONTRAST)) + 128.0f;
    b = ((b - 128.0f) * (1.0f + CONTRAST)) + 128.0f;

    /* gamma */
    if (GAMMA > 0.0f && fabsf(GAMMA - 1.0f) > 1e-6f) {
        r = 255.0f * powf(fmaxf(0.0f, r) / 255.0f, 1.0f / GAMMA);
        g = 255.0f * powf(fmaxf(0.0f, g) / 255.0f, 1.0f / GAMMA);
        b = 255.0f * powf(fmaxf(0.0f, b) / 255.0f, 1.0f / GAMMA);
    }

    uint8_t outR, outG, outB;
    if ((int)b > BLUE_THRESHOLD) {
        /* comparator hit: emphasize bluish/green tint */
        float intensity = (b - BLUE_THRESHOLD) / (255.0f - BLUE_THRESHOLD);
        if (intensity < 0.0f) intensity = 0.0f;
        if (intensity > 1.0f) intensity = 1.0f;
        outR = (uint8_t)fminf(255.0f, (0.2f * 255.0f * intensity) + (0.1f * r));
        outG = (uint8_t)fminf(255.0f, (0.9f * 255.0f * intensity) + (0.4f * g));
        outB = (uint8_t)fminf(255.0f, (0.6f * 255.0f * intensity) + (0.3f * b));
    } else {
        /* non-hit: dim and slightly blue-tinted background */
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

int main(void) {
    int fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        perror("open " DEVICE);
        return 1;
    }

    struct v4l2_capability capcap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &capcap) < 0) {
        perror("VIDIOC_QUERYCAP");
        close(fd);
        return 1;
    }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = PIXFORMAT;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

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

    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); close(fd); return 1; }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); close(fd); return 1; }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Cannot open X display\n"); goto vl_end; }
    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);

    XSetWindowAttributes attrs;
    attrs.background_pixel = BlackPixel(dpy, scr);
    attrs.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;
    Window win = XCreateWindow(dpy, root, 0, 0, WIDTH, HEIGHT, 0, DefaultDepth(dpy, scr),
                               CopyFromParent, DefaultVisual(dpy, scr), CWBackPixel | CWEventMask, &attrs);
    XStoreName(dpy, win, "Nightvision Blue Comparator");
    XMapWindow(dpy, win);

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(dpy, scr, 24, TrueColor, &vinfo)) {
        fprintf(stderr, "No 24-bit TrueColor visual, falling back to default visual\n");
    }
    Visual *vis = (vinfo.visual ? vinfo.visual : DefaultVisual(dpy, scr));

    XImage *ximg = NULL;
    XShmSegmentInfo shminfo;
    memset(&shminfo, 0, sizeof(shminfo));

    int img_width = WIDTH, img_height = HEIGHT;
    int bytes_per_pixel = 4;
    size_t img_size = img_width * img_height * bytes_per_pixel;
    shminfo.shmid = shmget(IPC_PRIVATE, img_size, IPC_CREAT | 0600);
    if (shminfo.shmid < 0) { perror("shmget"); goto x_end; }
    shminfo.shmaddr = (char *)shmat(shminfo.shmid, 0, 0);
    if (shminfo.shmaddr == (char *)-1) { perror("shmat"); goto x_end; }
    shminfo.readOnly = False;

    ximg = XShmCreateImage(dpy, vis, 24, ZPixmap, shminfo.shmaddr, &shminfo, img_width, img_height);
    if (!ximg) { fprintf(stderr, "XShmCreateImage failed\n"); goto x_end; }

    if (!XShmAttach(dpy, &shminfo)) { fprintf(stderr, "XShmAttach failed\n"); goto x_end; }
    XSync(dpy, False);

    fd_set fds;
    struct timeval tv;
    int maxfd = fd;
    uint32_t *convbuf = malloc(img_width * img_height * sizeof(uint32_t));
    if (!convbuf) { perror("malloc"); goto x_end; }

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
        yuyv_to_argb32_bluecmp(data, convbuf, img_width, img_height);

        memcpy(ximg->data, convbuf, img_size);
        XShmPutImage(dpy, win, DefaultGC(dpy, scr), ximg, 0, 0, 0, 0, img_width, img_height, False);
        XFlush(dpy);

        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_Escape) goto endloop;
            } else if (ev.type == DestroyNotify) {
                goto endloop;
            }
        }

        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); break; }
    }

endloop:
    free(convbuf);

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
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < req.count; ++i) {
        if (buffers[i].start && buffers[i].length) munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    close(fd);
    return 0;
}
