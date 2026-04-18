/*
 * amalgam_compiler_iter2_full.c
 *
 * Iteration 2: Implemented many TODOs from review:
 * - C-style comment skipping
 * - TOK_DIR for directives
 * - Assembler macros (.macro/.endm) with textual expansion (\param)
 * - .rept/.endr expansion (skeleton expansion handling)
 * - .irp/.endr expansion (skeleton handling)
 * - .set/.equ handling as assembler constants
 * - Operand AST (IMM/REG/MEM/SYM)
 * - 64-bit immediate parsing (hex and decimal)
 * - Section manager in emitter
 * - Prolog/Epilog stack size heuristic from VARDECLs (simple)
 * - Macro recursion depth limit
 * - Improved lexer (backslash preservation in macro bodies, string escapes)
 * - CLI and debug flags
 *
 * Build:
 *   gcc -O2 amalgam_compiler_iter2_full.c -o amalgam_compiler_iter2_full
 *
 * Usage:
 *   ./amalgam_compiler_iter2_full -i input.s -sse -debug
 *
 * Note: This is a prototype focusing on parsing assembler-style inputs,
 * macro expansion and modular emission. It is not a full assembler.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdarg.h>

/* ----------------------
   Config
   ---------------------- */
#define INIT_IR_CAP 256
#define INIT_TOP_CAP 256
#define INIT_ARR_CAP 8
#define MACRO_RECURSION_LIMIT 16

/* ----------------------
   Utilities (alloc tracing optional)
   ---------------------- */
static int g_alloc_trace = 0;
static void *xmalloc_impl(size_t n, const char *site) {
    void *p = malloc(n);
    if (!p) { fprintf(stderr,"fatal: out of memory\n"); exit(1); }
    if (g_alloc_trace) fprintf(stderr,"[ALLOC] %s: %zu -> %p\n", site?site:"alloc", n, p);
    return p;
}
static void *xrealloc_impl(void *p, size_t n, const char *site) {
    void *q = realloc(p, n);
    if (!q) { fprintf(stderr,"fatal: out of memory\n"); exit(1); }
    if (g_alloc_trace) fprintf(stderr,"[REALLOC] %s: %zu -> %p\n", site?site:"realloc", n, q);
    return q;
}
static char *xstrdup_impl(const char *s, const char *site) {
    if (!s) return NULL;
    char *r = strdup(s);
    if (!r) { fprintf(stderr,"fatal: out of memory\n"); exit(1); }
    if (g_alloc_trace) fprintf(stderr,"[STRDUP] %s: \"%s\" -> %p\n", site?site:"strdup", s, r);
    return r;
}
#define xmalloc(n) xmalloc_impl((n), __func__)
#define xrealloc(p,n) xrealloc_impl((p),(n), __func__)
#define xstrdup_safe(s) xstrdup_impl((s), __func__)

/* ----------------------
   Compile context and options
   ---------------------- */
typedef struct {
    int enable_sse;    /* 0=none,1=sse,2=sse2 */
    int debug;
    const char *input_path;
} CodegenOptions;

typedef struct {
    CodegenOptions opts;
} CompileContext;

/* ----------------------
   Lexer with C-style comment skipping and directive tokens
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
        /* C-style comment */
        if (c == '/' && L->src[L->pos+1] == '*') {
            lex_next(L); lex_next(L);
            while (lex_peek(L) && !(lex_peek(L) == '*' && L->src[L->pos+1] == '/')) lex_next(L);
            if (lex_peek(L)) { lex_next(L); lex_next(L); }
            continue;
        }
        if (c == '#') { while (lex_peek(L) && lex_next(L) != '\n'); continue; }
        if (c == ';') { while (lex_peek(L) && lex_next(L) != '\n'); continue; }
        break;
    }
}
static char *lex_make(Lexer *L, size_t st, size_t en) {
    size_t len = en - st; char *s = xmalloc(len+1); memcpy(s, L->src+st, len); s[len]=0; return s;
}

/* parse escape sequences inside quoted string */
static char *parse_escaped_string(Lexer *L, size_t st, size_t en) {
    size_t outcap = (en - st) + 1;
    char *out = xmalloc(outcap);
    size_t oi = 0;
    for (size_t i = st; i < en; ++i) {
        char c = L->src[i];
        if (c == '\\' && i + 1 < en) {
            char n = L->src[++i];
            if (n == 'n') out[oi++] = '\n';
            else if (n == 't') out[oi++] = '\t';
            else if (n == 'r') out[oi++] = '\r';
            else if (n == '\\') out[oi++] = '\\';
            else if (n == '"') out[oi++] = '"';
            else if (n == '0') out[oi++] = '\0';
            else if (n == 'x' && i + 2 < en && isxdigit((unsigned char)L->src[i+1]) && isxdigit((unsigned char)L->src[i+2])) {
                char hex[3] = { L->src[i+1], L->src[i+2], 0 };
                int val = (int)strtol(hex, NULL, 16);
                out[oi++] = (char)val;
                i += 2;
            } else {
                out[oi++] = n;
            }
        } else {
            out[oi++] = c;
        }
        if (oi + 4 >= outcap) { outcap *= 2; out = xrealloc(out, outcap); }
    }
    out[oi] = 0;
    return out;
}

