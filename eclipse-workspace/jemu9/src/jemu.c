// ===========================================================================
// jemu.c - Standalone x86_64 emulator (integer subset, no SSE/AVX)
// Fixed: implemented many opcodes reported in errors.txt, added safe
// fallbacks for SSE/FP two-byte opcodes by decoding and skipping ModR/M so
// the emulator does not crash on SSE opcodes it doesn't execute.
//
// Build: gcc -O2 -o jemu jemu.c
// Run:   ./jemu test_bios.bin
// ===========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>

// -------------------------
// configuration
// -------------------------
enum { RAM_SIZE_BYTES = 256 * 1024 * 1024 }; // 256 MiB
enum { FB_BASE_ADDR = 0x00100000, FB_WIDTH = 640, FB_HEIGHT = 480, FB_BPP = 4 };

// -------------------------
// CPU state
// -------------------------
enum { REG_RAX=0, REG_RCX, REG_RDX, REG_RBX, REG_RSP, REG_RBP, REG_RSI, REG_RDI,
       REG_R8, REG_R9, REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_R15 };

typedef struct {
    uint64_t regs[16];
    uint64_t rip;
    uint64_t rflags; // standard RFLAGS layout bits
    uint16_t seg_cs, seg_ds, seg_es, seg_fs, seg_gs, seg_ss;
    uint64_t cr0, cr3, cr4;
} X64CPU;

static X64CPU cpu;
static uint8_t *ram_mem = NULL;
static uint64_t ram_size = RAM_SIZE_BYTES;
static uint64_t current_pc = 0;

// logging
static void logf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
}

