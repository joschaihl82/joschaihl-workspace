// editor_csyntax_minimap.c
// gcc -O2 -o editor_csyntax_minimap editor_csyntax_minimap.c -lSDL2 -lSDL2_ttf
// Editor mit Tab (4 spaces), C-Syntax-Highlighting (inkl. /* ... */ block comments, #if/#else),
// und Minimap (rechts, links von Scrollbar). Alle bisherigen Maus-/Clipboard-/Selection-Features bleiben.

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define WINW 1200
#define WINH 900
#define FONT_SIZE 36
#define MARGIN 16
#define LINE_SPACING 8
#define SCROLLBAR_THICK 32
#define MINIMAP_W 80
#define CURSOR_W 4
#define BLINK_MS 500
#define BOTTOM_PADDING 80
#define TAB_WIDTH_SPACES 4

typedef struct {
	char *text;
	int len;
	int cap;
} Line;
typedef struct {
	Line *a;
	int n, cap;
} Doc;

typedef struct {
	SDL_Window *win;
	SDL_Renderer *ren;
	TTF_Font *font;
	SDL_Color bg, fg, sb_bg, sb_thumb;
	SDL_Color kw_col, com_col, str_col, pp_col;
} UI;

/* ---------------- UTF-8 helpers ---------------- */
static int utf8_char_len(unsigned char lead) {
	if ((lead & 0x80) == 0x00)
		return 1;
	if ((lead & 0xE0) == 0xC0)
		return 2;
	if ((lead & 0xF0) == 0xE0)
		return 3;
	if ((lead & 0xF8) == 0xF0)
		return 4;
	return 1;
}
static int measure_prefix_width(TTF_Font *font, const char *s, int prefix_len) {
	if (prefix_len <= 0)
		return 0;
	char *tmp = malloc(prefix_len + 1);
	memcpy(tmp, s, prefix_len);
	tmp[prefix_len] = '\0';
	int w = 0, h = 0;
	TTF_SizeUTF8(font, tmp, &w, &h);
	free(tmp);
	return w;
}
static int find_offset_binary(TTF_Font *font, const char *s, int byte_len,
		int target_x) {
	if (byte_len <= 0)
		return 0;
	int cap = 64, cnt = 0;
	int *offs = malloc(cap * sizeof(int));
	int p = 0;
	while (p < byte_len) {
		if (cnt + 1 >= cap) {
			cap *= 2;
			offs = realloc(offs, cap * sizeof(int));
		}
		offs[cnt++] = p;
		p += utf8_char_len((unsigned char) s[p]);
	}
	if (cnt + 1 >= cap) {
		cap *= 2;
		offs = realloc(offs, cap * sizeof(int));
	}
	offs[cnt++] = byte_len;
	int lo = 0, hi = cnt - 1;
	while (lo < hi) {
		int mid = (lo + hi) / 2;
		int w = measure_prefix_width(font, s, offs[mid]);
		if (w < target_x)
			lo = mid + 1;
		else
			hi = mid;
	}
	int result = offs[lo];
	int w_lo = measure_prefix_width(font, s, result);
	if (result > 0) {
		int prev = offs[lo > 0 ? lo - 1 : 0];
		int w_prev = measure_prefix_width(font, s, prev);
		if (abs(w_prev - target_x) <= abs(w_lo - target_x))
			result = prev;
	}
	free(offs);
	return result;
}

/* ---------------- Doc ---------------- */
static void doc_init(Doc *d) {
	d->cap = 128;
	d->n = 1;
	d->a = calloc(d->cap, sizeof(Line));
	d->a[0].cap = 256;
	d->a[0].text = calloc(d->a[0].cap, 1);
	d->a[0].len = 0;
}
static void doc_free(Doc *d) {
	for (int i = 0; i < d->n; ++i)
		free(d->a[i].text);
	free(d->a);
}
static void ensure_cap(Line *ln, int need) {
	if (ln->cap >= need)
		return;
	while (ln->cap < need)
		ln->cap *= 2;
	ln->text = realloc(ln->text, ln->cap);
}
static void insert_text(Line *ln, int pos, const char *s) {
	int add = (int) strlen(s);
	ensure_cap(ln, ln->len + add + 1);
	memmove(ln->text + pos + add, ln->text + pos, ln->len - pos + 1);
	memcpy(ln->text + pos, s, add);
	ln->len += add;
}
static void delete_back(Line *ln, int pos) {
	if (pos <= 0)
		return;
	memmove(ln->text + pos - 1, ln->text + pos, ln->len - pos + 1);
	ln->len -= 1;
}
static void doc_insert_line(Doc *d, int idx) {
	if (d->n + 1 > d->cap) {
		d->cap *= 2;
		d->a = realloc(d->a, d->cap * sizeof(Line));
	}
	for (int i = d->n; i > idx; --i)
		d->a[i] = d->a[i - 1];
	d->a[idx].cap = 128;
	d->a[idx].text = calloc(d->a[idx].cap, 1);
	d->a[idx].len = 0;
	d->n++;
}
static void doc_split_line(Doc *d, int idx, int pos) {
	if (d->n + 1 > d->cap) {
		d->cap *= 2;
		d->a = realloc(d->a, d->cap * sizeof(Line));
	}
	for (int i = d->n; i > idx + 1; --i)
		d->a[i] = d->a[i - 1];
	Line *cur = &d->a[idx];
	Line *nxt = &d->a[idx + 1];
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
	if (idx + 1 >= d->n)
		return;
	Line *a = &d->a[idx], *b = &d->a[idx + 1];
	ensure_cap(a, a->len + b->len + 1);
	memcpy(a->text + a->len, b->text, b->len + 1);
	a->len += b->len;
	free(b->text);
	for (int i = idx + 1; i < d->n - 1; ++i)
		d->a[i] = d->a[i + 1];
	d->n--;
}

