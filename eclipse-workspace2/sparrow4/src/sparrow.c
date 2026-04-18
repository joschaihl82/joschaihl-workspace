/*
 * sparrow.c
 *
 * Single-file JavaScript-like compiler that emits x86-64 AT&T assembly.
 * - Lexer, Parser -> AST
 * - Codegen -> x86-64 AT&T assembly (stdout)
 * - Compile-time JSON import: import_json("path", "prefix")
 * - Object & array literal support: parse { "k": v, ... } and [v, ...]
 *   and lower top-level var = { ... } to .rodata + globals.
 *
 * Notes:
 * - Top-level `var name = { ... }` is serialized to JSON and emitted into .rodata.
 *   Top-level keys become globals named `name_key` (sanitized) and are initialized.
 * - JSON parser ignores // and  comments.
 *
 * Usage:
 *   gcc -O2 -o sparrow sparrow.c
 *   ./sparrow demo.sparrow > out.s
 *   gcc -no-pie out.s -o demo -lc
 *   ./demo
 *
 * Educational code; not production-grade.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

/* ----------------------------------------------------------------------
 * CONFIG
 * ---------------------------------------------------------------------- */

#define MAX_LEXEME 4096
#define INITIAL_TOKEN_CAP 512
#define LABEL_RANDOM_MASK 0xFFFFF

/* ----------------------------------------------------------------------
 * UTILITIES & ERROR HANDLING
 * ---------------------------------------------------------------------- */

static char *global_input = NULL;
static const char *prog_name = "sparrow";

static void error_at(const char *loc, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = (int)(loc - global_input);
    fprintf(stderr, "\n--- COMPILE ERROR ---\n");
    if (global_input) {
        fprintf(stderr, "%s\n", global_input);
        fprintf(stderr, "%*s^ ", pos, "");
    }
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n---------------------\n");
    va_end(ap);
    exit(1);
}

static void fatal(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "\nFATAL: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

/* ----------------------------------------------------------------------
 * FILE IO
 * ---------------------------------------------------------------------- */

static char* read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0) sz = 0;
    rewind(f);
    char *buf = malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)sz, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

/* ----------------------------------------------------------------------
 * LEXER
 * ---------------------------------------------------------------------- */

typedef enum {
    TK_EOF = 0,
    TK_IDENT,
    TK_NUMBER,
    TK_STRING,
    /* keywords */
    TK_VAR, TK_FUNCTION, TK_RETURN, TK_IF, TK_ELSE, TK_WHILE, TK_FOR,
    /* punctuation */
    TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_LBRACKET, TK_RBRACKET, TK_COMMA, TK_SEMI, TK_COLON,
    /* operators */
    TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH,
    TK_ASSIGN, TK_EQ, TK_NEQ, TK_LT, TK_GT, TK_LE, TK_GE,
    TK_AND, TK_OR, TK_NOT,
    TK_UNKNOWN
} TokenKind;

typedef struct {
    TokenKind kind;
    char *lexeme;   /* allocated for identifiers and strings */
    double num_val;
    char *loc;      /* pointer into original input for error reporting */
} Token;

typedef struct {
    Token *arr;
    int count;
    int cap;
    int idx;
} TokenStream;

static void ts_init(TokenStream *ts) { ts->arr = NULL; ts->count = ts->cap = ts->idx = 0; }
static void ts_push(TokenStream *ts, Token t) {
    if (ts->count + 1 > ts->cap) {
        ts->cap = ts->cap ? ts->cap * 2 : INITIAL_TOKEN_CAP;
        ts->arr = realloc(ts->arr, ts->cap * sizeof(Token));
    }
    ts->arr[ts->count++] = t;
}
static Token ts_peek(TokenStream *ts) { return ts->arr[ts->idx]; }
static Token ts_next(TokenStream *ts) { return ts->arr[ts->idx++]; }
static int ts_eof(TokenStream *ts) { return ts->arr[ts->idx].kind == TK_EOF; }

static int is_ident_start(char c) { return isalpha((unsigned char)c) || c == '_'; }
static int is_ident_char(char c) { return isalnum((unsigned char)c) || c == '_'; }

static Token make_token(TokenKind k, const char *loc, const char *lex) {
    Token t;
    t.kind = k;
    t.loc = (char*)loc;
    t.lexeme = lex ? strdup(lex) : NULL;
    t.num_val = 0;
    return t;
}
static Token make_number_token(const char *loc, double v, const char *lex) {
    Token t = make_token(TK_NUMBER, loc, lex);
    t.num_val = v;
    return t;
}
static Token make_string_token(const char *loc, const char *s) {
    return make_token(TK_STRING, loc, s);
}

static TokenKind keyword_kind(const char *s) {
    if (!strcmp(s, "var")) return TK_VAR;
    if (!strcmp(s, "function")) return TK_FUNCTION;
    if (!strcmp(s, "return")) return TK_RETURN;
    if (!strcmp(s, "if")) return TK_IF;
    if (!strcmp(s, "else")) return TK_ELSE;
    if (!strcmp(s, "while")) return TK_WHILE;
    if (!strcmp(s, "for")) return TK_FOR;
    return TK_IDENT;
}

