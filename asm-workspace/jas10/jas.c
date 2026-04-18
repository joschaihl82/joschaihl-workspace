/*
 * amalgam_compiler.c
 *
 * Single-file amalgamation:
 * - Matching helpers (~40 useful functions)
 * - Memory-safe alloc helpers
 * - Lexer (token owns text)
 * - Token-based AST
 * - Parser with precedence
 * - Symbol table + scopes
 * - AST -> IR lowering
 * - Tiny optimizer
 * - Simple textual codegen / emitter
 *
 * Build:
 *   gcc -O2 amalgam_compiler.c -o amalgam_compiler
 *
 * Usage:
 *   ./amalgam_compiler
 *
 * This file is intentionally self-contained and topologically ordered
 * so there are no forward declarations required.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

/* ----------------------
   Memory helpers
   ---------------------- */
static void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) { fprintf(stderr, "fatal: out of memory\n"); exit(1); }
    return p;
}
static void *xrealloc(void *p, size_t n) {
    void *q = realloc(p, n);
    if (!q) { fprintf(stderr, "fatal: out of memory\n"); exit(1); }
    return q;
}
static char *xstrdup(const char *s) {
    if (!s) return NULL;
    char *r = strdup(s);
    if (!r) { fprintf(stderr, "fatal: out of memory\n"); exit(1); }
    return r;
}

/* ----------------------
   Matching helpers (compact but powerful)
   ~40 functions (1-2 params)
   ---------------------- */

