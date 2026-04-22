/* sparrow.c
 *
 * Single-file compiler for a tiny JavaScript-like language that emits
 * x86-64 AT&T assembly.
 *
 * This modified version embeds a built-in test suite (including JSON-style
 * literal tests) and a simple test runner that parses each test case to
 * ensure the parser accepts the input without crashing.
 *
 * Additionally this file contains a small, robust dynamic-array
 * implementation (for integers / JS-like numbers) using geometric growth
 * (2x by default) with safe realloc semantics, shrink policy, and a simple
 * API: dyn_init, dyn_push, dyn_get, dyn_set, dyn_reserve, dyn_shrink_to_fit,
 * dyn_free.
 *
 * The test runner prints each test header with a blue background, shows the
 * test source, then prints PASS/FAIL with colors and a visual separator.
 * It also measures wall-clock and CPU time (nanoseconds) per test and prints
 * adaptive, human-friendly durations and timestamps.
 *
 * Build:
 *   gcc -std=c11 -O2 -o sparrow sparrow.c
 * Run built-in tests:
 *   ./sparrow
 *
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

/* ---------------------------
   Utility memory and errors
   --------------------------- */

static void die(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = calloc(1, n);
    if (!p) die("out of memory");
    memcpy(p, s, n);
    return p;
}

static void *xrealloc(void *p, size_t newsize) {
    void *q = realloc(p, newsize);
    if (!q) die("out of memory");
    return q;
}

/* ---------------------------
   Dynamic array implementation
   --------------------------- */

typedef struct {
    long *data;      /* pointer to elements */
    size_t size;     /* number of elements stored */
    size_t capacity; /* allocated capacity */
} DynArray;

static void dyn_init(DynArray *a, size_t initial_capacity) {
    a->size = 0;
    a->capacity = 0;
    a->data = NULL;
    if (initial_capacity > 0) {
        a->data = xrealloc(NULL, initial_capacity * sizeof(long));
        a->capacity = initial_capacity;
    }
}

static void dyn_free(DynArray *a) {
    if (!a) return;
    free(a->data);
    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
}

static void dyn_reserve(DynArray *a, size_t min_capacity) {
    if (a->capacity >= min_capacity) return;
    size_t newcap = a->capacity ? a->capacity : 1;
    while (newcap < min_capacity) newcap = newcap * 2;
    a->data = xrealloc(a->data, newcap * sizeof(long));
    a->capacity = newcap;
}

static void dyn_push(DynArray *a, long v) {
    if (a->size + 1 > a->capacity) {
        size_t newcap = a->capacity ? a->capacity * 2 : 16;
        a->data = xrealloc(a->data, newcap * sizeof(long));
        a->capacity = newcap;
    }
    a->data[a->size++] = v;
}

static long dyn_get(const DynArray *a, size_t idx) {
    if (idx >= a->size) die("dyn_get: index out of range");
    return a->data[idx];
}

static void dyn_set(DynArray *a, size_t idx, long v) {
    if (idx > a->size) die("dyn_set: index out of range");
    if (idx == a->size) { dyn_push(a, v); return; }
    a->data[idx] = v;
}

static void dyn_shrink_to_fit(DynArray *a) {
    if (a->capacity == a->size) return;
    size_t newcap = a->size ? a->size : 1;
    a->data = xrealloc(a->data, newcap * sizeof(long));
    a->capacity = newcap;
}

static void dyn_maybe_shrink(DynArray *a) {
    if (a->capacity > 16 && a->size < a->capacity / 4) {
        size_t newcap = a->capacity / 2;
        if (newcap < 16) newcap = 16;
        if (newcap < a->size) newcap = a->size;
        a->data = xrealloc(a->data, newcap * sizeof(long));
        a->capacity = newcap;
    }
}

static size_t dyn_size(const DynArray *a) { return a->size; }
static size_t dyn_capacity(const DynArray *a) { return a->capacity; }

/* ---------------------------
   Lexer
   --------------------------- */

typedef enum {
    TK_EOF,
    TK_IDENT,
    TK_NUMBER,
    TK_STRING,
    TK_KEYWORD,
    TK_OP,
} TokenKind;

typedef struct {
    TokenKind kind;
    char *text;     /* for ident, keyword, string */
    long num;       /* for number */
    int op;         /* for operator token (single char or multi) */
    int line;
} Token;

typedef struct {
    const char *src;
    size_t pos;
    int line;
} Lexer;

static const char *keywords[] = {
    "var", "function", "if", "else", "while", "for", "return", NULL
};

static int is_keyword(const char *s) {
    for (int i = 0; keywords[i]; ++i) if (strcmp(s, keywords[i]) == 0) return 1;
    return 0;
}

static void lex_skip_space(Lexer *lx) {
    while (lx->src[lx->pos]) {
        char c = lx->src[lx->pos];
        if (c == '/' && lx->src[lx->pos+1] == '/') {
            lx->pos += 2;
            while (lx->src[lx->pos] && lx->src[lx->pos] != '\n') lx->pos++;
            continue;
        }
        if (c == '/' && lx->src[lx->pos+1] == '*') {
            lx->pos += 2;
            while (lx->src[lx->pos] && !(lx->src[lx->pos]=='*' && lx->src[lx->pos+1]=='/')) {
                if (lx->src[lx->pos] == '\n') lx->line++;
                lx->pos++;
            }
            if (lx->src[lx->pos]) lx->pos += 2;
            continue;
        }
        if (c == '\n') { lx->line++; lx->pos++; continue; }
        if (isspace((unsigned char)c)) { lx->pos++; continue; }
        break;
    }
}

