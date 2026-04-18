.data
NULL:
  .zero 8
.globl main
.text
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  movslq (%rax), %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
  andq %r10, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  movq $16, %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $12, %rax
  movslq (%rax), %rax
  orq %r10, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
  notq %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $12, %rax
  movslq (%rax), %rax
  xorq %r10, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $12, %rax
  movslq (%rax), %rax
  movq %r10, %rcx
  salq %cl, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $12, %rax
  movslq (%rax), %rax
  movq %r10, %rcx
  sarq %cl, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
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
