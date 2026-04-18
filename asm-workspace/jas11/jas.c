/*
 * att_parser_128.c
 *
 * Minimaler AT&T assembler parser demo:
 * - erkennt 128 häufige Direktiven/Instruktionen (Lookup)
 * - lexed labels, directives, instructions and operands
 * - baut ein kleines AST (LABEL, DIRECTIVE, INSTR)
 * - gibt AST strukturiert aus
 *
 * Build:
 *   gcc -O2 att_parser_128.c -o att_parser_128
 *
 * Hinweis: Operanden werden als rohe Strings gehalten; detailliertes Operand-Parsing
 * (Register vs. immediate vs. memory addressing) kann ergänzt werden.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ----------------------
   Utilities
   ---------------------- */
static void *xmalloc(size_t n) { void *p = malloc(n); if (!p) { fprintf(stderr,"fatal: oom\n"); exit(1);} return p; }
static char *xstrdup_safe(const char *s) { if (!s) return NULL; char *r = strdup(s); if (!r) { fprintf(stderr,"fatal: oom\n"); exit(1);} return r; }

/* ----------------------
   128 common AT&T directives + mnemonics
   (combined list: directives first, then mnemonics)
   ---------------------- */
static const char *att_list[128] = {
    /* 1..40 directives (examples, common) */
    ".text", ".data", ".bss", ".globl", ".local", ".section", ".align", ".byte", ".word", ".long",
    ".quad", ".ascii", ".asciz", ".string", ".comm", ".lcomm", ".type", ".size", ".p2align", ".file",
    ".ident", ".previous", ".weak", ".weakref", ".note", ".balign", ".zero", ".org", ".equ", ".set",
    ".if", ".else", ".endif", ".include", ".macro", ".endm", ".rept", ".endr", ".irp", ".irpc",
    /* 41..128 mnemonics (88 items) - common x86/x86-64 + SSE */
    "mov", "movb", "movw", "movl", "movq", "movabs", "lea", "push", "pushq", "pop",
    "popq", "add", "addb", "addw", "addl", "addq", "sub", "subb", "subw", "subl",
    "subq", "imul", "mul", "idiv", "div", "xor", "xorb", "xorw", "xorl", "xorq",
    "and", "orb", "orw", "orl", "orq", "test", "cmp", "cmpl", "cmpq", "sete",
    "setne", "setz", "setnz", "jg", "jge", "jl", "jle", "je", "jne", "jmp",
    "call", "ret", "nop", "inc", "dec", "sal", "shl", "shr", "sar", "rol",
    "ror", "bsf", "bsr", "bt", "btc", "btr", "bts", "stosb", "stosw", "stosl",
    "stosq", "lodsb", "lodsw", "lodsl", "lodsq", "scasb", "scasw", "scasl", "scasq", "cmovz",
    "cmovnz", "cmove", "cmovne", "cmovg", "cmovl", "cmovge", "cmovle", "movss", "movsd", "addss",
    "addsd", "subss", "subsd", "mulss", "mulsd", "divss", "divsd", "ucomiss", "ucomisd", "comiss",
    "comisd", "cvtsi2sd", "cvtsi2ss", "cvttsd2si", "cvttss2si", "fld", "fstp", "fadd", "fsub",
    "fmul", "fdiv", "syscall", "sysenter", "int", "hlt", "pause", "lfence", "sfence", "mfence"
};

/* helper: check if token is in list */
static int is_att_token(const char *s) {
    if (!s) return 0;
    for (int i = 0; i < 128; ++i) if (strcmp(att_list[i], s) == 0) return 1;
    return 0;
}

/* ----------------------
   Lexer
   ---------------------- */
typedef enum { TK_EOF, TK_IDENT, TK_NUM, TK_STR, TK_SYM, TK_LABEL, TK_DIR } TokKind;
typedef struct { TokKind kind; char *text; int line; } Token;
typedef struct { const char *src; size_t pos; int line; Token cur; } Lexer;

