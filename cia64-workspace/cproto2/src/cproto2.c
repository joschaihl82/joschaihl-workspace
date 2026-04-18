/*
 * cproto_clone.c
 *
 * Minimal clone of cproto (heuristic) that extracts function prototypes
 * from C source files and writes them to a header file.
 *
 * Build:
 *   gcc -std=c11 -O2 -Wall -Wextra -o cproto_clone cproto_clone.c
 *
 * Usage:
 *   ./cproto_clone [-o out.h] file1.c [file2.c ...]
 *
 * Notes:
 *  - Heuristic extractor, not a full parser.
 *  - Designed to avoid extracting calls inside function bodies.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* ---------- Configuration ---------- */
#define DEFAULT_OUT "prototypes.h"
#define READ_CHUNK 65536
#define MAX_SIG_LEN 16384
#define MAX_PARAM_LEN 8192

/* ---------- Safe allocation helpers ---------- */
static void *xcalloc(size_t nmemb, size_t size) {
    if (nmemb && size > SIZE_MAX / nmemb) {
        fprintf(stderr, "Allocation overflow\n");
        exit(2);
    }
    void *p = calloc(nmemb, size);
    if (!p) { perror("calloc"); exit(2); }
    return p;
}
static void *xrealloc(void *p, size_t newsize) {
    void *q = realloc(p, newsize);
    if (!q) { perror("realloc"); exit(2); }
    return q;
}
static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = xcalloc(n, 1);
    memcpy(p, s, n);
    return p;
}

/* ---------- File reading ---------- */
static char *read_file(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    size_t cap = READ_CHUNK;
    char *buf = xcalloc(cap + 1, 1);
    size_t len = 0;
    while (1) {
        if (len + READ_CHUNK + 1 > cap) {
            cap += READ_CHUNK;
            buf = xrealloc(buf, cap + 1);
        }
        size_t r = fread(buf + len, 1, READ_CHUNK, f);
        len += r;
        if (r < READ_CHUNK) break;
    }
    buf[len] = '\0';
    fclose(f);
    if (out_len) *out_len = len;
    return buf;
}

/* ---------- Strip comments and strings ---------- */
static char *strip_comments_and_strings(const char *s) {
    size_t n = strlen(s);
    char *out = xcalloc(n + 1, 1);
    size_t i = 0, j = 0;
    while (i < n) {
        if (s[i] == '/' && i + 1 < n && s[i+1] == '*') {
            i += 2;
            while (i + 1 < n && !(s[i] == '*' && s[i+1] == '/')) {
                if (s[i] == '\n') out[j++] = '\n';
                i++;
            }
            if (i + 1 < n) i += 2;
        } else if (s[i] == '/' && i + 1 < n && s[i+1] == '/') {
            i += 2;
            while (i < n && s[i] != '\n') i++;
        } else if (s[i] == '"' || s[i] == '\'') {
            char q = s[i++];
            out[j++] = ' ';
            while (i < n) {
                if (s[i] == '\\' && i + 1 < n) { i += 2; continue; }
                if (s[i] == q) { i++; break; }
                if (s[i] == '\n') out[j++] = '\n';
                i++;
            }
            out[j++] = ' ';
        } else {
            out[j++] = s[i++];
        }
    }
    out[j] = '\0';
    return out;
}

/* ---------- Tokenizer (simple) ---------- */
typedef enum { TK_EOF=0, TK_IDENT, TK_NUMBER, TK_PUNCT, TK_PREPROC, TK_STRING, TK_CHAR } tok_t;
typedef struct { tok_t type; char *text; size_t pos; } token;

static token make_token(tok_t t, const char *txt, size_t pos) {
    token tk;
    tk.type = t;
    tk.pos = pos;
    tk.text = txt ? xstrdup(txt) : xstrdup("");
    return tk;
}
static void free_token(token *tk) { if (tk->text) free(tk->text); tk->text = NULL; }

static int is_ident_start(char c) { return (c == '_' || isalpha((unsigned char)c)); }
static int is_ident_char(char c) { return (c == '_' || isalnum((unsigned char)c)); }

