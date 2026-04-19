// ============================================================================
// emu.c - vollständige, erweiterte x64-emulator-amalgamation
// - enthält: cpu struct, memory helpers, erweiterte opcode-handler (ca. 1000 zeilen)
// - dispatcher erweitert um alu, jcc, mov und register-movs.
// ============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <emmintrin.h> /* sse2 intrinsics */

/* -------------------------
   configuration switches
   ------------------------- */
#define USE_INLINE_ASM_FASTMEM 1
#define USE_INLINE_ASM_FETCH 1
#define USE_INLINE_ASM_STACK 1
#define USE_FB_STREAMING 1

/* -------------------------
   basic sizes and globals
   ------------------------- */
#define RAM_SIZE (64ULL * 1024 * 1024) /* 64 mib default */

static uint8_t *ram_mem = NULL;
static uint8_t *fb_mem = NULL;

/* minimal xmm type */
typedef struct { uint64_t lo; uint64_t hi; } XMM;

/* minimal cpu type */
typedef struct {
    uint64_t gpr[16];    /* rax..r15 */
    uint64_t rflags;
    uint64_t rip;
    XMM xmm[16];
    int op_size; /* 16/32/64 */
    int addr_size; /* 32/64 */
    int has_rex;
    int rex_w, rex_r, rex_x, rex_b;
    int modrm_mod, modrm_reg, modrm_rm;
    uint8_t last_opcode;
} X64CPU;

static X64CPU cpu;

/* -------------------------
   forward declarations
   ------------------------- */
uint8_t *get_memory_ptr(uint64_t addr);
uint8_t mem_read8(uint64_t addr);
uint16_t mem_read16(uint64_t addr);
uint32_t mem_read32(uint64_t addr);
uint64_t mem_read64(uint64_t addr);
void mem_write8(uint64_t addr, uint8_t v);
void mem_write16(uint64_t addr, uint16_t v);
void mem_write32(uint64_t addr, uint32_t v);
void mem_write64(uint64_t addr, uint64_t v);
uint8_t fetch8(void);
uint32_t fetch32(void);
uint64_t fetch64(void);
void decode_modrm(void);
uint64_t compute_ea(void);
void cpu_decode_reset(void);
void execute_one_extended(void);

/* -------------------------
   tracing / helpers
   ------------------------- */
static void trace_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
static const char *gpr_name(int idx) {
    static const char *names[16] = {
        "rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi",
        "r8","r9","r10","r11","r12","r13","r14","r15"
    };
    if (idx >= 0 && idx < 16) return names[idx];
    return "r?";
}

/* -------------------------
   memory helpers
   ------------------------- */
uint8_t *get_memory_ptr(uint64_t addr) {
    if (!ram_mem) return NULL;
    /* only use lowest 64MB for physical memory access */
    return &ram_mem[addr % RAM_SIZE];
}
uint8_t mem_read8(uint64_t addr) {
    uint8_t *p = get_memory_ptr(addr);
    return p ? p[0] : 0;
}
uint16_t mem_read16(uint64_t addr) {
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return 0;
    uint16_t v; memcpy(&v, p, 2); return v;
}
uint32_t mem_read32(uint64_t addr) {
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return 0;
    uint32_t v; memcpy(&v, p, 4); return v;
}
uint64_t mem_read64(uint64_t addr) {
#if USE_INLINE_ASM_FASTMEM && defined(__x86_64__) && defined(__GNUC__)
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return 0;
    uint64_t val;
    asm volatile ("movq (%1), %0" : "=r"(val) : "r"(p) : "memory");
    return val;
#else
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return 0;
    uint64_t v; memcpy(&v, p, 8); return v;
#endif
}
void mem_write8(uint64_t addr, uint8_t v) {
    uint8_t *p = get_memory_ptr(addr); if (!p) return; p[0] = v;
}
void mem_write16(uint64_t addr, uint16_t v) {
    uint8_t *p = get_memory_ptr(addr); if (!p) return; memcpy(p, &v, 2);
}
void mem_write32(uint64_t addr, uint32_t v) {
    uint8_t *p = get_memory_ptr(addr); if (!p) return; memcpy(p, &v, 4);
}
void mem_write64(uint64_t addr, uint64_t v) {
#if USE_INLINE_ASM_FASTMEM && defined(__x86_64__) && defined(__GNUC__)
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return;
    asm volatile ("movq %1, (%0)" : : "r"(p), "r"(v) : "memory");
#else
    uint8_t *p = get_memory_ptr(addr); if (!p) return; memcpy(p, &v, 8);
#endif
}

/* -------------------------
   fetch helpers
   ------------------------- */
