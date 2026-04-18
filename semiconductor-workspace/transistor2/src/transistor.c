/*
 * main.c
 *
 * GLUT/OpenGL CAD demo — Pan (drag) implemented: left-button drag pans the drawing area.
 * Other features: cursor-centered zoom, auto unit selection (nm..m), grid/rulers aligned,
 * non-overlapping ruler labels, bottom status bar.
 *
 * Compile:
 *   gcc main.c -o cad -lGL -lGLU -lglut -lm
 *
 * Controls:
 *   Left button down + move : pan (verschieben) the drawing area
 *   Right click             : add point at mouse world position
 *   Mouse wheel up/down     : zoom in/out at mouse
 *   + / =                   : zoom in (centered at cursor if inside content area)
 *   -                       : zoom out
 *   u                       : cycle manual unit (switches to manual)
 *   a                       : toggle auto-unit selection on/off
 *   Esc                     : quit
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Configuration */
#define RULER_THICKNESS 28
#define STATUS_HEIGHT 20
#define MAX_POINTS 8192

/* Units */
typedef struct {
    const char *name;   /* label */
    double mm_per_unit; /* how many mm in one unit */
} UnitDef;

static UnitDef units[] = {
    {"nm", 1e-6},
    {"µm", 1e-3},
    {"mm", 1.0},
    {"cm", 10.0},
    {"dm", 100.0},
    {"m", 1000.0}
};
static const int UNIT_COUNT = sizeof(units)/sizeof(units[0]);

/* Global state */
static int win_w = 1200, win_h = 800;
static double px_per_mm = 96.0 / 25.4; /* default px/mm (≈96 DPI) */

static double zoom_factor = 1.0; /* 1.0 = 100% */
static double pan_x = 0.0; /* world center x (mm) */
static double pan_y = 0.0; /* world center y (mm) */

static int mouse_x = 0, mouse_y = 0; /* last mouse position (window coords) */

static double points_x[MAX_POINTS];
static double points_y[MAX_POINTS];
static int points_count = 0;

/* Unit selection state */
static int manual_unit_index = 2; /* default mm when manual */
static int auto_unit_enabled = 1; /* automatic unit selection on by default */
static int current_unit_index = 2; /* effective unit index used for labels (updated each frame) */

/* Panning state */
static int panning = 0;           /* 1 while left button is held and dragging */
static int pan_last_x = 0;
static int pan_last_y = 0;

/* Helper: choose a "nice" multiplier (1,2,5,10,...) for ticks */
static double nice_multiplier_for_px(double px_per_unit, double target_px) {
    double seq[] = {1.0,2.0,5.0,10.0,20.0,50.0,100.0,200.0,500.0,1000.0,2000.0,5000.0};
    for (size_t i=0;i<sizeof(seq)/sizeof(seq[0]);++i) {
        if (seq[i] * px_per_unit >= target_px) return seq[i];
    }
    double v = 10000.0;
    while (v * px_per_unit < target_px) v *= 2.0;
    return v;
}
static double nice_multiplier_for_px_generic(double px_per_unit, double target_px) {
    return nice_multiplier_for_px(px_per_unit, target_px);
}

/* Content area (drawing area) */
static void get_content_rect(int *x0, int *y0, int *cw, int *ch) {
    if (x0) *x0 = RULER_THICKNESS;
    if (y0) *y0 = RULER_THICKNESS;
    if (cw) *cw = win_w - RULER_THICKNESS;
    if (ch) *ch = win_h - RULER_THICKNESS - STATUS_HEIGHT;
}

/* World (mm) -> Screen (px) using content area center as origin */
static void world_to_screen(double wx, double wy, int *sx, int *sy) {
    int cx, cy, cw, ch;
    get_content_rect(&cx, &cy, &cw, &ch);
    double origin_x = cx + cw / 2.0;
    double origin_y = cy + ch / 2.0;
    double zx = (wx - pan_x) * zoom_factor;
    double zy = (wy - pan_y) * zoom_factor;
    if (sx) *sx = (int)round(origin_x + zx);
    if (sy) *sy = (int)round(origin_y + zy);
}

