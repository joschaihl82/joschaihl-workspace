#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/stat.h>

/* --- 1. Forward Declarations & Typedefs --- */

#define bool _Bool
#define true 1
#define false 0

typedef struct String String;
typedef struct Token Token;
typedef struct Type Type;
typedef struct StructField StructField;
typedef struct StructType StructType;
typedef struct Node Node;
typedef struct StringLiteral StringLiteral;
typedef struct External External;
typedef struct Macro Macro;
typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct EnumVal EnumVal;
typedef struct TypeDef TypeDef;
typedef struct Func Func;
typedef struct ConsumeTypeRes ConsumeTypeRes;

/* --- 2. Enums --- */

typedef enum {
    TK_RESERVED, TK_NEWLINE, TK_IDENT, TK_NUM, TK_RETURN, TK_IF, TK_ELSE,
    TK_WHILE, TK_FOR, TK_INT, TK_CHAR, TK_ENUM, TK_STRUCT, TK_UNION,
    TK_EOF, TK_SIZEOF, TK_STRING, TK_CHAR_CONST, TK_PREPROCESSOR,
    TK_TYPEDEF, TK_SWITCH, TK_CASE, TK_DEFAULT, TK_BREAK, TK_CONTINUE,
    TK_VOID, TK_BOOL, TK_BUILTIN_VA_LIST, TK_EXTERN
} TokenKind;

typedef enum {
    INT, PTR, ARRAY, CHAR, STRUCT, VOID, FUNC, BOOL, VA_LIST_TAG
} TypeKind;

typedef enum {
    ND_ADD, ND_SUB, ND_MUL, ND_DIV, ND_ASSIGN, ND_ASSIGN_ARRAY,
    ND_EQ, ND_NE, ND_LT, ND_LE, ND_AND, ND_OR, ND_ADDR, ND_DEREF,
    ND_POST_INCR, ND_POST_DECR, ND_RETURN, ND_IF, ND_IFELSE,
    ND_SWITCH, ND_CASE, ND_DEFAULT, ND_BREAK, ND_CONTINUE,
    ND_WHILE, ND_FOR, ND_BLOCK, ND_LVAR, ND_GVAR, ND_CALL,
    ND_NUM, ND_STRING, ND_VA_START, ND_UNNAMED
} NodeKind;

typedef enum {
    EXT_FUNC, EXT_FUNCDECL, EXT_GVAR, EXT_ENUM, EXT_STRUCT, EXT_TYPEDEF
} ExternalKind;

/* --- 3. Struct Definitions --- */

struct String {
    char *ptr;
    int len;
};

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    String *str;
};

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
};

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;      // ND_NUM
    int offset;   // ND_LVAR
    Type *type;   // expr
    String *name; // ND_CALL
};

struct StringLiteral {
    StringLiteral *next;
    String *str;
    int offset;
};

struct External {
    ExternalKind kind;
    External *next; /* Added for linked list in main */
    String *name;
    Node *code;
    int offsets[6];
    int size;
    StringLiteral *literals;
    int stack_size;
    bool is_variadic;
    bool is_extern;
};

struct Macro {
    Macro *next;
    String *ident;
    Token *replace;
};

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

typedef struct OnceFile OnceFile;
struct OnceFile {
    OnceFile *next;
    char *filename;
};

/* --- 4. Global Variables --- */

Token *token = NULL;
char *user_input = NULL;
char *dir_name = NULL;
char *filename = NULL;
int current_stack_size = 0;
int arg_count = 0;
int literal_count = 0;

OnceFile *once_files = NULL;
Macro *macro_head = NULL;

LVar *locals = NULL;
GVar *globals = NULL;
EnumVal *enumVals = NULL;
StructType *structs = NULL;
TypeDef *typedefs = NULL;
Func *funcs = NULL;
External *ext = NULL;

/* --- 5. Function Prototypes --- */

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_at_token(Token *tok, char *fmt, ...);
void error_at_here(char *fmt, ...);
void assert_compiler(bool flag); /* Renamed to avoid conflict with assert.h macro */