/* ---------------- UI ---------------- */
static int init_sdl_ttf(void) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 0;
	}
	if (TTF_Init() != 0) {
		fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
		SDL_Quit();
		return 0;
	}
	return 1;
}
static int create_ui(UI *ui, const char *font_path) {
	ui->win = SDL_CreateWindow("Editor C Syntax + Minimap",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH,
			SDL_WINDOW_RESIZABLE);
	if (!ui->win)
		return 0;
	ui->ren = SDL_CreateRenderer(ui->win, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!ui->ren) {
		SDL_DestroyWindow(ui->win);
		return 0;
	}
	ui->font = TTF_OpenFont(font_path, FONT_SIZE);
	if (!ui->font) {
		SDL_DestroyRenderer(ui->ren);
		SDL_DestroyWindow(ui->win);
		return 0;
	}
	ui->bg = (SDL_Color ) { 255, 255, 255, 255 };
	ui->fg = (SDL_Color ) { 0, 0, 0, 255 };
	ui->sb_bg = (SDL_Color ) { 240, 240, 240, 255 };
	ui->sb_thumb = (SDL_Color ) { 120, 120, 120, 255 };
	ui->kw_col = (SDL_Color ) { 0, 0, 200, 255 };
	ui->com_col = (SDL_Color ) { 0, 150, 0, 255 };
	ui->str_col = (SDL_Color ) { 200, 0, 0, 255 };
	ui->pp_col = (SDL_Color ) { 150, 0, 150, 255 };
	return 1;
}
static void cleanup_ui(UI *ui) {
	if (ui->font)
		TTF_CloseFont(ui->font);
	if (ui->ren)
		SDL_DestroyRenderer(ui->ren);
	if (ui->win)
		SDL_DestroyWindow(ui->win);
	TTF_Quit();
	SDL_Quit();
}

