/*
 * asm.c — Minimal C99 assembler scaffold with AT&T syntax parsing.
 *
 * This file:
 *  - Implements all structs, flags, globals, and externs from the provided header.
 *  - Parses AT&T syntax: registers prefixed with '%', immediates with '$',
 *    memory as disp(base,index,scale), and mnemonic size suffixes (e.g., movq).
 *  - Provides a small encoder subset for x86-64: movq/addq/subq/jmp (short).
 *  - Supports simple pseudo-ops (byte/word/dword/qword/align/skip/fill/ascii/global/text/data/bss/org/bits).
 *  - Emits raw hex dump of text/data sections to stdout by default.
 *
 * Notes:
 *  - This is a teaching scaffold, not a complete assembler.
 *  - Operands are kept in AT&T order: src, dst. Encoders use operands[1] as dst.
 *  - Memory encoding is limited (only register-reg form implemented here).
 *  - Extend as needed for real use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

/* ===== Configuration ===== */

#ifndef MAX_OPERANDS
#define MAX_OPERANDS            4
#endif

#ifndef MAX_INSN_OPCODES
#define MAX_INSN_OPCODES        16
#endif

#ifndef INPUT_LINE_MAX
#define INPUT_LINE_MAX          2048
#endif

#ifndef NR_NAME_BUCKETS
#define NR_NAME_BUCKETS         32
#endif

#ifndef MAX_SYMBOLS
#define MAX_SYMBOLS             8192
#endif

/* ===== Forward declarations of opaque types used in externs ===== */

struct obj_symbol;
struct obj_header;

/* ===== Name table ===== */

struct name
{
    int                 length;
    unsigned            hash;
    char              * data;
    struct name       * link;
    struct obj_symbol * symbol;
    struct insn       * insn_entries;
    int             ( * pseudo )();
    int                 token;
};

/* ===== Operand kinds ===== */

#define OPERAND_REG     0
#define OPERAND_IMM     1
#define OPERAND_MEM     2

/* ===== Operand flags ===== */

/* immediate operands. all the bits that apply to the actual immediate value are set. */
#define O_IMM_S8        0x0000000000000001L  
#define O_IMM_U8        0x0000000000000002L
#define O_IMM_S16       0x0000000000000004L
#define O_IMM_U16       0x0000000000000008L
#define O_IMM_S32       0x0000000000000010L
#define O_IMM_U32       0x0000000000000020L
#define O_IMM_64        0x0000000000000040L

/* the O_REG_* are mutually exclusive, and refer to the general purpose registers. AX et al are also O_ACC. */
#define O_REG_8         0x0000000000000080L     /* registers */
#define O_REG_16        0x0000000000000100L
#define O_REG_32        0x0000000000000200L
#define O_REG_64        0x0000000000000400L

#define O_ACC_8         0x0000000000000800L     /* AL */
#define O_ACC_16        0x0000000000001000L     /* AX */
#define O_ACC_32        0x0000000000002000L     /* EAX */
#define O_ACC_64        0x0000000000004000L     /* RAX */

/* O_MEM_* cover memory "standard" memory references that can be represented in a mod/rm field. */
#define O_MEM_8         0x0000000000008000L     
#define O_MEM_16        0x0000000000010000L     
#define O_MEM_32        0x0000000000020000L  
#define O_MEM_64        0x0000000000040000L

/* O_MEM and O_MOFS references must have exactly one of O_ADDR_* set, to indicate the address size. */
#define O_ADDR_16       0x0000000000080000L  
#define O_ADDR_32       0x0000000000100000L   
#define O_ADDR_64       0x0000000000200000L

/* segment registers */
#define O_SREG2         0x0000000000400000L     /* 2-bit seg reg: SS, DS, ES, CS */
#define O_SREG3         0x0000000000800000L     /* 3-bit seg reg: FS, GS */

#define O_REL_8         0x0000000001000000L     /* 8-bit relative/immediate for short jumps */

/* non-standard memory references (direct displacements). */
#define O_MOFS_8        0x0000000002000000L     
#define O_MOFS_16       0x0000000004000000L     
#define O_MOFS_32       0x0000000008000000L  
#define O_MOFS_64       0x0000000010000000L

/* high (8-15) and low (0-7) control registers */
#define O_REG_CRL       0x0000000020000000L
#define O_REG_CRH       0x0000000040000000L

/* specifically CL (for shifts) */
#define O_REG_CL        0x0000000080000000L

/* XMM registers */
#define O_REG_XMM       0x0000000100000000L

