// overlay_amalgam.c
// Compile: gcc -O2 -o overlay_amalgam overlay_amalgam.c -lX11 -lXext -lXrender -lm
// Run: ./overlay_amalgam
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xext.h>
#include "X11/extensions/Xrender.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static volatile int running = 1;
static void handle_sigint(int _) { (void)_; running = 0; }

/* Simple Voss-like pink noise generator (integer version) */
typedef struct { int rows; uint32_t *values; unsigned counter; } PinkGen;
static PinkGen *pink_new(int rows) {
    PinkGen *p = calloc(1, sizeof(PinkGen));
    if (!p) return NULL;
    p->rows = rows;
    p->values = calloc(rows, sizeof(uint32_t));
    if (!p->values) { free(p); return NULL; }
    p->counter = 0;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (int i = 0; i < rows; ++i) p->values[i] = rand() & 0xFFFF;
    return p;
}
static void pink_free(PinkGen *p) { if (!p) return; free(p->values); free(p); }
static uint8_t pink_sample(PinkGen *p) {
    p->counter++;
    unsigned c = p->counter;
    for (int i = 0; i < p->rows; ++i) {
        if ((c & ((1u << i) - 1u)) == 0u) p->values[i] = rand() & 0xFFFF;
    }
    uint32_t sum = 0;
    for (int i = 0; i < p->rows; ++i) sum += p->values[i];
    return (uint8_t)((sum / (p->rows + 1)) & 0xFF);
}

/* Find a 32-bit ARGB visual */
static int find_argb_visual(Display *dpy, XVisualInfo *out) {
    XVisualInfo tmpl; tmpl.screen = DefaultScreen(dpy);
    int n = 0;
    XVisualInfo *vis = XGetVisualInfo(dpy, VisualScreenMask, &tmpl, &n);
    if (!vis || n <= 0) { if (vis) XFree(vis); return 0; }
    for (int i = 0; i < n; ++i) {
        if (vis[i].depth == 32 &&
           (vis[i].visual->class == TrueColor || vis[i].visual->class == DirectColor)) {
            *out = vis[i];
            XFree(vis);
            return 1;
        }
    }
    XFree(vis);
    return 0;
}

/* Compute root coordinates of a window by walking parents (best-effort) */
static int get_window_root_geometry(Display *dpy, Window w, int *out_x, int *out_y, unsigned *out_w, unsigned *out_h) {
    if (!dpy || w == None) return 0;
    XWindowAttributes wa;
    if (!XGetWindowAttributes(dpy, w, &wa)) return 0;
    int tx = wa.x, ty = wa.y;
    Window cur = w;
    while (1) {
        Window root_ret, parent, *children;
        unsigned nchildren;
        if (!XQueryTree(dpy, cur, &root_ret, &parent, &children, &nchildren)) break;
        if (children) XFree(children);
        if (parent == None || parent == root_ret) break;
        XWindowAttributes pa;
        if (!XGetWindowAttributes(dpy, parent, &pa)) break;
        tx += pa.x; ty += pa.y;
        cur = parent;
    }
    *out_x = tx; *out_y = ty; *out_w = wa.width; *out_h = wa.height;
    return 1;
}