static TokenStream tokenize(const char *input) {
    global_input = (char*)input;
    TokenStream ts;
    ts_init(&ts);
    const char *p = input;

    while (*p) {
        if (isspace((unsigned char)*p)) { p++; continue; }
        if (p[0] == '/' && p[1] == '/') { p += 2; while (*p && *p != '\n') p++; continue; }
        if (p[0] == '/' && p[1] == '*') { p += 2; while (*p && !(p[0] == '*' && p[1] == '/')) p++; if (*p) p += 2; continue; }

        if (is_ident_start(*p)) {
            const char *s = p;
            while (is_ident_char(*p)) p++;
            int len = (int)(p - s);
            char *lex = strndup(s, len);
            TokenKind k = keyword_kind(lex);
            Token t = make_token(k, s, lex);
            free(lex);
            ts_push(&ts, t);
            continue;
        }

        if (isdigit((unsigned char)*p) || (*p == '.' && isdigit((unsigned char)p[1]))) {
            char *end;
            double v = strtod(p, &end);
            int len = (int)(end - p);
            char *lex = strndup(p, len);
            Token t = make_number_token(p, v, lex);
            free(lex);
            ts_push(&ts, t);
            p = end;
            continue;
        }

        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            const char *start = p;
            char buf[MAX_LEXEME];
            int bi = 0;
            while (*p && *p != quote) {
                if (*p == '\\') {
                    p++;
                    if (*p == 'n') buf[bi++] = '\n';
                    else if (*p == 't') buf[bi++] = '\t';
                    else if (*p == '\\') buf[bi++] = '\\';
                    else buf[bi++] = *p;
                    p++;
                } else {
                    buf[bi++] = *p++;
                }
                if (bi >= MAX_LEXEME - 1) break;
            }
            if (*p != quote) error_at((char*)p, "Unterminated string literal");
            buf[bi] = '\0';
            p++;
            Token t = make_string_token(start - 1, buf);
            ts_push(&ts, t);
            continue;
        }

        if (!strncmp(p, "==", 2)) { ts_push(&ts, make_token(TK_EQ, p, "==")); p += 2; continue; }
        if (!strncmp(p, "!=", 2)) { ts_push(&ts, make_token(TK_NEQ, p, "!=")); p += 2; continue; }
        if (!strncmp(p, "<=", 2)) { ts_push(&ts, make_token(TK_LE, p, "<=")); p += 2; continue; }
        if (!strncmp(p, ">=", 2)) { ts_push(&ts, make_token(TK_GE, p, ">=")); p += 2; continue; }
        if (!strncmp(p, "&&", 2)) { ts_push(&ts, make_token(TK_AND, p, "&&")); p += 2; continue; }
        if (!strncmp(p, "||", 2)) { ts_push(&ts, make_token(TK_OR, p, "||")); p += 2; continue; }

        TokenKind tk = TK_UNKNOWN;
        const char *loc = p;
        char lex[2] = {0, 0};
        switch (*p++) {
            case '+': tk = TK_PLUS; lex[0] = '+'; break;
            case '-': tk = TK_MINUS; lex[0] = '-'; break;
            case '*': tk = TK_STAR; lex[0] = '*'; break;
            case '/': tk = TK_SLASH; lex[0] = '/'; break;
            case '(': tk = TK_LPAREN; lex[0] = '('; break;
            case ')': tk = TK_RPAREN; lex[0] = ')'; break;
            case '{': tk = TK_LBRACE; lex[0] = '{'; break;
            case '}': tk = TK_RBRACE; lex[0] = '}'; break;
            case '[': tk = TK_LBRACKET; lex[0] = '['; break;
            case ']': tk = TK_RBRACKET; lex[0] = ']'; break;
            case ',': tk = TK_COMMA; lex[0] = ','; break;
            case ';': tk = TK_SEMI; lex[0] = ';'; break;
            case ':': tk = TK_COLON; lex[0] = ':'; break;
            case '=': tk = TK_ASSIGN; lex[0] = '='; break;
            case '<': tk = TK_LT; lex[0] = '<'; break;
            case '>': tk = TK_GT; lex[0] = '>'; break;
            case '!': tk = TK_NOT; lex[0] = '!'; break;
            default: error_at((char*)loc, "Unexpected character '%c'", *(loc));
        }
        ts_push(&ts, make_token(tk, loc, lex));
    }

    ts_push(&ts, make_token(TK_EOF, (char*)input + strlen(input), "<EOF>"));
    return ts;
}

/* ----------------------------------------------------------------------
 * AST
 * ---------------------------------------------------------------------- */

typedef enum {
    AST_PROGRAM,
    AST_FUNC_DECL,
    AST_VAR_DECL,
    AST_BLOCK,
    AST_RETURN,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_EXPR_STMT,
    AST_BINARY,
    AST_UNARY,
    AST_CALL,
    AST_IDENT,
    AST_NUMBER,
    AST_STRING,
    AST_OBJECT,
    AST_ARRAY,
    AST_EMPTY
} ASTKind;

typedef struct AST {
    ASTKind kind;
    Token token;
    struct AST **children;
    int child_count;
    /* node-specific */
    char *name;
    double num;
    char *str;
    char **params;   /* reused for object keys or function params */
    int param_count;
} AST;

static AST* ast_new(ASTKind k, Token t) {
    AST *n = calloc(1, sizeof(AST));
    n->kind = k;
    n->token = t;
    n->children = NULL;
    n->child_count = 0;
    n->name = NULL;
    n->num = 0.0;
    n->str = NULL;
    n->params = NULL;
    n->param_count = 0;
    return n;
}
static void ast_add_child(AST *parent, AST *child) {
    if (!parent || !child) return;
    parent->children = realloc(parent->children, sizeof(AST*) * (parent->child_count + 1));
    parent->children[parent->child_count++] = child;
}

/* ----------------------------------------------------------------------
 * PARSER (recursive descent)
 * ---------------------------------------------------------------------- */

typedef struct {
    TokenStream *ts;
} Parser;

static Token cur(Parser *p) { return p->ts->arr[p->ts->idx]; }
static int accept(Parser *p, TokenKind k) {
    if (p->ts->arr[p->ts->idx].kind == k) { p->ts->idx++; return 1; }
    return 0;
}
static Token expect(Parser *p, TokenKind k) {
    Token t = cur(p);
    if (t.kind != k) error_at(t.loc, "Expected token %d but got %d", k, t.kind);
    p->ts->idx++;
    return t;
}

/* Forward declarations */
static AST* parse_program(Parser *p);
static AST* parse_statement(Parser *p);
static AST* parse_expression(Parser *p);
static AST* parse_assignment(Parser *p);
static AST* parse_logical_or(Parser *p);
static AST* parse_logical_and(Parser *p);
static AST* parse_equality(Parser *p);
static AST* parse_relational(Parser *p);
static AST* parse_add(Parser *p);
static AST* parse_mul(Parser *p);
static AST* parse_unary(Parser *p);
static AST* parse_primary(Parser *p);
static AST* parse_function_decl(Parser *p);
static AST* parse_object_literal(Parser *p);
static AST* parse_array_literal(Parser *p);

/* Primary */
static AST* parse_primary(Parser *p) {
    Token t = cur(p);
    /* object literal as expression */
    if (t.kind == TK_LBRACE) {
        return parse_object_literal(p);
    }
    /* array literal as expression */
    if (t.kind == TK_LBRACKET) {
        return parse_array_literal(p);
    }
    if (accept(p, TK_LPAREN)) {
        AST *e = parse_expression(p);
        expect(p, TK_RPAREN);
        return e;
    }
    if (t.kind == TK_NUMBER) {
        Token nt = expect(p, TK_NUMBER);
        AST *n = ast_new(AST_NUMBER, nt);
        n->num = nt.num_val;
        return n;
    }
    if (t.kind == TK_STRING) {
        Token st = expect(p, TK_STRING);
        AST *n = ast_new(AST_STRING, st);
        n->str = strdup(st.lexeme ? st.lexeme : "");
        return n;
    }
    if (t.kind == TK_IDENT) {
        Token id = expect(p, TK_IDENT);
        AST *ident = ast_new(AST_IDENT, id);
        ident->name = strdup(id.lexeme ? id.lexeme : "");
        if (accept(p, TK_LPAREN)) {
            AST *call = ast_new(AST_CALL, id);
            if (!accept(p, TK_RPAREN)) {
                AST *arg = parse_expression(p);
                ast_add_child(call, arg);
                while (accept(p, TK_COMMA)) {
                    AST *a = parse_expression(p);
                    ast_add_child(call, a);
                }
                expect(p, TK_RPAREN);
            }
            ast_add_child(call, ident); /* callee as last child */
            return call;
        }
        return ident;
    }
    error_at(t.loc, "Expected primary expression");
    return NULL;
}

