/*
  lilt_single.c
  Single-file terminal emulator using SDL2 + SDL2_ttf, PTY-backed, minimal ANSI parser.
  - No libtmt, no palette.h
  - Uses embedded palette_raw and bgpalette_raw as requested
  - Build: gcc lilt_single.c -o lilt_single -lSDL2 -lSDL2_ttf
  - Run: ./lilt_single -F /path/to/DejaVuSansMono.ttf -S 14 -d 80x25 -t "Lilt"
*/

#define _GNU_SOURCE
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <pty.h>
#include <sys/wait.h>

/* --- Embedded palette (user-provided) --- */
uint32_t palette_raw[] = {
  0x171717,
  0xa8203d,
  0x3da820,
  0xa88a20,
  0x203da8,
  0x8a20a8,
  0x20a88a,
  0xbdbdbd,
  0x3b3b3b,
  0xeb92a5,
  0xa5eb92,
  0xebd792,
  0x92a5eb,
  0xd792eb,
  0x92ebd7,
  0xf1f1f1,
};
uint32_t * bgpalette_raw = palette_raw;

/* --- Configurable defaults --- */
#define DEFAULT_TITLE "Lilt"
#define DEF_FG 15
#define DEF_BG 0

/* --- Terminal cell attributes and cell type --- */
typedef struct {
  unsigned char fg; /* 1..16 (0 means default) */
  unsigned char bg; /* 1..8 (0 means default) */
  unsigned char bold:1;
  unsigned char underline:1;
  unsigned char blink:1;
  unsigned char reverse:1;
  unsigned char invisible:1;
} CellAttr;

typedef struct {
  unsigned char c; /* byte value 0..255 */
  CellAttr a;
} Cell;

/* --- Globals --- */
static int term_w = 80;
static int term_h = 25;
static int tweakx = 0;
static int tweaky = 0;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *ttf_font = NULL;
static SDL_Texture *glyph_atlas = NULL;

static int glyph_w = 8;
static int glyph_h = 16;
static int glyph_count = 256;

static SDL_Color colors[16];
static SDL_Color bgcolors[8];
static int bg_intensity = 90;
static SDL_Color palcur[2];

static Cell **screen_buf = NULL;
static unsigned char *dirty_lines = NULL;

static int cursor_x = 0;
static int cursor_y = 0;
static int cursor_enabled = 1;
static int cursor_visible = 1;

static int master_fd = -1;
static pid_t child_pid = -1;

static int mouse_mode = -1; /* -1 disabled, 9 X10, 1006 */
static int mouse_motion = 0;

static char *font_path = NULL;
static int font_size = 14;
static char *window_title = DEFAULT_TITLE;

static int cursor_blink_delay = 0;

/* Parser state */
typedef enum { PS_GROUND, PS_ESC, PS_CSI, PS_OSC } ParserState;
static ParserState pstate = PS_GROUND;
static char csi_buf[128];
static int csi_len = 0;
static char osc_buf[512];
static int osc_len = 0;

/* Current attributes for new characters */
static CellAttr current_attr = { .fg = DEF_FG + 1, .bg = DEF_BG + 1, .bold = 0, .underline = 0, .blink = 0, .reverse = 0, .invisible = 0 };

/* --- Utility macros --- */
#define UCLAMP(x) ((x)<=255?(x):255)
#define DARKEN(c) UCLAMP(bg_intensity * ((c) & 0xff) / 100)
static inline int clampi(int v, int a, int b) { return v < a ? a : (v > b ? b : v); }

/* --- Forward declarations --- */
static void init_palette(void);
static int alloc_screen(int cols, int rows);
static void mark_line_dirty(int y);
static void scroll_up(int n);
static void scroll_down(int n);
static void move_cursor_to(int r, int c);
static void clear_screen(int mode);
static void clear_line(int mode);
static void set_sgr(const char *params);
static void put_char_to_cell(unsigned char ch);
static void handle_csi_sequence(const char *seq);
static void handle_osc_sequence(const char *seq);
static void feed_bytes_to_parser(const char *buf, ssize_t len);
static void recreate_glyph_atlas(void);
static void render_dirty_lines(void);
static void send_data_to_pty(const char *s, size_t len);
static int spawn_pty_and_child(char *argv0, char *cshell, char **run_cmd);
static void handle_resize_window(int new_w, int new_h);
static void handle_mouse_event(const SDL_Event *ev);

