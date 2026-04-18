#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

// --- posix/linux memory allocation for executable code ---
#include <sys/mman.h>

// --- sdl2 header (used conditionally for graphics) ---
#ifdef __INTELLISENSE__
// dummy include for linter/intellisense
#define SDL_INIT_VIDEO 0
#define SDL_WINDOWPOS_CENTERED 0
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef union SDL_Event SDL_Event;
#else
#include <SDL2/SDL.h>
#endif

// --- constants and memory layout (64-bit architecture) ---
#define FB_WIDTH 64
#define FB_HEIGHT 64
#define RAM_SIZE 4096 // 4k bytes of emulated ram/stack space

// define the emulated system state
// all registers are 64-bit, mapping naturally to x64.
typedef struct {
    // general purpose registers (16 x 64-bit)
    uint64_t rA;       // 0x00
    uint64_t rB;       // 0x08
    uint64_t rC;       // 0x10
    uint64_t rD;       // 0x18
    uint64_t rE;       // 0x20
    uint64_t rF;       // 0x28
    uint64_t rG;       // 0x30
    uint64_t rH;       // 0x38
    uint64_t rI;       // 0x40 (requires rex.b)
    uint64_t rJ;       // 0x48
    uint64_t rK;       // 0x50
    uint64_t rL;       // 0x58
    uint64_t rM;       // 0x60
    uint64_t rN;       // 0x68
    uint64_t rO;       // 0x70
    uint64_t rP;       // 0x78

    // system registers
    uint64_t rSP;      // stack pointer (offset 0x80) - points to the last pushed element
    uint64_t rFlags;   // flags register (offset 0x88) - bit 0: zero flag (zf)
    uint64_t rTimer;   // simple timer (offset 0x90) - increments on io access

    // framebuffer (64x64, argb)
    uint32_t framebuffer[FB_WIDTH * FB_HEIGHT]; // offset 0x98 (16384 bytes, 0x4000)

    // emulated ram/stack memory (starts immediately after framebuffer)
    uint8_t ram[RAM_SIZE]; // offset 0x98 + 0x4000 = 0x4098 (16536)

    int pc; // program counter for the rom (only used during jit compilation)
} CPUState;

// memory access macros (offsets from cpustate start)
#define RA_OFF 0x00
#define RB_OFF 0x08
#define RC_OFF 0x10
#define RD_OFF 0x18
#define RE_OFF 0x20
#define RF_OFF 0x28
#define RG_OFF 0x30
#define RH_OFF 0x38
#define RI_OFF 0x40
#define RJ_OFF 0x48
#define RK_OFF 0x50
#define RL_OFF 0x58
#define RM_OFF 0x60
#define RN_OFF 0x68
#define RO_OFF 0x70
#define RP_OFF 0x78
#define RSP_OFF 0x80
#define RFLAGS_OFF 0x88
#define RTIMER_OFF 0x90
#define FB_OFF 0x98 // 0x4000 bytes
#define RAM_OFF (FB_OFF + (FB_WIDTH * FB_HEIGHT * sizeof(uint32_t))) // 0x4098

// jit helper structure for jump fixup
typedef struct {
    int rom_pc;          // rom instruction address (multiple of 3)
    uint8_t* native_addr; // native code address in the buffer
    int instruction_len; // length of the native instruction
} JumpFixup;

// define the jit target function signature: takes a pointer to the state struct
typedef void (*JIT_Func)(CPUState* state);

// --- emulated virtual instruction set (64-bit, 3-byte instruction format) ---
// instruction format: opcode (1 byte) + arguments (2 bytes: reg_src/dest, immediate/offset)

// general purpose opcodes (over 30 instructions now)
#define OP_HALT 0x00        // stop execution
#define OP_MOV_IMM 0x01     // reg[a] = imm_16
#define OP_MOV_REG 0x02     // reg[a] = reg[b]
#define OP_ADD_REG 0x03     // reg[a] += reg[b]
#define OP_SUB_REG 0x04     // reg[a] -= reg[b]

#define OP_LOAD_MEM 0x05    // reg[a] = ram[reg[b] + imm_16] (load 64-bit)
#define OP_STORE_MEM 0x06   // ram[reg[b] + imm_16] = reg[a] (store 64-bit)
#define OP_WRITE_FB_A 0x07  // framebuffer[reg[c]] = reg[a] (pixel write)
#define OP_LOAD_IMM32 0x08  // reg[a] = imm_32 (constant in arg)

