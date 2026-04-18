/* 711as_table.c
 *
 * Table-driven x86-64 assembler (BNF subset)
 *
 * - All instruction encodings are driven by tables (insn_table).
 * - Parser enforces BNF subset; operand shapes are matched against table masks.
 * - Two-pass: first pass collects labels, second pass emits bytes and relocations.
 * - GNU-as-like error messages (file:line:col: error: ...).
 *
 * Limitations:
 * - Educational skeleton: many encodings simplified (ModR/M/SIB/REX limited).
 * - Not a full assembler; extend insn_table to add more encodings.
 *
 * Build:
 *   cc -std=c11 -O2 -Wall -Wextra -o 711as_table 711as_table.c
 *
 * Usage:
 *   ./711as_table input.s
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h> /* strcasecmp */
#include <ctype.h>
#include <stdarg.h>

/* ---------------------------
   Globals and helpers
   --------------------------- */
static const char *g_filename = "<stdin>";
static int g_errors = 0, g_warnings = 0;
static void asm_error_loc(int line, int col, const char *fmt, ...){
    va_list ap; va_start(ap, fmt);
    fprintf(stderr, "%s:%d:%d: error: ", g_filename, line, col);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    g_errors++;
}
static void asm_warn_loc(int line, int col, const char *fmt, ...){
    va_list ap; va_start(ap, fmt);
    fprintf(stderr, "%s:%d:%d: warning: ", g_filename, line, col);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    g_warnings++;
}

/* ---------------------------
   Dynamic buffer
   --------------------------- */
typedef struct { uint8_t *data; size_t len, cap; } Buf;
static void buf_init(Buf *b){ b->data=NULL; b->len=b->cap=0; }
static void buf_grow(Buf *b, size_t need){
    if(b->cap>=need) return;
    size_t n=b->cap?b->cap*2:256;
    while(n<need) n*=2;
    b->data = realloc(b->data, n);
    b->cap = n;
}
static void buf_put8(Buf *b, uint8_t v){ buf_grow(b,b->len+1); b->data[b->len++]=v; }
static void buf_put32(Buf *b, uint32_t v){ buf_grow(b,b->len+4); memcpy(b->data+b->len,&v,4); b->len+=4; }
static void buf_put64(Buf *b, uint64_t v){ buf_grow(b,b->len+8); memcpy(b->data+b->len,&v,8); b->len+=8; }

/* ---------------------------
   Symbol & Relocation
   --------------------------- */
typedef struct Sym { char *name; uint64_t addr; struct Sym *next; } Sym;
static Sym *symtab = NULL;
static Sym *sym_find(const char *n){ for(Sym*s=symtab;s;s=s->next) if(strcmp(s->name,n)==0) return s; return NULL; }
static void sym_add(const char *n, uint64_t addr){ Sym *s = malloc(sizeof(*s)); s->name=strdup(n); s->addr=addr; s->next=symtab; symtab=s; }

typedef enum { REL_ABS64, REL_ABS32, REL_RIP32 } RelKind;
typedef struct Rel { RelKind kind; size_t off; char *sym; int64_t addend; struct Rel *next; } Rel;
static Rel *relocs = NULL;
static void add_reloc(RelKind k, size_t off, const char *sym, int64_t add){ Rel *r=malloc(sizeof(*r)); r->kind=k; r->off=off; r->sym=strdup(sym); r->addend=add; r->next=relocs; relocs=r; }

/* ---------------------------
   Registers
   --------------------------- */
typedef enum {
    RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
    R8, R9, R10, R11, R12, R13, R14, R15,
    EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
    R8D, R9D, R10D, R11D, R12D, R13D, R14D, R15D,
    AX, CX, DX, BX, SI, DI, R8W, R9W, R10W, R11W, R12W, R13W, R14W, R15W,
    AL, CL, DL, BL, SIL, DIL, R8B, R9B, R10B, R11B, R12B, R13B, R14B, R15B,
    XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13,
    RIP, REG_INV
} RegId;