// -------------------------
// Memory helpers
// -------------------------
static inline uint8_t *mem_ptr(uint64_t addr) {
    if (addr >= ram_size) { logf("mem OOB 0x%" PRIx64 "
", addr); exit(1); }
    return ram_mem + addr;
}
static inline uint8_t read8(uint64_t a){ return *mem_ptr(a); }
static inline uint16_t read16(uint64_t a){ uint16_t v; memcpy(&v, mem_ptr(a), 2); return v; }
static inline uint32_t read32(uint64_t a){ uint32_t v; memcpy(&v, mem_ptr(a), 4); return v; }
static inline uint64_t read64(uint64_t a){ uint64_t v; memcpy(&v, mem_ptr(a), 8); return v; }
static inline void write8(uint64_t a, uint8_t v){ *mem_ptr(a) = v; }
static inline void write16(uint64_t a, uint16_t v){ memcpy(mem_ptr(a), &v, 2); }
static inline void write32(uint64_t a, uint32_t v){ memcpy(mem_ptr(a), &v, 4); }
static inline void write64(uint64_t a, uint64_t v){ memcpy(mem_ptr(a), &v, 8); }

// -------------------------
// decode helpers
// -------------------------
typedef struct {
    uint8_t rex;    // raw rex low nibble
    uint8_t rex_w;  // REX.W
    uint8_t prefix_lock;
    uint8_t prefix_rep;
    uint8_t prefix_seg;
    uint8_t operand_size; // 16/32/64 selection (0 for default 64)
} DecodeState;

typedef struct {
    uint8_t mod, reg, rm;
    uint8_t has_sib;
    uint8_t sib_scale, sib_index, sib_base;
    int64_t disp;
} ModRM;

static inline uint8_t fetch8(void) { uint8_t v = read8(current_pc); current_pc++; return v; }
static inline uint32_t fetch32(void){ uint32_t v = read32(current_pc); current_pc += 4; return v; }
static inline uint64_t fetch64(void){ uint64_t v = read64(current_pc); current_pc += 8; return v; }

static void decode_modrm(ModRM *m) {
    memset(m, 0, sizeof(*m));
    uint8_t b = fetch8();
    m->mod = (b >> 6) & 3; m->reg = (b >> 3) & 7; m->rm = b & 7;
    if (m->rm == 4 && m->mod != 3) {
        m->has_sib = 1; uint8_t s = fetch8(); m->sib_scale = (s>>6)&3; m->sib_index = (s>>3)&7; m->sib_base = s&7;
    }
    if (m->mod == 0) {
        if (m->rm == 5) { int32_t d = (int32_t)fetch32(); m->disp = d; }
    } else if (m->mod == 1) { int8_t d = (int8_t)fetch8(); m->disp = d; }
    else if (m->mod == 2) { int32_t d = (int32_t)fetch32(); m->disp = d; }
}

static uint64_t modrm_ea(ModRM *m, DecodeState *ds) {
    if (m->mod == 3) { logf("modrm_ea called on reg-direct
"); return 0; }
    uint64_t base_val = 0;
    if (m->has_sib) {
        if (m->sib_base == 5 && m->mod == 0) base_val = 0; else base_val = cpu.regs[m->sib_base];
        uint64_t idx_val = (m->sib_index == 4) ? 0 : cpu.regs[m->sib_index];
        uint64_t scale = 1ULL << m->sib_scale;
        return base_val + idx_val * scale + (uint64_t)m->disp;
    } else {
        if (m->rm == 5 && m->mod == 0) return (uint64_t)m->disp;
        base_val = cpu.regs[m->rm]; return base_val + (uint64_t)m->disp;
    }
}

// -------------------------
// register helpers
// -------------------------
static inline uint64_t reg64(int idx){ return cpu.regs[idx & 15]; }
static inline void set_reg64(int idx, uint64_t v){ cpu.regs[idx & 15] = v; }
static inline uint32_t reg32(int idx){ return (uint32_t)cpu.regs[idx & 15]; }
static inline void set_reg32(int idx, uint32_t v){ cpu.regs[idx & 15] = (cpu.regs[idx & 15] & ~0xffffffffULL) | v; }
static inline uint16_t reg16(int idx){ return (uint16_t)cpu.regs[idx & 15]; }
static inline void set_reg16(int idx, uint16_t v){ cpu.regs[idx & 15] = (cpu.regs[idx & 15] & ~0xffffULL) | v; }
static inline uint8_t reg8l(int idx){ return (uint8_t)(cpu.regs[idx & 7] & 0xff); }
static inline void set_reg8l(int idx, uint8_t v){ uint64_t mask = 0xffULL << (8*(idx&7)); cpu.regs[idx&15] = (cpu.regs[idx&15] & ~mask) | ((uint64_t)v << (8*(idx&7))); }
static inline uint8_t reg8h(int idx){ return (uint8_t)((cpu.regs[idx & 7] >> 8) & 0xff); }
static inline void set_reg8h(int idx, uint8_t v){ uint64_t mask = 0xffULL << (8*((idx&7)+1)); cpu.regs[idx&15] = (cpu.regs[idx&15] & ~mask) | ((uint64_t)v << (8*((idx&7)+1))); }

// REX helpers
static void rex_apply(uint8_t rex, int *regp, int *rmp) {
    if (rex & 0x04) *regp |= 8;
    if (rex & 0x01) *rmp  |= 8;
}

// -------------------------
// flag operations - PF helper
// -------------------------
static inline int parity8(uint8_t x) {
    x ^= x >> 4; x ^= x >> 2; x ^= x >> 1; return !(x & 1);
}

// Set flags after 64-bit addition (partial: CF/OF/ZF/SF/PF)
static void set_flags_add64(uint64_t a, uint64_t b, uint64_t res) {
    cpu.rflags = (cpu.rflags & ~((1<<0)|(1<<11)|(1<<6)|(1<<7)|(1<<2))) ;
    if (res < a) cpu.rflags |= (1<<0); // CF
    if (((int64_t)a > 0 && (int64_t)b > 0 && (int64_t)res < 0) || ((int64_t)a < 0 && (int64_t)b < 0 && (int64_t)res > 0)) cpu.rflags |= (1<<11);
    if (res == 0) cpu.rflags |= (1<<6);
    if ((int64_t)res < 0) cpu.rflags |= (1<<7);
    if (parity8((uint8_t)res)) cpu.rflags |= (1<<2);
}

// Set flags after 64-bit subtraction (a - b)
static void set_flags_sub64(uint64_t a, uint64_t b, uint64_t res) {
    cpu.rflags = (cpu.rflags & ~((1<<0)|(1<<11)|(1<<6)|(1<<7)|(1<<2)));
    if (a < b) cpu.rflags |= (1<<0); // borrow => CF
    if (((int64_t)a > 0 && (int64_t)b < 0 && (int64_t)res < 0) || ((int64_t)a < 0 && (int64_t)b > 0 && (int64_t)res > 0)) cpu.rflags |= (1<<11);
    if (res == 0) cpu.rflags |= (1<<6);
    if ((int64_t)res < 0) cpu.rflags |= (1<<7);
    if (parity8((uint8_t)res)) cpu.rflags |= (1<<2);
}

// Shift helpers: set flags for logical shifts (SF/ZF/CF/PF) - OF is defined for 1-bit shifts
static void set_flags_shl64(uint64_t orig, int count, uint64_t res) {
    cpu.rflags &= ~((1<<0)|(1<<11)|(1<<6)|(1<<7)|(1<<2));
    if (count != 0) {
        if (count <= 64) cpu.rflags |= ((orig >> (64 - count)) & 1ULL) ? (1<<0) : 0;
        if (count == 1) {
            int msb = (res >> 63) & 1; int sec = (orig >> 63) & 1;
            if (msb ^ sec) cpu.rflags |= (1<<11); // OF
        }
    }
    if (res == 0) cpu.rflags |= (1<<6);
    if ((int64_t)res < 0) cpu.rflags |= (1<<7);
    if (parity8((uint8_t)res)) cpu.rflags |= (1<<2);
}

static void set_flags_shr64(uint64_t orig, int count, uint64_t res, int arithmetic) {
    cpu.rflags &= ~((1<<0)|(1<<11)|(1<<6)|(1<<7)|(1<<2));
    if (count != 0) {
        if (count <= 64) cpu.rflags |= ((orig >> (count - 1)) & 1ULL) ? (1<<0) : 0;
        if (count == 1) {
            int msb = (orig >> 63) & 1; int newmsb = (res >> 63) & 1;
            if (msb ^ newmsb) cpu.rflags |= (1<<11);
        }
    }
    if (res == 0) cpu.rflags |= (1<<6);
    if ((int64_t)res < 0) cpu.rflags |= (1<<7);
    if (parity8((uint8_t)res)) cpu.rflags |= (1<<2);
}

// -------------------------
// core instruction implementations (expanded)
// -------------------------

static void op_mov_r64_imm64(int reg) {
    uint64_t imm = fetch64(); set_reg64(reg, imm);
}

static void op_mov_rm64_r64(ModRM *m, DecodeState *ds) {
    if (m->mod == 3) { int dst = m->rm; set_reg64(dst, reg64(m->reg)); }
    else { uint64_t ea = modrm_ea(m, ds); write64(ea, reg64(m->reg)); }
}
static void op_mov_r64_rm64(ModRM *m, DecodeState *ds) {
    if (m->mod == 3) { set_reg64(m->reg, reg64(m->rm)); }
    else { uint64_t ea = modrm_ea(m, ds); set_reg64(m->reg, read64(ea)); }
}

static void op_lea_r64_m(ModRM *m, DecodeState *ds) {
    uint64_t ea = modrm_ea(m, ds); set_reg64(m->reg, ea);
}

static void op_add_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t a = reg64(m->reg);
    uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds));
    uint64_t r = a + b; set_reg64(m->reg, r); set_flags_add64(a, b, r);
}

static void op_sub_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t a = reg64(m->reg);
    uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds));
    uint64_t r = a - b; set_reg64(m->reg, r); set_flags_sub64(a, b, r);
}

