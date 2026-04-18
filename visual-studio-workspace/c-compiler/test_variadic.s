.data
NULL:
  .zero 8
.globl my_func
.text
my_func:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  pushq %r9
  pushq %r8
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %rdi
  movl %edi, -4(%rbp)
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC0:
  .string "result: %d\n"
.globl main
.text
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  pushq $42
  pushq $1
  pushq $2
  pushq $3
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call my_func
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC0(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq $0, %rax
  call printf
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.section .note.GNU-stack,"",@progbits