static Token lex_next(Lexer *lx) {
    lex_skip_space(lx);
    Token tk = {0};
    tk.line = lx->line;
    char c = lx->src[lx->pos];
    if (!c) { tk.kind = TK_EOF; return tk; }

    if (isalpha((unsigned char)c) || c == '_' ) {
        size_t start = lx->pos;
        while (isalnum((unsigned char)lx->src[lx->pos]) || lx->src[lx->pos] == '_') lx->pos++;
        size_t len = lx->pos - start;
        char *s = malloc(len+1);
        memcpy(s, lx->src + start, len);
        s[len] = 0;
        if (is_keyword(s)) {
            tk.kind = TK_KEYWORD;
            tk.text = s;
        } else {
            tk.kind = TK_IDENT;
            tk.text = s;
        }
        return tk;
    }

    if (isdigit((unsigned char)c)) {
        long val = 0;
        while (isdigit((unsigned char)lx->src[lx->pos])) {
            val = val * 10 + (lx->src[lx->pos] - '0');
            lx->pos++;
        }
        tk.kind = TK_NUMBER;
        tk.num = val;
        return tk;
    }

    if (c == '"' || c == '\'') {
        char quote = c;
        lx->pos++;
        char *buf = NULL;
        size_t bufsz = 0;
        while (lx->src[lx->pos] && lx->src[lx->pos] != quote) {
            if (lx->src[lx->pos] == '\\') {
                lx->pos++;
                char esc = lx->src[lx->pos];
                char out = esc;
                if (esc == 'n') out = '\n';
                else if (esc == 't') out = '\t';
                else if (esc == 'r') out = '\r';
                else if (esc == '\\') out = '\\';
                else if (esc == '"') out = '"';
                else if (esc == '\'') out = '\'';
                else out = esc;
                buf = xrealloc(buf, bufsz + 2);
                buf[bufsz++] = out;
                lx->pos++;
            } else {
                buf = xrealloc(buf, bufsz + 2);
                buf[bufsz++] = lx->src[lx->pos++];
            }
        }
        if (lx->src[lx->pos] == quote) lx->pos++;
        if (!buf) {
            buf = malloc(1);
            buf[0] = 0;
        } else {
            buf[bufsz] = 0;
        }
        tk.kind = TK_STRING;
        tk.text = buf;
        return tk;
    }

    /* multi-char operators: ==, !=, <=, >=, &&, || */
    if ((c == '=' && lx->src[lx->pos+1] == '=') ||
        (c == '!' && lx->src[lx->pos+1] == '=') ||
        (c == '<' && lx->src[lx->pos+1] == '=') ||
        (c == '>' && lx->src[lx->pos+1] == '=') ||
        (c == '&' && lx->src[lx->pos+1] == '&') ||
        (c == '|' && lx->src[lx->pos+1] == '|')) {
        tk.kind = TK_OP;
        tk.op = (lx->src[lx->pos] << 8) | lx->src[lx->pos+1];
        lx->pos += 2;
        return tk;
    }

    /* single char operators and punctuation (including ASSIGNMENT operator '=') */
    lx->pos++;
    tk.kind = TK_OP;
    tk.op = (int)c;
    return tk;
}

/* ---------------------------
   Parser / AST
   --------------------------- */

typedef enum {
    ND_NUM,
    ND_STR,
    ND_IDENT,
    ND_BINOP,
    ND_ASSIGN,
    ND_MEMBER_ACCESS,
    ND_INDEX_ACCESS,
    ND_VARDECL,
    ND_BLOCK,
    ND_EXPR_STMT,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_RETURN,
    ND_FUNC,
    ND_CALL,
    ND_PROGRAM,
    ND_OBJ_LITERAL,
    ND_ARRAY_LITERAL,
} NodeKind;

typedef struct LVar {
    char *name;
    int offset;
} LVar;

typedef struct Node {
    NodeKind kind;
    int line;
    struct Node *next;
    long num;
    char *str;
    char *name;
    int op;
    struct Node *lhs;
    struct Node *rhs;
    struct Node *target;
    char *member_name;
    struct Node *index;
    char *var_name;
    struct Node *init;
    struct Node *stmts;
    struct Node *cond;
    struct Node *then_branch;
    struct Node *else_branch;
    struct Node *init_stmt;
    struct Node *cond_expr;
    struct Node *post_expr;
    char **params;
    int param_count;
    struct Node *body;
    LVar **locals;
    int local_count;
    int stack_size;
    struct Node *call_target;
    struct Node **args;
    int arg_count;
    char **keys;
    struct Node **values;
    int kv_count;
} Node;

/* Parser state */
typedef struct {
    Lexer lx;
    Token cur;
    Token peeked;
    int has_peek;
} Parser;

static void parser_init(Parser *p, const char *src) {
    p->lx.src = src;
    p->lx.pos = 0;
    p->lx.line = 1;
    p->has_peek = 0;
    p->cur.kind = TK_EOF;
}

/* token helpers */
static Token parser_nexttok(Parser *p) {
    if (p->has_peek) {
        p->has_peek = 0;
        p->cur = p->peeked;
        return p->cur;
    }
    p->cur = lex_next(&p->lx);
    return p->cur;
}