String *new_string(char *ptr, int len);
bool str_equals(String *s1, String *s2);
bool str_chr_equals(String *s1, char *s2);
bool startswith(char *p, char *q);

int align(int n, int al);
Type *new_type(TypeKind ty, Type *ptr_to);
Type *new_struct_type(String *name, bool is_union);
void add_field(StructType *type, Type *ty, String *name);
int sizeof_type(Type *type);

Token *tokenize(char *p, bool eof);
Token *preprocess(Token *tok);
char *read_file(char *path);
char *get_dir(char *path);

/* Parser Functions */
Node *expr();
Node *stmt();
Node *assign();
Node *logical();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();

/* --- 6. Helper Implementations --- */

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "%s: ", loc ? loc : "(unknown)");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void error_at_token(Token *tok, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "at token ");
    if (tok && tok->str)
        fprintf(stderr, "%.*s: ", tok->str->len, tok->str->ptr);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void error_at_here(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void assert_compiler(bool flag) {
    if (!flag)
        error_at_here("assertion failed");
}

String *new_string(char *ptr, int len) {
    String *substr = calloc(1, sizeof(String));
    substr->ptr = ptr;
    substr->len = len;
    return substr;
}

bool str_equals(String *s1, String *s2) {
    return s1->len == s2->len && !memcmp(s1->ptr, s2->ptr, s1->len);
}

bool str_chr_equals(String *s1, char *s2) {
    return s1->len == (int)strlen(s2) && !memcmp(s1->ptr, s2, s1->len);
}

bool startswith(char *p, char *q) {
    while (*q) {
        if (*p == '\0' || *p != *q) return false;
        p++; q++;
    }
    return true;
}

int align(int n, int al) {
    return (n + al - 1) / al * al;
}

Type *new_type(TypeKind ty, Type *ptr_to) {
    Type *type = calloc(1, sizeof(Type));
    type->ty = ty;
    type->ptr_to = ptr_to;
    return type;
}

Type *new_struct_type(String *name, bool is_union) {
    Type *ty = new_type(STRUCT, NULL);
    ty->struct_type = calloc(1, sizeof(StructType));
    ty->struct_type->name = name;
    ty->struct_type->is_union = is_union;
    return ty;
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
    default: break;
    }
    error_at_here("sizeof_type: unknown type");
    return 0;
}

void add_field(StructType *type, Type *ty, String *name) {
    StructField *field = calloc(1, sizeof(StructField));
    field->type = ty;
    field->name = name;

    int al;
    if (ty->ty == ARRAY)
        al = sizeof_type(ty->ptr_to);
    else
        al = sizeof_type(ty);

    if (al > type->alignment)
        type->alignment = al;

    if (type->is_union) {
        int fsize = sizeof_type(ty);
        if (fsize > type->size)
            type->size = fsize;
        field->offset = 0;
    }
    else {
        int base = type->size;
        field->offset = align(base, al);
        type->size = align(field->offset + sizeof_type(field->type), type->alignment);
    }

    field->next = type->fields;
    type->fields = field;
}

/* --- 7. Scope & Symbol Table Management --- */

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next)
        if (str_equals(var->name, tok->str)) return var;
    return NULL;
}

LVar *new_lvar(String *name, Type *type) {
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = name;
    lvar->type = type;
    if (locals)
        lvar->offset = locals->offset + sizeof_type(locals->type);
    else
        lvar->offset = 0;
    locals = lvar;
    return lvar;
}

EnumVal *find_enum_val(Token *tok) {
    for (EnumVal *val = enumVals; val; val = val->next)
        if (str_equals(val->name, tok->str)) return val;
    return NULL;
}

EnumVal *new_enum_val(String *name, int val) {
    EnumVal *enumVal = calloc(1, sizeof(EnumVal));
    enumVal->next = enumVals;
    enumVal->name = name;
    enumVal->val = val;
    enumVals = enumVal;
    return enumVal;
}

