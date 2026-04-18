.section .text
.globl get_val
get_val:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    mov $100, %rax
    jmp .Lfunc_end_get_val
.Lfunc_end_get_val:
    mov %rbp, %rsp
    pop %rbp
    ret
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    xor %rax, %rax
    call get_val
    mov %rax, Lglob_0(%rip)
    mov Lglob_0(%rip), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
.Lfunc_end_main:
    add $8, %rsp
    pop %rbp
    mov $0, %edi
    call exit
.section .rodata
Lfmt_int: .asciz "%ld\n"
Lstr_0: .asciz "%ld\n"
.section .data
Lglob_0: .quad 0
.section .text