// alu/logic
#define OP_AND_REG 0x10     // reg[a] &= reg[b]
#define OP_OR_REG 0x11      // reg[a] |= reg[b]
#define OP_XOR_REG 0x12     // reg[a] ^= reg[b]
#define OP_NOT_REG 0x13     // reg[a] = ~reg[a]
#define OP_SHL_REG 0x14     // reg[a] <<= reg[b] (shift left by reg[b])
#define OP_SHR_REG 0x15     // reg[a] >>= reg[b] (shift right by reg[b])
#define OP_INC_REG 0x16     // reg[a]++
#define OP_DEC_REG 0x17     // reg[a]--

// control flow
#define OP_CMP_REG 0x20     // compare reg[a] and reg[b], set rFlags.zf
#define OP_JMP_REL 0x21     // pc += signed_imm_16
#define OP_JNE_REL 0x22     // pc += signed_imm_16 if not equal (zf=0)
#define OP_JE_REL 0x23      // pc += signed_imm_16 if equal (zf=1)
#define OP_JGT_REL 0x24     // pc += signed_imm_16 if greater (signed check)

// stack and function calls (use rsp)
#define OP_PUSH_REG 0x30    // push reg[a] onto stack (decrement rsp by 8)
#define OP_POP_REG 0x31     // pop from stack into reg[a] (increment rsp by 8)
#define OP_CALL 0x32        // push pc+3, then jmp_rel (uses stack for return addr)
#define OP_RET 0x33         // pop pc, then jmp_abs (pop return addr from stack)

// hardware i/o
#define OP_GET_TIMER 0x40   // reg[a] = rTimer; rTimer++;
#define OP_SET_IRQ_MASK 0x41// rFlags.irq_mask = reg[a] (not fully implemented in jit, for future)

// --- x64 register mapping (rdi is always the state pointer) ---
// rax, rcx, rdx, rbx are used as scratch registers (r8, r9, r10, r11 could also be used)
#define SCRATCH_A 0 // rax
#define SCRATCH_C 1 // rcx
#define SCRATCH_D 2 // rdx
#define SCRATCH_B 3 // rbx

// rdi is 7

/**
 * @brief retrieves the offset of an emulated register from the CPUState struct.
 */
static uint64_t get_reg_offset(int reg_idx) {
    switch (reg_idx) {
        case 0: return RA_OFF; case 1: return RB_OFF; case 2: return RC_OFF; case 3: return RD_OFF;
        case 4: return RE_OFF; case 5: return RF_OFF; case 6: return RG_OFF; case 7: return RH_OFF;
        case 8: return RI_OFF; case 9: return RJ_OFF; case 10: return RK_OFF; case 11: return RL_OFF;
        case 12: return RM_OFF; case 13: return RN_OFF; case 14: return RO_OFF; case 15: return RP_OFF;
        default: return 0xFFFFFFFFFFFFFFFFULL; // error
    }
}

/**
 * @brief emits the modr/m byte structure relative to rdi (state pointer) for 64-bit access.
 * @param code_ptr pointer to the current position in the native code buffer.
 * @param offset the offset macro (e.g., RA_OFF, RAM_OFF).
 * @param x64_reg the x64 register index (0-7, or 8-15 via REX.B) to use in the reg field.
 */
static void emit_modrm_for_offset_64(uint8_t** code_ptr, uint64_t offset, int x64_reg) {
    // 64-bit addressing is always disp32 encoding relative to RDI (7)
    // mod=10, r/m=111 (rdi) -> 0x87
    // reg field contains the register to use (e.g., 0 for rax, 1 for rcx)

    // the modr/m byte is: (mod << 6) | (x64_reg << 3) | (r/m)
    // r/m = 0x07 (rdi, plus sib byte 0x24 required for 0x87 if r/m=4)
    // we use mod=10 (disp32) and r/m=7 (rdi) -> 0x87, plus the sib byte 0x24

    // mov (0x8b or 0x89) or alu (0x01/0x03/etc.)

    // since we are using rdi as base, we can use disp32.
    // modr/m: mod=10 (disp32), reg=x64_reg, r/m=7 (rdi)
    **code_ptr = (0x80) | (x64_reg << 3) | 0x07;
    (*code_ptr)++;

    // disp32
    *((uint32_t*)*code_ptr) = (uint32_t)offset;
    *code_ptr += 4;
}

// --- x64 code generation helpers (64-bit operations) ---

/**
 * @brief generates x64 code to load a 64-bit emulated register into a scratch register.
 * @param reg_idx emulated register index (0-15).
 * @param scratch_reg_idx x64 scratch register index (e.g., 0 for rax).
 */
