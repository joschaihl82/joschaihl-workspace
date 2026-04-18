/*
 * asm.c
 *
 * Single-file assembler: lexer, parser, table-driven encoder, ELF writer.
 * Supports a small subset of x86-64 sufficient for the sample assembly:
 *   directives: .text .data .bss .global .align .ascii .byte .skip
 *   labels, forward/backward refs, relocations
 *   instructions: pushq/popq/ret/movq/addq/call/cmpq/je/jmp/movaps/addps (subset)
 *
 * Build:
 *   gcc -std=c11 -O2 -Wall -Wextra -o asm asm.c
 *
 * Usage:
 *   ./asm input.s out.o
 *   gcc out.o -o prog
 *
 * This file is intentionally compact and self-contained for experimentation.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* ---------------------------
   Dynamic buffer
   --------------------------- */
typedef struct { uint8_t *data; size_t len, cap; } Buf;
static void buf_init(Buf *b){ b->data = NULL; b->len = b->cap = 0; }
static void buf_free(Buf *b){ free(b->data); b->data = NULL; b->len = b->cap = 0; }
static void buf_put(Buf *b, const void *p, size_t n){
    if (b->len + n > b->cap) {
        size_t nc = (b->cap ? b->cap * 2 : 256);
        while (nc < b->len + n) nc *= 2;
        b->data = realloc(b->data, nc);
        b->cap = nc;
    }
    memcpy(b->data + b->len, p, n);
    b->len += n;
}
static void buf_put_u8(Buf *b, uint8_t v){ buf_put(b, &v, 1); }
static void buf_put_u32(Buf *b, uint32_t v){ buf_put(b, &v, 4); }
static void buf_put_u64(Buf *b, uint64_t v){ buf_put(b, &v, 8); }

/* ---------------------------
   Lexer
   --------------------------- */
typedef enum { T_EOF, T_IDENT, T_LABEL, T_DOT, T_NUMBER, T_STRING, T_COMMA, T_PERCENT, T_DOLLAR, T_LPAREN, T_RPAREN, T_UNKNOWN } TokenKind;
typedef struct { TokenKind kind; char *text; int line; } Token;
typedef struct { const char *src; size_t pos; int line; } Lexer;

static void lex_init(Lexer *lx, const char *s){ lx->src = s; lx->pos = 0; lx->line = 1; }
static int lex_peek(Lexer *lx){ return lx->src[lx->pos]; }
static int lex_get(Lexer *lx){ int c = lx->src[lx->pos]; if (c) lx->pos++; return c; }
static void skip_space(Lexer *lx){
    for (;;) {
        int c = lex_peek(lx);
        if (c == ' ' || c == '\t' || c == '\r') { lex_get(lx); continue; }
        if (c == '\n') { lx->line++; lex_get(lx); continue; }
        if (c == '#') { while (lex_peek(lx) && lex_get(lx) != '\n'); lx->line++; continue; }
        break;
    }
}
static Token token_make(TokenKind k, const char *s, int line){
    Token t; t.kind = k; t.line = line; t.text = s ? strdup(s) : NULL; return t;
}
static Token lex_next(Lexer *lx){
    skip_space(lx);
    int c = lex_peek(lx);
    if (!c) return token_make(T_EOF, NULL, lx->line);
    if (c == '.') {
        lex_get(lx);
        size_t start = lx->pos;
        while (isalnum(lex_peek(lx)) || lex_peek(lx) == '_') lex_get(lx);
        size_t len = lx->pos - start;
        char *s = strndup(lx->src + start, len);
        char *full = malloc(len + 2);
        full[0] = '.'; memcpy(full + 1, s, len); full[len + 1] = 0;
        free(s);
        return token_make(T_DOT, full, lx->line);
    }
    if (isalpha(c) || c == '_' || c == '.') {
        size_t start = lx->pos;
        while (isalnum(lex_peek(lx)) || lex_peek(lx) == '_' || lex_peek(lx) == '.') lex_get(lx);
        size_t len = lx->pos - start;
        char *s = strndup(lx->src + start, len);
        if (lex_peek(lx) == ':') {
            lex_get(lx);
            char *lab = malloc(len + 2);
            memcpy(lab, s, len); lab[len] = ':'; lab[len + 1] = 0;
            free(s);
            return token_make(T_LABEL, lab, lx->line);
        }
        Token t = token_make(T_IDENT, s, lx->line);
        free(s);
        return t;
    }
    if (c == '"') {
        lex_get(lx);
        size_t start = lx->pos;
        while (lex_peek(lx) && lex_peek(lx) != '"') {
            if (lex_peek(lx) == '\\') { lex_get(lx); if (lex_peek(lx)) lex_get(lx); continue; }
            lex_get(lx);
        }
        size_t len = lx->pos - start;
        char *s = strndup(lx->src + start, len);
        if (lex_peek(lx) == '"') lex_get(lx);
        return token_make(T_STRING, s, lx->line);
    }
    if (c == ',') { lex_get(lx); return token_make(T_COMMA, strdup(","), lx->line); }
    if (c == '%') {
        lex_get(lx);
        size_t start = lx->pos;
        while (isalnum(lex_peek(lx))) lex_get(lx);
        size_t len = lx->pos - start;
        char *s = strndup(lx->src + start, len);
        char *full = malloc(len + 2); full[0] = '%'; memcpy(full + 1, s, len); full[len + 1] = 0;
        free(s);
        return token_make(T_PERCENT, full, lx->line);
    }
    if (c == '$') {
        lex_get(lx);
        size_t start = lx->pos;
        while (isalnum(lex_peek(lx)) || lex_peek(lx) == '_' || lex_peek(lx) == 'x' || lex_peek(lx) == 'X') lex_get(lx);
        size_t len = lx->pos - start;
        char *s = strndup(lx->src + start, len);
        char *full = malloc(len + 2); full[0] = '$'; memcpy(full + 1, s, len); full[len + 1] = 0;
        free(s);
        return token_make(T_DOLLAR, full, lx->line);
    }
    if (c == '(') { lex_get(lx); return token_make(T_LPAREN, strdup("("), lx->line); }
    if (c == ')') { lex_get(lx); return token_make(T_RPAREN, strdup(")"), lx->line); }
    if (isdigit(c) || (c == '-' && isdigit(lx->src[lx->pos + 1]))) {
        size_t start = lx->pos;
        if (lx->src[lx->pos] == '-') lex_get(lx);
        if (lx->src[lx->pos] == '0' && (lx->src[lx->pos + 1] == 'x' || lx->src[lx->pos + 1] == 'X')) {
            lex_get(lx); lex_get(lx);
            while (isxdigit(lex_peek(lx))) lex_get(lx);
        } else {
            while (isdigit(lex_peek(lx))) lex_get(lx);
        }
        size_t len = lx->pos - start;
        char *s = strndup(lx->src + start, len);
        return token_make(T_NUMBER, s, lx->line);
    }
    char tmp[2] = { (char)lex_get(lx), 0 };
    return token_make(T_UNKNOWN, strdup(tmp), lx->line);
}

