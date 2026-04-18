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
  subq $8, %rsp
  leaq .LC0(%rip), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $6, %rax
  movq %rbp, %rax
  subq $6, %rax
  pushq %rax
  leaq .LC0(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq 24(%rsp), %rax
  popq %r10
  movl %r10d, 2(%rax)
  popq %r10
  movb %r10b, 1(%rax)
  popq %r10
  movb %r10b, 0(%rax)
  popq %rax
  pushq $0
  popq %rax
  movq $4, %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $6, %rax
  pushq %rax
  popq %rax
  addq %r10, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rax, %r10
  movq $0, %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $6, %rax
  pushq %rax
  popq %rax
  addq %r11, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  addq %r10, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.section .note.GNU-stack,"",@progbits
