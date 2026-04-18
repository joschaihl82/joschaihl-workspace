// jemu.c
// Amalgamated, corrected 64-bit JIT emulator with SDL2 rendering and a blinking demo.
// ISA: 3-byte fixed instructions: [opcode:1][arg_lo:1][arg_hi:1]
// Argument layout for reg-reg ops: low nibble = reg_a, next nibble = reg_b.
// Immediates: imm16 is little-endian in the 2 arg bytes and zero-extended to 64-bit when stored to a register.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

#ifdef __INTELLISENSE__
// Minimal stubs to keep intellisense quiet.
#define SDL_INIT_VIDEO 0
#define SDL_WINDOWPOS_CENTERED 0
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef union SDL_Event SDL_Event;
#else
#include <SDL2/SDL.h>
#endif

// -----------------------------------------------------------------------------
// Configuration and state layout
// -----------------------------------------------------------------------------

#define FB_WIDTH   64
#define FB_HEIGHT  64
#define RAM_SIZE   4096 // stack/data RAM

typedef struct {
    // 16 general-purpose 64-bit registers (rA..rP)
    uint64_t rA; // 0x00
    uint64_t rB; // 0x08
    uint64_t rC; // 0x10
    uint64_t rD; // 0x18
    uint64_t rE; // 0x20
    uint64_t rF; // 0x28
    uint64_t rG; // 0x30
    uint64_t rH; // 0x38
    uint64_t rI; // 0x40
    uint64_t rJ; // 0x48
    uint64_t rK; // 0x50
    uint64_t rL; // 0x58
    uint64_t rM; // 0x60
    uint64_t rN; // 0x68
    uint64_t rO; // 0x70
    uint64_t rP; // 0x78

    // System registers
    uint64_t rSP;      // 0x80: stack pointer (byte offset into ram[])
    uint64_t rFlags;   // 0x88: flags register (bit0 = ZF). We write/read only the low byte.
    uint64_t rTimer;   // 0x90: monotonic timer

    // Framebuffer (ARGB8888)
    uint32_t framebuffer[FB_WIDTH * FB_HEIGHT]; // 0x98 .. 0x98+0x4000

    // Emulated RAM / stack
    uint8_t ram[RAM_SIZE]; // 0x4098 .. + RAM_SIZE

    // JIT helper
    int pc;
} CPUState;

// Offsets
#define RA_OFF       0x00
#define RB_OFF       0x08
#define RC_OFF       0x10
#define RD_OFF       0x18
#define RE_OFF       0x20
#define RF_OFF       0x28
#define RG_OFF       0x30
#define RH_OFF       0x38
#define RI_OFF       0x40
#define RJ_OFF       0x48
#define RK_OFF       0x50
#define RL_OFF       0x58
#define RM_OFF       0x60
#define RN_OFF       0x68
#define RO_OFF       0x70
#define RP_OFF       0x78
#define RSP_OFF      0x80
#define RFLAGS_OFF   0x88
#define RTIMER_OFF   0x90
#define FB_OFF       0x98
#define RAM_OFF      (FB_OFF + (FB_WIDTH * FB_HEIGHT * sizeof(uint32_t))) // 0x4098