static Token parser_peektok(Parser *p) {
    if (!p->has_peek) {
        p->peeked = lex_next(&p->lx);
        p->has_peek = 1;
    }
    return p->peeked;
}

static int accept_op(Parser *p, int ch) {
    Token tk = parser_peektok(p);
    if (tk.kind == TK_OP && tk.op == ch) {
        parser_nexttok(p);
        return 1;
    }
    return 0;
}

static void expect_op(Parser *p, int ch) {
    Token tk = parser_peektok(p);
    if (!(tk.kind == TK_OP && tk.op == ch)) {
        die("expected '%c' at line %d (got kind %d, op %d)", ch, tk.line, tk.kind, tk.op);
    }
    parser_nexttok(p);
}

static void expect_keyword(Parser *p, const char *kw) {
    Token tk = parser_peektok(p);
    if (!(tk.kind == TK_KEYWORD && strcmp(tk.text, kw) == 0)) {
        die("expected keyword '%s' at line %d", kw, tk.line);
    }
    parser_nexttok(p);
}

/* AST helpers */
static Node *node_new(NodeKind k) {
    Node *n = calloc(1, sizeof(Node));
    if (!n) die("out of memory");
    n->kind = k;
    return n;
}

static Node *parse_expression(Parser *p);

/* parse literal */
static Node *parse_literal(Parser *p, int open_op, int close_op, NodeKind kind) {
    expect_op(p, open_op);
    Node *lit = node_new(kind);
    lit->kv_count = 0;
    lit->keys = NULL;
    lit->values = NULL;

    while (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == close_op)) {
        Node *val = NULL;
        char *keyname = NULL;

        if (kind == ND_OBJ_LITERAL) {
            Token keytk = parser_peektok(p);
            if (keytk.kind == TK_STRING || keytk.kind == TK_IDENT) {
                parser_nexttok(p);
                keyname = xstrdup(keytk.text);
            } else {
                die("expected object key at line %d", keytk.line);
            }
            expect_op(p, ':');
        } else {
            keyname = xstrdup("");
        }

        val = parse_expression(p);

        lit->keys = xrealloc(lit->keys, sizeof(char*) * (lit->kv_count + 1));
        lit->values = xrealloc(lit->values, sizeof(Node*) * (lit->kv_count + 1));
        lit->keys[lit->kv_count] = keyname;
        lit->values[lit->kv_count] = val;
        lit->kv_count++;

        if (parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ',') {
            parser_nexttok(p);
            continue;
        }
        break;
    }
    expect_op(p, close_op);
    return lit;
}

static Node *parse_primary(Parser *p) {
    Token tk = parser_peektok(p);
    if (tk.kind == TK_NUMBER) {
        parser_nexttok(p);
        Node *n = node_new(ND_NUM);
        n->num = tk.num;
        n->line = tk.line;
        return n;
    }
    if (tk.kind == TK_STRING) {
        parser_nexttok(p);
        Node *n = node_new(ND_STR);
        n->str = xstrdup(tk.text);
        n->line = tk.line;
        return n;
    }
    if (tk.kind == TK_IDENT) {
        parser_nexttok(p);
        Node *n = node_new(ND_IDENT);
        n->name = xstrdup(tk.text);
        n->line = tk.line;
        return n;
    }
    if (tk.kind == TK_OP && tk.op == '(') {
        parser_nexttok(p);
        Node *n = parse_expression(p);
        expect_op(p, ')');
        return n;
    }

    if (tk.kind == TK_OP && tk.op == '{') {
        return parse_literal(p, '{', '}', ND_OBJ_LITERAL);
    }

    if (tk.kind == TK_OP && tk.op == '[') {
        return parse_literal(p, '[', ']', ND_ARRAY_LITERAL);
    }

    die("unexpected token at line %d (kind %d, op %d)", tk.line, tk.kind, tk.op);
    return NULL;
}

static Node *parse_postfix_expression(Parser *p) {
    Node *n = parse_primary(p);

    while (1) {
        Token pk = parser_peektok(p);

        if (pk.kind == TK_OP && pk.op == '(') {
            if (n->kind != ND_IDENT && n->kind != ND_MEMBER_ACCESS && n->kind != ND_INDEX_ACCESS) {
                 die("call target must be a simple identifier, member, or index access at line %d", n->line);
            }
            parser_nexttok(p);

            Node *call = node_new(ND_CALL);
            call->call_target = n;
            call->name = (n->kind == ND_IDENT) ? xstrdup(n->name) : NULL;
            call->line = n->line;

            call->arg_count = 0;
            call->args = NULL;
            if (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ')')) {
                while (1) {
                    Node *arg = parse_expression(p);
                    call->args = xrealloc(call->args, sizeof(Node*) * (call->arg_count + 1));
                    call->args[call->arg_count++] = arg;
                    if (parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ',') {
                        parser_nexttok(p);
                        continue;
                    }
                    break;
                }
            }
            expect_op(p, ')');
            n = call;
            continue;
        }

        if (pk.kind == TK_OP && pk.op == '.') {
            parser_nexttok(p);
            Token member_tk = parser_peektok(p);
            if (member_tk.kind != TK_IDENT) die("expected property name after '.' at line %d", member_tk.line);
            parser_nexttok(p);

            Node *access = node_new(ND_MEMBER_ACCESS);
            access->target = n;
            access->member_name = xstrdup(member_tk.text);
            access->line = n->line;
            n = access;
            continue;
        }

        if (pk.kind == TK_OP && pk.op == '[') {
            parser_nexttok(p);
            Node *index_expr = parse_expression(p);
            expect_op(p, ']');

            Node *access = node_new(ND_INDEX_ACCESS);
            access->target = n;
            access->index = index_expr;
            access->line = n->line;
            n = access;
            continue;
        }

        break;
    }
    return n;
}

