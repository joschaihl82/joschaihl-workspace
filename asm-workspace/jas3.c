/*
 * jas.c — Minimal x86-64 AT&T assembler + ELF .o writer + tiny linker
 *
 * Single-mode CLI:
 *   ./jas input.s
 *   -> produces input.o and links it to a.out (chmod 0755)
 *
 * Features:
 * - AT&T-like syntax: %regs, $immediates, disp(base,index,scale)
 * - Labels, global directive
 * - .text, .data, .bss, ascii/byte/dword/qword/skip directives
 * - Encodes ModR/M, SIB, disp8/disp32, RIP-relative memory
 * - Emits ELF64 relocatable .o with .text, .data, .symtab, .strtab, .rela.text, .rela.data, .shstrtab
 * - Minimal linker: concatenates .text/.data, resolves R_X86_64_PC32, R_X86_64_32, R_X86_64_64, writes ELF64 executable with two PT_LOAD segments
 *
 * Limitations:
 * - Not a full assembler; supports a useful subset of instructions and addressing modes
 * - No dynamic linking, no PLT/GOT, no relocations for every possible case
 * - Intended for learning and small experiments
 *
 * Build:
 *   gcc -O2 jas.c -o jas
 *
 * Usage:
 *   ./jas input.s
 *
 * The program will create input.o and link it to a.out, then set executable bit on a.out.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>

/* ---------- Configuration ---------- */
#define INPUT_LINE_MAX 4096
#define NR_NAME_BUCKETS 128
#define MAX_SYMBOLS 32768
#define MAX_RELOCS 32768
#define MAX_SECTION_SIZE (1<<20)

/* ---------- ELF minimal defs ---------- */
#define EI_NIDENT 16
#define ET_REL 1
#define ET_EXEC 2
#define EM_X86_64 62
#define EV_CURRENT 1

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4

#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_WRITE 0x1

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3

#define R_X86_64_64 1
#define R_X86_64_PC32 2
#define R_X86_64_32 10

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t e_type, e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff, e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize, e_phentsize, e_phnum;
    uint16_t e_shentsize, e_shnum, e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t sh_name, sh_type;
    uint64_t sh_flags, sh_addr, sh_offset, sh_size;
    uint32_t sh_link, sh_info;
    uint64_t sh_addralign, sh_entsize;
} Elf64_Shdr;

typedef struct {
    uint32_t st_name;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
} Elf64_Sym;

typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
    int64_t  r_addend;
} Elf64_Rela;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset, p_vaddr, p_paddr;
    uint64_t p_filesz, p_memsz;
    uint64_t p_align;
} Elf64_Phdr;

static inline uint64_t ELF64_R_INFO(uint32_t sym, uint32_t type) {
    return ((uint64_t)sym << 32) | (uint64_t)type;
}
static inline uint32_t ELF64_R_SYM(uint64_t info) { return (uint32_t)(info >> 32); }
static inline uint32_t ELF64_R_TYPE(uint64_t info) { return (uint32_t)(info & 0xFFFFFFFFu); }
static inline unsigned char ELF64_ST_INFO(unsigned char bind, unsigned char type) {
    return (unsigned char)((bind<<4) | (type & 0xF));
}

