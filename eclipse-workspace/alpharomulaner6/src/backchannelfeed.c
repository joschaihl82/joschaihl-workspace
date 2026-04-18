// overlay_forward_input.c
// Compile: gcc -O2 -o overlay_forward_input overlay_forward_input.c -lX11 -lXext -lXrender -lm
// Run: ./overlay_forward_input
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xext.h>
#include <X11/extensions/Xrender.h>
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

/* Small pink-noise generator */
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

/* Find 32-bit ARGB visual */
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

/* Helper: find window under root coordinates (x,y). Returns child or root if none. */
static Window window_at(Display *dpy, Window root, int x, int y, int *rx, int *ry) {
    Window root_ret = 0, child = 0;
    int win_x = 0, win_y = 0;
    unsigned int mask = 0;
    if (!XQueryPointer(dpy, root, &root_ret, &child, rx, ry, &win_x, &win_y, &mask)) {
        return root;
    }
    if (child == None) return root;
    /* try to translate to child coords */
    Window cur = child;
    int tx = 0, ty = 0;
    while (cur != None && cur != root) {
        XWindowAttributes a;
        if (!XGetWindowAttributes(dpy, cur, &a)) break;
        tx += a.x; ty += a.y;
        Window root_r, parent, *children;
        unsigned n;
        if (!XQueryTree(dpy, cur, &root_r, &parent, &children, &n)) break;
        if (children) XFree(children);
        cur = parent;
    }
    /* Return deepest child found (child) */
    return child ? child : root;
}

/* Forward a Button or Motion event to target window */
static void forward_pointer_event(Display *dpy, XEvent *evt, Window target, int root_x, int root_y) {
    if (!dpy || !evt) return;
    XEvent ne = *evt;
    if (ne.type == ButtonPress || ne.type == ButtonRelease || ne.type == MotionNotify) {
        XButtonEvent *be = (XButtonEvent*)&ne;
        /* Translate root coords to target window coords */
        Window child;
        int tx=0, ty=0;
        XTranslateCoordinates(dpy, RootWindow(dpy, DefaultScreen(dpy)), target, root_x, root_y, &tx, &ty, &child);
        be->window = target;
        be->x = tx; be->y = ty;
        be->x_root = root_x; be->y_root = root_y;
        be->send_event = True;
        XSendEvent(dpy, target, True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, &ne);
        XFlush(dpy);
    }
}

/* Forward a Key event to the window with input focus */
static void forward_key_event(Display *dpy, XEvent *evt) {
    if (!dpy || !evt) return;
    XKeyEvent *ke = (XKeyEvent*)evt;
    Window focus; int revert;
    XGetInputFocus(dpy, &focus, &revert);
    if (focus == None || focus == PointerRoot) return;
    XEvent ne = *evt;
    ne.xkey.window = focus;
    ne.xkey.send_event = True;
    XSendEvent(dpy, focus, True, KeyPressMask|KeyReleaseMask, &ne);
    XFlush(dpy);
}

