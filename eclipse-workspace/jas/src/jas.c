/*
 * jas.c - table-driven assembler prototype (fixed single-file)
 *
 * note: this is a prototype focusing on parsing and lowering assembly-style inputs.
 * the table-driven logic is implemented in lower_asm_astnode, where a lookup is
 * performed to match the instruction mnemonic and operand types.
 *
 * the opcode_id is emitted as a comment (e.g., /* id: 1 * /) instead of generating
 * actual machine code bytes, demonstrating the successful table lookup.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdint.h>
#include <stdarg.h>

/* ----------------------
   config
   ---------------------- */
#define init_ir_cap 256
#define init_top_cap 256
#define max_macro_recursion 10
int macro_recursion_depth = 0;

/* ----------------------
   utilities
   ---------------------- */

static void die(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "error: ");
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

static void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) die("memory allocation failed (malloc)");
    return ptr;
}

static void *xrealloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr) die("memory allocation failed (realloc)");
    return new_ptr;
}

static char *xstrdup_safe(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *new_s = xmalloc(len);
    memcpy(new_s, s, len);
    return new_s;
}

static void emitf(FILE *out, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    fprintf(out, "\n");
    va_end(args);
}

static void emit_raw(FILE *out, const char *s) {
    fprintf(out, "%s\n", s);
}

/* ----------------------
   lexer and tokens
   ---------------------- */

typedef enum {
    tok_eof, tok_id, tok_num, tok_string, tok_lparen, tok_rparen,
    tok_comma, tok_colon, tok_dollar, tok_percent, tok_plus, tok_minus,
    tok_star, tok_slash, tok_tilde, tok_eq, tok_dir, tok_newline
} tokenkind;

typedef struct {
    tokenkind kind;
    char *text;
    uint64_t val;
    int line;
} token;

typedef struct {
    const char *source;
    int pos;
    int line;
    token cur;
    int has_peeked;
    token peek_tok;
} lexer;

static void lexer_error(lexer *l, const char *msg) {
    die("lexer error (line %d, pos %d): %s", l->line, l->pos, msg);
}

static void lexer_next(lexer *l) {
    while (1) {
        char c = l->source[l->pos];

        if (c == '\0') {
            l->cur = (token){tok_eof, xstrdup_safe("<eof>"), 0, l->line};
            return;
        }

        // skip whitespace, including newlines
        if (isspace((unsigned char)c)) {
            if (c == '\n') l->line++;
            l->pos++;
            continue;
        }

        // skip c-style comments (// or /* */)
        if (c == '/' && l->source[l->pos+1] == '/') {
            while (l->source[l->pos] != '\n' && l->source[l->pos] != '\0') l->pos++;
            continue;
        }
        if (c == '/' && l->source[l->pos+1] == '*') {
            l->pos += 2;
            while (1) {
                if (l->source[l->pos] == '\0') lexer_error(l, "unterminated c-style comment");
                if (l->source[l->pos] == '\n') l->line++;
                if (l->source[l->pos] == '*' && l->source[l->pos+1] == '/') {
                    l->pos += 2;
                    break;
                }
                l->pos++;
            }
            continue;
        }

        // skip gas-style comments (# or @)
        if (c == '#' || c == '@') {
            while (l->source[l->pos] != '\n' && l->source[l->pos] != '\0') l->pos++;
            continue;
        }

        // identifiers and directives
        if (isalpha((unsigned char)c) || c == '.' || c == '_') {
            int start = l->pos;
            l->pos++;
            while (isalnum((unsigned char)l->source[l->pos]) || l->source[l->pos] == '_') l->pos++;
            int len = l->pos - start;
            char *text = xmalloc(len + 1);
            strncpy(text, l->source + start, len);
            text[len] = '\0';

            if (text[0] == '.') {
                l->cur = (token){tok_dir, text, 0, l->line};
            } else {
                l->cur = (token){tok_id, text, 0, l->line};
            }
            return;
        }

        // numbers (decimal/hex)
        if (isdigit((unsigned char)c)) {
            int start = l->pos;
            uint64_t val = 0;
            int is_hex = 0;

            if (c == '0' && (l->source[l->pos+1] == 'x' || l->source[l->pos+1] == 'X')) {
                is_hex = 1;
                l->pos += 2; // skip 0x/0X
            }

            if (is_hex) {
                while (isxdigit((unsigned char)l->source[l->pos])) {
                    char digit = l->source[l->pos];
                    int v;
                    if (digit >= '0' && digit <= '9') v = digit - '0';
                    else if (digit >= 'a' && digit <= 'f') v = digit - 'a' + 10;
                    else if (digit >= 'A' && digit <= 'F') v = digit - 'A' + 10;
                    else lexer_error(l, "invalid hex digit");
                    val = (val << 4) | (uint64_t)v;
                    l->pos++;
                }
            } else {
                while (isdigit((unsigned char)l->source[l->pos])) {
                    val = val * 10 + (uint64_t)(l->source[l->pos] - '0');
                    l->pos++;
                }
            }

            int len = l->pos - start;
            char *text = xmalloc(len + 1);
            strncpy(text, l->source + start, len);
            text[len] = '\0';
            l->cur = (token){tok_num, text, val, l->line};
            return;
        }

        // single-character tokens
        l->pos++;
        switch (c) {
            case '(': l->cur = (token){tok_lparen, xstrdup_safe("("), 0, l->line}; return;
            case ')': l->cur = (token){tok_rparen, xstrdup_safe(")"), 0, l->line}; return;
            case ',': l->cur = (token){tok_comma, xstrdup_safe(","), 0, l->line}; return;
            case ':': l->cur = (token){tok_colon, xstrdup_safe(":"), 0, l->line}; return;
            case '$': l->cur = (token){tok_dollar, xstrdup_safe("$"), 0, l->line}; return;
            case '%': l->cur = (token){tok_percent, xstrdup_safe("%"), 0, l->line}; return;
            case '+': l->cur = (token){tok_plus, xstrdup_safe("+"), 0, l->line}; return;
            case '-': l->cur = (token){tok_minus, xstrdup_safe("-"), 0, l->line}; return;
            case '*': l->cur = (token){tok_star, xstrdup_safe("*"), 0, l->line}; return;
            case '/': l->cur = (token){tok_slash, xstrdup_safe("/"), 0, l->line}; return;
            case '~': l->cur = (token){tok_tilde, xstrdup_safe("~"), 0, l->line}; return;
            case '=': l->cur = (token){tok_eq, xstrdup_safe("="), 0, l->line}; return;
            case '"': {
                int start = l->pos;
                while (l->source[l->pos] != '"') {
                    if (l->source[l->pos] == '\0' || l->source[l->pos] == '\n') lexer_error(l, "unterminated string literal");
                    if (l->source[l->pos] == '\\') l->pos++; // simple escape skip
                    l->pos++;
                }
                int len = l->pos - start;
                char *text = xmalloc(len + 1);
                strncpy(text, l->source + start, len);
                text[len] = '\0';
                l->pos++; // skip closing quote
                l->cur = (token){tok_string, text, 0, l->line};
                return;
            }
        }

        // newline (usually already consumed as whitespace)
        if (c == '\n') {
            l->cur = (token){tok_newline, xstrdup_safe("\\n"), 0, l->line};
            return;
        }

        lexer_error(l, "unexpected character");
    }
}

