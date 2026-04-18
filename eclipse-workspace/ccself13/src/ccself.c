// cc.c
// minimal educational c compiler (amalgamated single-file)
// enhanced: full bitwise/logical operators, expanded preprocessor, and fixed-size arrays
// usage: cc file1.c [file2.c ...]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

// --- i. data structures and global state ---

// type system (added ty_array)
typedef enum { ty_int, ty_char, ty_long, ty_ptr, ty_array, ty_struct } type_kind; // <-- ADDED ty_array
typedef struct ty {
    type_kind kind;
    struct ty *base; // used for pointers and arrays
    int size;        // 1, 4, 8, or arbitrary for struct/array
    int align;       // alignment requirement
    int is_unsigned;
    size_t array_len; // <-- ADDED array_len
} ty;

// local variables
typedef struct lvar {
    struct lvar *next;
    char *name;
    int len;
    int offset;
    ty *type;
    int is_static;   // 1 if this local is static (stored in data)
    char *gname;     // global label name for static locals
} lvar;

// global static variable descriptor (for emission)
typedef struct gvar {
    struct gvar *next;
    char *name;
    int size;
} gvar;

// functions (added is_variadic and is_static)
typedef struct function {
    struct function *next;
    char *name;
    struct node *body;
    lvar *locals;
    int stack_size;
    int num_params;
    int is_variadic;
    int is_static;
} function;

// tokenizer
typedef enum { tk_eof, tk_reserved, tk_keyword, tk_num, tk_ident } token_kind;
typedef struct token {
    token_kind kind;
    struct token *next;
    long val;
    char *str;
    int len;
    char *file_pos; // start of the token for error reporting
    ty *type;
} token;

// ast nodes
typedef enum {
    nd_num, nd_lvar, nd_assign, nd_funcall, nd_return,
    nd_if, nd_while, nd_for, nd_block, nd_expr_stmt,
    nd_add, nd_sub, nd_mul, nd_div, nd_mod,
    nd_eq, nd_ne, nd_lt, nd_le,
    nd_addr, nd_deref, nd_break, nd_continue, nd_switch, nd_case,
    nd_shl, nd_shr, nd_band, nd_bor, nd_bxor, nd_logical_and, nd_logical_or,
    nd_not, nd_bitnot,
    nd_cond,
    // Compound Assignments
    nd_add_assign, nd_sub_assign, nd_mul_assign, nd_div_assign, nd_mod_assign,
    nd_shl_assign, nd_shr_assign, nd_band_assign, nd_bor_assign, nd_bxor_assign,
    nd_array_init_zero // <-- ADDED for local array zero initialization
} node_kind;

typedef struct node {
    node_kind kind;
    struct node *lhs; // condition for nd_cond, lhs for binary
    struct node *rhs; // then expression for nd_cond, rhs for binary
    struct node *els; // else expression for nd_cond, else for nd_if/nd_for
    struct node *next;
    long val;
    char *name;
    ty *type;
    lvar *var;
} node;

// control flow labels for break/continue
typedef struct continue_break_labels {
    int continue_label;
    int break_label;
    struct continue_break_labels *next;
} continue_break_labels;

// --- global compiler state ---
char *user_input;
token *current_token;
function *program_head = NULL;
function *current_func = NULL;
continue_break_labels *loop_or_switch_labels = NULL;
int label_count = 0;
int static_var_count = 0;

// global static locals list for emission
gvar *global_statics = NULL;

// --- preprocessor macro table (object-like macros only) ---
typedef struct macro {
    struct macro *next;
    char *name;
    char *replacement; // raw string replacement
} macro;
macro *macro_table = NULL;

// function prototypes for recursive descent and helpers
token *tokenize(char *p);
void codegen_program(function *funcs);
void add_type(node *n);
lvar *find_lvar(token *tok, ty *type, int is_static);
ty *parse_type_specifier(int *is_unsigned_ptr, int *is_static_ptr);
ty *parse_declarator(ty *base);
int consume(char *op);
int consume_keyword(char *kw);
token *expect_ident();
void expect(char *op);
long expect_number();
node *new_num_node(long val);
node *new_node(node_kind kind, node *lhs, node *rhs);
node *expr();
void parse_fundef();
node *declaration();
node *param_declaration();

// --- ii. utility and error functions (enhanced) ---

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int line_num = 1;
    char *line_start = user_input;
    for (char *p = user_input; p < loc; p++) {
        if (*p == '\n') {
            line_num++;
            line_start = p + 1;
        }
    }

    char *line_end = loc;
    while (*line_end != '\n' && *line_end != '\0') line_end++;

    int column = loc - line_start;

    fprintf(stderr, "\n--- compilation error ---\n");
    fprintf(stderr, "line %d:%d: error: ", line_num, column + 1);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);
    fprintf(stderr, "%*s^\n", column, "");
    exit(1);
}

void error_tok(token *tok, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    char *loc = tok->str;

    int line_num = 1;
    char *line_start = user_input;
    for (char *p = user_input; p < loc; p++) {
        if (*p == '\n') {
            line_num++;
            line_start = p + 1;
        }
    }

    char *line_end = loc;
    while (*line_end != '\n' && *line_end != '\0') line_end++;

    int column = loc - line_start;

    fprintf(stderr, "\n--- compilation error ---\n");
    fprintf(stderr, "line %d:%d: error: ", line_num, column + 1);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);
    fprintf(stderr, "%*s^", column, "");
    for (int i = 0; i < tok->len - 1; i++) fprintf(stderr, "~");
    fprintf(stderr, "\n");
    exit(1);
}

// --- type helpers ---

ty *new_type(type_kind kind, int size, int align) {
    ty *t = calloc(1, sizeof(ty));
    t->kind = kind;
    t->size = size;
    t->align = align;
    t->is_unsigned = 0;
    return t;
}
ty *ty_int_def() { return new_type(ty_int, 4, 4); }
ty *ty_char_def() { return new_type(ty_char, 1, 1); }
ty *ty_long_def() { return new_type(ty_long, 8, 8); }
ty *new_type_ptr(ty *base) {
    ty *t = new_type(ty_ptr, 8, 8);
    t->base = base;
    return t;
}
// <-- ADDED new_type_array
ty *new_type_array(ty *base, size_t len) {
    int total_size = base->size * len;
    ty *t = new_type(ty_array, total_size, base->align);
    t->base = base;
    t->array_len = len;
    return t;
}
ty *new_type_struct(int size, int align) {
    ty *t = new_type(ty_struct, size, align);
    return t;
}
ty *get_common_type(ty *t1, ty *t2) {
    // simplified common type promotion: always long if sizes differ
    if (t1->kind == ty_ptr) return t1;
    if (t2->kind == ty_ptr) return t2;
    if (t1->size >= t2->size) return t1;
    return t2;
}
void add_type(node *n) {
    if (!n) return;
    if (n->type) return;

    add_type(n->lhs);
    add_type(n->rhs);
    add_type(n->els); // recurse on els

    switch (n->kind) {
        case nd_addr: n->type = new_type_ptr(n->lhs->type); return;
        case nd_deref:
            if (n->lhs->type->kind != ty_ptr) error_tok(current_token, "cannot dereference non-pointer");
            n->type = n->lhs->type->base;
            return;
        case nd_assign:
        case nd_add_assign: case nd_sub_assign: case nd_mul_assign: case nd_div_assign: case nd_mod_assign:
        case nd_shl_assign: case nd_shr_assign: case nd_band_assign: case nd_bor_assign: case nd_bxor_assign:
            n->type = n->lhs->type; return;
        case nd_add:
        case nd_sub:
            // pointer arithmetic: if lhs is ptr, scale rhs
            if (n->lhs->type->kind == ty_ptr) {
                // array decay: array automatically becomes a pointer
                if (n->lhs->type->kind == ty_array) {
                    n->type = new_type_ptr(n->lhs->type->base);
                } else {
                    n->type = n->lhs->type; return;
                }
            }
            n->type = get_common_type(n->lhs->type, n->rhs->type); return;
        case nd_mul: case nd_div: case nd_mod:
        case nd_shl: case nd_shr: case nd_band: case nd_bor: case nd_bxor:
        case nd_logical_and: case nd_logical_or:
            n->type = get_common_type(n->lhs->type, n->rhs->type); return;
        case nd_eq: case nd_ne: case nd_lt: case nd_le:
        case nd_not: case nd_bitnot:
            n->type = ty_int_def(); return;
        case nd_cond:
            // type is the common type of the two resulting expressions (rhs and els)
            n->type = get_common_type(n->rhs->type, n->els->type);
            return;
        case nd_lvar:
            // Array decay: when an array is used in an expression, it decays to a pointer to its first element.
            if (n->var->type->kind == ty_array) {
                n->type = new_type_ptr(n->var->type->base);
            } else {
                n->type = n->var->type;
            }
            return;
        case nd_num: case nd_funcall:
            return;
        default: break;
    }
}