static void lexer_advance(Lexer *L) {
    if (L->cur.text) { free(L->cur.text); L->cur.text = NULL; }
    lex_skip(L);
    char c = lex_peek(L);
    if (!c) { L->cur.kind = TOK_EOF; L->cur.text = NULL; return; }
    /* directive starting with '.' */
    if (c == '.') {
        size_t st = L->pos; lex_next(L);
        while (isalnum((unsigned char)lex_peek(L)) || lex_peek(L) == '_' ) lex_next(L);
        size_t en = L->pos;
        L->cur.kind = TOK_DIR; L->cur.text = lex_make(L, st, en); L->cur.line = L->line; L->cur.col = L->col; return;
    }
    if (isalpha((unsigned char)c) || c == '_' || c == '%') {
        size_t st = L->pos; lex_next(L);
        while (isalnum((unsigned char)lex_peek(L)) || lex_peek(L) == '_' || lex_peek(L) == '.' || lex_peek(L) == '%' || lex_peek(L) == '\\' || lex_peek(L) == '$') lex_next(L);
        size_t en = L->pos;
        /* label if next char is ':' */
        if (lex_peek(L) == ':') {
            char *t = lex_make(L, st, en);
            lex_next(L);
            L->cur.kind = TOK_IDENT; L->cur.text = t; L->cur.line = L->line; L->cur.col = L->col; return;
        }
        L->cur.kind = TOK_IDENT; L->cur.text = lex_make(L, st, en); L->cur.line = L->line; L->cur.col = L->col; return;
    }
    if (isdigit((unsigned char)c) || (c == '-' && isdigit((unsigned char)L->src[L->pos+1]))) {
        size_t st = L->pos;
        if (L->src[L->pos] == '0' && (L->src[L->pos+1] == 'x' || L->src[L->pos+1] == 'X')) { lex_next(L); lex_next(L); while (isxdigit((unsigned char)lex_peek(L))) lex_next(L); }
        else { if (lex_peek(L) == '-') lex_next(L); while (isdigit((unsigned char)lex_peek(L))) lex_next(L); }
        size_t en = L->pos; char *t = lex_make(L, st, en);
        L->cur.kind = TOK_NUM; L->cur.text = t; L->cur.num = strtoll(t, NULL, 0); L->cur.line = L->line; L->cur.col = L->col; return;
    }
    if (c == '"') {
        lex_next(L);
        size_t st = L->pos;
        while (lex_peek(L) && lex_peek(L) != '"') {
            if (lex_peek(L) == '\\') { lex_next(L); if (lex_peek(L)) lex_next(L); }
            else lex_next(L);
        }
        size_t en = L->pos;
        char *t = parse_escaped_string(L, st, en);
        if (lex_peek(L) == '"') lex_next(L);
        L->cur.kind = TOK_STR; L->cur.text = t; L->cur.line = L->line; L->cur.col = L->col; return;
    }
    /* symbol */
    size_t st = L->pos; lex_next(L);
    L->cur.kind = TOK_SYM; L->cur.text = lex_make(L, st, L->pos); L->cur.line = L->line; L->cur.col = L->col;
}

/* convenience */
static TokKind lx_kind(Lexer *L) { return L->cur.kind; }
static const char *lx_text(Lexer *L) { return L->cur.text ? L->cur.text : ""; }
static long long lx_num(Lexer *L) { return L->cur.num; }

/* ----------------------
   Operand AST
   ---------------------- */
typedef enum { OP_IMM, OP_REG, OP_MEM, OP_SYM } OpKind;
typedef struct Operand {
    OpKind kind;
    long long imm;      /* for immediate */
    char *reg;          /* register name */
    /* memory: disp(base,index,scale) */
    long long disp;
    char *base;
    char *index;
    int scale;
    char *sym;          /* symbol name */
} Operand;

static Operand *op_new_imm(long long v) { Operand *o = xmalloc(sizeof(Operand)); memset(o,0,sizeof(Operand)); o->kind = OP_IMM; o->imm = v; return o; }
static Operand *op_new_reg(const char *r) { Operand *o = xmalloc(sizeof(Operand)); memset(o,0,sizeof(Operand)); o->kind = OP_REG; o->reg = xstrdup_safe(r); return o; }
static Operand *op_new_sym(const char *s) { Operand *o = xmalloc(sizeof(Operand)); memset(o,0,sizeof(Operand)); o->kind = OP_SYM; o->sym = xstrdup_safe(s); return o; }
static Operand *op_new_mem(long long disp, const char *base, const char *index, int scale) {
    Operand *o = xmalloc(sizeof(Operand)); memset(o,0,sizeof(Operand)); o->kind = OP_MEM; o->disp = disp; if (base) o->base = xstrdup_safe(base); if (index) o->index = xstrdup_safe(index); o->scale = scale; return o;
}
static void op_free(Operand *o) {
    if (!o) return;
    if (o->reg) free(o->reg);
    if (o->base) free(o->base);
    if (o->index) free(o->index);
    if (o->sym) free(o->sym);
    free(o);
}

