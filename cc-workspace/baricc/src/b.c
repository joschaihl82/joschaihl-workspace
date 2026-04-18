#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
Token is the smallest meaningful unit of characters in given program.
It doesn't have semantics itself, but only when kind is TK_NUM, it can have
value of the number
*/
struct Token {
    TokenKind kind;  // type of the token
    Token* next;     // next token. we use linkedlist instead of array.
    int val;         // actual value when kind == TK_NUM
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
    ND_MEMBER,  // TODO: better to name ND_MEMBER_ACCESS
    ND_BREAK,
    ND_CONTINUE,
    ND_PRE_INC,  // ++a

    ND_PRE_DEC,  // --a
    ND_SUF_INC,  // a++
    ND_SUF_DEC,  // a--
    ND_NOT,      // !a
    ND_BITNOT,   // ~a
    ND_BITOR,    // |
    ND_BITXOR,   // ^
    ND_BITAND,   // &
    ND_LOGOR,    // ||
    ND_LOGAND,   // &&

    ND_TERNARY,
    ND_SWITCH,
    ND_CASE,
    ND_PADDING
} NodeKind;

/*
Node is a container to show structure of program after parse.

TODO:
Currently this Node struct has too many members which is for specific usecases.
we should have more specialized Node (e.g. VarNode, FuncNode, etc..) to make
code cleaner.
*/
struct Node {
    NodeKind kind;
    Node* lhs;  // Node has at most two children. lhs and rhs.
    Node* rhs;
    Node** block;  // used when kind == ND_BLOCK. To contains stmts in block or
    // items in initializer(this should be moved to another field)
    int val;         // used when kind == ND_NUM
    int offset;      // used when kind == ND_LVAR
    char* funcname;  // used when kind == ND_FUNC_CALL, or ND_FUNC_DEF
    Node** args;     // used when kind == ND_FUNC_DEF
    Type* type;      // used when kind == ND_LVAR
    char* varname;   // used when kind == ND_GVAR, ND_LVAR
    int varsize;  // used when kind == ND_GVAR, ND_LVAR Byte. TODO: varsize and
                  // offset can be unified.
    StringToken* string;  // used when kind == ND_STRING
    LVar* var;            // used when kind == ND_LVAR, ND_GVAR_DEF
    Member* member;       // used when kind == ND_MEMBER
    Node* next_case;  // used when kind == ND_CASE. TODO: this should moved out
                      // to like SwitchNode
    Node* default_case;  // used when kind == ND_DEFAULT TODO: this should moved
                         // out to like SwitchNode
    int case_label;  // used when kind == ND_SWITCH TODO: this should moved out
                     // to like SwitchNode
    int size;        // used when kind == ND_PADDING
};

/*
Type contains information of type of variables.

TODO:
- rename ptr_to as "of" because array also uses ptr_to now.
*/
typedef enum { INT, CHAR, PTR, ARRAY, STRUCT } TypeKind;
struct Type {
    TypeKind ty;
    struct Type*
        ptr_to;  // used when ty == PTR, ARRAY. 指し示す先の変数の型を持つ
    size_t array_size;
    Member* members;
    int size;
    bool incomplete;
};

/*
LVar represents variable.
baricc stores variables as linkedlist for global and each function
TODO: this is used for global variables as well, so it should be renamed as
VAR..
*/
struct LVar {
    LVar* next;
    char* name;
    int len;     // length of name
    int offset;  // offset from rbp
    Type* type;
    enum { LOCAL, GLOBAL } kind;
    Node* init;  // initializer
};

LVar* find_variable(Token* tok);
Node* find_enum_var(Token* tok);

/*
StringToken is container for string.
baricc stores all strings it encounters as linedlist and generate in assebmly at
last.
*/
struct StringToken {
    char* value;  // string value
    int index;    // the string's index in all strings baricc finds in the given
                  // program
    StringToken* next;
};

/*
Tag is alias to type name. struct's tag is also tag.
Type itself doesn't have name.
*/
struct Tag {
    Tag* next;
    char* name;
    Type* type;
};

/*
Member is definition of members of a struct.
ex:
struct Example {
    int a;  --> Member{next: b's member struct, ty: int, name: "a", offset: size
of int(=4) } char* b;  --> Member{next: NULL, ty: ptr of char, name: "b",
offset: size of ptr of char(=8) }
}
*/
struct Member {
    Member* next;  // next member definition. we use linkedlist instaed of array
    Type* ty;      // Type of the member
    char* name;    // the member's name
    int offset;    // offset of the member from the starting point of base
                   // variable. actually, offset = sum of sizes of previous
                   // members
};

/*
Define is a container to deliver type annotation of return value or some
variables.
*/
struct Define {
    Token* ident;
    Type* type;
};

/*
EnumVar is a container of enum members baricc finds.
baricc stores all enum members as linkedlist and replace them as int at compile
time.
*/
struct EnumVar {
    EnumVar* next;
    char* name;
    int value;
};

// tokenize the given program
bool consume(char* op);
Token* consume_kind(TokenKind kind);
bool check(char* op);
bool check_kind(TokenKind kind);
void expect(char* op);
Token* expect_kind(TokenKind kind);
int expect_number();
void advance_token();
bool at_eof();
Token* new_token(TokenKind kind, Token* cur, char* str, int len);
Token* tokenize();

// parse program and convert it into node tree from tokens.
Node* new_node(NodeKind kind);
Node* new_binary(NodeKind kind, Node* lhs, Node* rhs);
Node* new_num(int val);
void program();
Node* stmt();
Node* block();
Node* func_def(Define* def);
Node* define_arg();
Node* expr();
Node* assign();
Node* expr();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();
Node* conditional();
Node* logor();
Node* logand();
Node* bitor ();
Node* bitxor();
Node*bitand();
Node* define_variable(Define* def, LVar** varlist);
Node* variable(Token* tok);
Node* local_variable_init(Node* node);
Define* read_define();
Define* read_define_head();
void read_define_suffix(Define* def);
Type* type_annotation();
Type* define_struct();
Member* find_member(Token* tok, Type* type);
void push_tag(Token* tok, Type* type, bool is_complete);
Tag* find_tag(Token* tok);
Tag* find_or_register_tag(Token* tok);
Node* struct_ref(Node* node);
bool define_typedef();
Type* define_enum();
Type* int_type();
Node* ptr_conversion(Node* node, Node* right);
Node* initializer(Type* type);
void initializer_helper(Type* type, Node* init, int* i);
Node* convert_predefined_keyword_to_num();
Define* read_define();
void register_lval(Token* tok, Type* type);

Type* get_type(Node* node);
int get_type_size(Type* type);

// generate codes fron nodes
void gen(Node* node);
void gen_val(Node* node);

// util
// TODO: support rest parameter..
// void error_at(char* loc, char* fmt, ...);
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
char* get_token_kind_name(TokenKind kind);

// global variables
extern Token* token;
extern char* user_input;
extern char* filename;
extern Node* code[1000];
extern LVar* locals[100];
extern int cur_scope_depth;
extern LVar* globals[100];
extern StringToken* strings;
extern Tag* tags;
extern EnumVar* enum_vars;
extern int if_sequence;
extern int break_sequence;
extern int continue_sequence;

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
    // for making EOF ops easier
    if (size == 0 || buf[size - 1] != '\n') {
        buf[size++] = '\n';
    }

    buf[size] = '\n';
    fclose(fp);
    return buf;
}

// for debugging
// TODO(critical): Now stderr is not available.. so when we do self-copmile, we
// cannot use fprintf at all.
void print_token(Token *tok) {
    if (tok == NULL) {
        fprintf(stderr, "[DEBUG] token is null\n");
    } else {
        char name[100] = {0};
        memcpy(name, tok->str, tok->len);
        fprintf(stderr, "[DEBUG] token: %2d %s\n", tok->kind, name);
    }
}

void print_type(Type *type) {
    if (type == NULL) {
        fprintf(stderr, "[DEBUG] type is null");
    } else {
        char *name;
        switch (type->ty) {
            case INT:
                name = "INT";
                break;
            case CHAR:
                name = "CHAR";
                break;
            case PTR:
                name = "PTR";
                break;
            case ARRAY:
                name = "ARRAY";
                break;
            case STRUCT:
                name = "STRUCT";
            default:
                break;
        }
        fprintf(stderr, "[DEBUG] type = %s ", name);
    }

    if (type && (type->ty == ARRAY || type->ty == PTR)) {
        return;
    }

    fprintf(stderr, "\n");
}

void print_node(Node *node) {
    if (node == NULL) {
        fprintf(stderr, "[DEBUG] node is null\n");
    } else {
        if (node->funcname) {
            fprintf(stderr, "[DEBUG] node->funcname = %s\n", node->funcname);
        }
        if (node->varname) {
            fprintf(stderr, "[DEBUG] node->varname = %s\n", node->varname);
        }
        if (node->varsize) {
            fprintf(stderr, "[DEBUG] node->varsize = %d\n", node->varsize);
        }
        if (node->kind) {
            fprintf(stderr, "[DEBUG] node->kind = %d\n", node->kind);
        }
    }
}

void error0(char *fmt) {
    fprintf(stderr, fmt);
    fprintf(stderr, "\n");
    exit(1);
}

void error1(char *fmt, char *val) {
    fprintf(stderr, fmt, val);
    fprintf(stderr, "\n");
    exit(1);
}

