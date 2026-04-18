#ifndef APP_H
#define APP_H

/* Shared layout constants */
#define TOOLBAR_HEIGHT   40
#define RULER_THICKNESS  56
#define STATUS_HEIGHT    20

/* ---------- Render module API ---------- */
int  render_init(int win_w, int win_h);
void render_shutdown(void);
void render_resize(int w, int h);
void render_draw(void);

void render_zoom_to_centered(double new_zoom, int animate);
void render_zoom_in_centered(void);
void render_zoom_out_centered(void);
void render_pan_by(int dx, int dy);

void render_set_grid_visible(int on);
void render_toggle_grid(void);

/* ---------- UI module API (callbacks) ---------- */
void ui_init(void);
void ui_on_display(void);
void ui_on_reshape(int w, int h);
void ui_on_mouse(int button, int state, int x, int y);
void ui_on_motion(int x, int y);
void ui_on_passive_motion(int x, int y);
void ui_on_keyboard(unsigned char key, int x, int y);

#endif /* APP_H */
