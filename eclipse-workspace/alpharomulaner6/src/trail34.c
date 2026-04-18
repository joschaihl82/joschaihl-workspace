// maus_spuren.c
// Compile: gcc -O2 -o maus_spuren maus_spuren.c -lX11
// Run: ./maus_spuren

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <X11/cursorfont.h>

#define MAX_POINTS 10000
#define DOT_RADIUS 4
#define POLL_US 1000000

typedef struct { int x, y; int used; } Point;

static volatile int running = 1;
static void sigint_handler(int _) { running = 0; }

int main(void) {
    signal(SIGINT, sigint_handler);
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Kann X-Display nicht öffnen\n"); return 1; }

    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    Colormap cmap = DefaultColormap(dpy, screen);
    XWindowAttributes wa;
    XGetWindowAttributes(dpy, root, &wa);
    int width = wa.width, height = wa.height;

    GC gc = XCreateGC(dpy, root, 0, NULL);
    XSetLineAttributes(dpy, gc, 1, LineSolid, CapRound, JoinRound);

    // Pixmap für einmaliges Zeichnen auf Root (wird nicht gelöscht, Spuren persistent)
    Pixmap buffer = XCreatePixmap(dpy, root, width, height, DefaultDepth(dpy, screen));
    XSetForeground(dpy, gc, BlackPixel(dpy, screen));
    XFillRectangle(dpy, buffer, gc, 0, 0, width, height);

    // Vorallokierte Farbe (weiß) für Cursor-Icons; fallback falls Allocation fehlschlägt
    XColor xc; xc.red = xc.green = xc.blue = 65535; xc.flags = DoRed|DoGreen|DoBlue;
    if (!XAllocColor(dpy, cmap, &xc)) xc.pixel = WhitePixel(dpy, screen);

    XSetForeground(dpy, gc, xc.pixel);

    // Kreis-Pixmap als Cursor-Icon (optional, hier direkt als Arc gezeichnet)
    Point *points = calloc(MAX_POINTS, sizeof(Point));
    if (!points) { fprintf(stderr, "Speicherfehler\n"); return 1; }
    int head = 0;

    // Zeichne initialen Mauszeiger als normaler X-Cursor (optional)
    XDefineCursor(dpy, root, XCreateFontCursor(dpy, XC_left_ptr));

    while (running) {
        Window ret_root, ret_child;
        int rx, ry, wx, wy;
        unsigned int mask;
        if (!XQueryPointer(dpy, root, &ret_root, &ret_child, &rx, &ry, &wx, &wy, &mask)) {
            usleep(POLL_US);
            continue;
        }

        // Speichere Position in Ringpuffer; markiere benutzt
        points[head].x = rx;
        points[head].y = ry;
        points[head].used = 1;
        head = (head + 1) % MAX_POINTS;

        // Zeichne neuen "Cursor" auf Buffer und kopiere auf Root
        int x = rx - DOT_RADIUS;
        int y = ry - DOT_RADIUS;
        XFillArc(dpy, buffer, gc, x, y, DOT_RADIUS*2, DOT_RADIUS*2, 0, 360*64);

        // Kopiere nur die Region des neuen Punktes (schneller als ganze Fläche)
        XCopyArea(dpy, buffer, root, gc, x, y, DOT_RADIUS*2+1, DOT_RADIUS*2+1, x, y);
        XFlush(dpy);

        usleep(POLL_US);
    }

    // Aufräumen
    XFreePixmap(dpy, buffer);
    XFreeGC(dpy, gc);
    free(points);
    XCloseDisplay(dpy);
    return 0;
}

