.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
.globl sum
sum:
    push %rbp
    mov %rsp, %rbp
    sub $24, %rsp
    mov %rdi, -8(%rbp)
    mov $0, %rax
    mov %rax, -16(%rbp)
    mov $0, %rax
    mov %rax, -24(%rbp)
.Lfor_begin_1:
    mov -24(%rbp), %rax
    push %rax
    mov -8(%rbp), %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lfor_end_1
    lea -16(%rbp), %rax
    push %rax
    mov -16(%rbp), %rax
    push %rax
    mov -24(%rbp), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    lea -24(%rbp), %rax
    push %rax
    mov -24(%rbp), %rax
    push %rax
    mov $1, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    jmp .Lfor_begin_1
.Lfor_end_1:
    mov -16(%rbp), %rax
    jmp .Lfunc_end_sum
.Lfunc_end_sum:
    mov $0, %rax
    mov %rbp, %rsp
    pop %rbp
    ret
    mov $10, %rax
    push %rax
    pop %rdi
    xor %rax, %rax
    call sum
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
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
Lstr_0: .asciz "%ld\n"
.section .data
.section .text
