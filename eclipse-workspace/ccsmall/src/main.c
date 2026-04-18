// tinycc_selfhost.c
// Tiny C-like compiler with preprocessor and token linked list
// Simplified, intended as a self-hosting-capable toy compiler.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

/* ---------------------------
   Basic utilities and errors
   --------------------------- */
void fatal(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}

/* ---------------------------
   Preprocessor structures
   --------------------------- */

typedef struct macro {
    char name[128];
    char replacement[1024];
    struct macro* next;
} macro;

typedef struct filebuf {
    char* data;
    size_t len;
    char* path;
} filebuf;

/* Read entire file into memory */
filebuf read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if(!f) fatal("cannot open %s", path);
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc(sz + 1);
    if(!buf) fatal("out of memory");
    if(fread(buf, 1, sz, f) != (size_t)sz) fatal("read error %s", path);
    buf[sz] = '\0';
    fclose(f);
    filebuf fb = { buf, (size_t)sz, strdup(path) };
    return fb;
}

/* Simple macro table */
macro* macro_table = NULL;

void define_macro(const char* name, const char* repl) {
    macro* m = malloc(sizeof(macro));
    strncpy(m->name, name, sizeof(m->name)-1);
    strncpy(m->replacement, repl ? repl : "", sizeof(m->replacement)-1);
    m->next = macro_table;
    macro_table = m;
}

void undef_macro(const char* name) {
    macro** p = &macro_table;
    while(*p) {
        if(strcmp((*p)->name, name) == 0) {
            macro* rem = *p;
            *p = rem->next;
            free(rem);
            return;
        }
        p = &(*p)->next;
    }
}

const char* lookup_macro(const char* name) {
    for(macro* m = macro_table; m; m = m->next) {
        if(strcmp(m->name, name) == 0) return m->replacement;
    }
    return NULL;
}

/* ---------------------------
   Preprocessor: inline includes and macros
   --------------------------- */

/* Append string to dynamic buffer */
typedef struct {
    char* data;
    size_t len;
    size_t cap;
} dynbuf;

void db_init(dynbuf* db) { db->data = NULL; db->len = db->cap = 0; }
void db_append_char(dynbuf* db, char c) {
    if(db->len + 1 >= db->cap) {
        db->cap = db->cap ? db->cap * 2 : 4096;
        db->data = realloc(db->data, db->cap);
    }
    db->data[db->len++] = c;
    db->data[db->len] = '\0';
}
void db_append_str(dynbuf* db, const char* s) {
    while(*s) db_append_char(db, *s++);
}

/* Very simple tokenization for preprocessor directives */
static const char* skip_ws(const char* p) {
    while(*p && (*p == ' ' || *p == '\t')) p++;
    return p;
}

/* Process a single file buffer, handling includes and directives.
   This is a simple recursive inliner. */
void preprocess_filebuf(filebuf fb, dynbuf* out, const char* base_dir);

char* join_path(const char* base_dir, const char* name) {
    size_t n = strlen(base_dir);
    char* res = malloc(n + 1 + strlen(name) + 1);
    strcpy(res, base_dir);
    if(n && base_dir[n-1] != '/') strcat(res, "/");
    strcat(res, name);
    return res;
}

void handle_directive(const char* line, const char* base_dir, dynbuf* out) {
    const char* p = skip_ws(line);
    if(strncmp(p, "include", 7) == 0 && isspace((unsigned char)p[7])) {
        p = skip_ws(p + 7);
        if(*p == '"') {
            p++;
            const char* q = strchr(p, '"');
            if(!q) fatal("bad include");
            char name[512]; strncpy(name, p, q - p); name[q - p] = '\0';
            char* path = join_path(base_dir, name);
            filebuf inc = read_file(path);
            preprocess_filebuf(inc, out, base_dir);
            free(path);
            free(inc.data);
            return;
        } else {
            fatal("only include \"file\" supported");
        }
    } else if(strncmp(p, "define", 6) == 0 && isspace((unsigned char)p[6])) {
        p = skip_ws(p + 6);
        char name[128]; int i = 0;
        while(*p && (isalnum((unsigned char)*p) || *p == '_')) name[i++] = *p++;
        name[i] = '\0';
        p = skip_ws(p);
        // rest of line is replacement
        char repl[1024] = {0};
        int j = 0;
        while(*p && *p != '\n' && j < (int)sizeof(repl)-1) repl[j++] = *p++;
        repl[j] = '\0';
        define_macro(name, repl);
        return;
    } else if(strncmp(p, "undef", 5) == 0 && isspace((unsigned char)p[5])) {
        p = skip_ws(p + 5);
        char name[128]; int i = 0;
        while(*p && (isalnum((unsigned char)*p) || *p == '_')) name[i++] = *p++;
        name[i] = '\0';
        undef_macro(name);
        return;
    } else if(strncmp(p, "ifdef", 5) == 0 && isspace((unsigned char)p[5])) {
        // handled in preprocess_filebuf control flow
        return;
    } else if(strncmp(p, "ifndef", 6) == 0 && isspace((unsigned char)p[6])) {
        return;
    } else if(strncmp(p, "if", 2) == 0 && isspace((unsigned char)p[2])) {
        return;
    } else if(strncmp(p, "else", 4) == 0) {
        return;
    } else if(strncmp(p, "endif", 5) == 0) {
        return;
    } else {
        // unknown directive: ignore
        return;
    }
}