/* useful classes */
#define O_IMM_8         (O_IMM_S8 | O_IMM_U8)
#define O_IMM_16        (O_IMM_S16 | O_IMM_U16)
#define O_IMM_32        (O_IMM_S32 | O_IMM_U32)
#define O_IMM           (O_IMM_8 | O_IMM_16 | O_IMM_32 | O_IMM_64)   

#define O_MRM_8         (O_MEM_8 | O_REG_8)     /* operands suitable for mod/rm */
#define O_MRM_16        (O_MEM_16 | O_REG_16)
#define O_MRM_32        (O_MEM_32 | O_REG_32)
#define O_MRM_64        (O_MEM_64 | O_REG_64)

#define O_MEM           (O_MEM_8 | O_MEM_16 | O_MEM_32 | O_MEM_64 )
#define O_MOFS          (O_MOFS_8 | O_MOFS_16 | O_MOFS_32 | O_MOFS_64 )

/* these are in the same bitspace as operand.flags, but only ever appear in insn.operand_flags[] to aid in matching or encoding. */
#define O_I_ENDREG      0x8000000000000000L     /* xxxxxRRR and REX.B */
#define O_I_MIDREG      0x4000000000000000L     /* xxRRRxxx and REX.R */
#define O_I_MODRM       0x2000000000000000L     /* MMxxxRRR and REX.B/REX.X */
#define O_I_REL         0x1000000000000000L     /* emit this immediate as rIP-relative */

/* ===== Operand ===== */

struct operand
{
    int                 kind;           /* OPERAND_* */
    int                 reg;       
    int                 rip;
    int                 index;
    int                 scale;
    struct obj_symbol * symbol;
    long                offset;
    long                flags;          /* O_* */
    long                disp;           /* O_IMM_* indicating displacement size */
};

/* ===== Instruction table ===== */

struct insn
{
    char        * mnemonic;
    int           nr_operands;
    long          operand_flags[MAX_OPERANDS];      /* O_* and O_I_* */
    int           nr_opcodes;
    char          opcodes[MAX_INSN_OPCODES];
    long          insn_flags;                       /* I_* */

    struct name * name;
};

/* instruction flags help with either matching or encoding */
#define I_DATA_8        0x0000000000000001L     /* operand size */
#define I_DATA_16       0x0000000000000002L  
#define I_DATA_32       0x0000000000000004L  
#define I_DATA_64       0x0000000000000008L

#define I_PREFIX_66     0x0200000000000000L     /* 0x66 prefix (precedes REX) */
#define I_PREFIX_F3     0x0400000000000000L     /* 0xF3 prefix (precedes REX) */
#define I_PREFIX_F2     0x0800000000000000L     /* 0xF2 prefix (precedes REX) */

#define I_NO_DATA_REX   0x1000000000000000L     /* 64-bit operand default: no REX for 64-bit data */
#define I_NO_BITS_16    0x2000000000000000L     /* instruction not available in .bits 16 */
#define I_NO_BITS_32    0x4000000000000000L     /* instruction not available in .bits 32 */
#define I_NO_BITS_64    0x8000000000000000L     /* instruction not available in .bits 64 */

/* ===== Globals (definitions) ===== */

struct insn *insn = NULL;
struct operand operands[MAX_OPERANDS] = {0};
int nr_operands = 0;

/* Assembler state globals */
int               pass               = 0;
int               line_number        = 0;
int               input_index        = 0;
char           ** input_paths        = NULL;
int               token              = 0;
struct name     * name_token         = NULL;
long              number_token       = 0;
char              input_line[INPUT_LINE_MAX] = {0};
char            * input_pos          = input_line;
FILE            * list_file          = NULL;
FILE            * output_file        = NULL;
int               segment            = 0;
int               text_bytes         = 0;
int               data_bytes         = 0;
int               nr_symbols         = 0;
int               nr_symbol_changes  = 0;
int               nr_relocs          = 0;
int               name_bytes         = 0;
int               base_address       = 0;
int               bits               = 64; /* default to 64-bit for modern targets */

/* Object header */
struct obj_header {
    unsigned magic;
    unsigned version;
    unsigned flags;
    unsigned entry;
};

struct obj_header header = { 0xDEADBEEF, 1, 0, 0 };

/* ===== Symbol table ===== */

struct obj_symbol {
    struct name *n;
    int segment;        /* 1=text, 2=data, 3=bss */
    long offset;        /* byte offset in segment */
    int is_global;
};

static struct obj_symbol symbols[MAX_SYMBOLS] = {0};
static int symbols_count = 0;

/* ===== Simple output buffer per segment ===== */

#define MAX_SECTION_SIZE (1<<20)
static unsigned char text_buf[MAX_SECTION_SIZE];
static unsigned char data_buf[MAX_SECTION_SIZE];
static long text_len = 0;
static long data_len = 0;