static token *tokenize(const char *buf, size_t *out_count) {
    size_t n = strlen(buf);
    size_t i = 0;
    size_t cap = 256;
    token *arr = xcalloc(cap, sizeof(token));
    size_t cnt = 0;

    while (i < n) {
        if (isspace((unsigned char)buf[i])) { i++; continue; }

        if (buf[i] == '#') {
            size_t j = i;
            while (j < n && buf[j] != '\n') j++;
            size_t len = j - i;
            char *t = xcalloc(len + 1, 1);
            memcpy(t, buf + i, len);
            if (cnt + 1 >= cap) { cap *= 2; arr = xrealloc(arr, cap * sizeof(token)); }
            arr[cnt++] = make_token(TK_PREPROC, t, i);
            free(t);
            i = j;
            continue;
        }

        if (buf[i] == '"' ) {
            size_t j = i + 1;
            while (j < n) {
                if (buf[j] == '\\' && j + 1 < n) { j += 2; continue; }
                if (buf[j] == '"') { j++; break; }
                j++;
            }
            size_t len = j - i;
            char *t = xcalloc(len + 1, 1);
            memcpy(t, buf + i, len);
            if (cnt + 1 >= cap) { cap *= 2; arr = xrealloc(arr, cap * sizeof(token)); }
            arr[cnt++] = make_token(TK_STRING, t, i);
            free(t);
            i = j;
            continue;
        }

        if (buf[i] == '\'') {
            size_t j = i + 1;
            while (j < n) {
                if (buf[j] == '\\' && j + 1 < n) { j += 2; continue; }
                if (buf[j] == '\'') { j++; break; }
                j++;
            }
            size_t len = j - i;
            char *t = xcalloc(len + 1, 1);
            memcpy(t, buf + i, len);
            if (cnt + 1 >= cap) { cap *= 2; arr = xrealloc(arr, cap * sizeof(token)); }
            arr[cnt++] = make_token(TK_CHAR, t, i);
            free(t);
            i = j;
            continue;
        }

        if (is_ident_start(buf[i])) {
            size_t j = i + 1;
            while (j < n && is_ident_char(buf[j])) j++;
            size_t len = j - i;
            char *t = xcalloc(len + 1, 1);
            memcpy(t, buf + i, len);
            if (cnt + 1 >= cap) { cap *= 2; arr = xrealloc(arr, cap * sizeof(token)); }
            arr[cnt++] = make_token(TK_IDENT, t, i);
            free(t);
            i = j;
            continue;
        }

        if (isdigit((unsigned char)buf[i])) {
            size_t j = i + 1;
            while (j < n && (isdigit((unsigned char)buf[j]) || buf[j]=='.' || buf[j]=='x' || buf[j]=='X' || buf[j]=='u' || buf[j]=='U' || buf[j]=='l' || buf[j]=='L' || buf[j]=='e' || buf[j]=='E' || buf[j]=='+' || buf[j]=='-')) j++;
            size_t len = j - i;
            char *t = xcalloc(len + 1, 1);
            memcpy(t, buf + i, len);
            if (cnt + 1 >= cap) { cap *= 2; arr = xrealloc(arr, cap * sizeof(token)); }
            arr[cnt++] = make_token(TK_NUMBER, t, i);
            free(t);
            i = j;
            continue;
        }

        /* two-char punctuators */
        if (i + 1 < n) {
            char a = buf[i], b = buf[i+1];
            if ((a=='-' && b=='>') || (a==':' && b==':') || (a=='=' && b=='=') || (a=='!' && b=='=') ||
                (a=='<' && b=='=') || (a=='>' && b=='=') || (a=='<' && b=='<') || (a=='>' && b=='>') ||
                (a=='&' && b=='&') || (a=='|' && b=='|')) {
                char tmp[3] = {a,b,'\0'};
                if (cnt + 1 >= cap) { cap *= 2; arr = xrealloc(arr, cap * sizeof(token)); }
                arr[cnt++] = make_token(TK_PUNCT, tmp, i);
                i += 2;
                continue;
            }
        }

        /* single punctuation */
        char tmp2[2] = { buf[i], '\0' };
        if (cnt + 1 >= cap) { cap *= 2; arr = xrealloc(arr, cap * sizeof(token)); }
        arr[cnt++] = make_token(TK_PUNCT, tmp2, i);
        i++;
    }

    /* EOF token */
    if (cnt + 1 >= cap) arr = xrealloc(arr, (cap + 1) * sizeof(token));
    arr[cnt++] = make_token(TK_PUNCT, "", strlen(buf));
    *out_count = cnt;
    return arr;
}

