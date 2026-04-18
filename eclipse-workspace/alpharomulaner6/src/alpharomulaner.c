// animated_cursor_big.c
// Dynamischer Alpha-only Bubble-Cursor für X11
// Viele Bubbles und zusätzliche halbtransparente Rechtecke innen
// Großes weiches Fadenkreuz (31 x 31 Gesamtgröße: arm_length=15, thickness=3)
// Kompilieren: gcc -O2 -Wall -o animated_cursor_big animated_cursor_big.c -lX11 -lXcursor -lm

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

// Pack ARGB into 32-bit value (0xAARRGGBB)
static unsigned int rgba_pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return ( ((unsigned int)a << 24) | ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b );
}

// write pixel if new alpha greater than existing alpha (simple max blend)
static void write_pixel_max(unsigned int *buf, int w, int h, int x, int y, unsigned char a) {
    if (x < 0 || x >= w || y < 0 || y >= h) return;
    unsigned int existing = buf[y * w + x];
    unsigned char ex_a = (existing >> 24) & 0xFF;
    if (a > ex_a) {
        buf[y * w + x] = rgba_pixel(0,0,0,a);
    }
}

// draw filled circle with linear falloff for alpha toward edge
static void draw_circle_alpha(unsigned int *buf, int w, int h, int cx, int cy, int radius, unsigned char alpha) {
    if (radius <= 0) return;
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
                float dist = sqrtf((float)dist2);
                float t = 1.0f - (dist / (float)radius);
                if (t < 0.0f) t = 0.0f;
                unsigned char a = (unsigned char)(alpha * t);
                write_pixel_max(buf, w, h, x, y, a);
            }
        }
    }
}

// draw filled rectangle with soft alpha falloff from center (width and height)
static void draw_rect_alpha(unsigned int *buf, int w, int h, int rx, int ry, int rw, int rh, unsigned char alpha) {
    if (rw <= 0 || rh <= 0) return;
    int xmin = rx; if (xmin < 0) xmin = 0;
    int ymin = ry; if (ymin < 0) ymin = 0;
    int xmax = rx + rw - 1; if (xmax > w-1) xmax = w-1;
    int ymax = ry + rh - 1; if (ymax > h-1) ymax = h-1;
    float cx = rx + rw * 0.5f;
    float cy = ry + rh * 0.5f;
    float maxd = sqrtf((rw*rw + rh*rh) * 0.5f);
    if (maxd <= 0.0f) maxd = 1.0f;

    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            float dx = x - cx;
            float dy = y - cy;
            float d = sqrtf(dx*dx + dy*dy);
            float t = 1.0f - (d / maxd);
            if (t < 0.0f) t = 0.0f;
            unsigned char a = (unsigned char)(alpha * t);
            write_pixel_max(buf, w, h, x, y, a);
        }
    }
}