uint8_t fetch8(void) {
#if USE_INLINE_ASM_FETCH && defined(__x86_64__) && defined(__GNUC__)
    uint8_t b = 0;
    uint8_t *p = get_memory_ptr(cpu.rip);
    if (p) { asm volatile ("movb (%1), %0" : "=r"(b) : "r"(p) : "memory"); }
    cpu.rip += 1;
    return b;
#else
    uint8_t v = mem_read8(cpu.rip); cpu.rip += 1; return v;
#endif
}
uint16_t fetch16(void) {
    uint16_t v = mem_read16(cpu.rip); cpu.rip += 2; return v;
}
uint32_t fetch32(void) {
#if USE_INLINE_ASM_FETCH && defined(__x86_64__) && defined(__GNUC__)
    uint32_t v = 0;
    uint8_t *p = get_memory_ptr(cpu.rip);
    if (p) { asm volatile ("movl (%1), %0" : "=r"(v) : "r"(p) : "memory"); }
    cpu.rip += 4;
    return v;
#else
    uint32_t v = mem_read32(cpu.rip); cpu.rip += 4; return v;
#endif
}
uint64_t fetch64(void) {
#if USE_INLINE_ASM_FETCH && defined(__x86_64__) && defined(__GNUC__)
    uint64_t v = 0;
    uint8_t *p = get_memory_ptr(cpu.rip);
    if (p) { asm volatile ("movq (%1), %0" : "=r"(v) : "r"(p) : "memory"); }
    cpu.rip += 8;
    return v;
#else
    uint64_t v = mem_read64(cpu.rip); cpu.rip += 8; return v;
#endif
}

/* -------------------------
   simplified modrm / ea
   ------------------------- */
void decode_modrm(void) {
    uint8_t b = fetch8();
    cpu.modrm_mod = (b >> 6) & 3;
    cpu.modrm_reg = (b >> 3) & 7;
    cpu.modrm_rm  = b & 7;
}
uint64_t compute_ea(void) {
    /* simplified implementation for 64-bit mode */
    uint64_t addr = 0;
    int reg_rm = cpu.modrm_rm | (cpu.rex_b<<3);

    if (reg_rm == 4) { /* sib byte */
        /* sib not fully implemented - treat as base only for now */
        reg_rm = cpu.modrm_rm;
    }

    if (cpu.modrm_mod == 0) {
        if (reg_rm == 5) { /* [disp32] */
            int32_t disp = (int32_t)fetch32();
            addr = cpu.rip + (int64_t)disp; /* rip-relative addressing */
        } else { /* [reg] */
            addr = cpu.gpr[reg_rm];
        }
    } else if (cpu.modrm_mod == 1) { /* [reg+disp8] */
        int8_t disp8 = (int8_t)fetch8();
        addr = cpu.gpr[reg_rm] + (int64_t)disp8;
    } else if (cpu.modrm_mod == 2) { /* [reg+disp32] */
        int32_t disp32 = (int32_t)fetch32();
        addr = cpu.gpr[reg_rm] + (int64_t)disp32;
    }
    return addr;
}

void cpu_decode_reset(void) {
    cpu.op_size = 64; cpu.addr_size = 64;
    cpu.has_rex = 0;
    cpu.rex_w = cpu.rex_r = cpu.rex_x = cpu.rex_b = 0;
}

/* -------------------------
   flag helpers
   ------------------------- */
static inline int parity8(uint8_t x) {
    x ^= x >> 4; x &= 0xF; return (0x6996 >> x) & 1;
}

static inline void set_flag_zf_sf_pf8(uint64_t res, int width) {
    uint64_t mask = (width==64) ? ~0ULL : ((1ULL << width) - 1);
    uint64_t v = res & mask;
    if (v == 0) cpu.rflags |= (1ULL << 6); else cpu.rflags &= ~(1ULL << 6); /* zf */
    if (v & (1ULL << (width-1))) cpu.rflags |= (1ULL << 7); else cpu.rflags &= ~(1ULL << 7); /* sf */
    if (parity8((uint8_t)(v & 0xFF))) cpu.rflags |= (1ULL << 2); else cpu.rflags &= ~(1ULL << 2); /* pf */
}

static inline void update_add_flags_u64(uint64_t a, uint64_t b, uint64_t res, int width) {
    (void)width;
    /* cf */
    if (res < a) cpu.rflags |= 1ULL; else cpu.rflags &= ~1ULL;
    /* of */
    int sa = (a >> 63) & 1; int sb = (b >> 63) & 1; int sr = (res >> 63) & 1;
    if ((sa == sb) && (sr != sa)) cpu.rflags |= (1ULL << 11); else cpu.rflags &= ~(1ULL << 11);
}