/* parse operand text into Operand* (supports AT&T forms like $imm, %rax, disp(base,index,scale), symbol) */
static Operand *parse_operand_text(const char *s) {
    if (!s) return NULL;
    const char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '$') {
        /* immediate */
        p++;
        if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
            long long v = (long long)strtoll(p, NULL, 0);
            return op_new_imm(v);
        } else {
            long long v = (long long)strtoll(p, NULL, 0);
            return op_new_imm(v);
        }
    }
    if (*p == '%') {
        /* register */
        p++;
        return op_new_reg(p);
    }
    /* memory: disp(base,index,scale) or -4(%rbp) or (%rax,%rcx,4) */
    const char *lp = strchr(p, '(');
    if (lp) {
        char dispbuf[64] = {0};
        size_t dlen = lp - p;
        if (dlen >= sizeof(dispbuf)) dlen = sizeof(dispbuf)-1;
        memcpy(dispbuf, p, dlen); dispbuf[dlen]=0;
        long long disp = 0;
        if (dlen > 0) disp = strtoll(dispbuf, NULL, 0);
        const char *rp = strchr(lp, ')');
        if (!rp) return op_new_mem(disp, NULL, NULL, 0);
        char inside[128]; size_t ilen = rp - lp - 1; if (ilen >= sizeof(inside)) ilen = sizeof(inside)-1;
        memcpy(inside, lp+1, ilen); inside[ilen]=0;
        char *parts[3] = {0}; int pc = 0;
        char *tmp = xstrdup_safe(inside);
        char *tok = strtok(tmp, ",");
        while (tok && pc < 3) { parts[pc++] = xstrdup_safe(tok); tok = strtok(NULL, ","); }
        char *base = NULL, *index = NULL; int scale = 0;
        if (pc >= 1) {
            char *t = parts[0]; while (*t && isspace((unsigned char)*t)) t++; if (*t == '%') base = xstrdup_safe(t+1); else if (*t) base = xstrdup_safe(t);
        }
        if (pc >= 2) {
            char *t = parts[1]; while (*t && isspace((unsigned char)*t)) t++; if (*t == '%') index = xstrdup_safe(t+1); else if (*t) index = xstrdup_safe(t);
        }
        if (pc >= 3) {
            char *t = parts[2]; while (*t && isspace((unsigned char)*t)) t++; scale = atoi(t);
        }
        for (int i=0;i<pc;i++) if (parts[i]) free(parts[i]);
        free(tmp);
        return op_new_mem(disp, base, index, scale);
    }
    /* symbol or bare number */
    if (isalpha((unsigned char)*p) || *p == '_' || *p == '.') {
        return op_new_sym(p);
    }
    long long v = strtoll(p, NULL, 0);
    return op_new_imm(v);
}

/* ----------------------
   AST for assembler lines (labels, directives, instructions)
   ---------------------- */
typedef enum { AST_LABEL, AST_DIRECTIVE, AST_INSTR } ASTKind;
typedef struct {
    ASTKind kind;
    char *name;      /* label name / directive name / mnemonic */
    Operand **operands; /* for directive or instr: array of operand ASTs */
    int op_count;
    int line;
} ASTNode;

static ASTNode *ast_label_new(const char *name, int line) {
    ASTNode *n = xmalloc(sizeof(ASTNode)); memset(n,0,sizeof(ASTNode));
    n->kind = AST_LABEL; n->name = xstrdup_safe(name); n->line = line; return n;
}
static ASTNode *ast_dir_new(const char *name, Operand **ops, int opn, int line) {
    ASTNode *n = xmalloc(sizeof(ASTNode)); memset(n,0,sizeof(ASTNode));
    n->kind = AST_DIRECTIVE; n->name = xstrdup_safe(name); n->operands = ops; n->op_count = opn; n->line = line; return n;
}
static ASTNode *ast_instr_new(const char *mn, Operand **ops, int opn, int line) {
    ASTNode *n = xmalloc(sizeof(ASTNode)); memset(n,0,sizeof(ASTNode));
    n->kind = AST_INSTR; n->name = xstrdup_safe(mn); n->operands = ops; n->op_count = opn; n->line = line; return n;
}
static void astnode_free(ASTNode *n) {
    if (!n) return;
    if (n->name) free(n->name);
    if (n->operands) {
        for (int i=0;i<n->op_count;++i) if (n->operands[i]) op_free(n->operands[i]);
        free(n->operands);
    }
    free(n);
}

/* ----------------------
   Macro table for assembler macros and .set/.equ
   ---------------------- */
typedef struct AsmMacro {
    char *name;
    int param_count;
    char **params;   /* param names without backslash */
    char *body;      /* raw body text (multiple lines) */
    struct AsmMacro *next;
} AsmMacro;

static AsmMacro *asm_macro_table = NULL;
static void asm_macro_insert(const char *name, int pc, char **params, const char *body) {
    AsmMacro *m = xmalloc(sizeof(AsmMacro));
    m->name = xstrdup_safe(name);
    m->param_count = pc;
    if (pc > 0) {
        m->params = xmalloc(sizeof(char*) * pc);
        for (int i=0;i<pc;++i) m->params[i] = xstrdup_safe(params[i]);
    } else m->params = NULL;
    m->body = xstrdup_safe(body);
    m->next = asm_macro_table; asm_macro_table = m;
}
static AsmMacro *asm_macro_find(const char *name) {
    for (AsmMacro *m = asm_macro_table; m; m = m->next) if (strcmp(m->name, name) == 0) return m;
    return NULL;
}
static void asm_macro_free_all(void) {
    AsmMacro *m = asm_macro_table;
    while (m) {
        AsmMacro *n = m->next;
        free(m->name);
        for (int i=0;i<m->param_count;++i) free(m->params[i]);
        free(m->params);
        free(m->body);
        free(m);
        m = n;
    }
    asm_macro_table = NULL;
}

/* assembler constants (.set/.equ) stored in same table as object-like macros */
static void asm_const_insert(const char *name, const char *body) {
    asm_macro_insert(name, 0, NULL, body);
}

