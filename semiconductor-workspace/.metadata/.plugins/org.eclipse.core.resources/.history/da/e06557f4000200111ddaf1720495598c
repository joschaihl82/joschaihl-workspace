/*
 * main.c
 *
 * GLUT/OpenGL CAD demo — mit Toolbar über dem oberen Lineal (3D-Toolbuttons).
 *
 * Buttons (links->rechts): [Center] [+] [-]
 *
 * Compile:
 *   gcc main.c -o cad -lGL -lGLU -lglut -lm
 *
 * Controls:
 *   - Klick auf Toolbar-Buttons: Center / + / -
 *   - Linke Maustaste + Drag: Pan
 *   - Rechte Maustaste: Punkt setzen
 *   - Mausrad / +/-: animiertes Zoom (vom Content-Center)
 *   - 'u': manuelle Einheit wechseln (schaltet Auto aus)
 *   - 'a': Auto-Einheiten an/aus
 *   - Esc: Beenden
 *
 * Hinweis: Dieses File basiert auf dem vorherigen, korrigierten Code (Timer-Fixes, Zoom-Guards).
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RULER_THICKNESS 28
#define TOOLBAR_HEIGHT 40
#define STATUS_HEIGHT 20
#define MAX_POINTS 8192

typedef struct { const char *name; double mm_per_unit; } UnitDef;
static UnitDef units[] = {
    {"nm", 1e-6}, {"µm", 1e-3}, {"mm", 1.0}, {"cm", 10.0}, {"dm", 100.0}, {"m", 1000.0}
};
static const int UNIT_COUNT = sizeof(units)/sizeof(units[0]);

/* --- Configurable numeric magnitude target range --- */
static const double MAG_MIN = 0.1;
static const double MAG_MAX = 100.0;
/* --------------------------------------------------- */

static int win_w = 1200, win_h = 800;
static double px_per_mm = 96.0 / 25.4;
static double zoom_factor = 1.0;
static double pan_x = 0.0, pan_y = 0.0;
static int mouse_x = 0, mouse_y = 0;
static double points_x[MAX_POINTS], points_y[MAX_POINTS];
static int points_count = 0;

/* Unit selection */
static int manual_unit_index = 2;
static int auto_unit_enabled = 1;
static int current_unit_index = 2;

/* Panning */
static int panning = 0;
static int pan_last_x = 0, pan_last_y = 0;

/* Animation state for zoom */
static int zoom_animating = 0;
static double zoom_start = 1.0;
static double zoom_target = 1.0;
static double anim_duration = 0.20;
static double anim_start_time = 0.0;
static double anim_world_cx = 0.0, anim_world_cy = 0.0;

/* Toolbar state */
enum { BTN_CENTER = 0, BTN_PLUS = 1, BTN_MINUS = 2, BTN_COUNT = 3 };
static int hover_button = -1;
static int pressed_button = -1;

/* Timing helper */
static double now_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* Easing */
static double ease_out_quad(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    return 1.0 - (1.0 - t) * (1.0 - t);
}

/* Nice multipliers */
static double nice_multiplier_for_px(double px_per_unit, double target_px) {
    double seq[] = {1.0,2.0,5.0,10.0,20.0,50.0,100.0,200.0,500.0,1000.0,2000.0,5000.0};
    for (size_t i=0;i<sizeof(seq)/sizeof(seq[0]);++i) if (seq[i]*px_per_unit >= target_px) return seq[i];
    double v = 10000.0; while (v * px_per_unit < target_px) v *= 2.0; return v;
}
static double nice_multiplier_for_px_generic(double px_per_unit, double target_px) { return nice_multiplier_for_px(px_per_unit,target_px); }

