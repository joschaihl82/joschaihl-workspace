// cc.c - single-file small C compiler (amalgamated)
// Supports: long, short, unsigned variants, basic preprocessing, parsing, and codegen for x86-64
// Note: This is a compact educational compiler inspired by "9cc" and extended per request.

#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Basic helpers and declarations ---

void *calloc_or_die(size_t n, size_t s) {
    void *p = calloc(n, s);
    if (!p) {
        fprintf(stderr, "calloc failed\n");
        exit(1);
    }
    return p;
}

#undef calloc
#define calloc(n, s) calloc_or_die((n), (s))

#undef assert
#define assert(x) \
    do { if (!(x)) { fprintf(stderr, "assertion failed: %s:%d\n", __FILE__, __LINE__); exit(1); } } while (0)

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void error_at_here(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

// --- String wrapper ---

typedef struct String String;
struct String {
    char *ptr;
    int len;
};

String *new_string(char *ptr, int len) {
    String *s = calloc(1, sizeof(String));
    s->ptr = ptr;
    s->len = len;
    return s;
}

bool str_equals(String *s1, String *s2) {
    if (!s1 || !s2) return false;
    return s1->len == s2->len && !memcmp(s1->ptr, s2->ptr, s1->len);
}

bool str_chr_equals(String *s1, char *s2) {
    if (!s1 || !s2) return false;
    return s1->len == (int)strlen(s2) && !memcmp(s1->ptr, s2, s1->len);
}

// --- Tokenizer ---

typedef enum {
    TK_RESERVED,
    TK_NEWLINE,
    TK_IDENT,
    TK_NUM,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_INT,
    TK_CHAR,
    TK_ENUM,
    TK_STRUCT,
    TK_UNION,
    TK_EOF,
    TK_SIZEOF,
    TK_STRING,
    TK_CHAR_CONST,
    TK_PREPROCESSOR,
    TK_TYPEDEF,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_BREAK,
    TK_CONTINUE,
    TK_VOID,
    TK_BOOL,
    TK_BUILTIN_VA_LIST,
    TK_EXTERN,
    TK_LONG,
    TK_UNSIGNED,
    TK_SHORT
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    String *str;
    char *loc; // for error reporting
};

Token *token;

char *user_input;
char *filename;
char *dir_name;
int current_stack_size;
int arg_count;

bool startswith(char *p, char *q) {
    return strncmp(p, q, strlen(q)) == 0;
}

static bool is_ident1(char c) {
    return isalpha((unsigned char)c) || c == '_' || c == '$';
}
static bool is_ident2(char c) {
    return isalnum((unsigned char)c) || c == '_' || c == '$';
}

Token *new_token(TokenKind kind, char *start, int len) {
    Token *t = calloc(1, sizeof(Token));
    t->kind = kind;
    t->str = new_string(start, len);
    t->loc = start;
    return t;
}

Token *tokenize(char *p, bool eof) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // whitespace
        if (isspace((unsigned char)*p)) {
            if (*p == '\n') {
                cur->next = new_token(TK_NEWLINE, p, 1);
                cur = cur->next;
            }
            p++;
            continue;
        }

        // comments
        if (startswith(p, "//")) {
            while (*p && *p != '\n') p++;
            continue;
        }
        if (startswith(p, "/*")) {
            p += 2;
            while (*p && !startswith(p, "*/")) p++;
            if (*p) p += 2;
            continue;
        }

        // string literal
        if (*p == '"') {
            char *start = p + 1;
            p++;
            while (*p && *p != '"') {
                if (*p == '\\' && p[1]) p += 2;
                else p++;
            }
            if (*p != '"') error_at_here("unclosed string literal");
            int len = p - start;
            cur->next = new_token(TK_STRING, start, len);
            cur = cur->next;
            p++; // skip closing "
            continue;
        }

        // char constant
        if (*p == '\'') {
            char *start = p + 1;
            p++;
            if (*p == '\\' && p[1]) p += 2;
            else if (*p) p++;
            if (*p != '\'') error_at_here("unclosed char constant");
            int len = (start[0] == '\\') ? 2 : 1;
            cur->next = new_token(TK_CHAR_CONST, start, len);
            cur = cur->next;
            p++; // skip closing '
            continue;
        }

        // preprocessor
        if (*p == '#') {
            char *start = p;
            while (*p && *p != '\n') p++;
            int len = p - start;
            cur->next = new_token(TK_PREPROCESSOR, start, len);
            cur = cur->next;
            continue;
        }

        // number literal (decimal/hex)
        if (isdigit((unsigned char)*p)) {
            char *start = p;
            int base = 10;
            if (startswith(p, "0x") || startswith(p, "0X")) {
                base = 16;
                p += 2;
                while (isxdigit((unsigned char)*p)) p++;
            } else {
                while (isdigit((unsigned char)*p)) p++;
            }
            int val = (int)strtol(start, NULL, base);
            cur->next = new_token(TK_NUM, start, p - start);
            cur = cur->next;
            cur->val = val;
            continue;
        }

        // identifier or keyword
        if (is_ident1(*p)) {
            char *start = p;
            p++;
            while (is_ident2(*p)) p++;
            int len = p - start;
            TokenKind kind = TK_IDENT;
            if (len == 2 && strncmp(start, "if", 2) == 0) kind = TK_IF;
            else if (len == 4 && strncmp(start, "else", 4) == 0) kind = TK_ELSE;
            else if (len == 5 && strncmp(start, "while", 5) == 0) kind = TK_WHILE;
            else if (len == 3 && strncmp(start, "for", 3) == 0) kind = TK_FOR;
            else if (len == 6 && strncmp(start, "return", 6) == 0) kind = TK_RETURN;
            else if (len == 3 && strncmp(start, "int", 3) == 0) kind = TK_INT;
            else if (len == 4 && strncmp(start, "char", 4) == 0) kind = TK_CHAR;
            else if (len == 4 && strncmp(start, "enum", 4) == 0) kind = TK_ENUM;
            else if (len == 6 && strncmp(start, "struct", 6) == 0) kind = TK_STRUCT;
            else if (len == 5 && strncmp(start, "union", 5) == 0) kind = TK_UNION;
            else if (len == 4 && strncmp(start, "void", 4) == 0) kind = TK_VOID;
            else if (len == 4 && strncmp(start, "bool", 4) == 0) kind = TK_BOOL;
            else if (len == 6 && strncmp(start, "sizeof", 6) == 0) kind = TK_SIZEOF;
            else if (len == 7 && strncmp(start, "typedef", 7) == 0) kind = TK_TYPEDEF;
            else if (len == 6 && strncmp(start, "switch", 6) == 0) kind = TK_SWITCH;
            else if (len == 4 && strncmp(start, "case", 4) == 0) kind = TK_CASE;
            else if (len == 7 && strncmp(start, "default", 7) == 0) kind = TK_DEFAULT;
            else if (len == 5 && strncmp(start, "break", 5) == 0) kind = TK_BREAK;
            else if (len == 8 && strncmp(start, "continue", 8) == 0) kind = TK_CONTINUE;
            else if (len == 6 && strncmp(start, "extern", 6) == 0) kind = TK_EXTERN;
            else if (len == 4 && strncmp(start, "long", 4) == 0) kind = TK_LONG;
            else if (len == 8 && strncmp(start, "unsigned", 8) == 0) kind = TK_UNSIGNED;
            else if (len == 5 && strncmp(start, "short", 5) == 0) kind = TK_SHORT;
            else if (len >= 17 && strncmp(start, "__builtin_va_list", 17) == 0) kind = TK_BUILTIN_VA_LIST;
            cur->next = new_token(kind, start, len);
            cur = cur->next;
            continue;
        }

        // multi-char punctuators
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=") ||
            startswith(p, "&&") || startswith(p, "||") || startswith(p, "+=") || startswith(p, "-=") ||
            startswith(p, "*=") || startswith(p, "/=") || startswith(p, "++") || startswith(p, "--") ||
            startswith(p, "->")) {
            cur->next = new_token(TK_RESERVED, p, 2);
            cur = cur->next;
            p += 2;
            continue;
        }

        // single-char punctuators
        if (strchr("+-*/()<>;={},[]&|!~^%:.?", *p) || *p == '*' || *p == '&') {
            cur->next = new_token(TK_RESERVED, p, 1);
            cur = cur->next;
            p++;
            continue;
        }

        error_at_here("tokenize: unknown token at: %s", p);
    }

    if (eof) {
        cur->next = new_token(TK_EOF, p, 0);
        cur = cur->next;
    }

    return head.next;
}

