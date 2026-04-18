.section .text
.globl calculate
calculate:
    push %rbp
    mov %rsp, %rbp
    sub $24, %rsp
    mov %rdi, -8(%rbp)
    mov %rsi, -16(%rbp)
    mov -8(%rbp), %rax
    push %rax
    mov -16(%rbp), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, -24(%rbp)
    mov -24(%rbp), %rax
    push %rax
    mov $5, %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setg %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lelse_1
    mov -24(%rbp), %rax
    push %rax
    mov $2, %rax
    mov %rax, %rbx
    pop %rax
    imul %rbx, %rax
    jmp .Lfunc_end_calculate
    jmp .Lend_1
.Lelse_1:
    mov -24(%rbp), %rax
    push %rax
    mov $3, %rax
    mov %rax, %rbx
    pop %rax
    imul %rbx, %rax
    jmp .Lfunc_end_calculate
.Lend_1:
.Lfunc_end_calculate:
    mov %rbp, %rsp
    pop %rbp
    ret
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    mov $0, %rax
    mov %rax, Lglob_0(%rip)
    mov $10, %rax
    mov %rax, Lglob_1(%rip)
    mov $20, %rax
    mov %rax, Lglob_2(%rip)
    lea Lglob_1(%rip), %rax
    mov %rax, Lglob_3(%rip)
    mov $1, %rax
    mov %rax, Lglob_4(%rip)
    mov $5, %rax
    mov %rax, Lglob_5(%rip)
    lea Lglob_4(%rip), %rax
    mov %rax, Lglob_6(%rip)
    lea Lglob_0(%rip), %rax
    push %rax
    lea Lglob_4(%rip), %rax
    mov (%rax), %rax
    push %rax
    mov $0, %rax
    shl $3, %rax
    push %rax
    mov Lglob_3(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
    push %rax
    pop %rdi
    pop %rsi
    xor %rax, %rax
    call calculate
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    mov Lglob_0(%rip), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    mov $0, %rax
    mov %rax, Lglob_7(%rip)
.Lwhile_begin_2:
    mov Lglob_7(%rip), %rax
    push %rax
    lea Lglob_5(%rip), %rax
    mov (%rax), %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lwhile_end_2
    lea Lglob_0(%rip), %rax
    push %rax
    mov Lglob_0(%rip), %rax
    push %rax
    mov Lglob_7(%rip), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    lea Lglob_7(%rip), %rax
    push %rax
    mov Lglob_7(%rip), %rax
    push %rax
    mov $1, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    jmp .Lwhile_begin_2
.Lwhile_end_2:
    mov Lglob_0(%rip), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    mov $0, %rax
    mov %rax, Lglob_8(%rip)
    mov $0, %rax
    mov %rax, Lglob_9(%rip)
.Lfor_begin_3:
    mov Lglob_9(%rip), %rax
    push %rax
    mov $3, %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lfor_end_3
    lea Lglob_8(%rip), %rax
    push %rax
    mov Lglob_8(%rip), %rax
    push %rax
    mov Lglob_9(%rip), %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    lea Lglob_9(%rip), %rax
    push %rax
    mov Lglob_9(%rip), %rax
    push %rax
    mov $1, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    mov %rax, %rbx
    pop %rax
    mov %rbx, (%rax)
    mov %rbx, %rax
    jmp .Lfor_begin_3
.Lfor_end_3:
    mov Lglob_8(%rip), %rax
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
Lglob_5: .quad 0
Lglob_6: .quad 0
Lglob_7: .quad 0
Lglob_8: .quad 0
Lglob_9: .quad 0
.section .text