static inline void update_sub_flags_u64(uint64_t a, uint64_t b, uint64_t res, int width) {
    (void)width;
    /* cf */
    if (a < b) cpu.rflags |= 1ULL; else cpu.rflags &= ~1ULL;
    /* of */
    int sa = (a >> 63) & 1; int sb = (b >> 63) & 1; int sr = (res >> 63) & 1;
    if ((sa != sb) && (sr != sa)) cpu.rflags |= (1ULL << 11); else cpu.rflags &= ~(1ULL << 11);
}

static inline void clear_alu_flags() {
    cpu.rflags &= ~((1ULL<<0) | (1ULL<<2) | (1ULL<<4) | (1ULL<<6) | (1ULL<<7) | (1ULL<<11));
}

/* -------------------------
   conditional check (3.5 rule)
   ------------------------- */
static inline int funGet(int numBit) {
    return (cpu.rflags >> numBit) & 1;
}

static inline int funCnd(int numCode) {
    /* 0x0: O (OF) | 0x1: NO | 0x2: B/NAE (CF) | 0x3: NB/AE (NCF) | 0x4: Z/E (ZF) | 0x5: NZ/NE (NZF)
       0x6: BE/NA (CF|ZF) | 0x7: NBE/A | 0x8: S (SF) | 0x9: NS | 0xA: P/PE (PF) | 0xB: NP/PO
       0xC: L/NGE (SF != OF) | 0xD: NL/GE | 0xE: LE/NG | 0xF: NLE/G */
    switch (numCode & 0xF) {
        case 0x0: return funGet(11); /* O */
        case 0x1: return !funGet(11); /* NO */
        case 0x2: return funGet(0); /* B/NAE (CF) */
        case 0x3: return !funGet(0); /* NB/AE (NCF) */
        case 0x4: return funGet(6); /* Z/E (ZF) */
        case 0x5: return !funGet(6); /* NZ/NE (NZF) */
        case 0x6: return funGet(0) || funGet(6); /* BE/NA (CF|ZF) */
        case 0x7: return !(funGet(0) || funGet(6)); /* NBE/A */
        case 0x8: return funGet(7); /* S (SF) */
        case 0x9: return !funGet(7); /* NS */
        case 0xA: return funGet(2); /* P/PE (PF) */
        case 0xB: return !funGet(2); /* NP/PO */
        case 0xC: return funGet(7) != funGet(11); /* L/NGE (SF != OF) */
        case 0xD: return funGet(7) == funGet(11); /* NL/GE */
        case 0xE: return (funGet(7) != funGet(11)) || funGet(6); /* LE/NG */
        case 0xF: return !((funGet(7) != funGet(11)) || funGet(6)); /* NLE/G */
    }
    return 0;
}

/* -------------------------
   stack helpers
   ------------------------- */
static inline void push_u64(uint64_t v) {
#if USE_INLINE_ASM_STACK && defined(__x86_64__) && defined(__GNUC__)
    uint64_t sp = cpu.gpr[4] - 8;
    cpu.gpr[4] = sp;
    uint8_t *p = get_memory_ptr(sp);
    if (p) asm volatile ("movq %1, (%0)" : : "r"(p), "r"(v) : "memory");
#else
    cpu.gpr[4] -= 8; mem_write64(cpu.gpr[4], v);
#endif
}
static inline uint64_t pop_u64(void) {
#if USE_INLINE_ASM_STACK && defined(__x86_64__) && defined(__GNUC__)
    uint64_t sp = cpu.gpr[4];
    uint8_t *p = get_memory_ptr(sp);
    uint64_t v = 0;
    if (p) asm volatile ("movq (%1), %0" : "=r"(v) : "r"(p) : "memory");
    cpu.gpr[4] = sp + 8;
    return v;
#else
    uint64_t v = mem_read64(cpu.gpr[4]); cpu.gpr[4] += 8; return v;
#endif
}

/* -------------------------
   ALU execution helper (3.5 rule)
   ------------------------- */
typedef enum { opAdd, opOr, opAdc, opSbb, opAnd, opSub, opXor, opCmp } AluOp;

static uint64_t funAluExec(AluOp oper, uint64_t lhs, uint64_t rhs) {
    uint64_t res = 0;
    clear_alu_flags();

    switch (oper) {
        case opAdd:
            res = lhs + rhs;
            update_add_flags_u64(lhs, rhs, res, 64);
            break;
        case opOr:
            res = lhs | rhs;
            break; /* only Z/S/P affected */
        case opAnd:
            res = lhs & rhs;
            break; /* only Z/S/P affected */
        case opXor:
            res = lhs ^ rhs;
            break; /* only Z/S/P affected */
        case opSub:
        case opCmp:
            res = lhs - rhs;
            update_sub_flags_u64(lhs, rhs, res, 64);
            break;
        default:
            trace_printf("unimplemented alu op: %d\n", oper);
            exit(1);
    }
    set_flag_zf_sf_pf8(res, 64);
    if (oper == opOr || oper == opAnd || oper == opXor) {
        cpu.rflags &= ~((1ULL<<0) | (1ULL<<11)); /* cf, of clear */
    }
    return res;
}