static const struct { const char *name; RegId id; int bits; } regmap[] = {
    {"%rax", RAX, 64}, {"%rbx", RBX, 64}, {"%rcx", RCX, 64}, {"%rdx", RDX, 64},
    {"%rsi", RSI, 64}, {"%rdi", RDI, 64}, {"%rsp", RSP, 64}, {"%rbp", RBP, 64},
    {"%r8", R8, 64}, {"%r9", R9, 64}, {"%r10", R10, 64}, {"%r11", R11, 64},
    {"%r12", R12, 64}, {"%r13", R13, 64}, {"%r14", R14, 64}, {"%r15", R15, 64},
    {"%rip", RIP, 64},

    {"%eax", EAX, 32}, {"%ebx", EBX, 32}, {"%ecx", ECX, 32}, {"%edx", EDX, 32},
    {"%esi", ESI, 32}, {"%edi", EDI, 32}, {"%r8d", R8D, 32}, {"%r9d", R9D, 32},
    {"%r10d", R10D, 32}, {"%r11d", R11D, 32}, {"%r12d", R12D, 32}, {"%r13d", R13D, 32},
    {"%r14d", R14D, 32}, {"%r15d", R15D, 32},

    {"%ax", AX, 16}, {"%bx", BX, 16}, {"%cx", CX, 16}, {"%dx", DX, 16},
    {"%si", SI, 16}, {"%di", DI, 16}, {"%r8w", R8W, 16}, {"%r9w", R9W, 16},
    {"%r10w", R10W, 16}, {"%r11w", R11W, 16}, {"%r12w", R12W, 16}, {"%r13w", R13W, 16},
    {"%r14w", R14W, 16}, {"%r15w", R15W, 16},

    {"%al", AL, 8}, {"%bl", BL, 8}, {"%cl", CL, 8}, {"%dl", DL, 8},
    {"%sil", SIL, 8}, {"%dil", DIL, 8}, {"%r8b", R8B, 8}, {"%r9b", R9B, 8},
    {"%r10b", R10B, 8}, {"%r11b", R11B, 8}, {"%r12b", R12B, 8}, {"%r13b", R13B, 8},
    {"%r14b", R14B, 8}, {"%r15b", R15B, 8},

    {"%xmm0", XMM0, 128}, {"%xmm1", XMM1, 128}, {"%xmm2", XMM2, 128}, {"%xmm3", XMM3, 128},
    {"%xmm4", XMM4, 128}, {"%xmm5", XMM5, 128}, {"%xmm6", XMM6, 128}, {"%xmm7", XMM7, 128},
    {"%xmm8", XMM8, 128}, {"%xmm9", XMM9, 128}, {"%xmm10", XMM10, 128}, {"%xmm11", XMM11, 128},
    {"%xmm12", XMM12, 128}, {"%xmm13", XMM13, 128},
    {NULL, REG_INV, 0}
};

static RegId parse_register(const char *s, size_t *consumed){
    for(int i=0; regmap[i].name; ++i){
        size_t ln = strlen(regmap[i].name);
        if(strncmp(s, regmap[i].name, ln)==0){
            if(consumed) *consumed = ln;
            return regmap[i].id;
        }
    }
    if(consumed) *consumed = 0;
    return REG_INV;
}

/* ---------------------------
   Values and memory operands
   --------------------------- */
typedef struct {
    int is_label;
    char *label;
    int64_t num;
    int64_t addend;
} Value;

static int parse_number(const char *s, int64_t *out, size_t *consumed){
    const char *p=s; while(isspace((unsigned char)*p)) p++;
    int neg=0; if(*p=='-'){ neg=1; p++; }
    if(p[0]=='0' && (p[1]=='x' || p[1]=='X')){
        p+=2; uint64_t v=0; size_t n=0;
        while(isxdigit((unsigned char)*p)){ char c=*p++; v=(v<<4) + (isdigit((unsigned char)c)?c-'0':(tolower(c)-'a'+10)); n++; }
        if(n==0) return 0;
        *out = neg ? -(int64_t)v : (int64_t)v;
        if(consumed) *consumed = (size_t)(p-s);
        return 1;
    } else if(isdigit((unsigned char)*p)){
        int64_t v=0; size_t n=0;
        while(isdigit((unsigned char)*p)){ v = v*10 + (*p - '0'); p++; n++; }
        if(n==0) return 0;
        *out = neg ? -v : v;
        if(consumed) *consumed = (size_t)(p-s);
        return 1;
    }
    return 0;
}

static int parse_string_identifier(const char *s, char **out, size_t *consumed){
    const char *p=s; while(isspace((unsigned char)*p)) p++;
    if(*p!='"') return 0;
    p++;
    const char *st=p;
    while(*p && *p!='"') p++;
    if(*p!='"') return 0;
    size_t len = (size_t)(p-st);
    *out = malloc(len+1); memcpy(*out, st, len); (*out)[len]=0;
    if(consumed) *consumed = (size_t)(p - s + 1);
    return 1;
}

static int parse_immediate(const char *s, Value *v, size_t *consumed){
    const char *p=s; while(isspace((unsigned char)*p)) p++;
    v->is_label = 0; v->label = NULL; v->num = 0; v->addend = 0;
    size_t c=0;
    if(parse_number(p, &v->num, &c)){ if(consumed) *consumed = (size_t)(p - s + c); return 1; }
    char *ident = NULL;
    if(parse_string_identifier(p, &ident, &c)){
        v->is_label = 1; v->label = ident;
        if(consumed) *consumed = (size_t)(p - s + c);
        return 1;
    }
    return 0;
}

/* Memory operand forms per BNF */
typedef enum { MEM_OFF_BASE, MEM_IDENT_BASE, MEM_GOTPCREL } MemKind;
typedef struct {
    MemKind kind;
    int64_t offset;
    char *ident;
    RegId base;
} MemOp;

static int parse_register_base(const char *s, RegId *out, size_t *consumed){
    const char *p=s; while(isspace((unsigned char)*p)) p++;
    const char *candidates[] = {"%rbp","%rsp","%rip", NULL};
    for(int i=0;candidates[i];++i){
        size_t ln = strlen(candidates[i]);
        if(strncmp(p, candidates[i], ln)==0){
            *out = parse_register(candidates[i], NULL);
            if(consumed) *consumed = (size_t)(p - s + ln);
            return 1;
        }
    }
    return 0;
}