int main(void) {
    signal(SIGINT, handle_sigint);

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "XOpenDisplay failed\n"); return 1; }

    int shape_ev = 0, shape_err = 0;
    int have_shape = XShapeQueryExtension(dpy, &shape_ev, &shape_err);

    int shm_maj=0, shm_min=0; Bool shm_pix = 0;
    if (!XShmQueryVersion(dpy, &shm_maj, &shm_min, &shm_pix)) {
        fprintf(stderr, "XShm not available\n"); XCloseDisplay(dpy); return 1;
    }

    XVisualInfo vinfo;
    if (!find_argb_visual(dpy, &vinfo)) {
        fprintf(stderr, "No 32-bit ARGB visual\n"); XCloseDisplay(dpy); return 1;
    }

    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);
    int scr_w = DisplayWidth(dpy, scr), scr_h = DisplayHeight(dpy, scr);

    Colormap col = XCreateColormap(dpy, root, vinfo.visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = col; swa.background_pixel = 0; swa.border_pixel = 0;
    swa.override_redirect = True;

    /* If XShape is available, make input-transparent; otherwise set event mask and forward events */
    int want_forwarding = 1;
    Window overlay = XCreateWindow(dpy, root, 0, 0, scr_w, scr_h, 0,
                                   vinfo.depth, InputOutput, vinfo.visual,
                                   CWColormap | CWBackPixel | CWBorderPixel | CWOverrideRedirect,
                                   &swa);
    if (!overlay) { fprintf(stderr,"XCreateWindow failed\n"); XCloseDisplay(dpy); return 1; }

    if (have_shape) {
        XShapeCombineMask(dpy, overlay, ShapeInput, 0, 0, None, ShapeSet);
        want_forwarding = 0; /* no need to forward, X passes through */
    } else {
        /* register for pointer and key events so we can capture and forward them */
        XSelectInput(dpy, overlay, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | ExposureMask);
        want_forwarding = 1;
    }

    XMapWindow(dpy, overlay);
    XRaiseWindow(dpy, overlay);
    XFlush(dpy);

    /* XShm image */
    XShmSegmentInfo shminfo;
    XImage *xim = XShmCreateImage(dpy, vinfo.visual, vinfo.depth, ZPixmap, NULL, &shminfo, scr_w, scr_h);
    if (!xim) { fprintf(stderr,"XShmCreateImage failed\n"); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }
    size_t shmsize = xim->bytes_per_line * xim->height;
    shminfo.shmid = shmget(IPC_PRIVATE, shmsize, IPC_CREAT | 0600);
    if (shminfo.shmid < 0) { perror("shmget"); XDestroyImage(xim); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }
    shminfo.shmaddr = xim->data = shmat(shminfo.shmid, 0, 0);
    if (xim->data == (char*)-1) { perror("shmat"); shmctl(shminfo.shmid, IPC_RMID, 0); XDestroyImage(xim); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }
    shminfo.readOnly = False;
    if (!XShmAttach(dpy, &shminfo)) { fprintf(stderr,"XShmAttach failed\n"); shmdt(shminfo.shmaddr); shmctl(shminfo.shmid, IPC_RMID, 0); XDestroyImage(xim); XDestroyWindow(dpy, overlay); XCloseDisplay(dpy); return 1; }
    shmctl(shminfo.shmid, IPC_RMID, 0);

    GC gc = XCreateGC(dpy, overlay, 0, NULL);
    PinkGen *pg = pink_new(16);
    int bytespp = xim->bits_per_pixel / 8;
    int big_end = (ImageByteOrder(dpy) == MSBFirst);

    /* If we will forward events ourselves, select SubstructureNotify on root to track e.g. focus changes (optional) */
    if (want_forwarding) {
        XSelectInput(dpy, root, SubstructureNotifyMask);
    }

    /* Main loop */
    while (running) {
        /* Fill XImage with pink noise except focused window region */
        Window focus; int revert;
        XGetInputFocus(dpy, &focus, &revert);
        int have_focus_geo = 0;
        int fx=0, fy=0; unsigned fw=0, fh=0;
        if (focus != None && focus != PointerRoot) {
            if (get_window_root_geometry(dpy, focus, &fx, &fy, &fw, &fh)) have_focus_geo = 1;
        }

        uint8_t *dst = (uint8_t*)xim->data;
        for (int y = 0; y < scr_h; ++y) {
            for (int x = 0; x < scr_w; ++x) {
                int in_focus = 0;
                if (have_focus_geo) {
                    if ((unsigned)x >= (unsigned)fx && (unsigned)x < (unsigned)(fx + fw) &&
                        (unsigned)y >= (unsigned)fy && (unsigned)y < (unsigned)(fy + fh)) in_focus = 1;
                }
                uint8_t a = in_focus ? 0x00 : 0xC0;
                uint8_t v = in_focus ? 0x00 : pink_sample(pg);
                uint8_t r = v, g = v/2, b = v/3;
                if (bytespp >= 4) {
                    if (big_end) { dst[0]=a; dst[1]=r; dst[2]=g; dst[3]=b; }
                    else         { dst[0]=b; dst[1]=g; dst[2]=r; dst[3]=a; }
                } else if (bytespp == 3) { dst[0]=r; dst[1]=g; dst[2]=b; }
                else { for (int i=0;i<bytespp;++i) dst[i]=0; }
                dst += bytespp;
            }
        }

        XShmPutImage(dpy, overlay, gc, xim, 0,0,0,0, scr_w, scr_h, False);
        XFlush(dpy);

        /* Handle pending events (including forwarded ones if want_forwarding==1) */
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (!want_forwarding) {
                /* overlay is input-transparent; we may still want to handle Expose etc. */
                if (ev.type == Expose) { /* ignore */ }
                continue;
            }
            /* If overlay received pointer/keyboard events, forward them */
            switch (ev.type) {
                case ButtonPress:
                case ButtonRelease:
                case MotionNotify: {
                    int rx=0, ry=0;
                    /* get root coords from QueryPointer to be robust */
                    Window root_ret, child;
                    int winx, winy;
                    unsigned mask;
                    if (XQueryPointer(dpy, overlay, &root_ret, &child, &rx, &ry, &winx, &winy, &mask)) {
                        Window target = None;
                        /* find deepest child under pointer */
                        target = None;
                        Window cur_root = root_ret;
                        Window cur_child = child;
                        if (child != None) {
                            target = child;
                        } else {
                            target = root;
                        }
                        /* if child is reparented, try to pick the deepest descendant by QueryTree */
                        if (target == None) target = root;
                        forward_pointer_event(dpy, &ev, target, rx, ry);
                    }
                    break;
                }
                case KeyPress:
                case KeyRelease:
                    forward_key_event(dpy, &ev);
                    break;
                default:
                    /* allow other events to be processed or ignored */
                    break;
            }
        }

        /* small sleep to limit CPU */
        //usleep(20000); /* ~50 FPS */
    }

    /* cleanup */
    pink_free(pg);
    XFreeGC(dpy, gc);
    if (xim) {
        XShmDetach(dpy, &shminfo);
        XDestroyImage(xim);
        shmdt(shminfo.shmaddr);
    }
    XUnmapWindow(dpy, overlay);
    XDestroyWindow(dpy, overlay);
    XCloseDisplay(dpy);
    return 0;
}

