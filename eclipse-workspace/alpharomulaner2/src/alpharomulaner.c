// animated_cursor.c
// Dynamischer Alpha-only Bubble-Cursor für X11
// Kompilieren: gcc -O2 -Wall -o animated_cursor animated_cursor.c -lX11 -lXcursor -lm

#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>

static volatile int running = 1;
static void on_int(int x){ (void)x; running = 0; }

static unsigned int rgba_pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    // XcursorImage expects ARGB with 8-bit channels in native endianness packed into unsigned long
    return ( ((unsigned int)a << 24) | ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b );
}

// draw filled circle into 32-bit RGBA buffer (w x h), center (cx,cy), radius, alpha [0..255], color ignored for RGB if you want alpha-only
static void draw_circle_alpha(unsigned int *buf, int w, int h, int cx, int cy, int radius, unsigned char alpha) {
    int r2 = radius * radius;
    int ymin = cy - radius; if (ymin < 0) ymin = 0;
    int ymax = cy + radius; if (ymax > h-1) ymax = h-1;
    int xmin = cx - radius; if (xmin < 0) xmin = 0;
    int xmax = cx + radius; if (xmax > w-1) xmax = w-1;

    for (int y = ymin; y <= ymax; ++y) {
        int dy = y - cy;
        int dy2 = dy * dy;
        for (int x = xmin; x <= xmax; ++x) {
            int dx = x - cx;
            int dist2 = dx*dx + dy2;
            if (dist2 <= r2) {
                // simple linear falloff for alpha toward edge
                float dist = sqrtf((float)dist2);
                float t = 1.0f - (dist / (float)radius);
                if (t < 0.0f) t = 0.0f;
                unsigned char a = (unsigned char)(alpha * t);
                // Keep RGB zero to have alpha-only appearance (most compositors show colored translucency from cursor pixels anyway)
                unsigned int existing = buf[y * w + x];
                unsigned char ex_a = (existing >> 24) & 0xFF;
                // simple max alpha blending for multiple circles
                if (a > ex_a) {
                    buf[y * w + x] = rgba_pixel(0,0,0,a);
                }
            }
        }
    }
}

int main(void) {
    signal(SIGINT, on_int);
    srand((unsigned int)time(NULL));

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Cannot open display\n"); return 1; }

    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);

    // Cursor image size (choose power-of-two-ish small size for perf). Bigger -> more detail.
    const int W = 64;
    const int H = 64;
    const int hotspot_x = W/2;
    const int hotspot_y = H/2;

    // Animation parameters
    const int fps = 25;
    const useconds_t frame_delay = 1000000 / fps;
    const int bubbles_count = 6;
    const int max_radius = 12;
    const unsigned char max_alpha = 220; // max bubble alpha (0..255)

    // state for moving bubbles (relative to hotspot)
    float bx[64], by[64], bvx[64], bvy[64], brz[64];
    for (int i = 0; i < bubbles_count; ++i) {
        float angle = ((float)rand() / RAND_MAX) * M_PI * 2.0f;
        float rad = 8.0f + (rand() % 12);
        bx[i] = cosf(angle) * rad;
        by[i] = sinf(angle) * rad;
        bvx[i] = ((float)rand() / RAND_MAX - 0.5f) * 1.2f;
        bvy[i] = ((float)rand() / RAND_MAX - 0.5f) * 1.2f;
        brz[i] = 6 + (rand() % (max_radius - 4));
    }

    // keep track of last created cursor to free it
    Cursor last_cursor = None;

    while (running) {
        // prepare pixel buffer initialized to fully transparent
        unsigned int *pixels = (unsigned int*)malloc(W * H * sizeof(unsigned int));
        if (!pixels) break;
        memset(pixels, 0, W * H * sizeof(unsigned int)); // zero => alpha 0

        // update bubble positions and draw them
        for (int i = 0; i < bubbles_count; ++i) {
            // jitter / movement
            bx[i] += bvx[i];
            by[i] += bvy[i];
            // bounce
            if (bx[i] > W/2 - 2 || bx[i] < -W/2 + 2) bvx[i] = -bvx[i] + ((rand()%200)/100.0f - 1.0f)*0.2f;
            if (by[i] > H/2 - 2 || by[i] < -H/2 + 2) bvy[i] = -bvy[i] + ((rand()%200)/100.0f - 1.0f)*0.2f;
            // slight size oscillation
            float s = brz[i] * (0.8f + 0.4f * sinf((float)clock() / (float)CLOCKS_PER_SEC * (0.6f + i*0.13f)));
            int cx = hotspot_x + (int)roundf(bx[i]);
            int cy = hotspot_y + (int)roundf(by[i]);
            int r = (int)roundf(s);
            if (r < 1) r = 1;
            draw_circle_alpha(pixels, W, H, cx, cy, r, max_alpha);
        }

        // Optionally draw a faint core dot at hotspot to hint position (low alpha)
        draw_circle_alpha(pixels, W, H, hotspot_x, hotspot_y, 2, 120);

        // build XcursorImage
        XcursorImage *img = XcursorImageCreate(W, H);
        img->xhot = hotspot_x;
        img->yhot = hotspot_y;
        // copy pixels: XcursorImage expects pixel format 0xAARRGGBB
        for (int i = 0; i < W*H; ++i) {
            img->pixels[i] = pixels[i];
        }

        // load cursor and set it on root
        Cursor c = XcursorImageLoadCursor(dpy, img);
        if (c) {
            XDefineCursor(dpy, root, c);
            XFlush(dpy);
            // free previous cursor
            if (last_cursor != None) XFreeCursor(dpy, last_cursor);
            last_cursor = c;
        }

        // cleanup
        XcursorImageDestroy(img);
        free(pixels);

        // sleep and continue animating
        usleep(frame_delay);
    }

    // restore default by undefining cursor on root
    XUndefineCursor(dpy, root);
    if (last_cursor != None) XFreeCursor(dpy, last_cursor);
    XCloseDisplay(dpy);
    return 0;
}