/* ---------------- selection helpers (same as before) ---------------- */
typedef struct {
	int line, off;
} Pos;
typedef struct {
	Pos a, b;
	int active;
} Sel;
static void sel_clear(Sel *s) {
	s->active = 0;
	s->a.line = s->a.off = s->b.line = s->b.off = 0;
}
static int sel_empty(Sel *s) {
	return !s->active || (s->a.line == s->b.line && s->a.off == s->b.off);
}
static void sel_normalize(Sel *s, Pos *out_s, Pos *out_e) {
	if (!s->active) {
		out_s->line = out_s->off = out_e->line = out_e->off = 0;
		return;
	}
	if (s->a.line < s->b.line
			|| (s->a.line == s->b.line && s->a.off <= s->b.off)) {
		*out_s = s->a;
		*out_e = s->b;
	} else {
		*out_s = s->b;
		*out_e = s->a;
	}
}
static char* get_selected_text(Doc *doc, Sel *s) {
	if (!s->active)
		return NULL;
	Pos st, ed;
	sel_normalize(s, &st, &ed);
	if (st.line == ed.line) {
		int len = ed.off - st.off;
		char *out = malloc(len + 1);
		memcpy(out, doc->a[st.line].text + st.off, len);
		out[len] = '\0';
		return out;
	} else {
		int total = 0;
		total += doc->a[st.line].len - st.off;
		for (int i = st.line + 1; i < ed.line; ++i)
			total += doc->a[i].len;
		total += ed.off;
		total += (ed.line - st.line);
		char *out = malloc(total + 1);
		char *p = out;
		int first_len = doc->a[st.line].len - st.off;
		memcpy(p, doc->a[st.line].text + st.off, first_len);
		p += first_len;
		*p++ = '\n';
		for (int i = st.line + 1; i < ed.line; ++i) {
			memcpy(p, doc->a[i].text, doc->a[i].len);
			p += doc->a[i].len;
			*p++ = '\n';
		}
		if (ed.off > 0) {
			memcpy(p, doc->a[ed.line].text, ed.off);
			p += ed.off;
		}
		*p = '\0';
		return out;
	}
}
static Pos delete_selection(Doc *doc, Sel *s) {
	Pos st, ed;
	sel_normalize(s, &st, &ed);
	Pos res = st;
	if (!s->active)
		return res;
	if (st.line == ed.line) {
		Line *ln = &doc->a[st.line];
		memmove(ln->text + st.off, ln->text + ed.off, ln->len - ed.off + 1);
		ln->len -= (ed.off - st.off);
	} else {
		Line *lns = &doc->a[st.line];
		lns->text[st.off] = '\0';
		lns->len = st.off;
		Line *lne = &doc->a[ed.line];
		ensure_cap(lns, lns->len + ed.off + 1);
		memmove(lns->text + lns->len, lne->text, ed.off);
		lns->len += ed.off;
		lns->text[lns->len] = '\0';
		for (int i = st.line + 1; i <= ed.line; ++i)
			free(doc->a[i].text);
		int shift = ed.line - st.line;
		for (int i = st.line + 1; i + shift < doc->n; ++i)
			doc->a[i] = doc->a[i + shift];
		doc->n -= shift;
	}
	sel_clear(s);
	return res;
}
static Pos insert_text_at_caret(Doc *doc, Sel *sel, Pos caret, const char *text) {
	if (sel->active && !sel_empty(sel))
		caret = delete_selection(doc, sel);
	const char *p = text;
	const char *nl;
	Pos cur = caret;
	while ((nl = strchr(p, '\n')) != NULL) {
		int chunk = (int) (nl - p);
		ensure_cap(&doc->a[cur.line], doc->a[cur.line].len + chunk + 1);
		memmove(doc->a[cur.line].text + cur.off + chunk,
				doc->a[cur.line].text + cur.off,
				doc->a[cur.line].len - cur.off + 1);
		memcpy(doc->a[cur.line].text + cur.off, p, chunk);
		doc->a[cur.line].len += chunk;
		doc_split_line(doc, cur.line, cur.off + chunk);
		cur.line += 1;
		cur.off = 0;
		p = nl + 1;
	}
	if (*p) {
		insert_text(&doc->a[cur.line], cur.off, p);
		cur.off += (int) strlen(p);
	}
	return cur;
}

/* ---------------- scroll & caret visibility ---------------- */
static int compute_content_height(Doc *doc) {
	return doc->n * (FONT_SIZE + LINE_SPACING);
}
static int compute_view_height(UI *ui) {
	int ww, wh;
	SDL_GetWindowSize(ui->win, &ww, &wh);
	int vh = wh - SCROLLBAR_THICK - 2 * MARGIN;
	return vh < 0 ? 0 : vh;
}
static void clamp_scroll(Doc *doc, UI *ui, int *scroll_y) {
	int content_h = compute_content_height(doc);
	int view_h = compute_view_height(ui);
	int max_scroll = content_h - view_h + BOTTOM_PADDING;
	if (max_scroll < 0)
		max_scroll = 0;
	if (*scroll_y < 0)
		*scroll_y = 0;
	if (*scroll_y > max_scroll)
		*scroll_y = max_scroll;
}
static void ensure_caret_visible_with_padding(Doc *doc, UI *ui, Pos caret,
		int *scroll_y) {
	int view_h = compute_view_height(ui);
	if (view_h <= 0)
		return;
	int caret_ypix = caret.line * (FONT_SIZE + LINE_SPACING);
	int desired_bottom = view_h - (FONT_SIZE + LINE_SPACING) - BOTTOM_PADDING;
	if (desired_bottom < 0)
		desired_bottom = 0;
	if (caret_ypix - *scroll_y < 0)
		*scroll_y = caret_ypix - 20;
	if (caret_ypix - *scroll_y > desired_bottom)
		*scroll_y = caret_ypix - desired_bottom;
	clamp_scroll(doc, ui, scroll_y);
}

