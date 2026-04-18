// cc.c - small C compiler (single-file, with #include support for "..." and <...>)
// Compile: gcc -o cc cc.c
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
/* parse_directive and preprocess now accept output buffer pointers and current_dir */
char* parse_directive(char* p, char** output_ptr, char** q_ptr, size_t *output_cap_ptr, const char* current_dir);
char* preprocess(char* source, const char* current_dir);
bool is_file_included(const char* path);
void add_included_file(const char* path);

/* --- Types --- */

typedef struct Node Node;
typedef struct Token Token;
typedef struct Type Type;
typedef struct LVar LVar;
typedef struct Function Function;
typedef struct Define Define;
typedef struct Member Member;
typedef struct IncludedFile IncludedFile;

typedef enum {
    tk_reserved, tk_num, tk_eof, tk_ident, tk_return, tk_if, tk_else,
    tk_while, tk_for, tk_sizeof, tk_string, tk_struct, tk_typedef,
    tk_enum, tk_break, tk_continue,
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
    nd_add, nd_sub, nd_mul, nd_div, nd_num, nd_eq, nd_ne, nd_lt, nd_le,
    nd_assign, nd_lvar, nd_return, nd_if, nd_while, nd_for, nd_block,
    nd_func_call, nd_addr, nd_deref, nd_func_def,
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

typedef enum { int_ty, ptr_ty, struct_ty, array_ty } TypeKind;

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
    enum { local, global, param } kind;
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
    int active; // recursion guard
};

struct IncludedFile {
    IncludedFile* next;
    char* path;
};

/* --- Globals --- */

Token* token;
char* user_input; // Holds PREPROCESSED text
char* original_source_text; // RAW file content
char* filename;
LVar* locals;
int stack_offset = 0;
Function* functions;
Define* defines;
Type* typedefs;
Type* struct_tags;
IncludedFile* included_files;

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
    if (!path) return NULL;
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
    Define* def = defines;
    bool check_more = true;
    while (check_more) {
        if (!def) {
            check_more = false;
            continue;
        }
        // recursion check
        if (!def->active && def->name_len == len && strncmp(def->name, p, len) == 0) return def;
        def = def->next;
    }
    return NULL;
}

bool is_file_included(const char* path) {
    for (IncludedFile* f = included_files; f; f = f->next) {
        if (strcmp(f->path, path) == 0) return true;
    }
    return false;
}

void add_included_file(const char* path) {
    IncludedFile* f = calloc(1, sizeof(IncludedFile));
    f->path = strdup(path);
    f->next = included_files;
    included_files = f;
}

char* skip_whitespace(char* p) {
    while (isspace((unsigned char)*p)) p++;
    return p;
}

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

static char* dir_of_path(const char* path) {
    const char* p = strrchr(path, '/');
    if (!p) return strdup(".");
    size_t len = (size_t)(p - path);
    char* d = malloc(len + 1);
    memcpy(d, path, len);
    d[len] = '\0';
    return d;
}