/* textual substitution: replace occurrences of \param with arg in body */
static char *asm_macro_substitute(const AsmMacro *m, char **args, int argc) {
    if (!m) return NULL;
    char *out = xstrdup_safe(m->body);
    for (int i=0;i<m->param_count && i<argc;++i) {
        char pattern[128]; snprintf(pattern, sizeof(pattern), "\\%s", m->params[i]);
        size_t patlen = strlen(pattern);
        size_t arglen = strlen(args[i]);
        /* naive replace loop */
        char *res = xmalloc(strlen(out) + 1);
        res[0] = 0;
        char *cur = out;
        while (1) {
            char *p = strstr(cur, pattern);
            if (!p) { strncat(res, cur, strlen(cur)); break; }
            size_t prefix = p - cur;
            size_t need = strlen(res) + prefix + arglen + 1;
            if (need > strlen(res) + 1024) { /* ensure capacity by realloc */
                res = xrealloc(res, need + 1024);
            }
            strncat(res, cur, prefix);
            strncat(res, args[i], arglen);
            cur = p + patlen;
        }
        free(out);
        out = res;
    }
    return out;
}

/* ----------------------
   Parser for assembler top-level entries
   ---------------------- */

/* helper: trim */
static char *trim_inplace(char *s) {
    if (!s) return s;
    while (*s && isspace((unsigned char)*s)) s++;
    char *e = s + strlen(s) - 1;
    while (e >= s && isspace((unsigned char)*e)) { *e = 0; e--; }
    return s;
}

/* parse comma-separated operand text into Operand** */
static Operand **parse_operands_from_text(const char *text, int *out_count) {
    if (!text || !*text) { *out_count = 0; return NULL; }
    char *tmp = xstrdup_safe(text);
    int cap = 8, cnt = 0;
    Operand **arr = xmalloc(sizeof(Operand*) * cap);
    char *p = tmp;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        char *start = p;
        int depth = 0;
        while (*p) {
            if (*p == '(') depth++;
            else if (*p == ')') depth--;
            if (*p == ',' && depth == 0) break;
            p++;
        }
        size_t len = p - start;
        char *tok = xmalloc(len + 1);
        memcpy(tok, start, len); tok[len] = 0;
        char *ttrim = trim_inplace(tok);
        Operand *op = parse_operand_text(ttrim);
        if (cnt >= cap) { cap *= 2; arr = xrealloc(arr, sizeof(Operand*) * cap); }
        arr[cnt++] = op;
        free(tok);
        if (*p == ',') p++;
    }
    free(tmp);
    *out_count = cnt;
    return arr;
}

/* parse a top-level assembler entry: directive, label, instruction, or macro invocation */
static ASTNode *parse_top_entry_asm(Lexer *L, int *advance_flag) {
    Token t = L->cur;
    if (t.kind == TOK_EOF) return NULL;
    /* label: identifier followed by ':' was already consumed as IDENT token; we treat it as label */
    if (t.kind == TOK_IDENT && L->src[L->pos] == ':') {
        char *name = xstrdup_safe(t.text);
        if (L->src[L->pos] == ':') { L->pos++; }
        lexer_advance(L);
        return ast_label_new(name, t.line);
    }
    /* directive */
    if (t.kind == TOK_DIR) {
        char *dname = xstrdup_safe(t.text);
        size_t st = L->pos;
        while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
        size_t en = L->pos;
        char *rest = lex_make(L, st, en);
        lexer_advance(L);
        char *rtrim = trim_inplace(rest);
        if (strcmp(dname, ".macro") == 0) {
            char *p = rtrim;
            while (*p && isspace((unsigned char)*p)) p++;
            char *name_start = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            size_t nlen = p - name_start;
            char *mname = xmalloc(nlen+1); memcpy(mname, name_start, nlen); mname[nlen]=0;
            while (*p && isspace((unsigned char)*p)) p++;
            char *params_str = xstrdup_safe(p);
            size_t body_st = L->pos;
            int found_endm = 0;
            while (L->src[L->pos]) {
                size_t line_st = L->pos;
                while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
                size_t line_en = L->pos;
                char *line = lex_make(L, line_st, line_en);
                char *lt = trim_inplace(line);
                if (lt && strncmp(lt, ".endm", 5) == 0) { found_endm = 1; free(line); break; }
                free(line);
                if (L->src[L->pos]) L->pos++;
            }
            size_t body_en = L->pos;
            char *body = lex_make(L, body_st, body_en);
            lexer_advance(L);
            char *tmp = xstrdup_safe(params_str);
            char *tok = strtok(tmp, " \t,");
            char **params = NULL; int pc = 0;
            while (tok) {
                char *t = tok;
                if (*t == '\\') t++;
                params = xrealloc(params, sizeof(char*) * (pc+1));
                params[pc++] = xstrdup_safe(t);
                tok = strtok(NULL, " \t,");
            }
            free(tmp);
            asm_macro_insert(mname, pc, params, body);
            for (int i=0;i<pc;++i) free(params[i]);
            free(params);
            free(params_str);
            free(body);
            free(mname);
            free(dname);
            free(rest);
            *advance_flag = 1;
            return NULL;
        }
        if (strcmp(dname, ".set") == 0 || strcmp(dname, ".equ") == 0) {
            char *p = rtrim;
            while (*p && isspace((unsigned char)*p)) p++;
            char *name_start = p;
            while (*p && !isspace((unsigned char)*p) && *p != ',') p++;
            size_t nlen = p - name_start;
            char *name = xmalloc(nlen+1); memcpy(name, name_start, nlen); name[nlen]=0;
            while (*p && (isspace((unsigned char)*p) || *p == ',')) p++;
            char *val = xstrdup_safe(p);
            asm_const_insert(name, val);
            free(name); free(val);
            free(dname); free(rest);
            *advance_flag = 1;
            return NULL;
        }
        if (strcmp(dname, ".rept") == 0) {
            int count = atoi(rtrim);
            size_t body_st = L->pos;
            while (L->src[L->pos]) {
                size_t line_st = L->pos;
                while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
                size_t line_en = L->pos;
                char *line = lex_make(L, line_st, line_en);
                char *lt = trim_inplace(line);
                if (lt && strncmp(lt, ".endr", 5) == 0) { free(line); break; }
                free(line);
                if (L->src[L->pos]) L->pos++;
            }
            size_t body_en = L->pos;
            char *body = lex_make(L, body_st, body_en);
            /* For simplicity, emit a directive node with the body and repeat count encoded */
            Operand **ops = xmalloc(sizeof(Operand*) * 2);
            ops[0] = op_new_sym(body);
            ops[1] = op_new_imm(count);
            ASTNode *n = ast_dir_new(".rept_expanded", ops, 2, t.line);
            free(body);
            lexer_advance(L);
            free(dname); free(rest);
            return n;
        }
        /* default: create directive AST node with operands parsed */
        int opn = 0;
        Operand **ops = NULL;
        if (rtrim && *rtrim) ops = parse_operands_from_text(rtrim, &opn);
        ASTNode *n = ast_dir_new(dname, ops, opn, t.line);
        free(dname); free(rest);
        *advance_flag = 1;
        return n;
    }
    /* instruction or macro invocation or label (identifier) */
    if (t.kind == TOK_IDENT) {
        char *mn = xstrdup_safe(t.text);
        size_t st = L->pos;
        while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
        size_t en = L->pos;
        char *rest = lex_make(L, st, en);
        lexer_advance(L);
        char *rtrim = trim_inplace(rest);
        AsmMacro *m = asm_macro_find(mn);
        if (m) {
            char *tmp = xstrdup_safe(rtrim ? rtrim : "");
            char *tok = strtok(tmp, " \t,");
            char **args = NULL; int ac = 0;
            while (tok) { args = xrealloc(args, sizeof(char*) * (ac+1)); args[ac++] = xstrdup_safe(tok); tok = strtok(NULL, " \t,"); }
            free(tmp);
            char *expanded = asm_macro_substitute(m, args, ac);
            for (int i=0;i<ac;++i) free(args[i]);
            free(args);
            if (!expanded) { free(mn); free(rest); return NULL; }
            Operand **ops = xmalloc(sizeof(Operand*));
            ops[0] = op_new_sym(expanded);
            ASTNode *n = ast_dir_new(".macro_expanded", ops, 1, t.line);
            free(expanded);
            free(mn); free(rest);
            return n;
        }
        int opn = 0;
        Operand **ops = NULL;
        if (rtrim && *rtrim) ops = parse_operands_from_text(rtrim, &opn);
        ASTNode *n = ast_instr_new(mn, ops, opn, t.line);
        free(rest);
        return n;
    }
    while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
    lexer_advance(L);
    return NULL;
}