/* --- Palette init --- */
static void init_palette(void)
{
  for (int i = 0; i < 16; ++i)
  {
    colors[i].b = (palette_raw[i] >>  0) & 0xff;
    colors[i].g = (palette_raw[i] >>  8) & 0xff;
    colors[i].r = (palette_raw[i] >> 16) & 0xff;
    colors[i].a = 255;
  }
  for (int i = 0; i < 8; ++i)
  {
    uint32_t v = bgpalette_raw[i];
    bgcolors[i].b = DARKEN(v >> 0) & 0xff;
    bgcolors[i].g = DARKEN(v >> 8) & 0xff;
    bgcolors[i].r = DARKEN(v >> 16) & 0xff;
    bgcolors[i].a = 255;
  }
  palcur[0] = bgcolors[DEF_BG];
  palcur[1] = colors[DEF_FG];
}

/* --- Screen allocation --- */
static int alloc_screen(int cols, int rows)
{
  if (screen_buf)
  {
    for (int r = 0; r < term_h; ++r) free(screen_buf[r]);
    free(screen_buf);
    free(dirty_lines);
  }
  term_w = cols;
  term_h = rows;
  screen_buf = calloc(term_h, sizeof(Cell*));
  if (!screen_buf) return 0;
  for (int r = 0; r < term_h; ++r)
  {
    screen_buf[r] = calloc(term_w, sizeof(Cell));
    if (!screen_buf[r]) return 0;
  }
  dirty_lines = calloc(term_h, 1);
  if (!dirty_lines) return 0;
  for (int r = 0; r < term_h; ++r)
  {
    for (int c = 0; c < term_w; ++c)
    {
      screen_buf[r][c].c = ' ';
      screen_buf[r][c].a.fg = DEF_FG + 1;
      screen_buf[r][c].a.bg = DEF_BG + 1;
      screen_buf[r][c].a.bold = 0;
      screen_buf[r][c].a.underline = 0;
      screen_buf[r][c].a.blink = 0;
      screen_buf[r][c].a.reverse = 0;
      screen_buf[r][c].a.invisible = 0;
    }
    dirty_lines[r] = 1;
  }
  return 1;
}

static void mark_line_dirty(int y) { if (y >= 0 && y < term_h) dirty_lines[y] = 1; }

/* --- Scrolling --- */
static void scroll_up(int n)
{
  if (n <= 0) return;
  if (n >= term_h)
  {
    for (int r = 0; r < term_h; ++r)
    {
      for (int c = 0; c < term_w; ++c) screen_buf[r][c].c = ' ';
      dirty_lines[r] = 1;
    }
    return;
  }
  for (int r = 0; r < term_h - n; ++r)
  {
    memcpy(screen_buf[r], screen_buf[r + n], term_w * sizeof(Cell));
    dirty_lines[r] = 1;
  }
  for (int r = term_h - n; r < term_h; ++r)
  {
    for (int c = 0; c < term_w; ++c) screen_buf[r][c].c = ' ';
    dirty_lines[r] = 1;
  }
}

static void scroll_down(int n)
{
  if (n <= 0) return;
  if (n >= term_h)
  {
    for (int r = 0; r < term_h; ++r)
    {
      for (int c = 0; c < term_w; ++c) screen_buf[r][c].c = ' ';
      dirty_lines[r] = 1;
    }
    return;
  }
  for (int r = term_h - 1; r >= n; --r)
  {
    memcpy(screen_buf[r], screen_buf[r - n], term_w * sizeof(Cell));
    dirty_lines[r] = 1;
  }
  for (int r = 0; r < n; ++r)
  {
    for (int c = 0; c < term_w; ++c) screen_buf[r][c].c = ' ';
    dirty_lines[r] = 1;
  }
}

