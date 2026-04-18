// crossfade_mouse_10000.c
// Compile: gcc -O2 -o crossfade_mouse_10000 crossfade_mouse_10000.c -lX11
// Run: ./crossfade_mouse_10000

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#define MAX_POINTS 10000
#define DELAY_US 8000           // ~125 FPS, adjust as needed
#define DOT_RADIUS 3           // radius of each dot in pixels
#define COLOR_STEPS MAX_POINTS // number of colors to precompute

typedef struct {
    int x, y;
    int valid;
} Point;

static unsigned long *precolors = NULL;

int main(void) {
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    Visual *visual = DefaultVisual(dpy, screen);
    Colormap cmap = DefaultColormap(dpy, screen);

    XWindowAttributes wa;
    XGetWindowAttributes(dpy, root, &wa);
    int width = wa.width;
    int height = wa.height;

    GC gc = XCreateGC(dpy, root, 0, NULL);
    XSetLineAttributes(dpy, gc, 1, LineSolid, CapRound, JoinRound);

    // Offscreen pixmap for double buffering
    Pixmap buffer = XCreatePixmap(dpy, root, width, height, DefaultDepth(dpy, screen));

    // Precompute colors (gradient across hue-like spectrum)
    precolors = calloc(COLOR_STEPS, sizeof(unsigned long));
    if (!precolors) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }

    // We'll create an XImage-like color by allocating XColor entries once.
    // Build a simple RGB gradient: hue-like sweep by mapping position to RGB.
    for (int i = 0; i < COLOR_STEPS; ++i) {
        double t = (double)i / (COLOR_STEPS - 1); // 0..1 oldest->newest
        // Map t to RGB across spectrum: use smooth polynomial ramps
        double r = 0.5 + 0.5 * sin(2.0 * M_PI * (t + 0.0));
        double g = 0.5 + 0.5 * sin(2.0 * M_PI * (t + 0.33));
        double b = 0.5 + 0.5 * sin(2.0 * M_PI * (t + 0.66));

        XColor xc;
        xc.red   = (unsigned short)(r * 65535.0);
        xc.green = (unsigned short)(g * 65535.0);
        xc.blue  = (unsigned short)(b * 65535.0);
        xc.flags = DoRed | DoGreen | DoBlue;

        // Try to allocate color; if fails, fallback to a best-fit using XAllocColor
        if (!XAllocColor(dpy, cmap, &xc)) {
            xc.pixel = (xc.red >> 8) << 16 | (xc.green >> 8) << 8 | (xc.blue >> 8);
        }
        precolors[i] = xc.pixel;
    }

    // Circular buffer for points
    Point *points = calloc(MAX_POINTS, sizeof(Point));
    if (!points) {
        fprintf(stderr, "Point buffer allocation failed\n");
        return 1;
    }
    int head = 0;
    int count = 0;

    // Make buffer cleared initially
    XSetForeground(dpy, gc, BlackPixel(dpy, screen));
    XFillRectangle(dpy, buffer, gc, 0, 0, width, height);

    // For performance, precompute a small pixmap representing the dot (filled circle)
    int dot_d = DOT_RADIUS * 2 + 1;
    Pixmap dot_pix = XCreatePixmap(dpy, root, dot_d, dot_d, DefaultDepth(dpy, screen));
    GC dot_gc = XCreateGC(dpy, dot_pix, 0, NULL);
    XSetForeground(dpy, dot_gc, 0); // will be set per-blit using XSetForeground on main gc
    XFillRectangle(dpy, dot_pix, dot_gc, 0, 0, dot_d, dot_d);
    XFreeGC(dpy, dot_gc);

    // Main loop
    while (1) {
        // Query pointer once per loop
        Window ret_root, ret_child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        if (!XQueryPointer(dpy, root, &ret_root, &ret_child, &root_x, &root_y, &win_x, &win_y, &mask)) {
            // pointer not on screen or query failed; sleep and continue
            usleep(DELAY_US);
            continue;
        }

        // Add point to circular buffer
        points[head].x = root_x;
        points[head].y = root_y;
        points[head].valid = 1;
        head = (head + 1) % MAX_POINTS;
        if (count < MAX_POINTS) count++;

        // Clear buffer pixmap
        XSetForeground(dpy, gc, BlackPixel(dpy, screen));
        XFillRectangle(dpy, buffer, gc, 0, 0, width, height);

        // Draw trail from oldest to newest
        int idx = (head - count + MAX_POINTS) % MAX_POINTS;
        for (int i = 0; i < count; ++i) {
            Point *p = &points[idx];
            if (p->valid) {
                // Map age i (0 oldest) to color index (0 oldest -> COLOR_STEPS-1 newest)
                int color_idx = (int)((double)i / (count - 1 > 0 ? count - 1 : 1) * (COLOR_STEPS - 1));
                unsigned long pixel = precolors[color_idx];
                XSetForeground(dpy, gc, pixel);

                int x = p->x - DOT_RADIUS;
                int y = p->y - DOT_RADIUS;
                XFillArc(dpy, buffer, gc, x, y, DOT_RADIUS * 2, DOT_RADIUS * 2, 0, 360 * 64);
            }
            idx = (idx + 1) % MAX_POINTS;
        }

        // Copy buffer to root (fast XCopyArea)
        XCopyArea(dpy, buffer, root, gc, 0, 0, width, height, 0, 0);
        XFlush(dpy);

        // Sleep to control framerate
        usleep(DELAY_US);
    }

    // Cleanup (unreachable in this simple loop)
    XFreePixmap(dpy, buffer);
    XFreePixmap(dpy, dot_pix);
    XFreeGC(dpy, gc);
    free(points);
    free(precolors);
    XCloseDisplay(dpy);
    return 0;
}