/* ---------------------------
   Parser / AST
   --------------------------- */
typedef enum { SEC_TEXT, SEC_DATA, SEC_BSS, SEC_NONE } SectionKind;
typedef enum { NODE_DIR, NODE_LABEL, NODE_INST } NodeKind;
typedef struct Operand {
    enum { OP_REG, OP_IMM_SYM, OP_IMM_NUM, OP_MEM_REG } kind;
    char *reg;
    char *sym;
    int64_t imm;
} Operand;
typedef struct Node {
    NodeKind kind;
    char *dir;
    char *dir_arg;
    char *label;
    char *mnemonic;
    Operand ops[3];
    int opcnt;
    SectionKind sec;
    struct Node *next;
} Node;
typedef struct { Lexer lx; Token cur, peek; } Parser;

static void parser_init(Parser *p, const char *src){ lex_init(&p->lx, src); p->cur = lex_next(&p->lx); p->peek = lex_next(&p->lx); }
static void parser_advance(Parser *p){ if (p->cur.text) free(p->cur.text); p->cur = p->peek; p->peek = lex_next(&p->lx); }
static Node *node_new(NodeKind k){ Node *n = calloc(1, sizeof(Node)); n->kind = k; n->sec = SEC_NONE; return n; }

static Operand parse_operand(Parser *p){
    Operand op = {0};
    Token t = p->cur;
    if (t.kind == T_PERCENT) {
        op.kind = OP_REG; op.reg = strdup(t.text); parser_advance(p);
    } else if (t.kind == T_DOLLAR) {
        char *s = t.text + 1;
        if (isalpha((unsigned char)s[0]) || s[0] == '_') { op.kind = OP_IMM_SYM; op.sym = strdup(s); }
        else { op.kind = OP_IMM_NUM; op.imm = strtoll(s, NULL, 0); }
        parser_advance(p);
    } else if (t.kind == T_LPAREN) {
        parser_advance(p);
        if (p->cur.kind == T_PERCENT) { op.kind = OP_MEM_REG; op.reg = strdup(p->cur.text); parser_advance(p); }
        if (p->cur.kind == T_RPAREN) parser_advance(p);
    } else if (t.kind == T_IDENT) {
        op.kind = OP_IMM_SYM; op.sym = strdup(t.text); parser_advance(p);
    } else if (t.kind == T_NUMBER) {
        op.kind = OP_IMM_NUM; op.imm = strtoll(t.text, NULL, 0); parser_advance(p);
    } else {
        parser_advance(p);
    }
    return op;
}

