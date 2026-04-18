/*
 * pasm, pixie (elf) assembler, is an elf64 x86_64 assembler.
 * copyright (c) 2022 ahmad hadwan
 *
 * this program is free software; you can redistribute it and/or modify
 * it under the terms of the gnu general public license as published by
 * the free software foundation; either version 2 of the license, or
 * (at your option) any later version.

 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability and fitness for a particular purpose. see the
 * gnu general public license for more details.
 *
 * you should have received a copy of the gnu general public license along
 * with this program; see copying file for copyright and license details.
 */
#include <ctype.h>
#include <elf.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define alignto8(x) (x + ((8 - (x % 8)) * ((x % 8) != 0)))
#define outfile_default "a.out"

/* enums */
enum { id, label, directive, constant, register_op, comma,
       newline, endoffile, types_count };

/* x64 encoding enums */
typedef enum {
    op_none,
    op_reg_r64,    // 64-bit register (e.g., %rax)
    op_imm_i32,    // 32-bit immediate value
    op_mem_m64     // memory address/label
} operand_type_t;

typedef enum {
    ins_0_op,         // clc, ret, nop
    ins_1_op_reg_sp,  // push/pop %reg (special 50+r encoding)
    ins_1_op_modrm,   // inc/dec %reg (ff /0, ff /1 encoding)
    ins_2_op_r_r,     // mov/add/sub %reg, %reg (modr/m encoding)
    ins_2_op_i_rax,   // mov $imm32, %rax (a3 encoding)
    ins_jmp_rel32     // jmp label (requires full relocation)
} instruction_type_t;

/* structs */
typedef struct {
    elf64_ehdr *ehdr;
    uint8_t    *assembly;
    size_t     assembly_size;
    elf64_sym  *syms;
    size_t     section_count;
    size_t     label_count;
    size_t     glabel_count;
    char     **strtab;
    size_t     strtab_count;
    char     **shstrtab;
    size_t     shstrtab_count;
    elf64_shdr *shdrs;
    size_t     shdr_count;
} elf64_obj_t;

typedef struct {
    int    type;
    int    len;
    size_t start;
} token_t;

typedef struct {
    char  *src;
    size_t i;
} unit_t;

/* new enhanced operand structure for parsing */
typedef struct {
    operand_type_t type;
    uint8_t reg_code; // 0-7 for rax-rdi, 8-15 for r8-r15
    uint32_t imm_val;
} operand_x64_t;

/* new enhanced instruction structure */
typedef struct {
    const char *name;
    uint8_t opcode_byte;
    instruction_type_t type;
    uint8_t modrm_reg_field; // the /r field (0-7)
    size_t operand_count;
} ins_x64_t;

/* function declarations */
static int assemble_file(char *filename, char *outfile);
static int assemble_x86_64(char *src, char *outfile);
static int default_shdrtabs_x86_64(elf64_obj_t *obj);
static int default_symtabs_x86_64(elf64_obj_t *obj);
static int assemble_instruction(unit_t *unit, elf64_obj_t *obj, const ins_x64_t *ins);
static int parse_operand_x64(unit_t *unit, operand_x64_t *op);
static int get_register_code(const char *reg_name, uint8_t *reg_code);
static void append_byte(elf64_obj_t *obj, uint8_t byte);
static void append_dword(elf64_obj_t *obj, uint32_t dword);
static int lex(unit_t *unit, token_t *token);
static int lex_constant(unit_t *unit, token_t *token);
static int lex_id(unit_t *unit, token_t *token);
static int parse_x86_64(unit_t *unit, elf64_obj_t *obj);
static void skip_comments(unit_t *unit);
static void usage();
static int write_file_x86_64(char *outfile, elf64_obj_t *obj);

/* variables */
static const char *token_types[types_count] = {
    "identifier", "label", "directive", "constant", "register", "comma",
    "newline", "endoffile"
};

