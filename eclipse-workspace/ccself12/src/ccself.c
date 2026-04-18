// cc.c
// Minimal educational single-file C compiler (x86-64)
// Read one or more .c files, parse a small subset of C, and emit x86-64 assembly to stdout.
// Build: gcc -O2 -o cc cc.c
// Usage: ./cc file.c > out.s && gcc out.s -o out

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ----------------------------- Tokenizer -----------------------------

typedef enum {
    TK_RESERVED, // operators or punctuators
    TK_IDENT,    // identifier
    TK_NUM,      // numeric literal
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    long val;        // if kind == TK_NUM
    char *str;       // token string (pointer into input buffer)
    int len;         // token length
};

static char *user_input;
static Token *token;

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int line = 1;
    char *line_start = user_input;
    for (char *p = user_input; p < loc; p++) {
        if (*p == '\n') { line++; line_start = p + 1; }
    }
    char *line_end = loc;
    while (*line_end && *line_end != '\n') line_end++;
    int col = loc - line_start + 1;

    fprintf(stderr, "error: line %d:%d: ", line, col);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);
    fprintf(stderr, "%*s^\n", col - 1, "");
    exit(1);
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

static bool startswith(char *p, char *q) {
    return strncmp(p, q, strlen(q)) == 0;
}

static bool is_ident_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static Token *new_token(TokenKind kind, char *start, int len) {
    Token *t = calloc(1, sizeof(Token));
    t->kind = kind;
    t->str = start;
    t->len = len;
    return t;
}

static Token *tokenize(char *p) {
    user_input = p;
    Token head = {};
    Token *cur = &head;

    while (*p) {
        // skip whitespace
        if (isspace((unsigned char)*p)) { p++; continue; }

        // comment //
        if (p[0] == '/' && p[1] == '/') {
            p += 2;
            while (*p && *p != '\n') p++;
            continue;
        }

        // comment /* */
        if (p[0] == '/' && p[1] == '*') {
            p += 2;
            while (*p && !(p[0] == '*' && p[1] == '/')) p++;
            if (*p) p += 2;
            continue;
        }

        // ellipsis
        if (startswith(p, "...")) {
            cur->next = new_token(TK_RESERVED, p, 3);
            cur = cur->next;
            p += 3;
            continue;
        }

        // multi-char punctuators
        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=") ||
            startswith(p, "&&") || startswith(p, "||") ||
            startswith(p, "<<") || startswith(p, ">>") ||
            startswith(p, "+=") || startswith(p, "-=") ||
            startswith(p, "*=") || startswith(p, "/=") ||
            startswith(p, "&=") || startswith(p, "|=") ||
            startswith(p, "^=")) {
            cur->next = new_token(TK_RESERVED, p, 2);
            cur = cur->next;
            p += 2;
            continue;
        }

        // three-char operators like <<= or >>=
        if (startswith(p, "<<=") || startswith(p, ">>=")) {
            cur->next = new_token(TK_RESERVED, p, 3);
            cur = cur->next;
            p += 3;
            continue;
        }

        // single-char punctuators
        if (strchr("+-*/()<>;={},&|^~![]:.%", *p) || *p == ',' ) {
            cur->next = new_token(TK_RESERVED, p, 1);
            cur = cur->next;
            p++;
            continue;
        }

        // identifier or keyword
        if (isalpha((unsigned char)*p) || *p == '_') {
            char *q = p;
            while (is_ident_char(*q)) q++;
            cur->next = new_token(TK_IDENT, p, (int)(q - p));
            cur = cur->next;
            p = q;
            continue;
        }

        // number
        if (isdigit((unsigned char)*p)) {
            char *q = p;
            long val = strtol(p, &q, 0);
            cur->next = new_token(TK_NUM, p, (int)(q - p));
            cur = cur->next;
            cur->val = val;
            p = q;
            continue;
        }

        // unknown
        error_at(p, "invalid token");
    }

    cur->next = new_token(TK_EOF, p, 0);
    return head.next;
}

// ----------------------------- AST / Parser -----------------------------

