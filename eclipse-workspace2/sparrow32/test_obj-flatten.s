.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    mov $1, %rax
    mov %rax, Lglob_0(%rip)
    mov $2, %rax
    mov %rax, Lglob_1(%rip)
    lea Lglob_0(%rip), %rax
    mov %rax, Lglob_2(%rip)
    mov Lglob_0(%rip), %rax
    push %rax
    lea Lstr_0(%rip), %rax
    push %rax
    pop %rdi
    pop %rsi
    call printf
    mov Lglob_1(%rip), %rax
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
Lglob_0: .quad 0
Lglob_1: .quad 0
Lglob_2: .quad 0
.section .text