static int parse_memory_operand(const char *s, MemOp *m, size_t *consumed){
    const char *p=s; while(isspace((unsigned char)*p)) p++;
    if(*p=='-'){
        p++;
        Value imm; size_t c=0;
        if(!parse_immediate(p, &imm, &c)) return 0;
        p += c;
        while(isspace((unsigned char)*p)) p++;
        if(*p!='(') return 0;
        p++;
        RegId base; size_t cb=0;
        if(!parse_register_base(p, &base, &cb)) return 0;
        p += cb;
        while(isspace((unsigned char)*p)) p++;
        if(*p!=')') return 0;
        p++;
        m->kind = MEM_OFF_BASE; m->offset = imm.num; m->ident = NULL; m->base = base;
        if(consumed) *consumed = (size_t)(p - s);
        return 1;
    }
    char *ident = NULL; size_t ci=0;
    if(parse_string_identifier(p, &ident, &ci)){
        p += ci;
        while(isspace((unsigned char)*p)) p++;
        if(*p=='@'){
            const char *tag = "@GOTPCREL";
            size_t tlen = strlen(tag);
            if(strncmp(p, tag, tlen)==0){
                p += tlen;
                while(isspace((unsigned char)*p)) p++;
                if(*p!='('){ free(ident); return 0; }
                p++;
                RegId base; size_t cb=0;
                if(!parse_register_base(p, &base, &cb)){ free(ident); return 0; }
                p += cb;
                while(isspace((unsigned char)*p)) p++;
                if(*p!=')'){ free(ident); return 0; }
                p++;
                m->kind = MEM_GOTPCREL; m->ident = ident; m->base = base; m->offset = 0;
                if(consumed) *consumed = (size_t)(p - s);
                return 1;
            } else { free(ident); return 0; }
        } else {
            while(isspace((unsigned char)*p)) p++;
            if(*p!='('){ free(ident); return 0; }
            p++;
            RegId base; size_t cb=0;
            if(!parse_register_base(p, &base, &cb)){ free(ident); return 0; }
            p += cb;
            while(isspace((unsigned char)*p)) p++;
            if(*p!=')'){ free(ident); return 0; }
            p++;
            m->kind = MEM_IDENT_BASE; m->ident = ident; m->base = base; m->offset = 0;
            if(consumed) *consumed = (size_t)(p - s);
            return 1;
        }
    }
    return 0;
}

/* ---------------------------
   Operand and operand masks
   --------------------------- */
typedef enum { OT_REG, OT_MEM, OT_IMM } OpType;
typedef struct {
    OpType type;
    RegId reg;
    MemOp mem;
    Value imm;
    int bits;
} Operand;

static int parse_operand(const char *s, Operand *op, size_t *consumed){
    const char *p=s; while(isspace((unsigned char)*p)) p++;
    size_t c=0;
    RegId r = parse_register(p, &c);
    if(r!=REG_INV){
        op->type = OT_REG; op->reg = r;
        int bits = 0;
        for(int i=0; regmap[i].name; ++i) if(regmap[i].id==r){ bits = regmap[i].bits; break; }
        op->bits = bits;
        if(consumed) *consumed = (size_t)(p - s + c);
        return 1;
    }
    MemOp m; size_t cm=0;
    if(parse_memory_operand(p, &m, &cm)){
        op->type = OT_MEM; op->mem = m; op->bits = 64;
        if(consumed) *consumed = (size_t)(p - s + cm);
        return 1;
    }
    Value v; size_t ci=0;
    if(parse_immediate(p, &v, &ci)){
        op->type = OT_IMM; op->imm = v; op->bits = 64;
        if(consumed) *consumed = (size_t)(p - s + ci);
        return 1;
    }
    if(consumed) *consumed = 0;
    return 0;
}

/* operand kind masks used in table */
enum { K_REG=1, K_MEM=2, K_IMM=4, K_XMM=8, K_REG8=16, K_REG32=32, K_REG64=64, K_REL=128 };

/* ---------------------------
   Instruction table (table-driven)
   Each entry:
     - mnemonic
     - argc
     - operand masks (per operand)
     - opcode template bytes (up to 4)
     - opcode_len
     - encoding flags (simplified)
   Matching: mnemonic + argc + operand masks -> encoder uses generic routines
*/
typedef struct {
    const char *mn;
    int argc;
    int masks[3];
    uint8_t opcode[4];
    int opcode_len;
    int flags;
} InsnEntry;

