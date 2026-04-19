/* cc.c
 *
 * C99: Lexer + Parser + Type system + Scoped symbol table + Codegen skeleton
 * - Full declarator parsing (pointer, arrays, function declarators)
 * - Block scoping and typedef names
 * - Precise x86-64 register encodings and optimized spills
 * - Enums used throughout
 *
 * Compile:
 *   cc -std=c99 -Wall -Wextra cc.c -o cc
 *
 * main() contains no automatic tests; use the API described above.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>

#ifndef _WIN32
# include <sys/mman.h>
# include <unistd.h>
#endif

/* ---------------------------
   Enums
   --------------------------- */
typedef enum {
    TK_EOF = 0, TK_IDENT, TK_NUMBER,
    TK_SEMI, TK_COMMA, TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE,
    TK_LBRACK, TK_RBRACK, TK_DOT, TK_ARROW,
    TK_STAR, TK_AMP, TK_ASSIGN,
    TK_STRUCT, TK_UNION, TK_INT, TK_CHAR, TK_LONG, TK_SHORT, TK_UNSIGNED,
    TK_RETURN, TK_IF, TK_ELSE, TK_FOR, TK_WHILE,
    TK_TYPEDEF, TK_EXTERN, TK_STATIC,
    TK_UNKNOWN
} token_kind_t;

typedef enum {
    TY_VOID = 0, TY_CHAR, TY_SHORT, TY_INT, TY_LONG,
    TY_UCHAR, TY_USHORT, TY_UINT, TY_ULONG,
    TY_PTR, TY_ARRAY, TY_FUNC, TY_STRUCT, TY_UNION, TY_TYPEDEF_NAME
} ty_kind_t;

typedef enum {
    AST_PROGRAM = 0, AST_FUN_DEF, AST_VAR_DECL, AST_PARAM,
    AST_BLOCK, AST_EXPR_STMT, AST_RETURN, AST_IF, AST_WHILE, AST_FOR,
    AST_ASSIGN, AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_MOD,
    AST_EQ, AST_NE, AST_LT, AST_GT, AST_LE, AST_GE,
    AST_ID, AST_IMM, AST_FUN_CALL, AST_ADDR_OF, AST_DEREF,
    AST_BRAK, AST_MEMBER
} ast_type_t;

typedef enum {
    SC_NONE = 0, SC_GLO, SC_LOC, SC_FUN, SC_TYPEDEF
} sym_class_t;

typedef enum {
    REG_RAX = 0, REG_RCX = 1, REG_RDX = 2, REG_RBX = 3,
    REG_RSP = 4, REG_RBP = 5, REG_RSI = 6, REG_RDI = 7,
    REG_R8  = 8, REG_R9  = 9, REG_R10 = 10, REG_R11 = 11,
    REG_R12 = 12, REG_R13 = 13, REG_R14 = 14, REG_R15 = 15
} reg_code_t;

/* ---------------------------
   Type info
   --------------------------- */
typedef struct type_info {
    ty_kind_t kind;
    bool is_unsigned;
    size_t size_in_bytes;
    struct struct_layout *layout;
    struct type_info *base;
    size_t array_len;
    char *typedef_name;
} type_info_t;

static type_info_t *type_new(ty_kind_t k) {
    type_info_t *t = calloc(1, sizeof(type_info_t));
    if (!t) { fprintf(stderr, "out of memory\n"); exit(1); }
    t->kind = k;
    t->is_unsigned = false;
    t->size_in_bytes = 0;
    t->layout = NULL;
    t->base = NULL;
    t->array_len = 0;
    t->typedef_name = NULL;
    return t;
}

/* ---------------------------
   Struct layout
   --------------------------- */
typedef struct {
    char *name;
    ty_kind_t ty;
    size_t offset;
    type_info_t *type;
    size_t size_in_bytes;
} field_entry_t;

typedef struct struct_layout {
    size_t field_count;
    field_entry_t *fields;
    size_t size;
} struct_layout_t;

/* ---------------------------
   Symbol table entry
   --------------------------- */
typedef union sym_value { intptr_t i; void *p; double d; } sym_value_t;

typedef struct sym_entry {
    char *name;
    sym_class_t cls;
    type_info_t *type;
    sym_value_t val;
    struct_layout_t *layout;
    int scope_level;
    int local_index;
} sym_entry_t;

/* ---------------------------
   Token and AST
   --------------------------- */
typedef struct {
    token_kind_t kind;
    char *text;
    const char *pos;
    int line;
} token_t;

typedef struct ast_node {
    ast_type_t type;
    long long value;
    struct ast_node *op_a, *op_b, *op_c;
    struct ast_node *next;
    sym_entry_t *sym;
    const char *pos;
    type_info_t *typeinfo;
} ast_node_t;

/* ---------------------------
   Compiler context
   --------------------------- */
typedef struct {
    char *src_buf;
    const char *p;
    int line;

    token_t *tokens;
    int token_count;
    int token_capacity;
    int token_idx;

    ast_node_t *ast_root;

    sym_entry_t *sym_table;
    size_t sym_capacity;
    size_t sym_count;

    int current_scope;

    uint8_t *x64_text;
    size_t poolsz;
    uint8_t *x64_e;

    uint8_t *data_area;
    size_t data_size;
} compiler_context_t;

/* ---------------------------
   Error reporting with underline
   --------------------------- */