// draw a soft plus sign centered at (cx,cy)
// arm_length: half-length of each arm; thickness: perpendicular radius of arm
// center_alpha: alpha at center; arm_alpha: alpha at arm center
static void draw_plus_soft(unsigned int *buf, int w, int h, int cx, int cy,
                           int arm_length, int thickness, unsigned char center_alpha, unsigned char arm_alpha) {
    // center disc
    draw_circle_alpha(buf, w, h, cx, cy, thickness+1, center_alpha);

    // horizontal arm
    for (int dx = -arm_length; dx <= arm_length; ++dx) {
        int x = cx + dx;
        float along = (arm_length > 0) ? 1.0f - (fabsf((float)dx) / (float)arm_length) : 1.0f;
        if (along < 0.0f) along = 0.0f;
        unsigned char a_center_line = (unsigned char)(arm_alpha * along);
        for (int dy = -thickness; dy <= thickness; ++dy) {
            int y = cy + dy;
            float perp = 1.0f - (fabsf((float)dy) / (float)(thickness+1));
            if (perp < 0.0f) perp = 0.0f;
            unsigned char a = (unsigned char)(a_center_line * perp);
            write_pixel_max(buf, w, h, x, y, a);
        }
    }

    // vertical arm
    for (int dy = -arm_length; dy <= arm_length; ++dy) {
        int y = cy + dy;
        float along = (arm_length > 0) ? 1.0f - (fabsf((float)dy) / (float)arm_length) : 1.0f;
        if (along < 0.0f) along = 0.0f;
        unsigned char a_center_line = (unsigned char)(arm_alpha * along);
        for (int dx = -thickness; dx <= thickness; ++dx) {
            int x = cx + dx;
            float perp = 1.0f - (fabsf((float)dx) / (float)(thickness+1));
            if (perp < 0.0f) perp = 0.0f;
            unsigned char a = (unsigned char)(a_center_line * perp);
            write_pixel_max(buf, w, h, x, y, a);
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

    // Cursor image size
    const int W = 2000;
    const int H = 2000;
    const int hotspot_x = W/2;
    const int hotspot_y = H/2;

    // Animation parameters
    const int fps = 600;
    const useconds_t frame_delay = 10;
    const int bubbles_count = 10000;          // viele Bubbles
    const int rects_count = 40;            // viele kleine Rechtecke
    const int bubble_max_radius = 20;
    const unsigned char bubble_max_alpha = 200; // max bubble alpha (0..255)
    const unsigned char rect_max_alpha = 150;

    // state for moving bubbles (relative to hotspot)
    float bx[256], by[256], bvx[256], bvy[256], brz[256];
    for (int i = 0; i < bubbles_count; ++i) {
        float angle = ((float)rand() / RAND_MAX) * M_PI * 2.0f;
        float rad = 2.0f + (rand() % 60); // spread inward area
        bx[i] = cosf(angle) * rad;
        by[i] = sinf(angle) * rad;
        bvx[i] = ((float)rand() / RAND_MAX - 0.5f) * 3.0f;
        bvy[i] = ((float)rand() / RAND_MAX - 0.5f) * 3.0f;
        brz[i] = 3 + (rand() % (bubble_max_radius - 2));
    }

    // prepare rectangles static/random initial positions around hotspot
    int rrx[256], rry[256], rrw[256], rrh[256];
    for (int i = 0; i < rects_count; ++i) {
        int rx = hotspot_x + (rand() % 161) - 80; // within ±80 px
        int ry = hotspot_y + (rand() % 161) - 80;
        int rw = 2 + (rand() % 14);
        int rh = 2 + (rand() % 14);
        rrx[i] = rx; rry[i] = ry; rrw[i] = rw; rrh[i] = rh;
    }

    // big crosshair settings: 31x31 total => arm_length = 15 each side (15 + 1 center + 15 = 31)
    const int arm_length = 15;
    const int thickness = 3; // softness; visible thickness perpendicular
    const unsigned char center_alpha = 255;
    const unsigned char arm_alpha = 200;

    // keep track of last created cursor to free it
    Cursor last_cursor = None;

    while (running) {
        // prepare pixel buffer initialized to fully transparent
        unsigned int *pixels = (unsigned int*)malloc(W * H * sizeof(unsigned int));
        if (!pixels) break;
        memset(pixels, 0, W * H * sizeof(unsigned int)); // zero => alpha 0

        // update bubble positions and draw them
        for (int i = 0; i < bubbles_count; ++i) {
            // movement
            bx[i] += bvx[i] * ((float)rand() / RAND_MAX * 0.6f + 0.7f);
            by[i] += bvy[i] * ((float)rand() / RAND_MAX * 0.6f + 0.7f);
            // keep them roughly inside a box around hotspot +/- 140 px
            if (bx[i] > 140) { bx[i] = 140; bvx[i] = -fabsf(bvx[i]); }
            if (bx[i] < -140) { bx[i] = -140; bvx[i] = fabsf(bvx[i]); }
            if (by[i] > 140) { by[i] = 140; bvy[i] = -fabsf(bvy[i]); }
            if (by[i] < -140) { by[i] = -140; bvy[i] = fabsf(bvy[i]); }
            // slight size oscillation
            float s = brz[i] * (0.7f + 0.6f * sinf(((float)clock() / (float)CLOCKS_PER_SEC) * (0.6f + i*0.07f)));
            int cx = hotspot_x + (int)roundf(bx[i]);
            int cy = hotspot_y + (int)roundf(by[i]);
            int r = (int)roundf(s);
            if (r < 1) r = 1;
            draw_circle_alpha(pixels, W, H, cx, cy, r, bubble_max_alpha);
        }

        // draw many small rectangles randomly jittered each frame (adds visual clutter)
        for (int i = 0; i < rects_count; ++i) {
            int jitter_x = (rand() % 9) - 4;
            int jitter_y = (rand() % 9) - 4;
            int rx = rrx[i] + jitter_x;
            int ry = rry[i] + jitter_y;
            int rw = rrw[i];
            int rh = rrh[i];
            draw_rect_alpha(pixels, W, H, rx, ry, rw, rh, rect_max_alpha);
        }

        // draw soft plus at hotspot (31x31 overall)
        draw_plus_soft(pixels, W, H, hotspot_x, hotspot_y, arm_length, thickness, center_alpha, arm_alpha);

        // Optionally draw a faint core dot as extra hint (small)
        draw_circle_alpha(pixels, W, H, hotspot_x, hotspot_y, 3, 200);

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
