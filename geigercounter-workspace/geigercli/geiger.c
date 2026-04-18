// geiger_cli.c
// Command-line "Geiger" from webcam noise using V4L2 (pure C)

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#ifndef DEVICE_NODE
#define DEVICE_NODE "/dev/video0"
#endif
#ifndef IMG_W
#define IMG_W 640
#endif
#ifndef IMG_H
#define IMG_H 480
#endif
#ifndef Y_HOT_THRESH
#define Y_HOT_THRESH 200
#endif

// Capture settings
#define REQ_BUFFERS 4
#define PRINT_INTERVAL_MS 250

struct Buffer {
    void   *start;
    size_t  length;
};

static int running = 1;
static void on_sigint(int sig) { (void)sig; running = 0; }

static int xioctl(int fd, int request, void *arg) {
    int r;
    do r = ioctl(fd, request, arg);
    while (r == -1 && errno == EINTR);
    return r;
}

// YUYV: [Y0 U Y1 V] per 2 pixels.
// Return luma Y for pixel index x in the 2-pixel pair.
static inline uint8_t yuyv_get_Y(const uint8_t *p, int pairPixelIndex /*0 or 1*/) {
    return p[pairPixelIndex == 0 ? 0 : 2];
}

int main(int argc, char **argv) {
    const char *devnode = DEVICE_NODE;
    if (argc > 1) devnode = argv[1];

    // Install SIGINT handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;
    sigaction(SIGINT, &sa, NULL);

    int fd = open(devnode, O_RDWR | O_NONBLOCK, 0);
    if (fd == -1) {
        perror("open video device");
        return 1;
    }

    // Negotiate format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W;
    fmt.fmt.pix.height = IMG_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }
    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        fprintf(stderr, "Device does not support YUYV\n");
        close(fd);
        return 1;
    }
    if (fmt.fmt.pix.width != IMG_W || fmt.fmt.pix.height != IMG_H) {
        fprintf(stderr, "Device adjusted size to %ux%u\n",
                fmt.fmt.pix.width, fmt.fmt.pix.height);
    }

    // Request buffers
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = REQ_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("VIDIOC_REQBUFS");
        close(fd);
        return 1;
    }
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on device\n");
        close(fd);
        return 1;
    }

    struct Buffer *buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        close(fd);
        return 1;
    }

    for (unsigned i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
            perror("VIDIOC_QUERYBUF");
            free(buffers);
            close(fd);
            return 1;
        }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            free(buffers);
            close(fd);
            return 1;
        }
    }

    // Queue buffers
    for (unsigned i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF");
            for (unsigned j = 0; j < req.count; ++j)
                if (buffers[j].start && buffers[j].start != MAP_FAILED)
                    munmap(buffers[j].start, buffers[j].length);
            free(buffers);
            close(fd);
            return 1;
        }
    }

    // Start streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        perror("VIDIOC_STREAMON");
        for (unsigned j = 0; j < req.count; ++j)
            if (buffers[j].start && buffers[j].start != MAP_FAILED)
                munmap(buffers[j].start, buffers[j].length);
        free(buffers);
        close(fd);
        return 1;
    }

    // Analysis state
    double currentRadSmooth = 0.0;
    double totalDoseAcc = 0.0;

    // Timing
    struct timespec lastPrint = {0,0};
    clock_gettime(CLOCK_MONOTONIC, &lastPrint);

    fprintf(stderr, "Capturing from %s (YUYV %dx%d). Press Ctrl+C to stop.\n", devnode, IMG_W, IMG_H);

    while (running) {
        // Wait for frame
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        int r = select(fd+1, &fds, NULL, NULL, &tv);
        if (r == -1) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        } else if (r == 0) {
            fprintf(stderr, "select timeout\n");
            continue;
        }

        // Dequeue
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (xioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            if (errno == EAGAIN) continue;
            perror("VIDIOC_DQBUF");
            break;
        }

        // Analyze luma (Y) — sample every 4th pixel in both axes
        const uint8_t *frame = (const uint8_t*)buffers[buf.index].start;
        // Each line has IMG_W pixels, YUYV uses 2 bytes per pixel => IMG_W*2 bytes per line
        int hotPixels = 0;
        for (int y = 0; y < IMG_H; y += 4) {
            const uint8_t *line = frame + y * IMG_W * 2;
            // Step by 4 pixels => 2 YUYV pairs per step
            for (int x = 0; x < IMG_W; x += 4) {
                int pairOffset = (x / 2) * 4; // 4 bytes per 2 pixels
                const uint8_t *p = line + pairOffset;
                uint8_t Y0 = yuyv_get_Y(p, 0);
                uint8_t Y1 = yuyv_get_Y(p, 1);
                if (Y0 > Y_HOT_THRESH) hotPixels++;
                if (Y1 > Y_HOT_THRESH) hotPixels++;
            }
        }

        // Normalize and filter like original
        double rawVal = (double)hotPixels / 100.0;
        currentRadSmooth = (currentRadSmooth * 0.7) + (rawVal * 0.3);
        totalDoseAcc += currentRadSmooth * 0.001;
        int alertState = currentRadSmooth > 5.0;

        // Periodic print
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        long dt_ms = (now.tv_sec - lastPrint.tv_sec) * 1000L +
                     (now.tv_nsec - lastPrint.tv_nsec) / 1000000L;
        if (dt_ms >= PRINT_INTERVAL_MS) {
            lastPrint = now;
            printf("RAD: %.2f µSv/h | DOSE: %.4f µSv%s\n",
                   currentRadSmooth, totalDoseAcc,
                   alertState ? " | ALERT" : "");
            fflush(stdout);
        }

        // Re-queue buffer
        if (xioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF");
            break;
        }
    }

    // Stop streaming
    if (xioctl(fd, VIDIOC_STREAMOFF, &type) == -1) {
        perror("VIDIOC_STREAMOFF");
    }

    // Cleanup
    for (unsigned j = 0; j < req.count; ++j)
        if (buffers[j].start && buffers[j].start != MAP_FAILED)
            munmap(buffers[j].start, buffers[j].length);
    free(buffers);
    close(fd);

    fprintf(stderr, "Stopped.\n");
    return 0;
}

