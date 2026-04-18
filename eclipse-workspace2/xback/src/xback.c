/*
 * xback.c
 *
 * Paint a continuously changing rainbow onto the XFCE desktop window if drawable,
 * otherwise fall back to the root window. Installs an X error handler to recover
 * from BadMatch / other X errors by switching to the root window.
 *
 * Compile: gcc -O2 -o xback xback.c -lX11 -lm
 *
 * Run inside an X session (XFCE). Stop with Ctrl-C.
 */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>

static volatile int running = 1;
static Display *dpy = NULL;
static int saw_x_error = 0;

/* Simple X error handler: mark that an X error occurred and return */
static int x_error_handler(Display *d, XErrorEvent *ev) {
    (void)d;
    (void)ev;
    saw_x_error = 1;
    return 0;
}

void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

/* Convert HSV (h in [0,360), s,v in [0,1]) to RGB [0..1] */
static void hsv_to_rgb(double h, double s, double v, double *r, double *g, double *b) {
    double c = v * s;
    double hh = h / 60.0;
    double x = c * (1 - fabs(fmod(hh, 2.0) - 1));
    double m = v - c;
    double rr=0, gg=0, bb=0;
    if (hh >= 0 && hh < 1) { rr = c; gg = x; bb = 0; }
    else if (hh < 2) { rr = x; gg = c; bb = 0; }
    else if (hh < 3) { rr = 0; gg = c; bb = x; }
    else if (hh < 4) { rr = 0; gg = x; bb = c; }
    else if (hh < 5) { rr = x; gg = 0; bb = c; }
    else { rr = c; gg = 0; bb = x; }
    *r = rr + m; *g = gg + m; *b = bb + m;
}

/* Helper: get atom property of type CARDINAL (array of atoms/ints) */
static long get_window_cardinal_property(Display *d, Window w, Atom prop, Atom req_type, Atom *out, long max_items) {
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_ret = NULL;
    int status = XGetWindowProperty(d, w, prop, 0, max_items, False, req_type,
                                    &actual_type, &actual_format, &nitems, &bytes_after, &prop_ret);
    if (status != Success || actual_type == None || prop_ret == NULL) {
        if (prop_ret) XFree(prop_ret);
        return 0;
    }
    if (actual_format == 32) {
        unsigned long *vals = (unsigned long*)prop_ret;
        long i;
        for (i = 0; i < (long)nitems && i < max_items; ++i) out[i] = (Atom)vals[i];
        XFree(prop_ret);
        return (long)nitems;
    }
    XFree(prop_ret);
    return 0;
}