static void emit_load_reg_64(uint8_t** code_ptr, int reg_idx, int scratch_reg_idx) {
    uint64_t offset = get_reg_offset(reg_idx);

    // REX.W (0x48) prefix for 64-bit operation
    **code_ptr = 0x48;
    // REX.R: 1 if scratch_reg_idx >= 8.
    if (scratch_reg_idx >= 8) **code_ptr |= 0x44;

    // REX.B: 1 if R/M (rdi=7) is >= 8, not applicable here.
    (*code_ptr)++;

    // mov r64, [rdi + disp32] (8b)
    **code_ptr = 0x8B;
    (*code_ptr)++;

    // modr/m + disp32
    emit_modrm_for_offset_64(code_ptr, offset, scratch_reg_idx % 8);
}

/**
 * @brief generates x64 code to store a 64-bit scratch register into an emulated register.
 * @param reg_idx emulated register index (0-15).
 * @param scratch_reg_idx x64 scratch register index (e.g., 0 for rax).
 */
static void emit_store_reg_64(uint8_t** code_ptr, int reg_idx, int scratch_reg_idx) {
    uint64_t offset = get_reg_offset(reg_idx);

    // REX.W (0x48) prefix for 64-bit operation
    **code_ptr = 0x48;
    // REX.R: 1 if scratch_reg_idx >= 8.
    if (scratch_reg_idx >= 8) **code_ptr |= 0x44;

    (*code_ptr)++;

    // mov [rdi + disp32], r64 (89)
    **code_ptr = 0x89;
    (*code_ptr)++;

    // modr/m + disp32
    emit_modrm_for_offset_64(code_ptr, offset, scratch_reg_idx % 8);
}

/**
 * @brief generates x64 code to perform an alu operation (op reg_a, reg_b).
 * @param alu_opcode the x64 alu opcode (e.g., 0x01 for add, 0x29 for sub).
 * @param reg_a_idx emulated destination register index.
 * @param reg_b_idx emulated source register index.
 * @param comment_op textual representation of the operation for logging.
 */
static void emit_alu_op_64(uint8_t** code_ptr, uint8_t alu_opcode, int reg_a_idx, int reg_b_idx, const char* comment_op) {
    // goal: reg[a] op= reg[b]

    // 1. load reg[a] into rax (scratch_a)
    emit_load_reg_64(code_ptr, reg_a_idx, SCRATCH_A);

    // 2. load reg[b] into rcx (scratch_c)
    emit_load_reg_64(code_ptr, reg_b_idx, SCRATCH_C);

    // 3. alu op rax, rcx (rax op= rcx)
    // REX.W (0x48) prefix
    *code_ptr++ = 0x48;
    // alu_opcode (e.g., 0x01 for add r/m64, r64)
    *code_ptr++ = alu_opcode;
    // modr/m: mod=11 (register-to-register), reg=rcx (1), r/m=rax (0) -> 0xC8
    *code_ptr++ = 0xC8;

    // 4. store rax back to reg[a]
    emit_store_reg_64(code_ptr, reg_a_idx, SCRATCH_A);

    printf("  -> x64: r%d %s= r%d\n", reg_a_idx, comment_op, reg_b_idx);
}

/**
 * @brief allocates executable memory using mmap.
 */
static uint8_t* allocate_executable_memory(size_t size) {
    size_t page_size = (size_t)sysconf(_SC_PAGE_SIZE);
    // align size to page size
    size = (size + page_size - 1) & ~(page_size - 1);

    uint8_t* code_buffer = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (code_buffer == MAP_FAILED) {
        perror("fehler bei der zuweisung von ausführbarem speicher (mmap)");
        return NULL;
    }

    printf("[jit] %zu bytes ausführbarer speicher bei %p zugewiesen\n", size, (void*)code_buffer);
    return code_buffer;
}

/**
 * @brief handles program exit and cleanup.
 */
static void handle_exit(uint8_t* code_buffer, size_t max_code_size, const char* reason) {
    if (code_buffer != (uint8_t*)MAP_FAILED) {
        if (munmap((void*)code_buffer, max_code_size) == -1) {
            perror("fehler beim freigeben des ausführbaren speichers (munmap)");
        }
    }
    fprintf(stderr, "[emu] programm beendet: %s\n", reason);
    exit(1);
}

// --- jit compiler core (two passes) ---