/* --- Cursor and clearing --- */
static void move_cursor_to(int r, int c)
{
  if (r <= 0) r = 1;
  if (c <= 0) c = 1;
  cursor_y = clampi(r - 1, 0, term_h - 1);
  cursor_x = clampi(c - 1, 0, term_w - 1);
}

static void clear_screen(int mode)
{
  if (mode == 0)
  {
    for (int r = cursor_y; r < term_h; ++r)
    {
      int start = (r == cursor_y) ? cursor_x : 0;
      for (int c = start; c < term_w; ++c) screen_buf[r][c].c = ' ';
      dirty_lines[r] = 1;
    }
  }
  else if (mode == 1)
  {
    for (int r = 0; r <= cursor_y; ++r)
    {
      int end = (r == cursor_y) ? cursor_x : term_w - 1;
      for (int c = 0; c <= end; ++c) screen_buf[r][c].c = ' ';
      dirty_lines[r] = 1;
    }
  }
  else
  {
    for (int r = 0; r < term_h; ++r)
    {
      for (int c = 0; c < term_w; ++c) screen_buf[r][c].c = ' ';
      dirty_lines[r] = 1;
    }
  }
}

static void clear_line(int mode)
{
  if (mode == 0)
  {
    for (int c = cursor_x; c < term_w; ++c) screen_buf[cursor_y][c].c = ' ';
  }
  else if (mode == 1)
  {
    for (int c = 0; c <= cursor_x; ++c) screen_buf[cursor_y][c].c = ' ';
  }
  else
  {
    for (int c = 0; c < term_w; ++c) screen_buf[cursor_y][c].c = ' ';
  }
  dirty_lines[cursor_y] = 1;
}

/* --- SGR handling (basic) --- */
static void set_sgr(const char *params)
{
  if (!params || params[0] == '\0')
  {
    current_attr.fg = DEF_FG + 1;
    current_attr.bg = DEF_BG + 1;
    current_attr.bold = current_attr.underline = current_attr.blink = current_attr.reverse = current_attr.invisible = 0;
    return;
  }
  char tmp[128];
  strncpy(tmp, params, sizeof(tmp)-1);
  tmp[sizeof(tmp)-1] = 0;
  char *tok = strtok(tmp, ";");
  while (tok)
  {
    int v = atoi(tok);
    if (v == 0)
    {
      current_attr.fg = DEF_FG + 1;
      current_attr.bg = DEF_BG + 1;
      current_attr.bold = current_attr.underline = current_attr.blink = current_attr.reverse = current_attr.invisible = 0;
    }
    else if (v == 1) current_attr.bold = 1;
    else if (v == 4) current_attr.underline = 1;
    else if (v == 5) current_attr.blink = 1;
    else if (v == 7) current_attr.reverse = 1;
    else if (v == 8) current_attr.invisible = 1;
    else if (v >= 30 && v <= 37) current_attr.fg = (v - 30) + 1;
    else if (v >= 40 && v <= 47) current_attr.bg = (v - 40) % 8 + 1;
    else if (v >= 90 && v <= 97) current_attr.fg = (v - 90) + 8 + 1;
    else if (v >= 100 && v <= 107) current_attr.bg = (v - 100) + 8 + 1;
    tok = strtok(NULL, ";");
  }
}

/* --- Put character into buffer --- */
static void put_char_to_cell(unsigned char ch)
{
  if (ch == '\r') { cursor_x = 0; return; }
  if (ch == '\n') { cursor_x = 0; cursor_y++; if (cursor_y >= term_h) { scroll_up(1); cursor_y = term_h - 1; } return; }
  if (ch == '\b') { if (cursor_x > 0) cursor_x--; return; }
  if (ch == '\t') { int next = ((cursor_x / 8) + 1) * 8; if (next >= term_w) next = term_w - 1; cursor_x = next; return; }

  if (cursor_x >= term_w) { cursor_x = 0; cursor_y++; if (cursor_y >= term_h) { scroll_up(1); cursor_y = term_h - 1; } }
  Cell *cell = &screen_buf[cursor_y][cursor_x];
  cell->c = ch;
  cell->a = current_attr;
  dirty_lines[cursor_y] = 1;
  cursor_x++;
  if (cursor_x >= term_w) { cursor_x = 0; cursor_y++; if (cursor_y >= term_h) { scroll_up(1); cursor_y = term_h - 1; } }
}