/* ---------------- word/line selection helpers ---------------- */
static int is_word_char(unsigned char c) {
	if (c >= 'A' && c <= 'Z')
		return 1;
	if (c >= 'a' && c <= 'z')
		return 1;
	if (c >= '0' && c <= '9')
		return 1;
	if (c == '_' || c == '-')
		return 1;
	return 0;
}
static void select_word_at(Doc *doc, Sel *sel, Pos p) {
	Line *ln = &doc->a[p.line];
	if (ln->len == 0) {
		sel->active = 1;
		sel->a = sel->b = p;
		return;
	}
	int start = p.off;
	if (start > ln->len)
		start = ln->len;
	if (start == ln->len && start > 0)
		start--;
	int left = start;
	while (left > 0) {
		unsigned char lead = (unsigned char) ln->text[left - 1];
		int clen = utf8_char_len(lead);
		int idx = left - clen;
		if (idx < 0)
			break;
		unsigned char ch = (unsigned char) ln->text[idx];
		if (!is_word_char(ch))
			break;
		left = idx;
	}
	int right = start;
	while (right < ln->len) {
		unsigned char lead = (unsigned char) ln->text[right];
		int clen = utf8_char_len(lead);
		unsigned char ch = (unsigned char) ln->text[right];
		if (!is_word_char(ch))
			break;
		right += clen;
	}
	sel->active = 1;
	sel->a.line = p.line;
	sel->a.off = left;
	sel->b.line = p.line;
	sel->b.off = right;
}
static void select_line_at(Doc *doc, Sel *sel, Pos p) {
	sel->active = 1;
	sel->a.line = sel->b.line = p.line;
	sel->a.off = 0;
	sel->b.off = doc->a[p.line].len;
}

/* ---------------- C keyword list ---------------- */
static const char *c_keywords[] = { "auto", "break", "case", "char", "const",
		"continue", "default", "do", "double", "else", "enum", "extern",
		"float", "for", "goto", "if", "inline", "int", "long", "register",
		"restrict", "return", "short", "signed", "sizeof", "static", "struct",
		"switch", "typedef", "union", "unsigned", "void", "volatile", "while",
		"_Bool", "_Complex", "_Imaginary" };
static int is_c_keyword(const char *start, int len) {
	for (size_t i = 0; i < sizeof(c_keywords) / sizeof(c_keywords[0]); ++i) {
		const char *kw = c_keywords[i];
		int kwlen = (int) strlen(kw);
		if (kwlen == len && strncmp(start, kw, len) == 0)
			return 1;
	}
	return 0;
}

/* ---------------- token rendering ---------------- */
static void render_token(SDL_Renderer *ren, TTF_Font *font, const char *text,
		int len, int *x, int y, SDL_Color col) {
	if (len <= 0)
		return;
	char *tmp = malloc(len + 1);
	memcpy(tmp, text, len);
	tmp[len] = '\0';
	SDL_Surface *surf = TTF_RenderUTF8_Blended(font, tmp, col);
	free(tmp);
	if (!surf)
		return;
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
	SDL_Rect dst = { *x, y, surf->w, surf->h };
	SDL_RenderCopy(ren, tex, NULL, &dst);
	*x += surf->w;
	SDL_DestroyTexture(tex);
	SDL_FreeSurface(surf);
}

/* ---------------- syntax rendering with block comment state ---------------- */
static void render_line_c_syntax_with_state(SDL_Renderer *ren, UI *ui, Line *ln,
		int *x, int y, int *in_block_comment) {
	const char *s = ln->text;
	int n = ln->len;
	int i = 0;
	int first_nonspace = 0;
	while (first_nonspace < n
			&& (s[first_nonspace] == ' ' || s[first_nonspace] == '\t'))
		first_nonspace++;
	int is_pp = (first_nonspace < n && s[first_nonspace] == '#');

	if (*in_block_comment) {
		// find end of block comment in this line
		const char *end = strstr(s, "*/");
		if (end) {
			int len = (int) (end - s) + 2;
			render_token(ren, ui->font, s, len, x, y, ui->com_col);
			i = len;
			*in_block_comment = 0;
		} else {
			render_token(ren, ui->font, s, n, x, y, ui->com_col);
			return;
		}
	}

	while (i < n) {
		unsigned char c = (unsigned char) s[i];

		if (is_pp) {
			render_token(ren, ui->font, s + i, n - i, x, y, ui->pp_col);
			break;
		}

		// block comment start
		if (c == '/' && i + 1 < n && s[i + 1] == '*') {
			const char *end = strstr(s + i + 2, "*/");
			if (end) {
				int len = (int) (end - (s + i)) + 2;
				render_token(ren, ui->font, s + i, len, x, y, ui->com_col);
				i += len;
				continue;
			} else {
				// rest of line is block comment; set state
				render_token(ren, ui->font, s + i, n - i, x, y, ui->com_col);
				*in_block_comment = 1;
				return;
			}
		}

		// line comment //
		if (c == '/' && i + 1 < n && s[i + 1] == '/') {
			render_token(ren, ui->font, s + i, n - i, x, y, ui->com_col);
			break;
		}

		// string literal
		if (c == '"') {
			int start = i;
			i++;
			while (i < n) {
				if (s[i] == '\\' && i + 1 < n) {
					i += 2;
					continue;
				}
				if (s[i] == '"') {
					i++;
					break;
				}
				i++;
			}
			render_token(ren, ui->font, s + start, i - start, x, y,
					ui->str_col);
			continue;
		}

		// char literal
		if (c == '\'') {
			int start = i;
			i++;
			while (i < n) {
				if (s[i] == '\\' && i + 1 < n) {
					i += 2;
					continue;
				}
				if (s[i] == '\'') {
					i++;
					break;
				}
				i++;
			}
			render_token(ren, ui->font, s + start, i - start, x, y,
					ui->str_col);
			continue;
		}

		// identifier / keyword
		if (isalpha(c) || c == '_') {
			int start = i;
			i++;
			while (i < n && (isalnum((unsigned char)s[i]) || s[i] == '_'))
				i++;
			int len = i - start;
			SDL_Color col = is_c_keyword(s + start, len) ? ui->kw_col : ui->fg;
			render_token(ren, ui->font, s + start, len, x, y, col);
			continue;
		}

		// number
		if (isdigit(c)) {
			int start = i;
			i++;
			while (i < n
					&& (isalnum((unsigned char )s[i]) || s[i] == '.'
							|| s[i] == 'x' || s[i] == 'X'))
				i++;
			render_token(ren, ui->font, s + start, i - start, x, y, ui->fg);
			continue;
		}

		// other (punctuation, whitespace, etc.)
		int start = i;
		int clen = utf8_char_len(c);
		i += clen;
		render_token(ren, ui->font, s + start, clen, x, y, ui->fg);
	}
}

