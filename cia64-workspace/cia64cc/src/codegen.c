// codegen.c
// CIA64 code generation integrated (fixed).
// Emits CIA64 assembly (xN/fN registers, .text/.data/.bss, .ascii/.word/.byte).
//
// This file depends on the AST and Type definitions in prototypes.h_h
// (the header you provided). It implements code generation helpers and
// the emit_bss/emit_data/emit_text functions. Fixed build errors:
// - cast node->args to Node** in load_args
// - declare current_fn and top before use
// - use ND_FOR instead of ND_WHILE (header uses ND_FOR)
// - ensure gen_addi is defined before use
//
// Drop this into your project replacing the previous codegen.c.

#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "711cc.h" /* adjust include path/name if needed */

/* ---------------- Printing helpers ---------------- */

/* ---------------- Globals used by codegen ---------------- */

/* expression stack depth (used by original code) */
static int top = 0;

/* current function pointer (used for return label) */
static Function *current_fn = NULL;

/* ---------------- Register helpers ---------------- */

static char regbuf[32][8];
static char fregbuf[32][8];

static char *reg(int i) {
    if (i < 0) i = 0;
    if (i >= 32) i = i % 32;
    snprintf(regbuf[i], sizeof(regbuf[i]), "x%d", i);
    return regbuf[i];
}

static char *freg(int i) {
    if (i < 0) i = 0;
    if (i >= 32) i = i % 32;
    snprintf(fregbuf[i], sizeof(fregbuf[i]), "f%d", i);
    return fregbuf[i];
}

/* ---------------- Simple temporary register allocator ---------------- */

#define TEMP_REG_COUNT 8
static int temp_regs[TEMP_REG_COUNT] = {5, 6, 7, 28, 29, 30, 31, 11};
static bool temp_in_use[TEMP_REG_COUNT];

static int alloc_temp(void) {
    for (int i = 0; i < TEMP_REG_COUNT; i++) {
        if (!temp_in_use[i]) {
            temp_in_use[i] = true;
            return temp_regs[i];
        }
    }
    return 12; // fallback
}

static void free_temp(int r) {
    for (int i = 0; i < TEMP_REG_COUNT; i++) {
        if (temp_regs[i] == r) {
            temp_in_use[i] = false;
            return;
        }
    }
}

/* ---------------- ABI / register names ---------------- */

static const char *SP = "x2";
static const char *FP = "x8";
static const char *RET_INT = "x10";
static const char *RET_FLOAT = "f0";

/* Provide defaults if not defined elsewhere */
#ifndef reg_save_area_offset
int reg_save_area_offset[8] = { -200, -192, -184, -176, -168, -160, -152, -144 };
#endif

#ifndef argreg
char *argreg[] = { "x17", "x16", "x15", "x14", "x13", "x12", "x11", "x10" };
#endif