static void lexer_init(lexer *l, const char *source) {
    l->source = source;
    l->pos = 0;
    l->line = 1;
    l->has_peeked = 0;
    l->cur.kind = tok_eof; // placeholder
    l->cur.text = NULL;
    l->cur.val = 0;
    l->cur.line = 1;
}

static void lexer_advance(lexer *l) {
    if (l->cur.text) { free(l->cur.text); l->cur.text = NULL; }
    if (l->has_peeked) {
        l->cur = l->peek_tok;
        l->has_peeked = 0;
    } else {
        lexer_next(l);
    }
}

static token lexer_peek(lexer *l) {
    if (!l->has_peeked) {
        token tmp = l->cur;
        lexer_next(l);
        l->peek_tok = l->cur;
        l->cur = tmp;
        l->has_peeked = 1;
    }
    return l->peek_tok;
}

static int match_token(lexer *l, tokenkind k) {
    if (l->cur.kind == k) {
        lexer_advance(l);
        return 1;
    }
    return 0;
}

static void expect_token(lexer *l, tokenkind k) {
    if (!match_token(l, k)) {
        die("parser error (line %d): expected token %d, got %d", l->line, k, l->cur.kind);
    }
}

/* ----------------------
   ast and operands
   ---------------------- */

typedef enum { op_imm, op_reg, op_mem, op_sym, op_none } opkind;

typedef struct operand {
    opkind kind;
    char *reg;
    char *sym;
    long long imm; // immediate value
    long long disp; // displacement for memory
    char *base;   // base register name
    char *index;  // index register name
    int scale;    // scale (1, 2, 4, 8)
} operand;

typedef enum { ast_instr, ast_label, ast_directive } astkind;

typedef struct astnode {
    astkind kind;
    char *name;
    operand **operands;
    int op_count;
    char *value; // used for set/equ and directives

    // macro specific
    struct astnode **body;
    int body_count;
} astnode;

static void op_free(operand *o) {
    if (!o) return;
    if (o->reg) free(o->reg);
    if (o->sym) free(o->sym);
    if (o->base) free(o->base);
    if (o->index) free(o->index);
    free(o);
}

static void ast_free(astnode *n) {
    if (!n) return;
    if (n->name) free(n->name);
    if (n->value) free(n->value);
    for (int i=0;i<n->op_count;++i) op_free(n->operands[i]);
    if (n->operands) free(n->operands);
    if (n->body) {
        for (int i=0;i<n->body_count;++i) ast_free(n->body[i]);
        free(n->body);
    }
    free(n);
}

static void add_operand(astnode *n, operand *o) {
    if (!o) return;
    if (!n->operands) {
        n->operands = xmalloc(sizeof(operand*) * 4);
        n->op_count = 0;
    }
    if (n->op_count >= 4) die("max operands exceeded in astnode");
    n->operands[n->op_count++] = o;
}