/* instruction table (26 instructions, most are register-only) */
static const ins_x64_t ins_x64[] = {
    // --- 0-operand instructions (11) ---
    { "clc", 0xf8, ins_0_op, 0, 0 }, { "stc", 0xf9, ins_0_op, 0, 0 },
    { "cli", 0xfa, ins_0_op, 0, 0 }, { "sti", 0xfb, ins_0_op, 0, 0 },
    { "cld", 0xfc, ins_0_op, 0, 0 }, { "std", 0xfd, ins_0_op, 0, 0 },
    { "leave", 0xc9, ins_0_op, 0, 0 }, { "leaveq", 0xc9, ins_0_op, 0, 0 },
    { "nop", 0x90, ins_0_op, 0, 0 }, { "ret", 0xc3, ins_0_op, 0, 0 },
    { "syscall", 0x0f, ins_0_op, 0, 0 }, // special 0f 05

    // --- 1-operand register special (2) ---
    { "push", 0x50, ins_1_op_reg_sp, 0, 1 },
    { "pop", 0x58, ins_1_op_reg_sp, 0, 1 },

    // --- 1-operand register modr/m (2) ---
    { "inc", 0xff, ins_1_op_modrm, 0, 1 }, // ff /0
    { "dec", 0xff, ins_1_op_modrm, 1, 1 }, // ff /1

    // --- 2-operand register-register (6) ---
    // mov %r64, %r64 (89/r - r64 -> r/m64)
    { "mov", 0x89, ins_2_op_r_r, 0, 2 },
    // add %r64, %r64 (01/r - r64 -> r/m64)
    { "add", 0x01, ins_2_op_r_r, 0, 2 },
    // sub %r64, %r64 (29/r - r64 -> r/m64)
    { "sub", 0x29, ins_2_op_r_r, 0, 2 },

    // and %r64, %r64 (21/r - r64 -> r/m64)
    { "and", 0x21, ins_2_op_r_r, 0, 2 },
    // or %r64, %r64 (09/r - r64 -> r/m64)
    { "or", 0x09, ins_2_op_r_r, 0, 2 },
    // xor %r64, %r64 (31/r - r64 -> r/m64)
    { "xor", 0x31, ins_2_op_r_r, 0, 2 },

    // --- immediate-to-register (1) ---
    // mov $imm32, %rax (special a3 encoding)
    // NOTE: mov $imm, %reg is usually c7 /0, but using the specific a3 for brevity.
    { "mov_imm", 0xa3, ins_2_op_i_rax, 0, 2 },

    // --- control flow (2) ---
    // these are stubbed to emit an error, as they require relocation.
    { "jmp", 0xe9, ins_jmp_rel32, 0, 1 },
    { "call", 0xe8, ins_jmp_rel32, 0, 1 },
};

static const size_t ins_x64_count = sizeof(ins_x64) / sizeof(ins_x64[0]);


/* --- encoding helpers --- */

static void append_byte(elf64_obj_t *obj, uint8_t byte) {
    obj->assembly = realloc(obj->assembly, obj->assembly_size + 1);
    obj->assembly[obj->assembly_size++] = byte;
}

static void append_dword(elf64_obj_t *obj, uint32_t dword) {
    // little-endian encoding
    append_byte(obj, (uint8_t)(dword & 0xff));
    append_byte(obj, (uint8_t)((dword >> 8) & 0xff));
    append_byte(obj, (uint8_t)((dword >> 16) & 0xff));
    append_byte(obj, (uint8_t)((dword >> 24) & 0xff));
}

/* maps a register name (e.g., "rax") to its 4-bit code and returns 0 if found */
static int get_register_code(const char *reg_name, uint8_t *reg_code) {
    // only rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp (codes 0-7) are supported here
    if (!strcmp(reg_name, "rax")) *reg_code = 0;
    else if (!strcmp(reg_name, "rcx")) *reg_code = 1;
    else if (!strcmp(reg_name, "rdx")) *reg_code = 2;
    else if (!strcmp(reg_name, "rbx")) *reg_code = 3;
    else if (!strcmp(reg_name, "rsp")) *reg_code = 4;
    else if (!strcmp(reg_name, "rbp")) *reg_code = 5;
    else if (!strcmp(reg_name, "rsi")) *reg_code = 6;
    else if (!strcmp(reg_name, "rdi")) *reg_code = 7;
    else {
        return 1; // not found (includes r8-r15 and non-64 bit registers)
    }
    return 0;
}