void error2(char *fmt, char *val1, char *val2) {
    fprintf(stderr, fmt, val1, val2);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at0(char *loc, char *fmt) {
    // get the line where loc exists.
    // line is : head pos of the line
    // end is  : tail pos of the line
    char *line = loc;
    while (user_input < line && line[-1] != '\n') line--;

    char *end = loc;
    while (*end != '\n') end++;

    // find which line num the line is.
    int line_num = 1;
    char *p;
    for (p = user_input; p < line; p++)
        if (*p == '\n') line_num++;

    // print the line with line number and filename
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (end - line), line);

    // point where error is happening with "^"
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, fmt);
    exit(1);
}

void error_at1(char *loc, char *fmt, char *val) {
    char *line = loc;
    while (user_input < line && line[-1] != '\n') line--;

    char *end = loc;
    while (*end != '\n') end++;

    int line_num = 1;
    for (char *p = user_input; p < line; p++)
        if (*p == '\n') line_num++;

    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (end - line), line);

    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");  // pos個の空白を出力
    fprintf(stderr, fmt, val);
    exit(1);
}

void error_at2(char *loc, char *fmt, char *val1, char *val2) {
    char *line = loc;
    while (user_input < line && line[-1] != '\n') line--;

    char *end = loc;
    while (*end != '\n') end++;

    int line_num = 1;
    for (char *p = user_input; p < line; p++)
        if (*p == '\n') line_num++;

    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (end - line), line);

    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");  // pos個の空白を出力
    fprintf(stderr, fmt, val1, val2);
    exit(1);
}

// TODO: find better way to get enum name.
char *get_token_kind_name(TokenKind kind) {
    switch (kind) {
        case TK_RESERVED:
            return "TK_RESERVED";
        case TK_NUM:
            return "TK_NUM";
        case TK_EOF:
            return "TK_EOF";
        case TK_IDENT:
            return "TK_IDENT";
        case TK_RETURN:
            return "TK_RETURN";
        case TK_IF:
            return "TK_IF";
        case TK_ELSE:
            return "TK_ELSE";
        case TK_WHILE:
            return "TK_WHILE";
        case TK_FOR:
            return "TK_FOR";
        case TK_TYPE:
            return "TK_TYPE";
        case TK_SIZEOF:
            return "TK_SIZEOF";
        case TK_STRING:
            return "TK_STRING";
        case TK_STRUCT:
            return "TK_STRUCT";
        case TK_TYPEDEF:
            return "TK_TYPEDEF";
        case TK_ENUM:
            return "TK_ENUM";
        case TK_BREAK:
            return "TK_BREAK";
        case TK_CONTINUE:
            return "TK_CONTINUE";
        case TK_SWITCH:
            return "TK_SWITCH";
        case TK_CASE:
            return "TK_CASE";
        case TK_DEFAULT:
            return "TK_DEFAULT";
        case TK_TRUE:
            return "TK_TRUE";
        case TK_FALSE:
            return "TK_FALSE";
        case TK_NULL:
            return "TK_NULL";
        case TK_SEEKSET:
            return "TK_SEEKSET";
        case TK_SEEKCUR:
            return "TK_SEEKCUR";
        case TK_SEEKEND:
            return "TK_SEEKEND";
        case TK_ERRNO:
            return "TK_ERRNO";
        case TK_STDERR:
            return "TK_STDERR";
        default:
            error0("tried to read unexpected token kind");
    }
}

Token *token;
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
// otherwise return NULL.
Token *consume_kind(TokenKind kind) {
    if (!check_kind(kind)) {
        return NULL;
    }
    Token *tok = token;
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
bool check_kind(TokenKind kind) { return token->kind == kind; }

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
Token *expect_kind(TokenKind kind) {
    if (!check_kind(kind)) {
        error_at2(token->str, "unexpected token: '%s'\nactual: '%s'\n",
                  get_token_kind_name(kind), get_token_kind_name(token->kind));
    }
    Token *tok = token;
    advance_token();
    return tok;
}

// expect function dedicated for TK_NUM
int expect_number() {
    if (token->kind != TK_NUM) {
        print_token(token);
        error_at0(token->str, "数ではありません\n");
    }
    int val = token->val;
    advance_token();
    return val;
}

// step forward by 1 token
void advance_token() {
    // for debugging
    // print_token(token);

    token = token->next;
    return;
}

bool at_eof() { return token->kind == TK_EOF; }

// build token object.
// token's content is available by utilizing tok->str `and` tok->len.
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    // calloc get memories as requested and clear them with zero(malloc doesn't
    // do zero clearance)
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

bool is_alnum(char p) {
    return ('a' <= p && p <= 'z') || ('A' <= p && p <= 'Z') ||
           ('0' <= p && p <= '9') || (p == '_');
}

typedef struct ReservedWord {
    char *word;
    TokenKind kind;
} ReservedWord;

ReservedWord reserved_words[] = {
    {"return", TK_RETURN},
    {"if", TK_IF},
    {"else", TK_ELSE},
    {"while", TK_WHILE},
    {"for", TK_FOR},
    {"int", TK_TYPE},
    {"void", TK_TYPE},
    {"char", TK_TYPE},
    {"bool", TK_TYPE},
    {"size_t", TK_TYPE},
    {"FILE", TK_TYPE},
    {"sizeof", TK_SIZEOF},
    {"struct", TK_STRUCT},
    {"typedef", TK_TYPEDEF},
    {"enum", TK_ENUM},
    {"break", TK_BREAK},
    {"continue", TK_CONTINUE},
    {"switch", TK_SWITCH},
    {"case", TK_CASE},
    {"false", TK_FALSE},
    {"true", TK_TRUE},
    {"NULL", TK_NULL},
    {"SEEK_END", TK_SEEKEND},
    {"SEEK_SET", TK_SEEKSET},
    {"SEEK_CUR", TK_SEEKCUR},
    {"default", TK_DEFAULT},
    {"errno", TK_ERRNO},
    {"stderr", TK_STDERR},
    {"", TK_EOF},
};

/*
tokenize does tokenize from given program(char*), which is called user_input
note: using blank object as head of linkedlist is said to be one common way..
*/
Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    head.len = 0;
    head.str = NULL;
    Token *cur = &head;

    while (*p) {
        // print_token(cur);

        // skip blanks
        if (isspace(*p)) {
            p++;
            continue;
        }

        // skip block comments
        if (startswith(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q) {
                error_at0(p, "block comment is not closed.\n");
            }
            p = q + 2;
            continue;
        }

        // skip line comments
        if (startswith(p, "//")) {
            while (!startswith(p, "\n")) {
                p++;
            }
            continue;
        }

        // skip #include
        // TODO: skip all preprocessors/macro
        // TODO: imple preprocessor
        if (startswith(p, "#include")) {
            while (!startswith(p, "\n")) {
                p++;
            }
            continue;
        }

        // skip extern
        // TODO: impl extern
        if (startswith(p, "extern")) {
            while (!startswith(p, "\n")) {
                p++;
            }
            continue;
        }

        // try to parse reserved words before ident
        bool did_break = false;
        for (int i = 0; reserved_words[i].kind != TK_EOF; i++) {
            char *word = reserved_words[i].word;
            int len = strlen(word);
            TokenKind kind = reserved_words[i].kind;

            if (startswith(p, word) && !is_alnum(p[len])) {
                cur = new_token(kind, cur, p, len);
                p += len;
                did_break = true;
                break;
            }
        }
        if (did_break) {
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
            startswith(p, ">=") || startswith(p, "->") || startswith(p, "+=") ||
            startswith(p, "-=") || startswith(p, "*=") || startswith(p, "/=") ||
            startswith(p, "++") || startswith(p, "--") || startswith(p, "&&") ||
            startswith(p, "||")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr(".+-*/()<>;={},&[]!~|^?:", *p)) {
            cur = new_token(TK_RESERVED, cur, p, 1);
            p++;
            continue;
        }

        // char literal
        if (*p == '\'') {
            cur = read_char_literal(cur, p);
            p += cur->len;
            continue;
        }

        // string literal
        if (*p == '"') {
            p++;
            char *c = p;
            while (true) {
                if (startswith(c, "\\")) {
                    c += 2;
                    continue;
                }
                if (*c == '"') {
                    break;
                }
                c++;
            }
            int len = c - p;
            cur = new_token(TK_STRING, cur, p, len);
            p = c;
            p += 1;
            continue;
        }

        // a token which is composed of alphabets + some kinds of symbols are
        // ident(variable name, tag(type) name)
        if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z')) {
            char *start = p;
            while (is_alnum(*p)) {
                p++;
            }
            cur = new_token(TK_IDENT, cur, start, (p - start));
            continue;
        }

        // int literal
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            // move as many as length of the digit
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at0("failed to tokenize. %s\n", token->str);
    }

    // print_token(cur);
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

// TODO: read here in detail
//   'a'
//   ^start here
Token *read_char_literal(Token *cur, char *start) {
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

    Token *tok = new_token(TK_NUM, cur, start, p - start);
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
            return '\e';
        case '0':
            return '\0';
        default:
            return c;
    }
}

LVar* locals[100];
LVar* globals[100];
int cur_scope_depth = 0;
StringToken* strings;
Tag* tags;
EnumVar* enum_vars;
Node* current_switch = 0;
// How many function/global variables/global typedef this compiler supports.
// TODO replace Node*[] -> Node**
Node* code[1000];

/**************************
 * node builder
 * *************************/
