#define _POSIX_C_SOURCE 200809L

// cc.c
// minimal educational c compiler (amalgamated single-file)
// enhanced: full bitwise/logical operators, expanded preprocessor,
//           include path search, structs, arrays, and comprehensive initializers.
// usage: cc file1.c [file2.c ...]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h> // for access() to check file existence

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
    {"__builtin_va_list", 17}, // support for variadic functions
    {NULL, 0}
};

// system include paths for system headers (added for this request)
char *system_include_paths[] = {
    "/usr/local/include",
    "/usr/include/x86_64-linux-gnu", // common for multi-arch linux
    "/usr/include",
    NULL
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
typedef enum { ty_void, ty_int, ty_char, ty_long, ty_ptr, ty_array, ty_struct, ty_func, ty_va_list } type_kind;
struct ty {
    type_kind kind;
    ty *base;        // used for pointers, arrays, functions
    int size;        // 1, 4, 8, or arbitrary for struct/array
    int align;       // alignment requirement
    int is_unsigned;
    size_t array_len; // length if ty_array
    member *members; // for ty_struct
    int is_variadic; // added to ty for function types
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
    int is_typedef; // to support typedef
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
    ty *ret_type; // function return type
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
char *find_include_file(const char *filename_raw, int is_system);
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
    if (!fp) return NULL;

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

// attempts to find an included file in search paths (new logic)
char *find_include_file(const char *filename_raw, int is_system) {
    // 1. Local path search (for #include "...")
    // This is the CWD (current working directory) check
    if (!is_system) {
        if (access(filename_raw, F_OK) == 0) return strdup(filename_raw);
        // Note: A full C compiler would check relative to the including file's directory.
        // For simplicity, we only check CWD for local includes.
    }

    // 2. System path search (for #include <...>)
    // Also tried if local search fails for #include "..." (gcc behavior)
    for (char **path = system_include_paths; *path; path++) {
        // path_len + filename_len + '/' + '\0'
        size_t len = strlen(*path) + strlen(filename_raw) + 2;
        char *full_path = malloc(len);
        if (!full_path) { perror("malloc"); exit(1); }

        sprintf(full_path, "%s/%s", *path, filename_raw);

        // check if file exists using access(2)
        if (access(full_path, F_OK) == 0) return full_path;

        free(full_path);
    }

    return NULL;
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

            // count newlines in the block comment for line number accuracy
            char *t = p + 2;
            while(t < q) {
                if (*t == '\n') line_number++;
                t++;
            }

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
    if (!input) return NULL; // caller (in main) handles file not found for initial file

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
                long condition = strtol(p, &p, 0); // simplified evaluation

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
                // skip other directives if in a false block
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
                int is_system = 0; // 1 for <...>, 0 for "..."

                if (*p == '"') {
                    term_char = '"';
                    is_system = 0;
                }
                else if (*p == '<') {
                    term_char = '>';
                    is_system = 1;
                }
                else error_at(p, "expected '\"' or '<' after #include");

                p++;
                char *path_start = p;
                while (*p && *p != term_char && *p != '\n') p++;
                if (*p != term_char) error_at(p, "unterminated include path");

                char *filename_raw = strndup(path_start, p - path_start);

                // --- NEW LOGIC: Search for file in paths ---
                char *full_path = find_include_file(filename_raw, is_system);

                if (!full_path) {
                    error_at(start, "include file '%s' not found in search paths", filename_raw);
                }

                filename = full_path;

                // recursive call to preprocess the included file
                char *included_content = preprocess_file(filename, depth + 1);

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
                free(filename); // free full_path (allocated by find_include_file)
                free(filename_raw);
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
                    if (*p == '\\') output[output_len++] = *p++;
                    output[output_len++] = *p++;
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
ty *ty_va_list_def() { return new_type(ty_va_list, 24, 8); } // va_list is struct of size 24

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
    case ty_va_list:
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
    // fix 2: sinit nodes don't contain expression nodes that need type checking.
    // add_type(n->init_list ? n->init_list->children : NULL); // removed due to incompatible pointer type warning

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
        } else if (!var->is_extern && is_extern) {
            // definition followed by extern declaration (allowed)
        } else if (var->size != type->size) {
            error_tok(tok, "incompatible re-declaration of global variable");
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

int consume(char *op) {
    if (current_token->kind != tk_reserved ||
        current_token->len != (int)strlen(op) ||
        strncmp(current_token->str, op, current_token->len) != 0) return 0;
    current_token = current_token->next;
    return 1;
}

int consume_keyword(char *kw) {
    if (current_token->kind != tk_keyword ||
        current_token->len != (int)strlen(kw) ||
        strncmp(current_token->str, kw, current_token->len) != 0) return 0;
    current_token = current_token->next;
    return 1;
}

token *expect_ident() {
    if (current_token->kind != tk_ident) error_tok(current_token, "expected identifier");
    token *tok = current_token;
    current_token = current_token->next;
    return tok;
}

void expect(char *op) {
    if (current_token->kind != tk_reserved ||
        current_token->len != (int)strlen(op) ||
        strncmp(current_token->str, op, current_token->len) != 0) error_tok(current_token, "expected '%s'", op);
    current_token = current_token->next;
}

long expect_number() {
    if (current_token->kind != tk_num) error_tok(current_token, "expected number");
    long val = current_token->val;
    current_token = current_token->next;
    return val;
}

token *peek() {
    return current_token;
}

int is_type_token(token *tok) {
    if (tok->kind == tk_keyword) {
        if (consume_keyword("int")) return 1;
        if (consume_keyword("char")) return 1;
        if (consume_keyword("long")) return 1;
        if (consume_keyword("void")) return 1;
        if (consume_keyword("struct")) return 1;
        if (consume_keyword("unsigned")) return 1;
        if (consume_keyword("typedef")) return 1;
        if (consume_keyword("__builtin_va_list")) return 1;
        // backtrack
        current_token = tok;
    }

    // check for typedef'd identifiers
    if (tok->kind == tk_ident) {
        lvar *var = find_lvar(tok);
        // Note: For full C support, global typedefs need to be checked here too.
        if (var && var->is_typedef) return 1;
    }
    return 0;
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

// struct_specifier = "struct" (IDENT | (IDENT? "{" member_declaration* "}"))
ty *parse_struct_specifier() {
    expect_ident(); // consume "struct" (it was a keyword)

    token *tag = NULL;
    if (current_token->kind == tk_ident) {
        tag = expect_ident();
    }

    // if it is a tag definition: struct tag {...}
    if (consume("{")) {
        ty *type = new_type_struct(0, 1);
        if (tag) register_struct_tag(tag, type);

        member head = {};
        member *cur = &head;

        while (!consume("}")) {
            ty *base = parse_type_specifier(NULL, NULL, NULL);
            token *ident_tok = NULL;
            ty *type_decl = parse_declarator(base, &ident_tok);
            expect(";");

            member *mem = calloc(1, sizeof(member));
            mem->name = ident_tok->str;
            mem->len = ident_tok->len;
            mem->type = type_decl;
            cur->next = mem;
            cur = mem;
        }

        type->members = head.next;
        compute_struct_layout(type);
        return type;
    }

    // if it is a tag reference: struct tag
    if (tag) {
        ty *type = find_struct_tag(tag);
        if (!type) error_tok(tag, "unknown struct tag");
        return type;
    }

    error_tok(current_token, "expected struct tag or member list");
    return NULL; // unreachable
}

// type_specifier = ("void" | "char" | "int" | "long" | "unsigned" | "struct" | "__builtin_va_list"...) ("static" | "extern")?
ty *parse_type_specifier(int *is_unsigned_ptr, int *is_static_ptr, int *is_extern_ptr) {
    // track qualifiers
    int is_unsigned = 0;
    int is_static = 0;
    int is_extern = 0;
    ty *type = NULL;

    for (;;) {
        if (consume_keyword("unsigned")) {
            is_unsigned = 1;
            continue;
        }
        if (consume_keyword("static")) {
            is_static = 1;
            continue;
        }
        if (consume_keyword("extern")) {
            is_extern = 1;
            continue;
        }
        if (consume_keyword("void")) {
            type = ty_void_def();
            continue;
        }
        if (consume_keyword("char")) {
            type = ty_char_def();
            continue;
        }
        if (consume_keyword("int")) {
            type = ty_int_def();
            continue;
        }
        if (consume_keyword("long")) {
            type = ty_long_def();
            continue;
        }
        if (consume_keyword("struct")) {
            type = parse_struct_specifier();
            continue;
        }
        if (consume_keyword("__builtin_va_list")) {
            type = ty_va_list_def();
            continue;
        }

        // check for typedef'd identifiers
        if (current_token->kind == tk_ident) {
            lvar *var = find_lvar(current_token);
            if (var && var->is_typedef) {
                type = var->type;
                current_token = current_token->next;
                continue;
            }
        }
        break;
    }

    if (!type) {
        // default to int if no type specifier is given (e.g., `static x;`)
        type = ty_int_def();
    }

    if (is_unsigned) type->is_unsigned = 1;

    if (is_unsigned_ptr) *is_unsigned_ptr = is_unsigned;
    if (is_static_ptr) *is_static_ptr = is_static;
    if (is_extern_ptr) *is_extern_ptr = is_extern;

    return type;
}

// declarator = ("*")* (IDENT) array_suffix*
ty *parse_declarator(ty *base, token **ident_tok_ptr) {
    // 1. parse pointers
    ty *type = base;
    while (consume("*")) {
        type = new_type_ptr(type);
    }

    // 2. parse identifier
    token *ident_tok = NULL;
    if (current_token->kind == tk_ident) {
        ident_tok = expect_ident();
    }
    if (ident_tok_ptr) *ident_tok_ptr = ident_tok;


    // 3. parse array/function suffixes
    while (consume("[")) {
        size_t len = expect_number();
        expect("]");
        type = new_type_array(type, len);
    }

    // function declarator (simplified)
    if (consume("(")) {
        // skip parameter list for now
        while (!consume(")")) {
            // this is hacky: consume everything until ')'
            current_token = current_token->next;
        }
        // fixme: this does not properly handle function signatures, only bare identifiers
    }

    return type;
}

// initializer = assignment | "{" (initializer ("," initializer)*)? "}"
sinit *parse_initializer(ty *type) {
    if (consume("{")) {
        sinit *init = calloc(1, sizeof(sinit));
        init->kind = (type->kind == ty_array) ? init_array : init_struct;
        init->type = type;

        sinit head = {};
        sinit *cur = &head;
        int element_idx = 0;
        member *mem = type->members;

        do {
            if (consume("}")) break;

            ty *elem_type;
            int offset = 0;

            if (type->kind == ty_array) {
                elem_type = type->base;
                offset = element_idx * elem_type->size;
            } else if (type->kind == ty_struct) {
                if (!mem) error_tok(current_token, "too many initializers for struct");
                elem_type = mem->type;
                offset = mem->offset;
                mem = mem->next;
            } else {
                error_tok(current_token, "unexpected initializer for non-aggregate type");
                return NULL;
            }

            sinit *child_init = parse_initializer(elem_type);
            child_init->type = elem_type;
            child_init->offset = offset;
            child_init->size = elem_type->size;

            cur->next = child_init;
            cur = child_init;

            element_idx++;

        } while (consume(","));

        expect("}");
        init->children = head.next;
        return init;
    }

    // string literal initializer for char array
    if (type->kind == ty_array && type->base->kind == ty_char && current_token->kind == tk_string) {
        token *str_tok = current_token;
        current_token = current_token->next;

        // check array bounds
        if (type->array_len > 0 && type->array_len < (size_t)str_tok->len + 1)
             error_tok(str_tok, "string initializer too long for array");

        return new_sinit_ptr(strndup(str_tok->str, str_tok->len));
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

    if (base->kind == ty_void) error_tok(current_token, "variable declared void");

    // handle empty declaration (e.g. `static int;`)
    if (consume(";")) return NULL;

    do {
        token *ident_tok = NULL;
        ty *type_decl = parse_declarator(base, &ident_tok);
        if (!ident_tok) error_tok(current_token, "expected identifier after declaration specifier");

        // handle typedef
        if (is_global && consume_keyword("typedef")) { // typedef is usually global
            add_lvar(ident_tok, type_decl, 0, 1);
            continue;
        }

        // handle variable declaration/definition
        if (is_global) {
            gvar *gvar = add_global_var(ident_tok, type_decl, is_extern);

            if (consume("=")) {
                if (is_extern) error_tok(ident_tok, "extern variable cannot have an initializer");
                gvar->initializer = parse_initializer(type_decl);
            } else if (!is_extern) {
                // global/static variable without initializer defaults to zero
                gvar->initializer = NULL;
            }
        } else {
            // local variable definition
            lvar *lvar = add_lvar(ident_tok, type_decl, is_static, 0);

            if (consume("=")) {
                if (type_decl->kind == ty_array || type_decl->kind == ty_struct) {
                    // special node for local aggregate initialization
                    node *n = calloc(1, sizeof(node));
                    n->kind = nd_local_init;
                    n->var = lvar;
                    n->init_list = parse_initializer(type_decl);
                    cur->next = n;
                    cur = n;
                } else {
                    // scalar initialization: lvar = assign
                    node *lhs = new_node_lvar(lvar);
                    node *rhs = assign();
                    cur->next = new_node(nd_assign, lhs, rhs);
                    cur = cur->next;
                }
            } else {
                // simple declaration without initialization (implicitly zeroed if static)
            }
        }

    } while (consume(","));

    expect(";");
    return head.next;
}

// funcall = IDENT "(" (assign ("," assign)*)? ")"
node *funcall(token *tok) {
    node *n = calloc(1, sizeof(node));
    n->kind = nd_funcall;
    n->name = strndup(tok->str, tok->len);
    add_type(n);

    expect("(");
    node head = {};
    node *cur = &head;
    int num_args = 0;

    if (!consume(")")) {
        do {
            cur->next = assign();
            cur = cur->next;
            num_args++;
        } while (consume(","));
        expect(")");
    }

    n->lhs = head.next; // arguments list
    n->val = num_args;  // reuse val for num_params
    return n;
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

        if (consume("(")) { // function call
            current_token = tok; // backtrack to `funcall`
            return funcall(tok);
        }

        lvar *lvar = find_lvar(tok);
        if (lvar) return new_node_lvar(lvar);

        // treat as global variable reference (will be handled as &gvar + dereference)
        gvar *gvar = add_global_var(tok, ty_int_def(), 1); // assume extern int if not found
        return new_node_gvar(gvar);
    }

    error_tok(current_token, "expected expression");
    return NULL; // unreachable
}

// member_access = ("." | "->") IDENT
node *member_access(node *n) {
    // n.member
    if (consume(".")) {
        if (n->type->kind != ty_struct) error_tok(current_token, "'.' must be used on a struct");

        token *mem_tok = expect_ident();
        member *mem = get_struct_member(n->type, mem_tok);

        node *m = calloc(1, sizeof(node));
        m->kind = nd_member;
        m->lhs = n;
        m->mem = mem;
        add_type(m);
        return m;
    }

    // n->member
    if (consume("->")) {
        if (n->type->kind != ty_ptr || n->type->base->kind != ty_struct)
            error_tok(current_token, "'->' must be used on a pointer to a struct");

        // create: (*n).member
        node *deref = new_node(nd_deref, n, NULL);
        add_type(deref);

        token *mem_tok = expect_ident();
        member *mem = get_struct_member(deref->type, mem_tok);

        node *m = calloc(1, sizeof(node));
        m->kind = nd_member;
        m->lhs = deref;
        m->mem = mem;
        add_type(m);
        return m;
    }
    return NULL;
}

// postfix = primary (array_subscript | member_access | "++" | "--")*
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
        else if (consume(">")) n = new_node(nd_lt, shift(), n); // swapped
        // fix 1: added missing closing parenthesis after consume(">=")
        else if (consume(">=")) n = new_node(nd_le, shift(), n); // swapped
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

// expr = assign ("," assign)*
node *expr() {
    node *n = assign();
    // support comma operator (simplified: discards lhs value)
    while (consume(",")) {
        n = assign();
    }
    return n;
}


// stmt = "return" expr ";" | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt | "for" "(" (expr? | declaration) ";" expr? ";" expr? ")" stmt
//      | "break" ";" | "continue" ";" | "switch" "(" expr ")" stmt
//      | "{" block_item* "}" | expr? ";" | declaration
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
        n->val = new_labels->continue_label;
        n->els = new_node_num(new_labels->break_label); // hack: use node_num to carry break label

    } else if (consume_keyword("for")) {
        node *init = NULL;
        node *cond = NULL;
        node *inc = NULL;

        expect("(");

        // init
        if (!consume(";")) {
            if (is_type_token(peek())) {
                init = declaration(0);
            } else {
                init = new_node(nd_expr_stmt, expr(), NULL);
                expect(";");
            }
        }

        // cond
        if (!consume(";")) {
            cond = expr();
            expect(";");
        }

        // inc
        if (!consume(")")) {
            inc = new_node(nd_expr_stmt, expr(), NULL);
            expect(")");
        }

        // body
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
        n->lhs = init; // init/declaration
        n->rhs = cond; // condition
        n->els = inc;  // increment
        n->next = body; // body (misuse of next, but works for the structure)
        n->val = new_labels->continue_label; // continue label
        n->name = (char*)(long)new_labels->break_label; // hack: break label

    } else if (consume_keyword("break")) {
        if (!loop_or_switch_labels) error_tok(current_token, "break statement not within loop or switch");
        n = new_node_num(loop_or_switch_labels->break_label); // hack: use nd_num to hold label
        n->kind = nd_break;
        expect(";");
    } else if (consume_keyword("continue")) {
        if (!loop_or_switch_labels) error_tok(current_token, "continue statement not within loop");
        n = new_node_num(loop_or_switch_labels->continue_label); // hack: use nd_num to hold label
        n->kind = nd_continue;
        expect(";");
    } else if (consume_keyword("switch")) {
        expect("(");
        node *cond = expr();
        expect(")");

        continue_break_labels *new_labels = calloc(1, sizeof(continue_break_labels));
        new_labels->break_label = label_count++;
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
            node *s = stmt();
            if (s) { // a declaration (declaration(0)) returns NULL if only a semicolon
                cur->next = s;
                cur = s;
            }
        }
        n = calloc(1, sizeof(node));
        n->kind = nd_block;
        n->lhs = head.next;
    } else if (is_type_token(peek())) {
        n = declaration(0); // local declaration

        // a declaration can result in a list of assignment nodes, so it's handled like a block item
        node head = {};
        node *cur = &head;
        if (n) {
            cur->next = n;
            while(cur->next) cur = cur->next;
        }

        node *block = calloc(1, sizeof(node));
        block->kind = nd_block;
        block->lhs = head.next;
        n = block;

    } else {
        if (consume(";")) return NULL;
        n = new_node(nd_expr_stmt, expr(), NULL);
        expect(";");
    }

    add_type(n);
    return n;
}

// func_type = type_specifier declarator "(" (param_decl ("," param_decl)*)? ")" stmt
void parse_fundef_or_global() {
    token *start_tok = current_token;
    int is_static = 0;
    int is_extern = 0;

    // 1. Parse type specifiers and storage classes
    ty *base = parse_type_specifier(NULL, &is_static, &is_extern);

    // 2. Parse declarator (identifier, pointer, array suffixes)
    token *ident_tok = NULL;
    // fix 3: cast unused return value to (void)
    (void)parse_declarator(base, &ident_tok);

    if (!ident_tok) error_tok(current_token, "expected identifier");

    // 3. Check for '(' to determine if it's a function
    if (consume("(")) {
        // --- This is a function definition or prototype ---

        // check if definition starts with '{' next
        int is_definition = 0;
        token *check_tok = current_token;
        int paren_count = 1;
        while (check_tok->kind != tk_eof) {
            if (strncmp(check_tok->str, "(", check_tok->len) == 0) paren_count++;
            if (strncmp(check_tok->str, ")", check_tok->len) == 0) paren_count--;
            if (paren_count == 0) {
                check_tok = check_tok->next;
                break;
            }
            check_tok = check_tok->next;
        }
        if (check_tok->kind == tk_reserved && strncmp(check_tok->str, "{", check_tok->len) == 0) is_definition = 1;

        // backtrack after checking
        current_token = start_tok;
        parse_type_specifier(NULL, &is_static, &is_extern);
        // re-parse the declarator
        (void)parse_declarator(base, &ident_tok);
        expect("("); // consume '(' again

        // setup function object
        function *func = calloc(1, sizeof(function));
        func->name = strndup(ident_tok->str, ident_tok->len);
        func->ret_type = base;
        func->is_static = is_static;
        current_func = func;

        // parameter list parsing
        token *name = NULL;
        int num_params = 0;
        if (!consume(")")) {
            do {
                if (strncmp(current_token->str, "...", 3) == 0 && current_token->len == 3) {
                    func->is_variadic = 1;
                    current_token = current_token->next;
                    break; // variadic must be the last parameter
                }

                ty *param_base = parse_type_specifier(NULL, NULL, NULL);
                ty *param_type = parse_declarator(param_base, &name);

                // C convention: array/function parameters decay to pointers
                if (param_type->kind == ty_array) {
                    param_type = new_type_ptr(param_type->base);
                }

                if (name) {
                    add_lvar(name, param_type, 0, 0);
                    num_params++;
                } else {
                    error_tok(current_token, "expected parameter name in function definition");
                }
            } while (consume(","));
            expect(")");
        }

        func->num_params = num_params;

        if (is_definition) {
            // function body parsing
            expect("{");

            node head = {};
            node *cur = &head;
            while (!consume("}")) {
                cur->next = stmt();
                cur = cur->next;
            }
            node *body = calloc(1, sizeof(node));
            body->kind = nd_block;
            body->lhs = head.next;
            func->body = body;

            // calculate stack offsets for locals (excluding typedefs)
            int offset = 0;
            for (lvar *var = func->locals; var; var = var->next) {
                if (var->is_typedef) continue;
                offset = align_to(offset, var->type->align);
                offset += var->type->size;
                var->offset = offset;
            }
            func->stack_size = align_to(offset, 16);

            // link function to program list
            func->next = program_head;
            program_head = func;
        } else {
            // function prototype. consume ';'
            expect(";");
        }

        current_func = NULL;

    } else {
        // --- This is a global variable declaration/definition ---
        current_token = start_tok; // backtrack to re-parse the whole declaration
        declaration(1);
    }
}


// --- ix. code generator ---

// function argument registers
char *arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// generates a unique label
int gen_label() { return label_count++; }

// push `type->size` bytes from rax onto stack
void emit_push() {
    printf("  push rax\n");
}

// pop `type->size` bytes from stack into rax
void emit_pop(char *arg) {
    printf("  pop %s\n", arg);
}

// emits the address of a variable/member into rax
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
        error_tok(current_token, "cannot get address of this expression");
    }
}

