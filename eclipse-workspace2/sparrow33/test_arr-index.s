.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    movq $3, Lglob_1(%rip)
    movq $3, Lglob_2(%rip)
    mov $3, %rdi
    shl $3, %rdi
    call malloc
    mov %rax, Lglob_0(%rip)
    push %rax
    mov $100, %rax
    mov %rax, %rbx
    pop %rax
    mov %rax, %r8
    mov $0, %r9
    shl $3, %r9
    add %r8, %r9
    mov %rbx, (%r9)
    push %rax
    mov $200, %rax
    mov %rax, %rbx
    pop %rax
    mov %rax, %r8
    mov $1, %r9
    shl $3, %r9
    add %r8, %r9
    mov %rbx, (%r9)
    push %rax
    mov $300, %rax
    mov %rax, %rbx
    pop %rax
    mov %rax, %r8
    mov $2, %r9
    shl $3, %r9
    add %r8, %r9
    mov %rbx, (%r9)
    mov $1, %rax
    mov %rax, Lglob_3(%rip)
    mov Lglob_3(%rip), %rax
    shl $3, %rax
    push %rax
    mov Lglob_0(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    mov $0, %rax
    shl $3, %rax
    push %rax
    mov Lglob_0(%rip), %rax
    pop %rbx
    add %rbx, %rax
    push %rax
    mov $1, %rax
    shl $3, %rax
    push %rax
    mov Lglob_0(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
    push %rax
    mov $2, %rax
    shl $3, %rax
    push %rax
    mov Lglob_0(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    mov $0, %rax
    shl $3, %rax
    push %rax
    mov Lglob_0(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
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
Lglob_2: .quad 0
Lglob_3: .quad 0
.section .text