typedef enum {
    ND_ADD, ND_SUB, ND_MUL, ND_DIV,
    ND_EQ, ND_NE, ND_LT, ND_LE,
    ND_NUM, ND_VAR, ND_ASSIGN, ND_EXPR_STMT,
    ND_RETURN, ND_IF, ND_WHILE, ND_FOR, ND_BLOCK,
    ND_FUNCALL, ND_ADDR, ND_DEREF,
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *next; // for block or argument list
    long val;   // for ND_NUM
    char *name; // for var or func name (pointer into input)
    int len;    // name length
    int offset; // local variable offset
};

typedef struct LVar LVar;
struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

typedef struct Function Function;
struct Function {
    Function *next;
    char *name;
    int len;
    Node *body;
    LVar *locals;
    int stack_size;
    int params; // number of parameters
};

static Token *peek_token() { return token; }

static bool consume(char *op) {
    if (token->kind != TK_RESERVED) return false;
    if ((int)strlen(op) != token->len) return false;
    if (strncmp(token->str, op, token->len) != 0) return false;
    token = token->next;
    return true;
}

static Token *consume_ident() {
    if (token->kind != TK_IDENT) return NULL;
    Token *t = token;
    token = token->next;
    return t;
}

static void expect(char *op) {
    if (!consume(op)) {
        if (token && token->str) error_at(token->str, "expected '%s'", op);
        else error("expected '%s'", op);
    }
}

static long expect_number() {
    if (token->kind != TK_NUM) {
        if (token && token->str) error_at(token->str, "expected a number");
        else error("expected a number");
    }
    long val = token->val;
    token = token->next;
    return val;
}

static bool at_eof() { return token->kind == TK_EOF; }

// symbol table for locals (per function)
static LVar *locals;

static LVar *find_lvar(Token *tok) {
    for (LVar *v = locals; v; v = v->next) {
        if (v->len == tok->len && strncmp(v->name, tok->str, tok->len) == 0)
            return v;
    }
    return NULL;
}

static Node *new_node(NodeKind kind) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = kind;
    return n;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *n = new_node(kind);
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

static Node *new_num(long val) {
    Node *n = new_node(ND_NUM);
    n->val = val;
    return n;
}

static Node *new_var_node(Token *tok) {
    Node *n = new_node(ND_VAR);
    n->name = tok->str;
    n->len = tok->len;
    LVar *v = find_lvar(tok);
    if (!v) error_at(tok->str, "undefined variable");
    n->offset = v->offset;
    return n;
}

// forward declarations
static Node *stmt();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

static Function *functions = NULL;
static Function *curfn = NULL;

static Node *primary() {
    if (consume("(")) {
        Node *n = expr();
        expect(")");
        return n;
    }

    Token *t = consume_ident();
    if (t) {
        // function call or variable
        if (consume("(")) {
            Node *n = new_node(ND_FUNCALL);
            n->name = t->str;
            n->len = t->len;
            // parse args (comma separated)
            if (!consume(")")) {
                Node *arg = expr();
                n->lhs = arg;
                while (consume(",")) {
                    Node *more = expr();
                    Node *p = n->lhs;
                    while (p->next) p = p->next;
                    p->next = more;
                }
                expect(")");
            }
            return n;
        } else {
            // variable
            return new_var_node(t);
        }
    }

    if (token->kind == TK_NUM) {
        Node *n = new_num(token->val);
        token = token->next;
        return n;
    }

    if (token->kind == TK_EOF) error("unexpected end of input");
    error_at(token->str, "expected an expression");
    return NULL;
}

static Node *unary() {
    if (consume("+")) return unary();
    if (consume("-")) return new_binary(ND_SUB, new_num(0), unary());
    if (consume("&")) {
        Node *n = new_node(ND_ADDR);
        n->lhs = unary();
        return n;
    }
    if (consume("*")) {
        Node *n = new_node(ND_DEREF);
        n->lhs = unary();
        return n;
    }
    return primary();
}

static Node *mul() {
    Node *n = unary();
    for (;;) {
        if (consume("*")) n = new_binary(ND_MUL, n, unary());
        else if (consume("/")) n = new_binary(ND_DIV, n, unary());
        else break;
    }
    return n;
}

static Node *add() {
    Node *n = mul();
    for (;;) {
        if (consume("+")) n = new_binary(ND_ADD, n, mul());
        else if (consume("-")) n = new_binary(ND_SUB, n, mul());
        else break;
    }
    return n;
}

