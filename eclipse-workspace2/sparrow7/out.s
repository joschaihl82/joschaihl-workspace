.text
.text
.globl show_whole
show_whole:
    push %rbp
    mov %rsp, %rbp
    mov $0, %rax
    push %rax
    leaq .Lstr0(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    pop %rsi
    movq %rsi, %xmm0
    xor %rax, %rax
    call printf
    leave
    ret
.text
.globl show_fields
show_fields:
    push %rbp
    mov %rsp, %rbp
    mov $0, %rax
    push %rax
    leaq .Lstr1(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    pop %rsi
    movq %rsi, %xmm0
    xor %rax, %rax
    call printf
    mov $0, %rax
    push %rax
    leaq .Lstr2(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    pop %rsi
    movq %rsi, %xmm0
    xor %rax, %rax
    call printf
    mov $0, %rax
    cmp $0, %rax
    je .Lelse67946
    leaq .Lstr3(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    xor %rax, %rax
    call printf
    jmp .Lend448844
.Lelse67946:
    leaq .Lstr4(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    xor %rax, %rax
    call printf
.Lend448844:
    mov $0, %rax
    push %rax
    leaq .Lstr5(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    pop %rsi
    movq %rsi, %xmm0
    xor %rax, %rax
    call printf
    leave
    ret
.globl main
main:
.data
.globl data
.align 8
data: .quad .Lstr6
.data
.globl data_name
.align 8
data_name:
    .quad .Lstr7
.data
.globl data_age
.align 8
data_age:
    .quad 34
.data
.globl data_active
.align 8
data_active:
    .quad 1
.data
.globl data_tags
.align 8
data_tags:
    .quad .Lstr8
    leaq .Lstr9(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    xor %rax, %rax
    call printf
    leaq .Lstr10(%rip), %rax
    push %rax
    pop %rdi
    movq %rdi, %xmm0
    xor %rax, %rax
    call printf
    mov $0, %rax
    mov $60, %rax
    xor %rdi, %rdi
    syscall
.section .rodata
.Lstr10: .string "Demo end\n"
.Lstr9: .string "Demo start\n"
.Lstr8: .string "[\"dev\",\"c\",\"sparrow\"]"
.Lstr7: .string "Josh"
.Lstr6: .string "{\"name\":\"Josh\",\"age\":34,\"active\":true,\"tags\":[\"dev\",\"c\",\"sparrow\"]}"
.Lstr5: .string "Tags JSON: %s\n"
.Lstr4: .string "Active: false\n"
.Lstr3: .string "Active: true\n"
.Lstr2: .string "Age: %d"
.Lstr1: .string "Name: %s\n"
.Lstr0: .string "Full JSON: %s\n"

/* --- runtime stubs --- */
.section .text
.globl js_print_value
js_print_value:
    /* rdi = value. Try to guess type: small int vs pointer? */
    /* Heuristic: if value < 0x100000, treat as int, else string */
    cmp $0x100000, %rdi
    jb .print_int
    /* It's a pointer (string) */
    mov %rdi, %rsi
    lea .Lfmt_s(%rip), %rdi
    xor %rax, %rax
    call printf
    ret
.print_int:
    mov %rdi, %rsi
    lea .Lfmt_d(%rip), %rdi
    xor %rax, %rax
    call printf
    ret

.globl js_print_space
js_print_space:
    lea .Lfmt_sp(%rip), %rdi
    xor %rax, %rax
    call printf
    ret

.globl js_print_newline
js_print_newline:
    lea .Lfmt_nl(%rip), %rdi
    xor %rax, %rax
    call printf
    ret

.section .rodata
.Lfmt_s: .string "%s"
.Lfmt_d: .string "%lld"
.Lfmt_sp: .string " "
.Lfmt_nl: .string "\n"
.section .text
