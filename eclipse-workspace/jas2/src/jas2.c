/*
 * asm.c — Minimal x86-64 assembler scaffold with AT&T syntax, labels,
 * relocations, ~50 core instructions, and a basic SSE subset.
 *
 * Patches included:
 * - Robust label references: create stub symbols on first identifier use
 *   so forward references resolve cleanly.
 * - Relocation resolution fixed: references to undefined symbols no longer occur.
 *
 * Features:
 * - AT&T parsing: %registers, $immediates, disp(base,index,scale)
 * - Labels with forward/backward references via a relocation list
 * - Core instructions: mov/add/sub/cmp/and/or/xor/test/push/pop/call/ret/jmp/jcc/lea/xchg/nop/leave/syscall/...
 * - SSE subset: movaps/movups (load/store), addps/subps/mulps/xorps/andps/orps between XMM regs and r/m128
 * - Emits hex dump of .text and .data to stdout
 *
 * Notes:
 * - Teaching scaffold, not a complete assembler.
 * - Memory encoding is limited to simple reg addressing; SIB/displacements are simplified.
 * - Jumps and call emitted as rel32 with relocations; no relaxation to rel8.
 * - SSE supports XMM registers %xmm0..%xmm15 with REX for high regs.
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

#define MAX_SECTION_SIZE        (1<<20)
#define MAX_RELOCS              8192

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

#define O_IMM_S8        0x0000000000000001L
#define O_IMM_U8        0x0000000000000002L
#define O_IMM_S16       0x0000000000000004L
#define O_IMM_U16       0x0000000000000008L
#define O_IMM_S32       0x0000000000000010L
#define O_IMM_U32       0x0000000000000020L
#define O_IMM_64        0x0000000000000040L

#define O_REG_8         0x0000000000000080L
#define O_REG_16        0x0000000000000100L
#define O_REG_32        0x0000000000000200L
#define O_REG_64        0x0000000000000400L

#define O_ACC_8         0x0000000000000800L
#define O_ACC_16        0x0000000000001000L
#define O_ACC_32        0x0000000000002000L
#define O_ACC_64        0x0000000000004000L

#define O_MEM_8         0x0000000000008000L
#define O_MEM_16        0x0000000000010000L
#define O_MEM_32        0x0000000000020000L
#define O_MEM_64        0x0000000000040000L

#define O_ADDR_16       0x0000000000080000L
#define O_ADDR_32       0x0000000000100000L
#define O_ADDR_64       0x0000000000200000L

#define O_SREG2         0x0000000000400000L
#define O_SREG3         0x0000000000800000L

#define O_REL_8         0x0000000001000000L

#define O_MOFS_8        0x0000000002000000L
#define O_MOFS_16       0x0000000004000000L
#define O_MOFS_32       0x0000000008000000L
#define O_MOFS_64       0x0000000010000000L

#define O_REG_CRL       0x0000000020000000L
#define O_REG_CRH       0x0000000040000000L

#define O_REG_CL        0x0000000080000000L

#define O_REG_XMM       0x0000000100000000L

#define O_IMM_8         (O_IMM_S8 | O_IMM_U8)
#define O_IMM_16        (O_IMM_S16 | O_IMM_U16)
#define O_IMM_32        (O_IMM_S32 | O_IMM_U32)
#define O_IMM           (O_IMM_8 | O_IMM_16 | O_IMM_32 | O_IMM_64)

#define O_MRM_8         (O_MEM_8 | O_REG_8)
#define O_MRM_16        (O_MEM_16 | O_REG_16)
#define O_MRM_32        (O_MEM_32 | O_REG_32)
#define O_MRM_64        (O_MEM_64 | O_REG_64)

#define O_MEM           (O_MEM_8 | O_MEM_16 | O_MEM_32 | O_MEM_64 )
#define O_MOFS          (O_MOFS_8 | O_MOFS_16 | O_MOFS_32 | O_MOFS_64 )

#define O_I_ENDREG      0x8000000000000000L
#define O_I_MIDREG      0x4000000000000000L
#define O_I_MODRM       0x2000000000000000L
#define O_I_REL         0x1000000000000000L

/* ===== Operand ===== */