StructType *find_struct(Token *tok) {
    for (StructType *type = structs; type; type = type->next)
        if (str_equals(type->name, tok->str)) return type;
    return NULL;
}

StructField *find_struct_field(Token *tok, StructType *type) {
    for (StructField *field = type->fields; field; field = field->next)
        if (str_equals(field->name, tok->str)) return field;
    return NULL;
}

void new_gvar(String *name, Type *type) {
    GVar *gvar = calloc(1, sizeof(GVar));
    gvar->next = globals;
    gvar->name = name;
    gvar->type = type;
    globals = gvar;
}

GVar *find_gvar(Token *tok) {
    for (GVar *var = globals; var; var = var->next)
        if (str_equals(var->name, tok->str)) return var;
    return NULL;
}

TypeDef *find_typedef(Token *tok) {
    for (TypeDef *type = typedefs; type; type = type->next)
        if (str_equals(type->name, tok->str)) return type;
    return NULL;
}

TypeDef *new_typedef(String *name, Type *type) {
    TypeDef *typedef_ = calloc(1, sizeof(TypeDef));
    typedef_->next = typedefs;
    typedef_->name = name;
    typedef_->type = type;
    typedefs = typedef_;
    return typedef_;
}

Func *find_func(Token *tok) {
    for (Func *func = funcs; func; func = func->next)
        if (str_equals(func->name, tok->str)) return func;
    return NULL;
}

Func *new_func(String *name, Type *type) {
    Func *func = calloc(1, sizeof(Func));
    func->next = funcs;
    func->name = name;
    func->type = type;
    funcs = func;
    return func;
}

/* --- 8. Token Consumption Helpers --- */

bool consume(char *op) {
    if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
        return false;
    token = token->next;
    return true;
}

Token *consume_kind(TokenKind kind) {
    if (token->kind == kind) {
        Token *tok = token;
        token = token->next;
        return tok;
    }
    return NULL;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT) return NULL;
    Token *res = token;
    token = token->next;
    return res;
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
    if (token->kind != kind)
        error_at_here("token mismatch");
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM)
        error_at_here("token mismatch: expected number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

void next() {
    token = token->next;
}

/* --- 9. Type Parsing --- */

ConsumeTypeRes *consume_type();

Type *consume_type_name() {
    if (consume_kind(TK_INT)) return new_type(INT, NULL);
    if (consume_kind(TK_CHAR)) return new_type(CHAR, NULL);
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
        return new_type(INT, NULL);
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
    return NULL;
}

Type *consume_noident_type();

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
        consume_noident_type();
        consume_ident();
        while (consume(",")) {
            if (consume("...")) break;
            assert_compiler(consume_noident_type() != NULL);
            consume_ident();
        }
        expect(")");
        res->type = new_type(FUNC, type);
    }
    for (int i = 0; i < ptr; i++)
        res->type = new_type(PTR, res->type);

    while (consume("[")) {
        Node *size = expr();
        if (size->kind != ND_NUM) error_at_here("expected constant expression");
        expect("]");
        res->type = new_type(ARRAY, type);
        res->type->array_size = size->val;
    }
    return res;
}

ConsumeTypeRes *consume_type() {
    Type *type = consume_type_name();
    if (!type) return NULL;
    while (consume("*"))
        type = new_type(PTR, type);
    ConsumeTypeRes *res = expect_nested_type(type);
    return res;
}

