// cc.c - small C compiler (single-file, C-only, deterministic loops only)
// Compile: gcc -O0 -g3 -Wall -o cc cc.c
// Usage: ./cc input.c > out.s

#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Forward Declarations --- */
void error_at(char* loc, char* fmt, ...);
void error(char* fmt, ...);
char* read_file(char* path);
char* parse_directive(char* p, char** output_ptr, char** q_ptr, size_t *output_cap_ptr, const char* current_dir);
char* preprocess(char* source, const char* current_dir);

/* --- Types --- */

typedef struct Node Node;
typedef struct Token Token;
typedef struct Type Type;
typedef struct LVar LVar;
typedef struct Function Function;
typedef struct Define Define;
typedef struct Member Member;

typedef enum {
    TK_RESERVED, TK_NUM, TK_EOF, TK_IDENT, TK_RETURN, TK_IF, TK_ELSE,
    TK_WHILE, TK_FOR, TK_SIZEOF, TK_STRING, TK_STRUCT, TK_TYPEDEF,
    TK_ENUM, TK_BREAK, TK_CONTINUE,
} TokenKind;

struct Token {
    TokenKind kind;
    Token* next;
    int val;
    char* str;
    int len;
    int line_num;
    int col_num;
    char* line_start;
};

typedef enum {
    ND_ADD, ND_SUB, ND_MUL, ND_DIV, ND_NUM, ND_EQ, ND_NE, ND_LT, ND_LE,
    ND_ASSIGN, ND_LVAR, ND_RETURN, ND_IF, ND_WHILE, ND_FOR, ND_BLOCK,
    ND_FUNC_CALL, ND_ADDR, ND_DEREF, ND_FUNC_DEF,
} NodeKind;

struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    Node** block;
    int block_len;
    int val;
    int offset;
    char* funcname;
    Node** args;
    int args_len;
    char* varname;
    LVar* var;
    Type* type;
    LVar** params;
    int param_len;
    Type* ret_type;
};

typedef enum { INT, PTR, STRUCT, ARRAY } TypeKind;
struct Type {
    TypeKind ty;
    Type* ptr_to;
    int size;
    char* name;
    Member* members;
    long array_len;
};

struct Member {
    Member* next;
    Type* type;
    char* name;
    int offset;
};

struct LVar {
    LVar* next;
    char* name;
    int len;
    int offset;
    Type* type;
    enum { LOCAL, GLOBAL, PARAM } kind;
    Node* init;
};

struct Function {
    Function* next;
    char* name;
    int len;
    Node* node;
    LVar* locals;
    int stack_offset;
};

struct Define {
    Define* next;
    char* name;
    int name_len;
    char* body;
    int body_len;
};

/* --- Globals --- */

Token* token;
char* user_input; // Holds PREPROCESSED text (combined)
char* original_source_text; // RAW file content (kept for initial read)
char* filename;
LVar* locals;
int stack_offset = 0;
Function* functions;
Define* defines;
Type* typedefs;
Type* struct_tags;

/* --- Error helpers --- */

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_RESET        "\x1b[0m"

void print_error_line(char* loc, char* fmt, va_list ap) {
    char* source_to_use = user_input ? user_input : original_source_text;

    if (!loc || !source_to_use) {
        fprintf(stderr, "error: location unknown\n");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        return;
    }

    char* line = source_to_use;
    int line_num = 1;
    for (char* p = source_to_use; p <= loc && *p; p++) {
        if (*p == '\n') { line = p + 1; line_num++; }
    }

    int col_num = (int)(loc - line + 1);
    if (col_num < 1) col_num = 1;
    char* end = loc;
    while (*end != '\n' && *end != '\0') end++;
    int len = (int)(end - line);

    fprintf(stderr, "%s:%d:%d: " ANSI_COLOR_RED "error: " ANSI_RESET, filename, line_num, col_num);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    fprintf(stderr, "%.*s\n", len, line);
    fprintf(stderr, "%*s", col_num - 1, "");

    int error_len = 1;
    if (token && token->str == loc) {
        error_len = token->len > 0 ? token->len : 1;
    }

    for (int i = 0; i < error_len; i++) {
        fprintf(stderr, "^");
    }
    fprintf(stderr, "\n");
}

void error_at(char* loc, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    print_error_line(loc, fmt, ap);
    va_end(ap);
    exit(1);
}

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char* loc = token ? token->str : (user_input ? user_input : original_source_text);
    print_error_line(loc, fmt, ap);
    va_end(ap);
    exit(1);
}

/* --- Utilities --- */

bool startswith(char* p, char* q) {
    return strncmp(p, q, strlen(q)) == 0;
}

