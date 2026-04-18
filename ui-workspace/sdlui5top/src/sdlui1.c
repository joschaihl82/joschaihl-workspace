// editor_big2.c
// gcc -O2 -o editor_big2 editor_big2.c -lSDL2 -lSDL2_ttf
// Doppelt so groß wie vorher: Schrift 36px, größere Fenster, dickere Scrollbar, breiter Caret.
// Weißer Hintergrund, schwarze Schrift, vertikale Scrollbar, Caret per Maus, blinkend.
// UTF-8: codepoint-orientiert (nicht vollständige grapheme-cluster-Erkennung).

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define WINW 2048
#define WINH 1536
#define FONT_SIZE 36           // doppelte Schriftgröße (18 -> 36)
#define MARGIN 16              // doppelte Margin (8 -> 16)
#define LINE_SPACING 8         // doppelte Zeilenabstand (4 -> 8)
#define SCROLLBAR_THICK 32     // dickere Scrollbar (16 -> 32)
#define CURSOR_W 4             // breitere Caret (2 -> 4)
#define BLINK_MS 500

typedef struct {
    char *text;
    int len;
    int cap;
    SDL_Texture *tex;
    int tex_w, tex_h;
    int dirty;
} Line;

typedef struct {
    Line *a;
    int n, cap;
} Doc;

static void doc_init(Doc *d) {
    d->cap = 128;
    d->n = 1;
    d->a = calloc(d->cap, sizeof(Line));
    d->a[0].cap = 256;
    d->a[0].text = calloc(d->a[0].cap, 1);
    d->a[0].len = 0;
    d->a[0].tex = NULL;
    d->a[0].dirty = 1;
}
static void doc_free(Doc *d) {
    for (int i = 0; i < d->n; ++i) {
        free(d->a[i].text);
        if (d->a[i].tex) SDL_DestroyTexture(d->a[i].tex);
    }
    free(d->a);
}
static void ensure_cap(Line *ln, int need) {
    if (ln->cap >= need) return;
    while (ln->cap < need) ln->cap *= 2;
    ln->text = realloc(ln->text, ln->cap);
}
static void mark_dirty(Line *ln) { ln->dirty = 1; }
static void insert_text(Line *ln, int pos, const char *s) {
    int add = (int)strlen(s);
    ensure_cap(ln, ln->len + add + 1);
    memmove(ln->text + pos + add, ln->text + pos, ln->len - pos + 1);
    memcpy(ln->text + pos, s, add);
    ln->len += add;
    mark_dirty(ln);
}
static void delete_back(Line *ln, int pos) {
    if (pos <= 0) return;
    memmove(ln->text + pos - 1, ln->text + pos, ln->len - pos + 1);
    ln->len -= 1;
    mark_dirty(ln);
}
static void doc_insert_line(Doc *d, int idx) {
    if (d->n + 1 > d->cap) { d->cap *= 2; d->a = realloc(d->a, d->cap * sizeof(Line)); }
    for (int i = d->n; i > idx; --i) d->a[i] = d->a[i-1];
    d->a[idx].cap = 128;
    d->a[idx].text = calloc(d->a[idx].cap, 1);
    d->a[idx].len = 0;
    d->a[idx].tex = NULL;
    d->a[idx].dirty = 1;
    d->n++;
}
static void doc_split_line(Doc *d, int idx, int pos) {
    if (d->n + 1 > d->cap) { d->cap *= 2; d->a = realloc(d->a, d->cap * sizeof(Line)); }
    for (int i = d->n; i > idx + 1; --i) d->a[i] = d->a[i-1];
    Line *cur = &d->a[idx];
    Line *nxt = &d->a[idx+1];
    int rest = cur->len - pos;
    nxt->cap = rest + 8;
    nxt->text = malloc(nxt->cap);
    memcpy(nxt->text, cur->text + pos, rest);
    nxt->text[rest] = '\0';
    nxt->len = rest;
    nxt->tex = NULL; nxt->dirty = 1;
    cur->text[pos] = '\0';
    cur->len = pos;
    cur->dirty = 1;
    d->n++;
}
static void doc_join_line(Doc *d, int idx) {
    if (idx + 1 >= d->n) return;
    Line *a = &d->a[idx], *b = &d->a[idx+1];
    ensure_cap(a, a->len + b->len + 1);
    memcpy(a->text + a->len, b->text, b->len + 1);
    a->len += b->len;
    a->dirty = 1;
    if (b->tex) { SDL_DestroyTexture(b->tex); b->tex = NULL; }
    free(b->text);
    for (int i = idx + 1; i < d->n - 1; ++i) d->a[i] = d->a[i+1];
    d->n--;
}