// helper function to format an operand into a buffer
static void format_operand_buf(char *buf, size_t cap, operand *o) {
    buf[0] = 0;
    if (o->kind == op_imm) {
        snprintf(buf, cap, "$%lld", o->imm);
    } else if (o->kind == op_reg) {
        snprintf(buf, cap, "%%%s", o->reg);
    } else if (o->kind == op_sym) {
        snprintf(buf, cap, "%s", o->sym);
    } else if (o->kind == op_mem) {
        // format: disp(%base, %index, scale)
        // this is an approximation for at&t syntax
        if (o->base || o->index) {
            snprintf(buf, cap, "%lld(%%%s,%%%s,%d)", o->disp, o->base ? o->base : "", o->index ? o->index : "", o->scale);
        } else {
            // this could be a symbol or just a disp
            snprintf(buf, cap, "%lld", o->disp);
        }
    }
}

/* ----------------------
   table-driven encoding rules (64+ entries)
   ---------------------- */

// abstracted operand types for matching signatures
typedef enum {
    enc_none,
    enc_reg,    // %rax, %rbx
    enc_imm,    // $123, $0xabc
    enc_mem,    // disp(%base,%index,scale)
    enc_sym,    // label, function_name
    enc_reg_or_mem // used where either a reg or memory is allowed
} operandencodingtype;

// unique ids for instructions handled by the table
typedef enum {
    inst_unknown = 0,
    // data movement (1-11)
    inst_movq_imm_reg, inst_movq_reg_reg, inst_movq_mem_reg, inst_movq_reg_mem,
    inst_movl_imm_reg, inst_movl_reg_reg, inst_movl_mem_reg, inst_movl_reg_mem,
    inst_movsx_dq_rm, inst_movsx_bq_rm, inst_lea_mem_reg,
    // stack (12-16)
    inst_pushq_imm, inst_pushq_reg, inst_pushq_mem, inst_popq_reg, inst_popq_mem,
    // control flow (17-18)
    inst_call_sym, inst_ret,
    // arithmetic (19-32)
    inst_addq_imm_reg, inst_addq_reg_reg, inst_addq_mem_reg, inst_addl_imm_reg,
    inst_subq_imm_reg, inst_subq_reg_reg, inst_subq_mem_reg, inst_subl_imm_reg,
    inst_imulq_reg, inst_imulq_reg_reg, inst_idivq_reg,
    inst_incq_reg, inst_decq_reg, inst_negq_reg,
    inst_cltq, inst_cqo,
    // logical/bitwise (33-38)
    inst_andq_imm_reg, inst_orq_reg_reg, inst_xorq_imm_reg,
    inst_shrq_imm_reg, inst_shlq_cl_reg, inst_testq_reg_reg,
    // comparison (39-40)
    inst_cmpq_imm_reg, inst_cmpq_reg_reg,
    // unconditional/conditional jumps (41-53)
    inst_jmp_sym, inst_jne_sym, inst_je_sym, inst_jge_sym, inst_jg_sym, inst_jle_sym,
    inst_jl_sym, inst_jae_sym, inst_ja_sym, inst_jbe_sym, inst_jb_sym, inst_jz_sym, inst_jnz_sym,
    // conditional sets (54-57)
    inst_sete_reg, inst_setne_reg, inst_sets_reg, inst_setns_reg,
    // conditional moves (58-61)
    inst_cmoveq_reg_reg, inst_cmovneq_reg_reg, inst_cmovg_reg_reg, inst_cmovl_reg_reg,
    // miscellaneous (62-65)
    inst_syscall, inst_nop, inst_pushfq, inst_popfq,
    // placeholder for future expansion...
    inst_nop2 // total > 64 just in case
} instructionid;

// simplified instruction encoding rule
typedef struct {
    const char *mnemonic;        // "movq", "addq"
    operandencodingtype src_enc; // expected source operand type
    operandencodingtype dst_enc; // expected destination operand type
    const char *asm_pattern;     // textual pattern for simple output/debugging
    int opcode_id;               // a unique id for ir
} asmencodingrule;