char* read_file(char* path) {
    FILE* fp = fopen(path, "rb");
    if (!fp) error("cannot open %s: %s", path, strerror(errno));
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); error("fseek failed"); }
    long sz = ftell(fp);
    if (sz < 0) { fclose(fp); error("ftell failed"); }
    rewind(fp);
    char* buf = malloc(sz + 1);
    if (!buf) { fclose(fp); error("malloc failed"); }
    if (fread(buf, 1, sz, fp) != (size_t)sz) { fclose(fp); free(buf); error("fread failed"); }
    buf[sz] = '\0';
    fclose(fp);
    return buf;
}

/* --- Preprocessor helper to ensure output capacity --- */
static void ensure_output_capacity(char **output, size_t *cap, char **q, size_t extra) {
    size_t used = (size_t)(*q - *output);
    if (used + extra + 1 > *cap) {
        while (used + extra + 1 > *cap) *cap *= 2;
        *output = realloc(*output, *cap);
        *q = *output + used;
    }
}

/* --- Preprocessor Functions --- */

Define* find_define(char* p, int len) {
    for (Define* def = defines; def; def = def->next) {
        if (def->name_len == len && strncmp(def->name, p, len) == 0) return def;
    }
    return NULL;
}

char* skip_whitespace(char* p) {
    while (isspace((unsigned char)*p)) p++;
    return p;
}

/* Helper: read file into buffer but return NULL on failure (no error exit) */
static char* try_read_file(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return NULL;
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    long sz = ftell(fp);
    if (sz < 0) { fclose(fp); return NULL; }
    rewind(fp);
    char* buf = malloc(sz + 1);
    if (!buf) { fclose(fp); return NULL; }
    if (fread(buf, 1, sz, fp) != (size_t)sz) { fclose(fp); free(buf); return NULL; }
    buf[sz] = '\0';
    fclose(fp);
    return buf;
}

/* Build directory portion of a path (returns newly allocated string, caller frees) */
static char* dir_of_path(const char* path) {
    const char* p = strrchr(path, '/');
    if (!p) return strdup(".");
    size_t len = (size_t)(p - path);
    char* d = malloc(len + 1);
    memcpy(d, path, len);
    d[len] = '\0';
    return d;
}

/* Helper that constructs candidate path and attempts to read it (C-only) */
static char* try_candidate(const char* prefix, const char* path) {
    size_t prefix_len = prefix ? strlen(prefix) : 0;
    size_t cand_len = prefix_len + strlen(path) + 2;
    char* cand = malloc(cand_len);
    if (!cand) return NULL;
    cand[0] = '\0';

    if (prefix_len && strcmp(prefix, "") != 0) {
        strcpy(cand, prefix);
        if (cand[prefix_len - 1] != '/') {
            cand[prefix_len] = '/';
            cand[prefix_len + 1] = '\0';
        }
    }

    if (prefix_len && strcmp(prefix, "") != 0)
        strcat(cand, path);
    else
        strcpy(cand, path);

    char* buf = try_read_file(cand);
    free(cand);
    return buf;
}

