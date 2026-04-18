// Hinweis: Der bereitgestellte Code ist sehr umfangreich und enthält viele komplexe Komponenten.
// Ich werde die wichtigsten Fehler identifizieren, korrigieren und einige Verbesserungen vorschlagen.

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

// ELF-Strukturen und Konstanten
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

// Typen, AST, Symbole
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
    Member *members; // Für Struct-Mitglieder oder Funktionsparameter
    char *name;
    int enum_val; // Für Enum-Typen
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
        ND_EXPR_STMT,
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
    /* ND_MEMBER spezifisch */
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

// Globale Variablen
static Var *locals;
static Var *globals;
static Function *functions;
static Type *void_type = &(Type){TY_VOID, 1, 1};
static Type *bool_type = &(Type){TY_BOOL, 1, 1};
static Type *char_type = &(Type){TY_CHAR, 1, 1};
static Type *int_type = &(Type){TY_INT, 4, 4};
static Type *long_type = &(Type){TY_LONG, 8, 8};
static Function *current_fn = NULL;

// Patch-Liste für Funktionsaufrufe
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

static Token *token = NULL;
static char *current_input = NULL;

/* Fehlerfunktion: Zeichnung und Zeilennummer */
static void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
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

    char *line_end = line_start;
    while (*line_end && *line_end != '\n') line_end++;

    fprintf(stderr, "Fehler bei Zeile %d:\n", line_num);
    fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);
    fprintf(stderr, "%*s^\n", col - 1, "");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

/* Vergleich von Token mit Operator */
static bool equal(char *op) {
    if (strlen(op) != token->len || strncmp(token->loc, op, token->len))
        return false;
    token = token->next;
    return true;
}

/* Erwartung eines Symbols / Tokens */
static void expect(char *op) {
    if (!equal(op))
        error_at(token->loc, "Erwartet \"%s\"", op);
}

/* Erwartung einer Zahl */
static int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->loc, "Erwartete Zahl");
    int val = token->val;
    token = token->next;
    return val;
}

/* Neue Token-Erstellung */
static Token *new_token(TokenKind kind, char *start, char *end) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = end - start;
    return tok;
}

/* Identifikator-Prüfung */
static bool is_ident1(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}
static bool is_ident2(char c) {
    return is_ident1(c) || ('0' <= c && c <= '9');
}

/* Tokenizer-Funktion */
static Token *tokenize() {
    char *p = current_input;
    Token head = {0};
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

        // Sonst: Einzelne Symbole
        char *q = p++;
        cur = cur->next = new_token(TK_RESERVED, q, p);
    }
    #undef STARTS_WITH
    cur = cur->next = new_token(TK_EOF, p, p);
    return head.next;
}

/* Variable finden nach Name */
static Var *find_var_by_name(char *name, int len) {
    for (Var *v = locals; v; v = v->next)
        if ((int)strlen(v->name) == len && !strncmp(v->name, name, len))
            return v;
    for (Var *v = globals; v; v = v->next)
        if ((int)strlen(v->name) == len && !strncmp(v->name, name, len))
            return v;
    return NULL;
}

/* Neue Node-Erstellung */
static Node *new_node(int kind, Node *lhs, Node *rhs) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = kind;
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

/* Neue Nummer-Node */
static Node *new_num(int val) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = ND_NUM;
    n->val = val;
    return n;
}

/* Variable-Node */
static Node *new_var_node(Var *var) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = ND_LVAR;
    n->var = var;
    n->ty = var->ty;
    return n;
}

/* Weiterleitungen für Parserfunktionen */
static Node *expr();
static Node *stmt();
static Type *basetype();
static Type *declarator(Type *base, char **out_name);
static void compute_struct_layout(Type *st);
static Member *find_member(Type *st, const char *name);

/* Primary / Postfix / Unary Funktionen */
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
            Node *head = NULL, **cur = &head;
            if (!equal(")")) {
                *cur = expr(); 
                cur = &((*cur)->next);
                while (equal(",")) { 
                    *cur = expr(); 
                    cur = &((*cur)->next); 
                }
                expect(")");
            }
            Node *call = calloc(1, sizeof(Node));
            call->kind = ND_FUNCALL;
            call->name = name;
            call->args = head;
            return call;
        }
        Var *v = find_var_by_name(name_start, name_len);
        if (!v) error_at(tok->loc, "Undefinierte Variable: %.*s", name_len, name_start);
        return new_var_node(v);
    }
    error_at(tok->loc, "Erwarteter Ausdruck");
}