/* ===== Name infrastructure ===== */
static struct name * name_buckets[NR_NAME_BUCKETS] = { NULL };

static unsigned hash_name(const char *s, int len)
{
    const unsigned FNV_OFFSET = 2166136261U;
    const unsigned FNV_PRIME  = 16777619U;

    unsigned h = FNV_OFFSET;
    for (int i = 0; i < len; ++i) {
        h ^= (unsigned)(unsigned char)s[i];
        h *= FNV_PRIME;
    }
    return h;
}

struct name * lookup_name(const char *data, int len)
{
    if (!data || len < 0) return NULL;

    unsigned h = hash_name(data, len);
    unsigned bucket = h % NR_NAME_BUCKETS;

    struct name *n = name_buckets[bucket];
    for (; n; n = n->link) {
        if (n->hash == h && n->length == len && strncmp(n->data, data, (size_t)len) == 0) {
            return n;
        }
    }

    n = (struct name *)calloc(1, sizeof(*n));
    if (!n) return NULL;

    n->length = len;
    n->hash = h;
    n->data = (char *)malloc((size_t)len + 1);
    if (!n->data) {
        free(n);
        return NULL;
    }
    memcpy(n->data, data, (size_t)len);
    n->data[len] = '\0';

    n->link = name_buckets[bucket];
    name_buckets[bucket] = n;

    n->symbol = NULL;
    n->insn_entries = NULL;
    n->pseudo = NULL;
    n->token = 0;
    return n;
}

/* ===== Utility ===== */

static void emit_byte(unsigned char b)
{
    if (segment == 1) {
        if (text_len >= MAX_SECTION_SIZE) { fprintf(stderr,"error: text overflow\n"); return; }
        text_buf[text_len++] = b; text_bytes++;
    } else if (segment == 2) {
        if (data_len >= MAX_SECTION_SIZE) { fprintf(stderr,"error: data overflow\n"); return; }
        data_buf[data_len++] = b; data_bytes++;
    } else if (segment == 3) {
        data_bytes++; /* bss reservation */
    } else {
        fprintf(stderr,"error: no active segment to emit\n");
    }
}

static void emit_u32(unsigned v)
{
    emit_byte((unsigned char)(v & 0xFF));
    emit_byte((unsigned char)((v >> 8) & 0xFF));
    emit_byte((unsigned char)((v >> 16) & 0xFF));
    emit_byte((unsigned char)((v >> 24) & 0xFF));
}

static int ensure_output_file(void)
{
    if (!output_file) output_file = stdout;
    return 0;
}

/* ===== Expression & classification ===== */

long constant_expression(void)
{
    return number_token;
}

long classify(struct operand *op)
{
    if (!op) return 0;
    switch (op->kind) {
        case OPERAND_REG:
            return op->flags & (O_REG_8 | O_REG_16 | O_REG_32 | O_REG_64 | O_REG_CL | O_REG_XMM | O_REG_CRL | O_REG_CRH);
        case OPERAND_IMM:
            return op->flags & O_IMM;
        case OPERAND_MEM:
            return op->flags & (O_MEM | O_ADDR_16 | O_ADDR_32 | O_ADDR_64 | O_MOFS);
        default:
            return 0;
    }
}

/* ===== Pseudo-ops ===== */

int pseudo_byte(void)   { emit_byte((unsigned char)(number_token & 0xFF)); return 0; }
int pseudo_word(void)   { emit_byte((unsigned char)(number_token & 0xFF)); emit_byte((unsigned char)((number_token >> 8) & 0xFF)); return 0; }
int pseudo_dword(void)  { emit_u32((unsigned)number_token); return 0; }
int pseudo_qword(void)  { /* emit 8 bytes little-endian */ for (int i=0;i<8;i++) emit_byte((unsigned char)((number_token >> (8*i)) & 0xFF)); return 0; }

int pseudo_align(void)
{
    int align_to = (int)number_token;
    if (align_to <= 0) return 0;
    long pos = (segment == 1) ? text_len : (segment == 2 ? data_len : data_bytes);
    long pad = ((pos + align_to - 1) / align_to) * align_to - pos;
    while (pad-- > 0) emit_byte(0);
    return 0;
}

int pseudo_skip(void)   { for (long i=0;i<number_token;i++) emit_byte(0); return 0; }

int pseudo_fill(void)
{
    unsigned char val = (unsigned char)(number_token & 0xFF);
    long count = number_token >> 8; if (count <= 0) count = 1;
    for (long i=0;i<count;i++) emit_byte(val);
    return 0;
}