/* Modified parse_directive: now appends included file contents into output via q_ptr and resolves include paths */
char* parse_directive(char* p, char** output_ptr, char** q_ptr, size_t *output_cap_ptr, const char* current_dir) {
    char* output = *output_ptr;
    char* q = *q_ptr;
    p = skip_whitespace(p);
    if (!*p || *p != '#') return p;
    p++;
    p = skip_whitespace(p);

    char* name_start = p;
    while (isalpha((unsigned char)*p)) p++;
    int name_len = (int)(p - name_start);

    if (name_len == 6 && strncmp(name_start, "define", 6) == 0) {
        p = skip_whitespace(p);
        char* macro_name_start = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        int macro_name_len = (int)(p - macro_name_start);
        if (macro_name_len == 0) error_at(name_start, "#define expects a macro name");

        p = skip_whitespace(p);
        char* body_start = p;
        while (*p && *p != '\n') {
            if (startswith(p, "//") || startswith(p, "/*")) break;
            p++;
        }
        int body_len = (int)(p - body_start);

        Define* def = calloc(1, sizeof(Define));
        def->name = strndup(macro_name_start, macro_name_len);
        def->name_len = macro_name_len;
        def->body = strndup(body_start, body_len);
        def->body_len = body_len;
        def->next = defines;
        defines = def;

        while (*p && *p != '\n') p++;
        *output_ptr = output;
        *q_ptr = q;
        return p;
    } else if (name_len == 7 && strncmp(name_start, "include", 7) == 0) {
        p = skip_whitespace(p);

        char terminator;
        bool is_quoted = false;
        if (*p == '"') {
            terminator = '"';
            is_quoted = true;
            p++;
        } else if (*p == '<') {
            terminator = '>';
            p++;
        } else {
            error_at(p, "#include expects a filename in quotes or angle brackets");
            return p;
        }

        char* path_start = p;
        while (*p && *p != terminator && *p != '\n') p++;
        if (*p != terminator) {
            error_at(path_start, "unclosed #include directive");
            return p;
        }
        char* path_end = p;
        int path_len = (int)(path_end - path_start);
        char* path = strndup(path_start, path_len);
        p++; // consume terminator

        while (*p && *p != '\n') p++;

        /* Read included file: try several candidate locations */
        char* included_raw = NULL;
        bool read_ok = false;

        const char* system_dirs[] = { "/usr/include", "/usr/local/include" };

        if (is_quoted) {
            if (current_dir && strcmp(current_dir, ".") != 0) {
                included_raw = try_candidate(current_dir, path);
                if (included_raw) read_ok = true;
            }
            if (!read_ok) {
                included_raw = try_candidate("", path);
                if (included_raw) read_ok = true;
            }
            if (!read_ok) {
                included_raw = try_candidate(".", path);
                if (included_raw) read_ok = true;
            }
            for (size_t idx = 0; !read_ok && idx < sizeof(system_dirs)/sizeof(system_dirs[0]); idx++) {
                included_raw = try_candidate(system_dirs[idx], path);
                if (included_raw) read_ok = true;
            }
        } else {
            included_raw = try_candidate("", path);
            if (included_raw) read_ok = true;
            for (size_t idx = 0; !read_ok && idx < sizeof(system_dirs)/sizeof(system_dirs[0]); idx++) {
                included_raw = try_candidate(system_dirs[idx], path);
                if (included_raw) read_ok = true;
            }
        }

        if (!read_ok || !included_raw) {
            error_at(name_start, "cannot open include file '%s' (searched common include dirs)", path);
            free(path);
            *output_ptr = output;
            *q_ptr = q;
            return p;
        }

        char* included_dir = dir_of_path(path);

        char* included_preprocessed = preprocess(included_raw, included_dir ? included_dir : ".");
        free(included_raw);
        free(included_dir);

        size_t inc_len = strlen(included_preprocessed);
        ensure_output_capacity(&output, output_cap_ptr, &q, inc_len + 2);
        memcpy(q, included_preprocessed, inc_len);
        q += inc_len;
        if (inc_len == 0 || q[-1] != '\n') *q++ = '\n';

        free(included_preprocessed);
        free(path);

        *output_ptr = output;
        *q_ptr = q;
        return p;
    }

    while (*p && *p != '\n') p++;
    *output_ptr = output;
    *q_ptr = q;
    return p;
}

/* Modified preprocess to call the new parse_directive signature and accept current_dir */
char* preprocess(char* source, const char* current_dir) {
    size_t output_cap = strlen(source) * 2 + 128;
    char* output = calloc(output_cap, 1);
    char* q = output;
    char* p = source;

    while (*p) {
        ensure_output_capacity(&output, &output_cap, &q, 16);

        if (*p == '#') {
            p = parse_directive(p, &output, &q, &output_cap, current_dir);
            if (*p == '\n') { *q++ = *p++; }
            else if (*p) { p++; }
            continue;
        }

        if (startswith(p, "//")) {
            p += 2;
            while (*p && *p != '\n') p++;
            continue;
        }
        if (startswith(p, "/*")) {
            p += 2;
            while (*p && !startswith(p, "*/")) {
                if (*p == '\n') *q++ = *p;
                p++;
            }
            if (*p) p += 2;
            continue;
        }

        if (*p == '"' || *p == '\'') {
            char marker = *p;
            *q++ = *p++;
            while (*p && *p != marker) {
                if (*p == '\\') { *q++ = *p++; if (*p) *q++ = *p++; }
                else *q++ = *p++;
            }
            if (*p == marker) *q++ = *p++;
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            char* ident_start = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            int len = (int)(p - ident_start);

            Define* def = find_define(ident_start, len);
            if (def) {
                ensure_output_capacity(&output, &output_cap, &q, def->body_len);
                for (int i = 0; i < def->body_len; i++) *q++ = def->body[i];
                continue;
            }
        }

        *q++ = *p++;
    }

    *q = '\0';
    return output;
}

/* --- Tokenizer --- */

Token* new_token(TokenKind kind, Token* cur, char* str, int len, char* line_start, int line_num, int col_num) {
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    tok->line_start = line_start;
    tok->line_num = line_num;
    tok->col_num = col_num;
    if (cur) cur->next = tok;
    return tok;
}

char get_escape_char(char c) {
    switch (c) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"': return '"';
        case '0': return '\0';
        default: return c;
    }
}

Token* read_char_literal(Token* cur, char* start, char* line_start, int line_num, int col_num) {
    char* p = start + 1;
    char c = 0;
    if (*p == '\\') { p++; c = get_escape_char(*p ? *p : '\\'); if (*p) p++; }
    else { c = *p; p++; }
    if (*p != '\'') error_at(start, "invalid char literal");
    Token* tok = new_token(TK_NUM, cur, start, (int)(p - start + 1), line_start, line_num, col_num);
    tok->val = (int)c;
    return tok;
}

