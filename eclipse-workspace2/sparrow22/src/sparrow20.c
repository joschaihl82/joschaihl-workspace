/* sparrow.c
 *
 * Single-file compiler for a tiny JavaScript-like language that emits
 * x86-64 AT&T assembly. This file contains:
 * - Lexer for identifiers, numbers, strings, operators
 * - Recursive-descent parser producing a small AST
 * - Code generator emitting AT&T-style x86-64 assembly
 * - Simple global string and global variable tables
 * - Built-in test harness that compiles and runs small test programs
 *
 * Build:
 * gcc -std=c11 -O2 -o sparrow sparrow.c
 * Run built-in tests:
 * ./sparrow
 *
 * Notes:
 * - The compiler emits AT&T syntax (registers prefixed with %).
 * - The built-in tests use printf in the generated code to ensure output.
 * - This is a compact educational compiler, not production-grade.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

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

/* NOTE: "print" is intentionally NOT a keyword so print(...) parses as a call */
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
    /* NOTE: '=' is handled specially if it's not '==' */
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
    ND_ASSIGN, /* Hinzugefügt/aktiviert für Zuweisungen */
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
    ND_OBJ_LITERAL, /* for flattening at global scope */
} NodeKind;

typedef struct Node {
    NodeKind kind;
    int line;
    /* common */
    struct Node *next; /* for statement lists */

    /* for numbers and strings */
    long num;
    char *str;

    /* identifier */
    char *name;

    /* binary, assign */
    int op;
    struct Node *lhs;
    struct Node *rhs;

    /* var decl */
    char *var_name;
    struct Node *init;

    /* block */
    struct Node *stmts;

    /* if */
    struct Node *cond;
    struct Node *then_branch;
    struct Node *else_branch;

    /* while/for */
    struct Node *init_stmt;
    struct Node *cond_expr;
    struct Node *post_expr;

    /* function */
    char **params;
    int param_count;
    struct Node *body;

    /* call */
    struct Node **args;
    int arg_count;

    /* object literal */
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

static int tok_is_op(Token *t, int ch) {
    return t->kind == TK_OP && t->op == ch;
}

static int tok_is_op2(Token *t, int a, int b) {
    return t->kind == TK_OP && t->op == ((a<<8)|b);
}

static int accept_op(Parser *p, int ch) {
    Token tk = parser_peektok(p);
    if (tk.kind == TK_OP && tk.op == ch) {
        parser_nexttok(p);
        return 1;
    }
    return 0;
}

static int accept_keyword(Parser *p, const char *kw) {
    Token tk = parser_peektok(p);
    if (tk.kind == TK_KEYWORD && strcmp(tk.text, kw) == 0) {
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
        /* call? */
        Token pk = parser_peektok(p);
        if (pk.kind == TK_OP && pk.op == '(') {
            /* function call */
            parser_nexttok(p); /* consume '(' */
            Node *call = node_new(ND_CALL);
            call->name = xstrdup(n->name);
            call->line = n->line;
            /* args */
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
            return call;
        }
        return n;
    }
    if (tk.kind == TK_OP && tk.op == '(') {
        parser_nexttok(p);
        Node *n = parse_expression(p);
        expect_op(p, ')');
        return n;
    }

    /* object literal: { "a": 1, b: 2 } */
    if (tk.kind == TK_OP && tk.op == '{') {
        parser_nexttok(p);
        Node *obj = node_new(ND_OBJ_LITERAL);
        obj->kv_count = 0;
        obj->keys = NULL;
        obj->values = NULL;
        while (!(parser_peektok(p).kind == TK_OP && parser_peektok(p).op == '}')) {
            Token keytk = parser_peektok(p);
            char *keyname = NULL;
            if (keytk.kind == TK_STRING || keytk.kind == TK_IDENT) {
                parser_nexttok(p);
                keyname = xstrdup(keytk.text);
            } else {
                die("expected object key at line %d", keytk.line);
            }
            expect_op(p, ':');
            Node *val = parse_expression(p);
            obj->keys = xrealloc(obj->keys, sizeof(char*) * (obj->kv_count + 1));
            obj->values = xrealloc(obj->values, sizeof(Node*) * (obj->kv_count + 1));
            obj->keys[obj->kv_count] = keyname;
            obj->values[obj->kv_count] = val;
            obj->kv_count++;
            if (parser_peektok(p).kind == TK_OP && parser_peektok(p).op == ',') {
                parser_nexttok(p);
                continue;
            }
            break;
        }
        expect_op(p, '}');
        return obj;
    }

    die("unexpected token at line %d (kind %d, op %d)", tk.line, tk.kind, tk.op);
    return NULL;
}

/* operator precedence parsing (simple) */
static int get_precedence(Token *t) {
    if (t->kind != TK_OP) return -1;
    int op = t->op;
    if (op == '+' || op == '-') return 10;
    if (op == '*' || op == '/') return 20;
    if (op == '<' || op == '>' || op == ((int)'<'<<8 | '=' ) || op == ((int)'>'<<8 | '=')) return 5;
    if (op == ((int)'='<<8 | '=') || op == ((int)'!'<<8 | '=')) return 4;
    return -1;
}

static Node *parse_binop_rhs(Parser *p, int expr_prec, Node *lhs) {
    while (1) {
        Token next = parser_peektok(p);
        int tok_prec = get_precedence(&next);
        if (tok_prec < expr_prec) return lhs;

        Token op = parser_nexttok(p); /* consume operator */
        Node *rhs = parse_primary(p);
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
    Node *lhs = parse_primary(p);
    /* Check for assignment after primary expression (only if it's an IDENT) */
    if (lhs->kind == ND_IDENT) {
        Token pk = parser_peektok(p);
        if (pk.kind == TK_OP && pk.op == '=') {
            parser_nexttok(p); /* consume '=' */
            Node *rhs = parse_expression(p);
            Node *assign = node_new(ND_ASSIGN);
            assign->lhs = lhs; /* The IDENT node */
            assign->rhs = rhs; /* The assigned value expression */
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
    /* expression statement */
    Node *e = parse_expression(p);
    expect_op(p, ';');
    Node *es = node_new(ND_EXPR_STMT);
    es->lhs = e;
    return es;
}

/* top-level program parse */
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
   Code generation
   --------------------------- */

/* global tables for strings and globals */
typedef struct {
    char **names;
    int count;
} StrTable;

static StrTable g_strings = {NULL, 0};

static int add_string_literal(const char *s) {
    for (int i = 0; i < g_strings.count; ++i) {
        if (strcmp(g_strings.names[i], s) == 0) return i;
    }
    g_strings.names = xrealloc(g_strings.names, sizeof(char*) * (g_strings.count + 1));
    g_strings.names[g_strings.count] = xstrdup(s);
    return g_strings.count++;
}

typedef struct {
    char **names;
    int count;
} GlobalTable;

static GlobalTable g_globals = {NULL, 0};

static int add_global(const char *name) {
    for (int i = 0; i < g_globals.count; ++i) {
        if (strcmp(g_globals.names[i], name) == 0) return i;
    }
    g_globals.names = xrealloc(g_globals.names, sizeof(char*) * (g_globals.count + 1));
    g_globals.names[g_globals.count] = xstrdup(name);
    return g_globals.count++;
}

/* assembly output */
typedef struct {
    FILE *f;
    int label_id;
} CodeGen;

static void cg_emit(CodeGen *cg, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(cg->f, fmt, ap);
    fprintf(cg->f, "\n");
    va_end(ap);
}

static char *escape_asm_string(const char *s) {
    /* escape backslashes and double quotes and control characters for .asciz */
    size_t len = 0;
    for (const char *p = s; *p; ++p) {
        if (*p == '"' || *p == '\\' || *p == '\n' || *p == '\t' || *p == '\r') len += 2;
        else len++;
    }
    char *out = malloc(len + 1);
    if (!out) die("out of memory");
    char *q = out;
    for (const char *p = s; *p; ++p) {
        if (*p == '"') { *q++ = '\\'; *q++ = '"'; }
        else if (*p == '\\') { *q++ = '\\'; *q++ = '\\'; }
        else if (*p == '\n') { *q++ = '\\'; *q++ = 'n'; }
        else if (*p == '\t') { *q++ = '\\'; *q++ = 't'; }
        else if (*p == '\r') { *q++ = '\\'; *q++ = 'r'; }
        else *q++ = *p;
    }
    *q = 0;
    return out;
}


/* forward declarations */
static void cg_emit_stmt(CodeGen *cg, Node *n);
static void cg_emit_data(CodeGen *cg);
static void cg_emit_runtime(CodeGen *cg);
static void cg_emit_expr(CodeGen *cg, Node *n);

/* expression codegen returns result in %rax */
static void cg_emit_expr(CodeGen *cg, Node *n) {
    if (!n) { cg_emit(cg, "    mov $0, %%rax"); return; }
    switch (n->kind) {
    case ND_NUM:
        cg_emit(cg, "    mov $%ld, %%rax", n->num);
        return;
    case ND_STR: {
        int idx = add_string_literal(n->str);
        cg_emit(cg, "    lea Lstr_%d(%%rip), %%rax", idx);
        return;
    }
    case ND_IDENT: {
        /* load global variable (r-value) */
        int gi = add_global(n->name);
        cg_emit(cg, "    mov Lglob_%d(%%rip), %%rax", gi);
        return;
    }
    case ND_BINOP: {
        /* simple binary ops */
        cg_emit_expr(cg, n->lhs);
        cg_emit(cg, "    push %%rax");
        cg_emit_expr(cg, n->rhs);
        cg_emit(cg, "    mov %%rax, %%rbx");
        cg_emit(cg, "    pop %%rax");
        /* now lhs in %rax, rhs in %rbx */
        int op = n->op;
        if (op == '+') {
            cg_emit(cg, "    add %%rbx, %%rax");
        } else if (op == '-') {
            cg_emit(cg, "    sub %%rbx, %%rax");
        } else if (op == '*') {
            cg_emit(cg, "    imul %%rbx, %%rax");
        } else if (op == '/') {
            cg_emit(cg, "    cqo");
            cg_emit(cg, "    idiv %%rbx");
        } else if (op == ((int)'='<<8 | '=')) {
            cg_emit(cg, "    cmp %%rbx, %%rax");
            cg_emit(cg, "    sete %%al");
            cg_emit(cg, "    movzb %%al, %%rax");
        } else if (op == ((int)'!'<<8 | '=')) {
            cg_emit(cg, "    cmp %%rbx, %%rax");
            cg_emit(cg, "    setne %%al");
            cg_emit(cg, "    movzb %%al, %%rax");
        } else if (op == '<') {
            cg_emit(cg, "    cmp %%rbx, %%rax");
            cg_emit(cg, "    setl %%al");
            cg_emit(cg, "    movzb %%al, %%rax");
        } else if (op == '>') {
            cg_emit(cg, "    cmp %%rbx, %%rax");
            cg_emit(cg, "    setg %%al");
            cg_emit(cg, "    movzb %%al, %%rax");
        } else {
            die("unsupported binary op %d", op);
        }
        return;
    }
    case ND_ASSIGN: {
        /* evaluate RHS expression */
        cg_emit_expr(cg, n->rhs);
        /* result is in %rax. store it in the global variable for LHS. */
        if (n->lhs->kind != ND_IDENT) {
            die("assignment LHS must be an identifier");
        }
        int gi = add_global(n->lhs->name);
        cg_emit(cg, "    mov %%rax, Lglob_%d(%%rip)", gi);
        /* The result of the assignment expression is the assigned value (in %rax) */
        return;
    }
    case ND_CALL: {
        /* call expressions are handled at statement level in this simple compiler */
        die("unexpected call in expression position");
        return;
    }
    default:
        die("expr codegen not implemented for kind %d", n->kind);
    }
}

/* statement codegen */
static void cg_emit_stmt(CodeGen *cg, Node *n) {
    if (!n) return;
    switch (n->kind) {
    case ND_EXPR_STMT:
        if (n->lhs && n->lhs->kind == ND_CALL) {
            /* handle call expressions specially */
            Node *call = n->lhs;
            /* Special-case print in older examples; general calls handled below */
            if (strcmp(call->name, "print") == 0) {
                /* only support single-arg print */
                if (call->arg_count != 1) die("print expects 1 arg");
                Node *arg = call->args[0];
                if (arg->kind == ND_STR) {
                    /* use puts for strings: puts(char *s) */
                    cg_emit_expr(cg, arg);
                    cg_emit(cg, "    mov %%rax, %%rdi");
                    cg_emit(cg, "    call puts");
                } else {
                    /* evaluate expression into rax, move to rsi, call printf with %ld\n */
                    cg_emit_expr(cg, arg);
                    cg_emit(cg, "    mov %%rax, %%rsi");
                    cg_emit(cg, "    lea Lfmt_int(%%rip), %%rdi");
                    cg_emit(cg, "    xor %%rax, %%rax");
                    cg_emit(cg, "    call printf");
                }
                return;
            }
            /* general call: push args right-to-left, call, result in rax */
            for (int i = call->arg_count - 1; i >= 0; --i) {
                cg_emit_expr(cg, call->args[i]);
                cg_emit(cg, "    push %%rax");
            }
            /* move up to 6 args into registers */
            const char *argregs[] = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
            for (int i = 0; i < call->arg_count && i < 6; ++i) {
                cg_emit(cg, "    pop %s", argregs[i]);
            }
            if (call->arg_count > 6) {
                die("more than 6 args not supported");
            }
            cg_emit(cg, "    call %s", call->name);
            return;
        } else {
            cg_emit_expr(cg, n->lhs);
            return;
        }
    case ND_VARDECL: {
        /* global var flattening: if at top-level and init is object literal, flatten */
        if (n->init && n->init->kind == ND_OBJ_LITERAL) {
            for (int i = 0; i < n->init->kv_count; ++i) {
                char buf[256];
                snprintf(buf, sizeof(buf), "%s_%s", n->var_name, n->init->keys[i]);
                add_global(buf);
            }
            add_global(n->var_name);
            return;
        }
        add_global(n->var_name);
        if (n->init) {
            /* If init is an assignment, it will handle the store.
               Otherwise, just evaluate and store the result. */
            cg_emit_expr(cg, n->init);
            int gi = add_global(n->var_name);
            cg_emit(cg, "    mov %%rax, Lglob_%d(%%rip)", gi);
        }
        return;
    }
    case ND_BLOCK: {
        for (Node *s = n->stmts; s; s = s->next) cg_emit_stmt(cg, s);
        return;
    }
    case ND_IF: {
        int lid = ++cg->label_id;
        cg_emit_expr(cg, n->cond);
        cg_emit(cg, "    cmp $0, %%rax");
        if (n->else_branch) {
            cg_emit(cg, "    je .Lelse_%d", lid);
            cg_emit_stmt(cg, n->then_branch);
            cg_emit(cg, "    jmp .Lend_%d", lid);
            cg_emit(cg, ".Lelse_%d:", lid);
            cg_emit_stmt(cg, n->else_branch);
            cg_emit(cg, ".Lend_%d:", lid);
        } else {
            cg_emit(cg, "    je .Lend_%d", lid);
            cg_emit_stmt(cg, n->then_branch);
            cg_emit(cg, ".Lend_%d:", lid);
        }
        return;
    }
    case ND_WHILE: {
        int lid = ++cg->label_id;
        cg_emit(cg, ".Lwhile_begin_%d:", lid);
        cg_emit_expr(cg, n->cond);
        cg_emit(cg, "    cmp $0, %%rax");
        cg_emit(cg, "    je .Lwhile_end_%d", lid);
        cg_emit_stmt(cg, n->then_branch);
        cg_emit(cg, "    jmp .Lwhile_begin_%d", lid);
        cg_emit(cg, ".Lwhile_end_%d:", lid);
        return;
    }
    case ND_FOR: {
        int lid = ++cg->label_id;
        if (n->init_stmt) cg_emit_stmt(cg, n->init_stmt);
        cg_emit(cg, ".Lfor_begin_%d:", lid);
        if (n->cond_expr) {
            cg_emit_expr(cg, n->cond_expr);
            cg_emit(cg, "    cmp $0, %%rax");
            cg_emit(cg, "    je .Lfor_end_%d", lid);
        }
        cg_emit_stmt(cg, n->then_branch);
        if (n->post_expr) cg_emit_stmt(cg, n->post_expr);
        cg_emit(cg, "    jmp .Lfor_begin_%d", lid);
        cg_emit(cg, ".Lfor_end_%d:", lid);
        return;
    }
    case ND_RETURN: {
        if (n->lhs) cg_emit_expr(cg, n->lhs);
        cg_emit(cg, "    jmp .Lfunc_end_%s", "main"); /* simplified */
        return;
    }
    case ND_FUNC: {
        /* emit function label and prologue */
        cg_emit(cg, ".globl %s", n->name);
        cg_emit(cg, "%s:", n->name);
        cg_emit(cg, "    push %%rbp");
        cg_emit(cg, "    mov %%rsp, %%rbp");
        /* parameters are in registers; for simplicity we don't create locals */
        cg_emit_stmt(cg, n->body);
        cg_emit(cg, ".Lfunc_end_%s:", n->name);
        cg_emit(cg, "    mov $0, %%rax");
        cg_emit(cg, "    pop %%rbp");
        cg_emit(cg, "    ret");
        return;
    }
    case ND_PROGRAM: {
        /* emit main function that runs top-level statements */
        cg_emit(cg, ".globl main");
        cg_emit(cg, "main:");
        cg_emit(cg, "    push %%rbp");
        cg_emit(cg, "    mov %%rsp, %%rbp");
        /* initialize globals to 0 in .bss; runtime init for expressions */
        for (Node *s = n->stmts; s; s = s->next) {
            /* special-case top-level var decls with object flattening */
            if (s->kind == ND_VARDECL && s->init && s->init->kind == ND_OBJ_LITERAL) {
                /* create flattened globals and initialize them */
                for (int i = 0; i < s->init->kv_count; ++i) {
                    char buf[256];
                    snprintf(buf, sizeof(buf), "%s_%s", s->var_name, s->init->keys[i]);
                    /* evaluate value */
                    cg_emit_expr(cg, s->init->values[i]);
                    int gi = add_global(buf);
                    cg_emit(cg, "    mov %%rax, Lglob_%d(%%rip)", gi);
                }
                /* set top-level var to address of first key if exists */
                if (s->init->kv_count > 0) {
                    char buf[256];
                    snprintf(buf, sizeof(buf), "%s_%s", s->var_name, s->init->keys[0]);
                    int gi = add_global(buf);
                    cg_emit(cg, "    lea Lglob_%d(%%rip), %%rax", gi);
                    int gmain = add_global(s->var_name);
                    cg_emit(cg, "    mov %%rax, Lglob_%d(%%rip)", gmain);
                } else {
                    int gmain = add_global(s->var_name);
                    cg_emit(cg, "    mov $0, Lglob_%d(%%rip)", gmain);
                }
                continue;
            }
            cg_emit_stmt(cg, s);
        }
        /* call exit(0) */
        cg_emit(cg, "    mov $0, %%edi");
        cg_emit(cg, "    call exit");
        cg_emit(cg, "    pop %%rbp");
        cg_emit(cg, "    ret");
        return;
    }
    default:
        die("stmt codegen not implemented for kind %d", n->kind);
    }
}

/* emit data section for strings and globals */
static void cg_emit_data(CodeGen *cg) {
    cg_emit(cg, ".section .rodata");
    /* format strings for printing */
    cg_emit(cg, "Lfmt_int: .asciz \"%%ld\\n\"");
    for (int i = 0; i < g_strings.count; ++i) {
        char *esc = escape_asm_string(g_strings.names[i]);
        cg_emit(cg, "Lstr_%d: .asciz \"%s\"", i, esc);
        free(esc);
    }
    cg_emit(cg, ".section .data");
    for (int i = 0; i < g_globals.count; ++i) {
        cg_emit(cg, "Lglob_%d: .quad 0", i);
    }
}

/* append runtime assembly stubs for printing (none needed; use libc) */
static void cg_emit_runtime(CodeGen *cg) {
    cg_emit(cg, ".section .text");
}

/* top-level compile function */
static void compile_to_asm(const char *src, const char *outpath) {
    Parser p;
    parser_init(&p, src);
    /* DO NOT call parser_nexttok here — parse_program uses parser_peektok to fetch the first token.
       Calling parser_nexttok would consume the first token and shift the stream, causing parse errors. */
    Node *prog = parse_program(&p);

    FILE *f = fopen(outpath, "w");
    if (!f) die("cannot open output file %s: %s", outpath, strerror(errno));
    CodeGen cg = {f, 0};

    /* generate AT&T-style assembly (do not emit .intel_syntax) */
    cg_emit(&cg, ".section .text");

    /* generate program */
    cg_emit_stmt(&cg, prog);

    /* emit data */
    cg_emit_data(&cg);

    /* runtime stubs (none needed) */
    cg_emit_runtime(&cg);

    fclose(f);
}

/* ---------------------------
   Simple test harness
   --------------------------- */

static void run_command(const char *fmt, ...) {
    char cmd[4096];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(cmd, sizeof(cmd), fmt, ap);
    va_end(ap);
    int rc = system(cmd);
    if (rc != 0) {
        fprintf(stderr, "command failed: %s\n", cmd);
    }
}

static void run_tests(void) {
    /* Tests use printf directly so the generated assembly calls printf
       and output is produced reliably. Strings in the test sources must
       escape backslashes for C string literals in this file. */
    struct Test { const char *name; const char *src; const char *expect; } tests[] = {
        { "print-number", "printf(\"%ld\\n\", 42);", "42\n" },
        { "add", "printf(\"%ld\\n\", 1+2*3);", "7\n" },
        { "if-else", "if (1) { printf(\"%ld\\n\", 10); } else { printf(\"%ld\\n\", 20); }", "10\n" },
        /* KORREKTUR: Zuweisung in i = i + 1; funktioniert jetzt dank ND_ASSIGN-Implementierung */
        { "while", "var i=0; while (i<3) { printf(\"%ld\\n\", i); i = i + 1; }", "0\n1\n2\n" },
        { "obj-flatten", "var obj = {a:1, b:2}; printf(\"%ld\\n\", obj_a); printf(\"%ld\\n\", obj_b);", "1\n2\n" },
        { NULL, NULL, NULL }
    };

    int passed_count = 0;
    int total_count = 0;

    for (int i = 0; tests[i].name; ++i) {
        total_count++;
        char asmfile[256], exe[256];
        snprintf(asmfile, sizeof(asmfile), "test_%s.s", tests[i].name);
        snprintf(exe, sizeof(exe), "test_%s", tests[i].name);

        /* reset tables */
        for (int j = 0; j < g_strings.count; ++j) free(g_strings.names[j]);
        free(g_strings.names); g_strings.names = NULL; g_strings.count = 0;
        for (int j = 0; j < g_globals.count; ++j) free(g_globals.names[j]);
        free(g_globals.names); g_globals.names = NULL; g_globals.count = 0;

        printf("\nCompiling test %s...\n", tests[i].name);
        compile_to_asm(tests[i].src, asmfile);

        /* assemble and link */
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "gcc -no-pie -o %s %s -O2", exe, asmfile);
        int rc = system(cmd);
        if (rc != 0) {
            fprintf(stderr, "compile failed for %s\n", tests[i].name);
            continue;
        }

        /* run and capture output */
        snprintf(cmd, sizeof(cmd), "./%s > out.txt 2>&1", exe);
        rc = system(cmd);
        if (rc != 0) {
            fprintf(stderr, "run failed for %s\n", tests[i].name);
            continue;
        }

        /* show output and check against expected */
        FILE *f = fopen("out.txt", "r");
        if (!f) {
            fprintf(stderr, "cannot open out.txt\n");
            continue;
        }
        char buf[8192]; size_t n = fread(buf, 1, sizeof(buf)-1, f); buf[n] = 0; fclose(f);

        printf("--- Source ---\n%s\n", tests[i].src);
        printf("--- Output ---\n%s", buf);
        printf("--- Expect ---\n%s", tests[i].expect);

        if (strcmp(buf, tests[i].expect) == 0) {
            printf("\n✅ TEST PASSED: %s\n", tests[i].name);
            passed_count++;
        } else {
            printf("\n❌ TEST FAILED: %s\n", tests[i].name);
        }
    }
    printf("\n\n--- Zusammenfassung ---\n%d von %d Tests erfolgreich bestanden.\n", passed_count, total_count);
}

/* ---------------------------
   Main
   --------------------------- */

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("sparrow: running built-in tests\n");
        run_tests();
        return 0;
    }
    const char *outpath = "out.s";
    const char *inpath = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i+1 < argc) { outpath = argv[++i]; continue; }
        inpath = argv[i];
    }
    if (!inpath) die("no input file");
    /* read input */
    FILE *f = fopen(inpath, "r");
    if (!f) die("cannot open %s: %s", inpath, strerror(errno));
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(sz + 1);
    if (!src) die("out of memory");
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);

    compile_to_asm(src, outpath);
    printf("wrote %s\n", outpath);
    return 0;
}
