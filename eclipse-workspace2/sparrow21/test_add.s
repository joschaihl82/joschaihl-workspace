.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    mov $1, %rax
    push %rax
    mov $2, %rax
    push %rax
    mov $3, %rax
    mov %rax, %rbx
    pop %rax
    imul %rbx, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    push %rax
    lea Lstr_0(%rip), %rax
    push %rax
    pop %rdi
    pop %rsi
    call printf
    mov $0, %edi
    call exit
    pop %rbp
    ret
.section .rodata
Lfmt_int: .asciz "%ld\n"
Lstr_0: .asciz "%ld\n"
.section .data
.section .text