int pseudo_ascii(void)
{
    if (!name_token || !name_token->data) return 0;
    const char *s = name_token->data;
    for (size_t i=0;i<strlen(s);i++) emit_byte((unsigned char)s[i]);
    return 0;
}

int pseudo_global(void)
{
    if (!name_token) return 0;
    if (!name_token->symbol) {
        if (symbols_count >= MAX_SYMBOLS) { fprintf(stderr,"error: too many symbols\n"); return -1; }
        struct obj_symbol *sym = &symbols[symbols_count++];
        sym->n = name_token;
        sym->segment = segment;
        sym->offset = (segment == 1) ? text_len : (segment == 2 ? data_len : data_bytes);
        sym->is_global = 1;
        name_token->symbol = sym;
        nr_symbols++;
    } else {
        name_token->symbol->is_global = 1;
    }
    return 0;
}

int pseudo_text(void)  { segment = 1; return 0; }
int pseudo_data(void)  { segment = 2; return 0; }
int pseudo_bss(void)   { segment = 3; return 0; }
int pseudo_org(void)   { base_address = (int)number_token; return 0; }
int pseudo_bits(void)  { bits = (int)number_token; return 0; }

/* ===== Error reporting ===== */

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "error (line %d): ", line_number);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
}

/* ===== Register parsing (AT&T names, prefixed with %) ===== */

static int reg_from_name(const char *s)
{
    /* Return reg index 0..15 for rax..r15 (64-bit),
       100+ for eax..r15d, 200+ for al..r15b. */
    static const char *r64[16] = {
        "rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi",
        "r8","r9","r10","r11","r12","r13","r14","r15"
    };
    for (int i = 0; i < 16; ++i) if (strcmp(s, r64[i]) == 0) return i;
    static const char *r32[16] = {
        "eax","ecx","edx","ebx","esp","ebp","esi","edi",
        "r8d","r9d","r10d","r11d","r12d","r13d","r14d","r15d"
    };
    for (int i = 0; i < 16; ++i) if (strcmp(s, r32[i]) == 0) return 100+i;
    static const char *r8[16] = {
        "al","cl","dl","bl","spl","bpl","sil","dil",
        "r8b","r9b","r10b","r11b","r12b","r13b","r14b","r15b"
    };
    for (int i = 0; i < 16; ++i) if (strcmp(s, r8[i]) == 0) return 200+i;
    return -1;
}

/* ===== Mini tokenizer (AT&T-aware) ===== */

enum TokKind {
    T_EOF=0, T_IDENT, T_NUMBER, T_COMMA, T_COLON, T_LPAREN, T_RPAREN, T_STAR, T_PLUS, T_MINUS, T_STRING, T_PERCENT, T_DOLLAR
};

static enum TokKind tkind = T_EOF;
static char tok_text[256];
static long tok_number = 0;

static void skip_ws(void)
{
    while (*input_pos && isspace((unsigned char)*input_pos)) input_pos++;
}

static void next_token(void)
{
    skip_ws();
    if (!*input_pos) { tkind = T_EOF; return; }

    char c = *input_pos;

    if (c == ';') { /* comment to end of line */
        while (*input_pos && *input_pos!='\n') input_pos++;
        tkind = T_EOF; return;
    }

    if (isalpha((unsigned char)c) || c=='_' || c=='.') {
        int i=0;
        while (isalnum((unsigned char)*input_pos) || *input_pos=='_' || *input_pos=='.') {
            if (i < (int)sizeof(tok_text)-1) tok_text[i++] = *input_pos;
            input_pos++;
        }
        tok_text[i] = 0;
        tkind = T_IDENT;
        return;
    }
    if (isdigit((unsigned char)c)) {
        tok_number = 0;
        int base = 10;
        if (c=='0' && (input_pos[1]=='x' || input_pos[1]=='X')) {
            input_pos += 2;
            base = 16;
            while (isxdigit((unsigned char)*input_pos)) {
                char d = *input_pos++;
                if (isdigit((unsigned char)d)) tok_number = tok_number*16 + (d - '0');
                else tok_number = tok_number*16 + (10 + (tolower((unsigned char)d) - 'a'));
            }
        } else {
            while (isdigit((unsigned char)*input_pos)) {
                tok_number = tok_number*10 + (*input_pos - '0');
                input_pos++;
            }
        }
        tkind = T_NUMBER;
        return;
    }
    if (c=='"') {
        input_pos++;
        int i=0;
        while (*input_pos && *input_pos!='"') {
            if (*input_pos=='\\' && input_pos[1]) {
                input_pos++;
                char esc = *input_pos++;
                switch (esc) {
                    case 'n': tok_text[i++] = '\n'; break;
                    case 't': tok_text[i++] = '\t'; break;
                    case '"': tok_text[i++] = '"'; break;
                    case '\\': tok_text[i++]='\\'; break;
                    default: tok_text[i++] = esc; break;
                }
            } else {
                tok_text[i++] = *input_pos++;
            }
            if (i >= (int)sizeof(tok_text)-1) break;
        }
        if (*input_pos=='"') input_pos++;
        tok_text[i]=0;
        tkind = T_STRING;
        return;
    }

    input_pos++;
    switch (c) {
        case ',': tkind = T_COMMA; break;
        case ':': tkind = T_COLON; break;
        case '(': tkind = T_LPAREN; break;
        case ')': tkind = T_RPAREN; break;
        case '*': tkind = T_STAR; break;
        case '+': tkind = T_PLUS; break;
        case '-': tkind = T_MINUS; break;
        case '%': tkind = T_PERCENT; break;
        case '$': tkind = T_DOLLAR; break;
        default:  tkind = T_EOF; break;
    }
}