static Node *relational() {
    Node *n = add();
    for (;;) {
        if (consume("<")) n = new_binary(ND_LT, n, add());
        else if (consume("<=")) n = new_binary(ND_LE, n, add());
        else if (consume(">")) n = new_binary(ND_LT, add(), n);
        else if (consume(">=")) n = new_binary(ND_LE, add(), n);
        else break;
    }
    return n;
}

static Node *equality() {
    Node *n = relational();
    for (;;) {
        if (consume("==")) n = new_binary(ND_EQ, n, relational());
        else if (consume("!=")) n = new_binary(ND_NE, n, relational());
        else break;
    }
    return n;
}

static Node *assign() {
    Node *n = equality();
    if (consume("=")) n = new_binary(ND_ASSIGN, n, assign());
    return n;
}

static Node *expr() {
    return assign();
}

static Node *stmt() {
    if (consume(";")) return NULL;

    if (consume("return")) {
        Node *n = new_node(ND_RETURN);
        n->lhs = expr();
        expect(";");
        return n;
    }

    if (consume("if")) {
        expect("(");
        Node *n = new_node(ND_IF);
        n->cond = expr();
        expect(")");
        n->then = stmt();
        if (consume("else")) n->els = stmt();
        return n;
    }

    if (consume("while")) {
        expect("(");
        Node *n = new_node(ND_WHILE);
        n->cond = expr();
        expect(")");
        n->then = stmt();
        return n;
    }

    if (consume("for")) {
        expect("(");
        Node *n = new_node(ND_FOR);
        if (!consume(";")) {
            n->init = expr();
            expect(";");
        }
        if (!consume(";")) {
            n->cond = expr();
            expect(";");
        }
        if (!consume(")")) {
            n->inc = expr();
            expect(")");
        }
        n->then = stmt();
        return n;
    }

    if (consume("{")) {
        Node *n = new_node(ND_BLOCK);
        Node head = {};
        Node *cur = &head;
        while (!consume("}")) {
            Node *s = stmt();
            if (s) {
                cur->next = s;
                cur = cur->next;
            }
        }
        n->body = head.next;
        return n;
    }

    // expression statement
    Node *n = expr();
    expect(";");
    Node *es = new_node(ND_EXPR_STMT);
    es->lhs = n;
    return es;
}

// parse function: very simple "int name(params) { ... }"
static Function *parse_function() {
    // expect "int"
    if (!consume("int")) return NULL;
    Token *t = consume_ident();
    if (!t) error_at(token->str, "expected function name");
    Function *fn = calloc(1, sizeof(Function));
    fn->name = t->str;
    fn->len = t->len;

    expect("(");
    int param_count = 0;
    if (!consume(")")) {
        Token *p = consume_ident();
        if (!p) error_at(token->str, "expected parameter name");
        param_count++;
        while (consume(",")) {
            Token *q = consume_ident();
            if (!q) error_at(token->str, "expected parameter name");
            param_count++;
        }
        expect(")");
    }
    fn->params = param_count;

    // parse body
    Node *body = stmt();
    fn->body = body;

    fn->locals = NULL;
    fn->stack_size = 0;
    return fn;
}

static void parse(Token *tok) {
    token = tok;
    functions = NULL;
    while (!at_eof()) {
        Function *fn = parse_function();
        if (!fn) error_at(token->str, "expected function");
        fn->next = functions;
        functions = fn;
    }
}

// ----------------------------- Semantic: collect locals -----------------------------

// Walk AST and collect local variables (heuristic: any ND_ASSIGN with ND_VAR on LHS or ND_VAR usage)
static Function *cur_function;