/* A compact table: extend as needed */
static const InsnEntry insn_table[] = {
    /* mov reg, reg/mem */
    { "mov", 2, { K_REG|K_MEM|K_IMM, K_REG|K_MEM|K_IMM }, {0x89,0,0,0}, 1, 0 },
    { "lea", 2, { K_MEM, K_REG64 }, {0x8D,0,0,0}, 1, 0 },

    { "add", 2, { K_REG|K_MEM|K_IMM, K_REG64 }, {0x01,0x00,0,0}, 2, 0 },
    { "sub", 2, { K_REG|K_MEM|K_IMM, K_REG64 }, {0x29,0x00,0,0}, 2, 0 },

    { "imul", 2, { K_REG64, K_REG64 }, {0x0F,0xAF,0x00,0}, 3, 0 },
    { "div", 1, { K_REG64, 0,0 }, {0xF7,0x30,0,0}, 2, 0 },
    { "idiv",1, { K_REG64, 0,0 }, {0xF7,0x38,0,0}, 2, 0 },

    { "cmp", 2, { K_REG|K_MEM|K_IMM, K_REG64 }, {0x39,0x00,0,0}, 2, 0 },

    { "jmp", 1, { K_IMM|K_REL, 0,0 }, {0xE9,0,0,0}, 1, 0 },
    { "je", 1, { K_IMM|K_REL, 0,0 }, {0x0F,0x84,0,0}, 2, 0 },
    { "jne",1, { K_IMM|K_REL, 0,0 }, {0x0F,0x85,0,0}, 2, 0 },
    { "jl", 1, { K_IMM|K_REL, 0,0 }, {0x0F,0x8C,0,0}, 2, 0 },
    { "jle",1, { K_IMM|K_REL, 0,0 }, {0x0F,0x8E,0,0}, 2, 0 },

    { "sete", 1, { K_REG8, 0,0 }, {0x0F,0x94,0x00,0}, 3, 0 },
    { "setne",1, { K_REG8, 0,0 }, {0x0F,0x95,0x00,0}, 3, 0 },

    { "and", 2, { K_REG|K_MEM|K_IMM, K_REG64 }, {0x21,0x00,0,0}, 2, 0 },
    { "or",  2, { K_REG|K_MEM|K_IMM, K_REG64 }, {0x09,0x00,0,0}, 2, 0 },
    { "xor", 2, { K_REG|K_MEM|K_IMM, K_REG64 }, {0x31,0x00,0,0}, 2, 0 },

    { "not", 1, { K_REG64, 0,0 }, {0xF7,0x10,0,0}, 2, 0 },

    { "shl", 2, { K_REG8, K_REG64, 0 }, {0xD3,0x20,0,0}, 2, 0 },
    { "shr", 2, { K_REG8, K_REG64, 0 }, {0xD3,0x28,0,0}, 2, 0 },
    { "sar", 2, { K_REG8, K_REG64, 0 }, {0xD3,0x38,0,0}, 2, 0 },

    { "call", 2, { K_REG64, 0,0 }, {0xFF,0x10,0,0}, 2, 0 },

    { "push",1, { K_REG64, 0,0 }, {0x50,0,0,0}, 1, 0 },
    { "pop", 1, { K_REG64, 0,0 }, {0x58,0,0,0}, 1, 0 },

    { "ret", 0, {0,0,0}, {0xC3,0,0,0}, 1, 0 },

    /* SSE subset */
    { "cvtsi2sd", 2, { K_REG64, K_XMM, 0 }, {0xF2,0x0F,0x2A,0}, 3, 0 },
    { "cvtss2sd", 2, { K_XMM, K_XMM, 0 }, {0xF3,0x0F,0x5A,0}, 3, 0 },
    { "cvtsd2ss", 2, { K_XMM, K_XMM, 0 }, {0xF2,0x0F,0x5A,0}, 3, 0 },

    { "movss", 2, { K_REG|K_MEM|K_IMM, K_XMM, 0 }, {0xF3,0x0F,0x10,0}, 3, 0 },
    { "movsd", 2, { K_REG|K_MEM|K_IMM, K_XMM, 0 }, {0xF2,0x0F,0x10,0}, 3, 0 },

    { "movabs", 2, { K_IMM|K_REG64, K_REG64, 0 }, {0xB8,0x00,0,0}, 1, 0 },

    { NULL }
};

/* ---------------------------
   Matching helpers
   --------------------------- */
static int operand_kind_mask(const Operand *o){
    if(o->type==OT_REG){
        if(o->bits==8) return K_REG8;
        if(o->bits==16) return K_REG32;
        if(o->bits==32) return K_REG32;
        if(o->bits==64) return K_REG64;
        return K_REG;
    } else if(o->type==OT_MEM) return K_MEM;
    else if(o->type==OT_IMM) return K_IMM;
    return 0;
}

static const InsnEntry *find_insn_entry(const char *mn, int argc, Operand *ops){
    for(const InsnEntry *e = insn_table; e->mn; ++e){
        if(strcasecmp(e->mn, mn)!=0) continue;
        if(e->argc != argc) continue;
        int ok = 1;
        for(int i=0;i<argc;i++){
            int want = e->masks[i];
            int have = operand_kind_mask(&ops[i]);
            if(want & K_XMM){
                if(!(ops[i].type==OT_REG && (ops[i].reg>=XMM0 && ops[i].reg<=XMM13))){ ok=0; break; }
            } else if(want & K_REG8){
                if(!(ops[i].type==OT_REG && ops[i].bits==8)) { ok=0; break; }
            } else if(want & K_REG32){
                if(!(ops[i].type==OT_REG && ops[i].bits==32)) { ok=0; break; }
            } else if(want & K_REG64){
                if(!(ops[i].type==OT_REG && ops[i].bits==64)) { ok=0; break; }
            } else {
                if(want & K_REG){
                    if(ops[i].type!=OT_REG){ ok=0; break; }
                } else if(want & K_MEM){
                    if(ops[i].type!=OT_MEM){ ok=0; break; }
                } else if(want & K_IMM){
                    if(ops[i].type!=OT_IMM){ ok=0; break; }
                }
            }
        }
        if(ok) return e;
    }
    return NULL;
}