// --- Preprocessor (simple) ---

typedef struct FileList FileList;
struct FileList {
    char *filename;
    FileList *next;
};

typedef struct Macro Macro;
struct Macro {
    String *ident;
    Token *replace;
    Macro *next;
};

FileList *once_files_list = NULL;
Macro *macros_list = NULL;

Macro *find_macro(String *str) {
    for (Macro *m = macros_list; m; m = m->next) {
        if (str_equals(str, m->ident))
            return m;
    }
    return NULL;
}

Token *remove_newline(Token *tok) {
    Token *before = NULL;
    Token *head = tok;
    for (Token *t = head; t; t = t->next) {
        if (t->kind == TK_NEWLINE) {
            if (before)
                before->next = t->next;
            else
                head = t->next;
        } else {
            before = t;
        }
    }
    return head;
}

bool str_starts_with_str(String *s, char *q) {
    if (!s) return false;
    return startswith(s->ptr, q);
}

Token *preprocess(Token *tok) {
    Token *before = NULL;
    Token *start = NULL;

    for (Token *t = tok; t; t = t->next) {
        if (t->kind == TK_PREPROCESSOR) {
            // parse directive
            if (str_starts_with_str(t->str, "#include")) {
                // next token should be string
                t = t->next;
                if (!t || t->kind != TK_STRING) error_at_here("#include expects string");
                char *inc_name = strndup(t->str->ptr, t->str->len);
                char path[1024];
                if (dir_name)
                    snprintf(path, sizeof(path), "%s/%s", dir_name, inc_name);
                else
                    snprintf(path, sizeof(path), "%s", inc_name);
                free(inc_name);
                FILE *f = fopen(path, "r");
                if (!f) error_at_here("include file not found: %s", path);
                fseek(f, 0, SEEK_END);
                long sz = ftell(f);
                fseek(f, 0, SEEK_SET);
                char *buf = calloc(1, sz + 1);
                fread(buf, 1, sz, f);
                fclose(f);
                Token *header_token = tokenize(buf, false);
                header_token = preprocess(header_token);
                if (header_token) {
                    Token *end = header_token;
                    while (end->next) end = end->next;
                    if (before) before->next = header_token;
                    else start = header_token;
                    before = end;
                }
                continue;
            }

            if (str_starts_with_str(t->str, "#pragma")) {
                t = t->next;
                if (t && str_chr_equals(t->str, "once")) {
                    bool found = false;
                    for (FileList *fl = once_files_list; fl; fl = fl->next) {
                        if (strcmp(fl->filename, filename) == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (found) return NULL;
                    FileList *new_f = calloc(1, sizeof(FileList));
                    new_f->filename = filename;
                    new_f->next = once_files_list;
                    once_files_list = new_f;
                }
                continue;
            }

            if (str_starts_with_str(t->str, "#define")) {
                t = t->next;
                if (!t || t->kind != TK_IDENT) error_at_here("#define expects identifier");
                Macro *m = calloc(1, sizeof(Macro));
                m->ident = t->str;
                t = t->next;
                m->replace = t;
                while (t->next && t->next->kind != TK_NEWLINE) t = t->next;
                Token *last = t;
                t = t->next;
                last->next = NULL;
                m->next = macros_list;
                macros_list = m;
                continue;
            }

            // other directives ignored
            continue;
        } else {
            if (t->kind == TK_IDENT) {
                Macro *m = find_macro(t->str);
                if (!m) {
                    if (before) before->next = t; else start = t;
                    before = t;
                    continue;
                }
                Token *repl = m->replace;
                while (repl) {
                    Token *newt = calloc(1, sizeof(Token));
                    memcpy(newt, repl, sizeof(Token));
                    newt->next = NULL;
                    if (before) before->next = newt; else start = newt;
                    before = newt;
                    repl = repl->next;
                }
                continue;
            }
            if (before) before->next = t; else start = t;
            before = t;
        }
    }

    return remove_newline(start);
}

// --- Types and structs ---

typedef enum {
    INT,
    PTR,
    ARRAY,
    CHAR,
    STRUCT,
    VOID,
    FUNC,
    BOOL,
    VA_LIST_TAG,
    LONG,
    SHORT
} TypeKind;

typedef struct Type Type;
typedef struct StructField StructField;
typedef struct StructType StructType;

struct StructField {
    StructField *next;
    String *name;
    Type *type;
    int offset;
};

struct StructType {
    StructType *next;
    String *name;
    StructField *fields;
    int alignment;
    int size;
    bool is_union;
};

struct Type {
    TypeKind ty;
    Type *ptr_to;
    int array_size;
    StructType *struct_type;
    bool is_unsigned;
};

Type *new_type(TypeKind ty, Type *ptr_to) {
    Type *type = calloc(1, sizeof(Type));
    type->ty = ty;
    type->ptr_to = ptr_to;
    type->is_unsigned = false;
    return type;
}

Type *new_type_with_unsigned(TypeKind ty, Type *ptr_to, bool is_unsigned) {
    Type *t = new_type(ty, ptr_to);
    t->is_unsigned = is_unsigned;
    return t;
}

Type *new_struct_type(String *name, bool is_union) {
    Type *ty = new_type(STRUCT, NULL);
    ty->struct_type = calloc(1, sizeof(StructType));
    ty->struct_type->name = name;
    ty->struct_type->is_union = is_union;
    return ty;
}

int align(int n, int al) {
    return (n + al - 1) / al * al;
}

int sizeof_type(Type *type) {
    switch (type->ty) {
    case INT: return 4;
    case PTR: return 8;
    case ARRAY: return sizeof_type(type->ptr_to) * type->array_size;
    case CHAR: return 1;
    case STRUCT: return type->struct_type->size;
    case BOOL: return 1;
    case VA_LIST_TAG: return 24;
    case LONG: return 8;
    case SHORT: return 2;
    default:
        error_at_here("sizeof_type: unknown type");
    }
    return 0;
}

// Forward declarations for functions used before their definitions
void add_field(StructType *type, Type *ty, String *name);
Type *consume_noident_type(void);

// --- AST / Nodes ---

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_ASSIGN_ARRAY,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_AND,
    ND_OR,
    ND_ADDR,
    ND_DEREF,
    ND_POST_INCR,
    ND_POST_DECR,
    ND_RETURN,
    ND_IF,
    ND_IFELSE,
    ND_SWITCH,
    ND_CASE,
    ND_DEFAULT,
    ND_BREAK,
    ND_CONTINUE,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_LVAR,
    ND_GVAR,
    ND_CALL,
    ND_NUM,
    ND_STRING,
    ND_VA_START,
    ND_UNNAMED
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
    Type *type;
    String *name;
    Node *next; // for lists
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = kind;
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

Node *new_typed_node(NodeKind kind, Node *lhs, Node *rhs, Type *type) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = kind;
    n->lhs = lhs;
    n->rhs = rhs;
    n->type = type;
    return n;
}

Node *new_node_num(int val) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = ND_NUM;
    n->val = val;
    Type *t = new_type(INT, NULL);
    n->type = t;
    return n;
}