/* Main */
int main(void) {
    signal(SIGINT, handle_sigint);

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "XOpenDisplay failed\n"); return 1; }

    int shape_event_base = 0, shape_error_base = 0;
    int have_shape = XShapeQueryExtension(dpy, &shape_event_base, &shape_error_base);

    int shm_major = 0, shm_minor = 0; Bool shm_pixmaps = 0;
    if (!XShmQueryVersion(dpy, &shm_major, &shm_minor, &shm_pixmaps)) {
        fprintf(stderr, "XShm not available\n");
        XCloseDisplay(dpy); return 1;
    }

    XVisualInfo vinfo;
    if (!find_argb_visual(dpy, &vinfo)) {
        fprintf(stderr, "No 32-bit ARGB visual available\n");
        XCloseDisplay(dpy); return 1;
    }

    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);
    int scr_w = DisplayWidth(dpy, scr);
    int scr_h = DisplayHeight(dpy, scr);

    Colormap col = XCreateColormap(dpy, root, vinfo.visual, AllocNone);

    XSetWindowAttributes swa;
    swa.colormap = col;
    swa.background_pixel = 0;
    swa.border_pixel = 0;
    swa.override_redirect = True;
    swa.event_mask = 0;

    Window overlay = XCreateWindow(dpy, root, 0, 0, scr_w, scr_h, 0,
                                   vinfo.depth, InputOutput, vinfo.visual,
                                   CWColormap | CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWEventMask,
                                   &swa);
    if (!overlay) { fprintf(stderr, "XCreateWindow failed\n"); XCloseDisplay(dpy); return 1; }

    /* Make the window input-transparent: clear ShapeInput so it receives no input */
    if (have_shape) {
        XShapeCombineMask(dpy, overlay, ShapeInput, 0, 0, None, ShapeSet);
    } else {
        fprintf(stderr, "Warning: XShape not present; overlay may block input\n");
    }

    XMapWindow(dpy, overlay);
    XRaiseWindow(dpy, overlay);
    XFlush(dpy);

    /* Setup XShm image */
    XShmSegmentInfo shminfo;
    XImage *xim = XShmCreateImage(dpy, vinfo.visual, vinfo.depth, ZPixmap, NULL, &shminfo, scr_w, scr_h);
    if (!xim) { fprintf(stderr, "XShmCreateImage failed\n"); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }

    size_t shmsize = xim->bytes_per_line * xim->height;
    shminfo.shmid = shmget(IPC_PRIVATE, shmsize, IPC_CREAT | 0600);
    if (shminfo.shmid < 0) { perror("shmget"); XDestroyImage(xim); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }
    shminfo.shmaddr = xim->data = shmat(shminfo.shmid, 0, 0);
    if (xim->data == (char*)-1) { perror("shmat"); shmctl(shminfo.shmid, IPC_RMID, 0); XDestroyImage(xim); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }
    shminfo.readOnly = False;
    if (!XShmAttach(dpy, &shminfo)) { fprintf(stderr, "XShmAttach failed\n"); shmdt(shminfo.shmaddr); shmctl(shminfo.shmid, IPC_RMID, 0); XDestroyImage(xim); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }
    /* mark for removal */
    shmctl(shminfo.shmid, IPC_RMID, 0);

    GC gc = XCreateGC(dpy, overlay, 0, NULL);
    if (!gc) { fprintf(stderr, "XCreateGC failed\n"); XShmDetach(dpy, &shminfo); XDestroyImage(xim); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }

    PinkGen *pg = pink_new(16);
    if (!pg) pg = pink_new(8); /* fallback */

    int bytespp = xim->bits_per_pixel / 8;
    int big_endian = (ImageByteOrder(dpy) == MSBFirst);

    /* Main loop: draw pink noise except inside focused window rect */
    while (running) {
        /* Get focused window and geometry */
        Window focus; int revert;
        XGetInputFocus(dpy, &focus, &revert);
        int have_focus_geo = 0;
        int fx=0, fy=0; unsigned fw=0, fh=0;
        if (focus != None && focus != PointerRoot) {
            if (get_window_root_geometry(dpy, focus, &fx, &fy, &fw, &fh)) have_focus_geo = 1;
        }

        /* Fill XImage memory */
        uint8_t *dst = (uint8_t*)xim->data;
        for (int y = 0; y < scr_h; ++y) {
            for (int x = 0; x < scr_w; ++x) {
                int in_focus = 0;
                if (have_focus_geo) {
                    if ((unsigned)x >= (unsigned)fx && (unsigned)x < (unsigned)(fx + fw) &&
                        (unsigned)y >= (unsigned)fy && (unsigned)y < (unsigned)(fy + fh)) in_focus = 1;
                }
                uint8_t a = in_focus ? 0x00 : 0xC0; /* transparent inside focused window */
                uint8_t v = in_focus ? 0x00 : pink_sample(pg);
                uint8_t r = v;
                uint8_t g = v / 2;
                uint8_t b = v / 3;
                if (bytespp >= 4) {
                    if (big_endian) {
                        dst[0] = a; dst[1] = r; dst[2] = g; dst[3] = b;
                    } else {
                        dst[0] = b; dst[1] = g; dst[2] = r; dst[3] = a;
                    }
                } else if (bytespp == 3) {
                    /* pack as RGB if server expects 24bpp (unlikely for ARGB visual) */
                    dst[0] = r; dst[1] = g; dst[2] = b;
                } else {
                    /* fallback: write zero */
                    for (int i = 0; i < bytespp; ++i) dst[i] = 0;
                }
                dst += bytespp;
            }
        }

        /* Push to X server */
        XShmPutImage(dpy, overlay, gc, xim, 0, 0, 0, 0, scr_w, scr_h, False);
        XFlush(dpy);

        /* throttle */
        usleep(25000); /* ~40 FPS */
    }

    /* cleanup */
    pink_free(pg);
    XFreeGC(dpy, gc);
    XShmDetach(dpy, &shminfo);
    XDestroyImage(xim);
    shmdt(shminfo.shmaddr);
    XUnmapWindow(dpy, overlay);
    XDestroyWindow(dpy, overlay);
    XCloseDisplay(dpy);
    return 0;
}