// UTF-8 helper: length of next codepoint by leading byte
static int utf8_char_len(unsigned char lead) {
    if ((lead & 0x80) == 0x00) return 1;
    if ((lead & 0xE0) == 0xC0) return 2;
    if ((lead & 0xF0) == 0xE0) return 3;
    if ((lead & 0xF8) == 0xF0) return 4;
    return 1;
}

// Measure width of prefix (bytes = prefix_len)
static int measure_prefix_width(TTF_Font *font, const char *s, int prefix_len) {
    if (prefix_len <= 0) return 0;
    char *tmp = malloc(prefix_len + 1);
    memcpy(tmp, s, prefix_len);
    tmp[prefix_len] = '\0';
    int w = 0, h = 0;
    TTF_SizeUTF8(font, tmp, &w, &h);
    free(tmp);
    return w;
}

// Find byte offset for target_x using binary search over codepoint boundaries
static int find_offset_binary(TTF_Font *font, const char *s, int byte_len, int target_x) {
    if (byte_len <= 0) return 0;
    int cap = 64, cnt = 0;
    int *offs = malloc(cap * sizeof(int));
    int p = 0;
    while (p < byte_len) {
        if (cnt + 1 >= cap) { cap *= 2; offs = realloc(offs, cap * sizeof(int)); }
        offs[cnt++] = p;
        int step = utf8_char_len((unsigned char)s[p]);
        p += step;
    }
    if (cnt + 1 >= cap) { cap *= 2; offs = realloc(offs, cap * sizeof(int)); }
    offs[cnt++] = byte_len;
    int lo = 0, hi = cnt - 1;
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        int w = measure_prefix_width(font, s, offs[mid]);
        if (w < target_x) lo = mid + 1;
        else hi = mid;
    }
    int result = offs[lo];
    int w_lo = measure_prefix_width(font, s, result);
    if (result > 0) {
        int prev = offs[lo > 0 ? lo - 1 : 0];
        int w_prev = measure_prefix_width(font, s, prev);
        if (abs(w_prev - target_x) <= abs(w_lo - target_x)) result = prev;
    }
    free(offs);
    return result;
}