Node *new_node_char(char val) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = ND_NUM;
    n->val = val;
    Type *t = new_type(CHAR, NULL);
    n->type = t;
    return n;
}

// --- Symbol tables ---

typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct EnumVal EnumVal;
typedef struct ConsumeTypeRes ConsumeTypeRes;
typedef struct TypeDef TypeDef;
typedef struct Enum Enum;
typedef struct Func Func;

struct LVar {
    LVar *next;
    String *name;
    int offset;
    Type *type;
};

struct GVar {
    GVar *next;
    String *name;
    Type *type;
};

struct EnumVal {
    EnumVal *next;
    String *name;
    int val;
};

struct ConsumeTypeRes {
    Type *type;
    Token *tok;
};

struct TypeDef {
    TypeDef *next;
    String *name;
    Type *type;
};

struct Func {
    Func *next;
    String *name;
    Type *type;
};

LVar *locals;
GVar *globals;
EnumVal *enumVals;
StructType *structs;
TypeDef *typedefs;
Func *funcs;

LVar *find_lvar(Token *tok) {
    for (LVar *v = locals; v; v = v->next)
        if (str_equals(v->name, tok->str)) return v;
    return NULL;
}

LVar *new_lvar(String *name, Type *type) {
    LVar *l = calloc(1, sizeof(LVar));
    l->next = locals;
    l->name = name;
    l->type = type;
    if (locals)
        l->offset = locals->offset + sizeof_type(type);
    else
        l->offset = sizeof_type(type);
    locals = l;
    return l;
}

