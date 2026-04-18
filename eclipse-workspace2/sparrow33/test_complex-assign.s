.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    mov $10, %rax
    mov %rax, Lglob_0(%rip)
    mov $20, %rax
    mov %rax, Lglob_1(%rip)
    lea Lglob_0(%rip), %rax
    mov %rax, Lglob_2(%rip)
    mov $1, %rax
    mov %rax, Lglob_3(%rip)
    lea Lglob_3(%rip), %rax
    mov %rax, Lglob_4(%rip)
    mov $1, %rax
    shl $3, %rax
    push %rax
    mov Lglob_2(%rip), %rax
    pop %rbx
    add %rbx, %rax
    push %rax
    lea Lglob_3(%rip), %rax
    mov (%rax), %rax
    push %rax
    mov $5, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    mov $1, %rax
    shl $3, %rax
    push %rax
    mov Lglob_2(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    lea Lglob_3(%rip), %rax
    push %rax
    mov $99, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    mov $0, %rax
    shl $3, %rax
    push %rax
    mov Lglob_2(%rip), %rax
    pop %rbx
    add %rbx, %rax
    push %rax
    mov $1, %rax
    shl $3, %rax
    push %rax
    mov Lglob_2(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
    push %rax
    lea Lglob_3(%rip), %rax
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
    mov Lglob_2(%rip), %rax
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
Lglob_4: .quad 0
.section .text