/* Postfix */
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
            if (token->kind != TK_IDENT) error_at(token->loc, "Erwartet Member-Name nach .");
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
            if (token->kind != TK_IDENT) error_at(token->loc, "Erwartet Member-Name nach ->");
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

/* Unary */
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

/* Multiplikation */
static Node *mul() {
    Node *node = unary();
    for (;;) {
        if (equal("*")) node = new_node(ND_MUL, node, unary());
        else if (equal("/")) node = new_node(ND_DIV, node, unary());
        else break;
    }
    return node;
}

/* Addition / Subtraktion */
static Node *add() {
    Node *node = mul();
    for (;;) {
        if (equal("+")) node = new_node(ND_ADD, node, mul());
        else if (equal("-")) node = new_node(ND_SUB, node, mul());
        else break;
    }
    return node;
}

/* Relationale Operatoren */
static Node *relational() {
    Node *node = add();
    for (;;) {
        if (equal("<")) node = new_node(ND_LT, node, add());
        else if (equal("<=")) node = new_node(ND_LE, node, add());
        else if (equal(">")) node = new_node(ND_GT, node, add());
        else if (equal(">=")) node = new_node(ND_GE, node, add());
        else break;
    }
    return node;
}

/* Gleichheit / Ungleichheit */
static Node *equality() {
    Node *node = relational();
    for (;;) {
        if (equal("==")) node = new_node(ND_EQ, node, relational());
        else if (equal("!=")) node = new_node(ND_NE, node, relational());
        else break;
    }
    return node;
}

/* Zuweisung */
static Node *assign() {
    Node *node = equality();
    if (equal("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

/* Ausdruck */
static Node *expr() { return assign(); }

/* Statements & Funktionen */
static Node *stmt() {
    Node *node;
    if (equal("if")) {
        expect("("); 
        node = calloc(1, sizeof(Node)); 
        node->kind = ND_IF;
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (equal("else")) node->els = stmt();
        return node;
    }
    if (equal("while")) {
        expect("("); 
        node = calloc(1, sizeof(Node)); 
        node->kind = ND_WHILE;
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }
    if (equal("for")) {
        expect("("); 
        node = calloc(1, sizeof(Node)); 
        node->kind = ND_FOR;
        if (!equal(";")) {
            node->init = expr();
        }
        expect(";");
        if (!equal(";")) {
            node->cond = expr();
        }
        expect(";");
        if (!equal(")")) {
            node->inc = expr();
        }
        expect(")");
        node->then = stmt();
        return node;
    }
    if (equal("return")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
        return node;
    }
    if (equal("{")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        Node head = {0};
        Node *cur = &head;
        while (!equal("}")) {
            cur->next = stmt(); 
            cur = cur->next;
        }
        node->body = head.next;
        return node;
    }
    // Deklaration prüfen
    if (token->kind == TK_IDENT) {
        char *s = strndup(token->loc, token->len);
        bool is_decl = (!strcmp(s, "int") || !strcmp(s, "void") || !strcmp(s, "struct") || !strcmp(s, "enum"));
        free(s);
        if (is_decl) {
            Type *base = basetype();
            char *name = NULL;
            Type *ty = declarator(base, &name);
            expect(";");
            Var *v = calloc(1, sizeof(Var));
            v->name = name;
            v->ty = ty;
            v->next = locals;
            locals = v;
            // Kein echter Node, nur Platzhalter
            node = calloc(1, sizeof(Node));
            node->kind = ND_BLOCK;
            node->body = NULL;
            return node;
        }
    }
    // Ausdrucksstatement
    node = calloc(1, sizeof(Node));
    node->kind = ND_EXPR_STMT;
    node->lhs = expr();
    expect(";");
    return node;
}

/* Typen: basistypen & Deklarator */
static Type *basetype() {
    if (equal("void")) return void_type;
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
                return st;
            }
        } else {
            error_at(token->loc, "Erwartet Bezeichner nach struct");
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
            if (token->kind != TK_IDENT) error_at(token->loc, "Erwartet Bezeichner in enum");
            char *mname = strndup(token->loc, token->len);
            token = token->next;
            if (equal("="))
                val = expect_number(), expect(",");
            Member *m = calloc(1, sizeof(Member));
            m->name = mname;
            m->ty = et;
            m->offset = val++;
            *pp = m; pp = &m->next;
        }
        et->members = head;
        et->size = 4; 
        et->align = 4;
        expect("}");
        return et;
    }
    error_at(token->loc, "Unbekannter Basistyp");
    return NULL;
}