Token* tokenize() {
    char* p = user_input;
    Token head = {0};
    Token* cur = &head;
    int current_line = 1;
    char* line_start = p;

    while (*p) {
        if (*p == '\n') { p++; current_line++; line_start = p; continue; }
        if (isspace((unsigned char)*p)) { p++; continue; }

        int col_num = (int)(p - line_start + 1);

        if (*p == '"') {
            char* start = p; p++;
            while (*p && *p != '"') { if (*p == '\\') p += 2; else p++; }
            if (*p == '"') p++;
            cur = new_token(TK_STRING, cur, start, (int)(p - start), line_start, current_line, col_num);
            continue;
        }
        if (*p == '\'') {
            cur = read_char_literal(cur, p, line_start, current_line, col_num);
            p += cur->len;
            continue;
        }
        if (isdigit((unsigned char)*p)) {
            char* start = p;
            long val = strtol(p, &p, 10);
            Token* tok = new_token(TK_NUM, cur, start, (int)(p - start), line_start, current_line, col_num);
            tok->val = (int)val;
            cur = tok;
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            char* start = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            int len = (int)(p - start);
            Token* tok = new_token(TK_IDENT, cur, start, len, line_start, current_line, col_num);
            if (len == 6 && strncmp(start, "return", 6) == 0) tok->kind = TK_RETURN;
            else if (len == 2 && strncmp(start, "if", 2) == 0) tok->kind = TK_IF;
            else if (len == 4 && strncmp(start, "else", 4) == 0) tok->kind = TK_ELSE;
            else if (len == 5 && strncmp(start, "while", 5) == 0) tok->kind = TK_WHILE;
            else if (len == 3 && strncmp(start, "for", 3) == 0) tok->kind = TK_FOR;
            else if (len == 3 && strncmp(start, "int", 3) == 0) tok->kind = TK_RESERVED;
            else if (len == 6 && strncmp(start, "struct", 6) == 0) tok->kind = TK_STRUCT;
            else if (len == 7 && strncmp(start, "typedef", 7) == 0) tok->kind = TK_TYPEDEF;
            cur = tok;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=") ||
            startswith(p, "&&") || startswith(p, "||") || startswith(p, "++") || startswith(p, "--")) {
            Token* tok = new_token(TK_RESERVED, cur, p, 2, line_start, current_line, col_num);
            p += 2;
            cur = tok;
            continue;
        }

        cur = new_token(TK_RESERVED, cur, p, 1, line_start, current_line, col_num);
        p++;
    }
    new_token(TK_EOF, cur, p, 0, line_start, current_line, (int)(p - line_start + 1));
    return head.next;
}

/* --- Token helpers --- */

bool consume(char* op) {
    if (!token) return false;
    if (token->kind != TK_RESERVED) return false;
    if (token->len != (int)strlen(op)) return false;
    if (strncmp(token->str, op, token->len) == 0) {
        token = token->next;
        return true;
    }
    return false;
}

Token* consume_kind(TokenKind kind) {
    if (token && token->kind == kind) {
        Token* t = token;
        token = token->next;
        return t;
    }
    return NULL;
}

bool check(char* op) {
    if (!token) return false;
    if (token->kind != TK_RESERVED) return false;
    if (token->len != (int)strlen(op)) return false;
    return strncmp(token->str, op, token->len) == 0;
}

bool check_kind(TokenKind kind) {
    return token && token->kind == kind;
}

void expect(char* op) {
    if (!check(op)) {
        error_at(token ? token->str : user_input, "expected token '%s'", op);
    }
    token = token->next;
}

Token* expect_kind(TokenKind kind) {
    if (!token || token->kind != kind) error_at(token ? token->str : user_input, "unexpected token");
    Token* t = token;
    token = token->next;
    return t;
}