static char* try_include_candidate(const char* prefix, const char* path) {
    size_t prefix_len = prefix ? strlen(prefix) : 0;
    size_t path_len = path ? strlen(path) : 0;
    size_t cand_len = prefix_len + path_len + 1;
    char* cand = malloc(cand_len);
    if (!cand) return NULL;
    cand[0] = '\0';
    if (prefix_len) strcpy(cand, prefix);
    strcat(cand, path);
    char* buf = try_read_file(cand);
    free(cand);
    return buf;
}

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
        def->active = 0;
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

        if (is_file_included(path)) {
            free(path);
            *output_ptr = output;
            *q_ptr = q;
            return p;
        }

        char* included_raw = NULL;
        bool read_ok = false;
        const char* system_dirs[] = { "/usr/include/", "/usr/local/include/" };

        if (is_quoted) {
            if (current_dir && strcmp(current_dir, ".") != 0) {
                size_t cur_len = strlen(current_dir);
                char* prefix = malloc(cur_len + 2);
                strcpy(prefix, current_dir);
                if (prefix[cur_len - 1] != '/') strcat(prefix, "/");
                included_raw = try_include_candidate(prefix, path);
                free(prefix);
                if (included_raw) read_ok = true;
            }
            if (!read_ok) {
                included_raw = try_include_candidate("", path);
                if (included_raw) read_ok = true;
            }
            if (!read_ok) {
                included_raw = try_include_candidate("./", path);
                if (included_raw) read_ok = true;
            }
        }

        int i = 0;
        while (i < sizeof(system_dirs)/sizeof(system_dirs[0]) && !read_ok) {
            included_raw = try_include_candidate(system_dirs[i], path);
            if (included_raw) read_ok = true;
            i++;
        }
        if (!is_quoted && !read_ok) {
            included_raw = try_include_candidate("", path);
            if (included_raw) read_ok = true;
        }

        if (!read_ok || !included_raw) {
            error_at(name_start, "cannot open include file '%s'", path);
            free(path);
            *output_ptr = output;
            *q_ptr = q;
            return p;
        }

        char* included_dir = dir_of_path(path);
        add_included_file(path);

        char* included_preprocessed = preprocess(included_raw, included_dir ? included_dir : ".");
        free(included_raw);
        free(included_dir);

        size_t inc_len = strlen(included_preprocessed);
        ensure_output_capacity(output_ptr, output_cap_ptr, q_ptr, inc_len + 2);
        output = *output_ptr;
        q = *q_ptr;

        memcpy(q, included_preprocessed, inc_len);
        q += inc_len;
        if (inc_len == 0 || q[-1] != '\n') *q++ = '\n';

        free(included_preprocessed);
        free(path);

        *output_ptr = output;
        *q_ptr = q;
        return p;
    }

    // skip unknown directives
    while (*p && *p != '\n') p++;
    *output_ptr = output;
    *q_ptr = q;
    return p;
}

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
            bool end_found = false;
            while (*p && !end_found) {
                if (startswith(p, "*/")) {
                    end_found = true;
                } else {
                    if (*p == '\n') {
                        ensure_output_capacity(&output, &output_cap, &q, 1);
                        *q++ = *p;
                    }
                    p++;
                }
            }
            if (end_found) p += 2;
            continue;
        }

        if (*p == '"' || *p == '\'') {
            char marker = *p;
            char* start = p;
            p++;
            while (*p && *p != marker) {
                if (*p == '\\') { p++; if (*p) p++; }
                else p++;
            }
            if (*p == marker) p++;
            size_t len = (size_t)(p - start);
            ensure_output_capacity(&output, &output_cap, &q, len + 1);
            memcpy(q, start, len);
            q += len;
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            char* ident_start = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            int len = (int)(p - ident_start);

            Define* def = find_define(ident_start, len);
            if (def) {
                def->active = 1;
                ensure_output_capacity(&output, &output_cap, &q, def->body_len);
                for (int i = 0; i < def->body_len; i++) *q++ = def->body[i];
                def->active = 0;
                continue;
            }

            // Ensure capacity for the identifier if it's not a macro
            ensure_output_capacity(&output, &output_cap, &q, len + 1);
            p = ident_start;
            while (isalnum((unsigned char)*p) || *p == '_') {
                 *q++ = *p++;
            }
            continue;
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
    Token* tok = new_token(tk_num, cur, start, (int)(p - start + 1), line_start, line_num, col_num);
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
            bool end_found = false;
            while (*p && !end_found) {
                if (*p == '"') {
                    end_found = true;
                } else if (*p == '\\') {
                    p += 2;
                } else {
                    p++;
                }
            }
            if (end_found) p++;
            cur = new_token(tk_string, cur, start, (int)(p - start), line_start, current_line, col_num);
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
            Token* tok = new_token(tk_num, cur, start, (int)(p - start), line_start, current_line, col_num);
            tok->val = (int)val;
            cur = tok;
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            char* start = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            int len = (int)(p - start);
            Token* tok = new_token(tk_ident, cur, start, len, line_start, current_line, col_num);
            if (len == 6 && strncmp(start, "return", 6) == 0) tok->kind = tk_return;
            else if (len == 2 && strncmp(start, "if", 2) == 0) tok->kind = tk_if;
            else if (len == 4 && strncmp(start, "else", 4) == 0) tok->kind = tk_else;
            else if (len == 5 && strncmp(start, "while", 5) == 0) tok->kind = tk_while;
            else if (len == 3 && strncmp(start, "for", 3) == 0) tok->kind = tk_for;
            else if (len == 3 && strncmp(start, "int", 3) == 0) tok->kind = tk_reserved;
            else if (len == 6 && strncmp(start, "struct", 6) == 0) tok->kind = tk_struct;
            else if (len == 7 && strncmp(start, "typedef", 7) == 0) tok->kind = tk_typedef;
            cur = tok;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=") ||
            startswith(p, "&&") || startswith(p, "||") || startswith(p, "++") || startswith(p, "--")) {
            Token* tok = new_token(tk_reserved, cur, p, 2, line_start, current_line, col_num);
            p += 2;
            cur = tok;
            continue;
        }

        cur = new_token(tk_reserved, cur, p, 1, line_start, current_line, col_num);
        p++;
    }
    new_token(tk_eof, cur, p, 0, line_start, current_line, (int)(p - line_start + 1));
    return head.next;
}