/* parse object literal: { "k": expr, ... } */
static AST* parse_object_literal(Parser *p) {
    expect(p, TK_LBRACE);
    AST *obj = ast_new(AST_OBJECT, cur(p));
    obj->params = NULL; obj->param_count = 0;
    obj->children = NULL; obj->child_count = 0;
    if (accept(p, TK_RBRACE)) return obj;
    while (1) {
        Token keytok = cur(p);
        if (keytok.kind != TK_STRING) error_at(keytok.loc, "Expected string key in object literal");
        expect(p, TK_STRING);
        obj->params = realloc(obj->params, sizeof(char*) * (obj->param_count + 1));
        obj->params[obj->param_count++] = strdup(keytok.lexeme ? keytok.lexeme : "");
        expect(p, TK_COLON);
        AST *val = parse_expression(p);
        ast_add_child(obj, val);
        if (accept(p, TK_COMMA)) continue;
        if (accept(p, TK_RBRACE)) break;
        Token t = cur(p);
        error_at(t.loc, "Expected ',' or '}' in object literal");
    }
    return obj;
}

/* parse array literal: [ expr, expr, ... ] */
static AST* parse_array_literal(Parser *p) {
    expect(p, TK_LBRACKET);
    AST *arr = ast_new(AST_ARRAY, cur(p));
    arr->children = NULL;
    arr->child_count = 0;
    if (accept(p, TK_RBRACKET)) return arr; /* empty array */
    while (1) {
        AST *val = parse_expression(p);
        ast_add_child(arr, val);
        if (accept(p, TK_COMMA)) continue;
        if (accept(p, TK_RBRACKET)) break;
        Token t = cur(p);
        error_at(t.loc, "Expected ',' or ']' in array literal");
    }
    return arr;
}

/* Unary */
static AST* parse_unary(Parser *p) {
    Token t = cur(p);
    if (accept(p, TK_PLUS)) return parse_unary(p);
    if (accept(p, TK_MINUS)) {
        AST *n = ast_new(AST_UNARY, t);
        n->token = t;
        n->token.lexeme = strdup("-");
        AST *child = parse_unary(p);
        ast_add_child(n, child);
        return n;
    }
    if (accept(p, TK_NOT)) {
        AST *n = ast_new(AST_UNARY, t);
        n->token = t;
        n->token.lexeme = strdup("!");
        AST *child = parse_unary(p);
        ast_add_child(n, child);
        return n;
    }
    return parse_primary(p);
}

