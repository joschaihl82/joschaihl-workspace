// alpha_overlay_combined.c
// Vollbild-ARGB-Overlay mit ~85% Alpha, Input-Passthrough, robust gegen BadMatch
// Kompilieren: gcc -O2 -Wall -o alpha_overlay_combined alpha_overlay_combined.c -lX11 -lXrender -lXext

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xrender.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static volatile int running = 1;
static void on_sigint(int s) { (void)s; running = 0; }

int main(void) {
    signal(SIGINT, on_sigint);

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Fehler: Kann X-Display nicht öffnen\n");
        return 1;
    }

    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);
    int sw = DisplayWidth(dpy, scr);
    int sh = DisplayHeight(dpy, scr);

    XVisualInfo vinfo;
    Visual *visual = NULL;
    int depth = 0;
    int have_argb = 0;

    // Versuche 32-bit ARGB Visual (TrueColor, depth 32)
    if (XMatchVisualInfo(dpy, scr, 32, TrueColor, &vinfo)) {
        visual = vinfo.visual;
        depth = vinfo.depth;
        have_argb = 1;
    } else {
        // Fallback: benutze Default Visual/Depth
        visual = DefaultVisual(dpy, scr);
        depth = DefaultDepth(dpy, scr);
        have_argb = 0;
        fprintf(stderr, "Warnung: Kein 32-bit Visual verfügbar, benutze Default-Visual (Alpha evtl. nicht sichtbar)\n");
    }

    // Erzeuge Colormap passend zum gewählten Visual
    Colormap cmap = XCreateColormap(dpy, root, visual, AllocNone);

    XSetWindowAttributes attrs;
    attrs.override_redirect = True; // Fenster nicht vom WM verwalten lassen
    attrs.colormap = cmap;
    attrs.background_pixel = BlackPixel(dpy, scr);
    attrs.event_mask = ExposureMask | StructureNotifyMask;

    unsigned long valuemask = CWOverrideRedirect | CWColormap | CWBackPixel | CWEventMask;

    // Erzeuge sichtbares InputOutput-Fenster mit dem richtigen visual/depth
    Window win = XCreateWindow(
        dpy, root,
        0, 0, sw, sh, 0,
        depth,             // depth passend zum Visual
        InputOutput,       // sichtbares Fenster
        visual,            // Visual passend zur Colormap
        valuemask,
        &attrs
    );

    XStoreName(dpy, win, "alpha_overlay_combined");
    XMapRaised(dpy, win);
    XFlush(dpy);

    // Input-Passthrough: setze Input Shape auf leer (Eingaben gehen durch)
    Region empty_region = XCreateRegion();
    XShapeCombineRegion(dpy, win, ShapeInput, 0, 0, empty_region, ShapeSet);
    XDestroyRegion(empty_region);

    // XRender Picture für das Fenster, falls verfügbar
    XRenderPictFormat *pictFormat = XRenderFindVisualFormat(dpy, visual);
    Picture win_pic = 0;
    if (pictFormat) {
        XRenderPictureAttributes pa;
        pa.subwindow_mode = IncludeInferiors;
        win_pic = XRenderCreatePicture(dpy, win, pictFormat, CPSubwindowMode, &pa);
    } else {
        fprintf(stderr, "Warnung: XRender Format nicht gefunden. Zeichnung per Xlib fallback.\n");
    }

    // Erzeuge eine 128x128 Pixmap mit derselben Tiefe wie das Fenster für das Kachelmuster
    int pat_w = 128, pat_h = 128;
    Pixmap pat_pix = XCreatePixmap(dpy, win, pat_w, pat_h, depth);

    // Wenn XRender verfügbar ist, benutze Picture, sonst GC-Füllung
    Picture pat_pic = 0;
    if (pictFormat) {
        pat_pic = XRenderCreatePicture(dpy, pat_pix, pictFormat, 0, NULL);
    }

    // Alpha-Wert ca. 85% -> 216 / 255. XRenderColor uses 16-bit per channel; alpha as 16-bit.
    unsigned char a8 = (unsigned char)(0.85 * 255.0 + 0.5);
    XRenderColor cols[4];
    cols[0].red   = 0x8000; cols[0].green = 0x2000; cols[0].blue  = 0x2000; cols[0].alpha = (unsigned short)a8 * 257;
    cols[1].red   = 0x2000; cols[1].green = 0x8000; cols[1].blue  = 0x2000; cols[1].alpha = (unsigned short)a8 * 257;
    cols[2].red   = 0x2000; cols[2].green = 0x2000; cols[2].blue  = 0x8000; cols[2].alpha = (unsigned short)a8 * 257;
    cols[3].red   = 0x8000; cols[3].green = 0x8000; cols[3].blue  = 0x2000; cols[3].alpha = (unsigned short)a8 * 257;

    if (pat_pic) {
        // Fülle Pattern-Pixmap mit mehreren farbigen, halbtransparenten Rechtecken
        for (int y = 0; y < pat_h; y += 32) {
            for (int x = 0; x < pat_w; x += 32) {
                int idx = ((x + y) / 32) & 3;
                XRenderFillRectangle(dpy, PictOpOver, pat_pic, &cols[idx], x, y, 32, 32);
                XRenderFillRectangle(dpy, PictOpOver, pat_pic, &cols[(idx+1)&3], x+8, y+8, 16, 16);
            }
        }
    } else {
        // Fallback: einfache Xlib Zeichnung mit GC ohne Alpha
        GC gc = XCreateGC(dpy, pat_pix, 0, NULL);
        for (int y = 0; y < pat_h; y += 32) {
            for (int x = 0; x < pat_w; x += 32) {
                int idx = ((x + y) / 32) & 3;
                XSetForeground(dpy, gc, ((cols[idx].red & 0xFF00) << 8) | (cols[idx].green & 0xFF00) | ((cols[idx].blue & 0xFF00) >> 8));
                XFillRectangle(dpy, pat_pix, gc, x, y, 32, 32);
                XSetForeground(dpy, gc, ((cols[(idx+1)&3].red & 0xFF00) << 8) | (cols[(idx+1)&3].green & 0xFF00) | ((cols[(idx+1)&3].blue & 0xFF00) >> 8));
                XFillRectangle(dpy, pat_pix, gc, x+8, y+8, 16, 16);
            }
        }
        XFreeGC(dpy, gc);
    }

    XFlush(dpy);

    // Hauptloop: tile das Pattern über das Fenster. Wenn XRender verfügbar, benutze Composite mit PictOpOver.
    while (running) {
        if (pat_pic && win_pic) {
            for (int y = 0; y < sh; y += pat_h) {
                for (int x = 0; x < sw; x += pat_w) {
                    XRenderComposite(dpy, PictOpOver, pat_pic, None, win_pic,
                                     0, 0, 0, 0, x, y, pat_w, pat_h);
                }
            }
        } else {
            // Fallback: Kopiere Pixmap mittels XCopyArea (keine Alpha-Komposition erzeugt)
            GC gc = XCreateGC(dpy, win, 0, NULL);
            for (int y = 0; y < sh; y += pat_h) {
                for (int x = 0; x < sw; x += pat_w) {
                    XCopyArea(dpy, pat_pix, win, gc, 0, 0, pat_w, pat_h, x, y);
                }
            }
            XFreeGC(dpy, gc);
        }

        XFlush(dpy);

        // Reagiere auf Events (z. B. Resize) damit das Fenster korrekt bleibt
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == ConfigureNotify) {
                XConfigureEvent ce = ev.xconfigure;
                if (ce.width != sw || ce.height != sh) {
                    sw = ce.width;
                    sh = ce.height;
                }
            }
        }

        usleep(50000); // 50 ms Pause, ~20 FPS
    }

    // Aufräumen
    if (pat_pic) XRenderFreePicture(dpy, pat_pic);
    XFreePixmap(dpy, pat_pix);
    if (win_pic) XRenderFreePicture(dpy, win_pic);
    XDestroyWindow(dpy, win);
    if (cmap != DefaultColormap(dpy, scr)) XFreeColormap(dpy, cmap);
    XCloseDisplay(dpy);
    return 0;
}
