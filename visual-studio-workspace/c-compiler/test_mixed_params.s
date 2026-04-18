.data
NULL:
  .zero 8
.globl mixed_params
.text
mixed_params:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movb %dil, -1(%rbp)
  movl %esi, -5(%rbp)
  movb %dl, -6(%rbp)
  movl %ecx, -10(%rbp)
  movw %r8w, -12(%rbp)
  movl %r9d, -16(%rbp)
  movq %rbp, %rax
  subq $23, %rax
  movswl (%rax), %eax
  movq %rax, %r10
  movq %rbp, %rax
  subq $21, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $17, %rax
  movsbq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $16, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $12, %rax
  movswl (%rax), %eax
  movq %rax, %r11
  movq %rbp, %rax
  subq $10, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $6, %rax
  movsbq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $5, %rax
  movslq (%rax), %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $1, %rax
  movsbq (%rax), %rax
  addq %r11, %rax
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
  pushq $9
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  subq $8, %rsp
  movq $0, %rax
  call mixed_params
  addq $8, %rsp
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