/* --- CSI and OSC handling (small subset) --- */
static void handle_csi_sequence(const char *seq)
{
  if (!seq || seq[0] == '\0') return;
  int len = strlen(seq);
  char final = seq[len - 1];
  char params[128] = {0};
  if (len > 1) strncpy(params, seq, len - 1);
  params[len - 1] = 0;

  if (final == 'A') { int n = atoi(params); if (n <= 0) n = 1; cursor_y = clampi(cursor_y - n, 0, term_h - 1); }
  else if (final == 'B') { int n = atoi(params); if (n <= 0) n = 1; cursor_y = clampi(cursor_y + n, 0, term_h - 1); }
  else if (final == 'C') { int n = atoi(params); if (n <= 0) n = 1; cursor_x = clampi(cursor_x + n, 0, term_w - 1); }
  else if (final == 'D') { int n = atoi(params); if (n <= 0) n = 1; cursor_x = clampi(cursor_x - n, 0, term_w - 1); }
  else if (final == 'H' || final == 'f') { int r = 1, c = 1; if (params[0] != '\0') sscanf(params, "%d;%d", &r, &c); move_cursor_to(r, c); }
  else if (final == 'J') { int mode = atoi(params); clear_screen(mode); }
  else if (final == 'K') { int mode = atoi(params); clear_line(mode); }
  else if (final == 'm') { set_sgr(params); }
  mark_line_dirty(cursor_y);
}

static void handle_osc_sequence(const char *seq)
{
  if (!seq) return;
  if (strncmp(seq, "0;", 2) == 0 || strncmp(seq, "2;", 2) == 0)
  {
    const char *title = seq + 2;
    if (title && title[0]) SDL_SetWindowTitle(window, title);
  }
}

/* --- Feed bytes into parser --- */
static void feed_bytes_to_parser(const char *buf, ssize_t len)
{
  for (ssize_t i = 0; i < len; ++i)
  {
    unsigned char ch = (unsigned char)buf[i];
    switch (pstate)
    {
      case PS_GROUND:
        if (ch == 0x1b) { pstate = PS_ESC; csi_len = 0; osc_len = 0; }
        else if (ch < 0x20)
        {
          if (ch == '\r') put_char_to_cell('\r');
          else if (ch == '\n') put_char_to_cell('\n');
          else if (ch == '\b') put_char_to_cell('\b');
          else if (ch == '\t') put_char_to_cell('\t');
        }
        else put_char_to_cell(ch);
        break;

      case PS_ESC:
        if (ch == '[') { pstate = PS_CSI; csi_len = 0; csi_buf[0] = 0; }
        else if (ch == ']') { pstate = PS_OSC; osc_len = 0; osc_buf[0] = 0; }
        else pstate = PS_GROUND;
        break;

      case PS_CSI:
        if (csi_len < (int)sizeof(csi_buf)-1) csi_buf[csi_len++] = ch;
        csi_buf[csi_len] = 0;
        if (ch >= 0x40 && ch <= 0x7E) { handle_csi_sequence(csi_buf); pstate = PS_GROUND; }
        break;

      case PS_OSC:
        if (ch == 0x07) { osc_buf[osc_len] = 0; handle_osc_sequence(osc_buf); pstate = PS_GROUND; }
        else if (ch == 0x1b)
        {
          if (osc_len < (int)sizeof(osc_buf)-1) osc_buf[osc_len++] = ch;
        }
        else if (ch == '\\' && osc_len > 0 && osc_buf[osc_len-1] == 0x1b)
        {
          osc_buf[osc_len-1] = 0;
          handle_osc_sequence(osc_buf);
          pstate = PS_GROUND;
        }
        else
        {
          if (osc_len < (int)sizeof(osc_buf)-1) osc_buf[osc_len++] = ch;
        }
        break;
    }
  }
}