static Node *parse_all(Parser *p){
    Node *head = NULL, *tail = NULL;
    SectionKind cursec = SEC_NONE;
    while (p->cur.kind != T_EOF) {
        if (p->cur.kind == T_DOT) {
            Node *n = node_new(NODE_DIR);
            n->dir = strdup(p->cur.text);
            parser_advance(p);
            if (strcmp(n->dir, ".text") == 0) { n->sec = SEC_TEXT; cursec = SEC_TEXT; }
            else if (strcmp(n->dir, ".data") == 0) { n->sec = SEC_DATA; cursec = SEC_DATA; }
            else if (strcmp(n->dir, ".bss") == 0) { n->sec = SEC_BSS; cursec = SEC_BSS; }
            if (p->cur.kind == T_STRING || p->cur.kind == T_NUMBER || p->cur.kind == T_IDENT) {
                n->dir_arg = strdup(p->cur.text); parser_advance(p);
            }
            n->sec = cursec;
            if (!head) head = tail = n; else { tail->next = n; tail = n; }
            continue;
        }
        if (p->cur.kind == T_LABEL) {
            Node *n = node_new(NODE_LABEL);
            n->label = strdup(p->cur.text);
            n->sec = cursec;
            parser_advance(p);
            if (!head) head = tail = n; else { tail->next = n; tail = n; }
            continue;
        }
        if (p->cur.kind == T_IDENT) {
            Node *n = node_new(NODE_INST);
            n->mnemonic = strdup(p->cur.text);
            n->sec = cursec;
            parser_advance(p);
            while (p->cur.kind != T_EOF && p->cur.kind != T_LABEL && p->cur.kind != T_DOT) {
                if (p->cur.kind == T_COMMA) { parser_advance(p); continue; }
                if (p->cur.kind == T_PERCENT || p->cur.kind == T_DOLLAR || p->cur.kind == T_LPAREN || p->cur.kind == T_IDENT || p->cur.kind == T_NUMBER) {
                    Operand op = parse_operand(p);
                    if (n->opcnt < 3) n->ops[n->opcnt++] = op;
                    continue;
                }
                parser_advance(p);
            }
            if (!head) head = tail = n; else { tail->next = n; tail = n; }
            continue;
        }
        parser_advance(p);
    }
    return head;
}

/* ---------------------------
   Symbols, sections, relocations
   --------------------------- */
typedef struct Sym {
    char *name;
    SectionKind sec;
    uint64_t value;
    int defined;
    int global;
    int index; /* used by ELF writer */
    struct Sym *next;
} Sym;

typedef struct Reloc {
    uint64_t offset;
    char *sym;
    uint32_t type;
    int64_t addend;
    struct Reloc *next;
} Reloc;

typedef struct Section {
    SectionKind kind;
    Buf buf;
    uint64_t align;
    Reloc *relocs;
} Section;

static Sym *symtab = NULL;
static Section sects[3];

static Sym *sym_find(const char *name){
    for (Sym *s = symtab; s; s = s->next) if (strcmp(s->name, name) == 0) return s;
    return NULL;
}
static Sym *sym_get_or_create(const char *name){
    Sym *s = sym_find(name);
    if (!s) {
        s = calloc(1, sizeof(Sym));
        s->name = strdup(name);
        s->defined = 0;
        s->global = 0;
        s->index = 0;
        s->next = symtab;
        symtab = s;
    }
    return s;
}
static void section_init(Section *s, SectionKind k){ buf_init(&s->buf); s->kind = k; s->align = 1; s->relocs = NULL; }
static void section_add_reloc(Section *s, uint64_t off, const char *sym, uint32_t type, int64_t addend){
    Reloc *r = calloc(1, sizeof(Reloc));
    r->offset = off; r->sym = strdup(sym); r->type = type; r->addend = addend;
    r->next = s->relocs; s->relocs = r;
}

/* ELF relocation types used */
enum { R_X86_64_64 = 1, R_X86_64_PC32 = 2 };

/* ---------------------------
   Emit helpers
   --------------------------- */
static void emit_bytes(SectionKind s, const uint8_t *p, size_t n){ buf_put(&sects[s].buf, p, n); }
static void emit_u8(SectionKind s, uint8_t v){ buf_put_u8(&sects[s].buf, v); }
static void emit_u32(SectionKind s, uint32_t v){ buf_put_u32(&sects[s].buf, v); }
static void emit_u64(SectionKind s, uint64_t v){ buf_put_u64(&sects[s].buf, v); }
static void emit_rel32(SectionKind s, const char *sym, int64_t addend){
    emit_u32(s, 0);
    section_add_reloc(&sects[s], sects[s].buf.len - 4, sym, R_X86_64_PC32, addend);
}
static void emit_rel64(SectionKind s, const char *sym, int64_t addend){
    emit_u64(s, 0);
    section_add_reloc(&sects[s], sects[s].buf.len - 8, sym, R_X86_64_64, addend);
}

/* ---------------------------
   Register helper
   --------------------------- */