/* ---------- Helpers ---------- */
static void trim(char *s) {
    if (!s) return;
    char *a = s;
    while (*a && isspace((unsigned char)*a)) a++;
    if (a != s) memmove(s, a, strlen(a) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) s[--len] = '\0';
}
static int is_control_word(const char *w) {
    const char *kw[] = {"if","for","while","switch","return","sizeof","else","case","do","goto"};
    for (size_t i = 0; i < sizeof(kw)/sizeof(kw[0]); ++i) if (strcmp(w, kw[i])==0) return 1;
    return 0;
}
static void remove_trailing_attributes(char *s) {
    if (!s) return;
    char *p;
    while ((p = strstr(s, "__attribute__")) != NULL) {
        char *q = strchr(p, '(');
        if (!q) break;
        int depth = 0;
        char *r = q;
        while (*r) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
            r++;
        }
        if (*r == '\0') break;
        memmove(p, r, strlen(r) + 1);
    }
    while ((p = strstr(s, "__declspec")) != NULL) {
        char *q = strchr(p, '(');
        if (!q) break;
        int depth = 0;
        char *r = q;
        while (*r) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
            r++;
        }
        if (*r == '\0') break;
        memmove(p, r, strlen(r) + 1);
    }
}

/* Heuristic: remove parameter names */
static char *strip_param_names(const char *params) {
    if (!params) return xstrdup("()");
    size_t n = strlen(params);
    if (n > MAX_PARAM_LEN) return xstrdup("()");
    char *out = xcalloc(n + 1, 1);
    size_t oi = 0, i = 0;
    if (params[i] == '(') out[oi++] = params[i++];
    int depth = 0;
    while (i < n) {
        if (params[i] == ')' && depth == 0) { out[oi++] = ')'; i++; break; }
        if (params[i] == '(') { depth++; out[oi++] = '('; i++; continue; }
        if (params[i] == ')') { depth--; out[oi++] = ')'; i++; continue; }
        size_t start = i, j = i;
        int local_depth = depth;
        while (j < n) {
            if (params[j] == '(') local_depth++;
            else if (params[j] == ')') { if (local_depth == 0) break; local_depth--; }
            else if (params[j] == ',' && local_depth == depth) break;
            j++;
        }
        size_t plen = j - start;
        if (plen == 0) { i = j + 1; continue; }
        char *part = xcalloc(plen + 1, 1);
        memcpy(part, params + start, plen);
        part[plen] = '\0';
        trim(part);
        size_t k = strlen(part);
        if (k > 0) {
            while (k > 0 && isspace((unsigned char)part[k-1])) part[--k] = '\0';
            size_t endid = k;
            while (endid > 0 && (isalnum((unsigned char)part[endid-1]) || part[endid-1]=='_')) endid--;
            if (endid < k) {
                size_t p = endid;
                while (p > 0 && isspace((unsigned char)part[p-1])) p--;
                if (p > 0) { part[endid] = '\0'; trim(part); }
            }
        }
        size_t plen2 = strlen(part);
        if (oi + plen2 + 4 > n) { free(part); free(out); return xstrdup("()"); }
        memcpy(out + oi, part, plen2); oi += plen2;
        free(part);
        if (j < n && params[j] == ',') { out[oi++] = ','; out[oi++] = ' '; j++; }
        i = j;
    }
    out[oi] = '\0';
    return out;
}