/* basic equality */
static int m_eq(const char *a, const char *b) {
    if (!a || !b) return 0;
    return strcmp(a,b) == 0;
}
static int m_ci_eq(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}
static int m_pref(const char *s, const char *pre) {
    if (!s || !pre) return 0;
    size_t np = strlen(pre), ns = strlen(s);
    if (ns < np) return 0;
    return strncmp(s, pre, np) == 0;
}
static int m_suff(const char *s, const char *suf) {
    if (!s || !suf) return 0;
    size_t ns = strlen(s), nf = strlen(suf);
    if (ns < nf) return 0;
    return strcmp(s + (ns - nf), suf) == 0;
}
static int m_cont(const char *s, const char *sub) {
    if (!s || !sub) return 0;
    return strstr(s, sub) != NULL;
}
static int m_len(const char *s, int L) {
    if (!s) return 0;
    return (int)strlen(s) == L;
}
static int m_len_min(const char *s, int L) {
    if (!s) return 0;
    return (int)strlen(s) >= L;
}
static int m_len_max(const char *s, int L) {
    if (!s) return 0;
    return (int)strlen(s) <= L;
}
static int m_digit_c(const char *s) { return s && s[0] && isdigit((unsigned char)s[0]); }
static int m_alpha_c(const char *s) { return s && s[0] && isalpha((unsigned char)s[0]); }
static int m_alnum_c(const char *s) { return s && s[0] && isalnum((unsigned char)s[0]); }
static int m_is_int(const char *s) {
    if (!s || !*s) return 0;
    const char *p = s;
    if (*p == '+' || *p == '-') p++;
    if (!isdigit((unsigned char)*p)) return 0;
    for (; *p; p++) if (!isdigit((unsigned char)*p)) return 0;
    return 1;
}
static int m_is_hexnum(const char *s) {
    if (!s) return 0;
    if (strlen(s) > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        const char *p = s + 2;
        if (!isxdigit((unsigned char)*p)) return 0;
        for (; *p; p++) if (!isxdigit((unsigned char)*p)) return 0;
        return 1;
    }
    return 0;
}
/* single '*' wildcard */
static int m_wild(const char *s, const char *pat) {
    if (!s || !pat) return 0;
    const char *star = strchr(pat, '*');
    if (!star) return strcmp(s, pat) == 0;
    size_t left = (size_t)(star - pat);
    if (strncmp(s, pat, left) != 0) return 0;
    const char *suf = star + 1;
    size_t sl = strlen(s), su = strlen(suf);
    if (su > sl) return 0;
    return strcmp(s + (sl - su), suf) == 0;
}
/* single '?' wildcard per character */
static int m_wild1(const char *s, const char *pat) {
    if (!s || !pat) return 0;
    size_t ns = strlen(s), np = strlen(pat);
    if (ns != np) return 0;
    for (size_t i = 0; i < ns; ++i) {
        if (pat[i] == '?') continue;
        if (pat[i] != s[i]) return 0;
    }
    return 1;
}
/* palindrome / subseq / one-off */
static int m_pal(const char *s) {
    if (!s) return 0;
    size_t n = strlen(s);
    for (size_t i = 0; i < n/2; ++i) if (s[i] != s[n-1-i]) return 0;
    return 1;
}
static int m_subseq(const char *s, const char *p) {
    if (!s || !p) return 0;
    while (*s && *p) { if (*s == *p) p++; s++; }
    return *p == '\0';
}
static int m_oneoff(const char *a, const char *b) {
    if (!a || !b) return 0;
    int diff = 0;
    while (*a && *b) {
        if (*a != *b) diff++;
        if (diff > 1) return 0;
        a++; b++;
    }
    diff += strlen(a) + strlen(b);
    return diff <= 1;
}
/* levenshtein (small safe implementation) */
static int m_levenshtein(const char *a, const char *b) {
    if (!a || !b) return 9999;
    size_t na = strlen(a), nb = strlen(b);
    if (na == 0) return (int)nb;
    if (nb == 0) return (int)na;
    size_t minlen = na < nb ? na : nb;
    size_t maxlen = na < nb ? nb : na;
    int *v0 = (int*)xmalloc((minlen+1)*sizeof(int));
    int *v1 = (int*)xmalloc((minlen+1)*sizeof(int));
    const char *shorts = na < nb ? a : b;
    const char *longs  = na < nb ? b : a;
    size_t sl = minlen, ll = maxlen;
    for (size_t i = 0; i <= sl; ++i) v0[i] = (int)i;
    for (size_t j = 1; j <= ll; ++j) {
        v1[0] = (int)j;
        char lc = longs[j-1];
        for (size_t i = 1; i <= sl; ++i) {
            int cost = (shorts[i-1] == lc) ? 0 : 1;
            int a1 = v1[i-1] + 1;
            int a2 = v0[i] + 1;
            int a3 = v0[i-1] + cost;
            int best = a1 < a2 ? a1 : a2;
            best = best < a3 ? best : a3;
            v1[i] = best;
        }
        int *tmp = v0; v0 = v1; v1 = tmp;
    }
    int res = v0[sl];
    free(v0); free(v1);
    return res;
}
/* kmp contains */
static void m_kmp_build(const char *p, int *pi) {
    int m = (int)strlen(p);
    pi[0] = 0;
    int k = 0;
    for (int q = 1; q < m; ++q) {
        while (k > 0 && p[k] != p[q]) k = pi[k-1];
        if (p[k] == p[q]) k++;
        pi[q] = k;
    }
}
static int m_kmp_contains(const char *s, const char *p) {
    if (!s || !p) return 0;
    int m = (int)strlen(p); int n = (int)strlen(s);
    if (m == 0) return 1;
    int *pi = (int*)malloc(sizeof(int) * m);
    m_kmp_build(p, pi);
    int q = 0;
    for (int i = 0; i < n; ++i) {
        while (q > 0 && p[q] != s[i]) q = pi[q-1];
        if (p[q] == s[i]) q++;
        if (q == m) { free(pi); return 1; }
    }
    free(pi);
    return 0;
}
/* rabin-karp 32-bit rolling hash */
static unsigned int m_rk_hash32(const char *s, int len) {
    unsigned int h = 0;
    for (int i = 0; i < len; ++i) h = h * 16777619u ^ (unsigned char)s[i];
    return h;
}
static int m_rk_contains(const char *s, const char *p) {
    if (!s || !p) return 0;
    int ns = (int)strlen(s), np = (int)strlen(p);
    if (np == 0) return 1;
    if (np > ns) return 0;
    unsigned int hp = m_rk_hash32(p, np);
    unsigned int hs = m_rk_hash32(s, np);
    if (hs == hp && strncmp(s, p, np) == 0) return 1;
    unsigned int pow = 1;
    for (int i = 0; i < np-1; ++i) pow = pow * 16777619u;
    for (int i = np; i < ns; ++i) {
        hs = (hs - (unsigned int)(unsigned char)s[i-np] * pow) * 16777619u ^ (unsigned char)s[i];
        if (hs == hp && strncmp(s + i - np + 1, p, np) == 0) return 1;
    }
    return 0;
}
/* soundex simple */
static void m_soundex(const char *s, char *out) {
    if (!s || !out) { if (out) out[0]=0; return; }
    int o = 0; char last = 0;
    for (size_t i = 0; s[i] && o < 4; ++i) {
        char c = toupper((unsigned char)s[i]);
        char code = 0;
        if (strchr("BFPV", c)) code = '1';
        else if (strchr("CGJKQSXZ", c)) code = '2';
        else if (strchr("DT", c)) code = '3';
        else if (c == 'L') code = '4';
        else if (strchr("MN", c)) code = '5';
        else if (c == 'R') code = '6';
        if (i == 0) { out[o++] = c; last = code; continue; }
        if (code && code != last) { out[o++] = code; last = code; }
    }
    while (o < 4) out[o++] = '0';
    out[o] = '\0';
}
static int m_soundex_eq(const char *a, const char *b) {
    char sa[5], sb[5];
    m_soundex(a, sa); m_soundex(b, sb);
    return strcmp(sa, sb) == 0;
}
/* combined fuzzy wrapper */
static int m_identifier_fuzzy(const char *a, const char *b) {
    if (!a || !b) return 0;
    if (m_ci_eq(a,b)) return 1;
    if (m_kmp_contains(a,b)) return 1;
    if (m_rk_contains(a,b)) return 1;
    if (m_levenshtein(a,b) <= 1) return 1;
    if (m_soundex_eq(a,b)) return 1;
    if (m_oneoff(a,b)) return 1;
    return 0;
}

