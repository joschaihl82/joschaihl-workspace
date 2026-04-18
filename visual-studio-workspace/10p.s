.data
NULL:
  .zero 8
.globl add_ten
.text
add_ten:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  movl %edi, -4(%rbp)
  movl %esi, -8(%rbp)
  movl %edx, -12(%rbp)
  movl %ecx, -16(%rbp)
  movl %r8d, -20(%rbp)
  movl %r9d, -24(%rbp)
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  pushq $1
  pushq $2
  pushq $3
  pushq $4
  pushq $5
  pushq $6
  pushq $7
  pushq $8
  pushq $9
  pushq $10
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call add_ten
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
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
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call printf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
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