/* -------------------------
   0f group handler
   ------------------------- */
void handle_0F_group(uint8_t op2) {
    switch (op2) {
        case 0x80 ... 0x8F: { /* jcc rel32 */
            int32_t disp = (int32_t)fetch32();
            if (funCnd(op2)) {
                cpu.rip += (int64_t)disp;
            }
            break;
        }
        case 0x90 ... 0x9F: { /* setcc r/m8 */
            decode_modrm();
            uint8_t set_val = funCnd(op2) ? 1 : 0;
            if (cpu.modrm_mod == 3) {
                /* only al, cl, dl, bl, ah, ch, dh, bh */
                uint64_t *regp = &cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))];
                *regp = (*regp & ~0xFF) | set_val;
            } else {
                mem_write8(compute_ea(), set_val);
            }
            break;
        }
        default:
            trace_printf("Unimplemented 0F opcode: 0F %02X at rip=0x%016" PRIx64 "\n", op2, cpu.rip-2);
            exit(1);
    }
}

/* -------------------------
   extended execute
   ------------------------- */
void execute_one_extended(void) {
    cpu_decode_reset();
    int decoding = 1;
    uint8_t op = 0;
    while (decoding) {
        uint8_t b = fetch8();
        switch (b) {
            case 0x66: cpu.op_size = 16; continue;
            case 0x67: cpu.addr_size = 32; continue;
            default:
                if ((b & 0xF0) == 0x40) {
                    cpu.has_rex = 1;
                    cpu.rex_w = (b >> 3) & 1; cpu.rex_r = (b >> 2) & 1;
                    cpu.rex_x = (b >> 1) & 1; cpu.rex_b = b & 1;
                    /* REX.W forces 64-bit operation (op_size = 64) */
                    if (cpu.rex_w) cpu.op_size = 64;
                    continue;
                }
                op = b; decoding = 0;
                break;
        }
    }

    switch (op) {
        /* minimal placeholder - all logic moved to optimized loop */
        case 0x0F: handle_0F_group(fetch8()); break;
        default:
            trace_printf("Unimplemented extended (fallback): 0x%02X at rip=0x%016" PRIx64 "\n", op, cpu.rip-1);
            exit(1);
    }
}

/* -------------------------
   framebuffer / sdl
   ------------------------- */
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int width, height;
    int pitch;
} FBDisplay;
static FBDisplay fbdisp;