/* Preprocess with very simple conditional handling */
void preprocess_filebuf(filebuf fb, dynbuf* out, const char* base_dir) {
    const char* p = fb.data;
    while(*p) {
        if(*p == '#') {
            // read directive line
            const char* line_start = p + 1;
            const char* nl = strchr(line_start, '\n');
            size_t len = nl ? (size_t)(nl - line_start) : strlen(line_start);
            char line[1024]; strncpy(line, line_start, len); line[len] = '\0';
            // handle if/ifdef/ifndef
            const char* q = skip_ws(line);
            if(strncmp(q, "ifdef", 5) == 0 && isspace((unsigned char)q[5])) {
                q = skip_ws(q + 5);
                char name[128]; int i = 0;
                while(*q && (isalnum((unsigned char)*q) || *q == '_')) name[i++] = *q++;
                name[i] = '\0';
                int defined = lookup_macro(name) != NULL;
                // skip to matching else/endif
                const char* block = nl ? nl + 1 : NULL;
                int depth = 1;
                while(block && *block) {
                    if(*block == '#') {
                        const char* ls = block + 1;
                        const char* nln = strchr(ls, '\n');
                        size_t llen = nln ? (size_t)(nln - ls) : strlen(ls);
                        char l[1024]; strncpy(l, ls, llen); l[llen] = '\0';
                        const char* sk = skip_ws(l);
                        if(strncmp(sk, "ifdef", 5) == 0 || strncmp(sk, "ifndef", 6) == 0 || strncmp(sk, "if", 2) == 0) depth++;
                        else if(strncmp(sk, "endif", 5) == 0) { depth--; if(depth==0) { block = nln ? nln + 1 : NULL; break; } }
                        else if(strncmp(sk, "else", 4) == 0 && depth==1) {
                            // split region
                            const char* true_start = nl ? nl + 1 : NULL;
                            const char* else_start = nln ? nln + 1 : NULL;
                            if(defined) {
                                // process true_start .. nln-1
                                filebuf tmp = { NULL, 0, NULL };
                                size_t seglen = (size_t)(nln - true_start);
                                tmp.data = malloc(seglen + 1);
                                strncpy(tmp.data, true_start, seglen);
                                tmp.data[seglen] = '\0';
                                preprocess_filebuf(tmp, out, base_dir);
                                free(tmp.data);
                            } else {
                                // process else_start .. before endif
                                // find endif
                                const char* after = else_start;
                                int d2 = 1;
                                while(after && *after) {
                                    if(*after == '#') {
                                        const char* ls2 = after + 1;
                                        const char* nln2 = strchr(ls2, '\n');
                                        size_t llen2 = nln2 ? (size_t)(nln2 - ls2) : strlen(ls2);
                                        char l2[1024]; strncpy(l2, ls2, llen2); l2[llen2] = '\0';
                                        const char* sk2 = skip_ws(l2);
                                        if(strncmp(sk2, "if", 2) == 0) d2++;
                                        else if(strncmp(sk2, "endif", 5) == 0) { d2--; if(d2==0) { const char* endpos = nln2 ? nln2 : after + strlen(after); size_t seglen2 = (size_t)(endpos - else_start); filebuf tmp2 = { NULL, 0, NULL }; tmp2.data = malloc(seglen2+1); strncpy(tmp2.data, else_start, seglen2); tmp2.data[seglen2] = '\0'; preprocess_filebuf(tmp2, out, base_dir); free(tmp2.data); break; } }
                                        after = nln2 ? nln2 + 1 : NULL;
                                    } else {
                                        after++;
                                    }
                                }
                            }
                            // advance p to after endif
                            p = nln ? nln + 1 : NULL;
                            if(!p) return;
                            // skip the rest of the outer loop increment
                            continue;
                        }
                        block = nln ? nln + 1 : NULL;
                    } else block++;
                }
                // if no else found and defined true, process the block between nl+1 and matching endif
                if(defined) {
                    // naive: find next #endif at same depth
                    const char* scan = nl ? nl + 1 : NULL;
                    int d = 1;
                    const char* start = scan;
                    while(scan && *scan) {
                        if(*scan == '#') {
                            const char* ls = scan + 1;
                            const char* nln = strchr(ls, '\n');
                            size_t llen = nln ? (size_t)(nln - ls) : strlen(ls);
                            char l[1024]; strncpy(l, ls, llen); l[llen] = '\0';
                            const char* sk = skip_ws(l);
                            if(strncmp(sk, "if", 2) == 0) d++;
                            else if(strncmp(sk, "endif", 5) == 0) { d--; if(d==0) { size_t seglen = (size_t)(scan - start); filebuf tmp = { NULL, 0, NULL }; tmp.data = malloc(seglen+1); strncpy(tmp.data, start, seglen); tmp.data[seglen] = '\0'; preprocess_filebuf(tmp, out, base_dir); free(tmp.data); scan = nln ? nln + 1 : NULL; p = scan; break; } }
                            scan = nln ? nln + 1 : NULL;
                        } else scan++;
                    }
                    if(!scan) return;
                    continue;
                } else {
                    // skip to endif
                    const char* scan = nl ? nl + 1 : NULL;
                    int d = 1;
                    while(scan && *scan) {
                        if(*scan == '#') {
                            const char* ls = scan + 1;
                            const char* nln = strchr(ls, '\n');
                            size_t llen = nln ? (size_t)(nln - ls) : strlen(ls);
                            char l[1024]; strncpy(l, ls, llen); l[llen] = '\0';
                            const char* sk = skip_ws(l);
                            if(strncmp(sk, "if", 2) == 0) d++;
                            else if(strncmp(sk, "endif", 5) == 0) { d--; if(d==0) { p = nln ? nln + 1 : NULL; break; } }
                            scan = nln ? nln + 1 : NULL;
                        } else scan++;
                    }
                    if(!scan) return;
                    continue;
                }
            } else {
                handle_directive(line, base_dir, out);
                p = nl ? nl + 1 : p + strlen(p);
                continue;
            }
        }

        // normal text: copy char, but perform macro replacement for identifiers
        if(isalpha((unsigned char)*p) || *p == '_') {
            const char* q = p;
            char ident[256]; int i = 0;
            while(*q && (isalnum((unsigned char)*q) || *q == '_')) ident[i++] = *q++;
            ident[i] = '\0';
            const char* repl = lookup_macro(ident);
            if(repl) db_append_str(out, repl);
            else {
                for(int k = 0; k < i; k++) db_append_char(out, ident[k]);
            }
            p = q;
            continue;
        } else {
            db_append_char(out, *p++);
        }
    }
}

