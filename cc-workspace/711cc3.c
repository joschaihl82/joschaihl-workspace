#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

// ELF structures and constants
typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_Phdr;

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define EV_CURRENT 1
#define ET_EXEC 2
#define EM_X86_64 62
#define PT_LOAD 1
#define PF_X 1
#define PF_R 4
#define PF_W 2
#define SHN_UNDEF 0

// Types, AST, symbols
typedef struct Type Type;
typedef struct Member Member;
typedef struct Node Node;
typedef struct Var Var;
typedef struct Function Function;

enum {
    TY_VOID, TY_BOOL, TY_CHAR, TY_SHORT, TY_INT, TY_LONG, TY_ENUM,
    TY_PTR, TY_ARRAY, TY_FUNC, TY_STRUCT, TY_UNION,
};

struct Type {
    int kind;
    int size;
    int align;
    Type *base;
    Type *return_ty;
    bool is_variadic;
    Member *members; // for struct members or function params (reused)
    char *name;
    int enum_val; // Used for enum types
};

struct Member {
    Member *next;
    Type *ty;
    char *name;
    int offset;
    int align;
};

struct Node {
    enum {
        ND_ADD, ND_SUB, ND_MUL, ND_DIV, ND_NEG,
        ND_EQ, ND_NE, ND_LT, ND_LE, ND_GT, ND_GE,
        ND_ASSIGN, ND_LVAR, ND_NUM, ND_ADDR, ND_DEREF,
        ND_IF, ND_FOR, ND_WHILE, ND_BLOCK, ND_FUNCALL,
        ND_STMT_EXPR, ND_RETURN, ND_SIZEOF, ND_MEMBER,
        ND_PREINC, ND_PREDEC, ND_POSTINC, ND_POSTDEC,
    } kind;
    Node *next;
    Node *lhs;
    Node *rhs;
    Type *ty;
    int val;
    char *name;
    Var *var;
    Node *args;
    Node *cond, *then, *els, *init, *inc;
    Node *body;
    Node *expr;
    /* ND_MEMBER specifics */
    char *member_name;
    Member *member;
};

struct Var {
    Var *next;
    char *name;
    Type *ty;
    int offset;
};

struct Function {
    Function *next;
    char *name;
    Type *ty;
    Var *params;
    Node *body;
    Var *locals;
    int stack_size;
    int code_offset;
    int rsp_offset;
};

// Globals
static Var *locals;
static Var *globals;
static Function *functions;
static Type *void_type = &(Type){TY_VOID, 1, 1};
static Type *bool_type = &(Type){TY_BOOL, 1, 1};
static Type *char_type = &(Type){TY_CHAR, 1, 1};
static Type *int_type = &(Type){TY_INT, 4, 4};
static Type *long_type = &(Type){TY_LONG, 8, 8};
static Function *current_fn = NULL;

// Call patch list
typedef struct CallPatch {
    struct CallPatch *next;
    int pos;
    char *name;
} CallPatch;
static CallPatch *call_patches = NULL;

// Tokenizer
typedef enum { TK_RESERVED, TK_IDENT, TK_NUM, TK_EOF } TokenKind;
typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *loc;
    int len;
};

static Token *token;
static char *current_input;

static void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    // Calculate line number and column
    int line_num = 1;
    int col = 1;
    char *p = current_input;
    char *line_start = current_input;
    for (; p < loc; p++) {
        if (*p == '\n') {
            line_num++;
            col = 1;
            line_start = p + 1;
        } else {
            col++;
        }
    }

    // Find end of line
    char *line_end = line_start;
    while (*line_end && *line_end != '\n') line_end++;

    // Print error message with line number and line
    fprintf(stderr, "error at line %d:\n", line_num);
    fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);
    fprintf(stderr, "%*s^\n", col - 1, "");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

static bool equal(char *op) {
    if (strlen(op) != token->len || strncmp(token->loc, op, token->len))
        return false;
    token = token->next;
    return true;
}

static void expect(char *op) {
    if (!equal(op))
        error_at(token->loc, "expected \"%s\"", op);
}

static int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->loc, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

static Token *new_token(TokenKind kind, char *start, char *end) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = end - start;
    return tok;
}