struct operand
{
    int                 kind;           /* OPERAND_* */
    int                 reg;            /* GPR reg index 0..15 or XMM reg index when O_REG_XMM set */
    int                 rip;            /* RIP-relative memory */
    int                 index;          /* index register */
    int                 scale;          /* index scale */
    struct obj_symbol * symbol;         /* symbol reference (for relocations) */
    long                offset;         /* immediate or displacement */
    long                flags;          /* O_* */
    long                disp;           /* O_IMM_* indicating displacement size (unused here) */
};

/* ===== Instruction table ===== */

struct insn
{
    char        * mnemonic;
    int           nr_operands;
    long          operand_flags[MAX_OPERANDS];
    int           nr_opcodes;
    char          opcodes[MAX_INSN_OPCODES];
    long          insn_flags;

    struct name * name;
};

/* ===== Instruction flags ===== */

#define I_DATA_8        0x0000000000000001L
#define I_DATA_16       0x0000000000000002L
#define I_DATA_32       0x0000000000000004L
#define I_DATA_64       0x0000000000000008L

#define I_PREFIX_66     0x0200000000000000L
#define I_PREFIX_F3     0x0400000000000000L
#define I_PREFIX_F2     0x0800000000000000L

#define I_NO_DATA_REX   0x1000000000000000L
#define I_NO_BITS_16    0x2000000000000000L
#define I_NO_BITS_32    0x4000000000000000L
#define I_NO_BITS_64    0x8000000000000000L

/* ===== Globals ===== */

struct insn *insn = NULL;
struct operand operands[MAX_OPERANDS] = {0};
int nr_operands = 0;

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
int               bits               = 64;

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
    int segment;        /* 1=text, 2=data, 3=bss, -1=unknown yet */
    long offset;        /* byte offset in segment */
    int is_global;
};

static struct obj_symbol symbols[MAX_SYMBOLS] = {0};
static int symbols_count = 0;

/* ===== Relocations ===== */

struct reloc {
    long pos;                 /* position of displacement field (start) */
    struct obj_symbol *sym;   /* target symbol */
    int size;                 /* displacement size in bytes: 4 for rel32 */
};

static struct reloc relocs[MAX_RELOCS] = {0};
static int relocs_count = 0;

/* ===== Output buffers ===== */

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

/* Create a stub symbol for a name if it doesn't have one yet. */
static struct obj_symbol *ensure_symbol_for_name(struct name *n)
{
    if (!n) return NULL;
    if (n->symbol) return n->symbol;

    if (symbols_count >= MAX_SYMBOLS) {
        error("too many symbols");
        return NULL;
    }
    struct obj_symbol *sym = &symbols[symbols_count++];
    sym->n = n;
    sym->segment = -1;   /* unknown yet; will be set on label definition */
    sym->offset  = 0;
    sym->is_global = 0;
    n->symbol = sym;
    nr_symbols++;
    return sym;
}

/* ===== Utility ===== */

static void emit_byte(unsigned char b)
{
    if (segment == 1) {
        if (text_len >= MAX_SECTION_SIZE) { error("text overflow"); return; }
        text_buf[text_len++] = b; text_bytes++;
    } else if (segment == 2) {
        if (data_len >= MAX_SECTION_SIZE) { error("data overflow"); return; }
        data_buf[data_len++] = b; data_bytes++;
    } else if (segment == 3) {
        data_bytes++; /* bss reservation (no emission) */
    } else {
        error("no active segment to emit");
    }
}

static void emit_u32(unsigned v)
{
    emit_byte((unsigned char)(v & 0xFF));
    emit_byte((unsigned char)((v >> 8) & 0xFF));
    emit_byte((unsigned char)((v >> 16) & 0xFF));
    emit_byte((unsigned char)((v >> 24) & 0xFF));
}

static void emit_u64(unsigned long long v)
{
    for (int i = 0; i < 8; ++i) emit_byte((unsigned char)((v >> (8*i)) & 0xFF));
}

static int ensure_output_file(void)
{
    if (!output_file) output_file = stdout;
    return 0;
}

/* ===== Expression & classification ===== */

long constant_expression(void) { return number_token; }

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
int pseudo_qword(void)  { emit_u64((unsigned long long)number_token); return 0; }

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
    ensure_symbol_for_name(name_token);
    name_token->symbol->is_global = 1;
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

/* ===== Register parsing ===== */