/* --- parsing helpers --- */

/* parses a single operand (simplified: only %reg or $const) */
static int parse_operand_x64(unit_t *unit, operand_x64_t *op)
{
    token_t token;
    char *buff;

    if (lex(unit, &token)) return 1;

    if (token.type == register_op) {
        buff = malloc(token.len + 1);
        memcpy(buff, unit->src + token.start, token.len);
        buff[token.len] = '\0';

        // strip the leading '%'
        if (get_register_code(buff + 1, &(op->reg_code))) {
            fprintf(stderr, "error: unsupported register `%s`. only rax-rdi supported.\n", buff);
            free(buff);
            return 1;
        }

        op->type = op_reg_r64;
        free(buff);
        return 0;
    }
    else if (token.type == constant) {
        // assume lex_constant correctly set the token length for the number
        buff = malloc(token.len + 1);
        memcpy(buff, unit->src + token.start, token.len);
        buff[token.len] = '\0';

        // simple strtol (only base 10/16 supported)
        op->imm_val = strtol(buff, null, 0);
        op->type = op_imm_i32;
        free(buff);
        return 0;
    }
    else if (token.type == id) {
        op->type = op_mem_m64; // treat label as memory for error purposes
        return 0;
    }

    fprintf(stderr, "error: expected register or constant, found `%s`\n", token_types[token.type]);
    return 1;
}

/* --- core assembly dispatcher --- */