/* ===== Operand parsing (AT&T) ===== */

static int parse_register_after_percent(struct operand *op)
{
    if (tkind != T_IDENT) { error("expected register after '%%'"); return -1; }
    int r = reg_from_name(tok_text);
    if (r < 0) { error("unknown register '%s'", tok_text); return -1; }
    op->kind = OPERAND_REG;
    if (r >= 200) { op->reg = r-200; op->flags = O_REG_8; }
    else if (r >= 100) { op->reg = r-100; op->flags = O_REG_32; }
    else { op->reg = r; op->flags = O_REG_64; }
    next_token();
    return 0;
}

static int parse_immediate_after_dollar(struct operand *op)
{
    if (tkind == T_NUMBER) {
        op->kind = OPERAND_IMM;
        op->flags = O_IMM_64;
        op->offset = tok_number;
        next_token();
        return 0;
    } else if (tkind == T_IDENT) {
        /* symbol as immediate (will need relocation in real assembler) */
        struct name *n = lookup_name(tok_text, (int)strlen(tok_text));
        op->kind = OPERAND_IMM;
        op->flags = O_IMM_64;
        op->symbol = n ? n->symbol : NULL;
        next_token();
        return 0;
    }
    error("expected number or symbol after '$'");
    return -1;
}

static int parse_memory_operand(struct operand *op)
{
    /* AT&T: disp(base,index,scale)
       Examples:
         8(%rax)             -> disp=8, base=rax
         (%rax,%rcx,4)       -> base=rax, index=rcx, scale=4
         symbol(%rip)        -> RIP-relative symbol
         -16(%rbp)           -> disp=-16, base=rbp
       We only record fields; encoder for actual [reg] form is not implemented here.
    */
    op->kind = OPERAND_MEM;
    op->flags = O_MEM_64 | O_ADDR_64;
    op->reg = -1;
    op->index = -1;
    op->scale = 1;
    op->rip = 0;
    op->offset = 0;
    op->symbol = NULL;

    /* Optional displacement or symbol before '(' */
    if (tkind == T_NUMBER) {
        op->offset = tok_number;
        next_token();
    } else if (tkind == T_MINUS) {
        next_token();
        if (tkind == T_NUMBER) { op->offset = -tok_number; next_token(); }
        else { error("expected number after '-'"); return -1; }
    } else if (tkind == T_IDENT) {
        struct name *n = lookup_name(tok_text, (int)strlen(tok_text));
        op->symbol = n ? n->symbol : NULL;
        next_token();
    }

    if (tkind != T_LPAREN) { error("expected '(' in memory operand"); return -1; }
    next_token();

    /* base */
    if (tkind == T_PERCENT) {
        next_token();
        struct operand rop = {0};
        if (parse_register_after_percent(&rop) != 0) return -1;
        op->reg = rop.reg;
    } else if (tkind == T_IDENT && strcmp(tok_text,"rip")==0) {
        op->rip = 1;
        next_token();
    }

    /* optional , index */
    if (tkind == T_COMMA) {
        next_token();
        if (tkind == T_PERCENT) {
            next_token();
            struct operand iop = {0};
            if (parse_register_after_percent(&iop) != 0) return -1;
            op->index = iop.reg;
        }
        /* optional , scale */
        if (tkind == T_COMMA) {
            next_token();
            if (tkind == T_NUMBER) {
                op->scale = (int)tok_number;
                next_token();
            } else {
                error("expected scale number");
                return -1;
            }
        }
    }

    if (tkind != T_RPAREN) { error("expected ')' to close memory operand"); return -1; }
    next_token();
    return 0;
}

