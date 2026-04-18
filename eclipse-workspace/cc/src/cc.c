#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// from baricc.h (typedefs, structs, and external declarations)
typedef struct Node Node;
typedef struct Token Token;
typedef struct Type Type;
typedef struct Member Member;
typedef struct LVar LVar;
typedef struct Tag Tag;
typedef struct StringToken StringToken;
typedef struct Define Define;
typedef struct EnumVar EnumVar;

typedef enum {
    TK_RESERVED,  // any single character symbol
    TK_NUM,       // primray integer
    TK_EOF,       // EOF token
    TK_IDENT,     // identification of variable, function
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_TYPE,  // type annotation

    TK_SIZEOF,
    TK_STRING,
    TK_STRUCT,
    TK_TYPEDEF,
    TK_ENUM,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_TRUE,
    TK_FALSE,
    TK_NULL,
    TK_SEEKSET,
    TK_SEEKCUR,
    TK_SEEKEND,
    TK_ERRNO,
    TK_STDERR,
} TokenKind;

/*
token is the smallest meaningful unit of characters in given program.
it doesn't have semantics itself, but only when kind is tk_num, it can have
value of the number
*/
struct Token {
    TokenKind kind;  // type of the token
    Token* next;     // next token. we use linkedlist instead of array.
    int val;         // actual value when kind == tk_num
    char* str;       // pointer of where the token starts in the given program.
    int len;  // length of the token. with len and str, we can get the token
              // label.
};

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_ASSIGN,

    ND_LVAR,      // local variable
    ND_GVAR_DEF,  // global variable
    ND_GVAR,      // to use global variable
    ND_RETURN,
    ND_IF,
    ND_ELSE,
    ND_WHILE,
    ND_FOR,
    ND_FOR_LEFT,
    ND_FOR_RIGHT,

    ND_BLOCK,
    ND_FUNC_CALL,
    ND_FUNC_DEF,
    ND_ADDR,
    ND_DEREF,
    ND_STRING,
    ND_MEMBER,  // todo: better to name nd_member_access
    ND_PRE_INC,
    ND_SUF_INC,
    ND_PRE_DEC,
    ND_SUF_DEC,
    ND_SHL,
    ND_SHR,
    ND_LOGAND,
    ND_LOGOR,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_BITNOT,
    ND_NOT,
    ND_COND,  // for ? : operator
    ND_LABEL,
    ND_GOTO,
    ND_SWITCH,
    ND_CASE,
    ND_DEFAULT_CASE,
    ND_CAST,
    ND_NULL_STMT,
    ND_STMT_EXPR,  // gcc statement expression
} NodeKind;

/*
node is the smallest meaningful unit of abstract syntax tree.
*/
struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    Node* next;
    Node** block;      // used when kind == nd_block. array of node
    Type* ptr_to;      // used when kind == nd_deref, nd_addr. what's the type of
                       // the pointer points to.
    Node* init;  // array initializer(this should be moved to another field)
    int val;     // used when kind == nd_num
    int offset;  // used when kind == nd_lvar
    char* funcname;  // used when kind == nd_func_call, or nd_func_def
    Node** args;     // used when kind == nd_func_def
    Type* type;      // used when kind == nd_lvar
    char* varname;   // used when kind == nd_gvar, nd_lvar
    int varsize;     // used when kind == nd_gvar, nd_lvar byte. todo: varsize and
                     // offset can be unified.
    StringToken* string;  // used when kind == nd_string
    LVar* var;            // used when kind == nd_lvar, nd_gvar_def
    Member* member;       // used when kind == nd_member
    Node* next_case;  // used when kind == nd_case. todo: this should be merged
                      // with next field of node.
};

typedef enum {
    INT,
    CHAR,
    PTR,
    ARRAY,
    STRUCT,
    VOID,
    BOOL,
    ENUM,
    TYPEDEF,
} TypeKind;

struct Type {
    TypeKind ty;
    Type* ptr_to;
    size_t size;
    int array_size;  // used when kind == array
    Member* members;
    bool incomplete;
};

/*
member is definition of members of a struct.
ex:
struct example {
    int a;  // --> member{next: b's member struct, ty: int, name: "a", offset:
            // size of int(=4) }
    char* b;  // --> member{next: null, ty: ptr of char, name: "b", offset: size
              // of ptr of char(=8) }
}
*/
struct Member {
    Member* next;  // next member definition. we use linkedlist instaed of array
    Type* ty;      // type of the member
    char* name;    // the member's name
    int offset;  // offset of the member from the starting point of base struct
};

/*
lvar(local variable) has information about variables
*/
struct LVar {
    LVar* next;  // next lvar, linkedlist
    char* name;  // name of the variable
    int len;     // length of the name
    int offset;  // offset from rbp
    Type* type;
    Node* init;  // initialization value. only for global variables, and static
                 // local variables.
};

/*
tag has information about struct tag
*/
struct Tag {
    Tag* next;
    Type* type;
    char* name;
    int len;
    bool incomplete;
};

/*
stringtoken is a struct for string literal
*/
struct StringToken {
    StringToken* next;
    char* value;
    int index;
};

/*
define is a struct for temporary
*/
struct Define {
    Token* ident;
    Type* type;
};

/*
enumvar is a struct for enum value
*/
struct EnumVar {
    EnumVar* next;
    char* name;
    int value;
};

extern Token* token;
extern char* user_input;
extern char* filename;

extern LVar* locals[100];
extern LVar* globals[100];
extern int cur_scope_depth;
extern StringToken* strings;
extern Tag* tags;
extern EnumVar* enum_vars;
extern Node* current_switch;
extern Node* code[1000];

// tokenize functions
Token* tokenize();
void advance_token();
Token* new_token(TokenKind kind, Token* cur, char* str, int len);
bool consume(char* op);
Token* consume_kind(TokenKind kind);
bool check(char* op);
bool check_kind(TokenKind kind);
void expect(char* op);
Token* expect_kind(TokenKind kind);
int expect_number();
bool at_eof();
char* get_token_kind_name(TokenKind kind);

// parse functions
Node* func_def(Define* def);
void program();
Node* stmt();
Node* expr();
Node* assign();
Node* define_arg();
Node* variable(Token* tok);
LVar* find_lvar(Token* tok);
Type* define_struct();
Type* find_tag_struct(Token* tok);
Tag* push_tag(Token* tok, Type* type, bool complete);
Member* find_member_struct(Token* tok, Type* type);
LVar* find_variable(Token* tok);
Node* find_enum_var(Token* tok);
Tag* find_or_register_tag(Token* tok);
Node* struct_ref(Node* node);
bool define_typedef();
Type* define_enum();
Type* int_type();
Type* char_type();
Type* ptr_type();
Node* ptr_conversion(Node* node, Node* right);
Node* initializer(Type* type);
void initializer_helper(Type* type, Node* init, int* i);
Node* convert_predefined_keyword_to_num();
Define* read_define();
Define* read_define_head();
void read_define_suffix(Define* def);
Node* primary_suffix(Token* tok);
Node* block();
Node* primary();
Node* unary();
Node* mul();
Node* add();
Node* shift();
Node* relational();
Node* equality();
Node* bitand();
Node* bitor();
Node* bitxor();
Node* logand();
Node* logor();
Node* conditional();
Node* new_unary(NodeKind kind, Node* lhs);
Node* new_binary(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node(NodeKind kind);
Node* new_num(int val);
Node* new_string(StringToken* str);
Node* define_variable(Define* def, LVar** lvars);
LVar* push_lvar(Token* tok, Type* type);
LVar* push_gvar(Token* tok, Type* type);
LVar* find_gvar(Token* tok);
void define_enum_member(int* num);

Type* get_type(Node* node);
int get_type_size(Type* type);

// generate codes fron nodes
void gen(Node* node);
void gen_val(Node* node);
void gen_load(Type* type);
void gen_store(Type* type);

// util
void error_at0(char* loc, char* fmt);
void error_at1(char* loc, char* fmt, char* val);
void error_at2(char* loc, char* fmt, char* val1, char* val2);
void error0(char* fmt);
void error1(char* fmt, char* val);
void error2(char* fmt, char* val1, char* val2);
void print_token(Token* token);
void print_type(Type* type);
void print_node(Node* node);
bool startswith(char* p, char* q);
char* read_file(char* path);
Token* read_char_literal(Token* cur, char* start);
char get_escape_char(char c);
int align_to(int n, int align);

// from util.c
char *read_file(char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        error2("cannot open %s: %s", path, strerror(errno));
    }

    // check filesize
    if (fseek(fp, 0, SEEK_END) == -1) {
        error2("%s: fseek: %s", path, strerror(errno));
    }
    size_t size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) == -1) {
        error2("%s: fseek: %s", path, strerror(errno));
    }

    // read file
    char *buf = calloc(1, size + 2);
    fread(buf, size, 1, fp);

    // if the file doesn't have line break at the end of it, assert one. this is
    // for making eof ops easier
    if (size == 0 || buf[size - 1] != '\n') {
        buf[size++] = '\n';
    }

    buf[size] = '\n';
    fclose(fp);
    return buf;
}

// for debugging
// todo(critical): now stderr is not available.. so when we do self-copmile, we
// cannot use fprintf at all.
void print_token(Token *tok) {
    if (tok == NULL) {
        fprintf(stderr, "[debug] token is null\n");
    } else {
        char name[100] = {0};
        memcpy(name, tok->str, tok->len);
        fprintf(stderr, "[debug] token: %2d %s\n", tok->kind, name);
    }
}

void print_type(Type *type) {
    if (type == NULL) {
        fprintf(stderr, "[debug] type is null");
    } else {
        char *name;
        switch (type->ty) {
            case INT:
                name = "int";
                break;
            case CHAR:
                name = "char";
                break;
            case PTR:
                name = "ptr";
                break;
            case ARRAY:
                name = "array";
                break;
            case STRUCT:
                name = "struct";
                break;
            case VOID:
                name = "void";
                break;
            case BOOL:
                name = "bool";
                break;
            case ENUM:
                name = "enum";
                break;
            case TYPEDEF:
                name = "typedef";
                break;
            default:
                name = "unknown";
                break;
        }
        fprintf(stderr, "[debug] type: %s\n", name);
        if (type->ptr_to) {
            fprintf(stderr, "[debug]   ptr_to: ");
            print_type(type->ptr_to);
        }
        if (type->members) {
            for (Member *m = type->members; m; m = m->next) {
                fprintf(stderr, "[debug]   member: %s offset: %d\n", m->name, m->offset);
                print_type(m->ty);
            }
        }
    }
}