static JIT_Func jit_compile(const uint8_t* rom_data, size_t rom_size, CPUState* state_ptr) {
    // max code size required for 1024 rom instructions (1024 * ~30 bytes avg)
    const size_t max_code_size = 32768;
    uint8_t* code_buffer = allocate_executable_memory(max_code_size);
    if (!code_buffer) return NULL;

    uint8_t* code_ptr = code_buffer;
    int rom_pc = 0;

    // data structures for the two passes
    const int MAX_ROM_INSTRUCTIONS = 1024;
    size_t pc_to_addr[MAX_ROM_INSTRUCTIONS];
    JumpFixup jump_locations[MAX_ROM_INSTRUCTIONS];
    int jump_count = 0;

    printf("[jit] starte kompilierung in zwei durchgängen...\n");

    // --- pass 1: translate and record jump placeholders ---
    while (rom_pc < rom_size) {
        int inst_idx = rom_pc / 3;
        if (inst_idx >= MAX_ROM_INSTRUCTIONS) {
            handle_exit(code_buffer, max_code_size, "rom-größe überschreitet max-instruktionslimit");
        }

        // record the start address of the native code for the current rom pc
        pc_to_addr[inst_idx] = code_ptr - code_buffer;

        uint8_t opcode = rom_data[rom_pc];
        int current_rom_pc = rom_pc;
        rom_pc += 1;

        uint16_t argument = 0;
        if (opcode != OP_HALT && opcode != OP_RET && opcode != OP_WRITE_FB_A) {
            argument = rom_data[rom_pc] | (rom_data[rom_pc + 1] << 8);
            rom_pc += 2;
        } else if (opcode != OP_HALT && opcode != OP_RET) {
            // skip 2 bytes for 0-arg ops, but these ops typically still use the 2 bytes for reg indices/immediate
            rom_pc += 2;
        }

        int reg_a_idx = (argument & 0x000F);      // first 4 bits
        int reg_b_idx = (argument & 0x00F0) >> 4; // next 4 bits
        int16_t signed_imm_16 = (int16_t)argument;

        uint64_t reg_a_offset = get_reg_offset(reg_a_idx);
        uint64_t reg_b_offset = get_reg_offset(reg_b_idx);

        printf("  pc 0x%04X (op 0x%02X): ", current_rom_pc, opcode);

        switch (opcode) {
            case OP_HALT: {
                // x64: ret
                *code_ptr++ = 0xC3;
                printf("halt -> x64: ret\n");
                goto pass1_done; // exit compilation loop
            }

            case OP_MOV_IMM: { // reg[a] = imm_16
                // x64: mov r64, imm64
                // REX.W (0x48) + mov rax, imm64 (0xb8+r)
                *code_ptr++ = 0x48;
                *code_ptr++ = 0xC7; // c7 (mov r/m32, imm32)
                emit_modrm_for_offset_64(&code_ptr, reg_a_offset, SCRATCH_A);
                *((uint32_t*)code_ptr) = (uint32_t)argument; // use 32-bit immediate for 16-bit source
                code_ptr += 4;
                printf("mov_imm r%d, 0x%04X\n", reg_a_idx, argument);
                break;
            }

            case OP_MOV_REG: { // reg[a] = reg[b]
                emit_load_reg_64(&code_ptr, reg_b_idx, SCRATCH_A);
                emit_store_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);
                printf("mov_reg r%d, r%d\n", reg_a_idx, reg_b_idx);
                break;
            }

            // --- alu operations ---
            case OP_ADD_REG: emit_alu_op_64(&code_ptr, 0x01, reg_a_idx, reg_b_idx, "+"); break;
            case OP_SUB_REG: emit_alu_op_64(&code_ptr, 0x29, reg_a_idx, reg_b_idx, "-"); break;
            case OP_AND_REG: emit_alu_op_64(&code_ptr, 0x21, reg_a_idx, reg_b_idx, "&"); break;
            case OP_OR_REG: emit_alu_op_64(&code_ptr, 0x09, reg_a_idx, reg_b_idx, "|"); break;
            case OP_XOR_REG: emit_alu_op_64(&code_ptr, 0x31, reg_a_idx, reg_b_idx, "^"); break;

            case OP_NOT_REG: { // reg[a] = ~reg[a]
                // 1. load reg[a] into rax
                emit_load_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);
                // 2. not rax (0xf7 /2)
                *code_ptr++ = 0x48; *code_ptr++ = 0xF7; *code_ptr++ = 0xD0 | (2 << 3); // F7 /2 (NOT) mod=11, reg=2, r/m=0
                // 3. store rax back to reg[a]
                emit_store_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);
                printf("not_reg r%d\n", reg_a_idx);
                break;
            }

            case OP_INC_REG: { // reg[a]++
                // x64: inc qword ptr [rdi + reg_a_offset] (0xFF /0)
                *code_ptr++ = 0x48; // rex.w
                *code_ptr++ = 0xFF;
                emit_modrm_for_offset_64(&code_ptr, reg_a_offset, 0); // /0 is inc
                printf("inc_reg r%d\n", reg_a_idx);
                break;
            }

            case OP_DEC_REG: { // reg[a]--
                // x64: dec qword ptr [rdi + reg_a_offset] (0xFF /1)
                *code_ptr++ = 0x48; // rex.w
                *code_ptr++ = 0xFF;
                emit_modrm_for_offset_64(&code_ptr, reg_a_offset, 1); // /1 is dec
                printf("dec_reg r%d\n", reg_a_idx);
                break;
            }

            case OP_SHL_REG:
            case OP_SHR_REG: { // reg[a] <<= reg[b] or reg[a] >>= reg[b]
                // 1. load reg[a] into rax (the operand)
                emit_load_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);
                // 2. load reg[b] into rcx (the shift amount)
                emit_load_reg_64(&code_ptr, reg_b_idx, SCRATCH_C);

                // 3. shl/shr rax, cl (0xd3 /4 or /5)
                // REX.W (0x48) prefix
                *code_ptr++ = 0x48;
                *code_ptr++ = 0xD3; // shift group
                // modr/m: mod=11, reg=4(shl)/5(shr), r/m=rax(0) -> 0xE0 or 0xE8
                *code_ptr++ = (opcode == OP_SHL_REG) ? 0xE0 | (4 << 3) | 0x00 : 0xE0 | (5 << 3) | 0x00;

                // 4. store rax back to reg[a]
                emit_store_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);
                printf("%s_reg r%d, r%d\n", (opcode == OP_SHL_REG) ? "shl" : "shr", reg_a_idx, reg_b_idx);
                break;
            }

            // --- stack operations (8-byte values) ---
            case OP_PUSH_REG: { // push reg[a]
                // 1. load reg[a] into rax
                emit_load_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);

                // 2. load rsp into rcx
                emit_load_reg_64(&code_ptr, 0x80 / 8, SCRATCH_C); // index 16 is rsp (0x80)

                // 3. sub rcx, 0x08 (rcx = rsp - 8)
                *code_ptr++ = 0x48; *code_ptr++ = 0x83; *code_ptr++ = 0xE9; *code_ptr++ = 0x08;

                // 4. mov [rdi + ram_off + rcx], rax (store rax at ram[rsp-8])
                *code_ptr++ = 0x48; *code_ptr++ = 0x89; *code_ptr++ = 0x8C; *code_ptr++ = 0x0F; // sib: scale=1, index=rcx, base=rdi
                *((uint32_t*)code_ptr) = RAM_OFF; code_ptr += 4;

                // 5. store rcx back to rsp
                emit_store_reg_64(&code_ptr, 0x80 / 8, SCRATCH_C);

                printf("push_reg r%d\n", reg_a_idx);
                break;
            }

            case OP_POP_REG: { // pop reg[a]
                // 1. load rsp into rcx
                emit_load_reg_64(&code_ptr, 0x80 / 8, SCRATCH_C);

                // 2. mov rax, [rdi + ram_off + rcx] (rax = ram[rsp])
                *code_ptr++ = 0x48; *code_ptr++ = 0x8B; *code_ptr++ = 0x8C; *code_ptr++ = 0x0F; // sib: scale=1, index=rcx, base=rdi
                *((uint32_t*)code_ptr) = RAM_OFF; code_ptr += 4;

                // 3. add rcx, 0x08 (rcx = rsp + 8)
                *code_ptr++ = 0x48; *code_ptr++ = 0x83; *code_ptr++ = 0xC1; *code_ptr++ = 0x08;

                // 4. store rax back to reg[a]
                emit_store_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);

                // 5. store rcx back to rsp
                emit_store_reg_64(&code_ptr, 0x80 / 8, SCRATCH_C);

                printf("pop_reg r%d\n", reg_a_idx);
                break;
            }

            case OP_CALL: // push pc+3, then jmp_rel
            case OP_RET: // pop pc, then jmp_abs
                // implementation simplified: call is just a jump, ret is halt (complex stack/pc manipulation is hard in 1-pass)

            case OP_JMP_REL:
            case OP_JNE_REL:
            case OP_JE_REL:
            case OP_JGT_REL: {
                int target_rom_pc = current_rom_pc + 3 + signed_imm_16;

                uint8_t native_opcode = 0xE9; // jmp rel32
                int instruction_length = 5; // jmp rel32 is 5 bytes

                if (opcode != OP_JMP_REL) {
                    instruction_length = 6; // conditional jumps are 6 bytes (0x0F 0x8X)
                    *code_ptr++ = 0x0F; // 0x0F prefix
                    if (opcode == OP_JNE_REL) native_opcode = 0x85; // jne rel32
                    else if (opcode == OP_JE_REL) native_opcode = 0x84; // je rel32
                    else if (opcode == OP_JGT_REL) native_opcode = 0x8F; // jg rel32

                    // before the jump, we need a compare op
                    if (opcode != OP_RET) {
                         // 1. mov rax, [rdi+rflags_off]
                        *code_ptr++ = 0x48; *code_ptr++ = 0x8B; *code_ptr++ = 0x47; *code_ptr++ = RFLAGS_OFF;
                        // 2. test rax, rax (test zero flag) -> sets x64 flags
                        *code_ptr++ = 0x48; *code_ptr++ = 0x85; *code_ptr++ = 0xC0; // test rax, rax
                        // instruction_length += 6; // if compare is added
                    }
                }

                // emit the jump instruction
                *code_ptr++ = native_opcode;

                // placeholder for 4-byte displacement (rel32)
                uint8_t* disp_ptr = code_ptr;
                code_ptr += 4;

                // record jump location for fixup
                jump_locations[jump_count].rom_pc = target_rom_pc;
                jump_locations[jump_count].native_addr = disp_ptr;
                jump_locations[jump_count].instruction_len = instruction_length;
                jump_count++;

                printf("jump -> x64: placeholder for jump to rom pc 0x%X\n", target_rom_pc);
                break;
            }

            case OP_CMP_REG: { // compare reg[a] and reg[b], set rFlags.zf
                // 1. load reg[a] into rax
                emit_load_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);
                // 2. cmp rax, [rdi + reg_b_offset] (compare rax with reg[b]) -> sets x64 eflags
                *code_ptr++ = 0x48; *code_ptr++ = 0x3B;
                emit_modrm_for_offset_64(&code_ptr, reg_b_offset, SCRATCH_A);

                // 3. setz al (0x0F 0x94 0xC0) (al = zf)
                *code_ptr++ = 0x0F; *code_ptr++ = 0x94; *code_ptr++ = 0xC0;

                // 4. mov byte ptr [rdi + rflags_off], al (write al to rflags lsb)
                *code_ptr++ = 0x88; *code_ptr++ = 0x47; *code_ptr++ = RFLAGS_OFF;

                printf("cmp_reg r%d, r%d -> x64: cmp and set rFlags.zf\n", reg_a_idx, reg_b_idx);
                break;
            }

            case OP_WRITE_FB_A: { // framebuffer[rC] = rA
                // 1. load rc index into ecx (low 32-bit needed for array index)
                *code_ptr++ = 0x8B; *code_ptr++ = 0x4F; *code_ptr++ = RC_OFF + 4; // load high 4 bytes of rc (for 64-bit value)

                // 2. load ra color into eax (32-bit color)
                *code_ptr++ = 0x8B; *code_ptr++ = 0x07;

                // 3. mov dword ptr [rdi + fb_off + rcx*4], eax (sib addressing)
                *code_ptr++ = 0x89; *code_ptr++ = 0x8C; *code_ptr++ = 0x8F; // sib: scale=4, index=ecx, base=rdi
                *((uint32_t*)code_ptr) = FB_OFF;
                code_ptr += 4;

                printf("write_fb_a -> x64: framebuffer[rC] = rA (32-bit color)\n");
                break;
            }

            case OP_GET_TIMER: { // reg[a] = rTimer; rTimer++;
                // 1. load rTimer into rax
                *code_ptr++ = 0x48; *code_ptr++ = 0x8B; *code_ptr++ = 0x47; *code_ptr++ = RTIMER_OFF;

                // 2. store rax back to reg[a]
                emit_store_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);

                // 3. inc qword ptr [rdi + rtimer_off] (0xFF /0)
                *code_ptr++ = 0x48; *code_ptr++ = 0xFF;
                emit_modrm_for_offset_64(&code_ptr, RTIMER_OFF, 0);

                printf("get_timer r%d\n", reg_a_idx);
                break;
            }

            // --- complex memory access (ram[reg[b] + imm_16] = reg[a]) ---
            case OP_STORE_MEM: {
                // 1. load reg[b] (base address) into rcx
                emit_load_reg_64(&code_ptr, reg_b_idx, SCRATCH_C);
                // 2. load reg[a] (data) into rax
                emit_load_reg_64(&code_ptr, reg_a_idx, SCRATCH_A);

                // 3. mov [rdi + ram_off + rcx + imm_16], rax
                // REX.W (0x48) + mov [r/m], r64 (0x89)
                *code_ptr++ = 0x48; *code_ptr++ = 0x89;
                // modr/m: mod=01 (disp8), reg=rax(0), r/m=rcx(1) + sib required
                *code_ptr++ = 0x84; // modr/m with sib
                *code_ptr++ = 0x0F; // sib: scale=1, index=rcx, base=rdi

                // disp32 = RAM_OFF + imm_16
                uint32_t combined_disp = (uint32_t)(RAM_OFF + argument);
                *((uint32_t*)code_ptr) = combined_disp;
                code_ptr += 4;

                printf("store_mem r%d, [r%d+0x%X]\n", reg_a_idx, reg_b_idx, argument);
                break;
            }

            default:
                handle_exit(code_buffer, max_code_size, "unbekannter opcode");
        }

        // check for buffer overflow protection (allow a margin)
        if ((size_t)(code_ptr - code_buffer) > max_code_size - 100) {
            handle_exit(code_buffer, max_code_size, "jit-pufferüberlauf");
        }
    }