Type *expect_noident_type(Type *type) {
    while (consume("*"))
        type = new_type(PTR, type);
    if (consume("(")) {
        type = expect_noident_type(type);
        expect(")");
    }
    if (consume("(")) {
        consume_noident_type();
        while (consume(","))
            consume_noident_type();
        expect(")");
        type = new_type(FUNC, NULL);
    }
    while (consume("[")) {
        Node *size = expr();
        if (size->kind != ND_NUM) error_at_here("expected constant expression");
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

/* --- 10. Node Creation --- */

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    switch (kind) {
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_ASSIGN:
        if (lhs->type) node->type = lhs->type;
        break;
    case ND_EQ: case ND_NE: case ND_LT: case ND_LE: case ND_AND: case ND_OR:
        node->type = new_type(INT, NULL);
        break;
    case ND_ADDR:
        if (lhs) node->type = new_type(PTR, lhs->type);
        break;
    case ND_DEREF:
        if (lhs && lhs->type && lhs->type->ty == PTR)
            node->type = lhs->type->ptr_to;
        break;
    case ND_STRING:
        node->type = new_type(PTR, new_type(CHAR, NULL));
        break;
    case ND_CALL:
        node->type = new_type(INT, NULL);
        break;
    case ND_POST_INCR:
    case ND_POST_DECR:
        if (lhs->type) node->type = lhs->type;
        break;
    default: break;
    }
    return node;
}

Node *new_typed_node(NodeKind kind, Node *lhs, Node *rhs, Type *type) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    node->type = type;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    Type *type = calloc(1, sizeof(Type));
    type->ty = INT;
    node->type = type;
    return node;
}

Node *new_node_char(char val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    Type *type = calloc(1, sizeof(Type));
    type->ty = CHAR;
    node->type = type;
    return node;
}

/* --- 11. Recursive Descent Parser --- */

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
        if ((node->type->ty == PTR || node->type->ty == ARRAY) && node->type->ptr_to->ty != VOID) {
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
        if (rhs->kind == ND_NUM) {
            rhs->val = -rhs->val;
            return rhs;
        }
        else return new_node(ND_SUB, new_node_num(0), rhs);
    }
    else if (consume("&")) return new_node(ND_ADDR, postfix(), NULL);
    else if (consume("*")) {
        Node *l = primary();
        if (l->type->ty != PTR && l->type->ty != ARRAY)
            error_at_here("dereference failed: not a pointer");
        if (l->type->ty == ARRAY) l = new_node(ND_ADDR, l, NULL);
        return new_node(ND_DEREF, l, NULL);
    }
    else if (consume_kind(TK_SIZEOF)) {
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
    }
    else if (consume("!")) return new_node(ND_EQ, postfix(), new_node_num(0));
    else if (consume("++")) {
        Node *n = postfix();
        return new_node(ND_ASSIGN, n, new_node(ND_ADD, n, new_node_num(1)));
    }
    else if (consume("--")) {
        Node *n = postfix();
        return new_node(ND_ASSIGN, n, new_node(ND_SUB, n, new_node_num(1)));
    }
    return postfix();
}

Node *primary() {
    if (consume("(")) {
        Node *n = expr();
        expect(")");
        return n;
    }
    if (token->kind == TK_NUM) {
        int val = expect_number();
        return new_node_num(val);
    }
    if (token->kind == TK_CHAR_CONST) {
        Token *t = token;
        token = token->next;
        return new_node_char((char)t->val);
    }
    if (token->kind == TK_STRING) {
        Token *t = token;
        token = token->next;

        StringLiteral *lit = calloc(1, sizeof(StringLiteral));
        lit->str = new_string(t->str->ptr, t->str->len);
        lit->offset = literal_count;
        literal_count += align(t->str->len + 1, 8);

        if (ext) {
            lit->next = ext->literals;
            ext->literals = lit;
        }
        Node *node = new_node(ND_STRING, NULL, NULL);
        node->val = lit->offset;
        return node;
    }
    if (token->kind == TK_IDENT) {
        Token *id = token;
        token = token->next;

        if (consume("(")) {
            Node *args = NULL;
            Node *last = NULL;
            if (!consume(")")) {
                for (;;) {
                    Node *a = expr();
                    if (!args) args = new_node(ND_UNNAMED, a, NULL);
                    else last->rhs = new_node(ND_UNNAMED, a, NULL);
                    last = (args && !last) ? args : (last ? last->rhs : NULL);
                    if (!consume(",")) break;
                }
                expect(")");
            }
            Node *call = calloc(1, sizeof(Node));
            call->kind = ND_CALL;
            call->name = id->str;
            call->lhs = args;
            call->type = new_type(INT, NULL);
            return call;
        }

        EnumVal *ev = find_enum_val(id);
        if (ev) return new_node_num(ev->val);

        LVar *lvar = find_lvar(id);
        if (lvar) {
            Node *node = new_node(ND_LVAR, NULL, NULL);
            node->offset = lvar->offset;
            node->type = lvar->type;
            return node;
        }

        GVar *gvar = find_gvar(id);
        if (gvar) {
            Node *node = new_node(ND_GVAR, NULL, NULL);
            node->name = id->str;
            node->type = gvar->type;
            return node;
        }

        new_gvar(id->str, new_type(INT, NULL));
        Node *node = new_node(ND_GVAR, NULL, NULL);
        node->name = id->str;
        node->type = new_type(INT, NULL);
        return node;
    }
    error_at_here("expected an expression");
    return NULL;
}