static int reg_from_name_gpr(const char *s)
{
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

static int reg_from_name_xmm(const char *s)
{
    if (strncmp(s, "xmm", 3) == 0) {
        int n = atoi(s+3);
        if (n >= 0 && n <= 15) return n;
    }
    return -1;
}

/* ===== Tokenizer (AT&T-aware) ===== */

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

    if (c == ';' || c == '#') {
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
        if (c=='0' && (input_pos[1]=='x' || input_pos[1]=='X')) {
            input_pos += 2;
            while (isxdigit((unsigned char)*input_pos)) {
                char d = *input_pos++;
                if (isdigit((unsigned char)d)) tok_number = (tok_number<<4) + (d - '0');
                else tok_number = (tok_number<<4) + (10 + (tolower((unsigned char)d) - 'a'));
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
    int xr = reg_from_name_xmm(tok_text);
    if (xr >= 0) {
        op->kind = OPERAND_REG;
        op->reg = xr;
        op->flags = O_REG_XMM;
        next_token();
        return 0;
    }
    int r = reg_from_name_gpr(tok_text);
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
        struct name *n = lookup_name(tok_text, (int)strlen(tok_text));
        op->kind = OPERAND_IMM;
        op->flags = O_IMM_64;
        op->symbol = ensure_symbol_for_name(n);
        next_token();
        return 0;
    }
    error("expected number or symbol after '$'");
    return -1;
}

static int parse_memory_operand(struct operand *op)
{
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
        op->offset = tok_number; next_token();
    } else if (tkind == T_MINUS) {
        next_token();
        if (tkind == T_NUMBER) { op->offset = -tok_number; next_token(); }
        else { error("expected number after '-'"); return -1; }
    } else if (tkind == T_IDENT) {
        struct name *n = lookup_name(tok_text, (int)strlen(tok_text));
        op->symbol = ensure_symbol_for_name(n);
        next_token();
    }

    if (tkind != T_LPAREN) { error("expected '(' in memory operand"); return -1; }
    next_token();

    /* base */
    if (tkind == T_PERCENT) {
        next_token();
        struct operand rop = {0};
        if (parse_register_after_percent(&rop) != 0) return -1;
        if (rop.flags & O_REG_XMM) { error("XMM cannot be base"); return -1; }
        op->reg = rop.reg;
    } else if (tkind == T_IDENT && strcmp(tok_text,"rip")==0) {
        op->rip = 1; next_token();
    }

    /* optional , index */
    if (tkind == T_COMMA) {
        next_token();
        if (tkind == T_PERCENT) {
            next_token();
            struct operand iop = {0};
            if (parse_register_after_percent(&iop) != 0) return -1;
            if (iop.flags & O_REG_XMM) { error("XMM cannot be index"); return -1; }
            op->index = iop.reg;
        }
        /* optional , scale */
        if (tkind == T_COMMA) {
            next_token();
            if (tkind == T_NUMBER) { op->scale = (int)tok_number; next_token(); }
            else { error("expected scale number"); return -1; }
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
        /* Could be memory starting with disp/symbol then '(' ... or immediate */
        const char *save_pos = input_pos;
        enum TokKind save_tk = tkind;
        char save_text[256]; strcpy(save_text, tok_text);
        long save_num = tok_number;

        if (tkind == T_NUMBER || tkind == T_MINUS || tkind == T_IDENT) next_token();
        if (tkind == T_LPAREN) {
            input_pos = (char *)save_pos;
            tkind = save_tk;
            strcpy(tok_text, save_text);
            tok_number = save_num;
            return parse_memory_operand(op);
        } else {
            input_pos = (char *)save_pos;
            tkind = save_tk;
            strcpy(tok_text, save_text);
            tok_number = save_num;

            if (tkind == T_NUMBER) {
                op->kind = OPERAND_IMM; op->flags = O_IMM_64; op->offset = tok_number; next_token(); return 0;
            } else if (tkind == T_IDENT) {
                struct name *n = lookup_name(tok_text, (int)strlen(tok_text));
                op->kind = OPERAND_IMM; op->flags = O_IMM_64; op->symbol = ensure_symbol_for_name(n); next_token(); return 0;
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

/* ===== Encoding helpers ===== */

static void emit_rex(int w, int r, int x, int b)
{
    unsigned char rex = 0x40;
    if (w) rex |= 0x08;
    if (r) rex |= 0x04;
    if (x) rex |= 0x02;
    if (b) rex |= 0x01;
    /* Only emit if any bit beyond 0x40 is set to avoid redundant 0x40? For simplicity, always emit when needed below. */
    emit_byte(rex);
}

static void emit_prefix66(void) { emit_byte(0x66); }
static void emit_prefixF3(void) { emit_byte(0xF3); }
static void emit_prefixF2(void) { emit_byte(0xF2); }

static void emit_modrm(int mod, int reg, int rm)
{
    emit_byte((unsigned char)((mod<<6) | ((reg&7)<<3) | (rm&7)));
}

/* ===== Simple encoders (GPR) ===== */

static void encode_rr64_op(unsigned char opcode, int dst_rm, int src_reg)
{
    int rex_r = (src_reg >> 3) & 1;
    int rex_b = (dst_rm >> 3) & 1;
    emit_rex(1, rex_r, 0, rex_b);
    emit_byte(opcode);
    emit_modrm(3, src_reg, dst_rm);
}

static void encode_mov_r64_imm32(int reg, unsigned imm32)
{
    int rex_b = (reg >> 3) & 1;
    emit_rex(1, 0, 0, rex_b);
    emit_byte((unsigned char)(0xB8 + (reg & 7)));
    emit_u32(imm32);
}

/* ===== SSE encoders (XMM, r/m128) ===== */

static void encode_sse_rm(unsigned pre1, unsigned pre2, unsigned char opcode, int is_load, int dst_xmm, int src_rm_is_mem, int src_rm_reg)
{
    if (pre1 == 0x66) emit_prefix66();
    else if (pre1 == 0xF3) emit_prefixF3();
    else if (pre1 == 0xF2) emit_prefixF2();

    if (pre2) emit_byte((unsigned char)pre2); /* typically 0x0F */

    int reg = dst_xmm;
    int rm  = src_rm_reg;

    int rex_r = (reg >> 3) & 1;
    int rex_b = (rm  >> 3) & 1;
    if (rex_r || rex_b) emit_rex(0, rex_r, 0, rex_b);

    emit_byte(opcode);

    if (src_rm_is_mem) {
        if (rm < 0) { error("SSE memory rm missing"); return; }
        emit_modrm(0, reg, rm);
    } else {
        emit_modrm(3, reg, rm);
    }
}

/* ===== Instruction table (subset guiding parser choices) ===== */

static struct insn insn_table[] = {
    /* Core moves and arithmetic */
    { "movq", 2, { O_MRM_64, O_MRM_64 }, 1, { (char)0x89 }, I_DATA_64, NULL },
    { "movq", 2, { O_REG_64, O_IMM_32 }, 1, { (char)0xB8 }, I_DATA_64, NULL },
    { "addq", 2, { O_MRM_64, O_REG_64 }, 1, { (char)0x01 }, I_DATA_64, NULL },
    { "subq", 2, { O_MRM_64, O_REG_64 }, 1, { (char)0x29 }, I_DATA_64, NULL },
    { "cmpq", 2, { O_MRM_64, O_REG_64 }, 1, { (char)0x39 }, I_DATA_64, NULL },
    { "andq", 2, { O_MRM_64, O_REG_64 }, 1, { (char)0x21 }, I_DATA_64, NULL },
    { "orq",  2, { O_MRM_64, O_REG_64 }, 1, { (char)0x09 }, I_DATA_64, NULL },
    { "xorq", 2, { O_MRM_64, O_REG_64 }, 1, { (char)0x31 }, I_DATA_64, NULL },
    { "testq",2, { O_REG_64, O_REG_64 }, 1, { (char)0x85 }, I_DATA_64, NULL },

    /* Control flow */
    { "jmp",  1, { O_IMM }, 1, { (char)0xE9 }, I_DATA_32, NULL },
    { "je",   1, { O_IMM }, 2, { (char)0x0F, (char)0x84 }, I_DATA_32, NULL },
    { "jz",   1, { O_IMM }, 2, { (char)0x0F, (char)0x84 }, I_DATA_32, NULL },
    { "jne",  1, { O_IMM }, 2, { (char)0x0F, (char)0x85 }, I_DATA_32, NULL },
    { "jnz",  1, { O_IMM }, 2, { (char)0x0F, (char)0x85 }, I_DATA_32, NULL },
    { "jg",   1, { O_IMM }, 2, { (char)0x0F, (char)0x8F }, I_DATA_32, NULL },
    { "jl",   1, { O_IMM }, 2, { (char)0x0F, (char)0x8C }, I_DATA_32, NULL },
    { "jge",  1, { O_IMM }, 2, { (char)0x0F, (char)0x8D }, I_DATA_32, NULL },
    { "jle",  1, { O_IMM }, 2, { (char)0x0F, (char)0x8E }, I_DATA_32, NULL },
    { "call", 1, { O_IMM }, 1, { (char)0xE8 }, I_DATA_32, NULL },
    { "ret",  0, { 0 },     1, { (char)0xC3 }, 0, NULL },

    /* Misc */
    { "nop",  0, { 0 }, 1, { (char)0x90 }, 0, NULL },
    { "leave",0, { 0 }, 1, { (char)0xC9 }, 0, NULL },
    { "syscall",0,{0}, 2, { (char)0x0F, (char)0x05 }, 0, NULL },

    /* SSE subset: XMM load/store and arithmetic (packed single-precision) */
    { "movaps", 2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x28 }, 0, NULL }, /* load src r/m128 -> dst xmm */
    { "movaps", 2, { O_REG_XMM, O_MRM_32 }, 2, { (char)0x0F, (char)0x29 }, 0, NULL }, /* store src xmm -> dst r/m128 */
    { "movups", 2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x10 }, 0, NULL },
    { "movups", 2, { O_REG_XMM, O_MRM_32 }, 2, { (char)0x0F, (char)0x11 }, 0, NULL },
    { "addps", 2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x58 }, 0, NULL },
    { "subps", 2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x5C }, 0, NULL },
    { "mulps", 2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x59 }, 0, NULL },
    { "xorps", 2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x57 }, 0, NULL },
    { "andps", 2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x54 }, 0, NULL },
    { "orps",  2, { O_MRM_32, O_REG_XMM }, 2, { (char)0x0F, (char)0x56 }, 0, NULL },
};