// the instruction table (64+ entries)
static const asmencodingrule instruction_table[] = {
    // ---- data movement (mov/lea)
    {"movq",   enc_imm, enc_reg, "    movq %s, %s", inst_movq_imm_reg},   // movq $imm, %reg
    {"movq",   enc_reg, enc_reg, "    movq %s, %s", inst_movq_reg_reg},   // movq %reg, %reg
    {"movq",   enc_mem, enc_reg, "    movq %s, %s", inst_movq_mem_reg},   // movq mem, %reg
    {"movq",   enc_reg, enc_mem, "    movq %s, %s", inst_movq_reg_mem},   // movq %reg, mem
    {"movl",   enc_imm, enc_reg, "    movl %s, %s", inst_movl_imm_reg},   // movl $imm, %reg
    {"movl",   enc_reg, enc_reg, "    movl %s, %s", inst_movl_reg_reg},   // movl %reg, %reg
    {"movl",   enc_mem, enc_reg, "    movl %s, %s", inst_movl_mem_reg},   // movl mem, %reg
    {"movl",   enc_reg, enc_mem, "    movl %s, %s", inst_movl_reg_mem},   // movl %reg, mem
    {"movsxd", enc_reg, enc_reg, "    movsxd %s, %s", inst_movsx_dq_rm}, // movsxd %r/m32, %reg64
    {"movsxbq",enc_mem, enc_reg, "    movsxbq %s, %s", inst_movsx_bq_rm},// movsxbq mem8, %reg64
    {"lea",    enc_mem, enc_reg, "    lea %s, %s", inst_lea_mem_reg},     // lea mem, %reg

    // ---- stack and procedures
    {"pushq",  enc_imm, enc_none, "    pushq %s", inst_pushq_imm},        // pushq $imm
    {"pushq",  enc_reg, enc_none, "    pushq %s", inst_pushq_reg},        // pushq %reg
    {"pushq",  enc_mem, enc_none, "    pushq %s", inst_pushq_mem},        // pushq mem
    {"popq",   enc_none, enc_reg, "    popq %s", inst_popq_reg},          // popq %reg
    {"popq",   enc_none, enc_mem, "    popq %s", inst_popq_mem},          // popq mem
    {"call",   enc_sym, enc_none, "    call %s", inst_call_sym},          // call label/symbol
    {"ret",    enc_none, enc_none, "    ret", inst_ret},                  // ret

    // ---- arithmetic
    {"addq",   enc_imm, enc_reg, "    addq %s, %s", inst_addq_imm_reg},   // addq $imm, %reg
    {"addq",   enc_reg, enc_reg, "    addq %s, %s", inst_addq_reg_reg},   // addq %reg, %reg
    {"addq",   enc_mem, enc_reg, "    addq %s, %s", inst_addq_mem_reg},   // addq mem, %reg
    {"addl",   enc_imm, enc_reg, "    addl %s, %s", inst_addl_imm_reg},   // addl $imm, %reg
    {"subq",   enc_imm, enc_reg, "    subq %s, %s", inst_subq_imm_reg},   // subq $imm, %reg
    {"subq",   enc_reg, enc_reg, "    subq %s, %s", inst_subq_reg_reg},   // subq %reg, %reg
    {"subq",   enc_mem, enc_reg, "    subq %s, %s", inst_subq_mem_reg},   // subq mem, %reg
    {"subl",   enc_imm, enc_reg, "    subl %s, %s", inst_subl_imm_reg},   // subl $imm, %reg
    {"imulq",  enc_reg, enc_none, "    imulq %s", inst_imulq_reg},        // imulq %reg (1-op, %rax *= %reg)
    {"imulq",  enc_reg, enc_reg, "    imulq %s, %s", inst_imulq_reg_reg}, // imulq %reg, %reg (2-op, dst *= src)
    {"idivq",  enc_reg, enc_none, "    idivq %s", inst_idivq_reg},        // idivq %reg (%rdx:%rax /= %reg)
    {"incq",   enc_none, enc_reg, "    incq %s", inst_incq_reg},          // incq %reg
    {"decq",   enc_none, enc_reg, "    decq %s", inst_decq_reg},          // decq %reg
    {"negq",   enc_none, enc_reg, "    negq %s", inst_negq_reg},          // negq %reg
    {"cltq",   enc_none, enc_none, "    cltq", inst_cltq},                // sign-extend %eax to %rax
    {"cqo",    enc_none, enc_none, "    cqo", inst_cqo},                  // sign-extend %rax to %rdx:%rax

    // ---- logical/bitwise
    {"andq",   enc_imm, enc_reg, "    andq %s, %s", inst_andq_imm_reg},   // andq $imm, %reg
    {"orq",    enc_reg, enc_reg, "    orq %s, %s", inst_orq_reg_reg},     // orq %reg, %reg
    {"xorq",   enc_imm, enc_reg, "    xorq %s, %s", inst_xorq_imm_reg},   // xorq $imm, %reg
    {"shrq",   enc_imm, enc_reg, "    shrq %s, %s", inst_shrq_imm_reg},   // shrq $imm, %reg
    {"shlq",   enc_reg, enc_reg, "    shlq %s, %s", inst_shlq_cl_reg},    // shlq %cl, %reg (cl is matched as a reg)
    {"testq",  enc_reg, enc_reg, "    testq %s, %s", inst_testq_reg_reg}, // testq %reg, %reg

    // ---- comparison and jumps
    {"cmpq",   enc_imm, enc_reg, "    cmpq %s, %s", inst_cmpq_imm_reg},   // cmpq $imm, %reg
    {"cmpq",   enc_reg, enc_reg, "    cmpq %s, %s", inst_cmpq_reg_reg},   // cmpq %reg, %reg
    {"jmp",    enc_sym, enc_none, "    jmp %s", inst_jmp_sym},            // jmp label
    {"jne",    enc_sym, enc_none, "    jne %s", inst_jne_sym},            // jne label (conditional jumps)
    {"je",     enc_sym, enc_none, "    je %s", inst_je_sym},
    {"jge",    enc_sym, enc_none, "    jge %s", inst_jge_sym},
    {"jg",     enc_sym, enc_none, "    jg %s", inst_jg_sym},
    {"jle",    enc_sym, enc_none, "    jle %s", inst_jle_sym},
    {"jl",     enc_sym, enc_none, "    jl %s", inst_jl_sym},
    {"jae",    enc_sym, enc_none, "    jae %s", inst_jae_sym},
    {"ja",     enc_sym, enc_none, "    ja %s", inst_ja_sym},
    {"jbe",    enc_sym, enc_none, "    jbe %s", inst_jbe_sym},
    {"jb",     enc_sym, enc_none, "    jb %s", inst_jb_sym},
    {"jz",     enc_sym, enc_none, "    jz %s", inst_jz_sym},
    {"jnz",    enc_sym, enc_none, "    jnz %s", inst_jnz_sym},

    // ---- conditional sets (note: these typically use 8-bit registers, but we match against any reg for simplicity)
    {"sete",   enc_reg, enc_none, "    sete %s", inst_sete_reg},          // sete %reg8
    {"setne",  enc_reg, enc_none, "    setne %s", inst_setne_reg},        // setne %reg8
    {"sets",   enc_reg, enc_none, "    sets %s", inst_sets_reg},          // sets %reg8
    {"setns",  enc_reg, enc_none, "    setns %s", inst_setns_reg},        // setns %reg8

    // ---- conditional moves
    {"cmoveq", enc_reg, enc_reg, "    cmoveq %s, %s", inst_cmoveq_reg_reg}, // cmoveq %reg, %reg
    {"cmovneq",enc_reg, enc_reg, "    cmovneq %s, %s", inst_cmovneq_reg_reg}, // cmovneq %reg, %reg
    {"cmovgq", enc_reg, enc_reg, "    cmovgq %s, %s", inst_cmovg_reg_reg}, // cmovgq %reg, %reg
    {"cmovlq", enc_reg, enc_reg, "    cmovlq %s, %s", inst_cmovl_reg_reg}, // cmovlq %reg, %reg

    // ---- system and flags
    {"syscall",enc_none, enc_none, "    syscall", inst_syscall},          // syscall
    {"nop",    enc_none, enc_none, "    nop", inst_nop},                  // nop
    {"pushfq", enc_none, enc_none, "    pushfq", inst_pushfq},            // push flags
    {"popfq",  enc_none, enc_none, "    popfq", inst_popfq},              // pop flags

    // end sentinel
    {NULL, enc_none, enc_none, NULL, inst_unknown}
};

