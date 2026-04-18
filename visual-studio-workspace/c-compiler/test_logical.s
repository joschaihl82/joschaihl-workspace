.data
NULL:
  .zero 8
.globl g
.data
g:
  .zero 4
.globl inc
.text
inc:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  leaq g(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  leaq g(%rip), %rax
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
.globl main
.text
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movq $0, %rax
  movq %rax, %r11
  leaq g(%rip), %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq $0, %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $0
  popq %rax
  cmpq $0, %rax
  je .Lfalse1
  movq $0, %rax
  call inc
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse1
  pushq $1
  jmp .Lend1
.Lfalse1:
  pushq $0
.Lend1:
  popq %rax
  cmpq $0, %rax
  je .Lend0
  movq $1, %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend0:
  pushq $1
  popq %rax
  cmpq $0, %rax
  jne .Ltrue3
  movq $0, %rax
  call inc
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  jne .Ltrue3
  pushq $0
  jmp .Lend3
.Ltrue3:
  pushq $1
.Lend3:
  popq %rax
  cmpq $0, %rax
  je .Lend2
  movq $2, %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
  addq %r10, %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend2:
  pushq $1
  popq %rax
  cmpq $0, %rax
  je .Lfalse5
  movq $0, %rax
  call inc
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse5
  pushq $1
  jmp .Lend5
.Lfalse5:
  pushq $0
.Lend5:
  popq %rax
  cmpq $0, %rax
  je .Lend4
  movq $4, %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
  addq %r10, %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend4:
  pushq $0
  popq %rax
  cmpq $0, %rax
  jne .Ltrue7
  movq $0, %rax
  call inc
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  jne .Ltrue7
  pushq $0
  jmp .Lend7
.Ltrue7:
  pushq $1
.Lend7:
  popq %rax
  cmpq $0, %rax
  je .Lend6
  movq $8, %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
  addq %r10, %rax
  movq %rax, %r11
  movq %rbp, %rax
  subq $4, %rax
  movl %r11d, (%rax)
  movl %r11d, %eax
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend6:
  leaq g(%rip), %rax
  movslq (%rax), %rax
  movq %rax, %r10
  movq %rbp, %rax
  subq $4, %rax
  movslq (%rax), %rax
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
