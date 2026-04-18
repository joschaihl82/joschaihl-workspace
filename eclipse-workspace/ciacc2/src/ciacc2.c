/* * 5cc.c - Amalgamated Source
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================= */
/* 5cc.h                                    */
/* ========================================================================= */

typedef enum {
    TK_RESERVED,
    TK_NUM,
    TK_IDENT,
    TK_STR,
    TK_EOF,
} TokenKind;

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NEG,
    ND_NUM,
    ND_AND,
    ND_MOD,
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN,
    ND_VAR,
    ND_EXPR_STMT,
    ND_RETURN,
    ND_BLOCK,
    ND_IF,
    ND_FOR,
    ND_ADDR,
    ND_DEREF,
    ND_FNCALL,
    ND_COMMA,
    ND_DOTS,  // struct or union
    ND_STMT_EXPR,
} NodeKind;

typedef enum {
    TY_INT,
    TY_CHAR,
    TY_LONG,
    TY_SHORT,
    TY_VOID,
    TY_PTR,
    TY_FN,
    TY_ARRAY,
    TY_STRUCT,
    TY_UNION
} TypeKind;

typedef struct Token Token;
typedef struct Node Node;
typedef struct Obj Obj;
typedef struct Type Type;

struct Token {
    TokenKind kind;
    Token *next;
    char *loc;
    int len;

    int64_t val;
    char *string;
};

struct Obj {
    Obj *next;
    char *name;
    Type *type;

    // for Lvar
    bool is_lvar;
    int offset;

    // for Fn
    bool is_func;
    bool is_def;
    Obj *locals;
    Obj *params;
    Node *body;
    int stack_size;

    char *init_data;

    bool is_member;
};

struct Node {
    NodeKind kind;
    Type *type;
    Node *next;
    Token *tok;

    Node *lhs;
    Node *rhs;

    Node *cond;
    Node *then;
    Node *_else;

    Node *init;
    Node *inc;

    int val;

    Obj *var;
    Node *body;
    char *fn_name;
    Node *args;

    Obj *member;
};

struct Type {
    TypeKind kind;
    Type *base;
    int size;
    int align;

    int array_len;

    Obj *members;

    Token *name;
    Type *return_type;  // for func
    Type *params;
    Type *next;
};

Token *Tokenize(char *p);
Obj *ParseToken(Token *tok);
void GenCode(Obj *prog, FILE *out);

void AddType(Node *node);
bool IsTypeInteger(Type *ty);
Type *NewTypePTR2(Type *base);
Type *NewTypeFn(Type *return_type);
Type *NewTypeArrayOf(Type *base, int len);
Type *CopyType(Type *ty);

extern Type *ty_int;
extern Type *ty_char;
extern Type *ty_long;
extern Type *ty_short;
extern Type *ty_void;

extern char *UserInput;
extern char *InputPath;

bool IsStrSame(char *A, char *B);
void Error(char *fmt, ...);
void ErrorAt(char *loc, char *fmt, ...);
void ErrorToken(Token *tok, char *fmt, ...);
void Debug(char *fmt, ...);
void PrintToken(Token *tok);
void PrintObjFn(Obj *obj);

int align_to(int n, int align);

/* ========================================================================= */
/* util.c                                   */
/* ========================================================================= */

bool IsStrSame(char *A, char *B) {
    return (strncmp(A, B, strlen(B)) == 0);
}