/* ---------------- minimap rendering ---------------- */
static void render_minimap(Doc *doc, UI *ui, int scroll_y) {
	SDL_Renderer *ren = ui->ren;
	int win_w, win_h;
	SDL_GetWindowSize(ui->win, &win_w, &win_h);
	int map_w = MINIMAP_W;
	int map_h = win_h - SCROLLBAR_THICK - 2 * MARGIN;
	int map_x = win_w - SCROLLBAR_THICK - map_w - 8; // 8px gap before scrollbar
	int map_y = MARGIN;
	// background
	SDL_Rect bg = { map_x, map_y, map_w, map_h };
	SDL_SetRenderDrawColor(ren, 245, 245, 245, 255);
	SDL_RenderFillRect(ren, &bg);
	// draw per-line markers
	if (doc->n > 0 && map_h > 4) {
		for (int i = 0; i < doc->n; ++i) {
			int y = map_y
					+ (int) ((double) i / (double) doc->n * (double) map_h);
			// non-empty lines darker
			if (doc->a[i].len > 0) {
				SDL_SetRenderDrawColor(ren, 100, 100, 120, 255);
				SDL_Rect r = { map_x + 2, y, map_w - 4, 2 };
				SDL_RenderFillRect(ren, &r);
			} else {
				// faint dot for empty lines (optional)
			}
		}
		// viewport rectangle
		int content_h = compute_content_height(doc);
		int view_h = compute_view_height(ui);
		double top_ratio =
				(content_h > 0) ?
						((double) scroll_y / (double) (content_h + 0.0001)) :
						0.0;
		double view_ratio =
				(content_h > 0) ? ((double) view_h / (double) content_h) : 1.0;
		if (view_ratio > 1.0)
			view_ratio = 1.0;
		int vy = map_y + (int) (top_ratio * map_h);
		int vh = (int) (view_ratio * map_h);
		if (vh < 6)
			vh = 6;
		SDL_SetRenderDrawColor(ren, 50, 50, 80, 120);
		SDL_Rect vrect = { map_x + 1, vy, map_w - 2, vh };
		SDL_RenderDrawRect(ren, &vrect);
	}
}

/* ---------------- event handling & rendering (integrated) ---------------- */
/* For brevity the selection, clipboard, mouse, keyboard logic is similar to previous version.
 We'll reuse those functions but integrate block comment state into rendering loop. */