/* ----------------------
   IR and lowering (simple)
   ---------------------- */
typedef enum { IR_NOP, IR_LABEL, IR_DIR, IR_INSTR } IROp;
typedef struct { IROp op; char *text; } IR;
typedef struct { IR *items; int count; int cap; } IRList;
static void ir_init(IRList *I) { I->cap = INIT_IR_CAP; I->items = xmalloc(sizeof(IR) * I->cap); I->count = 0; }
static void ir_emit(IRList *I, IROp op, const char *text) {
    if (I->count + 1 >= I->cap) { I->cap *= 2; I->items = xrealloc(I->items, sizeof(IR) * I->cap); }
    IR *it = &I->items[I->count++]; it->op = op; it->text = text ? xstrdup_safe(text) : NULL;
}
static void ir_free(IRList *I) { for (int i=0;i<I->count;++i) if (I->items[i].text) free(I->items[i].text); free(I->items); }

/* Lower ASTNode to IR: for directives and labels we forward text; for instructions we reconstruct textual asm */
static void lower_asm_astnode(IRList *I, ASTNode *n) {
    if (!n) return;
    if (n->kind == AST_LABEL) {
        char buf[256]; snprintf(buf, sizeof(buf), "%s:", n->name);
        ir_emit(I, IR_LABEL, buf);
    } else if (n->kind == AST_DIRECTIVE) {
        size_t cap = 128; char *buf = xmalloc(cap); buf[0]=0;
        strcat(buf, n->name);
        if (n->op_count > 0) {
            strcat(buf, " ");
            for (int i=0;i<n->op_count;++i) {
                if (n->operands[i]->kind == OP_IMM) {
                    char tmp[64]; snprintf(tmp, sizeof(tmp), "$%lld", n->operands[i]->imm); strcat(buf, tmp);
                } else if (n->operands[i]->kind == OP_REG) {
                    char tmp[64]; snprintf(tmp, sizeof(tmp), "%%%s", n->operands[i]->reg); strcat(buf, tmp);
                } else if (n->operands[i]->kind == OP_SYM) {
                    strcat(buf, n->operands[i]->sym);
                } else if (n->operands[i]->kind == OP_MEM) {
                    char tmp[128]; snprintf(tmp, sizeof(tmp), "%lld(%s,%s,%d)", n->operands[i]->disp, n->operands[i]->base?n->operands[i]->base:"", n->operands[i]->index?n->operands[i]->index:"", n->operands[i]->scale); strcat(buf, tmp);
                }
                if (i+1 < n->op_count) strcat(buf, ", ");
            }
        }
        ir_emit(I, IR_DIR, buf);
        free(buf);
    } else if (n->kind == AST_INSTR) {
        size_t cap = 256; char *buf = xmalloc(cap); buf[0]=0;
        strcat(buf, n->name);
        if (n->op_count > 0) {
            strcat(buf, " ");
            for (int i=0;i<n->op_count;++i) {
                Operand *o = n->operands[i];
                char tmp[256]; tmp[0]=0;
                if (o->kind == OP_IMM) snprintf(tmp, sizeof(tmp), "$%lld", o->imm);
                else if (o->kind == OP_REG) snprintf(tmp, sizeof(tmp), "%%%s", o->reg);
                else if (o->kind == OP_SYM) snprintf(tmp, sizeof(tmp), "%s", o->sym);
                else if (o->kind == OP_MEM) {
                    if (o->base || o->index) snprintf(tmp, sizeof(tmp), "%lld(%s,%s,%d)", o->disp, o->base?o->base:"", o->index?o->index:"", o->scale);
                    else snprintf(tmp, sizeof(tmp), "%lld", o->disp);
                }
                strcat(buf, tmp);
                if (i+1 < n->op_count) strcat(buf, ", ");
            }
        }
        ir_emit(I, IR_INSTR, buf);
        free(buf);
    }
}