/* --- Token helpers --- */

bool consume(char* op) {
    if (!token) return false;
    if (token->kind != tk_reserved) return false;
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
    if (token->kind != tk_reserved) return false;
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
    if (!token || token->kind != tk_num) error_at(token ? token->str : user_input, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token && token->kind == tk_eof;
}

/* --- Type system helpers --- */

Type* new_type(TypeKind ty, int size) {
    Type* t = calloc(1, sizeof(Type));
    t->ty = ty;
    t->size = size;
    return t;
}

Type* pointer_to(Type* base) {
    Type* t = new_type(ptr_ty, 8);
    t->ptr_to = base;
    return t;
}

void add_type(Node* node) {
    if (!node || node->type) return;

    if (node->kind == nd_func_def) { add_type(node->lhs); return; }

    add_type(node->lhs);
    add_type(node->rhs);

    if (node->block) { for (int i = 0; i < node->block_len; i++) add_type(node->block[i]); }
    if (node->args) { for (int i = 0; i < node->args_len; i++) add_type(node->args[i]); }

    Type* lhs_t = node->lhs ? node->lhs->type : NULL;

    if (node->kind == nd_lvar && node->var && node->var->type->ty == array_ty) {
        node->type = pointer_to(node->var->type->ptr_to);
        return;
    }

    switch (node->kind) {
        case nd_num: case nd_eq: case nd_ne: case nd_lt: case nd_le:
            node->type = new_type(int_ty, 4);
            return;
        case nd_add: case nd_sub: case nd_mul: case nd_div:
            if (lhs_t && (lhs_t->ty == ptr_ty || lhs_t->ty == array_ty)) node->type = lhs_t;
            else node->type = new_type(int_ty, 4);
            return;
        case nd_assign: case nd_return:
            node->type = lhs_t;
            return;
        case nd_lvar:
            if (node->var) node->type = node->var->type;
            else if (node->lhs) node->type = node->type;
            else error("unknown lvar type");
            return;
        case nd_addr:
            node->type = pointer_to(node->lhs->type);
            return;
        case nd_deref:
            if (!lhs_t || (lhs_t->ty != ptr_ty && lhs_t->ty != array_ty)) error_at(token->str, "dereference of non-pointer/array");
            node->type = lhs_t->ptr_to;
            return;
        case nd_func_call:
            node->type = new_type(int_ty, 4);
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
    Node* n = new_node(nd_num);
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
    Type* ty = list;
    bool check_more = true;
    while (check_more) {
        if (!ty) {
            check_more = false;
            continue;
        }

        if (ty->name && len == (int)strlen(ty->name) && strncmp(ty->name, p, len) == 0) return ty;
        ty = ty->ptr_to;
    }
    return NULL;
}

Type* type_specifier() {
    if (consume("int")) return new_type(int_ty, 4);
    if (check_kind(tk_struct)) return struct_specifier();

    if (check_kind(tk_ident)) {
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
    Token* t = expect_kind(tk_ident);
    expect(";");

    Member* mem = calloc(1, sizeof(Member));
    mem->type = type;
    mem->name = strndup(t->str, t->len);
    return mem;
}

Type* struct_specifier() {
    expect_kind(tk_struct);

    if (check_kind(tk_ident) && !check("{")) {
        Token* tag_tok = expect_kind(tk_ident);
        Type* existing_tag = find_type(tag_tok->str, tag_tok->len, struct_tags);
        if (!existing_tag) error_at(tag_tok->str, "undefined struct tag");
        return existing_tag;
    }

    Token* tag_tok = consume_kind(tk_ident);
    expect("{");

    Type* ty = new_type(struct_ty, 0);

    Member head = {0};
    Member* cur = &head;
    int offset = 0;

    bool check_end_brac = true;
    while (check_end_brac) {
        if (check("}")) {
            check_end_brac = false;
            continue;
        }
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
    if (consume_kind(tk_typedef)) {
        Type* base_ty = type_specifier();
        while (consume("*")) {
            base_ty = pointer_to(base_ty);
        }
        Token* t = expect_kind(tk_ident);
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

    Token* t = expect_kind(tk_ident);

    while (consume("[")) {
        if (token->kind != tk_num) error_at(token->str, "array size must be an integer literal");
        long len = expect_number();
        expect("]");

        Type* array_type = calloc(1, sizeof(Type));
        array_type->ty = array_ty;
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
    v->kind = local;
    locals = v;

    if (consume("=")) {
        v->init = expr();
    }

    return v;
}

void program() {
    bool keep_parsing = true;
    while (keep_parsing) {
        if (at_eof()) {
            keep_parsing = false;
            continue;
        }

        if (check("int") && token->next && token->next->kind == tk_ident && token->next->next && check(token->next->next->str)) {
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

    Token* name_tok = expect_kind(tk_ident);
    expect("(");

    LVar** params = NULL;
    int cap = 0, len = 0;
    bool check_end_paren = true;
    while (check_end_paren) {
        if (check(")")) {
            check_end_paren = false;
            continue;
        }

        if (len > 0) expect(",");

        Type* param_type = type_specifier();
        while (consume("*")) {
            param_type = pointer_to(param_type);
        }
        Token* param_tok = expect_kind(tk_ident);

        LVar* v = calloc(1, sizeof(LVar));
        v->next = locals;
        v->name = strndup(param_tok->str, param_tok->len);
        v->len = param_tok->len;
        v->type = param_type;
        v->kind = param;
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

    Node* n = new_node(nd_func_def);
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
        Node* n = new_node(nd_return);
        n->lhs = expr();
        expect(";");
        return n;
    }
    if (consume("{")) {
        Node* n = new_node(nd_block);
        Node** stmts = NULL;
        int cap = 0, len = 0;
        bool check_end_brac = true;
        while (check_end_brac) {
            if (check("}")) {
                check_end_brac = false;
                continue;
            }
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
        Node* n = new_node(nd_if);
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
        Node* n = new_node(nd_while);
        n->lhs = cond;
        n->rhs = body;
        return n;
    }

    if (check_kind(tk_typedef) || check_kind(tk_struct) || check("int")) {
        LVar* v = declaration();
        if (v && v->init) {
            Node* n = new_node(nd_assign);
            Node* lvar_node = new_node(nd_lvar);
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
        return new_binary(nd_assign, node, rhs);
    }
    return node;
}
Node* equality() {
    Node* node = relational();
    bool check_more = true;
    while (check_more) {
        if (consume("==")) {
            node = new_binary(nd_eq, node, relational());
        } else if (consume("!=")) {
            node = new_binary(nd_ne, node, relational());
        } else {
            check_more = false;
        }
    }
    return node;
}
Node* relational() {
    Node* node = add();
    bool check_more = true;
    while (check_more) {
        if (consume("<")) {
            node = new_binary(nd_lt, node, add());
        } else if (consume("<=")) {
            node = new_binary(nd_le, node, add());
        } else if (consume(">")) {
            node = new_binary(nd_lt, add(), node);
        } else if (consume(">=")) {
            node = new_binary(nd_le, add(), node);
        } else {
            check_more = false;
        }
    }
    return node;
}
Node* add() {
    Node* node = mul();
    bool check_more = true;
    while (check_more) {
        if (consume("+")) {
            node = new_binary(nd_add, node, mul());
        } else if (consume("-")) {
            node = new_binary(nd_sub, node, mul());
        } else {
            check_more = false;
        }
    }
    return node;
}
Node* mul() {
    Node* node = unary();
    bool check_more = true;
    while (check_more) {
        if (consume("*")) {
            node = new_binary(nd_mul, node, unary());
        } else if (consume("/")) {
            node = new_binary(nd_div, node, unary());
        } else {
            check_more = false;
        }
    }
    return node;
}

Node* unary() {
    if (consume("+")) return unary();
    if (consume("-")) return new_binary(nd_sub, new_num(0), unary());
    if (consume("*")) {
        Node* n = new_node(nd_deref);
        n->lhs = unary();
        return n;
    }
    if (consume("&")) {
        Node* n = new_node(nd_addr);
        n->lhs = unary();
        return n;
    }

    Node* node = primary();

    bool check_member = true;
    while (check_member) {
        if (consume(".")) {
            Token* t = expect_kind(tk_ident);

            if (!node->type || node->type->ty != struct_ty) error_at(t->str, "member access on non-struct type");

            Member* found_mem = NULL;
            Member* mem = node->type->members;
            while (mem) {
                if (mem->name && t->len == (int)strlen(mem->name) && strncmp(mem->name, t->str, t->len) == 0) {
                    found_mem = mem;
                    break;
                }
                mem = mem->next;
            }
            if (!found_mem) error_at(t->str, "no such member in struct");

            Node* n = new_node(nd_lvar);
            n->type = found_mem->type;
            n->offset = found_mem->offset;
            n->lhs = node;
            node = n;
        } else {
            check_member = false;
        }
    }

    return node;
}

Node* primary() {
    if (consume("(")) {
        Node* n = expr();
        expect(")");
        return n;
    }
    if (check_kind(tk_num)) {
        Token* t = consume_kind(tk_num);
        return new_num(t->val);
    }
    if (check_kind(tk_ident)) {
        Token* t = consume_kind(tk_ident);
        if (check("(")) {
            expect("(");
            Node* n = new_node(nd_func_call);
            n->funcname = strndup(t->str, t->len);
            Node** args = NULL;
            int cap = 0, len = 0;
            bool check_end_paren = true;
            while (check_end_paren) {
                if (check(")")) {
                    check_end_paren = false;
                    continue;
                }
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
            Node* n = new_node(nd_lvar);
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

int labelseq = 0;
char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void push() { printf("  push rax\n"); }
void pop(char* reg) { printf("  pop %s\n", reg); }

void gen_lval(Node* node) {
    if (node->kind == nd_lvar) {
        if (node->var) {
            printf("  lea rax, [rbp-%d]\n", node->offset);
            push();
            return;
        }

        if (node->lhs && node->type && node->lhs->type->ty == struct_ty) {
            gen_lval(node->lhs);
            pop("rax");
            printf("  add rax, %d\n", node->offset);
            push();
            return;
        }
    }
    if (node->kind == nd_deref) {
        gen(node->lhs);
        return;
    }
    error("not an lvalue");
}

void gen(Node* node) {
    if (!node) return;

    switch (node->kind) {
        case nd_num:
            printf("  mov rax, %d\n", node->val);
            push();
            return;

        case nd_lvar:
            gen_lval(node);
            pop("rax");

            if (node->var && node->var->type->ty == array_ty) {
                push();
                return;
            }

            // load value (default 8 bytes or 4 for int)
            if (node->type && node->type->ty == int_ty) {
                printf("  mov eax, [rax]\n");
                printf("  movsx rax, eax\n"); // sign extend from 32-bit to 64-bit
                push();
                return;
            } else {
                printf("  mov rax, [rax]\n");
                push();
                return;
            }

        case nd_assign: {
            gen_lval(node->lhs);
            gen(node->rhs);
            pop("rdi"); // rhs value
            pop("rax"); // lval address

            // save 4 bytes for int, 8 otherwise (pointers, structs)
            if (node->lhs->type && node->lhs->type->ty == int_ty) {
                printf("  mov [rax], edi\n");
            } else {
                printf("  mov [rax], rdi\n");
            }

            printf("  mov rax, rdi\n");
            push();
            return;
        }

        case nd_addr:
            gen_lval(node->lhs);
            return;

        case nd_deref:
            gen(node->lhs);
            pop("rax");

            // load 4 bytes for int, 8 otherwise
            if (node->type && node->type->ty == int_ty) {
                printf("  mov eax, [rax]\n");
                printf("  movsx rax, eax\n");
                push();
                return;
            } else {
                printf("  mov rax, [rax]\n");
                push();
                return;
            }

        case nd_return:
            gen(node->lhs);
            pop("rax");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;

        case nd_if: {
            int seq = labelseq++;
            gen(node->lhs);
            pop("rax");
            printf("  cmp rax, 0\n");
            printf("  je .l_else%d\n", seq);
            gen(node->rhs);
            printf("  jmp .l_end%d\n", seq);
            printf(".l_else%d:\n", seq);
            if (node->rhs && node->rhs->rhs) gen(node->rhs->rhs);
            printf(".l_end%d:\n", seq);
            return;
        }

        case nd_while: {
            int seq = labelseq++;
            printf(".l_begin%d:\n", seq);
            gen(node->lhs);
            pop("rax");
            printf("  cmp rax, 0\n");
            printf("  je .l_end%d\n", seq);
            gen(node->rhs);
            printf("  jmp .l_begin%d\n", seq);
            printf(".l_end%d:\n", seq);
            return;
        }

        case nd_block:
            for (int i = 0; i < node->block_len; i++) gen(node->block[i]);
            return;

        case nd_func_call: {
            // evaluate args right-to-left and move into registers
            for (int i = node->args_len - 1; i >= 0; i--) {
                gen(node->args[i]);
            }
            // pop into registers in order
            for (int i = 0; i < node->args_len && i < 6; i++) {
                printf("  pop %s\n", arg_regs[i]);
            }
            // if more than 6 args, they are ignored in this simple compiler
            printf("  call %s\n", node->funcname);
            push();
            return;
        }

        case nd_eq:
        case nd_ne:
        case nd_lt:
        case nd_le: {
            gen(node->lhs);
            gen(node->rhs);
            pop("rdi");
            pop("rax");
            printf("  cmp rax, rdi\n");
            if (node->kind == nd_eq) printf("  sete al\n");
            else if (node->kind == nd_ne) printf("  setne al\n");
            else if (node->kind == nd_lt) printf("  setl al\n");
            else if (node->kind == nd_le) printf("  setle al\n");
            printf("  movzb rax, al\n");
            push();
            return;
        }

        case nd_add:
        case nd_sub:
        case nd_mul:
        case nd_div: {
            gen(node->lhs);
            gen(node->rhs);
            pop("rdi");
            pop("rax");

            if (node->kind == nd_add) {
                if (node->type && node->type->ty == ptr_ty && node->rhs->type->ty == int_ty) {
                    printf("  imul rdi, %d\n", node->type->ptr_to->size);
                } else if (node->type && node->type->ty == ptr_ty && node->lhs->type->ty == int_ty) {
                    printf("  imul rax, %d\n", node->type->ptr_to->size);
                }
                printf("  add rax, rdi\n");
            }
            else if (node->kind == nd_sub) {
                if (node->type && node->type->ty == ptr_ty && node->rhs->type->ty == int_ty) {
                    printf("  imul rdi, %d\n", node->type->ptr_to->size);
                }
                printf("  sub rax, rdi\n");
            }
            else if (node->kind == nd_mul) printf("  imul rax, rdi\n");
            else if (node->kind == nd_div) {
                printf("  cqo\n");
                printf("  idiv rdi\n");
            }
            push();
            return;
        }

        case nd_func_def:
            // handled elsewhere
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
    // align stack to 16 bytes for call convention
    int stack_size = (f->stack_offset + 15) & ~15;
    if (stack_size > 0) {
        printf("  sub rsp, %d\n", stack_size);
    }

    // move parameters from registers to stack slots (if any)
    for (int i = 0; i < f->node->param_len && i < 6; i++) {
        LVar* p = f->node->params[i];
        // store register into local slot, using 32-bit mov for int (4-byte)
        if (p->type->ty == int_ty) {
            printf("  mov [rbp-%d], %s\n", p->offset, arg_regs[i]);
        } else {
            printf("  mov [rbp-%d], %s\n", p->offset, arg_regs[i]);
        }
    }

    gen(f->node->lhs);

    // if function didn't return explicitly, emit epilogue
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

/* --- Main --- */

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: cc <input.c>\n");
        return 1;
    }
    filename = argv[1];
    original_source_text = read_file(filename);

    /* determine directory of the main source file so quoted includes resolve relative to it */
    char* main_dir = dir_of_path(filename);

    /* preprocess with current_dir = main_dir so #include "file" prefers the including file's directory */
    user_input = preprocess(original_source_text, main_dir ? main_dir : ".");

    /* after preprocessing we set original_source_text to the combined preprocessed text so that
       error reporting maps locations to the amalgamated source and line numbers match the
       preprocessed content (includes expanded inline). */
    if (original_source_text) free(original_source_text);
    original_source_text = user_input;

    free(main_dir);

    token = tokenize();
    program();

    // add types for all functions
    Function* f = functions;
    while (f) {
        add_type(f->node);
        f = f->next;
    }

    // emit assembly header
    printf(".intel_syntax noprefix\n");
    printf(".text\n");

    // generate code for each function (reverse list to preserve original order)
    int n = 0;
    f = functions;
    while (f) {
        n++;
        f = f->next;
    }
    Function** arr = malloc(sizeof(Function*) * (n > 0 ? n : 1));
    int i = 0;
    f = functions;
    while (f) {
        arr[i++] = f;
        f = f->next;
    }
    for (int j = n - 1; j >= 0; j--) gen_function(arr[j]);
    free(arr);

    return 0;
}