Node* new_node(NodeKind kind) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node* new_binary(NodeKind kind, Node* lhs, Node* rhs) {
    Node* node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_unary(NodeKind kind, Node* lhs) {
    Node* node = new_node(kind);
    node->lhs = lhs;
    return node;
}

// a util function to build num node
Node* new_num(int val) {
    Node* node = new_node(ND_NUM);
    node->val = val;
    return node;
}

// a util function to build string node
Node* new_string(StringToken* str) {
    Node* node = new_node(ND_STRING);
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
        Token* tk = token;
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

        // DEBUG: if(check_kind(TK_STRUCT)) would be better. will align with code above

        // top level statements which start with type + ident, like "int* hoge"
        // three types of such statements.
        // 1. function def: int main() {}
        // 2. struct def: struct Hoge {} ... this should be placed before this section. if (check_kind(TK_STRUCT)) is better.
        // 3. global var def: int hoge = 1;
        Define* def = read_define_head();

        if (def == NULL) {
            print_token(token);
            error0("def is NULL");
        }
        if (consume("(")) {
            Node* func_node = func_def(def);
            if (func_node) {
                code[i] = func_node;
            } else {
                continue;
            }
        } else if (def->type->ty == STRUCT) {
            /*
            # valid:
            struct Hoge {int a;} hoge;
            struct Hoge {int a;} hoge = {1};
            Hoge hoge;
            Hoge hoge[] = {};

            # invalid:
            struct {int a;} hoge;
            struct {int a;} hoge = {1};
            struct Hoge {int a;}[] a;  ... we cannot define array of struct in oneline.
            */
            if (check("[")) {
                // Hoge hoge[] pattern.
                Node* gvar = define_variable(def, globals);

                // TODO: refactor: should change kind in other place.
                gvar->kind = ND_GVAR_DEF;
                code[i] = gvar;
                expect(";");
            } else {
                // struct Hoge {int a;};
                define_struct();
                expect(";");
                continue;
            }
        } else {
            Node* gvar = define_variable(def, globals);

            // TODO: refactor: should change kind in other place.
            gvar->kind = ND_GVAR_DEF;
            code[i] = gvar;
            expect(";");
        }
        i++;
    }
    code[i] = NULL;
}

// ex:
//   typedef struct Hoge {int a;} Gegi;
//   typedef struct Hoge Geho;
//   typedef int INT;
//   typedef char* String;
// define_typedef ::= "typedef" type_decl alias
// TODO: this doesn't have to return bool.
bool define_typedef() {
    // TODO: do consume_kind(TK_TYPEDEF) here
    Define* def = read_define_head();
    read_define_head(def);

    // register this type(struct) as its name is this identident
    push_tag(def->ident, def->type, false);
    return true;
}