// only works for top-level statement node.
void print_node(Node *node) {
    if (node == NULL) {
        fprintf(stderr, "[debug] node is null\n");
        return;
    }

    char *name;
    switch (node->kind) {
        case ND_ADD:
            name = "nd_add";
            break;
        case ND_SUB:
            name = "nd_sub";
            break;
        case ND_MUL:
            name = "nd_mul";
            break;
        case ND_DIV:
            name = "nd_div";
            break;
        case ND_NUM:
            name = "nd_num";
            break;
        case ND_EQ:
            name = "nd_eq";
            break;
        case ND_NE:
            name = "nd_ne";
            break;
        case ND_LT:
            name = "nd_lt";
            break;
        case ND_LE:
            name = "nd_le";
            break;
        case ND_ASSIGN:
            name = "nd_assign";
            break;
        case ND_LVAR:
            name = "nd_lvar";
            break;
        case ND_GVAR_DEF:
            name = "nd_gvar_def";
            break;
        case ND_GVAR:
            name = "nd_gvar";
            break;
        case ND_RETURN:
            name = "nd_return";
            break;
        case ND_IF:
            name = "nd_if";
            break;
        case ND_ELSE:
            name = "nd_else";
            break;
        case ND_WHILE:
            name = "nd_while";
            break;
        case ND_FOR:
            name = "nd_for";
            break;
        case ND_FOR_LEFT:
            name = "nd_for_left";
            break;
        case ND_FOR_RIGHT:
            name = "nd_for_right";
            break;
        case ND_BLOCK:
            name = "nd_block";
            break;
        case ND_FUNC_CALL:
            name = "nd_func_call";
            break;
        case ND_FUNC_DEF:
            name = "nd_func_def";
            break;
        case ND_ADDR:
            name = "nd_addr";
            break;
        case ND_DEREF:
            name = "nd_deref";
            break;
        case ND_STRING:
            name = "nd_string";
            break;
        case ND_MEMBER:
            name = "nd_member";
            break;
        case ND_PRE_INC:
            name = "nd_pre_inc";
            break;
        case ND_SUF_INC:
            name = "nd_suf_inc";
            break;
        case ND_PRE_DEC:
            name = "nd_pre_dec";
            break;
        case ND_SUF_DEC:
            name = "nd_suf_dec";
            break;
        case ND_SHL:
            name = "nd_shl";
            break;
        case ND_SHR:
            name = "nd_shr";
            break;
        case ND_LOGAND:
            name = "nd_logand";
            break;
        case ND_LOGOR:
            name = "nd_logor";
            break;
        case ND_BITAND:
            name = "nd_bitand";
            break;
        case ND_BITOR:
            name = "nd_bitor";
            break;
        case ND_BITXOR:
            name = "nd_bitxor";
            break;
        case ND_BITNOT:
            name = "nd_bitnot";
            break;
        case ND_NOT:
            name = "nd_not";
            break;
        case ND_COND:
            name = "nd_cond";
            break;
        case ND_LABEL:
            name = "nd_label";
            break;
        case ND_GOTO:
            name = "nd_goto";
            break;
        case ND_SWITCH:
            name = "nd_switch";
            break;
        case ND_CASE:
            name = "nd_case";
            break;
        case ND_DEFAULT_CASE:
            name = "nd_default_case";
            break;
        case ND_CAST:
            name = "nd_cast";
            break;
        case ND_NULL_STMT:
            name = "nd_null_stmt";
            break;
        case ND_STMT_EXPR:
            name = "nd_stmt_expr";
            break;
        default:
            name = "unknown";
            break;
    }

    fprintf(stderr, "[debug] node: %s\n", name);
}

// report an error and exit.
// char *user_input;

// reports an error position to stderr.
// loc: position that error happens.
void verror_at(char *loc, char *fmt, va_list ap) {
    // find the head pos of the line
    char *line = loc;
    while (user_input < line && line[-1] != '\n') line--;

    // end is : tail pos of the line
    char *end = loc;
    while (*end != '\n') end++;

    // find which line num the line is.
    int line_num = 1;
    char *p;
    for (p = user_input; p < line; p++)
        if (*p == '\n') line_num++;

    // print the line with line number and filename
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // print the pointer that shows which column the error happens.
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");  // print pos spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    exit(1);
}

void error_at0(char *loc, char *fmt) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}

void error_at1(char *loc, char *fmt, char *val) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}

void error_at2(char *loc, char *fmt, char *val1, char *val2) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}

// report an error and exit.
void error0(char *fmt) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    exit(1);
}

void error1(char *fmt, char *val) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    exit(1);
}

void error2(char *fmt, char *val1, char *val2) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    exit(1);
}

char* get_token_kind_name(TokenKind kind) {
    switch (kind) {
        case TK_RESERVED:
            return "tk_reserved";
        case TK_NUM:
            return "tk_num";
        case TK_EOF:
            return "tk_eof";
        case TK_IDENT:
            return "tk_ident";
        case TK_RETURN:
            return "tk_return";
        case TK_IF:
            return "tk_if";
        case TK_ELSE:
            return "tk_else";
        case TK_WHILE:
            return "tk_while";
        case TK_FOR:
            return "tk_for";
        case TK_TYPE:
            return "tk_type";
        case TK_SIZEOF:
            return "tk_sizeof";
        case TK_STRING:
            return "tk_string";
        case TK_STRUCT:
            return "tk_struct";
        case TK_TYPEDEF:
            return "tk_typedef";
        case TK_ENUM:
            return "tk_enum";
        case TK_BREAK:
            return "tk_break";
        case TK_CONTINUE:
            return "tk_continue";
        case TK_SWITCH:
            return "tk_switch";
        case TK_CASE:
            return "tk_case";
        case TK_DEFAULT:
            return "tk_default";
        case TK_TRUE:
            return "tk_true";
        case TK_FALSE:
            return "tk_false";
        case TK_NULL:
            return "tk_null";
        case TK_SEEKSET:
            return "tk_seekset";
        case TK_SEEKCUR:
            return "tk_seekcur";
        case TK_SEEKEND:
            return "tk_seekend";
        case TK_ERRNO:
            return "tk_errno";
        case TK_STDERR:
            return "tk_stderr";
    }

    return "tk_unknown";
}

// from tokenize.c
token *token;
char *user_input;
char *filename;

// read current token, and check if it's expected content
// if it's as expected, go on to the next token and return true
// otherwise return false.
bool consume(char *op) {
    if (!check(op)) {
        return false;
    }
    advance_token();
    return true;
}

// read current token, and check if it's expected kind.
// if it's as expected, go on to the next token and return the token
// otherwise return null.
token *consume_kind(tokenkind kind) {
    if (!check_kind(kind)) {
        return NULL;
    }
    token *tok = token;
    advance_token();
    return tok;
}

// read current token, and check if it's expected content
// if it's as expected, just return true without moving to next one
bool check(char *op) {
    return (token->kind == TK_RESERVED) && (strlen(op) == token->len) &&
           (memcmp(token->str, op, token->len) == 0);
}

// read current token, and check if it's expected kind
// if it's as expected, just return true without moving to next one
bool check_kind(tokenkind kind) { return token->kind == kind; }

// read current token, and check if it's expected content
// if it's as expected, go on to the next token and return
// otherwise throw error.
void expect(char *op) {
    if (!check(op)) {
        char *tmp[100] = {0};
        memcpy(tmp, token->str, token->len);
        error_at2(token->str, "expected: '%c'\nactual: '%s'\n", *op, tmp);
    }
    advance_token();
    return;
}

// read current token, and check if it's expected kind
// if it's as expected, go on to the next token and return
// otherwise throw error.
token *expect_kind(tokenkind kind) {
    if (!check_kind(kind)) {
        error_at2(token->str, "unexpected token: '%s'\nactual: '%s'\n",
                  get_token_kind_name(kind), get_token_kind_name(token->kind));
    }
    token *tok = token;
    advance_token();
    return tok;
}

// expect function dedicated for tk_num
int expect_number() {
    if (token->kind != TK_NUM) {
        print_token(token);
        error_at0(token->str, "数ではありません\n");
    }
    int val = token->val;
    advance_token();
    return val;
}

// consume tk_eof
bool at_eof() { return token->kind == TK_EOF; }

// go on to the next token
void advance_token() { token = token->next; }