/* helper: map opkind to encoding type for matching */
static operandencodingtype opkind_to_enc(opkind k) {
    switch (k) {
        case op_imm: return enc_imm;
        case op_reg: return enc_reg;
        case op_mem: return enc_mem;
        case op_sym: return enc_sym;
        default: return enc_none;
    }
}

/* find rule by mnemonic and operand types */
static const asmencodingrule *find_encoding_rule(const char *mn, operand **ops, int n) {
    operandencodingtype src_enc = (n > 0) ? opkind_to_enc(ops[0]->kind) : enc_none;
    operandencodingtype dst_enc = (n > 1) ? opkind_to_enc(ops[1]->kind) : enc_none;

    const asmencodingrule *rule = instruction_table;
    while (rule->mnemonic) {
        if (strcasecmp(rule->mnemonic, mn) == 0) {
            if (n == 0) {
                if (rule->src_enc == enc_none && rule->dst_enc == enc_none) return rule;
            } else if (n == 1) {
                if (rule->src_enc == src_enc && rule->dst_enc == enc_none) return rule;
            } else if (n == 2) {
                if (rule->src_enc == src_enc && rule->dst_enc == dst_enc) return rule;
            }
        }
        ++rule;
    }
    return NULL; // no specific rule found
}

/* ----------------------
   ir (intermediate representation)
   ---------------------- */

typedef enum { ir_nop, ir_label, ir_dir, ir_instr } irop;

typedef struct {
    irop op;
    char *text;
} ir;

typedef struct {
    ir *items;
    int count;
    int cap;
} irlist;

static void ir_init(irlist *i) {
    i->cap = init_ir_cap;
    i->count = 0;
    i->items = xmalloc(sizeof(ir) * i->cap);
}

static void ir_emit(irlist *i, irop op, const char *text) {
    if (i->count + 1 >= i->cap) { i->cap *= 2; i->items = xrealloc(i->items, sizeof(ir) * i->cap); }
    ir *it = &i->items[i->count++];
    it->op = op;
    it->text = text ? xstrdup_safe(text) : NULL;
}

static void ir_free(irlist *i) {
    for (int idx=0;idx<i->count;++idx) {
        if (i->items[idx].text) free(i->items[idx].text);
    }
    free(i->items);
}

/* ----------------------
   parser
   ---------------------- */

