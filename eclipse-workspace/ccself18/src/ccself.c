#define _POSIX_C_SOURCE 200809L // FIX: moved to line 1 to be effective

// cc.c
// minimal educational c compiler (amalgamated single-file)
// enhanced: full bitwise/logical operators, expanded preprocessor,
// structs, arrays, and comprehensive initializers.
// usage: cc file1.c [file2.c ...]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

// --- global compiler data (self-compilation arrays) ---

// fix: introduced a typedef for the struct to fix the "expected expression" error
typedef struct keyword_entry {
    char *name;
    int len;
} keyword_entry;

// this array defines the keywords recognized by the compiler's tokenizer.
keyword_entry keywords[] = {
    {"return", 6}, {"if", 2}, {"else", 4}, {"while", 5}, {"for", 3},
    {"break", 5}, {"continue", 8}, {"switch", 6}, {"case", 4}, {"default", 7},
    {"int", 3}, {"char", 4}, {"long", 4}, {"unsigned", 8}, {"static", 6},
    {"inline", 6}, {"struct", 6}, {"typedef", 7}, {"extern", 6}, {"void", 4},
    {NULL, 0}
};

// --- i. data structures and global state ---

// forward declarations of structures
typedef struct ty ty;
typedef struct member member;
typedef struct lvar lvar;
typedef struct token token;
typedef struct node node;
typedef struct function function;
typedef struct sinit sinit;

// struct member
struct member {
    member *next;
    char *name;
    int len;
    ty *type;
    int offset;
};

// type system
typedef enum { ty_void, ty_int, ty_char, ty_long, ty_ptr, ty_array, ty_struct, ty_func } type_kind;
struct ty {
    type_kind kind;
    ty *base;        // used for pointers, arrays, functions
    int size;        // 1, 4, 8, or arbitrary for struct/array
    int align;       // alignment requirement
    int is_unsigned;
    size_t array_len; // length if ty_array
    member *members; // for ty_struct
    int is_variadic; // FIX: added to ty for function types
};

// local variables
struct lvar {
    lvar *next;
    char *name;
    int len;
    int offset;
    ty *type;
    int is_static;
    char *gname; // global label name for static locals
    int is_typedef; // new: to support typedef
};

// global struct tag definitions
typedef struct struct_tag {
    struct struct_tag *next;
    char *name;
    int len;
    ty *type;
} struct_tag;

struct_tag *struct_tags = NULL;

// global array and struct initialization data
typedef enum { init_zero, init_int, init_ptr, init_array, init_struct, init_string } init_kind;

struct sinit {
    sinit *next;
    init_kind kind;
    long val;
    char *str_content; // string literal content for init_string
    char *str_label;   // label for init_ptr/init_string (generated)
    ty *type;          // the type of the initialized object
    int offset;        // offset from start of struct/array
    int size;          // size of the object
    sinit *children;   // for array/struct
};

// global static variable descriptor (for emission)
typedef struct gvar {
    struct gvar *next;
    char *name;
    int size;
    sinit *initializer; // used for initialized global/static variables
    int is_extern;
} gvar;

gvar *global_vars = NULL;

// functions
struct function {
    function *next;
    char *name;
    node *body;
    lvar *locals;
    int stack_size;
    int num_params;
    int is_variadic;
    int is_static;
    ty *ret_type; // new: function return type
};

// tokenizer
typedef enum { tk_eof, tk_reserved, tk_keyword, tk_num, tk_ident, tk_string } token_kind;
struct token {
    token_kind kind;
    token *next;
    long val;
    char *str;
    int len;
    char *file_pos; // start of the token for error reporting
    ty *type;
};

// ast nodes
typedef enum {
    nd_num, nd_lvar, nd_assign, nd_funcall, nd_return,
    nd_if, nd_while, nd_for, nd_block, nd_expr_stmt,
    nd_add, nd_sub, nd_mul, nd_div, nd_mod,
    nd_eq, nd_ne, nd_lt, nd_le,
    nd_addr, nd_deref, nd_break, nd_continue, nd_switch, nd_case,
    nd_shl, nd_shr, nd_band, nd_bor, nd_bxor, nd_logical_and, nd_logical_or,
    nd_not, nd_bitnot,
    nd_cond, nd_member,
    // compound assignments
    nd_add_assign, nd_sub_assign, nd_mul_assign, nd_div_assign, nd_mod_assign,
    nd_shl_assign, nd_shr_assign, nd_band_assign, nd_bor_assign, nd_bxor_assign,
    // array/struct initialization for local vars
    nd_local_init,
    nd_gvar // for global variable access
} node_kind;

struct node {
    node_kind kind;
    node *lhs;
    node *rhs;
    node *els;
    node *next;
    long val;
    char *name;
    ty *type;
    lvar *var;
    gvar *gvar; // for nd_gvar
    member *mem; // for nd_member
    sinit *init_list; // for nd_local_init
};

// control flow labels for break/continue
typedef struct continue_break_labels {
    int continue_label;
    int break_label;
    struct continue_break_labels *next;
} continue_break_labels;

// preprocessor conditional compilation state
typedef struct pp_state {
    struct pp_state *next;
    int expecting_else; // 1 if a true block was seen (to skip subsequent #elif/#else)
    int skip_block;     // 1 if the current block should be skipped
    int if_line;        // line number of the `#if` for error reporting
} pp_state;

pp_state *pp_cond_state = NULL; // stack for conditional compilation

// object-like macros
typedef struct macro {
    struct macro *next;
    char *name;
} macro;

macro *macros = NULL;


// --- global compiler state ---
char *user_input; // the combined source code after preprocessing
token *current_token;
function *program_head = NULL;
function *current_func = NULL;
continue_break_labels *loop_or_switch_labels = NULL;
int label_count = 0;
int static_var_count = 0;
int string_label_count = 0;
int struct_tag_count = 0;
int line_number = 1; // current line number for error reporting

// --- function prototypes ---

// utility and i/o
char *read_file_to_buffer(const char *path);
void error_at(char *loc, char *fmt, ...);
void error_tok(token *tok, char *fmt, ...);
int align_to(int n, int align);
int is_ident_char(char c);

// preprocessor (enhanced)
char *preprocess_file(const char *path, int depth);
macro *find_macro(const char *name, int len);
void define_macro(const char *name, int len);
void undef_macro(const char *name, int len);
char *skip_junk(char *p);
char *find_include_path(char *filename, char term_char); // new: search for include path
int file_exists(const char *path); // new: utility to check file existence

// type and variable management
ty *new_type(type_kind kind, int size, int align);
ty *ty_int_def();
ty *ty_char_def();
ty *ty_long_def();
ty *ty_void_def();
ty *new_type_ptr(ty *base);
ty *new_type_array(ty *base, size_t len);
ty *new_type_struct(int size, int align);
ty *find_struct_tag(token *tok);
void register_struct_tag(token *tok, ty *type);
member *get_struct_member(ty *type, token *tok);
void add_type(node *n);
lvar *find_lvar(token *tok);
lvar *add_lvar(token *tok, ty *type, int is_static, int is_typedef);
gvar *find_global_var(token *tok);
gvar *add_global_var(token *tok, ty *type, int is_extern);
sinit *new_sinit_int(long val);
sinit *new_sinit_ptr(char *str_content);

// lexer
token *tokenize(char *p);
int consume(char *op);
int consume_keyword(char *kw);
token *expect_ident();
void expect(char *op);
long expect_number();
token *peek();
int is_type_token(token *tok);
void expect_keyword(char *kw); // FIX: Added declaration

// parser (declarations and statements)
ty *parse_struct_specifier();
ty *parse_type_specifier(int *is_unsigned_ptr, int *is_static_ptr, int *is_extern_ptr);
ty *parse_declarator(ty *base, token **ident_tok_ptr);
sinit *parse_initializer(ty *type);
node *declaration(int is_global);
node *stmt();
void parse_fundef_or_global();
void compute_struct_layout(ty *type);

// parser (expressions, lowest precedence first)
node *new_node(node_kind kind, node *lhs, node *rhs);
node *new_node_num(long val);
node *new_node_lvar(lvar *var);
node *new_node_gvar(gvar *gvar);
node *expr();
node *assign();
node *conditional();
node *logical_or();
node *logical_and();
node *inclusive_or();
node *exclusive_or();
node *band();
node *equality();
node *relational();
node *shift();
node *add();
node *mul();
node *unary();
node *postfix();
node *primary();
node *member_access(node *n);

// code generator
void emit_addr(node *n);
void emit_load(ty *type);
void emit_store(ty *type);
void emit_expr(node *n);
void emit_stmt(node *n);
void codegen_program(function *funcs);


// --- ii. utility and error functions ---

// reads a file content into a malloc'ed buffer
char *read_file_to_buffer(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL; // caller handles file not found for #include

    if (fseek(fp, 0, SEEK_END) == -1) { perror("fseek"); exit(1); }
    size_t size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) == -1) { perror("fseek"); exit(1); }

    char *buf = malloc(size + 1);
    if (!buf) { perror("malloc"); exit(1); }

    if (fread(buf, size, 1, fp) != 1) {
        if (feof(fp) == 0) {
            fprintf(stderr, "error reading file: %s\n", path);
            exit(1);
        }
    }

    fclose(fp);
    buf[size] = '\0';
    return buf;
}

// helper to check if a file exists (by trying to open and closing it)
int file_exists(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp) {
        fclose(fp);
        return 1;
    }
    return 0;
}

// finds the full path for an included file
// returns a dynamically allocated path string, or NULL if not found.
char *find_include_path(char *filename, char term_char) {
    // 1. always try filename directly (for "..." and first for <...>)
    if (file_exists(filename)) {
        return strdup(filename); // return a copy of the path
    }

    if (term_char == '>') {
        // 2. for system includes, try in /usr/include/
        const char *prefix = "/usr/include/";
        size_t prefix_len = strlen(prefix);
        size_t filename_len = strlen(filename);

        char *full_path = calloc(1, prefix_len + filename_len + 1);
        if (!full_path) { perror("calloc"); exit(1); }

        strcpy(full_path, prefix);
        strcpy(full_path + prefix_len, filename);

        if (file_exists(full_path)) {
            return full_path; // return the dynamically allocated path
        }
        free(full_path);
    }

    return NULL; // not found
}