// --- variable management ---

// check if a token is a type specifier start
static int is_type_token(token *tok) {
    return tok->kind == tk_keyword &&
           (strncmp(tok->str, "int", tok->len) == 0 ||
            strncmp(tok->str, "char", tok->len) == 0 ||
            strncmp(tok->str, "long", tok->len) == 0 ||
            strncmp(tok->str, "unsigned", tok->len) == 0 ||
            strncmp(tok->str, "static", tok->len) == 0);
}

// find or create a local variable
lvar *find_lvar(token *tok, ty *type, int is_static) {
    // check if local already exists (simple scope model)
    for (lvar *l = current_func->locals; l; l = l->next) {
        if (l->len == tok->len && strncmp(l->name, tok->str, l->len) == 0) return l;
    }

    // create new local variable
    lvar *l = calloc(1, sizeof(lvar));
    l->next = current_func->locals;
    l->name = tok->str;
    l->len = tok->len;
    l->type = type;
    l->is_static = is_static;
    current_func->locals = l;

    if (is_static) {
        // static local: assign a global label name
        l->gname = malloc(32);
        snprintf(l->gname, 32, ".lstatic%d", static_var_count++);
        // also register in global_statics list for data emission
        gvar *g = calloc(1, sizeof(gvar));
        g->name = strdup(l->gname);
        g->size = type->size;
        g->next = global_statics;
        global_statics = g;
    } else {
        // normal local: assign offset relative to rbp
        int offset = current_func->stack_size + l->type->size;
        // align offset up to type's alignment requirement
        if (l->type->align > 1) {
            offset = (offset + l->type->align - 1) & ~(l->type->align - 1);
        }
        l->offset = offset;
        current_func->stack_size = l->offset;
    }

    return l;
}

// --- iii. preprocessor: handle #include "file", #define, #undef, #if/#ifdef/#ifndef etc. ---

// read an entire file into a malloc'd buffer (null-terminated). caller frees.
static char *read_file_to_buffer(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    rewind(f);
    char *buf = malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (n != (size_t)sz) { free(buf); return NULL; }
    *(buf + n) = '\0';
    return buf;
}

// helper to ensure output buffer capacity
static void ensure_capacity(char **outp, size_t *cap, size_t need) {
    if (*cap >= need) return;
    size_t newcap = (*cap == 0) ? 4096 : *cap;
    while (newcap < need) newcap *= 2;
    *outp = realloc(*outp, newcap);
    *cap = newcap;
}

// macro helpers
static void macro_define(const char *name, const char *replacement) {
    for (macro *m = macro_table; m; m = m->next) {
        if (strcmp(m->name, name) == 0) {
            free(m->replacement);
            m->replacement = strdup(replacement);
            return;
        }
    }
    macro *m = calloc(1, sizeof(macro));
    m->name = strdup(name);
    m->replacement = strdup(replacement);
    m->next = macro_table;
    macro_table = m;
}
static void macro_undef(const char *name) {
    macro **pp = &macro_table;
    while (*pp) {
        if (strcmp((*pp)->name, name) == 0) {
            macro *del = *pp;
            *pp = del->next;
            free(del->name);
            free(del->replacement);
            free(del);
            return;
        }
        pp = &(*pp)->next;
    }
}
static const char *macro_lookup(const char *name) {
    for (macro *m = macro_table; m; m = m->next) {
        if (strcmp(m->name, name) == 0) return m->replacement;
    }
    return NULL;
}

// --- preprocessor expression parser (moved from inside eval_if_expr) ---

// helper to skip whitespace
static void pp_skip_spaces(char **s_ptr) {
    while (isspace((unsigned char)**s_ptr)) (*s_ptr)++;
}

// forward declarations for the expression parser helpers
static long pp_parse_factor(char **s_ptr);
static long pp_parse_term(char **s_ptr);
static long pp_parse_expr(char **s_ptr);

// parser for constant expressions in #if directives
static long pp_parse_factor(char **s_ptr) {
    pp_skip_spaces(s_ptr);
    if (**s_ptr == '(') {
        (*s_ptr)++;
        long v = pp_parse_expr(s_ptr);
        pp_skip_spaces(s_ptr);
        if (**s_ptr == ')') (*s_ptr)++;
        return v;
    }
    // parse number (decimal)
    char *end;
    long v = strtol(*s_ptr, &end, 0);
    *s_ptr = end;
    return v;
}

static long pp_parse_term(char **s_ptr) {
    long v = pp_parse_factor(s_ptr);
    for (;;) {
        pp_skip_spaces(s_ptr);
        if (**s_ptr == '*') { (*s_ptr)++; v *= pp_parse_factor(s_ptr); }
        else if (**s_ptr == '/') { (*s_ptr)++; v /= pp_parse_factor(s_ptr); }
        else if (**s_ptr == '%') { (*s_ptr)++; v %= pp_parse_factor(s_ptr); }
        else break;
    }
    return v;
}

static long pp_parse_expr(char **s_ptr) {
    long v = pp_parse_term(s_ptr);
    for (;;) {
        pp_skip_spaces(s_ptr);
        if (**s_ptr == '+') { (*s_ptr)++; v += pp_parse_term(s_ptr); }
        else if (**s_ptr == '-') { (*s_ptr)++; v -= pp_parse_term(s_ptr); }
        else break;
    }
    return v;
}