static operand *parse_operand(lexer *l) {
    operand *o = xmalloc(sizeof(operand));
    memset(o, 0, sizeof(operand));
    o->scale = 1; // default scale

    if (match_token(l, tok_dollar)) {
        // immediate: $num or $sym
        if (l->cur.kind == tok_num) {
            o->kind = op_imm;
            o->imm = (long long)l->cur.val;
            lexer_advance(l);
        } else if (l->cur.kind == tok_id) {
            o->kind = op_sym;
            o->sym = xstrdup_safe(l->cur.text);
            lexer_advance(l);
        } else {
            lexer_error(l, "expected number or symbol after '$'");
        }
    } else if (match_token(l, tok_percent)) {
        // register: %reg
        if (l->cur.kind == tok_id) {
            o->kind = op_reg;
            o->reg = xstrdup_safe(l->cur.text);
            lexer_advance(l);
        } else {
            lexer_error(l, "expected register name after '%'");
        }
    } else {
        // memory or symbol (base/index/disp)
        int is_mem = 0;

        // try to parse displacement (optional)
        if (l->cur.kind == tok_num) {
            o->disp = (long long)l->cur.val;
            lexer_advance(l);
            is_mem = 1;
        } else if (l->cur.kind == tok_id && lexer_peek(l).kind == tok_lparen) {
            // symbol(%base,...)
            o->kind = op_sym;
            o->sym = xstrdup_safe(l->cur.text);
            lexer_advance(l);
            is_mem = 1; // if it's symbol(..., then it's memory)
        }

        // memory addressing: (base, index, scale)
        if (match_token(l, tok_lparen)) {
            is_mem = 1;
            o->kind = op_mem;

            // 1. base register (optional)
            if (match_token(l, tok_percent) && l->cur.kind == tok_id) {
                o->base = xstrdup_safe(l->cur.text);
                lexer_advance(l);
            }

            // 2. comma and index register (optional)
            if (match_token(l, tok_comma)) {
                if (match_token(l, tok_percent) && l->cur.kind == tok_id) {
                    o->index = xstrdup_safe(l->cur.text);
                    lexer_advance(l);
                }
            } else if (o->base) {
                // if we parsed a base but no comma, we are done with (base)
                goto end_mem_parsing;
            }

            // 3. comma and scale (optional)
            if (match_token(l, tok_comma)) {
                if (l->cur.kind == tok_num) {
                    o->scale = (int)l->cur.val;
                    if (o->scale != 1 && o->scale != 2 && o->scale != 4 && o->scale != 8) {
                        lexer_error(l, "invalid scale factor (must be 1, 2, 4, or 8)");
                    }
                    lexer_advance(l);
                }
            }

        end_mem_parsing:
            expect_token(l, tok_rparen);

            // fixup: if we only saw a displacement before the paren, and no base/index,
            // it's a simple memory access to that address, which is already op_mem.
        }

        // if no disp/symbol and no lparen, check if it was a standalone symbol
        if (!is_mem && l->cur.kind == tok_id) {
            o->kind = op_sym;
            o->sym = xstrdup_safe(l->cur.text);
            lexer_advance(l);
        } else if (!is_mem && (o->disp != 0 || o->sym)) {
            // it was a displacement or a symbol without parens, treat as op_sym or op_mem(just disp)
            if (!o->sym) o->kind = op_mem;
            else o->kind = op_sym;
        } else if (!is_mem) {
            // must have been a failed parse
            op_free(o);
            return NULL;
        }
    }
    return o;
}

static astnode *parse_instruction(lexer *l, int *adv) {
    if (l->cur.kind != tok_id) return NULL;

    astnode *n = xmalloc(sizeof(astnode));
    memset(n, 0, sizeof(astnode));
    n->kind = ast_instr;
    n->name = xstrdup_safe(l->cur.text);
    lexer_advance(l);
    *adv = 1;

    // parse operands (up to 3, separated by commas)
    while (1) {
        operand *o = parse_operand(l);
        if (o) {
            add_operand(n, o);
        } else {
            break;
        }

        if (!match_token(l, tok_comma)) break;
    }

    return n;
}

static astnode *parse_label(lexer *l, int *adv) {
    if (l->cur.kind != tok_id || lexer_peek(l).kind != tok_colon) return NULL;

    astnode *n = xmalloc(sizeof(astnode));
    memset(n, 0, sizeof(astnode));
    n->kind = ast_label;
    n->name = xstrdup_safe(l->cur.text);
    lexer_advance(l); // consume id
    expect_token(l, tok_colon); // consume colon
    *adv = 1;
    return n;
}

static astnode *parse_directive(lexer *l, int *adv) {
    if (l->cur.kind != tok_dir) return NULL;

    astnode *n = xmalloc(sizeof(astnode));
    memset(n, 0, sizeof(astnode));
    n->kind = ast_directive;
    n->name = xstrdup_safe(l->cur.text);
    lexer_advance(l); // consume directive
    *adv = 1;

    // directives typically have arguments, we parse them as generic operands
    while (l->cur.kind != tok_eof && l->cur.kind != tok_newline && l->cur.kind != tok_colon) {
        operand *o = parse_operand(l);
        if (o) {
            add_operand(n, o);
        } else {
            // if not an operand, try to capture it as a raw string/id value
            if (l->cur.kind == tok_id || l->cur.kind == tok_string) {
                operand *sym_op = xmalloc(sizeof(operand));
                memset(sym_op, 0, sizeof(operand));
                sym_op->kind = op_sym;
                sym_op->sym = xstrdup_safe(l->cur.text);
                add_operand(n, sym_op);
                lexer_advance(l);
            } else {
                break;
            }
        }
        if (!match_token(l, tok_comma)) break;
    }

    return n;
}

static astnode *parse_top_entry_asm(lexer *l, int *adv) {
    astnode *n = NULL;
    *adv = 0;

    // try to parse label
    if ((n = parse_label(l, adv))) return n;

    // try to parse directive
    if ((n = parse_directive(l, adv))) return n;

    // try to parse instruction (must be last because it uses tok_id which is used by label)
    if ((n = parse_instruction(l, adv))) return n;

    // handle unexpected tokens/newlines
    if (match_token(l, tok_newline)) { *adv = 1; return NULL; }

    return NULL;
}