int fb_init_display(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return -1;
    fbdisp.width = width; fbdisp.height = height;
    fbdisp.window = SDL_CreateWindow("x64emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!fbdisp.window) return -1;
    fbdisp.renderer = SDL_CreateRenderer(fbdisp.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!fbdisp.renderer) return -1;
    fbdisp.texture = SDL_CreateTexture(fbdisp.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!fbdisp.texture) return -1;
    fbdisp.pitch = width * 4;
    if (!fb_mem) {
        fb_mem = malloc((size_t)fbdisp.pitch * height);
        if (!fb_mem) return -1;
        memset(fb_mem, 0, (size_t)fbdisp.pitch * height);
    }
    return 0;
}

void fb_present() {
    if (!fbdisp.texture || !fb_mem) return;
    SDL_UpdateTexture(fbdisp.texture, NULL, fb_mem, fbdisp.pitch);
    SDL_RenderClear(fbdisp.renderer);
    SDL_RenderCopy(fbdisp.renderer, fbdisp.texture, NULL, NULL);
    SDL_RenderPresent(fbdisp.renderer);
}
void fb_shutdown() {
    if (fb_mem) { free(fb_mem); fb_mem = NULL; }
    if (fbdisp.texture) SDL_DestroyTexture(fbdisp.texture);
    if (fbdisp.renderer) SDL_DestroyRenderer(fbdisp.renderer);
    if (fbdisp.window) SDL_DestroyWindow(fbdisp.window);
    SDL_Quit();
}

/* -------------------------
   optimized run loop (expanded)
   ------------------------- */
#if defined(__GNUC__) && defined(__x86_64__)
void run_loop_optimized(void) {
    static void *op_labels[256];
    static int table_init = 0;
    if (!table_init) {
        for (int i=0;i<256;i++) op_labels[i] = &&default_label;
        /* REX Prefix: 0x40 - 0x4F handled in loop preamble */

        /* ALU Group 1: ADD/OR/ADC/SBB/AND/SUB/XOR/CMP r/m64, r64 */
        op_labels[0x01] = &&op_alu_g1_r_m_r; /* ADD */
        op_labels[0x09] = &&op_alu_g1_r_m_r; /* OR */
        op_labels[0x11] = &&op_alu_g1_r_m_r; /* ADC - not implemented */
        op_labels[0x19] = &&op_alu_g1_r_m_r; /* SBB - not implemented */
        op_labels[0x21] = &&op_alu_g1_r_m_r; /* AND */
        op_labels[0x29] = &&op_alu_g1_r_m_r; /* SUB */
        op_labels[0x31] = &&op_alu_g1_r_m_r; /* XOR */
        op_labels[0x39] = &&op_alu_g1_r_m_r; /* CMP */

        /* ALU Group 1: ADD/OR/ADC/SBB/AND/SUB/XOR/CMP r64, r/m64 */
        op_labels[0x03] = &&op_alu_g1_r_r_m; /* ADD */
        op_labels[0x0B] = &&op_alu_g1_r_r_m; /* OR */
        op_labels[0x13] = &&op_alu_g1_r_r_m; /* ADC - not implemented */
        op_labels[0x1B] = &&op_alu_g1_r_r_m; /* SBB - not implemented */
        op_labels[0x23] = &&op_alu_g1_r_r_m; /* AND */
        op_labels[0x2B] = &&op_alu_g1_r_r_m; /* SUB */
        op_labels[0x33] = &&op_alu_g1_r_r_m; /* XOR */
        op_labels[0x3B] = &&op_alu_g1_r_r_m; /* CMP */

        /* Immediate ALU (Group 1) */
        op_labels[0x81] = &&op_alu_imm_g1; /* imm32 */
        op_labels[0x83] = &&op_alu_imm_g1; /* imm8 sign extended */

        /* MOV */
        op_labels[0x89] = &&op_mov_rm64_r64; /* MOV r/m64, r64 */
        op_labels[0x8B] = &&op_mov_r64_rm64; /* MOV r64, r/m64 */
        op_labels[0xB8] = &&op_mov_imm_reg; /* MOV r64, imm64 (0xB8 - 0xBF) */
        op_labels[0xB9] = &&op_mov_imm_reg;
        op_labels[0xBA] = &&op_mov_imm_reg;
        op_labels[0xBB] = &&op_mov_imm_reg;
        op_labels[0xBC] = &&op_mov_imm_reg;
        op_labels[0xBD] = &&op_mov_imm_reg;
        op_labels[0xBE] = &&op_mov_imm_reg;
        op_labels[0xBF] = &&op_mov_imm_reg;
        op_labels[0xC7] = &&op_mov_rm64_imm32; /* MOV r/m64, imm32 */

        /* PUSH/POP */
        op_labels[0x50] = &&op_push_reg; /* PUSH r64 (0x50 - 0x57) */
        op_labels[0x51] = &&op_push_reg;
        op_labels[0x52] = &&op_push_reg;
        op_labels[0x53] = &&op_push_reg;
        op_labels[0x54] = &&op_push_reg;
        op_labels[0x55] = &&op_push_reg;
        op_labels[0x56] = &&op_push_reg;
        op_labels[0x57] = &&op_push_reg;
        op_labels[0x58] = &&op_pop_reg; /* POP r64 (0x58 - 0x5F) */
        op_labels[0x59] = &&op_pop_reg;
        op_labels[0x5A] = &&op_pop_reg;
        op_labels[0x5B] = &&op_pop_reg;
        op_labels[0x5C] = &&op_pop_reg;
        op_labels[0x5D] = &&op_pop_reg;
        op_labels[0x5E] = &&op_pop_reg;
        op_labels[0x5F] = &&op_pop_reg;
        op_labels[0x6A] = &&op_push_imm8; /* PUSH imm8 */
        op_labels[0x68] = &&op_push_imm32; /* PUSH imm32 */
        op_labels[0x8F] = &&op_pop_rm64; /* POP r/m64 (Group 1) */

        /* Jumps/Calls/Returns */
        op_labels[0x70] = &&op_jcc_short; /* JO */
        op_labels[0x71] = &&op_jcc_short; /* JNO */
        op_labels[0x72] = &&op_jcc_short; /* JB */
        op_labels[0x73] = &&op_jcc_short; /* JNB */
        op_labels[0x74] = &&op_jcc_short; /* JZ */
        op_labels[0x75] = &&op_jcc_short; /* JNZ */
        op_labels[0x76] = &&op_jcc_short; /* JBE */
        op_labels[0x77] = &&op_jcc_short; /* JNBE */
        op_labels[0x78] = &&op_jcc_short; /* JS */
        op_labels[0x79] = &&op_jcc_short; /* JNS */
        op_labels[0x7A] = &&op_jcc_short; /* JP */
        op_labels[0x7B] = &&op_jcc_short; /* JNP */
        op_labels[0x7C] = &&op_jcc_short; /* JL */
        op_labels[0x7D] = &&op_jcc_short; /* JNL */
        op_labels[0x7E] = &&op_jcc_short; /* JLE */
        op_labels[0x7F] = &&op_jcc_short; /* JNLE */
        op_labels[0xE9] = &&op_jmp_rel; /* JMP rel32 */
        op_labels[0xEB] = &&op_jmp_short; /* JMP rel8 */
        op_labels[0xE8] = &&op_call_rel; /* CALL rel32 */
        op_labels[0xC3] = &&op_ret; /* RET */

        /* Group FF (INC/JMP/CALL/PUSH) */
        op_labels[0xFF] = &&op_ff_group;
        op_labels[0x8D] = &&op_lea_r64_m; /* LEA */
        op_labels[0x0F] = &&op_0F; /* 2-byte opcodes */

        table_init = 1;
    }

    for (;;) {
        cpu_decode_reset();
        int decoding = 1;
        uint8_t op = 0;

        /* prefix decoding */
        while (decoding) {
            uint8_t b = fetch8();
            switch (b) {
                case 0x66: cpu.op_size = 16; continue;
                case 0x67: cpu.addr_size = 32; continue;
                default:
                    if ((b & 0xF0) == 0x40) {
                        cpu.has_rex = 1;
                        cpu.rex_w = (b >> 3) & 1; cpu.rex_r = (b >> 2) & 1;
                        cpu.rex_x = (b >> 1) & 1; cpu.rex_b = b & 1;
                        if (cpu.rex_w) cpu.op_size = 64; /* rex.w forces 64-bit */
                        continue;
                    }
                    op = b; decoding = 0;
                    break;
            }
        }
        cpu.last_opcode = op;
        goto *op_labels[op];

    /* -------------------------------------
       GROUP 1 ALU Opcodes (r/m64, r64)
       0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x39
       ------------------------------------- */
    op_alu_g1_r_m_r: {
        decode_modrm();
        AluOp oper = (cpu.last_opcode >> 3) & 7;
        uint64_t src = cpu.gpr[(cpu.modrm_reg | (cpu.rex_r<<3))];

        if (cpu.modrm_mod == 3) { /* r/m is register */
            uint64_t *dstp = &cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))];
            uint64_t res = funAluExec(oper, *dstp, src);
            if (oper != opCmp) *dstp = res;
        } else { /* r/m is memory */
            uint64_t addr = compute_ea();
            uint64_t val = mem_read64(addr);
            uint64_t res = funAluExec(oper, val, src);
            if (oper != opCmp) mem_write64(addr, res);
        }
        goto dispatch_continue;
    }

    /* -------------------------------------
       GROUP 1 ALU Opcodes (r64, r/m64)
       0x03, 0x0B, 0x13, 0x1B, 0x23, 0x2B, 0x33, 0x3B
       ------------------------------------- */
    op_alu_g1_r_r_m: {
        decode_modrm();
        AluOp oper = (cpu.last_opcode >> 3) & 7;
        uint64_t *dstp = &cpu.gpr[(cpu.modrm_reg | (cpu.rex_r<<3))];
        uint64_t src = (cpu.modrm_mod == 3) ? cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] : mem_read64(compute_ea());

        uint64_t res = funAluExec(oper, *dstp, src);
        if (oper != opCmp) *dstp = res;
        goto dispatch_continue;
    }

    /* -------------------------------------
       GROUP 1 Immediate ALU (r/m64, imm32/imm8)
       0x81 (imm32) / 0x83 (imm8 sign extended)
       ------------------------------------- */
    op_alu_imm_g1: {
        decode_modrm();
        AluOp oper = cpu.modrm_reg;
        int imm_size = (cpu.last_opcode == 0x81) ? 32 : 8;
        uint64_t imm = (imm_size == 32) ? (uint64_t)(int64_t)(int32_t)fetch32() : (uint64_t)(int64_t)(int8_t)fetch8();

        uint64_t old_val;
        uint64_t *reg_dstp = NULL;
        uint64_t addr = 0;

        if (cpu.modrm_mod == 3) { /* r/m is register */
            reg_dstp = &cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))];
            old_val = *reg_dstp;
        } else { /* r/m is memory */
            addr = compute_ea();
            old_val = mem_read64(addr);
        }

        uint64_t res = funAluExec(oper, old_val, imm);

        if (oper != opCmp) {
            if (cpu.modrm_mod == 3) {
                *reg_dstp = res;
            } else {
                mem_write64(addr, res);
            }
        }
        goto dispatch_continue;
    }

    /* -------------------------------------
       MOV Opcodes
       ------------------------------------- */
    op_mov_rm64_r64: { /* 0x89: MOV r/m64, r64 */
        decode_modrm();
        uint64_t src = cpu.gpr[(cpu.modrm_reg | (cpu.rex_r<<3))];
        if (cpu.modrm_mod == 3) cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] = src;
        else mem_write64(compute_ea(), src);
        goto dispatch_continue;
    }
    op_mov_r64_rm64: { /* 0x8B: MOV r64, r/m64 */
        decode_modrm();
        uint64_t src = (cpu.modrm_mod == 3) ? cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] : mem_read64(compute_ea());
        cpu.gpr[(cpu.modrm_reg | (cpu.rex_r<<3))] = src;
        goto dispatch_continue;
    }
    op_mov_imm_reg: { /* 0xB8 - 0xBF: MOV r64, imm64 */
        int reg_idx = cpu.last_opcode & 0x7;
        if (cpu.has_rex) reg_idx |= (cpu.rex_b << 3);

        /* check for REX.W prefix for 64-bit immediate */
        uint64_t imm;
        if (cpu.rex_w) {
            imm = fetch64(); /* imm64 */
        } else {
            imm = (uint64_t)(int64_t)(int32_t)fetch32(); /* imm32 zero/sign extended to 64 */
        }
        cpu.gpr[reg_idx] = imm;
        goto dispatch_continue;
    }
    op_mov_rm64_imm32: { /* 0xC7: MOV r/m64, imm32 */
        decode_modrm();
        if (cpu.modrm_reg != 0) goto default_label; /* 0xC7 is for Group 1 opcodes (MOV) */

        uint32_t imm32 = fetch32();
        uint64_t val = (uint64_t)(int64_t)(int32_t)imm32; /* sign extend (x64 default) */

        if (cpu.modrm_mod == 3) cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] = val;
        else mem_write64(compute_ea(), val);
        goto dispatch_continue;
    }

    /* -------------------------------------
       JUMP/CALL/RET Opcodes
       ------------------------------------- */
    op_jcc_short: { /* 0x70 - 0x7F: Jcc rel8 */
        int8_t disp = (int8_t)fetch8();
        if (funCnd(cpu.last_opcode)) {
            cpu.rip += (int64_t)disp;
        }
        goto dispatch_continue;
    }
    op_jmp_short: { /* 0xEB: JMP rel8 */
        cpu.rip += (int64_t)(int8_t)fetch8();
        goto dispatch_continue;
    }
    op_jmp_rel: { /* 0xE9: JMP rel32 */
        cpu.rip += (int64_t)(int32_t)fetch32();
        goto dispatch_continue;
    }
    op_call_rel: { /* 0xE8: CALL rel32 */
        int32_t s = (int32_t)fetch32();
        push_u64(cpu.rip); cpu.rip += s;
        goto dispatch_continue;
    }
    op_ret: { /* 0xC3: RET */
        cpu.rip = pop_u64();
        goto dispatch_continue;
    }

    /* -------------------------------------
       PUSH/POP Opcodes
       ------------------------------------- */
    op_push_reg: { /* 0x50 - 0x57: PUSH r64 */
        int reg_idx = cpu.last_opcode & 0x7;
        if (cpu.has_rex) reg_idx |= (cpu.rex_b << 3);
        push_u64(cpu.gpr[reg_idx]);
        goto dispatch_continue;
    }
    op_pop_reg: { /* 0x58 - 0x5F: POP r64 */
        int reg_idx = cpu.last_opcode & 0x7;
        if (cpu.has_rex) reg_idx |= (cpu.rex_b << 3);
        cpu.gpr[reg_idx] = pop_u64();
        goto dispatch_continue;
    }
    op_push_imm8: { /* 0x6A: PUSH imm8 */
        push_u64((uint64_t)(int64_t)(int8_t)fetch8());
        goto dispatch_continue;
    }
    op_push_imm32: { /* 0x68: PUSH imm32 */
        push_u64((uint64_t)(int64_t)(int32_t)fetch32());
        goto dispatch_continue;
    }
    op_pop_rm64: { /* 0x8F: POP r/m64 */
        decode_modrm();
        if (cpu.modrm_reg != 0) goto default_label; /* 0x8F is for Group 1 opcodes (POP) */
        uint64_t val = pop_u64();
        if (cpu.modrm_mod == 3) cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] = val;
        else mem_write64(compute_ea(), val);
        goto dispatch_continue;
    }

    /* -------------------------------------
       Misc Opcodes
       ------------------------------------- */
    op_ff_group: { /* 0xFF: Group 5/3 */
        decode_modrm();
        int subop = cpu.modrm_reg;
        if (subop == 0) { /* INC r/m64 */
            uint64_t *valp;
            if (cpu.modrm_mod == 3) {
                valp = &cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))];
            } else {
                /* only registers implemented for simplicity in this group */
                goto default_label;
            }
            /* INC only affects OF/SF/ZF/AF/PF (CF is preserved) */
            uint64_t old_cf = cpu.rflags & 1;
            uint64_t old = *valp;
            uint64_t res = old + 1;
            *valp = res;
            set_flag_zf_sf_pf8(res, 64);
            update_add_flags_u64(old, 1, res, 64);
            cpu.rflags = (cpu.rflags & ~1ULL) | old_cf; /* restore CF */
        } else if (subop == 1) { /* DEC r/m64 */
            uint64_t *valp;
            if (cpu.modrm_mod == 3) {
                valp = &cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))];
            } else {
                goto default_label;
            }
            uint64_t old_cf = cpu.rflags & 1;
            uint64_t old = *valp;
            uint64_t res = old - 1;
            *valp = res;
            set_flag_zf_sf_pf8(res, 64);
            update_sub_flags_u64(old, 1, res, 64);
            cpu.rflags = (cpu.rflags & ~1ULL) | old_cf; /* restore CF */
        } else if (subop == 2) { /* CALL r/m64 */
            uint64_t dest = (cpu.modrm_mod == 3) ? cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] : mem_read64(compute_ea());
            push_u64(cpu.rip); cpu.rip = dest;
        } else if (subop == 4) { /* JMP r/m64 */
            cpu.rip = (cpu.modrm_mod == 3) ? cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] : mem_read64(compute_ea());
        } else if (subop == 6) { /* PUSH r/m64 */
            uint64_t val = (cpu.modrm_mod == 3) ? cpu.gpr[(cpu.modrm_rm | (cpu.rex_b<<3))] : mem_read64(compute_ea());
            push_u64(val);
        } else {
            goto default_label;
        }
        goto dispatch_continue;
    }
    op_lea_r64_m: { /* 0x8D: LEA r64, m */
        decode_modrm();
        /* LEA is special: it computes the effective address *without* dereferencing memory. */
        /* Since compute_ea handles the address calculation, we just call it. */
        uint64_t val = compute_ea();
        cpu.gpr[(cpu.modrm_reg | (cpu.rex_r<<3))] = val;
        goto dispatch_continue;
    }
    op_0F: { /* 2-byte opcodes */
        handle_0F_group(fetch8());
        goto dispatch_continue;
    }
    default_label:
        trace_printf("unimplemented opcode: 0x%02X at rip=0x%016" PRIx64 "\n", (unsigned)cpu.last_opcode, cpu.rip-1);
        exit(1);

    dispatch_continue:;
    }
}
#else
void run_loop_optimized(void) { for (;;) execute_one_extended(); }
#endif