/* ---------------------------
   Token linked list
   --------------------------- */

typedef enum {
    T_EOF = 0, T_KEYWORD, T_IDENT, T_NUMBER, T_CHAR, T_STRING, T_OP,
    T_LE, T_GE, T_EQ, T_NE, T_LOG_AND, T_LOG_OR, T_PTR_OP
} tokentype;

const char* keywords[] = {
    "asm","if","else","while","break","continue","return","int","char","void"
};
enum { KEYWORD_COUNT = sizeof(keywords)/sizeof(keywords[0]) };

typedef struct token {
    tokentype type;
    int kw_index;
    char op;
    long long number;
    char text[256];
    int line, col;
    struct token* next;
} token;

token* token_head = NULL;
token* token_tail = NULL;
token* cur_tok = NULL;

void tok_add(token* t) {
    if(!token_head) token_head = token_tail = t;
    else { token_tail->next = t; token_tail = t; }
}

token* tok_new(tokentype type, int line, int col) {
    token* t = calloc(1, sizeof(token));
    t->type = type; t->line = line; t->col = col; t->next = NULL;
    return t;
}

/* Scanner over preprocessed buffer */
void scan_buffer(const char* buf) {
    int line = 1, col = 0;
    const char* p = buf;
    while(*p) {
        col++;
        if(*p == '\n') { line++; col = 0; p++; continue; }
        if(isspace((unsigned char)*p)) { p++; continue; }
        if(*p == '/' && *(p+1) == '/') { while(*p && *p != '\n') p++; continue; }
        if(*p == '/' && *(p+1) == '*') { p+=2; while(*p && !(*p=='*' && *(p+1)=='/')) { if(*p=='\n') { line++; col=0; } p++; } if(*p) p+=2; continue; }

        // two-char ops
        if(*p == '<' && *(p+1) == '=') { token* t = tok_new(T_LE, line, col); tok_add(t); p+=2; continue; }
        if(*p == '>' && *(p+1) == '=') { token* t = tok_new(T_GE, line, col); tok_add(t); p+=2; continue; }
        if(*p == '=' && *(p+1) == '=') { token* t = tok_new(T_EQ, line, col); tok_add(t); p+=2; continue; }
        if(*p == '!' && *(p+1) == '=') { token* t = tok_new(T_NE, line, col); tok_add(t); p+=2; continue; }
        if(*p == '&' && *(p+1) == '&') { token* t = tok_new(T_LOG_AND, line, col); tok_add(t); p+=2; continue; }
        if(*p == '|' && *(p+1) == '|') { token* t = tok_new(T_LOG_OR, line, col); tok_add(t); p+=2; continue; }
        if(*p == '-' && *(p+1) == '>') { token* t = tok_new(T_PTR_OP, line, col); tok_add(t); p+=2; continue; }

        // single char ops
        if(strchr("+-*%&|~!=<>;:()[],@{}.", *p)) {
            token* t = tok_new(T_OP, line, col); t->op = *p; tok_add(t); p++; continue;
        }

        // char literal
        if(*p == '\'') {
            p++;
            char ch = 0;
            if(*p == '\\') { p++; if(*p == 'n') ch = '\n'; else ch = *p; p++; }
            else { ch = *p; p++; }
            if(*p != '\'') fatal("bad char literal");
            p++;
            token* t = tok_new(T_CHAR, line, col); t->text[0] = ch; t->text[1] = '\0'; tok_add(t); continue;
        }

        // string literal
        if(*p == '"') {
            p++;
            token* t = tok_new(T_STRING, line, col);
            int i = 0;
            while(*p && *p != '"') {
                if(*p == '\\') { p++; if(*p == 'n') t->text[i++] = '\n'; else t->text[i++] = *p; p++; }
                else t->text[i++] = *p++;
                if(i >= (int)sizeof(t->text)-1) fatal("string too long");
            }
            if(*p != '"') fatal("unclosed string");
            p++;
            t->text[i] = '\0';
            tok_add(t);
            continue;
        }

        // number
        if(isdigit((unsigned char)*p)) {
            long long v = 0;
            while(isdigit((unsigned char)*p)) { v = v*10 + (*p - '0'); p++; }
            token* t = tok_new(T_NUMBER, line, col); t->number = v; tok_add(t); continue;
        }

        // identifier or keyword
        if(isalpha((unsigned char)*p) || *p == '_') {
            char bufid[256]; int i = 0;
            while(isalnum((unsigned char)*p) || *p == '_') { if(i < (int)sizeof(bufid)-1) bufid[i++] = *p; p++; }
            bufid[i] = '\0';
            int k;
            for(k = 0; k < KEYWORD_COUNT; k++) if(strcmp(bufid, keywords[k]) == 0) break;
            if(k < KEYWORD_COUNT) { token* t = tok_new(T_KEYWORD, line, col); t->kw_index = k; tok_add(t); }
            else { token* t = tok_new(T_IDENT, line, col); strncpy(t->text, bufid, sizeof(t->text)-1); tok_add(t); }
            continue;
        }

        // unknown
        fatal("unknown char '%c' at %d:%d", *p, line, col);
    }
    token* te = tok_new(T_EOF, line, 0); tok_add(te);
}