/* Deklarator: gibt Type* zurück und setzt *out_name (auf Heap) */
static Type *declarator(Type *base, char **out_name) {
    while (equal("*")) {
        Type *t = calloc(1, sizeof(Type));
        t->kind = TY_PTR;
        t->base = base;
        t->size = 8;
        t->align = 8;
        base = t;
    }
    if (token->kind != TK_IDENT) error_at(token->loc, "Erwartet Bezeichner in Deklarator");
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
        Type *func_type = calloc(1, sizeof(Type));
        func_type->kind = TY_FUNC;
        func_type->return_ty = base;
        func_type->members = param_head;
        return func_type;
    }
    return base;
}

/* Helper: Ausrichtung auf */
static int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

/* Struct-Layout */
static void compute_struct_layout(Type *st) {
    if (!st || st->kind != TY_STRUCT) return;
    int offset = 0;
    int struct_align = 1;
    for (Member *m = st->members; m; m = m->next) {
        if (!m->ty->size) {
            if (m->ty->kind == TY_STRUCT && m->ty->members) 
                compute_struct_layout(m->ty);
            else error_at(current_input, "Mitglied unvollständiger Typ");
        }
        int m_align = m->ty->align ? m->ty->align : 1;
        if (m_align > struct_align) struct_align = m_align;
        offset = align_to(offset, m_align);
        m->offset = offset;
        m->align = m_align;
        offset += m->ty->size;
    }
    st->align = struct_align;
    st->size = align_to(offset, struct_align);
}

/* Mitglied finden */
static Member *find_member(Type *st, const char *name) {
    if (!st || st->kind != TY_STRUCT) return NULL;
    for (Member *m = st->members; m; m = m->next) {
        if (!strcmp(m->name, name))
            return m;
    }
    return NULL;
}

/* Funktion-Parser */
static Function *function() {
    locals = NULL;

    Function *fn = calloc(1, sizeof(Function));

    Type *base = basetype();

    char *fname = NULL;
    Type *fty = declarator(base, &fname);
    if (!fty || fty->kind != TY_FUNC)
        error_at(current_input, "Erwartete Funktionsdeklaration");

    fn->name = fname;

    // Parameter in locals funktionieren
    Var *last = NULL;
    for (Member *m = fty->members; m; m = m->next) {
        Var *v = calloc(1, sizeof(Var));
        v->ty = m->ty;
        v->name = strdup(m->name);
        v->next = NULL;
        if (!fn->params)
            fn->params = v;
        else
            last->next = v;
        last = v;
        v->next = locals;
        locals = v;
    }

    expect("{");
    fn->body = stmt();
    fn->locals = locals;

    // Stack-Offset für lokale Variablen berechnen
    int offset = 0;
    for (Var *v = fn->locals; v; v = v->next) {
        if (v->ty->size == 0)
            error_at(current_input, "Variable unvollständiger Typ");
        offset = align_to(offset, v->ty->align);
        offset += v->ty->size;
        v->offset = offset;
    }
    fn->stack_size = align_to(offset, 8);

    return fn;
}

/* Program-Parser */
static Function *parse() {
    Function head = {0};
    Function *cur = &head;
    while (token->kind != TK_EOF) {
        cur = cur->next = function();
        while (token->kind == TK_RESERVED && token->len == 1 && *token->loc == '}')
            token = token->next;
    }
    return head.next;
}

/* Code-Generator-Helper */
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