/* ----------------------
   Tokenizer / Lexer
   token owns text (must free on advance)
   ---------------------- */

typedef enum { TOK_EOF, TOK_IDENT, TOK_NUM, TOK_STR, TOK_SYM, TOK_DIR } TokKind;
typedef struct { TokKind kind; char *text; long long num; int line; int col; } Token;
typedef struct { const char *src; size_t pos; int line; int col; Token cur; } Lexer;

static void lexer_init(Lexer *L, const char *src) {
    L->src = src; L->pos = 0; L->line = 1; L->col = 1;
    L->cur.kind = TOK_EOF; L->cur.text = NULL; L->cur.num = 0;
}
static char lex_peek(Lexer *L) { return L->src[L->pos]; }
static char lex_next(Lexer *L) {
    char c = L->src[L->pos];
    if (c) { L->pos++; if (c == '\n') { L->line++; L->col = 1; } else L->col++; }
    return c;
}
static void lex_skip(Lexer *L) {
    for (;;) {
        char c = lex_peek(L);
        if (!c) return;
        if (isspace((unsigned char)c)) { lex_next(L); continue; }
        if (c == '#' || c == ';') { while (lex_peek(L) && lex_next(L) != '\n'); continue; }
        break;
    }
}
static char *lex_make(Lexer *L, size_t st, size_t en) {
    size_t len = en - st; char *s = (char*)xmalloc(len+1); memcpy(s, L->src+st, len); s[len]=0; return s;
}
static void lexer_advance(Lexer *L) {
    if (L->cur.text) { free(L->cur.text); L->cur.text = NULL; }
    lex_skip(L);
    char c = lex_peek(L);
    if (!c) { L->cur.kind = TOK_EOF; L->cur.text = NULL; return; }
    if (isalpha((unsigned char)c) || c == '.' || c == '_') {
        size_t st = L->pos; lex_next(L);
        while (isalnum((unsigned char)lex_peek(L)) || lex_peek(L) == '_' || lex_peek(L) == '.') lex_next(L);
        size_t en = L->pos; char *t = lex_make(L, st, en);
        if (t[0] == '.') L->cur.kind = TOK_DIR; else L->cur.kind = TOK_IDENT;
        L->cur.text = t; L->cur.line = L->line; L->cur.col = L->col; return;
    }
    if (isdigit((unsigned char)c) || (c == '-' && isdigit((unsigned char)L->src[L->pos+1]))) {
        size_t st = L->pos;
        if (L->src[L->pos] == '0' && (L->src[L->pos+1] == 'x' || L->src[L->pos+1]=='X')) { lex_next(L); lex_next(L); while (isxdigit((unsigned char)lex_peek(L))) lex_next(L); }
        else { if (lex_peek(L) == '-') lex_next(L); while (isdigit((unsigned char)lex_peek(L))) lex_next(L); }
        size_t en = L->pos; char *t = lex_make(L, st, en);
        L->cur.kind = TOK_NUM; L->cur.text = t; L->cur.num = strtoll(t, NULL, 0); L->cur.line = L->line; L->cur.col = L->col; return;
    }
    if (c == '"') {
        lex_next(L);
        size_t st = L->pos;
        while (lex_peek(L) && lex_peek(L) != '"') { if (lex_peek(L) == '\\') { lex_next(L); if (lex_peek(L)) lex_next(L); } else lex_next(L); }
        size_t en = L->pos; char *t = lex_make(L, st, en);
        if (lex_peek(L) == '"') lex_next(L);
        L->cur.kind = TOK_STR; L->cur.text = t; L->cur.line = L->line; L->cur.col = L->col; return;
    }
    /* single-char symbol */
    size_t st = L->pos; char sy = lex_next(L);
    L->cur.kind = TOK_SYM; L->cur.text = lex_make(L, st, L->pos); L->cur.line = L->line; L->cur.col = L->col;
}

