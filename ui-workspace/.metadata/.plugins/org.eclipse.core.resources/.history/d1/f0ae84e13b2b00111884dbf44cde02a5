// editor_8x_mouse.c
// gcc -o editor_8x_mouse editor_8x_mouse.c -lSDL2 -lSDL2_ttf
// Klick mit der Maus setzt den Caret an die angeklickte Position (UTF-8 codepoint-basiert).

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCALE 8
#define BASE_WINW 800
#define BASE_WINH 600
#define WINW (BASE_WINW * SCALE)
#define WINH (BASE_WINH * SCALE)
#define BASE_FONT 20
#define FONT_SIZE (BASE_FONT * SCALE)
#define MARGIN (8 * SCALE)
#define LINE_SPACING (6 * SCALE)
#define SCROLLBAR_THICK (16 * SCALE / 4)
#define CURSOR_W (6 * SCALE / 4)
#define BLINK_MS 500

typedef struct { char *text; int len, cap; } Line;
typedef struct { Line *a; int n, cap; } Doc;

static void doc_init(Doc *d) {
    d->cap = 64;
    d->n = 1;
    d->a = calloc(d->cap, sizeof(Line));
    d->a[0].cap = 128;
    d->a[0].text = calloc(d->a[0].cap, 1);
    d->a[0].len = 0;
}
static void doc_free(Doc *d) {
    for (int i = 0; i < d->n; ++i) free(d->a[i].text);
    free(d->a);
}
static void ensure(Line *ln, int need) {
    if (ln->cap >= need) return;
    while (ln->cap < need) ln->cap *= 2;
    ln->text = realloc(ln->text, ln->cap);
}
static void insert_text(Line *ln, int pos, const char *s) {
    int add = (int)strlen(s);
    ensure(ln, ln->len + add + 1);
    memmove(ln->text + pos + add, ln->text + pos, ln->len - pos + 1);
    memcpy(ln->text + pos, s, add);
    ln->len += add;
}
static void delete_back(Line *ln, int pos) {
    if (pos <= 0) return;
    memmove(ln->text + pos - 1, ln->text + pos, ln->len - pos + 1);
    ln->len -= 1;
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
    cur->text[pos] = '\0';
    cur->len = pos;
    d->n++;
}
static void doc_join_line(Doc *d, int idx) {
    if (idx + 1 >= d->n) return;
    Line *a = &d->a[idx], *b = &d->a[idx+1];
    ensure(a, a->len + b->len + 1);
    memcpy(a->text + a->len, b->text, b->len + 1);
    a->len += b->len;
    free(b->text);
    for (int i = idx + 1; i < d->n - 1; ++i) d->a[i] = d->a[i+1];
    d->n--;
}

// Bestimmt die Länge (in Bytes) des nächsten UTF-8 Codepoints anhand des führenden Bytes
static int utf8_char_len(unsigned char lead) {
    if ((lead & 0x80) == 0x00) return 1;
    if ((lead & 0xE0) == 0xC0) return 2;
    if ((lead & 0xF0) == 0xE0) return 3;
    if ((lead & 0xF8) == 0xF0) return 4;
    return 1;
}

