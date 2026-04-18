.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    mov $1, %rax
    cmp $0, %rax
    je .Lelse_1
    mov $10, %rax
    push %rax
    lea Lstr_0(%rip), %rax
    push %rax
    pop %rdi
    pop %rsi
    call printf
    jmp .Lend_1
.Lelse_1:
    mov $20, %rax
    push %rax
    lea Lstr_0(%rip), %rax
    push %rax
    pop %rdi
    pop %rsi
    call printf
.Lend_1:
    mov $0, %edi
    call exit
    pop %rbp
    ret
.section .rodata
Lfmt_int: .asciz "%ld\n"
Lstr_0: .asciz "%ld\n"
.section .data
.section .text