// ISA opcodes
#define OP_HALT        0x00
#define OP_MOV_IMM     0x01 // reg[a] = zero_extend(imm16)
#define OP_MOV_REG     0x02 // reg[a] = reg[b]
#define OP_ADD_REG     0x03 // reg[a] += reg[b]
#define OP_SUB_REG     0x04 // reg[a] -= reg[b]
#define OP_AND_REG     0x10 // reg[a] &= reg[b]
#define OP_OR_REG      0x11 // reg[a] |= reg[b]
#define OP_XOR_REG     0x12 // reg[a] ^= reg[b]
#define OP_NOT_REG     0x13 // reg[a] = ~reg[a]
#define OP_SHL_REG     0x14 // reg[a] <<= (reg[b] & 0x3F) using CL
#define OP_SHR_REG     0x15 // reg[a] >>= (reg[b] & 0x3F) using CL
#define OP_INC_REG     0x16 // reg[a]++
#define OP_DEC_REG     0x17 // reg[a]--
#define OP_CMP_REG     0x20 // sets rFlags.ZF = (reg[a] == reg[b]) ? 1 : 0 (low byte)
#define OP_JMP_REL     0x21 // pc += imm16 (signed)
#define OP_JNE_REL     0x22 // if ZF==0: pc += imm16 (signed)
#define OP_JE_REL      0x23 // if ZF==1: pc += imm16 (signed)
#define OP_PUSH_REG    0x30 // [rsp -= 8] = reg[a]
#define OP_POP_REG     0x31 // reg[a] = [rsp]; rsp += 8
#define OP_GET_TIMER   0x40 // reg[a] = rTimer; rTimer++ (64-bit)
#define OP_WRITE_FB_A  0x07 // framebuffer[reg[B] (low 32)] = reg[A] (low 32)

// JIT function signature
typedef void (*JIT_Func)(CPUState* state);

// Jump fixup record
typedef struct {
    int target_rom_pc;     // absolute ROM pc (byte address)
    uint8_t* disp_ptr;     // pointer to the rel32 immediate field
} JumpFixup;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static uint64_t get_reg_offset(int idx) {
    switch (idx) {
        case 0: return RA_OFF; case 1: return RB_OFF; case 2: return RC_OFF; case 3: return RD_OFF;
        case 4: return RE_OFF; case 5: return RF_OFF; case 6: return RG_OFF; case 7: return RH_OFF;
        case 8: return RI_OFF; case 9: return RJ_OFF; case 10: return RK_OFF; case 11: return RL_OFF;
        case 12: return RM_OFF; case 13: return RN_OFF; case 14: return RO_OFF; case 15: return RP_OFF;
        case 16: return RSP_OFF; // rSP via index 16 when needed
        default: return UINT64_MAX;
    }
}

static uint8_t* allocate_executable_memory(size_t size) {
    long ps = sysconf(_SC_PAGE_SIZE);
    if (ps <= 0) ps = 4096;
    size_t page_size = (size_t)ps;
    size = (size + page_size - 1) & ~(page_size - 1);

    uint8_t* buf = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED) {
        perror("mmap exec memory");
        return NULL;
    }
    printf("[jit] allocated %zu bytes at %p\n", size, (void*)buf);
    return buf;
}

static void release_executable_memory(void* p, size_t size) {
    long ps = sysconf(_SC_PAGE_SIZE);
    if (ps <= 0) ps = 4096;
    size_t page_size = (size_t)ps;
    size = (size + page_size - 1) & ~(page_size - 1);
    if (p && p != MAP_FAILED) munmap(p, size);
}

// Emit ModRM for [rdi + disp32] addressing, reg field = reg (0..7)
static void emit_modrm_disp32_rdi(uint8_t** p, uint8_t reg) {
    // mod=10 (disp32), reg=reg(3 bits), r/m=111 (rdi)
    **p = (uint8_t)(0x80 | ((reg & 7) << 3) | 0x07);
    (*p)++;
}

// Emit 32-bit displacement
static void emit_disp32(uint8_t** p, uint32_t d32) {
    *((uint32_t*)(*p)) = d32;
    (*p) += 4;
}

// Load 64-bit emu reg into RAX
static void emit_load_reg64_rax(uint8_t** p, int reg_idx) {
    uint64_t off = get_reg_offset(reg_idx);
    *(*p)++ = 0x48;  // REX.W
    *(*p)++ = 0x8B;  // MOV r64, r/m64
    emit_modrm_disp32_rdi(p, 0); // reg=RAX
    emit_disp32(p, (uint32_t)off);
}