static int handle_events_full(Doc *doc, UI *ui, Pos *caret, Sel *sel,
		int *scroll_y, int *dragging_thumb, int *thumb_drag_offset,
		int *selecting_mouse, Uint32 *last_click_time, int *click_count,
		int *last_click_x, int *last_click_y) {
	SDL_Event e;
	int running = 1;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			running = 0;
			break;
		} else if (e.type == SDL_WINDOWEVENT
				&& e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			clamp_scroll(doc, ui, scroll_y);
		} else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode k = e.key.keysym.sym;
			int mod = SDL_GetModState();
			int ctrl = (mod & KMOD_CTRL) != 0;
			int shift = (mod & KMOD_SHIFT) != 0;
			if (ctrl && k == SDLK_c) {
				if (!sel_empty(sel)) {
					char *txt = get_selected_text(doc, sel);
					if (txt) {
						SDL_SetClipboardText(txt);
						free(txt);
					}
				}
			} else if (ctrl && k == SDLK_x) {
				if (!sel_empty(sel)) {
					char *txt = get_selected_text(doc, sel);
					if (txt) {
						SDL_SetClipboardText(txt);
						free(txt);
					}
					Pos nc = delete_selection(doc, sel);
					*caret = nc;
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				}
			} else if (ctrl && k == SDLK_v) {
				char *clip = SDL_GetClipboardText();
				if (clip && *clip) {
					*caret = insert_text_at_caret(doc, sel, *caret, clip);
					SDL_free(clip);
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				} else if (clip)
					SDL_free(clip);
			} else if (k == SDLK_TAB) {
				char spaces[TAB_WIDTH_SPACES + 1];
				memset(spaces, ' ', TAB_WIDTH_SPACES);
				spaces[TAB_WIDTH_SPACES] = '\0';
				*caret = insert_text_at_caret(doc, sel, *caret, spaces);
				ensure_caret_visible_with_padding(doc, ui, *caret, scroll_y);
			} else if (k == SDLK_ESCAPE) {
				running = 0;
				break;
			} else if (k == SDLK_BACKSPACE) {
				if (!sel_empty(sel)) {
					Pos nc = delete_selection(doc, sel);
					*caret = nc;
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				} else {
					if (caret->off > 0) {
						delete_back(&doc->a[caret->line], caret->off);
						caret->off--;
					} else if (caret->line > 0) {
						int prevlen = doc->a[caret->line - 1].len;
						doc_join_line(doc, caret->line - 1);
						caret->line--;
						caret->off = prevlen;
					}
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				}
			} else if (k == SDLK_DELETE) {
				if (!sel_empty(sel)) {
					Pos nc = delete_selection(doc, sel);
					*caret = nc;
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				} else {
					Line *ln = &doc->a[caret->line];
					if (caret->off < ln->len) {
						memmove(ln->text + caret->off,
								ln->text + caret->off + 1,
								ln->len - caret->off);
						ln->len--;
					} else if (caret->line < doc->n - 1)
						doc_join_line(doc, caret->line);
				}
			} else if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
				if (!sel_empty(sel)) {
					Pos nc = delete_selection(doc, sel);
					*caret = nc;
				}
				doc_split_line(doc, caret->line, caret->off);
				caret->line++;
				caret->off = 0;
				sel_clear(sel);
				ensure_caret_visible_with_padding(doc, ui, *caret, scroll_y);
			} else if (k == SDLK_LEFT || k == SDLK_RIGHT || k == SDLK_UP
					|| k == SDLK_DOWN) {
				if (!shift)
					sel_clear(sel);
				if (k == SDLK_LEFT) {
					if (caret->off > 0)
						caret->off--;
					else if (caret->line > 0) {
						caret->line--;
						caret->off = doc->a[caret->line].len;
					}
				} else if (k == SDLK_RIGHT) {
					if (caret->off < doc->a[caret->line].len)
						caret->off++;
					else if (caret->line < doc->n - 1) {
						caret->line++;
						caret->off = 0;
					}
				} else if (k == SDLK_UP) {
					if (caret->line > 0) {
						caret->line--;
						if (caret->off > doc->a[caret->line].len)
							caret->off = doc->a[caret->line].len;
					}
				} else if (k == SDLK_DOWN) {
					if (caret->line < doc->n - 1) {
						caret->line++;
						if (caret->off > doc->a[caret->line].len)
							caret->off = doc->a[caret->line].len;
					}
				}
				if (shift) {
					if (!sel->active) {
						sel->active = 1;
						sel->a = *caret;
						sel->b = *caret;
					}
					sel->b = *caret;
				} else
					sel_clear(sel);
				ensure_caret_visible_with_padding(doc, ui, *caret, scroll_y);
			}
		} else if (e.type == SDL_TEXTINPUT) {
			*caret = insert_text_at_caret(doc, sel, *caret, e.text.text);
			ensure_caret_visible_with_padding(doc, ui, *caret, scroll_y);
		} else if (e.type == SDL_MOUSEWHEEL) {
			*scroll_y -= e.wheel.y * (FONT_SIZE + LINE_SPACING);
			clamp_scroll(doc, ui, scroll_y);
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				int mx = e.button.x, my = e.button.y;
				Uint32 now = SDL_GetTicks();
				int dx = mx - *last_click_x, dy = my - *last_click_y;
				if (now - *last_click_time < 400 && abs(dx) < 6 && abs(dy) < 6)
					(*click_count)++;
				else
					*click_count = 1;
				*last_click_time = now;
				*last_click_x = mx;
				*last_click_y = my;
				int click_x = mx - MARGIN;
				int click_y = my + *scroll_y - MARGIN;
				int line_h = FONT_SIZE + LINE_SPACING;
				int clicked_line = click_y / line_h;
				if (clicked_line < 0)
					clicked_line = 0;
				if (clicked_line >= doc->n)
					clicked_line = doc->n - 1;
				Pos p;
				p.line = clicked_line;
				int byte_len = doc->a[p.line].len;
				if (byte_len <= 0)
					p.off = 0;
				else {
					if (click_x < 0)
						click_x = 0;
					p.off = find_offset_binary(ui->font, doc->a[p.line].text,
							byte_len, click_x);
					if (p.off < 0)
						p.off = 0;
					if (p.off > byte_len)
						p.off = byte_len;
				}
				if (*click_count == 1) {
					sel->active = 1;
					sel->a = p;
					sel->b = p;
					*selecting_mouse = 1;
					*dragging_thumb = 0;
					*caret = p;
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				} else if (*click_count == 2) {
					select_word_at(doc, sel, p);
					*caret = sel->b;
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				} else {
					select_line_at(doc, sel, p);
					*caret = sel->b;
					ensure_caret_visible_with_padding(doc, ui, *caret,
							scroll_y);
				}
			}
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				*selecting_mouse = 0;
				*dragging_thumb = 0;
			}
		} else if (e.type == SDL_MOUSEMOTION) {
			if (*selecting_mouse && (e.motion.state & SDL_BUTTON_LMASK)) {
				int mx = e.motion.x, my = e.motion.y;
				int click_x = mx - MARGIN;
				int click_y = my + *scroll_y - MARGIN;
				int line_h = FONT_SIZE + LINE_SPACING;
				int clicked_line = click_y / line_h;
				if (clicked_line < 0)
					clicked_line = 0;
				if (clicked_line >= doc->n)
					clicked_line = doc->n - 1;
				Pos newp;
				newp.line = clicked_line;
				int byte_len = doc->a[newp.line].len;
				if (byte_len <= 0)
					newp.off = 0;
				else {
					if (click_x < 0)
						click_x = 0;
					newp.off = find_offset_binary(ui->font,
							doc->a[newp.line].text, byte_len, click_x);
					if (newp.off < 0)
						newp.off = 0;
					if (newp.off > byte_len)
						newp.off = byte_len;
				}
				*caret = newp;
				sel->b = *caret;
				int win_w, win_h;
				SDL_GetWindowSize(ui->win, &win_w, &win_h);
				int margin_scroll = 40;
				if (my < MARGIN + margin_scroll) {
					*scroll_y -= (MARGIN + margin_scroll - my);
					clamp_scroll(doc, ui, scroll_y);
				} else if (my > win_h - SCROLLBAR_THICK - margin_scroll) {
					*scroll_y +=
							(my - (win_h - SCROLLBAR_THICK - margin_scroll));
					clamp_scroll(doc, ui, scroll_y);
				}
				ensure_caret_visible_with_padding(doc, ui, *caret, scroll_y);
			}
		}
	}
	return running;
}