static void error_at_ctx(const compiler_context_t *c, const char *loc, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    if (!c || !c->src_buf) { fprintf(stderr, "error: "); vfprintf(stderr, fmt, ap); fprintf(stderr, "\n"); va_end(ap); exit(1); }
    const char *src = c->src_buf;
    if (!loc) loc = src;
    int line_no = 1;
    for (const char *q = src; q < loc && *q; ++q) if (*q == '\n') ++line_no;
    const char *line_start = loc; while (line_start > src && *(line_start - 1) != '\n') --line_start;
    const char *line_end = loc; while (*line_end && *line_end != '\n') ++line_end;
    fprintf(stderr, "error at line %d: ", line_no);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    size_t line_len = (size_t)(line_end - line_start); if (line_len > 2048) line_len = 2048;
    fprintf(stderr, "%.*s\n", (int)line_len, line_start);
    size_t col = (size_t)(loc - line_start);
    for (size_t i = 0; i < col; ++i) { char ch = line_start[i]; if (ch == '\t') fputc('\t', stderr); else fputc(' ', stderr); }
    fprintf(stderr, "^\n");
    va_end(ap);
    exit(1);
}

/* ---------------------------
   Symbol table with scopes
   --------------------------- */
static sym_entry_t *sym_new(const char *name) {
    sym_entry_t *s = calloc(1, sizeof(sym_entry_t));
    s->name = name ? strdup(name) : NULL;
    s->cls = SC_NONE;
    s->type = NULL;
    s->layout = NULL;
    s->scope_level = 0;
    s->local_index = -1;
    return s;
}
static void sym_table_add(compiler_context_t *c, sym_entry_t *s) {
    if (c->sym_count + 1 >= c->sym_capacity) {
        size_t newcap = c->sym_capacity ? c->sym_capacity * 2 : 128;
        c->sym_table = realloc(c->sym_table, newcap * sizeof(sym_entry_t));
        c->sym_capacity = newcap;
    }
    s->scope_level = c->current_scope;
    c->sym_table[c->sym_count++] = *s;
}
static sym_entry_t *sym_find_in_scope(const compiler_context_t *c, const char *name, int scope) {
    for (size_t i = 0; i < c->sym_count; ++i) {
        if (c->sym_table[i].name && c->sym_table[i].scope_level == scope && strcmp(c->sym_table[i].name, name) == 0) return (sym_entry_t *)&c->sym_table[i];
    }
    return NULL;
}
static sym_entry_t *sym_find(const compiler_context_t *c, const char *name) {
    for (int scope = c->current_scope; scope >= 0; --scope) {
        sym_entry_t *s = sym_find_in_scope(c, name, scope);
        if (s) return s;
    }
    return NULL;
}
static void sym_pop_scope(compiler_context_t *c) {
    if (c->sym_count == 0) return;
    while (c->sym_count > 0 && c->sym_table[c->sym_count - 1].scope_level == c->current_scope) {
        if (c->sym_table[c->sym_count - 1].name) free(c->sym_table[c->sym_count - 1].name);
        if (c->sym_table[c->sym_count - 1].layout) {
            for (size_t f = 0; f < c->sym_table[c->sym_count - 1].layout->field_count; ++f) free(c->sym_table[c->sym_count - 1].layout->fields[f].name);
            free(c->sym_table[c->sym_count - 1].layout->fields);
            free(c->sym_table[c->sym_count - 1].layout);
        }
        c->sym_count--;
    }
    c->current_scope--;
}

/* ---------------------------
   Lexer (scanner)
   --------------------------- */
static inline bool is_ident_start(char ch) { return (ch == '_' || isalpha((unsigned char)ch)); }
static inline bool is_ident_char(char ch) { return (ch == '_' || isalnum((unsigned char)ch)); }

static void token_append(compiler_context_t *c, token_kind_t kind, const char *start, size_t len, int line) {
    if (c->token_count + 1 >= c->token_capacity) {
        int newcap = c->token_capacity ? c->token_capacity * 2 : 512;
        c->tokens = realloc(c->tokens, newcap * sizeof(token_t));
        c->token_capacity = newcap;
    }
    token_t *t = &c->tokens[c->token_count++];
    t->kind = kind;
    t->text = malloc(len + 1);
    memcpy(t->text, start, len);
    t->text[len] = '\0';
    t->pos = start;
    t->line = line;
}