pass1_done:
    // --- pass 2: fixup jump targets ---
    printf("[jit] führe jump-fixup durch (anzahl: %d)...\n", jump_count);

    for (int i = 0; i < jump_count; i++) {
        JumpFixup fixup = jump_locations[i];

        int target_rom_pc = fixup.rom_pc;
        int target_inst_idx = target_rom_pc / 3;

        if (target_inst_idx >= MAX_ROM_INSTRUCTIONS) {
            handle_exit(code_buffer, max_code_size, "jump-ziel außerhalb des rom-bereichs");
        }

        size_t target_native_addr = pc_to_addr[target_inst_idx];

        // current instruction start in native code
        size_t current_instruction_start = fixup.native_addr - code_buffer - (fixup.instruction_len - 4);

        // displacement: target_address - (current_address + instruction_length)
        int32_t displacement = (int32_t)target_native_addr - (int32_t)(current_instruction_start + fixup.instruction_len);

        // write the calculated displacement back into the code buffer
        *((int32_t*)fixup.native_addr) = displacement;

        printf("  fix: rom pc 0x%X (native 0x%zX) -> ziel 0x%zX, disp %d\n",
               (target_inst_idx * 3), current_instruction_start, target_native_addr, displacement);
    }

    printf("[jit] kompilierung abgeschlossen. gesamtgröße: %zu bytes.\n", code_ptr - code_buffer);
    return (JIT_Func)code_buffer;
}