/* Multiplicative */
static AST* parse_mul(Parser *p) {
    AST *node = parse_unary(p);
    while (cur(p).kind == TK_STAR || cur(p).kind == TK_SLASH) {
        Token op = cur(p);
        p->ts->idx++;
        AST *n = ast_new(AST_BINARY, op);
        n->token = op;
        ast_add_child(n, node);
        AST *r = parse_unary(p);
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Additive */
static AST* parse_add(Parser *p) {
    AST *node = parse_mul(p);
    while (cur(p).kind == TK_PLUS || cur(p).kind == TK_MINUS) {
        Token op = cur(p);
        p->ts->idx++;
        AST *n = ast_new(AST_BINARY, op);
        n->token = op;
        ast_add_child(n, node);
        AST *r = parse_mul(p);
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Relational */
static AST* parse_relational(Parser *p) {
    AST *node = parse_add(p);
    while (cur(p).kind == TK_LT || cur(p).kind == TK_GT || cur(p).kind == TK_LE || cur(p).kind == TK_GE) {
        Token op = cur(p);
        p->ts->idx++;
        AST *n = ast_new(AST_BINARY, op);
        n->token = op;
        ast_add_child(n, node);
        AST *r = parse_add(p);
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Equality */
static AST* parse_equality(Parser *p) {
    AST *node = parse_relational(p);
    while (cur(p).kind == TK_EQ || cur(p).kind == TK_NEQ) {
        Token op = cur(p);
        p->ts->idx++;
        AST *n = ast_new(AST_BINARY, op);
        n->token = op;
        ast_add_child(n, node);
        AST *r = parse_relational(p);
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Logical AND */
static AST* parse_logical_and(Parser *p) {
    AST *node = parse_equality(p);
    while (accept(p, TK_AND)) {
        Token op = { .kind = TK_AND, .lexeme = strdup("&&"), .loc = cur(p).loc };
        AST *n = ast_new(AST_BINARY, op);
        n->token = op;
        ast_add_child(n, node);
        AST *r = parse_equality(p);
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Logical OR */
static AST* parse_logical_or(Parser *p) {
    AST *node = parse_logical_and(p);
    while (accept(p, TK_OR)) {
        Token op = { .kind = TK_OR, .lexeme = strdup("||"), .loc = cur(p).loc };
        AST *n = ast_new(AST_BINARY, op);
        n->token = op;
        ast_add_child(n, node);
        AST *r = parse_logical_and(p);
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Assignment (right-associative) */
static AST* parse_assignment(Parser *p) {
    AST *node = parse_logical_or(p);
    if (accept(p, TK_ASSIGN)) {
        Token asg = { .kind = TK_ASSIGN, .lexeme = strdup("="), .loc = cur(p).loc };
        AST *n = ast_new(AST_BINARY, asg);
        ast_add_child(n, node);
        AST *r = parse_assignment(p);
        ast_add_child(n, r);
        return n;
    }
    return node;
}

static AST* parse_expression(Parser *p) {
    return parse_assignment(p);
}

/* Compound statement */
static AST* parse_compound(Parser *p) {
    expect(p, TK_LBRACE);
    AST *blk = ast_new(AST_BLOCK, cur(p));
    while (!accept(p, TK_RBRACE)) {
        AST *s = parse_statement(p);
        ast_add_child(blk, s);
    }
    return blk;
}

/* Statement */
static AST* parse_statement(Parser *p) {
    Token t = cur(p);
    if (accept(p, TK_LBRACE)) {
        p->ts->idx--; /* roll back; parse_compound expects '{' consumed */
        return parse_compound(p);
    }
    if (accept(p, TK_IF)) {
        AST *n = ast_new(AST_IF, cur(p));
        expect(p, TK_LPAREN);
        AST *cond = parse_expression(p);
        expect(p, TK_RPAREN);
        ast_add_child(n, cond);
        AST *then = parse_statement(p);
        ast_add_child(n, then);
        if (accept(p, TK_ELSE)) {
            AST *els = parse_statement(p);
            ast_add_child(n, els);
        }
        return n;
    }
    if (accept(p, TK_WHILE)) {
        AST *n = ast_new(AST_WHILE, cur(p));
        expect(p, TK_LPAREN);
        AST *cond = parse_expression(p);
        expect(p, TK_RPAREN);
        ast_add_child(n, cond);
        AST *body = parse_statement(p);
        ast_add_child(n, body);
        return n;
    }
    if (accept(p, TK_FOR)) {
        AST *n = ast_new(AST_FOR, cur(p));
        expect(p, TK_LPAREN);
        if (!accept(p, TK_SEMI)) {
            if (cur(p).kind == TK_VAR) {
                AST *decl = parse_statement(p);
                ast_add_child(n, decl);
            } else {
                AST *init = parse_expression(p);
                expect(p, TK_SEMI);
                ast_add_child(n, init);
            }
        } else {
            ast_add_child(n, NULL);
        }
        if (!accept(p, TK_SEMI)) {
            AST *cond = parse_expression(p);
            expect(p, TK_SEMI);
            ast_add_child(n, cond);
        } else {
            ast_add_child(n, NULL);
        }
        if (!accept(p, TK_RPAREN)) {
            AST *incr = parse_expression(p);
            expect(p, TK_RPAREN);
            ast_add_child(n, incr);
        } else {
            ast_add_child(n, NULL);
        }
        AST *body = parse_statement(p);
        ast_add_child(n, body);
        return n;
    }
    if (accept(p, TK_RETURN)) {
        AST *n = ast_new(AST_RETURN, cur(p));
        if (!accept(p, TK_SEMI)) {
            AST *e = parse_expression(p);
            ast_add_child(n, e);
            expect(p, TK_SEMI);
        }
        return n;
    }
    if (cur(p).kind == TK_VAR) {
        expect(p, TK_VAR);
        Token id = expect(p, TK_IDENT);
        AST *decl = ast_new(AST_VAR_DECL, id);
        decl->name = strdup(id.lexeme ? id.lexeme : "");
        if (accept(p, TK_ASSIGN)) {
            AST *init = parse_expression(p);
            ast_add_child(decl, init);
        }
        expect(p, TK_SEMI);
        return decl;
    }
    if (cur(p).kind == TK_FUNCTION) {
        return parse_function_decl(p);
    }
    AST *expr = parse_expression(p);
    expect(p, TK_SEMI);
    AST *stmt = ast_new(AST_EXPR_STMT, expr->token);
    ast_add_child(stmt, expr);
    return stmt;
}

/* Function declaration */
static AST* parse_function_decl(Parser *p) {
    Token fn_tok = expect(p, TK_FUNCTION);
    Token name_tok = expect(p, TK_IDENT);
    AST *fn = ast_new(AST_FUNC_DECL, name_tok);
    fn->name = strdup(name_tok.lexeme ? name_tok.lexeme : "");
    expect(p, TK_LPAREN);
    if (!accept(p, TK_RPAREN)) {
        while (1) {
            Token pid = expect(p, TK_IDENT);
            fn->params = realloc(fn->params, sizeof(char*) * (fn->param_count + 1));
            fn->params[fn->param_count++] = strdup(pid.lexeme ? pid.lexeme : "");
            if (accept(p, TK_COMMA)) continue;
            expect(p, TK_RPAREN);
            break;
        }
    }
    AST *body = parse_statement(p);
    ast_add_child(fn, body);
    return fn;
}

/* Program */
static AST* parse_program(Parser *p) {
    AST *prog = ast_new(AST_PROGRAM, cur(p));
    while (cur(p).kind != TK_EOF) {
        AST *s = parse_statement(p);
        ast_add_child(prog, s);
    }
    return prog;
}

/* ----------------------------------------------------------------------
 * STRING / GLOBAL / FUNCTION TABLES
 * ---------------------------------------------------------------------- */

/* String literal table (also used to store initializer markers) */
typedef struct StrEntry {
    char *s;
    int id;
    struct StrEntry *next;
} StrEntry;

static StrEntry *str_table = NULL;
static int str_table_count = 0;

static int add_string_literal(const char *s) {
    for (StrEntry *e = str_table; e; e = e->next) {
        if (!strcmp(e->s, s)) return e->id;
    }
    StrEntry *n = calloc(1, sizeof(StrEntry));
    n->s = strdup(s);
    n->id = str_table_count++;
    n->next = str_table;
    str_table = n;
    return n->id;
}

/* Global variables table */
typedef struct GVar {
    char *name;
    int id;
    struct GVar *next;
} GVar;

static GVar *gvars = NULL;
static int gvar_count = 0;

static int add_global_var(const char *name) {
    for (GVar *g = gvars; g; g = g->next) {
        if (!strcmp(g->name, name)) return g->id;
    }
    GVar *n = calloc(1, sizeof(GVar));
    n->name = strdup(name);
    n->id = gvar_count++;
    n->next = gvars;
    gvars = n;
    return n->id;
}

/* Function registry */
typedef struct FuncEntry {
    char *name;
    AST *fn;
    struct FuncEntry *next;
} FuncEntry;

static FuncEntry *funcs = NULL;

static void register_function(const char *name, AST *fn) {
    FuncEntry *f = calloc(1, sizeof(FuncEntry));
    f->name = strdup(name);
    f->fn = fn;
    f->next = funcs;
    funcs = f;
}

/* ----------------------------------------------------------------------
 * JSON PARSER (compile-time)
 * ---------------------------------------------------------------------- */

/* JSON kinds */
typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JSONKind;

typedef struct JSONVal {
    JSONKind kind;
    double num;
    char *str;
    int boolean;
    struct JSONVal **items;
    int nitems;
    char **keys; /* for objects */
} JSONVal;

/* Skip whitespace and comments (supports // and) */
static const char* json_skip_ws(const char *p) {
    for (;;) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (p[0] == '/' && p[1] == '/') {
            p += 2;
            while (*p && *p != '\n') p++;
            continue;
        }
        if (p[0] == '/' && p[1] == '*') {
            p += 2;
            while (*p && !(p[0] == '*' && p[1] == '/')) p++;
            if (*p) p += 2;
            continue;
        }
        break;
    }
    return p;
}

/* parse JSON string */
static char* json_parse_string(const char **pp) {
    const char *p = *pp;
    if (*p != '"' && *p != '\'') return NULL;
    char quote = *p++;
    char buf[8192];
    int bi = 0;
    while (*p && *p != quote) {
        if (*p == '\\') {
            p++;
            if (*p == 'n') buf[bi++] = '\n';
            else if (*p == 't') buf[bi++] = '\t';
            else if (*p == 'r') buf[bi++] = '\r';
            else if (*p == '"') buf[bi++] = '"';
            else if (*p == '\\') buf[bi++] = '\\';
            else buf[bi++] = *p;
            p++;
        } else {
            buf[bi++] = *p++;
        }
        if (bi >= (int)sizeof(buf)-1) break;
    }
    if (*p != quote) error_at((char*)p, "Unterminated JSON string");
    buf[bi] = '\0';
    p++;
    *pp = p;
    return strdup(buf);
}

/* parse number */
static double json_parse_number(const char **pp) {
    char *end;
    double v = strtod(*pp, &end);
    if (end == *pp) error_at((char*)*pp, "Invalid JSON number");
    *pp = end;
    return v;
}

/* forward */
static JSONVal* json_parse_value(const char **pp);

/* parse array */
static JSONVal* json_parse_array(const char **pp) {
    const char *p = *pp;
    if (*p != '[') error_at((char*)p, "Expected '[' for JSON array");
    p++;
    p = json_skip_ws(p);
    JSONVal *v = calloc(1, sizeof(JSONVal));
    v->kind = JSON_ARRAY;
    v->items = NULL; v->nitems = 0;
    if (*p == ']') { p++; *pp = p; return v; }
    while (1) {
        JSONVal *item = json_parse_value(&p);
        v->items = realloc(v->items, sizeof(JSONVal*) * (v->nitems + 1));
        v->items[v->nitems++] = item;
        p = json_skip_ws(p);
        if (*p == ',') { p++; p = json_skip_ws(p); continue; }
        if (*p == ']') { p++; break; }
        error_at((char*)p, "Expected ',' or ']' in JSON array");
    }
    *pp = p;
    return v;
}

/* parse object */
static JSONVal* json_parse_object(const char **pp) {
    const char *p = *pp;
    if (*p != '{') error_at((char*)p, "Expected '{' for JSON object");
    p++;
    p = json_skip_ws(p);
    JSONVal *v = calloc(1, sizeof(JSONVal));
    v->kind = JSON_OBJECT;
    v->items = NULL; v->nitems = 0; v->keys = NULL;
    if (*p == '}') { p++; *pp = p; return v; }
    while (1) {
        p = json_skip_ws(p);
        char *key = json_parse_string(&p);
        p = json_skip_ws(p);
        if (*p != ':') error_at((char*)p, "Expected ':' after key in JSON object");
        p++;
        p = json_skip_ws(p);
        JSONVal *val = json_parse_value(&p);
        v->keys = realloc(v->keys, sizeof(char*) * (v->nitems + 1));
        v->items = realloc(v->items, sizeof(JSONVal*) * (v->nitems + 1));
        v->keys[v->nitems] = key;
        v->items[v->nitems] = val;
        v->nitems++;
        p = json_skip_ws(p);
        if (*p == ',') { p++; p = json_skip_ws(p); continue; }
        if (*p == '}') { p++; break; }
        error_at((char*)p, "Expected ',' or '}' in JSON object");
    }
    *pp = p;
    return v;
}

/* parse literal */
static JSONVal* json_parse_literal(const char **pp) {
    const char *p = *pp;
    if (!strncmp(p, "true", 4)) {
        JSONVal *v = calloc(1, sizeof(JSONVal)); v->kind = JSON_BOOL; v->boolean = 1; *pp = p + 4; return v;
    }
    if (!strncmp(p, "false", 5)) {
        JSONVal *v = calloc(1, sizeof(JSONVal)); v->kind = JSON_BOOL; v->boolean = 0; *pp = p + 5; return v;
    }
    if (!strncmp(p, "null", 4)) {
        JSONVal *v = calloc(1, sizeof(JSONVal)); v->kind = JSON_NULL; *pp = p + 4; return v;
    }
    error_at((char*)p, "Unknown JSON literal");
    return NULL;
}

/* parse value */
static JSONVal* json_parse_value(const char **pp) {
    const char *p = json_skip_ws(*pp);
    if (*p == '"') {
        char *s = json_parse_string(&p);
        JSONVal *v = calloc(1, sizeof(JSONVal)); v->kind = JSON_STRING; v->str = s; *pp = p; return v;
    }
    if (*p == '{') return json_parse_object(pp);
    if (*p == '[') return json_parse_array(pp);
    if (*p == '-' || isdigit((unsigned char)*p)) {
        double num = json_parse_number(&p);
        JSONVal *v = calloc(1, sizeof(JSONVal)); v->kind = JSON_NUMBER; v->num = num; *pp = p; return v;
    }
    return json_parse_literal(pp);
}

/* parse whole text */
static JSONVal* json_parse_text(const char *text) {
    const char *p = text;
    JSONVal *v = json_parse_value(&p);
    p = json_skip_ws(p);
    if (*p != '\0') error_at((char*)p, "Trailing characters after JSON value");
    return v;
}

/* free JSONVal */
static void json_free(JSONVal *v) {
    if (!v) return;
    if (v->kind == JSON_STRING) free(v->str);
    if (v->kind == JSON_ARRAY) {
        for (int i = 0; i < v->nitems; i++) json_free(v->items[i]);
        free(v->items);
    }
    if (v->kind == JSON_OBJECT) {
        for (int i = 0; i < v->nitems; i++) {
            free(v->keys[i]);
            json_free(v->items[i]);
        }
        free(v->keys);
        free(v->items);
    }
    free(v);
}

/* sanitize label */
static char* sanitize_label(const char *s) {
    size_t n = strlen(s);
    char *out = malloc(n + 32);
    char *o = out;
    for (size_t i = 0; i < n; i++) {
        char c = s[i];
        if (isalnum((unsigned char)c) || c == '_') *o++ = c;
        else *o++ = '_';
    }
    *o = '\0';
    return out;
}

/* Emit JSON object top-level keys as globals.
   We store initializer markers in the string table with prefixes:
   - "INIT_NUM:<label>:<value>"
   - "INIT_PTR:<label>:STR<sid>"
*/
static void emit_json_object_as_globals(const char *prefix_label, JSONVal *obj, const char *json_text) {
    int full_id = add_string_literal(json_text);
    for (int i = 0; i < obj->nitems; i++) {
        const char *key = obj->keys[i];
        JSONVal *val = obj->items[i];
        size_t bufsize = strlen(prefix_label) + 1 + strlen(key) + 1;
        char *gname = malloc(bufsize);
        snprintf(gname, bufsize, "%s_%s", prefix_label, key);
        char *san = sanitize_label(gname);
        free(gname);
        add_global_var(san);
        if (val->kind == JSON_NUMBER) {
            long long iv = (long long)val->num;
            char initbuf[256];
            snprintf(initbuf, sizeof(initbuf), "INIT_NUM:%s:%lld", san, iv);
            add_string_literal(initbuf);
        } else if (val->kind == JSON_STRING) {
            int sid = add_string_literal(val->str);
            char initbuf[256];
            snprintf(initbuf, sizeof(initbuf), "INIT_PTR:%s:STR%d", san, sid);
            add_string_literal(initbuf);
        } else if (val->kind == JSON_BOOL) {
            char initbuf[256];
            snprintf(initbuf, sizeof(initbuf), "INIT_NUM:%s:%d", san, val->boolean ? 1 : 0);
            add_string_literal(initbuf);
        } else if (val->kind == JSON_NULL) {
            char initbuf[256];
            snprintf(initbuf, sizeof(initbuf), "INIT_NUM:%s:0", san);
            add_string_literal(initbuf);
        } else {
            /* arrays/objects: point to full JSON text for simplicity */
            char initbuf[256];
            snprintf(initbuf, sizeof(initbuf), "INIT_PTR:%s:STR%d", san, full_id);
            add_string_literal(initbuf);
        }
        free(san);
    }
}

/* process import_json at compile time */
static int process_import_json_compiletime(Token *path_tok, Token *prefix_tok, int *out_sid) {
    if (!path_tok || path_tok->kind != TK_STRING) return 0;
    const char *path = path_tok->lexeme;
    char *json_text = read_file(path);
    if (!json_text) error_at(path_tok->loc, "Cannot open JSON file '%s'", path);
    JSONVal *root = json_parse_text(json_text);
    char prefixbuf[256];
    if (prefix_tok && prefix_tok->kind == TK_STRING && prefix_tok->lexeme && prefix_tok->lexeme[0]) {
        strncpy(prefixbuf, prefix_tok->lexeme, sizeof(prefixbuf)-1);
        prefixbuf[sizeof(prefixbuf)-1] = '\0';
    } else {
        const char *p = strrchr(path, '/'); if (!p) p = path; else p++;
        char tmp[256]; strncpy(tmp, p, sizeof(tmp)-1); tmp[sizeof(tmp)-1] = '\0';
        char *dot = strrchr(tmp, '.'); if (dot) *dot = '\0';
        strncpy(prefixbuf, tmp, sizeof(prefixbuf)-1); prefixbuf[sizeof(prefixbuf)-1] = '\0';
    }
    char *san_prefix = sanitize_label(prefixbuf);
    if (root->kind == JSON_OBJECT) {
        emit_json_object_as_globals(san_prefix, root, json_text);
        int sid = add_string_literal(json_text);
        *out_sid = sid;
        free(san_prefix);
        json_free(root);
        free(json_text);
        return 1;
    } else {
        int sid = add_string_literal(json_text);
        *out_sid = sid;
        free(san_prefix);
        json_free(root);
        free(json_text);
        return 1;
    }
}

/* ----------------------------------------------------------------------
 * ASSEMBLY EMISSION HELPERS
 * ---------------------------------------------------------------------- */

static void asm_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static char* escape_for_asciz(const char *s) {
    size_t n = strlen(s);
    char *out = malloc(n * 4 + 1);
    char *o = out;
    for (size_t i = 0; i < n; i++) {
        unsigned char c = (unsigned char)s[i];
        if (c == '"') { *o++ = '\\'; *o++ = '"'; }
        else if (c == '\\') { *o++ = '\\'; *o++ = '\\'; }
        else if (c == '\n') { *o++ = '\\'; *o++ = 'n'; }
        else if (c == '\t') { *o++ = '\\'; *o++ = 't'; }
        else if (c < 32 || c > 126) { o += sprintf(o, "\\x%02x", c); }
        else *o++ = c;
    }
    *o = '\0';
    return out;
}

static int gen_label_id(void) { return rand() & LABEL_RANDOM_MASK; }

/* ----------------------------------------------------------------------
 * OBJECT & ARRAY LITERAL -> JSON SERIALIZER (for compile-time lowering)
 * ---------------------------------------------------------------------- */

/* Append helper */
static void json_append(char **outp, size_t *cap, size_t *len, const char *s) {
    size_t sl = strlen(s);
    if (*len + sl + 1 > *cap) {
        size_t ncap = (*cap == 0) ? (sl + 1024) : (*cap * 2 + sl);
        *outp = realloc(*outp, ncap);
        *cap = ncap;
    }
    memcpy(*outp + *len, s, sl);
    *len += sl;
    (*outp)[*len] = '\0';
}

/* forward */
static void ast_serialize_value(AST *v, char **outp, size_t *cap, size_t *len);

static void ast_object_serialize_to_json(AST *obj, char **outp, size_t *cap, size_t *len) {
    json_append(outp, cap, len, "{");
    for (int i = 0; i < obj->param_count; i++) {
        if (i) json_append(outp, cap, len, ",");
        json_append(outp, cap, len, "\"");
        const char *k = obj->params[i];
        for (const char *q = k; *q; q++) {
            if (*q == '"' || *q == '\\') {
                char esc[3] = {'\\', *q, 0};
                json_append(outp, cap, len, esc);
            } else {
                char ch[2] = {*q, 0};
                json_append(outp, cap, len, ch);
            }
        }
        json_append(outp, cap, len, "\":");
        ast_serialize_value(obj->children[i], outp, cap, len);
    }
    json_append(outp, cap, len, "}");
}

static void ast_array_serialize_to_json(AST *arr, char **outp, size_t *cap, size_t *len) {
    json_append(outp, cap, len, "[");
    for (int i = 0; i < arr->child_count; i++) {
        if (i) json_append(outp, cap, len, ",");
        ast_serialize_value(arr->children[i], outp, cap, len);
    }
    json_append(outp, cap, len, "]");
}

static void ast_serialize_value(AST *v, char **outp, size_t *cap, size_t *len) {
    if (!v) { json_append(outp, cap, len, "null"); return; }
    switch (v->kind) {
        case AST_NUMBER: {
            char buf[64];
            snprintf(buf, sizeof(buf), "%g", v->num);
            json_append(outp, cap, len, buf);
            break;
        }
        case AST_STRING: {
            json_append(outp, cap, len, "\"");
            for (const char *q = v->str ? v->str : ""; *q; q++) {
                if (*q == '"' || *q == '\\') {
                    char esc[3] = {'\\', *q, 0};
                    json_append(outp, cap, len, esc);
                } else if (*q == '\n') json_append(outp, cap, len, "\\n");
                else {
                    char ch[2] = {*q, 0};
                    json_append(outp, cap, len, ch);
                }
            }
            json_append(outp, cap, len, "\"");
            break;
        }
        case AST_OBJECT:
            ast_object_serialize_to_json(v, outp, cap, len);
            break;
        case AST_ARRAY:
            ast_array_serialize_to_json(v, outp, cap, len);
            break;
        case AST_CALL:
        case AST_IDENT:
        case AST_BINARY:
        case AST_UNARY:
            /* unsupported expression inside literal: emit null */
            json_append(outp, cap, len, "null");
            break;
        default:
            json_append(outp, cap, len, "null");
            break;
    }
}

/* ----------------------------------------------------------------------
 * CODEGEN (expressions and statements)
 * ---------------------------------------------------------------------- */

static void cg_emit_stmt(AST *node);
static void cg_emit_expr(AST *node);

/* Expression emitter: result in %rax */
static void cg_emit_expr(AST *node) {
    if (!node) return;
    switch (node->kind) {
        case AST_NUMBER: {
            long long v = (long long)node->num;
            asm_printf("\t# number %g\n", node->num);
            asm_printf("\tmovq $%lld, %%rax\n", v);
            break;
        }
        case AST_STRING: {
            int id = add_string_literal(node->str ? node->str : "");
            asm_printf("\tleaq .Lstr%d(%%rip), %%rax\n", id);
            break;
        }
        case AST_IDENT: {
            const char *lbl = node->name;
            add_global_var(lbl);
            asm_printf("\tmovq %s(%%rip), %%rax\n", lbl);
            break;
        }
        case AST_UNARY: {
            AST *child = node->children[0];
            cg_emit_expr(child);
            if (node->token.kind == TK_MINUS) {
                asm_printf("\tnegq %%rax\n");
            } else if (node->token.kind == TK_NOT) {
                asm_printf("\tcmpq $0, %%rax\n\tsete %%al\n\tmovzbq %%al, %%rax\n");
            } else {
                error_at(node->token.loc, "Unsupported unary op");
            }
            break;
        }
        case AST_BINARY: {
            AST *lhs = node->children[0];
            AST *rhs = node->children[1];
            cg_emit_expr(lhs);
            asm_printf("\tpushq %%rax\n");
            cg_emit_expr(rhs);
            asm_printf("\tpopq %%rbx\n"); /* rbx = left, rax = right */
            asm_printf("\tmovq %%rax, %%rcx\n"); /* rcx = right */
            const char *op = node->token.lexeme ? node->token.lexeme : "";
            if (!strcmp(op, "+")) {
                asm_printf("\taddq %%rbx, %%rcx\n\tmovq %%rcx, %%rax\n");
            } else if (!strcmp(op, "-")) {
                asm_printf("\tsubq %%rcx, %%rbx\n\tmovq %%rbx, %%rax\n");
            } else if (!strcmp(op, "*")) {
                asm_printf("\timulq %%rcx, %%rbx\n\tmovq %%rbx, %%rax\n");
            } else if (!strcmp(op, "/")) {
                asm_printf("\tmovq %%rbx, %%rax\n\tcqto\n\tidivq %%rcx\n");
            } else if (!strcmp(op, "==") || !strcmp(op, "!=") || !strcmp(op, "<") || !strcmp(op, ">") || !strcmp(op, "<=") || !strcmp(op, ">=")) {
                asm_printf("\tcmpq %%rcx, %%rbx\n");
                if (!strcmp(op, "==")) asm_printf("\tsete %%al\n");
                else if (!strcmp(op, "!=")) asm_printf("\tsetne %%al\n");
                else if (!strcmp(op, "<")) asm_printf("\tsetl %%al\n");
                else if (!strcmp(op, ">")) asm_printf("\tsetg %%al\n");
                else if (!strcmp(op, "<=")) asm_printf("\tsetle %%al\n");
                else if (!strcmp(op, ">=")) asm_printf("\tsetge %%al\n");
                asm_printf("\tmovzbq %%al, %%rax\n");
            } else {
                error_at(node->token.loc, "Unsupported binary op: %s", op);
            }
            break;
        }
        case AST_CALL: {
            int nargs = node->child_count - 1;
            AST *callee = node->children[node->child_count - 1];
            if (callee->kind != AST_IDENT) error_at(callee->token.loc, "Call target must be identifier");
            const char *fname = callee->name;

            /* Special handling for import_json at compile time */
            if (!strcmp(fname, "import_json")) {
                Token *path_tok = NULL;
                Token *prefix_tok = NULL;
                if (nargs >= 1 && node->children[0]->kind == AST_STRING) path_tok = &node->children[0]->token;
                if (nargs >= 2 && node->children[1]->kind == AST_STRING) prefix_tok = &node->children[1]->token;
                int sid = -1;
                if (path_tok) {
                    int ok = process_import_json_compiletime(path_tok, prefix_tok, &sid);
                    if (ok && sid >= 0) {
                        asm_printf("\tleaq .Lstr%d(%%rip), %%rax\n", sid);
                    } else {
                        asm_printf("\tmovq $0, %%rax\n");
                    }
                } else {
                    /* runtime fallback not implemented */
                    asm_printf("\tmovq $0, %%rax\n");
                }
                break;
            }

            if (!strcmp(fname, "print")) {
                char fmtbuf[512] = {0};
                int pos = 0;
                for (int i = 0; i < nargs; i++) {
                    AST *a = node->children[i];
                    if (a->kind == AST_STRING) pos += snprintf(fmtbuf + pos, sizeof(fmtbuf) - pos, "%%s");
                    else pos += snprintf(fmtbuf + pos, sizeof(fmtbuf) - pos, "%%g");
                    if (i + 1 < nargs) pos += snprintf(fmtbuf + pos, sizeof(fmtbuf) - pos, " ");
                }
                pos += snprintf(fmtbuf + pos, sizeof(fmtbuf) - pos, "\\n");
                int sid = add_string_literal(fmtbuf);
                for (int i = nargs - 1; i >= 0; i--) {
                    cg_emit_expr(node->children[i]);
                    asm_printf("\tpushq %%rax\n");
                }
                asm_printf("\tleaq .Lstr%d(%%rip), %%rax\n", sid);
                asm_printf("\tpushq %%rax\n");
                asm_printf("\tcall printf@PLT\n");
                asm_printf("\taddq $%d, %%rsp\n", (nargs + 1) * 8);
                asm_printf("\tmovq $0, %%rax\n");
            } else {
                for (int i = nargs - 1; i >= 0; i--) {
                    cg_emit_expr(node->children[i]);
                    asm_printf("\tpushq %%rax\n");
                }
                asm_printf("\tcall %s\n", fname);
                if (nargs > 0) asm_printf("\taddq $%d, %%rsp\n", nargs * 8);
            }
            break;
        }
        default:
            error_at(node->token.loc, "Unhandled expression kind in codegen: %d", node->kind);
    }
}

/* Statement emitter */
static void cg_emit_stmt(AST *node) {
    if (!node) return;
    switch (node->kind) {
        case AST_EMPTY: break;
        case AST_BLOCK:
            for (int i = 0; i < node->child_count; i++) cg_emit_stmt(node->children[i]);
            break;
        case AST_VAR_DECL: {
            const char *lbl = node->name;
            /* If initializer is an object literal, lower it at compile time */
            if (node->child_count > 0 && node->children[0]->kind == AST_OBJECT) {
                AST *obj = node->children[0];
                char *out = NULL; size_t cap = 0; size_t len = 0;
                ast_object_serialize_to_json(obj, &out, &cap, &len);
                if (!out) { out = strdup("{}"); len = 2; }
                char *san_prefix = sanitize_label(lbl);
                /* parse the JSON text back into JSONVal to reuse emit_json_object_as_globals */
                JSONVal *parsed = json_parse_text(out);
                emit_json_object_as_globals(san_prefix, parsed, out);
                /* add global for var itself and initialize to pointer to full JSON text */
                add_global_var(lbl);
                int sid = add_string_literal(out);
                char initbuf[256];
                snprintf(initbuf, sizeof(initbuf), "INIT_PTR:%s:STR%d", lbl, sid);
                add_string_literal(initbuf);
                free(san_prefix);
                json_free(parsed);
                free(out);
            } else {
                /* existing behavior: evaluate initializer and store into global */
                add_global_var(lbl);
                if (node->child_count > 0) {
                    cg_emit_expr(node->children[0]);
                    asm_printf("\tmovq %%rax, %s(%%rip)\n", lbl);
                } else {
                    asm_printf("\tmovq $0, %%rax\n\tmovq %%rax, %s(%%rip)\n", lbl);
                }
            }
            break;
        }
        case AST_EXPR_STMT:
            cg_emit_expr(node->children[0]);
            break;
        case AST_RETURN:
            if (node->child_count > 0) cg_emit_expr(node->children[0]);
            else asm_printf("\tmovq $0, %%rax\n");
            asm_printf("\tleave\n\tret\n");
            break;
        case AST_IF: {
            int lid_else = gen_label_id();
            int lid_end = gen_label_id();
            cg_emit_expr(node->children[0]);
            asm_printf("\tcmpq $0, %%rax\n\tje .Lelse%d\n", lid_else);
            cg_emit_stmt(node->children[1]);
            asm_printf("\tjmp .Lend%d\n", lid_end);
            asm_printf(".Lelse%d:\n", lid_else);
            if (node->child_count > 2) cg_emit_stmt(node->children[2]);
            asm_printf(".Lend%d:\n", lid_end);
            break;
        }
        case AST_WHILE: {
            int lid_start = gen_label_id();
            int lid_end = gen_label_id();
            asm_printf(".Lwhile%d:\n", lid_start);
            cg_emit_expr(node->children[0]);
            asm_printf("\tcmpq $0, %%rax\n\tje .Lend%d\n", lid_end);
            cg_emit_stmt(node->children[1]);
            asm_printf("\tjmp .Lwhile%d\n", lid_start);
            asm_printf(".Lend%d:\n", lid_end);
            break;
        }
        case AST_FOR: {
            AST *init = node->child_count > 0 ? node->children[0] : NULL;
            AST *cond = node->child_count > 1 ? node->children[1] : NULL;
            AST *incr = node->child_count > 2 ? node->children[2] : NULL;
            AST *body = node->child_count > 3 ? node->children[3] : NULL;
            if (init) cg_emit_stmt(init);
            int lid_start = gen_label_id();
            int lid_end = gen_label_id();
            asm_printf(".Lfor%d:\n", lid_start);
            if (cond) {
                cg_emit_expr(cond);
                asm_printf("\tcmpq $0, %%rax\n\tje .Lend%d\n", lid_end);
            }
            cg_emit_stmt(body);
            if (incr) cg_emit_expr(incr);
            asm_printf("\tjmp .Lfor%d\n", lid_start);
            asm_printf(".Lend%d:\n", lid_end);
            break;
        }
        case AST_FUNC_DECL:
            register_function(node->name, node);
            break;
        default:
            error_at(node->token.loc, "Unhandled statement kind in codegen: %d", node->kind);
    }
}

/* Emit function */
static void emit_function(AST *fn) {
    asm_printf("\t.text\n");
    asm_printf("\t.globl %s\n", fn->name);
    asm_printf("%s:\n", fn->name);
    asm_printf("\tpushq %%rbp\n\tmovq %%rsp, %%rbp\n");
    cg_emit_stmt(fn->children[0]);
    asm_printf("\tleave\n\tret\n");
}

/* Emit data section with initializers interpreted from string table markers */
static void emit_data_section_with_inits(void) {
    /* rodata strings (skip INIT markers) */
    if (str_table) {
        asm_printf("\t.section .rodata\n");
        for (StrEntry *e = str_table; e; e = e->next) {
            if (!strncmp(e->s, "INIT_NUM:", 9)) continue;
            if (!strncmp(e->s, "INIT_PTR:", 9)) continue;
            char *esc = escape_for_asciz(e->s);
            asm_printf(".Lstr%d:\n", e->id);
            asm_printf("\t.asciz \"%s\"\n", esc);
            free(esc);
        }
    }
    /* globals and initializers */
    if (gvars) {
        asm_printf("\t.data\n");
        for (GVar *g = gvars; g; g = g->next) {
            long long init_num = 0;
            int has_init_num = 0;
            int init_ptr_sid = -1;
            for (StrEntry *e = str_table; e; e = e->next) {
                if (!strncmp(e->s, "INIT_NUM:", 9)) {
                    char *copy = strdup(e->s + 9);
                    char *colon = strchr(copy, ':');
                    if (colon) {
                        *colon = '\0';
                        char *label = copy;
                        char *valstr = colon + 1;
                        if (!strcmp(label, g->name)) {
                            init_num = atoll(valstr);
                            has_init_num = 1;
                        }
                    }
                    free(copy);
                } else if (!strncmp(e->s, "INIT_PTR:", 9)) {
                    char *copy = strdup(e->s + 9);
                    char *colon = strchr(copy, ':');
                    if (colon) {
                        *colon = '\0';
                        char *label = copy;
                        char *valstr = colon + 1;
                        if (!strcmp(label, g->name)) {
                            if (!strncmp(valstr, "STR", 3)) {
                                init_ptr_sid = atoi(valstr + 3);
                            }
                        }
                    }
                    free(copy);
                }
            }
            asm_printf("%s:\n", g->name);
            if (has_init_num) {
                asm_printf("\t.quad %lld\n", init_num);
            } else if (init_ptr_sid >= 0) {
                asm_printf("\t.quad .Lstr%d\n", init_ptr_sid);
            } else {
                asm_printf("\t.quad 0\n");
            }
        }
    }
}

/* Top-level codegen */
static void cg_emit_program(AST *prog) {
    for (int i = 0; i < prog->child_count; i++) {
        AST *s = prog->children[i];
        if (s->kind == AST_VAR_DECL) add_global_var(s->name);
        if (s->kind == AST_FUNC_DECL) register_function(s->name, s);
    }
    asm_printf("\t.text\n\t.globl main\nmain:\n");
    asm_printf("\tpushq %%rbp\n\tmovq %%rsp, %%rbp\n");
    for (int i = 0; i < prog->child_count; i++) {
        AST *s = prog->children[i];
        if (s->kind != AST_FUNC_DECL) cg_emit_stmt(s);
    }
    asm_printf("\tmovl $0, %%eax\n\tleave\n\tret\n");
    for (FuncEntry *f = funcs; f; f = f->next) emit_function(f->fn);
    emit_data_section_with_inits();
}

/* ----------------------------------------------------------------------
 * DEBUG: AST print
 * ---------------------------------------------------------------------- */

static void ast_print(AST *a, int indent) {
    if (!a) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (a->kind) {
        case AST_PROGRAM: printf("Program\n"); break;
        case AST_FUNC_DECL: printf("Function %s (%d params)\n", a->name ? a->name : "", a->param_count); break;
        case AST_VAR_DECL: printf("VarDecl %s\n", a->name ? a->name : ""); break;
        case AST_BLOCK: printf("Block\n"); break;
        case AST_RETURN: printf("Return\n"); break;
        case AST_IF: printf("If\n"); break;
        case AST_WHILE: printf("While\n"); break;
        case AST_FOR: printf("For\n"); break;
        case AST_EXPR_STMT: printf("ExprStmt\n"); break;
        case AST_BINARY: printf("Binary %s\n", a->token.lexeme ? a->token.lexeme : ""); break;
        case AST_UNARY: printf("Unary %s\n", a->token.lexeme ? a->token.lexeme : ""); break;
        case AST_CALL: printf("Call\n"); break;
        case AST_IDENT: printf("Ident %s\n", a->name ? a->name : ""); break;
        case AST_NUMBER: printf("Number %g\n", a->num); break;
        case AST_STRING: printf("String \"%s\"\n", a->str ? a->str : ""); break;
        case AST_OBJECT: {
            printf("Object {\n");
            for (int i = 0; i < a->param_count; i++) {
                for (int j = 0; j < indent + 1; j++) printf("  ");
                printf("\"%s\":\n", a->params[i]);
                ast_print(a->children[i], indent + 2);
            }
            for (int j = 0; j < indent; j++) printf("  ");
            printf("}\n");
            break;
        }
        case AST_ARRAY: {
            printf("Array [\n");
            for (int i = 0; i < a->child_count; i++) {
                ast_print(a->children[i], indent + 1);
            }
            for (int j = 0; j < indent; j++) printf("  ");
            printf("]\n");
            break;
        }
        case AST_EMPTY: printf("Empty\n"); break;
        default: printf("AST kind %d\n", a->kind); break;
    }
    if (a->kind != AST_OBJECT && a->kind != AST_ARRAY) {
        for (int i = 0; i < a->child_count; i++) ast_print(a->children[i], indent + 1);
    }
}

/* ----------------------------------------------------------------------
 * MAIN
 * ---------------------------------------------------------------------- */

int main(int argc, char **argv) {
    srand((unsigned)time(NULL));
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.sparrow > out.s\n", prog_name);
        return 1;
    }
    char *src = read_file(argv[1]);
    if (!src) { fprintf(stderr, "Cannot read %s\n", argv[1]); return 1; }

    TokenStream ts = tokenize(src);
    Parser p = { .ts = &ts };
    AST *prog = parse_program(&p);

    /* Uncomment to debug AST */
    /* ast_print(prog, 0); */

    cg_emit_program(prog);

    return 0;
}