// Ermittelt den Byte-Offset innerhalb der UTF-8-Bytefolge, der am besten zur Pixelposition passt
static int find_byte_offset_for_x(TTF_Font *font, const char *utf8, int byte_len, int target_x) {
    if (byte_len <= 0) return 0;
    int pos = 0;
    int last_good = 0;
    // Messen sukzessive bis wir die Zielposition überschreiten
    while (pos <= byte_len) {
        // Erzeuge temporäre Null-terminierte Teilstring bis pos
        char *tmp = malloc(pos + 1);
        if (pos > 0) memcpy(tmp, utf8, pos);
        tmp[pos] = '\0';
        int w = 0, h = 0;
        TTF_SizeUTF8(font, tmp, &w, &h);
        free(tmp);
        if (w >= target_x) {
            // Wenn wir bereits bei 0 sind, setze auf 0, sonst auf vorherigen Codepoint
            return last_good;
        }
        if (pos == byte_len) break;
        // advance by next codepoint length
        int step = utf8_char_len((unsigned char)utf8[pos]);
        last_good = pos + step;
        pos += step;
    }
    return byte_len;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    if (TTF_Init() != 0) { fprintf(stderr, "TTF_Init: %s\n", TTF_GetError()); SDL_Quit(); return 1; }

    SDL_Window *win = SDL_CreateWindow("SDL2 Editor 8x Mouse Caret", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH, SDL_WINDOW_RESIZABLE);
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
    int running = 1;
    SDL_StartTextInput();
    Uint32 last_blink = SDL_GetTicks();
    int show_cursor = 1;
    int scroll_x = 0, scroll_y = 0;
    int dragging_v = 0, dragging_h = 0;
    int drag_start = 0;

    SDL_Color bg = {28, 28, 28, 255};
    SDL_Color fg = {235, 235, 235, 255};
    SDL_Color sb_bg = {60, 60, 60, 255};
    SDL_Color sb_thumb = {200, 200, 200, 255};

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = 0; break; }
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
                if (SDL_GetModState() & KMOD_SHIFT) scroll_x -= e.wheel.y * (FONT_SIZE / 2);
                else scroll_y -= e.wheel.y * (FONT_SIZE + LINE_SPACING);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                int sbx = WINW - SCROLLBAR_THICK;
                int sby = WINH - SCROLLBAR_THICK;
                // Klick auf vertikale/horizontale Scrollbar wird hier nicht als Textklick behandelt
                if (mx >= sbx && my >= MARGIN && my < sby) { dragging_v = 1; drag_start = my; }
                else if (my >= sby && mx >= MARGIN && mx < sbx) { dragging_h = 1; drag_start = mx; }
                else {
                    // Klick in Textbereich -> Caret setzen
                    int click_x = mx + scroll_x - MARGIN;
                    int click_y = my + scroll_y - MARGIN;
                    int line_h = FONT_SIZE + LINE_SPACING;
                    int clicked_line = click_y / line_h;
                    if (clicked_line < 0) clicked_line = 0;
                    if (clicked_line >= doc.n) clicked_line = doc.n - 1;
                    cy = clicked_line;
                    // Bestimme Byte-Offset anhand von Codepoint-Grenzen und Pixelmessung
                    int byte_len = doc.a[cy].len;
                    if (byte_len <= 0) { cx = 0; }
                    else {
                        int target_x = click_x;
                        if (target_x < 0) target_x = 0;
                        int off = find_byte_offset_for_x(font, doc.a[cy].text, byte_len, target_x);
                        if (off < 0) off = 0;
                        if (off > byte_len) off = byte_len;
                        cx = off;
                    }
                    // Caret sichtbar machen und ggf. scrollen
                    // compute caret pixel pos
                    int caret_xpix = 0, caret_ypix = cy * (FONT_SIZE + LINE_SPACING);
                    if (cx > 0) {
                        char *tmp = malloc(cx + 1);
                        memcpy(tmp, doc.a[cy].text, cx);
                        tmp[cx] = '\0';
                        TTF_SizeUTF8(font, tmp, &caret_xpix, NULL);
                        free(tmp);
                    } else caret_xpix = 0;
                    int view_w = WINW - SCROLLBAR_THICK - 2 * MARGIN;
                    int view_h = WINH - SCROLLBAR_THICK - 2 * MARGIN;
                    if (caret_xpix - scroll_x < 0) scroll_x = caret_xpix - 20;
                    if (caret_xpix - scroll_x > view_w - 20) scroll_x = caret_xpix - (view_w - 20);
                    if (caret_ypix - scroll_y < 0) scroll_y = caret_ypix - 20;
                    if (caret_ypix - scroll_y > view_h - (FONT_SIZE + LINE_SPACING) + 20) scroll_y = caret_ypix - (view_h - (FONT_SIZE + LINE_SPACING) + 20);
                    if (scroll_x < 0) scroll_x = 0;
                    if (scroll_y < 0) scroll_y = 0;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) { dragging_v = dragging_h = 0; }
            else if (e.type == SDL_MOUSEMOTION) {
                if (dragging_v) {
                    int dy = e.motion.yrel;
                    scroll_y += dy * 2;
                }
                if (dragging_h) {
                    int dx = e.motion.xrel;
                    scroll_x += dx * 2;
                }
            }
        }

        // compute content size
        int maxw = 0;
        for (int i = 0; i < doc.n; ++i) {
            const char *t = doc.a[i].len ? doc.a[i].text : " ";
            int w = 0, h = 0;
            TTF_SizeUTF8(font, t, &w, &h);
            if (w > maxw) maxw = w;
        }
        int content_h = doc.n * (FONT_SIZE + LINE_SPACING);
        int content_w = maxw;
        int view_w = WINW - SCROLLBAR_THICK - 2 * MARGIN;
        int view_h = WINH - SCROLLBAR_THICK - 2 * MARGIN;

        // clamp scroll
        if (scroll_x < 0) scroll_x = 0;
        if (scroll_y < 0) scroll_y = 0;
        if (content_w > view_w && scroll_x > content_w - view_w) scroll_x = content_w - view_w;
        if (content_h > view_h && scroll_y > content_h - view_h) scroll_y = content_h - view_h;
        if (content_w <= view_w) scroll_x = 0;
        if (content_h <= view_h) scroll_y = 0;

        Uint32 now = SDL_GetTicks();
        if (now - last_blink >= BLINK_MS) { show_cursor = !show_cursor; last_blink = now; }

        // render
        SDL_SetRenderDrawColor(ren, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(ren);

        int y = MARGIN - scroll_y;
        for (int i = 0; i < doc.n; ++i) {
            const char *txt = doc.a[i].len ? doc.a[i].text : " ";
            SDL_Surface *surf = TTF_RenderUTF8_Blended(font, txt, fg);
            SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
            SDL_Rect dst = { MARGIN - scroll_x, y, surf->w, surf->h };
            SDL_RenderCopy(ren, tex, NULL, &dst);
            SDL_FreeSurface(surf); SDL_DestroyTexture(tex);

            if (i == cy && show_cursor) {
                int cxpix = 0, h = 0;
                if (cx > 0) {
                    char *tmp = malloc(cx + 1);
                    memcpy(tmp, doc.a[i].text, cx);
                    tmp[cx] = '\0';
                    TTF_SizeUTF8(font, tmp, &cxpix, &h);
                    free(tmp);
                } else {
                    cxpix = 0;
                    TTF_SizeUTF8(font, " ", NULL, &h);
                }
                SDL_Rect caret = { MARGIN - scroll_x + cxpix, y, CURSOR_W, FONT_SIZE };
                SDL_SetRenderDrawColor(ren, 245, 245, 245, 255);
                SDL_RenderFillRect(ren, &caret);
            }

            y += FONT_SIZE + LINE_SPACING;
            if (y > WINH) break;
        }

        // draw scrollbars
        SDL_Rect vbg = { WINW - SCROLLBAR_THICK, MARGIN, SCROLLBAR_THICK, WINH - SCROLLBAR_THICK - MARGIN };
        SDL_Rect hbg = { MARGIN, WINH - SCROLLBAR_THICK, WINW - SCROLLBAR_THICK - MARGIN, SCROLLBAR_THICK };
        SDL_SetRenderDrawColor(ren, sb_bg.r, sb_bg.g, sb_bg.b, sb_bg.a);
        SDL_RenderFillRect(ren, &vbg);
        SDL_RenderFillRect(ren, &hbg);

        if (content_h > 0 && view_h > 0) {
            float vh = (float)(vbg.h);
            float thumb_h = (view_h >= content_h) ? vh : (vh * ((float)view_h / (float)content_h));
            if (thumb_h < 20) thumb_h = 20;
            float pos = (content_h <= view_h) ? 0.0f : ((float)scroll_y / (float)(content_h - view_h));
            SDL_Rect vthumb = { vbg.x + 2, vbg.y + 2 + (int)(pos * (vh - thumb_h - 4)), vbg.w - 4, (int)thumb_h - 4 };
            SDL_SetRenderDrawColor(ren, sb_thumb.r, sb_thumb.g, sb_thumb.b, sb_thumb.a);
            SDL_RenderFillRect(ren, &vthumb);
        }

        if (content_w > 0 && view_w > 0) {
            float hw = (float)(hbg.w);
            float thumb_w = (view_w >= content_w) ? hw : (hw * ((float)view_w / (float)content_w));
            if (thumb_w < 20) thumb_w = 20;
            float pos = (content_w <= view_w) ? 0.0f : ((float)scroll_x / (float)(content_w - view_w));
            SDL_Rect hthumb = { hbg.x + 2 + (int)(pos * (hw - thumb_w - 4)), hbg.y + 2, (int)thumb_w - 4, hbg.h - 4 };
            SDL_SetRenderDrawColor(ren, sb_thumb.r, sb_thumb.g, sb_thumb.b, sb_thumb.a);
            SDL_RenderFillRect(ren, &hthumb);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(10);
    }

    SDL_StopTextInput();
    doc_free(&doc);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