/* ----------------------
   lowerer (ast -> ir)
   ---------------------- */

typedef struct {
    char *input_path;
    char *output_path;
    int debug;
    int sse;
} compileoptions;

typedef struct {
    compileoptions opts;
    // symbol table, macro table etc. would go here
} compilecontext;


static void lower_asm_astnode(irlist *i, astnode *n) {
    if (!n) return;

    if (n->kind == ast_label) {
        char buf[256]; snprintf(buf, sizeof(buf), "%s:", n->name);
        ir_emit(i, ir_label, buf);
    } else if (n->kind == ast_directive) {
        // procedural handling of directives
        size_t cap = 256; char *buf = xmalloc(cap); buf[0]=0;
        strcat(buf, n->name);
        if (n->op_count > 0) {
            strcat(buf, " ");
            for (int idx=0;idx<n->op_count;++idx) {
                char tmp[128]; format_operand_buf(tmp, sizeof(tmp), n->operands[idx]);
                // directives often strip the $, so we'll do that here heuristically
                if (tmp[0] == '$') strcat(buf, tmp+1);
                else strcat(buf, tmp);
                if (idx+1 < n->op_count) strcat(buf, ", ");
            }
        }
        ir_emit(i, ir_dir, buf);
        free(buf);
    } else if (n->kind == ast_instr) {
        // table-driven lookup
        const asmencodingrule *rule = find_encoding_rule(n->name, n->operands, n->op_count);

        if (rule) {
            size_t cap = 256; char *buf = xmalloc(cap); buf[0]=0;
            char op1_str[128], op2_str[128];
            op1_str[0]=0; op2_str[0]=0;

            if (n->op_count > 0) format_operand_buf(op1_str, sizeof(op1_str), n->operands[0]);
            if (n->op_count > 1) format_operand_buf(op2_str, sizeof(op2_str), n->operands[1]);

            // reconstruct instruction using rule->asm_pattern
            if (n->op_count == 2) {
                // remove '$' for immediates on display consistency
                const char *src = (op1_str[0] == '$') ? (op1_str + 1) : op1_str;
                snprintf(buf, cap, rule->asm_pattern, src, op2_str);
            } else if (n->op_count == 1) {
                const char *src = (op1_str[0] == '$') ? (op1_str + 1) : op1_str;
                snprintf(buf, cap, rule->asm_pattern, src);
            } else {
                snprintf(buf, cap, "%s", rule->asm_pattern);
            }

            // append id
            {
                char idbuf[64];
                size_t used = strlen(buf);
                snprintf(idbuf, sizeof(idbuf), " /* id: %d */", rule->opcode_id);
                if (used + strlen(idbuf) + 1 >= cap) {
                    cap = used + strlen(idbuf) + 1;
                    buf = xrealloc(buf, cap);
                }
                strcat(buf, idbuf);
            }

            ir_emit(i, ir_instr, buf);
            free(buf);
        } else {
            // no rule found: original textual reconstruction
            size_t cap = 256; char *buf = xmalloc(cap); buf[0]=0;
            strcat(buf, n->name);
            if (n->op_count > 0) {
                strcat(buf, " ");
                for (int idx=0;idx<n->op_count;++idx) {
                    char tmp[256]; format_operand_buf(tmp, sizeof(tmp), n->operands[idx]);
                    // trim '$' for immediate for consistency with directives
                    const char *src = (tmp[0] == '$') ? (tmp + 1) : tmp;
                    size_t need = strlen(buf) + strlen(src) + 3;
                    if (need >= cap) { cap = need + 64; buf = xrealloc(buf, cap); }
                    strcat(buf, src);
                    if (idx+1 < n->op_count) strcat(buf, ", ");
                }
            }
            ir_emit(i, ir_instr, buf);
            free(buf);
        }
    }
}

/* ----------------------
   emitter
   ---------------------- */

// section manager skeleton
typedef struct {
    FILE *out;
    char *current_section;
} sectionmanager;

static void section_init(sectionmanager *s, FILE *out) {
    s->out = out;
    s->current_section = xstrdup_safe(".text");
}

static void section_switch(sectionmanager *s, const char *new_sec) {
    if (strcmp(s->current_section, new_sec) != 0) {
        emitf(s->out, "\n    .section %s", new_sec);
        free(s->current_section);
        s->current_section = xstrdup_safe(new_sec);
    }
}

static void section_free(sectionmanager *s) {
    free(s->current_section);
}

// emitter logic (simplified stack optimization from the original prototype's spirit)
static void emit_add_stack(FILE *out) {
    emitf(out, "    popq %%rbx");
    emitf(out, "    popq %%rax");
    emitf(out, "    addq %%rbx, %%rax");
    emitf(out, "    pushq %%rax");
}

static void emit_sub_stack(FILE *out) {
    emitf(out, "    popq %%rbx");
    emitf(out, "    popq %%rax");
    emitf(out, "    subq %%rbx, %%rax");
    emitf(out, "    pushq %%rax");
}