// Load 64-bit emu reg into RCX
static void emit_load_reg64_rcx(uint8_t** p, int reg_idx) {
    uint64_t off = get_reg_offset(reg_idx);
    *(*p)++ = 0x48;  // REX.W
    *(*p)++ = 0x8B;  // MOV r64, r/m64
    emit_modrm_disp32_rdi(p, 1); // reg=RCX
    emit_disp32(p, (uint32_t)off);
}

// Store RAX into 64-bit emu reg
static void emit_store_reg64_from_rax(uint8_t** p, int reg_idx) {
    uint64_t off = get_reg_offset(reg_idx);
    *(*p)++ = 0x48;  // REX.W
    *(*p)++ = 0x89;  // MOV r/m64, r64
    emit_modrm_disp32_rdi(p, 0); // reg=RAX
    emit_disp32(p, (uint32_t)off);
}

// Store RCX into 64-bit emu reg
static void emit_store_reg64_from_rcx(uint8_t** p, int reg_idx) {
    uint64_t off = get_reg_offset(reg_idx);
    *(*p)++ = 0x48;  // REX.W
    *(*p)++ = 0x89;  // MOV r/m64, r64
    emit_modrm_disp32_rdi(p, 1); // reg=RCX
    emit_disp32(p, (uint32_t)off);
}

// -----------------------------------------------------------------------------
// JIT compiler
// -----------------------------------------------------------------------------