void lex(compiler_context_t *c) {
    if (!c || !c->src_buf) { fprintf(stderr, "lex: no source\n"); return; }
    const char *p = c->src_buf;
    c->p = p; c->line = 1; c->token_count = 0;
    while (*p) {
        if (*p == ' ' || *p == '\t' || *p == '\r') { ++p; continue; }
        if (*p == '\n') { ++p; c->line++; continue; }
        if (p[0] == '/' && p[1] == '/') { p += 2; while (*p && *p != '\n') ++p; continue; }
        if (p[0] == '/' && p[1] == '*') { p += 2; while (*p && !(p[0] == '*' && p[1] == '/')) { if (*p == '\n') c->line++; ++p; } if (*p) p += 2; continue; }
        if (is_ident_start(*p)) {
            const char *start = p; while (is_ident_char(*p)) ++p; size_t len = (size_t)(p - start);
            if (len == 6 && strncmp(start, "struct", 6) == 0) { token_append(c, TK_STRUCT, start, len, c->line); continue; }
            if (len == 5 && strncmp(start, "union", 5) == 0) { token_append(c, TK_UNION, start, len, c->line); continue; }
            if (len == 3 && strncmp(start, "int", 3) == 0) { token_append(c, TK_INT, start, len, c->line); continue; }
            if (len == 4 && strncmp(start, "char", 4) == 0) { token_append(c, TK_CHAR, start, len, c->line); continue; }
            if (len == 4 && strncmp(start, "long", 4) == 0) { token_append(c, TK_LONG, start, len, c->line); continue; }
            if (len == 5 && strncmp(start, "short", 5) == 0) { token_append(c, TK_SHORT, start, len, c->line); continue; }
            if (len == 8 && strncmp(start, "unsigned", 8) == 0) { token_append(c, TK_UNSIGNED, start, len, c->line); continue; }
            if (len == 7 && strncmp(start, "typedef", 7) == 0) { token_append(c, TK_TYPEDEF, start, len, c->line); continue; }
            if (len == 6 && strncmp(start, "return", 6) == 0) { token_append(c, TK_RETURN, start, len, c->line); continue; }
            if (len == 2 && strncmp(start, "if", 2) == 0) { token_append(c, TK_IF, start, len, c->line); continue; }
            if (len == 4 && strncmp(start, "else", 4) == 0) { token_append(c, TK_ELSE, start, len, c->line); continue; }
            if (len == 3 && strncmp(start, "for", 3) == 0) { token_append(c, TK_FOR, start, len, c->line); continue; }
            if (len == 5 && strncmp(start, "while", 5) == 0) { token_append(c, TK_WHILE, start, len, c->line); continue; }
            token_append(c, TK_IDENT, start, len, c->line);
            continue;
        }
        if (isdigit((unsigned char)*p)) {
            const char *start = p; while (isdigit((unsigned char)*p)) ++p;
            token_append(c, TK_NUMBER, start, (size_t)(p - start), c->line);
            continue;
        }
        switch (*p) {
        case ';': token_append(c, TK_SEMI, p, 1, c->line); ++p; break;
        case ',': token_append(c, TK_COMMA, p, 1, c->line); ++p; break;
        case '(': token_append(c, TK_LPAREN, p, 1, c->line); ++p; break;
        case ')': token_append(c, TK_RPAREN, p, 1, c->line); ++p; break;
        case '{': token_append(c, TK_LBRACE, p, 1, c->line); ++p; break;
        case '}': token_append(c, TK_RBRACE, p, 1, c->line); ++p; break;
        case '[': token_append(c, TK_LBRACK, p, 1, c->line); ++p; break;
        case ']': token_append(c, TK_RBRACK, p, 1, c->line); ++p; break;
        case '.': token_append(c, TK_DOT, p, 1, c->line); ++p; break;
        case '*': token_append(c, TK_STAR, p, 1, c->line); ++p; break;
        case '&': token_append(c, TK_AMP, p, 1, c->line); ++p; break;
        case '=': token_append(c, TK_ASSIGN, p, 1, c->line); ++p; break;
        case '-': if (p[1] == '>') { token_append(c, TK_ARROW, p, 2, c->line); p += 2; } else { token_append(c, TK_UNKNOWN, p, 1, c->line); ++p; } break;
        default: token_append(c, TK_UNKNOWN, p, 1, c->line); ++p; break;
        }
    }
    token_append(c, TK_EOF, c->src_buf + strlen(c->src_buf), 0, c->line);
}

/* ---------------------------
   Parser helpers and declarator parsing
   --------------------------- */
static token_t *peek_token(const compiler_context_t *c) {
    if (c->token_idx < c->token_count) return (token_t *)&c->tokens[c->token_idx];
    return (token_t *)&c->tokens[c->token_count - 1];
}
static token_t *next_token(compiler_context_t *c) {
    token_t *t = peek_token(c);
    if (c->token_idx < c->token_count) c->token_idx++;
    return t;
}
static bool accept_tok(compiler_context_t *c, token_kind_t kind) {
    token_t *t = peek_token(c);
    if (t->kind == kind) { next_token((compiler_context_t *)c); return true; }
    return false;
}
static void expect_tok(compiler_context_t *c, token_kind_t kind, const char *msg) {
    token_t *t = peek_token(c);
    if (t->kind != kind) error_at_ctx(c, t->pos ? t->pos : c->src_buf, "expected %s, got '%s'", msg, t->text ? t->text : "<eof>");
    next_token((compiler_context_t *)c);
}