/* ---------- Logging ---------- */
static void fatal(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    fprintf(stderr, "fatal: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}
static void warnf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    fprintf(stderr, "warn: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

/* ---------- Name / Symbol ---------- */
struct name {
    int length;
    unsigned hash;
    char *text;
    struct name *next;
    struct obj_symbol *sym;
    int strtab_off;
};

struct obj_symbol {
    struct name *n;
    int segment; /* 1=text,2=data,3=bss,-1 unknown */
    uint64_t offset;
    int is_global;
    int elf_index;
};

static struct name *name_buckets[NR_NAME_BUCKETS];
static struct obj_symbol symbols[MAX_SYMBOLS];
static int symbols_count = 0;

static unsigned hash_name(const char *s, int len) {
    const unsigned FNV_OFFSET = 2166136261U;
    const unsigned FNV_PRIME = 16777619U;
    unsigned h = FNV_OFFSET;
    for (int i = 0; i < len; ++i) { h ^= (unsigned)(unsigned char)s[i]; h *= FNV_PRIME; }
    return h;
}

static struct name *lookup_name(const char *text, int len) {
    if (!text || len < 0) return NULL;
    unsigned h = hash_name(text, len);
    unsigned b = h % NR_NAME_BUCKETS;
    for (struct name *n = name_buckets[b]; n; n = n->next) {
        if (n->hash == h && n->length == len && strncmp(n->text, text, len) == 0) return n;
    }
    struct name *n = calloc(1, sizeof(*n));
    if (!n) fatal("out of memory");
    n->length = len; n->hash = h; n->text = malloc(len+1);
    if (!n->text) fatal("out of memory");
    memcpy(n->text, text, len); n->text[len] = 0;
    n->next = name_buckets[b]; name_buckets[b] = n;
    n->sym = NULL; n->strtab_off = 0;
    return n;
}

static struct obj_symbol *ensure_symbol(struct name *n) {
    if (!n) return NULL;
    if (n->sym) return n->sym;
    if (symbols_count >= MAX_SYMBOLS) fatal("too many symbols");
    struct obj_symbol *s = &symbols[symbols_count++];
    s->n = n; s->segment = -1; s->offset = 0; s->is_global = 0; s->elf_index = -1;
    n->sym = s;
    return s;
}

/* ---------- Sections & Relocations ---------- */
static unsigned char text_buf[MAX_SECTION_SIZE];
static unsigned char data_buf[MAX_SECTION_SIZE];
static uint64_t text_len = 0;
static uint64_t data_len = 0;
static uint64_t bss_len = 0;

struct reloc {
    uint64_t offset;
    int section; /* 1=text,2=data */
    struct obj_symbol *sym;
    uint32_t type;
    int64_t addend;
};
static struct reloc relocs[MAX_RELOCS];
static int relocs_count = 0;

static void emit_text_byte(unsigned char b) {
    if (text_len >= MAX_SECTION_SIZE) fatal("text section overflow");
    text_buf[text_len++] = b;
}
static void emit_data_byte(unsigned char b) {
    if (data_len >= MAX_SECTION_SIZE) fatal("data section overflow");
    data_buf[data_len++] = b;
}
static void emit_text_u32(uint32_t v) {
    emit_text_byte((unsigned char)(v & 0xFF));
    emit_text_byte((unsigned char)((v >> 8) & 0xFF));
    emit_text_byte((unsigned char)((v >> 16) & 0xFF));
    emit_text_byte((unsigned char)((v >> 24) & 0xFF));
}
static void emit_data_u32(uint32_t v) {
    emit_data_byte((unsigned char)(v & 0xFF));
    emit_data_byte((unsigned char)((v >> 8) & 0xFF));
    emit_data_byte((unsigned char)((v >> 16) & 0xFF));
    emit_data_byte((unsigned char)((v >> 24) & 0xFF));
}
static void emit_text_u64(uint64_t v) {
    for (int i = 0; i < 8; ++i) emit_text_byte((unsigned char)((v >> (8*i)) & 0xFF));
}
static void emit_data_u64(uint64_t v) {
    for (int i = 0; i < 8; ++i) emit_data_byte((unsigned char)((v >> (8*i)) & 0xFF));
}

static void push_reloc(uint64_t offset, int section, struct obj_symbol *sym, uint32_t type, int64_t addend) {
    if (relocs_count >= MAX_RELOCS) fatal("too many relocations");
    relocs[relocs_count].offset = offset;
    relocs[relocs_count].section = section;
    relocs[relocs_count].sym = sym;
    relocs[relocs_count].type = type;
    relocs[relocs_count].addend = addend;
    relocs_count++;
}

/* ---------- Tokenizer ---------- */
enum TokKind { TK_EOF=0, TK_IDENT, TK_NUMBER, TK_COMMA, TK_COLON, TK_LPAREN, TK_RPAREN, TK_PERCENT, TK_DOLLAR, TK_STRING, TK_PLUS, TK_MINUS };
static enum TokKind tk = TK_EOF;
static char tk_text[512];
static long tk_number = 0;
static char line_buf[INPUT_LINE_MAX];
static char *line_pos;
static int line_no = 0;
static int current_segment = 1; /* 1=text,2=data,3=bss */

static void skip_ws(void) { while (*line_pos && isspace((unsigned char)*line_pos)) line_pos++; }

static void next_tok(void) {
    skip_ws();
    if (!*line_pos) { tk = TK_EOF; return; }
    char c = *line_pos;
    if (c == '#' || (c == '/' && line_pos[1] == '/') || c == ';') { while (*line_pos) line_pos++; tk = TK_EOF; return; }
    if (isalpha((unsigned char)c) || c == '_' || c == '.') {
        int i = 0;
        while (isalnum((unsigned char)*line_pos) || *line_pos == '_' || *line_pos == '.') {
            if (i < (int)sizeof(tk_text)-1) tk_text[i++] = *line_pos;
            line_pos++;
        }
        tk_text[i] = 0; tk = TK_IDENT; return;
    }
    if (isdigit((unsigned char)c)) {
        tk_number = 0;
        if (c == '0' && (line_pos[1] == 'x' || line_pos[1] == 'X')) {
            line_pos += 2;
            while (isxdigit((unsigned char)*line_pos)) {
                char d = *line_pos++;
                if (isdigit((unsigned char)d)) tk_number = (tk_number << 4) + (d - '0');
                else tk_number = (tk_number << 4) + (10 + (tolower((unsigned char)d) - 'a'));
            }
        } else {
            while (isdigit((unsigned char)*line_pos)) { tk_number = tk_number*10 + (*line_pos - '0'); line_pos++; }
        }
        tk = TK_NUMBER; return;
    }
    if (c == '"') {
        line_pos++; int i = 0;
        while (*line_pos && *line_pos != '"') {
            if (*line_pos == '\\' && line_pos[1]) { line_pos++; char esc = *line_pos++; tk_text[i++] = (esc == 'n') ? '\n' : (esc == 't') ? '\t' : esc; }
            else tk_text[i++] = *line_pos++;
            if (i >= (int)sizeof(tk_text)-1) break;
        }
        if (*line_pos == '"') line_pos++;
        tk_text[i] = 0; tk = TK_STRING; return;
    }
    line_pos++;
    switch (c) {
        case ',': tk = TK_COMMA; break;
        case ':': tk = TK_COLON; break;
        case '(': tk = TK_LPAREN; break;
        case ')': tk = TK_RPAREN; break;
        case '%': tk = TK_PERCENT; break;
        case '$': tk = TK_DOLLAR; break;
        case '+': tk = TK_PLUS; break;
        case '-': tk = TK_MINUS; break;
        default: tk = TK_EOF; break;
    }
}

/* ---------- Register helpers ---------- */
static int reg_gpr_index(const char *name) {
    static const char *r64[16] = {"rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14","r15"};
    for (int i=0;i<16;i++) if (strcmp(name, r64[i])==0) return i;
    static const char *r32[16] = {"eax","ecx","edx","ebx","esp","ebp","esi","edi","r8d","r9d","r10d","r11d","r12d","r13d","r14d","r15d"};
    for (int i=0;i<16;i++) if (strcmp(name, r32[i])==0) return 100+i;
    static const char *r8[16] = {"al","cl","dl","bl","spl","bpl","sil","dil","r8b","r9b","r10b","r11b","r12b","r13b","r14b","r15b"};
    for (int i=0;i<16;i++) if (strcmp(name, r8[i])==0) return 200+i;
    return -1;
}
static int reg_xmm_index(const char *name) {
    if (strncmp(name, "xmm", 3) == 0) {
        int n = atoi(name+3);
        if (n >= 0 && n <= 15) return n;
    }
    return -1;
}

/* ---------- Operand parsing ---------- */
struct operand {
    int kind; /* 0=reg,1=imm,2=mem */
    int reg;  /* base reg or reg index */
    int index;
    int scale;
    int rip;
    struct obj_symbol *sym;
    int64_t imm;
    int flags; /* XMM flag in high bit */
};

static int parse_register_after_percent(struct operand *op) {
    if (tk != TK_IDENT) { warnf("expected register after '%%'"); return -1; }
    int xr = reg_xmm_index(tk_text);
    if (xr >= 0) { op->kind = 0; op->reg = xr; op->flags = 1<<16; next_tok(); return 0; }
    int r = reg_gpr_index(tk_text);
    if (r < 0) { warnf("unknown register '%s'", tk_text); return -1; }
    op->kind = 0;
    if (r >= 200) { op->reg = r-200; op->flags = 8; }
    else if (r >= 100) { op->reg = r-100; op->flags = 4; }
    else { op->reg = r; op->flags = 4; }
    next_tok();
    return 0;
}

static int parse_immediate_after_dollar(struct operand *op) {
    if (tk == TK_NUMBER) { op->kind = 1; op->imm = tk_number; next_tok(); return 0; }
    if (tk == TK_IDENT) { struct name *n = lookup_name(tk_text, (int)strlen(tk_text)); op->kind = 1; op->sym = ensure_symbol(n); next_tok(); return 0; }
    warnf("expected immediate after '$'"); return -1;
}

static int parse_memory_operand(struct operand *op) {
    op->kind = 2; op->reg = -1; op->index = -1; op->scale = 1; op->rip = 0; op->sym = NULL; op->imm = 0;
    if (tk == TK_NUMBER) { op->imm = tk_number; next_tok(); }
    else if (tk == TK_MINUS) { next_tok(); if (tk == TK_NUMBER) { op->imm = -tk_number; next_tok(); } else { warnf("expected number after '-'"); return -1; } }
    else if (tk == TK_IDENT) { struct name *n = lookup_name(tk_text, (int)strlen(tk_text)); op->sym = ensure_symbol(n); next_tok(); }
    if (tk != TK_LPAREN) { warnf("expected '(' in memory operand"); return -1; }
    next_tok();
    if (tk == TK_PERCENT) { next_tok(); if (parse_register_after_percent(op) != 0) return -1; }
    else if (tk == TK_IDENT && strcmp(tk_text, "rip") == 0) { op->rip = 1; next_tok(); }
    if (tk == TK_COMMA) {
        next_tok();
        if (tk == TK_PERCENT) { next_tok(); struct operand tmp; if (parse_register_after_percent(&tmp) != 0) return -1; op->index = tmp.reg; }
        if (tk == TK_COMMA) { next_tok(); if (tk == TK_NUMBER) { op->scale = (int)tk_number; next_tok(); } else { warnf("expected scale"); return -1; } }
    }
    if (tk != TK_RPAREN) { warnf("expected ')'"); return -1; }
    next_tok();
    return 0;
}

static int parse_operand_generic(struct operand *op) {
    memset(op, 0, sizeof(*op));
    if (tk == TK_DOLLAR) { next_tok(); return parse_immediate_after_dollar(op); }
    if (tk == TK_PERCENT) { next_tok(); return parse_register_after_percent(op); }
    if (tk == TK_NUMBER || tk == TK_MINUS || tk == TK_IDENT) {
        const char *save_pos = line_pos; enum TokKind save_tk = tk; char save_text[512]; strcpy(save_text, tk_text); long save_num = tk_number;
        next_tok();
        if (tk == TK_LPAREN) { line_pos = (char*)save_pos; tk = save_tk; strcpy(tk_text, save_text); tk_number = save_num; return parse_memory_operand(op); }
        line_pos = (char*)save_pos; tk = save_tk; strcpy(tk_text, save_text); tk_number = save_num;
        if (tk == TK_NUMBER) { op->kind = 1; op->imm = tk_number; next_tok(); return 0; }
        if (tk == TK_IDENT) { struct name *n = lookup_name(tk_text, (int)strlen(tk_text)); op->kind = 1; op->sym = ensure_symbol(n); next_tok(); return 0; }
        if (tk == TK_MINUS) { next_tok(); if (tk == TK_NUMBER) { op->kind = 1; op->imm = -tk_number; next_tok(); return 0; } warnf("expected number after '-'"); return -1; }
    }
    if (tk == TK_LPAREN) return parse_memory_operand(op);
    warnf("unexpected token in operand");
    return -1;
}

/* ---------- ModR/M + SIB + Disp encoder ---------- */

/*
 * emit_modrm_sib_disp:
 *  - reg_field: 3-bit reg field to place into ModR/M.reg
 *  - mem: operand describing memory (kind==2)
 *  - section: 1=text,2=data -> where to emit displacement bytes
 *
 * Behavior:
 *  - Handles base/index/scale, SIB when needed, RIP-relative, disp8/disp32 selection
 *  - Emits bytes into the chosen section (text or data)
 *  - Caller must push reloc if RIP-relative or symbol-only displacement is used
 */
static void emit_modrm_sib_disp(int reg_field, struct operand *mem, int section) {
    #define EMIT_SEC_BYTE(s,b) do { if ((s)==1) emit_text_byte(b); else emit_data_byte(b); } while(0)
    #define EMIT_SEC_U32(s,v) do { if ((s)==1) emit_text_u32(v); else emit_data_u32(v); } while(0)

    int base = mem->reg; /* -1 if none */
    int index = mem->index; /* -1 if none */
    int scale = mem->scale;
    int need_sib = 0;
    int mod = 0;
    int rm = 0;

    if (mem->rip) {
        /* mod=00 rm=5, disp32 follows; caller should add R_X86_64_PC32 relocation */
        EMIT_SEC_BYTE(section, (unsigned char)((0<<6) | ((reg_field&7)<<3) | (5 & 7)));
        EMIT_SEC_U32(section, 0);
        return;
    }

    if (base == -1 && index == -1) {
        /* symbol or absolute disp -> mod=00 rm=5, disp32 */
        EMIT_SEC_BYTE(section, (unsigned char)((0<<6) | ((reg_field&7)<<3) | (5 & 7)));
        EMIT_SEC_U32(section, 0);
        return;
    }

    if (index != -1 || (base != -1 && (base & 7) == 4)) need_sib = 1;

    int64_t disp = mem->imm;
    if (disp == 0) mod = 0;
    else if (disp >= -128 && disp <= 127) mod = 1;
    else mod = 2;

    if (base != -1 && (base & 7) == 5 && mod == 0) {
        /* base == rbp/r13 with mod==0 ambiguous -> use mod=1 disp8==0 */
        mod = 1; disp = 0;
    }

    rm = need_sib ? 4 : (base & 7);
    EMIT_SEC_BYTE(section, (unsigned char)((mod<<6) | ((reg_field&7)<<3) | (rm & 7)));

    if (need_sib) {
        int sib_index = (index == -1) ? 4 : (index & 7);
        int sib_scale = 0;
        if (scale == 1) sib_scale = 0;
        else if (scale == 2) sib_scale = 1;
        else if (scale == 4) sib_scale = 2;
        else if (scale == 8) sib_scale = 3;
        int sib_base = (base == -1) ? 5 : (base & 7);
        EMIT_SEC_BYTE(section, (unsigned char)((sib_scale<<6) | ((sib_index&7)<<3) | (sib_base & 7)));
    }

    if (mod == 1) {
        EMIT_SEC_BYTE(section, (unsigned char)(disp & 0xFF));
    } else if (mod == 2) {
        EMIT_SEC_U32(section, (uint32_t)disp);
    }
    #undef EMIT_SEC_BYTE
    #undef EMIT_SEC_U32
}

/* ---------- Small encoders ---------- */
static void emit_rex(int w,int r,int x,int b) {
    unsigned char rex = 0x40;
    if (w) rex |= 0x08;
    if (r) rex |= 0x04;
    if (x) rex |= 0x02;
    if (b) rex |= 0x01;
    emit_text_byte(rex);
}
static int rex_bit(int reg) { return (reg>>3)&1; }

static void encode_rr64(unsigned char opcode, int dst, int src) {
    int rex_r = rex_bit(src), rex_b = rex_bit(dst);
    emit_rex(1, rex_r, 0, rex_b);
    emit_text_byte(opcode);
    emit_text_byte((unsigned char)((3<<6) | ((src&7)<<3) | (dst&7)));
}

static void encode_mov_imm_reg(int reg, uint32_t imm) {
    int rex_b = rex_bit(reg);
    emit_rex(1, 0, 0, rex_b);
    emit_text_byte((unsigned char)(0xB8 + (reg & 7)));
    emit_text_u32(imm);
}

static void encode_call_rel32_sym(struct obj_symbol *target) {
    emit_text_byte(0xE8);
    uint64_t off = text_len;
    emit_text_u32(0);
    push_reloc(off, 1, target, R_X86_64_PC32, -4);
}

static void encode_jmp_rel32_sym(struct obj_symbol *target) {
    emit_text_byte(0xE9);
    uint64_t off = text_len;
    emit_text_u32(0);
    push_reloc(off, 1, target, R_X86_64_PC32, -4);
}

static void encode_jcc_rel32_sym(unsigned char cc, struct obj_symbol *target) {
    emit_text_byte(0x0F); emit_text_byte(cc);
    uint64_t off = text_len;
    emit_text_u32(0);
    push_reloc(off, 1, target, R_X86_64_PC32, -4);
}

static void encode_sse_rm(unsigned char opcode, int dst_xmm, struct operand *src_mem) {
    int rex_r = (dst_xmm>>3)&1;
    int rex_b = (src_mem->reg != -1) ? ((src_mem->reg>>3)&1) : 0;
    if (rex_r || rex_b) emit_rex(0, rex_r, 0, rex_b);
    emit_text_byte(0x0F); emit_text_byte(opcode);
    emit_modrm_sib_disp(dst_xmm, src_mem, 1);
}

/* ---------- Instruction dispatch (subset) ---------- */
static void assemble_instruction(const char *mnemonic, struct operand *ops, int nops) {
    if (strcmp(mnemonic, "movq") == 0) {
        if (nops==2 && ops[0].kind==0 && ops[1].kind==0) { encode_rr64(0x89, ops[1].reg, ops[0].reg); return; }
        if (nops==2 && ops[0].kind==1 && ops[1].kind==0) { encode_mov_imm_reg(ops[1].reg, (uint32_t)ops[0].imm); if (ops[0].sym) push_reloc(text_len-4, 1, ops[0].sym, R_X86_64_64, 0); return; }
        warnf("unsupported movq form"); return;
    }
    if (strcmp(mnemonic, "addq") == 0) { if (nops==2 && ops[0].kind==0 && ops[1].kind==0) { encode_rr64(0x01, ops[1].reg, ops[0].reg); return; } warnf("unsupported addq"); return; }
    if (strcmp(mnemonic, "subq") == 0) { if (nops==2 && ops[0].kind==0 && ops[1].kind==0) { encode_rr64(0x29, ops[1].reg, ops[0].reg); return; } warnf("unsupported subq"); return; }
    if (strcmp(mnemonic, "cmpq") == 0) { if (nops==2 && ops[0].kind==0 && ops[1].kind==0) { encode_rr64(0x39, ops[1].reg, ops[0].reg); return; } warnf("unsupported cmpq"); return; }
    if (strcmp(mnemonic, "call") == 0) { if (nops==1 && ops[0].kind==1 && ops[0].sym) { encode_call_rel32_sym(ops[0].sym); return; } warnf("unsupported call"); return; }
    if (strcmp(mnemonic, "jmp") == 0) { if (nops==1 && ops[0].kind==1 && ops[0].sym) { encode_jmp_rel32_sym(ops[0].sym); return; } warnf("unsupported jmp"); return; }
    if (strcmp(mnemonic, "je")==0 || strcmp(mnemonic,"jz")==0) { if (nops==1 && ops[0].kind==1 && ops[0].sym) { encode_jcc_rel32_sym(0x84, ops[0].sym); return; } warnf("unsupported je"); return; }
    if (strcmp(mnemonic, "jne")==0 || strcmp(mnemonic,"jnz")==0) { if (nops==1 && ops[0].kind==1 && ops[0].sym) { encode_jcc_rel32_sym(0x85, ops[0].sym); return; } warnf("unsupported jne"); return; }
    if (strcmp(mnemonic, "ret")==0) { emit_text_byte(0xC3); return; }
    if (strcmp(mnemonic, "nop")==0) { emit_text_byte(0x90); return; }
    if (strcmp(mnemonic, "leave")==0) { emit_text_byte(0xC9); return; }
    if (strcmp(mnemonic, "syscall")==0) { emit_text_byte(0x0F); emit_text_byte(0x05); return; }

    if (strcmp(mnemonic, "movaps")==0) {
        if (nops==2 && (ops[0].flags & (1<<16)) && ops[1].kind==2) { encode_sse_rm(0x29, ops[0].reg, &ops[1]); return; }
        if (nops==2 && ops[0].kind==2 && (ops[1].flags & (1<<16))) { encode_sse_rm(0x28, ops[1].reg, &ops[0]); return; }
        warnf("unsupported movaps form"); return;
    }
    if (strcmp(mnemonic, "movups")==0) {
        if (nops==2 && (ops[0].flags & (1<<16)) && ops[1].kind==2) { encode_sse_rm(0x11, ops[0].reg, &ops[1]); return; }
        if (nops==2 && ops[0].kind==2 && (ops[1].flags & (1<<16))) { encode_sse_rm(0x10, ops[1].reg, &ops[0]); return; }
        warnf("unsupported movups form"); return;
    }

    warnf("unknown mnemonic '%s'", mnemonic);
}

/* ---------- Line processing ---------- */
static void define_label(const char *name) {
    struct name *n = lookup_name(name, (int)strlen(name));
    struct obj_symbol *s = ensure_symbol(n);
    s->segment = current_segment;
    if (current_segment == 1) s->offset = text_len;
    else if (current_segment == 2) s->offset = data_len;
    else if (current_segment == 3) s->offset = bss_len;
}

static void process_line(const char *line) {
    strncpy(line_buf, line, sizeof(line_buf)-1); line_buf[sizeof(line_buf)-1] = 0;
    line_pos = line_buf; line_no++;
    next_tok();
    if (tk == TK_EOF) return;

    if (tk == TK_IDENT) {
        char ident[512]; strncpy(ident, tk_text, sizeof(ident)-1); ident[sizeof(ident)-1]=0;
        next_tok();
        if (tk == TK_COLON) { next_tok(); define_label(ident); if (tk == TK_EOF) return; }
        else { line_pos = line_buf; next_tok(); }
    }

    if (tk == TK_IDENT && strcmp(tk_text, "text") == 0) { next_tok(); current_segment = 1; return; }
    if (tk == TK_IDENT && strcmp(tk_text, "data") == 0) { next_tok(); current_segment = 2; return; }
    if (tk == TK_IDENT && strcmp(tk_text, "bss") == 0) { next_tok(); current_segment = 3; return; }
    if (tk == TK_IDENT && strcmp(tk_text, "global") == 0) {
        next_tok();
        if (tk == TK_IDENT) { struct name *n = lookup_name(tk_text, (int)strlen(tk_text)); struct obj_symbol *s = ensure_symbol(n); s->is_global = 1; next_tok(); }
        return;
    }
    if (tk == TK_IDENT && strcmp(tk_text, "ascii") == 0) {
        next_tok();
        if (tk == TK_STRING) {
            const char *s = tk_text;
            for (size_t i = 0; i < strlen(s); ++i) {
                if (current_segment == 1) emit_text_byte((unsigned char)s[i]);
                else if (current_segment == 2) emit_data_byte((unsigned char)s[i]);
            }
            next_tok();
        }
        return;
    }
    if (tk == TK_IDENT && strcmp(tk_text, "byte") == 0) {
        next_tok();
        if (tk == TK_NUMBER) {
            if (current_segment == 1) emit_text_byte((unsigned char)tk_number);
            else if (current_segment == 2) emit_data_byte((unsigned char)tk_number);
            next_tok();
        }
        return;
    }
    if (tk == TK_IDENT && strcmp(tk_text, "dword") == 0) {
        next_tok();
        if (tk == TK_NUMBER) {
            if (current_segment == 1) emit_text_u32((uint32_t)tk_number);
            else if (current_segment == 2) emit_data_u32((uint32_t)tk_number);
            next_tok();
        }
        return;
    }
    if (tk == TK_IDENT && strcmp(tk_text, "qword") == 0) {
        next_tok();
        if (tk == TK_NUMBER) {
            if (current_segment == 1) emit_text_u64((uint64_t)tk_number);
            else if (current_segment == 2) emit_data_u64((uint64_t)tk_number);
            next_tok();
        }
        return;
    }
    if (tk == TK_IDENT && strcmp(tk_text, "skip") == 0) {
        next_tok();
        if (tk == TK_NUMBER) {
            int n = (int)tk_number;
            for (int i = 0; i < n; ++i) {
                if (current_segment == 1) emit_text_byte(0);
                else if (current_segment == 2) emit_data_byte(0);
                else if (current_segment == 3) bss_len++;
            }
            next_tok();
        }
        return;
    }

    if (tk != TK_IDENT) return;
    char mnemonic[64]; strncpy(mnemonic, tk_text, sizeof(mnemonic)-1); mnemonic[sizeof(mnemonic)-1]=0;
    next_tok();
    struct operand ops[4]; int nops = 0;
    if (tk != TK_EOF && tk != TK_COLON) {
        struct operand op;
        if (parse_operand_generic(&op) != 0) return;
        ops[nops++] = op;
        while (tk == TK_COMMA) { next_tok(); if (parse_operand_generic(&op) != 0) return; ops[nops++] = op; }
    }
    assemble_instruction(mnemonic, ops, nops);
}

/* ---------- ELF .o writer ---------- */
static int write_elf64_rel_o(const char *path) {
    /* Build .strtab */
    char *strtab = malloc(1<<14);
    if (!strtab) fatal("out of memory");
    memset(strtab, 0, 1<<14);
    size_t strtab_len = 1;
    for (int i = 0; i < symbols_count; ++i) {
        struct obj_symbol *s = &symbols[i];
        if (s->n && s->n->text && (s->segment == 1 || s->segment == 2)) {
            size_t l = strlen(s->n->text);
            memcpy(strtab + strtab_len, s->n->text, l);
            s->n->strtab_off = (int)strtab_len;
            strtab_len += l; strtab[strtab_len++] = '\0';
        }
    }

    int locals = 0, globals = 0;
    for (int i = 0; i < symbols_count; ++i) if (symbols[i].segment == 1 || symbols[i].segment == 2) { if (symbols[i].is_global) globals++; else locals++; }
    int total_syms = 1 + 2 + locals + globals;

    const char *sec_names[] = {"", ".text", ".data", ".symtab", ".strtab", ".rela.text", ".rela.data", ".shstrtab"};
    char shstr[512]; memset(shstr, 0, sizeof(shstr));
    unsigned sh_name_offs[8]; size_t shstrlen = 1;
    for (int i = 1; i < 8; ++i) {
        sh_name_offs[i] = (unsigned)shstrlen;
        size_t l = strlen(sec_names[i]);
        memcpy(shstr + shstrlen, sec_names[i], l);
        shstrlen += l; shstr[shstrlen++] = '\0';
    }

    Elf64_Ehdr eh; memset(&eh, 0, sizeof(eh));
    memcpy(eh.e_ident, "\177ELF\2\1\1", 7);
    eh.e_type = ET_REL; eh.e_machine = EM_X86_64; eh.e_version = EV_CURRENT;
    eh.e_ehsize = sizeof(Elf64_Ehdr); eh.e_shentsize = sizeof(Elf64_Shdr);
    eh.e_shnum = 8; eh.e_shstrndx = 7;

    uint64_t off = sizeof(Elf64_Ehdr);
    uint64_t text_off = off; uint64_t text_size = (uint64_t)text_len; off += text_size;
    uint64_t data_off = off; uint64_t data_size = (uint64_t)data_len; off += data_size;
    uint64_t sym_off = off; uint64_t sym_size = (uint64_t)(total_syms * sizeof(Elf64_Sym)); off += sym_size;
    uint64_t str_off = off; uint64_t str_size = (uint64_t)strtab_len; off += str_size;

    int rtext = 0, rdata = 0;
    for (int i = 0; i < relocs_count; ++i) { if (relocs[i].section == 1) rtext++; else if (relocs[i].section == 2) rdata++; }
    uint64_t rela_text_off = off; uint64_t rela_text_size = (uint64_t)rtext * sizeof(Elf64_Rela); off += rela_text_size;
    uint64_t rela_data_off = off; uint64_t rela_data_size = (uint64_t)rdata * sizeof(Elf64_Rela); off += rela_data_size;

    uint64_t shstr_off = off; uint64_t shstr_size = shstrlen; off += shstr_size;
    eh.e_shoff = off;

    FILE *f = fopen(path, "wb");
    if (!f) { perror("fopen .o"); free(strtab); return -1; }
    fwrite(&eh, 1, sizeof(eh), f);
    if (text_size) fwrite(text_buf, 1, text_size, f);
    if (data_size) fwrite(data_buf, 1, data_size, f);

    Elf64_Sym *syms = calloc(total_syms, sizeof(Elf64_Sym));
    if (!syms) fatal("out of memory");
    int si = 0;
    syms[si].st_name = 0; syms[si].st_info = ELF64_ST_INFO(STB_LOCAL, STT_NOTYPE); syms[si].st_shndx = 0; si++;
    syms[si].st_name = 0; syms[si].st_info = ELF64_ST_INFO(STB_LOCAL, STT_SECTION); syms[si].st_shndx = 1; si++;
    syms[si].st_name = 0; syms[si].st_info = ELF64_ST_INFO(STB_LOCAL, STT_SECTION); syms[si].st_shndx = 2; si++;
    int locals_start = si;

    for (int i = 0; i < symbols_count; ++i) {
        struct obj_symbol *s = &symbols[i];
        if ((s->segment == 1 || s->segment == 2) && !s->is_global) {
            syms[si].st_name = (uint32_t)(s->n ? s->n->strtab_off : 0);
            syms[si].st_info = ELF64_ST_INFO(STB_LOCAL, (s->segment==2?STT_OBJECT:STT_FUNC));
            syms[si].st_shndx = (s->segment==1?1:2);
            syms[si].st_value = s->offset;
            s->elf_index = si;
            si++;
        }
    }
    for (int i = 0; i < symbols_count; ++i) {
        struct obj_symbol *s = &symbols[i];
        if ((s->segment == 1 || s->segment == 2) && s->is_global) {
            syms[si].st_name = (uint32_t)(s->n ? s->n->strtab_off : 0);
            syms[si].st_info = ELF64_ST_INFO(STB_GLOBAL, (s->segment==2?STT_OBJECT:STT_FUNC));
            syms[si].st_shndx = (s->segment==1?1:2);
            syms[si].st_value = s->offset;
            s->elf_index = si;
            si++;
        }
    }
    fwrite(syms, 1, sym_size, f);
    fwrite(strtab, 1, str_size, f);

    if (rtext) {
        Elf64_Rela *rbuf = calloc(rtext, sizeof(Elf64_Rela));
        int idx = 0;
        for (int i = 0; i < relocs_count; ++i) if (relocs[i].section == 1) {
            struct reloc *R = &relocs[i];
            int sidx = R->sym ? R->sym->elf_index : 0;
            rbuf[idx].r_offset = R->offset;
            rbuf[idx].r_info = ELF64_R_INFO((uint32_t)sidx, R->type);
            rbuf[idx].r_addend = R->addend;
            idx++;
        }
        fwrite(rbuf, 1, rela_text_size, f);
        free(rbuf);
    }
    if (rdata) {
        Elf64_Rela *rbuf = calloc(rdata, sizeof(Elf64_Rela));
        int idx = 0;
        for (int i = 0; i < relocs_count; ++i) if (relocs[i].section == 2) {
            struct reloc *R = &relocs[i];
            int sidx = R->sym ? R->sym->elf_index : 0;
            rbuf[idx].r_offset = R->offset;
            rbuf[idx].r_info = ELF64_R_INFO((uint32_t)sidx, R->type);
            rbuf[idx].r_addend = R->addend;
            idx++;
        }
        fwrite(rbuf, 1, rela_data_size, f);
        free(rbuf);
    }

    Elf64_Shdr sh[8]; memset(sh, 0, sizeof(sh));
    sh[1].sh_name = sh_name_offs[1]; sh[1].sh_type = SHT_PROGBITS; sh[1].sh_flags = SHF_ALLOC | SHF_EXECINSTR; sh[1].sh_offset = text_off; sh[1].sh_size = text_size; sh[1].sh_addralign = 16;
    sh[2].sh_name = sh_name_offs[2]; sh[2].sh_type = SHT_PROGBITS; sh[2].sh_flags = SHF_ALLOC | SHF_WRITE; sh[2].sh_offset = data_off; sh[2].sh_size = data_size; sh[2].sh_addralign = 8;
    sh[3].sh_name = sh_name_offs[3]; sh[3].sh_type = SHT_SYMTAB; sh[3].sh_offset = sym_off; sh[3].sh_size = sym_size; sh[3].sh_link = 4; sh[3].sh_info = locals_start + locals; sh[3].sh_addralign = 8; sh[3].sh_entsize = sizeof(Elf64_Sym);
    sh[4].sh_name = sh_name_offs[4]; sh[4].sh_type = SHT_STRTAB; sh[4].sh_offset = str_off; sh[4].sh_size = str_size; sh[4].sh_addralign = 1;
    sh[5].sh_name = sh_name_offs[5]; sh[5].sh_type = SHT_RELA; sh[5].sh_offset = rela_text_off; sh[5].sh_size = rela_text_size; sh[5].sh_link = 3; sh[5].sh_info = 1; sh[5].sh_addralign = 8; sh[5].sh_entsize = sizeof(Elf64_Rela);
    sh[6].sh_name = sh_name_offs[6]; sh[6].sh_type = SHT_RELA; sh[6].sh_offset = rela_data_off; sh[6].sh_size = rela_data_size; sh[6].sh_link = 3; sh[6].sh_info = 2; sh[6].sh_addralign = 8; sh[6].sh_entsize = sizeof(Elf64_Rela);
    sh[7].sh_name = sh_name_offs[7]; sh[7].sh_type = SHT_STRTAB; sh[7].sh_offset = shstr_off; sh[7].sh_size = shstr_size; sh[7].sh_addralign = 1;

    fwrite(sh, 1, sizeof(sh), f);
    fclose(f);
    free(syms); free(strtab);
    return 0;
}

/* ---------- Linker: read .o and write exec ---------- */
typedef struct {
    uint8_t *text; size_t text_size;
    uint8_t *data; size_t data_size;
    Elf64_Sym *symtab; size_t sym_count;
    char *strtab; size_t str_size;
    Elf64_Rela *rela_text; size_t rela_text_count;
    Elf64_Rela *rela_data; size_t rela_data_count;
} Obj;

static int read_obj(const char *path, Obj *o) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); return -1; }
    Elf64_Ehdr eh;
    if (fread(&eh, 1, sizeof(eh), f) != sizeof(eh)) { fclose(f); return -1; }
    Elf64_Shdr *sh = malloc(eh.e_shnum * sizeof(Elf64_Shdr));
    if (!sh) { fclose(f); return -1; }
    fseek(f, eh.e_shoff, SEEK_SET);
    fread(sh, 1, eh.e_shnum * sizeof(Elf64_Shdr), f);

    Elf64_Shdr sh_text = {0}, sh_data = {0}, sh_sym = {0}, sh_str = {0}, sh_rela_text = {0}, sh_rela_data = {0};
    for (int i = 0; i < eh.e_shnum; ++i) {
        if (sh[i].sh_type == SHT_PROGBITS && (sh[i].sh_flags & SHF_EXECINSTR)) sh_text = sh[i];
        else if (sh[i].sh_type == SHT_PROGBITS && (sh[i].sh_flags & SHF_WRITE)) sh_data = sh[i];
        else if (sh[i].sh_type == SHT_SYMTAB) sh_sym = sh[i];
        else if (sh[i].sh_type == SHT_STRTAB && i != eh.e_shstrndx) sh_str = sh[i];
        else if (sh[i].sh_type == SHT_RELA && sh[i].sh_info == 1) sh_rela_text = sh[i];
        else if (sh[i].sh_type == SHT_RELA && sh[i].sh_info == 2) sh_rela_data = sh[i];
    }

    o->text_size = sh_text.sh_size; o->text = malloc(o->text_size); if (o->text_size) { fseek(f, sh_text.sh_offset, SEEK_SET); fread(o->text, 1, o->text_size, f); }
    o->data_size = sh_data.sh_size; o->data = malloc(o->data_size); if (o->data_size) { fseek(f, sh_data.sh_offset, SEEK_SET); fread(o->data, 1, o->data_size, f); }
    o->sym_count = sh_sym.sh_size / sizeof(Elf64_Sym); o->symtab = malloc(sh_sym.sh_size); if (o->sym_count) { fseek(f, sh_sym.sh_offset, SEEK_SET); fread(o->symtab, 1, sh_sym.sh_size, f); }
    o->str_size = sh_str.sh_size; o->strtab = malloc(o->str_size); if (o->str_size) { fseek(f, sh_str.sh_offset, SEEK_SET); fread(o->strtab, 1, o->str_size, f); }
    o->rela_text_count = sh_rela_text.sh_size / sizeof(Elf64_Rela); o->rela_text = malloc(sh_rela_text.sh_size); if (o->rela_text_count) { fseek(f, sh_rela_text.sh_offset, SEEK_SET); fread(o->rela_text, 1, sh_rela_text.sh_size, f); }
    o->rela_data_count = sh_rela_data.sh_size / sizeof(Elf64_Rela); o->rela_data = malloc(sh_rela_data.sh_size); if (o->rela_data_count) { fseek(f, sh_rela_data.sh_offset, SEEK_SET); fread(o->rela_data, 1, sh_rela_data.sh_size, f); }

    free(sh); fclose(f); return 0;
}