/* Content rect (verschoben nach unten wegen Toolbar + Ruler oben) */
static void get_content_rect(int *x0, int *y0, int *cw, int *ch) {
    if (x0) *x0 = RULER_THICKNESS;
    if (y0) *y0 = TOOLBAR_HEIGHT + RULER_THICKNESS;
    if (cw) *cw = win_w - RULER_THICKNESS;
    if (ch) *ch = win_h - (TOOLBAR_HEIGHT + RULER_THICKNESS) - STATUS_HEIGHT;
}
static void get_content_center_screen(int *cx, int *cy) {
    int x0,y0,cw,ch; get_content_rect(&x0,&y0,&cw,&ch);
    if (cx) *cx = x0 + cw/2;
    if (cy) *cy = y0 + ch/2;
}

/* World <-> Screen */
static void world_to_screen(double wx, double wy, int *sx, int *sy) {
    int x0,y0,cw,ch; get_content_rect(&x0,&y0,&cw,&ch);
    double ox = x0 + cw/2.0, oy = y0 + ch/2.0;
    double zx = (wx - pan_x) * zoom_factor, zy = (wy - pan_y) * zoom_factor;
    if (sx) *sx = (int)round(ox + zx);
    if (sy) *sy = (int)round(oy + zy);
}
static void screen_to_world(int sx, int sy, double *wx, double *wy) {
    int x0,y0,cw,ch; get_content_rect(&x0,&y0,&cw,&ch);
    double ox = x0 + cw/2.0, oy = y0 + ch/2.0;
    if (wx) *wx = (double)(sx - ox) / zoom_factor + pan_x;
    if (wy) *wy = (double)(sy - oy) / zoom_factor + pan_y;
}

/* Ensure zoom safe */
static void ensure_zoom_safe() {
    if (zoom_factor < 1e-12) zoom_factor = 1e-12;
}

/* Timer handler forward declaration */
static void timer_handler(int unused);

/* Start zoom animation */
static void start_zoom_animation(double target) {
    zoom_animating = 1;
    zoom_start = zoom_factor;
    zoom_target = target;
    if (anim_duration <= 0.0) anim_duration = 0.20;
    anim_start_time = now_seconds();
    int ccx, ccy; get_content_center_screen(&ccx, &ccy);
    screen_to_world(ccx, ccy, &anim_world_cx, &anim_world_cy);
    glutTimerFunc(16, timer_handler, 0);
}

/* Timer handler */
static void timer_handler(int unused) {
    (void)unused;
    if (!zoom_animating) return;
    double tnow = now_seconds();
    double t = (tnow - anim_start_time) / anim_duration;
    if (t >= 1.0) {
        zoom_factor = zoom_target;
        zoom_animating = 0;
        ensure_zoom_safe();
        int ccx, ccy; get_content_center_screen(&ccx, &ccy);
        pan_x = anim_world_cx - (double)ccx / zoom_factor;
        pan_y = anim_world_cy - (double)ccy / zoom_factor;
        glutPostRedisplay();
        return;
    }
    double e = ease_out_quad(t);
    zoom_factor = zoom_start + (zoom_target - zoom_start) * e;
    ensure_zoom_safe();
    int ccx, ccy; get_content_center_screen(&ccx, &ccy);
    pan_x = anim_world_cx - (double)ccx / zoom_factor;
    pan_y = anim_world_cy - (double)ccy / zoom_factor;
    glutPostRedisplay();
    glutTimerFunc(16, timer_handler, 0);
}

/* Zoom helpers */
static void zoom_in_centered() {
    double factor = 1.15;
    double new_zoom = zoom_factor * factor;
    if (new_zoom > 1e9) new_zoom = 1e9;
    start_zoom_animation(new_zoom);
}
static void zoom_out_centered() {
    double factor = 1.15;
    double new_zoom = zoom_factor / factor;
    if (new_zoom < 1e-9) new_zoom = 1e-9;
    start_zoom_animation(new_zoom);
}

