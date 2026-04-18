/*
 * render.c
 *
 * Rendering subsystem: GLSL150 grid shader, content viewport calculations,
 * zoom/pan, CPU fallback.
 */

#include "app.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* Window size */
static int win_w = 1200;
static int win_h = 800;

/* World <-> pixel */
static double px_per_mm   = 96.0 / 25.4;
static double zoom_factor = 1.0;
static double pan_x = 0.0, pan_y = 0.0;

/* Grid */
static int grid_visible = 1;

/* Shader + VAO */
static GLuint grid_prog = 0;
static GLuint fs_vao    = 0;

/* Uniform locations */
static GLint uni_viewport_origin = -1;
static GLint uni_viewport_size   = -1;
static GLint uni_pan             = -1;
static GLint uni_zoom            = -1;
static GLint uni_px_per_mm       = -1;
static GLint uni_step_mm         = -1;
static GLint uni_major_units     = -1;
static GLint uni_grid_visible    = -1;
static GLint uni_color_bg        = -1;
static GLint uni_color_minor     = -1;
static GLint uni_color_major     = -1;

/* Zoom animation */
static int    zoom_animating   = 0;
static double zoom_start       = 1.0;
static double zoom_target      = 1.0;
static double anim_start_time  = 0.0;
static double anim_duration    = 0.20;

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}
static double ease_out_quad(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    return 1.0 - (1.0 - t) * (1.0 - t);
}
static void ensure_zoom_safe(void) {
    if (zoom_factor < 1e-9) zoom_factor = 1e-9;
    if (zoom_factor > 1e12) zoom_factor = 1e12;
}

/* Shader helpers */
static GLuint compile_shader_safe(GLenum type, const char *src) {
    if (!src) return 0;
    GLuint s = glCreateShader(type);
    if (!s) return 0;
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char *log = (char*)malloc(len);
            glGetShaderInfoLog(s, len, NULL, log);
            fprintf(stderr, "Shader compile error:\n%s\n", log);
            free(log);
        }
        glDeleteShader(s);
        return 0;
    }
    return s;
}
static GLuint link_program_safe(GLuint vs, GLuint fs) {
    if (!vs || !fs) return 0;
    GLuint p = glCreateProgram();
    if (!p) return 0;
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char *log = (char*)malloc(len);
            glGetProgramInfoLog(p, len, NULL, log);
            fprintf(stderr, "Program link error:\n%s\n", log);
            free(log);
        }
        glDeleteProgram(p);
        return 0;
    }
    return p;
}

/* GLSL 150 shaders */
static const char *vs_src =
"#version 150\n"
"void main() {\n"
"    vec2 pos[3] = vec2[3](vec2(-1.0,-1.0), vec2(3.0,-1.0), vec2(-1.0,3.0));\n"
"    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);\n"
"}\n";

static const char *fs_src =
"#version 150\n"
"uniform vec2 u_viewport_origin;\n"
"uniform vec2 u_viewport_size;\n"
"uniform vec2 u_pan;\n"
"uniform float u_zoom;\n"
"uniform float u_px_per_mm;\n"
"uniform float u_step_mm;\n"
"uniform float u_major_units;\n"
"uniform int u_grid_visible;\n"
"uniform vec3 u_color_bg;\n"
"uniform vec3 u_color_minor;\n"
"uniform vec3 u_color_major;\n"
"out vec4 outColor;\n"
"float aa(float d, float w) { return smoothstep(w, 0.0, d); }\n"
"void main() {\n"
"  if (u_grid_visible == 0) { outColor = vec4(u_color_bg, 1.0); return; }\n"
"  vec2 sc = gl_FragCoord.xy;\n"
"  vec2 local = sc - u_viewport_origin;\n"
"  vec2 center = u_viewport_size * 0.5;\n"
"  vec2 world = (local - center) / u_zoom + u_pan;\n"
"  float fx = mod(world.x, u_step_mm) / u_step_mm;\n"
"  float fy = mod(world.y, u_step_mm) / u_step_mm;\n"
"  float dx = min(abs(fx), 1.0 - abs(fx)) * u_step_mm * u_zoom;\n"
"  float dy = min(abs(fy), 1.0 - abs(fy)) * u_step_mm * u_zoom;\n"
"  float units_x = world.x / u_step_mm;\n"
"  float units_y = world.y / u_step_mm;\n"
"  float rx = mod(abs(units_x), u_major_units);\n"
"  float ry = mod(abs(units_y), u_major_units);\n"
"  int is_major_x = (rx < 1e-3 || abs(rx - u_major_units) < 1e-3) ? 1 : 0;\n"
"  int is_major_y = (ry < 1e-3 || abs(ry - u_major_units) < 1e-3) ? 1 : 0;\n"
"  float a_x = aa(dx, 1.0);\n"
"  float a_y = aa(dy, 1.0);\n"
"  vec3 col = u_color_bg;\n"
"  if (a_x > 0.0) { if (is_major_x == 1) col = mix(col, u_color_major, a_x); else col = mix(col, u_color_minor, a_x); }\n"
"  if (a_y > 0.0) { if (is_major_y == 1) col = mix(col, u_color_major, a_y); else col = mix(col, u_color_minor, a_y); }\n"
"  outColor = vec4(col, 1.0);\n"
"}\n";