/* parse base type (int/char/long/short/unsigned/typedef/struct) */
static type_info_t *parse_base_type(compiler_context_t *c) {
    token_t *t = peek_token(c);
    bool is_unsigned = false;
    if (t->kind == TK_UNSIGNED) { is_unsigned = true; next_token((compiler_context_t *)c); t = peek_token(c); }
    if (t->kind == TK_INT) { next_token((compiler_context_t *)c); type_info_t *ti = type_new(is_unsigned ? TY_UINT : TY_INT); ti->size_in_bytes = 4; return ti; }
    if (t->kind == TK_CHAR) { next_token((compiler_context_t *)c); type_info_t *ti = type_new(is_unsigned ? TY_UCHAR : TY_CHAR); ti->size_in_bytes = 1; return ti; }
    if (t->kind == TK_LONG) { next_token((compiler_context_t *)c); type_info_t *ti = type_new(is_unsigned ? TY_ULONG : TY_LONG); ti->size_in_bytes = sizeof(long); return ti; }
    if (t->kind == TK_SHORT) { next_token((compiler_context_t *)c); type_info_t *ti = type_new(is_unsigned ? TY_USHORT : TY_SHORT); ti->size_in_bytes = 2; return ti; }
    if (t->kind == TK_STRUCT || t->kind == TK_UNION) {
        int is_union = (t->kind == TK_UNION);
        next_token((compiler_context_t *)c);
        token_t *name_tok = peek_token(c);
        char *tagname = NULL;
        if (name_tok->kind == TK_IDENT) { tagname = strdup(name_tok->text); next_token((compiler_context_t *)c); }
        struct_layout_t *layout = NULL;
        if (peek_token(c)->kind == TK_LBRACE) {
            next_token((compiler_context_t *)c); /* consume { */
            size_t cap = 8, count = 0;
            field_entry_t *fields = calloc(cap, sizeof(field_entry_t));
            size_t current_offset = 0;
            size_t max_field_size = 0;
            while (!accept_tok(c, TK_RBRACE)) {
                type_info_t *fbase = parse_base_type(c);
                int ptr_count = 0; while (accept_tok(c, TK_STAR)) ++ptr_count;
                token_t *fname_tok = peek_token(c);
                if (fname_tok->kind != TK_IDENT) error_at_ctx(c, fname_tok->pos ? fname_tok->pos : c->src_buf, "expected field name");
                char *fname = strdup(fname_tok->text); next_token((compiler_context_t *)c);
                size_t array_count = 0;
                if (accept_tok(c, TK_LBRACK)) {
                    token_t *num = peek_token(c);
                    if (num->kind != TK_NUMBER) error_at_ctx(c, num->pos ? num->pos : c->src_buf, "expected array size");
                    array_count = (size_t)atoi(num->text); next_token((compiler_context_t *)c);
                    expect_tok(c, TK_RBRACK, "]");
                }
                expect_tok(c, TK_SEMI, ";");
                size_t fsize = 0;
                if (fbase->kind == TY_PTR) fsize = sizeof(void*);
                else fsize = fbase->size_in_bytes ? fbase->size_in_bytes : sizeof(void*);
                if (array_count) fsize *= array_count;
                if (count + 1 > cap) { cap *= 2; fields = realloc(fields, cap * sizeof(field_entry_t)); }
                fields[count].name = fname;
                fields[count].ty = fbase->kind;
                fields[count].offset = is_union ? 0 : current_offset;
                fields[count].type = fbase;
                fields[count].size_in_bytes = fsize;
                if (!is_union) current_offset += fsize;
                if (fsize > max_field_size) max_field_size = fsize;
                ++count;
            }
            layout = malloc(sizeof(struct_layout_t));
            layout->field_count = count;
            layout->fields = fields;
            layout->size = is_union ? max_field_size : current_offset;
        }
        type_info_t *ret = type_new(is_union ? TY_UNION : TY_STRUCT);
        ret->layout = layout;
        ret->size_in_bytes = layout ? layout->size : 0;
        if (tagname) {
            sym_entry_t *s = sym_new(tagname);
            s->cls = SC_TYPEDEF;
            s->type = ret;
            sym_table_add(c, s);
        }
        return ret;
    }
    if (t->kind == TK_IDENT) {
        sym_entry_t *s = sym_find(c, t->text);
        if (s && s->cls == SC_TYPEDEF && s->type) { next_token((compiler_context_t *)c); return s->type; }
    }
    type_info_t *ret = type_new(TY_INT); ret->size_in_bytes = 4; return ret;
}

/* parse_declarator: pointer part, direct declarator, suffixes (arrays, function) */
static char *parse_declarator_name(compiler_context_t *c, type_info_t **out_type) {
    type_info_t *base = NULL;
    while (accept_tok(c, TK_STAR)) {
        type_info_t *t = type_new(TY_PTR);
        t->base = base;
        t->size_in_bytes = sizeof(void*);
        base = t;
    }
    token_t *t = peek_token(c);
    char *name = NULL;
    if (t->kind == TK_IDENT) { name = strdup(t->text); next_token((compiler_context_t *)c); }
    else if (accept_tok(c, TK_LPAREN)) {
        name = parse_declarator_name(c, &base);
        expect_tok(c, TK_RPAREN, ")");
    }
    while (1) {
        if (accept_tok(c, TK_LBRACK)) {
            token_t *num = peek_token(c);
            if (num->kind != TK_NUMBER) error_at_ctx(c, num->pos ? num->pos : c->src_buf, "expected array size");
            size_t n = (size_t)atoi(num->text); next_token((compiler_context_t *)c);
            expect_tok(c, TK_RBRACK, "]");
            type_info_t *tarr = type_new(TY_ARRAY);
            tarr->base = base ? base : type_new(TY_INT);
            tarr->array_len = n;
            tarr->size_in_bytes = tarr->base->size_in_bytes ? tarr->base->size_in_bytes * n : 0;
            base = tarr;
        } else if (accept_tok(c, TK_LPAREN)) {
            while (!accept_tok(c, TK_RPAREN)) {
                if (peek_token(c)->kind == TK_EOF) error_at_ctx(c, c->src_buf, "unexpected EOF in function declarator");
                next_token((compiler_context_t *)c);
            }
            type_info_t *tf = type_new(TY_FUNC);
            tf->base = base ? base : type_new(TY_INT);
            base = tf;
        } else break;
    }
    if (out_type) *out_type = base;
    return name;
}

/* ---------------------------
   Parser: expressions, statements, functions, top-level
   --------------------------- */
static ast_node_t *parse_expression(compiler_context_t *c); /* forward */