/* ---------------------------
   AST and parser (cursor over token list)
   --------------------------- */

enum {
    node_const, node_var, node_call, node_assign, node_op_unary, node_op_binary, node_if, node_while,
    node_return, node_block, node_declaration, node_break, node_continue, node_empty
};

typedef struct ast {
    int type;
    union {
        long long val;
        struct { char name[64]; int offset; int data_type; struct ast* init; } var;
        struct { int op; char name[64]; struct ast* operand; struct ast** args; int arg_count; } unary;
        struct { int op; struct ast* left; struct ast* right; } binary;
        struct { struct ast* cond; struct ast* body_true; struct ast* body_false; } if_stmt;
        struct { struct ast* cond; struct ast* body; } while_stmt;
        struct { struct ast* expr; } ret_stmt;
        struct { struct ast** stmts; int stmt_count; } block;
    };
} ast;

typedef struct {
    char name[64];
    int return_type;
    int param_count;
    char param_names[16][64];
    int param_types[16];
    ast* body;
    int is_global;
    int global_size;
} function_t;

function_t* parsed_functions = NULL;
int parsed_count = 0;
int parsed_cap = 0;

/* token cursor helpers */
token* peek() { return cur_tok; }
token* advance_tok() { if(cur_tok) cur_tok = cur_tok->next; return cur_tok; }
int tok_is_op(char op) { return cur_tok && cur_tok->type == T_OP && cur_tok->op == op; }
int tok_is_keyword(const char* kw) { return cur_tok && cur_tok->type == T_KEYWORD && strcmp(keywords[cur_tok->kw_index], kw) == 0; }
void expect_op(char op) { if(!tok_is_op(op)) fatal("expected '%c'", op); advance_tok(); }
void expect_keyword(const char* kw) { if(!tok_is_keyword(kw)) fatal("expected keyword %s", kw); advance_tok(); }

ast* new_ast(int type) { ast* a = calloc(1, sizeof(ast)); a->type = type; return a; }

/* simple symbol table for locals (per parse run) */
typedef struct varent { char name[64]; int offset; int type; struct varent* next; } varent;
varent* locals = NULL;
int local_count = 0;

void add_local(const char* name, int type) {
    varent* v = malloc(sizeof(varent));
    strncpy(v->name, name, sizeof(v->name)-1);
    v->offset = (++local_count) * 8;
    v->type = type;
    v->next = locals;
    locals = v;
}
varent* lookup_local(const char* name) {
    for(varent* v = locals; v; v = v->next) if(strcmp(v->name, name) == 0) return v;
    return NULL;
}

/* forward declarations */
ast* expression();
ast* statement();
ast* expr_level_zero();

int is_type_keyword(token* t) {
    if(!t) return 0;
    if(t->type != T_KEYWORD) return 0;
    int k = t->kw_index;
    return (k >= 7 && k <= 9); // int,char,void
}
int is_expr_begin(token* t) {
    if(!t) return 0;
    if(t->type == T_NUMBER || t->type == T_CHAR || t->type == T_STRING || t->type == T_IDENT) return 1;
    if(t->type == T_OP && (t->op == '-' || t->op == '!' || t->op == '*' || t->op == '(')) return 1;
    return 0;
}