static void lexer_init(Lexer *L, const char *src) {
    L->src = src; L->pos = 0; L->line = 1;
    L->cur.kind = TK_EOF; L->cur.text = NULL; L->cur.line = 1;
}
static char lex_peek(Lexer *L) { return L->src[L->pos]; }
static char lex_next(Lexer *L) {
    char c = L->src[L->pos];
    if (c) { L->pos++; if (c == '\n') L->line++; }
    return c;
}
static void lex_skip(Lexer *L) {
    for (;;) {
        char c = lex_peek(L);
        if (!c) return;
        if (isspace((unsigned char)c)) { lex_next(L); continue; }
        if (c == '#') { /* comment to EOL */ while (lex_peek(L) && lex_next(L) != '\n'); continue; }
        if (c == ';') { while (lex_peek(L) && lex_next(L) != '\n'); continue; }
        break;
    }
}
static char *lex_make(Lexer *L, size_t st, size_t en) {
    size_t len = en - st; char *s = xmalloc(len + 1); memcpy(s, L->src + st, len); s[len] = 0; return s;
}
static void lexer_advance(Lexer *L) {
    if (L->cur.text) { free(L->cur.text); L->cur.text = NULL; }
    lex_skip(L);
    char c = lex_peek(L);
    if (!c) { L->cur.kind = TK_EOF; L->cur.text = NULL; L->cur.line = L->line; return; }
    /* directive .name */
    if (c == '.') {
        size_t st = L->pos; lex_next(L);
        while (isalnum((unsigned char)lex_peek(L)) || lex_peek(L) == '_' ) lex_next(L);
        size_t en = L->pos;
        L->cur.kind = TK_DIR; L->cur.text = lex_make(L, st, en); L->cur.line = L->line; return;
    }
    /* identifier or label */
    if (isalpha((unsigned char)c) || c == '_' || c == '$' || c == '%') {
        size_t st = L->pos; lex_next(L);
        while (isalnum((unsigned char)lex_peek(L)) || lex_peek(L) == '_' || lex_peek(L) == '.' || lex_peek(L) == '%' || lex_peek(L) == '$' ) lex_next(L);
        size_t en = L->pos;
        /* label if next char is ':' */
        if (lex_peek(L) == ':') {
            char *t = lex_make(L, st, en);
            lex_next(L); /* consume ':' */
            L->cur.kind = TK_LABEL; L->cur.text = t; L->cur.line = L->line; return;
        }
        L->cur.kind = TK_IDENT; L->cur.text = lex_make(L, st, en); L->cur.line = L->line; return;
    }
    /* number */
    if (isdigit((unsigned char)c) || (c == '-' && isdigit((unsigned char)L->src[L->pos+1]))) {
        size_t st = L->pos;
        if (L->src[L->pos] == '0' && (L->src[L->pos+1] == 'x' || L->src[L->pos+1] == 'X')) { lex_next(L); lex_next(L); while (isxdigit((unsigned char)lex_peek(L))) lex_next(L); }
        else { if (lex_peek(L) == '-') lex_next(L); while (isdigit((unsigned char)lex_peek(L))) lex_next(L); }
        size_t en = L->pos;
        L->cur.kind = TK_NUM; L->cur.text = lex_make(L, st, en); L->cur.line = L->line; return;
    }
    /* string */
    if (c == '"') {
        lex_next(L);
        size_t st = L->pos;
        while (lex_peek(L) && lex_peek(L) != '"') {
            if (lex_peek(L) == '\\') { lex_next(L); if (lex_peek(L)) lex_next(L); }
            else lex_next(L);
        }
        size_t en = L->pos;
        if (lex_peek(L) == '"') lex_next(L);
        L->cur.kind = TK_STR; L->cur.text = lex_make(L, st, en); L->cur.line = L->line; return;
    }
    /* symbol (comma, parentheses, colon, etc.) */
    size_t st = L->pos; lex_next(L);
    L->cur.kind = TK_SYM; L->cur.text = lex_make(L, st, L->pos); L->cur.line = L->line;
}

/* ----------------------
   AST types
   ---------------------- */
typedef enum { AST_LABEL, AST_DIRECTIVE, AST_INSTR } ASTKind;
typedef struct {
    ASTKind kind;
    char *name;      /* label name / directive name / mnemonic */
    char **operands; /* for directive or instr: array of operand strings */
    int op_count;
    int line;
} ASTNode;

static ASTNode *ast_label_new(const char *name, int line) {
    ASTNode *n = xmalloc(sizeof(ASTNode)); memset(n,0,sizeof(ASTNode));
    n->kind = AST_LABEL; n->name = xstrdup_safe(name); n->line = line; return n;
}
static ASTNode *ast_dir_new(const char *name, char **ops, int opn, int line) {
    ASTNode *n = xmalloc(sizeof(ASTNode)); memset(n,0,sizeof(ASTNode));
    n->kind = AST_DIRECTIVE; n->name = xstrdup_safe(name); n->operands = ops; n->op_count = opn; n->line = line; return n;
}
static ASTNode *ast_instr_new(const char *mn, char **ops, int opn, int line) {
    ASTNode *n = xmalloc(sizeof(ASTNode)); memset(n,0,sizeof(ASTNode));
    n->kind = AST_INSTR; n->name = xstrdup_safe(mn); n->operands = ops; n->op_count = opn; n->line = line; return n;
}
static void ast_free(ASTNode *n) {
    if (!n) return;
    if (n->name) free(n->name);
    if (n->operands) {
        for (int i=0;i<n->op_count;++i) if (n->operands[i]) free(n->operands[i]);
        free(n->operands);
    }
    free(n);
}

/* ----------------------
   Parser for top-level assembly lines
   ---------------------- */
static char *trim(char *s) {
    if (!s) return NULL;
    while (*s && isspace((unsigned char)*s)) s++;
    char *e = s + strlen(s) - 1;
    while (e >= s && isspace((unsigned char)*e)) { *e = 0; e--; }
    return s;
}