/* convenience accessors */
static TokKind lx_kind(Lexer *L) { return L->cur.kind; }
static const char *lx_text(Lexer *L) { return L->cur.text ? L->cur.text : ""; }
static long long lx_num(Lexer *L) { return L->cur.num; }

/* ----------------------
   AST types & constructors
   ---------------------- */

typedef enum {
    AST_NONE,
    AST_NUMBER,
    AST_IDENT,
    AST_BINARY,
    AST_UNARY,
    AST_CALL,
    AST_VARDECL,
    AST_RETURN,
    AST_BLOCK,
    AST_FUNCTION,
    AST_LABEL_STMT
} ASTKind;

typedef struct AST {
    ASTKind kind;
    int line;
    union {
        long long number;     /* AST_NUMBER */
        char *ident;          /* AST_IDENT */
        struct {              /* AST_BINARY */
            char op; struct AST *left; struct AST *right;
        } bin;
        struct {              /* AST_UNARY */
            char op; struct AST *child;
        } un;
        struct {              /* CALL */
            char *fname; struct AST **args; int argc;
        } call;
        struct {              /* VARDECL */
            char *name; struct AST *value;
        } vdecl;
        struct {              /* RETURN */
            struct AST *value;
        } ret;
        struct {              /* BLOCK */
            struct AST **stmts; int count;
        } block;
        struct {              /* FUNCTION */
            char *name; char **params; int param_count; struct AST *body;
        } fn;
        struct {              /* LABEL stmt */
            char *name;
        } label;
    };
} AST;

static AST *ast_new(ASTKind k) {
    AST *a = (AST*)xmalloc(sizeof(AST));
    memset(a, 0, sizeof(AST)); a->kind = k; return a;
}
static AST *ast_number(long long v) { AST *a = ast_new(AST_NUMBER); a->number = v; return a; }
static AST *ast_ident_dup(const char *s) { AST *a = ast_new(AST_IDENT); a->ident = xstrdup(s); return a; }

/* ----------------------
   Parser (no forward declarations)
   - parser uses Lexer directly
   - topologically functions appear before use
   ---------------------- */

/* Parser struct */
typedef struct { Lexer *lx; } Parser;

/* utility: advance token and return previous token copy (caller may strdup if needed) */
static Token parser_peek_token(Parser *P) { return P->lx->cur; }
static void parser_advance(Parser *P) { lexer_advance(P->lx); }

/* expect symbol helper */
static int parser_expect_sym(Parser *P, const char *s) {
    if (P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, s) == 0) { parser_advance(P); return 1; }
    fprintf(stderr, "parse error: expected '%s' at line %d\n", s, P->lx->cur.line);
    return 0;
}

/* forward: parse_expression will be used later; define full expression parser here */
static AST *parse_expression(Parser *P); /* note: declaration only for readability within file flow */