static void collect_locals_node(Node *n) {
    if (!n) return;
    switch (n->kind) {
    case ND_ASSIGN:
        if (n->lhs && n->lhs->kind == ND_VAR) {
            // find in current function's locals
            LVar *v = NULL;
            for (LVar *p = cur_function->locals; p; p = p->next) {
                if (p->len == n->lhs->len && strncmp(p->name, n->lhs->name, p->len) == 0) { v = p; break; }
            }
            if (!v) {
                v = calloc(1, sizeof(LVar));
                v->name = n->lhs->name;
                v->len = n->lhs->len;
                v->next = cur_function->locals;
                cur_function->locals = v;
            }
        }
        break;
    case ND_VAR: {
        LVar *v = NULL;
        for (LVar *p = cur_function->locals; p; p = p->next) {
            if (p->len == n->len && strncmp(p->name, n->name, p->len) == 0) { v = p; break; }
        }
        if (!v) {
            v = calloc(1, sizeof(LVar));
            v->name = n->name;
            v->len = n->len;
            v->next = cur_function->locals;
            cur_function->locals = v;
        }
        break;
    }
    default:
        break;
    }
    collect_locals_node(n->lhs);
    collect_locals_node(n->rhs);
    collect_locals_node(n->cond);
    collect_locals_node(n->then);
    collect_locals_node(n->els);
    collect_locals_node(n->init);
    collect_locals_node(n->inc);
    collect_locals_node(n->body);
    collect_locals_node(n->next);
}

static void assign_lvar_offsets(Function *fn) {
    int offset = 0;
    for (LVar *v = fn->locals; v; v = v->next) {
        offset += 8;
        v->offset = offset;
    }
    fn->stack_size = (offset + 15) / 16 * 16; // align to 16
}

static void collect_locals(Function *fn) {
    cur_function = fn;
    collect_locals_node(fn->body);
    assign_lvar_offsets(fn);
}

// ----------------------------- Code generation -----------------------------

static void gen(Node *n);