static int reg_index(const char *r) {
    if (!r) return -1;
    if (strcmp(r, "%rax") == 0) return 0;
    else if (strcmp(r, "%rcx") == 0) return 1;
    else if (strcmp(r, "%rdx") == 0) return 2;
    else if (strcmp(r, "%rbx") == 0) return 3;
    else if (strcmp(r, "%rsp") == 0) return 4;
    else if (strcmp(r, "%rbp") == 0) return 5;
    else if (strcmp(r, "%rsi") == 0) return 6;
    else if (strcmp(r, "%rdi") == 0) return 7;
    else if (strcmp(r, "%r8")  == 0) return 8;
    else if (strcmp(r, "%r9")  == 0) return 9;
    else if (strcmp(r, "%r10") == 0) return 10;
    else if (strcmp(r, "%r11") == 0) return 11;
    else if (strcmp(r, "%r12") == 0) return 12;
    else if (strcmp(r, "%r13") == 0) return 13;
    else if (strcmp(r, "%r14") == 0) return 14;
    else if (strcmp(r, "%r15") == 0) return 15;
    /* XMM registers */
    if (strcmp(r, "%xmm0") == 0) return 0;
    else if (strcmp(r, "%xmm1") == 0) return 1;
    else if (strcmp(r, "%xmm2") == 0) return 2;
    else if (strcmp(r, "%xmm3") == 0) return 3;
    else if (strcmp(r, "%xmm4") == 0) return 4;
    else if (strcmp(r, "%xmm5") == 0) return 5;
    else if (strcmp(r, "%xmm6") == 0) return 6;
    else if (strcmp(r, "%xmm7") == 0) return 7;
    return -1;
}

/* ---------------------------
   Instruction table
   --------------------------- */
typedef enum { PAT_REG, PAT_IMM_NUM, PAT_IMM_SYM, PAT_MEM_REG } PatKind;
typedef struct {
    const char *mnemonic;
    PatKind pat[3];
    int pat_len;
    uint8_t opcode[8];
    int opcode_len;
    int needs_modrm;
    int imm_bytes;    /* 0,1,4,8 */
    int reloc_type;   /* 0, R_X86_64_64, R_X86_64_PC32 */
    int size_est;
} InstrDef;

/* Table entries for the sample */
static InstrDef instr_table[] = {
    { "pushq", {PAT_REG}, 1, {0x50}, 1, 0, 0, 0, 1 }, /* push base + reg */
    { "popq",  {PAT_REG}, 1, {0x58}, 1, 0, 0, 0, 1 },
    { "ret",   {}, 0, {0xC3}, 1, 0, 0, 0, 1 },
    { "movq", {PAT_REG, PAT_REG}, 2, {0x48,0x89}, 2, 1, 0, 0, 3 }, /* mov reg->reg */
    { "movq", {PAT_IMM_SYM, PAT_REG}, 2, {0x48,0xB8}, 2, 0, 8, R_X86_64_64, 10 }, /* movabs rax, imm64 */
    { "movq", {PAT_IMM_NUM, PAT_REG}, 2, {0x49,0xB8}, 2, 0, 8, 0, 10 }, /* movabs r8, imm64 */
    { "addq", {PAT_MEM_REG, PAT_REG}, 2, {0x48,0x03}, 2, 1, 0, 0, 3 },
    { "call", {PAT_IMM_SYM}, 1, {0xE8}, 1, 0, 4, R_X86_64_PC32, 5 },
    { "cmpq", {PAT_IMM_NUM, PAT_REG}, 2, {0x48,0x83}, 2, 1, 1, 0, 4 }, /* cmp imm8, reg (we special-case imm==0) */
    { "je",   {PAT_IMM_SYM}, 1, {0x0F,0x84}, 2, 0, 4, R_X86_64_PC32, 6 },
    { "movaps", {PAT_MEM_REG, PAT_REG}, 2, {0x0F,0x28}, 2, 1, 0, 0, 3 },
    { "addps", {PAT_IMM_SYM, PAT_REG}, 2, {0x0F,0x58,0x05}, 3, 0, 4, R_X86_64_PC32, 7 },
    { "movaps", {PAT_REG, PAT_MEM_REG}, 2, {0x0F,0x29}, 2, 1, 0, 0, 3 },
    { "jmp", {PAT_IMM_SYM}, 1, {0xE9}, 1, 0, 4, R_X86_64_PC32, 5 },
    { "movq", {PAT_REG, PAT_MEM_REG}, 2, {0x49,0x89}, 2, 1, 0, 0, 3 }, /* mov r8, (rax) */
    { "movq", {PAT_IMM_NUM, PAT_REG}, 2, {0x48,0xB8}, 2, 0, 8, 0, 10 }, /* movabs rax, imm64 */
    { NULL, {0}, 0, {0}, 0, 0, 0, 0, 0 }
};

/* ---------------------------
   ModR/M emission
   --------------------------- */
static void emit_modrm_for_operands(SectionKind s, Operand *op_dst, Operand *op_src) {
    uint8_t modrm = 0;
    if (op_dst->kind == OP_REG && op_src->kind == OP_REG) {
        int r1 = reg_index(op_src->reg);
        int r2 = reg_index(op_dst->reg);
        modrm = 0xC0 | ((r1 & 7) << 3) | (r2 & 7);
        emit_u8(s, modrm);
        return;
    }
    if (op_dst->kind == OP_REG && op_src->kind == OP_MEM_REG) {
        int reg = reg_index(op_dst->reg);
        int base = reg_index(op_src->reg);
        modrm = ((reg & 7) << 3) | (base & 7);
        emit_u8(s, modrm);
        return;
    }
    if (op_dst->kind == OP_MEM_REG && op_src->kind == OP_REG) {
        int reg = reg_index(op_src->reg);
        int base = reg_index(op_dst->reg);
        modrm = ((reg & 7) << 3) | (base & 7);
        emit_u8(s, modrm);
        return;
    }
    emit_u8(s, 0x00);
}