/* Draw text */
static void draw_text_bitmap(int x, int y, const char *s) {
    glRasterPos2i(x,y);
    for (const char *p=s; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
}

/* Draw crosshair */
static void draw_crosshair(int mx, int my) {
    glColor3ub(0,0,0); glLineWidth(1.0f);
    glBegin(GL_LINES);
      glVertex2i(mx-10,my); glVertex2i(mx+10,my);
      glVertex2i(mx,my-10); glVertex2i(mx,my+10);
    glEnd();
}

/* Button geometry helper: compute button rects (in screen coords) */
static void compute_button_rects(int rects[BTN_COUNT][4]) {
    /* layout: left margin 8px, spacing 8px, button width 80 (Center) and 36 for +/- */
    int left = 8;
    int top = 6; /* inside toolbar */
    int btn_h = TOOLBAR_HEIGHT - 12;
    int spacing = 8;
    int x = left;
    /* Center button */
    rects[BTN_CENTER][0] = x; rects[BTN_CENTER][1] = top; rects[BTN_CENTER][2] = x + 80; rects[BTN_CENTER][3] = top + btn_h;
    x += 80 + spacing;
    /* + button */
    rects[BTN_PLUS][0] = x; rects[BTN_PLUS][1] = top; rects[BTN_PLUS][2] = x + 36; rects[BTN_PLUS][3] = top + btn_h;
    x += 36 + spacing;
    /* - button */
    rects[BTN_MINUS][0] = x; rects[BTN_MINUS][1] = top; rects[BTN_MINUS][2] = x + 36; rects[BTN_MINUS][3] = top + btn_h;
}

/* Draw a single 3D-like button */
static void draw_button_rect(int x0, int y0, int x1, int y1, const char *label, int hovered, int pressed) {
    /* base color */
    unsigned char base_r = hovered ? 220 : 200;
    unsigned char base_g = hovered ? 230 : 210;
    unsigned char base_b = hovered ? 255 : 230;
    if (pressed) { base_r = (unsigned char)fmax(0, base_r - 30); base_g = (unsigned char)fmax(0, base_g - 30); base_b = (unsigned char)fmax(0, base_b - 30); }

    /* background */
    glColor3ub(base_r, base_g, base_b);
    glBegin(GL_QUADS);
      glVertex2i(x0, y0);
      glVertex2i(x1, y0);
      glVertex2i(x1, y1);
      glVertex2i(x0, y1);
    glEnd();

    /* bevel: top-left lighter, bottom-right darker for 3D effect */
    glLineWidth(1.0f);
    /* top-left highlight */
    glColor3ub(255,255,255);
    glBegin(GL_LINES);
      glVertex2i(x0, y1-1); glVertex2i(x0, y0);
      glVertex2i(x0, y0); glVertex2i(x1-1, y0);
    glEnd();
    /* bottom-right shadow */
    glColor3ub(120,120,120);
    glBegin(GL_LINES);
      glVertex2i(x1-1, y0); glVertex2i(x1-1, y1-1);
      glVertex2i(x1-1, y1-1); glVertex2i(x0, y1-1);
    glEnd();

    /* inner border */
    glColor3ub(160,160,160);
    glBegin(GL_LINE_LOOP);
      glVertex2i(x0+1, y0+1);
      glVertex2i(x1-1, y0+1);
      glVertex2i(x1-1, y1-1);
      glVertex2i(x0+1, y1-1);
    glEnd();

    /* label centered */
    int label_w = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)label);
    int lx = x0 + (x1 - x0 - label_w) / 2;
    int ly = y0 + (y1 - y0 - 12) / 2 + 10; /* baseline adjustment */
    glColor3ub(0,0,0);
    draw_text_bitmap(lx, ly, label);
}

