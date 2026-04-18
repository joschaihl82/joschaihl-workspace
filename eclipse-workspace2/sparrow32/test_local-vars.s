.section .text
.globl foo
foo:
    push %rbp
    mov %rsp, %rbp
    sub $40, %rsp
    mov %rdi, -8(%rbp)
    mov %rsi, -16(%rbp)
    mov $5, %rax
    mov %rax, -24(%rbp)
    lea -24(%rbp), %rax
    push %rax
    mov -24(%rbp), %rax
    push %rax
    mov -8(%rbp), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    mov $1, %rax
    mov %rax, -32(%rbp)
    lea -32(%rbp), %rax
    push %rax
    mov -32(%rbp), %rax
    push %rax
    mov -16(%rbp), %rax
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
    sub %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    mov -24(%rbp), %rax
    push %rax
    mov $10, %rax
    mov %rax, %rbx
    pop %rax
    imul %rbx, %rax
    push %rax
    mov -32(%rbp), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    push %rax
    mov Lglob_0(%rip), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    jmp .Lfunc_end_foo
.Lfunc_end_foo:
    mov %rbp, %rsp
    pop %rbp
    ret
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    mov $100, %rax
    mov %rax, Lglob_0(%rip)
    mov $2, %rax
    push %rax
    mov $1, %rax
    push %rax
    pop %rdi
    pop %rsi
    xor %rax, %rax
    call foo
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
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