/* Search children for a window with _NET_WM_WINDOW_TYPE_DESKTOP or WM_CLASS "xfdesktop" */
static Window find_xfdesktop_window(Display *d, Window root) {
    Atom net_wm_window_type = XInternAtom(d, "_NET_WM_WINDOW_TYPE", False);
    Atom net_wm_window_type_desktop = XInternAtom(d, "_NET_WM_WINDOW_TYPE_DESKTOP", False);

    Window root_return, parent_return;
    Window *children = NULL;
    unsigned int nchildren = 0;
    if (!XQueryTree(d, root, &root_return, &parent_return, &children, &nchildren)) {
        return 0;
    }

    for (unsigned int i = 0; i < nchildren; ++i) {
        Window w = children[i];

        /* Check _NET_WM_WINDOW_TYPE */
        Atom types[8];
        long n = get_window_cardinal_property(d, w, net_wm_window_type, XA_ATOM, types, 8);
        for (long j = 0; j < n; ++j) {
            if (types[j] == net_wm_window_type_desktop) {
                if (children) XFree(children);
                return w;
            }
        }

        /* Check WM_CLASS string for "xfdesktop" */
        XClassHint ch;
        if (XGetClassHint(d, w, &ch)) {
            if (ch.res_class && ch.res_name) {
                if (strstr(ch.res_class, "Xfdesktop") || strstr(ch.res_class, "xfdesktop") ||
                    strstr(ch.res_name, "Xfdesktop") || strstr(ch.res_name, "xfdesktop")) {
                    if (ch.res_name) XFree(ch.res_name);
                    if (ch.res_class) XFree(ch.res_class);
                    if (children) XFree(children);
                    return w;
                }
            }
            if (ch.res_name) XFree(ch.res_name);
            if (ch.res_class) XFree(ch.res_class);
        }
    }

    if (children) XFree(children);
    return 0;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    /* Install X error handler early so we can recover from BadMatch etc. */
    XSetErrorHandler(x_error_handler);

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);

    Window target = find_xfdesktop_window(dpy, root);
    if (target == 0) {
        fprintf(stderr, "XFCE desktop window not found; using root window\n");
        target = root;
    } else {
        fprintf(stderr, "Found XFCE desktop window: 0x%lx\n", (unsigned long)target);
    }

    /* Query attributes and ensure the target is drawable (InputOutput). If not, fall back to root. */
    XWindowAttributes attr;
    if (!XGetWindowAttributes(dpy, target, &attr)) {
        fprintf(stderr, "Failed to get window attributes; using root\n");
        target = root;
        if (!XGetWindowAttributes(dpy, target, &attr)) {
            fprintf(stderr, "Failed to get root window attributes\n");
            XCloseDisplay(dpy);
            return 1;
        }
    }

    if (attr.class != InputOutput) {
        fprintf(stderr, "Target window is not drawable (class=%d); falling back to root\n", attr.class);
        target = root;
        if (!XGetWindowAttributes(dpy, target, &attr)) {
            fprintf(stderr, "Failed to get root window attributes\n");
            XCloseDisplay(dpy);
            return 1;
        }
    }

    /* Create GC on the chosen drawable. If an X error occurs during GC creation,
       the error handler will set saw_x_error and we will fall back to root. */
    saw_x_error = 0;
    GC gc = XCreateGC(dpy, target, 0, NULL);
    XSync(dpy, False);
    if (saw_x_error || gc == NULL) {
        /* Try root as a robust fallback */
        fprintf(stderr, "XCreateGC failed on target; switching to root\n");
        target = root;
        if (!XGetWindowAttributes(dpy, target, &attr)) {
            fprintf(stderr, "Failed to get root window attributes\n");
            XCloseDisplay(dpy);
            return 1;
        }
        saw_x_error = 0;
        gc = XCreateGC(dpy, target, 0, NULL);
        XSync(dpy, False);
        if (saw_x_error || gc == NULL) {
            fprintf(stderr, "XCreateGC failed on root as well; aborting\n");
            XCloseDisplay(dpy);
            return 1;
        }
    }

    /* Use the colormap from the target's attributes so XAllocColor uses the correct colormap/visual */
    Colormap cmap = attr.colormap ? attr.colormap : DefaultColormap(dpy, screen);

    int width = attr.width;
    int height = attr.height;

    double hue = 0.0;
    const double hue_step = 1.0; /* degrees per frame */
    const int ms_sleep = 30; /* ~33 FPS */

    while (running) {
        /* Re-query attributes in case the window changed (size, class, colormap, etc.) */
        if (!XGetWindowAttributes(dpy, target, &attr)) break;
        width = attr.width; height = attr.height;

        /* If the window became non-drawable, switch to root and recreate GC */
        if (attr.class != InputOutput) {
            target = root;
            if (!XGetWindowAttributes(dpy, target, &attr)) break;
            width = attr.width; height = attr.height;
            cmap = attr.colormap ? attr.colormap : DefaultColormap(dpy, screen);

            XFreeGC(dpy, gc);
            saw_x_error = 0;
            gc = XCreateGC(dpy, target, 0, NULL);
            XSync(dpy, False);
            if (saw_x_error || gc == NULL) break;
        }

        double r, g, b;
        hsv_to_rgb(hue, 1.0, 1.0, &r, &g, &b);
        unsigned short rr = (unsigned short)(r * 65535.0);
        unsigned short gg = (unsigned short)(g * 65535.0);
        unsigned short bb = (unsigned short)(b * 65535.0);

        XColor color;
        color.red = rr;
        color.green = gg;
        color.blue = bb;
        color.flags = DoRed | DoGreen | DoBlue;

        /* Try to allocate color in the target's colormap; if that fails, fall back to 24-bit pixel */
        if (XAllocColor(dpy, cmap, &color)) {
            XSetForeground(dpy, gc, color.pixel);
        } else {
            unsigned long pixel = ((unsigned long)((int)(r*255)) << 16) |
                                  ((unsigned long)((int)(g*255)) << 8) |
                                  ((unsigned long)((int)(b*255)));
            XSetForeground(dpy, gc, pixel);
        }

        /* Fill the entire target window */
        XFillRectangle(dpy, target, gc, 0, 0, (unsigned int)width, (unsigned int)height);
        XFlush(dpy);

        hue += hue_step;
        if (hue >= 360.0) hue -= 360.0;

        usleep(ms_sleep * 1000);
    }

    if (gc) XFreeGC(dpy, gc);
    XCloseDisplay(dpy);
    return 0;
}