/* Draw toolbar */
static void draw_toolbar() {
    /* toolbar background */
    glColor3ub(245,245,250);
    glBegin(GL_QUADS);
      glVertex2i(0,0); glVertex2i(win_w,0); glVertex2i(win_w,TOOLBAR_HEIGHT); glVertex2i(0,TOOLBAR_HEIGHT);
    glEnd();

    /* subtle bottom separator */
    glColor3ub(180,180,180);
    glBegin(GL_LINES);
      glVertex2i(0,TOOLBAR_HEIGHT-1); glVertex2i(win_w,TOOLBAR_HEIGHT-1);
    glEnd();

    /* compute button rects and draw */
    int rects[BTN_COUNT][4];
    compute_button_rects(rects);
    for (int i=0;i<BTN_COUNT;++i) {
        int x0 = rects[i][0], y0 = rects[i][1], x1 = rects[i][2], y1 = rects[i][3];
        /* shift down by 0 because toolbar origin is at y=0 */
        const char *label = (i==BTN_CENTER) ? "Center" : (i==BTN_PLUS ? "+" : "-");
        int hovered = (hover_button == i);
        int pressed = (pressed_button == i);
        draw_button_rect(x0, y0, x1, y1, label, hovered, pressed);
    }
}

/* Improved auto unit selection */
static int select_unit_index_auto(double min_px_per_unit) {
    int best = 0;
    double best_score = -1.0;
    double geom_mean = sqrt(MAG_MIN * MAG_MAX);
    for (int i = UNIT_COUNT-1; i >= 0; --i) {
        double px_per_unit = px_per_mm * zoom_factor * units[i].mm_per_unit;
        double mult = nice_multiplier_for_px_generic(px_per_unit, min_px_per_unit);
        double eff_px = mult * px_per_unit;
        double step_units = mult;
        double target_px = min_px_per_unit * 1.8;
        double pixel_score = 1.0 / (1.0 + fabs(eff_px - target_px));
        double mag_score;
        if (step_units >= MAG_MIN && step_units <= MAG_MAX) mag_score = 1.0;
        else {
            double log_dist = fabs(log10(fmax(step_units, 1e-30)) - log10(geom_mean));
            mag_score = 1.0 / (1.0 + log_dist);
        }
        double score = pixel_score * 0.75 + mag_score * 0.25;
        if (score > best_score) { best_score = score; best = i; }
    }
    return best;
}