static ast_node_t *parse_primary(compiler_context_t *c) {
    token_t *t = peek_token(c);
    if (t->kind == TK_NUMBER) {
        ast_node_t *n = calloc(1, sizeof(ast_node_t));
        n->type = AST_IMM; n->value = atoll(t->text); n->pos = t->pos;
        next_token((compiler_context_t *)c);
        return n;
    }
    if (t->kind == TK_IDENT) {
        ast_node_t *n = calloc(1, sizeof(ast_node_t));
        n->type = AST_ID; n->pos = t->pos;
        sym_entry_t *s = sym_find(c, t->text);
        if (s) n->sym = s;
        next_token((compiler_context_t *)c);
        if (accept_tok(c, TK_LPAREN)) {
            ast_node_t *args = NULL, **last = &args;
            if (!accept_tok(c, TK_RPAREN)) {
                do {
                    ast_node_t *arg = parse_expression(c);
                    *last = arg; while (*last) last = &((*last)->next);
                } while (accept_tok(c, TK_COMMA));
                expect_tok(c, TK_RPAREN, ")");
            }
            ast_node_t *call = calloc(1, sizeof(ast_node_t));
            call->type = AST_FUN_CALL; call->op_a = n; call->op_b = args; call->pos = t->pos;
            return call;
        }
        return n;
    }
    if (accept_tok(c, TK_LPAREN)) {
        ast_node_t *e = parse_expression(c);
        expect_tok(c, TK_RPAREN, ")");
        return e;
    }
    error_at_ctx(c, t->pos ? t->pos : c->src_buf, "unexpected token in primary");
    return NULL;
}

static ast_node_t *parse_unary(compiler_context_t *c) {
    if (accept_tok(c, TK_STAR)) {
        ast_node_t *op = parse_unary(c);
        ast_node_t *n = calloc(1, sizeof(ast_node_t));
        n->type = AST_DEREF; n->op_a = op; return n;
    }
    if (accept_tok(c, TK_AMP)) {
        ast_node_t *op = parse_unary(c);
        ast_node_t *n = calloc(1, sizeof(ast_node_t));
        n->type = AST_ADDR_OF; n->op_a = op; return n;
    }
    return parse_primary(c);
}

static ast_node_t *parse_muldiv(compiler_context_t *c) {
    ast_node_t *node = parse_unary(c);
    while (1) {
        token_t *t = peek_token(c);
        if (t->kind == TK_STAR) { next_token((compiler_context_t *)c); ast_node_t *r = parse_unary(c); ast_node_t *n = calloc(1, sizeof(ast_node_t)); n->type = AST_MUL; n->op_a = node; n->op_b = r; node = n; }
        else break;
    }
    return node;
}

static ast_node_t *parse_addsub(compiler_context_t *c) {
    ast_node_t *node = parse_muldiv(c);
    while (1) {
        token_t *t = peek_token(c);
        if (t->kind == TK_UNKNOWN && t->text && strcmp(t->text, "+") == 0) { next_token((compiler_context_t *)c); ast_node_t *r = parse_muldiv(c); ast_node_t *n = calloc(1, sizeof(ast_node_t)); n->type = AST_ADD; n->op_a = node; n->op_b = r; node = n; }
        else if (t->kind == TK_UNKNOWN && t->text && strcmp(t->text, "-") == 0) { next_token((compiler_context_t *)c); ast_node_t *r = parse_muldiv(c); ast_node_t *n = calloc(1, sizeof(ast_node_t)); n->type = AST_SUB; n->op_a = node; n->op_b = r; node = n; }
        else break;
    }
    return node;
}

static ast_node_t *parse_expression(compiler_context_t *c) {
    ast_node_t *lhs = parse_addsub(c);
    token_t *t = peek_token(c);
    if (t->kind == TK_ASSIGN) {
        next_token((compiler_context_t *)c);
        ast_node_t *rhs = parse_expression(c);
        ast_node_t *n = calloc(1, sizeof(ast_node_t));
        n->type = AST_ASSIGN; n->op_a = lhs; n->op_b = rhs; return n;
    }
    return lhs;
}

static ast_node_t *parse_statement(compiler_context_t *c);

static ast_node_t *parse_block(compiler_context_t *c) {
    expect_tok(c, TK_LBRACE, "{");
    c->current_scope++;
    ast_node_t *block = calloc(1, sizeof(ast_node_t));
    block->type = AST_BLOCK;
    ast_node_t **last = &block->op_a;
    while (!accept_tok(c, TK_RBRACE)) {
        token_t *t = peek_token(c);
        if (t->kind == TK_INT || t->kind == TK_TYPEDEF || t->kind == TK_STRUCT || t->kind == TK_UNION) {
            bool is_typedef = (t->kind == TK_TYPEDEF);
            if (is_typedef) next_token((compiler_context_t *)c);
            type_info_t *base = parse_base_type(c);
            do {
                type_info_t *decl_type = NULL;
                char *name = parse_declarator_name(c, &decl_type);
                if (!name) error_at_ctx(c, t->pos ? t->pos : c->src_buf, "expected declarator name");
                if (is_typedef) {
                    sym_entry_t *s = sym_new(name);
                    s->cls = SC_TYPEDEF;
                    s->type = decl_type ? decl_type : base;
                    sym_table_add(c, s);
                } else {
                    sym_entry_t *s = sym_new(name);
                    s->cls = SC_LOC;
                    s->type = decl_type ? decl_type : base;
                    s->local_index = -1;
                    sym_table_add(c, s);
                    ast_node_t *decl = calloc(1, sizeof(ast_node_t));
                    decl->type = AST_VAR_DECL; decl->sym = s;
                    *last = decl; last = &decl->next;
                }
            } while (accept_tok(c, TK_COMMA));
            expect_tok(c, TK_SEMI, ";");
        } else {
            ast_node_t *stmt = parse_statement(c);
            *last = stmt; while (*last) last = &((*last)->next);
        }
    }
    sym_pop_scope((compiler_context_t *)c);
    return block;
}