/* --- Glyph atlas creation using SDL2_ttf --- */
static void recreate_glyph_atlas(void)
{
  if (glyph_atlas) { SDL_DestroyTexture(glyph_atlas); glyph_atlas = NULL; }

  if (!ttf_font || !renderer) return;

  glyph_h = TTF_FontHeight(ttf_font);
  int maxw = 0;
  for (int ch = 32; ch < 127; ++ch)
  {
    int minx, maxx, advance;
    if (TTF_GlyphMetrics(ttf_font, ch, &minx, &maxx, NULL, NULL, &advance) == 0)
    {
      if (advance > maxw) maxw = advance;
    }
  }
  if (maxw <= 0) maxw = glyph_h / 2;
  glyph_w = maxw;

  int atlas_w = glyph_w;
  int atlas_h = glyph_h * glyph_count;

  SDL_Surface *atlas = SDL_CreateRGBSurfaceWithFormat(0, atlas_w, atlas_h, 32, SDL_PIXELFORMAT_RGBA32);
  if (!atlas) return;
  SDL_FillRect(atlas, NULL, SDL_MapRGBA(atlas->format, 0,0,0,0));

  SDL_Color white = {255,255,255,255};
  for (int cc = 0; cc < glyph_count; ++cc)
  {
    char txt[5] = {0};
    if (cc < 32) txt[0] = ' ';
    else txt[0] = (char)cc;
    SDL_Surface *g = TTF_RenderUTF8_Blended(ttf_font, txt, white);
    if (!g) continue;
    int gx = (glyph_w - g->w) / 2;
    int gy = (glyph_h - g->h) / 2;
    if (gx < 0) gx = 0;
    if (gy < 0) gy = 0;
    SDL_Rect dest = { gx, cc * glyph_h + gy, g->w, g->h };
    SDL_BlitSurface(g, NULL, atlas, &dest);
    SDL_FreeSurface(g);
  }

  glyph_atlas = SDL_CreateTextureFromSurface(renderer, atlas);
  SDL_SetTextureBlendMode(glyph_atlas, SDL_BLENDMODE_BLEND);
  SDL_FreeSurface(atlas);
}

/* --- Rendering --- */
static void render_dirty_lines(void)
{
  if (!renderer || !glyph_atlas) return;

  SDL_SetRenderDrawColor(renderer, bgcolors[DEF_BG].r, bgcolors[DEF_BG].g, bgcolors[DEF_BG].b, 255);
  SDL_RenderClear(renderer);

  for (int r = 0; r < term_h; ++r)
  {
    if (!dirty_lines[r]) continue;
    for (int c = 0; c < term_w; ++c)
    {
      Cell *cell = &screen_buf[r][c];
      int fg = (cell->a.fg ? (cell->a.fg - 1) : DEF_FG);
      int bg = (cell->a.bg ? (cell->a.bg - 1) : DEF_BG);
      SDL_Rect dst = { c * glyph_w + tweakx, r * glyph_h + tweaky, glyph_w, glyph_h };
      SDL_SetRenderDrawColor(renderer, bgcolors[bg].r, bgcolors[bg].g, bgcolors[bg].b, 255);
      SDL_RenderFillRect(renderer, &dst);
      SDL_SetTextureColorMod(glyph_atlas, colors[fg].r, colors[fg].g, colors[fg].b);
      SDL_SetTextureAlphaMod(glyph_atlas, 255);
      unsigned cc = cell->c;
      if (cc >= (unsigned)glyph_count) cc = '?';
      SDL_Rect src = { 0, (int)(cc * glyph_h), glyph_w, glyph_h };
      SDL_RenderCopy(renderer, glyph_atlas, &src, &dst);
      if (cell->a.underline)
      {
        SDL_SetRenderDrawColor(renderer, colors[fg].r, colors[fg].g, colors[fg].b, 255);
        SDL_Rect ur = { dst.x, dst.y + glyph_h - 2, glyph_w, 2 };
        SDL_RenderFillRect(renderer, &ur);
      }
    }
    dirty_lines[r] = 0;
  }

  if (cursor_enabled && cursor_visible)
  {
    int fg = (screen_buf[cursor_y][cursor_x].a.fg ? (screen_buf[cursor_y][cursor_x].a.fg - 1) : DEF_FG);
    SDL_Rect cur = { cursor_x * glyph_w + tweakx, cursor_y * glyph_h + glyph_h - 2 + tweaky, glyph_w, 2 };
    SDL_SetRenderDrawColor(renderer, colors[fg].r, colors[fg].g, colors[fg].b, 255);
    SDL_RenderFillRect(renderer, &cur);
  }

  SDL_RenderPresent(renderer);
}