/**
 * @brief definiert das rom-programm (emulierte isa-bytes).
 * @return pointer auf die rom-daten.
 */
static const uint8_t* create_complex_rom(size_t* rom_size) {
    // dieses rom demonstriert 64-bit-arithmetik, bedingte sprünge und hardware-i/o.

    // code flow:
    // 0: op_mov_imm rA, 0x0001 (rA = 1)
    // 3: op_mov_imm rB, 0x0001 (rB = 1, counter)
    // 6: op_mov_imm rC, 0x0010 (rC = 16)
    // 9: op_mov_imm rD, 0x0020 (rD = 32, fb index limit)

    // --- loop_start (pc 12) ---
    // 12: op_get_timer rE, 0x0000 (rE = timer, timer++)
    // 15: op_and_reg rE, rC (rE = rE & 16, check if timer is multiple of 16)
    // 18: op_cmp_reg rE, rC (compare rE and rC, sets zf if rE==rC)
    // 21: op_jne_rel 0x0003 (jump to pc 27 if not equal)
    // 24: op_write_fb_a 0x0000 (framebuffer[rB] = rA, write pixel only on timer multiple)
    // 27: op_add_reg rA, rA (rA = rA + rA, double color)
    // 30: op_inc_reg rB (rB++)
    // 33: op_cmp_reg rB, rD (compare rB and rD)
    // 36: op_jne_rel 0xffec (jump to loop_start pc 12 if not equal)
    //                          jump_offset = 12 - (36 + 3) = 12 - 39 = -27 (0xffe5)
    // 39: op_halt 0x0000

    const int JUMP_OFFSET = 12 - 39; // -27 (0xffe5)

    const uint8_t COMPLEX_ROM[] = {
        // init
        OP_MOV_IMM, 0x00, 0x00, // 0: rA = 0 (color, idx 0)
        OP_MOV_IMM, 0x01, 0x00, // 3: rB = 1 (fb counter, idx 1)
        OP_MOV_IMM, 0x10, 0x00, // 6: rC = 16 (timer modulo check, idx 2)
        OP_MOV_IMM, 0x20, 0x00, // 9: rD = 32 (loop limit, idx 3)

        // loop_start (pc 12)
        OP_GET_TIMER, 0x40, 0x00,  // 12: rE = rTimer (idx 4)
        OP_AND_REG, 0x42, 0x00,    // 15: rE = rE & rC (idx 4, 2)
        OP_CMP_REG, 0x42, 0x00,    // 18: cmp rE, rC (idx 4, 2)
        // jump to next instruction (pc 27) if timer not multiple of 16
        OP_JNE_REL, 0x03, 0x00,    // 21: jne rel 3
        OP_WRITE_FB_A, 0x01, 0x00, // 24: framebuffer[rB] = rA (idx 0, 1)

        OP_ADD_REG, 0x00, 0x00,    // 27: rA = rA + rA (idx 0, 0)
        OP_INC_REG, 0x10, 0x00,    // 30: rB++ (idx 1)
        OP_CMP_REG, 0x13, 0x00,    // 33: cmp rB, rD (idx 1, 3)
        OP_JNE_REL, (uint8_t)(JUMP_OFFSET & 0xFF), (uint8_t)((JUMP_OFFSET >> 8) & 0xFF), // 36: jne rel -27

        // halt (pc 39)
        OP_HALT, 0x00, 0x00
    };

    *rom_size = sizeof(COMPLEX_ROM);
    // make a copy to ensure immutability and allow for future dynamic rom loading
    uint8_t* rom_copy = (uint8_t*)malloc(*rom_size);
    if (rom_copy) {
        memcpy(rom_copy, COMPLEX_ROM, *rom_size);
    }
    return rom_copy;
}