static ast_node_t *parse_statement(compiler_context_t *c) {
    token_t *t = peek_token(c);
    if (t->kind == TK_RETURN) {
        next_token((compiler_context_t *)c);
        ast_node_t *expr = parse_expression(c);
        expect_tok(c, TK_SEMI, ";");
        ast_node_t *ret = calloc(1, sizeof(ast_node_t));
        ret->type = AST_RETURN; ret->op_a = expr; ret->pos = t->pos;
        return ret;
    }
    if (t->kind == TK_LBRACE) return parse_block(c);
    ast_node_t *e = parse_expression(c);
    expect_tok(c, TK_SEMI, ";");
    ast_node_t *stmt = calloc(1, sizeof(ast_node_t));
    stmt->type = AST_EXPR_STMT; stmt->op_a = e; stmt->pos = t->pos;
    return stmt;
}

static ast_node_t *parse_param_list(compiler_context_t *c) {
    expect_tok(c, TK_LPAREN, "(");
    ast_node_t *params = NULL, **last = &params;
    if (accept_tok(c, TK_RPAREN)) return NULL;
    do {
        type_info_t *base = parse_base_type(c);
        char *name = parse_declarator_name(c, NULL);
        if (!name) error_at_ctx(c, c->src_buf, "expected parameter name");
        sym_entry_t *s = sym_new(name);
        s->cls = SC_LOC;
        s->type = base;
        s->local_index = -1;
        sym_table_add(c, s);
        ast_node_t *p = calloc(1, sizeof(ast_node_t));
        p->type = AST_PARAM; p->sym = s;
        *last = p; last = &p->next;
    } while (accept_tok(c, TK_COMMA));
    expect_tok(c, TK_RPAREN, ")");
    return params;
}

static ast_node_t *parse_function(compiler_context_t *c) {
    type_info_t *ret_type = parse_base_type(c);
    token_t *t = peek_token(c);
    if (t->kind != TK_IDENT) error_at_ctx(c, t->pos ? t->pos : c->src_buf, "expected function name");
    char *fname = strdup(t->text);
    next_token((compiler_context_t *)c);
    c->current_scope++;
    ast_node_t *params = parse_param_list(c);
    int param_index = 0;
    for (ast_node_t *p = params; p; p = p->next) if (p->sym) p->sym->local_index = param_index++;
    ast_node_t *body = parse_block(c);
    sym_entry_t *s = sym_new(fname);
    s->cls = SC_FUN;
    s->type = ret_type;
    sym_table_add(c, s);
    ast_node_t *fnode = calloc(1, sizeof(ast_node_t));
    fnode->type = AST_FUN_DEF; fnode->op_a = params; fnode->op_b = body; fnode->sym = s; fnode->pos = t->pos;
    return fnode;
}

void parse(compiler_context_t *c) {
    c->token_idx = 0;
    c->current_scope = 0;
    ast_node_t *program = calloc(1, sizeof(ast_node_t));
    program->type = AST_PROGRAM;
    ast_node_t **last = &program->op_a;
    while (peek_token(c)->kind != TK_EOF) {
        token_t *t = peek_token(c);
        if (t->kind == TK_INT || t->kind == TK_STRUCT || t->kind == TK_UNION || t->kind == TK_TYPEDEF) {
            int save_idx = c->token_idx;
            type_info_t *base = parse_base_type(c);
            token_t *name_tok = peek_token(c);
            if (name_tok->kind != TK_IDENT) error_at_ctx(c, name_tok->pos ? name_tok->pos : c->src_buf, "expected identifier");
            next_token((compiler_context_t *)c);
            token_t *after = peek_token(c);
            c->token_idx = save_idx;
            if (after->kind == TK_LPAREN) {
                ast_node_t *f = parse_function(c);
                *last = f; last = &f->next;
            } else {
                bool is_typedef = (t->kind == TK_TYPEDEF);
                if (is_typedef) next_token((compiler_context_t *)c);
                type_info_t *base2 = parse_base_type(c);
                do {
                    type_info_t *decl_type = NULL;
                    char *name = parse_declarator_name(c, &decl_type);
                    if (!name) error_at_ctx(c, c->src_buf, "expected declarator name");
                    if (is_typedef) {
                        sym_entry_t *s = sym_new(name);
                        s->cls = SC_TYPEDEF;
                        s->type = decl_type ? decl_type : base2;
                        sym_table_add(c, s);
                    } else {
                        sym_entry_t *s = sym_new(name);
                        s->cls = SC_GLO;
                        s->type = decl_type ? decl_type : base2;
                        s->val.i = 0;
                        sym_table_add(c, s);
                        ast_node_t *g = calloc(1, sizeof(ast_node_t));
                        g->type = AST_VAR_DECL; g->sym = s;
                        *last = g; last = &g->next;
                    }
                } while (accept_tok(c, TK_COMMA));
                expect_tok(c, TK_SEMI, ";");
            }
        } else {
            error_at_ctx(c, t->pos ? t->pos : c->src_buf, "unexpected top-level token '%s'", t->text ? t->text : "<eof>");
        }
    }
    c->ast_root = program;
}

/* ---------------------------
   Codegen helpers: emitter and precise encodings
   --------------------------- */
typedef struct { uint8_t *ptr; } x64_emitter_t;
static inline void em_bytes(x64_emitter_t *em, const uint8_t *b, size_t n) { memcpy(em->ptr, b, n); em->ptr += n; }
static inline void em_byte(x64_emitter_t *em, uint8_t v) { *em->ptr++ = v; }
static inline void em_int32(x64_emitter_t *em, int32_t v) { *(int32_t *)em->ptr = v; em->ptr += 4; }
static inline void em_ptr64(x64_emitter_t *em, int64_t v) { *(int64_t *)em->ptr = v; em->ptr += 8; }