/* Prolog & Epilog Ausgabe */
static void emit_lea_rbp_disp(int reg_num, int32_t disp) {
    int rex_r = (reg_num >> 3) & 1;
    emit_rex(1, rex_r, 0, 0);
    emit8(0x8D); // lea r, [rbp + disp]
    emit_modrm(0x2, reg_num & 7, 5);
    emit32((uint32_t)disp);
}
static void emit_mov_reg_from_rbp_disp(int reg_num, int32_t disp) {
    int rex_r = (reg_num >> 3) & 1;
    emit_rex(1, rex_r, 0, 0);
    emit8(0x8B); // mov r, [rbp + disp]
    emit_modrm(0x2, reg_num & 7, 5);
    emit32((uint32_t)disp);
}
static void emit_mov_rbp_disp_from_reg(int reg_num, int32_t disp) {
    int rex_r = (reg_num >> 3) & 1;
    emit_rex(1, rex_r, 0, 0);
    emit8(0x89); // mov [rbp + disp], r
    emit_modrm(0x2, reg_num & 7, 5);
    emit32((uint32_t)disp);
}

/* Stack-Anpassung bei Funktionsein- und -austritt */
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

/* Funktion-Einleitung */
static void emit_prolog(int stack_size) {
    emit8(0x55); // push rbp
    emit8(0x48); emit8(0x89); emit8(0xE5); // mov rbp, rsp
    if (stack_size) {
        emit8(0x48); emit8(0x81); emit8(0xEC);
        emit32(stack_size);
    }
}

/* Funktion-Ausleitung */
static void emit_epilog() {
    emit8(0x48); emit8(0x89); emit8(0xEC); // mov rsp, rbp
    emit8(0x5D); // pop rbp
    emit8(0xC3); // ret
}

/* Aufruf-Placeholder für Patch */
static void emit_call_placeholder(char *fname) {
    emit8(0xE8); // call relative
    int pos = code_pos;
    emit32(0);   // Platzhalter
    CallPatch *cp = calloc(1, sizeof(CallPatch));
    cp->pos = pos;
    cp->name = strdup(fname);
    cp->next = call_patches;
    call_patches = cp;
}

/* Patchen aller Funktionsaufrufe */
static void patch_calls(Function *prog) {
    for (CallPatch *cp = call_patches; cp; cp = cp->next) {
        Function *target = NULL;
        for (Function *f = prog; f; f = f->next) {
            if (!strcmp(f->name, cp->name)) {
                target = f;
                break;
            }
        }
        if (!target) {
            fprintf(stderr, "Unbekannte Funktion: %s\n", cp->name);
            exit(1);
        }
        int call_site = cp->pos;
        int32_t rel = (int32_t)(target->code_offset - (call_site + 4));
        *(int32_t *)(code + call_site) = rel;
    }
}

/* Parameter sammeln */
#define MAX_ARGS 64
static int collect_args(Node *args, Node *arr[]) {
    int n = 0;
    for (Node *a = args; a; a = a->next) {
        if (n >= MAX_ARGS) error_at(current_input, "Zu viele Funktionsargumente");
        arr[n++] = a;
    }
    return n;
}

/* Adresse generieren (lvalue) */
static void gen_addr(Node *node);

