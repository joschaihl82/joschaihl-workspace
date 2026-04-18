#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_POINTS 100
#define DELAY_US 10000

typedef struct {
    int x, y;
} Point;

int main() {
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    Window root = DefaultRootWindow(dpy);
    XWindowAttributes wa;
    XGetWindowAttributes(dpy, root, &wa);

    GC gc = XCreateGC(dpy, root, 0, NULL);
    XColor color;
    Colormap cmap = DefaultColormap(dpy, 0);

    Point points[MAX_POINTS];
    int index = 0;

    while (1) {
        Window ret_root, ret_child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;

        XQueryPointer(dpy, root, &ret_root, &ret_child,
                      &root_x, &root_y, &win_x, &win_y, &mask);

        points[index] = (Point){root_x, root_y};
        index = (index + 1) % MAX_POINTS;

        XClearWindow(dpy, root);

        for (int i = 0; i < MAX_POINTS; i++) {
            int pos = (index + i) % MAX_POINTS;
            int age = MAX_POINTS - i;

            color.red = (age * 65535) / MAX_POINTS;
            color.green = ((MAX_POINTS - age) * 65535) / MAX_POINTS;
            color.blue = 30000;
            color.flags = DoRed | DoGreen | DoBlue;
            XAllocColor(dpy, cmap, &color);
            XSetForeground(dpy, gc, color.pixel);

            XDrawArc(dpy, root, gc, points[pos].x - 2, points[pos].y - 2, 4, 4, 0, 360 * 64);
        }

        XFlush(dpy);
        usleep(DELAY_US);
    }

    XFreeGC(dpy, gc);
    XCloseDisplay(dpy);
    return 0;
}