// reports an error location and exits
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    // find start of line
    char *line = loc;
    while (user_input < line && line[-1] != '\n') line--;

    // find end of line
    char *end = loc;
    while (*end != '\n' && *end != '\0') end++;

    // print file content around error location
    int linelen = end - line;
    fprintf(stderr, "%.*s\n", linelen, line);

    // print indicator
    int pos = loc - line;
    for (int i = 0; i < pos; i++) fprintf(stderr, " ");
    fprintf(stderr, "^ ");

    // print error message
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_tok(token *tok, char *fmt, ...) {
    if (!tok || tok->kind == tk_eof) {
        fprintf(stderr, "error at eof: ");
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        exit(1);
    }
    error_at(tok->file_pos, fmt);
}

// aligns `n` to `align` boundary
int align_to(int n, int align) {
    return (n + align - 1) & ~(align - 1);
}

// helper for identifier characters
int is_ident_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}


// --- iii. preprocessor implementation (enhanced) ---

macro *find_macro(const char *name, int len) {
    for (macro *m = macros; m; m = m->next) {
        if (m->name && len == (int)strlen(m->name) && strncmp(name, m->name, len) == 0) return m;
    }
    return NULL;
}

void define_macro(const char *name, int len) {
    // simple object-like macro: just record its existence
    if (find_macro(name, len)) return; // already defined

    macro *m = calloc(1, sizeof(macro));
    m->name = strndup(name, len);
    m->next = macros;
    macros = m;
}

void undef_macro(const char *name, int len) {
    macro head = {};
    macro *cur = &head;
    head.next = macros;

    while (cur->next) {
        if (len == (int)strlen(cur->next->name) && strncmp(name, cur->next->name, len) == 0) {
            macro *tmp = cur->next;
            cur->next = tmp->next;
            free(tmp->name);
            free(tmp);
            macros = head.next;
            return;
        }
        cur = cur->next;
    }
    macros = head.next;
}

// helper to consume whitespace, comments, and line continuation
char *skip_junk(char *p) {
    for (;;) {
        if (isspace((unsigned char)*p)) {
            if (*p == '\n') line_number++;
            p++;
            continue;
        }
        if (*p == '\\' && p[1] == '\n') {
            line_number++;
            p += 2;
            continue;
        }
        if (p[0] == '/' && p[1] == '/') { // line comment
            while (*p && *p != '\n') p++;
            continue;
        }
        if (p[0] == '/' && p[1] == '*') { // block comment
            char *q = strstr(p + 2, "*/");
            if (!q) error_at(p, "unterminated block comment");
            p = q + 2;
            continue;
        }
        break;
    }
    return p;
}

// recursively read and preprocesses a single file.
char *preprocess_file(const char *path, int depth) {
    if (depth > 20) {
        fprintf(stderr, "too many recursive includes\n");
        exit(1);
    }

    char *input = read_file_to_buffer(path);
    if (!input) return NULL;

    // conservative allocation for output buffer
    size_t output_len = 0;
    size_t output_cap = strlen(input) * 2;
    char *output = malloc(output_cap);
    if (!output) { perror("malloc"); exit(1); }

    char *p = input;

    while (*p) {
        char *start = p;

        // handle preprocessor directives only at the start of a line
        if (*p == '#') {
            p++;
            p = skip_junk(p);

            // identify directive
            char *ident_start = p;
            while (isalpha((unsigned char)*p)) p++;
            int ident_len = p - ident_start;

            int skip_current = (pp_cond_state && pp_cond_state->skip_block);

            // --- conditional compilation directives ---

            if (strncmp(ident_start, "if", ident_len) == 0) {
                p = skip_junk(p);
                // simplified evaluation: only 0 or 1 is supported for now
                long condition = strtol(p, &p, 0);

                pp_state *new_state = calloc(1, sizeof(pp_state));
                new_state->next = pp_cond_state;
                new_state->if_line = line_number;
                pp_cond_state = new_state;

                if (skip_current) {
                    pp_cond_state->skip_block = 1;
                } else if (!condition) {
                    pp_cond_state->skip_block = 1;
                } else {
                    pp_cond_state->expecting_else = 1;
                }
            } else if (strncmp(ident_start, "ifdef", ident_len) == 0) {
                p = skip_junk(p);
                char *name_start = p;
                while (is_ident_char(*p)) p++;
                int defined = !!find_macro(name_start, p - name_start);

                pp_state *new_state = calloc(1, sizeof(pp_state));
                new_state->next = pp_cond_state;
                new_state->if_line = line_number;
                pp_cond_state = new_state;

                if (skip_current) {
                    pp_cond_state->skip_block = 1;
                } else if (!defined) {
                    pp_cond_state->skip_block = 1;
                } else {
                    pp_cond_state->expecting_else = 1;
                }
            } else if (strncmp(ident_start, "ifndef", ident_len) == 0) {
                p = skip_junk(p);
                char *name_start = p;
                while (is_ident_char(*p)) p++;
                int defined = !!find_macro(name_start, p - name_start);

                pp_state *new_state = calloc(1, sizeof(pp_state));
                new_state->next = pp_cond_state;
                new_state->if_line = line_number;
                pp_cond_state = new_state;

                if (skip_current) {
                    pp_cond_state->skip_block = 1;
                } else if (defined) {
                    pp_cond_state->skip_block = 1;
                } else {
                    pp_cond_state->expecting_else = 1;
                }
            } else if (strncmp(ident_start, "else", ident_len) == 0) {
                if (!pp_cond_state) error_at(start, "unmatched #else");
                if (pp_cond_state->next && pp_cond_state->next->skip_block) {
                    // if parent is skipping, this #else is skipped too
                } else if (pp_cond_state->expecting_else) {
                    pp_cond_state->skip_block = 1; // already found true block, so skip #else
                } else {
                    pp_cond_state->skip_block = 0;
                    pp_cond_state->expecting_else = 1; // mark that a block was taken
                }
            } else if (strncmp(ident_start, "elif", ident_len) == 0) {
                if (!pp_cond_state) error_at(start, "unmatched #elif");
                if (pp_cond_state->next && pp_cond_state->next->skip_block) {
                    // if parent is skipping, this #elif is skipped too
                } else if (pp_cond_state->expecting_else) {
                    pp_cond_state->skip_block = 1; // already found a true block, skip subsequent #elifs
                } else {
                    p = skip_junk(p);
                    long condition = strtol(p, &p, 0); // Simplified

                    if (condition) {
                        pp_cond_state->skip_block = 0;
                        pp_cond_state->expecting_else = 1;
                    } else {
                        pp_cond_state->skip_block = 1;
                    }
                }
            } else if (strncmp(ident_start, "endif", ident_len) == 0) {
                if (!pp_cond_state) error_at(start, "unmatched #endif");
                pp_state *tmp = pp_cond_state;
                pp_cond_state = pp_cond_state->next;
                free(tmp);
            } else if (skip_current) {
                // skip other directives like #define if in a false block
            } else if (strncmp(ident_start, "define", ident_len) == 0) {
                p = skip_junk(p);
                char *name_start = p;
                while (is_ident_char(*p)) p++;
                define_macro(name_start, p - name_start);

            } else if (strncmp(ident_start, "undef", ident_len) == 0) {
                p = skip_junk(p);
                char *name_start = p;
                while (is_ident_char(*p)) p++;
                undef_macro(name_start, p - name_start);

            } else if (strncmp(ident_start, "include", ident_len) == 0) {
                p = skip_junk(p);
                char *filename = NULL;
                char term_char = 0;

                if (*p == '"') term_char = '"';
                else if (*p == '<') term_char = '>';
                else error_at(p, "expected '\"' or '<' after #include");

                p++;
                char *path_start = p;
                while (*p && *p != term_char && *p != '\n') p++;
                if (*p != term_char) error_at(p, "unterminated include path");

                filename = strndup(path_start, p - path_start);

                // --- modified include search logic ---
                char *found_path = find_include_path(filename, term_char);

                char *included_content = NULL;

                if (found_path) {
                    // if a path is found, recursively preprocess it
                    included_content = preprocess_file(found_path, depth + 1);
                    free(found_path);
                }

                if (!included_content) {
                    error_at(start, "include file '%s' not found", filename);
                }

                free(filename); // free the original filename only after potential error reporting
                // --- end modified include search logic ---


                // append included content to output buffer
                size_t inc_len = strlen(included_content);
                if (output_len + inc_len + 2 > output_cap) {
                    output_cap = output_len + inc_len + output_cap;
                    output = realloc(output, output_cap);
                    if (!output) { perror("realloc"); exit(1); }
                }

                // add an extra newline for separation
                output[output_len++] = '\n';
                memcpy(output + output_len, included_content, inc_len);
                output_len += inc_len;
                output[output_len] = '\0';

                free(included_content);
                p++; // skip closing quote/bracket
            } else {
                // skip unknown directive
            }

            // skip to end of directive line (until newline)
            while (*p && *p != '\n') p++;

        } else if (pp_cond_state && pp_cond_state->skip_block) {
            // skip normal code lines if in a false conditional block
            while (*p && *p != '\n') p++;
        } else {
            // normal code/comment/literal handling
            if (*p == '"' || *p == '\'') { // copy string/char literals
                char quote = *p;
                output[output_len++] = *p++;
                while (*p && *p != quote) {
                    if (*p == '\\') p++; // skip escaped char
                    p++;
                    if (output_len + 1 >= output_cap) {
                        output_cap *= 2;
                        output = realloc(output, output_cap);
                        if (!output) { perror("realloc"); exit(1); }
                    }
                }
                if (*p) output[output_len++] = *p++;
                continue;
            }

            // check for comments/line continuation/whitespace
            char *next_p = skip_junk(p);
            if (next_p != p) {
                p = next_p;
                continue;
            }

            // copy character
            output[output_len++] = *p++;
            if (output_len + 1 >= output_cap) {
                output_cap *= 2;
                output = realloc(output, output_cap);
                if (!output) { perror("realloc"); exit(1); }
            }
        }

        // handle newline: update line_number
        if (*(p - 1) == '\n') line_number++;
    }

    if (pp_cond_state) {
        fprintf(stderr, "error: unterminated #if block started at line %d\n", pp_cond_state->if_line);
        exit(1);
    }

    output[output_len] = '\0';
    free(input);
    return output;
}