/* Screen -> World (mm) using content area center as origin */
static void screen_to_world(int sx, int sy, double *wx, double *wy) {
    int cx, cy, cw, ch;
    get_content_rect(&cx, &cy, &cw, &ch);
    double origin_x = cx + cw / 2.0;
    double origin_y = cy + ch / 2.0;
    if (wx) *wx = (double)(sx - origin_x) / zoom_factor + pan_x;
    if (wy) *wy = (double)(sy - origin_y) / zoom_factor + pan_y;
}

/* Adjust zoom so that a screen point remains on the same world coordinate */
static void adjust_zoom_at(double new_zoom, int screen_x, int screen_y) {
    if (new_zoom <= 0.0) return;
    double wx_before, wy_before;
    screen_to_world(screen_x, screen_y, &wx_before, &wy_before);
    zoom_factor = new_zoom;
    pan_x = wx_before - (double)screen_x / zoom_factor;
    pan_y = wy_before - (double)screen_y / zoom_factor;
}
static void zoom_in_at(int sx, int sy) {
    double factor = 1.15;
    double new_zoom = zoom_factor * factor;
    if (new_zoom > 1e9) new_zoom = 1e9;
    adjust_zoom_at(new_zoom, sx, sy);
}
static void zoom_out_at(int sx, int sy) {
    double factor = 1.15;
    double new_zoom = zoom_factor / factor;
    if (new_zoom < 1e-9) new_zoom = 1e-9;
    adjust_zoom_at(new_zoom, sx, sy);
}