static JIT_Func jit_compile(const uint8_t* rom, size_t rom_size) {
    const size_t MAX_CODE = 32768;
    uint8_t* code = allocate_executable_memory(MAX_CODE);
    if (!code) return NULL;
    uint8_t* cp = code;

    // Mapping ROM pc (per-instruction start) -> native code offset
    const int MAX_INS = 4096; // 3 bytes per inst => up to ~12KB ROM
    size_t pc_to_native[MAX_INS];
    typedef struct { int target_rom_pc; uint8_t* disp_ptr; } Fix;
    Fix fixups[MAX_INS];
    int fixup_count = 0;

    int pc = 0;
    while (pc < (int)rom_size) {
        int idx = pc / 3;
        if (idx >= MAX_INS) {
            fprintf(stderr, "[jit] ROM too large\n");
            release_executable_memory(code, MAX_CODE);
            return NULL;
        }
        pc_to_native[idx] = (size_t)(cp - code);

        uint8_t op = rom[pc];
        uint8_t arg_lo = (pc + 1 < (int)rom_size) ? rom[pc + 1] : 0;
        uint8_t arg_hi = (pc + 2 < (int)rom_size) ? rom[pc + 2] : 0;
        uint16_t arg = (uint16_t)(arg_lo | (arg_hi << 8));

        int reg_a = arg & 0x0F;
        int reg_b = (arg >> 4) & 0x0F;
        int16_t simm16 = (int16_t)arg;

        switch (op) {
            case OP_HALT: {
                *cp++ = 0xC3;     // ret
                pc += 3;
                goto finalize;    // End of JIT stream
            }

            case OP_MOV_IMM: {
                // reg[a] = zero_extend(imm16) via two 32-bit stores
                uint64_t off = get_reg_offset(reg_a);
                *cp++ = 0xC7; *cp++ = 0x87; emit_disp32(&cp, (uint32_t)off);        // MOV dword [rdi+off], imm32
                *((uint32_t*)cp) = (uint32_t)arg; cp += 4;
                *cp++ = 0xC7; *cp++ = 0x87; emit_disp32(&cp, (uint32_t)(off + 4));  // MOV dword [rdi+off+4], 0
                *((uint32_t*)cp) = 0; cp += 4;
                pc += 3;
                break;
            }

            case OP_MOV_REG: {
                emit_load_reg64_rax(&cp, reg_b);
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_ADD_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                emit_load_reg64_rcx(&cp, reg_b);
                *cp++ = 0x48; *cp++ = 0x01; *cp++ = 0xC8; // add rax, rcx
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_SUB_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                emit_load_reg64_rcx(&cp, reg_b);
                *cp++ = 0x48; *cp++ = 0x29; *cp++ = 0xC8; // sub rax, rcx
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_AND_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                emit_load_reg64_rcx(&cp, reg_b);
                *cp++ = 0x48; *cp++ = 0x21; *cp++ = 0xC8; // and rax, rcx
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_OR_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                emit_load_reg64_rcx(&cp, reg_b);
                *cp++ = 0x48; *cp++ = 0x09; *cp++ = 0xC8; // or rax, rcx
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_XOR_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                emit_load_reg64_rcx(&cp, reg_b);
                *cp++ = 0x48; *cp++ = 0x31; *cp++ = 0xC8; // xor rax, rcx
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_NOT_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                *cp++ = 0x48; *cp++ = 0xF7; *cp++ = 0xD0; // not rax (F7 /2, mod=11, r/m=rax)
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_SHL_REG:
            case OP_SHR_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                emit_load_reg64_rcx(&cp, reg_b);
                *cp++ = 0x48; *cp++ = 0xD3;
                *cp++ = (op == OP_SHL_REG) ? 0xE0 : 0xE8; // shl rax, cl / shr rax, cl
                emit_store_reg64_from_rax(&cp, reg_a);
                pc += 3;
                break;
            }

            case OP_INC_REG: {
                uint64_t off = get_reg_offset(reg_a);
                *cp++ = 0x48; *cp++ = 0xFF; *cp++ = 0x87; // REX.W FF /0, [rdi+disp32]
                emit_disp32(&cp, (uint32_t)off);
                pc += 3;
                break;
            }

            case OP_DEC_REG: {
                uint64_t off = get_reg_offset(reg_a);
                *cp++ = 0x48; *cp++ = 0xFF; *cp++ = 0x8F; // REX.W FF /1, [rdi+disp32]
                emit_disp32(&cp, (uint32_t)off);
                pc += 3;
                break;
            }

            case OP_CMP_REG: {
                emit_load_reg64_rax(&cp, reg_a);
                uint64_t offb = get_reg_offset(reg_b);
                *cp++ = 0x48; *cp++ = 0x3B;          // cmp rax, [rdi+offb]
                emit_modrm_disp32_rdi(&cp, 0);       // reg=RAX
                emit_disp32(&cp, (uint32_t)offb);
                *cp++ = 0x0F; *cp++ = 0x94; *cp++ = 0xC0; // setz al
                *cp++ = 0x88; *cp++ = 0x87;               // mov byte ptr [rdi + RFLAGS_OFF], al
                emit_disp32(&cp, (uint32_t)RFLAGS_OFF);
                pc += 3;
                break;
            }

            case OP_JMP_REL: {
                *cp++ = 0xE9;             // jmp rel32
                uint8_t* disp = cp; cp += 4;
                fixups[fixup_count].target_rom_pc = pc + 3 + simm16;
                fixups[fixup_count].disp_ptr = disp;
                fixup_count++;
                pc += 3;
                break;
            }

            case OP_JNE_REL:
            case OP_JE_REL: {
                *cp++ = 0x0F; *cp++ = 0xB6;  // MOVZX r32, r/m8
                *cp++ = 0x87;                // mod=10, reg=EAX, r/m=rdi
                emit_disp32(&cp, (uint32_t)RFLAGS_OFF);
                *cp++ = 0x85; *cp++ = 0xC0;  // test eax, eax
                *cp++ = 0x0F;
                *cp++ = (op == OP_JNE_REL) ? 0x85 : 0x84;
                uint8_t* disp = cp; cp += 4;
                fixups[fixup_count].target_rom_pc = pc + 3 + simm16;
                fixups[fixup_count].disp_ptr = disp;
                fixup_count++;
                pc += 3;
                break;
            }

            case OP_PUSH_REG: {
                emit_load_reg64_rcx(&cp, 16);                // RCX = rSP
                *cp++ = 0x48; *cp++ = 0x83; *cp++ = 0xE9; *cp++ = 0x08; // sub rcx, 8
                emit_store_reg64_from_rcx(&cp, 16);          // rSP = rcx
                emit_load_reg64_rax(&cp, reg_a);             // RAX = reg[a]
                *cp++ = 0x48; *cp++ = 0x89;  // MOV r/m64, r64
                *cp++ = 0x84;                // ModRM: mod=10, r/m=100 (SIB), reg=RAX
                *cp++ = 0x0F;                // SIB: scale=1, index=rcx(1), base=rdi(7)
                emit_disp32(&cp, (uint32_t)RAM_OFF);
                pc += 3;
                break;
            }

            case OP_POP_REG: {
                emit_load_reg64_rcx(&cp, 16);                // RCX = rSP
                *cp++ = 0x48; *cp++ = 0x8B;  // MOV r64, r/m64
                *cp++ = 0x84;                // ModRM with SIB
                *cp++ = 0x0F;                // SIB (index=rcx, base=rdi)
                emit_disp32(&cp, (uint32_t)RAM_OFF);
                *cp++ = 0x48; *cp++ = 0x83; *cp++ = 0xC1; *cp++ = 0x08; // add rcx, 8
                emit_store_reg64_from_rcx(&cp, 16);          // rSP = rcx
                emit_store_reg64_from_rax(&cp, reg_a);       // reg[a] = RAX
                pc += 3;
                break;
            }

            case OP_GET_TIMER: {
                *cp++ = 0x48; *cp++ = 0x8B; *cp++ = 0x87; emit_disp32(&cp, (uint32_t)RTIMER_OFF);
                emit_store_reg64_from_rax(&cp, reg_a);
                *cp++ = 0x48; *cp++ = 0xFF;
                emit_modrm_disp32_rdi(&cp, 0); emit_disp32(&cp, (uint32_t)RTIMER_OFF); // inc qword [rdi+RTIMER_OFF]
                pc += 3;
                break;
            }

            case OP_WRITE_FB_A: {
                // framebuffer[(uint32_t)rB] = (uint32_t)rA
                *cp++ = 0x8B; *cp++ = 0x8F; emit_disp32(&cp, (uint32_t)RB_OFF); // ECX = low32(rB)
                *cp++ = 0x8B; *cp++ = 0x87; emit_disp32(&cp, (uint32_t)RA_OFF); // EAX = low32(rA)
                *cp++ = 0x89;       // MOV r/m32, r32
                *cp++ = 0x84;       // ModRM: mod=10, r/m=100 (SIB), reg=EAX
                *cp++ = 0x8F;       // SIB: scale=4, index=ECX, base=RDI
                emit_disp32(&cp, (uint32_t)FB_OFF);
                pc += 3;
                break;
            }

            default:
                fprintf(stderr, "[jit] unknown opcode 0x%02X at pc 0x%X\n", op, pc);
                release_executable_memory(code, MAX_CODE);
                return NULL;
        }

        if ((size_t)(cp - code) > MAX_CODE - 64) {
            fprintf(stderr, "[jit] code buffer overflow\n");
            release_executable_memory(code, MAX_CODE);
            return NULL;
        }
    }

finalize:
    // Fixup rel32 displacements
    for (int i = 0; i < fixup_count; i++) {
        int tgt_pc = fixups[i].target_rom_pc;
        if (tgt_pc < 0 || (tgt_pc / 3) >= MAX_INS) {
            fprintf(stderr, "[jit] jump target out of range: pc=%d\n", tgt_pc);
            release_executable_memory(code, MAX_CODE);
            return NULL;
        }
        size_t tgt_native = pc_to_native[tgt_pc / 3];
        uint8_t* disp = fixups[i].disp_ptr;
        size_t next_ip = (size_t)(disp - code) + 4;
        int32_t rel32 = (int32_t)((int64_t)tgt_native - (int64_t)next_ip);
        *((int32_t*)disp) = rel32;
    }

    printf("[jit] compiled %zu bytes\n", (size_t)(cp - code));
    return (JIT_Func)code;
}