EnumVal *find_enum_val(Token *tok) {
    for (EnumVal *v = enumVals; v; v = v->next)
        if (str_equals(v->name, tok->str)) return v;
    return NULL;
}

EnumVal *new_enum_val(String *name, int val) {
    EnumVal *e = calloc(1, sizeof(EnumVal));
    e->next = enumVals;
    e->name = name;
    e->val = val;
    enumVals = e;
    return e;
}

StructType *find_struct(Token *tok) {
    for (StructType *t = structs; t; t = t->next)
        if (str_equals(t->name, tok->str)) return t;
    return NULL;
}

StructField *find_struct_field(Token *tok, StructType *type) {
    for (StructField *f = type->fields; f; f = f->next)
        if (str_equals(f->name, tok->str)) return f;
    return NULL;
}

void new_gvar(String *name, Type *type) {
    GVar *g = calloc(1, sizeof(GVar));
    g->next = globals;
    g->name = name;
    g->type = type;
    globals = g;
}

GVar *find_gvar(Token *tok) {
    for (GVar *g = globals; g; g = g->next)
        if (str_equals(g->name, tok->str)) return g;
    return NULL;
}

TypeDef *find_typedef(Token *tok) {
    for (TypeDef *t = typedefs; t; t = t->next)
        if (str_equals(t->name, tok->str)) return t;
    return NULL;
}

TypeDef *new_typedef(String *name, Type *type) {
    TypeDef *td = calloc(1, sizeof(TypeDef));
    td->next = typedefs;
    td->name = name;
    td->type = type;
    typedefs = td;
    return td;
}

Func *find_func(Token *tok) {
    for (Func *f = funcs; f; f = f->next)
        if (str_equals(f->name, tok->str)) return f;
    return NULL;
}

Func *new_func(String *name, Type *type) {
    Func *f = calloc(1, sizeof(Func));
    f->next = funcs;
    f->name = name;
    f->type = type;
    funcs = f;
    return f;
}

// --- Parser helpers ---

bool consume(char *op) {
    if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op)) return false;
    token = token->next;
    return true;
}

Token *consume_kind(TokenKind kind) {
    if (token->kind == kind) {
        Token *t = token;
        token = token->next;
        return t;
    }
    return NULL;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT) return NULL;
    Token *t = token;
    token = token->next;
    return t;
}

bool check_kind(TokenKind kind) {
    return token->kind == kind;
}

void go_to(Token *tok) {
    token = tok;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
        error_at_here("token mismatch: expected %s", op);
    token = token->next;
}

void expect_kind(TokenKind kind) {
    if (token->kind != kind) error_at_here("token mismatch");
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM) error_at_here("token mismatch: expected number");
    int v = token->val;
    token = token->next;
    return v;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// --- Type parsing with long/short/unsigned support ---

ConsumeTypeRes *consume_type();

Type *consume_type_name() {
    bool saw_unsigned = false;
    if (consume_kind(TK_UNSIGNED)) saw_unsigned = true;

    // long, short, int, char, void, bool, builtin va_list, enum, struct/union, typedefs
    if (consume_kind(TK_LONG)) {
        Type *t = new_type(LONG, NULL);
        t->is_unsigned = saw_unsigned;
        return t;
    }
    if (consume_kind(TK_SHORT)) {
        Type *t = new_type(SHORT, NULL);
        t->is_unsigned = saw_unsigned;
        return t;
    }
    if (consume_kind(TK_INT)) {
        Type *t = new_type(INT, NULL);
        t->is_unsigned = saw_unsigned;
        return t;
    }
    if (consume_kind(TK_CHAR)) {
        Type *t = new_type(CHAR, NULL);
        t->is_unsigned = saw_unsigned;
        return t;
    }
    if (consume_kind(TK_VOID)) return new_type(VOID, NULL);
    if (consume_kind(TK_BOOL)) return new_type(BOOL, NULL);
    if (consume_kind(TK_BUILTIN_VA_LIST)) {
        Type *type = new_type(ARRAY, new_type(VA_LIST_TAG, NULL));
        type->array_size = 1;
        return type;
    }
    if (consume_kind(TK_ENUM)) {
        consume_kind(TK_IDENT);
        if (consume("{")) {
            int i = 0;
            while (!consume("}")) {
                Token *name = consume_ident();
                if (!name) error_at_here("expected enum name");
                new_enum_val(name->str, i++);
                consume(",");
            }
        }
        Type *t = new_type(INT, NULL);
        t->is_unsigned = saw_unsigned;
        return t;
    }

    if (check_kind(TK_STRUCT) || check_kind(TK_UNION)) {
        bool is_union = false;
        if (consume_kind(TK_UNION)) is_union = true;
        else consume_kind(TK_STRUCT);

        Token *id = consume_kind(TK_IDENT);

        if (consume("{")) {
            String *name = NULL;
            if (id) name = id->str;
            Type *type = new_struct_type(name, is_union);
            while (!consume("}")) {
                ConsumeTypeRes *res = consume_type();
                if (!res) error_at_here("expected type");
                add_field(type->struct_type, res->type, res->tok->str);
                expect(";");
            }
            if (id) {
                StructType *s = find_struct(id);
                if (s) {
                    if (s->fields) error_at_here("struct %.*s is already defined", id->str->len, id->str->ptr);
                    else {
                        s->fields = type->struct_type->fields;
                        s->is_union = type->struct_type->is_union;
                        s->alignment = type->struct_type->alignment;
                        s->size = type->struct_type->size;
                    }
                } else {
                    type->struct_type->next = structs;
                    structs = type->struct_type;
                }
            }
            return type;
        } else {
            if (!id) error_at_here("expected struct name");
            StructType *type = find_struct(id);
            Type *ty;
            if (!type) {
                ty = new_struct_type(id->str, is_union);
                ty->struct_type->next = structs;
                structs = ty->struct_type;
            } else {
                ty = new_type(STRUCT, NULL);
                ty->struct_type = type;
            }
            return ty;
        }
    }

    Token *tok = consume_kind(TK_IDENT);
    if (tok) {
        TypeDef *tdef = find_typedef(tok);
        if (tdef) return tdef->type;
        go_to(tok);
    }

    if (saw_unsigned) {
        Type *t = new_type(INT, NULL);
        t->is_unsigned = true;
        return t;
    }

    return NULL;
}