static void gen_stmt(Node *n) {
    if (!n) return;
    switch (n->kind) {
    case ND_RETURN:
        gen(n->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_EXPR_STMT:
        gen(n->lhs);
        printf("  add rsp, 8\n"); // pop result
        return;
    case ND_IF: {
        int id = rand();
        int l1 = id & 0xffff;
        int l2 = (id ^ 0x1234) & 0xffff;
        gen(n->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        if (n->els) {
            printf("  je .Lelse%d\n", l1);
            gen_stmt(n->then);
            printf("  jmp .Lend%d\n", l2);
            printf(".Lelse%d:\n", l1);
            gen_stmt(n->els);
            printf(".Lend%d:\n", l2);
        } else {
            printf("  je .Lend%d\n", l1);
            gen_stmt(n->then);
            printf(".Lend%d:\n", l1);
        }
        return;
    }
    case ND_WHILE: {
        int id = rand();
        int l1 = id & 0xffff;
        int l2 = (id ^ 0x4321) & 0xffff;
        printf(".Lbegin%d:\n", l1);
        gen(n->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", l2);
        gen_stmt(n->then);
        printf("  jmp .Lbegin%d\n", l1);
        printf(".Lend%d:\n", l2);
        return;
    }
    case ND_FOR: {
        int id = rand();
        int l1 = id & 0xffff;
        int l2 = (id ^ 0x9999) & 0xffff;
        if (n->init) { gen(n->init); printf("  add rsp, 8\n"); }
        printf(".Lbegin%d:\n", l1);
        if (n->cond) { gen(n->cond); printf("  pop rax\n"); printf("  cmp rax, 0\n"); printf("  je .Lend%d\n", l2); }
        gen_stmt(n->then);
        if (n->inc) { gen(n->inc); printf("  add rsp, 8\n"); }
        printf("  jmp .Lbegin%d\n", l1);
        printf(".Lend%d:\n", l2);
        return;
    }
    case ND_BLOCK: {
        for (Node *p = n->body; p; p = p->next) gen_stmt(p);
        return;
    }
    default:
        gen(n);
        printf("  add rsp, 8\n");
        return;
    }
}

static void gen(Node *n) {
    if (!n) return;
    switch (n->kind) {
    case ND_NUM:
        printf("  push %ld\n", n->val);
        return;
    case ND_VAR:
        // load variable value
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", n->offset);
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        if (n->lhs->kind == ND_VAR) {
            gen(n->rhs);
            printf("  pop rax\n");
            printf("  mov rdi, rbp\n");
            printf("  sub rdi, %d\n", n->lhs->offset);
            printf("  mov [rdi], rax\n");
            printf("  push rax\n");
            return;
        } else {
            error("not a variable on LHS of assignment");
        }
    case ND_ADD:
        gen(n->lhs);
        gen(n->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  add rax, rdi\n");
        printf("  push rax\n");
        return;
    case ND_SUB:
        gen(n->lhs);
        gen(n->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  sub rax, rdi\n");
        printf("  push rax\n");
        return;
    case ND_MUL:
        gen(n->lhs);
        gen(n->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  imul rax, rdi\n");
        printf("  push rax\n");
        return;
    case ND_DIV:
        gen(n->lhs);
        gen(n->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  cqo\n");
        printf("  idiv rdi\n");
        printf("  push rax\n");
        return;
    case ND_EQ: case ND_NE: case ND_LT: case ND_LE:
        gen(n->lhs);
        gen(n->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  cmp rax, rdi\n");
        if (n->kind == ND_EQ) printf("  sete al\n");
        else if (n->kind == ND_NE) printf("  setne al\n");
        else if (n->kind == ND_LT) printf("  setl al\n");
        else if (n->kind == ND_LE) printf("  setle al\n");
        printf("  movzb rax, al\n");
        printf("  push rax\n");
        return;
    case ND_FUNCALL: {
        // evaluate args left-to-right and push them
        int nargs = 0;
        Node *arg = n->lhs;
        Node *args[16];
        int i = 0;
        for (Node *p = arg; p; p = p->next) {
            args[i++] = p;
        }
        nargs = i;
        for (int j = nargs - 1; j >= 0; j--) gen(args[j]);
        // call
        printf("  call %.*s\n", n->len, n->name);
        if (nargs > 0) printf("  add rsp, %d\n", nargs * 8);
        printf("  push rax\n");
        return;
    }
    case ND_ADDR:
        error("address-of not implemented");
    case ND_DEREF:
        error("deref not implemented");
    default:
        error("unknown node kind");
    }
}

static void gen_function(Function *fn) {
    collect_locals(fn);

    // set offsets for ND_VAR nodes
    void set_offsets(Node *n) {
        if (!n) return;
        if (n->kind == ND_VAR) {
            for (LVar *v = fn->locals; v; v = v->next) {
                if (v->len == n->len && strncmp(v->name, n->name, v->len) == 0) {
                    n->offset = v->offset;
                    break;
                }
            }
        }
        set_offsets(n->lhs);
        set_offsets(n->rhs);
        set_offsets(n->cond);
        set_offsets(n->then);
        set_offsets(n->els);
        set_offsets(n->init);
        set_offsets(n->inc);
        set_offsets(n->body);
        set_offsets(n->next);
    }
    set_offsets(fn->body);

    printf(".global %.*s\n", fn->len, fn->name);
    printf("%.*s:\n", fn->len, fn->name);
    // prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    if (fn->stack_size > 0) printf("  sub rsp, %d\n", fn->stack_size);

    // zero-initialize locals
    for (LVar *v = fn->locals; v; v = v->next) {
        printf("  mov qword ptr [rbp - %d], 0\n", v->offset);
    }

    gen_stmt(fn->body);

    // default return 0
    printf("  mov rax, 0\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

// ----------------------------- Main / IO -----------------------------

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); exit(1); }
    if (fseek(f, 0, SEEK_END) != 0) { perror("fseek"); exit(1); }
    long sz = ftell(f);
    rewind(f);
    char *buf = malloc(sz + 1);
    if (!buf) { perror("malloc"); exit(1); }
    if (fread(buf, 1, sz, f) != (size_t)sz) { perror("fread"); exit(1); }
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: cc file.c\n");
        return 1;
    }

    // seed rand for label generation
    srand((unsigned)time(NULL));

    // read and concatenate input files
    size_t total = 0;
    char *all = NULL;
    for (int i = 1; i < argc; i++) {
        char *s = read_file(argv[i]);
        size_t len = strlen(s);
        all = realloc(all, total + len + 2);
        memcpy(all + total, s, len);
        total += len;
        all[total++] = '\n';
        all[total] = '\0';
        free(s);
    }

    Token *tok = tokenize(all);
    parse(tok);

    // emit assembly for each function (functions list is reversed)
    for (Function *f = functions; f; f = f->next) {
        gen_function(f);
    }

    free(all);
    return 0;
}