Node *postfix() {
    Node *node = primary();
    for (;;) {
        if (consume("[")) {
            Node *subscript = expr();
            int size = sizeof_type(node->type->ptr_to);
            if (node->type->ty == ARRAY)
                node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
            node = new_node(ND_DEREF, new_node(ND_ADD, node, new_node(ND_MUL, subscript, new_node_num(size))), NULL);
            expect("]");
            continue;
        }
        if (consume(".")) {
            Token *tok = consume_ident();
            if (!tok) error_at_token(tok, "expected identifier after '.'");
            if (node->type->ty != STRUCT) error_at_token(tok, "expected struct type");

            StructField *field = find_struct_field(tok, node->type->struct_type);
            if (!field) error_at_here("no such field");

            node = new_node(ND_ADDR, node, NULL);
            node = new_node(ND_ADD, node, new_node_num(field->offset));
            node->type = new_type(PTR, field->type);
            node = new_node(ND_DEREF, node, NULL);
            continue;
        }
        if (consume("->")) {
            Token *tok = consume_ident();
            if (!tok) error_at_token(tok, "expected identifier after '->'");
            if (node->type->ty != PTR) error_at_token(tok, "expected pointer type");
            if (node->type->ptr_to->ty != STRUCT) error_at_token(tok, "expected struct type");

            StructField *field = find_struct_field(tok, node->type->ptr_to->struct_type);
            if (!field) error_at_here("no such field");

            node = new_node(ND_ADD, node, new_node_num(field->offset));
            node->type = new_type(PTR, field->type);
            node = new_node(ND_DEREF, node, NULL);
            continue;
        }
        return node;
    }
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
        node = new_node(ND_BREAK, NULL, NULL); expect(";");
    } else if (consume_kind(TK_CONTINUE)) {
        node = new_node(ND_CONTINUE, NULL, NULL); expect(";");
    } else {
        node = expr();
        expect(";");
    }
    return node;
}

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
        go_to(res->tok); next(); expect("(");

        Token *cur = token;
        bool no_args = false;
        if (consume_kind(TK_VOID)) {
            if (consume(")")) no_args = true;
            else go_to(cur);
        }
        if (!consume(")") && !no_args) {
            for (;;) {
                if (consume("...")) { external->is_variadic = true; break; }
                ConsumeTypeRes *arg_res = consume_type();
                if (!arg_res) error_at_here("failed to parse argument");
                LVar *lvar = find_lvar(arg_res->tok);
                if (!lvar) lvar = new_lvar(arg_res->tok->str, arg_res->type);
                external->offsets[i++] = lvar->offset;
                if (!consume(",")) break;
            }
            expect(")");
        }
        if (consume(";")) { external->kind = EXT_FUNCDECL; return external; }
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

/* --- 12. Tokenizer Implementation --- */

static char *dup_range(char *p, int len) {
    char *s = calloc(1, len + 1);
    memcpy(s, p, len);
    s[len] = '\0';
    return s;
}

