.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    mov $0, %rax
    mov %rax, Lglob_0(%rip)
.Lwhile_begin_1:
    mov Lglob_0(%rip), %rax
    push %rax
    mov $3, %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lwhile_end_1
    mov Lglob_0(%rip), %rax
    push %rax
    lea Lstr_0(%rip), %rax
    push %rax
    pop %rdi
    pop %rsi
    call printf
    mov Lglob_0(%rip), %rax
    push %rax
    mov $1, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, Lglob_0(%rip)
    jmp .Lwhile_begin_1
.Lwhile_end_1:
    mov $0, %edi
    call exit
    pop %rbp
    ret
.section .rodata
Lfmt_int: .asciz "%ld\n"
Lstr_0: .asciz "%ld\n"
.section .data
Lglob_0: .quad 0
.section .text