// initialize new token and put it into given linkedlist
token *new_token(tokenkind kind, token *cur, char *str, int len) {
    // using calloc here is better since it does zero clearance
    token *tok = calloc(1, sizeof(token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

bool is_alnum(char p) {
    return ('a' <= p && p <= 'z') || ('a' <= p && p <= 'z') ||
           ('0' <= p && p <= '9') || (p == '_');
}

typedef struct ReservedWord {
    char *word;
    TokenKind kind;
} ReservedWord;

// tokenization table
reservedword reserved_words[] = {
    {"return", TK_RETURN},
    {"if", TK_IF},
    {"else", TK_ELSE},
    {"while", TK_WHILE},
    {"for", TK_FOR},
    {"int", TK_TYPE},
    {"void", TK_TYPE},
    {"char", TK_TYPE},
    {"bool", TK_TYPE},
    {"sizeof", TK_SIZEOF},
    {"struct", TK_STRUCT},
    {"typedef", TK_TYPEDEF},
    {"enum", TK_ENUM},
    {"break", TK_BREAK},
    {"continue", TK_CONTINUE},
    {"switch", TK_SWITCH},
    {"case", TK_CASE},
    {"default", TK_DEFAULT},
    {"true", TK_TRUE},
    {"false", TK_FALSE},
    {"null", TK_NULL},
    {"seek_set", TK_SEEKSET},
    {"seek_cur", TK_SEEKCUR},
    {"seek_end", TK_SEEKEND},
    {"errno", TK_ERRNO},
    {"stderr", TK_STDERR},
};

// tokenize the input stream and return linkedlist of token
token *tokenize() {
    char *p = user_input;
    token head;
    head.next = NULL;
    token *cur = &head;

    while (*p) {
        // skip whitespace
        if (isspace(*p)) {
            p++;
            continue;
        }

        // skip line comments
        if (startswith(p, "//")) {
            p += 2;
            while (*p != '\n') p++;
            continue;
        }

        // skip block comments
        if (startswith(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q) {
                error_at0(p, "unclosed block comment");
            }
            p = q + 2;
            continue;
        }

        // multi-char reserved tokens(like ==, !=, <=, >=, ->, +=, -=, *=, /=,
        // ++, --, &&, ||, <<, >>)
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
            startswith(p, ">=") || startswith(p, "->") || startswith(p, "+=") ||
            startswith(p, "-=") || startswith(p, "*=") || startswith(p, "/=") ||
            startswith(p, "++") || startswith(p, "--") || startswith(p, "&&") ||
            startswith(p, "||") || startswith(p, "<<") || startswith(p, ">>")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // single-char reserved tokens(like +, -, *, /, (, ), <, >, =, {, }, ;,
        // ,, &, |, ^, ~, [, ], :, ., !, #)
        if (strchr("+-*/()<>={};,^~&|![]:.?", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        // character literal
        if (*p == '\'') {
            cur = read_char_literal(cur, p);
            p = cur->str + cur->len;
            continue;
        }

        // string literal
        if (*p == '"') {
            char *q = strchr(p + 1, '"');
            if (!q) {
                error_at0(p, "unclosed string literal");
            }
            cur = new_token(TK_STRING, cur, p + 1, q - (p + 1));
            p = q + 1;
            continue;
        }

        // ident or reserved word
        if (isalpha(*p) || *p == '_') {
            int len = 0;
            char *q = p;
            while (is_alnum(*q)) {
                len++;
                q++;
            }
            char *ident = calloc(1, len + 1);
            memcpy(ident, p, len);
            ident[len] = '\0';

            // check if ident is reserved word
            bool is_reserved_word = false;
            for (int i = 0;
                 i < sizeof(reserved_words) / sizeof(reservedword); i++) {
                if (strcmp(ident, reserved_words[i].word) == 0) {
                    cur = new_token(reserved_words[i].kind, cur, p, len);
                    is_reserved_word = true;
                    break;
                }
            }

            if (!is_reserved_word) {
                cur = new_token(TK_IDENT, cur, p, len);
            }
            p += len;
            continue;
        }

        // number literal
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            // move as many as length of the digit
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at0(p, "failed to tokenize.\n");
    }

    // print_token(cur);
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

// todo: read here in detail
//   'a'
//   ^start here
token *read_char_literal(token *cur, char *start) {
    char *p = start + 1;
    if (*p == '\0') {
        error_at0(start, "unclosed char literal");
    }

    char c;
    if (*p == '\\') {
        // when escaping char literal
        p++;
        c = get_escape_char(*p);
        p++;
    } else {
        c = *p;
        p++;
    }

    // when ptr reaches here, it must be the closing single quote.
    if (*p != '\'') {
        error_at0(start, "char literal must be one length");
    }
    p++;

    token *tok = new_token(TK_NUM, cur, start, p - start);
    tok->val = c;

    return tok;
}

char get_escape_char(char c) {
    switch (c) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 't':
            return '\t';
        case 'n':
            return '\n';
        case 'v':
            return '\v';
        case 'f':
            return '\f';
        case 'r':
            return '\r';
        case 'e':
            return 27;  // ascii escape character
        case '0':
            return 0;
        default:
            return c;
    }
}

// from parse.c
lvar* locals[100];
lvar* globals[100];
int cur_scope_depth = 0;
stringtoken* strings;
tag* tags;
enumvar* enum_vars;
node* current_switch = 0;
// how many function/global variables/global typedef this compiler supports.
// todo replace node*[] -> node**
node* code[1000];

/**************************
 * node builder
 * *************************/
node* new_node(nodekind kind) {
    node* node = calloc(1, sizeof(node));
    node->kind = kind;
    return node;
}

node* new_binary(nodekind kind, node* lhs, node* rhs) {
    node* node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

node* new_unary(nodekind kind, node* lhs) {
    node* node = new_node(kind);
    node->lhs = lhs;
    return node;
}

// a util function to build num node
node* new_num(int val) {
    node* node = new_node(ND_NUM);
    node->val = val;
    return node;
}

// a util function to build string node
node* new_string(stringtoken* str) {
    node* node = new_node(ND_STRING);
    node->string = str;
    return node;
}

/**************************
 * parse functions
 * *************************/
// program ::= ( func_def
//             | define_variable ";"
//             | "struct" define_struct ";"
//             | "enum" define_enum ";"
//             | "typedef" define_typedef
//             )*
void program() {
    int i = 0;
    while (!at_eof()) {
        token* tk = token;
        if (consume_kind(TK_TYPEDEF)) {
            define_typedef();
            expect(";");
            continue;
        }

        if (check_kind(TK_ENUM)) {
            define_enum();
            expect(";");
            continue;
        }

        // debug: if(check_kind(tk_struct)) would be better. will align with code above

        // top level statements which start with type + ident, like "int* hoge"
        // three types of such statements.
        // 1. function def: int main() {}
        // 2. struct def: struct hoge {} ... this should be placed before this section. if (check_kind(tk_struct)) is better.
        // 3. global var def: int hoge = 1;
        define* def = read_define_head();

        if (def == NULL) {
            print_token(token);
            error0("def is null");
        }
        if (consume("(")) {
            node* func_node = func_def(def);
            if (func_node) {
                code[i] = func_node;
            } else {
                continue;
            }
        } else if (def->type->ty == STRUCT) {
            /*
            # valid:
            struct hoge {int a;} hoge;
            struct hoge {int a;} hoge = {1};
            hoge hoge;
            hoge hoge[] = {};

            # invalid:
            struct {int a;} hoge;
            struct {int a;} hoge = {1};
            struct hoge {int a;}[] a;  ... we cannot define array of struct in oneline.
            */
            if (check("[")) {
                // hoge hoge[] pattern.
                node* gvar = define_variable(def, globals);

                // todo: refactor: should change kind in other place.
                gvar->kind = ND_GVAR_DEF;
                code[i] = gvar;
                expect(";");
            } else {
                // struct hoge {int a;};
                define_struct();
                expect(";");
                continue;
            }
        } else {
            node* gvar = define_variable(def, globals);

            // todo: refactor: should change kind in other place.
            gvar->kind = ND_GVAR_DEF;
            code[i] = gvar;
            expect(";");
        }
        i++;
    }
    code[i] = NULL;
}

// ex:
//   typedef struct hoge {int a;} gegi;
//   typedef struct hoge geho;
//   typedef int int;
//   typedef char* string;
// define_typedef ::= "typedef" type_decl alias
// todo: this doesn't have to return bool.
bool define_typedef() {
    // todo: do consume_kind(tk_typedef) here
    define* def = read_define_head();
    read_define_suffix(def);

    // register this type(struct) as its name is this identident
    push_tag(def->ident, def->type, false);
    return true;
}

// define_enum ::= "enum" ident "{" (define_enum_member (","
type* define_enum() {
    if (!consume_kind(TK_ENUM)) {
        return NULL;
    }

    token* name = consume_kind(TK_IDENT);

    if (name && !check("{")) {
        tag* tag = find_or_register_tag(name);
        if (tag->type->incomplete) {
            error0("type not found.");
        }
        return tag->type;  // int should be returned
    }

    expect("{");
    int num = 0;
    while (true) {
        if (consume("}")) break;

        define_enum_member(&num);

        // it's legal to have trailing comma
        if (consume("}")) break;
        expect(",");
    }

    // register to tags as int type
    if (name) {
        push_tag(name, int_type(), true);
    }

    return int_type();
}

/*
define_enum_member ::= ident ("=" number)
    `aaa = 10`
    or
    `aaa`
define_enum_member register each enum members as enumvar
auto assigned number value starts from 1;
*/
void define_enum_member(int* num) {
    token* tok = expect_kind(TK_IDENT);

    if (consume("=")) {
        *num = expect_number();
    } else {
        *num += 1;
    }

    enumvar* e = calloc(1, sizeof(enumvar));
    e->name = calloc(100, sizeof(char));
    memcpy(e->name, tok->str, tok->len);
    e->value = *num;
    e->next = enum_vars;
    enum_vars = e;
    return;
}

type* int_type() {
    type* t = calloc(1, sizeof(type));
    t->ty = INT;
    t->size = 4;
    return t;
}
type* char_type() {
    type* t = calloc(1, sizeof(type));
    t->ty = CHAR;
    t->size = 1;
    return t;
}
type* ptr_type() {
    type* t = calloc(1, sizeof(type));
    t->ty = PTR;
    t->size = 8;
    return t;
}

/*
define new struct tag or finde defined struct tag, and return the tag's type

struct itself doesn't print to assembly;
it's information only compiler has.

when define_struct_body doesn't exist,
it registers the struct tentatively or retrieves already registered tag

define_struct ::= "struct" ident? define_struct_body
                  | "struct" ident
define_struct_body ::= "{" (define_variable ";")* "}"
*/
type* define_struct() {
    if (!consume_kind(TK_STRUCT)) {
        return NULL;
    }

    token* name = consume_kind(TK_IDENT);

    // if ident(name) is found and this struct doesn't have define_struct_body,
    // retrieve a tag tied with the name
    if (name && !check("{")) {
        tag* tag = find_or_register_tag(name);
        // when the struct is still imcomplete, this tag is returned as int,
        // so override it as struct forcibly.
        // todo: refactor
        tag->type->ty = STRUCT;
        return tag->type;
    }

    expect("{");

    type* struct_type = calloc(1, sizeof(type));
    struct_type->ty = STRUCT;

    member head;
    member* cur = &head;

    int offset = 0;
    int maxsize = 0;
    while (!consume("}")) {
        // retrieve type info of a member
        define* def = read_define();
        expect(";");
        // build a members with the member's name and type
        //// name
        member* m = calloc(1, sizeof(member));
        m->name = calloc(100, sizeof(char));
        memcpy(m->name, def->ident->str, def->ident->len);
        //// type
        m->ty = def->type;
        int size = get_type_size(def->type);

        /*
        calc offset for each member.
        here it just defines offset for each members.
        todo:
        with initializer: we need nd_padding
        without initializer: we don't need nd_padding
        ... why? see how assembly is generated with these 4 patterns
        1. local * with initializer
        2. global * with initializer
        3. local * without initializer
        4. global * without initializer


        struct {
            int a;
            char b;
            int c;
        }
        a: offset = 0
        b: offset = 4
        c: offset = 8
        */
        // todo: learn in more detail here.
        offset = align_to(offset, size);
        m->offset = offset;
        offset += size;
        cur->next = m;
        cur = m;

        if (maxsize <= 8 && maxsize <= size) {
            maxsize = size;
        }
    }

    struct_type->members = head.next;
    struct_type->size = align_to(offset, maxsize);

    // register to tags
    if (name) {
        push_tag(name, struct_type, true);
    }

    return struct_type;
}

/*
func_def ::= read_define "(" (define_arg ("," define_ar)*)? ")" block
note: c cannot pass array as arg. so we cannot use read_define for arg.

node{
    kind: nd_func_def
    funcname: function name
    args: array of node(nd_lvar). note: register args as local variables too!
    lhs: function body as block
}
*/
node* func_def(define* def) {
    // one lvar linked list per one function.
    cur_scope_depth++;

    node* node = new_node(ND_FUNC_DEF);

    // function name
    node->funcname = calloc(100, sizeof(char));
    memcpy(node->funcname, def->ident->str, def->ident->len);
    node->args = calloc(10, sizeof(node*));

    // function args
    for (int i = 0; !consume(")"); i++) {
        if (i != 0) expect(",");
        node->args[i] = define_arg();
    }

    // note: int hoge(); is prototype decl.
    // just skip this kind of statement since baricc doesn't support prototype decl(it just ignores)
    if (consume(";")) {
        locals[cur_scope_depth] = NULL;
        cur_scope_depth--;
        return NULL;
    }

    // function body
    node->lhs = block();

    return node;
}

// define_arg ::= type "*"* ident
node* define_arg() {
    define* arg_def = read_define_head();
    if (arg_def == NULL) {
        char* name[100] = {0};
        memcpy(name, token->str, token->len);
        error1("invalid token comes here. %d", name);
    }

    // extract arg as nd_lvar and register it as local variable
    return define_variable(arg_def, locals);
}

// todo: this ebnf is nore up-to-date.
// stmt = expr ";"
//        | "return" expr ";"
//        | "if" "(" expr ")" stmt ("else" stmt)?
//        | "while" "(" expr ")" stmt
//        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//        | define_variable ";"
//        | block
//        | "break" ";"
//        | "continue" ";"
//        | ("--" | "++") ident
//        | ident ("--" | "++")
node* stmt() {
    node* node;

    if (consume_kind(TK_RETURN)) {
        node = new_node(ND_RETURN);
        if (!consume(";")) {
            node->lhs = expr();
            expect(";");
        }
        return node;
    }

    if (consume_kind(TK_IF)) {
        /*
        # without else
        nd_if:
            lhs: cond
            rhs: main

        # with else
        nd_if:
            lhs: cond
            rhs: else
                lhs: main
                rhs: alt(stmt)
        */
        node = new_node(ND_IF);
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        if (consume_kind(TK_ELSE)) {
            node* els = new_node(ND_ELSE);
            els->lhs = node->rhs;
            els->rhs = stmt();
            node->rhs = els;
        }
        return node;
    }

    if (consume_kind(TK_WHILE)) {
        node* node = new_node(ND_WHILE);
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        return node;
    }

    if (consume_kind(TK_SWITCH)) {
        /*
            switch(expr) {
                case int:
                    stmt*
                    (break;)*
                default:
                    stmt*
            }
        */
        node* node = new_node(ND_SWITCH);
        expect("(");
        node->lhs = expr();  // switch's flag expr
        expect(")");

        // to allow nested switch...
        node* prev_switch = current_switch;
        current_switch = node;
        node->rhs = block();
        current_switch = prev_switch;

        return node;
    }

    if (consume_kind(TK_CASE)) {
        // consider `case 1:` this line as one stmt which outputs label only.
        if (!current_switch) {
            error0("stray case");
        }

        token* t = token;
        token* ident = consume_kind(TK_IDENT);
        node* num_node = NULL;
        if (ident) {
            num_node = find_enum_var(ident);
        }

        if (num_node == NULL) {
            num_node = convert_predefined_keyword_to_num();
        }

        int val;
        if (num_node) {
            val = num_node->val;
        } else {
            val = expect_number();
        }

        expect(":");
        node* node = new_node(ND_CASE);
        node->val = val;

        // to check labels from the top one.
        node->next_case = current_switch->next_case;
        current_switch->next_case = node;
        return node;
    }

    if (consume_kind(TK_DEFAULT)) {
        expect(":");
        node* node = new_node(ND_DEFAULT_CASE);
        node->next_case = current_switch->next_case;
        current_switch->next_case = node;
        return node;
    }

    if (consume_kind(TK_FOR)) {
        /*
        for({a}; {b}; {c)) d

        [a]
        .lbeginxxx:
        [b]
        pop rax
        cmp rax, 0
        je .lendxxx
        [d]
        [c]
        jmp .lbeginxxx
        .lendxxx:

        kind: nd_for
        lhs:
          lhs: a: init
          rhs: b: cond
        rhs:
          lhs: c: post ops
          rhs: d: body
        */

        node* node = new_node(ND_FOR);
        expect("(");

        // a node which contain init(a) and cond(b)
        node* left = new_node(ND_FOR_LEFT);

        // init(a)
        // define_variable is not a statement, but it contains expression(ex: int
        // a=1)
        if (check_kind(TK_TYPE)) {
            node* init_def = define_variable(read_define(), locals);
            left->lhs = init_def;
        } else if (!consume(";")) {
            left->lhs = expr();
            expect(";");
        }

        // cond(b)
        if (!consume(";")) {
            left->rhs = expr();
            expect(";");
        }

        node->lhs = left;

        // a node which contain post_ops(c) and body(d)
        node* right = new_node(ND_FOR_RIGHT);

        // post ops(c)
        if (!consume(")")) {
            right->lhs = expr();
            expect(")");
        }
        right->rhs = stmt();
        node->rhs = right;

        return node;
    }

    if (check_kind(TK_TYPE)) {
        node* node = define_variable(read_define(), locals);
        expect(";");
        return node;
    }

    if (consume_kind(TK_BREAK)) {
        node = new_node(ND_BREAK);
        expect(";");
        return node;
    }

    if (consume_kind(TK_CONTINUE)) {
        node = new_node(ND_CONTINUE);
        expect(";");
        return node;
    }

    // block = "{" stmt* "}"
    if (check("{")) {
        return block();
    }

    node = expr();
    expect(";");
    return node;
}

// expr ::= assign
node* expr() { return assign(); }

// assign ::= conditional ("=" assign
//                  | "+=" assign
//                  | "-=" assign
//                  | "*=" assign
//                  | "/=" assign
//                  | "<<=" assign
//                  | ">>=" assign
//                  | "%=" assign
//                  | "&=" assign
//                  | "|=" assign
//                  | "^=" assign
//                  )?
node* assign() {
    node* node = conditional();

    if (consume("=")) {
        node = new_binary(ND_ASSIGN, node, assign());
        return node;
    }
    if (consume("+=")) {
        node* add = new_binary(ND_ADD, node, ptr_conversion(node, assign()));
        node = new_binary(ND_ASSIGN, node, add);
        return node;
    }
    if (consume("-=")) {
        node* sub = new_binary(ND_SUB, node, ptr_conversion(node, assign()));
        node = new_binary(ND_ASSIGN, node, sub);
        return node;
    }
    if (consume("*=")) {
        node* mul = new_node(ND_MUL);
        node* right = assign();
        mul->lhs = node;
        mul->rhs = right;
        node = new_binary(ND_ASSIGN, node, mul);
        return node;
    }
    if (consume("/=")) {
        node* div = new_node(ND_DIV);
        node* right = assign();
        div->lhs = node;
        div->rhs = right;
        node = new_binary(ND_ASSIGN, node, div);
        return node;
    }
    if (consume("%=")) {
        node* mod = new_node(ND_MOD);
        node* right = assign();
        mod->lhs = node;
        mod->rhs = right;
        node = new_binary(ND_ASSIGN, node, mod);
        return node;
    }
    if (consume("<<=")) {
        node* shl = new_node(ND_SHL);
        node* right = assign();
        shl->lhs = node;
        shl->rhs = right;
        node = new_binary(ND_ASSIGN, node, shl);
        return node;
    }
    if (consume(">>=")) {
        node* shr = new_node(ND_SHR);
        node* right = assign();
        shr->lhs = node;
        shr->rhs = right;
        node = new_binary(ND_ASSIGN, node, shr);
        return node;
    }
    if (consume("&=")) {
        node* bitand = new_node(ND_BITAND);
        node* right = assign();
        bitand->lhs = node;
        bitand->rhs = right;
        node = new_binary(ND_ASSIGN, node, bitand);
        return node;
    }
    if (consume("|=")) {
        node* bitor = new_node(ND_BITOR);
        node* right = assign();
        bitor->lhs = node;
        bitor->rhs = right;
        node = new_binary(ND_ASSIGN, node, bitor);
        return node;
    }
    if (consume("^=")) {
        node* bitxor = new_node(ND_BITXOR);
        node* right = assign();
        bitxor->lhs = node;
        bitxor->rhs = right;
        node = new_binary(ND_ASSIGN, node, bitxor);
        return node;
    }

    return node;
}

// conditional ::= logor ("?" expr ":" conditional)?
node* conditional() {
    node* node = logor();
    if (consume("?")) {
        node* cond = new_node(ND_COND);
        cond->lhs = node;
        cond->rhs = new_node(ND_ELSE);
        cond->rhs->lhs = expr();
        expect(":");
        cond->rhs->rhs = conditional();
        node = cond;
    }
    return node;
}

// logor ::= logand ("||" logand)*
node* logor() {
    node* node = logand();
    while (consume("||")) {
        node = new_binary(ND_LOGOR, node, logand());
    }
    return node;
}

// logand ::= bitxor ("&&" bitxor)*
node* logand() {
    node* node = bitxor();
    while (consume("&&")) {
        node = new_binary(ND_LOGAND, node, bitxor());
    }
    return node;
}

/*
ex: a ^ b ^ c => (a ^ b) ^ c
nd_bitxor:
  lhs: nd_bitxor
    lhs: a
    rhs: b
  rhs: c

todo: now it doesn't work like this. add test and fix.
*/
// bitxor ::= bitor ("^" bitor)*
node* bitxor() {
    node* node = bitor();
    while (consume("^")) {
        node = new_binary(ND_BITXOR, node, bitor());
    }
    return node;
}

// bitor ::= bitand ("|" bitand)*
node* bitor() {
    node* node = bitand();
    while (consume("|")) {
        node = new_binary(ND_BITOR, node, bitand());
    }
    return node;
}

/*
ex: a & b & c => (a & b) & c
nd_bitand:
  lhs: nd_bitand
    lhs: a
    rhs: b
  rhs: c

todo: now it doesn't work like this. add test and fix.
*/
// bitand ::= equality ("&" equality)*
node* bitand() {
    node* node = equality();
    while (consume("&")) {
        node = new_binary(ND_BITAND, node, equality());
    }
    return node;
}

// equality ::= relational ("==" relational | "!=" relational)*
node* equality() {
    node* node = relational();

    for (;;) {
        if (consume("==")) {
            node = new_binary(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_binary(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

// relational ::= shift ("<" shift | "<=" shift | ">" shift | ">=" shift)*
node* relational() {
    node* node = shift();

    for (;;) {
        if (consume("<")) {
            node = new_binary(ND_LT, node, shift());
        } else if (consume("<=")) {
            node = new_binary(ND_LE, node, shift());
        } else if (consume(">")) {
            // x > y is same as y < x
            node = new_binary(ND_LT, shift(), node);
        } else if (consume(">=")) {
            // x >= y is same as y <= x
            node = new_binary(ND_LE, shift(), node);
        } else {
            return node;
        }
    }
}

// shift ::= add ("<<" add | ">>" add)*
node* shift() {
    node* node = add();

    for (;;) {
        if (consume("<<")) {
            node = new_binary(ND_SHL, node, add());
        } else if (consume(">>")) {
            node = new_binary(ND_SHR, node, add());
        } else {
            return node;
        }
    }
}

// add = mul ("+" mul | "-" mul)*
node* add() {
    node* node = mul();

    for (;;) {
        if (consume("+")) {
            node = new_binary(ND_ADD, node, ptr_conversion(node, mul()));
        } else if (consume("-")) {
            node = new_binary(ND_SUB, node, ptr_conversion(node, mul()));
        } else {
            return node;
        }
    }
}

// mul = unary ("*" unary | "/" unary | "%" unary)*
node* mul() {
    node* node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_binary(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_binary(ND_DIV, node, unary());
        } else if (consume("%")) {
            node = new_binary(ND_MOD, node, unary());
        } else {
            return node;
        }
    }
}

// unary = ("+" | "-" | "*" | "&" | "!" | "~")? primary
//       | ("++" | "--") unary
//       | "sizeof" unary
//       | "sizeof" "(" type_decl ")"
node* unary() {
    if (consume("+")) {
        return primary();
    }
    if (consume("-")) {
        return new_binary(ND_SUB, new_num(0), unary());
    }
    if (consume("*")) {
        return new_unary(ND_DEREF, unary());
    }
    if (consume("&")) {
        return new_unary(ND_ADDR, unary());
    }
    if (consume("!")) {
        return new_unary(ND_NOT, unary());
    }
    if (consume("~")) {
        return new_unary(ND_BITNOT, unary());
    }

    if (consume("++")) {
        return new_unary(ND_PRE_INC, unary());
    }
    if (consume("--")) {
        return new_unary(ND_PRE_DEC, unary());
    }

    if (consume_kind(TK_SIZEOF)) {
        // when pure type is passed
        if ((check("(") && token->next->kind == TK_TYPE) || check_kind(TK_TYPE)) {
            bool has_brace = false;
            if (consume("(")) {
                has_brace = true;
            }

            define* def = read_define();

            if (has_brace) {
                expect(")");
            }

            int size = get_type_size(def->type);
            return new_num(size);
        } else {
            node* node = unary();
            return new_num(get_type_size(get_type(node)));
        }
    }

    return primary();
}

// primary = "(" expr ")" | num | ident ( func_call | "[" expr "]" | "." ident |
// "->" ident | "++" | "--")*
node* primary() {
    // "(" expr ")"
    if (consume("(")) {
        if (check("{")) {
            // statement expression(gcc extension)
            node* node = new_node(ND_STMT_EXPR);
            node->lhs = block();
            expect(")");
            return node;
        }
        node* node = expr();
        expect(")");
        return node;
    }

    // num
    token* tok = consume_kind(TK_NUM);
    if (tok) {
        return new_num(tok->val);
    }

    // ident ( func_call | "[" expr "]" | "." ident | "->" ident | "++" | "--")*
    tok = consume_kind(TK_IDENT);
    if (tok) {
        return primary_suffix(tok);
    }

    // string literal
    if (tok = consume_kind(TK_STRING)) {
        // todo: builder 関数を整理したい
        stringtoken* s = calloc(1, sizeof(stringtoken));
        s->value = calloc(100, sizeof(char));
        memcpy(s->value, tok->str, tok->len);
        if (strings) {
            s->index = strings->index + 1;
        } else {
            s->index = 0;
        }
        s->next = strings;
        strings = s;
        return new_string(s);
    }

    error_at0(token->str, "invalid token here");
    return NULL;  // dummy
}

// primary_suffix handles ident with suffix: ( func_call | "[" expr "]" | "."
// ident | "->" ident | "++" | "--")*
node* primary_suffix(token* tok) {
    node* node;
    type* tp = NULL;
    char* varname = NULL;

    // func call
    if (consume("(")) {
        node = new_node(ND_FUNC_CALL);
        node->funcname = calloc(100, sizeof(char));
        memcpy(node->funcname, tok->str, tok->len);
        // todo: 引数とりあえず10こまで。
        node->block = calloc(10, sizeof(node));
        for (int i = 0; !consume(")"); i++) {
            if (i != 0) {
                expect(",");
            }
            node->block[i] = expr();
        }
        return node;
    }

    // whne enum
    node* num = find_enum_var(tok);
    if (num) {
        return num;
    }

    // calling variable
    node = variable(tok);
    tp = node->type;
    varname = node->varname;

    for (;;) {
        // array access
        // a[b] is equal to *(a + b)
        if (consume("[")) {
            node* idx = expr();
            expect("]");
            node* add = new_binary(ND_ADD, node, idx);
            node = new_unary(ND_DEREF, add);
            tp = node->type;
            node->varname = varname;
            continue;
        }

        // access to member
        if (consume(".")) {
            node = struct_ref(node);
            tp = node->type;
            node->varname = varname;
            continue;
        }

        // access to member (base is ptr)
        // ptr->field => (*ptr).field
        if (consume("->")) {
            type* t = node->type->ptr_to;
            node* deref = new_node(ND_DEREF);
            deref->lhs = node;  // this node is ptr lvar
            deref->type = t;    // now deref is done. we do the same thing in a
                                // block above.
            node = struct_ref(deref);
            tp = node->type;
            node->varname = varname;
            continue;
        }

        // a++
        if (consume("++")) {
            node = new_binary(ND_SUF_INC, node, NULL);
            node->varname = varname;
            continue;
        }

        // a--
        if (consume("--")) {
            node = new_binary(ND_SUF_DEC, node, NULL);
            node->varname = varname;
            continue;
        }

        // something is wrong if node's type is incomplete...
        if (tp && tp->incomplete) {
            error0("incomplete type is used here.");
        }

        return node;
    }
}

// struct_ref handles .member and ->member as a part of member access
node* struct_ref(node* node) {
    // should have already checked for struct type
    if (node->type->ty != STRUCT) {
        error0("member is called for non-struct type.");
    }
    member* member = find_member_struct(expect_kind(TK_IDENT), node->type);

    node* member_node = new_node(ND_MEMBER);
    member_node->member = member;
    member_node->lhs = node;
    member_node->type = member->ty;

    return member_node;
}

// block = "{" stmt* "}"
node* block() {
    node* node = new_node(ND_BLOCK);
    expect("{");

    // blocks are implemented as a set of statements. so they are just array of
    // nodes. 100 statements are available per one block.
    // todo: support more number of statements in block
    node->block = calloc(100, sizeof(node));

    // block will have its own local scope
    lvar* old_locals = locals[cur_scope_depth];
    cur_scope_depth++;
    locals[cur_scope_depth] = NULL;

    int i = 0;
    while (!consume("}")) {
        node->block[i] = stmt();
        i++;
    }
    locals[cur_scope_depth] = old_locals;
    cur_scope_depth--;

    return node;
}

// find a local variable by name
lvar* find_lvar(token* tok) {
    for (lvar* var = locals[cur_scope_depth]; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

// find a global variable by name
lvar* find_gvar(token* tok) {
    for (lvar* var = globals[0]; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

// register a local variable
// note: for now it assumes all lvar takes 8 bytes for size.
lvar* push_lvar(token* tok, type* type) {
    lvar* var = calloc(1, sizeof(lvar));
    var->next = locals[cur_scope_depth];
    var->name = calloc(100, sizeof(char));
    memcpy(var->name, tok->str, tok->len);
    var->len = tok->len;
    var->type = type;
    if (locals[cur_scope_depth]) {
        var->offset = locals[cur_scope_depth]->offset + get_type_size(type);
    } else {
        var->offset = get_type_size(type);
    }
    locals[cur_scope_depth] = var;
    return var;
}

// register a global variable
lvar* push_gvar(token* tok, type* type) {
    lvar* var = calloc(1, sizeof(lvar));
    var->next = globals[0];
    var->name = calloc(100, sizeof(char));
    memcpy(var->name, tok->str, tok->len);
    var->len = tok->len;
    var->type = type;
    globals[0] = var;
    return var;
}

// register a variable as nd_lvar or nd_gvar
node* variable(token* tok) {
    lvar* lvar = find_lvar(tok);

    if (lvar) {
        node* node = new_node(ND_LVAR);
        node->offset = lvar->offset;
        node->type = lvar->type;
        node->varname = lvar->name;
        node->varsize = get_type_size(lvar->type);
        node->var = lvar;
        return node;
    }

    lvar* gvar = find_gvar(tok);
    if (gvar) {
        node* node = new_node(ND_GVAR);
        node->varname = gvar->name;
        node->type = gvar->type;
        node->varsize = get_type_size(gvar->type);
        node->var = gvar;
        return node;
    }

    // todo: better to check whether it's function definition or func call here...
    error_at1(tok->str, "undefined variable: %s\n", tok->str);
    return NULL;
}

/*
read_define: read variable definition and register lval.
ex: `int a = 1`
this function defines `int a` as local variable `a` and returns `nd_lvar` for it
as node
*/
// read_type_and_ident ("=" initializer)?
node* define_variable(define* def, lvar** lvars) {
    lvar* var;
    if (lvars == locals) {
        var = push_lvar(def->ident, def->type);
    } else {
        var = push_gvar(def->ident, def->type);
    }

    node* node = new_node(ND_LVAR);
    node->offset = var->offset;
    node->type = var->type;
    node->varname = var->name;
    node->varsize = get_type_size(var->type);
    node->var = var;

    // assignment operator
    if (consume("=")) {
        node->var->init = initializer(def->type);
    }

    // struct initializer is not yet fully supported..
    if (node->var->type->ty == STRUCT) {
        if (node->var->init) {
            // struct with initializer
            error0("struct initializer is not supported.");
        }
    }

    // array initializer with array size defined
    // char a[5] = {1, 2, 3};
    // char a[] = "foobar";
    // int b[] = {1, 2, 3};
    if (node->var->type->ty == ARRAY && node->var->init &&
        node->var->init->kind == ND_BLOCK) {
        node* block = new_node(ND_BLOCK);
        block->block =
            calloc(node->var->type->array_size, sizeof(node));

        // char a[] = "foobar";
        if (node->var->init->block[0]->kind == ND_STRING) {
            // this is a char a[] = "foobar" case
            int len = strlen(node->var->init->block[0]->string->value) + 1;
            node->var->type->array_size = len;
        }

        for (int i = 0; node->var->init->block[i]; i++) {
            // a[0] ==> *(a + 0 * size)
            node* add = new_node(ND_ADD);
            add->lhs = node;
            if (node->type && node->type->ptr_to) {
                int size = get_type_size(node->type->ptr_to);
                add->rhs = new_num(i * size);
            }
            node* deref = new_node(ND_DEREF);
            deref->lhs = add;
            node* assign = new_node(ND_ASSIGN);
            assign->lhs = deref;
            assign->rhs = node->var->init->block[i];
            block->block[i] = assign;
        }
        node->var->init = block;
    }

    return node;
}

// initializer ::= assign | "{" (initializer ("," initializer)*)? "}"
node* initializer(type* type) {
    node* init = new_node(ND_BLOCK);
    int i = 0;
    if (check("{")) {
        initializer_helper(type, init, &i);
    } else {
        init = expr();
        if (init->kind == ND_STRING) {
            int len = strlen(init->string->value) + 1;
            if (type->array_size < len) {
                type->array_size = len;
            }
        }
    }
    return init;
}

// initializer_helper helps initializer to parse nested initializer
void initializer_helper(type* type, node* init, int* i) {
    // todo: refactor: it must be able to cunsume "{" here.
    if (consume("{")) {
        if (type->ty == ARRAY) {
            for (; !consume("}");) {
                if (check("{")) {
                    initializer_helper(type->ptr_to, init, i);
                } else {
                    init->block[*i] = expr();
                    init->block[*i]->type = type->ptr_to;
                    *i += 1;
                }
                consume(",");
            }
            // when size of initializer is larger than array_size, override the
            // array_size
            if (type->array_size < *i) {
                type->array_size = *i;
            }
        } else if (type->ty == STRUCT) {
            // struct initializer
            error0("struct initializer is not supported.");
        } else {
            init->block[*i] = expr();
            consume("}");
        }
        return;
    }

    // only expr is available in single value initializer
    init->block[*i] = expr();
}

// ptr_conversion converts value for pointer calculation.
// when a pointer is added/substracted by an integer, the integer value must be
// multiplied by the size of object that the pointer points to.
node* ptr_conversion(node* node, node* right) {
    if (get_type(node)->ty != PTR && get_type(node)->ty != ARRAY) {
        return right;
    }
    if (get_type(right)->ty != INT && get_type(right)->ty != CHAR) {
        return right;
    }

    type* pointed_to_type = get_type(node)->ptr_to;
    if (pointed_to_type == NULL) {
        return right;
    }

    int type_size = get_type_size(pointed_to_type);
    return new_binary(ND_MUL, right, new_num(type_size));
}

// get_type returns the type of node
type* get_type(node* node) {
    switch (node->kind) {
        case ND_NUM:
        case ND_MEMBER:
        case ND_LVAR:
        case ND_GVAR:
            return node->type;
        case ND_ADD:
        case ND_SUB:
        case ND_MUL:
        case ND_DIV:
            return int_type();
        case ND_ASSIGN:
            return get_type(node->lhs);
        case ND_DEREF:
            return node->type->ptr_to;
        case ND_ADDR:
            return ptr_type();
        case ND_FUNC_CALL:
            return int_type();  // currently only returns int
        case ND_COND:
            return get_type(node->rhs->rhs);
        default:
            return int_type();
    }
}

// get_type_size returns the size of type.
int get_type_size(type* type) {
    switch (type->ty) {
        case INT:
            return 4;
        case PTR:
            return 8;
        case VOID:
            return 1;
        case BOOL:
            return 1;
        case CHAR:
            return 1;
        case ARRAY:
            if (type->array_size == 0) {
                error0("array size is zero.");
            }
            return get_type_size(type->ptr_to) * type->array_size;
        case STRUCT:
            // struct stores size info in itself
            return type->size;
        case TYPEDEF:
            return get_type_size(type->ptr_to);
        default:
            error0("unexpected type->ty comes here");
    }
}

// read_type_and_ident ::= type_info_prefix ident type_info_suffix
define* read_define() {
    define* def = read_define_head();
    read_define_suffix(def);
    return def;
}

/*
read_define_head ::= type "*"* ident
type ::= "int" | "char" | "void" | "bool"
read incopmlete type info and ident, and put them into define as container, then
return it
*/
define* read_define_head() {
    type* type = NULL;
    // for rollback
    token* t = token;

    // read alias
    token* ident = consume_kind(TK_IDENT);
    if (ident) {
        tag* tag = find_tag_struct(ident);
        if (tag) {
            type = tag->type;
        }
    }

    // read type keyword
    token* type_token = consume_kind(TK_TYPE);
    if (type_token) {
        char* name = calloc(100, sizeof(char));
        memcpy(name, type_token->str, type_token->len);
        if (strcmp(name, "int") == 0) {
            type = int_type();
        } else if (strcmp(name, "char") == 0) {
            type = char_type();
        } else if (strcmp(name, "void") == 0) {
            type* t = calloc(1, sizeof(type));
            t->ty = VOID;
            t->size = 1;
            type = t;
        } else if (strcmp(name, "bool") == 0) {
            type* t = calloc(1, sizeof(type));
            t->ty = BOOL;
            t->size = 1;
            type = t;
        }
    }

    // read struct
    if (check_kind(TK_STRUCT)) {
        type* struct_type = define_struct();
        type = struct_type;
    }

    // read enum
    if (check_kind(TK_ENUM)) {
        type* enum_type = define_enum();
        type = enum_type;
    }

    if (type == NULL) {
        if (ident) {
            token = t;
            return NULL;
        } else {
            return NULL;
        }
    }

    // read ptr to
    while (consume("*")) {
        type* t = ptr_type();
        t->ptr_to = type;
        type = t;
    }

    // read ident
    ident = consume_kind(TK_IDENT);
    if (ident == NULL) {
        error0("variable name is not found.");
    }

    define* def = calloc(1, sizeof(define));
    def->ident = ident;
    def->type = type;
    return def;
}

// read_define_suffix ::= ("[" num? "]")*
void read_define_suffix(define* def) {
    type* type = def->type;
    // check if it's array or not;
    while (consume("[")) {
        type* t;
        t = calloc(1, sizeof(type));
        t->ty = ARRAY;
        t->ptr_to = type;
        // size of array is optional.
        t->array_size = 0;
        token* num = NULL;
        if (num = consume_kind(TK_NUM)) {
            t->array_size = num->val;
        }
        type = t;
        expect("]");
    }
    def->type = type;
}

// todo: learn here
int align_to(int n, int align) { return (n + align - 1) & ~(align - 1); }

// in general, when calling push_tag, the type should be complete.
tag* push_tag(token* tok, type* type, bool complete) {
    tag* tag = calloc(1, sizeof(tag));
    tag->next = tags;
    tag->type = type;
    tag->name = calloc(100, sizeof(char));
    memcpy(tag->name, tok->str, tok->len);
    tag->len = tok->len;
    tag->incomplete = !complete;
    tags = tag;
    return tag;
}

// find tag from already existing tags
type* find_tag_struct(token* tok) {
    for (tag* tag = tags; tag; tag = tag->next) {
        if (tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len)) {
            return tag->type;
        }
    }
    return NULL;
}

tag* find_or_register_tag(token* tok) {
    for (tag* tag = tags; tag; tag = tag->next) {
        if (tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len)) {
            return tag;
        }
    }

    type* int_t = int_type();
    int_t->incomplete = true;
    return push_tag(tok, int_t, false);
}

// find_member_struct finds a member of struct
member* find_member_struct(token* tok, type* type) {
    char* member_name = calloc(100, sizeof(char));
    memcpy(member_name, tok->str, tok->len);

    for (member* m = type->members; m; m = m->next) {
        if (strcmp(member_name, m->name) == 0) {
            return m;
        }
    }
    error0("member ident is not found");
    return NULL; // dummy
}

// find and return lvar from locals and globals
lvar* find_variable(token* tok) {
    for (lvar* var = locals[cur_scope_depth]; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }

    for (lvar* var = globals[0]; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

// find_enum_var returns enum as int if a enum whose name is corresponding to
// the given token. if it cannot find the enum name, it returns null. enum name
// here means `aaa` of `enum enum {aaa, bbb}`
node* find_enum_var(token* tok) {
    char* name = calloc(100, sizeof(char));
    memcpy(name, tok->str, tok->len);

    for (enumvar* e = enum_vars; e; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            return new_num(e->value);
        }
    }

    return NULL;
}

// predefined const variable. they are treated as pure int.
// it's also fine to add codes in gen to evaluate these token..
node* convert_predefined_keyword_to_num() {
    if (consume_kind(TK_TRUE)) {
        return new_num(1);
    }
    if (consume_kind(TK_FALSE)) {
        return new_num(0);
    }
    if (consume_kind(TK_NULL)) {
        return new_num(0);
    }
    if (consume_kind(TK_SEEKEND)) {
        return new_num(2);
    }
    if (consume_kind(TK_SEEKCUR)) {
        return new_num(1);
    }
    if (consume_kind(TK_SEEKSET)) {
        return new_num(0);
    }
    if (consume_kind(TK_ERRNO)) {
        error0("errno is not supported");
    }
    if (consume_kind(TK_STDERR)) {
        error0("stderr is not supported");
    }

    return NULL;
}

// from codegen.c
// sets of alias based on byte size.
// ex.
// dil  = least 1byte of rdi
// di   = least 2bytes of rdi
// edi  = least 4bytes of rdi
char* argreg1[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
char* argreg2[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
char* argreg4[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
char* argreg8[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// when we use jmp with label, we must increment this value.
int if_sequence = 0;
int break_sequence = 0;
int continue_sequence = 0;

// note: be careful when you change here.
// it's not easy to find bugs in assembler.
void gen(node* node) {
    if (node == NULL) {
        error0("node must not be null");
    }
    int id = if_sequence;
    int original_brk = 0;
    int original_cnt = 0;
    int num_args = 0;
    type* type;
    node* n;

    switch (node->kind) {
        case ND_LOGAND:
            if_sequence++;
            // evalulate from left item, and once false is found, it leaves here
            // and jump to else clause
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .lfalse%d\n", id);
            gen(node->rhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .lfalse%d\n", id);
            // when it comes here, cond is true so push 1 as true
            printf("  push 1\n");
            printf("  jmp .lend%d\n", id);
            printf(".lfalse%d:\n", id);
            printf("  push 0\n");
            printf(".lend%d:\n", id);
            return;
        case ND_LOGOR:
            if_sequence++;
            // evaluate from left item, and once true is found, it leaves here
            // and jump to else caluse
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  jne .ltrue%d\n", id);
            gen(node->rhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  jne .ltrue%d\n", id);
            // when it comes here, cond is false so push 1 as true
            printf("  push 0\n");
            printf("  jmp .lend%d\n", id);
            printf(".ltrue%d:\n", id);
            printf("  push 1\n");
            printf(".lend%d:\n", id);
            return;
        case ND_BITNOT:
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  not rax\n");
            printf("  push rax\n");
            return;
        case ND_NOT:
            // just compare evaluated value with 0
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  sete al\n");  // result is stored in al(=rax)
            printf("  movzb rax, al\n");
            printf("  push rax\n");
            return;
        case ND_PRE_INC:
            // evaluate `++a` as two operation
            // 1. a + 1
            // 2. push a
            // nd_assign pushes its lhs variable's value at last, so it's fine
            // just to gen nd_assign here.
            n = new_node(ND_ASSIGN);
            n->lhs = node->lhs;
            n->rhs = new_binary(ND_ADD, node->lhs, new_num(1));
            gen(n);
            return;
        case ND_SUF_INC:  // a++
            // do operation of nd_pre_inc first
            // after that, decrement the top value on stack
            gen(new_binary(ND_PRE_INC, node->lhs, NULL));
            printf("  push 1\n");
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  sub rax, rdi\n");
            printf("  push rax\n");
            return;
        case ND_PRE_DEC:
            n = new_node(ND_ASSIGN);
            n->lhs = node->lhs;
            n->rhs = new_binary(ND_SUB, node->lhs, new_num(1));
            gen(n);
            return;
        case ND_SUF_DEC:
            gen(new_binary(ND_PRE_DEC, node->lhs, NULL));
            printf("  push 1\n");
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  add rax, rdi\n");
            printf("  push rax\n");
            return;
        case ND_STRING:
            // todo: learn push offset {symbol, label}
            printf("  push offset .lc%d\n", node->string->index);
            return;
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_GVAR_DEF:
            /*
                int a = 3;
                char b[] = "foobar";
                int *c = &a;
                char *d = b + 3;

                register int value with type(size) and lable in .data space
                ex: int a = 3; is translated as:

                a:
                  .long 3

                register string value with type(size) and label in .data space
                ex: char b[] = "foobar\0";

                b:
                  .byte 0x66 // 'f'
                  .byte 0x6f // 'o'
                  .byte 0x6f // 'o'
                  .byte 0x62 // 'b'
                  .byte 0x61 // 'a'
                  .byte 0x72 // 'r'
                  .byte 0    // '\0'

                register address value with type(size) and label
                we can use labels which are defined beforehand
                ex: int *c = &a;
                c:
                  .quad a

                we can do some calc in .data space
                ref: https://mfumi.hatenadiary.org/entry/20121231/1356880556
                d:
                  .quad b + 3

                // todo: learn what size are available in .data space
                .quad
                .byte
                .string
                .ascii
                .zero
            */

            printf("%s:\n", node->varname);

            if (node->var->init == NULL) {
                printf("  .zero %d\n", node->varsize);
                return;
            }

            // array
            if (node->var->init->kind == ND_BLOCK) {
                // array initializer, ex: int a[] = {1, 2, 3};
                for (int i = 0; node->var->init->block[i]; i++) {
                    gen(node->var->init->block[i]);
                    printf("  pop rax\n");
                    if (node->var->type->ptr_to->size == 1) {
                        printf("  .byte %d\n", (int)node->var->init->block[i]->val);
                    } else if (node->var->type->ptr_to->size == 4) {
                        printf("  .long %d\n", (int)node->var->init->block[i]->val);
                    } else if (node->var->type->ptr_to->size == 8) {
                        printf("  .quad %d\n", (int)node->var->init->block[i]->val);
                    } else {
                        error0("invalid type size for array initializer.");
                    }
                }
                return;
            }

            // when string
            if (node->var->init->kind == ND_STRING) {
                // strings are registed with .lc%d labels in .data space
                // we need to
                // todo: understand in more detail
                if (node->type->ty == ARRAY) {
                    // 1. char a[3] = "hoge"
                    printf("  .string \"%.*s\"\n", node->var->init->string->len, node->var->init->string->value);
                } else {
                    // 2. char *a = "hoge". a is pointer.
                    printf("  .quad .lc%d\n", node->var->init->string->index);
                }
                return;
            }

            // when int
            if (node->var->init->kind == ND_NUM) {
                printf("  .long 0x%x\n", node->var->init->val);
                return;
            }

            // when address(pointer)
            if (node->var->init->kind == ND_ADDR) {
                if (node->var->init->lhs->kind == ND_GVAR) {
                    printf("  .quad %s\n", node->var->init->lhs->varname);
                    return;
                }
                error0("address of local variable cannot be used as global variable initializer.");
            }

            error0("invalid global variable initializer.");
            return;
        case ND_MEMBER:
        case ND_GVAR:
        case ND_LVAR:
            // evaulation of variables means pushing their "values" in stack.
            // 1. evaluate lhs variabel as lvalue == push its address in stack
            gen_val(node);
            // todo: understand in more details
            type = get_type(node);
            if (type && type->ty == ARRAY) {
                // do nothing for array here, address is its value.
                return;
            }
            if (type && type->ty == STRUCT) {
                // struct has value of address only.
                return;
            }

            // 2. load its value and push to stack
            gen_load(type);
            return;
        case ND_ASSIGN:
            // assign expression evaluates the assignment.
            // 1. calculate lhs and put its address in stack
            // 2. calculate rhs and put its value in stack
            // 3. do assignment
            gen_val(node->lhs);
            gen(node->rhs);
            // store
            gen_store(get_type(node->lhs));
            return;
        case ND_DEREF:
            gen(node->lhs);
            gen_load(node->type);
            return;
        case ND_ADDR:
            gen_val(node->lhs);
            return;
        case ND_FUNC_CALL:
            // arg evaluation
            for (int i = 0; node->block[i] != NULL; i++) {
                gen(node->block[i]);
                num_args++;
            }
            if (num_args > 6) {
                error_at0(token->str, "invalid number of args. lteq 6.");
            }

            // arg passing
            // arguments are passed from right to left
            // it means last argument is pushed first.
            // also we can only use registers for first 6 args.
            // the order is: rdi, rsi, rdx, rcx, r8, r9
            // here, stack has args in reverse order
            for (int i = num_args - 1; i >= 0; i--) {
                printf("  pop %s\n", argreg8[i]);
            }

            // for alignment of stack(16 bytes)
            // when stack is aligned, rax must be 0
            printf("  mov rax, 0\n");
            printf("  call %s\n", node->funcname);
            // after returning from function call, the result is in rax, so put it
            // into stack.
            printf("  push rax\n");
            return;
        case ND_FUNC_DEF:
            // global var's scope is 0
            cur_scope_depth--;

            printf(".text\n");
            printf(".global %s\n", node->funcname);
            printf("%s:\n", node->funcname);

            // prologue: allocate stack frame for function
            printf("  push rbp\n");  // save caller's rbp
            printf("  mov rbp, rsp\n");

            // calculate offset and reserve space for local variables
            // the offset is calculated in parse.c
            // we use this space for temporal local variables for this func.
            if (locals[cur_scope_depth + 1]) {
                int offset = locals[cur_scope_depth + 1]->offset;
                printf("  sub rsp, %d\n", offset);
            }

            // copy arguments from registers to space right below rbp.
            // todo: doesn't this conflict with local variables?
            cur_scope_depth++;
            for (int i = 0; node->args[i]; i++) {
                if (node->args[i]->varsize == 1) {
                    printf("  mov [rbp-%d], %s\n", node->args[i]->offset,
                           argreg1[i]);
                } else if (node->args[i]->varsize == 4) {
                    printf("  mov [rbp-%d], %s\n", node->args[i]->offset,
                           argreg4[i]);
                } else if (node->args[i]->varsize == 8) {
                    printf("  mov [rbp-%d], %s\n", node->args[i]->offset,
                           argreg8[i]);
                } else {
                    error0("invalid type size for argument.");
                }
            }

            // generate codes for body
            gen(node->lhs);

            // epilogue is in nd_return
            // when no return statement, return 0 for safe.
            printf(".lend_func%s:\n", node->funcname);
            printf("  mov rax, 0\n");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");

            cur_scope_depth--;
            return;
        case ND_RETURN:
            if (node->lhs) {
                gen(node->lhs);
            } else {
                // when return value is not provided(e.g. `return;`), just
                // return 0;
                gen(new_num(0));
            }
            printf("  pop rax\n");  // set evaulated value on rax(this is abi
                                    // requirement)
            printf("  jmp .lend_func%s\n", node->funcname);
            return;
        case ND_BLOCK:
            // todo: locals[cur_scope_depth] must be recovered when exiting scope
            for (int i = 0; node->block[i] != NULL; i++) {
                gen(node->block[i]);
                printf("  pop rax\n");
            }
            // push 0 for block result value. (since block usually returns no value)
            printf("  push 0\n");
            return;
        case ND_IF:
            if_sequence++;
            // lhs: cond
            // rhs: main or else
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .lelse%d\n", id);  // jump to else
            gen(node->rhs);
            printf("  jmp .lend%d\n", id);
            printf(".lelse%d:\n", id);

            if (node->rhs->kind == ND_ELSE) {
                gen(node->rhs->rhs);
            }
            printf(".lend%d:\n", id);
            return;
        case ND_ELSE:
            // this is child of nd_if, so it does nothing here
            // this node structure is just for parser.
            gen(node->lhs);
            return;
        case ND_WHILE:
            if_sequence++;
            original_brk = break_sequence;
            original_cnt = continue_sequence;
            break_sequence = id;
            continue_sequence = id;

            printf(".lbegin%d:\n", id);
            gen(node->lhs);  // cond
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .lend%d\n", id);
            gen(node->rhs);  // body
            printf(".lcontinue%d:\n", id);
            printf("  jmp .lbegin%d\n", id);
            printf(".lend%d:\n", id);

            break_sequence = original_brk;
            continue_sequence = original_cnt;
            return;
        case ND_FOR:
            if_sequence++;
            original_brk = break_sequence;
            original_cnt = continue_sequence;
            break_sequence = id;
            continue_sequence = id;

            // a: init
            if (node->lhs->lhs != NULL) {
                gen(node->lhs->lhs);
            }

            printf(".lbegin%d:\n", id);

            // b: cond
            if (node->lhs->rhs != NULL) {
                gen(node->lhs->rhs);
            } else {
                printf("  push 1\n");  // if cond is not given, it's always
                                      // true. ex: for(;;) {} == while(true) {}
            }
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .lend%d\n", id);

            // d: body
            gen(node->rhs->rhs);

            // c: post ops
            printf(".lcontinue%d:\n", id);
            if (node->rhs->lhs != NULL) {
                gen(node->rhs->lhs);
            }

            printf("  jmp .lbegin%d\n", id);
            printf(".lend%d:\n", id);

            break_sequence = original_brk;
            continue_sequence = original_cnt;
            return;
        case ND_BREAK:
            if (break_sequence == 0) {
                error0("currently not in for, while, switch");
            }
            printf("  jmp .lend%d\n", break_sequence);
            return;
        case ND_CONTINUE:
            if (continue_sequence == 0) {
                error0("currently not in for, while, switch");
            }
            printf("  jmp .lcontinue%d\n", continue_sequence);
            return;
        case ND_SWITCH:
            if_sequence++;
            original_brk = break_sequence;
            break_sequence = id;

            gen(node->lhs);  // evaluate switch expression
            printf("  pop rax\n");
            printf("  mov r10, rax\n");  // move switch value to r10

            // compare switch value with case values
            // generate jump to case label
            for (node* n = node->next_case; n; n = n->next_case) {
                if (n->kind == ND_DEFAULT_CASE) {
                    continue;
                }
                printf("  cmp r10, %d\n", n->val);
                printf("  je .lcase%d_%d\n", id, n->val);
            }

            // jump to default case if exists, otherwise jump to end
            node* default_case = NULL;
            for (node* n = node->next_case; n; n = n->next_case) {
                if (n->kind == ND_DEFAULT_CASE) {
                    default_case = n;
                    break;
                }
            }

            if (default_case) {
                printf("  jmp .lcase%d_default\n", id);
            } else {
                printf("  jmp .lend%d\n", id);
            }

            // generate case labels
            for (node* n = node->next_case; n; n = n->next_case) {
                if (n->kind == ND_DEFAULT_CASE) {
                    printf(".lcase%d_default:\n", id);
                } else {
                    printf(".lcase%d_%d:\n", id, n->val);
                }
            }

            // generate body
            gen(node->rhs);

            printf(".lend%d:\n", id);

            break_sequence = original_brk;
            return;
        case ND_CASE:
            // this is only a marker for parser.
            // nothing to generate here.
            printf("  push 0\n");
            return;
        case ND_DEFAULT_CASE:
            // this is only a marker for parser.
            // nothing to generate here.
            printf("  push 0\n");
            return;
        case ND_NULL_STMT:
            printf("  push 0\n");
            return;
        default:
            break;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    // handle pointer arithmetic
    type = get_type(node);
    if (type && type->ty == PTR) {
        // if lhs is pointer, we need to convert rhs to the size of pointed type.
        // ex: p + 1 => p + 1 * sizeof(*p)
        int size = get_type_size(type->ptr_to);
        printf("  imul rdi, %d\n", size);
    }

    switch (node->kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_MOD:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            printf("  mov rax, rdx\n");
            break;
        case ND_SHL:
            printf("  mov cl, dil\n");
            printf("  shl rax, cl\n");
            break;
        case ND_SHR:
            printf("  mov cl, dil\n");
            printf("  shr rax, cl\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NE:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_BITXOR:
            printf("  xor rax, rdi\n");
            break;
        case ND_BITOR:
            printf("  or rax, rdi\n");
            break;
        case ND_BITAND:
            printf("  and rax, rdi\n");
            break;
    }

    printf("  push rax\n");
}

/*
gen_val treat the given node(nd_lvar) as lvalue.
that means baricc pushes its "address" in stack here for assign operation.
*/
void gen_val(node* node) {
    // when node is nd_deref, treat it as right-value(= push its "value" in
    // stack)
    if (node->kind == ND_DEREF) {
        gen(node->lhs);
        return;
    }

    if (node->kind == ND_LVAR) {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\\n", node->offset);
        printf("  push rax\n");
        return;
    }

    if (node->kind == ND_GVAR) {
        printf("  push offset %s\n", node->varname);
        return;
    }

    if (node->kind == ND_MEMBER) {
        // 1. push address of base struct/union
        gen_val(node->lhs);
        // 2. add offset of member
        printf("  pop rax\n");
        printf("  add rax, %d\n", node->member->offset);
        printf("  push rax\n");
        return;
    }

    error0("invalid assignment target");
}

// gen_load loads value from given address which is on the top of stack
void gen_load(type* type) {
    if (type->ty == ARRAY) {
        return;  // array is treated as pointer so its value is its address
    }
    if (type->ty == STRUCT) {
        return;  // struct is also treated as a pointer(address)
    }

    printf("  pop rax\n");

    if (type->size == 1) {
        printf("  movsx rax, byte ptr [rax]\n");
    } else if (type->size == 4) {
        printf("  movsxd rax, dword ptr [rax]\n");
    } else if (type->size == 8) {
        printf("  mov rax, [rax]\n");
    } else {
        error0("invalid type size for load.");
    }

    printf("  push rax\n");
}

// gen_store stores value to the address(top of stack) with value(second top of
// stack)
void gen_store(type* type) {
    printf("  pop rdi\n");  // value
    printf("  pop rax\n");  // address

    if (type->size == 1) {
        printf("  mov [rax], dil\n");
    } else if (type->size == 4) {
        printf("  mov [rax], edi\n");
    } else if (type->size == 8) {
        printf("  mov [rax], rdi\n");
    } else {
        error0("invalid type size for store.");
    }

    // store rdi's value = evaluation result
    printf("  push rdi\n");
}

// from main.c
int main(int argc, char** argv) {
    // make a linkedlist of token from all of the given files.
    token* t = NULL;
    for (int i = 1; i < argc; i++) {
        filename = argv[i];
        printf("; i = %d, filename=%s\n", i, filename);
        user_input = read_file(filename);
        t = tokenize();
        if (!token) {
            token = t;
        } else {
            token* tt = token;
            while (true) {
                if (tt->next->kind == TK_EOF) {
                    tt->next = t;
                    break;
                }
                tt = tt->next;
            }
            // replace tk_eof of current linkedlist with first token of next file
            // so that we can chain multiple files.
            tt->next = t;
        }
    }

    cur_scope_depth = 0;
    program();

    // specify what syntax to use for this output.
    printf(".intel_syntax noprefix\n");

    // todo: learn .bss
    // .bss
    // declare global variables without initializers
    printf(".bss\n");  // todo: what's this
    for (int i = 0; code[i]; i++) {
        if (code[i]->kind == ND_GVAR_DEF) {
            if (code[i]->var->init == NULL) {
                gen(code[i]);
            }
        }
    }

    // todo: learn .data
    // .data
    // declare global variables with initializers
    printf(".data\n");
    for (stringtoken* str = strings; str; str = str->next) {
        printf(".lc%d:\n", str->index);
        // add length check to prevent buffer overread in case of unclosed string or similar bug, although tokenize.c attempts to prevent this.
        printf("  .string \"%.*s\"\n", str->len, str->value);
    }
    for (int i = 0; code[i]; i++) {
        if (code[i]->kind == ND_GVAR_DEF) {
            if (code[i]->var->init != NULL) {
                gen(code[i]);
            }
        }
    }

    // todo: learn .text
    // .text
    // declare functions
    // the first function must be entrypoint, which should be main.
    printf(".text\n");
    for (int i = 0; code[i]; i++) {
        if (code[i]->kind == ND_FUNC_DEF) {
            gen(code[i]);
        }
    }

    return 0;
}