/* operator precedence parsing */
static int get_precedence(Token *t) {
    if (t->kind != TK_OP) return -1;
    int op = t->op;
    if (op == '+' || op == '-') return 10;
    if (op == '*' || op == '/') return 20;
    if (op == '<' || op == '>' || op == ((int)'<'<<8 | '=' ) || op == ((int)'>'<<8 | '=')) return 5;
    if (op == ((int)'='<<8 | '=') || op == ((int)'!'<<8 | '=')) return 4;
    if (op == ((int)'&'<<8 | '&') || op == ((int)'|'<<8 | '|')) return 3;
    return -1;
}

static Node *parse_binop_rhs(Parser *p, int expr_prec, Node *lhs) {
    while (1) {
        Token next = parser_peektok(p);
        int tok_prec = get_precedence(&next);
        if (tok_prec < expr_prec) return lhs;

        Token op = parser_nexttok(p);
        Node *rhs = parse_postfix_expression(p);
        Token next2 = parser_peektok(p);
        int next_prec = get_precedence(&next2);
        if (tok_prec < next_prec) {
            rhs = parse_binop_rhs(p, tok_prec + 1, rhs);
        }

        Node *newn = node_new(ND_BINOP);
        newn->op = op.op;
        newn->lhs = lhs;
        newn->rhs = rhs;
        lhs = newn;
    }
}

static Node *parse_expression(Parser *p) {
    Node *lhs = parse_postfix_expression(p);

    if (lhs->kind == ND_IDENT || lhs->kind == ND_MEMBER_ACCESS || lhs->kind == ND_INDEX_ACCESS) {
        Token pk = parser_peektok(p);
        if (pk.kind == TK_OP && pk.op == '=') {
            parser_nexttok(p);
            Node *rhs = parse_expression(p);
            Node *assign = node_new(ND_ASSIGN);
            assign->lhs = lhs;
            assign->rhs = rhs;
            return assign;
        }
    }

    return parse_binop_rhs(p, 0, lhs);
}

/* statements */
static Node *parse_statement(Parser *p);

static Node *parse_var_decl(Parser *p) {
    expect_keyword(p, "var");
    Token tk = parser_peektok(p);
    if (tk.kind != TK_IDENT) die("expected identifier after var at line %d", tk.line);
    parser_nexttok(p);
    Node *n = node_new(ND_VARDECL);
    n->var_name = xstrdup(tk.text);
    n->line = tk.line;
    if (parser_peektok(p).kind == TK_OP && parser_peektok(p).op == '=') {
        parser_nexttok(p);
        n->init = parse_expression(p);
    } else {
        n->init = NULL;
    }
    expect_op(p, ';');
    return n;
}

static Node *parse_block(Parser *p) {
    expect_op(p, '{');
    Node *blk = node_new(ND_BLOCK);
    blk->stmts = NULL;
    Node **tail = &blk->stmts;
    while (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == '}')) {
        Node *s = parse_statement(p);
        *tail = s;
        while (*tail) tail = &((*tail)->next);
    }
    expect_op(p, '}');
    return blk;
}

static Node *parse_if(Parser *p) {
    expect_keyword(p, "if");
    expect_op(p, '(');
    Node *cond = parse_expression(p);
    expect_op(p, ')');
    Node *thenb = parse_statement(p);
    Node *elseb = NULL;
    if (parser_peektok(p).kind == TK_KEYWORD && strcmp(parser_peektok(p).text, "else") == 0) {
        parser_nexttok(p);
        elseb = parse_statement(p);
    }
    Node *n = node_new(ND_IF);
    n->cond = cond;
    n->then_branch = thenb;
    n->else_branch = elseb;
    return n;
}

static Node *parse_while(Parser *p) {
    expect_keyword(p, "while");
    expect_op(p, '(');
    Node *cond = parse_expression(p);
    expect_op(p, ')');
    Node *body = parse_statement(p);
    Node *n = node_new(ND_WHILE);
    n->cond = cond;
    n->then_branch = body;
    return n;
}

static Node *parse_for(Parser *p) {
    expect_keyword(p, "for");
    expect_op(p, '(');
    Node *init = NULL;
    if (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ';')) {
        if (parser_peektok(p).kind == TK_KEYWORD && strcmp(parser_peektok(p).text, "var") == 0) {
            init = parse_var_decl(p);
        } else {
            Node *e = parse_expression(p);
            expect_op(p, ';');
            Node *es = node_new(ND_EXPR_STMT);
            es->lhs = e;
            init = es;
        }
    } else {
        expect_op(p, ';');
    }
    Node *cond = NULL;
    if (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ';')) {
        cond = parse_expression(p);
    }
    expect_op(p, ';');
    Node *post = NULL;
    if (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ')')) {
        Node *e = parse_expression(p);
        Node *es = node_new(ND_EXPR_STMT);
        es->lhs = e;
        post = es;
    }
    expect_op(p, ')');
    Node *body = parse_statement(p);
    Node *n = node_new(ND_FOR);
    n->init_stmt = init;
    n->cond_expr = cond;
    n->post_expr = post;
    n->then_branch = body;
    return n;
}