static Token *token_new(TokenKind kind, char *start, int len) {
    Token *t = calloc(1, sizeof(Token));
    t->kind = kind;
    if (start && len > 0) t->str = new_string(dup_range(start, len), len);
    else t->str = NULL;
    return t;
}

static Token *token_append(Token *tail, Token *t) {
    if (!tail) return t;
    tail->next = t;
    return t;
}

static const char *multi_puncts[] = {
    "==", "!=", "<=", ">=", "&&", "||", "->", "++", "--", "+=", "-=", "*=", "/=", NULL
};

Token *tokenize(char *p, bool eof) {
    Token head; head.next = NULL;
    Token *tail = &head;
    while (*p) {
        if (*p == ' ' || *p == '\t' || *p == '\r') { p++; continue; }
        if (*p == '\n') {
            Token *t = token_new(TK_NEWLINE, NULL, 0);
            tail = token_append(tail, t); p++; continue;
        }
        if (*p == '#') {
            char *q = p;
            while (*q && *q != '\n') q++;
            Token *t = token_new(TK_PREPROCESSOR, p, (int)(q - p));
            tail = token_append(tail, t); p = q; continue;
        }
        if (*p == '"') {
            char *q = p + 1;
            while (*q && *q != '"') {
                if (*q == '\\' && q[1]) q += 2; else q++;
            }
            if (*q != '"') error_at_here("unterminated string literal");
            Token *t = token_new(TK_STRING, p + 1, (int)(q - (p + 1)));
            tail = token_append(tail, t); p = q + 1; continue;
        }
        if (*p == '\'') {
            char *q = p + 1;
            int val = 0;
            if (*q == '\\') {
                q++;
                switch (*q) {
                case 'n': val = '\n'; break;
                case 't': val = '\t'; break;
                case '\\': val = '\\'; break;
                case '\'': val = '\''; break;
                case '0': val = '\0'; break;
                default: val = *q; break;
                }
                q++;
            } else { val = *q; q++; }
            if (*q != '\'') error_at_here("unterminated char constant");
            Token *t = token_new(TK_CHAR_CONST, NULL, 0);
            t->val = val;
            tail = token_append(tail, t); p = q + 1; continue;
        }
        if (isdigit((unsigned char)*p)) {
            char *q = p;
            long val = 0;
            while (isdigit((unsigned char)*q)) {
                val = val * 10 + (*q - '0');
                q++;
            }
            Token *t = token_new(TK_NUM, NULL, 0);
            t->val = (int)val;
            tail = token_append(tail, t); p = q; continue;
        }
        if (isalpha((unsigned char)*p) || *p == '_') {
            char *q = p + 1;
            while (isalnum((unsigned char)*q) || *q == '_') q++;
            int len = (int)(q - p);
            Token *t = token_new(TK_IDENT, p, len);
            if (len == 2 && strncmp(p, "if", 2) == 0) t->kind = TK_IF;
            else if (len == 4 && strncmp(p, "else", 4) == 0) t->kind = TK_ELSE;
            else if (len == 6 && strncmp(p, "return", 6) == 0) t->kind = TK_RETURN;
            else if (len == 5 && strncmp(p, "while", 5) == 0) t->kind = TK_WHILE;
            else if (len == 3 && strncmp(p, "for", 3) == 0) t->kind = TK_FOR;
            else if (len == 3 && strncmp(p, "int", 3) == 0) t->kind = TK_INT;
            else if (len == 4 && strncmp(p, "char", 4) == 0) t->kind = TK_CHAR;
            else if (len == 4 && strncmp(p, "enum", 4) == 0) t->kind = TK_ENUM;
            else if (len == 6 && strncmp(p, "struct", 6) == 0) t->kind = TK_STRUCT;
            else if (len == 5 && strncmp(p, "union", 5) == 0) t->kind = TK_UNION;
            else if (len == 4 && strncmp(p, "void", 4) == 0) t->kind = TK_VOID;
            else if (len == 4 && strncmp(p, "true", 4) == 0) { t->kind = TK_NUM; t->val = 1; }
            else if (len == 5 && strncmp(p, "false", 5) == 0) { t->kind = TK_NUM; t->val = 0; }
            else if (len == 6 && strncmp(p, "sizeof", 6) == 0) t->kind = TK_SIZEOF;
            else if (len == 7 && strncmp(p, "typedef", 7) == 0) t->kind = TK_TYPEDEF;
            else if (len == 6 && strncmp(p, "switch", 6) == 0) t->kind = TK_SWITCH;
            else if (len == 4 && strncmp(p, "case", 4) == 0) t->kind = TK_CASE;
            else if (len == 7 && strncmp(p, "default", 7) == 0) t->kind = TK_DEFAULT;
            else if (len == 5 && strncmp(p, "break", 5) == 0) t->kind = TK_BREAK;
            else if (len == 8 && strncmp(p, "continue", 8) == 0) t->kind = TK_CONTINUE;
            else if (len == 4 && strncmp(p, "bool", 4) == 0) t->kind = TK_BOOL;
            else if (len == 6 && strncmp(p, "extern", 6) == 0) t->kind = TK_EXTERN;
            else if (len == 9 && strncmp(p, "__builtin_va_list", 9) == 0) t->kind = TK_BUILTIN_VA_LIST;
            tail = token_append(tail, t); p = q; continue;
        }
        bool matched = false;
        for (const char **pp = multi_puncts; *pp; pp++) {
            int L = (int)strlen(*pp);
            if (strncmp(p, *pp, L) == 0) {
                Token *t = token_new(TK_RESERVED, p, L);
                tail = token_append(tail, t); p += L; matched = true; break;
            }
        }
        if (matched) continue;
        if (strchr("+-*/(){}[];:,<>=%&|!.~^?#", *p) || *p == '.' || *p == '&' || *p == '*') {
            Token *t = token_new(TK_RESERVED, p, 1);
            tail = token_append(tail, t); p++; continue;
        }
        error_at_here("tokenize: unknown character '%c'", *p);
    }
    if (eof) {
        Token *t = token_new(TK_EOF, NULL, 0);
        tail = token_append(tail, t);
    }
    return head.next;
}