/* ---------------- render frame (with block comment state and minimap) ---------------- */
static void render_frame_full(Doc *doc, UI *ui, Pos caret, Sel *sel,
		int scroll_y, int show_caret) {
	SDL_Renderer *ren = ui->ren;
	TTF_Font *font = ui->font;
	int win_w, win_h;
	SDL_GetWindowSize(ui->win, &win_w, &win_h);
	int view_w = win_w - SCROLLBAR_THICK - MINIMAP_W - 3 * MARGIN;
	int view_h = win_h - SCROLLBAR_THICK - 2 * MARGIN;
	if (view_w < 10)
		view_w = 10;
	if (view_h < 10)
		view_h = 10;

	SDL_SetRenderDrawColor(ren, ui->bg.r, ui->bg.g, ui->bg.b, ui->bg.a);
	SDL_RenderClear(ren);

	SDL_Color sel_col = { 180, 200, 230, 255 };

	int y = MARGIN - scroll_y;
	SDL_Rect clip = { MARGIN, MARGIN, view_w, view_h };
	SDL_RenderSetClipRect(ren, &clip);

	Pos sstart, send;
	int has_sel = sel->active && !sel_empty(sel);
	if (has_sel)
		sel_normalize(sel, &sstart, &send);

	int in_block_comment = 0; // state across lines for /* ... */

	for (int i = 0; i < doc->n; ++i) {
		Line *ln = &doc->a[i];

		// draw selection background for this line if selected
		if (has_sel && (i >= sstart.line && i <= send.line)) {
			int sel_from = 0, sel_to = 0;
			if (sstart.line == send.line) {
				sel_from = sstart.off;
				sel_to = send.off;
			} else if (i == sstart.line) {
				sel_from = sstart.off;
				sel_to = ln->len;
			} else if (i == send.line) {
				sel_from = 0;
				sel_to = send.off;
			} else {
				sel_from = 0;
				sel_to = ln->len;
			}
			if (sel_from < sel_to) {
				int x1 = MARGIN
						+ measure_prefix_width(font, ln->text, sel_from);
				int x2 = MARGIN + measure_prefix_width(font, ln->text, sel_to);
				SDL_Rect r = { x1, y, x2 - x1, FONT_SIZE };
				SDL_SetRenderDrawColor(ren, sel_col.r, sel_col.g, sel_col.b,
						sel_col.a);
				SDL_RenderFillRect(ren, &r);
			}
		}

		int x = MARGIN;
		if (ln->len > 0) {
			render_line_c_syntax_with_state(ren, ui, ln, &x, y,
					&in_block_comment);
		}

		if (show_caret && i == caret.line) {
			int cxpix =
					caret.off > 0 ?
							measure_prefix_width(font, ln->text, caret.off) : 0;
			SDL_Rect caret_r = { MARGIN + cxpix, y, CURSOR_W, FONT_SIZE };
			SDL_SetRenderDrawColor(ren, ui->fg.r, ui->fg.g, ui->fg.b, ui->fg.a);
			SDL_RenderFillRect(ren, &caret_r);
		}

		y += FONT_SIZE + LINE_SPACING;
		if (y > win_h)
			break;
	}

	SDL_RenderSetClipRect(ren, NULL);

	// minimap (left of scrollbar)
	render_minimap(doc, ui, scroll_y);

	// scrollbar
	SDL_Rect vbg = { win_w - SCROLLBAR_THICK, MARGIN, SCROLLBAR_THICK, win_h
			- SCROLLBAR_THICK - MARGIN };
	SDL_SetRenderDrawColor(ren, ui->sb_bg.r, ui->sb_bg.g, ui->sb_bg.b,
			ui->sb_bg.a);
	SDL_RenderFillRect(ren, &vbg);

	int content_h = compute_content_height(doc);
	if (content_h > 0 && view_h > 0) {
		float vh = (float) vbg.h;
		float thumb_h =
				(view_h >= content_h) ?
						vh : (vh * ((float) view_h / (float) content_h));
		if (thumb_h < 30)
			thumb_h = 30;
		float pos =
				(content_h <= view_h) ?
						0.0f :
						((float) scroll_y
								/ (float) (content_h - view_h + BOTTOM_PADDING));
		SDL_Rect vthumb =
				{ vbg.x + 4, vbg.y + 4 + (int) (pos * (vh - thumb_h - 8)), vbg.w
						- 8, (int) thumb_h - 8 };
		SDL_SetRenderDrawColor(ren, ui->sb_thumb.r, ui->sb_thumb.g,
				ui->sb_thumb.b, ui->sb_thumb.a);
		SDL_RenderFillRect(ren, &vthumb);
	}

	SDL_RenderPresent(ren);
}