/* Draw text using GLUT bitmap font at pixel coordinates (x,y) baseline */
static void draw_text_bitmap(int x, int y, const char *s) {
    glRasterPos2i(x, y);
    for (const char *p = s; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
}

/* Draw crosshair at mouse position */
static void draw_crosshair(int mx, int my) {
    glColor3ub(0,0,0);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
      glVertex2i(mx - 10, my);
      glVertex2i(mx + 10, my);
      glVertex2i(mx, my - 10);
      glVertex2i(mx, my + 10);
    glEnd();
}

/* Automatic unit selection:
   choose unit index so that a "nice" step in that unit yields readable pixel spacing
*/
static int select_unit_index_auto(double min_px_per_unit) {
    int best = 0;
    double best_score = -1.0;
    for (int i = UNIT_COUNT-1; i >= 0; --i) {
        double px_per_unit = px_per_mm * zoom_factor * units[i].mm_per_unit;
        double mult = nice_multiplier_for_px_generic(px_per_unit, min_px_per_unit);
        double eff_px = mult * px_per_unit;
        double target = min_px_per_unit * 1.8;
        double score = 1.0 / (1.0 + fabs(eff_px - target));
        if (score > best_score) { best_score = score; best = i; }
    }
    return best;
}

/* Draw rulers and ticks. Uses the exact same origin and step as the grid.
   Labels are numeric only (no unit text) and are drawn only if they do not overlap.
*/
static void draw_rulers_and_ticks() {
    /* Decide unit: auto or manual */
    const double MIN_PX_PER_UNIT = 40.0; /* threshold to keep ticks readable */
    if (auto_unit_enabled) {
        current_unit_index = select_unit_index_auto(MIN_PX_PER_UNIT);
    } else {
        current_unit_index = manual_unit_index;
    }
    UnitDef u = units[current_unit_index];

    /* compute px per unit (unit = current selected unit) */
    double px_per_unit = px_per_mm * zoom_factor * u.mm_per_unit; /* px per 1 unit */
    if (px_per_unit <= 0.0) px_per_unit = (96.0/25.4) * zoom_factor * u.mm_per_unit;

    /* choose a nice multiplier for ticks so that ticks are not too dense */
    double step_units = nice_multiplier_for_px_generic(px_per_unit, MIN_PX_PER_UNIT); /* e.g., 1,2,5,10,... units */
    double step_mm = step_units * u.mm_per_unit; /* convert to mm for world grid */
    double major_units = (step_units >= 10.0) ? step_units : 10.0;

    int cx, cy, cw, ch;
    get_content_rect(&cx, &cy, &cw, &ch);
    int origin_sx = cx + cw / 2;
    int origin_sy = cy + ch / 2;

    /* Ruler backgrounds */
    glColor3ub(240,240,240);
    glBegin(GL_QUADS);
      glVertex2i(0,0); glVertex2i(win_w,0); glVertex2i(win_w,RULER_THICKNESS); glVertex2i(0,RULER_THICKNESS);
      glVertex2i(0,0); glVertex2i(RULER_THICKNESS,0); glVertex2i(RULER_THICKNESS,win_h); glVertex2i(0,win_h);
    glEnd();

    /* Separators */
    glColor3ub(160,160,160);
    glBegin(GL_LINES);
      glVertex2i(0, RULER_THICKNESS-1); glVertex2i(win_w, RULER_THICKNESS-1);
      glVertex2i(RULER_THICKNESS-1, 0); glVertex2i(RULER_THICKNESS-1, win_h);
    glEnd();

    /* Compute visible world bounds in mm using content corners */
    double wx_left_top, wy_left_top, wx_right_bottom, wy_right_bottom;
    screen_to_world(cx, cy, &wx_left_top, &wy_left_top);
    screen_to_world(cx + cw, cy + ch, &wx_right_bottom, &wy_right_bottom);

    /* Determine integer k ranges for multiples of step_mm */
    int kx_min = (int)floor(wx_left_top / step_mm) - 1;
    int kx_max = (int)ceil(wx_right_bottom / step_mm) + 1;
    int ky_min = (int)floor(wy_left_top / step_mm) - 1;
    int ky_max = (int)ceil(wy_right_bottom / step_mm) + 1;

    /* Top ruler ticks (horizontal) */
    glColor3ub(0,0,0);
    /* track rightmost pixel used by last label to avoid overlap */
    int last_label_right = -100000;
    for (int k = kx_min; k <= kx_max; ++k) {
        double world_x = k * step_mm; /* mm */
        int sx, sy;
        world_to_screen(world_x, pan_y, &sx, &sy); /* sy unused */
        if (sx < cx - 2 || sx > cx + cw + 2) continue;
        double units_at_x = world_x / u.mm_per_unit;
        double rem = fmod(fabs(units_at_x), major_units);
        if (rem < 1e-9 || fabs(rem - major_units) < 1e-9) {
            /* major tick */
            glBegin(GL_LINES);
              glVertex2i(sx, RULER_THICKNESS-1);
              glVertex2i(sx, RULER_THICKNESS-1 - 12);
            glEnd();
            /* numeric label only (no unit text) */
            double label_val = units_at_x;
            char buf[64];
            if (fabs(label_val) < 0.5) snprintf(buf, sizeof(buf), "0");
            else {
                if (strcmp(u.name, "nm") == 0 || strcmp(u.name, "µm") == 0) {
                    long long iv = (long long)llround(label_val);
                    snprintf(buf, sizeof(buf), "%lld", iv);
                } else {
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%.3f", label_val);
                    char *p = tmp + strlen(tmp) - 1;
                    while (p > tmp && *p == '0') { *p = '\0'; --p; }
                    if (p > tmp && *p == '.') *p = '\0';
                    snprintf(buf, sizeof(buf), "%s", tmp);
                }
            }
            /* compute label width using GLUT helper */
            int label_w = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)buf);
            int label_left = sx - label_w/2;
            int label_right = label_left + label_w;
            /* ensure label fits inside content area horizontally and does not overlap previous label */
            int margin = 4;
            if (label_left >= cx + margin && label_right <= cx + cw - margin && label_left > last_label_right + margin) {
                draw_text_bitmap(label_left, RULER_THICKNESS - 14, buf);
                last_label_right = label_right;
            }
        } else {
            /* minor tick */
            glBegin(GL_LINES);
              glVertex2i(sx, RULER_THICKNESS-1);
              glVertex2i(sx, RULER_THICKNESS-1 - 6);
            glEnd();
        }
    }

    /* Left ruler ticks (vertical) */
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
              glVertex2i(RULER_THICKNESS-1, sy);
              glVertex2i(RULER_THICKNESS-1 - 12, sy);
            glEnd();
            char buf[64];
            if (fabs(units_at_y) < 0.5) snprintf(buf, sizeof(buf), "0");
            else {
                if (strcmp(u.name, "nm") == 0 || strcmp(u.name, "µm") == 0) {
                    long long iv = (long long)llround(units_at_y);
                    snprintf(buf, sizeof(buf), "%lld", iv);
                } else {
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%.3f", units_at_y);
                    char *p = tmp + strlen(tmp) - 1;
                    while (p > tmp && *p == '0') { *p = '\0'; --p; }
                    if (p > tmp && *p == '.') *p = '\0';
                    snprintf(buf, sizeof(buf), "%s", tmp);
                }
            }
            /* For left ruler avoid vertical overlap */
            int label_h = 12; /* approx height of GLUT_BITMAP_HELVETICA_12 */
            int label_top = sy - label_h/2;
            int label_bottom = label_top + label_h;
            int margin = 4;
            if (label_top >= cy + margin && label_bottom <= cy + ch - margin && label_top > last_label_bottom + margin) {
                draw_text_bitmap(4, sy + 4, buf);
                last_label_bottom = label_bottom;
            }
        } else {
            glBegin(GL_LINES);
              glVertex2i(RULER_THICKNESS-1, sy);
              glVertex2i(RULER_THICKNESS-1 - 6, sy);
            glEnd();
        }
    }

    /* Corner label: show unit and whether auto is active (status only) */
    char corner[64];
    if (auto_unit_enabled) snprintf(corner, sizeof(corner), "auto (%s)", u.name);
    else snprintf(corner, sizeof(corner), "%s (manual)", units[manual_unit_index].name);
    draw_text_bitmap(6, RULER_THICKNESS - 6, corner);
}