/* expression parsing (recursive descent) */
ast* expr_level_zero() {
    token* t = peek();
    if(!t) fatal("unexpected EOF in expr");
    if(t->type == T_OP && t->op == '!') { advance_tok(); ast* n = new_ast(node_op_unary); n->unary.op = '!'; n->unary.operand = expr_level_zero(); return n; }
    if(t->type == T_OP && t->op == '*') { advance_tok(); ast* n = new_ast(node_op_unary); n->unary.op = '*'; n->unary.operand = expr_level_zero(); return n; }
    if(t->type == T_OP && t->op == '-') {
        advance_tok();
        if(peek() && peek()->type == T_NUMBER) { ast* n = new_ast(node_const); n->val = -peek()->number; advance_tok(); return n; }
        ast* n = new_ast(node_op_unary); n->unary.op = '-'; n->unary.operand = expr_level_zero(); return n;
    }
    if(t->type == T_NUMBER) { ast* n = new_ast(node_const); n->val = t->number; advance_tok(); return n; }
    if(t->type == T_CHAR) { ast* n = new_ast(node_const); n->val = (unsigned char)t->text[0]; advance_tok(); return n; }
    if(t->type == T_OP && t->op == '(') { advance_tok(); ast* n = expression(); if(!peek() || !(peek()->type==T_OP && peek()->op==')')) fatal("expected )"); advance_tok(); return n; }
    if(t->type == T_IDENT) {
        char name[64]; strncpy(name, t->text, sizeof(name)-1); advance_tok();
        if(peek() && peek()->type == T_OP && peek()->op == '(') {
            advance_tok(); ast* n = new_ast(node_call); n->unary.arg_count = 0; n->unary.args = NULL; strncpy(n->unary.name, name, sizeof(n->unary.name)-1);
            if(peek() && !(peek()->type==T_OP && peek()->op==')')) {
                int cap = 4; n->unary.args = malloc(cap * sizeof(ast*));
                while(1) {
                    if(n->unary.arg_count >= cap) { cap *= 2; n->unary.args = realloc(n->unary.args, cap * sizeof(ast*)); }
                    n->unary.args[n->unary.arg_count++] = expression();
                    if(peek() && peek()->type==T_OP && peek()->op==',') { advance_tok(); continue; }
                    break;
                }
            }
            if(!peek() || !(peek()->type==T_OP && peek()->op==')')) fatal("expected ) in call");
            advance_tok();
            return n;
        } else if(peek() && peek()->type==T_OP && peek()->op=='=') {
            ast* n = new_ast(node_assign);
            n->binary.left = new_ast(node_var);
            strncpy(n->binary.left->var.name, name, sizeof(n->binary.left->var.name)-1);
            advance_tok(); // =
            n->binary.right = expression();
            return n;
        } else {
            ast* n = new_ast(node_var); strncpy(n->var.name, name, sizeof(n->var.name)-1); return n;
        }
    }
    if(t->type == T_STRING) fatal("string literal not supported in expr");
    fatal("bad expression start");
    return NULL;
}

ast* expr_level_bin(ast* (*next)(), const char* ops) {
    ast* left = next();
    while(peek() && ((peek()->type==T_OP && strchr(ops, peek()->op)) || peek()->type==T_LE || peek()->type==T_GE || peek()->type==T_EQ || peek()->type==T_NE || peek()->type==T_LOG_AND || peek()->type==T_LOG_OR)) {
        int op_type = peek()->type;
        int op_char = 0;
        if(op_type == T_OP) op_char = peek()->op;
        advance_tok();
        ast* right = next();
        ast* n = new_ast(node_op_binary);
        n->binary.op = (op_type == T_OP) ? op_char : op_type;
        n->binary.left = left; n->binary.right = right;
        left = n;
    }
    return left;
}

ast* expr_level_one() { return expr_level_bin(expr_level_zero, "*/%"); }
ast* expr_level_two() { return expr_level_bin(expr_level_one, "+-"); }
ast* expr_level_three() { return expr_level_bin(expr_level_two, "<>"); }
ast* expr_level_four() { return expr_level_bin(expr_level_three, ""); }
ast* expr_level_five() { return expr_level_bin(expr_level_four, "&"); }
ast* expr_level_six() { return expr_level_bin(expr_level_five, "|"); }
ast* expr_level_seven() { return expr_level_bin(expr_level_six, ""); }

ast* expression() { return expr_level_bin(expr_level_seven, ""); }

/* statements */
ast* statement_list();

