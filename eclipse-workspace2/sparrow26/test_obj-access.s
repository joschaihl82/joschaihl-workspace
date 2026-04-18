.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    lea Lstr_0(%rip), %rax
    mov %rax, Lglob_0(%rip)
    mov $30, %rax
    mov %rax, Lglob_1(%rip)
    lea Lglob_0(%rip), %rax
    mov %rax, Lglob_2(%rip)
    lea Lglob_1(%rip), %rax
    push %rax
    lea Lglob_1(%rip), %rax
    mov (%rax), %rax
    push %rax
    mov $1, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    lea Lglob_1(%rip), %rax
    mov (%rax), %rax
    mov %rax, %rsi
    lea Lstr_1(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    add $8, %rsp
.Lfunc_end_main:
    mov $0, %edi
    call exit
    pop %rbp
    ret
.section .rodata
Lfmt_int: .asciz "%ld\n"
Lstr_0: .asciz "alice"
Lstr_1: .asciz "%ld\n"
.section .data
Lglob_0: .quad 0
Lglob_1: .quad 0
Lglob_2: .quad 0
.section .text