/* init shader + vao */
static int init_shader_and_vao(void) {
    GLuint vs = compile_shader_safe(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compile_shader_safe(GL_FRAGMENT_SHADER, fs_src);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return 0;
    }
    grid_prog = link_program_safe(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (!grid_prog) return 0;

    uni_viewport_origin = glGetUniformLocation(grid_prog, "u_viewport_origin");
    uni_viewport_size   = glGetUniformLocation(grid_prog, "u_viewport_size");
    uni_pan             = glGetUniformLocation(grid_prog, "u_pan");
    uni_zoom            = glGetUniformLocation(grid_prog, "u_zoom");
    uni_px_per_mm       = glGetUniformLocation(grid_prog, "u_px_per_mm");
    uni_step_mm         = glGetUniformLocation(grid_prog, "u_step_mm");
    uni_major_units     = glGetUniformLocation(grid_prog, "u_major_units");
    uni_grid_visible    = glGetUniformLocation(grid_prog, "u_grid_visible");
    uni_color_bg        = glGetUniformLocation(grid_prog, "u_color_bg");
    uni_color_minor     = glGetUniformLocation(grid_prog, "u_color_minor");
    uni_color_major     = glGetUniformLocation(grid_prog, "u_color_major");

    glGenVertexArrays(1, &fs_vao);
    glBindVertexArray(fs_vao);
    glBindVertexArray(0);

    fprintf(stderr, "render: shader program %u ready; uniforms: vp_origin=%d vp_size=%d pan=%d zoom=%d grid=%d\n",
            (unsigned)grid_prog, uni_viewport_origin, uni_viewport_size, uni_pan, uni_zoom, uni_grid_visible);
    return 1;
}

/* content rect bottom-left */
static void get_content_rect_bottom(int *out_x, int *out_y, int *out_w, int *out_h) {
    int cx = RULER_THICKNESS;
    int cy_top = TOOLBAR_HEIGHT + RULER_THICKNESS;
    int ch = win_h - STATUS_HEIGHT - cy_top;
    int cy_bottom = STATUS_HEIGHT;
    int cw = win_w - RULER_THICKNESS;
    if (out_x) *out_x = cx;
    if (out_y) *out_y = cy_bottom;
    if (out_w) *out_w = cw;
    if (out_h) *out_h = ch;
}

/* Public API */

int render_init(int w, int h) {
    win_w = w; win_h = h;
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!init_shader_and_vao()) {
        fprintf(stderr, "render_init: GPU shader not available, CPU fallback will be used.\n");
        grid_prog = 0;
    }
    grid_visible = 1;
    zoom_factor = 1.0;
    pan_x = 0.0; pan_y = 0.0;
    return 1;
}

void render_shutdown(void) {
    if (grid_prog) { glDeleteProgram(grid_prog); grid_prog = 0; }
    if (fs_vao) { glDeleteVertexArrays(1, &fs_vao); fs_vao = 0; }
}

void render_resize(int w, int h) {
    win_w = w; win_h = h;
    glViewport(0, 0, w, h);
}