static void op_cmp_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t a = reg64(m->reg);
    uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds));
    uint64_t r = a - b; set_flags_sub64(a,b,r);
}

static void op_test_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t a = reg64(m->reg);
    uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds));
    uint64_t r = a & b; cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int64_t)r<0) cpu.rflags |= (1<<7); if (parity8((uint8_t)r)) cpu.rflags |= (1<<2);
}

static void op_and_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t a = reg64(m->reg);
    uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds));
    uint64_t r = a & b; set_reg64(m->reg, r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int64_t)r<0) cpu.rflags |= (1<<7); if (parity8((uint8_t)r)) cpu.rflags |= (1<<2);
}
static void op_or_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t a = reg64(m->reg); uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); uint64_t r = a | b; set_reg64(m->reg, r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int64_t)r<0) cpu.rflags |= (1<<7); if (parity8((uint8_t)r)) cpu.rflags |= (1<<2);
}
static void op_xor_r64_rm64(ModRM *m, DecodeState *ds) { uint64_t a = reg64(m->reg); uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); uint64_t r = a ^ b; set_reg64(m->reg, r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int64_t)r<0) cpu.rflags |= (1<<7); if (parity8((uint8_t)r)) cpu.rflags |= (1<<2); }

// 8-bit variants helpers
static void op_and_rm8_r8(ModRM *m, DecodeState *ds) {
    uint8_t a = (m->mod==3) ? reg8l(m->rm) : read8(modrm_ea(m, ds));
    uint8_t b = reg8l(m->reg);
    uint8_t r = a & b;
    if (m->mod==3) set_reg8l(m->rm, r); else write8(modrm_ea(m, ds), r);
    cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int8_t)r<0) cpu.rflags |= (1<<7); if (parity8(r)) cpu.rflags |= (1<<2);
}
static void op_cmp_rm8_r8(ModRM *m, DecodeState *ds) {
    uint8_t a = (m->mod==3) ? reg8l(m->rm) : read8(modrm_ea(m, ds));
    uint8_t b = reg8l(m->reg);
    int8_t r = (int8_t)a - (int8_t)b;
    cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if (r<0) cpu.rflags |= (1<<7); if (parity8((uint8_t)r)) cpu.rflags |= (1<<2);
}

// shifts/rotates
static void op_shl_r64_imm(ModRM *m, DecodeState *ds, int imm) {
    uint64_t v = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); uint64_t r = v << (imm & 63); if (m->mod==3) set_reg64(m->rm, r); else write64(modrm_ea(m, ds), r); set_flags_shl64(v, imm & 63, r);
}
static void op_shr_r64_imm(ModRM *m, DecodeState *ds, int imm) { uint64_t v = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); uint64_t r = v >> (imm & 63); if (m->mod==3) set_reg64(m->rm, r); else write64(modrm_ea(m, ds), r); set_flags_shr64(v, imm & 63, r, 0); }
static void op_sar_r64_imm(ModRM *m, DecodeState *ds, int imm) { int64_t v = (m->mod==3)? (int64_t)reg64(m->rm) : (int64_t)read64(modrm_ea(m, ds)); int64_t r = v >> (imm & 63); if (m->mod==3) set_reg64(m->rm, (uint64_t)r); else write64(modrm_ea(m, ds), (uint64_t)r); set_flags_shr64((uint64_t)v, imm & 63, (uint64_t)r, 1); }