/* Display callback */
static void display_cb(void) {
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, win_w, win_h, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Grid (background) uses same step as rulers */
    UnitDef u;
    if (auto_unit_enabled) u = units[current_unit_index]; else u = units[manual_unit_index];
    /* compute px per unit */
    double px_per_unit = px_per_mm * zoom_factor * u.mm_per_unit;
    if (px_per_unit <= 0.0) px_per_unit = (96.0/25.4) * zoom_factor * u.mm_per_unit;
    double MIN_PX_PER_UNIT = 40.0;
    double step_units = nice_multiplier_for_px_generic(px_per_unit, MIN_PX_PER_UNIT);
    double step_mm = step_units * u.mm_per_unit;
    double major_units = (step_units >= 10.0) ? step_units : 10.0;

    int cx, cy, cw, ch;
    get_content_rect(&cx, &cy, &cw, &ch);

    /* Compute visible world bounds using content corners */
    double wx0, wy0, wx1, wy1;
    screen_to_world(cx, cy, &wx0, &wy0);
    screen_to_world(cx + cw, cy + ch, &wx1, &wy1);

    /* Determine integer k ranges for multiples of step_mm */
    int kx_min = (int)floor(wx0 / step_mm) - 1;
    int kx_max = (int)ceil(wx1 / step_mm) + 1;
    int ky_min = (int)floor(wy0 / step_mm) - 1;
    int ky_max = (int)ceil(wy1 / step_mm) + 1;

    /* Draw vertical grid lines */
    glLineWidth(1.0f);
    for (int k = kx_min; k <= kx_max; ++k) {
        double world_x = k * step_mm;
        int sx0, sy0, sx1, sy1;
        world_to_screen(world_x, wy0, &sx0, &sy0);
        world_to_screen(world_x, wy1, &sx1, &sy1);
        double units_at_x = world_x / u.mm_per_unit;
        double rem = fmod(fabs(units_at_x), major_units);
        if (rem < 1e-9 || fabs(rem - major_units) < 1e-9) glColor3ub(192,192,192);
        else glColor3ub(224,224,224);
        glBegin(GL_LINES); glVertex2i(sx0, sy0); glVertex2i(sx1, sy1); glEnd();
    }

    /* Draw horizontal grid lines */
    for (int k = ky_min; k <= ky_max; ++k) {
        double world_y = k * step_mm;
        int sx0, sy0, sx1, sy1;
        world_to_screen(wx0, world_y, &sx0, &sy0);
        world_to_screen(wx1, world_y, &sx1, &sy1);
        double units_at_y = world_y / u.mm_per_unit;
        double rem = fmod(fabs(units_at_y), major_units);
        if (rem < 1e-9 || fabs(rem - major_units) < 1e-9) glColor3ub(192,192,192);
        else glColor3ub(224,224,224);
        glBegin(GL_LINES); glVertex2i(sx0, sy0); glVertex2i(sx1, sy1); glEnd();
    }

    /* Draw points (above grid, below rulers) */
    glColor3ub(0,0,0);
    for (int i=0;i<points_count;++i) {
        int sx, sy; world_to_screen(points_x[i], points_y[i], &sx, &sy);
        int r = (int)fmax(2.0, 3.0 * zoom_factor);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(sx, sy);
        for (int a=0;a<=20;++a) {
            double ang = a * (2.0*M_PI/20.0);
            glVertex2f(sx + cos(ang)*r, sy + sin(ang)*r);
        }
        glEnd();
    }

    /* Draw rulers on top (foreground) */
    draw_rulers_and_ticks();

    /* Bottom status bar */
    int status_y = win_h - STATUS_HEIGHT;
    glColor3ub(248,248,248);
    glBegin(GL_QUADS);
      glVertex2i(0, status_y); glVertex2i(win_w, status_y); glVertex2i(win_w, win_h); glVertex2i(0, win_h);
    glEnd();
    glColor3ub(192,192,192);
    glBegin(GL_LINES); glVertex2i(0, status_y); glVertex2i(win_w, status_y); glEnd();

    /* Status text: show current unit and zoom */
    char status[256];
    if (auto_unit_enabled)
        snprintf(status, sizeof(status), "Unit: auto (%s)   Zoom: %.2f%%   Points: %d   (press 'u' to manual, 'a' to toggle auto)", units[current_unit_index].name, zoom_factor*100.0, points_count);
    else
        snprintf(status, sizeof(status), "Unit: manual (%s)   Zoom: %.2f%%   Points: %d   (press 'u' to cycle, 'a' to toggle auto)", units[manual_unit_index].name, zoom_factor*100.0, points_count);
    glColor3ub(0,0,0);
    draw_text_bitmap(8, status_y + STATUS_HEIGHT - 6, status);

    /* Crosshair only inside content area */
    if (mouse_x >= cx && mouse_x <= cx + cw && mouse_y >= cy && mouse_y <= cy + ch) {
        draw_crosshair(mouse_x, mouse_y);
    }

    glutSwapBuffers();
}