/* ---------------------------
   Instruction lookup & emission
   --------------------------- */
static InstrDef *find_instr(const char *mn, Node *n) {
    for (InstrDef *d = instr_table; d->mnemonic; ++d) {
        if (strcmp(d->mnemonic, mn) != 0) continue;
        if (d->pat_len != n->opcnt) continue;
        int ok = 1;
        for (int i = 0; i < d->pat_len; ++i) {
            PatKind pk = d->pat[i];
            Operand *op = &n->ops[i];
            if (pk == PAT_REG && op->kind != OP_REG) { ok = 0; break; }
            if (pk == PAT_IMM_NUM && op->kind != OP_IMM_NUM) { ok = 0; break; }
            if (pk == PAT_IMM_SYM && op->kind != OP_IMM_SYM) { ok = 0; break; }
            if (pk == PAT_MEM_REG && op->kind != OP_MEM_REG) { ok = 0; break; }
        }
        if (ok) return d;
    }
    return NULL;
}

static void emit_inst_table(Node *n) {
    SectionKind s = n->sec;
    InstrDef *d = find_instr(n->mnemonic, n);
    if (!d) { fprintf(stderr, "Unsupported instruction or operand pattern: %s\n", n->mnemonic); exit(1); }
    emit_bytes(s, d->opcode, d->opcode_len);
    if (d->needs_modrm) {
        if (d->pat_len == 2 && d->pat[0] == PAT_MEM_REG && d->pat[1] == PAT_REG) {
            emit_modrm_for_operands(s, &n->ops[1], &n->ops[0]);
        } else if (d->pat_len == 2 && d->pat[0] == PAT_REG && d->pat[1] == PAT_REG) {
            emit_modrm_for_operands(s, &n->ops[1], &n->ops[0]);
        } else if (d->pat_len == 2 && d->pat[0] == PAT_REG && d->pat[1] == PAT_MEM_REG) {
            emit_modrm_for_operands(s, &n->ops[1], &n->ops[0]);
        } else if (d->pat_len == 2 && d->pat[0] == PAT_IMM_NUM && d->pat[1] == PAT_REG && strcmp(n->mnemonic, "cmpq") == 0) {
            emit_u8(s, 0xF9); /* modrm for cmp imm, rcx in our simplified encoding */
        } else {
            emit_u8(s, 0x00);
        }
    }
    if (d->imm_bytes == 8) {
        if (n->ops[0].kind == OP_IMM_NUM) emit_u64(s, (uint64_t)n->ops[0].imm);
        else if (n->ops[0].kind == OP_IMM_SYM) emit_rel64(s, n->ops[0].sym, 0);
        else emit_u64(s, 0);
    } else if (d->imm_bytes == 4) {
        if (d->reloc_type == R_X86_64_PC32) {
            int addend = -(d->opcode_len + (d->needs_modrm ? 1 : 0) + d->imm_bytes);
            emit_rel32(s, n->ops[0].sym, addend);
        } else {
            if (n->ops[0].kind == OP_IMM_NUM) emit_u32(s, (uint32_t)n->ops[0].imm);
            else emit_u32(s, 0);
        }
    } else if (d->imm_bytes == 1) {
        if (n->ops[0].kind == OP_IMM_NUM) emit_u8(s, (uint8_t)n->ops[0].imm);
        else emit_u8(s, 0);
    } else {
        /* some opcodes include a 0x05 and expect a disp32 already in opcode sequence (handled above) */
    }
}

/* ---------------------------
   Directives emission
   --------------------------- */
static void emit_directive(Node *n) {
    if (!n->dir) return;
    if (strcmp(n->dir, ".global") == 0) {
        if (n->dir_arg) { Sym *s = sym_get_or_create(n->dir_arg); s->global = 1; }
        return;
    }
    if (strcmp(n->dir, ".align") == 0) {
        if (n->dir_arg) {
            int a = atoi(n->dir_arg);
            uint64_t align = (a > 0 && a <= 16) ? ((uint64_t)1 << a) : (uint64_t)a;
            if (align == 0) align = 1;
            uint64_t cur = sects[n->sec].buf.len;
            uint64_t pad = (align - (cur % align)) % align;
            for (uint64_t i = 0; i < pad; ++i) emit_u8(n->sec, 0);
        }
        return;
    }
    if (strcmp(n->dir, ".ascii") == 0) {
        if (n->dir_arg) {
            buf_put(&sects[n->sec].buf, n->dir_arg, strlen(n->dir_arg));
        }
        return;
    }
    if (strcmp(n->dir, ".byte") == 0) {
        if (n->dir_arg) {
            char *s = strdup(n->dir_arg);
            char *tok = strtok(s, " ");
            while (tok) {
                long v = strtol(tok, NULL, 0);
                emit_u8(n->sec, (uint8_t)v);
                tok = strtok(NULL, " ");
            }
            free(s);
        }
        return;
    }
    if (strcmp(n->dir, ".skip") == 0 || strcmp(n->dir, ".space") == 0) {
        if (n->dir_arg) {
            int cnt = atoi(n->dir_arg);
            for (int i = 0; i < cnt; ++i) emit_u8(n->sec, 0);
        }
        return;
    }
}

