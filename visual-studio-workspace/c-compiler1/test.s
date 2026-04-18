.data
NULL:
  .zero 8
.globl longus
.text
longus:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl main
.text
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call longus
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