/* ---------------------------
   Encoder utilities (generic, table-driven)
   - Very simplified: emits opcode bytes, optional ModR/M for reg/reg and reg/mem,
     and immediates/relocs. REX handling minimal.
*/
static void emit_rex(Buf *out, int w, RegId reg, RegId rm, RegId idx){
    uint8_t rex = 0x40;
    if(w) rex |= 0x08;
    if(reg>=R8 && reg<=R15) rex |= 0x04;
    if(idx>=R8 && idx<=R15) rex |= 0x02;
    if(rm>=R8 && rm<=R15) rex |= 0x01;
    if(rex!=0x40) buf_put8(out, rex);
}

static int reg_low3(RegId r){
    switch(r){
        case RAX: case EAX: case AX: case AL: return 0;
        case RCX: case ECX: case CX: case CL: return 1;
        case RDX: case EDX: case DX: case DL: return 2;
        case RBX: case EBX: case BX: case BL: return 3;
        case RSP: case ESP: return 4;
        case RBP: case EBP: return 5;
        case RSI: case ESI: return 6;
        case RDI: case EDI: return 7;
        case R8: case R8D: case R8W: case R8B: return 0;
        case R9: case R9D: case R9W: case R9B: return 1;
        case R10: case R10D: case R10W: case R10B: return 2;
        case R11: case R11D: case R11W: case R11B: return 3;
        case R12: case R12D: case R12W: case R12B: return 4;
        case R13: case R13D: case R13W: case R13B: return 5;
        case R14: case R14D: case R14W: case R14B: return 6;
        case R15: case R15D: case R15W: case R15B: return 7;
        default: return 0;
    }
}

/* ModR/M emission simplified */
static void emit_modrm_reg_reg(Buf *out, int reg_field, RegId rm_reg){
    uint8_t modrm = 0xC0 | ((reg_field & 7) << 3) | (reg_low3(rm_reg) & 7);
    buf_put8(out, modrm);
}
static void emit_modrm_reg_mem(Buf *out, int reg_field, const MemOp *m){
    if(m->base == RBP){
        uint8_t modrm = 0x00 | ((reg_field & 7) << 3) | 5;
        buf_put8(out, modrm);
        size_t off = out->len; buf_put32(out, 0);
        if(m->kind==MEM_OFF_BASE){
            uint32_t d = (uint32_t)m->offset; memcpy(out->data+off, &d, 4);
        } else {
            add_reloc(REL_ABS32, off, m->ident, 0);
        }
        return;
    }
    if(m->base == RSP){
        uint8_t modrm = 0x00 | ((reg_field & 7) << 3) | 4;
        buf_put8(out, modrm);
        uint8_t sib = (0<<6) | (4<<3) | (reg_low3(RSP)&7);
        buf_put8(out, sib);
        if(m->kind==MEM_OFF_BASE){
            size_t off = out->len; buf_put32(out, 0); uint32_t d=(uint32_t)m->offset; memcpy(out->data+off,&d,4);
        } else {
            size_t off = out->len; buf_put32(out, 0); add_reloc(REL_ABS32, off, m->ident, 0);
        }
        return;
    }
    if(m->base == RIP){
        uint8_t modrm = 0x00 | ((reg_field & 7) << 3) | 5;
        buf_put8(out, modrm);
        size_t off = out->len; buf_put32(out, 0);
        if(m->kind==MEM_GOTPCREL) add_reloc(REL_RIP32, off, m->ident, 0);
        else if(m->kind==MEM_IDENT_BASE) add_reloc(REL_RIP32, off, m->ident, 0);
        else { uint32_t d = (uint32_t)m->offset; memcpy(out->data+off, &d, 4); }
        return;
    }
    uint8_t modrm = 0x00 | ((reg_field & 7) << 3) | (reg_low3(m->base) & 7);
    buf_put8(out, modrm);
    if(m->kind==MEM_OFF_BASE){
        size_t off = out->len; buf_put32(out, 0); uint32_t d=(uint32_t)m->offset; memcpy(out->data+off,&d,4);
    } else {
        size_t off = out->len; buf_put32(out, 0); add_reloc(REL_ABS32, off, m->ident, 0);
    }
}

/* immediate emission */
static void emit_immediate_or_label(Buf *out, const Value *v, int bytes){
    if(v->is_label){
        size_t off = out->len;
        for(int i=0;i<bytes;i++) buf_put8(out, 0);
        add_reloc(bytes==8?REL_ABS64:REL_ABS32, off, v->label, v->addend);
    } else {
        if(bytes==1) buf_put8(out, (uint8_t)v->num);
        else if(bytes==2){ uint16_t w=(uint16_t)v->num; buf_put8(out,(uint8_t)(w&0xff)); buf_put8(out,(uint8_t)((w>>8)&0xff)); }
        else if(bytes==4) buf_put32(out, (uint32_t)v->num);
        else if(bytes==8) buf_put64(out, (uint64_t)v->num);
    }
}