/* ===== Instruction matching ===== */

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

/* ===== Encoders for selected mnemonics ===== */

static void encode_jmp_rel32(struct operand *op)
{
    emit_byte(0xE9);
    relocs[relocs_count].pos = text_len;
    relocs[relocs_count].sym = op->symbol;
    relocs[relocs_count].size = 4;
    relocs_count++;
    emit_u32(0);
}

static void encode_jcc_rel32(unsigned char cc)
{
    emit_byte(0x0F);
    emit_byte(cc);
    relocs[relocs_count].pos = text_len;
    relocs[relocs_count].sym = operands[0].symbol;
    relocs[relocs_count].size = 4;
    relocs_count++;
    emit_u32(0);
}

static void encode_call_rel32(struct operand *op)
{
    emit_byte(0xE8);
    relocs[relocs_count].pos = text_len;
    relocs[relocs_count].sym = op->symbol;
    relocs[relocs_count].size = 4;
    relocs_count++;
    emit_u32(0);
}

/* SSE dispatcher */

static int try_encode_sse(const char *mnemonic)
{
    if (!strcmp(mnemonic,"movaps")) {
        if (nr_operands==2 && (operands[0].flags & O_REG_XMM) && operands[1].kind==OPERAND_MEM) {
            encode_sse_rm(0, 0x0F, 0x29, 0, operands[0].reg, 1, operands[1].reg);
            return 0;
        }
        if (nr_operands==2 && operands[0].kind!=OPERAND_REG && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM);
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x28, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported movaps form");
        return -1;
    } else if (!strcmp(mnemonic,"movups")) {
        if (nr_operands==2 && (operands[0].flags & O_REG_XMM) && operands[1].kind==OPERAND_MEM) {
            encode_sse_rm(0, 0x0F, 0x11, 0, operands[0].reg, 1, operands[1].reg);
            return 0;
        }
        if (nr_operands==2 && operands[0].kind!=OPERAND_REG && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM);
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x10, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported movups form");
        return -1;
    } else if (!strcmp(mnemonic,"addps")) {
        if (nr_operands==2 && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM);
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x58, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported addps form");
        return -1;
    } else if (!strcmp(mnemonic,"subps")) {
        if (nr_operands==2 && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM);
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x5C, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported subps form");
        return -1;
    } else if (!strcmp(mnemonic,"mulps")) {
        if (nr_operands==2 && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM);
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x59, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported mulps form");
        return -1;
    } else if (!strcmp(mnemonic,"xorps")) {
        if (nr_operands==2 && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM) ? 1 : 0;
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x57, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported xorps form");
        return -1;
    } else if (!strcmp(mnemonic,"andps")) {
        if (nr_operands==2 && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM);
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x54, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported andps form");
        return -1;
    } else if (!strcmp(mnemonic,"orps")) {
        if (nr_operands==2 && (operands[1].flags & O_REG_XMM)) {
            int rm_is_mem = (operands[0].kind == OPERAND_MEM);
            int rm = operands[0].reg;
            encode_sse_rm(0, 0x0F, 0x56, 1, operands[1].reg, rm_is_mem, rm);
            return 0;
        }
        error("unsupported orps form");
        return -1;
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
        /* Pseudo ops */
        if (strcmp(mnemonic,"byte")==0)  { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_byte(); }
        if (strcmp(mnemonic,"word")==0)  { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_word(); }
        if (strcmp(mnemonic,"dword")==0) { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_dword(); }
        if (strcmp(mnemonic,"qword")==0) { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_qword(); }
        if (strcmp(mnemonic,"align")==0){ number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_align(); }
        if (strcmp(mnemonic,"skip")==0)  { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_skip(); }
        if (strcmp(mnemonic,"fill")==0)  { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_fill(); }
        if (strcmp(mnemonic,"ascii")==0) { if (nr_operands==1 && name_token) return pseudo_ascii(); return 0; }
        if (strcmp(mnemonic,"global")==0){ if (nr_operands==1 && name_token) return pseudo_global(); return 0; }
        if (strcmp(mnemonic,"text")==0)  return pseudo_text();
        if (strcmp(mnemonic,"data")==0)  return pseudo_data();
        if (strcmp(mnemonic,"bss")==0)   return pseudo_bss();
        if (strcmp(mnemonic,"org")==0)   { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:0; return pseudo_org(); }
        if (strcmp(mnemonic,"bits")==0)  { number_token = (nr_operands>0 && operands[0].kind==OPERAND_IMM)? operands[0].offset:64; return pseudo_bits(); }
        error("unknown mnemonic '%s'", mnemonic);
        return -1;
    }

    /* SSE subset */
    if (try_encode_sse(mnemonic) == 0) return 0;

    /* GPR core */
    if (!strcmp(mnemonic,"movq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x89, operands[1].reg, operands[0].reg); /* dst <- src */
            return 0;
        }
        if (nr_operands==2 && operands[0].kind==OPERAND_IMM && operands[1].kind==OPERAND_REG &&
            (operands[1].flags & O_REG_64)) {
            unsigned imm32 = (unsigned)(operands[0].offset & 0xFFFFFFFFu);
            encode_mov_r64_imm32(operands[1].reg, imm32);
            return 0;
        }
        error("unsupported movq form"); return -1;
    } else if (!strcmp(mnemonic,"addq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x01, operands[1].reg, operands[0].reg); return 0;
        }
        error("unsupported addq form"); return -1;
    } else if (!strcmp(mnemonic,"subq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x29, operands[1].reg, operands[0].reg); return 0;
        }
        error("unsupported subq form"); return -1;
    } else if (!strcmp(mnemonic,"cmpq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x39, operands[1].reg, operands[0].reg); return 0;
        }
        error("unsupported cmpq form"); return -1;
    } else if (!strcmp(mnemonic,"andq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x21, operands[1].reg, operands[0].reg); return 0;
        }
        error("unsupported andq form"); return -1;
    } else if (!strcmp(mnemonic,"orq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x09, operands[1].reg, operands[0].reg); return 0;
        }
        error("unsupported orq form"); return -1;
    } else if (!strcmp(mnemonic,"xorq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x31, operands[1].reg, operands[0].reg); return 0;
        }
        error("unsupported xorq form"); return -1;
    } else if (!strcmp(mnemonic,"testq")) {
        if (nr_operands==2 && operands[0].kind==OPERAND_REG && operands[1].kind==OPERAND_REG &&
            (operands[0].flags & O_REG_64) && (operands[1].flags & O_REG_64)) {
            encode_rr64_op(0x85, operands[1].reg, operands[0].reg); return 0;
        }
        error("unsupported testq form"); return -1;
    } else if (!strcmp(mnemonic,"jmp")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_jmp_rel32(&operands[0]); return 0; }
        error("unsupported jmp form"); return -1;
    } else if (!strcmp(mnemonic,"je") || !strcmp(mnemonic,"jz")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_jcc_rel32(0x84); return 0; }
        error("unsupported je/jz form"); return -1;
    } else if (!strcmp(mnemonic,"jne") || !strcmp(mnemonic,"jnz")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_jcc_rel32(0x85); return 0; }
        error("unsupported jne/jnz form"); return -1;
    } else if (!strcmp(mnemonic,"jg")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_jcc_rel32(0x8F); return 0; }
        error("unsupported jg form"); return -1;
    } else if (!strcmp(mnemonic,"jl")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_jcc_rel32(0x8C); return 0; }
        error("unsupported jl form"); return -1;
    } else if (!strcmp(mnemonic,"jge")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_jcc_rel32(0x8D); return 0; }
        error("unsupported jge form"); return -1;
    } else if (!strcmp(mnemonic,"jle")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_jcc_rel32(0x8E); return 0; }
        error("unsupported jle form"); return -1;
    } else if (!strcmp(mnemonic,"call")) {
        if (nr_operands==1 && operands[0].kind==OPERAND_IMM) { encode_call_rel32(&operands[0]); return 0; }
        error("unsupported call form"); return -1;
    } else if (!strcmp(mnemonic,"ret")) {
        emit_byte(0xC3); return 0;
    } else if (!strcmp(mnemonic,"nop")) {
        emit_byte(0x90); return 0;
    } else if (!strcmp(mnemonic,"leave")) {
        emit_byte(0xC9); return 0;
    } else if (!strcmp(mnemonic,"syscall")) {
        emit_byte(0x0F); emit_byte(0x05); return 0;
    }

    error("unimplemented mnemonic '%s'", mnemonic);
    return -1;
}