int expect_number() {
    if (!token || token->kind != TK_NUM) error_at(token ? token->str : user_input, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token && token->kind == TK_EOF;
}

/* --- Type system helpers --- */

Type* new_type(TypeKind ty, int size) {
    Type* t = calloc(1, sizeof(Type));
    t->ty = ty;
    t->size = size;
    return t;
}

Type* pointer_to(Type* base) {
    Type* t = new_type(PTR, 8);
    t->ptr_to = base;
    return t;
}

void add_type(Node* node) {
    if (!node || node->type) return;

    if (node->kind == ND_FUNC_DEF) { add_type(node->lhs); return; }

    add_type(node->lhs);
    add_type(node->rhs);

    if (node->block) { for (int i = 0; i < node->block_len; i++) add_type(node->block[i]); }
    if (node->args) { for (int i = 0; i < node->args_len; i++) add_type(node->args[i]); }

    Type* lhs_t = node->lhs ? node->lhs->type : NULL;

    if (node->kind == ND_LVAR && node->var && node->var->type->ty == ARRAY) {
        node->type = pointer_to(node->var->type->ptr_to);
        return;
    }

    switch (node->kind) {
        case ND_NUM: case ND_EQ: case ND_NE: case ND_LT: case ND_LE:
            node->type = new_type(INT, 4);
            return;
        case ND_ADD: case ND_SUB: case ND_MUL: case ND_DIV:
            if (lhs_t && (lhs_t->ty == PTR || lhs_t->ty == ARRAY)) node->type = lhs_t;
            else node->type = new_type(INT, 4);
            return;
        case ND_ASSIGN: case ND_RETURN:
            node->type = lhs_t;
            return;
        case ND_LVAR:
            if (node->var) node->type = node->var->type;
            else if (node->lhs) node->type = node->type;
            else error("unknown lvar type");
            return;
        case ND_ADDR:
            node->type = pointer_to(node->lhs->type);
            return;
        case ND_DEREF:
            if (!lhs_t || (lhs_t->ty != PTR && lhs_t->ty != ARRAY)) error_at(token->str, "dereference of non-pointer/array");
            node->type = lhs_t->ptr_to;
            return;
        case ND_FUNC_CALL:
            node->type = new_type(INT, 4);
            return;
        default: return;
    }
}

/* --- AST constructors --- */

Node* new_node(NodeKind kind) {
    Node* n = calloc(1, sizeof(Node));
    n->kind = kind;
    return n;
}

Node* new_binary(NodeKind kind, Node* lhs, Node* rhs) {
    Node* n = new_node(kind);
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

Node* new_num(int val) {
    Node* n = new_node(ND_NUM);
    n->val = val;
    return n;
}

/* --- Local variable helpers --- */

LVar* find_lvar(Token* tok) {
    for (LVar* v = locals; v; v = v->next) {
        if ((int)tok->len == v->len && strncmp(tok->str, v->name, v->len) == 0) return v;
    }
    return NULL;
}

/* --- Parser --- */

Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();
Node* function_def();
Type* struct_specifier();

Type* find_type(char* p, int len, Type* list) {
    for (Type* ty = list; ty; ty = ty->ptr_to) {
        if (ty->name && len == (int)strlen(ty->name) && strncmp(ty->name, p, len) == 0) return ty;
    }
    return NULL;
}

Type* type_specifier() {
    if (consume("int")) return new_type(INT, 4);
    if (check_kind(TK_STRUCT)) return struct_specifier();

    if (check_kind(TK_IDENT)) {
        Token* t = token;
        Type* ty = find_type(t->str, t->len, typedefs);
        if (ty) {
            token = token->next;
            return ty;
        }
    }
    error_at(token->str, "expected type specifier");
    return NULL;
}

Member* struct_declaration() {
    Type* type = type_specifier();
    while (consume("*")) {
        type = pointer_to(type);
    }
    Token* t = expect_kind(TK_IDENT);
    expect(";");

    Member* mem = calloc(1, sizeof(Member));
    mem->type = type;
    mem->name = strndup(t->str, t->len);
    return mem;
}

Type* struct_specifier() {
    expect_kind(TK_STRUCT);

    if (check_kind(TK_IDENT) && !check("{")) {
        Token* tag_tok = expect_kind(TK_IDENT);
        Type* existing_tag = find_type(tag_tok->str, tag_tok->len, struct_tags);
        if (!existing_tag) error_at(tag_tok->str, "undefined struct tag");
        return existing_tag;
    }

    Token* tag_tok = consume_kind(TK_IDENT);
    expect("{");

    Type* ty = new_type(STRUCT, 0);

    Member head = {0};
    Member* cur = &head;
    int offset = 0;

    while (!check("}")) {
        Member* mem = struct_declaration();

        offset = (offset + mem->type->size + 7) & ~7;
        mem->offset = offset;
        offset += mem->type->size;

        cur->next = mem;
        cur = mem;
    }
    expect("}");

    ty->members = head.next;
    ty->size = (offset + 7) & ~7;

    if (tag_tok) {
        ty->name = strndup(tag_tok->str, tag_tok->len);
        ty->ptr_to = struct_tags;
        struct_tags = ty;
    }
    return ty;
}

LVar* declaration() {
    if (consume_kind(TK_TYPEDEF)) {
        Type* base_ty = type_specifier();
        while (consume("*")) {
            base_ty = pointer_to(base_ty);
        }
        Token* t = expect_kind(TK_IDENT);
        expect(";");

        Type* ty = calloc(1, sizeof(Type));
        ty->ty = base_ty->ty;
        ty->size = base_ty->size;
        ty->ptr_to = base_ty->ptr_to;
        ty->members = base_ty->members;
        ty->array_len = base_ty->array_len;

        ty->name = strndup(t->str, t->len);
        ty->ptr_to = typedefs;
        typedefs = ty;
        return NULL;
    }

    Type* ty = type_specifier();

    while (consume("*")) {
        ty = pointer_to(ty);
    }

    Token* t = expect_kind(TK_IDENT);

    while (consume("[")) {
        if (token->kind != TK_NUM) error_at(token->str, "array size must be an integer literal");
        long len = expect_number();
        expect("]");

        Type* array_type = calloc(1, sizeof(Type));
        array_type->ty = ARRAY;
        array_type->ptr_to = ty;
        array_type->array_len = len;
        array_type->size = (int)(len * ty->size);

        ty = array_type;
    }

    LVar* v = calloc(1, sizeof(LVar));
    v->next = locals;
    v->name = strndup(t->str, t->len);
    v->len = t->len;
    v->type = ty;
    v->kind = LOCAL;
    locals = v;

    if (consume("=")) {
        v->init = expr();
    }

    return v;
}

void program() {
    while (!at_eof()) {
        if (check("int") && token->next && token->next->kind == TK_IDENT && token->next->next && check(token->next->next->str)) {
             if (strcmp(token->next->next->str, "(") == 0) {
                Node* n = function_def();
                (void)n;
                continue;
            }
        }

        LVar* v = declaration();
        if (v && v->init) {
            error_at(token->str, "global variable initialization not fully supported");
        }
    }
}

Node* function_def() {
    LVar* old_locals = locals;
    locals = NULL;
    stack_offset = 0;

    Type* ret_type = type_specifier();
    while (consume("*")) {
        ret_type = pointer_to(ret_type);
    }

    Token* name_tok = expect_kind(TK_IDENT);
    expect("(");

    LVar** params = NULL;
    int cap = 0, len = 0;
    while (!check(")")) {
        if (len > 0) expect(",");

        Type* param_type = type_specifier();
        while (consume("*")) {
            param_type = pointer_to(param_type);
        }
        Token* param_tok = expect_kind(TK_IDENT);

        LVar* v = calloc(1, sizeof(LVar));
        v->next = locals;
        v->name = strndup(param_tok->str, param_tok->len);
        v->len = param_tok->len;
        v->type = param_type;
        v->kind = PARAM;
        locals = v;

        if (len + 1 > cap) {
            cap = cap ? cap * 2 : 4;
            params = realloc(params, sizeof(LVar*) * cap);
        }
        params[len++] = v;
    }
    expect(")");

    Node* body = stmt();

    LVar* cur = locals;
    int offset = 0;
    while (cur) {
        offset = (offset + cur->type->size + 7) & ~7;
        cur->offset = offset;
        cur = cur->next;
    }
    stack_offset = offset;

    Node* n = new_node(ND_FUNC_DEF);
    n->funcname = strndup(name_tok->str, name_tok->len);
    n->ret_type = ret_type;
    n->lhs = body;
    n->params = params;
    n->param_len = len;

    Function* func = calloc(1, sizeof(Function));
    func->name = n->funcname;
    func->len = name_tok->len;
    func->node = n;
    func->locals = locals;
    func->stack_offset = stack_offset;
    func->next = functions;
    functions = func;

    locals = old_locals;
    return n;
}

Node* stmt() {
    if (consume("return")) {
        Node* n = new_node(ND_RETURN);
        n->lhs = expr();
        expect(";");
        return n;
    }
    if (consume("{")) {
        Node* n = new_node(ND_BLOCK);
        Node** stmts = NULL;
        int cap = 0, len = 0;
        while (!check("}")) {
            if (at_eof()) error("unclosed block");
            Node* s = stmt();
            if (s) {
                if (len + 1 > cap) {
                    cap = cap ? cap * 2 : 8;
                    stmts = realloc(stmts, sizeof(Node*) * cap);
                }
                stmts[len++] = s;
            }
        }
        expect("}");
        n->block = stmts;
        n->block_len = len;
        return n;
    }
    if (consume("if")) {
        expect("(");
        Node* cond = expr();
        expect(")");
        Node* then = stmt();
        Node* n = new_node(ND_IF);
        n->lhs = cond;
        n->rhs = then;
        if (consume("else")) {
            Node* els = stmt();
            n->rhs->rhs = els;
        }
        return n;
    }
    if (consume("while")) {
        expect("(");
        Node* cond = expr();
        expect(")");
        Node* body = stmt();
        Node* n = new_node(ND_WHILE);
        n->lhs = cond;
        n->rhs = body;
        return n;
    }

    if (check_kind(TK_TYPEDEF) || check_kind(TK_STRUCT) || check("int")) {
        LVar* v = declaration();
        if (v && v->init) {
            Node* n = new_node(ND_ASSIGN);
            Node* lvar_node = new_node(ND_LVAR);
            lvar_node->var = v;
            n->lhs = lvar_node;
            n->rhs = v->init;
            return n;
        }
        return NULL;
    }

    Node* n = expr();
    expect(";");
    return n;
}

Node* expr() { return assign(); }
Node* assign() {
    Node* node = equality();
    if (consume("=")) {
        Node* rhs = assign();
        return new_binary(ND_ASSIGN, node, rhs);
    }
    return node;
}
Node* equality() {
    Node* node = relational();
    /* deterministic loop: continue while next token is == or != */
    while (token && (check("==") || check("!="))) {
        if (consume("==")) node = new_binary(ND_EQ, node, relational());
        else if (consume("!=")) node = new_binary(ND_NE, node, relational());
    }
    return node;
}
Node* relational() {
    Node* node = add();
    /* deterministic loop: continue while relational operators present */
    while (token && (check("<") || check("<=") || check(">") || check(">="))) {
        if (consume("<")) node = new_binary(ND_LT, node, add());
        else if (consume("<=")) node = new_binary(ND_LE, node, add());
        else if (consume(">")) node = new_binary(ND_LT, add(), node);
        else if (consume(">=")) node = new_binary(ND_LE, add(), node);
    }
    return node;
}
Node* add() {
    Node* node = mul();
    /* deterministic loop: continue while + or - present */
    while (token && (check("+") || check("-"))) {
        if (consume("+")) node = new_binary(ND_ADD, node, mul());
        else if (consume("-")) node = new_binary(ND_SUB, node, mul());
    }
    return node;
}
Node* mul() {
    Node* node = unary();
    /* deterministic loop: continue while * or / present */
    while (token && (check("*") || check("/"))) {
        if (consume("*")) node = new_binary(ND_MUL, node, unary());
        else if (consume("/")) node = new_binary(ND_DIV, node, unary());
    }
    return node;
}

Node* unary() {
    if (consume("+")) return unary();
    if (consume("-")) return new_binary(ND_SUB, new_num(0), unary());
    if (consume("*")) {
        Node* n = new_node(ND_DEREF);
        n->lhs = unary();
        return n;
    }
    if (consume("&")) {
        Node* n = new_node(ND_ADDR);
        n->lhs = unary();
        return n;
    }

    Node* node = primary();

    /* member access: repeat while '.' present */
    while (token && check(".")) {
        consume(".");
        Token* t = expect_kind(TK_IDENT);

        if (!node->type || node->type->ty != STRUCT) error_at(t->str, "member access on non-struct type");

        Member* found_mem = NULL;
        for (Member* mem = node->type->members; mem; mem = mem->next) {
            if (mem->name && t->len == (int)strlen(mem->name) && strncmp(mem->name, t->str, t->len) == 0) {
                found_mem = mem;
                break;
            }
        }
        if (!found_mem) error_at(t->str, "no such member in struct");

        Node* n = new_node(ND_LVAR);
        n->type = found_mem->type;
        n->offset = found_mem->offset;
        n->lhs = node;
        node = n;
    }

    return node;
}

Node* primary() {
    if (consume("(")) {
        Node* n = expr();
        expect(")");
        return n;
    }
    if (check_kind(TK_NUM)) {
        Token* t = consume_kind(TK_NUM);
        return new_num(t->val);
    }
    if (check_kind(TK_IDENT)) {
        Token* t = consume_kind(TK_IDENT);
        if (check("(")) {
            expect("(");
            Node* n = new_node(ND_FUNC_CALL);
            n->funcname = strndup(t->str, t->len);
            Node** args = NULL;
            int cap = 0, len = 0;
            while (!check(")")) {
                if (at_eof()) error("unclosed argument list");
                if (len > 0) expect(",");
                Node* arg = assign();
                if (len + 1 > cap) {
                    cap = cap ? cap * 2 : 4;
                    args = realloc(args, sizeof(Node*) * cap);
                }
                args[len++] = arg;
            }
            expect(")");
            n->args = args;
            n->args_len = len;
            return n;
        } else {
            LVar* v = find_lvar(t);
            if (!v) error_at(t->str, "undefined variable '%s'", strndup(t->str, t->len));
            Node* n = new_node(ND_LVAR);
            n->var = v;
            n->offset = v->offset;
            n->varname = v->name;
            return n;
        }
    }
    error_at(token ? token->str : user_input, "unexpected token in primary");
    return NULL;
}

/* --- Code generation --- */

/* forward declaration for codegen function used inside gen_lval */
void gen(Node* node);

int labelseq = 0;
char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void push() { printf("  push rax\n"); }
void pop(char* reg) { printf("  pop %s\n", reg); }

void gen_lval(Node* node) {
    if (node->kind == ND_LVAR) {
        if (node->var) {
            printf("  lea rax, [rbp-%d]\n", node->offset);
            push();
            return;
        }

        if (node->lhs && node->type && node->lhs->type && node->lhs->type->ty == STRUCT) {
            gen_lval(node->lhs);
            pop("rax");
            printf("  add rax, %d\n", node->offset);
            push();
            return;
        }
    }
    if (node->kind == ND_DEREF) {
        gen(node->lhs);
        return;
    }
    error("not an lvalue");
}

void gen(Node* node) {
    if (!node) return;

    switch (node->kind) {
        case ND_NUM:
            printf("  mov rax, %d\n", node->val);
            push();
            return;

        case ND_LVAR:
            gen_lval(node);
            pop("rax");

            if (node->var && node->var->type->ty == ARRAY) {
                push();
                return;
            }

            if (node->type && node->type->ty == INT) {
                printf("  mov rax, [rax]\n");
                push();
                return;
            } else {
                printf("  mov rax, [rax]\n");
                push();
                return;
            }

        case ND_ASSIGN: {
            gen_lval(node->lhs);
            gen(node->rhs);
            pop("rdi"); // rhs value
            pop("rax"); // lval address
            printf("  mov [rax], rdi\n");
            printf("  mov rax, rdi\n");
            push();
            return;
        }

        case ND_ADDR:
            gen_lval(node->lhs);
            return;

        case ND_DEREF:
            gen(node->lhs);
            pop("rax");
            printf("  mov rax, [rax]\n");
            push();
            return;

        case ND_RETURN:
            gen(node->lhs);
            pop("rax");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;

        case ND_IF: {
            int seq = labelseq++;
            gen(node->lhs);
            pop("rax");
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%d\n", seq);
            gen(node->rhs);
            printf("  jmp .Lend%d\n", seq);
            printf(".Lelse%d:\n", seq);
            if (node->rhs && node->rhs->rhs) gen(node->rhs->rhs);
            printf(".Lend%d:\n", seq);
            return;
        }

        case ND_WHILE: {
            int seq = labelseq++;
            printf(".Lbegin%d:\n", seq);
            gen(node->lhs);
            pop("rax");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", seq);
            gen(node->rhs);
            printf("  jmp .Lbegin%d\n", seq);
            printf(".Lend%d:\n", seq);
            return;
        }

        case ND_BLOCK:
            for (int i = 0; i < node->block_len; i++) gen(node->block[i]);
            return;

        case ND_FUNC_CALL: {
            for (int i = node->args_len - 1; i >= 0; i--) {
                gen(node->args[i]);
            }
            for (int i = 0; i < node->args_len && i < 6; i++) {
                printf("  pop %s\n", arg_regs[i]);
            }
            printf("  call %s\n", node->funcname);
            push();
            return;
        }

        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE: {
            gen(node->lhs);
            gen(node->rhs);
            pop("rdi");
            pop("rax");
            printf("  cmp rax, rdi\n");
            if (node->kind == ND_EQ) printf("  sete al\n");
            else if (node->kind == ND_NE) printf("  setne al\n");
            else if (node->kind == ND_LT) printf("  setl al\n");
            else if (node->kind == ND_LE) printf("  setle al\n");
            printf("  movzb rax, al\n");
            push();
            return;
        }

        case ND_ADD:
        case ND_SUB:
        case ND_MUL:
        case ND_DIV: {
            gen(node->lhs);
            gen(node->rhs);
            pop("rdi");
            pop("rax");
            if (node->kind == ND_ADD) printf("  add rax, rdi\n");
            else if (node->kind == ND_SUB) printf("  sub rax, rdi\n");
            else if (node->kind == ND_MUL) printf("  imul rax, rdi\n");
            else if (node->kind == ND_DIV) {
                printf("  cqo\n");
                printf("  idiv rdi\n");
            }
            push();
            return;
        }

        case ND_FUNC_DEF:
            return;

        default:
            error("codegen: unhandled node kind %d", node->kind);
    }
}

/* --- Top-level codegen for functions --- */

void gen_function(Function* f) {
    printf(".global %s\n", f->name);
    printf("%s:\n", f->name);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    if (f->stack_offset > 0) {
        printf("  sub rsp, %d\n", f->stack_offset);
    }

    for (int i = 0; i < f->node->param_len && i < 6; i++) {
        LVar* p = f->node->params[i];
        printf("  mov [rbp-%d], %s\n", p->offset, arg_regs[i]);
    }

    gen(f->node->lhs);

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

/* --- Main --- */

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: cc <input.c>\n");
        return 1;
    }
    filename = argv[1];
    original_source_text = read_file(filename);

    char* main_dir = dir_of_path(filename);

    user_input = preprocess(original_source_text, main_dir ? main_dir : ".");

    if (original_source_text) free(original_source_text);
    original_source_text = user_input;

    free(main_dir);

    token = tokenize();
    program();

    for (Function* f = functions; f; f = f->next) {
        add_type(f->node);
    }

    printf(".intel_syntax noprefix\n");
    printf(".text\n");

    int n = 0;
    for (Function* f = functions; f; f = f->next) n++;
    Function** arr = malloc(sizeof(Function*) * (n > 0 ? n : 1));
    int i = 0;
    for (Function* f = functions; f; f = f->next) arr[i++] = f;
    for (int j = n - 1; j >= 0; j--) gen_function(arr[j]);
    free(arr);

    return 0;
}
