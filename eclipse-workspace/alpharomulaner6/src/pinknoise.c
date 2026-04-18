// overlay_visible_input_pass.c
// Compile: gcc -O2 -o overlay_visible_input_pass overlay_visible_input_pass.c -lX11 -lXext -lXrender -lm
// Run: ./overlay_visible_input_pass
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>
#include <X11/Xatom.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static volatile int running = 1;
static void onint(int s){ (void)s; running = 0; }

/* find a 32-bit ARGB visual */
static int find_argb_visual(Display *dpy, XVisualInfo *out) {
    XVisualInfo tmpl; tmpl.screen = DefaultScreen(dpy);
    int n;
    XVisualInfo *vis = XGetVisualInfo(dpy, VisualScreenMask, &tmpl, &n);
    if (!vis) return 0;
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

int main(void){
    signal(SIGINT, onint);
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr,"cannot open display\n"); return 1; }
    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);

    XVisualInfo vinfo;
    if (!find_argb_visual(dpy, &vinfo)) {
        fprintf(stderr,"no 32-bit ARGB visual available\n");
        XCloseDisplay(dpy); return 1;
    }

    Colormap col = XCreateColormap(dpy, root, vinfo.visual, AllocNone);

    XSetWindowAttributes swa;
    swa.colormap = col;
    swa.background_pixel = 0;
    swa.border_pixel = 0;
    swa.override_redirect = True;
    swa.event_mask = 0;

    int w = DisplayWidth(dpy, scr), h = DisplayHeight(dpy, scr);
    Window overlay = XCreateWindow(dpy, root, 0, 0, w, h, 0,
                                   vinfo.depth, InputOutput, vinfo.visual,
                                   CWColormap | CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWEventMask,
                                   &swa);

    /* Make input shape empty so the window receives no input at all */
    if (XShapeQueryExtension(dpy, NULL, NULL)) {
        /* Using None with ShapeSet clears the input shape -> window receives no input */
        XShapeCombineMask(dpy, overlay, ShapeInput, 0, 0, None, ShapeSet);
    } else {
        fprintf(stderr,"XShape not available; overlay may block input\n");
    }

    XMapWindow(dpy, overlay);
    XRaiseWindow(dpy, overlay);

    /* Setup XShm image for fast ARGB updates */
    if (!XShmQueryExtension(dpy)) {
        fprintf(stderr,"XShm not available\n"); XCloseDisplay(dpy); return 1;
    }

    XShmSegmentInfo shminfo;
    XImage *xim = XShmCreateImage(dpy, vinfo.visual, vinfo.depth, ZPixmap, NULL, &shminfo, w, h);
    shminfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line * xim->height, IPC_CREAT | 0600);
    if (shminfo.shmid < 0) { perror("shmget"); return 1; }
    shminfo.shmaddr = xim->data = shmat(shminfo.shmid, 0, 0);
    if (xim->data == (char*)-1) { perror("shmat"); return 1; }
    shminfo.readOnly = False;
    if (!XShmAttach(dpy, &shminfo)) { fprintf(stderr,"XShmAttach failed\n"); return 1; }
    shmctl(shminfo.shmid, IPC_RMID, 0);

    GC gc = XCreateGC(dpy, overlay, 0, NULL);
    int bytespp = xim->bits_per_pixel / 8;
    int big_end = (ImageByteOrder(dpy) == MSBFirst);

    /* simple animated pink-ish noise for demonstration */
    srand((unsigned)time(NULL));
    while (running) {
        uint8_t *dst = (uint8_t*)xim->data;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                uint8_t v = rand() & 0xFF;
                uint8_t a = 0xC0;               /* semi-opaque */
                uint8_t r = v;                  /* pink-ish */
                uint8_t g = v/2;
                uint8_t b = v/3;
                if (big_end) {
                    dst[0]=a; dst[1]=r; dst[2]=g; dst[3]=b;
                } else {
                    dst[0]=b; dst[1]=g; dst[2]=r; dst[3]=a;
                }
                dst += bytespp;
            }
        }
        XShmPutImage(dpy, overlay, gc, xim, 0,0,0,0, w, h, False);
        XFlush(dpy);
        usleep(30000);
    }

    XShmDetach(dpy, &shminfo);
    XDestroyImage(xim);
    shmdt(shminfo.shmaddr);
    XFreeGC(dpy, gc);
    XUnmapWindow(dpy, overlay);
    XDestroyWindow(dpy, overlay);
    XCloseDisplay(dpy);
    return 0;
}

