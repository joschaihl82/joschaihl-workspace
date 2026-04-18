// editor_big.c
// Kompiliert mit: gcc -o editor_big editor_big.c -lSDL2 -lSDL2_ttf
// Hinweis: Pfad zur TTF-Datei ggf. anpassen.

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCALE 4
#define BASE_WINW 800
#define BASE_WINH 600
#define WINW (BASE_WINW * SCALE)
#define WINH (BASE_WINH * SCALE)
#define BASE_FONT_SIZE 20
#define FONT_SIZE (BASE_FONT_SIZE * SCALE)
#define MARGIN (8 * SCALE)
#define LINE_SPACING (6 * SCALE)
#define CURSOR_WIDTH (3 * SCALE)
#define BLINK_MS 500

typedef struct {
    char *text;      // UTF-8 byte buffer (simple handling)
    int len;         // bytes length
    int cap;         // capacity
} Line;

typedef struct {
    Line *lines;
    int count;
    int cap;
} Doc;

static void doc_init(Doc *d) {
    d->cap = 16;
    d->count = 1;
    d->lines = calloc(d->cap, sizeof(Line));
    d->lines[0].cap = 32;
    d->lines[0].text = calloc(d->lines[0].cap, 1);
    d->lines[0].len = 0;
}

static void doc_free(Doc *d) {
    for (int i = 0; i < d->count; ++i) free(d->lines[i].text);
    free(d->lines);
}

static void ensure_line_cap(Line *ln, int need) {
    if (ln->cap >= need) return;
    while (ln->cap < need) ln->cap *= 2;
    ln->text = realloc(ln->text, ln->cap);
}

static void doc_insert_line(Doc *d, int idx) {
    if (d->count + 1 > d->cap) {
        d->cap *= 2;
        d->lines = realloc(d->lines, d->cap * sizeof(Line));
    }
    for (int i = d->count; i > idx; --i) d->lines[i] = d->lines[i-1];
    d->lines[idx].cap = 32;
    d->lines[idx].text = calloc(d->lines[idx].cap, 1);
    d->lines[idx].len = 0;
    d->count++;
}

static void doc_delete_line(Doc *d, int idx) {
    free(d->lines[idx].text);
    for (int i = idx; i < d->count - 1; ++i) d->lines[i] = d->lines[i+1];
    d->count--;
}

static void insert_text_at(Line *ln, int pos, const char *s) {
    int add = (int)strlen(s);
    ensure_line_cap(ln, ln->len + add + 1);
    memmove(ln->text + pos + add, ln->text + pos, ln->len - pos + 1);
    memcpy(ln->text + pos, s, add);
    ln->len += add;
}

