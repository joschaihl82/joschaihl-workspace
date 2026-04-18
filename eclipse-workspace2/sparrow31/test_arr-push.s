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
    mov %rax, Lglob_10(%rip)
    mov $2, Lglob_11(%rip)
    mov Lglob_11(%rip), %rcx
    cmp $10, %rcx
    jge .Lpush_overflow_1
    mov $30, %rax
    push %rax
    mov Lglob_10(%rip), %rax
    mov %rax, %r8
    pop %rax
    mov %rcx, %r9
    shl $3, %r9
    add %r8, %r9
    mov (%r9), %r10
    mov %rax, (%r9)
    mov Lglob_11(%rip), %r10
    inc %r10
    mov %r10, Lglob_11(%rip)
    mov $0, %rax
    jmp .Lpush_end_1
.Lpush_overflow_1:
    mov $255, %edi
    call exit
.Lpush_end_1:
    mov Lglob_11(%rip), %rcx
    cmp $10, %rcx
    jge .Lpush_overflow_2
    mov $40, %rax
    push %rax
    mov Lglob_10(%rip), %rax
    mov %rax, %r8
    pop %rax
    mov %rcx, %r9
    shl $3, %r9
    add %r8, %r9
    mov (%r9), %r10
    mov %rax, (%r9)
    mov Lglob_11(%rip), %r10
    inc %r10
    mov %r10, Lglob_11(%rip)
    mov $0, %rax
    jmp .Lpush_end_2
.Lpush_overflow_2:
    mov $255, %edi
    call exit
.Lpush_end_2:
    mov Lglob_11(%rip), %rcx
    cmp $10, %rcx
    jge .Lpush_overflow_3
    mov $50, %rax
    push %rax
    mov Lglob_10(%rip), %rax
    mov %rax, %r8
    pop %rax
    mov %rcx, %r9
    shl $3, %r9
    add %r8, %r9
    mov (%r9), %r10
    mov %rax, (%r9)
    mov Lglob_11(%rip), %r10
    inc %r10
    mov %r10, Lglob_11(%rip)
    mov $0, %rax
    jmp .Lpush_end_3
.Lpush_overflow_3:
    mov $255, %edi
    call exit
.Lpush_end_3:
    mov Lglob_11(%rip), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    mov $2, %rax
    shl $3, %rax
    push %rax
    mov Lglob_10(%rip), %rax
    pop %rbx
    add %rbx, %rax
    mov (%rax), %rax
    mov %rax, %rsi
    lea Lstr_0(%rip), %rax
    mov %rax, %rdi
    xor %rax, %rax
    call printf
    mov $4, %rax
    shl $3, %rax
    push %rax
    mov Lglob_10(%rip), %rax
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
Lglob_5: .quad 0
Lglob_6: .quad 0
Lglob_7: .quad 0
Lglob_8: .quad 0
Lglob_9: .quad 0
Lglob_10: .quad 0
Lglob_11: .quad 0
.section .text