/* parse a comma-separated operand list from current lexer position (rest of line) */
static char **parse_operands_from_text(const char *text, int *out_count) {
    /* split by commas, but keep whitespace inside tokens */
    char *tmp = xstrdup_safe(text);
    int cap = 8, cnt = 0;
    char **arr = xmalloc(sizeof(char*) * cap);
    char *p = tmp;
    while (*p) {
        /* skip leading spaces */
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        char *start = p;
        int depth = 0;
        while (*p) {
            if (*p == '(') depth++;
            else if (*p == ')') depth--;
            if (*p == ',' && depth == 0) break;
            p++;
        }
        size_t len = p - start;
        char *tok = xmalloc(len + 1);
        memcpy(tok, start, len); tok[len] = 0;
        char *ttrim = trim(tok);
        if (cnt >= cap) { cap *= 2; arr = realloc(arr, sizeof(char*) * cap); }
        arr[cnt++] = xstrdup_safe(ttrim);
        free(tok);
        if (*p == ',') p++;
    }
    free(tmp);
    *out_count = cnt;
    return arr;
}

/* parse a single top-level entry: directive, label, or instruction */
static ASTNode *parse_top_entry(Lexer *L) {
    Token t = L->cur;
    if (t.kind == TK_EOF) return NULL;
    /* label */
    if (t.kind == TK_LABEL) {
        char *name = xstrdup_safe(t.text);
        lexer_advance(L);
        return ast_label_new(name, t.line);
    }
    /* directive .name ... (rest of line as operands) */
    if (t.kind == TK_DIR) {
        char *dname = xstrdup_safe(t.text);
        /* collect rest of line until newline or EOF */
        size_t st = L->pos;
        /* find end of line */
        while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
        size_t en = L->pos;
        char *rest = lex_make(L, st, en);
        lexer_advance(L); /* move to next token */
        int opn = 0;
        char **ops = NULL;
        char *rtrim = trim(rest);
        if (rtrim && *rtrim) ops = parse_operands_from_text(rtrim, &opn);
        free(rest);
        ASTNode *n = ast_dir_new(dname, ops, opn, t.line);
        free(dname);
        return n;
    }
    /* instruction or identifier line */
    if (t.kind == TK_IDENT) {
        char *mn = xstrdup_safe(t.text);
        lexer_advance(L);
        /* collect rest of line as operand text */
        size_t st = L->pos;
        while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
        size_t en = L->pos;
        char *rest = lex_make(L, st, en);
        lexer_advance(L);
        char *rtrim = trim(rest);
        int opn = 0;
        char **ops = NULL;
        if (rtrim && *rtrim) ops = parse_operands_from_text(rtrim, &opn);
        /* check if mnemonic is in our 128-list; if not, still create instr node but mark name as-is */
        ASTNode *n = ast_instr_new(mn, ops, opn, t.line);
        free(rest);
        return n;
    }
    /* other tokens: skip line */
    /* consume until newline */
    while (L->src[L->pos] && L->src[L->pos] != '\n') L->pos++;
    lexer_advance(L);
    return NULL;
}

/* ----------------------
   Demo parse driver
   ---------------------- */
static const char *demo_asm =
".text\n"
".globl main\n"
"start_label:\n"
"    pushq %rbp\n"
"    movq %rsp, %rbp\n"
"    subq $16, %rsp\n"
"    movl $5, -4(%rbp)\n"
"    movl $3, -8(%rbp)\n"
"    movl -4(%rbp), %eax\n"
"    addl -8(%rbp), %eax\n"
"    movl %eax, -12(%rbp)\n"
"    call printf\n"
"    addq $16, %rsp\n"
"    popq %rbp\n"
"    ret\n"
".data\n"
"msg: .asciz \"Result: %d\\n\"\n";

int main(void) {
    Lexer L; lexer_init(&L, demo_asm);
    lexer_advance(&L);

    /* parse all top-level entries */
    ASTNode **nodes = xmalloc(sizeof(ASTNode*) * 512);
    int ncap = 512, ncount = 0;
    while (L.cur.kind != TK_EOF) {
        ASTNode *n = parse_top_entry(&L);
        if (n) {
            if (ncount >= ncap) { ncap *= 2; nodes = realloc(nodes, sizeof(ASTNode*) * ncap); }
            nodes[ncount++] = n;
        }
    }

    /* print AST */
    for (int i = 0; i < ncount; ++i) {
        ASTNode *n = nodes[i];
        if (!n) continue;
        if (n->kind == AST_LABEL) {
            printf("LABEL (line %d): %s\n", n->line, n->name);
        } else if (n->kind == AST_DIRECTIVE) {
            printf("DIRECTIVE (line %d): %s", n->line, n->name);
            if (n->op_count > 0) {
                printf("  operands:");
                for (int j=0;j<n->op_count;++j) printf(" [%s]", n->operands[j]);
            }
            printf("\n");
        } else if (n->kind == AST_INSTR) {
            /* check if mnemonic is in our 128-list */
            int known = is_att_token(n->name);
            printf("INSTR (line %d): %s%s", n->line, n->name, known ? " (known)" : " (unknown)");
            if (n->op_count > 0) {
                printf("  operands:");
                for (int j=0;j<n->op_count;++j) printf(" [%s]", n->operands[j]);
            }
            printf("\n");
        }
    }

    /* cleanup */
    for (int i=0;i<ncount;++i) ast_free(nodes[i]);
    free(nodes);
    return 0;
}

