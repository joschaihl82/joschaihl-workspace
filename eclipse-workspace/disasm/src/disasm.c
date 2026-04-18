#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

// --- forward declarations ---
// required to fix 'implicit declaration' and 'conflicting types' errors

size_t load_file(const char* filename, uint8_t** buffer);
int32_t get_displacement(const uint8_t* code, size_t len);
uint64_t get_immediate(const uint8_t* code, size_t len);

// --- constants and macros ---

#define MAX_CODE_SIZE 0x20000 // 128kb max code buffer
#define MODRM_MOD(b) ((b >> 6) & 0x03)
#define MODRM_REG(b) ((b >> 3) & 0x07)
#define MODRM_RM(b)  (b & 0x07)
#define SIB_SCALE(b) ((b >> 6) & 0x03)
#define SIB_INDEX(b) ((b >> 3) & 0x07)
#define SIB_BASE(b)  (b & 0x07)
#define REX_W(b)     ((b >> 3) & 0x01)
#define REX_R(b)     ((b >> 2) & 0x01)
#define REX_X(b)     ((b >> 1) & 0x01)
#define REX_B(b)     (b & 0x01)

// --- data structures ---

typedef struct {
    uint8_t prefixes[4];
    uint8_t rex;
    size_t prefix_count;
    uint8_t opcode;
    uint8_t secondary_opcode; // for 0x0f instructions
    uint8_t modrm;
    uint8_t sib;
    int disp_len;
    int32_t displacement;
    int imm_len;
    uint64_t immediate;
    size_t instruction_len;
    int op_size; // 1, 2, 4, 8 (byte, word, dword, qword)
    int addr_size; // 4 or 8 (32-bit or 64-bit addressing)
    char mnemonic[32];
} instruction_t;

// --- global register names and tables ---