/* codegen für Node (Codeausgabe) */
static void gen(Node *node) {
    switch (node->kind) {
        case ND_NUM:
            emit8(0x48); emit8(0xC7); emit8(0xC0); emit32((uint32_t)node->val); // mov rax, imm32
            break;
        case ND_LVAR:
            gen_addr(node);
            emit8(0x58); // pop rax
            emit_rex(1,0,0,0);
            emit8(0x8B); // mov rax, [rax]
            emit_modrm(0x0,0,0);
            break;
        case ND_ASSIGN:
            gen_addr(node->lhs);
            gen(node->rhs);
            emit8(0x5F); // pop rdi (Adresse)
            emit_rex(1,0,0,0);
            emit8(0x89);
            emit_modrm(0x0,0,7); // mov [rdi], rax
            break;
        case ND_RETURN:
            gen(node->lhs);
            emit8(0xC3); // ret
            break;
        case ND_IF:
        {
            gen(node->cond);
            emit8(0x48); emit8(0x85); emit8(0xC0); // test rax, rax
            emit8(0x0F); emit8(0x84); 
            int else_pos = code_pos; 
            emit32(0);
            gen(node->then);
            int end_pos = code_pos; 
            emit8(0xE9); 
            emit32(0);
            // Patch für else
            *(uint32_t *)(code + else_pos) = code_pos - (else_pos + 4);
            if (node->els) {
                gen(node->els);
            }
            // Patch für Ende
            *(uint32_t *)(code + end_pos) = code_pos - (end_pos + 4);
            break;
        }
        case ND_WHILE:
        {
            int start = code_pos;
            gen(node->cond);
            emit8(0x48); emit8(0x85); emit8(0xC0);
            emit8(0x0F); emit8(0x84);
            int end = code_pos;
            emit32(0);
            gen(node->then);
            emit8(0xE9);
            emit32(start - (code_pos + 4));
            *(uint32_t *)(code + end) = code_pos - (end + 4);
            break;
        }
        case ND_FOR:
        {
            if (node->init) gen(node->init);
            int start = code_pos;
            if (node->cond) {
                gen(node->cond);
                emit8(0x48); emit8(0x85); emit8(0xC0);
                emit8(0x0F); emit8(0x84);
                int end = code_pos;
                emit32(0);
                gen(node->then);
                if (node->inc)
                    gen(node->inc);
                emit8(0xE9);
                emit32(start - (code_pos + 4));
                *(uint32_t *)(code + end) = code_pos - (end + 4);
            } else {
                gen(node->then);
                if (node->inc)
                    gen(node->inc);
                emit8(0xE9);
                emit32(start - (code_pos + 4));
            }
            break;
        }
        case ND_BLOCK:
            for (Node *n = node->body; n; n = n->next)
                gen(n);
            break;
        case ND_EXPR_STMT:
            gen(node->lhs);
            emit8(0x50); // push rax
            emit8(0x58); // pop rax (Wert verwerfen)
            break;
        case ND_ADD:
        {
            gen(node->lhs);
            emit8(0x50); // push rax
            gen(node->rhs);
            emit8(0x5A); // pop rdx
            emit8(0x48); emit8(0x01); emit8(0xD0); // add rax, rdx
            break;
        }
        case ND_SUB:
        {
            gen(node->lhs);
            emit8(0x50);
            gen(node->rhs);
            emit8(0x5A);
            emit8(0x48); emit8(0x29); emit8(0xC2); // sub rdx, rax
            emit8(0x48); emit8(0x89); emit8(0xD0); // mov rax, rdx
            break;
        }
        case ND_MUL:
        {
            gen(node->lhs);
            emit8(0x50);
            gen(node->rhs);
            emit8(0x5A);
            emit8(0x48); emit8(0x0F); emit8(0xAF); emit8(0xC2); // imul rax, rdx
            break;
        }
        case ND_DIV:
        {
            gen(node->lhs);
            emit8(0x50);
            gen(node->rhs);
            emit8(0x5A);
            emit8(0x48); emit8(0x99); // cqo
            emit8(0x48); emit8(0xF7); emit8(0xF2); // idiv rdx
            break;
        }
        case ND_NEG:
            gen(node->lhs);
            emit8(0x48); emit8(0xF7); emit8(0xD8); // neg rax
            break;
        case ND_ADDR:
            gen_addr(node->lhs);
            break;
        case ND_DEREF:
            gen(node->lhs);
            emit_rex(1,0,0,0);
            emit8(0x8B); // mov rax, [rax]
            emit_modrm(0x0,0,0);
            break;
        case ND_MEMBER:
            gen_addr(node);
            emit8(0x58); // pop rax
            emit_rex(1,0,0,0);
            emit8(0x8B);
            emit_modrm(0x0,0,0);
            break;
        case ND_PREINC:
        {
            gen_addr(node->lhs);
            emit8(0xFF);
            emit_modrm(0x0,0,0); // inc [rax]
            emit_rex(1,0,0,0);
            emit8(0x8B);
            emit_modrm(0x0,0,0); // mov rax, [rax]
            break;
        }
        case ND_PREDEC:
        {
            gen_addr(node->lhs);
            emit8(0xFF);
            emit_modrm(0x1,0,0); // dec [rax]
            emit_rex(1,0,0,0);
            emit8(0x8B);
            emit_modrm(0x0,0,0);
            break;
        }
        case ND_POSTINC:
        {
            gen_addr(node->lhs);
            emit8(0x50); // push addr
            gen(node->lhs);
            emit8(0x50); // push old
            emit8(0x58); // pop rax
            emit8(0xFF);
            emit_modrm(0x0,0,0); // inc [rax]
            break;
        }
        case ND_POSTDEC:
        {
            gen_addr(node->lhs);
            emit8(0x50);
            emit8(0x58); // pop rax
            emit8(0xFF);
            emit_modrm(0x0,0,0); // dec [rax]
            break;
        }
        case ND_FUNCALL:
        {
            if (!strcmp(node->name, "exit")) {
                // Inline-Zeile für exit
                Node *arg = node->args;
                if (!arg || arg->next) error_at(current_input, "exit erwartet ein Argument");
                gen(arg);
                emit8(0x48); emit8(0x89); emit8(0xC7); // mov rdi, rax
                emit8(0x48); emit8(0xC7); emit8(0xC0); emit32(60); // mov rax, 60
                emit8(0x0F); emit8(0x05); // syscall
                break;
            }
            Node *args[MAX_ARGS];
            int nargs = collect_args(node->args, args);
            for (int i = nargs - 1; i >= 0; i--) {
                gen(args[i]);
                emit8(0x50); // push rax
            }
            emit8(0x48); emit8(0x83); emit8(0xEC); emit8((uint8_t)(nargs * 8)); // sub rsp, nargs*8
            emit_call_placeholder(node->name);
            emit8(0x48); emit8(0x83); emit8(0xC4); emit8((uint8_t)(nargs * 8)); // add rsp, nargs*8
            break;
        }
        default:
            error_at(current_input, "Unbekannter Node-Typ: %d", node->kind);
    }
}