// --- iv. type helpers ---

ty *new_type(type_kind kind, int size, int align) {
    ty *type = calloc(1, sizeof(ty));
    type->kind = kind;
    type->size = size;
    type->align = align;
    return type;
}

ty *ty_int_def() { return new_type(ty_int, 4, 4); }
ty *ty_char_def() { return new_type(ty_char, 1, 1); }
ty *ty_long_def() { return new_type(ty_long, 8, 8); }
ty *ty_void_def() { return new_type(ty_void, 0, 1); }

ty *new_type_ptr(ty *base) {
    ty *type = new_type(ty_ptr, 8, 8);
    type->base = base;
    return type;
}

ty *new_type_array(ty *base, size_t len) {
    ty *type = new_type(ty_array, base->size * len, base->align);
    type->base = base;
    type->array_len = len;
    return type;
}

ty *new_type_struct(int size, int align) {
    ty *type = new_type(ty_struct, size, align);
    return type;
}

// compute padding and offset for struct members
void compute_struct_layout(ty *type) {
    int offset = 0;
    int max_align = 1;
    for (member *mem = type->members; mem; mem = mem->next) {
        max_align = (max_align < mem->type->align) ? mem->type->align : max_align;
        offset = align_to(offset, mem->type->align);
        mem->offset = offset;
        offset += mem->type->size;
    }
    type->size = align_to(offset, max_align);
    type->align = max_align;
}

// returns true if two types are compatible
int is_compatible_type(ty *t1, ty *t2) {
    if (t1->kind != t2->kind) return 0;
    if (t1->is_unsigned != t2->is_unsigned) return 0;

    switch (t1->kind) {
    case ty_int:
    case ty_char:
    case ty_long:
    case ty_void:
        return 1;
    case ty_ptr:
        return is_compatible_type(t1->base, t2->base);
    case ty_array:
        if (t1->array_len != t2->array_len) return 0;
        return is_compatible_type(t1->base, t2->base);
    case ty_struct:
        // simplified: struct types are only compatible if they are the exact same type definition (pointer equality on members)
        return t1->members == t2->members;
    case ty_func:
        // simplified: function compatibility is complex.
        return 1;
    default:
        return 0;
    }
}

// recursively adds type information to ast nodes
void add_type(node *n) {
    if (!n || n->type) return;

    add_type(n->lhs);
    add_type(n->rhs);
    add_type(n->next);
    add_type(n->els);
    // FIX: removed incompatible sinit * to node * call. sinit is not an ast node.
    // add_type(n->init_list ? n->init_list->children : NULL); // for local_init

    switch (n->kind) {
    case nd_num:
        n->type = ty_long_def();
        break;
    case nd_lvar:
        n->type = n->var->type;
        break;
    case nd_gvar:
        n->type = new_type_ptr(ty_char_def()); // treat global name as a pointer
        break;
    case nd_member:
        n->type = n->mem->type;
        break;
    case nd_deref:
        if (n->lhs->type->kind != ty_ptr && n->lhs->type->kind != ty_array)
            error_at(current_token->file_pos, "invalid pointer dereference");
        n->type = n->lhs->type->base;
        break;
    case nd_addr:
        n->type = new_type_ptr(n->lhs->type);
        break;
    case nd_assign:
    case nd_add:
    case nd_sub:
    case nd_mul:
    case nd_div:
    case nd_mod:
    case nd_shl:
    case nd_shr:
    case nd_band:
    case nd_bor:
    case nd_bxor:
        n->type = n->lhs->type; // simplified: assumes promotion/conversion happened
        break;
    case nd_eq:
    case nd_ne:
    case nd_lt:
    case nd_le:
    case nd_logical_and:
    case nd_logical_or:
    case nd_not:
        n->type = ty_int_def();
        break;
    case nd_bitnot:
    case nd_cond:
        n->type = n->lhs->type;
        break;
    case nd_funcall:
        // for simplicity, assume funcall returns int if not specified (will be fixed later)
        n->type = ty_int_def();
        break;
    case nd_add_assign:
    case nd_sub_assign:
    case nd_mul_assign:
    case nd_div_assign:
    case nd_mod_assign:
    case nd_shl_assign:
    case nd_shr_assign:
    case nd_band_assign:
    case nd_bor_assign:
    case nd_bxor_assign:
        n->type = n->lhs->type;
        break;
    case nd_local_init:
    case nd_return:
    case nd_if:
    case nd_while:
    case nd_for:
    case nd_block:
    case nd_expr_stmt:
    case nd_break:
    case nd_continue:
    case nd_switch:
    case nd_case:
        break;
    }
}


// --- v. variable and global data management ---

ty *find_struct_tag(token *tok) {
    for (struct_tag *t = struct_tags; t; t = t->next) {
        if (t->len == tok->len && strncmp(t->name, tok->str, t->len) == 0) return t->type;
    }
    return NULL;
}

void register_struct_tag(token *tok, ty *type) {
    struct_tag *t = calloc(1, sizeof(struct_tag));
    t->name = tok->str;
    t->len = tok->len;
    t->type = type;
    t->next = struct_tags;
    struct_tags = t;
}

member *get_struct_member(ty *type, token *tok) {
    for (member *mem = type->members; mem; mem = mem->next) {
        if (mem->len == tok->len && strncmp(mem->name, tok->str, tok->len) == 0) return mem;
    }
    error_tok(tok, "no such member in struct");
    return NULL; // unreachable
}

lvar *find_lvar(token *tok) {
    for (lvar *var = current_func->locals; var; var = var->next) {
        if (var->len == tok->len && strncmp(var->name, tok->str, var->len) == 0) return var;
    }
    return NULL;
}

lvar *add_lvar(token *tok, ty *type, int is_static, int is_typedef) {
    lvar *var = find_lvar(tok);
    if (var) {
        error_tok(tok, "redefinition of local variable or typedef");
    }

    var = calloc(1, sizeof(lvar));
    var->next = current_func->locals;
    var->name = tok->str;
    var->len = tok->len;
    var->type = type;
    var->is_static = is_static;
    var->is_typedef = is_typedef;
    current_func->locals = var;

    // assign offset later in parse_fundef_or_global
    return var;
}

gvar *find_global_var(token *tok) {
    for (gvar *var = global_vars; var; var = var->next) {
        if (var->name && var->next && tok->len == (int)strlen(var->name) && strncmp(var->name, tok->str, tok->len) == 0) return var;
    }
    return NULL;
}

gvar *add_global_var(token *tok, ty *type, int is_extern) {
    gvar *var = find_global_var(tok);
    if (var) {
        // handle re-declarations (e.g. extern) vs definitions
        if (var->is_extern && !is_extern) {
            // extern declaration followed by definition
            var->is_extern = is_extern;
            var->size = type->size;
        } else if (var->is_extern && is_extern) {
            // extern followed by extern (allowed)
        } else if (!var->is_extern && is_extern) {
            // definition followed by extern declaration (allowed)
        } else if (var->size != type->size) {
            error_tok(tok, "incompatible re-declaration of global variable");
        } else if (!var->is_extern && !is_extern) {
            error_tok(tok, "redefinition of global variable");
        }
        return var;
    }

    var = calloc(1, sizeof(gvar));
    var->next = global_vars;
    var->name = strndup(tok->str, tok->len);
    var->size = type->size;
    var->is_extern = is_extern;
    global_vars = var;

    return var;
}

// initializer constructors
sinit *new_sinit_int(long val) {
    sinit *init = calloc(1, sizeof(sinit));
    init->kind = init_int;
    init->val = val;
    return init;
}

sinit *new_sinit_ptr(char *str_content) {
    sinit *init = calloc(1, sizeof(sinit));
    init->kind = init_string;
    init->str_content = str_content;

    // generate unique label
    char buf[20];
    sprintf(buf, ".lstr%d", string_label_count++);
    init->str_label = strdup(buf);

    return init;
}


// --- vi. lexer implementation ---

int is_keyword(token *tok) {
    for (keyword_entry *k = keywords; k->name; k++) {
        if (tok->len == k->len && strncmp(tok->str, k->name, k->len) == 0) return 1;
    }
    return 0;
}