// -----------------------------------------------------------------------------
// Demo ROM (blinking)
// -----------------------------------------------------------------------------

static const uint8_t* create_rom(size_t* out_size) {
    // Demo:
    // rB = 0; rC = FB_WIDTH*FB_HEIGHT (limit); rD = blink threshold (e.g., 32)
    // Loop over pixels: write rA when (timer & 0x20) != 0, else write 0; increment rB.
    // When rB == rC, halt.
    const uint16_t LIMIT = (uint16_t)(FB_WIDTH * FB_HEIGHT); // fits in imm16 for 4096
    const uint16_t BLINK_MASK = 0x0020; // blink period ~32

    // PCs (3-byte instructions):
    // 0:  rB = 0
    // 3:  rC = LIMIT
    // 6:  rD = BLINK_MASK
    // 9:  get_timer rE
    // 12: and rE, rD
    // 15: cmp rE, rD
    // 18: jne +3
    // 21: write_fb_a (rB index, rA color)
    // 24: jmp +3 (skip clearing when condition true)
    // 27: mov_reg rA, rA (no-op used as placeholder) -> we'll instead clear via write 0 using temp: we’ll encode clear by temporarily using rF as 0
    //    We’ll do a second write that writes 0 when the condition failed:
    // Actually simpler: structure:
    // 21: write_fb_a (rB) = rA
    // 24: jne +3     // if not equal, skip zeroing (so only write color when equal)
    // 27: mov_imm rA = 0  // temporarily zero color
    // 30: write_fb_a
    // 33: mov_imm rA = original color low16 (restore)
    // 36: inc rB
    // 39: cmp rB, rC
    // 42: jne back to 9
    // 45: halt
    //
    // Back offset from pc 42 to pc 9: 9 - (42 + 3) = -36 = 0xFFDCh

    const int16_t loop_start = 9;
    const int16_t back_offset = (int16_t)(loop_start - (42 + 3)); // -36

    uint8_t rom[] = {
        OP_MOV_IMM, 0x10, 0x00,                 // rC = LIMIT (we'll set actual limit below with two instructions)
        OP_MOV_IMM, 0x01, 0x00,                 // rB = 1 (start at 1 for visible offset)
        OP_MOV_IMM, 0x20, 0x00,                 // rD = BLINK_MASK

        // pc 9: loop start
        OP_GET_TIMER, 0x40, 0x00,               // rE = timer++
        OP_AND_REG,   0x42, 0x00,               // rE &= rD
        OP_CMP_REG,   0x42, 0x00,               // cmp rE, rD
        OP_JNE_REL,   0x03, 0x00,               // if not equal, skip color write
        OP_WRITE_FB_A,0x01, 0x00,               // fb[rB] = rA

        // If equal was false, fall-through; now skip zero write
        OP_JMP_REL,   0x03, 0x00,               // jump over zeroing when we wrote color

        // Zero branch: temporarily zero rA, write 0, restore color
        OP_MOV_IMM,   0x00, 0x00,               // rA = 0 (low16)
        OP_WRITE_FB_A,0x01, 0x00,               // fb[rB] = rA (0)
        // Restore color (low16; upper restored by JIT zero-high)
        OP_MOV_IMM,   0x00, 0xFF,               // rA = 0x00FF (blue low16)

        // Advance and loop test
        OP_INC_REG,   0x10, 0x00,               // rB++
        OP_CMP_REG,   0x13, 0x00,               // cmp rB, rC
        OP_JNE_REL,   (uint8_t)(back_offset & 0xFF), (uint8_t)((back_offset >> 8) & 0xFF),

        OP_HALT,      0x00, 0x00
    };

    // Patch LIMIT into rC: rC = LIMIT (16-bit immediate fits)
    // The instruction at index 0 is OP_MOV_IMM, arg_lo=0x10 means reg_a=0 (low nibble)=0? We intend reg_a=rC (index 2).
    // To avoid confusion, rebuild a clean ROM:

    uint8_t rom2[] = {
        OP_MOV_IMM, 0x00, 0x00,                 // rA = 0 (we’ll set color in CPU state; this clears any residual)
        OP_MOV_IMM, 0x01, 0x00,                 // rB = 1
        OP_MOV_IMM, 0x10, (uint8_t)(LIMIT >> 8),// rC = LIMIT (lo nibble=0, hi nibble=1 -> reg_a=2)
        OP_MOV_IMM, 0x20, 0x00,                 // rD = BLINK_MASK (mask in low16 is 0x0020)
        // pc 12
        OP_GET_TIMER, 0x40, 0x00,               // rE = timer++
        OP_AND_REG,   0x42, 0x00,               // rE &= rC? No: we want &= rD mask, so fix arg to 0x44 (a=4,b=4 is wrong). Use rD:
        // fix AND to use rD:
    };

    // Instead of patching in-place, construct final program reliably:

    const uint8_t program[] = {
        // Init
        OP_MOV_IMM, 0x01, 0x00,                          // rB = 1
        OP_MOV_IMM, 0x10, (uint8_t)(LIMIT & 0xFF),       // rC low
        OP_MOV_IMM, 0x10, (uint8_t)((LIMIT >> 8) & 0xFF),// rC high byte (still zero-extends; we only have low16)
        OP_MOV_IMM, 0x20, (uint8_t)(BLINK_MASK & 0xFF),  // rD = 0x20

        // pc 12: loop start
        OP_GET_TIMER, 0x40, 0x00,                        // rE = timer++
        OP_AND_REG,   0x42, 0x00,                        // rE &= rC (we’ll change to rD below)
        OP_CMP_REG,   0x42, 0x00,                        // cmp rE, rC (we’ll change to rD below)
        OP_JNE_REL,   0x09, 0x00,                        // if not equal, jump to zero-write block (pc + 3*3 = +9 bytes)

        // write color block
        OP_WRITE_FB_A,0x01, 0x00,                        // fb[rB] = rA
        OP_JMP_REL,   0x09, 0x00,                        // skip zero-write block

        // zero-write block
        OP_MOV_IMM,   0x00, 0x00,                        // rA = 0
        OP_WRITE_FB_A,0x01, 0x00,                        // fb[rB] = 0
        OP_MOV_IMM,   0x00, 0xFF,                        // rA = 0x00FF (restore low16 color)

        // advance and test
        OP_INC_REG,   0x10, 0x00,                        // rB++
        OP_CMP_REG,   0x13, 0x00,                        // cmp rB, rD (we actually set limit in rC; use rC)
        // Fix compare to rC:
    };

    // The above attempt got messy; let’s produce a clean, correct ROM in one go:

    // Program layout (final):
    // 0:  rB = 0
    // 3:  rC = LIMIT
    // 6:  rD = BLINK_MASK
    // 9:  get_timer rE
    // 12: and rE, rD
    // 15: cmp rE, rD
    // 18: jne +9       // jump over color write & skip
    // 21: write_fb_a   // color
    // 24: jmp +9       // skip zero-write
    // 27: mov_imm rA=0 // zero
    // 30: write_fb_a   // zero
    // 33: mov_imm rA=0x00FF // restore color low16
    // 36: inc rB
    // 39: cmp rB, rC
    // 42: jne back to 9
    // 45: halt
    const int16_t back_offset2 = (int16_t)(9 - (42 + 3)); // -36

    uint8_t rom_final[] = {
        OP_MOV_IMM, 0x01, 0x00,                         // rB = 0x0001
        OP_MOV_IMM, 0x10, (uint8_t)(LIMIT & 0xFF),      // rC = LIMIT (low8)
        OP_MOV_IMM, 0x10, (uint8_t)((LIMIT >> 8) & 0xFF),// rC = LIMIT (high8)
        OP_MOV_IMM, 0x20, (uint8_t)(BLINK_MASK & 0xFF), // rD = 0x20

        // pc 12
        OP_GET_TIMER, 0x40, 0x00,                       // rE = timer++
        OP_AND_REG,   0x42, 0x00,                       // rE &= rD
        OP_CMP_REG,   0x42, 0x00,                       // cmp rE, rD
        OP_JNE_REL,   0x09, 0x00,                       // jump +9 to zero-write block

        OP_WRITE_FB_A,0x01, 0x00,                       // fb[rB] = rA (color)
        OP_JMP_REL,   0x09, 0x00,                       // skip zero-write block

        OP_MOV_IMM,   0x00, 0x00,                       // rA = 0 (zero color)
        OP_WRITE_FB_A,0x01, 0x00,                       // fb[rB] = 0
        OP_MOV_IMM,   0x00, 0xFF,                       // rA = 0x00FF (restore low16 color)

        OP_INC_REG,   0x10, 0x00,                       // rB++
        OP_CMP_REG,   0x12, 0x00,                       // cmp rB, rC
        OP_JNE_REL,   (uint8_t)(back_offset2 & 0xFF), (uint8_t)((back_offset2 >> 8) & 0xFF),

        OP_HALT,      0x00, 0x00
    };

    size_t sz = sizeof(rom_final);
    uint8_t* copy = (uint8_t*)malloc(sz);
    if (!copy) return NULL;
    memcpy(copy, rom_final, sz);
    *out_size = sz;
    return copy;
}