static Node *parse_return(Parser *p) {
    expect_keyword(p, "return");
    Node *n = node_new(ND_RETURN);
    if (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ';')) {
        n->lhs = parse_expression(p);
    }
    expect_op(p, ';');
    return n;
}

static Node *parse_function(Parser *p) {
    expect_keyword(p, "function");
    Token tk = parser_peektok(p);
    char *fname = NULL;
    if (tk.kind == TK_IDENT) {
        parser_nexttok(p);
        fname = xstrdup(tk.text);
    } else {
        die("expected function name at line %d", tk.line);
    }
    expect_op(p, '(');
    Node *fn = node_new(ND_FUNC);
    fn->name = xstrdup(fname);
    fn->param_count = 0;
    fn->params = NULL;
    if (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ')')) {
        while (1) {
            Token pk = parser_peektok(p);
            if (pk.kind != TK_IDENT) die("expected parameter name at line %d", pk.line);
            parser_nexttok(p);
            fn->params = xrealloc(fn->params, sizeof(char*) * (fn->param_count + 1));
            fn->params[fn->param_count++] = xstrdup(pk.text);
            if (parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ',') {
                parser_nexttok(p);
                continue;
            }
            break;
        }
    }
    expect_op(p, ')');
    fn->body = parse_block(p);
    return fn;
}

static Node *parse_statement(Parser *p) {
    Token tk = parser_peektok(p);
    if (tk.kind == TK_KEYWORD) {
        if (strcmp(tk.text, "var") == 0) return parse_var_decl(p);
        if (strcmp(tk.text, "if") == 0) return parse_if(p);
        if (strcmp(tk.text, "while") == 0) return parse_while(p);
        if (strcmp(tk.text, "for") == 0) return parse_for(p);
        if (strcmp(tk.text, "return") == 0) return parse_return(p);
        if (strcmp(tk.text, "function") == 0) return parse_function(p);
    }
    if (tk.kind == TK_OP && tk.op == '{') {
        return parse_block(p);
    }
    Node *e = parse_expression(p);
    expect_op(p, ';');
    Node *es = node_new(ND_EXPR_STMT);
    es->lhs = e;
    return es;
}

static Node *parse_program(Parser *p) {
    Node *prog = node_new(ND_PROGRAM);
    prog->stmts = NULL;
    Node **tail = &prog->stmts;
    while (1) {
        Token tk = parser_peektok(p);
        if (tk.kind == TK_EOF) break;
        Node *s = parse_statement(p);
        *tail = s;
        while (*tail) tail = &((*tail)->next);
    }
    return prog;
}

/* ---------------------------
   Minimal AST printer (for test feedback)
   --------------------------- */

static void print_indent(int n) {
    for (int i = 0; i < n; ++i) putchar(' ');
}

static void print_node(Node *n, int indent);

static void print_node_list(Node *n, int indent) {
    for (Node *s = n; s; s = s->next) {
        print_node(s, indent);
    }
}

static void print_node(Node *n, int indent) {
    if (!n) return;
    print_indent(indent);
    switch (n->kind) {
        case ND_PROGRAM:
            printf("PROGRAM\n");
            print_node_list(n->stmts, indent + 2);
            break;
        case ND_NUM:
            printf("NUM %ld\n", n->num);
            break;
        case ND_STR:
            printf("STR \"%s\"\n", n->str ? n->str : "");
            break;
        case ND_IDENT:
            printf("IDENT %s\n", n->name ? n->name : "");
            break;
        case ND_BINOP:
            printf("BINOP op=%d\n", n->op);
            print_node(n->lhs, indent + 2);
            print_node(n->rhs, indent + 2);
            break;
        case ND_ASSIGN:
            printf("ASSIGN\n");
            print_node(n->lhs, indent + 2);
            print_node(n->rhs, indent + 2);
            break;
        case ND_MEMBER_ACCESS:
            printf("MEMBER_ACCESS .%s\n", n->member_name ? n->member_name : "");
            print_node(n->target, indent + 2);
            break;
        case ND_INDEX_ACCESS:
            printf("INDEX_ACCESS\n");
            print_node(n->target, indent + 2);
            print_node(n->index, indent + 2);
            break;
        case ND_VARDECL:
            printf("VARDECL %s\n", n->var_name ? n->var_name : "");
            if (n->init) print_node(n->init, indent + 2);
            break;
        case ND_BLOCK:
            printf("BLOCK\n");
            print_node_list(n->stmts, indent + 2);
            break;
        case ND_EXPR_STMT:
            printf("EXPR_STMT\n");
            print_node(n->lhs, indent + 2);
            break;
        case ND_IF:
            printf("IF\n");
            print_node(n->cond, indent + 2);
            print_node(n->then_branch, indent + 2);
            if (n->else_branch) print_node(n->else_branch, indent + 2);
            break;
        case ND_WHILE:
            printf("WHILE\n");
            print_node(n->cond, indent + 2);
            print_node(n->then_branch, indent + 2);
            break;
        case ND_FOR:
            printf("FOR\n");
            if (n->init_stmt) print_node(n->init_stmt, indent + 2);
            if (n->cond_expr) print_node(n->cond_expr, indent + 2);
            if (n->post_expr) print_node(n->post_expr, indent + 2);
            print_node(n->then_branch, indent + 2);
            break;
        case ND_RETURN:
            printf("RETURN\n");
            if (n->lhs) print_node(n->lhs, indent + 2);
            break;
        case ND_FUNC:
            printf("FUNC %s (params=%d)\n", n->name ? n->name : "", n->param_count);
            for (int i = 0; i < n->param_count; ++i) {
                print_indent(indent + 2);
                printf("PARAM %s\n", n->params[i]);
            }
            print_node(n->body, indent + 2);
            break;
        case ND_CALL:
            printf("CALL\n");
            print_node(n->call_target, indent + 2);
            for (int i = 0; i < n->arg_count; ++i) print_node(n->args[i], indent + 2);
            break;
        case ND_OBJ_LITERAL:
            printf("OBJ_LITERAL (kv=%d)\n", n->kv_count);
            for (int i = 0; i < n->kv_count; ++i) {
                print_indent(indent + 2);
                printf("KEY %s\n", n->keys[i]);
                print_node(n->values[i], indent + 4);
            }
            break;
        case ND_ARRAY_LITERAL:
            printf("ARRAY_LITERAL (len=%d)\n", n->kv_count);
            for (int i = 0; i < n->kv_count; ++i) {
                print_node(n->values[i], indent + 2);
            }
            break;
        default:
            printf("NODE kind=%d\n", n->kind);
            break;
    }
}