static int assemble_instruction(unit_t *unit, elf64_obj_t *obj, const ins_x64_t *ins)
{
    operand_x64_t op1, op2;
    uint8_t rex = 0x48; // rex.w prefix (always 64-bit operation)
    token_t next_token;

    // helper to consume the next token and check for newline/endoffile
    #define check_eol() \
        if (lex(unit, &next_token)) return 1; \
        if (next_token.type != newline && next_token.type != endoffile) { \
            fprintf(stderr, "error: unexpected token after instruction: `%s`\n", ins->name); \
            return 1; \
        }

    switch (ins->type) {
        case ins_0_op:
            if (ins->opcode_byte == 0x0f) { append_byte(obj, 0x0f); append_byte(obj, 0x05); } // syscall
            else { append_byte(obj, ins->opcode_byte); }
            check_eol();
            break;

        case ins_1_op_reg_sp: // push/pop %rax (50+r or 58+r)
            if (parse_operand_x64(unit, &op1)) return 1;
            if (op1.type != op_reg_r64) { fprintf(stderr, "error: push/pop requires a register.\n"); return 1; }

            append_byte(obj, ins->opcode_byte + op1.reg_code);
            check_eol();
            break;

        case ins_1_op_modrm: // inc/dec %rax (ff /0 or ff /1)
            if (parse_operand_x64(unit, &op1)) return 1;
            if (op1.type != op_reg_r64) { fprintf(stderr, "error: inc/dec requires a register.\n"); return 1; }

            append_byte(obj, rex); // 1. rex prefix (0x48)
            append_byte(obj, ins->opcode_byte); // 2. opcode (0xff)
            // 3. modr/m byte (mod=3 for register, reg=/r field, rm=reg_code)
            append_byte(obj, 0xc0 | (ins->modrm_reg_field << 3) | op1.reg_code);
            check_eol();
            break;

        case ins_2_op_r_r: // mov %reg1, %reg2 (modr/m encoding for register-register)
            if (parse_operand_x64(unit, &op1)) return 1; // reg1 (source)

            if (lex(unit, &next_token)) return 1;
            if (next_token.type != comma) { fprintf(stderr, "error: expected comma.\n"); return 1; }

            if (parse_operand_x64(unit, &op2)) return 1; // reg2 (destination)

            if (op1.type != op_reg_r64 || op2.type != op_reg_r64) {
                fprintf(stderr, "error: 2-operand instruction only supports reg, reg for now.\n");
                return 1;
            }

            // instruction format: opcode r/m64, r64 (r64 -> r/m64, d=0)
            // reg field in modr/m is the source (op1), r/m field is the dest (op2)

            append_byte(obj, rex); // 1. rex prefix (0x48)
            append_byte(obj, ins->opcode_byte); // 2. opcode (0x89, 0x01, 0x29, etc.)
            // 3. modr/m byte (mod=3, reg=op1.reg_code, rm=op2.reg_code)
            append_byte(obj, 0xc0 | (op1.reg_code << 3) | op2.reg_code);

            check_eol();
            break;

        case ins_2_op_i_rax: // mov $imm32, %rax (special a3 encoding)
            if (parse_operand_x64(unit, &op1)) return 1; // imm (source)

            if (lex(unit, &next_token)) return 1;
            if (next_token.type != comma) { fprintf(stderr, "error: expected comma.\n"); return 1; }

            if (parse_operand_x64(unit, &op2)) return 1; // %rax (destination)

            if (op1.type != op_imm_i32) { fprintf(stderr, "error: first operand must be an immediate value.\n"); return 1; }
            if (op2.type != op_reg_r64 || op2.reg_code != 0) { fprintf(stderr, "error: second operand must be %%rax for this simple mov_imm.\n"); return 1; }

            append_byte(obj, rex); // 1. rex prefix (0x48)
            append_byte(obj, 0xc7); // general mov %imm, %reg opcode
            append_byte(obj, 0xc0 | (0 << 3) | 0); // modr/m: mod=3, reg=/0, rm=rax
            append_dword(obj, op1.imm_val); // 4. immediate dword

            check_eol();
            break;

        case ins_jmp_rel32: // jmp/call label (error due to missing relocation)
            if (parse_operand_x64(unit, &op1)) return 1;
            fprintf(stderr, "error: control flow instruction `%s` requires relocation which is not implemented.\n", ins->name);
            return 1;

        default:
            fprintf(stderr, "internal assembler error: unhandled instruction type %d\n", ins->type);
            return 1;
    }

    return 0;

    #undef check_eol
}


/* --- lexer and parser --- */

int lex_constant(unit_t *unit, token_t *token)
{
    token->type = constant;
    token->start = unit->i;
    // support hex (0x...) and decimal
    if (unit->src[unit->i] == '0' && (unit->src[unit->i + 1] == 'x' || unit->src[unit->i + 1] == 'X')) {
        unit->i += 2;
        while (isxdigit(unit->src[unit->i])) {
            unit->i++;
        }
    } else {
        while (isdigit(unit->src[unit->i])) {
            unit->i++;
        }
    }
    token->len = unit->i - token->start;
    if (token->len == 0) {
        fprintf(stderr, "error: expected digits after $ or 0x.\n");
        return 1;
    }
    return 0;
}

int lex(unit_t *unit, token_t *token)
{
    char c;
    int return_value;

    /* skip whitespace */
    while (isblank(unit->src[unit->i])) {
        unit->i++;
    }

    skip_comments(unit);

    c = unit->src[unit->i];
    return_value = 0;

    switch (c)
    {
        case '$':
            unit->i++;
            return_value = lex_constant(unit, token);
            break;
        case '%':
            unit->i++;
            return_value = lex_id(unit, token);
            token->type = register_op;
            break;
        case ',':
            token->type = comma;
            token->start = unit->i;
            unit->i++;
            token->len = 1;
            break;
        case '\n':
            token->type = newline;
            token->start = unit->i;
            unit->i++;
            token->len = 1;
            break;
        case '\0':
            token->type = endoffile;
            token->start = unit->i;
            token->len = 1;
            break;
        default:
            if (c == '.' || c == '_' || isalpha(c)) {
                return_value = lex_id(unit, token);
                if (unit->src[unit->i] == ':') {
                    token->type = label;
                    unit->i++;
                }
                else if (c == '.') {
                    token->type = directive;
                }
                break;
            }

            // memory addressing is too complex; treat bare numbers/symbols as error
            if (isdigit(c)) {
                fprintf(stderr, "error: bare number found. complex memory addressing not supported.\n");
                return 1;
            }

            fprintf(stderr, "invalid character `%c`.\n", c);
            return 1;
    }

    return return_value;
}

