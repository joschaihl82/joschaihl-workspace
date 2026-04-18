.data
NULL:
  .zero 8
.data
.LC0:
  .string "hello"
.globl main
.text
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq $5, %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC0(%rip), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $12, %rax
  movq %r11, (%rax)
  movq %r11, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $16, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
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