ConsumeTypeRes *expect_nested_type(Type *type) {
    int ptr = 0;
    ConsumeTypeRes *res;

    if (consume("(")) {
        while (consume("*")) ptr++;
        res = expect_nested_type(type);
        expect(")");
    } else {
        Token *tok = consume_ident();
        if (!tok) error_at_here("expected identifier");
        res = calloc(1, sizeof(ConsumeTypeRes));
        res->type = type;
        res->tok = tok;
    }

    if (consume("(")) {
        // parse parameter types without identifiers
        Type *tmp = consume_noident_type();
        if (!tmp) error_at_here("expected type in parameter");
        while (consume(",")) {
            if (consume("...")) break;
            Type *tmp2 = consume_noident_type();
            if (!tmp2) error_at_here("expected type in parameter");
        }
        expect(")");
        res->type = new_type(FUNC, type);
    }

    for (int i = 0; i < ptr; i++) res->type = new_type(PTR, res->type);

    while (consume("[")) {
        Node *size = NULL;
        if (token->kind == TK_NUM) {
            size = new_node_num(token->val);
            token = token->next;
        } else {
            size = new_node_num(0);
        }
        expect("]");
        res->type = new_type(ARRAY, type);
        res->type->array_size = size->val;
    }

    return res;
}

ConsumeTypeRes *consume_type() {
    Type *type = consume_type_name();
    if (!type) return NULL;
    while (consume("*")) type = new_type(PTR, type);
    ConsumeTypeRes *res = expect_nested_type(type);
    return res;
}

Type *expect_noident_type(Type *type) {
    while (consume("*")) type = new_type(PTR, type);

    if (consume("(")) {
        type = expect_noident_type(type);
        expect(")");
    }

    if (consume("(")) {
        // function type
        if (!consume(")")) {
            for (;;) {
                Type *tmp = consume_noident_type();
                if (!tmp) error_at_here("expected type in parameter");
                if (!consume(",")) break;
            }
            expect(")");
        }
        type = new_type(FUNC, NULL);
    }

    while (consume("[")) {
        Node *size = NULL;
        if (token->kind == TK_NUM) {
            size = new_node_num(token->val);
            token = token->next;
        } else {
            size = new_node_num(0);
        }
        expect("]");
        type = new_type(ARRAY, type);
        type->array_size = size->val;
    }

    return type;
}

Type *consume_noident_type() {
    Type *type = consume_type_name();
    if (!type) return NULL;
    return expect_noident_type(type);
}

// --- Parser (expressions/statements) ---

Node *stmt();
Node *expr();
Node *assign();
Node *logical();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();

Node *new_node_lvar(LVar *lvar) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = ND_LVAR;
    n->offset = lvar->offset;
    n->type = lvar->type;
    return n;
}

Node *new_node_gvar(GVar *gvar) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = ND_GVAR;
    n->name = gvar->name;
    n->type = gvar->type;
    return n;
}