ast* statement() {
    token* t = peek();
    if(!t) fatal("unexpected EOF in stmt");
    if(t->type==T_OP && t->op=='{') { advance_tok(); ast* b = statement_list(); if(!peek() || !(peek()->type==T_OP && peek()->op=='}')) fatal("expected }"); advance_tok(); return b; }
    if(is_type_keyword(t)) {
        int dtype = t->kw_index; advance_tok();
        if(!peek() || peek()->type != T_IDENT) fatal("expected ident in decl");
        ast* n = new_ast(node_declaration);
        strncpy(n->var.name, peek()->text, sizeof(n->var.name)-1);
        n->var.data_type = dtype;
        add_local(n->var.name, dtype);
        n->var.offset = lookup_local(n->var.name)->offset;
        advance_tok();
        if(peek() && peek()->type==T_OP && peek()->op=='=') { advance_tok(); n->var.init = expression(); } else n->var.init = NULL;
        if(!peek() || !(peek()->type==T_OP && peek()->op==';')) fatal("expected ; after decl");
        advance_tok();
        return n;
    }
    if(t->type==T_KEYWORD && strcmp(keywords[t->kw_index], "if")==0) {
        advance_tok(); if(!peek() || !(peek()->type==T_OP && peek()->op=='(')) fatal("expected ( after if"); advance_tok();
        ast* n = new_ast(node_if); n->if_stmt.cond = expression(); if(!peek() || !(peek()->type==T_OP && peek()->op==')')) fatal("expected )"); advance_tok();
        n->if_stmt.body_true = statement();
        if(peek() && peek()->type==T_KEYWORD && strcmp(keywords[peek()->kw_index],"else")==0) { advance_tok(); n->if_stmt.body_false = statement(); } else n->if_stmt.body_false = NULL;
        return n;
    }
    if(t->type==T_KEYWORD && strcmp(keywords[t->kw_index],"while")==0) {
        advance_tok(); if(!peek() || !(peek()->type==T_OP && peek()->op=='(')) fatal("expected ( after while"); advance_tok();
        ast* n = new_ast(node_while); n->while_stmt.cond = expression(); if(!peek() || !(peek()->type==T_OP && peek()->op==')')) fatal("expected )"); advance_tok();
        n->while_stmt.body = statement(); return n;
    }
    if(t->type==T_KEYWORD && strcmp(keywords[t->kw_index],"return")==0) {
        advance_tok(); ast* n = new_ast(node_return); if(is_expr_begin(peek())) n->ret_stmt.expr = expression(); else n->ret_stmt.expr = NULL;
        if(!peek() || !(peek()->type==T_OP && peek()->op==';')) fatal("expected ; after return"); advance_tok(); return n;
    }
    if(t->type==T_KEYWORD && strcmp(keywords[t->kw_index],"break")==0) { advance_tok(); if(!peek() || !(peek()->type==T_OP && peek()->op==';')) fatal("expected ; after break"); advance_tok(); return new_ast(node_break); }
    if(t->type==T_KEYWORD && strcmp(keywords[t->kw_index],"continue")==0) { advance_tok(); if(!peek() || !(peek()->type==T_OP && peek()->op==';')) fatal("expected ; after continue"); advance_tok(); return new_ast(node_continue); }
    if(is_expr_begin(peek())) { ast* e = expression(); if(!peek() || !(peek()->type==T_OP && peek()->op==';')) fatal("expected ; after expr"); advance_tok(); return e; }
    if(peek() && peek()->type==T_OP && peek()->op==';') { advance_tok(); return new_ast(node_empty); }
    fatal("unexpected token in statement");
    return NULL;
}

ast* statement_list() {
    ast* block = new_ast(node_block);
    block->block.stmts = malloc(16 * sizeof(ast*));
    block->block.stmt_count = 0;
    int cap = 16;
    while(peek() && peek()->type != T_EOF && !(peek()->type==T_OP && peek()->op=='}')) {
        if(block->block.stmt_count >= cap) { cap *= 2; block->block.stmts = realloc(block->block.stmts, cap * sizeof(ast*)); }
        block->block.stmts[block->block.stmt_count++] = statement();
    }
    return block;
}

/* ---------------------------
   Top-level parse into function_t[]
   --------------------------- */

void ensure_parsed_cap() {
    if(!parsed_functions) { parsed_cap = 16; parsed_functions = calloc(parsed_cap, sizeof(function_t)); parsed_count = 0; }
    if(parsed_count + 1 > parsed_cap) { parsed_cap *= 2; parsed_functions = realloc(parsed_functions, parsed_cap * sizeof(function_t)); }
}