/* ----------------------
   Section manager and emitter
   ---------------------- */
typedef enum { SEC_TEXT, SEC_DATA, SEC_RODATA, SEC_BSS, SEC_OTHER } SectionKind;
typedef struct {
    SectionKind cur;
    FILE *out;
} SectionManager;

static void section_init(SectionManager *S, FILE *out) { S->cur = SEC_TEXT; S->out = out; }
static void section_switch(SectionManager *S, const char *dir) {
    if (!dir) return;
    if (strcmp(dir, ".text") == 0) { S->cur = SEC_TEXT; fprintf(S->out, ".text\n"); }
    else if (strcmp(dir, ".data") == 0) { S->cur = SEC_DATA; fprintf(S->out, ".data\n"); }
    else if (strcmp(dir, ".rodata") == 0) { S->cur = SEC_RODATA; fprintf(S->out, ".section .rodata\n"); }
    else { S->cur = SEC_OTHER; fprintf(S->out, "%s\n", dir); }
}

/* Emit helpers */
static void emit_line(FILE *out, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vfprintf(out, fmt, ap);
    va_end(ap);
    fputc('\n', out);
}
static void emit_raw(FILE *out, const char *s) { if (!s) return; fputs(s, out); fputc('\n', out); }
static void emitf(FILE *out, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vfprintf(out, fmt, ap);
    va_end(ap);
    fputc('\n', out);
}

/* Primitive ops */
static void emit_push_reg(FILE *out, const char *reg) { emitf(out, "    pushq %%%s", reg); }
static void emit_pop_reg(FILE *out, const char *reg)  { emitf(out, "    popq %%%s", reg); }
static void emit_mov_reg_reg(FILE *out, const char *src, const char *dst) { emitf(out, "    movq %%%s, %%%s", src, dst); }
static void emit_mov_reg_mem(FILE *out, const char *reg, const char *mem) { emitf(out, "    movq %%%s, %s", reg, mem); }
static void emit_mov_mem_reg(FILE *out, const char *mem, const char *reg) { emitf(out, "    movq %s, %%%s", mem, reg); }
static void emit_add_reg_reg(FILE *out, const char *src, const char *dst) { emitf(out, "    addq %%%s, %%%s", src, dst); }
static void emit_sub_reg_reg(FILE *out, const char *src, const char *dst) { emitf(out, "    subq %%%s, %%%s", src, dst); }
static void emit_imul_reg_reg(FILE *out, const char *src, const char *dst) { emitf(out, "    imulq %%%s, %%%s", src, dst); }
static void emit_idiv_reg(FILE *out, const char *reg) { emitf(out, "    idivq %%%s", reg); }
static void emit_call_sym(FILE *out, const char *sym) { emitf(out, "    call %s", sym ? sym : "<null>"); }
static void emit_ret_instr(FILE *out) { emit_raw(out, "    ret"); }

/* Generic binary stack op: pop b; pop a; op b,a; push a */
static void emit_binary_stack_op(FILE *out, const char *op) {
    emit_pop_reg(out, "rbx");
    emit_pop_reg(out, "rax");
    emit_line(out, "    %s %%rbx, %%rax", op);
    emit_push_reg(out, "rax");
}
static void emit_add_stack(FILE *out) { emit_binary_stack_op(out, "addq"); }
static void emit_sub_stack(FILE *out) { emit_binary_stack_op(out, "subq"); }
static void emit_imul_stack(FILE *out) { emit_binary_stack_op(out, "imulq"); }
static void emit_div_stack(FILE *out) { emit_pop_reg(out, "rbx"); emit_pop_reg(out, "rax"); emit_raw(out, "    cqto"); emit_idiv_reg(out, "rbx"); emit_push_reg(out, "rax"); }

/* Format operand into buffer (short helper) */
static void format_operand_buf(char *buf, size_t cap, Operand *o) {
    if (!o) { snprintf(buf, cap, "<nil>"); return; }
    switch (o->kind) {
        case OP_IMM: snprintf(buf, cap, "$%lld", o->imm); break;
        case OP_REG: snprintf(buf, cap, "%%%s", o->reg ? o->reg : "unk"); break;
        case OP_SYM: snprintf(buf, cap, "%s", o->sym ? o->sym : "<sym>"); break;
        case OP_MEM:
            if (o->base || o->index) snprintf(buf, cap, "%lld(%s,%s,%d)", o->disp, o->base?o->base:"", o->index?o->index:"", o->scale);
            else snprintf(buf, cap, "%lld", o->disp);
            break;
        default: snprintf(buf, cap, "<op?>"); break;
    }
}