/* ---------- Extraction using tokens ---------- */
static void extract_from_tokens(token *toks, size_t tcount, FILE *out) {
    int brace_depth = 0;
    for (size_t idx = 0; idx < tcount; ++idx) {
        token *tk = &toks[idx];
        if (tk->type == TK_PUNCT && strcmp(tk->text, "{") == 0) {
            if (brace_depth != 0) { brace_depth++; continue; }

            /* find previous non-preproc token */
            ssize_t prev = (ssize_t)idx - 1;
            while (prev >= 0 && toks[prev].type == TK_PREPROC) prev--;
            if (prev < 0) { brace_depth++; continue; }

            /* require previous token to be ')' */
            if (!(toks[prev].type == TK_PUNCT && strcmp(toks[prev].text, ")") == 0)) {
                brace_depth++; continue;
            }

            /* find matching '(' */
            ssize_t rparen = prev;
            ssize_t p = rparen;
            int depth = 0;
            ssize_t lparen = -1;
            for (; p >= 0; --p) {
                if (toks[p].type == TK_PUNCT) {
                    if (strcmp(toks[p].text, ")") == 0) depth++;
                    else if (strcmp(toks[p].text, "(") == 0) {
                        depth--;
                        if (depth == 0) { lparen = p; break; }
                    }
                }
            }
            if (lparen < 0) { brace_depth++; continue; }

            /* token before '(' should be identifier (function name) */
            ssize_t before = lparen - 1;
            while (before >= 0 && toks[before].type == TK_PREPROC) before--;
            if (before < 0 || toks[before].type != TK_IDENT) { brace_depth++; continue; }
            if (is_control_word(toks[before].text)) { brace_depth++; continue; }

            /* ensure no disqualifying tokens between lparen and idx-1 */
            int bad = 0;
            for (ssize_t q = lparen; q <= (ssize_t)idx - 1; ++q) {
                if (toks[q].type == TK_PUNCT) {
                    const char *pt = toks[q].text;
                    if (strcmp(pt, ";") == 0 || strcmp(pt, "=") == 0 || strcmp(pt, ",") == 0) { bad = 1; break; }
                }
            }
            if (bad) { brace_depth++; continue; }

            /* find start token: previous ';' or '}' or start */
            ssize_t start_tok = before;
            ssize_t s = before;
            for (; s >= 0; --s) {
                if (toks[s].type == TK_PUNCT && (strcmp(toks[s].text, ";") == 0 || strcmp(toks[s].text, "}") == 0)) {
                    start_tok = s + 1;
                    break;
                }
                start_tok = s;
            }

            /* compose signature string */
            char sig[MAX_SIG_LEN];
            sig[0] = '\0';
            size_t si = 0;
            for (ssize_t k = start_tok; k <= rparen; ++k) {
                if (toks[k].type == TK_PREPROC) continue;
                if (toks[k].type == TK_STRING || toks[k].type == TK_CHAR) continue;
                if (si > 0 && !isspace((unsigned char)sig[si-1])) {
                    if ((toks[k].text[0] == '_' || isalnum((unsigned char)toks[k].text[0])) &&
                        (isalnum((unsigned char)sig[si-1]) || sig[si-1] == '_')) {
                        if (si + 2 < sizeof(sig)) sig[si++] = ' ';
                    }
                }
                size_t tlen = strlen(toks[k].text);
                if (si + tlen + 2 >= sizeof(sig)) break;
                memcpy(sig + si, toks[k].text, tlen);
                si += tlen;
                sig[si] = '\0';
            }
            trim(sig);
            remove_trailing_attributes(sig);

            /* find '(' and ')' in sig */
            char *rpar = strrchr(sig, ')');
            char *lpar = NULL;
            if (rpar) {
                int d = 0;
                for (char *pp = rpar; pp >= sig; --pp) {
                    if (*pp == ')') d++;
                    else if (*pp == '(') { d--; if (d == 0) { lpar = pp; break; } }
                }
            }
            if (!lpar) { brace_depth++; continue; }

            size_t lidx = (size_t)(lpar - sig);
            size_t ridx = (size_t)(rpar - sig);
            size_t plen = ridx - lidx + 1;
            if (plen == 0 || plen > MAX_PARAM_LEN) { brace_depth++; continue; }

            char *params = xcalloc(plen + 1, 1);
            memcpy(params, sig + lidx, plen);
            params[plen] = '\0';
            char *clean = strip_param_names(params);

            /* build prototype */
            size_t prelen = lidx;
            size_t cap = prelen + strlen(clean) + 8;
            char *proto = xcalloc(cap, 1);
            memcpy(proto, sig, prelen);
            proto[prelen] = '\0';
            trim(proto);
            remove_trailing_attributes(proto);
            strcat(proto, clean);
            strcat(proto, ";");

            fprintf(out, "%s\n\n", proto);

            free(params);
            free(clean);
            free(proto);

            brace_depth++;
            continue;
        } else if (tk->type == TK_PUNCT && strcmp(tk->text, "}") == 0) {
            if (brace_depth > 0) brace_depth--;
            continue;
        } else {
            continue;
        }
    }
}