void parse_all() {
    parsed_count = 0;
    while(peek() && peek()->type != T_EOF) {
        if(!is_type_keyword(peek())) fatal("expected type at top level");
        int ret = peek()->kw_index; advance_tok();
        if(!peek() || peek()->type != T_IDENT) fatal("expected ident after type");
        function_t f; memset(&f,0,sizeof(f));
        strncpy(f.name, peek()->text, sizeof(f.name)-1);
        f.return_type = ret;
        advance_tok();
        if(peek() && peek()->type==T_OP && peek()->op==';') {
            f.is_global = 1; f.global_size = 8; ensure_parsed_cap(); parsed_functions[parsed_count++] = f; advance_tok(); continue;
        }
        if(!peek() || !(peek()->type==T_OP && peek()->op=='(')) fatal("expected ( after name");
        advance_tok();
        f.param_count = 0;
        if(peek() && !(peek()->type==T_OP && peek()->op==')')) {
            while(1) {
                if(!is_type_keyword(peek())) fatal("expected param type");
                int ptype = peek()->kw_index; advance_tok();
                if(peek() && peek()->type==T_OP && peek()->op=='*') advance_tok();
                if(!peek() || peek()->type!=T_IDENT) fatal("expected param name");
                strncpy(f.param_names[f.param_count], peek()->text, sizeof(f.param_names[0])-1);
                f.param_types[f.param_count] = ptype;
                f.param_count++;
                advance_tok();
                if(peek() && peek()->type==T_OP && peek()->op==',') { advance_tok(); continue; }
                break;
            }
        }
        if(!peek() || !(peek()->type==T_OP && peek()->op==')')) fatal("expected ) after params");
        advance_tok();
        if(!peek() || !(peek()->type==T_OP && peek()->op=='{')) fatal("expected { for body");
        advance_tok();
        // reset locals and add params
        locals = NULL; local_count = 0;
        for(int i=0;i<f.param_count;i++) add_local(f.param_names[i], f.param_types[i]);
        f.body = statement_list();
        if(!peek() || !(peek()->type==T_OP && peek()->op=='}')) fatal("expected } at end of function");
        advance_tok();
        ensure_parsed_cap();
        parsed_functions[parsed_count++] = f;
        // clear locals for next top-level
        locals = NULL; local_count = 0;
    }
}

/* ---------------------------
   Codegen (simple)
   --------------------------- */

const char* regs[] = { "r8","r9","r11","rax" };
enum { CACHE_SIZE = 4 };
int cache[CACHE_SIZE];
int stack_size = 0;
int label = 0;
int while_labels[256];
int while_level = -1;

const char* regname(int i) { return regs[cache[i]]; }
void init_cache() { for(int i=0;i<CACHE_SIZE;i++) cache[i]=i; stack_size=0; }
void push_reg() {
    int i = CACHE_SIZE-1; int tmp = cache[i];
    if(stack_size >= CACHE_SIZE) printf("\tpush %s\n", regs[tmp]);
    while(i>0) { cache[i] = cache[i-1]; i--; }
    cache[0] = tmp; stack_size++;
}
void pop_reg() {
    stack_size--;
    if(stack_size < 0) { stack_size = 0; return; }
    if(stack_size == 0) init_cache();
    else {
        int i = 0; int tmp = cache[0];
        while(i < CACHE_SIZE-1) { cache[i] = cache[i+1]; i++; }
        cache[i] = tmp;
        if(stack_size >= CACHE_SIZE) printf("\tpop %s\n", regs[i]);
    }
}

void codegen_expr(ast* n) {
    if(!n) return;
    switch(n->type) {
        case node_const: push_reg(); printf("\tmov %s, %lld\n", regname(0), n->val); break;
        case node_var: {
            varent* v = lookup_local(n->var.name);
            if(!v) fatal("var not found %s", n->var.name);
            push_reg(); printf("\tmov %s, qword ptr [rbp - %d]\n", regname(0), v->offset); break;
        }
        case node_assign:
            if(n->binary.left->type != node_var) fatal("assign target not var");
            codegen_expr(n->binary.right);
            {
                varent* v = lookup_local(n->binary.left->var.name);
                if(!v) fatal("assign target not found");
                printf("\tmov qword ptr [rbp - %d], %s\n", v->offset, regname(0));
            }
            break;
        case node_op_unary:
            codegen_expr(n->unary.operand);
            if(n->unary.op == '-') printf("\tneg %s\n", regname(0));
            else if(n->unary.op == '!') { printf("\ttest %s, %s\n", regname(0), regname(0)); printf("\tsetz cl\n"); printf("\tmovzx %s, cl\n", regname(0)); }
            else if(n->unary.op == '*') printf("\tmov %s, qword ptr [%s]\n", regname(0), regname(0));
            break;
        case node_op_binary:
            codegen_expr(n->binary.left);
            codegen_expr(n->binary.right);
            {
                int op = n->binary.op;
                if(op == '+') { printf("\tadd %s, %s\n", regname(1), regname(0)); pop_reg(); }
                else if(op == '-') { printf("\tsub %s, %s\n", regname(1), regname(0)); pop_reg(); }
                else if(op == '*') { printf("\timul %s, %s\n", regname(1), regname(0)); pop_reg(); }
                else if(op == '/') { printf("\tmov rax, %s\n", regname(1)); printf("\tcqo\n"); printf("\tidiv %s\n", regname(0)); pop_reg(); printf("\tmov %s, rax\n", regname(0)); }
                else if(op == '<' || op == '>' || op == T_LE || op == T_GE || op == T_EQ || op == T_NE) {
                    const char* comp;
                    if(op == '<') comp = "l"; else if(op == '>') comp = "g";
                    else if(op == T_LE) comp = "le"; else if(op == T_GE) comp = "ge";
                    else if(op == T_EQ) comp = "e"; else comp = "ne";
                    printf("\tcmp %s, %s\n", regname(1), regname(0));
                    printf("\tset%s cl\n", comp); printf("\tmovzx %s, cl\n", regname(1)); pop_reg();
                } else if(op == T_LOG_AND) { printf("\tand %s, %s\n", regname(1), regname(0)); pop_reg(); }
                else if(op == T_LOG_OR) { printf("\tor %s, %s\n", regname(1), regname(0)); pop_reg(); }
                else fatal("unknown binary op");
            }
            break;
        case node_call:
            for(int i=0;i<n->unary.arg_count && i<6;i++) {
                codegen_expr(n->unary.args[i]);
                const char* dest = (i==0?"rdi":(i==1?"rsi":(i==2?"rdx":(i==3?"rcx":(i==4?"r8":"r9")))));
                printf("\tmov %s, %s\n", dest, regname(0));
                pop_reg();
            }
            printf("\tcall %s\n", n->unary.name);
            push_reg(); printf("\tmov %s, rax\n", regname(0));
            break;
        default: fatal("unhandled expr node %d", n->type);
    }
}