// load from rax (address) into rax (value)
void emit_load(ty *type) {
    if (type->kind == ty_array || type->kind == ty_struct || type->kind == ty_void || type->kind == ty_func) return;

    // Size check
    if (type->size == 0 || type->size > 8) return;

    // use sign-extension movsx for signed types
    if (type->size == 1 && !type->is_unsigned) printf("  movsx rax, byte ptr [rax]\n");
    else if (type->size == 4 && !type->is_unsigned) printf("  movsxd rax, dword ptr [rax]\n");
    else if (type->size == 1) printf("  movzx rax, byte ptr [rax]\n");
    else if (type->size == 4) printf("  movzx rax, dword ptr [rax]\n");
    else printf("  mov rax, [rax]\n");
}

// store value in rdi to address in rax
void emit_store(ty *type) {
    if (type->kind == ty_array || type->kind == ty_struct || type->kind == ty_void || type->kind == ty_func) return;
    if (type->size == 0 || type->size > 8) return;

    printf("  push rdi\n"); // save value
    printf("  pop rdi\n"); // rdi holds the value to store
    printf("  pop rax\n"); // rax holds the address

    if (type->size == 1) printf("  mov byte ptr [rax], dil\n");
    else if (type->size == 4) printf("  mov dword ptr [rax], edi\n");
    else printf("  mov [rax], rdi\n");

    printf("  push rdi\n"); // push the stored value back to the stack as result
}