/* Rulers and ticks (labels without unit text) */
static void draw_rulers_and_ticks() {
    const double MIN_PX_PER_UNIT = 40.0;
    if (auto_unit_enabled) current_unit_index = select_unit_index_auto(MIN_PX_PER_UNIT);
    else current_unit_index = manual_unit_index;
    UnitDef u = units[current_unit_index];

    double px_per_unit = px_per_mm * zoom_factor * u.mm_per_unit;
    if (px_per_unit <= 0.0) px_per_unit = (96.0/25.4) * zoom_factor * u.mm_per_unit;
    double step_units = nice_multiplier_for_px_generic(px_per_unit, MIN_PX_PER_UNIT);
    double step_mm = step_units * u.mm_per_unit;
    double major_units = (step_units >= 10.0) ? step_units : 10.0;

    int cx,cy,cw,ch; get_content_rect(&cx,&cy,&cw,&ch);

    /* ruler backgrounds: top ruler sits below toolbar */
    glColor3ub(240,240,240);
    glBegin(GL_QUADS);
      /* top ruler area (below toolbar) */
      glVertex2i(0,TOOLBAR_HEIGHT); glVertex2i(win_w,TOOLBAR_HEIGHT); glVertex2i(win_w,TOOLBAR_HEIGHT+RULER_THICKNESS); glVertex2i(0,TOOLBAR_HEIGHT+RULER_THICKNESS);
      /* left ruler */
      glVertex2i(0,0); glVertex2i(RULER_THICKNESS,0); glVertex2i(RULER_THICKNESS,win_h); glVertex2i(0,win_h);
    glEnd();

    /* separators */
    glColor3ub(160,160,160);
    glBegin(GL_LINES);
      glVertex2i(0, TOOLBAR_HEIGHT + RULER_THICKNESS - 1); glVertex2i(win_w, TOOLBAR_HEIGHT + RULER_THICKNESS - 1);
      glVertex2i(RULER_THICKNESS - 1, 0); glVertex2i(RULER_THICKNESS - 1, win_h);
    glEnd();

    /* visible world bounds */
    double wx_left_top, wy_left_top, wx_right_bottom, wy_right_bottom;
    screen_to_world(cx, cy, &wx_left_top, &wy_left_top);
    screen_to_world(cx + cw, cy + ch, &wx_right_bottom, &wy_right_bottom);

    int kx_min = (int)floor(wx_left_top / step_mm) - 1;
    int kx_max = (int)ceil(wx_right_bottom / step_mm) + 1;
    int ky_min = (int)floor(wy_left_top / step_mm) - 1;
    int ky_max = (int)ceil(wy_right_bottom / step_mm) + 1;

    /* top ruler ticks (horizontal) */
    glColor3ub(0,0,0);
    int last_label_right = -100000;
    for (int k = kx_min; k <= kx_max; ++k) {
        double world_x = k * step_mm;
        int sx, sy;
        world_to_screen(world_x, pan_y, &sx, &sy);
        if (sx < cx - 2 || sx > cx + cw + 2) continue;
        double units_at_x = world_x / u.mm_per_unit;
        double rem = fmod(fabs(units_at_x), major_units);
        if (rem < 1e-9 || fabs(rem - major_units) < 1e-9) {
            glBegin(GL_LINES);
              glVertex2i(sx, TOOLBAR_HEIGHT + RULER_THICKNESS - 1);
              glVertex2i(sx, TOOLBAR_HEIGHT + RULER_THICKNESS - 1 - 12);
            glEnd();
            char buf[64];
            double absval = fabs(units_at_x);
            if (absval > 1e6 || (absval > 0 && absval < 1e-3)) snprintf(buf, sizeof(buf), "%.3e", units_at_x);
            else {
                if (strcmp(u.name,"nm")==0 || strcmp(u.name,"µm")==0) snprintf(buf, sizeof(buf), "%lld", (long long)llround(units_at_x));
                else {
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%.3f", units_at_x);
                    char *p = tmp + strlen(tmp) - 1;
                    while (p > tmp && *p == '0') { *p = '\0'; --p; }
                    if (p > tmp && *p == '.') *p = '\0';
                    snprintf(buf, sizeof(buf), "%s", tmp);
                }
            }
            int label_w = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)buf);
            int label_left = sx - label_w/2;
            int label_right = label_left + label_w;
            int margin = 4;
            if (label_left >= cx + margin && label_right <= cx + cw - margin && label_left > last_label_right + margin) {
                draw_text_bitmap(label_left, TOOLBAR_HEIGHT + RULER_THICKNESS - 14, buf);
                last_label_right = label_right;
            }
        } else {
            glBegin(GL_LINES);
              glVertex2i(sx, TOOLBAR_HEIGHT + RULER_THICKNESS - 1);
              glVertex2i(sx, TOOLBAR_HEIGHT + RULER_THICKNESS - 1 - 6);
            glEnd();
        }
    }

    /* left ruler ticks (vertical) */
    int last_label_bottom = -100000;
    for (int k = ky_min; k <= ky_max; ++k) {
        double world_y = k * step_mm;
        int sx, sy;
        world_to_screen(pan_x, world_y, &sx, &sy);
        if (sy < cy - 2 || sy > cy + ch + 2) continue;
        double units_at_y = world_y / u.mm_per_unit;
        double rem = fmod(fabs(units_at_y), major_units);
        if (rem < 1e-9 || fabs(rem - major_units) < 1e-9) {
            glBegin(GL_LINES);
              glVertex2i(RULER_THICKNESS - 1, sy);
              glVertex2i(RULER_THICKNESS - 1 - 12, sy);
            glEnd();
            char buf[64];
            double absval = fabs(units_at_y);
            if (absval > 1e6 || (absval > 0 && absval < 1e-3)) snprintf(buf, sizeof(buf), "%.3e", units_at_y);
            else {
                if (strcmp(u.name,"nm")==0 || strcmp(u.name,"µm")==0) snprintf(buf, sizeof(buf), "%lld", (long long)llround(units_at_y));
                else {
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%.3f", units_at_y);
                    char *p = tmp + strlen(tmp) - 1;
                    while (p > tmp && *p == '0') { *p = '\0'; --p; }
                    if (p > tmp && *p == '.') *p = '\0';
                    snprintf(buf, sizeof(buf), "%s", tmp);
                }
            }
            int label_h = 12;
            int label_top = sy - label_h/2;
            int label_bottom = label_top + label_h;
            int margin = 4;
            if (label_top >= cy + margin && label_bottom <= cy + ch - margin && label_top > last_label_bottom + margin) {
                draw_text_bitmap(4, sy + 4, buf);
                last_label_bottom = label_bottom;
            }
        } else {
            glBegin(GL_LINES);
              glVertex2i(RULER_THICKNESS - 1, sy);
              glVertex2i(RULER_THICKNESS - 1 - 6, sy);
            glEnd();
        }
    }

    /* corner status */
    char corner[64];
    if (auto_unit_enabled) snprintf(corner,sizeof(corner),"auto (%s)", units[current_unit_index].name);
    else snprintf(corner,sizeof(corner),"%s (manual)", units[manual_unit_index].name);
    draw_text_bitmap(6, TOOLBAR_HEIGHT + RULER_THICKNESS - 6, corner);
}