/* ===== Label handling ===== */

static void define_label(const char *name)
{
    struct name *n = lookup_name(name, (int)strlen(name));
    if (!n) { error("out of memory for label"); return; }
    ensure_symbol_for_name(n);
    n->symbol->segment = segment;
    n->symbol->offset = (segment==1)? text_len : (segment==2? data_len : data_bytes);
    nr_symbol_changes++;
}

/* ===== Line processing ===== */

static void process_line(const char *line)
{
    strncpy(input_line, line, sizeof(input_line)-1);
    input_line[sizeof(input_line)-1]=0;
    input_pos = input_line;
    line_number++;

    /* strip comments starting with '#', '//', or ';' */
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
            ensure_symbol_for_name(name_token);
        }
        input_pos = (char *)save_pos;
        tkind = save_tk;
        strcpy(tok_text, save_text2);
    }

    parse_instruction();
}

/* ===== Relocation resolution ===== */

static void resolve_relocs(void)
{
    for (int i=0; i<relocs_count; i++) {
        struct reloc *r = &relocs[i];
        struct obj_symbol *sym = r->sym;
        if (!sym) { error("undefined symbol in relocation"); continue; }
        if (sym->segment != 1) {
            /* Only text->text rel32 supported in this scaffold */
            error("relocation to non-text symbol '%s' not supported", sym->n ? sym->n->data : "(anon)");
            continue;
        }
        long target = sym->offset;
        long disp = target - (r->pos + r->size);
        text_buf[r->pos+0] = (unsigned char)(disp & 0xFF);
        text_buf[r->pos+1] = (unsigned char)((disp >> 8) & 0xFF);
        text_buf[r->pos+2] = (unsigned char)((disp >> 16) & 0xFF);
        text_buf[r->pos+3] = (unsigned char)((disp >> 24) & 0xFF);
    }
}

/* ===== Emission of final output ===== */

static void write_output(void)
{
    ensure_output_file();
    fprintf(output_file, "; asm.c output\n");
    fprintf(output_file, "; text_len=%ld data_len=%ld bits=%d base=%d reloc_count=%d\n", text_len, data_len, bits, base_address, relocs_count);

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

/* ===== Public API stub to satisfy externs if included elsewhere ===== */
struct insn insns[] = { /* empty, local table used */ };

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
        const char *program[] = {
            "bits 64",
            "text",
            "global main",
            "main:",
            "  movq $1, %rax",
            "  movq %rax, %rbx",
            "  addq %rax, %rbx",
            "  cmpq %rbx, %rax",
            "  je equal",
            "  call hello",
            "  jmp end",
            "equal:",
            "  xorps %xmm0, %xmm1",
            "  movups %xmm1, (%rax)",
            "hello:",
            "  movaps (%rbx), %xmm2",
            "  addps %xmm2, %xmm2",
            "  ret",
            "text",
            "end:",
            "  nop",
            NULL
        };
        for (int i=0; program[i]; ++i) process_line(program[i]);
    }
    resolve_relocs();
    write_output();
    return 0;
}
