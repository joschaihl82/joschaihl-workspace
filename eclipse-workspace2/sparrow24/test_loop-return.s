.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
.globl sum
sum:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    mov %rdi, -8(%rbp)
    mov $0, %rax
    mov %rax, -16(%rbp)