void render_draw(void) {
    int vx, vy, vw, vh;
    get_content_rect_bottom(&vx, &vy, &vw, &vh);

    if (grid_prog && grid_visible) {
        glUseProgram(grid_prog);
        if (uni_viewport_origin >= 0) glUniform2f(uni_viewport_origin, (GLfloat)vx, (GLfloat)vy);
        if (uni_viewport_size   >= 0) glUniform2f(uni_viewport_size,   (GLfloat)vw, (GLfloat)vh);
        if (uni_pan             >= 0) glUniform2f(uni_pan,             (GLfloat)pan_x, (GLfloat)pan_y);
        if (uni_zoom            >= 0) glUniform1f(uni_zoom,            (GLfloat)zoom_factor);
        if (uni_px_per_mm       >= 0) glUniform1f(uni_px_per_mm,       (GLfloat)px_per_mm);

        float step_mm = 10.0f;
        float major_units = 10.0f;
        if (uni_step_mm     >= 0) glUniform1f(uni_step_mm,     step_mm);
        if (uni_major_units >= 0) glUniform1f(uni_major_units, major_units);

        if (uni_grid_visible >= 0) glUniform1i(uni_grid_visible, grid_visible ? 1 : 0);
        if (uni_color_bg    >= 0) glUniform3f(uni_color_bg,    1.0f, 1.0f, 1.0f);
        if (uni_color_minor >= 0) glUniform3f(uni_color_minor, 0.88f, 0.88f, 0.88f);
        if (uni_color_major >= 0) glUniform3f(uni_color_major, 0.75f, 0.75f, 0.75f);

        glEnable(GL_SCISSOR_TEST);
        glScissor(vx, vy, vw, vh);

        glBindVertexArray(fs_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glDisable(GL_SCISSOR_TEST);
        glUseProgram(0);
    } else {
        /* CPU fallback */
        int vx2 = vx, vy2 = vy, vw2 = vw, vh2 = vh;
        glEnable(GL_SCISSOR_TEST);
        glScissor(vx2, vy2, vw2, vh2);

        glLineWidth(1.0f);
        glColor3ub(224,224,224);

        double step_mm = 10.0;
        double px_per_unit = px_per_mm * zoom_factor;
        if (px_per_unit * step_mm < 20.0) step_mm = 20.0;
        if (px_per_unit * step_mm > 200.0) step_mm = 5.0;

        double center_x = vx2 + vw2 * 0.5;
        for (int k = -500; k <= 500; ++k) {
            double wx = k * step_mm;
            double sx = center_x + (wx - pan_x) * zoom_factor;
            if (sx < vx2 - 2 || sx > vx2 + vw2 + 2) continue;
            glBegin(GL_LINES);
              glVertex2f((GLfloat)sx, (GLfloat)vy2);
              glVertex2f((GLfloat)sx, (GLfloat)(vy2 + vh2));
            glEnd();
        }
        double center_y = vy2 + vh2 * 0.5;
        for (int k = -500; k <= 500; ++k) {
            double wy = k * step_mm;
            double sy = center_y + (wy - pan_y) * zoom_factor;
            if (sy < vy2 - 2 || sy > vy2 + vh2 + 2) continue;
            glBegin(GL_LINES);
              glVertex2f((GLfloat)vx2, (GLfloat)sy);
              glVertex2f((GLfloat)(vx2 + vw2), (GLfloat)sy);
            glEnd();
        }

        glDisable(GL_SCISSOR_TEST);
    }
}

/* Zoom animation timer */
static void timer_handler_zoom(int value) {
    (void)value;
    if (!zoom_animating) return;
    double t = (now_seconds() - anim_start_time) / anim_duration;
    if (t >= 1.0) {
        zoom_factor = zoom_target;
        zoom_animating = 0;
        ensure_zoom_safe();
        glutPostRedisplay();
        return;
    }
    double e = ease_out_quad(t);
    zoom_factor = zoom_start + (zoom_target - zoom_start) * e;
    ensure_zoom_safe();
    glutPostRedisplay();
    glutTimerFunc(16, timer_handler_zoom, 0);
}

void render_zoom_to_centered(double new_zoom, int animate) {
    if (!(new_zoom > 0.0)) return;
    if (new_zoom < 1e-9) new_zoom = 1e-9;
    if (new_zoom > 1e12) new_zoom = 1e12;

    if (!animate) {
        zoom_factor = new_zoom;
        ensure_zoom_safe();
        glutPostRedisplay();
        return;
    }

    zoom_start = zoom_factor;
    zoom_target = new_zoom;
    anim_start_time = now_seconds();
    zoom_animating = 1;
    glutTimerFunc(16, timer_handler_zoom, 0);
}
void render_zoom_in_centered(void) { render_zoom_to_centered(zoom_factor * 1.15, 1); }
void render_zoom_out_centered(void) { render_zoom_to_centered(zoom_factor / 1.15, 1); }

/* Pan by screen pixels. dx positive -> mouse moved right; dy positive -> mouse moved down.
   Convert to world pan: pan_x -= dx/zoom; pan_y -= dy/zoom (screen Y down -> world Y up). */
void render_pan_by(int dx, int dy) {
    pan_x -= (double)dx / zoom_factor;
    pan_y -= (double)dy / zoom_factor;
    glutPostRedisplay();
}

void render_set_grid_visible(int on) {
    grid_visible = (on != 0);
    fprintf(stderr, "render: grid_visible = %d\n", grid_visible);
    glutPostRedisplay();
}
void render_toggle_grid(void) { render_set_grid_visible(!grid_visible); }
