.data
NULL:
  .zero 8
.globl add_many
.text
add_many:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movl %edi, -4(%rbp)
  movl %esi, -8(%rbp)
  movl %edx, -12(%rbp)
  movl %ecx, -16(%rbp)
  movl %r8d, -20(%rbp)
  movl %r9d, -24(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  movslq (%rax), %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $28, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $24, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $20, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $16, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $12, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $8, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
  addq %r11, %rax
  addq %r11, %rax
  addq %r11, %rax
  addq %r11, %rax
  addq %r11, %rax
  addq %r11, %rax
  addq %r10, %rax
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
  pushq $1
  pushq $2
  pushq $3
  pushq $4
  pushq $5
  pushq $6
  pushq $7
  pushq $8
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call add_many
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