//===================================================================
// Error
//===================================================================
void Error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void verror_at(char *file, char *input, char *loc, char *msg, va_list ap) {
    char *line = loc;
    while (input < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n' &&*end != '\0')
        end++;

    int line_num = 1;
    for (char *p = input; p < line; p++) {
        if (*p == '\n')
            line_num++;
    }

    int indent = fprintf(stderr, "%s:%d:", file, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
}

void ErrorAt(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(InputPath, UserInput, loc, fmt, ap);
    exit(1);
}

void ErrorToken(Token *tok, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(InputPath, UserInput, tok->loc, fmt, ap);
    exit(1);
}

//===================================================================
// Debug
//===================================================================
void Debug(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    return;
}

void PrintToken(Token *tok) {
    for (Token *t = tok; t; t = t->next) {
        switch (t->kind) {
        case TK_NUM:
            Debug("Number");
            continue;
        case TK_RESERVED:
            Debug("Reserved");
            continue;
        case TK_IDENT:
            Debug("Ident");
            continue;
        case TK_STR:
            Debug("String");
            Debug("-: %s", t->string);
            continue;
        case TK_EOF:
            Debug("End Of File");
            return;
        }
    }
}

void PrintNode(Node *node) {
    #define DEBUG_NODE(token) case token:Debug(#token);continue;
    for (Node *n = node; n; n = n->next) {
        switch (n->kind) {
        DEBUG_NODE(ND_ADD);
        DEBUG_NODE(ND_SUB);
        DEBUG_NODE(ND_MUL);
        DEBUG_NODE(ND_DIV);
        DEBUG_NODE(ND_NEG);
        DEBUG_NODE(ND_NUM);
        DEBUG_NODE(ND_EQ); // ==
        DEBUG_NODE(ND_NE); // !=
        DEBUG_NODE(ND_LT); // <
        DEBUG_NODE(ND_LE); // <=
        DEBUG_NODE(ND_ASSIGN);
        DEBUG_NODE(ND_VAR);

        DEBUG_NODE(ND_RETURN);

        DEBUG_NODE(ND_IF);
        DEBUG_NODE(ND_FOR);
        DEBUG_NODE(ND_ADDR);
        DEBUG_NODE(ND_DEREF);
        DEBUG_NODE(ND_FNCALL);
        DEBUG_NODE(ND_COMMA);
        DEBUG_NODE(ND_DOTS);  // struct or union
        DEBUG_NODE(ND_STMT_EXPR);
        DEBUG_NODE(ND_EXPR_STMT);
        case ND_BLOCK:
            Debug("ND_BLOCK");
            PrintNode(n->body);
            continue;
        }
    }
    #undef DEBUG_NODE
}

void PrintObjFn(Obj *obj) {
     for (Obj *fn = obj; fn; fn = fn->next) {
        if (!fn->is_func || !fn->is_def) continue;
        Debug("FUNCTION");
        PrintNode(fn->body);
     }
}

/* ========================================================================= */
/* type.c                                   */
/* ========================================================================= */

Type *ty_int = &(Type){.kind = TY_INT, .size = 4, .align = 4};
Type *ty_char = &(Type){.kind = TY_CHAR, .size = 1, .align = 1};
Type *ty_long = &(Type){.kind = TY_LONG, .size = 8, .align = 8};
Type *ty_short = &(Type){.kind = TY_SHORT, .size = 2, .align = 2};
Type *ty_void = &(Type){.kind = TY_VOID, .size = 1, .align = 1};

Type *NewType(TypeKind kind, int size, int align) {
    Type *new = calloc(1, sizeof(Type));
    new->kind = kind;
    new->size = size;
    new->align = align;
    return new;
}

Type *NewTypePTR2(Type *base) {
    Type *new = NewType(TY_PTR, 8, 8);
    new->base = base;
    return new;
}

Type *NewTypeFn(Type *return_type) {
    Type *new = calloc(1, sizeof(Type));
    new->kind = TY_FN;
    new->return_type = return_type;
    return new;
}

Type *NewTypeArrayOf(Type *base, int len) {
    Type *new = NewType(TY_ARRAY, base->size * len, base->align);
    new->base = base;
    new->array_len = len;
    return new;
}

bool IsTypeInteger(Type *ty) {
    return ty->kind == TY_INT || ty->kind == TY_CHAR || ty->kind == TY_LONG || ty->kind == TY_SHORT;
}

Type *CopyType(Type *ty) {
    Type *ret = calloc(1, sizeof(Type));
    *ret = *ty;
    return ret;
}

void AddType(Node *node) {
    if (!node || node->type)
        return;
    AddType(node->lhs);
    AddType(node->rhs);
    AddType(node->_else);
    AddType(node->cond);
    AddType(node->inc);
    AddType(node->init);
    AddType(node->then);

    for (Node *n = node->body; n; n = n->next)
        AddType(n);

    for (Node *n = node->args; n; n = n->next)
        AddType(n);

    switch (node->kind) {
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_NEG:
    case ND_MOD:
    case ND_AND:
        node->type = node->lhs->type;
        return;
    case ND_COMMA:
        node->type = node->rhs->type;
        return;
    case ND_ASSIGN:
        if (node->lhs->type->kind == TY_ARRAY)
            ErrorToken(node->tok, "not an lvalue");
        node->type = node->lhs->type;
        return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_NUM:
    case ND_FNCALL:
        node->type = ty_long;
        return;
    case ND_DOTS:
        node->type = node->member->type;
        return;
    case ND_VAR:
        node->type = node->var->type;
        return;

    case ND_ADDR:
        if (node->lhs->type->kind == TY_ARRAY)
            node->type = NewTypePTR2(node->lhs->type->base);
        else
            node->type = NewTypePTR2(node->lhs->type);
        return;
    case ND_DEREF:
        if (!node->lhs->type->base)
            ErrorToken(node->tok, "invalid pointer dereference");
        if (node->lhs->type->base->kind == TY_VOID)
            ErrorToken(node->tok, "dereferencing a void pointer");
        node->type = node->lhs->type->base;
        return;

    case ND_STMT_EXPR:
        if (node->body) {
            Node *stmt = node->body;
            while (stmt->next)
                stmt = stmt->next;
            if (stmt->kind == ND_EXPR_STMT) {
                node->type = stmt->lhs->type;
            return;
            }
        }
        ErrorToken(node->tok, "statement expression returning void is not supported");
        return;
    }
}

/* ========================================================================= */
/* tokenizer.c                                */
/* ========================================================================= */

static bool is_al(char c) {
    return isalpha(c) ||
           (c == '_');
}

static bool is_alnum(char c) {
    return is_al(c) || ('0' <= c && c <= '9');
}

static bool IsStrReserved(char *A, char *reserved) {
    return IsStrSame(A, reserved) && !is_alnum(A[strlen(reserved)]);
}

static Token *NewToken(TokenKind TK, char *start, char *end) {
    Token *new = calloc(1, sizeof(Token));
    new->kind = TK;
    new->loc = start;
    new->len = end - start;
    return new;
}

static Token *NewTokenReserved(char **start) {
    Token *new = NULL;
    static struct {
        char *word;
        int len;
    } symbol[] = {
        {"<=", 2}, {">=", 2}, {"==", 2}, {"!=", 2}, {"->", 2},
        {"-", 1}, {"+", 1}, {"/", 1}, {"*", 1}, {"%", 1},
        {"<", 1}, {">", 1}, {"(", 1}, {")", 1},
        {";", 1}, {"=", 1}, {"{", 1}, {"}", 1},
        {"&", 1}, {",", 1}, {"[", 1}, {"]", 1},
        {".", 1},
        {NULL, 0},
    };

    static struct {
        char *word;
        int len;
    } keyword[] = {
        {"return", 6}, {"while", 5}, {"else", 4}, {"for", 3},
        {"sizeof", 6}, {"short", 5}, {"char", 4}, {"int", 3},
        {"struct", 6}, {"union", 5}, {"long", 4}, {"if", 2},
        {"typedef", 7}, {"void", 4},
        {NULL, 0},
    };

    for (int i = 0; symbol[i].word; i++) {
        if (IsStrSame(*start, symbol[i].word)) {
            new = NewToken(TK_RESERVED, *start, *start + symbol[i].len);
            *start = *start + symbol[i].len;
            return new;
        }
    }
    for (int i = 0; keyword[i].word; i++) {
        if (IsStrReserved(*start, keyword[i].word)) {
            new = NewToken(TK_RESERVED, *start, *start + keyword[i].len);
            *start = *start + keyword[i].len;
            return new;
        }
    }
    return new;
}

static char ReadEscapedLiteral(char *p) {
    switch (*p) {
    case 'a': return '\a';
    case 'b': return '\b';
    case 't': return '\t';
    case 'n': return '\n';
    case 'v': return '\v';
    case 'f': return '\f';
    case 'r': return '\r';
    case 'e': return 27;
    default: return *p;
    }
}

static char *EndOfStrLiteral(char *p) {
    char *start = p;
    for (; *p != '\"'; p++) {
        if (*p == '\n' || *p == '\0') {
            ErrorAt(start, "unclosed string literal");
        }
        if (*p == '\\') p++;
    }
    return p;
}

static Token *ReadStrLiteral(char **start) {
    char *end = EndOfStrLiteral(*start + 1);
    char *string = calloc(end - *start, sizeof(char));
    int len  = 0;

    for (char *p = *start + 1; p < end; p++) {
        if (*p == '\\') {
            string[len++] = ReadEscapedLiteral(p + 1);
            p++;
        } else {
            string[len++] = *p;
        }
    }
    Token *tok = NewToken(TK_STR, *start, end + 1);
    tok->string = string;
    *start = end + 1;
    return tok;
}

Token *Tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (isdigit(*p)) {
            cur = cur->next = NewToken(TK_NUM, p, p);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        if (IsStrSame(p, "//")) {
            p += 2;
            while (*p != '\n') p++;
            continue;
        }

        if (IsStrSame(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q) ErrorAt(p, "unclosed comment");
            p = q + 2;
            continue;
        }

        Token *tok = NewTokenReserved(&p);
        if (tok) {
            cur = cur->next = tok;
            continue;
        }

        if (*p == '"') {
            cur = cur->next = ReadStrLiteral(&p);
            continue;
        }

        if (is_al(*p)) {
            char *start = p;
            for (; is_alnum(*p);) p++;  // len(ident_name)
            cur = cur->next = NewToken(TK_IDENT, start, p);
            continue;
        }

         ErrorAt(p, "Can't tokenize!");
    }

    cur = cur->next = NewToken(TK_EOF, p, p);
    return head.next;
}

/* ========================================================================= */
/* parser.c                                  */
/* ========================================================================= */

//===================================================================
static bool IsTokenEqual(Token *tok, char *op) {
    return strlen(op) == tok->len && !strncmp(tok->loc, op, tok->len);
    // return tok->kind == TK_RESERVED && memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

static Token *SkipToken(Token *tok, char *s) {
  if (!IsTokenEqual(tok, s))
    ErrorToken(tok, "expected '%s'", s);
  return tok->next;
}

static bool ConsumeToken(Token **rest, Token *tok, char *op) {
    if (IsTokenEqual(tok, op)) {
        *rest = tok->next;
        return true;
    }
    *rest = tok;
    return false;
}

static bool IsTokenAtEof(Token *tok) {
  return tok->kind == TK_EOF;
}

static char *NewUniqueName(void) {
    static int count = 0;
    char *name = calloc(sizeof(char), 16);
    sprintf(name, ".L.L.%d", count++);
    return name;
}

static char *GetTokenIdent(Token *tok) {
    if (tok->kind != TK_IDENT)
        ErrorToken(tok, "This is not ident");
    return strndup(tok->loc, tok->len);
}

static int GetTokenNum(Token *tok) {
    if (tok->kind != TK_NUM)
        ErrorToken(tok, "This is not number");
    return tok->val;
}

//===================================================================
typedef struct VarScope VarScope;
typedef struct TagScope TagScope;
typedef struct Scope Scope;

struct VarScope {
    VarScope *next;
    Obj *var;
    char *name;
    Type *type_def;
};

struct TagScope {
    TagScope *next;
    char *name;
    Type *type;
};

struct Scope {
    Scope *next;
    VarScope *vars;
    TagScope *tags;
};

typedef struct {
    bool is_typedef;
} VarAttr;

static Scope *scope = &(Scope){};

static void EnterScope() {
    Scope *new = calloc(1, sizeof(Scope));
    new->next = scope;
    scope = new;
}

static void LeaveScope() {
    scope = scope->next;
}

static VarScope *PushScope(char *name) {
    VarScope *new = calloc(1, sizeof(VarScope));
    new->name = name;
    new->next = scope->vars;
    scope->vars = new;
    return new;
}

static void PushTagScope(char *name, Type *type) {
    TagScope *new = calloc(1, sizeof(TagScope));
    new->name = name;
    new->type = type;
    new->next = scope->tags;
    scope->tags = new;
}

static Type *FindTagScope(Token *tok) {
    for (Scope *sc = scope; sc; sc = sc->next)
        for (TagScope *tsc = sc->tags; tsc; tsc = tsc->next)
            if (IsTokenEqual(tok, tsc->name))
                return tsc->type;
    return NULL;
}

static VarScope *FindVarScope(Token *tok) {
    for (Scope *sc = scope; sc; sc = sc->next)
        for (VarScope *vsc = sc->vars; vsc; vsc = vsc->next)
            if (IsTokenEqual(tok, vsc->name))
                return vsc;
    return NULL;
}

static Type *FindTypedef(Token *tok) {
    if (tok->kind == TK_IDENT) {
        VarScope *sc = FindVarScope(tok);
        if (sc)
            return sc->type_def;
    }
    return NULL;
}

static bool IsTokenType(Token *tok) {
    char *TY[] = {"int", "char", "long", "short", "struct", "union", "void", "typedef", NULL};
    for (int i = 0; TY[i]; i++)
        if (IsTokenEqual(tok, TY[i]))
            return true;
    return FindTypedef(tok);
}

//===================================================================
static Obj *locals;
static Obj *globals;

static Obj *NewObj(char *name, Type *type) {
    Obj *new = calloc(1, sizeof(Obj));
    new->name = name;
    new->type = type;
    return new;
}

static Obj *NewObjMember(char *name, Type *type) {
    Obj *new = NewObj(name, type);
    new->is_member = true;
    return new;
}

static Obj *NewObjVar(char *name, Type *type) {
    Obj *new = NewObj(name, type);
    PushScope(name)->var = new;
    return new;
}

static Obj *NewObjLVar(char *name, Type *type) {
    Obj *new = NewObjVar(name, type);
    new->is_lvar = true;
    new->next = locals;
    locals = new;
    return new;
}

static Obj *NewObjGVar(char *name, Type *type) {
    Obj *new = NewObjVar(name, type);
    new->next = globals;
    globals = new;
    return new;
}

static Obj *FindObjMember(Type *type, Token *tok) {
    for (Obj *mem = type->members; mem; mem = mem->next)
        if (IsTokenEqual(tok, mem->name))
            return mem;
    ErrorToken(tok, "No such member");
}

static Obj *NewObjGVarAnon(Type *type) {
    return NewObjGVar(NewUniqueName(), type);
}

static Obj *NewObjString(Token *tok) {
    Obj *new = NewObjGVarAnon(NewTypeArrayOf(ty_char, strlen(tok->string) + 1));
    new->init_data = tok->string;
    return new;
}

//===================================================================
static Node *NewNodeKind(NodeKind kind, Token *tok) {
    Node *new = calloc(1, sizeof(Node));
    new->kind = kind;
    new->tok = tok;
    return new;
}

static Node *NewNodeBinary(NodeKind kind, Token *tok, Node *lhs, Node *rhs) {
    Node *new = NewNodeKind(kind, tok);
    new->lhs = lhs;
    new->rhs = rhs;
    return new;
}

static Node *NewNodeNum(Token *tok, int64_t val) {
    Node *new = NewNodeKind(ND_NUM, tok);
    new->val = val;
    return new;
}

static Node *NewNodeUnary(NodeKind kind, Token *tok, Node *lhs) {
    Node *new = NewNodeKind(kind, tok);
    new->lhs = lhs;
    return new;
}

static Node *NewNodeVar(Token *tok, Obj *var) {
    Node *new = NewNodeKind(ND_VAR, tok);
    new->var = var;
    new->type = var->type;
    return new;
}

//===================================================================
static Node *primary(Token **rest, Token *tok);
static Node *postfix(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *expr(Token **rest, Token *tok);
static Node *expr_stmt(Token **rest, Token *tok);
static Node *compound_stmt(Token **rest, Token *tok);
static Node *stmt(Token **rest, Token *tok);

static Type *params(Token **rest, Token *tok, Type *ty);
static Type *declspec(Token **rest, Token *tok, VarAttr *attr);
static Type *type_suffix(Token **rest, Token *tok, Type *ty);
static Type *declarator(Token **rest, Token *tok, Type *ty);
static Node *declaration(Token **rest, Token *tok, Type *base);
static void create_param_lvars(Type *param);
static Type *struct_declspec(Token **rest, Token *tok);
static void struct_members(Token **rest, Token *tok, Type *type);
static Type *union_declspec(Token **rest, Token *tok);
static void parse_typedef(Token **rest, Token *tok, Type *base);
//===================================================================

static Type *declspec(Token **rest, Token *tok, VarAttr *attr) {
    enum {
        VOID  = 1 << 0,
        CHAR  = 1 << 2,
        SHORT = 1 << 4,
        INT   = 1 << 6,
        LONG  = 1 << 8,
        OTHER = 1 << 10,
    };

    Type *ty = ty_int;
    int counter = 0;

    while (IsTokenType(tok)) {
        if (IsTokenEqual(tok, "typedef")) {
            if (!attr)
                ErrorToken(tok, "storage class specifier is not allowed in this context");
            attr->is_typedef = true;
            tok = tok->next;
            continue;
        }
        Type *ty2 = FindTypedef(tok);
        if (IsTokenEqual(tok, "struct") || IsTokenEqual(tok, "union") || ty2) {
            if (counter)
                break;
            if (IsTokenEqual(tok, "struct")) ty = struct_declspec(&tok, tok->next);
            else if (IsTokenEqual(tok, "union")) ty = union_declspec(&tok, tok->next);
            else {
                ty = ty2;
                tok = tok->next;
            }
            counter += OTHER;
            continue;
        }

        if (IsTokenEqual(tok, "int")) counter += INT;
        else if (IsTokenEqual(tok, "char")) counter += CHAR;
        else if (IsTokenEqual(tok, "long")) counter += LONG;
        else if (IsTokenEqual(tok, "short")) counter += SHORT;
        else if (IsTokenEqual(tok, "void")) counter += VOID;

        switch (counter) {
        case VOID:
            ty = ty_void;
            break;
        case CHAR:
            ty = ty_char;
            break;
        case SHORT:
        case SHORT + INT:
            ty = ty_short;
            break;
        case INT:
            ty = ty_int;
            break;
        case LONG:
        case LONG + INT:
        case LONG + LONG:
        case LONG + LONG + INT:
            ty = ty_long;
            break;
        default:
            ErrorToken(tok, "invalid type");
        }
        tok = tok->next;
    }
    *rest = tok;
    return ty;
}

static Type *struion_declspec(Token **rest, Token *tok) {
    Token *tag = NULL;
    if (tok->kind == TK_IDENT) {
        tag = tok;
        tok = tok->next;
    }

    if (tag && !IsTokenEqual(tok, "{")) {
        Type *type = FindTagScope(tag);
        if (!type) ErrorToken(tok, "undefined struct");
        *rest = tok;
        return type;
    }

    Type *type = calloc(1, sizeof(Type));
    type->kind = TY_STRUCT;
    struct_members(rest, tok->next, type);
    type->align = 1;

    if (tag)
        PushTagScope(GetTokenIdent(tag), type);
    return type;
}

static Type *struct_declspec(Token **rest, Token *tok) {
    Type *type = struion_declspec(rest, tok);
    type->kind = TY_STRUCT;

    int offset = 0;
    for (Obj *mem = type->members; mem; mem = mem->next) {
        offset = align_to(offset, mem->type->align);
        mem->offset = offset;
        offset += mem->type->size;
        if (type->align < mem->type->align)
            type->align = mem->type->align;
    }
    type->size = align_to(offset, type->align);

    return type;
}

static Type *union_declspec(Token **rest, Token *tok) {
    Type *type = struion_declspec(rest, tok);
    type->kind = TY_UNION;

    for (Obj *mem = type->members; mem; mem = mem->next) {
        if (type->align < mem->type->align)
            type->align = mem->type->align;
        if (type->size < mem->type->size)
            type->size = mem->type->size;
    }
    type->size = align_to(type->size, type->align);
    return type;
}

static void struct_members(Token **rest, Token *tok, Type *type) {
    Obj head = {};
    Obj *cur = &head;
    while (!IsTokenEqual(tok, "}")) {
        Type *base = declspec(&tok, tok, NULL);

        for (int i = 0; !ConsumeToken(&tok, tok, ";"); i++) {
            if (i > 0)
                tok = SkipToken(tok, ",");

            Type *ty = declarator(&tok, tok, base);
            cur = cur->next = NewObjMember(GetTokenIdent(ty->name), ty);
        }
    }
    *rest = tok->next;
    type->members = head.next;
}

static Type *params(Token **rest, Token *tok, Type *ty) {
    Type head = {};
    Type *cur = &head;

    while (!IsTokenEqual(tok, ")")) {
        if (cur != &head)
            tok = SkipToken(tok, ",");
        Type *basety = declspec(&tok, tok, NULL);
        Type *ty = declarator(&tok, tok, basety);
        cur = cur->next = CopyType(ty);
    }

    ty = NewTypeFn(ty);
    ty->params = head.next;
    *rest = tok->next;
    return ty;
}

static Type *type_suffix(Token **rest, Token *tok, Type *ty) {
    if (IsTokenEqual(tok, "("))
        return params(rest, tok->next, ty);

    if (IsTokenEqual(tok, "[")) {
        int len = GetTokenNum(tok->next);
        tok = SkipToken(tok->next->next, "]");
        ty = type_suffix(rest, tok, ty);
        return NewTypeArrayOf(ty, len);
    }
    *rest = tok;
    return ty;
}

static Type *declarator(Token **rest, Token *tok, Type *ty) {
    while (ConsumeToken(&tok, tok, "*"))
        ty = NewTypePTR2(ty);

     if (IsTokenEqual(tok, "(")) {
        Token *start = tok;
        Type dummy = {};
        declarator(&tok, start->next, &dummy);
        tok = SkipToken(tok, ")");
        ty = type_suffix(rest, tok, ty);
        return declarator(&tok, start->next, ty);
    }

    if (tok->kind != TK_IDENT)
        ErrorToken(tok, "expected a variable name");



    ty = type_suffix(rest, tok->next, ty);
    ty->name = tok;

    return ty;
}

static Node *declaration(Token **rest, Token *tok, Type *base_type) {
    Node head = {};
    Node *cur = &head;

    for (int i = 0; !IsTokenEqual(tok, ";"); i++) {
        if (i > 0)
            tok = SkipToken(tok, ",");

        Type *ty = declarator(&tok, tok, base_type);
        if (ty->kind == TY_VOID) ErrorToken(tok, "variable declared void");
        Obj *var = NewObjLVar(GetTokenIdent(ty->name), ty);

        if (!IsTokenEqual(tok, "="))
            continue;

        Node *lhs = NewNodeVar(tok, var);
        Node *rhs = assign(&tok, tok->next);
        Node *node = NewNodeBinary(ND_ASSIGN, tok, lhs, rhs);
        cur = cur->next = NewNodeUnary(ND_EXPR_STMT, tok, node);
    }

    Node *node = NewNodeKind(ND_BLOCK, tok);
    node->body = head.next;
    *rest = tok->next;
    return node;
}

static void create_param_lvars(Type *param) {
    if (param) {
        create_param_lvars(param->next);
        NewObjLVar(GetTokenIdent(param->name), param);
    }
}

static void parse_typedef(Token **rest, Token *tok, Type *base) {
    for (int i = 0; !ConsumeToken(&tok, tok, ";"); i++) {
        if (i > 0)
            tok = SkipToken(tok, ",");
        Type *ty = declarator(&tok, tok, base);
        PushScope(GetTokenIdent(ty->name))->type_def = ty;
    }
    *rest = tok;
}

static Type *abstract_declarator(Token **rest, Token *tok, Type *ty) {
    while (ConsumeToken(&tok, tok, "*"))
        ty = NewTypePTR2(ty);

     if (IsTokenEqual(tok, "(")) {
        Token *start = tok;
        Type dummy = {};
        abstract_declarator(&tok, start->next, &dummy);
        tok = SkipToken(tok, ")");
        ty = type_suffix(rest, tok, ty);
        return abstract_declarator(&tok, start->next, ty);
    }

    return type_suffix(rest, tok, ty);
}

static Type *type_name(Token **rest, Token *tok) {
    Type *ty = declspec(&tok, tok, NULL);
    return abstract_declarator(rest, tok, ty);
}

static Node *stmt(Token **rest, Token *tok) {
    if (IsTokenEqual(tok, "return")) {
        Node *node = NewNodeUnary(ND_RETURN, tok, expr(&tok, tok->next));
        *rest = SkipToken(tok, ";");
        return node;
    }
    if (IsTokenEqual(tok, "{")) {
        return compound_stmt(rest, tok->next);
    }
    if (IsTokenEqual(tok, "if")) {
        Node *node = NewNodeKind(ND_IF, tok);
        tok = SkipToken(tok->next, "(");
        node->cond = expr(&tok, tok);
        tok = SkipToken(tok, ")");
        node->then = stmt(&tok, tok);
        if (IsTokenEqual(tok, "else"))
            node->_else = stmt(&tok, tok->next);
        *rest = tok;
        return node;
    }
    if (IsTokenEqual(tok, "for")) {
        Node *node = NewNodeKind(ND_FOR, tok);
        tok = SkipToken(tok->next, "(");

        EnterScope();
        if (!ConsumeToken(&tok, tok, ";")) {
            if (IsTokenType(tok)) {
                Type *base = declspec(&tok, tok, NULL);
                node->init = declaration(&tok, tok, base);
            } else
                node->init = expr_stmt(&tok, tok);
        }
        if (!ConsumeToken(&tok, tok, ";")) {
            node->cond = expr(&tok, tok);
            tok = SkipToken(tok, ";");
        }
        if (!ConsumeToken(&tok, tok, ")")) {
            node->inc = expr(&tok, tok);
            tok = SkipToken(tok, ")");
        }
        node->then = stmt(&tok, tok);
        LeaveScope();
        *rest = tok;
        return node;
    }
    if (IsTokenEqual(tok, "while")) {
        Node *node = NewNodeKind(ND_FOR, tok);
        tok = SkipToken(tok->next, "(");
        node->cond = expr(&tok, tok);
        tok = SkipToken(tok, ")");
        node->then = stmt(&tok, tok);
        *rest = tok;
        return node;
    }
    return expr_stmt(rest, tok);
}

static Node *compound_stmt(Token **rest, Token *tok) {
    Node head  = {};
    Node *cur = &head;
    EnterScope();
    while (!IsTokenEqual(tok, "}")) {
        if (IsTokenType(tok)) {
            VarAttr attr = {};
            Type *base = declspec(&tok, tok, &attr);

            if (attr.is_typedef) {
                parse_typedef(&tok, tok, base);
                continue;
            }

            cur = cur->next = declaration(&tok, tok, base);
        } else {
            cur = cur->next = stmt(&tok, tok);
        }
        AddType(cur);
    }

    LeaveScope();
    Node *node = NewNodeKind(ND_BLOCK, tok);
    node->body = head.next;
    *rest = tok->next;
    return node;
}

static Node *expr_stmt(Token **rest, Token *tok) {
    if (IsTokenEqual(tok, ";")) {
        *rest = SkipToken(tok, ";");
        return NewNodeKind(ND_BLOCK, tok);
    }
    Node *node = NewNodeUnary(ND_EXPR_STMT, tok, expr(&tok, tok));
   *rest = SkipToken(tok, ";");
    return node;
}

static Node *expr(Token **rest, Token *tok) {
    Node *node = assign(&tok, tok);
    if (IsTokenEqual(tok, ",")) {
        return NewNodeBinary(ND_COMMA, tok, node, expr(rest, tok->next));
    }
    *rest = tok;
    return node;
}

static Node *assign(Token **rest, Token *tok) {
    Node *node = equality(&tok, tok);

    if (IsTokenEqual(tok, "=")) {
        node = NewNodeBinary(ND_ASSIGN, tok, node, assign(&tok, tok->next));
    }
    *rest = tok;
    return node;
}

static Node *equality(Token **rest, Token *tok) {
    Node *node = relational(&tok, tok);

    for (;;) {
        if (IsTokenEqual(tok, "==")) {
            node = NewNodeBinary(ND_EQ, tok, node, add(&tok, tok->next));
            continue;
        }
        if (IsTokenEqual(tok, "!=")) {
            node = NewNodeBinary(ND_NE, tok, node, add(&tok, tok->next));
            continue;
        }
        *rest = tok;
        return node;
    }
}

static Node *relational(Token **rest, Token *tok) {
    Node *node  = add(&tok, tok);

    for (;;) {
        if (IsTokenEqual(tok, "<=")) {
            node = NewNodeBinary(ND_LE, tok, node, add(&tok, tok->next));
            continue;
        }
        if (IsTokenEqual(tok, "<")) {
            node = NewNodeBinary(ND_LT, tok, node, add(&tok, tok->next));
            continue;
        }
        if (IsTokenEqual(tok, ">=")) {
            node = NewNodeBinary(ND_LE, tok, add(&tok, tok->next), node);
            continue;
        }
        if (IsTokenEqual(tok, ">")) {
            node = NewNodeBinary(ND_LT, tok, add(&tok, tok->next), node);
            continue;
        }
        *rest = tok;
        return node;
    }
}

static Node *NewNodeAdd(Token *tok, Node *lhs, Node *rhs) {
    AddType(lhs);
    AddType(rhs);

    if (IsTypeInteger(lhs->type) && IsTypeInteger(rhs->type))
        return NewNodeBinary(ND_ADD, tok, lhs, rhs);

    if (lhs->type->base && rhs->type->base)
        ErrorToken(tok, "can't add ptr to ptr.");

    if (!lhs->type->base && rhs->type->base)
        return NewNodeAdd(tok, rhs, lhs);

    rhs = NewNodeBinary(ND_MUL, tok, rhs, NewNodeNum(tok, lhs->type->base->size));
    return NewNodeBinary(ND_ADD, tok, lhs, rhs);
}

static Node *NewNodeSub(Token *tok, Node *lhs, Node *rhs) {
    AddType(lhs);
    AddType(rhs);

    if (IsTypeInteger(lhs->type) && IsTypeInteger(rhs->type))
        return NewNodeBinary(ND_SUB, tok, lhs, rhs);

    if (lhs->type->base && rhs->type->base) {
        Node *node = NewNodeBinary(ND_SUB, tok, lhs, rhs);
        node->type = ty_int;
        return NewNodeBinary(ND_DIV, tok, node, NewNodeNum(tok, lhs->type->base->size));
    }

    if (lhs->type->base && IsTypeInteger(rhs->type)) {
        rhs = NewNodeBinary(ND_MUL, tok, rhs, NewNodeNum(tok, lhs->type->base->size));
        AddType(rhs);
        Node *node = NewNodeBinary(ND_SUB, tok, lhs, rhs);
        node->type = lhs->type;
        return node;
    }
    ErrorToken(tok ,"invalid operands");
}

static Node *add(Token **rest, Token *tok) {
    Node *node = mul(&tok, tok);

    for (;;) {
        if (IsTokenEqual(tok, "+")) {
            node = NewNodeAdd(tok, node, mul(&tok, tok->next));
            continue;
        }
        if (IsTokenEqual(tok, "-")) {
            node = NewNodeSub(tok, node, mul(&tok, tok->next));
            continue;
        }
        *rest = tok;
        return node;
    }
}

static Node *mul(Token **rest, Token *tok) {
    Node *node = unary(&tok, tok);

    for (;;) {
        if (IsTokenEqual(tok, "*")) {
            node = NewNodeBinary(ND_MUL, tok, node, unary(&tok, tok->next));
            continue;
        }
        if (IsTokenEqual(tok, "/")) {
            node = NewNodeBinary(ND_DIV, tok, node, unary(&tok, tok->next));
            continue;
        }
        if (IsTokenEqual(tok, "%")) {
            node = NewNodeBinary(ND_MOD, tok, node, unary(&tok, tok->next));
            continue;
        }
        if (IsTokenEqual(tok, "&")) {
            node = NewNodeBinary(ND_AND, tok, node, unary(&tok, tok->next));
            continue;
        }
        *rest = tok;
        return node;
    }
}

static Node *unary(Token **rest, Token *tok) {
    if (IsTokenEqual(tok, "+")) {
        return unary(rest, tok->next);
    }
    if (IsTokenEqual(tok, "-")) {
        return NewNodeUnary(ND_NEG, tok, unary(rest, tok->next));
    }
    if (IsTokenEqual(tok, "*")) {
        return NewNodeUnary(ND_DEREF, tok, unary(rest, tok->next));
    }
    if (IsTokenEqual(tok, "&")) {
        return NewNodeUnary(ND_ADDR, tok, unary(rest, tok->next));
    }
    return postfix(rest, tok);
}

static Node *struct_ref(Token *tok, Node *lhs) {
    AddType(lhs);
    if (lhs->type->kind != TY_STRUCT && lhs->type->kind != TY_UNION) ErrorToken(lhs->tok, "not a struct nor an union");
    Node *node = NewNodeUnary(ND_DOTS, tok, lhs);
    node->member = FindObjMember(lhs->type, tok->next);
    return node;
}

static Node *postfix(Token **rest, Token *tok) {
    Node *node = primary(&tok, tok);
    for (;;) {
        if (IsTokenEqual(tok, "[")) { // a[b] => *(a + b)
            Node *index = expr(&tok, tok->next);
            tok = SkipToken(tok, "]");
            node = NewNodeUnary(ND_DEREF, tok, NewNodeAdd(tok, node, index));
            continue;
        }
        if (IsTokenEqual(tok, ".")) {
            node = struct_ref(tok, node);
            tok = tok->next->next;
            continue;
        }
        if (IsTokenEqual(tok, "->")) { // a->b => (*a).b
            node = NewNodeUnary(ND_DEREF, tok, node);
            node = struct_ref(tok, node);
            tok = tok->next->next;
            continue;
        }
        *rest = tok;
        return node;
    }
}

static Node *fncall(Token **rest, Token *tok) {
    Node *node = NewNodeKind(ND_FNCALL, tok);
    node->fn_name = strndup(tok->loc, tok->len);
    tok = tok->next->next;

    Node head = {};
    Node *cur = &head;

    while (!IsTokenEqual(tok, ")")) {
        cur = cur->next = assign(&tok, tok);
        if (!IsTokenEqual(tok, ")"))
            tok = SkipToken(tok, ",");
    }

    *rest = SkipToken(tok, ")");
    node->args = head.next;
    return node;
}

static Node *primary(Token **rest, Token *tok) {
    if (IsTokenEqual(tok, "(")) {
        if (IsTokenEqual(tok->next, "{")) {
            Node *node = NewNodeKind(ND_STMT_EXPR, tok);
            node->body =  compound_stmt(&tok, tok->next->next)->body;
            *rest = SkipToken(tok, ")");
            return node;
        } else {
            Node *node = expr(&tok, tok->next);
            *rest = SkipToken(tok, ")");
            return node;
        }
    }
    if (IsTokenEqual(tok, "sizeof")) {
        Token *start = tok;
        if (IsTokenEqual(tok->next, "(") && IsTokenType(tok->next->next)) {
            Type *base = type_name(&tok, tok->next->next);
            *rest = SkipToken(tok, ")");

            return NewNodeNum(start, base->size);
        }
        Node *node = unary(rest, tok->next);
        AddType(node);
        return NewNodeNum(tok, node->type->size);
    }

    if (tok->kind == TK_IDENT) {
        if (IsTokenEqual(tok->next, "(")) {
            return fncall(rest, tok);
        } else {
            VarScope *sc = FindVarScope(tok);
            if (!sc || !sc->var)
                ErrorToken(tok, "undeclared valuable");
            *rest = tok->next;
            return NewNodeVar(tok, sc->var);
        }
    }

    if (tok->kind == TK_NUM) {
        Node *node = NewNodeNum(tok, tok->val);
        *rest = tok->next;
        return node;
    }

    if (tok->kind == TK_STR) {
        Obj *str = NewObjString(tok);
        *rest = tok->next;
        return NewNodeVar(tok, str);
    }
    ErrorToken(tok, "Something is wrong");
}

static Token *Function(Token *tok, Type *base) {
    Type *ty = declarator(&tok, tok, base);

    Obj *fn = NewObjGVar(GetTokenIdent(ty->name), ty);
    fn->is_func = true;
    fn->is_def = !ConsumeToken(&tok, tok, ";");

    if (!fn->is_def)
        return tok;

    locals = NULL;
    EnterScope();
    create_param_lvars(ty->params);
    fn->params = locals;

    tok = SkipToken(tok, "{");
    fn->body = compound_stmt(&tok, tok);
    fn->locals = locals;

    LeaveScope();
    return tok;
}

static Token *Gvar(Token *tok, Type *base) {
    for (int i = 0; !ConsumeToken(&tok, tok, ";"); i++) {
        if (i > 0) tok = SkipToken(tok, ",");

        Type *ty = declarator(&tok, tok, base);
        NewObjGVar(GetTokenIdent(ty->name), ty);
    }
    return tok;
}

static bool IsFunc(Token *tok) {
    if (IsTokenEqual(tok, ";")) return false;

    Type tmp = {};
    Type *ty = declarator(&tok, tok, &tmp);
    return ty->kind == TY_FN;
}

Obj *ParseToken(Token *tok) {
    globals = NULL;

    while (!IsTokenAtEof(tok)) {
        VarAttr attr = {};
        Type *base = declspec(&tok, tok, &attr);

        if (attr.is_typedef) {
            parse_typedef(&tok, tok, base);
            continue;
        }
        if (IsFunc(tok)) {
            tok = Function(tok, base);
            continue;
        }
        tok = Gvar(tok, base);
    }
    return globals;
}

/* ========================================================================= */
/* codegen.c                                  */
/* ========================================================================= */

static int depth;

static char *argreg8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
static char *argreg16[] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
static char *argreg32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
static char *argreg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

static Obj *current_fn;
static FILE *output_file;

static void println(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(output_file, fmt, ap);
    va_end(ap);
    fprintf(output_file, "\n");
    return ;
}

static void comment(char *msg) {
    putchar('#');
    println(msg);
}

static void push(void) {
  println("\tpush %%rax");
  depth++;
}

static void pop(char *arg) {
  println("\tpop %s", arg);
  depth--;
}

static int count() {
    static int i = 1;
    return i++;
}

int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

static void load(Type *type) {
    if (type->kind == TY_ARRAY || type->kind == TY_STRUCT || type->kind == TY_UNION)
        return;
    if (type->size == 1)
        println("\tmovsbq (%%rax), %%rax");
    else if (type->size == 2)
        println("\tmovswq (%%rax), %%rax");
    else if (type->size == 4)
        println("\tmovsxd (%%rax), %%rax");
    else
        println("\tmov (%%rax), %%rax");
    return;
}

static void store(Type *type) {
    pop("%rdi");
    if (type->kind == TY_STRUCT || type->kind == TY_UNION) {
        for (int i = 0; i < type->size; i++) {
            println("\tmov %d(%%rax), %%r8b", i);
            println("\tmov %%r8b, %d(%%rdi)", i);
        }
        return;
    }
    if (type->size == 1)
        println("\tmov %%al, (%%rdi)");
    else if (type->size == 2)
        println("\tmov %%ax, (%%rdi)");
    else if (type->size == 4)
        println("\tmov %%eax, (%%rdi)");
    else
        println("\tmov %%rax, (%%rdi)");
}

static void gen_stmt(Node *node);
static void gen_expr(Node *node);

static void gen_addr(Node *node) {
    switch (node->kind) {
    case ND_VAR:
        if (node->var->is_lvar) {
            println("\tlea %d(%%rbp), %%rax", node->var->offset);
        } else {
            println("\tlea %s(%%rip), %%rax", node->var->name);
        }
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        return;
    case ND_DOTS:
        gen_addr(node->lhs);
        println("\tadd $%d, %%rax", node->member->offset);
        return;
    case ND_COMMA:
        gen_expr(node->lhs);
        gen_addr(node->rhs);
        return;
    }
    Error("not an lvalue");
}

static void gen_expr(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        println("\tmov $%ld, %%rax", node->val);
        return;
    case ND_NEG:
        gen_expr(node->lhs);
        println("\tneg %%rax");
        return;
    case ND_VAR:
        gen_addr(node);
        load(node->type);
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        store(node->type);
        return;
    case ND_ADDR:
        gen_addr(node->lhs);
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        load(node->type);
        return;
    case ND_COMMA:
        gen_expr(node->lhs);
        gen_expr(node->rhs);
        return;
    case ND_DOTS:
        gen_addr(node);
        load(node->type);
        return;
    case ND_STMT_EXPR:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
        return;
    case ND_FNCALL:{
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr(arg);
            push();
            nargs++;
        }

        for (int i = nargs - 1; i >= 0; i--)
            pop(argreg64[i]);

        println("\tmov $0, %%rax");
        println("\tcall %s", node->fn_name);
        return;
    }
    }

    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    pop("%rdi");

    char *ax, *di;

    if (node->lhs->type->kind == TY_LONG || node->lhs->type->base) {
        ax = "%rax";
        di = "%rdi";
    } else {
        ax = "%eax";
        di = "%edi";
    }

    switch (node->kind) {
    case ND_ADD:
        println("\tadd %s, %s", di, ax);
        return;
    case ND_SUB:
        println("\tsub %s, %s", di, ax);
        return;
    case ND_MUL:
        println("\timul %s, %s", di, ax);
        return;
    case ND_DIV:
        if (node->lhs->type->size == 8)
            println("\tcqo");
        else
            println("\tcdq");
        println("\tidiv %s", di);
        return;
    case ND_MOD:
        if (node->lhs->type->size == 8) {
            println("\tcqo");
            println("\tidiv %s", di);
            println("\tmov %%rdx, %s", ax);
        } else {
            println("\tcdq");
            println("\tidiv %s", di);
            println("\tmov %%edx, %s", ax);
        }
        return;
    case ND_AND:
        println("and(%s, %s)", di, ax);
        return;
    case ND_EQ:
    case ND_NE:
    case ND_LE:
    case ND_LT:
        println("\tcmp %s, %s", di, ax);
        if (node->kind == ND_EQ) {
            println("\tsete %%al");
        } else if (node->kind == ND_NE) {
            println("\tsetne %%al");
        } else if (node->kind == ND_LT) {
            println("\tsetl %%al");
        } else if (node->kind == ND_LE) {
            println("\tsetle %%al");
        }
        println("\tmovzb %%al, %%rax");
        return;
    }

    Error("invalid expression");
}

static void gen_stmt(Node *node) {
    switch (node->kind) {
    case ND_EXPR_STMT:
        gen_expr(node->lhs);
        return;
    case ND_RETURN:
        gen_expr(node->lhs);
        println("\tjmp .L.return.%s", current_fn->name);
        return;
    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
        return;
    case ND_IF:{
        int c = count();
        gen_expr(node->cond);
        println("\tcmp $0, %%rax");
        println("\tje  .L.else.%d", c);
        gen_stmt(node->then);
        println("\tjmp .L.end.%d", c);
        println(".L.else.%d:", c);
        if (node->_else)
            gen_stmt(node->_else);
        println(".L.end.%d:", c);
        return;
    }
    case ND_FOR:{
        int c = count();
        if (node->init)
            gen_stmt(node->init);
        println(".L.begin.%d:", c);
        if (node->cond) {
            gen_expr(node->cond);
            println("\tcmp $0, %%rax");
            println("\tje  .L.end.%d", c);
        }

        gen_stmt(node->then);
        if (node->inc)
            gen_expr(node->inc);
        println("\tjmp .L.begin.%d", c);
        println(".L.end.%d:", c);
        return;
    }
    }

    Error("invalid expression");
}

static void InitLVarOffset(Obj *func) {
    int offset = 0;
    for (Obj *lv = func->locals; lv; lv = lv->next) {
        offset += lv->type->size;
        offset = align_to(offset, lv->type->align);
        lv->offset = -offset;
    }
    func->stack_size = align_to(offset, 16);
}

static void store_param(int r, int offset, int size) {
    switch (size) {
    case 1:
        println("\tmov %s, %d(%%rbp)", argreg8[r], offset);
        return;
    case 2:
        println("\tmov %s, %d(%%rbp)", argreg16[r], offset);
        return;
    case 4:
        println("\tmov %s, %d(%%rbp)", argreg32[r], offset);
        return;
    case 8:
        println("\tmov %s, %d(%%rbp)", argreg64[r], offset);
        return;
  }
  Error("something is wrong");
}

static void EmitData(Obj* gvar) {
    for (Obj *var = gvar; var; var = var->next) {
        if (var->is_func) continue;

        println(".data");
        println("\t.global %s", var->name);
        println("%s:", var->name);

        if (var->init_data) {
            for (int i = 0; i < var->type->array_len; i++)
                println("\t.byte %d", var->init_data[i]);
        } else {
            println("\t.zero %d", var->type->size);
        }
    }
}

static void EmitFunc(Obj *func) {
    for (Obj *fn = func; fn; fn = fn->next) {
        if (!fn->is_func || !fn->is_def) continue;
        assert(fn->is_func);
        InitLVarOffset(fn);
        current_fn = fn;
        println(".text");
        println("\t.globl %s", fn->name);

        println("%s:", fn->name);

        println("\tpush %%rbp");
        println("\tmov %%rsp, %%rbp");
        println("\tsub $%d, %%rsp", fn->stack_size);

        int i = 0;
        for (Obj *var = fn->params; var; var = var->next) {
            store_param(i++, var->offset, var->type->size);
        }

        for (Node *n = fn->body; n; n = n->next) {
            gen_stmt(n);
            assert(depth == 0);
        }
        println(".L.return.%s:", fn->name);
        println("\tmov %%rbp, %%rsp");
        println("\tpop %%rbp");
        println("\tret");
    }
}

void GenCode(Obj *prog, FILE *out) {
    output_file = out;

    EmitData(prog);
    EmitFunc(prog);
}

/* ========================================================================= */
/* main.c                                   */
/* ========================================================================= */

static char *opt_o;
static char *opt_c;
static bool opt_D;

char *InputPath;
char *UserInput;

static void usage(int status) {
    fprintf(stderr, "5cc [ -o <path> || -c <cmd>] <file>\n");
    exit(status);
}

static char *ReadFile(char *path) {
    FILE *fp;
    if (!strcmp(path, "-")) {
        fp = stdin;
    } else {
        fp = fopen(path, "r");
        if (!fp) Error("can't open file %s: %s", path, strerror(errno));
    }


    char *buf;
    size_t buflen;
    FILE *out = open_memstream(&buf, &buflen);

    for (;;) {
        char buf2[4096];
        int n = fread(buf2, 1, sizeof(buf2), fp);
        if (n == 0)
            break;
        fwrite(buf2, 1, n, out);
    }

    if (fp != stdin)
        fclose(fp);

    fflush(out);
    if (buflen == 0 || buf[buflen - 1] != '\n')
        fputc('\n', out);
    fputc('\0', out);
    fclose(out);
    return buf;
}

void Compile(char *code, FILE *out) {
    Token *token = Tokenize(code);
    if (opt_D) PrintToken(token);
    Obj *node = ParseToken(token);
    if (opt_D) PrintObjFn(node);
    GenCode(node, out);
}

static void ParseArgs(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            usage(0);
        }
        if (!strcmp(argv[i], "-o")) {
            if (!argv[++i]) usage(1);
            opt_o = argv[i];
            continue;
        } else if (IsStrSame(argv[i], "-o")) {
            opt_o = argv[i] + 2;
            continue;
        }
        if (!strcmp(argv[i], "-c")) {
            if (!argv[++i]) usage(1);
            opt_c = argv[i];
            InputPath = "<arg>:";
            continue;
        }
        if (!strcmp(argv[i], "-D")) {
            opt_D = true;
            continue;
        }
        if (argv[i][0] == '-' && argv[i][1] != '\0')
            Error("unknown argument: %s", argv[i]);

        InputPath = argv[i];
    }
    if (!IsStrSame(InputPath, "<arg>:") && opt_c) Error("invaild argument");
    if (!InputPath) Error("no input files");

}

static FILE *OpenFile(char *path) {
    if (!path || strcmp(path, "-") == 0)
        return stdout;

    FILE *out = fopen(path, "w");
    if (!out)
        Error("cannot open output file: %s: %s", path, strerror(errno));
    return out;
}

static char *ReadCode() {
    if (opt_c)
        return opt_c;
    return ReadFile(InputPath);
}

int main(int argc, char **argv) {
    if (argc < 2)
        usage(1);
    ParseArgs(argc, argv);
    UserInput = ReadCode();
    Compile(UserInput, OpenFile(opt_o));

    return 0;
}