static int write_exec(const char *path, Obj *objs, int nobj) {
    const uint64_t base = 0x400000;
    size_t total_text = 0, total_data = 0;
    for (int i = 0; i < nobj; ++i) { total_text += objs[i].text_size; total_data += objs[i].data_size; }
    uint64_t text_vaddr = base + 0x1000;
    uint64_t data_vaddr = text_vaddr + ((total_text + 0xFFF) & ~0xFFF);

    uint64_t *text_va = calloc(nobj, sizeof(uint64_t));
    uint64_t *data_va = calloc(nobj, sizeof(uint64_t));
    uint64_t cur_text = text_vaddr, cur_data = data_vaddr;
    for (int i = 0; i < nobj; ++i) { text_va[i] = cur_text; cur_text += objs[i].text_size; data_va[i] = cur_data; cur_data += objs[i].data_size; }

    typedef struct { char *name; uint64_t addr; } GSym;
    GSym *gs = calloc(16384, sizeof(GSym)); int gcount = 0;
    for (int i = 0; i < nobj; ++i) {
        for (size_t s = 0; s < objs[i].sym_count; ++s) {
            Elf64_Sym *sym = &objs[i].symtab[s];
            unsigned char type = sym->st_info & 0xF;
            if (sym->st_shndx == 1 || sym->st_shndx == 2) {
                char *name = objs[i].strtab + sym->st_name;
                uint64_t addr = (sym->st_shndx == 1) ? (text_va[i] + sym->st_value) : (data_va[i] + sym->st_value);
                gs[gcount].name = strdup(name); gs[gcount].addr = addr; gcount++;
            }
        }
    }

    for (int i = 0; i < nobj; ++i) {
        for (size_t r = 0; r < objs[i].rela_text_count; ++r) {
            Elf64_Rela *R = &objs[i].rela_text[r];
            uint32_t si = ELF64_R_SYM(R->r_info);
            uint32_t tp = ELF64_R_TYPE(R->r_info);
            Elf64_Sym *sym = &objs[i].symtab[si];
            char *name = objs[i].strtab + sym->st_name;
            uint64_t target = 0;
            for (int g = 0; g < gcount; ++g) if (strcmp(gs[g].name, name) == 0) { target = gs[g].addr; break; }
            uint64_t place = text_va[i] + R->r_offset;
            if (tp == R_X86_64_PC32) {
                int32_t disp = (int32_t)(target - (place + 4) + R->r_addend);
                uint8_t *p = objs[i].text + R->r_offset;
                p[0] = (uint8_t)(disp & 0xFF); p[1] = (uint8_t)((disp >> 8) & 0xFF); p[2] = (uint8_t)((disp >> 16) & 0xFF); p[3] = (uint8_t)((disp >> 24) & 0xFF);
            } else if (tp == R_X86_64_32) {
                uint32_t val = (uint32_t)(target + R->r_addend);
                uint8_t *p = objs[i].text + R->r_offset;
                p[0] = (uint8_t)(val & 0xFF); p[1] = (uint8_t)((val >> 8) & 0xFF); p[2] = (uint8_t)((val >> 16) & 0xFF); p[3] = (uint8_t)((val >> 24) & 0xFF);
            } else if (tp == R_X86_64_64) {
                uint64_t val = target + R->r_addend;
                uint8_t *p = objs[i].text + R->r_offset;
                for (int b = 0; b < 8; ++b) p[b] = (uint8_t)((val >> (8*b)) & 0xFF);
            }
        }
        for (size_t r = 0; r < objs[i].rela_data_count; ++r) {
            Elf64_Rela *R = &objs[i].rela_data[r];
            uint32_t si = ELF64_R_SYM(R->r_info);
            uint32_t tp = ELF64_R_TYPE(R->r_info);
            Elf64_Sym *sym = &objs[i].symtab[si];
            char *name = objs[i].strtab + sym->st_name;
            uint64_t target = 0;
            for (int g = 0; g < gcount; ++g) if (strcmp(gs[g].name, name) == 0) { target = gs[g].addr; break; }
            uint64_t place = data_va[i] + R->r_offset;
            if (tp == R_X86_64_32) {
                uint32_t val = (uint32_t)(target + R->r_addend);
                uint8_t *p = objs[i].data + R->r_offset;
                p[0] = (uint8_t)(val & 0xFF); p[1] = (uint8_t)((val >> 8) & 0xFF); p[2] = (uint8_t)((val >> 16) & 0xFF); p[3] = (uint8_t)((val >> 24) & 0xFF);
            } else if (tp == R_X86_64_64) {
                uint64_t val = target + R->r_addend;
                uint8_t *p = objs[i].data + R->r_offset;
                for (int b = 0; b < 8; ++b) p[b] = (uint8_t)((val >> (8*b)) & 0xFF);
            } else if (tp == R_X86_64_PC32) {
                int32_t disp = (int32_t)(target - (place + 4) + R->r_addend);
                uint8_t *p = objs[i].data + R->r_offset;
                p[0] = (uint8_t)(disp & 0xFF); p[1] = (uint8_t)((disp >> 8) & 0xFF); p[2] = (uint8_t)((disp >> 16) & 0xFF); p[3] = (uint8_t)((disp >> 24) & 0xFF);
            }
        }
    }

    FILE *f = fopen(path, "wb");
    if (!f) { perror("fopen exec"); return -1; }
    Elf64_Ehdr eh; memset(&eh, 0, sizeof(eh)); memcpy(eh.e_ident, "\177ELF\2\1\1", 7);
    eh.e_type = ET_EXEC; eh.e_machine = EM_X86_64; eh.e_version = EV_CURRENT;
    uint64_t entry = text_vaddr;
    for (int g = 0; g < gcount; ++g) { if (!strcmp(gs[g].name, "main")) { entry = gs[g].addr; break; } if (!strcmp(gs[g].name, "_start")) { entry = gs[g].addr; break; } }
    eh.e_entry = entry;
    eh.e_ehsize = sizeof(eh); eh.e_phoff = sizeof(eh); eh.e_phentsize = sizeof(Elf64_Phdr); eh.e_phnum = 2;

    Elf64_Phdr ph_text; memset(&ph_text, 0, sizeof(ph_text));
    ph_text.p_type = 1; ph_text.p_flags = 5; ph_text.p_offset = 0; ph_text.p_vaddr = base; ph_text.p_paddr = base;
    ph_text.p_filesz = sizeof(eh) + 2*sizeof(Elf64_Phdr) + total_text + total_data; ph_text.p_memsz = ph_text.p_filesz; ph_text.p_align = 0x1000;

    Elf64_Phdr ph_data; memset(&ph_data, 0, sizeof(ph_data));
    ph_data.p_type = 1; ph_data.p_flags = 6; ph_data.p_offset = sizeof(eh) + 2*sizeof(Elf64_Phdr) + total_text; ph_data.p_vaddr = data_vaddr; ph_data.p_paddr = data_vaddr;
    ph_data.p_filesz = total_data; ph_data.p_memsz = total_data; ph_data.p_align = 0x1000;

    fwrite(&eh, 1, sizeof(eh), f);
    fwrite(&ph_text, 1, sizeof(ph_text), f);
    fwrite(&ph_data, 1, sizeof(ph_data), f);
    for (int i = 0; i < nobj; ++i) if (objs[i].text_size) fwrite(objs[i].text, 1, objs[i].text_size, f);
    for (int i = 0; i < nobj; ++i) if (objs[i].data_size) fwrite(objs[i].data, 1, objs[i].data_size, f);
    fclose(f);

    for (int i = 0; i < gcount; ++i) free(gs[i].name);
    free(gs); free(text_va); free(data_va);
    return 0;
}