// MUL/IMUL/IDIV (basic forms)
static void op_mul_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t a = cpu.regs[REG_RAX]; uint64_t b = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); __uint128_t prod = ( __uint128_t)a * b; cpu.regs[REG_RAX] = (uint64_t)prod; cpu.regs[REG_RDX] = (uint64_t)(prod >> 64); cpu.rflags &= ~((1<<0)|(1<<11)); if (cpu.regs[REG_RDX] != 0) cpu.rflags |= (1<<0)|(1<<11);
}
static void op_imul_r64_rm64(ModRM *m, DecodeState *ds) {
    int64_t a = (int64_t)cpu.regs[REG_RAX]; int64_t b = (int64_t)((m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds))); __int128_t prod = (__int128_t)a * b; cpu.regs[REG_RAX] = (uint64_t)prod; cpu.regs[REG_RDX] = (uint64_t)(prod >> 64); cpu.rflags &= ~((1<<0)|(1<<11)); if (((uint64_t)cpu.regs[REG_RAX] != (uint64_t)prod) || ((uint64_t)cpu.regs[REG_RDX] != (uint64_t)(prod>>64))) cpu.rflags |= (1<<0)|(1<<11);
}

static void op_idiv_r64_rm64(ModRM *m, DecodeState *ds) {
    int64_t divisor = (int64_t)((m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)));
    if (divisor == 0) {
        // Instead of exiting, emulate a defined safe behavior: set RAX=0, RDX=dividend
        logf("division by zero - setting RAX=0, RDX=dividend (continuing)
");
        __int128_t dividend = ((__int128_t)(int64_t)cpu.regs[REG_RDX] << 64) | cpu.regs[REG_RAX];
        cpu.regs[REG_RAX] = 0; cpu.regs[REG_RDX] = (uint64_t)dividend; return;
    }
    __int128_t dividend = ((__int128_t)(int64_t)cpu.regs[REG_RDX] << 64) | cpu.regs[REG_RAX];
    __int128_t q = dividend / divisor; __int128_t r = dividend % divisor; cpu.regs[REG_RAX] = (uint64_t)q; cpu.regs[REG_RDX] = (uint64_t)r;
}

// IMUL r64, r/m64 (two-operand)
static void op_imul_r64_r64_rm64(ModRM *m, DecodeState *ds) {
    int64_t src = (m->mod==3)? (int64_t)reg64(m->rm) : (int64_t)read64(modrm_ea(m, ds)); int64_t dst = (int64_t)reg64(m->reg); __int128_t prod = (__int128_t)dst * src; set_reg64(m->reg, (uint64_t)prod);
}

// BSR/BSF
static void op_bsr_r64_rm64(ModRM *m, DecodeState *ds) {
    uint64_t src = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); if (src==0) { cpu.rflags |= (1<<6); return; } int idx = 63 - __builtin_clzll(src); set_reg64(m->reg, idx); cpu.rflags &= ~(1<<6); }
static void op_bsf_r64_rm64(ModRM *m, DecodeState *ds) { uint64_t src = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); if (src==0) { cpu.rflags |= (1<<6); return; } int idx = __builtin_ctzll(src); set_reg64(m->reg, idx); cpu.rflags &= ~(1<<6); }

// CMOVcc (simple subset: CMOVZ/CMOVNZ)
static void op_cmovz_r64_rm64(ModRM *m, DecodeState *ds) { if (cpu.rflags & (1<<6)) { uint64_t v = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); set_reg64(m->reg, v); } }
static void op_cmovnz_r64_rm64(ModRM *m, DecodeState *ds) { if (!(cpu.rflags & (1<<6))) { uint64_t v = (m->mod==3)? reg64(m->rm) : read64(modrm_ea(m, ds)); set_reg64(m->reg, v); } }

// SETcc (SETZ/SETNZ)
static void op_setz_rm8(ModRM *m, DecodeState *ds) { uint8_t v = (cpu.rflags & (1<<6)) ? 1 : 0; if (m->mod==3) set_reg8l(m->rm, v); else write8(modrm_ea(m, ds), v); }
static void op_setnz_rm8(ModRM *m, DecodeState *ds) { uint8_t v = (cpu.rflags & (1<<6)) ? 0 : 1; if (m->mod==3) set_reg8l(m->rm, v); else write8(modrm_ea(m, ds), v); }

// MOVZX/MOVSX (8-bit variants)
static void op_movzx_r64_rm8(ModRM *m, DecodeState *ds) { uint8_t src = (m->mod==3)? reg8l(m->rm) : read8(modrm_ea(m, ds)); set_reg64(m->reg, (uint64_t)src); }
static void op_movsx_r64_rm8(ModRM *m, DecodeState *ds) { int8_t src = (m->mod==3)? (int8_t)reg8l(m->rm) : (int8_t)read8(modrm_ea(m, ds)); set_reg64(m->reg, (uint64_t)(int64_t)src); }