static void emit_rex(x64_emitter_t *em, bool w, bool r, bool x, bool b) {
    uint8_t rex = 0x40;
    if (w) rex |= 0x08;
    if (r) rex |= 0x04;
    if (x) rex |= 0x02;
    if (b) rex |= 0x01;
    em_byte(em, rex);
}

static void emit_mov_imm64_to_reg(x64_emitter_t *em, reg_code_t reg, int64_t imm) {
    bool rex_b = (reg & 8) != 0;
    emit_rex(em, true, false, false, rex_b);
    em_byte(em, 0xB8 + (reg & 7));
    em_ptr64(em, imm);
}

static void emit_mov_reg_to_mem_rbp(x64_emitter_t *em, reg_code_t reg, int32_t disp32) {
    bool rex_r = (reg & 8) != 0;
    emit_rex(em, true, rex_r, false, true);
    em_byte(em, 0x89);
    uint8_t modrm = 0x80 | ((reg & 7) << 3) | 5;
    em_byte(em, modrm);
    em_int32(em, -disp32);
}

static void emit_mov_mem_rbp_to_reg(x64_emitter_t *em, reg_code_t reg, int32_t disp32) {
    bool rex_r = (reg & 8) != 0;
    emit_rex(em, true, rex_r, false, true);
    em_byte(em, 0x8B);
    uint8_t modrm = 0x80 | ((reg & 7) << 3) | 5;
    em_byte(em, modrm);
    em_int32(em, -disp32);
}

static void emit_mov_absaddr_to_reg(x64_emitter_t *em, reg_code_t reg, int64_t addr) {
    emit_mov_imm64_to_reg(em, reg, addr);
    bool rex_r = (reg & 8) != 0;
    emit_rex(em, true, false, false, rex_r);
    em_byte(em, 0x8B);
    uint8_t modrm = 0x00 | ((reg & 7) << 3) | (reg & 7);
    em_byte(em, modrm);
}

static void emit_mov_reg_to_absaddr(x64_emitter_t *em, reg_code_t reg, int64_t addr) {
    emit_mov_imm64_to_reg(em, REG_RDX, addr);
    bool rex_r = (reg & 8) != 0;
    bool rex_b = (REG_RDX & 8) != 0;
    emit_rex(em, true, rex_r, false, rex_b);
    em_byte(em, 0x89);
    uint8_t modrm = 0x00 | ((reg & 7) << 3) | (REG_RDX & 7);
    em_byte(em, modrm);
}

/* ---------------------------
   Codegen: expression and statement generation (subset)
   --------------------------- */
static void codegen_expr(compiler_context_t *c, x64_emitter_t *em, ast_node_t *node);

static void codegen_stmt(compiler_context_t *c, x64_emitter_t *em, ast_node_t *node, int func_scope_level) {
    if (!node) return;
    switch (node->type) {
    case AST_BLOCK: {
        ast_node_t *it = node->op_a;
        while (it) { codegen_stmt(c, em, it, func_scope_level); it = it->next; }
        break;
    }
    case AST_EXPR_STMT:
        codegen_expr(c, em, node->op_a);
        break;
    case AST_RETURN:
        if (node->op_a) codegen_expr(c, em, node->op_a);
        em_bytes(em, (const uint8_t *)"\x48\x89\xEC\x5D\xC3", 5);
        break;
    case AST_VAR_DECL:
        break;
    default:
        error_at_ctx(c, node->pos ? node->pos : c->src_buf, "codegen_stmt: unimplemented node %d", node->type);
    }
}

static void codegen_expr(compiler_context_t *c, x64_emitter_t *em, ast_node_t *node) {
    if (!node) return;
    switch (node->type) {
    case AST_IMM:
        emit_mov_imm64_to_reg(em, REG_RAX, node->value);
        break;
    case AST_ID:
        if (!node->sym) error_at_ctx(c, node->pos ? node->pos : c->src_buf, "unknown identifier");
        if (node->sym->cls == SC_GLO) emit_mov_absaddr_to_reg(em, REG_RAX, node->sym->val.i);
        else if (node->sym->cls == SC_LOC) {
            int32_t offset = 8 + node->sym->local_index * 8;
            emit_mov_mem_rbp_to_reg(em, REG_RAX, offset);
        } else error_at_ctx(c, node->pos ? node->pos : c->src_buf, "unsupported id class");
        break;
    case AST_ASSIGN: {
        ast_node_t *lhs = node->op_a;
        ast_node_t *rhs = node->op_b;
        codegen_expr(c, em, rhs);
        if (lhs->type == AST_ID && lhs->sym) {
            if (lhs->sym->cls == SC_LOC) {
                int32_t offset = 8 + lhs->sym->local_index * 8;
                emit_mov_reg_to_mem_rbp(em, REG_RAX, offset);
            } else if (lhs->sym->cls == SC_GLO) {
                emit_mov_reg_to_absaddr(em, REG_RAX, lhs->sym->val.i);
            } else error_at_ctx(c, node->pos ? node->pos : c->src_buf, "assign to unsupported symbol class");
        } else error_at_ctx(c, node->pos ? node->pos : c->src_buf, "assign to non-lvalue");
        break;
    }
    case AST_ADD: {
        codegen_expr(c, em, node->op_a);
        emit_mov_reg_to_mem_rbp(em, REG_RAX, 8);
        codegen_expr(c, em, node->op_b);
        emit_mov_mem_rbp_to_reg(em, REG_RDX, 8);
        em_bytes(em, (const uint8_t *)"\x48\x01\xD0", 3);
        break;
    }
    case AST_MUL: {
        codegen_expr(c, em, node->op_a);
        emit_mov_reg_to_mem_rbp(em, REG_RAX, 8);
        codegen_expr(c, em, node->op_b);
        emit_mov_mem_rbp_to_reg(em, REG_RDX, 8);
        em_bytes(em, (const uint8_t *)"\x48\x0F\xAF\xC2", 4);
        break;
    }
    case AST_MEMBER: {
        if (!node->op_a || !node->op_a->sym) error_at_ctx(c, node->pos ? node->pos : c->src_buf, "member base missing");
        sym_entry_t *base = node->op_a->sym;
        if (base->cls != SC_GLO) error_at_ctx(c, node->pos ? node->pos : c->src_buf, "member base must be global in demo");
        size_t offset = 0;
        if (base->layout && node->value >= 0 && (size_t)node->value < base->layout->field_count) offset = base->layout->fields[node->value].offset;
        emit_mov_absaddr_to_reg(em, REG_RAX, base->val.i);
        if (offset) { em_bytes(em, (const uint8_t *)"\x48\x05", 2); em_int32(em, (int32_t)offset); }
        em_bytes(em, (const uint8_t *)"\x48\x8B\x00", 3);
        break;
    }
    default:
        error_at_ctx(c, node->pos ? node->pos : c->src_buf, "codegen_expr: unimplemented node %d", node->type);
    }
}