token *new_token(token_kind kind, token *cur, char *str, int len) {
    token *tok = calloc(1, sizeof(token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    tok->file_pos = str;
    cur->next = tok;
    return tok;
}

token *tokenize(char *p) {
    token head;
    head.next = NULL;
    token *cur = &head;

    while (*p) {
        p = skip_junk(p);

        if (!*p) break;

        // multi-character operators
        if (strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0 ||
            strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0 ||
            strncmp(p, ">>", 2) == 0 || strncmp(p, "<<", 2) == 0 ||
            strncmp(p, "&&", 2) == 0 || strncmp(p, "||", 2) == 0 ||
            strncmp(p, "+=", 2) == 0 || strncmp(p, "-=", 2) == 0 ||
            strncmp(p, "*=", 2) == 0 || strncmp(p, "/=", 2) == 0 ||
            strncmp(p, "%=", 2) == 0 || strncmp(p, "&=", 2) == 0 ||
            strncmp(p, "|=", 2) == 0 || strncmp(p, "^=", 2) == 0 ||
            strncmp(p, "++", 2) == 0 || strncmp(p, "--", 2) == 0 ||
            strncmp(p, "->", 2) == 0 ||
            strncmp(p, "<<=", 3) == 0 || strncmp(p, ">>=", 3) == 0) {

            int len = (p[2] == '=') ? 3 : 2;
            cur = new_token(tk_reserved, cur, p, len);
            p += len;
            continue;
        }

        // single-character operators and symbols
        if (strchr("+-*/%()><=;{},&[]!~?:.^|", *p)) {
            cur = new_token(tk_reserved, cur, p++, 1);
            continue;
        }

        // string literal
        if (*p == '"') {
            char *start = ++p;
            while (*p && *p != '"') {
                if (*p == '\\') p++; // skip escaped char
                p++;
            }
            if (!*p) error_at(start, "unterminated string literal");

            cur = new_token(tk_string, cur, start, p - start);
            p++; // skip closing quote
            continue;
        }

        // character literal
        if (*p == '\'') {
            char *start = ++p;
            if (*p == '\\') p++; // skip escape sequence
            p++;
            if (*p != '\'') error_at(start, "multi-character or unterminated char literal");

            cur = new_token(tk_num, cur, start, p - start);
            // simplified: value is just the first char. proper escape sequence handling needed for production
            cur->val = *start;

            p++; // skip closing quote
            continue;
        }


        // identifier or keyword
        if (isalpha((unsigned char)*p) || *p == '_') {
            char *start = p;
            while (is_ident_char(*p)) p++;

            int len = p - start;
            cur = new_token(tk_ident, cur, start, len);

            if (is_keyword(cur)) {
                cur->kind = tk_keyword;
            }
            continue;
        }

        // number
        if (isdigit((unsigned char)*p)) {
            char *start = p;
            cur = new_token(tk_num, cur, p, 0);
            cur->val = strtol(p, &p, 0);
            cur->len = p - start;
            continue;
        }

        error_at(p, "invalid token");
    }

    new_token(tk_eof, cur, p, 0);
    return head.next;
}

// --- lexer helpers for parser ---

// consumes the current token if it matches the operator `op`. returns 1 if consumed, 0 otherwise.
int consume(char *op) {
    if (current_token->kind != tk_reserved || current_token->len != (int)strlen(op) || strncmp(current_token->str, op, current_token->len) != 0) return 0;
    current_token = current_token->next;
    return 1;
}

// consumes the current token if it matches the keyword `kw`. returns 1 if consumed, 0 otherwise.
int consume_keyword(char *kw) {
    if (current_token->kind != tk_keyword || current_token->len != (int)strlen(kw) || strncmp(current_token->str, kw, current_token->len) != 0) return 0;
    current_token = current_token->next;
    return 1;
}

// expects the current token to be an identifier and consumes it.
token *expect_ident() {
    if (current_token->kind != tk_ident) error_tok(current_token, "expected identifier");
    token *tok = current_token;
    current_token = current_token->next;
    return tok;
}

// expects the current token to be the operator `op` and consumes it.
void expect(char *op) {
    if (current_token->kind != tk_reserved || current_token->len != (int)strlen(op) || strncmp(current_token->str, op, current_token->len) != 0) error_tok(current_token, "expected '%s'", op);
    current_token = current_token->next;
}

// expects the current token to be a number and consumes it.
long expect_number() {
    if (current_token->kind != tk_num) error_tok(current_token, "expected number");
    long val = current_token->val;
    current_token = current_token->next;
    return val;
}

// returns the current token without consuming it.
token *peek() {
    return current_token;
}

// checks if the token can start a type specifier
int is_type_token(token *tok) {
    if (tok->kind == tk_keyword) {
        if (tok->len == 3 && strncmp(tok->str, "int", 3) == 0) return 1;
        if (tok->len == 4 && strncmp(tok->str, "char", 4) == 0) return 1;
        if (tok->len == 4 && strncmp(tok->str, "long", 4) == 0) return 1;
        if (tok->len == 4 && strncmp(tok->str, "void", 4) == 0) return 1;
        if (tok->len == 8 && strncmp(tok->str, "unsigned", 8) == 0) return 1;
        if (tok->len == 6 && strncmp(tok->str, "struct", 6) == 0) return 1;
        if (tok->len == 7 && strncmp(tok->str, "typedef", 7) == 0) return 1;
        if (tok->len == 6 && strncmp(tok->str, "static", 6) == 0) return 1;
        if (tok->len == 6 && strncmp(tok->str, "extern", 6) == 0) return 1;
    }
    // check for typedef'd types (simplified: only check for lvar is_typedef)
    if (tok->kind == tk_ident && find_lvar(tok) && find_lvar(tok)->is_typedef) return 1; // required for full c
    return 0;
}

// FIX: define missing function
void expect_keyword(char *kw) {
    if (!consume_keyword(kw)) error_tok(current_token, "expected keyword '%s'", kw);
}


// --- vii. ast node constructors ---

node *new_node(node_kind kind, node *lhs, node *rhs) {
    node *n = calloc(1, sizeof(node));
    n->kind = kind;
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

node *new_node_num(long val) {
    node *n = calloc(1, sizeof(node));
    n->kind = nd_num;
    n->val = val;
    add_type(n);
    return n;
}

node *new_node_lvar(lvar *var) {
    node *n = calloc(1, sizeof(node));
    n->kind = nd_lvar;
    n->var = var;
    add_type(n);
    return n;
}

node *new_node_gvar(gvar *gvar) {
    node *n = calloc(1, sizeof(node));
    n->kind = nd_gvar;
    n->gvar = gvar;
    add_type(n);
    return n;
}

// --- viii. parser implementation ---

// struct_specifier = "struct" (IDENT | "{" declaration_list "}")
ty *parse_struct_specifier() {
    // FIX: use defined expect_keyword
    expect_keyword("struct");

    token *tag_tok = NULL;
    if (current_token->kind == tk_ident) {
        tag_tok = expect_ident();
    }

    if (consume("{")) {
        // struct definition
        ty *type = new_type_struct(0, 1);
        int max_align = 1;
        int offset = 0;
        member head = {};
        member *cur = &head;

        while (!consume("}")) {
            // member declarations: type_specifier declarator ("," declarator)* ";"
            ty *base = parse_type_specifier(NULL, NULL, NULL);

            do {
                token *ident_tok = NULL;
                ty *type_decl = parse_declarator(base, &ident_tok);
                if (!ident_tok) error_tok(current_token, "expected member name in struct declaration");

                member *mem = calloc(1, sizeof(member));
                mem->name = ident_tok->str;
                mem->len = ident_tok->len;
                mem->type = type_decl;
                cur->next = mem;
                cur = cur->next;

                // compute layout: offset will be updated in compute_struct_layout
                max_align = (max_align < type_decl->align) ? type_decl->align : max_align;
                offset = align_to(offset, type_decl->align);
                mem->offset = offset;
                offset += mem->type->size;

            } while (consume(","));
            expect(";");
        }
        type->members = head.next;
        type->size = align_to(offset, max_align);
        type->align = max_align;

        if (tag_tok) {
            register_struct_tag(tag_tok, type);
        }
        return type;

    } else if (tag_tok) {
        // struct forward declaration or tag usage
        ty *type = find_struct_tag(tag_tok);
        if (!type) {
            // simplified: allow undefined struct tag for forward declaration
            type = new_type_struct(0, 1);
            register_struct_tag(tag_tok, type);
        }
        return type;
    }

    error_tok(current_token, "expected struct tag or definition");
    return NULL; // unreachable
}

// type_specifier = ("int" | "char" | "long" | "unsigned" | "void" | struct_specifier | typedef_name)+
// also handles storage class specifiers like 'static' and 'extern'
ty *parse_type_specifier(int *is_unsigned_ptr, int *is_static_ptr, int *is_extern_ptr) {
    ty *type = NULL;
    int is_unsigned = 0;
    int is_static = 0;
    int is_extern = 0;
    int found_type = 0;

    for (;;) {
        if (consume_keyword("char")) {
            if (found_type) error_tok(current_token, "duplicate type specifier");
            type = ty_char_def();
            found_type = 1;
        } else if (consume_keyword("int")) {
            if (found_type) error_tok(current_token, "duplicate type specifier");
            type = ty_int_def();
            found_type = 1;
        } else if (consume_keyword("long")) {
            // treat 'long' as 8-byte, like 'long long'
            if (found_type && type->kind == ty_long) error_tok(current_token, "too many 'long's");
            type = ty_long_def();
            found_type = 1;
        } else if (consume_keyword("void")) {
            if (found_type) error_tok(current_token, "duplicate type specifier");
            type = ty_void_def();
            found_type = 1;
        } else if (consume_keyword("unsigned")) {
            if (is_unsigned) error_tok(current_token, "duplicate 'unsigned'");
            is_unsigned = 1;
        } else if (consume_keyword("static")) {
            if (is_static) error_tok(current_token, "duplicate 'static'");
            if (is_extern) error_tok(current_token, "'static' and 'extern' are mutually exclusive");
            is_static = 1;
        } else if (consume_keyword("extern")) {
            if (is_extern) error_tok(current_token, "duplicate 'extern'");
            if (is_static) error_tok(current_token, "'static' and 'extern' are mutually exclusive");
            is_extern = 1;
        } else if (current_token->kind == tk_keyword && strncmp(current_token->str, "struct", 6) == 0) {
            if (found_type) error_tok(current_token, "duplicate type specifier");
            type = parse_struct_specifier();
            found_type = 1;
        } else if (current_token->kind == tk_ident && find_lvar(current_token) && find_lvar(current_token)->is_typedef) {
            // consume typedef name
            lvar *typedef_var = find_lvar(expect_ident());
            if (found_type) error_tok(current_token, "duplicate type specifier");
            type = typedef_var->type;
            found_type = 1;
        } else {
            break; // no more type specifiers
        }
    }

    if (!found_type) {
        // default to int if no type specified (e.g. `static a;`)
        type = ty_int_def();
    }

    if (type->kind == ty_void && is_unsigned) error_tok(current_token, "'unsigned void' is invalid");
    type->is_unsigned = is_unsigned;

    if (is_unsigned_ptr) *is_unsigned_ptr = is_unsigned;
    if (is_static_ptr) *is_static_ptr = is_static;
    if (is_extern_ptr) *is_extern_ptr = is_extern;

    return type;
}

// declarator = ("*")* (IDENT | "(" declarator ")") (func_params | array_size)*
// returns the final type, updates ident_tok_ptr with the declared name token
ty *parse_declarator(ty *base, token **ident_tok_ptr) {
    ty *type = base;
    int pointer_count = 0;

    // 1. handle pointers
    while (consume("*")) pointer_count++;

    // 2. handle identifier or nested declarator
    token *ident_tok = NULL;
    if (current_token->kind == tk_ident) {
        ident_tok = expect_ident();
    } else if (consume("(")) {
        type = parse_declarator(base, &ident_tok);
        expect(")");
    } else if (pointer_count == 0) {
        // anonymous declarator (e.g. in abstract declarator, or just a specifier like struct {int a;};)
    } else {
        // only pointers are left, but no identifier or nested declarator
        error_tok(current_token, "expected identifier or '(' after pointer specifier");
    }

    // 3. handle array and function specifiers
    for (;;) {
        if (consume("[")) {
            // array: base[size]
            long len = 0;
            if (current_token->kind == tk_num) {
                len = expect_number();
            } else if (consume_keyword("sizeof")) {
                error_tok(current_token, "sizeof not implemented for array length");
            } else if (!consume("]")) {
                error_tok(current_token, "expected array size or ']'");
            }
            expect("]");
            type = new_type_array(type, len);
        } else if (consume("(")) {
            // function: base()
            ty *func_type = new_type(ty_func, 0, 1);
            func_type->base = type; // return type

            // simplified parameter list: only count, don't store types (for now)
            int num_params = 0;
            if (!consume(")")) {
                do {
                    // skip type specifiers and declarator for parameters
                    parse_type_specifier(NULL, NULL, NULL);
                    token *param_name = NULL;
                    parse_declarator(ty_int_def(), &param_name); // base type doesn't matter much here

                    if (consume("...")) {
                        func_type->is_variadic = 1;
                    }
                    num_params++;
                } while (consume(","));
                expect(")");
            }

            // need to correctly track num_params in the function type if needed
            // func_type->num_params = num_params; // not available in ty, but in function

            type = func_type;
        } else {
            break;
        }
    }

    // 4. apply pointers backwards
    for (int i = 0; i < pointer_count; i++) {
        type = new_type_ptr(type);
    }

    if (ident_tok_ptr) *ident_tok_ptr = ident_tok;
    return type;
}

// initializer = assignment_expression | "{" initializer_list ","? "}"
// initializer_list = initializer ("," initializer)*
// Simplified: only supports constant expression or string literal for scalar/char array.
sinit *parse_initializer(ty *type) {
    if (type->kind == ty_array && type->base->kind == ty_char && current_token->kind == tk_string) {
        // string literal initializer for char array
        token *str_tok = current_token;
        current_token = current_token->next;
        return new_sinit_ptr(strndup(str_tok->str, str_tok->len)); // copy the string
    }

    // initializer list for arrays/structs (simplified)
    if (consume("{")) {
        sinit *head = calloc(1, sizeof(sinit));
        sinit *cur = head;

        // Simplified list handling: expect a flat list of constant expressions
        do {
            node *init_expr = conditional();
            if (init_expr->kind != nd_num) {
                error_tok(current_token, "initializer list elements must be constant integers (simplified)");
            }
            cur->next = new_sinit_int(init_expr->val);
            cur = cur->next;
        } while (consume(","));

        consume("}"); // optional trailing comma handled by looping condition

        sinit *list_node = calloc(1, sizeof(sinit));
        list_node->kind = (type->kind == ty_array) ? init_array : init_struct;
        list_node->type = type;
        list_node->children = head->next;
        free(head);
        return list_node;
    }

    // scalar initializer
    node *init_expr = conditional();
    if (init_expr->kind == nd_num) {
        return new_sinit_int(init_expr->val);
    }

    // complex expressions for initialization are not fully supported yet, only constant folding needed
    error_tok(current_token, "initializer must be constant expression or initializer list");
    return NULL; // unreachable
}

// declaration = type_specifier (declarator ("=" initializer)? ("," declarator ("=" initializer)?)*)? ";"
node *declaration(int is_global) {
    int is_static = 0;
    int is_extern = 0;
    ty *base = parse_type_specifier(NULL, &is_static, &is_extern);

    node head = {};
    node *cur = &head;

    if (base->kind == ty_void && !consume_keyword("typedef")) error_tok(current_token, "variable declared void");

    // handle empty declaration (e.g. `static int;`)
    if (consume(";")) return NULL;

    do {
        token *ident_tok = NULL;
        ty *type_decl = parse_declarator(base, &ident_tok);
        if (!ident_tok) error_tok(current_token, "expected identifier after declaration specifier");

        // handle typedef
        if (consume_keyword("typedef")) {
            if (!is_global) error_tok(ident_tok, "typedef not allowed in local scope (simplified)");
            add_lvar(ident_tok, type_decl, 0, 1);
            continue;
        }

        sinit *initializer = NULL;
        if (consume("=")) {
            if (is_extern) error_tok(ident_tok, "extern variable cannot have an initializer");
            initializer = parse_initializer(type_decl);
        }

        // handle variable declaration/definition
        if (is_global || is_static) {
            // Global or static local variable
            gvar *gvar = add_global_var(ident_tok, type_decl, is_extern);
            if (is_static) {
                // Static local variables need a unique generated name
                char buf[20];
                sprintf(buf, ".lstatic%d", static_var_count++);
                gvar->name = strdup(buf);
            }
            gvar->initializer = initializer;
            if (!is_extern && !gvar->initializer) {
                // global/static variable without initializer defaults to zero
                // gvar->initializer = NULL is correct; emitter handles default zero
            }
        } else {
            // local (automatic) variable definition
            lvar *lvar = add_lvar(ident_tok, type_decl, 0, 0);

            if (initializer) {
                // local initialization needs an nd_local_init node
                node *n = calloc(1, sizeof(node));
                n->kind = nd_local_init;
                n->var = lvar;
                n->init_list = initializer;

                // local initialization is an expression statement
                node *expr_stmt = new_node(nd_expr_stmt, n, NULL);
                cur->next = expr_stmt;
                cur = cur->next;
            }
        }
    } while (consume(","));

    expect(";");

    // return the list of nd_expr_stmt nodes (local initializations) or NULL
    return head.next;
}

// compound_stmt = "{" block_item* "}"
// block_item = declaration | stmt
// function definition = type_specifier declarator "{" block_item* "}"
void parse_fundef_or_global() {
    token *start_tok = current_token;
    int is_static = 0;
    int is_extern = 0;
    ty *base = parse_type_specifier(NULL, &is_static, &is_extern);

    token *ident_tok = NULL;
    ty *type_decl = parse_declarator(base, &ident_tok);

    // backtrack for function parameter parsing if this is a function definition
    if (type_decl->kind == ty_func && ident_tok && current_token->kind == tk_reserved && *current_token->str == '{') {
        // Function Definition
        function *func = calloc(1, sizeof(function));
        func->name = strndup(ident_tok->str, ident_tok->len);
        func->ret_type = type_decl->base;
        func->is_static = is_static;
        func->next = program_head;
        program_head = func;
        current_func = func;

        // Reset local variables for function scope
        func->locals = NULL;

        // Re-parse parameters now that the function context is set
        current_token = start_tok; // Re-parse up to the function name
        parse_type_specifier(NULL, &is_static, &is_extern); // consume specifiers
        // FIX: Removed assignment to unused variable func_name_tok
        expect_ident();

        // parameter list (full definition parsing)
        expect("(");
        int num_params = 0;
        if (!consume(")")) {
            do {
                ty *param_base = parse_type_specifier(NULL, NULL, NULL);
                token *name = NULL;
                ty *param_type = parse_declarator(param_base, &name);

                // treat array parameter as pointer
                if (param_type->kind == ty_array) {
                    param_type = new_type_ptr(param_type->base);
                }

                if (strncmp(current_token->str, "...", 3) == 0 && current_token->len == 3) {
                    func->is_variadic = 1;
                    current_token = current_token->next;
                } else if (name) {
                    add_lvar(name, param_type, 0, 0);
                    num_params++;
                }
            } while (consume(","));
            expect(")");
        }
        func->num_params = num_params;

        expect("{");

        // function body
        node head = {};
        node *cur = &head;
        while (!consume("}")) {
            cur->next = stmt();
            cur = cur->next;
        }

        func->body = calloc(1, sizeof(node));
        func->body->kind = nd_block;
        func->body->lhs = head.next;

        // compute variable offsets (stack size)
        int offset = 0;
        for (lvar *var = func->locals; var; var = var->next) {
            if (var->is_typedef) continue;

            offset = align_to(offset, var->type->align);
            offset += var->type->size;
            var->offset = offset;
        }
        func->stack_size = align_to(offset, 16);

        current_func = NULL;

    } else {
        // Global Variable Declaration or Definition (already handled in declaration)
        current_token = start_tok; // Re-parse everything
        declaration(1);
    }
}

// stmt = "return" expr ";"
// | "if" "(" expr ")" stmt ("else" stmt)?
// | "while" "(" expr ")" stmt
// | "for" "(" (expr? | declaration) ";" expr? ";" expr? ")" stmt
// | "break" ";"
// | "continue" ";"
// | "switch" "(" expr ")" stmt
// | "{" block_item* "}"
// | expr? ";"
// | declaration
node *stmt() {
    node *n = NULL;

    if (consume_keyword("return")) {
        n = new_node(nd_return, expr(), NULL);
        expect(";");
    } else if (consume_keyword("if")) {
        expect("(");
        node *cond = expr();
        expect(")");
        node *then = stmt();
        n = new_node(nd_if, cond, then);
        if (consume_keyword("else")) {
            n->els = stmt();
        }
    } else if (consume_keyword("while")) {
        expect("(");
        node *cond = expr();
        expect(")");

        continue_break_labels *new_labels = calloc(1, sizeof(continue_break_labels));
        new_labels->continue_label = label_count++;
        new_labels->break_label = label_count++;
        new_labels->next = loop_or_switch_labels;
        loop_or_switch_labels = new_labels;

        node *body = stmt();

        loop_or_switch_labels = new_labels->next;
        free(new_labels);

        n = new_node(nd_while, cond, body);
        n->val = new_labels->continue_label;          // continue label
        n->els = new_node_num(new_labels->break_label); // break label
    } else if (consume_keyword("for")) {
        node *init = NULL;
        node *cond = NULL;
        node *inc = NULL;
        expect("(");

        // init
        if (!consume(";")) {
            if (is_type_token(peek())) {
                init = declaration(0);
                // declaration returns a list of assignment nodes, so it's a block
                if (init) {
                    node *block = calloc(1, sizeof(node));
                    block->kind = nd_block;
                    block->lhs = init;
                    init = block;
                }
            } else {
                init = new_node(nd_expr_stmt, expr(), NULL);
                expect(";");
            }
        }

        // cond
        if (!consume(";")) {
            cond = expr();
            expect(";");
        } else {
            // default condition is 1 (true)
            cond = new_node_num(1);
        }

        // inc
        if (!consume(")")) {
            inc = new_node(nd_expr_stmt, expr(), NULL);
            expect(")");
        }

        continue_break_labels *new_labels = calloc(1, sizeof(continue_break_labels));
        new_labels->continue_label = label_count++;
        new_labels->break_label = label_count++;
        new_labels->next = loop_or_switch_labels;
        loop_or_switch_labels = new_labels;

        node *body = stmt();

        loop_or_switch_labels = new_labels->next;
        free(new_labels);

        n = calloc(1, sizeof(node));
        n->kind = nd_for;
        n->lhs = init;                       // init
        n->rhs = cond;                       // cond
        n->els = body;                       // body
        n->next = inc;                       // inc (misuse of next)
        n->val = new_labels->continue_label; // continue label
        // FIX: Cast new_node_num (node*) to ty* to resolve type incompatibility warning.
        // The value will be retrieved later by casting n->type back to node*.
        n->type = (ty *)new_node_num(new_labels->break_label); // break label (misuse of type field)
    } else if (consume_keyword("break")) {
        if (!loop_or_switch_labels) error_tok(current_token, "break not within loop or switch");
        n = new_node_num(loop_or_switch_labels->break_label); // hack: use nd_num to hold label
        n->kind = nd_break;
        expect(";");
    } else if (consume_keyword("continue")) {
        if (!loop_or_switch_labels || loop_or_switch_labels->continue_label == 0) error_tok(current_token, "continue not within loop");
        n = new_node_num(loop_or_switch_labels->continue_label); // hack: use nd_num to hold label
        n->kind = nd_continue;
        expect(";");
    } else if (consume_keyword("switch")) {
        expect("(");
        node *cond = expr();
        expect(")");

        continue_break_labels *new_labels = calloc(1, sizeof(continue_break_labels));
        new_labels->break_label = label_count++; // break label for switch
        new_labels->next = loop_or_switch_labels;
        loop_or_switch_labels = new_labels;

        node *body = stmt();

        loop_or_switch_labels = new_labels->next;
        free(new_labels);

        n = new_node(nd_switch, cond, body);
        n->val = new_labels->break_label; // break label
    } else if (consume_keyword("case")) {
        if (!loop_or_switch_labels || loop_or_switch_labels->break_label == 0) error_tok(current_token, "case statement not within switch");
        node *val = new_node_num(expect_number());
        expect(":");
        n = new_node(nd_case, val, stmt());
    } else if (consume_keyword("default")) {
        if (!loop_or_switch_labels || loop_or_switch_labels->break_label == 0) error_tok(current_token, "default statement not within switch");
        expect(":");
        n = calloc(1, sizeof(node));
        n->kind = nd_case;
        n->rhs = stmt(); // default case has no lhs (value)
    } else if (consume("{")) {
        node head = {};
        node *cur = &head;
        while (!consume("}")) {
            cur->next = stmt();
            cur = cur->next;
        }
        n = calloc(1, sizeof(node));
        n->kind = nd_block;
        n->lhs = head.next;
    } else if (is_type_token(peek())) {
        n = declaration(0); // local declaration
        // a declaration can result in a list of assignment nodes, so it's wrapped in a block if non-null
        if (n) {
            node *block = calloc(1, sizeof(node));
            block->kind = nd_block;
            block->lhs = n;
            n = block;
        } else {
            // empty declaration like `int;`
        }
    } else {
        // expression statement
        if (consume(";")) {
            n = calloc(1, sizeof(node)); // empty statement (null statement)
        } else {
            n = new_node(nd_expr_stmt, expr(), NULL);
            expect(";");
        }
    }

    // recursively add type information
    add_type(n);
    return n;
}

// --- ix. parser (expressions) ---

// member_access = ("." IDENT | "->" IDENT)
node *member_access(node *n) {
    if (consume(".")) {
        if (n->type->kind != ty_struct) error_tok(current_token, "'.' operator requires struct type");
        token *ident = expect_ident();
        member *mem = get_struct_member(n->type, ident);
        node *new_node = calloc(1, sizeof(node));
        new_node->kind = nd_member;
        new_node->lhs = n;
        new_node->mem = mem;
        add_type(new_node);
        return new_node;
    } else if (consume("->")) {
        if (n->type->kind != ty_ptr || n->type->base->kind != ty_struct) error_tok(current_token, "'->' operator requires pointer to struct");
        // a->b is equivalent to (*a).b
        node *deref = new_node(nd_deref, n, NULL);
        add_type(deref);
        return member_access(deref); // recurse for '.'
    }
    return NULL;
}

// primary = num | IDENT | funcall | string | "(" expr ")"
node *primary() {
    if (consume("(")) {
        node *n = expr();
        expect(")");
        return n;
    }

    if (current_token->kind == tk_num) {
        return new_node_num(expect_number());
    }

    if (current_token->kind == tk_string) {
        // string literal is treated as a pointer to a global char array
        token *tok = current_token;
        current_token = current_token->next;

        // create a temporary gvar to hold the string literal content
        char buf[20];
        sprintf(buf, ".lstr%d", string_label_count++);
        gvar *gvar = calloc(1, sizeof(gvar));
        gvar->next = global_vars;
        gvar->name = strdup(buf);
        gvar->size = tok->len + 1;
        gvar->initializer = new_sinit_ptr(strndup(tok->str, tok->len));
        gvar->initializer->kind = init_string;
        gvar->initializer->str_label = gvar->name;
        global_vars = gvar;

        node *n = new_node_gvar(gvar);
        n->type = new_type_ptr(ty_char_def());
        return n;
    }

    if (current_token->kind == tk_ident) {
        token *tok = current_token;
        current_token = current_token->next;

        if (consume("(")) {
            // function call
            node *n = calloc(1, sizeof(node));
            n->kind = nd_funcall;
            n->name = strndup(tok->str, tok->len);

            node head = {};
            node *cur = &head;
            int num_args = 0;

            if (!consume(")")) {
                do {
                    cur->next = assign(); // use assign to allow comma operator in args
                    cur = cur->next;
                    num_args++;
                } while (consume(","));
                expect(")");
            }

            n->lhs = head.next; // arguments list
            n->val = num_args;  // reuse val for num_params
            add_type(n);
            return n;
        }

        lvar *lvar = find_lvar(tok);
        if (lvar) return new_node_lvar(lvar);

        // treat as global variable reference (will be handled as &gvar + dereference)
        gvar *gvar = add_global_var(tok, ty_int_def(), 1); // assume extern int if undeclared
        return new_node_gvar(gvar);
    }

    error_tok(current_token, "expected expression");
    return NULL; // unreachable
}

// postfix = primary ("[" expr "]" | "." IDENT | "->" IDENT | "++" | "--")*
node *postfix() {
    node *n = primary();
    for (;;) {
        if (consume("[")) {
            // a[b] is parsed as *(a + b)
            node *idx = expr();
            expect("]");
            // pointer arithmetic converts to nd_add
            n = new_node(nd_add, n, idx);
            add_type(n);
            n = new_node(nd_deref, n, NULL);
            add_type(n);
            continue;
        }

        node *m = member_access(n);
        if (m) {
            n = m;
            continue;
        }

        // simplified: ++ and -- as expr statement (not real postfix)
        if (consume("++") || consume("--")) {
            error_tok(current_token, "postfix ++/-- not fully implemented for proper side-effect ordering");
            continue;
        }
        break;
    }
    return n;
}

// unary = ("+" | "-" | "*" | "&" | "!" | "~")* postfix
node *unary() {
    if (consume("+")) return unary();
    if (consume("-")) return new_node(nd_sub, new_node_num(0), unary());
    if (consume("*")) return new_node(nd_deref, unary(), NULL);
    if (consume("&")) return new_node(nd_addr, unary(), NULL);
    if (consume("!")) return new_node(nd_not, unary(), NULL);
    if (consume("~")) return new_node(nd_bitnot, unary(), NULL);
    return postfix();
}

// mul = unary (("*" | "/" | "%") unary)*
node *mul() {
    node *n = unary();
    for (;;) {
        if (consume("*")) n = new_node(nd_mul, n, unary());
        else if (consume("/")) n = new_node(nd_div, n, unary());
        else if (consume("%")) n = new_node(nd_mod, n, unary());
        else return n;
    }
}

// add = mul (("+" | "-") mul)*
node *add() {
    node *n = mul();
    for (;;) {
        if (consume("+")) n = new_node(nd_add, n, mul());
        else if (consume("-")) n = new_node(nd_sub, n, mul());
        else return n;
    }
}

// shift = add (("<<" | ">>") add)*
node *shift() {
    node *n = add();
    for (;;) {
        if (consume("<<")) n = new_node(nd_shl, n, add());
        else if (consume(">>")) n = new_node(nd_shr, n, add());
        else return n;
    }
}

// relational = shift (("<" | "<=" | ">" | ">=") shift)*
node *relational() {
    node *n = shift();
    for (;;) {
        if (consume("<")) n = new_node(nd_lt, n, shift());
        else if (consume("<=")) n = new_node(nd_le, n, shift());
        else if (consume(">")) n = new_node(nd_lt, shift(), n); // a > b is b < a
        else if (consume(">=")) n = new_node(nd_le, shift(), n); // a >= b is b <= a
        else return n;
    }
}

// equality = relational (("==" | "!=") relational)*
node *equality() {
    node *n = relational();
    for (;;) {
        if (consume("==")) n = new_node(nd_eq, n, relational());
        else if (consume("!=")) n = new_node(nd_ne, n, relational());
        else return n;
    }
}

// band = equality ("&" equality)* (bitwise and)
node *band() {
    node *n = equality();
    for (;;) {
        if (consume("&")) n = new_node(nd_band, n, equality());
        else return n;
    }
}

// exclusive_or = band ("^" band)* (bitwise xor)
node *exclusive_or() {
    node *n = band();
    for (;;) {
        if (consume("^")) n = new_node(nd_bxor, n, band());
        else return n;
    }
}

// inclusive_or = exclusive_or ("|" exclusive_or)* (bitwise or)
node *inclusive_or() {
    node *n = exclusive_or();
    for (;;) {
        if (consume("|")) n = new_node(nd_bor, n, exclusive_or());
        else return n;
    }
}

// logical_and = inclusive_or ("&&" inclusive_or)*
node *logical_and() {
    node *n = inclusive_or();
    for (;;) {
        if (consume("&&")) n = new_node(nd_logical_and, n, inclusive_or());
        else return n;
    }
}

// logical_or = logical_and ("||" logical_and)*
node *logical_or() {
    node *n = logical_and();
    for (;;) {
        if (consume("||")) n = new_node(nd_logical_or, n, logical_and());
        else return n;
    }
}

// conditional = logical_or ("?" expr ":" conditional)?
node *conditional() {
    node *n = logical_or();
    if (consume("?")) {
        node *cond = calloc(1, sizeof(node));
        cond->kind = nd_cond;
        cond->lhs = n;
        cond->rhs = expr(); // true branch
        expect(":");
        cond->els = conditional(); // false branch
        add_type(cond);
        return cond;
    }
    return n;
}

// assign = conditional (ASSIGN_OP assign)?
node *assign() {
    node *n = conditional();
    if (consume("=")) n = new_node(nd_assign, n, assign());
    else if (consume("+=")) n = new_node(nd_add_assign, n, assign());
    else if (consume("-=")) n = new_node(nd_sub_assign, n, assign());
    else if (consume("*=")) n = new_node(nd_mul_assign, n, assign());
    else if (consume("/=")) n = new_node(nd_div_assign, n, assign());
    else if (consume("%=")) n = new_node(nd_mod_assign, n, assign());
    else if (consume("<<=")) n = new_node(nd_shl_assign, n, assign());
    else if (consume(">>=")) n = new_node(nd_shr_assign, n, assign());
    else if (consume("&=")) n = new_node(nd_band_assign, n, assign());
    else if (consume("|=")) n = new_node(nd_bor_assign, n, assign());
    else if (consume("^=")) n = new_node(nd_bxor_assign, n, assign());
    return n;
}

// expr = assign
node *expr() {
    return assign();
}


// --- x. code generator (x86_64 assembly) ---

int gen_label() {
    return label_count++;
}

// reg order: rdi, rsi, rdx, rcx, r8, r9
char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// returns the address of the given node `n`
void emit_addr(node *n) {
    switch (n->kind) {
    case nd_lvar:
        if (n->var->is_static) {
            printf("  lea rax, %s[rip]\n", n->var->gname);
        } else {
            printf("  mov rax, rbp\n");
            printf("  sub rax, %d\n", n->var->offset);
        }
        return;
    case nd_gvar:
        printf("  lea rax, %s[rip]\n", n->gvar->name);
        return;
    case nd_deref:
        emit_expr(n->lhs);
        return;
    case nd_member:
        emit_addr(n->lhs);
        printf("  add rax, %d\n", n->mem->offset);
        return;
    default:
        error_tok(current_token, "not an lvalue: %d", n->kind);
    }
}

// loads the value pointed to by rax onto the stack
void emit_load(ty *type) {
    if (type->kind == ty_array || type->kind == ty_struct || type->kind == ty_void) {
        // cannot load an array, struct, or void
        return;
    }

    if (type->size == 1) {
        printf("  movzx rax, byte ptr [rax]\n");
    } else if (type->size == 4) {
        printf("  movsxd rax, dword ptr [rax]\n");
    } else { // 8 byte (long/ptr)
        printf("  mov rax, [rax]\n");
    }
}

// pops a value from stack (rdi) and stores it at the address (rax)
void emit_store(ty *type) {
    if (type->kind == ty_struct || type->kind == ty_array || type->kind == ty_void) {
        // structs and arrays are only assignable via memcpy, which is not implemented here.
        return;
    }

    printf("  pop rdi\n"); // value to store (rhs)
    printf("  pop rax\n"); // address to store into (lhs)

    if (type->size == 1) {
        printf("  mov [rax], dil\n");
    } else if (type->size == 4) {
        printf("  mov [rax], edi\n");
    } else { // 8 byte (long/ptr)
        printf("  mov [rax], rdi\n");
    }
    printf("  push rdi\n"); // push stored value as expr result
}

// recursively generates code for an expression
void emit_expr(node *n) {
    add_type(n);

    switch (n->kind) {
    case nd_num:
        printf("  push %ld\n", n->val);
        return;
    case nd_lvar:
    case nd_gvar:
    case nd_member:
        emit_addr(n);
        emit_load(n->type);
        return;
    case nd_addr:
        emit_addr(n->lhs);
        printf("  push rax\n");
        return;
    case nd_deref:
        emit_expr(n->lhs);
        emit_load(n->type);
        return;
    case nd_assign:
        emit_addr(n->lhs); // address of LHS onto stack (rax)
        printf("  push rax\n");
        emit_expr(n->rhs); // value of RHS onto stack (rdi)
        emit_store(n->type);
        return;
    case nd_funcall: {
        int num_args = n->val;
        node *cur = n->lhs;
        int i = 0;

        // push arguments onto stack in reverse order
        for (node *arg = cur; arg; arg = arg->next) {
            emit_expr(arg);
        }

        // pop arguments into registers
        for (i = num_args - 1; i >= 0; i--) {
            printf("  pop %s\n", argreg[i]);
        }

        printf("  call %s\n", n->name);
        printf("  push rax\n"); // push return value
        return;
    }
    case nd_cond: { // ? :
        int else_label = gen_label();
        int end_label = gen_label();

        emit_expr(n->lhs); // condition
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", else_label);

        emit_expr(n->rhs); // true branch
        printf("  jmp .l%d\n", end_label);

        printf(".l%d:\n", else_label);
        emit_expr(n->els); // false branch

        printf(".l%d:\n", end_label);
        return;
    }
    case nd_local_init: {
        // Zero out the space for the variable
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", n->var->offset);
        printf("  mov rdi, rax\n");
        printf("  mov rcx, %d\n", n->var->type->size);
        printf("  xor al, al\n");
        printf("  rep stosb\n");

        // Assignment logic for initialization
        // emit_addr(lvar) is already known (rax=rbp-offset)
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", n->var->offset);
        printf("  push rax\n"); // address onto stack

        if (n->init_list->kind == init_string) {
            // string literal: value is address of global string
            printf("  lea rdi, %s[rip]\n", n->init_list->str_label);
            printf("  push rdi\n");
        } else if (n->init_list->kind == init_int) {
            // scalar initialization
            printf("  push %ld\n", n->init_list->val);
        } else {
            error_tok(current_token, "unsupported local aggregate initializer type");
        }

        emit_store(n->var->type); // uses rax (addr) and rdi (val)
        return;
    }
    case nd_add_assign:
    case nd_sub_assign:
    case nd_mul_assign:
    case nd_div_assign:
    case nd_mod_assign:
    case nd_shl_assign:
    case nd_shr_assign:
    case nd_band_assign:
    case nd_bor_assign:
    case nd_bxor_assign:
        // emit: addr(lhs), push addr
        emit_addr(n->lhs);
        printf("  push rax\n");
        // emit: expr(lhs), push val
        emit_expr(n->lhs);
        // emit: expr(rhs), push val
        emit_expr(n->rhs);

        // perform operation
        printf("  pop rdi\n"); // rhs
        printf("  pop rax\n"); // lhs
        if (n->kind == nd_add_assign) {
            if (n->lhs->type->kind == ty_ptr || n->lhs->type->kind == ty_array) {
                printf("  imul rdi, rdi, %d\n", n->lhs->type->base->size);
            }
            printf("  add rax, rdi\n");
        } else if (n->kind == nd_sub_assign) {
            if (n->lhs->type->kind == ty_ptr || n->lhs->type->kind == ty_array) {
                printf("  imul rdi, rdi, %d\n", n->lhs->type->base->size);
            }
            printf("  sub rax, rdi\n");
        } else if (n->kind == nd_mul_assign) {
            printf("  imul rax, rdi\n");
        } else if (n->kind == nd_div_assign || n->kind == nd_mod_assign) {
            printf("  cqo\n");
            printf("  idiv rdi\n");
            if (n->kind == nd_mod_assign) printf("  mov rax, rdx\n");
        } else if (n->kind == nd_shl_assign) {
            printf("  mov rcx, rdi\n");
            printf("  sal rax, cl\n");
        } else if (n->kind == nd_shr_assign) {
            printf("  mov rcx, rdi\n");
            printf("  sar rax, cl\n");
        } else if (n->kind == nd_band_assign) {
            printf("  and rax, rdi\n");
        } else if (n->kind == nd_bor_assign) {
            printf("  or rax, rdi\n");
        } else if (n->kind == nd_bxor_assign) {
            printf("  xor rax, rdi\n");
        }

        printf("  push rax\n"); // result of op (value to store)
        emit_store(n->lhs->type);
        return;

    case nd_not:
        emit_expr(n->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        printf("  push rax\n");
        return;
    case nd_bitnot:
        emit_expr(n->lhs);
        printf("  pop rax\n");
        printf("  not rax\n");
        printf("  push rax\n");
        return;
    case nd_logical_and: {
        int false_label = gen_label();
        int end_label = gen_label();

        emit_expr(n->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", false_label); // first expr is false

        emit_expr(n->rhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", false_label); // second expr is false

        // both true
        printf("  push 1\n");
        printf("  jmp .l%d\n", end_label);

        printf(".l%d:\n", false_label);
        printf("  push 0\n"); // result is false
        printf(".l%d:\n", end_label);
        return;
    }
    case nd_logical_or: {
        int true_label = gen_label();
        int false_label = gen_label();
        int end_label = gen_label();

        emit_expr(n->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  jne .l%d\n", true_label); // first expr is true

        emit_expr(n->rhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", false_label); // second expr is false

        printf(".l%d:\n", true_label);
        printf("  push 1\n"); // result is true
        printf("  jmp .l%d\n", end_label);

        printf(".l%d:\n", false_label);
        printf("  push 0\n"); // result is false
        printf(".l%d:\n", end_label);
        return;
    }
    default:
        // binary operators
        emit_expr(n->lhs);
        emit_expr(n->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");

        switch (n->kind) {
        case nd_add:
            // handle pointer arithmetic: if lhs is pointer, multiply rhs by base size
            if (n->lhs->type->kind == ty_ptr || n->lhs->type->kind == ty_array) {
                printf("  imul rdi, rdi, %d\n", n->lhs->type->base->size);
            }
            printf("  add rax, rdi\n");
            break;
        case nd_sub:
            // handle pointer arithmetic: if lhs is pointer, multiply rhs by base size
            if (n->lhs->type->kind == ty_ptr || n->lhs->type->kind == ty_array) {
                printf("  imul rdi, rdi, %d\n", n->lhs->type->base->size);
            }
            printf("  sub rax, rdi\n");
            break;
        case nd_mul:
            printf("  imul rax, rdi\n");
            break;
        case nd_div:
        case nd_mod:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            if (n->kind == nd_mod) printf("  mov rax, rdx\n");
            break;
        case nd_shl:
            printf("  mov rcx, rdi\n");
            printf("  sal rax, cl\n");
            break;
        case nd_shr:
            printf("  mov rcx, rdi\n");
            printf("  sar rax, cl\n");
            break;
        case nd_band:
            printf("  and rax, rdi\n");
            break;
        case nd_bor:
            printf("  or rax, rdi\n");
            break;
        case nd_bxor:
            printf("  xor rax, rdi\n");
            break;
        case nd_eq:
        case nd_ne:
        case nd_lt:
        case nd_le:
            printf("  cmp rax, rdi\n");
            if (n->kind == nd_eq) printf("  sete al\n");
            else if (n->kind == nd_ne) printf("  setne al\n");
            else if (n->kind == nd_lt) printf("  setl al\n");
            else if (n->kind == nd_le) printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        default:
            error_tok(current_token, "invalid expression node");
        }
        printf("  push rax\n");
        return;
    }
}

// recursively generate code for a statement
void emit_stmt(node *n) {
    if (!n) return;

    switch (n->kind) {
    case nd_return:
        emit_expr(n->lhs);
        printf("  pop rax\n");
        printf("  jmp .l%s_ret\n", current_func->name);
        return;
    case nd_if: {
        int else_label = gen_label();
        int end_label = gen_label();

        emit_expr(n->lhs); // condition
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", else_label);

        emit_stmt(n->rhs); // then
        printf("  jmp .l%d\n", end_label);

        printf(".l%d:\n", else_label);
        emit_stmt(n->els); // else

        printf(".l%d:\n", end_label);
        return;
    }
    case nd_while: {
        int loop_start = n->val; // continue label
        int break_label = n->els->val; // break label

        printf(".l%d:\n", loop_start); // loop start (continue target)

        emit_expr(n->lhs); // condition
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", break_label);

        emit_stmt(n->rhs); // body

        printf("  jmp .l%d\n", loop_start);
        printf(".l%d:\n", break_label);
        return;
    }
    case nd_for: {
        int loop_start = n->val; // continue label
        // FIX: cast n->type (ty*) back to node* to correctly access the val field (long break label)
        long break_label = ((node *)n->type)->val; // break label

        emit_stmt(n->lhs); // init

        printf(".l%ld:\n", loop_start); // loop start (continue target)

        emit_expr(n->rhs); // cond
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .l%ld\n", break_label);

        emit_stmt(n->els); // body

        emit_stmt(n->next); // inc
        printf("  jmp .l%ld\n", loop_start);
        printf(".l%ld:\n", break_label);
        return;
    }
    case nd_break: {
        printf("  jmp .l%ld\n", n->val);
        return;
    }
    case nd_continue: {
        printf("  jmp .l%ld\n", n->val);
        return;
    }
    case nd_switch: {
        int break_label = n->val;
        int default_label = break_label;

        emit_expr(n->lhs); // evaluate switch condition
        printf("  push rax\n"); // save switch value

        // first pass to find case matches and default label
        for (node *case_n = n->rhs; case_n; case_n = case_n->next) {
            case_n->val = gen_label(); // assign a unique label to each case body

            if (case_n->lhs) { // case
                printf("  pop rdi\n"); // restore switch value
                printf("  push rdi\n"); // save again

                emit_expr(case_n->lhs); // case value
                printf("  pop rdi\n"); // case value
                printf("  pop rax\n"); // switch value (temporarily)
                printf("  cmp rax, rdi\n");
                printf("  push rax\n"); // restore switch value

                // FIX: use %ld for long int node->val
                printf("  je .l%ld\n", case_n->val); // jump to case body label
            } else { // default
                default_label = case_n->val;
            }
        }

        printf("  pop rax\n"); // pop switch value
        printf("  jmp .l%d\n", default_label); // jump to default or break

        // emit case bodies (labels are in case_n->val)
        for (node *case_n = n->rhs; case_n; case_n = case_n->next) {
            // FIX: use %ld for long int node->val
            printf(".l%ld:\n", case_n->val);
            emit_stmt(case_n->rhs);
        }

        printf(".l%d:\n", break_label);
        return;
    }
    case nd_case:
        // cases are handled by the switch statement.
        // we reuse the nd_case node to hold a unique label (in val) for the jump target.
        // n->val = gen_label(); // label already generated in nd_switch
        return; // emit_stmt(n->rhs) is done in nd_switch
    case nd_block:
        for (node *cur = n->lhs; cur; cur = cur->next) {
            emit_stmt(cur);
        }
        return;
    case nd_expr_stmt:
        emit_expr(n->lhs);
        printf("  pop rax\n"); // discard result
        return;
    default:
        error_tok(current_token, "invalid statement node");
    }
}

// initializes global/static variables
void emit_data_section() {
    printf(".data\n");

    for (gvar *var = global_vars; var; var = var->next) {
        if (var->is_extern) continue; // skip extern declarations

        // static local variables have a unique label
        if (strncmp(var->name, ".lstatic", 8) == 0) {
            printf(".local %s\n", var->name);
        } else if (strncmp(var->name, ".lstr", 5) == 0) {
            // string literals are local
            printf(".local %s\n", var->name);
        } else {
            printf(".global %s\n", var->name);
        }

        printf("%s:\n", var->name);

        if (var->initializer) {
            sinit *init = var->initializer;
            if (init->kind == init_string) {
                // string literal definition
                printf("  .asciz \"%.*s\"\n", (int)strlen(init->str_content), init->str_content);
            } else if (init->kind == init_int) {
                // simple scalar initializer (simplified: assume 8-byte)
                printf("  .quad %ld\n", init->val);
            } else if (init->kind == init_array || init->kind == init_struct) {
                // array/struct initializer (simplified: flat list of quads)
                for (sinit *child = init->children; child; child = child->next) {
                    if (child->kind == init_int) {
                        printf("  .quad %ld\n", child->val);
                    } else {
                        fprintf(stderr, "unsupported initializer element in aggregate\n");
                        exit(1);
                    }
                }
            } else {
                fprintf(stderr, "unsupported initializer type in global variable\n");
                exit(1);
            }
        } else {
            // uninitialized global/static variable defaults to zero
            printf("  .zero %d\n", var->size);
        }
    }
}

// generates code for all functions
void codegen_program(function *funcs) {
    emit_data_section();

    printf(".text\n");

    for (function *func = funcs; func; func = func->next) {
        current_func = func;

        if (func->is_static) {
            printf(".local %s\n", func->name);
        } else {
            printf(".global %s\n", func->name);
        }
        printf("%s:\n", func->name);

        // prolog
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        printf("  sub rsp, %d\n", func->stack_size);

        // push arguments onto stack
        int i = 0;
        for (lvar *var = func->locals; var && i < func->num_params; var = var->next) {
            if (var->is_typedef || var->is_static) continue;
            if (i >= 6) { // arguments past the 6th are already on the stack
                // simplified: assuming arguments are passed in memory (not just registers)
                // for now, we only handle the first 6 args being pushed to stack
            } else {
                printf("  mov [rbp - %d], %s\n", var->offset, argreg[i]);
            }
            i++;
        }

        // body
        emit_stmt(func->body);

        // epilog
        printf(".l%s_ret:\n", func->name);
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
    }
}


// --- main driver ---

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: cc file.c [file2.c ...]\n");
        return 1;
    }

    // concatenate all input files after full preprocessing
    char *combined = NULL;
    size_t combined_len = 0;

    for (int i = 1; i < argc; i++) {
        char *pp = preprocess_file(argv[i], 0);

        if (!pp) {
            fprintf(stderr, "error: file not found or empty: %s\n", argv[i]);
            return 1;
        }

        if (!combined) {
            combined = pp;
            combined_len = strlen(combined);
        } else {
            size_t newlen = combined_len + strlen(pp) + 2;
            combined = realloc(combined, newlen);
            if (!combined) { perror("realloc"); exit(1); }
            // add a newline to separate files in the buffer
            combined[combined_len++] = '\n';
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

    // parse all functions and global variables
    while (current_token->kind != tk_eof) {
        parse_fundef_or_global();
    }

    // type-check the whole program
    for (function *func = program_head; func; func = func->next) {
        add_type(func->body);
    }

    // generate code
    printf(".intel_syntax noprefix\n");
    codegen_program(program_head);

    return 0;
}