/* Display */
static void display_cb(void) {
    glClearColor(1,1,1,1); glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0,win_w,win_h,0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    /* draw toolbar first (topmost area) */
    draw_toolbar();

    /* draw grid, points, rulers, status bar */
    UnitDef u;
    if (auto_unit_enabled) u = units[current_unit_index]; else u = units[manual_unit_index];
    double px_per_unit = px_per_mm * zoom_factor * u.mm_per_unit;
    if (px_per_unit <= 0.0) px_per_unit = (96.0/25.4) * zoom_factor * u.mm_per_unit;
    double MIN_PX_PER_UNIT = 40.0;
    double step_units = nice_multiplier_for_px_generic(px_per_unit, MIN_PX_PER_UNIT);
    double step_mm = step_units * u.mm_per_unit;
    double major_units = (step_units >= 10.0) ? step_units : 10.0;

    int cx,cy,cw,ch; get_content_rect(&cx,&cy,&cw,&ch);
    double wx0,wy0,wx1,wy1; screen_to_world(cx,cy,&wx0,&wy0); screen_to_world(cx+cw,cy+ch,&wx1,&wy1);

    int kx_min = (int)floor(wx0/step_mm)-1, kx_max = (int)ceil(wx1/step_mm)+1;
    int ky_min = (int)floor(wy0/step_mm)-1, ky_max = (int)ceil(wy1/step_mm)+1;

    /* grid */
    glLineWidth(1.0f);
    for (int k=kx_min;k<=kx_max;++k) {
        double wx = k * step_mm; int sx0,sy0,sx1,sy1; world_to_screen(wx,wy0,&sx0,&sy0); world_to_screen(wx,wy1,&sx1,&sy1);
        double units_at_x = wx / u.mm_per_unit; double rem = fmod(fabs(units_at_x), major_units);
        if (rem < 1e-9 || fabs(rem-major_units) < 1e-9) glColor3ub(192,192,192); else glColor3ub(224,224,224);
        glBegin(GL_LINES); glVertex2i(sx0,sy0); glVertex2i(sx1,sy1); glEnd();
    }
    for (int k=ky_min;k<=ky_max;++k) {
        double wy = k * step_mm; int sx0,sy0,sx1,sy1; world_to_screen(wx0,wy,&sx0,&sy0); world_to_screen(wx1,wy,&sx1,&sy1);
        double units_at_y = wy / u.mm_per_unit; double rem = fmod(fabs(units_at_y), major_units);
        if (rem < 1e-9 || fabs(rem-major_units) < 1e-9) glColor3ub(192,192,192); else glColor3ub(224,224,224);
        glBegin(GL_LINES); glVertex2i(sx0,sy0); glVertex2i(sx1,sy1); glEnd();
    }

    /* points */
    glColor3ub(0,0,0);
    for (int i=0;i<points_count;++i) {
        int sx,sy; world_to_screen(points_x[i], points_y[i], &sx, &sy);
        int r = (int)fmax(2.0, 3.0 * zoom_factor);
        glBegin(GL_TRIANGLE_FAN); glVertex2i(sx,sy);
        for (int a=0;a<=20;++a) { double ang = a*(2.0*M_PI/20.0); glVertex2f(sx+cos(ang)*r, sy+sin(ang)*r); }
        glEnd();
    }

    /* rulers on top */
    draw_rulers_and_ticks();

    /* status bar */
    int status_y = win_h - STATUS_HEIGHT;
    glColor3ub(248,248,248); glBegin(GL_QUADS); glVertex2i(0,status_y); glVertex2i(win_w,status_y); glVertex2i(win_w,win_h); glVertex2i(0,win_h); glEnd();
    glColor3ub(192,192,192); glBegin(GL_LINES); glVertex2i(0,status_y); glVertex2i(win_w,status_y); glEnd();
    char status[256];
    if (auto_unit_enabled) snprintf(status,sizeof(status),"Unit: auto (%s)   Zoom: %.2f%%   Points: %d   (press 'u' to manual, 'a' to toggle auto)", units[current_unit_index].name, zoom_factor*100.0, points_count);
    else snprintf(status,sizeof(status),"Unit: manual (%s)   Zoom: %.2f%%   Points: %d   (press 'u' to cycle, 'a' to toggle auto)", units[manual_unit_index].name, zoom_factor*100.0, points_count);
    glColor3ub(0,0,0); draw_text_bitmap(8, status_y + STATUS_HEIGHT - 6, status);

    /* crosshair inside content */
    if (mouse_x >= cx && mouse_x <= cx+cw && mouse_y >= cy && mouse_y <= cy+ch) draw_crosshair(mouse_x, mouse_y);

    glutSwapBuffers();
}