/* generic emitter driven by table entry */
static int emit_from_entry(const InsnEntry *e, Operand *ops, Buf *out, uint64_t cur_off, int line){
    /* determine REX W if any 64-bit regs used */
    int need_rex_w = 0;
    RegId rex_reg = REG_INV, rex_rm = REG_INV, rex_idx = REG_INV;
    for(int i=0;i<e->argc;i++){
        if(ops[i].type==OT_REG && ops[i].bits==64) need_rex_w = 1;
        if(ops[i].type==OT_REG){
            if(rex_reg==REG_INV) rex_reg = ops[i].reg;
            else rex_rm = ops[i].reg;
        }
        if(ops[i].type==OT_MEM) rex_rm = ops[i].mem.base;
    }
    emit_rex(out, need_rex_w, rex_reg, rex_rm, rex_idx);

    /* emit opcode template */
    for(int i=0;i<e->opcode_len;i++) buf_put8(out, e->opcode[i]);

    /* handle common operand shapes */
    if(e->argc==2){
        Operand *a=&ops[0], *b=&ops[1];
        if(a->type==OT_REG && b->type==OT_REG){
            emit_modrm_reg_reg(out, reg_low3(b->reg), a->reg);
        } else if(a->type==OT_MEM && b->type==OT_REG){
            emit_modrm_reg_mem(out, reg_low3(b->reg), &a->mem);
        } else if(a->type==OT_REG && b->type==OT_MEM){
            emit_modrm_reg_mem(out, reg_low3(a->reg), &b->mem);
        } else if(a->type==OT_IMM && b->type==OT_REG){
            /* immediate to reg: simplified as 32-bit immediate */
            emit_immediate_or_label(out, &a->imm, 4);
        } else if(a->type==OT_REG && b->type==OT_IMM){
            emit_immediate_or_label(out, &b->imm, 4);
        } else {
            asm_error_loc(line, 1, "Unsupported operand combination for %s", e->mn);
            return -1;
        }
    } else if(e->argc==1){
        Operand *a=&ops[0];
        if(a->type==OT_REG){
            if(e->opcode[0]==0x50 || e->opcode[0]==0x58){
                out->data[out->len-1] = (uint8_t)(e->opcode[0] + (reg_low3(a->reg) & 7));
            } else {
                emit_modrm_reg_reg(out, 0, a->reg);
            }
        } else if(a->type==OT_IMM){
            emit_immediate_or_label(out, &a->imm, 4);
        } else if(a->type==OT_MEM){
            emit_modrm_reg_mem(out, 0, &a->mem);
        } else {
            asm_error_loc(line, 1, "Unsupported operand for %s", e->mn);
            return -1;
        }
    } else {
        /* zero-operand: nothing else */
    }
    return 0;
}

/* ---------------------------
   Parser and AST nodes
   --------------------------- */
typedef enum { NODE_DIR, NODE_LABEL, NODE_INSTR, NODE_BLANK, NODE_ERR } NodeKind;
typedef struct Node {
    NodeKind kind;
    char *mn;
    char *args;
    char *label;
    int line;
    struct Node *next;
} Node;
static Node *make_node(NodeKind k, int line){ Node *n = calloc(1,sizeof(*n)); n->kind=k; n->line=line; return n; }

static int is_label_token(const char *s, size_t *len){
    if(strncmp(s, ".L.", 3)==0){
        const char *p = s+3;
        while(*p && (isalnum((unsigned char)*p) || *p=='.' || *p=='_' || *p=='-' || *p=='+')) p++;
        if(len) *len = (size_t)(p - s);
        return 1;
    }
    return 0;
}

static Node *parse_line_bnf(char *line, int lineno){
    const char *p=line; while(isspace((unsigned char)*p)) p++;
    if(*p==0) return make_node(NODE_BLANK, lineno);
    if(*p=='.'){
        const char *q=p+1;
        while(isalpha((unsigned char)*q)) q++;
        size_t len = (size_t)(q - (p+1));
        Node *n = make_node(NODE_DIR, lineno);
        n->mn = malloc(len+2);
        memcpy(n->mn, p+1, len); n->mn[len]=0;
        while(isspace((unsigned char)*q)) q++;
        n->args = strdup(q);
        return n;
    }
    size_t lablen=0;
    if(is_label_token(p, &lablen) && p[lablen]==':'){
        Node *n = make_node(NODE_LABEL, lineno);
        n->label = malloc(lablen+1); memcpy(n->label, p, lablen); n->label[lablen]=0;
        return n;
    }
    const char *q=p;
    while(isalpha((unsigned char)*q) || *q=='.' || *q=='_' ) q++;
    if(q==p){
        Node *n = make_node(NODE_ERR, lineno);
        n->args = strdup(p);
        return n;
    }
    size_t mlen = (size_t)(q-p);
    Node *n = make_node(NODE_INSTR, lineno);
    n->mn = malloc(mlen+1); memcpy(n->mn, p, mlen); n->mn[mlen]=0;
    while(isspace((unsigned char)*q)) q++;
    n->args = strdup(q);
    return n;
}