static void delete_backspace(Line *ln, int pos) {
    if (pos <= 0) return;
    // remove one byte before pos (simple byte-based)
    memmove(ln->text + pos - 1, ln->text + pos, ln->len - pos + 1);
    ln->len -= 1;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("SDL2 Texteditor (4x)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (!win) { fprintf(stderr, "CreateWindow: %s\n", SDL_GetError()); return 1; }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) { fprintf(stderr, "CreateRenderer: %s\n", SDL_GetError()); return 1; }

    const char *font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    TTF_Font *font = TTF_OpenFont(font_path, FONT_SIZE);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont failed for %s: %s\n", font_path, TTF_GetError());
        SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); TTF_Quit(); SDL_Quit();
        return 1;
    }

    Doc doc;
    doc_init(&doc);

    int cx = 0, cy = 0; // cursor byte offset and line index
    SDL_StartTextInput();
    int running = 1;
    Uint32 last_blink = SDL_GetTicks();
    int show_cursor = 1;

    SDL_Color bg = {30, 30, 30, 255};
    SDL_Color fg = {230, 230, 230, 255};
    SDL_Color line_num_col = {100, 100, 100, 255};

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = 0; break; }
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                // nothing special, renderer handles it
            }
            else if (e.type == SDL_KEYDOWN) {
                SDL_Keycode k = e.key.keysym.sym;
                if (k == SDLK_ESCAPE) { running = 0; break; }
                else if (k == SDLK_BACKSPACE) {
                    if (cx > 0) {
                        delete_backspace(&doc.lines[cy], cx);
                        cx--;
                    } else if (cy > 0) {
                        int prevlen = doc.lines[cy-1].len;
                        // append current line to previous
                        ensure_line_cap(&doc.lines[cy-1], prevlen + doc.lines[cy].len + 1);
                        memcpy(doc.lines[cy-1].text + prevlen, doc.lines[cy].text, doc.lines[cy].len + 1);
                        doc.lines[cy-1].len = prevlen + doc.lines[cy].len;
                        doc_delete_line(&doc, cy);
                        cy--; cx = prevlen;
                    }
                } else if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
                    // split current line at cx
                    char *rest = strdup(doc.lines[cy].text + cx);
                    doc.lines[cy].text[cx] = '\0';
                    doc.lines[cy].len = cx;
                    doc_insert_line(&doc, cy + 1);
                    free(doc.lines[cy+1].text);
                    doc.lines[cy+1].text = rest;
                    doc.lines[cy+1].len = (int)strlen(rest);
                    doc.lines[cy+1].cap = doc.lines[cy+1].len + 1;
                    cy++; cx = 0;
                } else if (k == SDLK_LEFT) {
                    if (cx > 0) cx--;
                    else if (cy > 0) { cy--; cx = doc.lines[cy].len; }
                } else if (k == SDLK_RIGHT) {
                    if (cx < doc.lines[cy].len) cx++;
                    else if (cy < doc.count - 1) { cy++; cx = 0; }
                } else if (k == SDLK_UP) {
                    if (cy > 0) {
                        cy--;
                        if (cx > doc.lines[cy].len) cx = doc.lines[cy].len;
                    }
                } else if (k == SDLK_DOWN) {
                    if (cy < doc.count - 1) {
                        cy++;
                        if (cx > doc.lines[cy].len) cx = doc.lines[cy].len;
                    }
                } else if (k == SDLK_a && (e.key.keysym.mod & KMOD_CTRL)) {
                    // select all - not implemented, placeholder
                }
            }
            else if (e.type == SDL_TEXTINPUT) {
                insert_text_at(&doc.lines[cy], cx, e.text.text);
                cx += (int)strlen(e.text.text);
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - last_blink >= BLINK_MS) {
            show_cursor = !show_cursor;
            last_blink = now;
        }

        // render
        SDL_SetRenderDrawColor(ren, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(ren);

        int y = MARGIN;
        int line_num_w = 0;
        // optional: compute width for line numbers
        {
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "%d", doc.count);
            TTF_SizeUTF8(font, tmp, &line_num_w, NULL);
            line_num_w += 12 * SCALE;
        }

        for (int i = 0; i < doc.count; ++i) {
            Line *ln = &doc.lines[i];
            // render line number
            char numbuf[32];
            snprintf(numbuf, sizeof(numbuf), "%d", i + 1);
            SDL_Surface *num_s = TTF_RenderText_Blended(font, numbuf, line_num_col);
            SDL_Texture *num_t = SDL_CreateTextureFromSurface(ren, num_s);
            SDL_Rect num_dst = {MARGIN, y, num_s->w, num_s->h};
            SDL_RenderCopy(ren, num_t, NULL, &num_dst);
            SDL_FreeSurface(num_s); SDL_DestroyTexture(num_t);

            // render text (if empty, render a space to keep height)
            const char *render_text = ln->len ? ln->text : " ";
            SDL_Surface *surf = TTF_RenderUTF8_Blended(font, render_text, fg);
            SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
            SDL_Rect dst = {MARGIN + line_num_w + 8 * SCALE, y, surf->w, surf->h};
            SDL_RenderCopy(ren, tex, NULL, &dst);
            SDL_FreeSurface(surf); SDL_DestroyTexture(tex);

            // if this is cursor line, draw caret
            if (i == cy && show_cursor) {
                // compute pixel width of substring up to cx (byte-based)
                int cxpix = 0, h = 0;
                if (cx > 0) {
                    // create temporary buffer with first cx bytes (ensure null-terminated)
                    char *tmp = malloc(cx + 1);
                    memcpy(tmp, ln->text, cx);
                    tmp[cx] = '\0';
                    TTF_SizeUTF8(font, tmp, &cxpix, &h);
                    free(tmp);
                } else {
                    cxpix = 0;
                    TTF_SizeUTF8(font, " ", NULL, &h);
                }
                SDL_Rect caret = {
                    MARGIN + line_num_w + 8 * SCALE + cxpix,
                    y,
                    CURSOR_WIDTH,
                    FONT_SIZE
                };
                SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
                SDL_RenderFillRect(ren, &caret);
            }

            y += FONT_SIZE + LINE_SPACING;
            // stop drawing if beyond window (simple optimization)
            if (y > WINH - MARGIN) break;
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