// evaluate a simple integer expression for #if
static int eval_if_expr(const char *expr) {
    char *buf = strdup(expr);
    char *p = buf;
    while ((p = strstr(p, "defined")) != NULL) {
        char *q = p + strlen("defined");
        while (isspace((unsigned char)*q)) q++;
        if (*q == '(') {
            q++;
            char name[256] = {0};
            char *r = q;
            int i = 0;
            while (*r && (isalnum((unsigned char)*r) || *r == '_')) {
                if (i < (int)sizeof(name)-1) name[i++] = *r;
                r++;
            }
            name[i] = '\0';
            const char *rep = macro_lookup(name);
            char repl[4];
            snprintf(repl, sizeof(repl), "%d", rep ? 1 : 0);
            size_t prefix = p - buf;
            size_t suffix_len = strlen(r);
            char *newbuf = malloc(prefix + strlen(repl) + suffix_len + 1);
            memcpy(newbuf, buf, prefix);
            strcpy(newbuf + prefix, repl);
            strcpy(newbuf + prefix + strlen(repl), r);
            free(buf);
            buf = newbuf;
            p = buf + prefix + strlen(repl);
        } else {
            char name[256] = {0};
            char *r = q;
            while (*r && isspace((unsigned char)*r)) r++;
            int i = 0;
            while (*r && (isalnum((unsigned char)*r) || *r == '_')) {
                if (i < (int)sizeof(name)-1) name[i++] = *r;
                r++;
            }
            name[i] = '\0';
            const char *rep = macro_lookup(name);
            char repl[4];
            snprintf(repl, sizeof(repl), "%d", rep ? 1 : 0);
            size_t prefix = p - buf;
            size_t suffix_len = strlen(r);
            char *newbuf = malloc(prefix + strlen(repl) + suffix_len + 1);
            memcpy(newbuf, buf, prefix);
            strcpy(newbuf + prefix, repl);
            strcpy(newbuf + prefix + strlen(repl), r);
            free(buf);
            buf = newbuf;
            p = buf + prefix + strlen(repl);
        }
    }

    char *out = malloc(strlen(buf) + 1);
    char *dst = out;
    p = buf;
    while (*p) {
        if (isalpha((unsigned char)*p) || *p == '_') {
            char name[256] = {0};
            int i = 0;
            char *q = p;
            while (*q && (isalnum((unsigned char)*q) || *q == '_')) {
                if (i < (int)sizeof(name)-1) name[i++] = *q;
                q++;
            }
            name[i] = '\0';
            const char *rep = macro_lookup(name);
            if (rep) {
                strcpy(dst, rep);
                dst += strlen(rep);
            } else {
                *dst++ = '0';
            }
            p = q;
        } else {
            *dst++ = *p++;
        }
    }
    *dst = '\0';

    char *s = out;
    long result = pp_parse_expr(&s);
    free(buf);
    free(out);
    return (int)result;
}

// forward declaration for recursion
static char *preprocess_includes_and_macros(const char *input, int depth);

// preprocess includes and directives (unchanged)
static char *preprocess_includes_and_macros(const char *input, int depth) {
    if (depth > 256) {
        error_at((char *)input, "include recursion too deep");
    }

    size_t cap = 0;
    char *out = NULL;
    size_t outlen = 0;

    const char *p = input;
    int cond_stack[256];
    int cond_top = 0;
    cond_stack[cond_top++] = 1;

    while (*p) {
        const char *line_start = p;
        const char *line_end = strchr(p, '\n');
        if (!line_end) line_end = p + strlen(p);

        const char *s = p;
        while (*s == ' ' || *s == '\t') s++;

        if (*s == '#') {
            const char *d = s + 1;
            while (*d == ' ' || *d == '\t') d++;
            const char *tok = d;
            while (isalpha((unsigned char)*d)) d++;
            size_t tlen = d - tok;
            char dir[32] = {0};
            if (tlen < sizeof(dir)) memcpy(dir, tok, tlen);
            else memcpy(dir, tok, sizeof(dir)-1);

            const char *rest = d;
            while (*rest == ' ' || *rest == '\t') rest++;
            size_t rest_len = line_end - rest;
            char *restbuf = malloc(rest_len + 1);
            memcpy(restbuf, rest, rest_len);
            restbuf[rest_len] = '\0';

            if (strcmp(dir, "include") == 0) {
                if (cond_stack[cond_top-1]) {
                    const char *q = rest;
                    while (*q == ' ' || *q == '\t') q++;
                    if (*q == '"') {
                        q++;
                        const char *qend = q;
                        while (*qend && *qend != '"' && qend < line_end) qend++;
                        if (*qend == '"') {
                            size_t fnlen = qend - q;
                            char *fname = malloc(fnlen + 1);
                            memcpy(q, fname, fnlen);
                            fname[fnlen] = '\0';

                            char *incbuf = read_file_to_buffer(fname);
                            if (!incbuf) {
                                free(fname);
                                free(restbuf);
                                error_at((char *)line_start, "cannot open include file: %s", fname);
                            }
                            char *incpp = preprocess_includes_and_macros(incbuf, depth + 1);
                            free(incbuf);

                            size_t need = outlen + strlen(incpp) + 1;
                            ensure_capacity(&out, &cap, need);
                            memcpy(out + outlen, incpp, strlen(incpp));
                            outlen += strlen(incpp);
                            out[outlen] = '\0';

                            free(incpp);
                            free(fname);
                        }
                    }
                }
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "define") == 0) {
                if (cond_stack[cond_top-1]) {
                    const char *q = rest;
                    while (*q == ' ' || *q == '\t') q++;
                    char name[256] = {0};
                    int i = 0;
                    while (*q && (isalnum((unsigned char)*q) || *q == '_')) {
                        if (i < (int)sizeof(name)-1) name[i++] = *q;
                        q++;
                    }
                    name[i] = '\0';
                    while (*q == ' ' || *q == '\t') q++;
                    char *rep = strdup(q);
                    size_t rlen = strlen(rep);
                    while (rlen > 0 && (rep[rlen-1] == '\n' || rep[rlen-1] == '\r')) rep[--rlen] = '\0';
                    macro_define(name, rep);
                    free(rep);
                }
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "undef") == 0) {
                if (cond_stack[cond_top-1]) {
                    const char *q = rest;
                    while (*q == ' ' || *q == '\t') q++;
                    char name[256] = {0};
                    int i = 0;
                    while (*q && (isalnum((unsigned char)*q) || *q == '_')) {
                        if (i < (int)sizeof(name)-1) name[i++] = *q;
                        q++;
                    }
                    name[i] = '\0';
                    macro_undef(name);
                }
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "ifdef") == 0) {
                const char *q = rest;
                while (*q == ' ' || *q == '\t') q++;
                char name[256] = {0};
                int i = 0;
                while (*q && (isalnum((unsigned char)*q) || *q == '_')) {
                    if (i < (int)sizeof(name)-1) name[i++] = *q;
                    q++;
                }
                name[i] = '\0';
                int defined = macro_lookup(name) ? 1 : 0;
                int include = cond_stack[cond_top-1] && defined;
                cond_stack[cond_top++] = include;
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "ifndef") == 0) {
                const char *q = rest;
                while (*q == ' ' || *q == '\t') q++;
                char name[256] = {0};
                int i = 0;
                while (*q && (isalnum((unsigned char)*q) || *q == '_')) {
                    if (i < (int)sizeof(name)-1) name[i++] = *q;
                    q++;
                }
                name[i] = '\0';
                int defined = macro_lookup(name) ? 1 : 0;
                int include = cond_stack[cond_top-1] && !defined;
                cond_stack[cond_top++] = include;
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "if") == 0) {
                int include = 0;
                if (cond_stack[cond_top-1]) {
                    include = eval_if_expr(rest);
                } else {
                    include = 0;
                }
                cond_stack[cond_top++] = include;
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "elif") == 0) {
                if (cond_top <= 1) { free(restbuf); error_at((char *)line_start, "unexpected #elif"); }
                int prev = cond_stack[cond_top-1];
                if (cond_stack[cond_top-2] == 0) {
                    cond_stack[cond_top-1] = 0;
                } else if (prev) {
                    cond_stack[cond_top-1] = 0;
                } else {
                    cond_stack[cond_top-1] = eval_if_expr(rest);
                }
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "else") == 0) {
                if (cond_top <= 1) { free(restbuf); error_at((char *)line_start, "unexpected #else"); }
                if (cond_stack[cond_top-2] == 0) {
                    cond_stack[cond_top-1] = 0;
                } else {
                    cond_stack[cond_top-1] = !cond_stack[cond_top-1];
                }
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else if (strcmp(dir, "endif") == 0) {
                if (cond_top <= 1) { free(restbuf); error_at((char *)line_start, "unexpected #endif"); }
                cond_top--;
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            } else {
                free(restbuf);
                p = (*line_end == '\n') ? line_end + 1 : line_end;
                continue;
            }
        }

        if (cond_stack[cond_top-1]) {
            const char *q = p;
            while (q < line_end) {
                if (isalpha((unsigned char)*q) || *q == '_') {
                    const char *id_start = q;
                    char name[256] = {0};
                    int i = 0;
                    while (q < line_end && (isalnum((unsigned char)*q) || *q == '_')) {
                        if (i < (int)sizeof(name)-1) name[i++] = *q;
                        q++;
                    }
                    name[i] = '\0';
                    const char *rep = macro_lookup(name);
                    if (rep) {
                        size_t rep_len = strlen(rep);
                        size_t need = outlen + rep_len + 1;
                        ensure_capacity(&out, &cap, need);
                        memcpy(out + outlen, rep, rep_len);
                        outlen += rep_len;
                    } else {
                        size_t id_len = q - id_start;
                        size_t need = outlen + id_len + 1;
                        ensure_capacity(&out, &cap, need);
                        memcpy(out + outlen, id_start, id_len);
                        outlen += id_len;
                    }
                } else {
                    size_t need = outlen + 2;
                    ensure_capacity(&out, &cap, need);
                    out[outlen++] = *q++;
                }
            }
            if (*line_end == '\n') {
                ensure_capacity(&out, &cap, outlen + 2);
                out[outlen++] = '\n';
            }
        }
        p = (*line_end == '\n') ? line_end + 1 : line_end;
    }

    ensure_capacity(&out, &cap, outlen + 1);
    out[outlen] = '\0';
    return out;
}