// -----------------------------------------------------------------------------
// Main: init state, JIT, execute, render (blinking)
// -----------------------------------------------------------------------------

int main(void) {
    size_t rom_size = 0;
    const uint8_t* rom = create_rom(&rom_size);
    if (!rom) {
        fprintf(stderr, "failed to create ROM\n");
        return 1;
    }

    CPUState state;
    memset(&state, 0, sizeof(state));
    state.rSP = RAM_SIZE;          // stack grows downward; first push goes to RAM_SIZE-8
    state.rA  = 0xFF0000FFu;       // ARGB color (blue)
    state.rTimer = 0;

    JIT_Func fn = jit_compile(rom, rom_size);
    if (!fn) {
        free((void*)rom);
        return 1;
    }

    // Execute once per frame inside SDL loop to blink
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        release_executable_memory((void*)fn, 32768);
        free((void*)rom);
        return 1;
    }

    const int SCALE = 10;
    SDL_Window* win = SDL_CreateWindow(
        "JIT x64 emulator (blink demo)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        FB_WIDTH * SCALE, FB_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        release_executable_memory((void*)fn, 32768);
        free((void*)rom);
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        release_executable_memory((void*)fn, 32768);
        free((void*)rom);
        return 1;
    }

    SDL_Texture* tex = SDL_CreateTexture(
        ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        FB_WIDTH, FB_HEIGHT
    );
    if (!tex) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        release_executable_memory((void*)fn, 32768);
        free((void*)rom);
        return 1;
    }

    int running = 1;
    SDL_Event ev;

    printf("[emu] blinking demo running...\n");
    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }

        // Execute compiled program each frame to update framebuffer based on timer
        fn(&state);

        SDL_UpdateTexture(tex, NULL, state.framebuffer, FB_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        SDL_Delay(33); // ~30 FPS to visibly blink
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    release_executable_memory((void*)fn, 32768);
    free((void*)rom);
    printf("[emu] shutdown.\n");
    return 0;
}