Node *stmt() {
    Node *node;
    if (consume_kind(TK_CASE)) {
        Node *e = expr();
        if (e->kind != ND_NUM) error_at_here("expected constant expression");
        expect(":");
        node = new_node(ND_CASE, e, NULL);
    } else if (consume_kind(TK_DEFAULT)) {
        expect(":");
        node = new_node(ND_DEFAULT, NULL, NULL);
    } else if (consume_kind(TK_IF)) {
        expect("(");
        Node *lhs = expr();
        expect(")");
        Node *rhs = stmt();
        if (consume_kind(TK_ELSE))
            node = new_node(ND_IFELSE, lhs, new_node(ND_UNNAMED, rhs, stmt()));
        else
            node = new_node(ND_IF, lhs, rhs);
    } else if (consume_kind(TK_SWITCH)) {
        expect("(");
        Node *lhs = expr();
        expect(")");
        Node *rhs = stmt();
        node = new_node(ND_SWITCH, lhs, rhs);
    } else if (consume_kind(TK_WHILE)) {
        expect("(");
        Node *lhs = expr();
        expect(")");
        node = new_node(ND_WHILE, lhs, stmt());
    } else if (consume_kind(TK_FOR)) {
        Node *a = NULL, *b = NULL, *c = NULL, *d = NULL;
        expect("(");
        if (!consume(";")) { a = expr(); expect(";"); }
        if (!consume(";")) { b = expr(); expect(";"); }
        if (!consume(")")) { c = expr(); expect(")"); }
        d = stmt();
        Node *lhs = new_node(ND_UNNAMED, a, b);
        Node *rhs = new_node(ND_UNNAMED, c, d);
        node = new_node(ND_FOR, lhs, rhs);
    } else if (consume("{")) {
        node = new_node(ND_BLOCK, NULL, NULL);
        for (Node *last = node; !consume("}"); last = last->rhs)
            last->rhs = new_node(ND_UNNAMED, stmt(), NULL);
    } else if (consume_kind(TK_RETURN)) {
        if (consume(";")) node = new_node(ND_RETURN, NULL, NULL);
        else { node = new_node(ND_RETURN, expr(), NULL); expect(";"); }
    } else if (consume_kind(TK_BREAK)) {
        node = new_node(ND_BREAK, NULL, NULL);
        expect(";");
    } else if (consume_kind(TK_CONTINUE)) {
        node = new_node(ND_CONTINUE, NULL, NULL);
        expect(";");
    } else {
        node = expr();
        expect(";");
    }
    return node;
}

Node *expr() { return assign(); }

Node *assign() {
    Node *node = logical();
    if (consume("=")) node = new_node(ND_ASSIGN, node, assign());
    else if (consume("+=")) node = new_node(ND_ASSIGN, node, new_node(ND_ADD, node, assign()));
    else if (consume("-=")) node = new_node(ND_ASSIGN, node, new_node(ND_SUB, node, assign()));
    else if (consume("*=")) node = new_node(ND_ASSIGN, node, new_node(ND_MUL, node, assign()));
    else if (consume("/=")) node = new_node(ND_ASSIGN, node, new_node(ND_DIV, node, assign()));
    return node;
}

Node *logical() {
    Node *node = equality();
    for (;;) {
        if (consume("&&")) node = new_node(ND_AND, node, logical());
        else if (consume("||")) node = new_node(ND_OR, node, logical());
        else return node;
    }
}

Node *equality() {
    Node *node = relational();
    for (;;) {
        if (consume("==")) node = new_node(ND_EQ, node, relational());
        else if (consume("!=")) node = new_node(ND_NE, node, relational());
        else return node;
    }
}

Node *relational() {
    Node *node = add();
    for (;;) {
        if (consume("<=")) node = new_node(ND_LE, node, add());
        else if (consume("<")) node = new_node(ND_LT, node, add());
        else if (consume(">=")) node = new_node(ND_LE, add(), node);
        else if (consume(">")) node = new_node(ND_LT, add(), node);
        else return node;
    }
}

Node *add() {
    Node *node = mul();
    for (;;) {
        if ((node->type->ty == PTR || node->type->ty == ARRAY) && node->type->ptr_to && node->type->ptr_to->ty != VOID) {
            int size = sizeof_type(node->type->ptr_to);
            if (consume("+")) {
                if (node->type->ty == ARRAY)
                    node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
                node = new_node(ND_ADD, node, new_node(ND_MUL, mul(), new_node_num(size)));
            } else if (consume("-")) {
                if (node->type->ty == ARRAY)
                    node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
                node = new_node(ND_SUB, node, new_node(ND_MUL, mul(), new_node_num(size)));
            }
            return node;
        }
        if (consume("+")) {
            Node *rhs = mul();
            if (node->kind == ND_NUM && rhs->kind == ND_NUM) node->val += rhs->val;
            else node = new_node(ND_ADD, node, rhs);
        } else if (consume("-")) {
            Node *rhs = mul();
            if (node->kind == ND_NUM && rhs->kind == ND_NUM) node->val -= rhs->val;
            else node = new_node(ND_SUB, node, rhs);
        } else return node;
    }
}

Node *mul() {
    Node *node = unary();
    for (;;) {
        if (consume("*")) {
            Node *rhs = unary();
            if (node->kind == ND_NUM && rhs->kind == ND_NUM) node->val *= rhs->val;
            else node = new_node(ND_MUL, node, rhs);
        } else if (consume("/")) {
            Node *rhs = unary();
            if (node->kind == ND_NUM && rhs->kind == ND_NUM) node->val /= rhs->val;
            else node = new_node(ND_DIV, node, rhs);
        } else return node;
    }
}

Node *unary() {
    if (consume("+")) return postfix();
    else if (consume("-")) {
        Node *rhs = postfix();
        if (rhs->kind == ND_NUM) { rhs->val = -rhs->val; return rhs; }
        else return new_node(ND_SUB, new_node_num(0), rhs);
    } else if (consume("&")) return new_node(ND_ADDR, postfix(), NULL);
    else if (consume("*")) {
        Node *l = primary();
        if (l->type->ty != PTR && l->type->ty != ARRAY) error_at_here("dereference failed: not a pointer");
        if (l->type->ty == ARRAY) l = new_node(ND_ADDR, l, NULL);
        return new_node(ND_DEREF, l, NULL);
    } else if (consume_kind(TK_SIZEOF)) {
        Token *cur = token;
        if (consume("(")) {
            Type *type = consume_noident_type();
            if (type) {
                expect(")");
                return new_node_num(sizeof_type(type));
            }
            go_to(cur);
        }
        Node *n = unary();
        return new_node_num(sizeof_type(n->type));
    } else if (consume("!")) return new_node(ND_EQ, postfix(), new_node_num(0));
    else if (consume("++")) {
        Node *n = postfix();
        return new_node(ND_ASSIGN, n, new_node(ND_ADD, n, new_node_num(1)));
    } else if (consume("--")) {
        Node *n = postfix();
        return new_node(ND_ASSIGN, n, new_node(ND_SUB, n, new_node_num(1)));
    }
    return postfix();
}