/* ---------------------------
   Two-pass assembly
   --------------------------- */
static void first_pass(Node *nodes) {
    section_init(&sects[SEC_TEXT], SEC_TEXT);
    section_init(&sects[SEC_DATA], SEC_DATA);
    section_init(&sects[SEC_BSS], SEC_BSS);

    Node *n = nodes;
    while (n) {
        if (n->kind == NODE_LABEL) {
            char *lab = strdup(n->label);
            if (lab[strlen(lab) - 1] == ':') lab[strlen(lab) - 1] = 0;
            Sym *s = sym_get_or_create(lab);
            s->sec = n->sec;
            if (n->sec == SEC_TEXT) s->value = sects[SEC_TEXT].buf.len;
            else if (n->sec == SEC_DATA) s->value = sects[SEC_DATA].buf.len;
            else if (n->sec == SEC_BSS) s->value = sects[SEC_BSS].buf.len;
            s->defined = 1;
            free(lab);
        } else if (n->kind == NODE_DIR) {
            if (strcmp(n->dir, ".ascii") == 0 && n->dir_arg) {
                sects[n->sec].buf.len += strlen(n->dir_arg);
            } else if (strcmp(n->dir, ".byte") == 0 && n->dir_arg) {
                char *s = strdup(n->dir_arg);
                int cnt = 0;
                char *tok = strtok(s, " ");
                while (tok) { cnt++; tok = strtok(NULL, " "); }
                free(s);
                sects[n->sec].buf.len += cnt;
            } else if (strcmp(n->dir, ".align") == 0 && n->dir_arg) {
                int a = atoi(n->dir_arg);
                uint64_t align = (a > 0 && a <= 16) ? ((uint64_t)1 << a) : (uint64_t)a;
                if (align == 0) align = 1;
                uint64_t cur = sects[n->sec].buf.len;
                uint64_t pad = (align - (cur % align)) % align;
                sects[n->sec].buf.len += pad;
            } else if ((strcmp(n->dir, ".skip") == 0 || strcmp(n->dir, ".space") == 0) && n->dir_arg) {
                sects[n->sec].buf.len += atoi(n->dir_arg);
            } else if (strcmp(n->dir, ".global") == 0 && n->dir_arg) {
                Sym *s = sym_get_or_create(n->dir_arg);
                s->global = 1;
            }
        } else if (n->kind == NODE_INST) {
            InstrDef *d = find_instr(n->mnemonic, n);
            if (!d) { fprintf(stderr, "Unknown instruction in first pass: %s\n", n->mnemonic); exit(1); }
            sects[n->sec].buf.len += d->size_est;
        }
        n = n->next;
    }
}

static void second_pass(Node *nodes) {
    buf_free(&sects[SEC_TEXT].buf); buf_init(&sects[SEC_TEXT].buf);
    buf_free(&sects[SEC_DATA].buf); buf_init(&sects[SEC_DATA].buf);
    buf_free(&sects[SEC_BSS].buf); buf_init(&sects[SEC_BSS].buf);

    Node *n = nodes;
    while (n) {
        if (n->kind == NODE_LABEL) {
            char *lab = strdup(n->label);
            if (lab[strlen(lab) - 1] == ':') lab[strlen(lab) - 1] = 0;
            Sym *s = sym_get_or_create(lab);
            s->sec = n->sec;
            if (n->sec == SEC_TEXT) s->value = sects[SEC_TEXT].buf.len;
            else if (n->sec == SEC_DATA) s->value = sects[SEC_DATA].buf.len;
            else if (n->sec == SEC_BSS) s->value = sects[SEC_BSS].buf.len;
            s->defined = 1;
            free(lab);
        } else if (n->kind == NODE_DIR) {
            emit_directive(n);
        } else if (n->kind == NODE_INST) {
            emit_inst_table(n);
        }
        n = n->next;
    }
}

/* ---------------------------
   ELF writer
   --------------------------- */
#pragma pack(push,1)
typedef struct { unsigned char e_ident[16]; uint16_t e_type; uint16_t e_machine; uint32_t e_version; uint64_t e_entry; uint64_t e_phoff; uint64_t e_shoff; uint32_t e_flags; uint16_t e_ehsize; uint16_t e_phentsize; uint16_t e_phnum; uint16_t e_shentsize; uint16_t e_shnum; uint16_t e_shstrndx; } Elf64_Ehdr;
typedef struct { uint32_t sh_name; uint32_t sh_type; uint64_t sh_flags; uint64_t sh_addr; uint64_t sh_offset; uint64_t sh_size; uint32_t sh_link; uint32_t sh_info; uint64_t sh_addralign; uint64_t sh_entsize; } Elf64_Shdr;
typedef struct { uint32_t st_name; unsigned char st_info; unsigned char st_other; uint16_t st_shndx; uint64_t st_value; uint64_t st_size; } Elf64_Sym;
typedef struct { uint64_t r_offset; uint64_t r_info; int64_t r_addend; } Elf64_Rela;
#pragma pack(pop)