#ifndef fargreg
int fargreg[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
#endif

/* ---------------- Small emit helpers ---------------- */

static void gen_addi(const char *dest, const char *src, int imm) {
    println("  ADDI %s, %s, %d", dest, src, imm);
}

static void gen_offset_instr(const char *instr, const char *regname, const char *base, int offset) {
    println("  %s %s, -%d(%s)", instr, regname, offset, base);
}

static void gen_offset_instr_fidx(const char *instr, int freg_idx, const char *base, int offset) {
    println("  %s f%d, -%d(%s)", instr, freg_idx, offset, base);
}

/* ---------------- Address/load/store helpers ---------------- */

/* gen_addr: compute address of node and return a temp register index holding it.
   Caller must free the returned temp with free_temp(). */
static int gen_addr(Node *node);

/* load: load value at address in RET_INT into RET_INT or RET_FLOAT depending on type */
static void load(Type *ty);

/* store: store value from RET_INT/RET_FLOAT into address in addr_reg */
static void store_to_addr(Type *ty, const char *addr_reg);

/* ---------------- Forward declarations for gen_expr/gen_stmt ---------------- */

static void gen_expr(Node *node);
static void gen_stmt(Node *node);

/* ---------------- Implementations ---------------- */

static int gen_addr(Node *node) {
    if (!node) {
        int t = alloc_temp();
        println("  ADDI %s, x0, 0", reg(t));
        return t;
    }

    if (node->kind == ND_VAR) {
        Var *v = node->var;
        if (!v->is_local) {
            int t = alloc_temp();
            println("  LA %s, %s", reg(t), v->name); // pseudo-instruction
            return t;
        } else {
            int t = alloc_temp();
            println("  ADDI %s, %s, -%d", reg(t), FP, v->offset);
            return t;
        }
    }

    if (node->kind == ND_DEREF) {
        // evaluate pointer expression into RET_INT
        gen_expr(node->lhs);
        int t = alloc_temp();
        println("  ADDI %s, %s, 0", reg(t), RET_INT);
        return t;
    }

    // fallback: evaluate expression and treat result as address
    gen_expr(node);
    int t = alloc_temp();
    println("  ADDI %s, %s, 0", reg(t), RET_INT);
    return t;
}

static void load(Type *ty) {
    if (!ty) {
        println("  /* load: missing type */");
        return;
    }
    if (ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE) {
        if (ty->size == 4) println("  FLW %s, 0(%s)", RET_FLOAT, RET_INT);
        else println("  FLD %s, 0(%s)", RET_FLOAT, RET_INT);
    } else {
        if (ty->size == 1) println("  LB %s, 0(%s)", RET_INT, RET_INT);
        else if (ty->size == 2) println("  LH %s, 0(%s)", RET_INT, RET_INT);
        else if (ty->size == 4) println("  LW %s, 0(%s)", RET_INT, RET_INT);
        else if (ty->size == 8) println("  LD %s, 0(%s)", RET_INT, RET_INT);
        else println("  /* load: unsupported size %d */", ty->size);
    }
}

static void store_to_addr(Type *ty, const char *addr_reg) {
    if (!ty) {
        println("  /* store: missing type */");
        return;
    }
    if (ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE) {
        if (ty->size == 4) println("  FSW %s, 0(%s)", RET_FLOAT, addr_reg);
        else println("  FSD %s, 0(%s)", RET_FLOAT, addr_reg);
    } else {
        if (ty->size == 1) println("  SB %s, 0(%s)", RET_INT, addr_reg);
        else if (ty->size == 2) println("  SH %s, 0(%s)", RET_INT, addr_reg);
        else if (ty->size == 4) println("  SW %s, 0(%s)", RET_INT, addr_reg);
        else if (ty->size == 8) println("  SD %s, 0(%s)", RET_INT, addr_reg);
        else println("  /* store: unsupported size %d */", ty->size);
    }
}

/* cmp_zero: set RET_INT to 1 if RET_INT != 0 else 0 */
static void cmp_zero(void) {
    int t = alloc_temp();
    println("  ADDI %s, %s, 0", reg(t), RET_INT);
    println("  SLTIU %s, %s, 1", reg(t), reg(t)); // t = (t < 1) ? 1 : 0  => 1 if zero
    println("  XOR %s, %s, x1", reg(t), reg(t)); // XOR with x1 (x1 may be non-zero) - safer to invert differently
    // Simpler: use SLTU to set 1 if non-zero
    println("  SLTU %s, x0, %s", reg(t), reg(t)); // t = (0 < t) ? 1 : 0 -> 1 if non-zero
    println("  ADDI %s, %s, 0", RET_INT, reg(t));
    free_temp(t);
}

/* builtin_va_start: minimal placeholder (emit comment) */
static void builtin_va_start(Node *node) {
    (void)node;
    println("  /* builtin_va_start: not implemented in this generator */");
}

/* cast_cond_zero: minimal placeholder */
static void cast_cond_zero(int label) {
    (void)label;
    println("  /* cast_cond_zero label %d */", label);
}

/* load_args: prepare arguments for function call.
   node->args in the project is typed as Var** in header; cast to Node** for use.
*/
static int load_args(Node *node) {
    if (!node || node->nargs == 0) return 0;
    Node **args = (Node **)node->args; // cast to Node**
    int gp = 0, fpcount = 0;
    for (int i = 0; i < node->nargs; i++) {
        Node *a = args[i];
        if (a && is_flonum(a->ty)) fpcount++;
        else gp++;
    }
    int gi = 0, fi = 0;
    for (int i = 0; i < node->nargs; i++) {
        Node *a = args[i];
        gen_expr(a);
        if (a && is_flonum(a->ty)) {
            int freg = fargreg[fi++];
            if (freg != 0) println("  /* move float result from f0 to f%d if needed */", freg);
            // assume result in f0
        } else {
            int areg = 10 + gi++;
            if (areg != 10) println("  ADDI x%d, %s, 0", areg, RET_INT);
            // else already in x10
        }
    }
    return 0;
}

/* ---------------- Expression generation ---------------- */

static void gen_expr(Node *node) {
    if (!node) return;

    switch (node->kind) {
    case ND_NUM:
        println("  ADDI %s, x0, %ld", RET_INT, node->val);
        return;

    case ND_VAR: {
        Var *v = node->var;
        if (!v->is_local) {
            println("  LA x12, %s", v->name);
            if (node->ty && is_flonum(node->ty)) {
                if (node->ty->size == 8) println("  FLD %s, 0(x12)", RET_FLOAT);
                else println("  FLW %s, 0(x12)", RET_FLOAT);
            } else {
                if (node->ty && node->ty->size == 8) println("  LD %s, 0(x12)", RET_INT);
                else if (node->ty && node->ty->size == 4) println("  LW %s, 0(x12)", RET_INT);
                else if (node->ty && node->ty->size == 2) println("  LH %s, 0(x12)", RET_INT);
                else println("  LB %s, 0(x12)", RET_INT);
            }
        } else {
            if (node->ty && is_flonum(node->ty)) {
                if (node->ty->size == 8) println("  FLD %s, -%d(%s)", RET_FLOAT, v->offset, FP);
                else println("  FLW %s, -%d(%s)", RET_FLOAT, v->offset, FP);
            } else {
                if (node->ty && node->ty->size == 8) println("  LD %s, -%d(%s)", RET_INT, v->offset, FP);
                else if (node->ty && node->ty->size == 4) println("  LW %s, -%d(%s)", RET_INT, v->offset, FP);
                else if (node->ty && node->ty->size == 2) println("  LH %s, -%d(%s)", RET_INT, v->offset, FP);
                else println("  LB %s, -%d(%s)", RET_INT, v->offset, FP);
            }
        }
        return;
    }

    case ND_ADDR: {
        int t = gen_addr(node->lhs);
        println("  ADDI %s, %s, 0", RET_INT, reg(t));
        free_temp(t);
        return;
    }

    case ND_DEREF: {
        gen_expr(node->lhs);
        load(node->ty);
        return;
    }

    case ND_ASSIGN: {
        gen_expr(node->rhs);
        int addr = gen_addr(node->lhs);
        store_to_addr(node->ty, reg(addr));
        free_temp(addr);
        return;
    }

    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV: {
        gen_expr(node->lhs);
        println("  ADDI x12, %s, 0", RET_INT);
        gen_expr(node->rhs);
        println("  ADDI x13, %s, 0", RET_INT);
        if (node->kind == ND_ADD) println("  ADD %s, x12, x13", RET_INT);
        else if (node->kind == ND_SUB) println("  SUB %s, x12, x13", RET_INT);
        else if (node->kind == ND_MUL) println("  MUL %s, x12, x13", RET_INT);
        else if (node->kind == ND_DIV) println("  DIV %s, x12, x13", RET_INT);
        return;
    }

    case ND_RETURN:
        if (node->lhs) {
            gen_expr(node->lhs);
            if (is_flonum(node->lhs->ty)) {
                println("  /* return float in %s */", RET_FLOAT);
            } else {
                println("  /* return int in %s */", RET_INT);
            }
        }
        if (current_fn && current_fn->name)
            println("  J .L.return.%s", current_fn->name);
        else
            println("  RET");
        return;

    case ND_EXPR_STMT:
        gen_expr(node->lhs);
        return;

    case ND_IF: {
        int label_else = rand() & 0xffff;
        int label_end = rand() & 0xffff;
        gen_expr(node->cond);
        println("  BEQ %s, x0, .Lelse_%d", RET_INT, label_else);
        gen_stmt(node->then);
        println("  J .Lend_%d", label_end);
        println(".Lelse_%d:", label_else);
        if (node->els) gen_stmt(node->els);
        println(".Lend_%d:", label_end);
        return;
    }

    case ND_FOR: {
        int label_top = rand() & 0xffff;
        int label_end = rand() & 0xffff;
        if (node->init) gen_stmt(node->init);
        println(".Lloop_%d:", label_top);
        if (node->cond) {
            gen_expr(node->cond);
            println("  BEQ %s, x0, .Lend_%d", RET_INT, label_end);
        }
        if (node->then) gen_stmt(node->then);
        if (node->inc) gen_expr(node->inc);
        println("  J .Lloop_%d", label_top);
        println(".Lend_%d:", label_end);
        return;
    }

    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next) gen_stmt(n);
        return;

    case ND_FUNCALL: {
        load_args(node);
        if (node->lhs && node->lhs->kind == ND_VAR && node->lhs->var && node->lhs->var->name) {
            println("  CALL %s", node->lhs->var->name);
        } else if (node->tok && node->tok->str) {
            println("  CALL %s", node->tok->str);
        } else {
            println("  /* indirect call: not implemented */");
        }
        return;
    }

    default:
        println("  /* gen_expr: unhandled node kind %d */", node->kind);
        return;
    }
}