/* --- 13. Preprocessor --- */

Macro *find_macro(String *str) {
    for (Macro *m = macro_head; m; m = m->next)
        if (str_equals(str, m->ident)) return m;
    return NULL;
}

bool once_file_contains(char *name) {
    for (OnceFile *f = once_files; f; f = f->next)
        if (strcmp(f->filename, name) == 0) return true;
    return false;
}

void once_file_add(char *name) {
    OnceFile *f = calloc(1, sizeof(OnceFile));
    f->filename = name;
    f->next = once_files;
    once_files = f;
}

Token *remove_newline(Token *tok) {
    Token *before = NULL;
    Token *head = tok;
    for (Token *t = tok; t; t = t->next) {
        if (t->kind == TK_NEWLINE) {
            if (before) before->next = t->next;
            else head = t->next;
        }
        else before = t;
    }
    return head;
}

Token *preprocess(Token *tok) {
    Token *before = NULL;
    Token *start = tok;
    for (Token *t = tok; t; t = t->next) {
        if (t->kind == TK_PREPROCESSOR) {
            if (startswith(t->str->ptr, "#include")) {
                char *curfile = filename;
                filename = calloc(1, 100);
                char *path = calloc(1, 200);

                t = t->next;
                assert_compiler(t->kind == TK_STRING);
                strncpy(filename, t->str->ptr, t->str->len);
                filename[t->str->len] = '\0';
                snprintf(path, 200, "%s/%s", dir_name, filename);

                char *header = read_file(path);
                Token *header_token = tokenize(header, false);
                header_token = preprocess(header_token);

                free(filename); filename = curfile;
                if (header_token) {
                    Token *header_token_end = header_token;
                    while (header_token_end->next) header_token_end = header_token_end->next;
                    if (before) before->next = header_token; else start = header_token;
                    before = header_token_end;
                }
                free(path); continue;
            }
            if (startswith(t->str->ptr, "#ifdef")) {
                while (t && !startswith(t->str->ptr, "#else") && !startswith(t->str->ptr, "#endif"))
                    t = t->next;
                if (t && startswith(t->str->ptr, "#else")) {
                    t = t->next;
                    if (before) before->next = t; else start = t;
                    while (t && !startswith(t->str->ptr, "#endif")) { before = t; t = t->next; }
                } else { assert_compiler(t && startswith(t->str->ptr, "#endif")); }
                continue;
            }
            if (startswith(t->str->ptr, "#pragma")) {
                t = t->next;
                if (str_chr_equals(t->str, "once")) {
                    if (once_file_contains(filename)) return NULL;
                    once_file_add(filename);
                }
                continue;
            }
            if (startswith(t->str->ptr, "#define")) {
                t = t->next;
                assert_compiler(t->kind == TK_IDENT);
                Macro *m = calloc(1, sizeof(Macro));
                m->ident = t->str;
                t = t->next;
                m->replace = t;
                while (t->next && t->next->kind != TK_NEWLINE) t = t->next;
                Token *last = t;
                t = t->next;
                last->next = NULL;
                m->next = macro_head;
                macro_head = m;
                continue;
            }
        } else {
            if (t->kind == TK_IDENT) {
                Macro *m = find_macro(t->str);
                if (!m) {
                    if (before) before->next = t; else start = t;
                    before = t; continue;
                }
                Token *replace = m->replace;
                while (replace) {
                    Token *newt = calloc(1, sizeof(Token));
                    newt->kind = replace->kind;
                    newt->val = replace->val;
                    newt->str = replace->str;
                    if (before) before->next = newt; else start = newt;
                    before = newt;
                    replace = replace->next;
                }
                continue;
            }
            if (before) before->next = t; else start = t;
            before = t;
        }
    }
    start = remove_newline(start);
    return start;
}