Node *postfix() {
    Node *node = primary();
    for (;;) {
        if (consume("[")) {
            Node *subscript = expr();
            int size = sizeof_type(node->type->ptr_to);
            if (node->type->ty == ARRAY)
                node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
            node = new_node(ND_ADD, node, new_node(ND_MUL, subscript, new_node_num(size)));
            expect("]");
        } else if (consume("++")) {
            node = new_node(ND_POST_INCR, node, new_node_num(1));
        } else if (consume("--")) {
            node = new_node(ND_POST_DECR, node, new_node_num(1));
        } else if (consume("(")) {
            // function call
            Node *n = new_node(ND_CALL, NULL, NULL);
            n->name = node->name;
            n->type = node->type;
            if (!consume(")")) {
                n->lhs = expr();
                while (consume(",")) n->lhs = new_node(ND_UNNAMED, n->lhs, expr());
                expect(")");
            }
            node = n;
        } else break;
    }
    return node;
}

Node *primary() {
    if (consume("(")) {
        Node *n = expr();
        expect(")");
        return n;
    }
    if (token->kind == TK_NUM) {
        Node *n = new_node_num(token->val);
        token = token->next;
        return n;
    }
    if (token->kind == TK_CHAR_CONST) {
        char c = token->str->ptr[0];
        Node *n = new_node_char(c);
        token = token->next;
        return n;
    }
    if (token->kind == TK_STRING) {
        Node *n = new_node(ND_STRING, NULL, NULL);
        n->name = token->str;
        token = token->next;
        return n;
    }
    if (token->kind == TK_IDENT) {
        Token *t = token;
        token = token->next;
        // variable or function
        LVar *lv = find_lvar(t);
        if (lv) return new_node_lvar(lv);
        GVar *gv = find_gvar(t);
        if (gv) return new_node_gvar(gv);
        Func *f = find_func(t);
        if (f) {
            Node *n = calloc(1, sizeof(Node));
            n->kind = ND_CALL;
            n->name = t->str;
            n->type = f->type;
            return n;
        }
        // unknown identifier -> treat as global var
        new_gvar(t->str, new_type(INT, NULL));
        GVar *g2 = find_gvar(t);
        return new_node_gvar(g2);
    }
    error_at_here("primary: unexpected token");
    return NULL;
}

// --- Struct field addition (used earlier) ---

void add_field(StructType *type, Type *ty, String *name) {
    StructField *field = calloc(1, sizeof(StructField));
    field->type = ty;
    field->name = name;
    int al;
    if (ty->ty == ARRAY) al = sizeof_type(ty->ptr_to);
    else al = sizeof_type(ty);
    if (al > type->alignment) type->alignment = al;
    if (type->is_union) {
        if (sizeof_type(ty) > type->size) type->size = sizeof_type(ty);
    } else {
        if (type->fields) {
            field->offset = align(type->fields->offset + sizeof_type(type->fields->type), al);
            type->size = align(field->offset + sizeof_type(field->type), type->alignment);
        } else {
            field->offset = 0;
            type->size = align(sizeof_type(field->type), type->alignment);
        }
    }
    field->next = type->fields;
    type->fields = field;
}

// --- External (top-level) parsing ---

typedef enum {
    EXT_FUNC,
    EXT_FUNCDECL,
    EXT_GVAR,
    EXT_ENUM,
    EXT_STRUCT,
    EXT_TYPEDEF
} ExternalKind;

typedef struct External External;
struct External {
    ExternalKind kind;
    String *name;
    Node *code;
    int offsets[6];
    int size;
    String *literals; // simplified
    int stack_size;
    bool is_variadic;
    bool is_extern;
};

External *ext;
int literal_count;