// 64-bit general purpose registers (gprs)
const char* reg_64[] = {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
                        "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
// 32-bit gprs
const char* reg_32[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
                        "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
// 16-bit gprs
const char* reg_16[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
                        "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
// 8-bit gprs (low byte)
const char* reg_8_low[] = {"al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil",
                           "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};

// 128-bit xmm registers (for sse/sse2)
const char* reg_xmm[] = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
                         "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"};

// opcode group 1 mnemonics (modr/m reg field selects operation)
const char* group1_mnemonics[] = {"add", "or", "adc", "sbb", "and", "sub", "xor", "cmp"};

// two-byte conditional jump mnemonics
const char* jcc_mnemonics[] = {"jo", "jno", "jb", "jnb", "jz", "jnz", "jbe", "jnbe",
                               "js", "jns", "jp", "jnp", "jl", "jnl", "jle", "jnle"};

// --- helper functions ---

/**
 * @brief prints the instruction bytes in hex format.
 */
void print_bytes(const uint8_t* code, size_t offset, size_t len) {
    printf("%08zx: ", offset);
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", code[i]);
    }
    // padding for alignment
    for (size_t i = len; i < 16; i++) {
        printf("   ");
    }
    printf(" ");
}

/**
 * @brief determines the appropriate size prefix string for memory operands.
 */
const char* get_size_prefix(int op_size) {
    switch (op_size) {
        case 1: return "byte ptr ";
        case 2: return "word ptr ";
        case 4: return "dword ptr ";
        case 8: return "qword ptr ";
        case 16: return "dqword ptr "; // 128-bit for xmm
        default: return "";
    }
}

/**
 * @brief gets the register name array based on size, rex prefix, and register type.
 */
const char** get_reg_array(int op_size, int reg_type) {
    // reg_type 0: gpr (default), 1: xmm
    if (reg_type == 1) return reg_xmm;

    switch (op_size) {
        case 1: return reg_8_low;
        case 2: return reg_16;
        case 4: return reg_32;
        case 8: // fallthrough
        default: return reg_64;
    }
}

/**
 * @brief decodes the modr/m and sib bytes to generate the operand string.
 */
void decode_operand(const uint8_t* code, const instruction_t* instr, int op_size, int reg_type, char* buffer) {
    uint8_t mod = MODRM_MOD(instr->modrm);
    uint8_t rm = MODRM_RM(instr->modrm) | (instr->rex ? REX_B(instr->rex) << 3 : 0);

    // choose the appropriate register array for the r/m field
    const char** rm_reg_list = get_reg_array(op_size, reg_type);

    if (mod == 3) { // register-to-register (mod = 11b)
        sprintf(buffer, "%s", rm_reg_list[rm]);
    } else { // memory addressing

        const char* size_prefix = get_size_prefix(op_size);

        if (rm == 4) { // sib byte present
            uint8_t sib = instr->sib;
            uint8_t base = SIB_BASE(sib) | (instr->rex ? REX_B(instr->rex) << 3 : 0);
            uint8_t index = SIB_INDEX(sib) | (instr->rex ? REX_X(instr->rex) << 3 : 0);
            uint8_t scale = SIB_SCALE(sib);

            // gprs are always used for addressing, regardless of op_size
            const char* base_reg = (base == 5 && mod == 0) ? "" : reg_64[base];
            const char* index_reg = (index == 4) ? "" : reg_64[index];

            char sib_str[128] = {0};

            // check for rip-relative addressing (rip+disp32) or [disp32]
            if (rm == 5 && mod == 0) {
                // this is the displacement-only case for sib-less or sib-present
                sprintf(sib_str, "[rip + 0x%x", instr->displacement);
            } else if (base == 5 && mod == 0) { // [disp32] only case
                 if (*index_reg) {
                    // base is replaced by disp32
                    sprintf(sib_str, "[0x%x + %s*%d", instr->displacement, index_reg, 1 << scale);
                } else {
                    sprintf(sib_str, "[0x%x", instr->displacement);
                }
            }
            // base + index*scale
            else if (*base_reg && *index_reg) {
                sprintf(sib_str, "[%s + %s*%d", base_reg, index_reg, 1 << scale);
            }
            // base only
            else if (*base_reg) {
                sprintf(sib_str, "[%s", base_reg);
            }
            // index*scale only
            else if (*index_reg) {
                sprintf(sib_str, "[%s*%d", index_reg, 1 << scale);
            }

            // finalize sib string with displacement (only if displacement is not already the base)
            if (instr->displacement != 0 && !(rm == 5 && mod == 0) && !(base == 5 && mod == 0)) {
                if (*base_reg || *index_reg) { // only add disp if base or index exists
                    if (instr->displacement > 0) {
                        sprintf(sib_str + strlen(sib_str), " + 0x%x", instr->displacement);
                    } else {
                        sprintf(sib_str + strlen(sib_str), " - 0x%x", (uint32_t)-instr->displacement);
                    }
                }
            }

            sprintf(buffer, "%s%s]", size_prefix, sib_str + 1); // remove the starting '['

        } else if (rm == 5 && mod == 0) { // ip-relative addressing (rip+disp32)
            // this handles the sib-less rm=5, mod=0 case
            sprintf(buffer, "%s[rip + 0x%x]", size_prefix, instr->displacement);
        } else { // direct base/index addressing (no sib)
            // general case: [reg + disp] or [reg]
            if (instr->displacement == 0) {
                 sprintf(buffer, "%s[%s]", size_prefix, reg_64[rm]);
            } else {
                if (instr->displacement > 0) {
                    sprintf(buffer, "%s[%s + 0x%x]", size_prefix, reg_64[rm], instr->displacement);
                } else {
                    sprintf(buffer, "%s[%s - 0x%x]", size_prefix, reg_64[rm], (uint32_t)-instr->displacement);
                }
            }
        }
    }
}


/**
 * @brief attempts to decode a single x64 instruction.
 * @return the length of the instruction on success, 0 on failure/unsupported
 */
size_t decode_instruction(const uint8_t* code, size_t offset, instruction_t* instr) {
    memset(instr, 0, sizeof(instruction_t));
    size_t pos = 0;

    // defaults
    instr->op_size = 4; // default to dword (32-bit) in 64-bit mode (unless rex.w is set)
    instr->addr_size = 8; // default to 64-bit addressing
    int has_f2 = 0;
    int has_f3 = 0;
    int has_66 = 0;
    int reg_type_op1 = 0; // 0=gpr, 1=xmm
    int reg_type_op2 = 0;

    // 1. decode prefixes (up to 4) and rex
    for (int i = 0; i < 4; i++) {
        uint8_t byte = code[pos];
        if (byte == 0x66) has_66 = 1; // operand size override
        if (byte == 0xf2) has_f2 = 1; // repne / sse scalar double prefix
        if (byte == 0xf3) has_f3 = 1; // rep / sse scalar single prefix

        if (byte == 0x66 || byte == 0xf0 || byte == 0xf2 || byte == 0xf3 || byte == 0x2e || byte == 0x36) {
            instr->prefixes[instr->prefix_count++] = byte;
            pos++;
        } else if (byte >= 0x40 && byte <= 0x4f) {
            instr->rex = byte;
            pos++;
            break; // rex must be the last prefix
        } else {
            break;
        }
    }

    // 1b. determine operand size based on prefixes
    if (instr->rex && REX_W(instr->rex)) {
        instr->op_size = 8; // qword (64-bit)
    } else if (has_66) {
        instr->op_size = 2; // word (16-bit)
    }
    // default 4 (32-bit) is kept otherwise


    // 2. decode opcode
    instr->opcode = code[pos++];

    // 3. decode instruction (the core logic)

    char op1_str[128] = "";
    char op2_str[128] = "";

    // pointer to the code block *after* the opcode(s)
    const uint8_t* post_opcode_ptr = code + pos;

    // two-byte opcodes (0x0f)
    if (instr->opcode == 0x0f) {
        instr->secondary_opcode = code[pos++];
        post_opcode_ptr = code + pos;

        // --- conditional jumps (jcc rel32, 0x0f 0x80 - 0x0f 0x8f) ---
        if (instr->secondary_opcode >= 0x80 && instr->secondary_opcode <= 0x8f) {
            if (!has_f2 && !has_f3 && !has_66) { // check for non-sse usage
                strcpy(instr->mnemonic, jcc_mnemonics[instr->secondary_opcode - 0x80]);

                instr->imm_len = 4; // rel32
                int32_t relative_offset = *(int32_t*)post_opcode_ptr;
                pos += instr->imm_len;

                size_t target_addr = offset + pos + relative_offset;
                sprintf(op1_str, "0x%zx", target_addr);
            }
        }

        // --- sse/sse2 instruction decoding (selective) ---
        else if (instr->secondary_opcode == 0x10) { // movupd/movss/movsd/movups
            reg_type_op1 = 1; reg_type_op2 = 1;
            instr->op_size = 16;

            if (has_f2) {
                strcpy(instr->mnemonic, "movsd"); instr->op_size = 8;
            } else if (has_f3) {
                strcpy(instr->mnemonic, "movss"); instr->op_size = 4;
            } else if (has_66) {
                strcpy(instr->mnemonic, "movupd");
            } else {
                strcpy(instr->mnemonic, "movups");
            }
        }
        else if (instr->secondary_opcode == 0x58) { // addpd/addps/addsd/addss
            reg_type_op1 = 1; reg_type_op2 = 1;
            instr->op_size = 16;

            if (has_f2) {
                strcpy(instr->mnemonic, "addsd"); instr->op_size = 8;
            } else if (has_f3) {
                strcpy(instr->mnemonic, "addss"); instr->op_size = 4;
            } else if (has_66) {
                strcpy(instr->mnemonic, "addpd");
            } else {
                strcpy(instr->mnemonic, "addps");
            }
        }

        // --- two-byte gpr instructions ---
        else if (instr->secondary_opcode == 0xae) { // xsave/fxsave (modr/m reg field selects)
            // if an sse instruction was matched, continue to modr/m decoding
            instr->modrm = code[pos++];
            uint8_t reg_field = MODRM_REG(instr->modrm);
            if (reg_field == 0) strcpy(instr->mnemonic, "fxsave");
            else if (reg_field == 1) strcpy(instr->mnemonic, "fxrstor");
            else if (reg_field == 2) strcpy(instr->mnemonic, "ldmxcsr");
            else if (reg_field == 3) strcpy(instr->mnemonic, "stmxcsr");
            else strcpy(instr->mnemonic, "0fae_grp_unk");
        }

        // if an instruction requires modr/m but didn't read it yet (e.g., movsd/addsd)
        if (instr->secondary_opcode == 0x10 || instr->secondary_opcode == 0x58 || instr->secondary_opcode == 0x11) {
             if (instr->modrm == 0) instr->modrm = code[pos++];
        }

    } // end of 0x0f opcodes

    // one-byte opcodes

    // --- general instructions with modr/m byte ---
    else if (instr->opcode >= 0x80 && instr->opcode <= 0x83) { // group 1: add/sub/cmp imm to r/m
        instr->modrm = code[pos++];
        uint8_t reg_field = MODRM_REG(instr->modrm);
        strcpy(instr->mnemonic, group1_mnemonics[reg_field]);

        // determine immediate length
        if (instr->opcode == 0x83) instr->imm_len = 1; // sign-extended byte
        else instr->imm_len = 4; // dword
    }
    else if (instr->opcode == 0x8b || instr->opcode == 0x89) { // mov r/m to r (0x8b) or r to r/m (0x89)
        strcpy(instr->mnemonic, "mov");
        instr->modrm = code[pos++];
    }
    else if (instr->opcode == 0xff) { // group 5: call/jmp/push/inc/dec near, far (modr/m reg field selects)
        instr->modrm = code[pos++];
        uint8_t reg_field = MODRM_REG(instr->modrm);
        if (reg_field == 2) strcpy(instr->mnemonic, "call");
        else if (reg_field == 4) strcpy(instr->mnemonic, "jmp");
        else if (reg_field == 6) strcpy(instr->mnemonic, "push");
        else strcpy(instr->mnemonic, "ff_grp5_unk");
    }

    // --- mov immediate to reg (0xb8 - 0xbf) ---
    else if (instr->opcode >= 0xb8 && instr->opcode <= 0xbf) {
        strcpy(instr->mnemonic, "mov");
        instr->imm_len = instr->op_size; // immediate size matches operand size

        uint8_t reg_field = (instr->opcode & 0x07) | (instr->rex ? REX_B(instr->rex) << 3 : 0);
        const char** reg_list = get_reg_array(instr->op_size, 0);
        sprintf(op1_str, "%s", reg_list[reg_field]);

        // read immediate here
        instr->immediate = get_immediate(post_opcode_ptr, instr->imm_len); // correct pointer usage
        pos += instr->imm_len;
        sprintf(op2_str, "0x%lx", instr->immediate); // fixed format specifier: %lx for uint64_t
    }

    // --- simple control flow instructions ---
    else if (instr->opcode == 0xe8) { // call rel32
        strcpy(instr->mnemonic, "call"); instr->imm_len = 4;
    } else if (instr->opcode == 0xe9) { // jmp rel32
        strcpy(instr->mnemonic, "jmp"); instr->imm_len = 4;
    } else if (instr->opcode == 0x74) { // jz rel8
        strcpy(instr->mnemonic, "jz"); instr->imm_len = 1;
    }

    // --- simple one-byte instructions ---
    else if (instr->opcode == 0xc3) {
        strcpy(instr->mnemonic, "ret");
    } else if (instr->opcode == 0x90) {
        strcpy(instr->mnemonic, "nop");
    }

    // 4. decode modr/m and address fields if present

    if (instr->modrm) {
        uint8_t mod = MODRM_MOD(instr->modrm);
        uint8_t rm = MODRM_RM(instr->modrm);

        // check for sib byte
        if (mod != 3 && rm == 4) {
            instr->sib = code[pos++];
        }

        // determine displacement length
        if (mod == 1) instr->disp_len = 1;
        else if (mod == 2 || (mod == 0 && rm == 5)) instr->disp_len = 4; // disp32

        // read displacement
        if (instr->disp_len > 0) {
            instr->displacement = get_displacement(code + pos, instr->disp_len);
            pos += instr->disp_len;
        }

        // read immediate (if not already handled, e.g., mov imm to reg)
        if (instr->imm_len > 0) {
            instr->immediate = get_immediate(code + pos, instr->imm_len);
            pos += instr->imm_len;
        }

        // --- operand string creation (based on opcode type) ---

        uint8_t reg_field = MODRM_REG(instr->modrm) | (instr->rex ? REX_R(instr->rex) << 3 : 0);

        // get reg names for operand 1 (from reg field) and operand 2 (from r/m field)
        const char** op1_reg_list = get_reg_array(instr->op_size, reg_type_op1);

        if (instr->opcode == 0x89) { // mov r to r/m
            sprintf(op1_str, "%s", op1_reg_list[reg_field]); // reg is op1
            decode_operand(code + pos, instr, instr->op_size, reg_type_op2, op2_str); // r/m is op2
        } else { // mov r/m to r (0x8b, group 1)
            decode_operand(code + pos, instr, instr->op_size, reg_type_op2, op2_str); // r/m is op2
            sprintf(op1_str, "%s", op1_reg_list[reg_field]); // reg is op1

            // swap for instructions like group 1 (imm to r/m)
            if (instr->opcode >= 0x80 && instr->opcode <= 0x83) {
                // op1 is r/m, op2 is immediate
                char temp[128];
                strcpy(temp, op1_str);
                strcpy(op1_str, op2_str);
                sprintf(op2_str, "0x%lx", instr->immediate);
            }
        }

        // sse instructions: op1 is reg, op2 is r/m
        if (instr->opcode == 0x0f && (instr->secondary_opcode == 0x10 || instr->secondary_opcode == 0x58 || instr->secondary_opcode == 0x11)) {
            // op1 is reg (xmm) and op2 is r/m (xmm/mem)
            sprintf(op1_str, "%s", op1_reg_list[reg_field]);
            decode_operand(code + pos, instr, instr->op_size, reg_type_op2, op2_str);
        }

    }
    // 5. read immediate for instructions without modr/m but with relative jumps
    else if (instr->imm_len > 0) {
        // immediate/displacement for non-modr/m instructions (e.g., call rel32, jz rel8)
        // this is only for control flow instructions that didn't read modr/m
        if (!(instr->opcode >= 0xb8 && instr->opcode <= 0xbf)) { // skip mov imm to reg
            instr->immediate = get_immediate(post_opcode_ptr, instr->imm_len);
            pos += instr->imm_len;

            // print target address for jump/call
            if (instr->opcode == 0xe8 || instr->opcode == 0xe9 || (instr->opcode >= 0x70 && instr->opcode <= 0x7f)) {
                int32_t relative_offset = (int32_t)instr->immediate;
                size_t target_addr = offset + pos + relative_offset;
                sprintf(op1_str, "0x%zx", target_addr);
            }
        }
    }

    instr->instruction_len = pos;

    // final checks for unsupported (if no mnemonic was found)
    if (instr->mnemonic[0] == '\0') {
        if (instr->opcode == 0x0f) {
            sprintf(instr->mnemonic, "unk_0f_%02x", instr->secondary_opcode);
            instr->instruction_len = 2;
        } else {
            strcpy(instr->mnemonic, "unknown");
            instr->instruction_len = 1; // minimum length
        }
    }


    // 6. print the result
    print_bytes(code, offset, instr->instruction_len);
    printf("%-8s %s%s%s", instr->mnemonic, op1_str,
           (op1_str[0] != '\0' && op2_str[0] != '\0') ? ", " : "", op2_str);

    // print memory address of target for jumps/calls
    if (instr->mnemonic[0] == 'j' || !strcmp(instr->mnemonic, "call")) {
        printf(" ; -> %s", op1_str);
    }
    printf("\n");

    return instr->instruction_len;
}

// --- concrete helper function definitions (moved here to resolve 'implicit declaration' and 'conflicting types') ---

/**
 * @brief extracts a displacement value (1, 2, or 4 bytes).
 */
int32_t get_displacement(const uint8_t* code, size_t len) {
    if (len == 1) return (int32_t)(int8_t)code[0];
    if (len == 2) return (int32_t)*(int16_t*)code;
    if (len == 4) return *(int32_t*)code;
    return 0;
}

/**
 * @brief extracts an immediate value (1, 2, 4, or 8 bytes).
 */
uint64_t get_immediate(const uint8_t* code, size_t len) {
    if (len == 1) return (uint64_t)code[0];
    if (len == 2) return (uint64_t)*(uint16_t*)code;
    if (len == 4) return (uint64_t)*(uint32_t*)code;
    if (len == 8) return *(uint64_t*)code;
    return 0;
}

/**
 * @brief loads the content of a file into a buffer.
 */
size_t load_file(const char* filename, uint8_t** buffer) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("error opening file");
        return 0;
    }

    // correct: use the standard uppercase macros for fseek
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size > MAX_CODE_SIZE) {
        fprintf(stderr, "warning: file size 0x%lx exceeds max code size 0x%x. truncating.\n", file_size, MAX_CODE_SIZE);
        file_size = MAX_CODE_SIZE;
    }

    *buffer = (uint8_t*)malloc(file_size);
    if (!*buffer) {
        perror("error allocating memory");
        fclose(fp);
        return 0;
    }

    size_t bytes_read = fread(*buffer, 1, file_size, fp);
    fclose(fp);

    if (bytes_read != file_size) {
        fprintf(stderr, "error: read %zu bytes, expected %lu bytes.\n", bytes_read, file_size);
        free(*buffer);
        *buffer = NULL;
        return 0;
    }

    return bytes_read;
}


// --- main function ---

int main(int argc, char* argv[]) {
    printf("x64 simplified disassembler (selective functionality)\n\n");

    if (argc != 2) {
        fprintf(stderr, "usage: %s <binary_file>\n", argv[0]);
        return 1;
    }

    uint8_t* code_buffer = NULL;
    size_t code_size = load_file(argv[1], &code_buffer);

    if (code_size == 0) {
        return 1;
    }

    printf("disassembling 0x%zx bytes...\n\n", code_size);

    size_t current_offset = 0;
    instruction_t instr;

    while (current_offset < code_size) {
        // attempt to decode the instruction starting at current_offset
        size_t len = decode_instruction(code_buffer + current_offset, current_offset, &instr);

        if (len == 0 || !strcmp(instr.mnemonic, "unknown")) {
            // treat as a data byte if decoding failed
            print_bytes(code_buffer + current_offset, current_offset, 1);
            printf("db 0x%02x ; data or unsupported\n", code_buffer[current_offset]);
            current_offset++;
        } else {
            current_offset += len;
        }

        // safety limit for the demo
        if (current_offset > 0x1000) break;
    }

    free(code_buffer);
    printf("\ndisassembly complete (limit 0x1000 reached or end of file).\n");

    return 0;
}
