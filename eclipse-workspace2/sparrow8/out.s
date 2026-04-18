.text
.globl main
main:
    leaq .Lstr0(%rip), %rax
    mov %rax, %rdi
    call js_print_value
    call js_print_newline
    mov $0, %rax
    mov $0, %rax
    mov $60, %rax
    xor %rdi, %rdi
    syscall
.section .rodata
.Lstr0: .string "DONE"

/* --- runtime stubs --- */
.section .text
.globl js_print_value
js_print_value:
    /* rdi = value. Try to guess type: small int vs pointer? */
    /* Heuristic: if value < 0x100000, treat as int, else string */
    cmp $0x100000, %rdi
    jb .print_int
    /* It's a pointer (string) */
    mov %rdi, %rsi
    lea .Lfmt_s(%rip), %rdi
    xor %rax, %rax
    call printf
    ret
.print_int:
    mov %rdi, %rsi
    lea .Lfmt_d(%rip), %rdi
    xor %rax, %rax
    call printf
    ret

.globl js_print_space
js_print_space:
    lea .Lfmt_sp(%rip), %rdi
    xor %rax, %rax
    call printf
    ret

.globl js_print_newline
js_print_newline:
    lea .Lfmt_nl(%rip), %rdi
    xor %rax, %rax
    call printf
    ret

.section .rodata
.Lfmt_s: .string "%s"
.Lfmt_d: .string "%lld"
.Lfmt_sp: .string " "
.Lfmt_nl: .string "\n"
.section .text