External *external() {
    locals = NULL;
    if (!globals) new_gvar(new_string("NULL", 4), new_type(PTR, new_type(VOID, NULL)));

    External *external = calloc(1, sizeof(External));
    ext = external;
    int i = 0;

    if (consume_kind(TK_EXTERN)) ext->is_extern = true;

    if (check_kind(TK_ENUM)) {
        ext->kind = EXT_ENUM;
        consume_type_name();
        expect(";");
        return external;
    }

    if (check_kind(TK_STRUCT) || check_kind(TK_UNION)) {
        ext->kind = EXT_STRUCT;
        Type *ty = consume_type_name();
        if (!ty) error_at_here("expected struct or union");
        expect(";");
        return external;
    }

    if (consume_kind(TK_TYPEDEF)) {
        ext->kind = EXT_TYPEDEF;
        ConsumeTypeRes *res = consume_type();
        if (!res) error_at_here("expected type");
        new_typedef(res->tok->str, res->type);
        expect(";");
        return external;
    }

    ConsumeTypeRes *res = consume_type();
    if (!res) error_at_here("invalid type");
    external->name = res->tok->str;
    if (res->type->ty == FUNC) {
        external->kind = EXT_FUNC;
        if (!find_func(res->tok)) new_func(res->tok->str, res->type->ptr_to);
        go_to(res->tok);
        token = token->next; // skip identifier
        expect("(");
        Token *cur = token;
        bool no_args = false;
        if (consume_kind(TK_VOID)) {
            if (consume(")")) no_args = true;
            else go_to(cur);
        }
        if (!consume(")") && !no_args) {
            for (;;) {
                if (consume("...")) { external->is_variadic = true; break; }
                ConsumeTypeRes *cres = consume_type();
                if (!cres) error_at_here("failed to parse argument");
                LVar *lvar = find_lvar(cres->tok);
                if (!lvar) lvar = new_lvar(cres->tok->str, cres->type);
                external->offsets[i++] = lvar->offset;
                if (!consume(",")) break;
            }
            expect(")");
        }
        if (consume(";")) {
            external->kind = EXT_FUNCDECL;
            return external;
        }
        external->code = stmt();
        if (external->code->kind != ND_BLOCK) error_at_here("expected block");
    } else {
        external->kind = EXT_GVAR;
        new_gvar(res->tok->str, res->type);
        external->size = sizeof_type(res->type);
        expect(";");
    }
    if (locals) external->stack_size = (locals->offset / 8) * 8 + 8;
    return external;
}

// --- Code generation (very small x86-64 emitter) ---

// We'll emit assembly to stdout. This is a minimal emitter for demonstration.

void gen_lval(Node *node) {
    if (node->kind == ND_LVAR) {
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n", node->offset);
        printf("    push rax\n");
        return;
    }
    if (node->kind == ND_GVAR) {
        printf("    lea rax, [rip+.%.*s]\n", node->name->len, node->name->ptr);
        printf("    push rax\n");
        return;
    }
    error_at_here("not an lvalue");
}

void gen(Node *node);

void gen_binary_arith(Node *node, char *op) {
    gen(node->lhs);
    gen(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    if (strcmp(op, "add") == 0) printf("    add rax, rdi\n");
    else if (strcmp(op, "sub") == 0) printf("    sub rax, rdi\n");
    else if (strcmp(op, "mul") == 0) printf("    imul rax, rdi\n");
    else if (strcmp(op, "div") == 0) {
        printf("    cqo\n");
        printf("    idiv rdi\n");
    }
    printf("    push rax\n");
}

void gen(Node *node) {
    if (!node) return;
    switch (node->kind) {
    case ND_NUM:
        printf("    mov rax, %d\n", node->val);
        printf("    push rax\n");
        return;
    case ND_STRING:
        // emit label and push address
        printf("    lea rax, [rip+.Lstr%d]\n", literal_count++);
        printf("    push rax\n");
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_GVAR:
        printf("    lea rax, [rip+.%.*s]\n", node->name->len, node->name->ptr);
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("    pop rdi\n"); // rhs value
        printf("    pop rax\n"); // lval address
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    case ND_ADD:
        gen_binary_arith(node, "add");
        return;
    case ND_SUB:
        gen_binary_arith(node, "sub");
        return;
    case ND_MUL:
        gen_binary_arith(node, "mul");
        return;
    case ND_DIV:
        gen_binary_arith(node, "div");
        return;
    case ND_RETURN:
        if (node->lhs) gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    case ND_BLOCK:
        for (Node *n = node->rhs; n; n = n->rhs) gen(n->lhs);
        return;
    case ND_CALL:
        {
            const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
            // flatten args list
            Node *alist[16];
            int ac = 0;
            for (Node *a = node->lhs; a; a = a->rhs) {
                alist[ac++] = a->lhs;
            }
            for (int i = ac - 1; i >= 0; i--) gen(alist[i]);
            for (int i = 0; i < ac; i++) {
                printf("    pop %s\n", regs[i]);
            }
            printf("    call %.*s\n", node->name->len, node->name->ptr);
            printf("    push rax\n");
            return;
        }
    default:
        error_at_here("codegen: unsupported node kind");
    }
}

// --- Top-level compile driver ---

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: cc <file.c>\n");
        return 1;
    }
    filename = argv[1];
    // read file
    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); return 1; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = calloc(1, sz + 1);
    fread(buf, 1, sz, f);
    fclose(f);
    user_input = buf;
    // set dir_name
    char *last_slash = strrchr(filename, '/');
    if (last_slash) {
        dir_name = strndup(filename, last_slash - filename);
    } else {
        dir_name = ".";
    }

    Token *tok = tokenize(buf, true);
    tok = preprocess(tok);
    if (!tok) error_at_here("preprocess returned NULL");

    token = tok;

    // parse top-level externals and emit assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    while (!at_eof()) {
        External *e = external();
        if (!e) break;
        if (e->kind == EXT_GVAR) {
            printf(".data\n");
            printf(".align 8\n");
            printf(".%.*s:\n", e->name->len, e->name->ptr);
            printf("    .zero %d\n", e->size);
        } else if (e->kind == EXT_FUNC || e->kind == EXT_FUNCDECL) {
            if (e->kind == EXT_FUNC) {
                printf(".text\n");
                printf(".%.*s:\n", e->name->len, e->name->ptr);
                printf("    push rbp\n");
                printf("    mov rbp, rsp\n");
                // generate function body
                gen(e->code);
                // if no return emitted
                printf("    mov rsp, rbp\n");
                printf("    pop rbp\n");
                printf("    ret\n");
            } else {
                // function declaration: nothing to emit
            }
        }
    }

    return 0;
}