/* Main Funktion */
int main(int argc, char **argv) {
    char *output_file = "a.out";
    int opt;
    while ((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
            case 'o':
                output_file = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-o output] inputfiles...\n", argv[0]);
                return 1;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Keine Eingabedateien angegeben.\n");
        return 1;
    }

    // Eingabedateien zusammenfügen
    char *all_input = NULL;
    size_t total_size = 0;
    for (int i = optind; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (!fp) {
            perror("fopen");
            return 1;
        }
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        rewind(fp);
        all_input = realloc(all_input, total_size + size + 1);
        fread(all_input + total_size, 1, size, fp);
        total_size += size;
        all_input[total_size++] = '\n'; // Separator
        fclose(fp);
    }
    all_input[total_size] = '\0';
    current_input = all_input;

    // Tokenisierung
    token = tokenize();

    // Parsing
    Function *prog = parse();

    // Code generieren
    code = malloc(1024 * 1024); // 1MB
    code_pos = 0;
    for (Function *fn = prog; fn; fn = fn->next) {
        fn->code_offset = code_pos;
        emit_prolog(fn->stack_size);
        gen(fn->body);
        emit_epilog();
    }

    // Patchen
    patch_calls(prog);

    // ELF Header & Program Header erstellen
    Elf64_Ehdr ehdr = {0};
    ehdr.e_ident[0] = 0x7f; ehdr.e_ident[1] = 'E'; ehdr.e_ident[2] = 'L'; ehdr.e_ident[3] = 'F';
    ehdr.e_ident[4] = ELFCLASS64;
    ehdr.e_ident[5] = ELFDATA2LSB;
    ehdr.e_ident[6] = EV_CURRENT;
    ehdr.e_type = ET_EXEC;
    ehdr.e_machine = EM_X86_64;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_entry = 0x400000; // Einstiegspunkt

    Elf64_Phdr phdr = {0};
    phdr.p_type = PT_LOAD;
    phdr.p_flags = PF_R | PF_W | PF_X; // Lesen, Schreiben, Ausführen
    phdr.p_offset = sizeof(ehdr) + sizeof(phdr);
    phdr.p_vaddr = 0x400000;
    phdr.p_paddr = 0;
    phdr.p_filesz = code_pos;
    phdr.p_memsz = code_pos;
    phdr.p_align = 0x1000;

    // Datei schreiben
    FILE *f = fopen(output_file, "wb");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fwrite(&ehdr, sizeof(ehdr), 1, f);
    fwrite(&phdr, sizeof(phdr), 1, f);
    fwrite(code, 1, code_pos, f);
    fclose(f);

    free(all_input);
    free(code);
    return 0;
}