static bool is_ident1(char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'; }
static bool is_ident2(char c) { return is_ident1(c) || ('0' <= c && c <= '9'); }

static Token *tokenize() {
    char *p = current_input;
    Token head = {};
    Token *cur = &head;

    #define STARTS_WITH(s) (strncmp(p, (s), strlen(s)) == 0)

    while (*p) {
        if (isspace(*p)) { p++; continue; }

        if (*p == '"') {
            char *q = p++;
            while (*p && *p != '"') {
                if (*p == '\\' && p[1]) p += 2; else p++;
            }
            if (*p == '"') p++;
            cur = cur->next = new_token(TK_RESERVED, q, p);
            continue;
        }

        if (STARTS_WITH("==") || STARTS_WITH("!=") || STARTS_WITH("<=") || STARTS_WITH(">=") ||
            STARTS_WITH("->") || STARTS_WITH("++") || STARTS_WITH("--")) {
            cur = cur->next = new_token(TK_RESERVED, p, p + 2);
            p += 2;
            continue;
        }

        if (isdigit(*p)) {
            char *q = p;
            long val = strtol(p, &p, 10);
            cur = cur->next = new_token(TK_NUM, q, p);
            cur->val = (int)val;
            continue;
        }

        if (is_ident1(*p)) {
            char *q = p++;
            while (is_ident2(*p)) p++;
            cur = cur->next = new_token(TK_IDENT, q, p);
            continue;
        }

        char *q = p++;
        cur = cur->next = new_token(TK_RESERVED, q, p);
    }
    #undef STARTS_WITH
    if (p > current_input) p--;
    cur = cur->next = new_token(TK_EOF, p, p);
    return head.next;
}

// Parser helpers and AST builders
static Var *find_var_by_name(char *name, int len) {
    for (Var *var = locals; var; var = var->next) {
        if ((int)strlen(var->name) == len && !strncmp(var->name, name, len))
            return var;
    }
    for (Var *var = globals; var; var = var->next) {
        if ((int)strlen(var->name) == len && !strncmp(var->name, name, len))
            return var;
    }
    return NULL;
}

static Node *new_node(int kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

static Node *new_var_node(Var *var) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->var = var;
    node->ty = var->ty;  // Set type
    return node;
}

// Forward declarations for parser functions that are used across each other
static Node *expr();
static Node *stmt();
static Type *basetype();
static Type *declarator(Type *base, char **out_name);
static void compute_struct_layout(Type *st);
static Member *find_member(Type *st, const char *name);

// primary/postfix/unary/mul/add/relational/equality/assign/expr
static Node *postfix(); // forward

static Node *primary() {
    if (equal("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }
    Token *tok = token;
    if (tok->kind == TK_NUM) return new_num(expect_number());
    if (tok->kind == TK_IDENT) {
        char *name_start = tok->loc;
        int name_len = tok->len;
        char *name = strndup(name_start, name_len);
        token = token->next;
        if (equal("(")) {
            Node *head = NULL;
            Node **cur = &head;
            if (!equal(")")) {
                *cur = expr(); cur = &((*cur)->next);
                while (equal(",")) { *cur = expr(); cur = &((*cur)->next); }
                expect(")");
            }
            Node *call = calloc(1, sizeof(Node));
            call->kind = ND_FUNCALL;
            call->name = name;
            call->args = head;
            return call;
        }
        Var *v = find_var_by_name(name_start, name_len);
        if (!v) error_at(tok->loc, "undefined variable: %.*s", name_len, name_start);
        return new_var_node(v);
    }
    error_at(tok->loc, "expected expression");
}

static Node *postfix() {
    Node *node = primary();
    for (;;) {
        if (equal("[")) {
            Node *idx = expr();
            expect("]");
            Node *addn = new_node(ND_ADD, node, idx);
            Node *deref = new_node(ND_DEREF, addn, NULL);
            node = deref;
            continue;
        }
        if (equal(".")) {
            if (token->kind != TK_IDENT) error_at(token->loc, "expected member name after .");
            char *mname = strndup(token->loc, token->len);
            token = token->next;
            Node *mnode = calloc(1, sizeof(Node));
            mnode->kind = ND_MEMBER;
            mnode->lhs = node;
            mnode->member_name = mname;
            node = mnode;
            continue;
        }
        if (equal("->")) {
            if (token->kind != TK_IDENT) error_at(token->loc, "expected member name after ->");
            char *mname = strndup(token->loc, token->len);
            token = token->next;
            Node *d = new_node(ND_DEREF, node, NULL);
            Node *mnode = calloc(1, sizeof(Node));
            mnode->kind = ND_MEMBER;
            mnode->lhs = d;
            mnode->member_name = mname;
            node = mnode;
            continue;
        }
        if (equal("++")) {
            Node *n = calloc(1, sizeof(Node));
            n->kind = ND_POSTINC;
            n->lhs = node;
            node = n;
            continue;
        }
        if (equal("--")) {
            Node *n = calloc(1, sizeof(Node));
            n->kind = ND_POSTDEC;
            n->lhs = node;
            node = n;
            continue;
        }
        break;
    }
    return node;
}

static Node *unary() {
    if (equal("++")) {
        Node *n = calloc(1, sizeof(Node));
        n->kind = ND_PREINC;
        n->lhs = unary();
        return n;
    }
    if (equal("--")) {
        Node *n = calloc(1, sizeof(Node));
        n->kind = ND_PREDEC;
        n->lhs = unary();
        return n;
    }
    if (equal("&")) return new_node(ND_ADDR, unary(), NULL);
    if (equal("*")) return new_node(ND_DEREF, unary(), NULL);
    if (equal("+")) return unary();
    if (equal("-")) return new_node(ND_NEG, unary(), NULL);
    return postfix();
}

static Node *mul() {
    Node *node = unary();
    for (;;) {
        if (equal("*")) node = new_node(ND_MUL, node, unary());
        else if (equal("/")) node = new_node(ND_DIV, node, unary());
        else return node;
    }
}

static Node *add() {
    Node *node = mul();
    for (;;) {
        if (equal("+")) node = new_node(ND_ADD, node, mul());
        else if (equal("-")) node = new_node(ND_SUB, node, mul());
        else return node;
    }
}

static Node *relational() {
    Node *node = add();
    for (;;) {
        if (equal("<")) node = new_node(ND_LT, node, add());
        else if (equal("<=")) node = new_node(ND_LE, node, add());
        else if (equal(">")) node = new_node(ND_GT, node, add());
        else if (equal(">=")) node = new_node(ND_GE, node, add());
        else return node;
    }
}

static Node *equality() {
    Node *node = relational();
    for (;;) {
        if (equal("==")) node = new_node(ND_EQ, node, relational());
        else if (equal("!=")) node = new_node(ND_NE, node, relational());
        else return node;
    }
}

static Node *assign() {
    Node *node = equality();
    if (equal("=")) node = new_node(ND_ASSIGN, node, assign());
    return node;
}

static Node *expr() { return assign(); }

// Statements and functions
static Node *stmt() {
    Node *node;
    if (equal("if")) {
        expect("("); node = calloc(1, sizeof(Node)); node->kind = ND_IF;
        node->cond = expr(); expect(")"); node->then = stmt();
        if (equal("else")) node->els = stmt();
        return node;
    }
    if (equal("while")) {
        expect("("); node = calloc(1, sizeof(Node)); node->kind = ND_WHILE;
        node->cond = expr(); expect(")"); node->then = stmt();
        return node;
    }
    if (equal("for")) {
        expect("("); node = calloc(1, sizeof(Node)); node->kind = ND_FOR;
        if (!equal(";")) node->init = expr(); expect(";");
        if (!equal(";")) node->cond = expr(); expect(";");
        if (!equal(")")) node->inc = expr(); expect(")");
        node->then = stmt();
        return node;
    }
    if (equal("return")) {
        node = calloc(1, sizeof(Node)); node->kind = ND_RETURN;
        node->lhs = expr(); expect(";");
        return node;
    }
    if (equal("{")) {
        node = calloc(1, sizeof(Node)); node->kind = ND_BLOCK;
        Node head = {};
        Node *cur = &head;
        while (!equal("}")) { cur->next = stmt(); cur = cur->next; }
        node->body = head.next;
        return node;
    }
    node = expr(); expect(";");
    return node;
}

// Type parsing: basetype, declarator
static Type *basetype() {
    if (equal("int")) return int_type;
    if (equal("struct")) {
        if (token->kind == TK_IDENT) {
            char *sname = strndup(token->loc, token->len);
            token = token->next;
            if (equal("{")) {
                Member *head = NULL, **pp = &head;
                while (!equal("}")) {
                    Type *mbase = basetype();
                    char *mname = NULL;
                    Type *mty = declarator(mbase, &mname);
                    expect(";");
                    Member *m = calloc(1, sizeof(Member));
                    m->name = mname;
                    m->ty = mty;
                    *pp = m; pp = &m->next;
                }
                Type *st = calloc(1, sizeof(Type));
                st->kind = TY_STRUCT;
                st->members = head;
                st->name = sname;
                compute_struct_layout(st);
                return st;
            } else {
                Type *st = calloc(1, sizeof(Type));
                st->kind = TY_STRUCT;
                st->name = sname;
                // Size remains 0 for forward declaration
                return st;
            }
        } else {
            error_at(token->loc, "expected identifier after struct");
        }
    }
    if (equal("enum")) {
        Type *et = calloc(1, sizeof(Type));
        et->kind = TY_ENUM;
        et->name = strndup(token->loc, token->len);
        token = token->next;
        expect("{");
        Member *head = NULL, **pp = &head;
        int val = 0;
        while (!equal("}")) {
            if (token->kind != TK_IDENT) error_at(token->loc, "expected identifier in enum");
            char *mname = strndup(token->loc, token->len);
            token = token->next;
            expect(",");
            Member *m = calloc(1, sizeof(Member));
            m->name = mname;
            m->ty = et;
            m->offset = val++;
            *pp = m; pp = &m->next;
        }
        et->members = head;
        et->size = 4;  // Set size for enum
        et->align = 4;
        expect("}");
        return et;
    }
    error_at(token->loc, "unknown base type");
    return NULL;
}

// declarator: returns Type* and sets *out_name to identifier name (heap-allocated)
static Type *declarator(Type *base, char **out_name) {
    while (equal("*")) {
        Type *t = calloc(1, sizeof(Type));
        t->kind = TY_PTR;
        t->base = base;
        t->size = 8;
        t->align = 8;
        base = t;
    }

    if (token->kind != TK_IDENT)
        error_at(token->loc, "expected identifier in declarator");
    *out_name = strndup(token->loc, token->len);
    token = token->next;

    if (equal("[")) {
        int len = expect_number();
        expect("]");
        Type *t = calloc(1, sizeof(Type));
        t->kind = TY_ARRAY;
        t->base = base;
        t->size = base->size * len;
        t->align = base->align;
        return t;
    }

    if (equal("(")) {
        Type *t = calloc(1, sizeof(Type));
        t->kind = TY_FUNC;
        t->return_ty = base;
        Member *param_head = NULL, **pp = &param_head;
        if (!equal(")")) {
            do {
                Type *pt = basetype();
                char *pname = NULL;
                Type *param_ty = declarator(pt, &pname);
                Member *m = calloc(1, sizeof(Member));
                m->name = pname;
                m->ty = param_ty;
                *pp = m; pp = &m->next;
            } while (equal(","));
            expect(")");
        }
        t->members = param_head;
        return t;
    }

    return base;
}

// Struct layout helpers
static int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

static void compute_struct_layout(Type *st) {
    if (!st || st->kind != TY_STRUCT) return;
    int offset = 0;
    int struct_align = 1;
    for (Member *m = st->members; m; m = m->next) {
        if (!m->ty->size) {
            if (m->ty->kind == TY_STRUCT && m->ty->members) compute_struct_layout(m->ty);
            else error_at(current_input, "member of incomplete type");
        }
        int m_align = m->ty->align ? m->ty->align : 1;
        struct_align = struct_align > m_align ? struct_align : m_align;
        offset = align_to(offset, m_align);
        m->offset = offset;
        m->align = m_align;
        offset += m->ty->size;
    }
    st->align = struct_align;
    st->size = align_to(offset, struct_align);
}

static Member *find_member(Type *st, const char *name) {
    if (!st || st->kind != TY_STRUCT) return NULL;
    for (Member *m = st->members; m; m = m->next) {
        if (!strcmp(m->name, name)) return m;
    }
    return NULL;
}

// Function parsing
static Function *function() {
    locals = NULL;
    Function *fn = calloc(1, sizeof(Function));

    Type *base = basetype();
    char *fname = NULL;
    Type *fty = declarator(base, &fname);
    if (!fty || fty->kind != TY_FUNC) error_at(current_input, "expected function declaration");

    fn->name = fname;
    Var *last = NULL;
    for (Member *m = fty->members; m; m = m->next) {
        Var *v = calloc(1, sizeof(Var));
        v->ty = m->ty;
        v->name = m->name ? strdup(m->name) : NULL;
        if (!fn->params) fn->params = last = v; else last = last->next = v;
        v->next = locals;
        locals = v;
    }

    expect("{");
    fn->body = stmt();
    fn->locals = locals;

    // Compute stack offsets for locals
    int offset = 0;
    for (Var *v = fn->locals; v; v = v->next) {
        if (v->ty->size == 0) error_at(current_input, "variable of incomplete type");
        offset = align_to(offset, v->ty->align);
        offset += v->ty->size;
        v->offset = offset;
    }
    fn->stack_size = align_to(offset, 8);

    return fn;
}

static Function *parse() {
    Function head = {};
    Function *cur = &head;
    while (token->kind != TK_EOF) {
        cur = cur->next = function();
        while (token->kind == TK_RESERVED && token->len == 1 && *token->loc == '}') token = token->next;
    }
    return head.next;
}

// Code generation helpers
static uint8_t *code;
static int code_pos;

static void emit8(uint8_t b) { code[code_pos++] = b; }
static void emit32(uint32_t v) { *(uint32_t *)(code + code_pos) = v; code_pos += 4; }
static void emit64(uint64_t v) { *(uint64_t *)(code + code_pos) = v; code_pos += 8; }

static void emit_rex(int w, int r, int x, int b) {
    uint8_t rex = 0x40;
    if (w) rex |= 0x08;
    if (r) rex |= 0x04;
    if (x) rex |= 0x02;
    if (b) rex |= 0x01;
    emit8(rex);
}
static void emit_modrm(uint8_t mod, uint8_t reg, uint8_t rm) {
    emit8((mod << 6) | ((reg & 7) << 3) | (rm & 7));
}
static void emit_sib(uint8_t scale, uint8_t index, uint8_t base) {
    emit8((scale << 6) | ((index & 7) << 3) | (base & 7));
}

static void emit_lea_rbp_disp(int reg_num, int32_t disp) {
    int rex_r = (reg_num >> 3) & 1;
    emit_rex(1, rex_r, 0, 0);
    emit8(0x8D);
    emit_modrm(0x2, reg_num & 7, 5);
    emit32((uint32_t)disp);
}
static void emit_mov_reg_from_rbp_disp(int reg_num, int32_t disp) {
    int rex_r = (reg_num >> 3) & 1;
    emit_rex(1, rex_r, 0, 0);
    emit8(0x8B);
    emit_modrm(0x2, reg_num & 7, 5);
    emit32((uint32_t)disp);
}
static void emit_mov_rbp_disp_from_reg(int reg_num, int32_t disp) {
    int rex_r = (reg_num >> 3) & 1;
    emit_rex(1, rex_r, 0, 0);
    emit8(0x89);
    emit_modrm(0x2, reg_num & 7, 5);
    emit32((uint32_t)disp);
}

static void adjust_rsp(Function *fn, int delta) { fn->rsp_offset += delta; }
static void emit_add_rsp(int32_t imm) {
    if (imm == 0) return;
    if (imm > 0 && imm <= 127) {
        emit8(0x48); emit8(0x83); emit8(0xC4); emit8((uint8_t)imm);
    } else if (imm < 0 && imm >= -128) {
        emit8(0x48); emit8(0x83); emit8(0xEC); emit8((uint8_t)(-imm));
    } else {
        if (imm > 0) {
            emit8(0x48); emit8(0x81); emit8(0xC4); emit32((uint32_t)imm);
        } else {
            emit8(0x48); emit8(0x81); emit8(0xEC); emit32((uint32_t)(-imm));
        }
    }
}

static void emit_prolog(int stack_size) {
    emit8(0x55);
    emit8(0x48); emit8(0x89); emit8(0xE5);
    if (stack_size) {
        emit8(0x48); emit8(0x81); emit8(0xEC);
        emit32(stack_size);
    }
}
static void emit_epilog() {
    emit8(0x48); emit8(0x89); emit8(0xEC);
    emit8(0x5D);
    emit8(0xC3);
}

static void emit_call_placeholder(char *fname) {
    emit8(0xE8);
    int pos = code_pos;
    emit32(0);
    CallPatch *cp = calloc(1, sizeof(CallPatch));
    cp->pos = pos;
    cp->name = strdup(fname);
    cp->next = call_patches;
    call_patches = cp;
}
static void patch_calls(Function *prog) {
    for (CallPatch *cp = call_patches; cp; cp = cp->next) {
        Function *target = NULL;
        for (Function *f = prog; f; f = f->next) {
            if (!strcmp(f->name, cp->name)) { target = f; break; }
        }
        if (!target) {
            fprintf(stderr, "undefined function: %s\n", cp->name);
            exit(1);
        }
        int call_site = cp->pos - 1;
        int32_t rel = (int32_t)(target->code_offset - (call_site + 5));
        *(int32_t *)(code + cp->pos) = rel;
    }
}

#define MAX_ARGS 64
static int collect_args(Node *args, Node *arr[]) {
    int n = 0;
    for (Node *a = args; a; a = a->next) {
        if (n >= MAX_ARGS) error_at(current_input, "too many function arguments");
        arr[n++] = a;
    }
    return n;
}

// gen_addr: compute address for lvalues
static void gen(Node *node); // forward

static void gen_addr(Node *node) {
    if (node->kind == ND_LVAR) {
        int32_t disp = -node->var->offset;
        emit_lea_rbp_disp(0, disp); // lea rax, [rbp + disp]
        emit8(0x50); // push rax
        return;
    }
    if (node->kind == ND_DEREF) {
        gen(node->lhs); // gen(lhs) should push pointer value
        return;
    }
    if (node->kind == ND_MEMBER) {
        gen_addr(node->lhs); // pushes base address
        emit8(0x58); // pop rax
        if (!node->lhs->ty) error_at(current_input, "lhs has no type for member access");
        if (!node->member) {
            Type *basety = node->lhs->ty;
            Member *m = find_member(basety, node->member_name);
            if (!m) error_at(current_input, "no such member %s", node->member_name);
            node->member = m;
            node->ty = m->ty;  // Set type
        }
        int off = node->member->offset;
        emit8(0x48); emit8(0x81); emit8(0xC0); emit32((uint32_t)off); // add rax, off
        emit8(0x50); // push rax
        return;
    }
    error_at(current_input, "not an lvalue");
}

// Code generation
static void gen(Node *node) {
    switch (node->kind) {
        case ND_NUM:
            emit8(0x48); emit8(0xC7); emit8(0xC0); emit32((uint32_t)node->val); // mov rax, imm32
            emit8(0x50); // push rax
            break;

        case ND_LVAR:
            gen_addr(node);
            emit8(0x58); // pop rax
            emit_rex(1, 0, 0, 0);
            emit8(0x8B);
            emit_modrm(0x0, 0, 0); // mov rax, [rax]
            emit8(0x50); // push rax
            break;

        case ND_ASSIGN:
            gen_addr(node->lhs);
            gen(node->rhs);
            emit8(0x58); // pop rax (rhs)
            emit8(0x5F); // pop rdi (addr)
            emit_rex(1, 0, 0, 0);
            emit8(0x89);
            emit_modrm(0x0, 0, 7); // mov [rdi], rax
            emit8(0x50); // push rax
            break;

        case ND_RETURN:
            gen(node->lhs);
            emit8(0x58); // pop rax
            emit8(0xC3); // ret
            break;

        case ND_IF: {
            int else_label = code_pos + 20;
            int end_label = code_pos + 30;
            gen(node->cond);
            emit8(0x58); // pop rax
            emit8(0x48); emit8(0x85); emit8(0xC0); // test rax, rax
            emit8(0x0F); emit8(0x84); emit32(else_label - (code_pos + 4)); // je else
            gen(node->then);
            emit8(0xE9); emit32(end_label - (code_pos + 4)); // jmp end
            if (node->els) {
                *(uint32_t *)(code + else_label - 4) = code_pos - else_label;
                gen(node->els);
            }
            *(uint32_t *)(code + end_label - 4) = code_pos - end_label;
            break;
        }

        case ND_WHILE: {
            int begin = code_pos;
            int end_label = code_pos + 20;
            gen(node->cond);
            emit8(0x58); // pop rax
            emit8(0x48); emit8(0x85); emit8(0xC0);
            emit8(0x0F); emit8(0x84); emit32(end_label - (code_pos + 4));
            gen(node->then);
            emit8(0xE9); emit32(begin - (code_pos + 4));
            *(uint32_t *)(code + end_label - 4) = code_pos - end_label;
            break;
        }

        case ND_BLOCK:
            for (Node *n = node->body; n; n = n->next) gen(n);
            break;

        case ND_ADD:
            gen(node->lhs);
            gen(node->rhs);
            emit8(0x58); // pop rax
            emit8(0x5A); // pop rdx
            emit8(0x48); emit8(0x01); emit8(0xD0); // add rax, rdx
            emit8(0x50); // push rax
            break;

        case ND_SUB:
            gen(node->lhs);
            gen(node->rhs);
            emit8(0x58);
            emit8(0x5A);
            emit8(0x48); emit8(0x29); emit8(0xD0); // sub rax, rdx
            emit8(0x50);
            break;

        case ND_MUL:
            gen(node->lhs);
            gen(node->rhs);
            emit8(0x58);
            emit8(0x5A);
            emit8(0x48); emit8(0x0F); emit8(0xAF); emit8(0xC2); // imul rax, rdx
            emit8(0x50);
            break;

        case ND_DIV:
            gen(node->lhs);
            gen(node->rhs);
            emit8(0x58);
            emit8(0x5A);
            emit8(0x48); emit8(0x99); // cqo
            emit8(0x48); emit8(0xF7); emit8(0xF2); // idiv rdx
            emit8(0x50);
            break;

        case ND_FUNCALL: {
            Node *arg_arr[MAX_ARGS];
            int nargs = collect_args(node->args, arg_arr);
            for (int i = nargs - 1; i >= 0; i--) gen(arg_arr[i]);
          
            emit8(0x48); emit8(0x83); emit8(0xEC); emit8((uint8_t)(nargs * 8)); // sub rsp, nargs*8
            emit_call_placeholder(node->name);
            emit8(0x48); emit8(0x83); emit8(0xC4); emit8((uint8_t)(nargs * 8)); // add rsp, nargs*8
            emit8(0x50); // push rax
            break;
        }

        default:
            error_at(current_input, "unsupported node kind: %d", node->kind);
    }
}

// Main function
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Read input file
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    // Read input file into string
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    current_input = malloc(size + 1);
    fread(current_input, 1, size, fp);
    current_input[size] = '\0';
    fclose(fp);

    // Tokenize input
    token = tokenize();

    // Parse input
    Function *prog = parse();

    // Generate code
    code = malloc(1024 * 1024); // 1MB
    code_pos = 0;
    for (Function *fn = prog; fn; fn = fn->next) {
        fn->code_offset = code_pos;
        emit_prolog(fn->stack_size);
        gen(fn->body);
        emit_epilog();
    }

    // Patch calls
    patch_calls(prog);

    // Create ELF header and program header
    Elf64_Ehdr ehdr = {0};
    ehdr.e_ident[0] = 0x7f; ehdr.e_ident[1] = 'E'; ehdr.e_ident[2] = 'L'; ehdr.e_ident[3] = 'F';
    ehdr.e_ident[4] = ELFCLASS64;
    ehdr.e_ident[5] = ELFDATA2LSB;
    ehdr.e_ident[6] = EV_CURRENT;
    ehdr.e_type = ET_EXEC;
    ehdr.e_machine = EM_X86_64;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_phoff = sizeof(Elf64_Ehdr);
    ehdr.e_shoff = 0;
    ehdr.e_flags = 0;
    ehdr.e_ehsize = sizeof(Elf64_Ehdr);
    ehdr.e_phentsize = sizeof(Elf64_Phdr);
    ehdr.e_phnum = 1;
    ehdr.e_shentsize = 0;
    ehdr.e_shnum = 0;
    ehdr.e_shstrndx = SHN_UNDEF;

    Elf64_Phdr phdr = {0};
    phdr.p_type = PT_LOAD;
    phdr.p_flags = PF_R | PF_W | PF_X;
    phdr.p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
    phdr.p_vaddr = 0x400000 + phdr.p_offset;
    phdr.p_paddr = phdr.p_vaddr;
    phdr.p_filesz = code_pos;
    phdr.p_memsz = code_pos;
    phdr.p_align = 0x1000;

    ehdr.e_entry = phdr.p_vaddr; // Entry point to start of code

    // Write output to file
    fp = fopen("output.elf", "wb");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    fwrite(&ehdr, sizeof(ehdr), 1, fp);
    fwrite(&phdr, sizeof(phdr), 1, fp);
    fwrite(code, code_pos, 1, fp);
    fclose(fp);

    return 0;
}