static int parse_operand(struct operand *op)
{
    memset(op, 0, sizeof(*op));

    if (tkind == T_DOLLAR) {
        next_token();
        return parse_immediate_after_dollar(op);
    } else if (tkind == T_PERCENT) {
        next_token();
        return parse_register_after_percent(op);
    } else if (tkind == T_NUMBER || tkind == T_MINUS || tkind == T_IDENT) {
        /* Could be memory starting with disp/symbol then '(' ... */
        /* Lookahead for '(' */
        const char *save_pos = input_pos;
        enum TokKind save_tk = tkind;
        char save_text[256]; strcpy(save_text, tok_text);
        long save_num = tok_number;

        /* consume optional disp/symbol */
        if (tkind == T_NUMBER || tkind == T_MINUS || tkind == T_IDENT) {
            next_token();
        }
        if (tkind == T_LPAREN) {
            /* restore and parse as memory */
            input_pos = (char *)save_pos;
            tkind = save_tk;
            strcpy(tok_text, save_text);
            tok_number = save_num;
            return parse_memory_operand(op);
        } else {
            /* restore and treat as immediate symbol/number (no memory) */
            input_pos = (char *)save_pos;
            tkind = save_tk;
            strcpy(tok_text, save_text);
            tok_number = save_num;

            if (tkind == T_NUMBER) {
                op->kind = OPERAND_IMM;
                op->flags = O_IMM_64;
                op->offset = tok_number;
                next_token();
                return 0;
            } else if (tkind == T_IDENT) {
                struct name *n = lookup_name(tok_text, (int)strlen(tok_text));
                op->kind = OPERAND_IMM;
                op->flags = O_IMM_64;
                op->symbol = n ? n->symbol : NULL;
                next_token();
                return 0;
            } else if (tkind == T_MINUS) {
                next_token();
                if (tkind == T_NUMBER) { op->kind=OPERAND_IMM; op->flags=O_IMM_64; op->offset=-tok_number; next_token(); return 0; }
                error("expected number after '-'"); return -1;
            }
        }
    } else if (tkind == T_LPAREN) {
        return parse_memory_operand(op);
    }

    error("unexpected token in operand");
    return -1;
}

/* ===== Instruction table (tiny AT&T subset) ===== */

static struct insn insn_table[] = {
    { "movq", 2, { O_MRM_64, O_MRM_64 }, 1, { (char)0x89 }, I_DATA_64, NULL }, /* movq src, dst => encode dst/src mapping */
    { "movq", 2, { O_REG_64, O_IMM_32 }, 1, { (char)0xB8 }, I_DATA_64, NULL }, /* movq $imm32, %r64 */
    { "addq", 2, { O_MRM_64, O_REG_64 }, 1, { (char)0x01 }, I_DATA_64, NULL }, /* addq src, dst */
    { "subq", 2, { O_MRM_64, O_REG_64 }, 1, { (char)0x29 }, I_DATA_64, NULL }, /* subq src, dst */
    { "jmp",  1, { O_IMM | O_MEM | O_MOFS | O_REL_8 }, 1, { (char)0xEB }, I_DATA_8, NULL },  /* jmp rel8 (short) */
};

/* ===== Encoding helpers ===== */

static void emit_rex(int w, int r, int x, int b)
{
    unsigned char rex = 0x40;
    if (w) rex |= 0x08;
    if (r) rex |= 0x04;
    if (x) rex |= 0x02;
    if (b) rex |= 0x01;
    emit_byte(rex);
}

static void emit_modrm(int mod, int reg, int rm)
{
    emit_byte((unsigned char)((mod<<6) | ((reg&7)<<3) | (rm&7)));
}

static void encode_mov_rr64(int dst, int src)
{
    /* mov r/m64, r64 with r/m as reg => 0x89 /r ; dst is r/m, src is reg */
    int rex_r = (src >> 3) & 1;
    int rex_b = (dst >> 3) & 1;
    emit_rex(1, rex_r, 0, rex_b);
    emit_byte(0x89);
    emit_modrm(3, src, dst);
}

static void encode_mov_r64_imm32(int reg, unsigned imm32)
{
    int rex_b = (reg >> 3) & 1;
    emit_rex(1, 0, 0, rex_b);
    emit_byte((unsigned char)(0xB8 + (reg & 7)));
    emit_u32(imm32);
}

static void encode_add_rr64(int dst_rm, int src_reg)
{
    int rex_r = (src_reg >> 3) & 1;
    int rex_b = (dst_rm >> 3) & 1;
    emit_rex(1, rex_r, 0, rex_b);
    emit_byte(0x01);
    emit_modrm(3, src_reg, dst_rm);
}