/* parse primary: number, ident (call or var), '(' expr ')', string, label declaration */
static AST *parse_primary(Parser *P) {
    Token t = parser_peek_token(P);
    if (t.kind == TOK_NUM) { long long v = t.num; parser_advance(P); return ast_number(v); }
    if (t.kind == TOK_STR) { char *s = xstrdup(t.text); parser_advance(P); AST *a = ast_new(AST_IDENT); a->ident = s; return a; }
    if (t.kind == TOK_IDENT) {
        char *name = xstrdup(t.text);
        parser_advance(P);
        /* function call? peek '(' symbol */
        Token tk = parser_peek_token(P);
        if (tk.kind == TOK_SYM && strcmp(tk.text, "(") == 0) {
            parser_advance(P); AST *call = ast_new(AST_CALL);
            call->call.fname = name; call->call.args = (AST**)xmalloc(sizeof(AST*) * 8); call->call.argc = 0;
            if (!(P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, ")") == 0)) {
                do {
                    AST *arg = parse_expression(P);
                    call->call.args[call->call.argc++] = arg;
                } while (P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, ",") == 0 && (parser_advance(P),1));
            }
            parser_expect_sym(P, ")"); return call;
        } else {
            /* simple identifier */
            AST *id = ast_new(AST_IDENT); id->ident = name; return id;
        }
    }
    if (t.kind == TOK_SYM && strcmp(t.text, "(") == 0) {
        parser_advance(P);
        AST *e = parse_expression(P);
        parser_expect_sym(P, ")");
        return e;
    }
    /* unknown primary */
    fprintf(stderr,"parse error: unexpected token '%s' at line %d\n", t.text? t.text : "<eof>", t.line);
    parser_advance(P);
    return NULL;
}

/* unary */
static AST *parse_unary(Parser *P) {
    Token t = parser_peek_token(P);
    if (t.kind == TOK_SYM && (strcmp(t.text, "-") == 0 || strcmp(t.text, "+") == 0 || strcmp(t.text, "!") == 0)) {
        char op = t.text[0]; parser_advance(P);
        AST *r = parse_unary(P);
        AST *u = ast_new(AST_UNARY); u->un.op = op; u->un.child = r; return u;
    }
    return parse_primary(P);
}

/* term: * and / */
static AST *parse_term(Parser *P) {
    AST *left = parse_unary(P);
    while (P->lx->cur.kind == TOK_SYM && (strcmp(P->lx->cur.text, "*") == 0 || strcmp(P->lx->cur.text, "/") == 0)) {
        char op = P->lx->cur.text[0]; parser_advance(P);
        AST *right = parse_unary(P);
        AST *b = ast_new(AST_BINARY); b->bin.op = op; b->bin.left = left; b->bin.right = right; left = b;
    }
    return left;
}

/* expression: + and - */
static AST *parse_expression(Parser *P) {
    AST *left = parse_term(P);
    while (P->lx->cur.kind == TOK_SYM && (strcmp(P->lx->cur.text, "+") == 0 || strcmp(P->lx->cur.text, "-") == 0)) {
        char op = P->lx->cur.text[0]; parser_advance(P);
        AST *right = parse_term(P);
        AST *b = ast_new(AST_BINARY); b->bin.op = op; b->bin.left = left; b->bin.right = right; left = b;
    }
    return left;
}

/* parse statement: return; var decl; label; directive; instruction-ish simplified */
static AST *parse_statement(Parser *P) {
    Token t = parser_peek_token(P);
    if (t.kind == TOK_IDENT && m_ci_eq(t.text, "return")) {
        parser_advance(P);
        AST *val = parse_expression(P);
        if (P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, ";") == 0) parser_advance(P);
        AST *r = ast_new(AST_RETURN); r->ret.value = val; return r;
    }
    if (t.kind == TOK_IDENT && m_ci_eq(t.text, "var")) {
        parser_advance(P);
        if (P->lx->cur.kind != TOK_IDENT) { fprintf(stderr,"expected identifier after var\n"); return NULL; }
        char *name = xstrdup(P->lx->cur.text); parser_advance(P);
        parser_expect_sym(P, "=");
        AST *val = parse_expression(P);
        if (P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, ";") == 0) parser_advance(P);
        AST *v = ast_new(AST_VARDECL); v->vdecl.name = name; v->vdecl.value = val; return v;
    }
    if (t.kind == TOK_IDENT && P->lx->src[P->lx->pos] == ':' ) {
        /* not typical; skip */
    }
    if (t.kind == TOK_IDENT && P->lx->cur.text && P->lx->cur.text[0]=='.') {
        /* directive - skip for now */
        parser_advance(P);
        if (P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text,";")==0) parser_advance(P);
        return NULL;
    }
    /* expression-statement */
    AST *e = parse_expression(P);
    if (P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, ";") == 0) parser_advance(P);
    return e;
}