// define_enum ::= "enum" ident "{" (define_enum_member (","
Type* define_enum() {
    if (!consume_kind(TK_ENUM)) {
        return NULL;
    }

    Token* name = consume_kind(TK_IDENT);

    if (name && !check("{")) {
        Tag* tag = find_or_register_tag(name);
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
    `AAA = 10`
    or
    `AAA`
define_enum_member register each enum members as EnumVar
auto assigned number value starts from 1;
*/
void define_enum_member(int* num) {
    Token* tok = expect_kind(TK_IDENT);

    if (consume("=")) {
        *num = expect_number();
    } else {
        *num += 1;
    }

    EnumVar* e = calloc(1, sizeof(EnumVar));
    e->name = calloc(100, sizeof(char));
    memcpy(e->name, tok->str, tok->len);
    e->value = *num;
    e->next = enum_vars;
    enum_vars = e;
    return;
}

Type* int_type() {
    Type* t = calloc(1, sizeof(Type));
    t->ty = INT;
    t->size = 4;
    return t;
}
Type* char_type() {
    Type* t = calloc(1, sizeof(Type));
    t->ty = CHAR;
    t->size = 1;
    return t;
}
Type* ptr_type() {
    Type* t = calloc(1, sizeof(Type));
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
Type* define_struct() {
    if (!consume_kind(TK_STRUCT)) {
        return NULL;
    }

    Token* name = consume_kind(TK_IDENT);

    // if ident(name) is found and this struct doesn't have define_struct_body,
    // retrieve a tag tied with the name
    if (name && !check("{")) {
        Tag* tag = find_or_register_tag(name);
        // when the struct is still imcomplete, this tag is returned as INT,
        // so override it as STRUCT forcibly.
        // TODO: refactor
        tag->type->ty = STRUCT;
        return tag->type;
    }

    expect("{");

    Type* struct_type = calloc(1, sizeof(Type));
    struct_type->ty = STRUCT;

    Member head;
    Member* cur = &head;

    int offset = 0;
    int maxSize = 0;
    while (!consume("}")) {
        // retrieve type info of a member
        Define* def = read_define();
        expect(";");
        // build a members with the member's name and type
        //// name
        Member* m = calloc(1, sizeof(Member));
        m->name = calloc(100, sizeof(char));
        memcpy(m->name, def->ident->str, def->ident->len);
        //// type
        m->ty = def->type;
        int size = get_type_size(def->type);

        /*
        calc offset for each member.
        here it just defines offset for each members.
        TODO:
        with initializer: we need ND_PADDING
        without initializer: we don't need ND_PADDING
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
        // TODO: learn in more detail here.
        offset = align_to(offset, size);
        m->offset = offset;
        offset += size;
        cur->next = m;
        cur = m;

        if (maxSize <= 8 && maxSize <= size) {
            maxSize = size;
        }
    }

    struct_type->members = head.next;
    struct_type->size = align_to(offset, maxSize);

    // register to tags
    if (name) {
        push_tag(name, struct_type, true);
    }

    return struct_type;
}

/*
func_def ::= read_define "(" (define_arg ("," define_ar)*)? ")" block
NOTE: c cannot pass array as arg. so we cannot use read_define for arg.

Node{
    kind: ND_FUNC_DEF
    funcname: function name
    args: array of Node(ND_LVAR). NOTE: register args as local variables too!
    lhs: function body as block
}
*/
Node* func_def(Define* def) {
    // one LVar linked list per one function.
    cur_scope_depth++;

    Node* node = new_node(ND_FUNC_DEF);

    // function name
    node->funcname = calloc(100, sizeof(char));
    memcpy(node->funcname, def->ident->str, def->ident->len);
    node->args = calloc(10, sizeof(Node*));

    // function args
    for (int i = 0; !consume(")"); i++) {
        if (i != 0) expect(",");
        node->args[i] = define_arg();
    }

    // NOTE: int hoge(); is prototype decl.
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
Node* define_arg() {
    Define* arg_def = read_define_head();
    if (arg_def == NULL) {
        char* name[100] = {0};
        memcpy(name, token->str, token->len);
        error1("invalid token comes here. %d", name);
    }

    // extract arg as ND_LVAR and register it as local variable
    return define_variable(arg_def, locals);
}

// TODO: this eBNF is nore up-to-date.
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
Node* stmt() {
    Node* node;

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
        ND_IF:
            lhs: cond
            rhs: main

        # with else
        ND_IF:
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
            Node* els = new_node(ND_ELSE);
            els->lhs = node->rhs;
            els->rhs = stmt();
            node->rhs = els;
        }
        return node;
    }

    if (consume_kind(TK_WHILE)) {
        Node* node = new_node(ND_WHILE);
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
        Node* node = new_node(ND_SWITCH);
        expect("(");
        node->lhs = expr();  // switch's flag expr
        expect(")");

        // To allow nested switch, escape current_switch
        Node* sw = current_switch;
        current_switch = node;

        // ND_BLOCK comes here, and it has many ND_CASE and one ND_DEFAULT stmt
        node->rhs = stmt();

        current_switch = sw;
        return node;
    }

    if (consume_kind(TK_CASE)) {
        // consider `case 1:` this line as one stmt which outputs label only.
        if (!current_switch) {
            error0("stray case");
        }
        Token* t = token;
        Token* ident = consume_kind(TK_IDENT);
        Node* num_node = NULL;
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

        Node* node = new_node(ND_CASE);
        node->val = val;
        // To check labels from the top one.
        node->next_case = current_switch->next_case;
        current_switch->next_case = node;
        return node;
    }

    if (consume_kind(TK_DEFAULT)) {
        /*
        consider `default:` as one stmt which outputs label only.
        */
        if (!current_switch) {
            error0("stray case");
        }
        Node* default_node = new_node(ND_CASE);
        expect(":");
        current_switch->default_case = default_node;
        return default_node;
    }

    if (consume_kind(TK_FOR)) {
        /*
        ND_FOR:
            lhs:
                lhs: stmt(initializer)
                rhs: cond to loop again
            rhs:
                lhs: post ops
                rhs: body to loop
        */
        Node* node = new_node(ND_FOR);
        Node* left = new_node(ND_FOR_LEFT);
        Node* right = new_node(ND_FOR_RIGHT);
        node->lhs = left;
        node->rhs = right;

        expect("(");
        if (!consume(";")) {
            // To allow variable declaration, using stmt.
            // TODO: modify stmt not to allow if/while in this part
            left->lhs = stmt();
        }

        if (!consume(";")) {
            left->rhs = expr();
            expect(";");
        }

        if (!consume(")")) {
            right->lhs = expr();  // TODO: this should be stmt?
            expect(")");
        }

        right->rhs = stmt();
        return node;
    }

    if (consume_kind(TK_BREAK)) {
        Node* node = new_node(ND_BREAK);
        expect(";");
        return node;
    }

    if (consume_kind(TK_CONTINUE)) {
        Node* node = new_node(ND_CONTINUE);
        expect(";");
        return node;
    }

    if (check("{")) {
        return block();
    }

    // decl new variable with/without initializer.
    Define* def = read_define_head();
    if (def != NULL) {
        Node* node = define_variable(def, locals);
        // convert ND_LVAR node to ND_ASSIGN
        node = local_variable_init(node);
        expect(";");
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

// block = "{" stmt* "}"
Node* block() {
    Node* node = NULL;
    if (consume("{")) {
        node = new_node(ND_BLOCK);
        int max_block_size = 1000;
        node->block = calloc(max_block_size, sizeof(Node));
        for (int i = 0; !consume("}"); i++) {
            if (i >= max_block_size) {
                error0(
                    "size of statements in one block is over the limitation.");
            }
            node->block[i] = stmt();
        }
    }
    return node;
}

// TODO: refactor. move assign in stmt and expr should work current "conditional" function.
// expr ::= assign
Node* expr() { return assign(); }

// what are described below from here are corresponding to expr in general.
// assign ::= equality ("=" equality)?
Node* assign() {
    Node* node = conditional();

    if (consume("=")) {
        node = new_binary(ND_ASSIGN, node, conditional());
        return node;
    }

    if (consume("+=")) {
        Node* add =
            new_binary(ND_ADD, node, ptr_conversion(node, conditional()));
        node = new_binary(ND_ASSIGN, node, add);
        return node;
    }

    if (consume("-=")) {
        Node* sub =
            new_binary(ND_SUB, node, ptr_conversion(node, conditional()));
        node = new_binary(ND_ASSIGN, node, sub);
        return node;
    }
    if (consume("*=")) {
        Node* mul = new_node(ND_MUL);
        Node* right = conditional();
        mul->lhs = node;
        mul->rhs = right;
        node = new_binary(ND_ASSIGN, node, mul);
        return node;
    }
    if (consume("/=")) {
        Node* div = new_node(ND_DIV);
        Node* right = conditional();
        div->lhs = node;
        div->rhs = right;
        node = new_binary(ND_ASSIGN, node, div);
        return node;
    }

    return node;
}

// conditional ::= logor ("?" logor ":" logor)?
Node* conditional() {
    Node* node = logor();
    if (consume("?")) {
        Node* cond = node;
        Node* then = logor();
        expect(":");
        Node* alt = logor();

        Node* els = new_node(ND_ELSE);
        els->lhs = then;
        els->rhs = alt;
        Node* ternary = new_node(ND_TERNARY);
        ternary->lhs = cond;
        ternary->rhs = els;

        node = ternary;
    }

    return node;
}

/*
ex: A || B || C => (A || B) || C
ND_LOGOR:
    lhs: ND_LOGOR
        lhs: A
        rhs: B
    rhs: C
TODO: Now it doesn't work like this. add test and fix.
*/
Node* logor() {
    Node* node = logand();
    while (consume("||")) {
        node = new_binary(ND_LOGOR, node, logor());
    }
    return node;
}
/*
ex: A && B && C => (A && B) && C
ND_LOGAND:
    lhs: ND_LOGAND
        lhs: A
        rhs: B
    rhs: C
TODO: Now it doesn't work like this. add test and fix.
*/
Node* logand() {
    Node* node = bitor ();
    while (consume("&&")) {
        node = new_binary(ND_LOGAND, node, logand());
    }
    return node;
}

/*
ex: A | B | C => (A | B) | C
ND_BITOR:
    lhs: ND_BITOR
        lhs: A
        rhs: B
    rhs: C
TODO: Now it doesn't work like this. add test and fix.
*/
Node* bitor () {
    Node* node = bitxor();  // xor has higher priority than bitor
    while (consume("|")) {
        node = new_binary(ND_BITOR, node, bitor ());
    }
    return node;
}

/*
ex: A ^ B ^ C => (A ^ B) ^ C
ND_BITXOR:
    lhs: ND_BITXOR
        lhs: A
        rhs: B
    rhs: C
TODO: Now it doesn't work like this. add test and fix.
*/
Node* bitxor() {
    Node* node = bitand();
    while (consume("^")) {
        node = new_binary(ND_BITXOR, node, bitxor());
    }
    return node;
}

/*
ex: A & B & C => (A & B) & C
ND_BITAND:
    lhs: ND_BITAND
        lhs: A
        rhs: B
    rhs: C
TODO: Now it doesn't work like this. add test and fix.
*/
Node*bitand() {
    Node* node = equality();
    while (consume("&")) {
        node = new_binary(ND_BITAND, node, bitand());
    }
    return node;
}

// equality ::= relational ("==" relational | "!=" relational)*
Node* equality() {
    Node* node = relational();

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

// relational = add ("<" add | ">" add | "<=" add | ">=" add)?
Node* relational() {
    Node* node = add();

    for (;;) {
        if (consume("<")) {
            node = new_binary(ND_LT, node, add());
        } else if (consume("<=")) {
            node = new_binary(ND_LE, node, add());
        } else if (consume(">")) {
            node = new_binary(ND_LT, add(), node);
        } else if (consume(">=")) {
            node = new_binary(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

// add = mul("+" mul | "-" mul)*
Node* add() {
    Node* node = mul();

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

// mul = unary("*" unary| "/" unary)*
Node* mul() {
    Node* node = unary();
    for (;;) {
        if (consume("*")) {
            node = new_binary(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_binary(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

// unary = ("sizeof" | "+" | "-" | "*" | "&" | "++" | "--")? unary | primary
Node* unary() {
    // ++a
    if (consume("++")) {
        return new_binary(ND_PRE_INC, unary(), NULL);
    }
    // --a
    if (consume("--")) {
        return new_binary(ND_PRE_DEC, unary(), NULL);
    }

    if (consume("+")) {
        return unary();
    }

    if (consume("-")) {
        return new_binary(ND_SUB, new_num(0), unary());
    }

    if (consume("*")) {
        return new_binary(ND_DEREF, unary(), NULL);
    }

    if (consume("&")) {
        return new_binary(ND_ADDR, unary(), NULL);
    }

    if (consume("!")) {
        return new_binary(ND_NOT, unary(), NULL);
    }

    if (consume("~")) {
        return new_binary(ND_BITNOT, unary(), NULL);
    }

    if (consume_kind(TK_SIZEOF)) {
        // when pure type is passed
        if ((check("(") && token->next->kind == TK_TYPE) ||
            check_kind(TK_TYPE)) {
            bool has_brace = false;
            if (consume("(")) {
                has_brace = true;
            }
            char* name = calloc(100, sizeof(char));
            memcpy(name, token->str, token->len);

            consume_kind(TK_TYPE);

            Node* size_of_type;
            if (strcmp(name, "int") == 0) {
                size_of_type = new_num(get_type_size(int_type()));
            } else if (strcmp(name, "char") == 0) {
                size_of_type = new_num(get_type_size(char_type()));
            } else {
                error1("invalid type name is passed here. %s", name);
            }
            if (has_brace) consume(")");
            return size_of_type;
        }

        // when struct / enum definition is passed
        if ((check("(") && (token->next->kind == TK_ENUM ||
                            token->next->kind == TK_STRUCT)) ||
            (check_kind(TK_ENUM) || check_kind(TK_STRUCT))) {
            bool has_brace = false;
            if (consume("(")) {
                has_brace = true;
            }
            Define* def = read_define_head();
            Node* size_of_type = new_num(get_type_size(def->type));

            if (has_brace) consume(")");
            return size_of_type;
        }

        // when tag(aslias of type) is passed
        if ((check("(") && token->next->kind == TK_IDENT) ||
            (check_kind(TK_IDENT))) {
            bool has_brace = false;
            Tag* tag;
            if (check("(")) {
                tag = find_tag(token->next);
                if (tag) {
                    consume("(");
                }
                has_brace = true;
            } else {
                tag = find_tag(token);
            }

            if (tag) {
                if (tag->type->ty == STRUCT && tag->type->incomplete) {
                    error0(
                        "it's not allowed to use sizeof for incomplete type");
                }
                consume_kind(TK_IDENT);

                // when sizeof(SomeType*)
                Node* size_of_type;
                if (consume("*")) {
                    size_of_type = new_num(get_type_size(ptr_type()));
                } else {
                    size_of_type = new_num(get_type_size(tag->type));
                }

                if (has_brace) consume(")");
                return size_of_type;
            }
        }

        // when expr is passed here.
        Node* node = unary();
        int size;
        if (node->kind == ND_NUM) {
            size = 4;
        } else {
            Type* t = get_type(node);
            size = get_type_size(t);
        }

        return new_num(size);
    }

    return primary();
}

// primary = num
//           | ident ("(" (expr ",")* ")")?
//           | "(" expr ")"
//           | '""string'"'
Node* primary() {
    // recursive expr
    if (consume("(")) {
        Node* node = expr();
        expect(")");
        return node;
    }

    // ident
    Token* tok = consume_kind(TK_IDENT);
    if (tok) {
        // when func call
        if (consume("(")) {
            // node->blockに引数となるexprを詰める
            Node* node = new_node(ND_FUNC_CALL);
            node->funcname = calloc(100, sizeof(char));
            memcpy(node->funcname, tok->str, tok->len);
            // TODO: 引数とりあえず10こまで。
            node->block = calloc(10, sizeof(Node));
            for (int i = 0; !consume(")"); i++) {
                if (i != 0) {
                    expect(",");
                }
                node->block[i] = expr();
            }

            return node;
        }

        // whne enum
        Node* num = find_enum_var(tok);
        if (num) {
            return num;
        }

        // calling variable
        return variable(tok);
    }

    // predefined keywords
    Node* num_node = convert_predefined_keyword_to_num();
    if (num_node) {
        return num_node;
    }

    // string
    if (tok = consume_kind(TK_STRING)) {
        // TODO: builder 関数を整理したい
        StringToken* s = calloc(1, sizeof(StringToken));
        s->value = calloc(100, sizeof(char));
        memcpy(s->value, tok->str, tok->len);
        if (strings) {
            s->index = strings->index + 1;
        } else {
            s->index = 0;
        }
        s->next = strings;
        Node* node = new_string(s);
        strings = s;
        return node;
    }

    // otherwise number
    return new_num(expect_number());
}

// a[1] ==> *(a+1)
// variable = ident ("[" num "]")?
//            ^start
Node* variable(Token* tok) {
    Node* node = new_node(ND_LVAR);
    LVar* lvar = find_variable(tok);
    if (lvar == NULL) {
        char* name[100] = {0};
        memcpy(name, tok->str, tok->len);
        printf("; [DEBUG] %s name\n", name);
        error1("variable %s is not defined yet", name);
    }

    // if the lvar is already defined, use the offset
    node->offset = lvar->offset;
    node->type = lvar->type;

    node->kind = (lvar->kind == LOCAL) ? ND_LVAR : ND_GVAR;
    node->varname = calloc(100, sizeof(char));
    memcpy(node->varname, tok->str, tok->len);
    char* varname = node->varname;
    Type* tp = node->type;

    // a.b[0]->c[2][4]
    while (true) {
        bool has_index = false;
        while (consume("[")) {
            has_index = true;
            // a[3] => *(a + 3)
            // DEREF:
            //   ADD:
            //     a
            //     MUL:
            //       3(expr)
            //       sizeof(a)
            //
            // a[2][3] -> *( *(a + 2) + 2 )
            // value of (a + 1) is address, and [a + 1] is still address
            // To get a[1][2] from `int a[n][m];`,
            // we use DEREF( (a + sizeof(int * m) * 1) + sizeof(int) * 2 )
            // we just do deref at last only
            Node* add = new_node(ND_ADD);
            add->lhs = node;  // variable

            Node* index_val = expr();

            // we check size of element, so ew already see tp->ptr_to
            add->rhs = new_binary(ND_MUL, index_val,
                                  new_num(get_type_size(tp->ptr_to)));
            tp = tp->ptr_to;
            node = add;
            node->type = tp;
            node->varname = varname;
            expect("]");
        }
        if (has_index) {
            Node* deref_node = new_node(ND_DEREF);
            deref_node->lhs = node;
            node = deref_node;
            node->type = tp;
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
            Type* t = node->type->ptr_to;
            Node* deref = new_node(ND_DEREF);
            deref->lhs = node;  // this node is ptr lvar
            deref->type = t;
            // now deref is done. we do the same thing in a block above.
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

        break;
    }

    return node;
}

/*
define_variable read variable decl and register it as NDLVAR to varlist(globals or locals)
TODO: support scope

define_variable ::= read_define ("=" initializer)
*/
Node* define_variable(Define* def, LVar** varlist) {
    if (def == NULL) {
        print_token(token);
        error0("invalid definition of function or variable");
    }
    int scope_depth = varlist == locals ? cur_scope_depth : 0;
    LVar* varlist_to_use = varlist[scope_depth];

    read_define_suffix(def);

    Type* type = def->type;
    Token* tok = def->ident;

    Node* init = NULL;
    if (consume("=")) {
        init = initializer(type);
    }

    int size = get_type_size(type);

    Node* node = new_node(ND_LVAR);
    LVar* var = find_variable(tok);

    node->varname = calloc(100, sizeof(char));
    memcpy(node->varname, tok->str, tok->len);
    node->varsize = size;

    if (var) {
        // TODO because scope is not implemented yet.
        // It should not be allowed to redefine variable in general.
        // error1("redefining variable %s", node->varname);
    }

    // register a new lvar.
    var = calloc(1, sizeof(LVar));
    var->next = varlist_to_use;
    var->name = tok->str;
    var->len = tok->len;
    var->type = type;
    var->kind = (varlist == locals) ? LOCAL : GLOBAL;
    var->init = init;

    // TODO: clearnup
    if (varlist_to_use == NULL) {
        var->offset = size;
    } else {
        var->offset = varlist_to_use->offset + size;
    }
    varlist[scope_depth] = var;

    // complete node
    node->offset = varlist[scope_depth]->offset;
    node->type = type;
    node->kind = (var->kind == LOCAL) ? ND_LVAR : ND_GVAR;
    node->var = var;
    return node;
}

// initializer ::= num | string | char | { (expr ("," expr)*)? }
// TODO: add descriptin about how this and initializer_helper work
Node* initializer(Type* type) {
    Node* init = calloc(1, sizeof(Node));
    init->block = calloc(100, sizeof(Node));
    int start = 0;
    int* i;
    i = &start;

    if (check("{")) {
        int debug;
        initializer_helper(type, init, i);
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

void initializer_helper(Type* type, Node* init, int* i) {
    // TODO: refactor: it must be able to cunsume "{" here.
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

            // when `int a[5] = {5}`
            // fill empty items as zero value
            for (; *i < type->array_size;) {
                init->block[*i] = new_num(0);
                *i += 1;
            }
        } else if (type->ty == STRUCT) {
            // when struct comes here
            Member* m = type->members;
            int total = 0;

            for (; !consume("}");) {
                if (check("{")) {
                    initializer_helper(m->ty, init, i);
                } else {
                    // padding between members
                    int aligned_total = align_to(total, get_type_size(m->ty));
                    if (aligned_total != total) {
                        init->block[*i] = new_node(ND_PADDING);
                        init->block[*i]->size = (aligned_total - total);
                        *i += 1;
                        total += (aligned_total - total);
                    }
                    init->block[*i] = expr();
                    init->block[*i]->type = m->ty;
                    *i += 1;

                    total += get_type_size(m->ty);
                }

                consume(",");

                m = m->next;
            }

            if (total != type->size) {
                init->block[*i] = new_node(ND_PADDING);
                init->block[*i]->size = type->size - total;
                *i += 1;
            }

        } else {
            print_type(type);
            error0("unexpected type is passed.");
        }
    }
}

/**************************
 * utils
 * *************************/

// access to member
Node* struct_ref(Node* node) {
    Node* member = new_node(ND_MEMBER);
    member->lhs = node;  // base object

    // get a member to access
    member->member = find_member(consume_kind(TK_IDENT), node->type);
    member->type = member->member->ty;
    return member;
}

// Among members a type(struct) has, find one which has a name corresponding to the given tok
Member* find_member(Token* tok, Type* type) {
    if (tok == NULL) {
        error0("member ident must come here");
    }
    if (type == NULL) {
        error0("member type is not passed.");
    }

    if (type->ty != STRUCT) {
        error0("this type doesn't have members.");
    }

    char* member_name[100] = {0};
    memcpy(member_name, tok->str, tok->len);

    for (Member* m = type->members; m; m = m->next) {
        if (strcmp(member_name, m->name) == 0) {
            return m;
        }
    }

    error0("member ident is not found");
}

// find and return LVar from locals and globals
LVar* find_variable(Token* tok) {
    // char* tmp[100] = {0};
    // memcpy(tmp, tok->str, tok->len);
    // fprintf(stderr, "[DEBUG] looking up variable: %s\n", tmp);

    for (LVar* var = locals[cur_scope_depth]; var; var = var->next) {
        // char* tmp2[100] = {0};
        // memcpy(tmp2, var->name, var->len);
        // fprintf(stderr, "[DEBUG] comparing local token: %s and var: %s\n", tmp,
        //         tmp2);
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            // fprintf(stderr, "[DEBUG] found local variable: %s\n", tmp);
            var->kind = LOCAL;
            return var;
        }
    }

    for (LVar* var = globals[0]; var; var = var->next) {
        // char* tmp2[100] = {0};
        // memcpy(tmp2, var->name, var->len);
        // fprintf(stderr, "[DEBUG] comparing global token: %s and var: %s\n", tmp,
        //         tmp2);
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            // fprintf(stderr, "[DEBUG] found global variable: %s\n", tmp);
            var->kind = GLOBAL;
            return var;
        }
    }
    // fprintf(stderr, "[DEBUG] not found variable: %s\n", tmp);

    return NULL;
}

// convert node(ND_LVAR) with init -> ND_BLOCK which has several ND_ASSIGN
Node* local_variable_init(Node* node) {
    if (!node->var->init) {
        return node;
    }

    // let's say now we wanna define and initialize an array.
    // this copmiler evaluates `int a[2] = {1, 2};`
    // as:
    // int a[2];
    // a[0] = 1;
    // a[1] = 2;
    // these three lines.
    //
    // when num of itens in initializers are not enough to array_size, fill with
    // zero.

    // TODO: refactor: unify the almost same operations below.
    // when initializing string
    if (node->var->init->kind == ND_STRING) {
        Node* block = new_node(ND_BLOCK);
        block->block = calloc(node->var->type->array_size, sizeof(Node));

        int len = strlen(node->var->init->string->value);
        for (int i = 0; i < node->var->type->array_size; i++) {
            // a[0] ==> *(a + 0 * size)
            Node* add = new_node(ND_ADD);
            add->lhs = node;
            if (node->type && node->type->ptr_to) {
                int size = get_type_size(node->type->ptr_to);
                add->rhs = new_num(i * size);
            }
            Node* deref = new_node(ND_DEREF);
            deref->lhs = add;

            Node* assign = new_node(ND_ASSIGN);
            assign->lhs = deref;

            if (len > i) {
                assign->rhs = new_num(node->var->init->string->value[i]);
            } else {
                assign->rhs = new_num(0);
            }

            block->block[i] = assign;
        }

        return block;
    }

    // when initializing init a[] = {1, 2, func()};
    if (node->type->ty == ARRAY && node->var->init->block) {
        Node* block = new_node(ND_BLOCK);
        block->block = calloc(node->var->type->array_size, sizeof(Node));

        for (int i = 0; node->var->init->block[i]; i++) {
            // a[0] ==> *(a + 0 * size)
            Node* add = new_node(ND_ADD);
            add->lhs = node;
            if (node->type && node->type->ptr_to) {
                int size = get_type_size(node->type->ptr_to);
                add->rhs = new_num(i * size);
            }
            Node* deref = new_node(ND_DEREF);
            deref->lhs = add;

            Node* assign = new_node(ND_ASSIGN);
            assign->lhs = deref;
            assign->rhs = node->var->init->block[i];

            block->block[i] = assign;
        }

        return block;
    }

    // when initializing struct Hoge h = {123, "asdf"};
    if (node->type->ty == STRUCT && node->var->init->block) {
        Node* block = new_node(ND_BLOCK);
        block->block = calloc(node->var->type->array_size, sizeof(Node));

        Member* m = node->var->type->members;
        for (int i = 0; node->var->init->block[i]; i++) {
            Node* add = new_node(ND_ADD);
            add->lhs = node;
            // TODO: check if this works correctly
            // TODO: is it fine for offset to start from zero?
            add->rhs = new_num(m->offset);
            Node* deref = new_node(ND_DEREF);
            deref->lhs = add;

            Node* assign = new_node(ND_ASSIGN);
            assign->lhs = deref;
            assign->rhs = node->var->init->block[i];

            block->block[i] = assign;
            m = m->next;
        }

        return block;
    }

    Node* assign = new_node(ND_ASSIGN);
    assign->lhs = node;
    assign->rhs = node->var->init;

    return assign;
}

// DEREF node like `*(a + 1) * b`(a is ptr and b is int) should return ptr_to's type(in this case, INT)
Type* get_type(Node* node) {
    if (node == NULL) {
        return NULL;
    }

    if (node->type) {
        return node->type;
    }

    // check left first
    Type* t = get_type(node->lhs);
    if (!t) {
        t = get_type(node->rhs);
    }

    // when node is DEREF, it returns ptr_to's type
    if (t && node->kind == ND_DEREF) {
        // TODO: ND_DEREF should have ptr_to? fix.
        if (t->ptr_to != NULL) {
            t = t->ptr_to;
            if (t == NULL) {
                print_node(node);
                print_type(t);
                print_type(t);

                error0("invalid dereference");
            }
        }
        return t;
    }
    return t;
}

// get_type_size returns suze if the type = (how much memory it needs to allocate itself)
// INT -> 4
// PTR -> 8
// CHAR -> 1
// TODO: change Type so that all Type object has its own size info in it.
int get_type_size(Type* type) {
    if (type == NULL) {
        error0("type should be non null");
    }

    int size;
    switch (type->ty) {
        case INT:
            return 4;
        case PTR:
            return 8;
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
        default:
            error0("unexpected Type->ty comes here");
    }
}

// read_type_and_ident ::= type_info_prefix ident type_info_suffix
Define* read_define() {
    Define* def = read_define_head();
    read_define_suffix(def);
    return def;
}

/*
read_define_head ::= type "*"* ident
type ::= "int" | "char" | "void" | "bool"
read incopmlete type info and ident, and put them into Define as container, then return it
*/
Define* read_define_head() {
    Type* type = NULL;

    // for rollback
    Token* t = token;

    // read alias
    Token* ident = consume_kind(TK_IDENT);
    if (ident) {
        Tag* tag = find_tag(ident);
        if (tag) {
            type = tag->type;
        } else {
            // a = 1; のような式もココに入ってきてしまう
            token = t;  // rollback
        }
    }

    // read struct definition if possible
    if (type == NULL) {
        type = define_struct();
    }

    // read enum definition if possible
    bool is_enum = false;
    if (type == NULL) {
        type = define_enum();
        if (type) {
            is_enum = true;
        }
    }

    // if any type is not found so far yet,
    // try to read (("int" | "char") "*"* | "void")
    if (type == NULL) {
        Token* typeToken = consume_kind(TK_TYPE);
        if (typeToken == NULL) {
            return NULL;
        }

        type = calloc(1, sizeof(Type));
        int isChar = memcmp("char", typeToken->str, typeToken->len) == 0;

        // NOTE: handle void as an alias of int in this compiler.
        type->ty = INT;  // default
        if (isChar) type->ty = CHAR;
    }
    while (consume("*")) {
        Type* tt = calloc(1, sizeof(Type));
        tt->ty = PTR;
        tt->ptr_to = type;
        type = tt;
    }

    if (type == NULL) {
        return NULL;
    }

    Node* node = NULL;
    Token* tok = consume_kind(TK_IDENT);

    // NOTE: struct/enumの定義時には型の直後に変数名不要.
    // TODO: refactor
    if (tok == NULL && type->ty != STRUCT && !is_enum) {
        print_type(type);
        error0("ident should come here.");
    }

    Define* def = calloc(1, sizeof(Define));
    def->type = type;
    def->ident = tok;

    return def;
}

/*
read suffix of variable decl and updated given def with the info
*/
void read_define_suffix(Define* def) {
    if (def == NULL) {
        print_token(token);
        error0("invalid definition of function or variable");
    }

    Type* type = def->type;
    Token* tok = def->ident;

    // check if it's array or not;
    while (consume("[")) {
        Type* t;
        t = calloc(1, sizeof(Type));
        t->ty = ARRAY;
        t->ptr_to = type;

        // size of array is optional.
        t->array_size = 0;
        Token* num = NULL;
        if (num = consume_kind(TK_NUM)) {
            t->array_size = num->val;
        }

        type = t;
        expect("]");
    }

    def->type = type;
}

// TODO: learn here
int align_to(int n, int align) { return (n + align - 1) & ~(align - 1); }

// in general, when calling push_tag, the type should be complete
// push_tag registers or update a tag in tags(global variable).
void push_tag(Token* tok, Type* type, bool is_complete) {
    char* name = calloc(100, sizeof(char));
    memcpy(name, tok->str, tok->len);

    // NOTE: tag->type = type doesn't work
    // because there are some parts that watching incomplete type directly.
    Tag* tag = find_or_register_tag(tok);
    // fprintf(stderr, "[DEBUG] updating tag: %s\n", name);
    tag->name = name;
    tag->type->array_size = type->array_size;
    tag->type->members = type->members;
    // NOTE: typedef struct Hoge Hoge; doesn't make the struct definition
    // complete.
    if (tag->type->incomplete && is_complete) {
        tag->type->incomplete = false;
    }
    tag->type->ptr_to = type->ptr_to;
    tag->type->size = type->size;
    tag->type->ty = type->ty;
}

Tag* find_tag(Token* tok) {
    char* n = calloc(100, sizeof(char));
    memcpy(n, tok->str, tok->len);

    // fprintf(stderr, "[DEBUG] finding tag: %s\n", n);
    for (Tag* tag = tags; tag; tag = tag->next) {
        if (strcmp(tag->name, n) == 0) {
            // fprintf(stderr, "[DEBUG] tag: %s is found.\n", n);
            return tag;
        }
    }

    // fprintf(stderr, "[DEBUG] tag: %s is not found.\n", n);
    return NULL;
}

Tag* find_or_register_tag(Token* tok) {
    char* n = calloc(100, sizeof(char));
    memcpy(n, tok->str, tok->len);

    // fprintf(stderr, "[DEBUG] finding tag: %s\n", n);
    for (Tag* tag = tags; tag; tag = tag->next) {
        if (strcmp(tag->name, n) == 0) {
            // fprintf(stderr, "[DEBUG] tag: %s is found.\n", n);
            return tag;
        }
    }
    // fprintf(stderr, "[DEBUG] tag: %s is not found. register it
    // tentatively\n",
    //         n);

    // when tag is not found, incomplete tag is returned as a dummy
    // so that we can use undefined type in advance.
    Tag* tag = calloc(1, sizeof(Tag));
    tag->name = n;
    tag->type = calloc(1, sizeof(Type));
    tag->type->incomplete = true;
    if (tags) {
        tag->next = tags;
    }
    tags = tag;
    return tag;
}

// find_enum_var returns enum as int if a enum whose name is corresponding to
// the given token. if it cannot find the enum name, it returns NULL. enum name
// here means `AAA` of `enum Enum {AAA, BBB}`
Node* find_enum_var(Token* tok) {
    char* name = calloc(100, sizeof(char));
    memcpy(name, tok->str, tok->len);

    for (EnumVar* e = enum_vars; e; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            return new_num(e->value);
        }
    }

    return NULL;
}

// predefined const variable. they are treated as pure int.
// it's also fine to add codes in gen to evaluate these token..
Node* convert_predefined_keyword_to_num() {
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
        return new_num(0);
    }
    if (consume_kind(TK_STDERR)) {
        return new_num(0);
    }

    return NULL;
}

Node* ptr_conversion(Node* node, Node* right) {
    if (node->type && node->type->ptr_to) {
        int size_of_type = get_type_size(node->type->ptr_to);
        return new_binary(ND_MUL, right, new_num(size_of_type));
    }
    return right;
}

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

// NOTE: BE CAREFUL when you change here.
// It's not easy to find bugs in assembler.
void gen(Node* node) {
    if (node == NULL) {
        error0("node must not be NULL");
    }
    int id = if_sequence;
    int original_brk = 0;
    int original_cnt = 0;
    int num_args = 0;
    Type* type;
    Node* n;

    switch (node->kind) {
        case ND_LOGAND:
            if_sequence++;
            // evalulate from left item, and once false is found, it leaves here
            // and jump to else clause
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lfalse%d\n", id);
            gen(node->rhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lfalse%d\n", id);
            // when it comes here, cond is true so push 1 as true
            printf("  push 1\n");
            printf("  jmp .Lend%d\n", id);
            printf(".Lfalse%d:\n", id);
            printf("  push 0\n");
            printf(".Lend%d:\n", id);
            return;
        case ND_LOGOR:
            if_sequence++;
            // evaluate from left item, and once true is found, it leaves here
            // and jump to else caluse
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  jne .Ltrue%d\n", id);
            gen(node->rhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  jne .Ltrue%d\n", id);
            // when it comes here, cond is false so push 1 as true
            printf("  push 0\n");
            printf("  jmp .Lend%d\n", id);
            printf(".Ltrue%d:\n", id);
            printf("  push 1\n");
            printf(".Lend%d:\n", id);
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
            // ND_ASSIGN pushes its lhs variable's value at last, so it's fine
            // just to gen ND_ASSIGN here.
            n = new_node(ND_ASSIGN);
            n->lhs = node->lhs;
            n->rhs = new_binary(ND_ADD, node->lhs, new_num(1));
            gen(n);
            return;
        case ND_SUF_INC:  // a++
            // do operation of ND_PRE_INC first
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
            // TODO: learn push offset {symbol, label}
            printf("  push offset .LC%d\n", node->string->index);
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

                // TODO: learn what size are available in .data space
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

            // array's initializer doesn't always have block. ex: g_msg2[4] =
            // "bar";
            if (node->type->ty == ARRAY && node->var->init->block) {
                for (int i = 0; node->var->init->block[i]; i++) {
                    if (node->var->init->block[i]->kind == ND_PADDING) {
                        printf("  .zero 0x%x\n",
                               node->var->init->block[i]->size);
                        continue;
                    }
                    switch (node->var->init->block[i]->type->ty) {
                        case INT:
                            // %x: Hexadecimal(16)
                            printf("  .long 0x%x\n",
                                   node->var->init->block[i]->val);
                            break;
                        case CHAR:
                            printf("  .byte 0x%x\n",
                                   node->var->init->block[i]->val);
                            break;
                        case PTR:
                            printf("  .quad .LC%d\n",
                                   node->var->init->block[i]->string->index);
                            break;
                        default:
                            break;
                    }
                }
                return;
            }
            // TODO: support global variable struct initializer

            // when string
            if (node->var->init->kind == ND_STRING) {
                // strings are registed with .LC%d labels in .data space
                // we need to

                // TODO: understand in more detail
                if (node->type->ty == ARRAY) {
                    // 1. char a[3] = "hoge"
                    printf("  .string \"%s\"\n",
                           node->var->init->string->value);
                } else {
                    // 2. char *a = "hoge". a is pointer.
                    printf("  .quad .LC%d\n", node->var->init->string->index);
                }
                return;
            }

            // when int
            printf("  .long 0x%x\n", node->var->init->val);
            return;
        case ND_MEMBER:
        case ND_GVAR:
        case ND_LVAR:
            // evaulation of variables means pushing their "values" in stack.

            // 1. evaluate lhs variabel as lvalue == push its address in stack
            gen_val(node);

            // TODO: understand in more details
            type = get_type(node);
            if (type && type->ty == ARRAY) {
                return;
            }

            /*
            DEREF operation is to push a value of specified address in stack.
            So, first compiler pushes an address of variable, and replace the
            value with its actual value
            */
            // 2. push the variable's address
            printf("  pop rax\n");

            // 3. get a value of specified address
            if (type) {
                switch (type->ty) {
                    case CHAR:
                        // TODO: learn movsc, movsxd
                        printf("  movsx rax, BYTE PTR [rax]\n");
                        break;
                    case INT:
                        printf("  movsxd rax, DWORD PTR [rax]\n");
                        break;
                    default:
                        printf("  mov rax, [rax]\n");
                        break;
                }
            } else {
                printf("  mov rax, [rax]\n");
            }

            // 4. and push the value in stack
            printf("  push rax\n");
            return;
        case ND_ASSIGN:
            type = get_type(node);
            // evaluate lhs as lvalue
            gen_val(node->lhs);
            // push evaluated value of lhs
            gen(node->rhs);
            printf("  pop rdi\n");  // rdi contains value
            printf("  pop rax\n");  // rax contains variable's address

            /*
            calling mov with 64bit register will write the whole bit(=8bytes) of
            the receiver register. when we wanna write on only part of the
            receiver, we should use aliases of registers. ex. dil is an alias of
            rdi and it just hold least 8 bits of rdi. `mov [rax], dil` will copy
            only 8bits from rdi to [rax].
            */

            if (type && type->ty == CHAR) {
                // NOTE: dil = least 8 bits(1B) of rdi
                printf("  mov [rax], dil\n");
            } else if (type && type->ty == INT) {
                // NOTE: edi = least 32bits (4B) of rdi
                printf("  mov [rax], edi\n");
            } else {
                printf("  mov [rax], rdi\n");
            }

            // store rdi's value = evaluation result
            printf("  push rdi\n");
            return;
        case ND_RETURN:
            if (node->lhs) {
                gen(node->lhs);
            } else {
                // when return value is not provided(e.g. `return;`), just
                // return 0;
                gen(new_num(0));
            }
            printf("  pop rax\n");  // set evaulated value on rax(this is ABI
                                    // requirement)
            printf(
                "  mov rsp, rbp\n");  // don't forget to have epilogue on return
            printf("  pop rbp\n");
            printf("  ret\n");
            return;
        case ND_BREAK:
            if (break_sequence == 0) {
                error0("currently not in for, while");
            }
            printf("  jmp .Lend%d\n", break_sequence);
            return;
        case ND_CONTINUE:
            if (continue_sequence == 0) {
                error0("currently not in for, while");
            }
            printf("  jmp .Lcontinue%d\n", continue_sequence);
            return;
        case ND_IF:
            if_sequence++;
            // lhs: cond
            // rhs: stmt(main) or else(lhs=main, rhs=alt)

            // cond
            gen(node->lhs);
            printf("  pop rax\n");
            // if lhs(=cond)'s evaulated result is 0(=false), jump.
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%d\n", id);

            if (node->rhs->kind == ND_ELSE) {
                Node* els = node->rhs;
                gen(els->lhs);
                printf("  jmp .Lend%d\n", id);
                printf(".Lelse%d:\n", id);
                gen(els->rhs);
                printf("  jmp .Lend%d\n", id);
            } else {
                gen(node->rhs);
                printf("  jmp .Lend%d\n", id);
                printf(".Lelse%d:\n", id);
            }

            printf(".Lend%d:\n", id);

            // TODO: with current impl, `if` statement stores a value which is
            // last evaluated value in cond or then or alt. To make it
            // `statement`, we should change, but if it's ok for `if` to be
            // expr, current style is fine..
            return;
        case ND_TERNARY:
            /*
                # vs if
                - ternary explicitly pushes evaluated result on stack.
                - ternary always has else clause

            kind: ND_TERNARY
            lhs:  cond
            rhs:  stmt(then) or ND_ELSE(lhs=then, rhs=alt)
            */

            if_sequence++;
            // cond
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%d\n", id);

            Node* els = node->rhs;
            gen(els->lhs);  // then
            printf("  jmp .Lend%d\n", id);
            printf(".Lelse%d:\n", id);
            gen(els->rhs);  // alt
            printf("  jmp .Lend%d\n", id);

            printf(".Lend%d:\n", id);
            return;
        case ND_WHILE:
            if_sequence++;
            original_brk = break_sequence;
            break_sequence = id;
            original_cnt = continue_sequence;
            continue_sequence = id;
            /*
            while({cond}) {body}

            [cond]
            je end
            [body]
            .end:
            */
            printf(".Lbegin%d:\n", id);
            printf(".Lcontinue%d:\n", id);
            // cond
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", id);
            // body
            gen(node->rhs);
            printf("  jmp .Lbegin%d\n", id);
            printf(".Lend%d:\n", id);
            break_sequence = original_brk;
            continue_sequence = original_cnt;
            return;
        case ND_FOR:
            if_sequence++;
            original_brk = break_sequence;
            break_sequence = id;
            original_cnt = continue_sequence;
            continue_sequence = id;
            /*
            for({A}; {B}; {C)) D

            [A]
            .LbeginXXX:
            [B]
            pop rax
            cmp rax, 0
            je  .LendXXX
            [D]
            [C]
            jmp .LbeginXXX
            .LendXXX:

            kind: ND_FOR
            lhs:
                lhs: A: init
                rhs: B: cond
            rhs:
                lhs: C: post ops
                rhs: D: body
            */

            // A: init
            if (node->lhs->lhs != NULL) {
                gen(node->lhs->lhs);
            }
            printf(".Lbegin%d:\n", id);

            // B: cond
            if (node->lhs->rhs != NULL) {
                gen(node->lhs->rhs);
            } else {
                printf("  push 1\n");  // if cond is not given, it's always
                                       // true. ex: for(;;) {} == while(true) {}
            }
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", id);

            // D: body
            gen(node->rhs->rhs);

            // C: post ops
            // continue stmt in for loop will jump right before post ops.
            printf(".Lcontinue%d:\n", id);
            if (node->rhs->lhs != NULL) {
                gen(node->rhs->lhs);
            }

            printf("  jmp .Lbegin%d\n", id);
            printf(".Lend%d:\n", id);
            break_sequence = original_brk;
            continue_sequence = original_cnt;
            return;
        case ND_BLOCK:
            for (int i = 0; node->block[i] != NULL; i++) {
                gen(node->block[i]);
            }
            return;
        case ND_SWITCH:
            /*
            switch({expr}) {
                case {label}: --> collected as ND_CASE and stored in ND_SWITCH's
            next_case and default_case members {body};   --> collected in
            ND_SWITCH's rhs
            }

            kind: ND_SWITCH
            lhs: expr
            rhs: all bodies in each case segments
            */
            original_brk = break_sequence;
            break_sequence = id;

            // expr
            gen(node->lhs);
            printf("  pop rax\n");

            // labels
            for (Node* n = node->next_case; n; n = n->next_case) {
                // specify unique label id for each case so that cpu can jump to
                // one of them directly.
                n->case_label = ++if_sequence;
                printf("  cmp rax, %d\n", n->val);
                printf("  je .Lcase%d\n", n->case_label);
            }
            if (node->default_case) {
                node->default_case->case_label = ++if_sequence;
                printf("  jmp .Lcase%d\n", node->default_case->case_label);
            }
            // for the case where no lable matches the given cond.
            printf("  jmp .Lend%d\n", id);

            // body
            // in this body, we may have break statements which jump to Lend
            // label right below.
            gen(node->rhs);
            printf(".Lend%d:\n", id);

            break_sequence = original_brk;
            return;
        case ND_CASE:
            printf(".Lcase%d:\n", node->case_label);
            return;
        case ND_FUNC_CALL:
            /*
            1. puts arguments in registers in manner defined by ABI
                - In func def, we retrieve these registered values
            2. adjust rsp so that it is multiple of 16 (this is ABI requirement)
            3. call func
            4. push rax after returning (returning value is put in rax. this is
            ABI requirement)

            Q: don't we have to do this in epilogue?
                - No. epilogues is in function definition.
            TODO: currently, function can have at most 6 argumnets. we should
            levarage stack if we need more.

            kind: ND_FUNC_CALL
            block: now putting args in block member. node->args is for func def.

            */

            if_sequence++;

            // evaluate args and push them into stack
            for (int i = 0; node->block[i] != NULL; i++) {
                gen(node->block[i]);
                num_args++;
            }
            if (num_args > 6) {
                error_at0(token->str, "invalid number of args. lteq 6.");
            }

            // registere evaluated values in stack to registers.
            // now last argument is put on the top of stack.
            for (int i = num_args - 1; i >= 0; i--) {
                printf("  pop %s\n", argreg8[i]);
            }

            // adjust rsp so that it can be multiple of 16.
            printf("  mov rax, rsp\n");
            printf("  and rax, 15\n");
            printf("  jnz .L.call.%03d\n", id);
            // clear al(least 8 bits of rax) with 0 value. this is prep for rest
            // parameters.
            // TODO: support rest parameters
            printf("  mov rax, 0\n");
            printf("  call %s\n", node->funcname);
            printf("  jmp .L.end.%03d\n", id);
            printf(".L.call.%03d:\n", id);
            // TODO: is it ok just sub 8 when it's not multiple of 16?
            printf("  sub rsp, 8\n");
            // clear al(least 8 bits of rax) with 0 value. this is prep for rest
            // parameters.
            printf("  mov rax, 0\n");
            printf("  call %s\n", node->funcname);
            printf("  add rsp, 8\n");
            printf(".L.end.%03d:\n", id);

            // push value in rax. this means we always get returning value from
            // functions.
            printf("  push rax\n");
            return;
        case ND_FUNC_DEF:
            /*
            int* hoge(char a, int b) {body}

            kind: ND_FUNC_DEF
            funcname: ident of funcname
            args: arguments' definitions
            lhs: body

            TODO: Currently ND_FUNC_DEF doesn't have type information of
            returning value since we don't have semantic analysis.

            locals[cur_scope_depth]: local variable's definitions.


            memory layout:
            -------rbp
            arg0
            arg1
            ...
            arg6
            local variable0
            local variable1
            local variable2
            ......
            local variableN


            */

            // label
            printf(".global %s\n", node->funcname);
            printf("%s:\n", node->funcname);

            /*
            prologue
            1. push ret address          <- this is done in `call` intro. `call`
            = push ret address + jump to func label
            2. push caller's RBP         <- to restore state when this func is
            finished.
            3. push args from registers  <- ABI requirement.
            ...
            */
            printf("  push rbp\n");
            printf("  mov rbp, rsp\n");

            // push down stack to store space for local variables as many as
            // local variables this func has(not including args) we use this
            // space for temporal local variables for this func.
            if (locals[cur_scope_depth]) {
                int offset = locals[cur_scope_depth]->offset;
                printf("  sub rsp, %d\n", offset);
            }

            // copy arguments from registers to space right below rbp.
            // TODO: doesn't this conflict with local variables?
            for (int i = 0; node->args[i]; i++) {
                if (node->args[i]->varsize == 1) {
                    printf("  mov [rbp-%d], %s\n", node->args[i]->offset,
                           argreg1[i]);
                } else if (node->args[i]->varsize == 4) {
                    printf("  mov [rbp-%d], %s\n", node->args[i]->offset,
                           argreg4[i]);
                } else {
                    printf("  mov [rbp-%d], %s\n", node->args[i]->offset,
                           argreg8[i]);
                }
            }

            // body
            gen(node->lhs);

            // epilogue
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");

            return;
        case ND_ADDR:
            /*
            push address of the variable
            kind: ND_ADDR
            lhs: ND_LVAR
            */
            gen_val(node->lhs);
            return;
        case ND_DEREF:
            /*
            push "value" of the variable(this must be ptr), and do deref ops.
            almost same as what ND_LVAR does
            */
            // evaluate the variable(ptr) as right variable to get address of a
            // variable pointed by the given variable(ptr)
            gen(node->lhs);

            // push the address to rax
            printf("  pop rax\n");
            // and get value of the pointed variable
            type = get_type(node);
            if (type && type->ty == CHAR) {
                printf("  movsx rax, BYTE PTR [rax]\n");
            } else if (type && type->ty == INT) {
                printf("  movsxd rax, DWORD PTR [rax]\n");
            } else {
                printf("  mov rax, [rax]\n");
            }
            // then push the value on stack
            printf("  push rax\n");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

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
            /*
            idiv: division with signs
            it implicitly gets rdx and rax, sum them, and devide the result by
            passed register to idiv inst. it sets result in rax and remains in
            rdx with cqo inst, it consider the result as 128bits and set it
            across rdx, rax.

            pop rdi   // set value as devider
            pop rax   // set value as devidee
            cqo       // set 10 as 128bit across rdx, rax. rdx is zero cleared
            here.
                      // TODO: ???rax will be 10?
            idiv rdi  // (rdx(=0) + rax) / rdi

            TODO: learn sete, setne, setl, setle
            */

            // TODO: don't we need to clear rdx with zero before calling idiv?
            // devide rax by rdi
            printf("  idiv rdi\n");
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
gen_val treat the given node(ND_LVAR) as lvalue.
That means baricc pushes its "address" in stack here for assign operation.
*/
void gen_val(Node* node) {
    // when node is ND_DEREF, treat it as right-value(= push its "value" in
    // stack)
    if (node->kind == ND_DEREF) {
        gen(node->lhs);
        return;
    }

    if (node->kind == ND_LVAR) {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", node->offset);
        printf("  push rax\n");
    } else if (node->kind == ND_GVAR) {
        printf("  push offset %s\n", node->varname);
    } else if (node->kind == ND_MEMBER) {
        // add offset of the member
        gen_val(node->lhs);
        printf("  pop rax\n");
        printf("  add rax, %d\n", node->member->offset);
        printf("  push rax\n");
    } else {
        error0("not variable");
    }
}
    for (Define* def = defines; def; def = def->next) {
        if (def->name_len == len && strncmp(def->name, p, len) == 0) return def;
    }

int main(int argc, char** argv) {
    // make a LinkedList of token from all of the given files.
    Token* t = NULL;
    for (int i = 1; i < argc; i++) {
        filename = argv[i];
        printf("; i = %d, filename=%s\n", i, filename);
        user_input = read_file(filename);
        t = tokenize();
        if (!token) {
            token = t;
        } else {
            Token* tt = token;
            while (true) {
                if (tt->next->kind == TK_EOF) {
                    tt->next = t;
                    break;
                }
                tt = tt->next;
            }
            tt->next = t;
        }
    }

    cur_scope_depth = 0;
    program();

    // specify what syntax to use for this output.
    printf(".intel_syntax noprefix\n");

    // TODO: learn .bss
    // .bss
    // declare global variables without initializers
    printf(".bss\n");  // TODO: what's this
    for (int i = 0; code[i]; i++) {
        if (code[i]->kind == ND_GVAR_DEF) {
            if (code[i]->var->init == NULL) {
                gen(code[i]);
            }
        }
    }

    // TODO: learn .data
    // .data
    // declare global variables with initializers
    printf(".data\n");
    for (StringToken* str = strings; str; str = str->next) {
        printf(".LC%d:\n", str->index);
        printf("  .string \"%s\"\n", str->value);
    }
    for (int i = 0; code[i]; i++) {
        if (code[i]->kind == ND_GVAR_DEF) {
            if (code[i]->var->init != NULL) {
                gen(code[i]);
            }
        }
    }

    // TODO: learn .text
    // .text
    printf(".text\n");
    printf(".globl main\n");
    cur_scope_depth = 0;
    for (int i = 0; code[i]; i++) {
        if (code[i]->kind == ND_FUNC_DEF) {
            cur_scope_depth++;
            gen(code[i]);
        }
    }

    return 0;
}