/* --- PTY I/O --- */
static void send_data_to_pty(const char *s, size_t len)
{
  if (master_fd < 0) return;
  ssize_t rv = write(master_fd, s, len);
  (void)rv;
}

static int spawn_pty_and_child(char *argv0, char *cshell, char **run_cmd)
{
  char *nshell = getenv("SHELL");
  if (nshell && strlen(nshell)) cshell = nshell;
  char **cmd = (run_cmd && run_cmd[0]) ? run_cmd : (char*[]){ cshell ? cshell : "/bin/sh", NULL };

  int master = -1;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);
  if (pid < 0) return -1;
  if (pid == 0)
  {
    execvp(cmd[0], cmd);
    _exit(1);
  }
  master_fd = master;
  child_pid = pid;
  int flags = fcntl(master_fd, F_GETFL, 0);
  fcntl(master_fd, F_SETFL, flags | O_NONBLOCK);
  return 0;
}

/* --- Resize handling --- */
static void handle_resize_window(int new_w, int new_h)
{
  int new_cols = new_w / glyph_w;
  int new_rows = new_h / glyph_h;
  if (new_cols < 5) new_cols = 5;
  if (new_rows < 5) new_rows = 5;
  alloc_screen(new_cols, new_rows);
  tweakx = (new_w % glyph_w) / 2;
  tweaky = (new_h % glyph_h) / 2;
  if (master_fd >= 0)
  {
    struct winsize ws;
    ws.ws_row = term_h;
    ws.ws_col = term_w;
    ws.ws_xpixel = term_w * glyph_w;
    ws.ws_ypixel = term_h * glyph_h;
    ioctl(master_fd, TIOCSWINSZ, &ws);
  }
}

/* --- Read PTY and feed parser --- */
static void read_from_pty_and_process(void)
{
  if (master_fd < 0) return;
  char buf[4096];
  ssize_t rv;
  while ((rv = read(master_fd, buf, sizeof(buf))) > 0)
  {
    feed_bytes_to_parser(buf, rv);
  }
  if (rv == 0)
  {
    int status;
    waitpid(child_pid, &status, WNOHANG);
    master_fd = -1;
  }
}

/* --- Mouse reporting --- */
static void handle_mouse_event(const SDL_Event *ev)
{
  if (mouse_mode == -1) return;
  int x = (ev->motion.x - tweakx) / glyph_w;
  int y = (ev->motion.y - tweaky) / glyph_h;
  if (x < 0) x = 0; if (x >= term_w) x = term_w - 1;
  if (y < 0) y = 0; if (y >= term_h) y = term_h - 1;

  char *buf = NULL;
  if (mouse_mode == 9)
  {
    int button = 0;
    if (ev->type == SDL_MOUSEBUTTONDOWN)
    {
      if (ev->button.button == SDL_BUTTON_LEFT) button = 0;
      else if (ev->button.button == SDL_BUTTON_MIDDLE) button = 1;
      else if (ev->button.button == SDL_BUTTON_RIGHT) button = 2;
    }
    if (asprintf(&buf, "\x1b[M%c%c%c", (char)(button+32), (char)(x+1+32), (char)(y+1+32)) == -1) buf = NULL;
  }
  else if (mouse_mode == 1006)
  {
    int button = 0;
    if (ev->type == SDL_MOUSEBUTTONDOWN)
    {
      if (ev->button.button == SDL_BUTTON_LEFT) button = 0;
      else if (ev->button.button == SDL_BUTTON_MIDDLE) button = 1;
      else if (ev->button.button == SDL_BUTTON_RIGHT) button = 2;
    }
    char c = (ev->type == SDL_MOUSEBUTTONUP) ? 'm' : 'M';
    if (asprintf(&buf, "\x1b[<%d;%d;%d%c", button, x+1, y+1, c) == -1) buf = NULL;
  }
  if (buf) { send_data_to_pty(buf, strlen(buf)); free(buf); }
}