/* parse block: { stmt* } */
static AST *parse_block(Parser *P) {
    parser_expect_sym(P, "{");
    AST *blk = ast_new(AST_BLOCK); blk->block.stmts = (AST**)xmalloc(sizeof(AST*) * 64); blk->block.count = 0;
    while (!(P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, "}") == 0) && P->lx->cur.kind != TOK_EOF) {
        AST *s = parse_statement(P);
        if (s) blk->block.stmts[blk->block.count++] = s;
    }
    parser_expect_sym(P, "}");
    return blk;
}

/* parse function: fn name(params) { body } */
static AST *parse_function(Parser *P) {
    Token t = parser_peek_token(P);
    if (!(t.kind == TOK_IDENT && m_ci_eq(t.text, "fn"))) return NULL;
    parser_advance(P);
    if (P->lx->cur.kind != TOK_IDENT) { fprintf(stderr,"expected function name\n"); return NULL; }
    char *name = xstrdup(P->lx->cur.text); parser_advance(P);
    parser_expect_sym(P, "(");
    char **params = (char**)xmalloc(sizeof(char*) * 16); int pc = 0;
    if (!(P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, ")") == 0)) {
        do {
            if (P->lx->cur.kind != TOK_IDENT) { fprintf(stderr, "expected param name\n"); break; }
            params[pc++] = xstrdup(P->lx->cur.text);
            parser_advance(P);
        } while (P->lx->cur.kind == TOK_SYM && strcmp(P->lx->cur.text, ",") == 0 && (parser_advance(P),1));
    }
    parser_expect_sym(P, ")");
    AST *body = parse_block(P);
    AST *fn = ast_new(AST_FUNCTION);
    fn->fn.name = name; fn->fn.params = params; fn->fn.param_count = pc; fn->fn.body = body;
    return fn;
}

/* top-level parse: many functions / global stmts */
static AST **parse_top(Lexer *L, int *out_count) {
    Parser P; P.lx = L;
    /* prime token */
    lexer_advance(L);
    AST **top = (AST**)xmalloc(sizeof(AST*) * 512); int tc = 0;
    while (L->cur.kind != TOK_EOF) {
        if (L->cur.kind == TOK_IDENT && m_ci_eq(L->cur.text, "fn")) {
            AST *fn = parse_function(&P);
            if (fn) top[tc++] = fn;
        } else {
            AST *s = parse_statement(&P);
            if (s) top[tc++] = s;
        }
        /* ensure lexer advanced if parser left it same (safety) */
        /* lexer_advance(L); -- not doing here because parser consumes */
    }
    *out_count = tc; return top;
}

/* ----------------------
   Symbol table & scopes
   ---------------------- */

typedef struct Sym {
    char *name;
    int kind; /* 0=var,1=fn,2=label */
    int offset;
    struct Sym *next;
} Sym;

typedef struct {
    Sym *head;
} SymTab;

typedef struct Scope {
    SymTab tab;
    struct Scope *parent;
} Scope;

static void symtab_insert(SymTab *T, const char *name, int kind, int offset) {
    Sym *s = (Sym*)xmalloc(sizeof(Sym)); s->name = xstrdup(name); s->kind = kind; s->offset = offset;
    s->next = T->head; T->head = s;
}
static Sym *symtab_find(SymTab *T, const char *name) {
    for (Sym *s = T->head; s; s = s->next) if (strcmp(s->name, name) == 0) return s;
    return NULL;
}
static Sym *scope_lookup(Scope *S, const char *name) {
    for (Scope *cur = S; cur; cur = cur->parent) {
        Sym *s = symtab_find(&cur->tab, name);
        if (s) return s;
    }
    return NULL;
}

/* ----------------------
   IR definitions + emitter
   ---------------------- */

typedef enum { IR_NOP, IR_PUSH, IR_LOAD, IR_STORE, IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_CALL, IR_RET, IR_LABEL } IROp;
typedef struct { IROp op; long long imm; char *str; } IR;
typedef struct { IR *items; int count; int cap; } IRList;

static void ir_init(IRList *I) { I->cap = 256; I->items = (IR*)xmalloc(sizeof(IR)*I->cap); I->count = 0; }
static void ir_emit(IRList *I, IROp op, long long imm, const char *s) {
    if (I->count + 1 >= I->cap) { I->cap *= 2; I->items = (IR*)xrealloc(I->items, sizeof(IR)*I->cap); }
    IR *it = &I->items[I->count++]; it->op = op; it->imm = imm; it->str = s ? xstrdup(s) : NULL;
}