/* -------------------------
   init helper
   ------------------------- */
int x64emu_init_standalone(void) {
    if (!ram_mem) {
        ram_mem = malloc(RAM_SIZE);
        if (!ram_mem) return -1;
        memset(ram_mem, 0, RAM_SIZE);
    }
    cpu.gpr[4] = RAM_SIZE - 0x1000; /* rsp */
    cpu.rip = 0;
    cpu.rflags = 0;
    return 0;
}

/* -------------------------
   bios loader
   ------------------------- */
int funLoad(const char *strPath) {
    FILE *ptrFile = fopen(strPath, "rb");
    if (!ptrFile) {
        fprintf(stderr, "err: cannot open file: %s\n", strPath);
        return -1;
    }

    fseek(ptrFile, 0, SEEK_END);
    long cntSize = ftell(ptrFile);
    fseek(ptrFile, 0, SEEK_SET);

    if (cntSize > RAM_SIZE) {
        fprintf(stderr, "err: file too big for ram\n");
        fclose(ptrFile);
        return -1;
    }

    /* load at address 0x0000 */
    size_t cntRead = fread(ram_mem, 1, cntSize, ptrFile);
    fclose(ptrFile);

    printf("sys: loaded %ld bytes from %s\n", cntRead, strPath);
    return 0;
}

/* -------------------------
   main entry point
   ------------------------- */
int main(int cntArg, char **arrArg) {
    if (cntArg < 2) {
        fprintf(stderr, "usage: %s <bios_file>\n", arrArg[0]);
        return 1;
    }

    if (x64emu_init_standalone() != 0) {
        fprintf(stderr, "err: init failed\n");
        return 1;
    }

    if (fb_init_display(640, 480) != 0) {
        fprintf(stderr, "err: fb init failed\n");
        return 1;
    }

    /* load bios */
    if (funLoad(arrArg[1]) != 0) {
        return 1;
    }

    /* start emulation */
    cpu.rip = 0;
    printf("sys: starting emulation...\n");

    run_loop_optimized();

    fb_shutdown();
    return 0;
}