void codegen_stmt(ast* n) {
    if(!n) return;
    int l_end, l_next, l_begin;
    switch(n->type) {
        case node_empty: break;
        case node_declaration:
            if(n->var.init) { codegen_expr(n->var.init); printf("\tmov qword ptr [rbp - %d], %s\n", n->var.offset, regname(0)); pop_reg(); }
            break;
        case node_assign: codegen_expr(n); pop_reg(); break;
        case node_op_binary: codegen_expr(n); pop_reg(); break;
        case node_block: for(int i=0;i<n->block.stmt_count;i++) codegen_stmt(n->block.stmts[i]); break;
        case node_if:
            codegen_expr(n->if_stmt.cond);
            l_end = label++; l_next = label++;
            printf("\ttest %s, %s\n", regname(0), regname(0));
            printf("\tjz .L%d\n", l_next);
            init_cache(); pop_reg();
            codegen_stmt(n->if_stmt.body_true);
            printf("\tjmp .L%d\n", l_end);
            printf(".L%d:\n", l_next);
            if(n->if_stmt.body_false) { init_cache(); codegen_stmt(n->if_stmt.body_false); }
            printf(".L%d:\n", l_end);
            break;
        case node_while:
            while_level++;
            if(while_level == 256) fatal("while nesting limit");
            while_labels[while_level] = label; label += 2;
            l_begin = while_labels[while_level]; l_end = while_labels[while_level] + 1;
            printf(".L%d:\n", l_begin);
            codegen_expr(n->while_stmt.cond);
            printf("\ttest %s, %s\n", regname(0), regname(0));
            printf("\tjz .L%d\n", l_end);
            init_cache(); pop_reg();
            codegen_stmt(n->while_stmt.body);
            printf("\tjmp .L%d\n", l_begin);
            printf(".L%d:\n", l_end);
            while_level--;
            break;
        case node_return:
            if(n->ret_stmt.expr) { codegen_expr(n->ret_stmt.expr); if(strcmp(regname(0),"rax")!=0) printf("\tmov rax, %s\n", regname(0)); pop_reg(); }
            printf("\tleave\n\tret\n");
            break;
        case node_break: if(while_level<0) fatal("break without while"); printf("\tjmp .L%d\n", while_labels[while_level]+1); break;
        case node_continue: if(while_level<0) fatal("continue without while"); printf("\tjmp .L%d\n", while_labels[while_level]); break;
        default: fatal("unknown stmt node %d", n->type);
    }
}

void codegen_program() {
    printf("\t.intel_syntax noprefix\n\t.text\n");
    for(int i=0;i<parsed_count;i++) {
        function_t* f = &parsed_functions[i];
        if(f->is_global) { printf("\t.comm %s, %d, 8\n", f->name, f->global_size); continue; }
        printf("\t.globl %s\n%s:\n\tpush rbp\n\tmov rbp, rsp\n", f->name, f->name);
        // simple frame: local_count * 8 + 8
        int frame = local_count * 8 + 8;
        if(frame > 0) printf("\tsub rsp, %d\n", frame);
        init_cache();
        codegen_stmt(f->body);
        printf("\tleave\n\tret\n");
    }
}

/* ---------------------------
   Main driver
   --------------------------- */

int main(int argc, char** argv) {
    if(argc < 2 || argc > 3) { fprintf(stderr, "usage: %s <source> [out]\n", argv[0]); return 1; }
    const char* src = argv[1];
    filebuf fb = read_file(src);
    // base dir for includes
    char base_dir[1024] = ".";
    const char* slash = strrchr(src, '/');
    if(slash) {
        size_t n = (size_t)(slash - src);
        strncpy(base_dir, src, n); base_dir[n] = '\0';
    }
    dynbuf db; db_init(&db);
    preprocess_filebuf(fb, &db, base_dir);
    // now db.data contains preprocessed source
    scan_buffer(db.data);
    cur_tok = token_head;
    parse_all();
    // optionally redirect output to file
    if(argc == 3) {
        freopen(argv[2], "w", stdout);
    }
    codegen_program();
    // cleanup
    free(fb.data);
    free(db.data);
    return 0;
}