/* ---------- Header helpers ---------- */
static void write_preamble(FILE *out, const char *outname, const char *files) {
    char guard[256]; size_t gj = 0;
    for (size_t i = 0; outname[i] && gj + 2 < sizeof(guard); ++i) {
        unsigned char c = (unsigned char)outname[i];
        if (isalnum(c)) guard[gj++] = toupper(c); else guard[gj++] = '_';
    }
    if (gj + 3 < sizeof(guard)) { guard[gj++] = '_'; guard[gj++] = 'H'; guard[gj] = '\0'; } else guard[gj-1] = '\0';
    fprintf(out, "#ifndef %s\n#define %s\n\n", guard, guard);
    fprintf(out, "/* Prototypes extracted by cproto_clone */\n");
    if (files && files[0]) fprintf(out, "/* Source files: %s */\n\n", files); else fprintf(out, "\n");
    fprintf(out, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");
}
static void write_postamble(FILE *out, const char *outname) {
    char guard[256]; size_t gj = 0;
    for (size_t i = 0; outname[i] && gj + 2 < sizeof(guard); ++i) {
        unsigned char c = (unsigned char)outname[i];
        if (isalnum(c)) guard[gj++] = toupper(c); else guard[gj++] = '_';
    }
    if (gj + 3 < sizeof(guard)) { guard[gj++] = '_'; guard[gj++] = 'H'; guard[gj] = '\0'; } else guard[gj-1] = '\0';
    fprintf(out, "#ifdef __cplusplus\n}\n#endif\n\n#endif /* %s */\n", guard);
}

/* ---------- Main ---------- */
int main(int argc, char **argv) {
    const char *prog = argc > 0 ? argv[0] : "cproto_clone";
    const char *outname = DEFAULT_OUT;
    int argi = 1;
    if (argc < 2) { fprintf(stderr, "Usage: %s [-o out.h] file1.c [file2.c ...]\n", prog); return 1; }
    if (strcmp(argv[1], "-o") == 0) {
        if (argc < 4) { fprintf(stderr, "Usage: %s [-o out.h] file1.c [file2.c ...]\n", prog); return 1; }
        outname = argv[2];
        argi = 3;
    }
    if (argi >= argc) { fprintf(stderr, "No input files\n"); return 1; }

    /* collect file list string */
    char files[1024]; files[0] = '\0';
    for (int i = argi; i < argc; ++i) {
        if (i > argi) strncat(files, ", ", sizeof(files) - strlen(files) - 1);
        strncat(files, argv[i], sizeof(files) - strlen(files) - 1);
    }

    FILE *out = fopen(outname, "w");
    if (!out) { fprintf(stderr, "Cannot open %s: %s\n", outname, strerror(errno)); return 1; }

    write_preamble(out, outname, files);

    for (int i = argi; i < argc; ++i) {
        size_t len = 0;
        char *raw = read_file(argv[i], &len);
        if (!raw) { fprintf(stderr, "Warning: cannot read %s\n", argv[i]); continue; }
        char *clean = strip_comments_and_strings(raw);
        free(raw);
        if (!clean) { fprintf(stderr, "Memory error\n"); fclose(out); return 1; }

        size_t tcount = 0;
        token *toks = tokenize(clean, &tcount);
        if (toks && tcount > 0) {
            extract_from_tokens(toks, tcount, out);
            for (size_t k = 0; k < tcount; ++k) free_token(&toks[k]);
            free(toks);
        }
        free(clean);
    }

    write_postamble(out, outname);
    fclose(out);
    return 0;
}