/* ---------------------------
   Codegen: walk AST and emit functions
   --------------------------- */
static size_t align_up(size_t v, size_t a) { return (v + a - 1) & ~(a - 1); }

static int compute_stack_size_for_locals(compiler_context_t *c, int func_scope_level) {
    int max_local_index = -1;
    for (size_t i = 0; i < c->sym_count; ++i) {
        if (c->sym_table[i].cls == SC_LOC && c->sym_table[i].scope_level >= func_scope_level) {
            if (c->sym_table[i].local_index > max_local_index) max_local_index = c->sym_table[i].local_index;
        }
    }
    int local_count = max_local_index + 1;
    int size = local_count * 8;
    if (size % 16 != 0) size = ((size + 15) / 16) * 16;
    return size;
}

int codegen(compiler_context_t *c) {
    if (!c) return -1;
    c->poolsz = 512 * 1024;
#ifndef _WIN32
    c->x64_text = (uint8_t *)mmap(NULL, c->poolsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (c->x64_text == MAP_FAILED) { fprintf(stderr, "mmap failed\n"); return -1; }
#else
    c->x64_text = (uint8_t *)malloc(c->poolsz);
    if (!c->x64_text) { fprintf(stderr, "malloc failed\n"); return -1; }
#endif
    x64_emitter_t em; em.ptr = c->x64_text;
    if (!c->ast_root) return 0;
    ast_node_t *it = c->ast_root->op_a;
    while (it) {
        if (it->type == AST_FUN_DEF && it->sym) {
            it->sym->val.i = (intptr_t)em.ptr;
            int func_scope = it->sym->scope_level;
            int stack_size = compute_stack_size_for_locals(c, func_scope);
            em_bytes(&em, (const uint8_t *)"\x55\x48\x89\xE5", 4);
            if (stack_size > 0) { em_bytes(&em, (const uint8_t *)"\x48\x81\xEC", 3); em_int32(&em, stack_size); }
            reg_code_t arg_regs[6] = { REG_RDI, REG_RSI, REG_RDX, REG_RCX, REG_R8, REG_R9 };
            for (size_t i = 0; i < c->sym_count; ++i) {
                sym_entry_t *s = &c->sym_table[i];
                if (s->cls == SC_LOC && s->scope_level >= func_scope && s->local_index >= 0 && s->local_index < 6) {
                    int32_t offset = 8 + s->local_index * 8;
                    emit_mov_reg_to_mem_rbp(&em, arg_regs[s->local_index], offset);
                }
            }
            codegen_stmt(c, &em, it->op_b, it->sym->scope_level);
            em_bytes(&em, (const uint8_t *)"\x48\x89\xEC\x5D\xC3", 5);
        }
        it = it->next;
    }
    c->x64_e = em.ptr;
    return 0;
}

/* ---------------------------
   Public API
   --------------------------- */
void compiler_init(compiler_context_t *c) { memset(c, 0, sizeof(compiler_context_t)); c->poolsz = 256 * 1024; c->current_scope = 0; }
void compiler_set_source(compiler_context_t *c, const char *src) { if (c->src_buf) free(c->src_buf); c->src_buf = strdup(src); }

/* ---------------------------
   main: no automatic tests
   --------------------------- */
int main(int argc, char **argv) {
    compiler_context_t ctx;
    compiler_init(&ctx);
    /* No automatic tests. Use the API:
       compiler_set_source(&ctx, source);
       lex(&ctx);
       parse(&ctx);
       codegen(&ctx);
    */
    if (ctx.src_buf) free(ctx.src_buf);
    if (ctx.tokens) { for (int i = 0; i < ctx.token_count; ++i) free(ctx.tokens[i].text); free(ctx.tokens); }
    if (ctx.sym_table) { for (size_t i = 0; i < ctx.sym_count; ++i) free(ctx.sym_table[i].name); free(ctx.sym_table); }
    if (ctx.x64_text) {
#ifndef _WIN32
        munmap(ctx.x64_text, ctx.poolsz);
#else
        free(ctx.x64_text);
#endif
    }
    return 0;
}