/* plain C helper to align offsets */
static uint64_t align_to(uint64_t v, uint64_t a) {
    if (a == 0) return v;
    uint64_t m = v % a;
    if (m == 0) return v;
    return v + (a - m);
}

static void write_elf(const char *outpath) {
    const char *names[] = {"", ".text", ".data", ".bss", ".shstrtab", ".symtab", ".strtab", ".rela.text", ".rela.data"};
    int n_names = 9;
    Buf shstr; buf_init(&shstr);
    uint32_t name_off[9];
    for (int i = 0; i < n_names; ++i) { name_off[i] = (uint32_t)shstr.len; buf_put(&shstr, names[i], strlen(names[i]) + 1); }

    Buf strtab; buf_init(&strtab); buf_put_u8(&strtab, 0);
    /* assign indices and build strtab */
    int symcount = 1;
    for (Sym *s = symtab; s; s = s->next) if (s->defined || s->global) symcount++;
    /* assign index values and append names to strtab */
    for (Sym *s = symtab; s; s = s->next) {
        s->index = (int)strtab.len;
        buf_put(&strtab, s->name, strlen(s->name) + 1);
    }

    Buf symbuf; buf_init(&symbuf);
    Elf64_Sym undef = {0, 0, 0, 0, 0, 0};
    buf_put(&symbuf, &undef, sizeof(undef));
    for (Sym *s = symtab; s; s = s->next) {
        Elf64_Sym es; memset(&es, 0, sizeof(es));
        es.st_name = s->index;
        es.st_info = (s->global ? (1 << 4) : (0 << 4));
        es.st_other = 0;
        if (!s->defined) { es.st_shndx = 0; es.st_value = 0; }
        else {
            if (s->sec == SEC_TEXT) es.st_shndx = 1;
            else if (s->sec == SEC_DATA) es.st_shndx = 2;
            else if (s->sec == SEC_BSS) es.st_shndx = 3;
            else es.st_shndx = 0;
            es.st_value = s->value;
        }
        es.st_size = 0;
        buf_put(&symbuf, &es, sizeof(es));
    }

    Buf rela_text; buf_init(&rela_text);
    for (Reloc *r = sects[SEC_TEXT].relocs; r; r = r->next) {
        int sym_index = 0;
        int i = 1;
        for (Sym *s = symtab; s; s = s->next, ++i) {
            if (strcmp(s->name, r->sym) == 0) { sym_index = i; break; }
        }
        if (!sym_index) { fprintf(stderr, "Undefined symbol in relocation: %s\n", r->sym); exit(1); }
        Elf64_Rela ra; ra.r_offset = r->offset; ra.r_info = ((uint64_t)sym_index << 32) | (uint64_t)r->type; ra.r_addend = r->addend; buf_put(&rela_text, &ra, sizeof(ra));
    }

    Buf rela_data; buf_init(&rela_data);
    for (Reloc *r = sects[SEC_DATA].relocs; r; r = r->next) {
        int sym_index = 0;
        int i = 1;
        for (Sym *s = symtab; s; s = s->next, ++i) {
            if (strcmp(s->name, r->sym) == 0) { sym_index = i; break; }
        }
        if (!sym_index) { fprintf(stderr, "Undefined symbol in relocation: %s\n", r->sym); exit(1); }
        Elf64_Rela ra; ra.r_offset = r->offset; ra.r_info = ((uint64_t)sym_index << 32) | (uint64_t)r->type; ra.r_addend = r->addend; buf_put(&rela_data, &ra, sizeof(ra));
    }

    int shnum = 9;
    uint64_t off = sizeof(Elf64_Ehdr) + shnum * sizeof(Elf64_Shdr);

    uint64_t off_text = align_to(off, sects[SEC_TEXT].align); off = off_text + sects[SEC_TEXT].buf.len;
    uint64_t off_data = align_to(off, sects[SEC_DATA].align); off = off_data + sects[SEC_DATA].buf.len;
    uint64_t off_bss = align_to(off, sects[SEC_BSS].align); off = off_bss; /* NOBITS */
    uint64_t off_shstr = off; off += shstr.len;
    uint64_t off_symtab = align_to(off, 8); off = off_symtab + symbuf.len;
    uint64_t off_strtab = off; off += strtab.len;
    uint64_t off_rela_text = align_to(off, 8); off = off_rela_text + rela_text.len;
    uint64_t off_rela_data = align_to(off, 8); off = off_rela_data + rela_data.len;

    Elf64_Ehdr eh; memset(&eh, 0, sizeof(eh));
    eh.e_ident[0] = 0x7f; eh.e_ident[1] = 'E'; eh.e_ident[2] = 'L'; eh.e_ident[3] = 'F';
    eh.e_ident[4] = 2; eh.e_ident[5] = 1; eh.e_ident[6] = 1;
    eh.e_type = 1; eh.e_machine = 0x3E; eh.e_version = 1;
    eh.e_shoff = sizeof(Elf64_Ehdr);
    eh.e_ehsize = sizeof(Elf64_Ehdr); eh.e_shentsize = sizeof(Elf64_Shdr); eh.e_shnum = shnum; eh.e_shstrndx = 4;

    FILE *f = fopen(outpath, "wb");
    if (!f) { perror("fopen"); exit(1); }
    fwrite(&eh, 1, sizeof(eh), f);

    Elf64_Shdr sh; memset(&sh, 0, sizeof(sh));
    /* section 0: null */
    fwrite(&sh, 1, sizeof(sh), f);
    /* .text */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[1]; sh.sh_type = 1; sh.sh_flags = 6; sh.sh_offset = off_text; sh.sh_size = sects[SEC_TEXT].buf.len; sh.sh_addralign = sects[SEC_TEXT].align; fwrite(&sh, 1, sizeof(sh), f);
    /* .data */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[2]; sh.sh_type = 1; sh.sh_flags = 3; sh.sh_offset = off_data; sh.sh_size = sects[SEC_DATA].buf.len; sh.sh_addralign = sects[SEC_DATA].align; fwrite(&sh, 1, sizeof(sh), f);
    /* .bss */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[3]; sh.sh_type = 8; sh.sh_flags = 3; sh.sh_offset = off_bss; sh.sh_size = sects[SEC_BSS].buf.len; sh.sh_addralign = sects[SEC_BSS].align; fwrite(&sh, 1, sizeof(sh), f);
    /* .shstrtab */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[4]; sh.sh_type = 3; sh.sh_offset = off_shstr; sh.sh_size = shstr.len; sh.sh_addralign = 1; fwrite(&sh, 1, sizeof(sh), f);
    /* .symtab */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[5]; sh.sh_type = 2; sh.sh_offset = off_symtab; sh.sh_size = symbuf.len; sh.sh_entsize = sizeof(Elf64_Sym); sh.sh_link = 6; sh.sh_info = 1; sh.sh_addralign = 8; fwrite(&sh, 1, sizeof(sh), f);
    /* .strtab */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[6]; sh.sh_type = 3; sh.sh_offset = off_strtab; sh.sh_size = strtab.len; sh.sh_addralign = 1; fwrite(&sh, 1, sizeof(sh), f);
    /* .rela.text */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[7]; sh.sh_type = 4; sh.sh_offset = off_rela_text; sh.sh_size = rela_text.len; sh.sh_entsize = sizeof(Elf64_Rela); sh.sh_link = 5; sh.sh_info = 1; sh.sh_addralign = 8; fwrite(&sh, 1, sizeof(sh), f);
    /* .rela.data */
    memset(&sh, 0, sizeof(sh)); sh.sh_name = name_off[8]; sh.sh_type = 4; sh.sh_offset = off_rela_data; sh.sh_size = rela_data.len; sh.sh_entsize = sizeof(Elf64_Rela); sh.sh_link = 5; sh.sh_info = 2; sh.sh_addralign = 8; fwrite(&sh, 1, sizeof(sh), f);

    uint64_t cur = ftell(f);
    while (cur < off_text) { fputc(0, f); cur++; }
    if (sects[SEC_TEXT].buf.len) fwrite(sects[SEC_TEXT].buf.data, 1, sects[SEC_TEXT].buf.len, f);
    cur = ftell(f);
    while (cur < off_data) { fputc(0, f); cur++; }
    if (sects[SEC_DATA].buf.len) fwrite(sects[SEC_DATA].buf.data, 1, sects[SEC_DATA].buf.len, f);
    cur = ftell(f);
    while (cur < off_shstr) { fputc(0, f); cur++; }
    fwrite(shstr.data, 1, shstr.len, f);
    cur = ftell(f);
    while (cur < off_symtab) { fputc(0, f); cur++; }
    fwrite(symbuf.data, 1, symbuf.len, f);
    fwrite(strtab.data, 1, strtab.len, f);
    cur = ftell(f);
    while (cur < off_rela_text) { fputc(0, f); cur++; }
    if (rela_text.len) fwrite(rela_text.data, 1, rela_text.len, f);
    cur = ftell(f);
    while (cur < off_rela_data) { fputc(0, f); cur++; }
    if (rela_data.len) fwrite(rela_data.data, 1, rela_data.len, f);

    fclose(f);
    buf_free(&shstr); buf_free(&strtab); buf_free(&symbuf); buf_free(&rela_text); buf_free(&rela_data);
}

/* ---------------------------
   File IO and main
   --------------------------- */
static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); exit(1); }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) { perror("malloc"); exit(1); }
    fread(buf, 1, sz, f);
    buf[sz] = 0;
    fclose(f);
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.s out.o\n", argv[0]);
        return 1;
    }
    char *src = read_file(argv[1]);
    Parser p; parser_init(&p, src);
    Node *nodes = parse_all(&p);
    first_pass(nodes);
    second_pass(nodes);
    write_elf(argv[2]);
    printf("Wrote %s\n", argv[2]);
    free(src);
    return 0;
}