static Node *read_program(FILE *f){
    Node *head=NULL, **tail=&head;
    char *line=NULL; size_t cap=0; ssize_t n;
    int lineno = 0;
    while((n=getline(&line,&cap,f))!=-1){
        lineno++;
        if(n>0 && line[n-1]=='\n') line[n-1]=0;
        Node *nd = parse_line_bnf(line, lineno);
        *tail = nd; tail = &nd->next;
    }
    free(line);
    return head;
}

/* ---------------------------
   Directives handling (table-driven)
   Supported: align, globl, byte, quad, zero, file, text, data, bss
*/
static int handle_directive(Node *n){
    if(!n->mn) return 0;
    if(strcasecmp(n->mn,"align")==0){
        Value v; if(!parse_immediate(n->args, &v, NULL)){ asm_error_loc(n->line, 1, ".align expects immediate"); return -1; }
        uint64_t a = (uint64_t)v.num;
        if(a==0) return 0;
        extern Buf outbuf; /* declared later; hack to avoid forward decl */
        /* we'll set alignment in second pass via cur_loc; here just accept */
        return 0;
    }
    if(strcasecmp(n->mn,"globl")==0){
        char *ident=NULL; if(!parse_string_identifier(n->args, &ident, NULL)){ asm_error_loc(n->line, 1, ".globl expects string identifier"); return -1; }
        if(!sym_find(ident)) sym_add(ident, 0);
        free(ident);
        return 0;
    }
    if(strcasecmp(n->mn,"byte")==0 || strcasecmp(n->mn,".byte")==0){
        return 0;
    }
    if(strcasecmp(n->mn,"quad")==0){
        return 0;
    }
    if(strcasecmp(n->mn,"zero")==0){
        return 0;
    }
    if(strcasecmp(n->mn,"file")==0){
        return 0;
    }
    if(strcasecmp(n->mn,"text")==0 || strcasecmp(n->mn,"data")==0 || strcasecmp(n->mn,"bss")==0){
        return 0;
    }
    asm_error_loc(n->line, 1, "Unknown directive: .%s", n->mn);
    return -1;
}

/* ---------------------------
   Two-pass assembly
*/
static Buf outbuf;
static uint64_t cur_loc = 0;

static void emit_align(uint64_t a){
    if(a==0) return;
    uint64_t newloc = (cur_loc + (a-1)) & ~(a-1);
    while(cur_loc < newloc){ buf_put8(&outbuf, 0); cur_loc++; }
}

static void first_pass(Node *head){
    cur_loc = 0;
    for(Node *n=head; n; n=n->next){
        if(n->kind==NODE_LABEL){
            if(sym_find(n->label)) asm_warn_loc(n->line, 1, "duplicate label %s", n->label);
            else sym_add(n->label, cur_loc);
        } else if(n->kind==NODE_DIR){
            if(strcasecmp(n->mn,"align")==0){
                Value v; parse_immediate(n->args, &v, NULL);
                if(v.num>0) cur_loc = (cur_loc + (v.num-1)) & ~(v.num-1);
            } else if(strcasecmp(n->mn,"byte")==0) cur_loc += 1;
            else if(strcasecmp(n->mn,"quad")==0) cur_loc += 8;
            else if(strcasecmp(n->mn,"zero")==0){ Value v; if(parse_immediate(n->args,&v,NULL)) cur_loc += v.num; }
            else { }
        } else if(n->kind==NODE_INSTR){
            /* crude estimate: opcode + modrm + imm */
            cur_loc += 6;
        }
    }
}

/* resolve relocations */
static int resolve_relocs(Buf *out){
    for(Rel *r=relocs; r; r=r->next){
        Sym *s = sym_find(r->sym);
        if(!s){ asm_error_loc(1,1,"Undefined symbol: %s", r->sym); return -1; }
        uint64_t tgt = s->addr + (uint64_t)r->addend;
        if(r->kind==REL_ABS64){
            memcpy(out->data + r->off, &tgt, 8);
        } else if(r->kind==REL_ABS32){
            uint32_t v = (uint32_t)tgt; memcpy(out->data + r->off, &v, 4);
        } else if(r->kind==REL_RIP32){
            int64_t next_ip = (int64_t)(r->off + 4);
            int64_t disp = (int64_t)tgt - next_ip;
            uint32_t v = (uint32_t)disp; memcpy(out->data + r->off, &v, 4);
        }
    }
    return 0;
}

/* parse instruction args into operands (table-driven) */
static int parse_operands_for_node(Node *n, Operand *ops, int *out_argc){
    const char *mn = n->mn;
    const char *args = n->args ? n->args : "";
    const InsnEntry *entry = NULL;
    /* We don't know argc until parsing; parse comma-separated tokens first */
    const char *p = args; while(*p && isspace((unsigned char)*p)) p++;
    if(*p==0){ *out_argc = 0; return 0; }
    /* special-case movz/movs handled elsewhere if needed */
    const char *cur = p;
    int argc = 0;
    while(*cur){
        Operand op; memset(&op,0,sizeof(op));
        size_t consumed=0;
        if(!parse_operand(cur, &op, &consumed)){
            asm_error_loc(n->line, (int)(cur - args) + 1, "Operand parse error near: %s", cur);
            return -1;
        }
        ops[argc++] = op;
        cur += consumed;
        while(isspace((unsigned char)*cur)) cur++;
        if(*cur==','){ cur++; while(isspace((unsigned char)*cur)) cur++; continue; }
        else break;
    }
    *out_argc = argc;
    return 0;
}