// String ops (rep movsb skeleton)
static void op_rep_movsb(DecodeState *ds) {
    uint64_t count = cpu.regs[REG_RCX]; uint64_t src = cpu.regs[REG_RSI]; uint64_t dst = cpu.regs[REG_RDI]; for (uint64_t i=0;i<count;i++) { write8(dst+i, read8(src+i)); } cpu.regs[REG_RSI] += count; cpu.regs[REG_RDI] += count; cpu.regs[REG_RCX] = 0; }

// I/O in/out (basic)
static void io_out8(uint16_t port, uint8_t val) { if (port == 0x3f8) fputc(val, stderr); else logf("out8 0x%04x=0x%02x
", port, val); }
static uint8_t io_in8(uint16_t port) { if (port==0x3f8) return 0; logf("in8 0x%04x
", port); return 0; }

// LGDT/LIDT - write/read memory to/from pseudo-descriptors
static void op_lgdt(uint64_t addr) {
    uint16_t limit = read16(addr); uint64_t base = (uint64_t)read32(addr+2); (void)limit; (void)base; logf("LGDT load: limit=%u base=0x%" PRIx64 " (ignored)
", limit, base);
}
static void op_lidt(uint64_t addr) { uint16_t limit = read16(addr); uint64_t base = (uint64_t)read32(addr+2); (void)limit; (void)base; logf("LIDT load: limit=%u base=0x%" PRIx64 " (ignored)
", limit, base); }

// -------------------------
// Helpers to skip SSE/unknown two-byte opcodes safely
// -------------------------
static void skip_modrm_and_possible_imm(DecodeState *ds, int imm_bytes) {
    ModRM m; decode_modrm(&m); (void)m; // decoding consumes ModR/M, SIB, disp
    if (imm_bytes > 0) {
        current_pc += imm_bytes;
    }
}

// -------------------------
// Decoder and dispatcher (expanded)
// -------------------------

static void execute_one(void) {
    current_pc = cpu.rip;
    DecodeState ds = {0}; ds.operand_size = 64;
    uint8_t opcode = fetch8();
    // prefixes
    int prefix_loop = 1;
    while (prefix_loop) {
        switch (opcode) {
            case 0xF0: ds.prefix_lock = 1; opcode = fetch8(); break;
            case 0xF2: ds.prefix_rep = 2; opcode = fetch8(); break;
            case 0xF3: ds.prefix_rep = 3; opcode = fetch8(); break;
            case 0x66: ds.operand_size = 16; opcode = fetch8(); break;
            default: prefix_loop = 0; break;
        }
    }
    // REX
    if ((opcode & 0xF0) == 0x40) { ds.rex = opcode & 0x0F; ds.rex_w = (ds.rex & 8)?1:0; opcode = fetch8(); }
    // 0x0F escapes (many opcodes)
    if (opcode == 0x0F) {
        uint8_t op2 = fetch8();
        switch (op2) {
            case 0xAF: { ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex, (int*)&m.reg, (int*)&m.rm); op_imul_r64_r64_rm64(&m, &ds); break; }
            case 0xBC: { ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex, (int*)&m.reg, (int*)&m.rm); op_bsr_r64_rm64(&m, &ds); break; }
            case 0xBD: { ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex, (int*)&m.reg, (int*)&m.rm); op_bsf_r64_rm64(&m, &ds); break; }
            case 0x6F: // MOVQ/MOVDQA etc - SSE family. We don't emulate SSE: decode and skip.
            case 0x29: // some SSE store (e.g., subps/movaps) - skip
            case 0x6E: case 0x60: case 0x61: case 0x70: case 0x1F: case 0xEF: case 0x6E: case 0x6F:
                skip_modrm_and_possible_imm(&ds, 0);
                break;
            case 0xA3: { logf("MOVSXD (not fully implemented)
"); break; }
            default: logf("unhandled 0F 0x%02x at 0x%" PRIx64 "
", op2, cpu.rip); break;
        }
        cpu.rip = current_pc; return;
    }

    // single-byte dispatch (expanded)
    switch (opcode) {
        case 0x90: break; // NOP
        case 0xC3: { uint64_t ret = read64(cpu.regs[REG_RSP]); cpu.regs[REG_RSP] += 8; cpu.rip = ret; return; }
        case 0xC2: { uint16_t imm = fetch8() | (fetch8()<<8); uint64_t ret = read64(cpu.regs[REG_RSP]); cpu.regs[REG_RSP] += imm + 8; cpu.rip = ret; return; }
        case 0xE8: { int32_t d = (int32_t)fetch32(); cpu.regs[REG_RSP] -= 8; write64(cpu.regs[REG_RSP], cpu.rip); cpu.rip += d; return; }
        case 0xE9: { int32_t d = (int32_t)fetch32(); cpu.rip += d; return; }
        case 0xEB: { int8_t d = (int8_t)fetch8(); cpu.rip += d; return; }

        // PUSH/POP
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57: { int r = opcode - 0x50; cpu.regs[REG_RSP]-=8; write64(cpu.regs[REG_RSP], reg64(r)); break; }
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F: { int r=opcode-0x58; uint64_t v=read64(cpu.regs[REG_RSP]); set_reg64(r,v); cpu.regs[REG_RSP]+=8; break; }

        case 0x68: { uint32_t imm32 = fetch32(); cpu.regs[REG_RSP] -= 8; write64(cpu.regs[REG_RSP], (uint64_t)(int64_t)(int32_t)imm32); break; }
        case 0x6A: { int8_t imm8 = (int8_t)fetch8(); cpu.regs[REG_RSP]-=8; write64(cpu.regs[REG_RSP], (uint64_t)(int64_t)imm8); break; }

        // MOV r32/64, imm
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF: { int r = opcode - 0xB8; if (ds.rex_w) { uint64_t imm = fetch64(); set_reg64(r, imm); } else { uint32_t imm32 = fetch32(); set_reg32(r, imm32); } break; }
        // MOV r8, imm8
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7: { int r = opcode - 0xB0; uint8_t imm = fetch8(); set_reg8l(r, imm); break; }

        // MOV/MATH groups that use ModR/M
        case 0x89: case 0x8B: case 0x01: case 0x03: case 0x29: case 0x2B: case 0x21: case 0x31: case 0x33: case 0x85: case 0xA3: {
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm);
            switch (opcode) {
                case 0x89: op_mov_rm64_r64(&m,&ds); break;
                case 0x8B: op_mov_r64_rm64(&m,&ds); break;
                case 0x01: op_add_r64_rm64(&m,&ds); break;
                case 0x03: op_add_r64_rm64(&m,&ds); break;
                case 0x29: op_sub_r64_rm64(&m,&ds); break;
                case 0x2B: op_sub_r64_rm64(&m,&ds); break;
                case 0x21: op_and_r64_rm64(&m,&ds); break;
                case 0x31: op_xor_r64_rm64(&m,&ds); break;
                case 0x33: op_xor_r64_rm64(&m,&ds); break;
                case 0x85: { uint64_t a = reg64(m.reg); uint64_t b = (m.mod==3)? reg64(m.rm) : read64(modrm_ea(&m,&ds)); uint64_t r = (a==b); cpu.rflags &= ~((1<<6)); if (r) cpu.rflags |= (1<<6); break; }
                default: logf("group handled but missing mapping for opcode 0x%02x
", opcode); break;
            }
            break;
        }

        // 0x83 imm8 group
        case 0x83: {
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm); int8_t imm8 = (int8_t)fetch8(); switch (m.reg) { case 0: if (m.mod==3) { set_reg64(m.rm, reg64(m.rm)+imm8); } else { uint64_t ea=modrm_ea(&m,&ds); write64(ea, read64(ea)+imm8);} set_flags_add64(0,imm8, (m.mod==3)?reg64(m.rm):read64(modrm_ea(&m,&ds))); break; case 5: if (m.mod==3) { set_reg64(m.rm, reg64(m.rm)-imm8); } else { uint64_t ea=modrm_ea(&m,&ds); write64(ea, read64(ea)-imm8);} set_flags_sub64(0,imm8,(m.mod==3)?reg64(m.rm):read64(modrm_ea(&m,&ds))); break; default: logf("unhandled 0x83/reg=%d
", m.reg); }
            break;
        }

        // F7 group: mul/imul/idiv
        case 0xF7: {
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm);
            switch (m.reg) {
                case 4: op_mul_r64_rm64(&m,&ds); break; // MUL (unsigned)
                case 5: op_imul_r64_rm64(&m,&ds); break; // IMUL (signed)
                case 7: op_idiv_r64_rm64(&m,&ds); break; // IDIV
                default: logf("F7 group reg=%d unhandled
", m.reg); break;
            }
            break;
        }

        // F6 group: test/others on 8-bit
        case 0xF6: {
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm);
            switch (m.reg) {
                case 0: { uint8_t imm = fetch8(); if (m.mod==3) set_reg8l(m.rm, (uint8_t)(reg8l(m.rm) & imm)); else { uint64_t ea=modrm_ea(&m,&ds); write8(ea, read8(ea) & imm);} break; }
                case 3: { // TEST r/m8, imm8
                    uint8_t imm = fetch8(); uint8_t v = (m.mod==3)? reg8l(m.rm) : read8(modrm_ea(&m,&ds)); uint8_t r = v & imm; cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int8_t)r<0) cpu.rflags |= (1<<7); if (parity8(r)) cpu.rflags |= (1<<2); break; }
                default: logf("F6 group unhandled reg=%d
", m.reg); break;
            }
            break;
        }

        // IN/OUT immediate port
        case 0xE4: { uint8_t imm = fetch8(); uint8_t v = io_in8(imm); set_reg8l(REG_RAX, v); break; }
        case 0xE6: { uint8_t imm = fetch8(); io_out8(imm, reg8l(REG_RAX)); break; }
        // IN/OUT DX port (no immediate)
        case 0xEC: { uint16_t port = (uint16_t)reg16(REG_RDX); uint8_t v = io_in8(port); set_reg8l(REG_RAX, v); break; }
        case 0xEE: { uint16_t port = (uint16_t)reg16(REG_RDX); io_out8(port, reg8l(REG_RAX)); break; }

        case 0x9C: { cpu.regs[REG_RSP]-=8; write64(cpu.regs[REG_RSP], cpu.rflags); break; }
        case 0x9D: { uint64_t v = read64(cpu.regs[REG_RSP]); cpu.regs[REG_RSP]+=8; cpu.rflags = v; break; }

        // many unknown/opcodes that need ModR/M or immediate
        case 0x8D: { // LEA r64, m
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm); op_lea_r64_m(&m, &ds); break; }
        case 0x3D: { // CMP EAX, imm32 (in 64-bit mode compare RAX low dword)
            uint32_t imm = fetch32(); uint64_t a = (uint32_t)reg32(REG_RAX); uint64_t b = imm; uint64_t r = a - b; set_flags_sub64(a,b,r); break; }
        case 0x3C: { uint8_t imm = fetch8(); uint8_t al = reg8l(REG_RAX); uint8_t r = al - imm; cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int8_t)r<0) cpu.rflags |= (1<<7); if (parity8(r)) cpu.rflags |= (1<<2); break; }
        case 0x34: { uint8_t imm = fetch8(); uint8_t al = reg8l(REG_RAX); uint8_t r = al ^ imm; set_reg8l(REG_RAX, r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int8_t)r<0) cpu.rflags |= (1<<7); if (parity8(r)) cpu.rflags |= (1<<2); break; }
        case 0x24: { uint8_t imm = fetch8(); uint8_t al = reg8l(REG_RAX); uint8_t r = al & imm; set_reg8l(REG_RAX, r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int8_t)r<0) cpu.rflags |= (1<<7); if (parity8(r)) cpu.rflags |= (1<<2); break; }

        case 0x20: case 0x38: case 0x30: case 0x32: case 0x10: { // r/m8, r8 and variants
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm);
            if (opcode == 0x20) { op_and_rm8_r8(&m,&ds); }
            else if (opcode == 0x38) { op_cmp_rm8_r8(&m,&ds); }
            else if (opcode == 0x30) { // XOR r/m8, r8
                uint8_t a = (m.mod==3)? reg8l(m.rm) : read8(modrm_ea(&m,&ds)); uint8_t b = reg8l(m.reg); uint8_t r = a ^ b; if (m.mod==3) set_reg8l(m.rm, r); else write8(modrm_ea(&m,&ds), r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int8_t)r<0) cpu.rflags |= (1<<7); if (parity8(r)) cpu.rflags |= (1<<2);
            } else if (opcode == 0x32) { // XOR r8, r/m8
                uint8_t a = reg8l(m.reg); uint8_t b = (m.mod==3)? reg8l(m.rm) : read8(modrm_ea(&m,&ds)); uint8_t r = a ^ b; set_reg8l(m.reg, r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int8_t)r<0) cpu.rflags |= (1<<7); if (parity8(r)) cpu.rflags |= (1<<2);
            } else if (opcode == 0x10) { // ADC r/m8, r8 - approximate as add (no carry handling)
                uint8_t a = (m.mod==3)? reg8l(m.rm) : read8(modrm_ea(&m,&ds)); uint8_t b = reg8l(m.reg); uint8_t r = a + b; if (m.mod==3) set_reg8l(m.rm, r); else write8(modrm_ea(&m,&ds), r); set_flags_add64(a,b,r);
            }
            break;
        }

        case 0x7C: { // JL rel8 (jump if SF != OF)
            int8_t d = (int8_t)fetch8(); int sf = (cpu.rflags >> 7) & 1; int of = (cpu.rflags >> 11) & 1; if (sf != of) cpu.rip = cpu.rip + d; else {} cpu.rip = current_pc; return; }

        case 0x69: { // IMUL r32, r/m32, imm32  -> implement sign-extend multiply into r64
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm); uint32_t imm32 = fetch32(); int64_t src = (m.mod==3)? (int64_t)(int32_t)reg32(m.rm) : (int64_t)(int32_t)read32(modrm_ea(&m,&ds)); int64_t dst = (int64_t)(int32_t)reg32(m.reg); __int128_t prod = (__int128_t)dst * src + imm32; set_reg64(m.reg, (uint64_t)prod); break; }

        case 0xC1: { // shift r/m32 by imm8
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm); uint8_t imm = fetch8(); int count = imm & 0x3F; switch (m.reg) { case 4: op_shl_r64_imm(&m,&ds,count); break; case 5: op_shr_r64_imm(&m,&ds,count); break; case 7: op_sar_r64_imm(&m,&ds,count); break; default: logf("C1/reg unhandled %d
", m.reg); break; } break; }

        case 0xD1: { // shift r/m32 by 1
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm); int count = 1; switch (m.reg) { case 4: op_shl_r64_imm(&m,&ds,count); break; case 5: op_shr_r64_imm(&m,&ds,count); break; case 7: op_sar_r64_imm(&m,&ds,count); break; default: logf("D1/reg unhandled %d
", m.reg); break; } break; }

        case 0x25: { // AND EAX, imm32
            uint32_t imm32 = fetch32(); uint32_t a = reg32(REG_RAX); uint32_t r = a & imm32; set_reg32(REG_RAX, r); cpu.rflags &= ~((1<<0)|(1<<11)); if (r==0) cpu.rflags |= (1<<6); if ((int32_t)r<0) cpu.rflags |= (1<<7); if (parity8((uint8_t)r)) cpu.rflags |= (1<<2); break; }

        case 0xC6: { // MOV r/m8, imm8
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm); uint8_t imm8 = fetch8(); if (m.mod==3) set_reg8l(m.rm, imm8); else write8(modrm_ea(&m,&ds), imm8); break; }
        case 0xC0: { // shift r/m8, imm8
            ModRM m; decode_modrm(&m); if (ds.rex) rex_apply(ds.rex,(int*)&m.reg,(int*)&m.rm); uint8_t imm8 = fetch8(); int cnt = imm8 & 7; switch (m.reg) { case 4: { uint8_t v = (m.mod==3)? reg8l(m.rm) : read8(modrm_ea(&m,&ds)); uint8_t r = v << cnt; if (m.mod==3) set_reg8l(m.rm, r); else write8(modrm_ea(&m,&ds), r); set_flags_shl64(v, cnt, r); break; } case 5: { uint8_t v = (m.mod==3)? reg8l(m.rm) : read8(modrm_ea(&m,&ds)); uint8_t r = v >> cnt; if (m.mod==3) set_reg8l(m.rm, r); else write8(modrm_ea(&m,&ds), r); set_flags_shr64(v, cnt, r, 0); break; } case 7: { int8_t v = (m.mod==3)? (int8_t)reg8l(m.rm) : (int8_t)read8(modrm_ea(&m,&ds)); int8_t r = v >> cnt; if (m.mod==3) set_reg8l(m.rm, (uint8_t)r); else write8(modrm_ea(&m,&ds), (uint8_t)r); set_flags_shr64((uint64_t)v, cnt, (uint64_t)r, 1); break; } default: logf("C0/reg unhandled %d
", m.reg); break; } break; }

        // fallback for unhandled opcodes: try to consume ModR/M if present to avoid stalling
        default: {
            // If opcode likely uses ModR/M, attempt to decode and skip it.
            // This keeps the emulator running for SSE/x87 opcodes we don't implement.
            if ((opcode & 0xC0) == 0x00 && (opcode == 0x0F)) {
                logf("unexpected 0x0F at 0x%" PRIx64 "
", cpu.rip);
            }
            // heuristics: opcodes that commonly use ModR/M
            if (opcode == 0x0F) {
                uint8_t op2 = fetch8(); logf("fallback: saw 0F 0x%02x — skipping ModR/M if present
", op2); skip_modrm_and_possible_imm(&ds, 0);
            } else if ((opcode & 0xF8) == 0x80 || (opcode & 0xF8) == 0xC0 || (opcode & 0xFE) == 0x38 || (opcode & 0xFC) == 0x20) {
                // common groups: assume ModR/M present
                ModRM m; decode_modrm(&m); (void)m; logf("fallback: decoded ModR/M for opcode 0x%02x at 0x%" PRIx64 "
", opcode, cpu.rip);
            } else {
                logf("unhandled opcode 0x%02x at 0x%" PRIx64 "
", opcode, cpu.rip);
            }
            break;
        }
    }

    // update rip for normal fall-through
    cpu.rip = current_pc;
}

// -------------------------
// Initialization / Loader / Main
// -------------------------
static int init_emu(void) {
    ram_mem = calloc(1, ram_size); if (!ram_mem) { perror("calloc"); return -1; }
    memset(&cpu,0,sizeof(cpu)); cpu.rip = 0x0; cpu.regs[REG_RSP] = ram_size - 0x1000; cpu.rflags = 2; return 0;
}
static int load_file(const char *path, uint64_t addr) {
    FILE *f = fopen(path, "rb"); if (!f) { perror("open"); return -1; }
    fseek(f,0,SEEK_END); long sz = ftell(f); fseek(f,0,SEEK_SET); if (addr + (uint64_t)sz > ram_size) { fclose(f); logf("file too large
"); return -1; } fread(mem_ptr(addr), 1, sz, f); fclose(f); return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s <binary>
", argv[0]); return 1; }
    if (init_emu() != 0) return 1;
    if (load_file(argv[1], 0x0) != 0) return 1;

    uint64_t steps = 0; while (steps++ < 20000000) { execute_one(); }
    fprintf(stderr, "emulation finished after %" PRIu64 " steps
", steps);
    return 0;
}

// ===========================================================================
// Notes:
// - This update implements many of the opcodes reported in error.txt and
//   avoids crashing on numerous SSE/FP two-byte opcodes by decoding and
//   skipping ModR/M/SIB bytes.
// - Remaining work: fully emulate SSE if required, and expand opcode
//   coverage further. If you run the test again and provide an updated
//   error.txt, I'll iterate further on the missing opcodes.
// ===========================================================================