static void encode_sub_rr64(int dst_rm, int src_reg)
{
    int rex_r = (src_reg >> 3) & 1;
    int rex_b = (dst_rm >> 3) & 1;
    emit_rex(1, rex_r, 0, rex_b);
    emit_byte(0x29);
    emit_modrm(3, src_reg, dst_rm);
}

static void encode_jmp_rel8(long disp)
{
    emit_byte(0xEB);
    emit_byte((unsigned char)(disp & 0xFF));
}

/* ===== Instruction match ===== */

static int match_insn(const char *mnemonic, struct insn **out)
{
    for (size_t i = 0; i < sizeof(insn_table)/sizeof(insn_table[0]); ++i) {
        if (strcmp(mnemonic, insn_table[i].mnemonic) == 0) {
            *out = &insn_table[i];
            return 0;
        }
    }
    return -1;
}

/* ===== Parse an instruction line (AT&T) ===== */

static int parse_instruction(void)
{
    if (tkind != T_IDENT) return -1;
    char mnemonic[64];
    strncpy(mnemonic, tok_text, sizeof(mnemonic)-1);
    mnemonic[sizeof(mnemonic)-1]=0;
    next_token();

    nr_operands = 0;
    if (tkind != T_EOF && tkind != T_COLON) {
        struct operand op;
        if (parse_operand(&op) != 0) return -1;
        operands[nr_operands++] = op;
        while (tkind == T_COMMA) {
            next_token();
            if (parse_operand(&op) != 0) return -1;
            operands[nr_operands++] = op;
        }
    }

    struct insn *found = NULL;
    if (match_insn(mnemonic, &found) != 0) {
        /* Pseudo ops by mnemonic (AT&T: same words) */
        if (strcmp(mnemonic,"byte")==0) { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_byte(); }
        if (strcmp(mnemonic,"word")==0) { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_word(); }
        if (strcmp(mnemonic,"dword")==0){ number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_dword(); }
        if (strcmp(mnemonic,"qword")==0){ number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_qword(); }
        if (strcmp(mnemonic,"align")==0){ number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_align(); }
        if (strcmp(mnemonic,"skip")==0) { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_skip(); }
        if (strcmp(mnemonic,"fill")==0) { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_fill(); }
        if (strcmp(mnemonic,"ascii")==0){
            if (nr_operands==1 && operands[0].kind==OPERAND_IMM && name_token) return pseudo_ascii();
            return 0;
        }
        if (strcmp(mnemonic,"global")==0){
            if (nr_operands==1 && operands[0].kind==OPERAND_IMM && name_token) return pseudo_global();
            return 0;
        }
        if (strcmp(mnemonic,"text")==0)  return pseudo_text();
        if (strcmp(mnemonic,"data")==0)  return pseudo_data();
        if (strcmp(mnemonic,"bss")==0)   return pseudo_bss();
        if (strcmp(mnemonic,"org")==0)   { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_org(); }
        if (strcmp(mnemonic,"bits")==0)  { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:64; return pseudo_bits(); }
        error("unknown mnemonic '%s'", mnemonic);
        return -1;
    }

    /* AT&T order: src, dst. Our encoders take dst first parameter where applicable. */
    if (strcmp(mnemonic,"movq")==0) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_mov_rr64(operands[1].reg, operands[0].reg); /* dst <- src */
            return 0;
        }
        if (nr_operands==2 && operands[0].kind==OPERAND_IMM && operands[1].kind==OPERAND_REG &&
            (operands[1].flags & O_REG_64)) {
            unsigned imm32 = (unsigned)(operands[0].offset & 0xFFFFFFFFu);
            encode_mov_r64_imm32(operands[1].reg, imm32);
            return 0;
        }
        error("unsupported movq form");
        return -1;
    } else if (strcmp(mnemonic,"addq")==0) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_add_rr64(operands[1].reg, operands[0].reg); /* dst += src */
            return 0;
        }
        error("unsupported addq form");
        return -1;
    } else if (strcmp(mnemonic,"subq")==0) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_sub_rr64(operands[1].reg, operands[0].reg); /* dst -= src */
            return 0;
        }
        error("unsupported subq form");
        return -1;
    } else if (strcmp(mnemonic,"jmp")==0) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) {
            encode_jmp_rel8(operands[0].offset);
            return 0;
        }
        error("unsupported jmp form");
        return -1;
    }

    error("unimplemented mnemonic '%s'", mnemonic);
    return -1;
}

/* ===== Label handling ===== */

