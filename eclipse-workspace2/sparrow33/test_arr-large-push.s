.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    sub $8, %rsp
    movq $8, Lglob_1(%rip)
    movq $0, Lglob_2(%rip)
    mov $8, %rdi
    shl $3, %rdi
    call malloc
    mov %rax, Lglob_0(%rip)
    mov $0, %rax
    mov %rax, Lglob_3(%rip)
.Lfor_begin_1:
    mov Lglob_3(%rip), %rax
    push %rax
    mov $10000, %rax
    mov %rax, %rbx
    pop %rax
    cmp %rbx, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je .Lfor_end_1
    mov Lglob_2(%rip), %r10
    mov Lglob_1(%rip), %r11
    mov Lglob_3(%rip), %rax
    push %rax
    mov $100, %rax
    mov %rax, %rbx
    pop %rax
    add %rbx, %rax
    push %rax
    cmp %r11, %r10
    jne .Lpush_store_2
.Lpush_realloc_2:
    mov $2, %r8
    imul %r8, %r11
    mov Lglob_0(%rip), %rdi
    mov %r11, %rsi
    shl $3, %rsi
    call realloc
    cmp $0, %rax
    je .Lrealloc_fail_2
    mov %rax, Lglob_0(%rip)
    mov %r11, Lglob_1(%rip)
    jmp .Lpush_store_2
.Lpush_store_2:
    mov Lglob_0(%rip), %r8
    mov %r10, %r9
    shl $3, %r9
    add %r8, %r9
    pop %rbx
    mov %rbx, (%r9)
    inc %r10
    mov %r10, Lglob_2(%rip)
    mov $0, %rax
    jmp .Lpush_end_2
.Lrealloc_fail_2:
    mov $255, %edi
    call exit
.Lpush_end_2:
    lea Lglob_3(%rip), %rax
    push %rax
    mov Lglob_3(%rip), %rax
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
    mov Lglob_2(%rip), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    mov $1, %rax
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
    mov $9999, %rax
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