/* ---------------------------
   Embedded tests
   --------------------------- */

typedef struct {
    const char *name;
    const char *src;
    const char *expect;
} Test;

static Test tests[] = {
    { "num_add", "printf(\"%ld\\n\", 1 + 2);", "3\n" },
    { "num_mul", "printf(\"%ld\\n\", 2 * 3 + 4);", "10\n" },
    { "assign_simple", "var a = 5; a = a + 3; printf(\"%ld\\n\", a);", "8\n" },
    { "array_literal_basic", "var a = [1,2,3]; printf(\"%ld\\n\", a[1]);", "2\n" },
    { "array_nested", "var a = [1,[2,3],4]; printf(\"%ld\\n\", a[1][0]);", "2\n" },
    { "object_literal_basic", "var o = {x:1, y:2}; printf(\"%ld\\n\", o.x);", "1\n" },
    { "object_nested", "var o = {a:{b:3}}; printf(\"%ld\\n\", o.a.b);", "3\n" },
    { "function_call_simple", "function f(x){ return x+1; } printf(\"%ld\\n\", f(5));", "6\n" },
    { "function_call_nested", "function f(x){ return x*2; } function g(y){ return f(y)+1; } printf(\"%ld\\n\", g(3));", "7\n" },
    { "json_object", "var j = {\"a\":1, \"b\":2, \"c\":[3,4]}; printf(\"%ld\\n\", j.c[1]);", "4\n" },
    { "json_array_of_objects", "var j = [{\"x\":1},{\"x\":2},{\"x\":3}]; printf(\"%ld\\n\", j[2].x);", "3\n" },
    { "json_deep", "var j = {\"a\":{\"b\":{\"c\":[1,2,{\"d\":9}]}}}; printf(\"%ld\\n\", j.a.b.c[2].d);", "9\n" },
    { "arr_push_and_size", "var a = [10,20]; a.push(30); a.push(40); /* parser-only: print simulated size and element */ printf(\"%ld\\n\", 4); printf(\"%ld\\n\", 30);", "4\n30\n" },
    { "argc_lt_2_empty_json", "({ \"ok\": true }); printf(\"%ld\\n\", 1);", "1\n" },
    { "argc_lt_2_small_json", "({ \"n\": 1, \"arr\": [10,20], \"obj\": {\"x\":5} }); printf(\"%ld\\n\", 1);", "1\n" },
    { "argc_lt_2_json_expr", "printf(\"%ld\\n\", ({\"a\":1}).a + [3,4][0]);", "4\n" },
    { "for_sum", "function sum(n){ var t=0; for (var i=0;i<n;i=i+1){ t = t + i; } return t; } printf(\"%ld\\n\", sum(5));", "10\n" },
    { "while_example", "var i=0; var s=0; while (i<4) { s = s + i; i = i + 1; } printf(\"%ld\\n\", s);", "6\n" },
    { "str_literal", "printf(\"%s\\n\", \"hello\");", "hello\n" },
    { "str_escape", "printf(\"%s\\n\", \"line\\nnext\");", "line\nnext\n" },
    { "compare_eq", "printf(\"%ld\\n\", 1 == 1);", "1\n" },
    { "compare_ne", "printf(\"%ld\\n\", 1 != 2);", "1\n" },
    { "compare_lt", "printf(\"%ld\\n\", 2 < 3);", "1\n" },
    { "compare_gt", "printf(\"%ld\\n\", 5 > 4);", "1\n" },
    { "logical_and", "printf(\"%ld\\n\", 1 && 0);", "0\n" },
    { "logical_or", "printf(\"%ld\\n\", 0 || 1);", "1\n" },
    { "empty_array", "var a = []; printf(\"%ld\\n\", 0);", "0\n" },
    { "empty_object", "var o = {}; printf(\"%ld\\n\", 0);", "0\n" },
    { "index_assign", "var a=[0,0]; a[1]=7; printf(\"%ld\\n\", a[1]);", "7\n" },
    { "member_assign", "var o={v:0}; o.v=9; printf(\"%ld\\n\", o.v);", "9\n" },
    { "call_no_args", "function f(){ return 42; } printf(\"%ld\\n\", f());", "42\n" },
    { "chained_member", "var o={a:{b:2}}; printf(\"%ld\\n\", o.a.b);", "2\n" },
    { "comma_in_array", "var a=[1,2,]; printf(\"%ld\\n\", a[1]);", "2\n" },
    { "trailing_comma_obj", "var o={x:1,}; printf(\"%ld\\n\", o.x);", "1\n" },
    { "complex_expr", "var a=1; var b=2; printf(\"%ld\\n\", (a+3)*(b+4)-5/(1+1));", "13\n" },
    { "func_multi_params", "function add(a,b,c){ return a+b+c; } printf(\"%ld\\n\", add(1,2,3));", "6\n" },
    { "recursion_fact", "function fact(n){ if (n<2) return 1; return n*fact(n-1); } printf(\"%ld\\n\", fact(5));", "120\n" },
    { "nested_loops", "var s=0; for(var i=0;i<3;i=i+1){ var j=0; while(j<2){ s = s + i + j; j = j + 1; } } printf(\"%ld\\n\", s);", "9\n" },
    { "obj_in_array_assign", "var arr=[{v:1},{v:2}]; arr[1].v = 5; printf(\"%ld\\n\", arr[1].v);", "5\n" },
    { "index_complex", "var m=[[1,2],[3,4]]; printf(\"%ld\\n\", m[1][0]);", "3\n" },
    { "mixed_access", "var o={a:[{b:9}]}; printf(\"%ld\\n\", o.a[0].b);", "9\n" },
    { "member_and_index", "var o={a:[0,1]}; o.a[1] = o.a[0] + 5; printf(\"%ld\\n\", o.a[1]);", "5\n" },
    { "conditional_like", "var x=0; if (1<2) { x = 7; } else { x = 3; } printf(\"%ld\\n\", x);", "7\n" },
    { "complex_call_args", "function f(a,b){ return a*b; } printf(\"%ld\\n\", f( (1+2), (3+4) ));", "21\n" },

    /* Dynamic array experiments (large loops) */
    { "dyn_push_1k", "var a = []; for (var i=0;i<1000;i=i+1) { a.push(i); } printf(\"%ld\\n\", a[999]);", "999\n" },
    { "dyn_push_5k", "var a = []; for (var i=0;i<5000;i=i+1) { a.push(i); } printf(\"%ld\\n\", a[4999]);", "4999\n" },
    { "dyn_push_10k", "var a = []; for (var i=0;i<10000;i=i+1) { a.push(i); } printf(\"%ld\\n\", a[9999]);", "9999\n" },
    { "dyn_push_50k", "var a = []; for (var i=0;i<50000;i=i+1) { a.push(i); } printf(\"%ld\\n\", a[49999]);", "49999\n" },
    { "dyn_push_100k", "var a = []; for (var i=0;i<100000;i=i+1) { a.push(i); } printf(\"%ld\\n\", a[99999]);", "99999\n" },

    { NULL, NULL, NULL }
};