int lex_id(unit_t *unit, token_t *token)
{
    token->type = id;
    token->start = unit->i++;

    while (isalnum(unit->src[unit->i]) || unit->src[unit->i] == '_') {
        unit->i++;
    }

    token->len = unit->i - token->start;
    return 0;
}

int parse_x86_64(unit_t *unit, elf64_obj_t *obj)
{
    token_t token;
    char *buff;

    while (!lex(unit, &token)) {
        buff = malloc(token.len + 1);
        memcpy(buff, unit->src + token.start, token.len);
        buff[token.len] = '\0';

        printf("token: type=%s, text=`", token_types[token.type]);
        switch (token.type)
        {
            case newline: printf("\\n"); break;
            case endoffile: printf("\\0"); break;
            default: printf("%s", buff); break;
        }
        printf("`\n");

        switch (token.type)
        {
            case id:
            {
                for (char *p = buff; *p; ++p) { *p = tolower(*p); }

                int found = 0;
                const ins_x64_t *ins_found = null;
                for (size_t i = 0; i < ins_x64_count; i++) {
                    if (!strcmp(buff, ins_x64[i].name)) {
                        ins_found = &ins_x64[i];
                        found = 1;
                        break;
                    }
                }

                if (found) {
                    if (assemble_instruction(unit, obj, ins_found)) {
                        goto free_buff_error;
                    }
                } else {
                    fprintf(stderr, "error: unknown instruction: `%s`\n", buff);
                    goto free_buff_error;
                }

                free(buff);
                break;
            }
            case label:
            {
                // this is a simplified label handler
                // it records the label but does not use its offset for relocation
                free(buff);
                break;
            }
            case directive:
            {
                // .globl directive handling (no relocation done)
                if (!strcmp(buff, ".globl")) {
                    free(buff);
                    if (lex(unit, &token)) return 1;
                    if (token.type != id) {
                        fprintf(stderr, "error: .globl directive expected a symbol.\n");
                        return 1;
                    }
                    if (lex(unit, &token)) return 1;
                    if (token.type != newline && token.type != endoffile) {
                        fprintf(stderr, "error: junk at end of line after .globl.\n");
                        return 1;
                    }
                } else {
                    fprintf(stderr, "error: unknown pseudo-op: `%s`\n", buff);
                    goto free_buff_error;
                }
                break;
            }
            case endoffile: { free(buff); return 0; }
            default: free(buff); break;
        }
    }

    return 1;

free_buff_error:
    free(buff);
    return 1;
}

void skip_comments(unit_t *unit)
{
    if (unit->src[unit->i] == ';' || (unit->src[unit->i] == '/' && unit->src[unit->i + 1] == '/')) {
        do { unit->i++; } while (unit->src[unit->i] != '\n' && unit->src[unit->i] != '\0');
    }
}

/* --- elf file functions --- (simplified/stubbed for single-file submission) */

int default_symtabs_x86_64(elf64_obj_t *obj)
{
    obj->syms = malloc(4 * sizeof(elf64_sym));
    obj->section_count += 4;
    return 0;
}

int default_shdrtabs_x86_64(elf64_obj_t *obj)
{
    obj->shdrs = malloc(7 * sizeof(elf64_shdr));
    obj->shdr_count += 7;
    return 0;
}