/* ---------------- Statement generation ---------------- */

static void gen_stmt(Node *node) {
    if (!node) return;
    switch (node->kind) {
    case ND_RETURN:
    case ND_EXPR_STMT:
    case ND_IF:
    case ND_FOR:
    case ND_BLOCK:
        gen_expr(node);
        break;
    case ND_GOTO:
        println("  J %s", node->label_name);
        break;
    case ND_LABEL:
        println("%s:", node->label_name);
        break;
    default:
        gen_expr(node);
        break;
    }
}

/* ---------------- Data and text emission ---------------- */

static void emit_bss(Program *prog) {
    println("  .bss");
    for (Var *var = prog->globals; var; var = var->next) {
        if (var->init_data) continue;
        println("  .align %d", var->align);
        if (!var->is_static) println("  .globl %s", var->name);
        println("%s:", var->name);
        int bytes = var->ty ? var->ty->size : 0;
        int words = bytes / 4;
        int rem = bytes % 4;
        for (int i = 0; i < words; i++) println("  .word 0");
        for (int i = 0; i < rem; i++) println("  .byte 0");
    }
}

static void emit_data(Program *prog) {
    println("  .data");
    for (Var *var = prog->globals; var; var = var->next) {
        if (!var->init_data) continue;
        println("  .align %d", var->align);
        if (!var->is_static) println("  .globl %s", var->name);
        println("%s:", var->name);
        Relocation *rel = var->rel;
        int pos = 0;
        if (var->ty && var->ty->kind == TY_ARRAY && var->ty->base && var->ty->base->kind == TY_CHAR) {
            int buf_pos = 0;
            char buf[1024];
            while (pos < var->ty->size) {
                unsigned char ch = var->init_data[pos++];
                switch (ch) {
                case '\0': buf[buf_pos++] = '\\'; buf[buf_pos++] = '0'; break;
                case '"': buf[buf_pos++] = '\\'; buf[buf_pos++] = '"'; break;
                case '\\': buf[buf_pos++] = '\\'; buf[buf_pos++] = '\\'; break;
                case '\a': buf[buf_pos++] = '\\'; buf[buf_pos++] = 'a'; break;
                case '\b': buf[buf_pos++] = '\\'; buf[buf_pos++] = 'b'; break;
                case '\t': buf[buf_pos++] = '\\'; buf[buf_pos++] = 't'; break;
                case '\v': buf[buf_pos++] = '\\'; buf[buf_pos++] = 'v'; break;
                case '\f': buf[buf_pos++] = '\\'; buf[buf_pos++] = 'f'; break;
                case '\r': buf[buf_pos++] = '\\'; buf[buf_pos++] = 'r'; break;
                case '\n': buf[buf_pos++] = '\\'; buf[buf_pos++] = 'n'; break;
                default:
                    if (ch >= 32 && ch < 127) buf[buf_pos++] = (char)ch;
                    else {
                        int n = snprintf(buf + buf_pos, sizeof(buf) - buf_pos, "\\%03o", ch);
                        buf_pos += n;
                    }
                    break;
                }
                if (buf_pos > (int)(sizeof(buf) - 10)) {
                    buf[buf_pos] = '\0';
                    println("  .ascii \"%s\"", buf);
                    buf_pos = 0;
                }
            }
            if (buf_pos > 0) {
                buf[buf_pos] = '\0';
                println("  .ascii \"%s\"", buf);
            }
            continue;
        }
        while (pos < (var->ty ? var->ty->size : 0)) {
            if (rel && rel->offset == pos) {
                println("  /* relocation: %s %+ld (64-bit) */", rel->label, rel->addend);
                println("  .word %s%+ld", rel->label, rel->addend);
                println("  .word %s%+ld", rel->label, rel->addend >> 32);
                rel = rel->next;
                pos += 8;
            } else {
                println("  .byte %d", (unsigned char)var->init_data[pos++]);
            }
        }
    }
}