/**
 * @brief setzt sdl2 auf, führt den jit-code aus und zeichnet den framebuffer.
 */
int main(int argc, char* argv[]) {
    // --- 1. rom definition ---
    size_t rom_size = 0;
    const uint8_t* rom_data = create_complex_rom(&rom_size);
    if (!rom_data) {
        fprintf(stderr, "fehler beim erstellen des roms.\n");
        return 1;
    }

    // --- 2. initialize cpu state ---
    CPUState state = {0};
    // initialize rsp to point to the end of ram
    state.rSP = RAM_SIZE;
    // initialize a starting color (ARGB blue)
    state.rA = 0xFF0000FFULL;
    state.rTimer = 0;

    // --- 3. jit compile the rom ---
    JIT_Func jit_code = jit_compile(rom_data, rom_size, &state);

    if (jit_code == NULL) {
        free((void*)rom_data);
        return 1;
    }

    // --- 4. sdl initialization and rendering ---

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "sdl_init fehler: %s\n", SDL_GetError());
        munmap((void*)jit_code, 32768);
        free((void*)rom_data);
        return 1;
    }

    const int WIDTH = FB_WIDTH;
    const int HEIGHT = FB_HEIGHT;
    const int SCALE = 10;
    SDL_Window *window = SDL_CreateWindow("sophisticated jit x64 emulator (64-bit)",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "sdl_createwindow fehler: %s\n", SDL_GetError());
        goto cleanup_sdl;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "sdl_createrenderer fehler: %s\n", SDL_GetError());
        goto cleanup_sdl;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);

    // --- 5. execute the compiled code ---
    printf("[emu] führe jit-kompilierten code aus (rA start: 0x%lX)...\n", state.rA);
    jit_code(&state);
    printf("[emu] jit-ausführung abgeschlossen. rB (zähler): %lu\n", state.rB);

    // --- 6. main rendering loop ---
    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        // update texture with the final framebuffer state
        SDL_UpdateTexture(texture, NULL, state.framebuffer, WIDTH * sizeof(uint32_t));

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 fps
    }

    // --- 7. cleanup ---
cleanup_sdl:
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    munmap((void*)jit_code, 32768);
    free((void*)rom_data);

    printf("[emu] emulator erfolgreich heruntergefahren.\n");
    return 0;
}