/* --- Main --- */
int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "");

  int opt;
  int opt_e_index = -1;
  int opt_max = 0;
  char *cshell = NULL;
  char **run_cmd = NULL;
  bool resizable = true;

  while ((opt = getopt(argc, argv, "+s:et:d:b:mMB:F:S:")) != -1)
  {
    opt_max = optind;
    switch (opt)
    {
      case 's': /* not used */ break;
      case 'e': opt_e_index = optind; break;
      case 't': window_title = strdup(optarg); break;
      case 'd': { int w=0,h=0; if (sscanf(optarg,"%ix%i",&w,&h)==2){ term_w=w; term_h=h; } break; }
      case 'b': cursor_blink_delay = atoi(optarg); if (cursor_blink_delay < 0) cursor_blink_delay = 0; break;
      case 'm': mouse_mode = 9; break;
      case 'M': mouse_mode = 1006; break;
      case 'F': free(font_path); font_path = strdup(optarg); break;
      case 'S': font_size = atoi(optarg); if (font_size < 6) font_size = 6; break;
      case 'B': bg_intensity = atoi(optarg); break;
    }
    if (opt_e_index != -1) break;
  }

  if (opt_e_index == opt_max) run_cmd = argv + opt_e_index;
  else if (optind == argc - 1) cshell = argv[optind];

  init_palette();

  if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError()); return 1; }
  if (TTF_Init() != 0) { fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError()); SDL_Quit(); return 1; }

  if (!font_path) font_path = strdup("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");

  ttf_font = TTF_OpenFont(font_path, font_size);
  if (!ttf_font) { fprintf(stderr, "TTF_OpenFont failed for %s: %s\n", font_path, TTF_GetError()); TTF_Quit(); SDL_Quit(); return 1; }

  /* Create temporary renderer to compute metrics if needed */
  window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, glyph_w * term_w, glyph_h * term_h, resizable ? SDL_WINDOW_RESIZABLE : 0);
  if (!window) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); TTF_CloseFont(ttf_font); TTF_Quit(); SDL_Quit(); return 1; }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) { fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError()); SDL_DestroyWindow(window); TTF_CloseFont(ttf_font); TTF_Quit(); SDL_Quit(); return 1; }

  recreate_glyph_atlas();

  /* Set initial window size based on glyph metrics */
  int win_w = glyph_w * term_w;
  int win_h = glyph_h * term_h;
  SDL_SetWindowSize(window, win_w, win_h);

  alloc_screen(term_w, term_h);

  if (spawn_pty_and_child(argv[0], cshell, run_cmd) != 0) { fprintf(stderr, "Failed to spawn child\n"); return 2; }

  SDL_StartTextInput();

  bool quitting = false;
  uint32_t last_blink = SDL_GetTicks();
  bool blink_state = true;

  const int max_sdl_delay = 100;
  const int sdl_boost_time = 600;
  bool sdl_boosting = false;
  uint32_t sdl_boost_start = 0;

  const int mouse_active_delay = 300;
  uint32_t mouse_active_at = 0;
  bool mouse_active = false;

  while (!quitting)
  {
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
      sdl_boost_start = SDL_GetTicks();
      sdl_boosting = true;

      if (ev.type == SDL_QUIT) { quitting = true; break; }
      else if (ev.type == SDL_KEYDOWN)
      {
        bool ctrl = (ev.key.keysym.mod & KMOD_CTRL);
        char buf[8] = {0};
        const char *seq = NULL;
        switch (ev.key.keysym.sym)
        {
          case SDLK_UP: seq = "\x1b[A"; break;
          case SDLK_DOWN: seq = "\x1b[B"; break;
          case SDLK_RIGHT: seq = "\x1b[C"; break;
          case SDLK_LEFT: seq = "\x1b[D"; break;
          case SDLK_HOME: seq = "\x1b[H"; break;
          case SDLK_END: seq = "\x1b[F"; break;
          case SDLK_BACKSPACE: seq = "\x7f"; break;
          case SDLK_RETURN: seq = "\r"; break;
          case SDLK_TAB: seq = "\t"; break;
          case SDLK_ESCAPE: seq = "\x1b"; break;
          default:
            if (ctrl && ev.key.keysym.sym >= SDLK_a && ev.key.keysym.sym <= SDLK_z)
            {
              char c = (char)(ev.key.keysym.sym - SDLK_a + 1);
              buf[0] = c;
              send_data_to_pty(buf, 1);
            }
            break;
        }
        if (seq) send_data_to_pty(seq, strlen(seq));
      }
      else if (ev.type == SDL_TEXTINPUT)
      {
        send_data_to_pty(ev.text.text, strlen(ev.text.text));
      }
      else if (ev.type == SDL_WINDOWEVENT)
      {
        if (ev.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) mouse_active_at = SDL_GetTicks() + mouse_active_delay;
        else if (ev.window.event == SDL_WINDOWEVENT_EXPOSED)
        {
          for (int r = 0; r < term_h; ++r) dirty_lines[r] = 1;
          render_dirty_lines();
        }
        else if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
        {
          handle_resize_window(ev.window.data1, ev.window.data2);
          for (int r = 0; r < term_h; ++r) dirty_lines[r] = 1;
        }
      }
      else if (ev.type == SDL_MOUSEMOTION)
      {
        if (mouse_active) handle_mouse_event(&ev);
      }
      else if (ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP)
      {
        if (mouse_active) handle_mouse_event(&ev);
      }
    }

    if (quitting) break;

    uint32_t now = SDL_GetTicks();
    if (!mouse_active && (mouse_active_at < now)) mouse_active = true;

    if (cursor_blink_delay && (now - last_blink >= (uint32_t)cursor_blink_delay))
    {
      last_blink = now;
      blink_state = !blink_state;
      cursor_visible = blink_state;
      mark_line_dirty(cursor_y);
      render_dirty_lines();
    }

    read_from_pty_and_process();

    bool any_dirty = false;
    for (int r = 0; r < term_h; ++r) if (dirty_lines[r]) { any_dirty = true; break; }
    if (any_dirty) render_dirty_lines();

    int delay = max_sdl_delay;
    if (sdl_boosting)
    {
      int boosting = (int)(SDL_GetTicks() - sdl_boost_start);
      if (boosting > sdl_boost_time) sdl_boosting = false;
      else
      {
        delay = boosting << 2;
        if (delay < 10) delay = 10;
        else if (delay > max_sdl_delay) delay = max_sdl_delay;
      }
    }
    if (cursor_blink_delay && (delay > (int)(last_blink + cursor_blink_delay - now))) delay = (int)(last_blink + cursor_blink_delay - now);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = delay * 1000;
    fd_set rfds;
    FD_ZERO(&rfds);
    if (master_fd >= 0) FD_SET(master_fd, &rfds);
    int nfds = (master_fd >= 0) ? master_fd + 1 : 0;
    int rv = select(nfds, &rfds, NULL, NULL, &tv);
    if (rv == -1)
    {
      if (errno == EINTR) { quitting = true; break; }
    }
  }

  SDL_StopTextInput();
  if (glyph_atlas) SDL_DestroyTexture(glyph_atlas);
  if (ttf_font) TTF_CloseFont(ttf_font);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();

  if (screen_buf)
  {
    for (int r = 0; r < term_h; ++r) free(screen_buf[r]);
    free(screen_buf);
    free(dirty_lines);
  }
  free(font_path);
  return 0;
}