/* --- 14. File I/O --- */

char *read_file(char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) error_at_here("cannot open file: %s", path);
    struct stat st;
    if (stat(path, &st) == 0) {
        size_t sz = (size_t)st.st_size;
        char *buf = calloc(1, sz + 1);
        if (fread(buf, 1, sz, f) != sz) {}
        fclose(f); return buf;
    } else {
        size_t cap = 4096;
        char *buf = calloc(1, cap);
        size_t len = 0;
        int c;
        while ((c = fgetc(f)) != EOF) {
            if (len + 1 >= cap) { cap *= 2; buf = realloc(buf, cap); }
            buf[len++] = (char)c;
        }
        buf[len] = '\0';
        fclose(f); return buf;
    }
}

char *get_dir(char *path) {
    char *p = strrchr(path, '/');
    if (!p) p = strrchr(path, '\\');
    if (!p) {
        char *s = calloc(1, 3);
        strcpy(s, ".");
        return s;
    }
    int len = (int)(p - path);
    char *dir = calloc(1, len + 1);
    memcpy(dir, path, len);
    dir[len] = '\0';
    return dir;
}

/* --- 15. Main --- */

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <file.c>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];
    user_input = read_file(path);
    dir_name = get_dir(path);
    filename = calloc(1, 256);
    strncpy(filename, path, 255);

    Token *tok = tokenize(user_input, true);
    if (!tok) {
        fprintf(stderr, "tokenize returned NULL\n");
        return 1;
    }

    tok = preprocess(tok);
    if (!tok) {
        fprintf(stderr, "preprocess returned NULL (maybe #pragma once)\n");
        return 0;
    }

    token = tok;

    External *first_ext = NULL;
    External *last_ext = NULL;

    while (!at_eof()) {
        External *e = external();
        if (!first_ext) {
            first_ext = last_ext = e;
        } else {
            /* Fixed: Use the 'next' pointer added to struct External */
            last_ext->next = e;
            last_ext = e;
        }
        if (token && token->kind == TK_EOF) break;
    }

    printf("Parsing finished successfully. Literals: %d\n", literal_count);
    return 0;
}