/* Reshape */
static void reshape_cb(int w, int h) {
    win_w = w; win_h = h;
    glViewport(0,0,w,h);
    glutPostRedisplay();
}

/* Mouse button:
   - Left button down starts panning (store last mouse pos)
   - Left button up stops panning
   - Right button down adds a point at world position
   - Wheel (buttons 3/4) zooms at mouse
*/
static void mouse_cb(int button, int state, int x, int y) {
    mouse_x = x; mouse_y = y;
    int cx, cy, cw, ch; get_content_rect(&cx, &cy, &cw, &ch);

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            /* start panning only if inside content area */
            if (x >= cx && x <= cx + cw && y >= cy && y <= cy + ch) {
                panning = 1;
                pan_last_x = x;
                pan_last_y = y;
            } else {
                panning = 0;
            }
        } else {
            /* button up */
            panning = 0;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            /* add point at world coordinate */
            double wx, wy; screen_to_world(x, y, &wx, &wy);
            if (points_count < MAX_POINTS) { points_x[points_count] = wx; points_y[points_count] = wy; points_count++; }
            glutPostRedisplay();
        }
    } else if (button == 3) { /* wheel up */
        int zx = x, zy = y;
        if (!(x >= cx && x <= cx + cw && y >= cy && y <= cy + ch)) { zx = cx + cw/2; zy = cy + ch/2; }
        zoom_in_at(zx, zy);
        glutPostRedisplay();
    } else if (button == 4) { /* wheel down */
        int zx = x, zy = y;
        if (!(x >= cx && x <= cx + cw && y >= cy && y <= cy + ch)) { zx = cx + cw/2; zy = cy + ch/2; }
        zoom_out_at(zx, zy);
        glutPostRedisplay();
    }
}