static void define_label(const char *name)
{
    struct name *n = lookup_name(name, (int)strlen(name));
    if (!n) { error("out of memory for label"); return; }
    if (!n->symbol) {
        if (symbols_count >= MAX_SYMBOLS) { error("too many symbols"); return; }
        struct obj_symbol *sym = &symbols[symbols_count++];
        sym->n = n;
        sym->segment = segment;
        sym->offset = (segment==1)? text_len : (segment==2? data_len : data_bytes);
        sym->is_global = 0;
        n->symbol = sym;
        nr_symbols++;
    } else {
        n->symbol->segment = segment;
        n->symbol->offset = (segment==1)? text_len : (segment==2? data_len : data_bytes);
        nr_symbol_changes++;
    }
}

/* ===== Line processing ===== */

static void process_line(const char *line)
{
    strncpy(input_line, line, sizeof(input_line)-1);
    input_line[sizeof(input_line)-1]=0;
    input_pos = input_line;
    line_number++;

    /* strip comments starting with '#' or '//' or ';' */
    char *p = input_line;
    while (*p) {
        if (*p=='#' || (*p=='/' && p[1]=='/') || *p==';') { *p=0; break; }
        p++;
    }

    next_token();
    if (tkind == T_EOF) return;

    /* label: ident ':' */
    if (tkind == T_IDENT) {
        char ident[256];
        strncpy(ident, tok_text, sizeof(ident)-1);
        ident[sizeof(ident)-1]=0;
        next_token();
        if (tkind == T_COLON) {
            next_token();
            define_label(ident);
            if (tkind == T_EOF) return;
        } else {
            /* rewind to start to treat as mnemonic */
            input_pos = input_line;
            next_token();
        }
    }

    /* prepare name_token for ascii/global (capture following token) */
    name_token = NULL;
    const char *save_pos = input_pos;
    enum TokKind save_tk = tkind;
    char save_text2[256]; strcpy(save_text2, tok_text);

    if (tkind == T_IDENT && (strcmp(tok_text,"ascii")==0 || strcmp(tok_text,"global")==0)) {
        char mnemonic[64]; strncpy(mnemonic, tok_text, sizeof(mnemonic)-1); mnemonic[sizeof(mnemonic)-1]=0;
        next_token();
        if (strcmp(mnemonic,"ascii")==0 && tkind==T_STRING) {
            name_token = lookup_name(tok_text, (int)strlen(tok_text));
        } else if (strcmp(mnemonic,"global")==0 && tkind==T_IDENT) {
            name_token = lookup_name(tok_text, (int)strlen(tok_text));
        }
        /* reset to start to reparse fully */
        input_pos = (char *)save_pos;
        tkind = save_tk;
        strcpy(tok_text, save_text2);
    }

    parse_instruction();
}

/* ===== Emission of final output ===== */

static void write_output(void)
{
    ensure_output_file();
    fprintf(output_file, "; asm.c minimal AT&T output\n");
    fprintf(output_file, "; text_len=%ld data_len=%ld bits=%d base=%d\n", text_len, data_len, bits, base_address);

    fprintf(output_file, "section .text\n");
    for (long i=0;i<text_len;i++) {
        fprintf(output_file, "%02X", text_buf[i]);
        if ((i%16)==15 || i==text_len-1) fputc('\n', output_file);
        else fputc(' ', output_file);
    }
    fprintf(output_file, "section .data\n");
    for (long i=0;i<data_len;i++) {
        fprintf(output_file, "%02X", data_buf[i]);
        if ((i%16)==15 || i==data_len-1) fputc('\n', output_file);
        else fputc(' ', output_file);
    }
    fflush(output_file);
}

/* ===== Public API stubs to satisfy externs if included elsewhere ===== */

struct insn insns[] = { /* empty, using local insn_table instead */ };

/* ===== Optional tiny driver ===== */


int main(int argc, char **argv)
{
    output_file = stdout;
    segment = 1; /* default to .text */

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) { perror("fopen"); return 1; }
        char buf[INPUT_LINE_MAX];
        while (fgets(buf, sizeof(buf), f)) process_line(buf);
        fclose(f);
    } else {
        /* Demo program with AT&T syntax */
        const char *program[] = {
            "bits 64",
            "text",
            "global main",
            "main:",
            "  movq $1, %rax",
            "  movq %rax, %rbx",
            "  addq %rax, %rbx",
            "  subq %rax, %rbx",
            "  jmp 2",
            "data",
            "msg: ascii \"Hello, world!\"",
            NULL
        };
        for (int i=0; program[i]; ++i) process_line(program[i]);
    }
    write_output();
    return 0;
}

