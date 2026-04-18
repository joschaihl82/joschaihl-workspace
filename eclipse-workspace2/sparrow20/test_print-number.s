.intel_syntax noprefix
.section .text
.globl main
main:
    push %rbp
    mov %rsp, %rbp
    mov $42, %rax
    mov $0, %edi
    call exit
    pop %rbp
    ret
.section .rodata
.section .data
.section .text
.globl js_print_value
js_print_value:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, %rsi
    mov $fmt_try_string, %rdi
    call printf
    pop %rbp
    ret
fmt_try_string: .asciz "fmt_try_string: .asciz "%s""
.globl js_print_newline
js_print_newline:
    push %rbp
    mov %rsp, %rbp
    mov $fmt_nl, %rdi
    call printf
    pop %rbp
    ret
fmt_nl: .asciz "\n"