/* Reshape */
static void reshape_cb(int w, int h) { win_w = w; win_h = h; glViewport(0,0,w,h); glutPostRedisplay(); }

/* Helper: check if point inside rect */
static int point_in_rect(int x, int y, int rect[4]) {
    return (x >= rect[0] && x <= rect[2] && y >= rect[1] && y <= rect[3]);
}

/* Mouse handling */
static void mouse_cb(int button, int state, int x, int y) {
    mouse_x = x; mouse_y = y;
    /* toolbar area is y in [0, TOOLBAR_HEIGHT) */
    int rects[BTN_COUNT][4];
    compute_button_rects(rects);

    if (y < TOOLBAR_HEIGHT) {
        /* click in toolbar */
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                /* determine which button */
                hover_button = -1;
                for (int i=0;i<BTN_COUNT;++i) {
                    if (point_in_rect(x, y, rects[i])) { pressed_button = i; hover_button = i; break; }
                }
            } else {
                /* release: if released over same button, trigger action */
                for (int i=0;i<BTN_COUNT;++i) {
                    if (pressed_button == i && point_in_rect(x, y, rects[i])) {
                        /* trigger action */
                        if (i == BTN_CENTER) {
                            /* center view: pan to 0,0 */
                            pan_x = 0.0; pan_y = 0.0;
                            glutPostRedisplay();
                        } else if (i == BTN_PLUS) {
                            zoom_in_centered();
                        } else if (i == BTN_MINUS) {
                            zoom_out_centered();
                        }
                    }
                }
                pressed_button = -1;
                hover_button = -1;
            }
        }
        /* ignore other toolbar interactions for wheel etc. */
        return;
    }

    /* not in toolbar: handle content interactions */
    int cx,cy,cw,ch; get_content_rect(&cx,&cy,&cw,&ch);

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (x >= cx && x <= cx+cw && y >= cy && y <= cy+ch) { panning = 1; pan_last_x = x; pan_last_y = y; }
            else panning = 0;
        } else panning = 0;
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            double wx,wy; screen_to_world(x,y,&wx,&wy);
            if (points_count < MAX_POINTS) { points_x[points_count]=wx; points_y[points_count]=wy; points_count++; }
            glutPostRedisplay();
        }
    } else if (button == 3) { /* wheel up */
        double factor = 1.15; double new_zoom = zoom_factor * factor; if (new_zoom > 1e9) new_zoom = 1e9;
        start_zoom_animation(new_zoom);
    } else if (button == 4) { /* wheel down */
        double factor = 1.15; double new_zoom = zoom_factor / factor; if (new_zoom < 1e-9) new_zoom = 1e-9;
        start_zoom_animation(new_zoom);
    }
}