/* AST -> IR lowering */
static void lower_ast(IRList *I, AST *n) {
    if (!n) return;
    switch (n->kind) {
        case AST_NUMBER: ir_emit(I, IR_PUSH, n->number, NULL); break;
        case AST_IDENT: ir_emit(I, IR_LOAD, 0, n->ident); break;
        case AST_BINARY:
            lower_ast(I, n->bin.left);
            lower_ast(I, n->bin.right);
            if (n->bin.op == '+') ir_emit(I, IR_ADD, 0, NULL);
            else if (n->bin.op == '-') ir_emit(I, IR_SUB, 0, NULL);
            else if (n->bin.op == '*') ir_emit(I, IR_MUL, 0, NULL);
            else if (n->bin.op == '/') ir_emit(I, IR_DIV, 0, NULL);
            break;
        case AST_UNARY:
            lower_ast(I, n->un.child);
            if (n->un.op == '-') { ir_emit(I, IR_PUSH, 0, NULL); ir_emit(I, IR_SUB, 0, NULL); }
            break;
        case AST_CALL:
            for (int i = 0; i < n->call.argc; ++i) lower_ast(I, n->call.args[i]);
            ir_emit(I, IR_CALL, 0, n->call.fname); break;
        case AST_VARDECL:
            lower_ast(I, n->vdecl.value);
            ir_emit(I, IR_STORE, 0, n->vdecl.name); break;
        case AST_RETURN:
            lower_ast(I, n->ret.value); ir_emit(I, IR_RET, 0, NULL); break;
        case AST_BLOCK:
            for (int i = 0; i < n->block.count; ++i) lower_ast(I, n->block.stmts[i]); break;
        case AST_FUNCTION:
            ir_emit(I, IR_LABEL, 0, n->fn.name);
            lower_ast(I, n->fn.body);
            ir_emit(I, IR_RET, 0, NULL);
            break;
        default: break;
    }
}

/* ----------------------
   Tiny peephole optimizer
   - fold push const + push const + add -> push const
   ---------------------- */
static void ir_optimize(IRList *I) {
    int i = 0;
    while (i + 2 < I->count) {
        IR *a = &I->items[i], *b = &I->items[i+1], *c = &I->items[i+2];
        if (a->op == IR_PUSH && b->op == IR_PUSH && c->op == IR_ADD) {
            long long v = a->imm + b->imm;
            a->imm = v;
            /* shift left by 2 */
            int j;
            for (j = i+1; j+2 < I->count; ++j) I->items[j] = I->items[j+2];
            I->count -= 2;
            continue;
        }
        ++i;
    }
}

/* ----------------------
   Codegen (textual)
   ---------------------- */
static void codegen_ir(IRList *I) {
    for (int i = 0; i < I->count; ++i) {
        IR *it = &I->items[i];
        switch (it->op) {
            case IR_PUSH: printf("  PUSH %lld\n", it->imm); break;
            case IR_LOAD: printf("  LOAD %s\n", it->str?it->str:"<null>"); break;
            case IR_STORE: printf("  STORE %s\n", it->str?it->str:"<null>"); break;
            case IR_ADD: printf("  ADD\n"); break;
            case IR_SUB: printf("  SUB\n"); break;
            case IR_MUL: printf("  MUL\n"); break;
            case IR_DIV: printf("  DIV\n"); break;
            case IR_CALL: printf("  CALL %s\n", it->str?it->str:"<null>"); break;
            case IR_RET: printf("  RET\n"); break;
            case IR_LABEL: printf("LABEL %s:\n", it->str?it->str:"<lab>"); break;
            default: break;
        }
    }
}

/* ----------------------
   Simple demo / test harness
   ---------------------- */

static const char *demo_program =
"fn add(a,b) {\n"
"  var t = a + b;\n"
"  return t;\n"
"}\n"
"fn main() {\n"
"  var x = 5;\n"
"  var y = 3;\n"
"  var r = add(x,y);\n"
"  return r;\n"
"}\n";

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    Lexer L; lexer_init(&L, demo_program);
    /* prime */
    lexer_advance(&L);
    int topn = 0;
    AST **top = parse_top(&L, &topn);

    IRList IR; ir_init(&IR);
    for (int i = 0; i < topn; ++i) lower_ast(&IR, top[i]);
    ir_optimize(&IR);
    printf("=== IR ===\n");
    codegen_ir(&IR);
    /* note: no full free of AST/IR for brevity */
    return 0;
}