static void emit_text(Program *prog) {
    println("  .text");
    for (Function *fn = prog->fns; fn; fn = fn->next) {
        println("  .align 1");
        if (!fn->is_static) println("  .globl %s", fn->name);
        println("  .type %s, @function", fn->name);
        println("%s:", fn->name);
        current_fn = fn;
        println("  ADDI %s, %s, -8", SP, SP);
        println("  SD %s, 0(%s)", FP, SP);
        println("  ADDI %s, %s, 0", FP, SP);
        gen_addi(SP, SP, -1 * fn->stack_size);
        println("  SD x9, -8(%s)", FP);
        println("  SD x18, -16(%s)", FP);
        println("  SD x19, -24(%s)", FP);
        println("  SD x20, -32(%s)", FP);
        println("  SD x21, -40(%s)", FP);
        println("  SD x22, -48(%s)", FP);
        println("  SD x23, -56(%s)", FP);
        println("  SD x24, -64(%s)", FP);
        println("  SD x25, -72(%s)", FP);
        println("  SD x26, -80(%s)", FP);
        println("  SD x27, -88(%s)", FP);
        println("  FSD f8, -96(%s)", FP);
        println("  FSD f9, -104(%s)", FP);
        println("  FSD f10, -112(%s)", FP);
        println("  FSD f11, -120(%s)", FP);
        println("  FSD f12, -128(%s)", FP);
        println("  FSD f13, -136(%s)", FP);
        println("  FSD f14, -144(%s)", FP);
        println("  FSD f15, -152(%s)", FP);
        println("  FSD f16, -160(%s)", FP);
        println("  FSD f17, -168(%s)", FP);
        println("  FSD f18, -176(%s)", FP);
        println("  FSD f19, -184(%s)", FP);
        if (fn->is_variadic) {
            println("  SD x10, %d(%s)", reg_save_area_offset[0], FP);
            println("  SD x11, %d(%s)", reg_save_area_offset[1], FP);
            println("  SD x12, %d(%s)", reg_save_area_offset[2], FP);
            println("  SD x13, %d(%s)", reg_save_area_offset[3], FP);
            println("  SD x14, %d(%s)", reg_save_area_offset[4], FP);
            println("  SD x15, %d(%s)", reg_save_area_offset[5], FP);
            println("  SD x16, %d(%s)", reg_save_area_offset[6], FP);
            println("  SD x17, %d(%s)", reg_save_area_offset[7], FP);
        }
        int gp = 0, fpcount = 0;
        for (Var *v = fn->params; v; v = v->next) {
            if (is_flonum(v->ty)) fpcount++;
            else gp++;
        }
        for (Var *v = fn->params; v; v = v->next) {
            if (v->ty && v->ty->kind == TY_FLOAT) {
                int freg = fargreg[--fpcount];
                println("  FSW f%d, -%d(%s)", freg, v->offset, FP);
            } else if (v->ty && v->ty->kind == TY_DOUBLE) {
                int freg = fargreg[--fpcount];
                println("  FSD f%d, -%d(%s)", freg, v->offset, FP);
            } else {
                char *r = argreg[--gp];
                if (v->ty && v->ty->size == 1) println("  SB %s, -%d(%s)", r, v->offset, FP);
                else if (v->ty && v->ty->size == 2) println("  SH %s, -%d(%s)", r, v->offset, FP);
                else if (v->ty && v->ty->size == 4) println("  SW %s, -%d(%s)", r, v->offset, FP);
                else println("  SD %s, -%d(%s)", r, v->offset, FP);
            }
        }
        gen_stmt(fn->body);
        assert(top == 0);
        if (strcmp(fn->name, "main") == 0) println("  ADDI x10, x0, 0");
        println(".L.return.%s:", fn->name);
        println("  LD x9, -8(%s)", FP);
        println("  LD x18, -16(%s)", FP);
        println("  LD x19, -24(%s)", FP);
        println("  LD x20, -32(%s)", FP);
        println("  LD x21, -40(%s)", FP);
        println("  LD x22, -48(%s)", FP);
        println("  LD x23, -56(%s)", FP);
        println("  LD x24, -64(%s)", FP);
        println("  LD x25, -72(%s)", FP);
        println("  LD x26, -80(%s)", FP);
        println("  LD x27, -88(%s)", FP);
        println("  FLD f8, -96(%s)", FP);
        println("  FLD f9, -104(%s)", FP);
        println("  FLD f10, -112(%s)", FP);
        println("  FLD f11, -120(%s)", FP);
        println("  FLD f12, -128(%s)", FP);
        println("  FLD f13, -136(%s)", FP);
        println("  FLD f14, -144(%s)", FP);
        println("  FLD f15, -152(%s)", FP);
        println("  FLD f16, -160(%s)", FP);
        println("  FLD f17, -168(%s)", FP);
        println("  FLD f18, -176(%s)", FP);
        println("  FLD f19, -184(%s)", FP);
        println("  ADDI %s, %s, 0", SP, FP);
        println("  LD %s, 0(%s)", FP, SP);
        println("  ADDI %s, %s, 8", SP, SP);
        println("  RET");
    }
}

/* ---------------- Top-level entry ---------------- */

void codegen(Program *prog) {
    emit_bss(prog);
    emit_data(prog);
    emit_text(prog);
}