/* Motion while pressed: pan if active; also update hover for toolbar when mouse moves */
static void motion_cb(int x, int y) {
    int rects[BTN_COUNT][4];
    compute_button_rects(rects);
    mouse_x = x; mouse_y = y;

    if (y < TOOLBAR_HEIGHT) {
        /* update hover state */
        hover_button = -1;
        for (int i=0;i<BTN_COUNT;++i) if (point_in_rect(x,y,rects[i])) { hover_button = i; break; }
        glutPostRedisplay();
        return;
    } else {
        hover_button = -1;
    }

    if (panning) {
        int dx = x - pan_last_x, dy = y - pan_last_y;
        pan_x -= (double)dx / zoom_factor;
        pan_y -= (double)dy / zoom_factor;
        pan_last_x = x; pan_last_y = y;
        glutPostRedisplay();
    } else glutPostRedisplay();
}

/* Passive motion: update hover for toolbar */
static void passive_motion_cb(int x, int y) {
    int rects[BTN_COUNT][4];
    compute_button_rects(rects);
    mouse_x = x; mouse_y = y;
    if (y < TOOLBAR_HEIGHT) {
        hover_button = -1;
        for (int i=0;i<BTN_COUNT;++i) if (point_in_rect(x,y,rects[i])) { hover_button = i; break; }
    } else hover_button = -1;
    glutPostRedisplay();
}

/* Keyboard */
static void keyboard_cb(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) exit(0);
    else if (key == '+' || key == '=') { double factor = 1.15; double new_zoom = zoom_factor * factor; if (new_zoom > 1e9) new_zoom = 1e9; start_zoom_animation(new_zoom); }
    else if (key == '-') { double factor = 1.15; double new_zoom = zoom_factor / factor; if (new_zoom < 1e-9) new_zoom = 1e-9; start_zoom_animation(new_zoom); }
    else if (key == 'u' || key == 'U') { manual_unit_index = (manual_unit_index+1)%UNIT_COUNT; auto_unit_enabled = 0; current_unit_index = manual_unit_index; glutPostRedisplay(); }
    else if (key == 'a' || key == 'A') { auto_unit_enabled = !auto_unit_enabled; if (!auto_unit_enabled) current_unit_index = manual_unit_index; glutPostRedisplay(); }
}

/* Hide cursor and init GL */
static void hide_system_cursor() { glutSetCursor(GLUT_CURSOR_NONE); }
static void init_gl() { glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_w, win_h);
    glutCreateWindow("GLUT CAD - Toolbar mit 3D-Buttons");
    /* Start fullscreen */
    glutFullScreen();

    init_gl();
    hide_system_cursor();

    glutDisplayFunc(display_cb);
    glutReshapeFunc(reshape_cb);
    glutMouseFunc(mouse_cb);
    glutMotionFunc(motion_cb);
    glutPassiveMotionFunc(passive_motion_cb);
    glutKeyboardFunc(keyboard_cb);

    glutMainLoop();
    return 0;
}