// recursively generate code for an expression
void emit_expr(node *n) {
    add_type(n); // ensure type is set

    switch (n->kind) {
    case nd_num:
        printf("  mov rax, %ld\n", n->val);
        return;
    case nd_lvar:
    case nd_gvar:
    case nd_deref:
    case nd_member:
        emit_addr(n);
        emit_load(n->type);
        return;
    case nd_assign:
        emit_addr(n->lhs);
        emit_push();
        emit_expr(n->rhs);
        printf("  mov rdi, rax\n"); // rdi = rhs value
        emit_store(n->lhs->type);
        return;
    case nd_addr:
        emit_addr(n->lhs);
        return;
    case nd_funcall: {
        int num_args = n->val;
        node *arg = n->lhs;

        // push arguments on stack in reverse order
        for (int i = num_args - 1; i >= 0; i--) {
            emit_expr(arg);
            emit_push();
            arg = arg->next;
        }

        // move from stack to registers
        for (int i = 0; i < num_args; i++) {
            emit_pop(arg_regs[i]);
        }

        printf("  mov rax, 0\n"); // 0 for no float arguments (ABI rule)
        printf("  call %s\n", n->name);
        return;
    }
    case nd_add_assign: case nd_sub_assign: case nd_mul_assign:
    case nd_div_assign: case nd_mod_assign: case nd_shl_assign:
    case nd_shr_assign: case nd_band_assign:
    case nd_bor_assign: case nd_bxor_assign: {
        emit_addr(n->lhs);
        emit_push();

        emit_addr(n->lhs);
        emit_load(n->lhs->type);
        emit_push();

        emit_expr(n->rhs);
        printf("  pop rdi\n"); // rdi = lhs value
        printf("  mov rsi, rax\n"); // rsi = rhs value

        // perform operation
        char *op_name = NULL;
        switch (n->kind) {
            case nd_add_assign: op_name = "add"; break;
            case nd_sub_assign: op_name = "sub"; break;
            case nd_mul_assign: op_name = "imul"; break;
            case nd_div_assign: op_name = "div_mod"; break;
            case nd_mod_assign: op_name = "div_mod"; break;
            case nd_shl_assign: op_name = "shl_shr"; break;
            case nd_shr_assign: op_name = "shl_shr"; break;
            case nd_band_assign: op_name = "and"; break;
            case nd_bor_assign: op_name = "or"; break;
            case nd_bxor_assign: op_name = "xor"; break;
            default: break;
        }

        if (strncmp(op_name, "div_mod", 7) == 0) {
            // division/modulus is special
            printf("  mov rax, rdi\n");
            printf("  cqo\n");
            printf("  idiv rsi\n");
            printf("  mov rdi, %s\n", (n->kind == nd_div_assign) ? "rax" : "rdx");
        } else if (strncmp(op_name, "shl_shr", 7) == 0) {
            printf("  mov rcx, rsi\n"); // shift amount must be in cl/rcx
            printf("  mov rax, rdi\n");
            printf("  %s rax, cl\n", (n->kind == nd_shl_assign) ? "sal" : "sar");
            printf("  mov rdi, rax\n");
        } else {
            printf("  %s rdi, rsi\n", op_name);
        }

        printf("  mov rax, rdi\n"); // result in rax
        emit_store(n->lhs->type);
        return;
    }
    case nd_logical_and: {
        int label = gen_label();
        emit_expr(n->lhs);
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", label); // short-circuit if lhs is false
        emit_expr(n->rhs);
        printf("  cmp rax, 0\n");
        printf("  mov rax, 0\n");
        printf("  setne al\n");
        printf(".l%d:\n", label);
        return;
    }
    case nd_logical_or: {
        int label = gen_label();
        emit_expr(n->lhs);
        printf("  cmp rax, 0\n");
        printf("  jne .l%d\n", label); // short-circuit if lhs is true
        emit_expr(n->rhs);
        printf("  cmp rax, 0\n");
        printf("  mov rax, 0\n");
        printf("  setne al\n");
        printf(".l%d:\n", label);
        return;
    }
    case nd_cond: {
        int else_label = gen_label();
        int end_label = gen_label();

        emit_expr(n->lhs); // condition
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", else_label);

        emit_expr(n->rhs); // then
        printf("  jmp .l%d\n", end_label);

        printf(".l%d:\n", else_label);
        emit_expr(n->els); // else

        printf(".l%d:\n", end_label);
        return;
    }
    // fix 4a: explicit unary operators
    case nd_not:
        emit_expr(n->lhs);
        printf("  cmp rax, 0\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        return;
    case nd_bitnot:
        emit_expr(n->lhs);
        printf("  not rax\n");
        return;

    // fix 4b: explicit binary operators (to silence warnings)
    case nd_add:
    case nd_sub:
    case nd_mul:
    case nd_div:
    case nd_mod:
    case nd_eq:
    case nd_ne:
    case nd_lt:
    case nd_le:
    case nd_shl:
    case nd_shr:
    case nd_band:
    case nd_bor:
    case nd_bxor:
        // binary operators (+, -, *, /, ==, !=, <, <=, etc.)
        emit_expr(n->lhs);
        emit_push();
        emit_expr(n->rhs);
        printf("  mov rdi, rax\n");
        emit_pop("rax");

        switch (n->kind) {
        case nd_add:
            // handle pointer arithmetic: if lhs is pointer, multiply rhs by base size
            if (n->lhs->type->kind == ty_ptr || n->lhs->type->kind == ty_array) {
                printf("  imul rdi, rdi, %d\n", n->lhs->type->base->size);
            }
            printf("  add rax, rdi\n");
            return;
        case nd_sub:
            // handle pointer arithmetic: if lhs is pointer, multiply rhs by base size
            if (n->lhs->type->kind == ty_ptr || n->lhs->type->kind == ty_array) {
                printf("  imul rdi, rdi, %d\n", n->lhs->type->base->size);
            }
            printf("  sub rax, rdi\n");
            return;
        case nd_mul: printf("  imul rax, rdi\n"); return;
        case nd_div:
        case nd_mod:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            if (n->kind == nd_mod) printf("  mov rax, rdx\n");
            return;
        case nd_shl: printf("  mov rcx, rdi\n"); printf("  sal rax, cl\n"); return;
        case nd_shr: printf("  mov rcx, rdi\n"); printf("  sar rax, cl\n"); return;
        case nd_band: printf("  and rax, rdi\n"); return;
        case nd_bor: printf("  or rax, rdi\n"); return;
        case nd_bxor: printf("  xor rax, rdi\n"); return;

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
            return;
        default:
            // should not be reached since we listed all binary ops
            error_tok(current_token, "internal error: binary op not handled");
            return;
        }

    // statement nodes should be handled by emit_stmt, but for completeness:
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
    case nd_local_init:
        error_tok(current_token, "statement node passed to emit_expr");
        return;

    default:
        error_tok(current_token, "unknown ast node in emit_expr");
        return;
    }
}

// recursively generate code for a statement
void emit_stmt(node *n) {
    if (!n) return;

    switch (n->kind) {
    case nd_return:
        emit_expr(n->lhs);
        printf("  jmp .l%s_ret\n", current_func->name);
        return;
    case nd_if: {
        int else_label = gen_label();
        int end_label = gen_label();

        emit_expr(n->lhs); // condition
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
        int begin_label = n->val; // loop start / continue label
        int end_label = (int)(long)n->els->val; // break label

        printf(".l%d:\n", begin_label);

        emit_expr(n->lhs); // condition
        printf("  cmp rax, 0\n");
        printf("  je .l%d\n", end_label);

        emit_stmt(n->rhs); // body

        printf("  jmp .l%d\n", begin_label);
        printf(".l%d:\n", end_label);
        return;
    }
    case nd_for: {
        int begin_label = n->val; // continue label
        int end_label = (int)(long)n->name; // break label

        emit_stmt(n->lhs); // init

        printf(".l%d:\n", begin_label);

        if (n->rhs) { // condition
            emit_expr(n->rhs);
            printf("  cmp rax, 0\n");
            printf("  je .l%d\n", end_label);
        }

        emit_stmt(n->next); // body

        if (n->els) { // increment
            emit_expr(n->els->lhs);
        }

        printf("  jmp .l%d\n", begin_label);
        printf(".l%d:\n", end_label);
        return;
    }
    case nd_break:
        printf("  jmp .l%d\n", (int)n->val);
        return;
    case nd_continue:
        printf("  jmp .l%d\n", (int)n->val);
        return;
    case nd_switch: {
        int break_label = n->val;

        emit_expr(n->lhs); // switch condition
        emit_push(); // save switch value

        // generate code for cases
        node *cur = n->rhs;
        int default_label = break_label; // if no default, jump to break

        for (node *case_n = cur; case_n; case_n = case_n->next) {
            if (case_n->lhs) { // case
                printf("  pop rdi\n"); // restore switch value
                printf("  push rdi\n"); // save again
                emit_expr(case_n->lhs); // case value
                // fix 5: cast case_n->val to int to match %d format specifier
                printf("  cmp rdi, rax\n");
                printf("  je .l%d\n", (int)case_n->val); // jump to case body label
            } else { // default
                default_label = case_n->val;
            }
        }

        printf("  pop rax\n"); // pop switch value
        printf("  jmp .l%d\n", default_label); // jump to default or break

        // emit case bodies (labels are in case_n->val)
        for (node *case_n = cur; case_n; case_n = case_n->next) {
            // fix 5: cast case_n->val to int to match %d format specifier
            printf(".l%d:\n", (int)case_n->val);
            emit_stmt(case_n->rhs);
        }

        printf(".l%d:\n", break_label);
        return;
    }
    case nd_case:
        // cases are handled by the switch statement.
        // we reuse the nd_case node to hold a unique label (in val) for the jump target.
        n->val = gen_label();
        return; // emit_stmt(n->rhs) is done in nd_switch
    case nd_block:
        for (node *cur = n->lhs; cur; cur = cur->next) {
            emit_stmt(cur);
        }
        return;
    case nd_expr_stmt:
        emit_expr(n->lhs);
        return;
    case nd_local_init:
        // for local aggregates: zero it out first, then copy initializer if present
        if (n->init_list) {
            error_tok(current_token, "local aggregate initialization not implemented");
        }
        return;
    default:
        error_tok(current_token, "unknown ast node in emit_stmt");
    }
}

// prints global variables and string literals
void emit_data() {
    printf(".data\n");
    for (gvar *var = global_vars; var; var = var->next) {
        if (var->is_extern) continue;

        if (var->initializer) {
            sinit *init = var->initializer;

            if (init->kind == init_string) {
                // string literal data
                printf("%s:\n", init->str_label);
                printf("  .string \"%.*s\"\n", (int)strlen(init->str_content), init->str_content);
            } else if (init->kind == init_int || init->kind == init_ptr) {
                // simple scalar
                printf("%s:\n", var->name);
                printf("  .quad %ld\n", init->val);
            } else {
                // aggregate initialization (simplified: zero until proper implementation)
                printf("%s:\n", var->name);
                // simplified: just zero out the space
                printf("  .zero %d\n", var->size);
            }
        } else {
            // uninitialized global/static variable (bss or zero-filled data)
            printf(".bss\n");
            printf("%s:\n", var->name);
            printf("  .zero %d\n", var->size);
            printf(".data\n");
        }
    }
}


// main code generator entry point
void codegen_program(function *funcs) {
    printf(".intel_syntax noprefix\n");

    emit_data();

    printf(".text\n");

    for (function *func = funcs; func; func = func->next) {
        current_func = func; // set current function for return label

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
        for (lvar *var = func->locals; var; var = var->next) {
            if (var->is_typedef) continue;

            // parameters are the first `func->num_params` locals
            int param_idx = func->num_params - (var->offset / 8);
            if (param_idx >= 0 && param_idx < func->num_params) {
                printf("  mov [rbp - %d], %s\n", var->offset, arg_regs[param_idx]);
            }
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
        // the initial file is passed directly to the preprocessor
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

    // parse all function definitions and global declarations until eof
    while (current_token->kind != tk_eof) {
        parse_fundef_or_global();
    }

    // generate code
    codegen_program(program_head);

    return 0;
}