int write_file_x86_64(char *outfile, elf64_obj_t *obj)
{
    uint8_t *raw_obj;
    size_t raw_obj_len;
    file *fd;

    // determine final file size (simplified)
    raw_obj_len = 240 + alignto8(obj->assembly_size);

    raw_obj = calloc(1, raw_obj_len); // calloc for zeroing padding
    if (obj->assembly) {
        memcpy(raw_obj + sizeof(elf64_ehdr), obj->assembly, obj->assembly_size);
        free(obj->assembly);
    }

    // write to file
    fd = fopen(outfile, "w");
    if (fd) {
        fwrite(raw_obj, raw_obj_len, 1, fd);
        fclose(fd);
    } else {
        fprintf(stderr, "error: could not write to output file %s\n", outfile);
    }

    free(raw_obj);
    return 0;
}

int assemble_x86_64(char *src, char *outfile)
{
    elf64_obj_t obj;
    elf64_ehdr ehdr;
    unit_t unit;

    obj = (elf64_obj_t){};
    obj.strtab = malloc(sizeof(char *)); obj.shstrtab = malloc(sizeof(char *));
    unit = (unit_t){ .src = src, .i = 0 };

    default_symtabs_x86_64(&obj);

    if (parse_x86_64(&unit, &obj)) {
        return 1;
    }

    // minimal ehdr setup
    ehdr = (elf64_ehdr){ .e_ident[ei_mag0] = elfmago, .e_ident[ei_class] = elfclass64,
                         .e_ident[ei_data] = elfdata2lsb, .e_type = et_rel,
                         .e_machine = em_x86_64, .e_shoff = 0x80 + alignto8(obj.assembly_size),
                         .e_shentsize = 64, .e_shnum = 7, .e_shstrndx = 6 };

    obj.ehdr = &ehdr;

    default_shdrtabs_x86_64(&obj);
    write_file_x86_64(outfile, &obj);

    free(obj.syms); free(obj.shdrs);
    for (int i = 0; i < obj.strtab_count; i++) free(obj.strtab[i]); free(obj.strtab);
    for (int i = 0; i < obj.shstrtab_count; i++) free(obj.shstrtab[i]); free(obj.shstrtab);

    return 0;
}

int assemble_file(char *filename, char *outfile)
{
    file *fd;
    struct stat filestat;
    char *src;
    int return_value;

    fd = fopen(filename, "r");
    if (fd == null) { fprintf(stderr, "failed to open `%s`.\n", filename); return 1; }

    if (stat(filename, &filestat)) { fprintf(stderr, "failed to stat `%s`.\n", filename); return 1; }

    src = malloc(filestat.st_size + 1);
    src[filestat.st_size] = '\0';
    fread(src, sizeof(char), filestat.st_size, fd);
    fclose(fd);

    return_value = assemble_x86_64(src, outfile);

    free(src);
    return return_value;
}

void usage()
{
    puts("usage: pasm [options] asmfile\n"
         "options:\n"
         "  --help     display this information.\n"
         "  -o outfile specify the output file name. (default is "outfile_default")"
    );
}

int main(int argc, char **argv)
{
    char *filename, *outfile;

    filename = outfile = null;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (!strcmp(argv[i], "--help")) { usage(); return 0; }
            else if (!strcmp(argv[i], "-o")) {
                i++;
                if (outfile) { fprintf(stderr, "output file name was already specified!\n"); return 1; }
                if (i >= argc) { fprintf(stderr, "option `-o` requires an argument.\n"); return 1; }
                outfile = argv[i];
            } else { usage(); return 1; }
        } else {
            if (filename) { fprintf(stderr, "pasm: fatal error: only one input file is supported.\n"); return 1; }
            filename = argv[i];
        }
    }

    if (!filename) { fprintf(stderr, "pasm: fatal error: no input files.\n"); return 1; }
    if (!outfile) { outfile = outfile_default; }

    return assemble_file(filename, outfile);
}