/* ---------- Driver: assemble_file and main ---------- */
static void process_file_lines(FILE *f) {
    char buf[INPUT_LINE_MAX];
    while (fgets(buf, sizeof(buf), f)) process_line(buf);
}

static int assemble_file(const char *in_path, const char *out_o) {
    FILE *f = fopen(in_path, "r");
    if (!f) { perror("fopen input"); return -1; }
    current_segment = 1;
    process_file_lines(f);
    fclose(f);
    if (write_elf64_rel_o(out_o) != 0) return -1;
    fprintf(stderr, "wrote %s (text=%llu data=%llu bss=%llu relocs=%d symbols=%d)\n", out_o, (unsigned long long)text_len, (unsigned long long)data_len, (unsigned long long)bss_len, relocs_count, symbols_count);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s input.s\n", argv[0]);
        return 1;
    }
    const char *infile = argv[1];
    char out_o[4096];
    strncpy(out_o, infile, sizeof(out_o)-1); out_o[sizeof(out_o)-1] = '\0';
    char *dot = strrchr(out_o, '.');
    if (dot) *dot = '\0';
    strncat(out_o, ".o", sizeof(out_o)-strlen(out_o)-1);

    if (assemble_file(infile, out_o) != 0) {
        fprintf(stderr, "assembly failed for %s\n", infile);
        return 1;
    }
    fprintf(stderr, "assembled -> %s\n", out_o);

    const char *exe_name = "a.out";
    Obj single;
    if (read_obj(out_o, &single) != 0) {
        fprintf(stderr, "failed to read object %s for linking\n", out_o);
        return 1;
    }
    Obj *objs = calloc(1, sizeof(Obj));
    if (!objs) { fprintf(stderr, "out of memory\n"); free(single.text); free(single.data); free(single.symtab); free(single.strtab); free(single.rela_text); free(single.rela_data); return 1; }
    objs[0] = single;
    if (write_exec(exe_name, objs, 1) != 0) {
        fprintf(stderr, "linking failed\n");
        free(single.text); free(single.data); free(single.symtab); free(single.strtab); free(single.rela_text); free(single.rela_data);
        free(objs);
        return 1;
    }
    free(single.text); free(single.data); free(single.symtab); free(single.strtab); free(single.rela_text); free(single.rela_data);
    free(objs);

    if (chmod(exe_name, 0755) != 0) {
        fprintf(stderr, "chmod failed: %s\n", strerror(errno));
    } else {
        fprintf(stderr, "set executable permissions on %s\n", exe_name);
    }
    fprintf(stderr, "linked -> %s\n", exe_name);
    return 0;
}