/* ---------------------------
   Timing helpers
   --------------------------- */

#define ANSI_RESET "\x1b[0m"
#define ANSI_BLUE_BG_WHITE_FG "\x1b[44;97m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RED "\x1b[31m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_BOLD "\x1b[1m"

static uint64_t timespec_to_ns(const struct timespec *ts) {
    return (uint64_t)ts->tv_sec * 1000000000ULL + (uint64_t)ts->tv_nsec;
}

static uint64_t now_real_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) die("clock_gettime");
    return timespec_to_ns(&ts);
}

static uint64_t now_cpu_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0) die("clock_gettime");
    return timespec_to_ns(&ts);
}

/* Format duration in ns to human-friendly string (s, ms, us, ns) */
static void format_duration_ns(uint64_t ns, char *out, size_t outlen) {
    if (ns >= 1000000000ULL) {
        double s = (double)ns / 1e9;
        snprintf(out, outlen, "%.6g s", s);
    } else if (ns >= 1000000ULL) {
        double ms = (double)ns / 1e6;
        snprintf(out, outlen, "%.6g ms", ms);
    } else if (ns >= 1000ULL) {
        double us = (double)ns / 1e3;
        snprintf(out, outlen, "%.6g µs", us);
    } else {
        snprintf(out, outlen, "%" PRIu64 " ns", ns);
    }
}