static void emit_ir_as_asm(irlist *i, compilecontext *ctx, FILE *out) {
    sectionmanager s; section_init(&s, out);

    for (int idx = 0; idx < i->count; ++idx) {
        ir *it = &i->items[idx];
        if (ctx->opts.debug) emitf(out, "  /* ir[%d] op=%d text=%s */", idx, it->op, it->text?it->text:"");

        switch (it->op) {
            case ir_label: emit_raw(out, it->text ? it->text : "<label>:"); break;
            case ir_dir:
                // very simple section switching for directives
                if (it->text && strncmp(it->text, ".section ", 9) == 0) {
                    char *sec = strchr(it->text, ' ');
                    if (sec) { sec++; section_switch(&s, sec); break; }
                }
                emitf(out, "    %s", it->text ? it->text : "");
                break;
            case ir_instr:
                // re-apply old stack logic for add/sub for fun, otherwise emit as-is
                if (it->text && strstr(it->text, "popq") && strstr(it->text, "addq")) {
                    emit_add_stack(out);
                } else if (it->text && strstr(it->text, "popq") && strstr(it->text, "subq")) {
                    emit_sub_stack(out);
                } else {
                    emit_raw(out, it->text ? it->text : "");
                }
                break;
            default: break;
        }
    }
    section_free(&s);
}

/* ----------------------
   main
   ---------------------- */

static void print_usage(const char *progname) {
    fprintf(stderr, "usage: %s [-i input.s] [-o output.s] [-debug]\n", progname);
}

const char *demo_asm =
    ".section .data\n"
    "msg: .quad 1, 2, 3\n"
    "size: .quad 24\n"
    "\n"
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "    movq $1, %rax\n"
    "    movq msg, %rdi\n"
    "    lea 8(%rsp), %rbx\n"
    "    pushq $10\n"
    "    popq %rcx\n"
    "    addq %rdi, %rax\n"
    "    subq $5, %rcx\n"
    "    imulq %rbx, %rax\n"
    "    cmpq $100, %rax\n"
    "    jg label_exit\n"
    "label_loop:\n"
    "    incq %rbx\n"
    "    pushq %rbx\n"
    "    popq %rax\n"
    "    jmp label_loop\n"
    "label_exit:\n"
    "    syscall\n"
    "    ret\n";

int main(int argc, char **argv) {
    compilecontext ctx;
    memset(&ctx, 0, sizeof(compilecontext));
    ctx.opts.output_path = xstrdup_safe("a.s");

    for (int i=1;i<argc;++i) {
        if (strcmp(argv[i], "-i") == 0) {
            if (i+1 < argc) ctx.opts.input_path = argv[++i];
            else { print_usage(argv[0]); return 1; }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i+1 < argc) { free(ctx.opts.output_path); ctx.opts.output_path = xstrdup_safe(argv[++i]); }
            else { print_usage(argv[0]); return 1; }
        } else if (strcmp(argv[i], "-debug") == 0) {
            ctx.opts.debug = 1;
        } else if (strcmp(argv[i], "-sse") == 0) {
            ctx.opts.sse = 1;
        }
        else { print_usage(argv[0]); return 1; }
    }

    char *source = NULL;
    if (ctx.opts.input_path) {
        FILE *f = fopen(ctx.opts.input_path, "rb");
        if (!f) { perror("fopen"); return 1; }
        if (fseek(f, 0, SEEK_END) != 0) { perror("fseek"); fclose(f); return 1; }
        long sz = ftell(f);
        if (sz < 0) { perror("ftell"); fclose(f); return 1; }
        if (fseek(f, 0, SEEK_SET) != 0) { perror("fseek"); fclose(f); return 1; }
        source = xmalloc((size_t)sz + 1);
        if (fread(source, 1, (size_t)sz, f) != (size_t)sz) { perror("fread"); fclose(f); free(source); return 1; }
        source[sz] = 0; fclose(f);
    } else {
        source = xstrdup_safe(demo_asm);
    }

    /* lex and parse top-level assembler lines */
    lexer l; lexer_init(&l, source);
    lexer_advance(&l);
    astnode **nodes = xmalloc(sizeof(astnode*) * 4096);
    int ncap = 4096, ncount = 0;
    while (l.cur.kind != tok_eof) {
        int adv = 0;
        astnode *n = parse_top_entry_asm(&l, &adv);
        if (n) {
            if (ncount >= ncap) { ncap *= 2; nodes = xrealloc(nodes, sizeof(astnode*) * ncap); }
            nodes[ncount++] = n;
        } else {
            if (!adv) lexer_advance(&l); // force advance to avoid loop
        }
    }

    /* lower ast to ir */
    irlist i; ir_init(&i);
    for (int idx=0;idx<ncount;++idx) {
        lower_asm_astnode(&i, nodes[idx]);
    }

    /* emit ir as assembly */
    FILE *out = fopen(ctx.opts.output_path, "w");
    if (!out) { perror("fopen output"); return 1; }

    emitf(out, "/* output generated by jas.c (table-driven prototype) */\n");
    emit_ir_as_asm(&i, &ctx, out);

    fclose(out);

    /* cleanup */
    for (int idx=0;idx<ncount;++idx) ast_free(nodes[idx]);
    free(nodes);
    ir_free(&i);
    free(source);
    if (ctx.opts.output_path) free(ctx.opts.output_path);

    return 0;
}