/* ---------------- main ---------------- */
int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	const char *font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

	if (!init_sdl_ttf())
		return 1;
	UI ui = { 0 };
	if (!create_ui(&ui, font_path)) {
		fprintf(stderr, "UI creation failed (prüfe font_path).\n");
		cleanup_ui(&ui);
		return 1;
	}

	Doc doc;
	doc_init(&doc);
	Pos caret = { 0, 0 };
	Sel sel = { { 0, 0 }, { 0, 0 }, 0 };
	int scroll_y = 0;
	int dragging_thumb = 0, thumb_drag_offset = 0, selecting_mouse = 0;
	Uint32 last_click_time = 0;
	int click_count = 0, last_click_x = 0, last_click_y = 0;

	SDL_StartTextInput();
	int running = 1;
	Uint32 last_blink = SDL_GetTicks();
	int show_cursor = 1;

	while (running) {
		running = handle_events_full(&doc, &ui, &caret, &sel, &scroll_y,
				&dragging_thumb, &thumb_drag_offset, &selecting_mouse,
				&last_click_time, &click_count, &last_click_x, &last_click_y);
		Uint32 now = SDL_GetTicks();
		if (now - last_blink >= BLINK_MS) {
			show_cursor = !show_cursor;
			last_blink = now;
		}
		render_frame_full(&doc, &ui, caret, &sel, scroll_y, show_cursor);
		SDL_Delay(10);
	}

	SDL_StopTextInput();
	doc_free(&doc);
	cleanup_ui(&ui);
	return 0;
}