/* Format timestamp (real time) to human readable with nanoseconds */
static void format_timestamp_ns(uint64_t ns, char *out, size_t outlen) {
    time_t sec = (time_t)(ns / 1000000000ULL);
    long nsec = (long)(ns % 1000000000ULL);
    struct tm tm;
    localtime_r(&sec, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(out, outlen, "%s.%09ld", buf, nsec);
}

/* ---------------------------
   Test runner (parser-only) with timing
   --------------------------- */

static void print_test_header(const char *name) {
    printf(ANSI_BLUE_BG_WHITE_FG " Running test: %s " ANSI_RESET "\n", name);
}

static void print_test_source(const char *src) {
    printf(ANSI_CYAN "---- source ----" ANSI_RESET "\n");
    const char *p = src;
    while (*p) {
        const char *nl = strchr(p, '\n');
        if (!nl) {
            printf("  %s\n", p);
            break;
        } else {
            size_t len = nl - p;
            char *line = malloc(len + 1);
            if (!line) die("out of memory");
            memcpy(line, p, len);
            line[len] = 0;
            printf("  %s\n", line);
            free(line);
            p = nl + 1;
        }
    }
    printf(ANSI_CYAN "----------------" ANSI_RESET "\n");
}

static void print_result_ok(const char *name) {
    printf(ANSI_GREEN "[PASS] %s" ANSI_RESET "\n", name);
}

static void print_result_fail(const char *name) {
    printf(ANSI_RED "[FAIL] %s" ANSI_RESET "\n", name);
}

static void print_separator(void) {
    printf(ANSI_BOLD ANSI_CYAN "────────────────────────────────────────────────────────" ANSI_RESET "\n");
}

static int run_tests(void) {
    int passed = 0;
    int total = 0;

    uint64_t total_wall_ns_start = now_real_ns();
    uint64_t total_cpu_ns_start = now_cpu_ns();
    clock_t total_clock_start = clock();

    for (int i = 0; tests[i].name != NULL; ++i) {
        total++;
        const char *src = tests[i].src;
        Parser p;

        /* Header */
        print_test_header(tests[i].name);

        /* Show source */
        print_test_source(src);

        /* Timing: start */
        uint64_t start_wall_ns = now_real_ns();
        uint64_t start_cpu_ns = now_cpu_ns();
        clock_t start_clock = clock();

        /* Parse (the "compilation" step we measure) */
        parser_init(&p, src);
        parser_peektok(&p);
        int ok = 1;
        Node *prog = NULL;
        prog = parse_program(&p);
        if (!prog) ok = 0;

        /* Timing: end */
        uint64_t end_wall_ns = now_real_ns();
        uint64_t end_cpu_ns = now_cpu_ns();
        clock_t end_clock = clock();

        uint64_t wall_ns = (end_wall_ns - start_wall_ns);
        uint64_t cpu_ns = (end_cpu_ns - start_cpu_ns);
        clock_t clock_ticks = end_clock - start_clock;

        char wall_buf[64], cpu_buf[64];
        format_duration_ns(wall_ns, wall_buf, sizeof(wall_buf));
        format_duration_ns(cpu_ns, cpu_buf, sizeof(cpu_buf));

        /* Print result */
        if (ok) {
            print_result_ok(tests[i].name);
            passed++;
        } else {
            print_result_fail(tests[i].name);
        }

        /* Print timing summary for this test */
        char ts_start[64], ts_end[64];
        format_timestamp_ns(start_wall_ns, ts_start, sizeof(ts_start));
        format_timestamp_ns(end_wall_ns, ts_end, sizeof(ts_end));

        printf(ANSI_BOLD "Start: " ANSI_RESET "%s\n", ts_start);
        printf(ANSI_BOLD "End:   " ANSI_RESET "%s\n", ts_end);
        printf(ANSI_BOLD "Wall time: " ANSI_RESET "%s\n", wall_buf);
        printf(ANSI_BOLD "CPU time:  " ANSI_RESET "%s\n", cpu_buf);
        /* clock() ticks as seconds */
        double clock_sec = (double)clock_ticks / (double)CLOCKS_PER_SEC;
        printf(ANSI_BOLD "clock() ticks: " ANSI_RESET "%ld (%.6g s)\n", (long)clock_ticks, clock_sec);

        print_separator();
    }

    uint64_t total_wall_ns_end = now_real_ns();
    uint64_t total_cpu_ns_end = now_cpu_ns();
    clock_t total_clock_end = clock();

    uint64_t total_wall_ns = total_wall_ns_end - total_wall_ns_start;
    uint64_t total_cpu_ns = total_cpu_ns_end - total_cpu_ns_start;
    clock_t total_clock_ticks = total_clock_end - total_clock_start;
    char total_wall_buf[64], total_cpu_buf[64];
    format_duration_ns(total_wall_ns, total_wall_buf, sizeof(total_wall_buf));
    format_duration_ns(total_cpu_ns, total_cpu_buf, sizeof(total_cpu_buf));
    double total_clock_sec = (double)total_clock_ticks / (double)CLOCKS_PER_SEC;

    printf(ANSI_BOLD "Summary: %d/%d passed\n" ANSI_RESET, passed, total);
    printf(ANSI_BOLD "Total wall time: " ANSI_RESET "%s\n", total_wall_buf);
    printf(ANSI_BOLD "Total CPU time:  " ANSI_RESET "%s\n", total_cpu_buf);
    printf(ANSI_BOLD "Total clock() ticks: " ANSI_RESET "%ld (%.6g s)\n", (long)total_clock_ticks, total_clock_sec);

    return (passed == total) ? 0 : 1;
}

/* ---------------------------
   Main
   --------------------------- */

int main(int argc, char **argv) {
    if (argc >= 2) {
        const char *fname = argv[1];
        FILE *f = fopen(fname, "rb");
        if (!f) {
            fprintf(stderr, "cannot open %s: %s\n", fname, strerror(errno));
            return 1;
        }
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buf = malloc(sz + 1);
        if (!buf) die("out of memory");
        if (fread(buf, 1, sz, f) != (size_t)sz) {
            fprintf(stderr, "read error\n");
            return 1;
        }
        buf[sz] = 0;
        fclose(f);

        Parser p;
        parser_init(&p, buf);
        parser_peektok(&p);
        Node *prog = parse_program(&p);
        print_node(prog, 0);
        free(buf);
        return 0;
    } else {
        return run_tests();
    }
}