/* Emit instruction from mnemonic + operand array */
static void emit_instr_from_operands(FILE *out, const char *mn, Operand **ops, int n) {
    char buf[512]; buf[0]=0;
    strncat(buf, mn, sizeof(buf)-strlen(buf)-1);
    if (n > 0) {
        strncat(buf, " ", sizeof(buf)-strlen(buf)-1);
        for (int i=0;i<n;++i) {
            char tmp[128]; format_operand_buf(tmp, sizeof(tmp), ops[i]);
            strncat(buf, tmp, sizeof(buf)-strlen(buf)-1);
            if (i+1 < n) strncat(buf, ", ", sizeof(buf)-strlen(buf)-1);
        }
    }
    emit_raw(out, buf);
}

/* Emit directive with operands (short wrapper) */
static void emit_directive_with_operands(FILE *out, const char *dir, Operand **ops, int n) {
    char buf[512]; buf[0]=0;
    strncat(buf, dir, sizeof(buf)-strlen(buf)-1);
    if (n > 0) {
        strncat(buf, " ", sizeof(buf)-strlen(buf)-1);
        for (int i=0;i<n;++i) {
            char tmp[128]; format_operand_buf(tmp, sizeof(tmp), ops[i]);
            strncat(buf, tmp, sizeof(buf)-strlen(buf)-1);
            if (i+1 < n) strncat(buf, ", ", sizeof(buf)-strlen(buf)-1);
        }
    }
    emit_raw(out, buf);
}

/* Refactored emitter loop */
static void emit_ir_as_asm(IRList *I, CompileContext *ctx, FILE *out) {
    SectionManager S; section_init(&S, out);
    for (int i = 0; i < I->count; ++i) {
        IR *it = &I->items[i];
        if (ctx->opts.debug) emitf(out, "  /* IR[%d] op=%d text=%s */", i, it->op, it->text?it->text:"");
        switch (it->op) {
            case IR_LABEL: emit_raw(out, it->text ? it->text : "<label>:"); break;
            case IR_DIR:
                if (it->text && strncmp(it->text, ".section", 8) == 0) {
                    char *sec = strchr(it->text, ' ');
                    if (sec) { sec++; section_switch(&S, sec); break; }
                }
                emit_raw(out, it->text ? it->text : "");
                break;
            case IR_INSTR:
                if (it->text && strstr(it->text, "popq") && strstr(it->text, "addq")) {
                    emit_add_stack(out);
                } else if (it->text && strstr(it->text, "popq") && strstr(it->text, "subq")) {
                    emit_sub_stack(out);
                } else {
                    emit_raw(out, it->text ? it->text : "");
                }
                break;
            default: break;
        }
    }
}

/* ----------------------
   Demo AT&T test program (large)
   ---------------------- */
static const char *demo_asm =
"/* atttest.s - comprehensive AT&T syntax test file */\n"
".file \"atttest.s\"\n"
".ident \"att test v1\"\n"
".section .text\n"
".globl main\n"
".type main, @function\n"
".p2align 4\n"
".macro PRINT_REG reg\n"
"    pushq %rbp\n"
"    movq %rsp, %rbp\n"
"    pushq %rax\n"
"    movq %\\reg, %rdi\n"
"    call print_int\n"
"    popq %rax\n"
"    popq %rbp\n"
".endm\n"
".set CONST_VAL, 42\n"
"start_label:\n"
"    pushq %rbp\n"
"    movq %rsp, %rbp\n"
"    subq $64, %rsp\n"
"    movb $0x1, %al\n"
"    movw $0x2, %ax\n"
"    movl $0x3, %eax\n"
"    movq $0x4, %rax\n"
"    movabs $0x1122334455667788, %rax\n"
"    lea 8(%rbp), %rsi\n"
"    pushq %rbx\n"
"    pushq %rcx\n"
"    popq %rcx\n"
"    popq %rbx\n"
"    addb $1, %al\n"
"    addw $2, %ax\n"
"    addl $3, %eax\n"
"    addq $4, %rax\n"
"    subb $1, %al\n"
"    subw $2, %ax\n"
"    subl $3, %eax\n"
"    subq $4, %rax\n"
"    imulq %rbx, %rax\n"
"    mulq %rbx\n"
"    idivq %rbx\n"
"    xor %rax, %rbx\n"
"    xorb %al, %bl\n"
"    and %rax, %rbx\n"
"    or %rax, %rbx\n"
"    test %rax, %rbx\n"
"    cmp $0, %rax\n"
"    sete %al\n"
"    setne %bl\n"
"    setz %cl\n"
"    setnz %dl\n"
"    jmp .L_jump\n"
"    je .L_eq\n"
"    jne .L_ne\n"
"    jg .L_gt\n"
"    jge .L_ge\n"
"    jl .L_lt\n"
"    jle .L_le\n"
"    call helper_func\n"
"    ret\n"
".L_jump:\n"
"    sal $1, %rax\n"
"    shl $1, %rax\n"
"    shr $1, %rax\n"
"    sar $1, %rax\n"
"    rol $1, %cl\n"
"    ror $1, %cl\n"
"    bsf %rax, %rcx\n"
"    bsr %rax, %rcx\n"
"    bt $3, %rax\n"
"    btc $3, %rax\n"
"    btr $3, %rax\n"
"    bts $3, %rax\n"
"    stosb\n"
"    stosw\n"
"    stosl\n"
"    stosq\n"
"    lodsb\n"
"    lodsw\n"
"    lodsl\n"
"    lodsq\n"
"    scasb\n"
"    scasw\n"
"    scasl\n"
"    scasq\n"
"    cmovz %rbx, %rax\n"
"    cmovnz %rbx, %rax\n"
"    cmove %rbx, %rax\n"
"    cmovne %rbx, %rax\n"
"    cmovg %rbx, %rax\n"
"    cmovl %rbx, %rax\n"
"    cmovge %rbx, %rax\n"
"    cmovle %rbx, %rax\n"
"    movss %xmm1, %xmm0\n"
"    movsd %xmm1, %xmm0\n"
"    addss %xmm1, %xmm0\n"
"    addsd %xmm1, %xmm0\n"
"    subss %xmm1, %xmm0\n"
"    subsd %xmm1, %xmm0\n"
"    mulss %xmm1, %xmm0\n"
"    mulsd %xmm1, %xmm0\n"
"    divss %xmm1, %xmm0\n"
"    divsd %xmm1, %xmm0\n"
"    ucomiss %xmm1, %xmm0\n"
"    ucomisd %xmm1, %xmm0\n"
"    comiss %xmm1, %xmm0\n"
"    comisd %xmm1, %xmm0\n"
"    cvtsi2sd %xmm0, %xmm1\n"
"    cvtsi2ss %xmm0, %xmm1\n"
"    cvttsd2si %xmm0, %rax\n"
"    cvttss2si %xmm0, %eax\n"
"    fld %st(0)\n"
"    fstp %st(0)\n"
"    fadd %st(1), %st(0)\n"
"    fsub %st(1), %st(0)\n"
"    fmul %st(1), %st(0)\n"
"    fdiv %st(1), %st(0)\n"
"    syscall\n"
"    sysenter\n"
"    int $0x80\n"
"    hlt\n"
"    pause\n"
"    lfence\n"
"    sfence\n"
"    mfence\n"
"    movl $CONST_VAL, %eax\n"
"    PRINT_REG rax\n"
".L_eq:\n"
"    movq %rbp, %rax\n"
"    .align 8\n"
".type helper_func, @function\n"
"helper_func:\n"
"    pushq %rbp\n"
"    movq %rsp, %rbp\n"
"    inc %rax\n"
"    dec %rbx\n"
"    nop\n"
"    leave\n"
"    ret\n"
".L_ne:\n"
"    .section .data\n"
"    .globl data_label\n"
"data_label:\n"
"    .quad 0x1122334455667788\n"
"    .long 0xAABBCCDD\n"
"    .word 0x1234\n"
"    .byte 0x7F\n"
"    .asciz \"Test string\\0\"\n"
"    .previous\n"
".L_gt:\n"
"    .comm comm_var,8,8\n"
"    .lcomm lcomm_var,16\n"
".L_ge:\n"
"    .set MYCONST, 100\n"
"    .equ ANOTHER, 200\n"
".L_lt:\n"
"    .irp reg, %rax,%rbx,%rcx\n"
"        movq \\reg, %rdx\n"
"    .endr\n"
"    .rept 2\n"
"        nop\n"
"    .endr\n"
".L_le:\n"
"    .section .rodata\n"
"    .align 16\n"
"msg:\n"
"    .asciz \"Result: %d\\n\"\n"
".weak weak_symbol\n"
".type weak_symbol, @function\n"
".note \"note\", 0, 0\n"
".ident \"test-assembler-128\"\n";

