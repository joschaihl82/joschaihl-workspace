.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    mov $10, %rax
    mov %rax, Lglob_0(%rip)
    mov $5, %rax
    mov %rax, Lglob_1(%rip)
    mov Lglob_0(%rip), %rax
    push %rax
    mov Lglob_1(%rip), %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setg %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lelse_1
    lea Lglob_1(%rip), %rax
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
    jmp .Lend_1
.Lelse_1:
    lea Lglob_1(%rip), %rax
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
    jmp .Lend_1
.Lend_1:
    mov Lglob_1(%rip), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    mov Lglob_1(%rip), %rax
    push %rax
    mov $10, %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setle %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lelse_2
    lea Lglob_1(%rip), %rax
    push %rax
    mov $0, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    jmp .Lend_2
.Lelse_2:
    lea Lglob_1(%rip), %rax
    push %rax
    mov $20, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    jmp .Lend_2
.Lend_2:
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