/* second pass: table-driven emission */
static int second_pass(Node *head){
    cur_loc = 0;
    buf_init(&outbuf);
    for(Node *n=head; n; n=n->next){
        if(n->kind==NODE_BLANK || n->kind==NODE_LABEL) continue;
        if(n->kind==NODE_DIR){
            if(handle_directive(n)!=0) return -1;
            /* handle simple directives that emit bytes */
            if(strcasecmp(n->mn,"align")==0){
                Value v; parse_immediate(n->args, &v, NULL);
                emit_align((uint64_t)v.num);
            } else if(strcasecmp(n->mn,"byte")==0){
                Value v; if(!parse_immediate(n->args,&v,NULL)){ asm_error_loc(n->line,1,".byte expects immediate"); return -1; }
                if(v.is_label){ size_t off=outbuf.len; buf_put8(&outbuf,0); add_reloc(REL_ABS32, off, v.label, v.addend); cur_loc+=1; }
                else { buf_put8(&outbuf,(uint8_t)v.num); cur_loc+=1; }
            } else if(strcasecmp(n->mn,"quad")==0){
                const char *p = n->args; while(isspace((unsigned char)*p)) p++;
                char *ident=NULL; size_t ci=0;
                if(!parse_string_identifier(p, &ident, &ci)){ asm_error_loc(n->line,1,".quad expects identifier"); return -1; }
                p += ci; while(isspace((unsigned char)*p)) p++;
                int64_t add=0;
                if(*p=='-'){ p++; Value vv; if(!parse_immediate(p,&vv,NULL)){ asm_error_loc(n->line,1,".quad offset parse error"); free(ident); return -1; } add = -vv.num; }
                size_t off = outbuf.len; buf_put64(&outbuf, 0); add_reloc(REL_ABS64, off, ident, add); cur_loc += 8;
                free(ident);
            } else if(strcasecmp(n->mn,"zero")==0){
                Value v; if(!parse_immediate(n->args,&v,NULL)){ asm_error_loc(n->line,1,".zero expects immediate"); return -1; }
                for(int64_t i=0;i<v.num;i++){ buf_put8(&outbuf,0); cur_loc++; }
            }
            continue;
        }
        if(n->kind==NODE_INSTR){
            Operand ops[3]; int argc=0;
            if(parse_operands_for_node(n, ops, &argc)!=0) return -1;
            const InsnEntry *entry = find_insn_entry(n->mn, argc, ops);
            if(!entry){ asm_error_loc(n->line,1,"No matching instruction encoding for: %s %s", n->mn, n->args? n->args : ""); return -1; }
            if(strcmp(entry->mn,"movabs")==0 && argc==2){
                /* special-case movabs: opcode B8+rd imm64 */
                if(ops[0].type!=OT_IMM || ops[1].type!=OT_REG){ asm_error_loc(n->line,1,"movabs expects immediate, register"); return -1; }
                RegId rd = ops[1].reg;
                int low = reg_low3(rd) & 7;
                emit_rex(&outbuf, 1, rd, REG_INV, REG_INV);
                buf_put8(&outbuf, 0xB8 + low);
                if(ops[0].imm.is_label){ size_t off = outbuf.len; buf_put64(&outbuf,0); add_reloc(REL_ABS64, off, ops[0].imm.label, ops[0].imm.addend); }
                else buf_put64(&outbuf, (uint64_t)ops[0].imm.num);
                cur_loc = outbuf.len;
                continue;
            }
            if(emit_from_entry(entry, ops, &outbuf, cur_loc, n->line)!=0) return -1;
            cur_loc = outbuf.len;
        }
    }
    return 0;
}

/* ---------------------------
   Resolve relocations and dump
*/
static int finalize_and_dump(){
    if(resolve_relocs(&outbuf)!=0) return -1;
    printf("Assembled %zu bytes:\n", outbuf.len);
    for(size_t i=0;i<outbuf.len;i++){
        printf("%02x", outbuf.data[i]);
        if(i+1<outbuf.len) printf(" ");
        if((i+1)%16==0) printf("\n");
    }
    if(outbuf.len%16) printf("\n");
    printf("\nSymbols:\n");
    for(Sym *s=symtab; s; s=s->next) printf(" %s -> 0x%llx\n", s->name, (unsigned long long)s->addr);
    if(g_errors) fprintf(stderr, "%s: %d error(s), %d warning(s)\n", g_filename, g_errors, g_warnings);
    return g_errors?1:0;
}

/* ---------------------------
   Main
*/
int main(int argc, char **argv){
    FILE *f = stdin;
    if(argc>1){
        f = fopen(argv[1],"r");
        if(!f){ perror("fopen"); return 1; }
        g_filename = argv[1];
    } else g_filename = "<stdin>";

    Node *prog = read_program(f);
    if(f!=stdin) fclose(f);

    buf_init(&outbuf);
    first_pass(prog);
    if(second_pass(prog)!=0){ fprintf(stderr, "%s: assembly aborted due to errors\n", g_filename); return 1; }
    return finalize_and_dump();
}