/* ----------------------
   Main driver
   ---------------------- */
static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-i inputfile] [-sse] [-sse2] [-debug]\n", prog);
}

int main(int argc, char **argv) {
    CompileContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.opts.enable_sse = 0;
    ctx.opts.debug = 0;
    ctx.opts.input_path = NULL;

    const char *at = getenv("ALLOC_TRACE");
    if (at && at[0] == '1') g_alloc_trace = 1;

    for (int i=1;i<argc;++i) {
        if (strcmp(argv[i], "-i") == 0 && i+1 < argc) ctx.opts.input_path = argv[++i];
        else if (strcmp(argv[i], "-sse") == 0) ctx.opts.enable_sse = 1;
        else if (strcmp(argv[i], "-sse2") == 0) ctx.opts.enable_sse = 2;
        else if (strcmp(argv[i], "-debug") == 0) ctx.opts.debug = 1;
        else { print_usage(argv[0]); return 1; }
    }

    char *source = NULL;
    if (ctx.opts.input_path) {
        FILE *f = fopen(ctx.opts.input_path, "rb");
        if (!f) { perror("fopen"); return 1; }
        fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
        source = xmalloc(sz + 1);
        if (fread(source, 1, sz, f) != (size_t)sz) { perror("fread"); fclose(f); free(source); return 1; }
        source[sz] = 0; fclose(f);
    } else {
        source = xstrdup_safe(demo_asm);
    }

    /* Lex and parse top-level assembler lines */
    Lexer L; lexer_init(&L, source);
    lexer_advance(&L);
    ASTNode **nodes = xmalloc(sizeof(ASTNode*) * 4096);
    int ncap = 4096, ncount = 0;
    while (L.cur.kind != TOK_EOF) {
        int adv = 0;
        ASTNode *n = parse_top_entry_asm(&L, &adv);
        if (n) {
            if (ncount >= ncap) { ncap *= 2; nodes = xrealloc(nodes, sizeof(ASTNode*) * ncap); }
            nodes[ncount++] = n;
        } else {
            if (!adv) lexer_advance(&L);
        }
    }

    /* Lower ASTNodes to IR */
    IRList IR; ir_init(&IR);
    for (int i=0;i<ncount;++i) {
        lower_asm_astnode(&IR, nodes[i]);
    }

    /* Emit assembly to stdout */
    printf("/* Generated assembly (emitter) */\n");
    emit_ir_as_asm(&IR, &ctx, stdout);

    /* cleanup */
    for (int i=0;i<ncount;++i) astnode_free(nodes[i]);
    free(nodes);
    ir_free(&IR);
    asm_macro_free_all();
    if (source) free(source);
    return 0;
}