// --- iv. lexer implementation ---

token *new_token(token_kind kind, token *cur, char *str, int len) {
    token *tok = calloc(1, sizeof(token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    tok->file_pos = str;
    cur->next = tok;
    return tok;
}

int is_ident_char(char c) { return isalnum((unsigned char)c) || c == '_'; }

int consume(char *op) {
    if (current_token->kind != tk_reserved || current_token->len != (int)strlen(op) ||
        strncmp(current_token->str, op, current_token->len) != 0) return 0;
    current_token = current_token->next;
    return 1;
}

int consume_keyword(char *kw) {
    if (current_token->kind != tk_keyword || current_token->len != (int)strlen(kw) ||
        strncmp(current_token->str, kw, current_token->len) != 0) return 0;
    current_token = current_token->next;
    return 1;
}

token *expect_ident() {
    if (current_token->kind != tk_ident) error_tok(current_token, "expected identifier");
    token *t = current_token;
    current_token = current_token->next;
    return t;
}

void expect(char *op) {
    if (!consume(op)) error_tok(current_token, "expected '%s'", op);
}

long expect_number() {
    if (current_token->kind != tk_num) error_tok(current_token, "expected number");
    long val = current_token->val;
    current_token = current_token->next;
    return val;
}

struct { char *name; int len; } keywords[] = {
    {"return", 6}, {"if", 2}, {"else", 4}, {"while", 5}, {"for", 3},
    {"break", 5}, {"continue", 8}, {"switch", 6}, {"case", 4}, {"default", 7},
    {"int", 3}, {"char", 4}, {"long", 4}, {"unsigned", 8}, {"static", 6},
    {"inline", 6}, {NULL, 0}
};

token *tokenize(char *p) {
    token *head = calloc(1, sizeof(token));
    token *cur = head;

    while (*p) {
        if (isspace((unsigned char)*p)) { p++; continue; }
        if (p[0] == '/' && p[1] == '/') { while (*p && *p != '\n') p++; continue; }
        if (p[0] == '/' && p[1] == '*') {
            p += 2;
            while (*p) {
                if (p[0] == '*' && p[1] == '/') { p += 2; break; }
                p++;
            }
            if (!*p) break;
            continue;
        }

        if (strncmp(p, "...", 3) == 0) { cur = new_token(tk_reserved, cur, p, 3); p += 3; continue; }

        // Combined 2 and 3 character operators (including all compound assignments)
        if (strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0 ||
            strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0 ||
            strncmp(p, "&&", 2) == 0 || strncmp(p, "||", 2) == 0 ||
            strncmp(p, "<<=", 3) == 0 || strncmp(p, ">>=", 3) == 0 ||
            strncmp(p, "<<", 2) == 0 || strncmp(p, ">>", 2) == 0 ||
            strncmp(p, "+=", 2) == 0 || strncmp(p, "-=", 2) == 0 ||
            strncmp(p, "*=", 2) == 0 || strncmp(p, "/=", 2) == 0 ||
            strncmp(p, "%=", 2) == 0 ||
            strncmp(p, "&=", 2) == 0 || strncmp(p, "|=", 2) == 0 ||
            strncmp(p, "^=", 2) == 0) {
            int len = (p[2] == '=') ? 3 : 2;
            cur = new_token(tk_reserved, cur, p, len); p += len; continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '=' ||
            *p == '(' || *p == ')' || *p == ';' || *p == '<' || *p == '>' ||
            *p == '{' || *p == '}' || *p == '&' || *p == ':' || *p == ',' ||
            *p == '|' || *p == '^' || *p == '~' || *p == '!' || *p == '?' ||
            *p == '%' || *p == '[' || *p == ']') { // <-- ADDED [ and ]
            cur = new_token(tk_reserved, cur, p, 1); p++; continue;
        }

        int is_kw = 0;
        for (int i = 0; keywords[i].name; i++) {
            if (strncmp(p, keywords[i].name, keywords[i].len) == 0 &&
                !is_ident_char(p[keywords[i].len])) {
                cur = new_token(tk_keyword, cur, p, keywords[i].len);
                p += keywords[i].len;
                is_kw = 1;
                break;
            }
        }
        if (is_kw) continue;

        if (isalpha((unsigned char)*p) || *p == '_') {
            char *q = (char *)p;
            while (is_ident_char(*q)) q++;
            cur = new_token(tk_ident, cur, p, q - p);
            p = q;
            continue;
        }

        if (isdigit((unsigned char)*p)) {
            char *q = (char *)p;
            cur = new_token(tk_num, cur, p, 0);
            cur->val = strtol(p, &p, 0);
            cur->len = p - q;
            cur->type = ty_int_def();
            continue;
        }

        // --- fix: character literal parsing ---
        if (*p == '\'') {
            char *start = p;
            p++; // move past the first '
            long val = 0;

            if (*p == '\\') { // handle escaped character
                p++;
                if (*p == 'n') { val = 10; p++; }
                else if (*p == 't') { val = 9; p++; }
                else if (*p == 'r') { val = 13; p++; }
                else if (*p == '0') { val = 0; p++; }
                else if (*p == '\'') { val = '\''; p++; }
                else if (*p == '"') { val = '"'; p++; }
                else if (*p == '\\') { val = '\\'; p++; }
                else {
                    error_at(p, "unknown escape sequence");
                }
            } else if (*p != '\'') {
                val = *p;
                p++;
            } else {
                error_at(start, "empty character literal");
            }

            if (*p != '\'') {
                error_at(p, "expected closing quote for character literal");
            }
            p++; // move past the closing '

            cur = new_token(tk_num, cur, start, p - start);
            cur->val = val;
            cur->type = ty_char_def();
            continue;
        }

        // --- fix: string literal skipping (minimal support) ---
        if (*p == '"') {
            p++;
            while (*p != '"' && *p != '\0') {
                if (*p == '\\') p++; // skip escaped char
                p++;
            }
            if (*p == '"') p++;
            // this minimal compiler doesn't store string literals; we just skip them.
            continue;
        }

        error_at((char *)p, "invalid token");
    }

    new_token(tk_eof, cur, p, 0);
    return head->next;
}

// --- v. parser implementation (core functions added) ---

// function prototypes for recursive descent
node *primary();
node *unary();
node *mul();
node *add();
node *shift();
node *relational();
node *equality();
node *bitwise_and();
node *bitwise_xor();
node *bitwise_or();
node *logical_and();
node *logical_or();
node *conditional(); // new parser function
node *assign();
node *expr();
node *stmt();
node *block();

// node constructors
node *new_node(node_kind kind, node *lhs, node *rhs) {
    node *n = calloc(1, sizeof(node));
    n->kind = kind;
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

node *new_num_node(long val) {
    node *n = calloc(1, sizeof(node));
    n->kind = nd_num;
    n->val = val;
    n->type = ty_int_def();
    return n;
}

// parser core functions (missing/fixed)

node *primary() {
    // parenthesized expression
    if (consume("(")) {
        node *n = expr();
        expect(")");
        return n;
    }

    // function call or variable
    token *tok = current_token;
    if (tok->kind == tk_ident) {
        current_token = current_token->next;
        // function call
        if (consume("(")) {
            node *n = calloc(1, sizeof(node));
            n->kind = nd_funcall;
            n->name = strndup(tok->str, tok->len);
            node head = {};
            node *cur = &head;
            while (!consume(")")) {
                if (cur != &head) consume(",");
                cur->next = assign();
                cur = cur->next;
            }
            n->lhs = head.next; // arguments list
            // simplified type: assume int return
            n->type = ty_int_def();
            return n;
        }

        // local variable
        lvar *l = find_lvar(tok, NULL, 0); // look up existing local/param
        node *n = calloc(1, sizeof(node));
        n->kind = nd_lvar;
        n->var = l;
        // n->type is set in add_type to handle array decay
        return n;
    }

    // numeric literal (includes char literals tokenized as tk_num)
    if (current_token->kind == tk_num) {
        return new_num_node(expect_number());
    }

    error_tok(current_token, "expected expression");
    return NULL;
}

node *unary() {
    if (consume("+")) return unary();
    if (consume("-")) return new_node(nd_sub, new_num_node(0), unary());
    if (consume("!")) return new_node(nd_not, unary(), NULL);
    if (consume("~")) return new_node(nd_bitnot, unary(), NULL);
    if (consume("*")) return new_node(nd_deref, unary(), NULL);
    if (consume("&")) return new_node(nd_addr, unary(), NULL);
    return primary();
}

node *mul() {
    node *n = unary();
    for (;;) {
        if (consume("*")) n = new_node(nd_mul, n, unary());
        else if (consume("/")) n = new_node(nd_div, n, unary());
        else if (consume("%")) n = new_node(nd_mod, n, unary());
        else return n;
    }
}

// add function implementation (fixed for pointer arithmetic logic)
node *add() {
    node *n = mul();
    for (;;) {
        if (consume("+")) {
            node *rhs = mul();
            add_type(n); // ensure types are calculated
            // simple pointer arithmetic scale fix
            if (n->type && (n->type->kind == ty_ptr || n->type->kind == ty_array)) {
                ty *base_type = (n->type->kind == ty_array) ? n->type->base : n->type->base;
                rhs = new_node(nd_mul, rhs, new_num_node(base_type->size));
            } else if (rhs->type && (rhs->type->kind == ty_ptr || rhs->type->kind == ty_array)) {
                ty *base_type = (rhs->type->kind == ty_array) ? rhs->type->base : rhs->type->base;
                n = new_node(nd_mul, n, new_num_node(base_type->size));
            }
            n = new_node(nd_add, n, rhs);
        }
        else if (consume("-")) {
            node *rhs = mul();
            add_type(n);
            // simple pointer arithmetic scale fix
            if (n->type && (n->type->kind == ty_ptr || n->type->kind == ty_array) &&
                rhs->type && rhs->type->kind != ty_ptr && rhs->type->kind != ty_array) {
                ty *base_type = (n->type->kind == ty_array) ? n->type->base : n->type->base;
                rhs = new_node(nd_mul, rhs, new_num_node(base_type->size));
            }
            n = new_node(nd_sub, n, rhs);
        }
        else break;
    }
    return n;
}

node *shift() {
    node *n = add();
    for (;;) {
        if (consume("<<")) n = new_node(nd_shl, n, add());
        else if (consume(">>")) n = new_node(nd_shr, n, add());
        else return n;
    }
}

node *relational() {
    node *n = shift();
    for (;;) {
        if (consume("<")) n = new_node(nd_lt, n, shift());
        else if (consume("<=")) n = new_node(nd_le, n, shift());
        else if (consume(">")) n = new_node(nd_lt, shift(), n);
        else if (consume(">=")) n = new_node(nd_le, shift(), n);
        else return n;
    }
}

node *equality() {
    node *n = relational();
    for (;;) {
        if (consume("==")) n = new_node(nd_eq, n, relational());
        else if (consume("!=")) n = new_node(nd_ne, n, relational());
        else return n;
    }
}

node *bitwise_and() {
    node *n = equality();
    for (;;) {
        if (consume("&")) n = new_node(nd_band, n, equality());
        else return n;
    }
}

node *bitwise_xor() {
    node *n = bitwise_and();
    for (;;) {
        if (consume("^")) n = new_node(nd_bxor, n, bitwise_and());
        else return n;
    }
}

node *bitwise_or() {
    node *n = bitwise_xor();
    for (;;) {
        if (consume("|")) n = new_node(nd_bor, n, bitwise_xor());
        else return n;
    }
}

node *logical_and() {
    node *n = bitwise_or();
    for (;;) {
        if (consume("&&")) n = new_node(nd_logical_and, n, bitwise_or());
        else return n;
    }
}

node *logical_or() {
    node *n = logical_and();
    for (;;) {
        if (consume("||")) n = new_node(nd_logical_or, n, logical_and());
        else return n;
    }
}

// new conditional expression parser (? :)
node *conditional() {
    node *n = logical_or();

    if (consume("?")) {
        node *then_expr = expr();
        expect(":");
        node *else_expr = conditional(); // right-associative

        node *cond_node = calloc(1, sizeof(node));
        cond_node->kind = nd_cond;
        cond_node->lhs = n;           // condition
        cond_node->rhs = then_expr;   // true expression
        cond_node->els = else_expr;   // false expression
        n = cond_node;
    }
    return n;
}

node *assign() {
    node *n = conditional();

    // Check for all assignment operators
    if (consume("=")) {
        n = new_node(nd_assign, n, assign());
    } else if (consume("+=")) {
        n = new_node(nd_add_assign, n, assign());
    } else if (consume("-=")) {
        n = new_node(nd_sub_assign, n, assign());
    } else if (consume("*=")) {
        n = new_node(nd_mul_assign, n, assign());
    } else if (consume("/=")) {
        n = new_node(nd_div_assign, n, assign());
    } else if (consume("%=")) {
        n = new_node(nd_mod_assign, n, assign());
    } else if (consume("<<=")) {
        n = new_node(nd_shl_assign, n, assign());
    } else if (consume(">>=")) {
        n = new_node(nd_shr_assign, n, assign());
    } else if (consume("&=")) {
        n = new_node(nd_band_assign, n, assign());
    } else if (consume("|=")) {
        n = new_node(nd_bor_assign, n, assign());
    } else if (consume("^=")) {
        n = new_node(nd_bxor_assign, n, assign());
    }
    return n;
}

node *expr() {
    return assign();
}

node *block() {
    expect("{");
    node head = {};
    node *cur = &head;
    while (!consume("}")) {
        cur->next = stmt();
        cur = cur->next;
    }
    node *n = new_node(nd_block, NULL, NULL);
    n->next = head.next;
    return n;
}

node *stmt() {
    if (consume_keyword("return")) {
        node *n = new_node(nd_return, expr(), NULL);
        expect(";");
        return n;
    }
    if (consume_keyword("if")) {
        expect("(");
        node *cond = expr();
        expect(")");
        node *then_stmt = stmt();
        node *n = new_node(nd_if, cond, then_stmt);
        if (consume_keyword("else")) {
            n->els = stmt();
        }
        return n;
    }
    if (consume_keyword("while")) {
        expect("(");
        node *cond = expr();
        expect(")");
        node *body = stmt();
        return new_node(nd_while, cond, body);
    }
    if (consume_keyword("for")) {
        expect("(");
        // init
        node *init = NULL;
        if (!consume(";")) { init = expr(); expect(";"); }
        // cond
        node *cond = NULL;
        if (!consume(";")) { cond = expr(); expect(";"); }
        // inc
        node *inc = NULL;
        if (!consume(")")) { inc = expr(); expect(")"); }

        node *body = stmt();
        node *n = new_node(nd_for, init, cond);
        n->els = inc;
        n->next = body;
        return n;
    }
    if (consume_keyword("break")) {
        expect(";");
        return new_node(nd_break, NULL, NULL);
    }
    if (consume_keyword("continue")) {
        expect(";");
        return new_node(nd_continue, NULL, NULL);
    }
    if (current_token->kind == tk_reserved && *current_token->str == '{') {
        return block();
    }
    if (is_type_token(current_token)) {
        return declaration();
    }

    // expression statement
    node *n = new_node(nd_expr_stmt, expr(), NULL);
    expect(";");
    return n;
}

// simplified declaration parser:
node *declaration() {
    // only simple declarations like 'int x;' or 'int *x = 5;'
    int is_unsigned = 0;
    int is_static = 0;
    ty *base = parse_type_specifier(&is_unsigned, &is_static);
    ty *type = parse_declarator(base);
    token *tok = expect_ident();
    type->is_unsigned = is_unsigned;

    lvar *l = find_lvar(tok, type, is_static);
    node *n = NULL;

    if (consume("=")) {
        if (l->type->kind == ty_array) { // <-- Handle array initialization
            // Check for simple zero initialization: {0}
            if (consume("{")) {
                if (consume("0")) {
                    expect("}");
                    // Create a placeholder node for zero-initialization
                    node *init_node = calloc(1, sizeof(node));
                    init_node->kind = nd_array_init_zero;
                    init_node->var = l; // Link to the variable
                    n = init_node;
                } else {
                    error_tok(current_token, "only '{0}' array initialization is supported for now");
                }
            } else {
                error_tok(current_token, "only '{0}' array initialization is supported for arrays");
            }
        } else {
            // Scalar initialization: int x = 5;
            node *init = assign();
            // create assignment node: nd_assign (nd_lvar, init)
            node *lvar_node = calloc(1, sizeof(node));
            lvar_node->kind = nd_lvar;
            lvar_node->var = l;
            lvar_node->type = type;
            n = new_node(nd_assign, lvar_node, init);
        }
    }
    expect(";");
    // return an expression statement node containing the declaration's optional initialization
    return new_node(nd_expr_stmt, n, NULL);
}

ty *parse_type_specifier(int *is_unsigned_ptr, int *is_static_ptr) {
    ty *base = NULL;
    int saw_type = 0;
    while (1) {
        if (consume_keyword("unsigned")) {
            if (*is_unsigned_ptr) error_tok(current_token, "duplicate 'unsigned'");
            *is_unsigned_ptr = 1;
            continue;
        }
        if (consume_keyword("static")) {
            if (*is_static_ptr) error_tok(current_token, "duplicate 'static'");
            *is_static_ptr = 1;
            continue;
        }
        if (consume_keyword("char")) {
            if (saw_type) error_tok(current_token, "multiple type specifiers");
            base = ty_char_def(); saw_type = 1; continue;
        }
        if (consume_keyword("int")) {
            if (saw_type) error_tok(current_token, "multiple type specifiers");
            base = ty_int_def(); saw_type = 1; continue;
        }
        if (consume_keyword("long")) {
            if (saw_type) error_tok(current_token, "multiple type specifiers");
            base = ty_long_def(); saw_type = 1; continue;
        }
        break;
    }
    if (!base) base = ty_int_def(); // default to int
    if (*is_unsigned_ptr) base->is_unsigned = 1;
    return base;
}

ty *parse_declarator(ty *base) {
    // 1. handle pointers
    while (consume("*")) {
        base = new_type_ptr(base);
    }

    // 2. handle array dimensions (simplified: array declarator binds stronger than pointer)
    while (current_token->kind == tk_reserved && strncmp(current_token->str, "[", 1) == 0) {
        expect("[");
        if (current_token->kind != tk_num) {
            error_tok(current_token, "expected array size number");
        }
        long len = expect_number();
        expect("]");
        base = new_type_array(base, (size_t)len);
    }

    return base;
}

node *param_declaration(int param_idx) {
    int is_unsigned = 0;
    int is_static = 0;
    ty *base = parse_type_specifier(&is_unsigned, &is_static);
    ty *type = parse_declarator(base);
    token *tok = expect_ident();
    type->is_unsigned = is_unsigned;

    // parameters are treated as locals, not static
    lvar *l = calloc(1, sizeof(lvar));
    l->next = current_func->locals;
    l->name = tok->str;
    l->len = tok->len;
    l->type = type;
    l->offset = param_idx * 8; // simplified param offset, adjusted by codegen

    current_func->locals = l;
    current_func->num_params++;
    return NULL; // return is handled by funcdef
}

void parse_fundef() {
    int is_unsigned = 0;
    int is_static = 0;
    // skip 'inline' if present
    consume_keyword("inline");
    ty *base = parse_type_specifier(&is_unsigned, &is_static);
    ty *type = parse_declarator(base);
    token *tok = expect_ident();

    // function definition
    function *f = calloc(1, sizeof(function));
    f->name = strndup(tok->str, tok->len);
    f->is_static = is_static;
    f->next = program_head;
    program_head = f;

    current_func = f;
    current_func->stack_size = 0; // reset for new function

    expect("(");
    // parameter list
    while (!consume(")")) {
        if (f->num_params > 0) consume(",");
        if (consume("...")) {
            f->is_variadic = 1;
            expect(")");
            break;
        }
        param_declaration(f->num_params);
    }

    // function body
    f->body = block();

    // after parsing, set stack_size for alignment (align to 16)
    f->stack_size = ((f->stack_size + 15) / 16) * 16;
    current_func = NULL; // reset global state
}

// --- vi. code generation implementation (missing functions added) ---

// control flow stack (implemented missing helpers)
void enter_control_block(int continue_label, int break_label) {
    continue_break_labels *l = calloc(1, sizeof(continue_break_labels));
    l->continue_label = continue_label;
    l->break_label = break_label;
    l->next = loop_or_switch_labels;
    loop_or_switch_labels = l;
}
void exit_control_block() {
    if (loop_or_switch_labels) {
        continue_break_labels *del = loop_or_switch_labels;
        loop_or_switch_labels = loop_or_switch_labels->next;
        free(del);
    }
}

// lvalue code generation (implemented missing helpers)

// generates code to push the address of an lvalue onto the stack
void gen_lval(node *n) {
    if (n->kind == nd_lvar) {
        if (n->var->is_static) {
            // static local variable (treated as a global symbol)
            printf("  lea rax, %s[rip]\n", n->var->gname);
            printf("  push rax\n");
        } else {
            // local variable: [rbp - offset]
            printf("  mov rax, rbp\n");
            printf("  sub rax, %d\n", n->var->offset);
            printf("  push rax\n");
        }
        return;
    }
    if (n->kind == nd_deref) {
        gen_expr(n->lhs);
        return;
    }
    error_tok(current_token, "cannot take address of non-lvalue");
}

// generates code to load the value at the address on top of the stack (pop, load, push)
void gen_load(ty *type) {
    // Array decay: arrays automatically decay to a pointer to their first element (which is the address already on the stack)
    if (type->kind == ty_array) {
        return; // array address is already on stack (array decay to pointer)
    }

    if (type->kind == ty_int || type->kind == ty_long || type->kind == ty_ptr) {
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
    } else if (type->kind == ty_char) {
        printf("  pop rax\n");
        printf("  movsx rax, byte ptr [rax]\n");
        printf("  push rax\n");
    } else {
        error_tok(current_token, "gen_load: cannot load type size %d", type->size);
    }
}

// generates code to store the value on the second stack slot to the address on the first stack slot
// (addr: on stack; val: on stack) -> pop val into rdi, pop addr into rax, mov [rax], rdi, push rdi
void gen_store(ty *type) {
    printf("  pop rdi\n"); // value to store (rhs)
    printf("  pop rax\n"); // address to store to (lhs)
    if (type->kind == ty_int || type->kind == ty_long || type->kind == ty_ptr) {
        printf("  mov [rax], rdi\n");
    } else if (type->kind == ty_char) {
        printf("  mov [rax], dil\n");
    } else {
        error_tok(current_token, "gen_store: cannot store type size %d", type->size);
    }
    printf("  push rdi\n"); // push stored value back (for assignment result)
}

// emit label helper
void emit_label(int id) {
    printf(".l%d:\n", id);
}

// generate code for expressions
const char *arg_regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_expr(node *n) {
    if (!n) return;
    add_type(n);

    switch (n->kind) {
        case nd_num:
            printf("  push %ld\n", n->val);
            return;
        case nd_lvar:
            gen_lval(n);
            gen_load(n->type);
            return;
        case nd_assign:
            gen_lval(n->lhs);
            gen_expr(n->rhs);
            gen_store(n->lhs->type);
            return;
        case nd_addr:
            gen_lval(n->lhs);
            return;
        case nd_deref:
            gen_expr(n->lhs);
            gen_load(n->type);
            return;

        // --- COMPOUND ASSIGNMENTS ---
        case nd_add_assign: case nd_sub_assign: case nd_mul_assign: case nd_div_assign: case nd_mod_assign:
        case nd_shl_assign: case nd_shr_assign: case nd_band_assign: case nd_bor_assign: case nd_bxor_assign: {
            // 1. Get address of LHS (pushes addr to stack)
            gen_lval(n->lhs);
            // 2. Evaluate RHS (pushes value to stack)
            gen_expr(n->rhs);

            // stack now: [addr] [val]

            // 3. Pop RHS value into RDI
            printf("  pop rdi\n");
            // 4. Pop LHS address into RAX
            printf("  pop rax\n");
            // 5. Load current LHS value into RCX (for the binary op)
            // Use n->lhs->type for load size determination
            ty *ltype = n->lhs->type;
            if (ltype->kind == ty_char) {
                printf("  movsx rcx, byte ptr [rax]\n");
            } else {
                printf("  mov rcx, [rax]\n");
            }

            // 6. Perform the operation (RCX = RCX op RDI)
            if (n->kind == nd_add_assign) {
                printf("  add rcx, rdi\n");
            } else if (n->kind == nd_sub_assign) {
                printf("  sub rcx, rdi\n");
            } else if (n->kind == nd_mul_assign) {
                printf("  imul rcx, rdi\n");
            } else if (n->kind == nd_div_assign || n->kind == nd_mod_assign) {
                // Division/Modulus requires RAX for input, RDI for divisor, RDX:RAX for output
                printf("  push rdi\n"); // Save RDI (RHS value)
                printf("  mov rax, rcx\n"); // move LHS value (RCX) to RAX
                printf("  cqo\n"); // sign-extend RAX to RDX:RAX
                printf("  pop rdi\n"); // Restore RDI (RHS value)
                printf("  idiv rdi\n");

                if (n->kind == nd_div_assign) {
                    printf("  mov rcx, rax\n"); // Quotient in RAX to RCX
                } else { // nd_mod_assign
                    printf("  mov rcx, rdx\n"); // Remainder in RDX to RCX
                }
            } else if (n->kind == nd_shl_assign) {
                printf("  push rdi\n"); // Save RDI (RHS value)
                printf("  mov cl, dil\n"); // Move count to CL
                printf("  shl rcx, cl\n");
                printf("  pop rdi\n"); // Restore RDI
            } else if (n->kind == nd_shr_assign) {
                printf("  push rdi\n"); // Save RDI
                printf("  mov cl, dil\n");
                printf("  sar rcx, cl\n");
                printf("  pop rdi\n"); // Restore RDI
            } else if (n->kind == nd_band_assign) {
                printf("  and rcx, rdi\n");
            } else if (n->kind == nd_bor_assign) {
                printf("  or rcx, rdi\n");
            } else if (n->kind == nd_bxor_assign) {
                printf("  xor rcx, rdi\n");
            }

            // 7. Store the result (RCX) back to the address (RAX)
            if (ltype->kind == ty_char) {
                printf("  mov [rax], cl\n"); // only store the low byte
            } else {
                printf("  mov [rax], rcx\n");
            }

            // 8. Push the result of the expression (RCX)
            printf("  push rcx\n");
            return;
        }

        case nd_add: case nd_sub: case nd_mul: case nd_div: case nd_mod:
        case nd_eq: case nd_ne: case nd_lt: case nd_le:
        case nd_shl: case nd_shr: case nd_band: case nd_bor: case nd_bxor:
        case nd_logical_and: case nd_logical_or:
        case nd_not: case nd_bitnot: {
            // for binary operators (excluding short-circuit logic from gen_expr):
            if (n->rhs) gen_expr(n->rhs);
            if (n->lhs) gen_expr(n->lhs);
            printf("  pop rax\n"); // lhs
            printf("  pop rdi\n"); // rhs

            // special case for pointer arithmetic and division modulus
            if (n->kind == nd_add) {
                printf("  add rax, rdi\n");
            } else if (n->kind == nd_sub) {
                printf("  sub rax, rdi\n");
            } else if (n->kind == nd_mul) {
                printf("  imul rax, rdi\n");
            } else if (n->kind == nd_div) {
                printf("  cqo\n");
                printf("  idiv rdi\n");
            } else if (n->kind == nd_mod) {
                printf("  cqo\n");
                printf("  idiv rdi\n");
                printf("  mov rax, rdx\n"); // remainder in rdx
            } else if (n->kind == nd_shl) {
                printf("  mov rcx, rdi\n");
                printf("  shl rax, cl\n");
            } else if (n->kind == nd_shr) {
                printf("  mov rcx, rdi\n");
                printf("  sar rax, cl\n");
            } else if (n->kind == nd_band) {
                printf("  and rax, rdi\n");
            } else if (n->kind == nd_bor) {
                printf("  or rax, rdi\n");
            } else if (n->kind == nd_bxor) {
                printf("  xor rax, rdi\n");
            } else if (n->kind == nd_eq || n->kind == nd_ne || n->kind == nd_lt || n->kind == nd_le) {
                printf("  cmp rax, rdi\n");
                if (n->kind == nd_eq) printf("  sete al\n");
                else if (n->kind == nd_ne) printf("  setne al\n");
                else if (n->kind == nd_lt) printf("  setl al\n");
                else if (n->kind == nd_le) printf("  setle al\n");
                printf("  movzb rax, al\n");
            } else if (n->kind == nd_not) {
                printf("  cmp rax, 0\n");
                printf("  sete al\n");
                printf("  movzb rax, al\n");
            } else if (n->kind == nd_bitnot) {
                printf("  not rax\n");
            }
            printf("  push rax\n");
            return;
        }

        case nd_cond: {
            int lelse = label_count++;
            int lend = label_count++;

            // 1. evaluate condition (lhs)
            gen_expr(n->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            // jump to else if condition is false
            printf("  je .l%d\n", lelse);

            // 2. true expression (rhs)
            gen_expr(n->rhs);
            // jump to end
            printf("  jmp .l%d\n", lend);

            // 3. else expression (els)
            emit_label(lelse);
            gen_expr(n->els);

            // 4. end label
            emit_label(lend);
            return;
        }

        case nd_array_init_zero:
            // This node is handled directly in gen_stmt (as it is not an expression with a return value)
            return;

        case nd_funcall: {
            int nargs = 0;
            node *a = n->lhs;
            node *args[16];
            int idx = 0;
            while (a && idx < 16) { args[idx++] = a; a = a->next; }
            nargs = idx;

            // 1. evaluate args and push onto stack
            for (int i = 0; i < nargs; i++) gen_expr(args[i]);

            // 2. move args from stack to registers/stack slots
            // stack must be 16-byte aligned before call (implicitly handled by f->stack_size in this compiler)
            for (int i = 0; i < nargs; i++) {
                if (i < 6) {
                    printf("  pop %s\n", arg_regs[i]);
                }
                // for >6 args, they are already on the stack.
            }

            // 3. call
            printf("  call %s\n", n->name);

            // 4. push return value
            printf("  push rax\n");
            return;
        }
        default:
            error_tok(current_token, "codegen: unimplemented node kind %d", n->kind);
    }
}

void gen_stmt(node *n) {
    if (!n) return;
    switch (n->kind) {
        case nd_block: {
            for (node *cur = n->next; cur; cur = cur->next) {
                gen_stmt(cur);
            }
            return;
        }
        case nd_expr_stmt:
            if (n->lhs && n->lhs->kind == nd_array_init_zero) {
                // Zero initialization for local array: memset-like loop
                lvar *l = n->lhs->var;
                int size = l->type->size;
                int offset = l->offset;
                int zero_start_label = label_count++;
                int zero_end_label = label_count++;

                // setup address (RAX) and size counter (RDI)
                printf("  mov rax, rbp\n");
                printf("  sub rax, %d\n", offset); // rax = array start address
                printf("  mov rdi, %d\n", size); // rdi = counter (size)

                emit_label(zero_start_label);
                printf("  cmp rdi, 0\n");
                printf("  je .l%d\n", zero_end_label);

                printf("  sub rdi, 1\n"); // rdi is index now (size-1 down to 0)
                printf("  mov byte ptr [rax+rdi], 0\n"); // write 0

                printf("  jmp .l%d\n", zero_start_label);
                emit_label(zero_end_label);

            } else {
                gen_expr(n->lhs);
                printf("  pop rax\n");
            }
            return;
        case nd_return:
            gen_expr(n->lhs);
            printf("  pop rax\n");
            printf("  jmp .l.return.%s\n", current_func->name);
            return;
        case nd_if: {
            int lelse = label_count++;
            int lend = label_count++;
            gen_expr(n->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            if (n->els) {
                printf("  je .l%d\n", lelse);
                gen_stmt(n->rhs);
                printf("  jmp .l%d\n", lend);
                emit_label(lelse);
                gen_stmt(n->els);
                emit_label(lend);
            } else {
                printf("  je .l%d\n", lend);
                gen_stmt(n->rhs);
                emit_label(lend);
            }
            return;
        }
        case nd_while: {
            int lbegin = label_count++;
            int lend = label_count++;
            enter_control_block(lbegin, lend);
            emit_label(lbegin);
            gen_expr(n->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .l%d\n", lend);
            gen_stmt(n->rhs);
            printf("  jmp .l%d\n", lbegin);
            emit_label(lend);
            exit_control_block();
            return;
        }
        case nd_for: {
            int lbegin = label_count++;
            int linc = label_count++; // for continue
            int lend = label_count++; // for break
            enter_control_block(linc, lend);

            if (n->lhs) gen_stmt(n->lhs); // init (expr_stmt)

            emit_label(lbegin);
            if (n->rhs) {
                gen_expr(n->rhs); // cond
                printf("  pop rax\n");
                printf("  cmp rax, 0\n");
                printf("  je .l%d\n", lend);
            }

            gen_stmt(n->next); // body

            emit_label(linc);
            if (n->els) { gen_expr(n->els); printf("  pop rax\n"); } // inc (expr)

            printf("  jmp .l%d\n", lbegin);
            emit_label(lend);
            exit_control_block();
            return;
        }
        case nd_break: {
            if (!loop_or_switch_labels) error_tok(current_token, "break not in loop or switch");
            printf("  jmp .l%d\n", loop_or_switch_labels->break_label);
            return;
        }
        case nd_continue: {
            if (!loop_or_switch_labels) error_tok(current_token, "continue not in loop");
            printf("  jmp .l%d\n", loop_or_switch_labels->continue_label);
            return;
        }
        case nd_switch:
        case nd_case:
        default:
            error_tok(current_token, "unimplemented statement kind %d", n->kind);
            return;
    }
}

// emit global static variables
void emit_global_statics() {
    if (!global_statics) return;
    printf("  .data\n");
    for (gvar *g = global_statics; g; g = g->next) {
        printf("  .globl %s\n", g->name);
        printf("%s:\n", g->name);
        // .zero fills memory with zeros
        printf("  .zero %d\n", g->size);
    }
}

// emit functions
void codegen_program(function *funcs) {
    // emit statics first
    emit_global_statics();

    // emit each function
    for (function *f = funcs; f; f = f->next) {
        if (!f->is_static) printf("  .globl %s\n", f->name);
        printf("  .text\n");
        printf("%s:\n", f->name);
        // prologue
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        // allocate stack for locals (aligned to 16)
        int stack = f->stack_size;
        if (stack > 0) printf("  sub rsp, %d\n", stack);

        // move parameters from registers to stack (6 integer/pointer args supported)
        lvar *l = f->locals;
        for (int i = 0; i < 6 && l; i++, l = l->next) {
            // parameter's 'offset' is treated as its true offset from rbp
            printf("  mov [rbp - %d], %s\n", l->offset, arg_regs[i]);
        }

        // generate body
        gen_stmt(f->body);

        // epilogue label
        printf(".l.return.%s:\n", f->name);
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
    }
}

// --- main driver ---

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: cc file.c\n");
        return 1;
    }

    // concatenate all input files into one buffer with preprocessing
    char *combined = NULL;
    size_t combined_len = 0;
    for (int i = 1; i < argc; i++) {
        char *buf = read_file_to_buffer(argv[i]);
        if (!buf) {
            fprintf(stderr, "cannot open file: %s\n", argv[i]);
            return 1;
        }
        // preprocess includes and macros per file
        char *pp = preprocess_includes_and_macros(buf, 0);
        free(buf);
        if (!combined) {
            combined = pp;
            combined_len = strlen(combined);
        } else {
            size_t newlen = combined_len + strlen(pp) + 2;
            combined = realloc(combined, newlen);
            memcpy(combined + combined_len, pp, strlen(pp));
            combined_len += strlen(pp);
            combined[combined_len] = '\0';
            free(pp);
        }
    }

    user_input = combined;

    // tokenize
    token *tok = tokenize(user_input);
    current_token = tok;

    // parse all function definitions until eof
    while (current_token->kind != tk_eof) {
        parse_fundef();
    }

    // codegen
    codegen_program(program_head);

    return 0;
}