/* Motion while a button is pressed: if panning active, update pan based on mouse delta */
static void motion_cb(int x, int y) {
    int cx, cy, cw, ch; get_content_rect(&cx, &cy, &cw, &ch);
    mouse_x = x; mouse_y = y;
    if (panning) {
        int dx = x - pan_last_x;
        int dy = y - pan_last_y;
        /* update pan: pan_x -= dx / zoom, pan_y -= dy / zoom */
        pan_x -= (double)dx / zoom_factor;
        pan_y -= (double)dy / zoom_factor;
        pan_last_x = x;
        pan_last_y = y;
        glutPostRedisplay();
    } else {
        glutPostRedisplay();
    }
}

/* Passive motion (no buttons): just update mouse pos for crosshair */
static void passive_motion_cb(int x, int y) { mouse_x = x; mouse_y = y; glutPostRedisplay(); }

/* Keyboard: zoom centered at cursor if inside content area; 'u' cycles manual unit; 'a' toggles auto */
static void keyboard_cb(unsigned char key, int x, int y) {
    (void)x; (void)y;
    int cx, cy, cw, ch; get_content_rect(&cx, &cy, &cw, &ch);
    int zx = mouse_x, zy = mouse_y;
    if (!(mouse_x >= cx && mouse_x <= cx + cw && mouse_y >= cy && mouse_y <= cy + ch)) {
        zx = cx + cw/2;
        zy = cy + ch/2;
    }
    if (key == 27) exit(0);
    else if (key == '+' || key == '=') { zoom_in_at(zx, zy); glutPostRedisplay(); }
    else if (key == '-') { zoom_out_at(zx, zy); glutPostRedisplay(); }
    else if (key == 'u' || key == 'U') {
        manual_unit_index = (manual_unit_index + 1) % UNIT_COUNT;
        auto_unit_enabled = 0; /* switch to manual when user cycles */
        current_unit_index = manual_unit_index;
        glutPostRedisplay();
    } else if (key == 'a' || key == 'A') {
        auto_unit_enabled = !auto_unit_enabled;
        if (!auto_unit_enabled) current_unit_index = manual_unit_index;
        glutPostRedisplay();
    }
}

/* Hide system cursor and draw our own crosshair */
static void hide_system_cursor() { glutSetCursor(GLUT_CURSOR_NONE); }

/* Init GL */
static void init_gl() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/* main */
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_w, win_h);
    glutCreateWindow("GLUT CAD - Pan with left-drag; Cursor-centered Zoom; Auto units");

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
