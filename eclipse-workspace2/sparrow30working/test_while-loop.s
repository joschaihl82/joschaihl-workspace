.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    mov $0, %rax
    mov %rax, Lglob_0(%rip)
    mov $0, %rax
    mov %rax, Lglob_1(%rip)
.Lwhile_begin_1:
    mov Lglob_0(%rip), %rax
    push %rax
    mov $5, %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lwhile_end_1
    lea Lglob_1(%rip), %rax
    push %rax
    mov Lglob_1(%rip), %rax
    push %rax
    mov Lglob_0(%rip), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    lea Lglob_0(%rip), %rax
    push %rax
    mov Lglob_0(%rip), %rax
    push %rax
    mov $1, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    jmp .Lwhile_begin_1
.Lwhile_end_1:
    mov Lglob_1(%rip), %rax
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
Lglob_1: .quad 0
.section .text