// Render a single line into texture if dirty
static void render_line_if_needed(Line *ln, SDL_Renderer *ren, TTF_Font *font, SDL_Color color) {
    if (!ln->dirty) return;
    if (ln->tex) { SDL_DestroyTexture(ln->tex); ln->tex = NULL; }
    const char *txt = ln->len ? ln->text : " ";
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, txt, color);
    if (!surf) return;
    ln->tex = SDL_CreateTextureFromSurface(ren, surf);
    ln->tex_w = surf->w; ln->tex_h = surf->h;
    SDL_FreeSurface(surf);
    ln->dirty = 0;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    if (TTF_Init() != 0) { fprintf(stderr, "TTF_Init: %s\n", TTF_GetError()); SDL_Quit(); return 1; }

    SDL_Window *win = SDL_CreateWindow("Editor Big 2x", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH, SDL_WINDOW_RESIZABLE);
    if (!win) { fprintf(stderr, "CreateWindow: %s\n", SDL_GetError()); TTF_Quit(); SDL_Quit(); return 1; }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) { fprintf(stderr, "CreateRenderer: %s\n", SDL_GetError()); SDL_DestroyWindow(win); TTF_Quit(); SDL_Quit(); return 1; }

    const char *font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    TTF_Font *font = TTF_OpenFont(font_path, FONT_SIZE);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont failed for %s: %s\n", font_path, TTF_GetError());
        SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); TTF_Quit(); SDL_Quit();
        return 1;
    }

    Doc doc; doc_init(&doc);
    int cx = 0, cy = 0;

    SDL_StartTextInput();
    int running = 1;
    Uint32 last_blink = SDL_GetTicks();
    int show_cursor = 1;

    int scroll_y = 0;
    int dragging_thumb = 0;
    int thumb_drag_offset = 0;

    SDL_Color bg = {255,255,255,255};
    SDL_Color fg = {0,0,0,255};
    SDL_Color sb_bg = {240,240,240,255};
    SDL_Color sb_thumb = {120,120,120,255};

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = 0; break; }
            else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                // viewport recalculated each frame
            }
            else if (e.type == SDL_KEYDOWN) {
                SDL_Keycode k = e.key.keysym.sym;
                if (k == SDLK_ESCAPE) { running = 0; break; }
                else if (k == SDLK_BACKSPACE) {
                    if (cx > 0) { delete_back(&doc.a[cy], cx); cx--; }
                    else if (cy > 0) { int prevlen = doc.a[cy-1].len; doc_join_line(&doc, cy-1); cy--; cx = prevlen; }
                } else if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
                    doc_split_line(&doc, cy, cx);
                    cy++; cx = 0;
                } else if (k == SDLK_LEFT) {
                    if (cx > 0) cx--;
                    else if (cy > 0) { cy--; cx = doc.a[cy].len; }
                } else if (k == SDLK_RIGHT) {
                    if (cx < doc.a[cy].len) cx++;
                    else if (cy < doc.n - 1) { cy++; cx = 0; }
                } else if (k == SDLK_UP) {
                    if (cy > 0) { cy--; if (cx > doc.a[cy].len) cx = doc.a[cy].len; }
                } else if (k == SDLK_DOWN) {
                    if (cy < doc.n - 1) { cy++; if (cx > doc.a[cy].len) cx = doc.a[cy].len; }
                }
            }
            else if (e.type == SDL_TEXTINPUT) {
                insert_text(&doc.a[cy], cx, e.text.text);
                cx += (int)strlen(e.text.text);
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                scroll_y -= e.wheel.y * (FONT_SIZE + LINE_SPACING);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                int win_w, win_h; SDL_GetWindowSize(win, &win_w, &win_h);
                int vbg_x = win_w - SCROLLBAR_THICK;
                int vbg_y = MARGIN;
                int vbg_h = win_h - SCROLLBAR_THICK - MARGIN;
                if (mx >= vbg_x && my >= vbg_y && my <= vbg_y + vbg_h) {
                    int view_h = win_h - SCROLLBAR_THICK - 2 * MARGIN;
                    int content_h = doc.n * (FONT_SIZE + LINE_SPACING);
                    float vh = (float)vbg_h;
                    float thumb_h = (view_h >= content_h) ? vh : (vh * ((float)view_h / (float)content_h));
                    if (thumb_h < 30) thumb_h = 30;
                    float pos = (content_h <= view_h) ? 0.0f : ((float)scroll_y / (float)(content_h - view_h));
                    int thumb_y = vbg_y + 2 + (int)(pos * (vh - thumb_h - 4));
                    int thumb_h_i = (int)thumb_h - 4;
                    if (my >= thumb_y && my <= thumb_y + thumb_h_i) {
                        dragging_thumb = 1;
                        thumb_drag_offset = my - thumb_y;
                    } else {
                        if (my < thumb_y) scroll_y -= view_h;
                        else scroll_y += view_h;
                    }
                } else {
                    int click_x = mx - MARGIN;
                    int click_y = my + scroll_y - MARGIN;
                    int line_h = FONT_SIZE + LINE_SPACING;
                    int clicked_line = click_y / line_h;
                    if (clicked_line < 0) clicked_line = 0;
                    if (clicked_line >= doc.n) clicked_line = doc.n - 1;
                    cy = clicked_line;
                    int byte_len = doc.a[cy].len;
                    if (byte_len <= 0) { cx = 0; }
                    else {
                        int target_x = click_x;
                        if (target_x < 0) target_x = 0;
                        cx = find_offset_binary(font, doc.a[cy].text, byte_len, target_x);
                        if (cx < 0) cx = 0;
                        if (cx > byte_len) cx = byte_len;
                    }
                    int caret_ypix = cy * (FONT_SIZE + LINE_SPACING);
                    int view_h = win_h - SCROLLBAR_THICK - 2 * MARGIN;
                    if (caret_ypix - scroll_y < 0) scroll_y = caret_ypix - 20;
                    if (caret_ypix - scroll_y > view_h - (FONT_SIZE + LINE_SPACING) + 20) scroll_y = caret_ypix - (view_h - (FONT_SIZE + LINE_SPACING) + 20);
                    if (scroll_y < 0) scroll_y = 0;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                dragging_thumb = 0;
            }
            else if (e.type == SDL_MOUSEMOTION) {
                if (dragging_thumb) {
                    int my = e.motion.y;
                    int win_w, win_h; SDL_GetWindowSize(win, &win_w, &win_h);
                    int vbg_y = MARGIN;
                    int vbg_h = win_h - SCROLLBAR_THICK - MARGIN;
                    int view_h = win_h - SCROLLBAR_THICK - 2 * MARGIN;
                    int content_h = doc.n * (FONT_SIZE + LINE_SPACING);
                    float vh = (float)vbg_h;
                    float thumb_h = (view_h >= content_h) ? vh : (vh * ((float)view_h / (float)content_h));
                    if (thumb_h < 30) thumb_h = 30;
                    float max_pos = vh - thumb_h - 4;
                    int thumb_y = my - thumb_drag_offset;
                    if (thumb_y < vbg_y + 2) thumb_y = vbg_y + 2;
                    if (thumb_y > vbg_y + 2 + (int)max_pos) thumb_y = vbg_y + 2 + (int)max_pos;
                    float pos = (float)(thumb_y - (vbg_y + 2)) / max_pos;
                    if (pos < 0) pos = 0; if (pos > 1) pos = 1;
                    if (content_h > view_h) scroll_y = (int)(pos * (content_h - view_h));
                    else scroll_y = 0;
                }
            }
        } // events

        int win_w, win_h; SDL_GetWindowSize(win, &win_w, &win_h);
        int view_w = win_w - SCROLLBAR_THICK - 2 * MARGIN;
        int view_h = win_h - SCROLLBAR_THICK - 2 * MARGIN;
        if (view_w < 10) view_w = 10;
        if (view_h < 10) view_h = 10;
        int content_h = doc.n * (FONT_SIZE + LINE_SPACING);

        if (scroll_y < 0) scroll_y = 0;
        if (content_h > view_h && scroll_y > content_h - view_h) scroll_y = content_h - view_h;
        if (content_h <= view_h) scroll_y = 0;

        Uint32 now = SDL_GetTicks();
        if (now - last_blink >= BLINK_MS) { show_cursor = !show_cursor; last_blink = now; }

        SDL_SetRenderDrawColor(ren, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(ren);

        int y = MARGIN - scroll_y;
        SDL_Rect clip = { MARGIN, MARGIN, view_w, view_h };
        SDL_RenderSetClipRect(ren, &clip);
        for (int i = 0; i < doc.n; ++i) {
            Line *ln = &doc.a[i];
            render_line_if_needed(ln, ren, font, fg);
            if (ln->tex) {
                SDL_Rect dst = { MARGIN, y, ln->tex_w, ln->tex_h };
                SDL_RenderCopy(ren, ln->tex, NULL, &dst);
            }
            if (i == cy && show_cursor) {
                int cxpix = 0;
                if (cx > 0) cxpix = measure_prefix_width(font, doc.a[i].text, cx);
                else cxpix = 0;
                SDL_Rect caret = { MARGIN + cxpix, y, CURSOR_W, FONT_SIZE };
                SDL_SetRenderDrawColor(ren, fg.r, fg.g, fg.b, fg.a);
                SDL_RenderFillRect(ren, &caret);
            }
            y += FONT_SIZE + LINE_SPACING;
            if (y > win_h) break;
        }
        SDL_RenderSetClipRect(ren, NULL);

        SDL_Rect vbg = { win_w - SCROLLBAR_THICK, MARGIN, SCROLLBAR_THICK, win_h - SCROLLBAR_THICK - MARGIN };
        SDL_SetRenderDrawColor(ren, sb_bg.r, sb_bg.g, sb_bg.b, sb_bg.a);
        SDL_RenderFillRect(ren, &vbg);

        if (content_h > 0 && view_h > 0) {
            float vh = (float)vbg.h;
            float thumb_h = (view_h >= content_h) ? vh : (vh * ((float)view_h / (float)content_h));
            if (thumb_h < 30) thumb_h = 30;
            float pos = (content_h <= view_h) ? 0.0f : ((float)scroll_y / (float)(content_h - view_h));
            SDL_Rect vthumb = { vbg.x + 4, vbg.y + 4 + (int)(pos * (vh - thumb_h - 8)), vbg.w - 8, (int)thumb_h - 8 };
            SDL_SetRenderDrawColor(ren, sb_thumb.r, sb_thumb.g, sb_thumb.b, sb_thumb.a);
            SDL_RenderFillRect(ren, &vthumb);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(10);
    }

    SDL_StopTextInput();
    doc_free(&doc);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
