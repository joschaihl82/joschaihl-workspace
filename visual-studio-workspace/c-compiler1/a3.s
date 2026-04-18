.data
NULL:
  .zero 8
.globl new_type
.text
new_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movl %edi, -4(%rbp)
  movq %rsi, -12(%rbp)
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $40
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_struct_type
.text
new_struct_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movb %sil, -9(%rbp)
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  pushq $6
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $1
  pushq $40
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl align
.text
align:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movl %edi, -4(%rbp)
  movl %esi, -8(%rbp)
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
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cqto
  idivq %rdi
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl add_field
.text
add_field:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rdx, -24(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse0
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
  jmp .Lend0
.Lelse0:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
.Lend0:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend1
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
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
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend1:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse2
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
.Lend3:
  jmp .Lend2
.Lelse2:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse4
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
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
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call align
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  call align
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
  jmp .Lend4
.Lelse4:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  call align
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
.Lend4:
.Lend2:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC0:
  .string "sizeof_type: unknown type"
.globl sizeof_type
.text
sizeof_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lcase1_0
  cmpq $1, %rax
  je .Lcase1_1
  cmpq $2, %rax
  je .Lcase1_2
  cmpq $3, %rax
  je .Lcase1_3
  cmpq $4, %rax
  je .Lcase1_4
  cmpq $5, %rax
  je .Lcase1_5
  cmpq $6, %rax
  je .Lcase1_6
  cmpq $9, %rax
  je .Lcase1_9
  cmpq $10, %rax
  je .Lcase1_10
.Lcase1_0:
  pushq $4
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_1:
  pushq $2
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_2:
  pushq $8
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_3:
  pushq $8
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_4:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_5:
  pushq $1
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_6:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_9:
  pushq $1
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_10:
  pushq $24
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lbreak1:
  leaq .LC0(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.globl user_input
.data
user_input:
  .zero 8
.globl filename
.data
filename:
  .zero 8
.globl dir_name
.data
dir_name:
  .zero 8
.globl current_stack_size
.data
current_stack_size:
  .zero 4
.globl arg_count
.data
arg_count:
  .zero 4
.globl new_string
.text
new_string:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movl %esi, -12(%rbp)
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $16
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
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
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl str_equals
.text
str_equals:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse5
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call memcmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
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
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl str_chr_equals
.text
str_chr_equals:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strlen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse6
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call memcmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse6
  pushq $1
  jmp .Lend6
.Lfalse6:
  pushq $0
.Lend6:
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl once_files_list
.data
once_files_list:
  .zero 8
.globl macros_list
.data
macros_list:
  .zero 8
.globl find_macro
.text
find_macro:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq macros_list(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin7:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend7
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend8
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend8:
.Lcontinue1:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin7
.Lend7:
.Lbreak2:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl remove_newline
.text
remove_newline:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin9:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend9
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse11
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend11
.Lelse11:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend11:
  jmp .Lend10
.Lelse10:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend10:
.Lcontinue2:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin9
.Lend9:
.Lbreak3:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC5:
  .string "#define"
.data
.LC4:
  .string "once"
.data
.LC3:
  .string "#pragma"
.data
.LC2:
  .string "%s/%s"
.data
.LC1:
  .string "#include"
.globl preprocess
.text
preprocess:
  pushq %rbp
  movq %rsp, %rbp
  subq $136, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin12:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend12
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $21
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse13
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  leaq .LC1(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend14
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $19
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  call strndup
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $1
  pushq $512
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $512
  leaq .LC2(%rip), %rax
  pushq %rax
  leaq dir_name(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
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
  call snprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call read_file
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call tokenize
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call preprocess
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq filename(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend15
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin16:
.Lcontinue4:
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend16
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin16
.Lend16:
.Lbreak5:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse17
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend17
.Lelse17:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend17:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend15:
  jmp .Lcontinue3
.Lend14:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  leaq .LC3(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend18
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  leaq .LC4(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_chr_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend19
  movq %rbp, %rax
  subq $88, %rax
  pushq %rax
  leaq once_files_list(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin20:
  movq %rbp, %rax
  subq $88, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend20
  movq %rbp, %rax
  subq $88, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strcmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend21
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend21:
.Lcontinue5:
  movq %rbp, %rax
  subq $88, %rax
  pushq %rax
  movq %rbp, %rax
  subq $88, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin20
.Lend20:
.Lbreak6:
  movq %rbp, %rax
  subq $96, %rax
  pushq %rax
  pushq $1
  pushq $16
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $96, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $96, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq once_files_list(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq once_files_list(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $96, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend19:
  jmp .Lcontinue3
.Lend18:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  leaq .LC5(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend22
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin23:
.Lcontinue6:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse24
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse24
  pushq $1
  jmp .Lend24
.Lfalse24:
  pushq $0
.Lend24:
  popq %rax
  cmpq $0, %rax
  je .Lend23
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin23
.Lend23:
.Lbreak7:
  movq %rbp, %rax
  subq $112, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $112, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq macros_list(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq macros_list(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue3
.Lend22:
  jmp .Lend13
.Lelse13:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend25
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_macro
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend26
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse27
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend27
.Lelse27:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend27:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue3
.Lend26:
  movq %rbp, %rax
  subq $120, %rax
  pushq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin28:
.Lcontinue7:
  movq %rbp, %rax
  subq $120, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend28
  movq %rbp, %rax
  subq $128, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $128, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $120, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call memcpy
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $128, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse29
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $128, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend29
.Lelse29:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $128, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend29:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $128, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $120, %rax
  pushq %rax
  movq %rbp, %rax
  subq $120, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin28
.Lend28:
.Lbreak8:
  jmp .Lcontinue3
.Lend25:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse30
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend30
.Lelse30:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend30:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend13:
.Lcontinue3:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin12
.Lend12:
.Lbreak4:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call remove_newline
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
.globl locals
.data
locals:
  .zero 8
.globl globals
.data
globals:
  .zero 8
.globl enumVals
.data
enumVals:
  .zero 8
.globl structs
.data
structs:
  .zero 8
.globl typedefs
.data
typedefs:
  .zero 8
.globl funcs
.data
funcs:
  .zero 8
.globl find_lvar
.text
find_lvar:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin31:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend31
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend32
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend32:
.Lcontinue8:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin31
.Lend31:
.Lbreak9:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_lvar
.text
new_lvar:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse33
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend33
.Lelse33:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
.Lend33:
  leaq locals(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl find_enum_val
.text
find_enum_val:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq enumVals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin34:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend34
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend35
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend35:
.Lcontinue9:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin34
.Lend34:
.Lbreak10:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_enum_val
.text
new_enum_val:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movl %esi, -12(%rbp)
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq enumVals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
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
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq enumVals(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl find_struct
.text
find_struct:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq structs(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin36:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend36
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend37
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend37:
.Lcontinue10:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin36
.Lend36:
.Lbreak11:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl find_struct_field
.text
find_struct_field:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin38:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend38
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend39
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend39:
.Lcontinue11:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin38
.Lend38:
.Lbreak12:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_gvar
.text
new_gvar:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq globals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq globals(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.globl find_gvar
.text
find_gvar:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq globals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin40:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend40
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend41
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend41:
.Lcontinue12:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin40
.Lend40:
.Lbreak13:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl find_typedef
.text
find_typedef:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq typedefs(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin42:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend42
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend43
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend43:
.Lcontinue13:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin42
.Lend42:
.Lbreak14:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_typedef
.text
new_typedef:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq typedefs(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq typedefs(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl find_func
.text
find_func:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq funcs(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin44:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend44
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend45
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend45:
.Lcontinue14:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin44
.Lend44:
.Lbreak15:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_func
.text
new_func:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq funcs(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq funcs(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl consume
.text
consume:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue47
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_chr_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue47
  pushq $0
  jmp .Lend47
.Ltrue47:
  pushq $1
.Lend47:
  popq %rax
  cmpq $0, %rax
  je .Lend46
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend46:
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $1
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl consume_kind
.text
consume_kind:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movl %edi, -4(%rbp)
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend48
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend48:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl consume_ident
.text
consume_ident:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend49
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend49:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl check_kind
.text
check_kind:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movl %edi, -4(%rbp)
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl go_to
.text
go_to:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  leaq token(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC6:
  .string "token mismatch: expected %s"
.globl expect
.text
expect:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue51
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_chr_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue51
  pushq $0
  jmp .Lend51
.Ltrue51:
  pushq $1
.Lend51:
  popq %rax
  cmpq $0, %rax
  je .Lend50
  leaq .LC6(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend50:
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC7:
  .string "token mismatch"
.globl expect_kind
.text
expect_kind:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movl %edi, -4(%rbp)
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend52
  leaq .LC7(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend52:
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC8:
  .string "token mismatch: expected number"
.globl expect_number
.text
expect_number:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $3
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend53
  leaq .LC8(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend53:
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
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
.globl at_eof
.text
at_eof:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $17
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC18:
  .string "expected struct name"
.data
.LC17:
  .string "struct %.*s is already defined"
.data
.LC16:
  .string ";"
.data
.LC15:
  .string "expected type"
.data
.LC14:
  .string "}"
.data
.LC13:
  .string "{"
.data
.LC12:
  .string ","
.data
.LC11:
  .string "expected enum name"
.data
.LC10:
  .string "}"
.data
.LC9:
  .string "{"
.globl consume_type_name
.text
consume_type_name:
  pushq %rbp
  movq %rsp, %rbp
  subq $88, %rsp
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  pushq $13
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend54
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
.Lend54:
  pushq $9
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend55
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  pushq $0
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend55:
  pushq $11
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend56
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  pushq $1
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend56:
  pushq $12
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend57
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  pushq $2
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend57:
  pushq $10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend58
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  pushq $5
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend58:
  pushq $28
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend59
  pushq $7
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend59:
  pushq $29
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend60
  pushq $9
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend60:
  pushq $30
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend61
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  pushq $4
  pushq $10
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend61:
  pushq $14
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend62
  pushq $2
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  leaq .LC9(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend63
  movq %rbp, %rax
  subq $21, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin64:
.Lcontinue15:
  leaq .LC10(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend64
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_ident
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend65
  leaq .LC11(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend65:
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $21, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_enum_val
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  leaq .LC12(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  jmp .Lbegin64
.Lend64:
.Lbreak16:
.Lend63:
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  pushq $0
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend62:
  pushq $15
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call check_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue67
  pushq $16
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call check_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue67
  pushq $0
  jmp .Lend67
.Ltrue67:
  pushq $1
.Lend67:
  popq %rax
  cmpq $0, %rax
  je .Lend66
  movq %rbp, %rax
  subq $30, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  pushq $16
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse68
  movq %rbp, %rax
  subq $30, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend68
.Lelse68:
  pushq $15
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
.Lend68:
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  pushq $2
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC13(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse69
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend70
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend70:
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $30, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_struct_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin71:
.Lcontinue16:
  leaq .LC14(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend71
  movq %rbp, %rax
  subq $46, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $46, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend72
  leaq .LC15(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend72:
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $46, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $46, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call add_field
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC16(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lbegin71
.Lend71:
.Lbreak17:
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend73
  movq %rbp, %rax
  subq $54, %rax
  pushq %rax
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_struct
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $54, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse74
  movq %rbp, %rax
  subq $54, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse75
  leaq .LC17(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lend75
.Lelse75:
  movq %rbp, %rax
  subq $54, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $54, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $54, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $54, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend75:
  jmp .Lend74
.Lelse74:
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq structs(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq structs(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend74:
.Lend73:
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend69
.Lelse69:
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend76
  leaq .LC18(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend76:
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_struct
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse77
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  movq %rbp, %rax
  subq $38, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $30, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_struct_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq structs(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq structs(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend77
.Lelse77:
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  pushq $6
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend77:
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $62, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend69:
.Lend66:
  movq %rbp, %rax
  subq $70, %rax
  pushq %rax
  pushq $2
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $70, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend78
  movq %rbp, %rax
  subq $78, %rax
  pushq %rax
  movq %rbp, %rax
  subq $70, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_typedef
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $78, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend79
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend80
  movq %rbp, %rax
  subq $86, %rax
  pushq %rax
  pushq $1
  pushq $40
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $86, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $78, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
  movq 24(%rax), %r10
  pushq %r10
  movq 32(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq 40(%rsp), %rax
  popq %r10
  movq %r10, 32(%rax)
  popq %r10
  movq %r10, 24(%rax)
  popq %r10
  movq %r10, 16(%rax)
  popq %r10
  movq %r10, 8(%rax)
  popq %r10
  movq %r10, 0(%rax)
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $86, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $86, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend80:
  movq %rbp, %rax
  subq $78, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend79:
  movq %rbp, %rax
  subq $70, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call go_to
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend78:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC29:
  .string "]"
.data
.LC28:
  .string "expected constant expression"
.data
.LC27:
  .string "["
.data
.LC26:
  .string ")"
.data
.LC25:
  .string "..."
.data
.LC24:
  .string ","
.data
.LC23:
  .string "("
.data
.LC22:
  .string "expected identifier"
.data
.LC21:
  .string ")"
.data
.LC20:
  .string "*"
.data
.LC19:
  .string "("
.globl expect_nested_type
.text
expect_nested_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  leaq .LC19(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse81
.Lbegin82:
.Lcontinue17:
  leaq .LC20(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend82
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin82
.Lend82:
.Lbreak18:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect_nested_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC21(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lend81
.Lelse81:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_ident
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend83
  leaq .LC22(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend83:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $16
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend81:
  leaq .LC23(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend84
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_noident_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_ident
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
.Lbegin85:
.Lcontinue18:
  leaq .LC24(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend85
  leaq .LC25(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend86
  jmp .Lbreak19
.Lend86:
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_noident_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_ident
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  jmp .Lbegin85
.Lend85:
.Lbreak19:
  leaq .LC26(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $8
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend84:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin87:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend87
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lcontinue19:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin87
.Lend87:
.Lbreak20:
.Lbegin88:
.Lcontinue20:
  leaq .LC27(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend88
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend89
  leaq .LC28(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend89:
  leaq .LC29(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin88
.Lend88:
.Lbreak21:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC30:
  .string "*"
.globl consume_type
.text
consume_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type_name
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend90
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend90:
.Lbegin91:
.Lcontinue21:
  leaq .LC30(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend91
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin91
.Lend91:
.Lbreak22:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect_nested_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC39:
  .string "]"
.data
.LC38:
  .string "expected constant expression"
.data
.LC37:
  .string "["
.data
.LC36:
  .string ")"
.data
.LC35:
  .string ","
.data
.LC34:
  .string "("
.data
.LC33:
  .string ")"
.data
.LC32:
  .string "("
.data
.LC31:
  .string "*"
.globl expect_noident_type
.text
expect_noident_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
.Lbegin92:
.Lcontinue22:
  leaq .LC31(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend92
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin92
.Lend92:
.Lbreak23:
  leaq .LC32(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend93
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect_noident_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC33(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend93:
  leaq .LC34(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend94
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_noident_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
.Lbegin95:
.Lcontinue23:
  leaq .LC35(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend95
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_noident_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  jmp .Lbegin95
.Lend95:
.Lbreak24:
  leaq .LC36(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $8
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend94:
.Lbegin96:
.Lcontinue24:
  leaq .LC37(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend96
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend97
  leaq .LC38(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend97:
  leaq .LC39(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin96
.Lend96:
.Lbreak25:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl consume_noident_type
.text
consume_noident_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type_name
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend98
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend98:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect_noident_type
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
.globl next
.text
next:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC40:
  .string "internal error"
.globl new_node
.text
new_node:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movl %edi, -4(%rbp)
  movq %rsi, -12(%rbp)
  movq %rdx, -20(%rbp)
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lcase2_0
  cmpq $1, %rax
  je .Lcase2_1
  cmpq $2, %rax
  je .Lcase2_2
  cmpq $3, %rax
  je .Lcase2_3
  cmpq $4, %rax
  je .Lcase2_4
  cmpq $6, %rax
  je .Lcase2_6
  cmpq $7, %rax
  je .Lcase2_7
  cmpq $8, %rax
  je .Lcase2_8
  cmpq $9, %rax
  je .Lcase2_9
  cmpq $10, %rax
  je .Lcase2_10
  cmpq $11, %rax
  je .Lcase2_11
  cmpq $12, %rax
  je .Lcase2_12
  cmpq $13, %rax
  je .Lcase2_13
  cmpq $31, %rax
  je .Lcase2_31
  cmpq $29, %rax
  je .Lcase2_29
  cmpq $14, %rax
  je .Lcase2_14
  cmpq $15, %rax
  je .Lcase2_15
  cmpq $27, %rax
  je .Lcase2_27
  cmpq $28, %rax
  je .Lcase2_28
  jmp .Ldefault2
.Lcase2_0:
.Lcase2_1:
.Lcase2_2:
.Lcase2_3:
.Lcase2_4:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak26
.Lcase2_6:
.Lcase2_7:
.Lcase2_8:
.Lcase2_9:
.Lcase2_10:
.Lcase2_11:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak26
.Lcase2_12:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak26
.Lcase2_13:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $3
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak26
.Lcase2_31:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $3
  pushq $5
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak26
.Lcase2_29:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak26
.Lcase2_14:
.Lcase2_15:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak26
.Lcase2_27:
.Lcase2_28:
  leaq .LC40(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lbreak26
.Ldefault2:
  jmp .Lbreak26
.Lbreak26:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_typed_node
.text
new_typed_node:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movl %edi, -4(%rbp)
  movq %rsi, -12(%rbp)
  movq %rdx, -20(%rbp)
  movq %rcx, -28(%rbp)
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_node_num
.text
new_node_num:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movl %edi, -4(%rbp)
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $40
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_node_char
.text
new_node_char:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movb %dil, -1(%rbp)
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  pushq $1
  pushq $40
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $5
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl ext
.data
ext:
  .zero 8
.globl literal_count
.data
literal_count:
  .zero 4
.data
.LC57:
  .string ";"
.data
.LC56:
  .string "expected block"
.data
.LC55:
  .string ";"
.data
.LC54:
  .string ")"
.data
.LC53:
  .string ","
.data
.LC52:
  .string "failed to parse argument"
.data
.LC51:
  .string "..."
.data
.LC50:
  .string ")"
.data
.LC49:
  .string ")"
.data
.LC48:
  .string "("
.data
.LC47:
  .string "invalid type"
.data
.LC46:
  .string ";"
.data
.LC45:
  .string "expected type"
.data
.LC44:
  .string ";"
.data
.LC43:
  .string "expected struct or union"
.data
.LC42:
  .string ";"
.data
.LC41:
  .string "NULL"
.globl external
.text
external:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  leaq locals(%rip), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq globals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend99
  leaq .LC41(%rip), %rax
  pushq %rax
  pushq $4
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_string
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $3
  pushq $7
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_gvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend99:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $1
  pushq $72
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq ext(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  pushq $31
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend100
  leaq ext(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $69
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
.Lend100:
  pushq $14
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call check_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend101
  leaq ext(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $3
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type_name
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  leaq .LC42(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend101:
  pushq $15
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call check_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue103
  pushq $16
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call check_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue103
  pushq $0
  jmp .Lend103
.Ltrue103:
  pushq $1
.Lend103:
  popq %rax
  cmpq $0, %rax
  je .Lend102
  leaq ext(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type_name
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend104
  leaq .LC43(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend104:
  leaq .LC44(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend102:
  pushq $22
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend105
  leaq ext(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $5
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend106
  leaq .LC45(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend106:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typedef
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  leaq .LC46(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend105:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend107
  leaq .LC47(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend107:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse108
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_func
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend109
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_func
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
.Lend109:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call go_to
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call next
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC48(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $37, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  pushq $28
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend110
  leaq .LC49(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse111
  movq %rbp, %rax
  subq $37, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend111
.Lelse111:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call go_to
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend111:
.Lend110:
  leaq .LC50(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse113
  movq %rbp, %rax
  subq $37, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse113
  pushq $1
  jmp .Lend113
.Lfalse113:
  pushq $0
.Lend113:
  popq %rax
  cmpq $0, %rax
  je .Lend112
.Lbegin114:
  leaq .LC51(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend115
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $68
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak27
.Lend115:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend116
  leaq .LC52(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend116:
  movq %rbp, %rax
  subq $45, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_lvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $45, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend117
  movq %rbp, %rax
  subq $45, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_lvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend117:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $45, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC53(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend118
  jmp .Lbreak27
.Lend118:
.Lcontinue25:
  jmp .Lbegin114
.Lend114:
.Lbreak27:
  leaq .LC54(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend112:
  leaq .LC55(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend119
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend119:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $26
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend120
  leaq .LC56(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend120:
  jmp .Lend108
.Lelse108:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_gvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $48
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
  leaq .LC57(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend108:
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend121
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $64
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  cqto
  idivq %rdi
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend121:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC80:
  .string ";"
.data
.LC79:
  .string ";"
.data
.LC78:
  .string ";"
.data
.LC77:
  .string ";"
.data
.LC76:
  .string ";"
.data
.LC75:
  .string "}"
.data
.LC74:
  .string "{"
.data
.LC73:
  .string ")"
.data
.LC72:
  .string ")"
.data
.LC71:
  .string ";"
.data
.LC70:
  .string ";"
.data
.LC69:
  .string ";"
.data
.LC68:
  .string ";"
.data
.LC67:
  .string "("
.data
.LC66:
  .string ")"
.data
.LC65:
  .string "("
.data
.LC64:
  .string ")"
.data
.LC63:
  .string "("
.data
.LC62:
  .string ")"
.data
.LC61:
  .string "("
.data
.LC60:
  .string ":"
.data
.LC59:
  .string ":"
.data
.LC58:
  .string "expected constant expression"
.globl stmt
.text
stmt:
  pushq %rbp
  movq %rsp, %rbp
  subq $80, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  pushq $24
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse122
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend123
  leaq .LC58(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend123:
  leaq .LC59(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $20
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend122
.Lelse122:
  pushq $25
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse124
  leaq .LC60(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $21
  pushq $0
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend124
.Lelse124:
  pushq $5
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse125
  leaq .LC61(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC62(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $6
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse126
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $18
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $33
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend126
.Lelse126:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $17
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend126:
  jmp .Lend125
.Lelse125:
  pushq $23
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse127
  leaq .LC63(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC64(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $19
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend127
.Lelse127:
  pushq $7
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse128
  leaq .LC65(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC66(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $24
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend128
.Lelse128:
  pushq $8
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse129
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC67(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC68(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend130
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC69(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend130:
  leaq .LC70(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend131
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC71(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend131:
  leaq .LC72(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend132
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC73(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend132:
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $33
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $33
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $25
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend129
.Lelse129:
  leaq .LC74(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse133
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $26
  pushq $0
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin134:
  leaq .LC75(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend134
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $33
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lcontinue26:
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin134
.Lend134:
.Lbreak28:
  jmp .Lend133
.Lelse133:
  pushq $4
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse135
  leaq .LC76(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse136
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $16
  pushq $0
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend136
.Lelse136:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $16
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC77(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend136:
  jmp .Lend135
.Lelse135:
  pushq $26
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse137
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $22
  pushq $0
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC78(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lend137
.Lelse137:
  pushq $27
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse138
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $23
  pushq $0
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC79(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lend138
.Lelse138:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC80(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend138:
.Lend137:
.Lend135:
.Lend133:
.Lend129:
.Lend128:
.Lend127:
.Lend125:
.Lend124:
.Lend122:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl expr
.text
expr:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
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
.data
.LC85:
  .string "/="
.data
.LC84:
  .string "*="
.data
.LC83:
  .string "-="
.data
.LC82:
  .string "+="
.data
.LC81:
  .string "="
.globl assign
.text
assign:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call logical
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC81(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse139
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend139
.Lelse139:
  leaq .LC82(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse140
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend140
.Lelse140:
  leaq .LC83(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse141
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend141
.Lelse141:
  leaq .LC84(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse142
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend142
.Lelse142:
  leaq .LC85(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend143
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend143:
.Lend142:
.Lend141:
.Lend140:
.Lend139:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC87:
  .string "||"
.data
.LC86:
  .string "&&"
.globl logical
.text
logical:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call equality
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin144:
  leaq .LC86(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse145
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call logical
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend145
.Lelse145:
  leaq .LC87(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse146
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $11
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call logical
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend146
.Lelse146:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend146:
.Lend145:
.Lcontinue27:
  jmp .Lbegin144
.Lend144:
.Lbreak29:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC89:
  .string "!="
.data
.LC88:
  .string "=="
.globl equality
.text
equality:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call relational
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin147:
  leaq .LC88(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse148
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $6
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call relational
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend148
.Lelse148:
  leaq .LC89(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse149
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $7
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call relational
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend149
.Lelse149:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend149:
.Lend148:
.Lcontinue28:
  jmp .Lbegin147
.Lend147:
.Lbreak30:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC93:
  .string ">"
.data
.LC92:
  .string ">="
.data
.LC91:
  .string "<"
.data
.LC90:
  .string "<="
.globl relational
.text
relational:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call add
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin150:
  leaq .LC90(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse151
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $9
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call add
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend151
.Lelse151:
  leaq .LC91(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse152
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $8
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call add
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend152
.Lelse152:
  leaq .LC92(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse153
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $9
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call add
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend153
.Lelse153:
  leaq .LC93(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse154
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $8
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call add
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend154
.Lelse154:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend154:
.Lend153:
.Lend152:
.Lend151:
.Lcontinue29:
  jmp .Lbegin150
.Lend150:
.Lbreak31:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC97:
  .string "-"
.data
.LC96:
  .string "+"
.data
.LC95:
  .string "-"
.data
.LC94:
  .string "+"
.globl add
.text
add:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call mul
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin155:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $3
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue158
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue158
  pushq $0
  jmp .Lend158
.Ltrue158:
  pushq $1
.Lend158:
  popq %rax
  cmpq $0, %rax
  je .Lfalse157
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $7
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse157
  pushq $1
  jmp .Lend157
.Lfalse157:
  pushq $0
.Lend157:
  popq %rax
  cmpq $0, %rax
  je .Lend156
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
  leaq .LC94(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse159
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend160
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $12
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  pushq $3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend160:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call mul
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend159
.Lelse159:
  leaq .LC95(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend161
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend162
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $12
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  pushq $3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend162:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $1
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call mul
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend161:
.Lend159:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend156:
  leaq .LC96(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse163
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call mul
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse165
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse165
  pushq $1
  jmp .Lend165
.Lfalse165:
  pushq $0
.Lend165:
  popq %rax
  cmpq $0, %rax
  je .Lelse164
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend164
.Lelse164:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend164:
  jmp .Lend163
.Lelse163:
  leaq .LC97(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse166
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call mul
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse168
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse168
  pushq $1
  jmp .Lend168
.Lfalse168:
  pushq $0
.Lend168:
  popq %rax
  cmpq $0, %rax
  je .Lelse167
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend167
.Lelse167:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $1
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend167:
  jmp .Lend166
.Lelse166:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend166:
.Lend163:
.Lcontinue30:
  jmp .Lbegin155
.Lend155:
.Lbreak32:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC99:
  .string "/"
.data
.LC98:
  .string "*"
.globl mul
.text
mul:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call unary
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin169:
  leaq .LC98(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse170
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call unary
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse172
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse172
  pushq $1
  jmp .Lend172
.Lfalse172:
  pushq $0
.Lend172:
  popq %rax
  cmpq $0, %rax
  je .Lelse171
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend171
.Lelse171:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $2
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend171:
  jmp .Lend170
.Lelse170:
  leaq .LC99(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse173
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call unary
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse175
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse175
  pushq $1
  jmp .Lend175
.Lfalse175:
  pushq $0
.Lend175:
  popq %rax
  cmpq $0, %rax
  je .Lelse174
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cqto
  idivq %rdi
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend174
.Lelse174:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend174:
  jmp .Lend173
.Lelse173:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend173:
.Lend170:
.Lcontinue31:
  jmp .Lbegin169
.Lend169:
.Lbreak33:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC109:
  .string "--"
.data
.LC108:
  .string "++"
.data
.LC107:
  .string "!"
.data
.LC106:
  .string ")"
.data
.LC105:
  .string "("
.data
.LC104:
  .string "dereference failed: not a pointer"
.data
.LC103:
  .string "*"
.data
.LC102:
  .string "&"
.data
.LC101:
  .string "-"
.data
.LC100:
  .string "+"
.globl unary
.text
unary:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  leaq .LC100(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse176
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call postfix
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend176
.Lelse176:
  leaq .LC101(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse177
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call postfix
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse178
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend178
.Lelse178:
  pushq $1
  pushq $0
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend178:
  jmp .Lend177
.Lelse177:
  leaq .LC102(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse179
  pushq $12
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call postfix
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend179
.Lelse179:
  leaq .LC103(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse180
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call primary
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $3
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse182
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse182
  pushq $1
  jmp .Lend182
.Lfalse182:
  pushq $0
.Lend182:
  popq %rax
  cmpq $0, %rax
  je .Lend181
  leaq .LC104(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend181:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend183
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $12
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend183:
  pushq $13
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend180
.Lelse180:
  pushq $18
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse184
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC105(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend185
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_noident_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend186
  leaq .LC106(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend186:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call go_to
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend185:
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call unary
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend184
.Lelse184:
  leaq .LC107(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse187
  pushq $6
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call postfix
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend187
.Lelse187:
  leaq .LC108(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse188
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call postfix
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $4
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend188
.Lelse188:
  leaq .LC109(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend189
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call postfix
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $4
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend189:
.Lend188:
.Lend187:
.Lend184:
.Lend180:
.Lend179:
.Lend177:
.Lend176:
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call postfix
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
.data
.LC122:
  .string "--"
.data
.LC121:
  .string "++"
.data
.LC120:
  .string "no such field"
.data
.LC119:
  .string "expected struct type"
.data
.LC118:
  .string "expected pointer type"
.data
.LC117:
  .string "expected identifier after '->'"
.data
.LC116:
  .string "->"
.data
.LC115:
  .string "no such field"
.data
.LC114:
  .string "expected struct type"
.data
.LC113:
  .string "expected identifier after '.'"
.data
.LC112:
  .string "."
.data
.LC111:
  .string "]"
.data
.LC110:
  .string "["
.globl postfix
.text
postfix:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call primary
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin190:
  leaq .LC110(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend191
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend192
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $12
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  pushq $3
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend192:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $13
  pushq $0
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC111(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lcontinue32
.Lend191:
  leaq .LC112(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend193
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_ident
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend194
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC113(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend194:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $6
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend195
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC114(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend195:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_struct_field
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend196
  leaq .LC115(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend196:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $12
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $13
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue32
.Lend193:
  leaq .LC116(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend197
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_ident
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend198
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC117(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend198:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $3
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend199
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC118(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend199:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $6
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend200
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC119(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend200:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_struct_field
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend201
  leaq .LC120(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_here
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend201:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $13
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue32
.Lend197:
  leaq .LC121(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend202
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $14
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue32
.Lend202:
  leaq .LC122(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend203
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $15
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue32
.Lend203:
  jmp .Lbreak34
.Lcontinue32:
  jmp .Lbegin190
.Lend190:
.Lbreak34:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC136:
  .string "unknown escape sequence"
.data
.LC135:
  .string "%.*s is not defined"
.data
.LC134:
  .string ","
.data
.LC133:
  .string ")"
.data
.LC132:
  .string ")"
.data
.LC131:
  .string "va_start"
.data
.LC130:
  .string "("
.data
.LC129:
  .string "}"
.data
.LC128:
  .string ","
.data
.LC127:
  .string "type mismatch"
.data
.LC126:
  .string "{"
.data
.LC125:
  .string "="
.data
.LC124:
  .string ")"
.data
.LC123:
  .string "("
.globl primary
.text
primary:
  pushq %rbp
  movq %rsp, %rbp
  subq $104, %rsp
  leaq .LC123(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend204
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC124(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend204:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $19
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend205
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq ext(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $56
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq ext(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $56
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $31
  pushq $0
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq literal_count(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend205:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend206
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_lvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend207
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_lvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend207:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $27
  pushq $0
  pushq $0
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC125(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend208
  leaq .LC126(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse209
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend210
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC127(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend210:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $5
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $52, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin211:
  movq %rbp, %rax
  subq $52, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend211
  movq %rbp, %rax
  subq $60, %rax
  pushq %rax
  pushq $0
  pushq $12
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  movq %rbp, %rax
  subq $52, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
  pushq $13
  movq %rbp, %rax
  subq $60, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $33
  pushq $4
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $52, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend212
  leaq .LC128(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend212:
.Lcontinue33:
  movq %rbp, %rax
  subq $52, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin211
.Lend211:
.Lbreak35:
  leaq .LC129(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lend209
.Lelse209:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assign
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend209:
.Lend208:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend206:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_ident
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend213
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  leaq .LC130(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse214
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_func
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse215
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_func
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend215
.Lelse215:
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  pushq $0
  pushq $0
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend215:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $29
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  leaq .LC131(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call str_chr_equals
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend216
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend216:
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC132(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend217
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend217:
.Lbegin218:
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $33
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC133(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse219
  jmp .Lbreak36
  jmp .Lend219
.Lelse219:
  leaq .LC134(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend219:
.Lcontinue34:
  jmp .Lbegin218
.Lend218:
.Lbreak36:
  jmp .Lend214
.Lelse214:
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_lvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse220
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $27
  pushq $0
  pushq $0
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend220
.Lelse220:
  movq %rbp, %rax
  subq $92, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_enum_val
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $92, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse221
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $92, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend221
.Lelse221:
  movq %rbp, %rax
  subq $100, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call find_gvar
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $100, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse222
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $28
  pushq $0
  pushq $0
  movq %rbp, %rax
  subq $100, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_typed_node
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend222
.Lelse222:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC135(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend222:
.Lend221:
.Lend220:
.Lend214:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend213:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $20
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call consume_kind
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend223
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $92
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend224
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $1
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $92, %rax
  je .Lcase3_92
  cmpq $39, %rax
  je .Lcase3_39
  cmpq $110, %rax
  je .Lcase3_110
  cmpq $48, %rax
  je .Lcase3_48
  jmp .Ldefault3
.Lcase3_92:
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  pushq $92
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak37
.Lcase3_39:
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  pushq $39
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak37
.Lcase3_110:
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak37
.Lcase3_48:
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak37
.Ldefault3:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC136(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lbreak37:
.Lend224:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_char
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend223:
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call expect_number
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_node_num
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
.globl token
.data
token:
  .zero 8
.globl new_token
.text
new_token:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movl %edi, -4(%rbp)
  movq %rsi, -12(%rbp)
  movq %rdx, -20(%rbp)
  movl %ecx, -24(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $24, %rax
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
  call new_string
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl is_alnum
.text
is_alnum:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movb %dil, -1(%rbp)
  pushq $97
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse226
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $122
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse226
  pushq $1
  jmp .Lend226
.Lfalse226:
  pushq $0
.Lend226:
  popq %rax
  cmpq $1, %rax
  je .Ltrue225
  pushq $65
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse228
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $90
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse228
  pushq $1
  jmp .Lend228
.Lfalse228:
  pushq $0
.Lend228:
  popq %rax
  cmpq $1, %rax
  je .Ltrue227
  pushq $48
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse230
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $57
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse230
  pushq $1
  jmp .Lend230
.Lfalse230:
  pushq $0
.Lend230:
  popq %rax
  cmpq $1, %rax
  je .Ltrue229
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $95
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue229
  pushq $0
  jmp .Lend229
.Ltrue229:
  pushq $1
.Lend229:
  popq %rax
  cmpq $1, %rax
  je .Ltrue227
  pushq $0
  jmp .Lend227
.Ltrue227:
  pushq $1
.Lend227:
  popq %rax
  cmpq $1, %rax
  je .Ltrue225
  pushq $0
  jmp .Lend225
.Ltrue225:
  pushq $1
.Lend225:
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl startswith
.text
startswith:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strlen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call memcmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC181:
  .string "tokenize failed"
.data
.LC180:
  .string "extern"
.data
.LC179:
  .string "_Bool"
.data
.LC178:
  .string "__builtin_va_list"
.data
.LC177:
  .string "void"
.data
.LC176:
  .string "continue"
.data
.LC175:
  .string "break"
.data
.LC174:
  .string "default"
.data
.LC173:
  .string "case"
.data
.LC172:
  .string "switch"
.data
.LC171:
  .string "typedef"
.data
.LC170:
  .string "union"
.data
.LC169:
  .string "struct"
.data
.LC168:
  .string "enum"
.data
.LC167:
  .string "char"
.data
.LC166:
  .string "sizeof"
.data
.LC165:
  .string "long"
.data
.LC164:
  .string "short"
.data
.LC163:
  .string "unsigned"
.data
.LC162:
  .string "int"
.data
.LC161:
  .string "for"
.data
.LC160:
  .string "while"
.data
.LC159:
  .string "else"
.data
.LC158:
  .string "if"
.data
.LC157:
  .string "return"
.data
.LC156:
  .string "+-*/()<>:;={},&[].!"
.data
.LC155:
  .string "..."
.data
.LC154:
  .string "->"
.data
.LC153:
  .string "tokenize failed: \"*/\" not found"
.data
.LC152:
  .string "*/"
.data
.LC151:
  .string "/*"
.data
.LC150:
  .string "//"
.data
.LC149:
  .string "/="
.data
.LC148:
  .string "*="
.data
.LC147:
  .string "-="
.data
.LC146:
  .string "+="
.data
.LC145:
  .string "--"
.data
.LC144:
  .string "++"
.data
.LC143:
  .string "&&"
.data
.LC142:
  .string "||"
.data
.LC141:
  .string ">="
.data
.LC140:
  .string "<="
.data
.LC139:
  .string "!="
.data
.LC138:
  .string "=="
.data
.LC137:
  .string "#"
.globl tokenize
.text
tokenize:
  pushq %rbp
  movq %rsp, %rbp
  subq $80, %rsp
  movq %rdi, -8(%rbp)
  movb %sil, -9(%rbp)
  movq %rbp, %rax
  subq $41, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
  movq 24(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $41, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  movq %rbp, %rax
  subq $41, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin231:
.Lcontinue35:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend231
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend232
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $1
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  pushq $1
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend232:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call isspace
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend233
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lcontinue35
.Lend233:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC137(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend234
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin235:
.Lcontinue36:
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call isspace
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend235
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin235
.Lend235:
.Lbreak39:
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $21
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend234:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC138(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue237
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC139(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue238
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC140(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue239
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC141(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue239
  pushq $0
  jmp .Lend239
.Ltrue239:
  pushq $1
.Lend239:
  popq %rax
  cmpq $1, %rax
  je .Ltrue238
  pushq $0
  jmp .Lend238
.Ltrue238:
  pushq $1
.Lend238:
  popq %rax
  cmpq $1, %rax
  je .Ltrue237
  pushq $0
  jmp .Lend237
.Ltrue237:
  pushq $1
.Lend237:
  popq %rax
  cmpq $0, %rax
  je .Lend236
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend236:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC142(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue241
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC143(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue241
  pushq $0
  jmp .Lend241
.Ltrue241:
  pushq $1
.Lend241:
  popq %rax
  cmpq $0, %rax
  je .Lend240
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend240:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC144(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue243
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC145(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue243
  pushq $0
  jmp .Lend243
.Ltrue243:
  pushq $1
.Lend243:
  popq %rax
  cmpq $0, %rax
  je .Lend242
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend242:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC146(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue245
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC147(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue246
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC148(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue247
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC149(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue247
  pushq $0
  jmp .Lend247
.Ltrue247:
  pushq $1
.Lend247:
  popq %rax
  cmpq $1, %rax
  je .Ltrue246
  pushq $0
  jmp .Lend246
.Ltrue246:
  pushq $1
.Lend246:
  popq %rax
  cmpq $1, %rax
  je .Ltrue245
  pushq $0
  jmp .Lend245
.Ltrue245:
  pushq $1
.Lend245:
  popq %rax
  cmpq $0, %rax
  je .Lend244
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend244:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC150(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call startswith
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend248
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin249:
.Lcontinue37:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend249
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin249
.Lend249:
.Lbreak40:
  jmp .Lcontinue35
.Lend248:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC151(%rip), %rax
  pushq %rax
  pushq $2
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend250
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC152(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strstr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend251
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC153(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend251:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend250:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC154(%rip), %rax
  pushq %rax
  pushq $2
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend252
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend252:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC155(%rip), %rax
  pushq %rax
  pushq $3
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend253
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend253:
  leaq .LC156(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strchr
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend254
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $0
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  pushq $1
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend254:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $34
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend255
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $19
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  pushq $0
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin256:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $92
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue257
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $34
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue257
  pushq $0
  jmp .Lend257
.Ltrue257:
  pushq $1
.Lend257:
  popq %rax
  cmpq $0, %rax
  je .Lend256
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
.Lcontinue38:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin256
.Lend256:
.Lbreak41:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lcontinue35
.Lend255:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $39
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend258
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $20
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  pushq $0
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin259:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $92
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse261
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $92
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse261
  pushq $1
  jmp .Lend261
.Lfalse261:
  pushq $0
.Lend261:
  popq %rax
  cmpq $1, %rax
  je .Ltrue260
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $39
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue260
  pushq $0
  jmp .Lend260
.Ltrue260:
  pushq $1
.Lend260:
  popq %rax
  cmpq $0, %rax
  je .Lend259
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
.Lcontinue39:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin259
.Lend259:
.Lbreak42:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lcontinue35
.Lend258:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call isdigit
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend262
  movq %rbp, %rax
  subq $65, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $69, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $65, %rax
  pushq %rax
  pushq $10
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strtol
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
  movq %rbp, %rax
  subq $73, %rax
  pushq %rax
  movq %rbp, %rax
  subq $65, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $65, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $3
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $73, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $69, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend262:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC157(%rip), %rax
  pushq %rax
  pushq $6
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse264
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse264
  pushq $1
  jmp .Lend264
.Lfalse264:
  pushq $0
.Lend264:
  popq %rax
  cmpq $0, %rax
  je .Lend263
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $4
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend263:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC158(%rip), %rax
  pushq %rax
  pushq $2
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse266
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse266
  pushq $1
  jmp .Lend266
.Lfalse266:
  pushq $0
.Lend266:
  popq %rax
  cmpq $0, %rax
  je .Lend265
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $5
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend265:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC159(%rip), %rax
  pushq %rax
  pushq $4
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse268
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse268
  pushq $1
  jmp .Lend268
.Lfalse268:
  pushq $0
.Lend268:
  popq %rax
  cmpq $0, %rax
  je .Lend267
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $6
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend267:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC160(%rip), %rax
  pushq %rax
  pushq $5
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse270
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse270
  pushq $1
  jmp .Lend270
.Lfalse270:
  pushq $0
.Lend270:
  popq %rax
  cmpq $0, %rax
  je .Lend269
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $7
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend269:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC161(%rip), %rax
  pushq %rax
  pushq $3
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse272
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse272
  pushq $1
  jmp .Lend272
.Lfalse272:
  pushq $0
.Lend272:
  popq %rax
  cmpq $0, %rax
  je .Lend271
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $8
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend271:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC162(%rip), %rax
  pushq %rax
  pushq $3
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse274
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse274
  pushq $1
  jmp .Lend274
.Lfalse274:
  pushq $0
.Lend274:
  popq %rax
  cmpq $0, %rax
  je .Lend273
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $9
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $3
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend273:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC163(%rip), %rax
  pushq %rax
  pushq $8
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse276
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse276
  pushq $1
  jmp .Lend276
.Lfalse276:
  pushq $0
.Lend276:
  popq %rax
  cmpq $0, %rax
  je .Lend275
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $13
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend275:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC164(%rip), %rax
  pushq %rax
  pushq $5
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse278
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse278
  pushq $1
  jmp .Lend278
.Lfalse278:
  pushq $0
.Lend278:
  popq %rax
  cmpq $0, %rax
  je .Lend277
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $11
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend277:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC165(%rip), %rax
  pushq %rax
  pushq $4
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse280
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse280
  pushq $1
  jmp .Lend280
.Lfalse280:
  pushq $0
.Lend280:
  popq %rax
  cmpq $0, %rax
  je .Lend279
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $12
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend279:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC166(%rip), %rax
  pushq %rax
  pushq $6
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse282
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse282
  pushq $1
  jmp .Lend282
.Lfalse282:
  pushq $0
.Lend282:
  popq %rax
  cmpq $0, %rax
  je .Lend281
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $18
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend281:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC167(%rip), %rax
  pushq %rax
  pushq $4
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse284
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse284
  pushq $1
  jmp .Lend284
.Lfalse284:
  pushq $0
.Lend284:
  popq %rax
  cmpq $0, %rax
  je .Lend283
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $10
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend283:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC168(%rip), %rax
  pushq %rax
  pushq $4
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse286
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse286
  pushq $1
  jmp .Lend286
.Lfalse286:
  pushq $0
.Lend286:
  popq %rax
  cmpq $0, %rax
  je .Lend285
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $14
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend285:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC169(%rip), %rax
  pushq %rax
  pushq $6
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse288
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse288
  pushq $1
  jmp .Lend288
.Lfalse288:
  pushq $0
.Lend288:
  popq %rax
  cmpq $0, %rax
  je .Lend287
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $15
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend287:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC170(%rip), %rax
  pushq %rax
  pushq $5
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse290
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse290
  pushq $1
  jmp .Lend290
.Lfalse290:
  pushq $0
.Lend290:
  popq %rax
  cmpq $0, %rax
  je .Lend289
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $16
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend289:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC171(%rip), %rax
  pushq %rax
  pushq $7
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse292
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $7
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse292
  pushq $1
  jmp .Lend292
.Lfalse292:
  pushq $0
.Lend292:
  popq %rax
  cmpq $0, %rax
  je .Lend291
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $22
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $7
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $7
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend291:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC172(%rip), %rax
  pushq %rax
  pushq $6
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse294
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse294
  pushq $1
  jmp .Lend294
.Lfalse294:
  pushq $0
.Lend294:
  popq %rax
  cmpq $0, %rax
  je .Lend293
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $23
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend293:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC173(%rip), %rax
  pushq %rax
  pushq $4
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse296
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse296
  pushq $1
  jmp .Lend296
.Lfalse296:
  pushq $0
.Lend296:
  popq %rax
  cmpq $0, %rax
  je .Lend295
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $24
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend295:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC174(%rip), %rax
  pushq %rax
  pushq $7
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse298
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $7
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse298
  pushq $1
  jmp .Lend298
.Lfalse298:
  pushq $0
.Lend298:
  popq %rax
  cmpq $0, %rax
  je .Lend297
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $25
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $7
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $7
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend297:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC175(%rip), %rax
  pushq %rax
  pushq $5
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse300
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse300
  pushq $1
  jmp .Lend300
.Lfalse300:
  pushq $0
.Lend300:
  popq %rax
  cmpq $0, %rax
  je .Lend299
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $26
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend299:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC176(%rip), %rax
  pushq %rax
  pushq $8
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse302
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse302
  pushq $1
  jmp .Lend302
.Lfalse302:
  pushq $0
.Lend302:
  popq %rax
  cmpq $0, %rax
  je .Lend301
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $27
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend301:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC177(%rip), %rax
  pushq %rax
  pushq $4
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse304
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse304
  pushq $1
  jmp .Lend304
.Lfalse304:
  pushq $0
.Lend304:
  popq %rax
  cmpq $0, %rax
  je .Lend303
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $28
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend303:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC178(%rip), %rax
  pushq %rax
  pushq $17
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse306
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $17
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse306
  pushq $1
  jmp .Lend306
.Lfalse306:
  pushq $0
.Lend306:
  popq %rax
  cmpq $0, %rax
  je .Lend305
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $30
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $17
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $17
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend305:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC179(%rip), %rax
  pushq %rax
  pushq $5
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse308
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse308
  pushq $1
  jmp .Lend308
.Lfalse308:
  pushq $0
.Lend308:
  popq %rax
  cmpq $0, %rax
  je .Lend307
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $29
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $5
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend307:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC180(%rip), %rax
  pushq %rax
  pushq $6
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncmp
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse310
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call is_alnum
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse310
  pushq $1
  jmp .Lend310
.Lfalse310:
  pushq $0
.Lend310:
  popq %rax
  cmpq $0, %rax
  je .Lend309
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $31
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $6
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend309:
  pushq $97
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse313
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $122
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse313
  pushq $1
  jmp .Lend313
.Lfalse313:
  pushq $0
.Lend313:
  popq %rax
  cmpq $1, %rax
  je .Ltrue312
  pushq $65
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse315
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $90
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse315
  pushq $1
  jmp .Lend315
.Lfalse315:
  pushq $0
.Lend315:
  popq %rax
  cmpq $1, %rax
  je .Ltrue314
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $95
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue314
  pushq $0
  jmp .Lend314
.Ltrue314:
  pushq $1
.Lend314:
  popq %rax
  cmpq $1, %rax
  je .Ltrue312
  pushq $0
  jmp .Lend312
.Ltrue312:
  pushq $1
.Lend312:
  popq %rax
  cmpq $0, %rax
  je .Lend311
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin316:
  pushq $97
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse318
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $122
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse318
  pushq $1
  jmp .Lend318
.Lfalse318:
  pushq $0
.Lend318:
  popq %rax
  cmpq $1, %rax
  je .Ltrue317
  pushq $65
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse320
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $90
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse320
  pushq $1
  jmp .Lend320
.Lfalse320:
  pushq $0
.Lend320:
  popq %rax
  cmpq $1, %rax
  je .Ltrue319
  pushq $48
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse322
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $57
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse322
  pushq $1
  jmp .Lend322
.Lfalse322:
  pushq $0
.Lend322:
  popq %rax
  cmpq $1, %rax
  je .Ltrue321
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $95
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue321
  pushq $0
  jmp .Lend321
.Ltrue321:
  pushq $1
.Lend321:
  popq %rax
  cmpq $1, %rax
  je .Ltrue319
  pushq $0
  jmp .Lend319
.Ltrue319:
  pushq $1
.Lend319:
  popq %rax
  cmpq $1, %rax
  je .Ltrue317
  pushq $0
  jmp .Lend317
.Ltrue317:
  pushq $1
.Lend317:
  popq %rax
  cmpq $0, %rax
  je .Lend316
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lcontinue40:
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin316
.Lend316:
.Lbreak43:
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  pushq $2
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue35
.Lend311:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC181(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lbegin231
.Lend231:
.Lbreak38:
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend323
  pushq $17
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call new_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rax
.Lend323:
  movq %rbp, %rax
  subq $41, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl label_count
.data
label_count:
  .zero 4
.globl switch_count
.data
switch_count:
  .zero 4
.globl max_switch_count
.data
max_switch_count:
  .zero 4
.globl break_count
.data
break_count:
  .zero 4
.globl max_break_count
.data
max_break_count:
  .zero 4
.globl continue_count
.data
continue_count:
  .zero 4
.globl max_continue_count
.data
max_continue_count:
  .zero 4
.data
.LC184:
  .string "  .string \"%.*s\"\n"
.data
.LC183:
  .string ".LC%d:\n"
.data
.LC182:
  .string ".data\n"
.globl gen_string_literal
.text
gen_string_literal:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin324:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend324
  leaq .LC182(%rip), %rax
  pushq %rax
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
  leaq .LC183(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  leaq .LC184(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
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
.Lcontinue41:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin324
.Lend324:
.Lbreak44:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC192:
  .string "left value of assignment must be variable: found %d\n"
.data
.LC191:
  .string "  pushq %%rax\n"
.data
.LC190:
  .string "  leaq .LC%d(%%rip), %%rax\n"
.data
.LC189:
  .string "  pushq %%rax\n"
.data
.LC188:
  .string "  leaq %.*s(%%rip), %%rax\n"
.data
.LC187:
  .string "  pushq %%rax\n"
.data
.LC186:
  .string "  subq $%d, %%rax\n"
.data
.LC185:
  .string "  movq %%rbp, %%rax\n"
.globl gen_lval
.text
gen_lval:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $13
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse325
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lend325
.Lelse325:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $27
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse326
  leaq .LC185(%rip), %rax
  pushq %rax
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
  leaq .LC186(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  leaq .LC187(%rip), %rax
  pushq %rax
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
  jmp .Lend326
.Lelse326:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $28
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse327
  leaq .LC188(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
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
  leaq .LC189(%rip), %rax
  pushq %rax
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
  jmp .Lend327
.Lelse327:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $31
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse328
  leaq .LC190(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  leaq .LC191(%rip), %rax
  pushq %rax
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
  jmp .Lend328
.Lelse328:
  leaq .LC192(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend328:
.Lend327:
.Lend326:
.Lend325:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC233:
  .string "  popq %%rax\n"
.data
.LC232:
  .string "  jmp .Lcontinue%d\n"
.data
.LC231:
  .string "  jmp .Lbreak%d\n"
.data
.LC230:
  .string "  ret\n"
.data
.LC229:
  .string "  popq %%rbp\n"
.data
.LC228:
  .string "  movq %%rbp, %%rsp\n"
.data
.LC227:
  .string "  popq %%rax\n"
.data
.LC226:
  .string "  pushq $0\n"
.data
.LC225:
  .string ".Lbreak%d:\n"
.data
.LC224:
  .string ".Lend%d:\n"
.data
.LC223:
  .string "  jmp .Lbegin%d\n"
.data
.LC222:
  .string ".Lcontinue%d:\n"
.data
.LC221:
  .string "  je .Lend%d\n"
.data
.LC220:
  .string "  cmpq $0, %%rax\n"
.data
.LC219:
  .string "  popq %%rax\n"
.data
.LC218:
  .string ".Lbegin%d:\n"
.data
.LC217:
  .string ".Lbreak%d:\n"
.data
.LC216:
  .string ".Lend%d:\n"
.data
.LC215:
  .string "  jmp .Lbegin%d\n"
.data
.LC214:
  .string "  je .Lend%d\n"
.data
.LC213:
  .string "  cmpq $0, %%rax\n"
.data
.LC212:
  .string "  popq %%rax\n"
.data
.LC211:
  .string ".Lcontinue%d:\n"
.data
.LC210:
  .string ".Lbegin%d:\n"
.data
.LC209:
  .string ".Ldefault%d:\n"
.data
.LC208:
  .string ".Lcase%d_%d:\n"
.data
.LC207:
  .string ".Lbreak%d:\n"
.data
.LC206:
  .string "  jmp .Ldefault%d\n"
.data
.LC205:
  .string "  je .Lcase%d_%d\n"
.data
.LC204:
  .string "  cmpq $%d, %%rax\n"
.data
.LC203:
  .string "  popq %%rax\n"
.data
.LC202:
  .string ".Lend%d:\n"
.data
.LC201:
  .string ".Lelse%d:\n"
.data
.LC200:
  .string "  jmp .Lend%d\n"
.data
.LC199:
  .string "  je .Lelse%d\n"
.data
.LC198:
  .string "  cmpq $0, %%rax\n"
.data
.LC197:
  .string "  popq %%rax\n"
.data
.LC196:
  .string ".Lend%d:\n"
.data
.LC195:
  .string "  je .Lend%d\n"
.data
.LC194:
  .string "  cmpq $0, %%rax\n"
.data
.LC193:
  .string "  popq %%rax\n"
.globl gen_stmt
.text
gen_stmt:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $26, %rax
  je .Lcase4_26
  cmpq $17, %rax
  je .Lcase4_17
  cmpq $18, %rax
  je .Lcase4_18
  cmpq $19, %rax
  je .Lcase4_19
  cmpq $20, %rax
  je .Lcase4_20
  cmpq $21, %rax
  je .Lcase4_21
  cmpq $24, %rax
  je .Lcase4_24
  cmpq $25, %rax
  je .Lcase4_25
  cmpq $16, %rax
  je .Lcase4_16
  cmpq $22, %rax
  je .Lcase4_22
  cmpq $23, %rax
  je .Lcase4_23
  jmp .Ldefault4
.Lcase4_26:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin329:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend329
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lcontinue42:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin329
.Lend329:
.Lbreak46:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_17:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  leaq label_count(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC193(%rip), %rax
  pushq %rax
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
  leaq .LC194(%rip), %rax
  pushq %rax
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
  leaq .LC195(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC196(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
.Lcase4_18:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  leaq label_count(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC197(%rip), %rax
  pushq %rax
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
  leaq .LC198(%rip), %rax
  pushq %rax
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
  leaq .LC199(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC200(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  leaq .LC201(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC202(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
.Lcase4_19:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $26
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC203(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  leaq max_break_count(%rip), %rax
  pushq %rax
  leaq max_break_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  leaq switch_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq switch_count(%rip), %rax
  pushq %rax
  leaq max_switch_count(%rip), %rax
  pushq %rax
  leaq max_switch_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin330:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend330
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse332
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $20
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse332
  pushq $1
  jmp .Lend332
.Lfalse332:
  pushq $0
.Lend332:
  popq %rax
  cmpq $0, %rax
  je .Lend331
  leaq .LC204(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  leaq .LC205(%rip), %rax
  pushq %rax
  leaq switch_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdx
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
.Lend331:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse334
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $21
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse334
  pushq $1
  jmp .Lend334
.Lfalse334:
  pushq $0
.Lend334:
  popq %rax
  cmpq $0, %rax
  je .Lend333
  leaq .LC206(%rip), %rax
  pushq %rax
  leaq switch_count(%rip), %rax
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
.Lend333:
.Lcontinue43:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin330
.Lend330:
.Lbreak47:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC207(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
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
  leaq break_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq switch_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_20:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $30
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC208(%rip), %rax
  pushq %rax
  leaq switch_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdx
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
.Lcase4_21:
  leaq .LC209(%rip), %rax
  pushq %rax
  leaq switch_count(%rip), %rax
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
.Lcase4_24:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  leaq label_count(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  leaq continue_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq continue_count(%rip), %rax
  pushq %rax
  leaq max_continue_count(%rip), %rax
  pushq %rax
  leaq max_continue_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  leaq max_break_count(%rip), %rax
  pushq %rax
  leaq max_break_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC210(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  leaq .LC211(%rip), %rax
  pushq %rax
  leaq continue_count(%rip), %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC212(%rip), %rax
  pushq %rax
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
  leaq .LC213(%rip), %rax
  pushq %rax
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
  leaq .LC214(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC215(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  leaq .LC216(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  leaq .LC217(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
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
  leaq continue_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_25:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  leaq label_count(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  leaq continue_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  leaq continue_count(%rip), %rax
  pushq %rax
  leaq max_continue_count(%rip), %rax
  pushq %rax
  leaq max_continue_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  leaq max_break_count(%rip), %rax
  pushq %rax
  leaq max_break_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend335
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend335:
  leaq .LC218(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend336
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC219(%rip), %rax
  pushq %rax
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
  leaq .LC220(%rip), %rax
  pushq %rax
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
  leaq .LC221(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
.Lend336:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC222(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend337
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend337:
  leaq .LC223(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  leaq .LC224(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
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
  leaq .LC225(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
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
  leaq continue_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_16:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse338
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lend338
.Lelse338:
  leaq .LC226(%rip), %rax
  pushq %rax
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
.Lend338:
  leaq .LC227(%rip), %rax
  pushq %rax
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
  leaq .LC228(%rip), %rax
  pushq %rax
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
  leaq .LC229(%rip), %rax
  pushq %rax
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
  leaq .LC230(%rip), %rax
  pushq %rax
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
.Lcase4_22:
  leaq .LC231(%rip), %rax
  pushq %rax
  leaq break_count(%rip), %rax
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
.Lcase4_23:
  leaq .LC232(%rip), %rax
  pushq %rax
  leaq continue_count(%rip), %rax
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
.Ldefault4:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC233(%rip), %rax
  pushq %rax
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
.Lbreak45:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC272:
  .string "  ret\n"
.data
.LC271:
  .string "  popq %%rbp\n"
.data
.LC270:
  .string "  movq %%rbp, %%rsp\n"
.data
.LC269:
  .string "invalid size"
.data
.LC268:
  .string "  movb %s, -%d(%%rbp)\n"
.data
.LC267:
  .string "  movw %s, -%d(%%rbp)\n"
.data
.LC266:
  .string "  movl %s, -%d(%%rbp)\n"
.data
.LC265:
  .string "  movq %s, -%d(%%rbp)\n"
.data
.LC264:
  .string "  pushq %s\n"
.data
.LC263:
  .string "  subq $%d, %%rsp\n"
.data
.LC262:
  .string "  movq %%rsp, %%rbp\n"
.data
.LC261:
  .string "  pushq %%rbp\n"
.data
.LC260:
  .string "%.*s:\n"
.data
.LC259:
  .string ".text\n"
.data
.LC258:
  .string ".globl %.*s\n"
.data
.LC257:
  .string "%r9"
.data
.LC256:
  .string "%r8"
.data
.LC255:
  .string "%rcx"
.data
.LC254:
  .string "%rdx"
.data
.LC253:
  .string "%rsi"
.data
.LC252:
  .string "%rdi"
.data
.LC251:
  .string "%r9w"
.data
.LC250:
  .string "%r8w"
.data
.LC249:
  .string "%cx"
.data
.LC248:
  .string "%dx"
.data
.LC247:
  .string "%si"
.data
.LC246:
  .string "%di"
.data
.LC245:
  .string "%r9d"
.data
.LC244:
  .string "%r8d"
.data
.LC243:
  .string "%ecx"
.data
.LC242:
  .string "%edx"
.data
.LC241:
  .string "%esi"
.data
.LC240:
  .string "%edi"
.data
.LC239:
  .string "%r9b"
.data
.LC238:
  .string "%r8b"
.data
.LC237:
  .string "%cl"
.data
.LC236:
  .string "%dl"
.data
.LC235:
  .string "%sil"
.data
.LC234:
  .string "%dil"
.globl gen_function
.text
gen_function:
  pushq %rbp
  movq %rsp, %rbp
  subq $224, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC234(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC235(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC236(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC237(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC238(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC239(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC240(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC241(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC242(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC243(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC244(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC245(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC246(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC247(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC248(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC249(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC250(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC251(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC252(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC253(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC254(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC255(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC256(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC257(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $56
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_string_literal
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC258(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
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
  leaq .LC259(%rip), %rax
  pushq %rax
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
  leaq .LC260(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
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
  leaq .LC261(%rip), %rax
  pushq %rax
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
  leaq .LC262(%rip), %rax
  pushq %rax
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
  leaq .LC263(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $64
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $68
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend339
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  pushq $5
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin340:
  pushq $0
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend340
  leaq .LC264(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
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
.Lcontinue44:
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin340
.Lend340:
.Lbreak48:
.Lend339:
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $208, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin341:
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $6
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse342
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse342
  pushq $1
  jmp .Lend342
.Lfalse342:
  pushq $0
.Lend342:
  popq %rax
  cmpq $0, %rax
  je .Lend341
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse343
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend343
.Lelse343:
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend343:
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin344:
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend344
  pushq $8
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse345
  leaq .LC265(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $200, %rax
  pushq %rax
  movq %rbp, %rax
  subq $208, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdx
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
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend345
.Lelse345:
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse346
  leaq .LC266(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  movq %rbp, %rax
  subq $208, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdx
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
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend346
.Lelse346:
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse347
  leaq .LC267(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  movq %rbp, %rax
  subq $208, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdx
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
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend347
.Lelse347:
  movq %rbp, %rax
  subq $212, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse348
  leaq .LC268(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  movq %rbp, %rax
  subq $208, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdx
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
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  movq %rbp, %rax
  subq $216, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend348
.Lelse348:
  leaq .LC269(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend348:
.Lend347:
.Lend346:
.Lend345:
.Lcontinue46:
  movq %rbp, %rax
  subq $208, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin344
.Lend344:
.Lbreak50:
.Lcontinue45:
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin341
.Lend341:
.Lbreak49:
  leaq arg_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $204, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq current_stack_size(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $64
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_stmt
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC270(%rip), %rax
  pushq %rax
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
  leaq .LC271(%rip), %rax
  pushq %rax
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
  leaq .LC272(%rip), %rax
  pushq %rax
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
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC451:
  .string "  pushq %%rax\n"
.data
.LC450:
  .string "unreachable"
.data
.LC449:
  .string "  movzbq %%al, %%rax\n"
.data
.LC448:
  .string "  setle %%al\n"
.data
.LC447:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC446:
  .string "  movzbq %%al, %%rax\n"
.data
.LC445:
  .string "  setl %%al\n"
.data
.LC444:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC443:
  .string "  movzbq %%al, %%rax\n"
.data
.LC442:
  .string "  setne %%al\n"
.data
.LC441:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC440:
  .string "  movzbq %%al, %%rax\n"
.data
.LC439:
  .string "  sete %%al\n"
.data
.LC438:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC437:
  .string "  idivq %%rdi\n"
.data
.LC436:
  .string "  cqto\n"
.data
.LC435:
  .string "  imulq %%rdi, %%rax\n"
.data
.LC434:
  .string "  subq %%rdi, %%rax\n"
.data
.LC433:
  .string "  addq %%rdi, %%rax\n"
.data
.LC432:
  .string "  popq %%rax\n"
.data
.LC431:
  .string "  popq %%rdi\n"
.data
.LC430:
  .string ".Lend%d:\n"
.data
.LC429:
  .string "  pushq $1\n"
.data
.LC428:
  .string ".Ltrue%d:\n"
.data
.LC427:
  .string "  jmp .Lend%d\n"
.data
.LC426:
  .string "  pushq $0\n"
.data
.LC425:
  .string "  je .Ltrue%d\n"
.data
.LC424:
  .string "  cmpq $1, %%rax\n"
.data
.LC423:
  .string "  popq %%rax\n"
.data
.LC422:
  .string "  je .Ltrue%d\n"
.data
.LC421:
  .string "  cmpq $1, %%rax\n"
.data
.LC420:
  .string "  popq %%rax\n"
.data
.LC419:
  .string ".Lend%d:\n"
.data
.LC418:
  .string "  pushq $0\n"
.data
.LC417:
  .string ".Lfalse%d:\n"
.data
.LC416:
  .string "  jmp .Lend%d\n"
.data
.LC415:
  .string "  pushq $1\n"
.data
.LC414:
  .string "  je .Lfalse%d\n"
.data
.LC413:
  .string "  cmpq $0, %%rax\n"
.data
.LC412:
  .string "  popq %%rax\n"
.data
.LC411:
  .string "  je .Lfalse%d\n"
.data
.LC410:
  .string "  cmpq $0, %%rax\n"
.data
.LC409:
  .string "  popq %%rax\n"
.data
.LC408:
  .string "unexpected type"
.data
.LC407:
  .string "  pushq %%rax\n"
.data
.LC406:
  .string "  movsbq (%%rax), %%rax\n"
.data
.LC405:
  .string "  movzbl (%%rax), %%eax\n"
.data
.LC404:
  .string "  pushq %%rax\n"
.data
.LC403:
  .string "  movslq %%eax, %%rax\n"
.data
.LC402:
  .string "  movswl (%%rax), %%eax\n"
.data
.LC401:
  .string "  movzwl (%%rax), %%eax\n"
.data
.LC400:
  .string "  pushq %%rax\n"
.data
.LC399:
  .string "  movslq (%%rax), %%rax\n"
.data
.LC398:
  .string "  movl (%%rax), %%eax\n"
.data
.LC397:
  .string "  pushq %%rax\n"
.data
.LC396:
  .string "  movq (%%rax), %%rax\n"
.data
.LC395:
  .string "  pushq %%r10\n"
.data
.LC394:
  .string "  movzbl %d(%%rax), %%r10d\n"
.data
.LC393:
  .string "  pushq %%r10\n"
.data
.LC392:
  .string "  movzwl %d(%%rax), %%r10d\n"
.data
.LC391:
  .string "  pushq %%r10\n"
.data
.LC390:
  .string "  movl %d(%%rax), %%r10d\n"
.data
.LC389:
  .string "  pushq %%r10\n"
.data
.LC388:
  .string "  movq %d(%%rax), %%r10\n"
.data
.LC387:
  .string "  popq %%rax\n"
.data
.LC386:
  .string "  pushq $0\n"
.data
.LC385:
  .string "  movq %%rax, -%d(%%rbp)\n"
.data
.LC384:
  .string "  leaq -%d(%%rbp), %%rax\n"
.data
.LC383:
  .string "  movq %%rax, -%d(%%rbp)\n"
.data
.LC382:
  .string "  leaq 16(%%rbp), %%rax\n"
.data
.LC381:
  .string "  movl %%eax, -%d(%%rbp)\n"
.data
.LC380:
  .string "  movl $48, %%eax\n"
.data
.LC379:
  .string "  movl %%eax, -%d(%%rbp)\n"
.data
.LC378:
  .string "  movl $%d, %%eax\n"
.data
.LC377:
  .string "not implemented: return value"
.data
.LC376:
  .string "  pushq %%rax\n"
.data
.LC375:
  .string "  pushq %%rax\n"
.data
.LC374:
  .string "  movswq %%ax, %%rax\n"
.data
.LC373:
  .string "  pushq %%rax\n"
.data
.LC372:
  .string "  andq $0xffff, %%rax\n"
.data
.LC371:
  .string "  pushq %%rax\n"
.data
.LC370:
  .string "  movslq %%eax, %%rax\n"
.data
.LC369:
  .string "  pushq %%rax\n"
.data
.LC368:
  .string "  andq $0xffffffff, %%rax\n"
.data
.LC367:
  .string "  pushq %%rax\n"
.data
.LC366:
  .string "  movsbq %%al, %%rax\n"
.data
.LC365:
  .string "  pushq %%rax\n"
.data
.LC364:
  .string "  andq $0xff, %%rax\n"
.data
.LC363:
  .string "  pushq $0\n"
.data
.LC362:
  .string "  movq %%rdi, %%rsp\n"
.data
.LC361:
  .string "  popq %%rdi\n"
.data
.LC360:
  .string "  popq %%rdi\n"
.data
.LC359:
  .string "  call %.*s\n"
.data
.LC358:
  .string "  movq $0, %%rax\n"
.data
.LC357:
  .string "  pushq $0\n"
.data
.LC356:
  .string "  pushq %%r10\n"
.data
.LC355:
  .string "  andq $-16, %%rsp\n"
.data
.LC354:
  .string "  movq %%rsp, %%r10\n"
.data
.LC353:
  .string "  popq %s\n"
.data
.LC352:
  .string "%r9"
.data
.LC351:
  .string "%r8"
.data
.LC350:
  .string "%rcx"
.data
.LC349:
  .string "%rdx"
.data
.LC348:
  .string "%rsi"
.data
.LC347:
  .string "%rdi"
.data
.LC346:
  .string "too many arguments"
.data
.LC345:
  .string "not implemented: too big object"
.data
.LC344:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC343:
  .string "  subb $1, %%dil\n"
.data
.LC342:
  .string "  pushq %%rdi\n"
.data
.LC341:
  .string "  movb (%%rax), %%dil\n"
.data
.LC340:
  .string "  movw %%di, (%%rax)\n"
.data
.LC339:
  .string "  subw $1, %%di\n"
.data
.LC338:
  .string "  pushq %%rdi\n"
.data
.LC337:
  .string "  movw (%%rax), %%di\n"
.data
.LC336:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC335:
  .string "  subl $1, %%edi\n"
.data
.LC334:
  .string "  pushq %%rdi\n"
.data
.LC333:
  .string "  movl (%%rax), %%edi\n"
.data
.LC332:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC331:
  .string "  subq $1, %%rdi\n"
.data
.LC330:
  .string "  pushq %%rdi\n"
.data
.LC329:
  .string "  movq (%%rax), %%rdi\n"
.data
.LC328:
  .string " popq %%rax\n"
.data
.LC327:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC326:
  .string "  addb $1, %%dil\n"
.data
.LC325:
  .string "  pushq %%rdi\n"
.data
.LC324:
  .string "  movb (%%rax), %%dil\n"
.data
.LC323:
  .string "  movw %%di, (%%rax)\n"
.data
.LC322:
  .string "  addw $1, %%di\n"
.data
.LC321:
  .string "  pushq %%rdi\n"
.data
.LC320:
  .string "  movw (%%rax), %%di\n"
.data
.LC319:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC318:
  .string "  addl $1, %%edi\n"
.data
.LC317:
  .string "  pushq %%rdi\n"
.data
.LC316:
  .string "  movl (%%rax), %%edi\n"
.data
.LC315:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC314:
  .string "  addq $1, %%rdi\n"
.data
.LC313:
  .string "  pushq %%rdi\n"
.data
.LC312:
  .string "  movq (%%rax), %%rdi\n"
.data
.LC311:
  .string " popq %%rax\n"
.data
.LC310:
  .string "  pushq $0\n"
.data
.LC309:
  .string "  popq %%rax\n"
.data
.LC308:
  .string "  pushq $0\n"
.data
.LC307:
  .string "  popq %%rax\n"
.data
.LC306:
  .string "  movb %%r10b, %d(%%rax)\n"
.data
.LC305:
  .string "  movw %%r10w, %d(%%rax)\n"
.data
.LC304:
  .string "  movl %%r10d, %d(%%rax)\n"
.data
.LC303:
  .string "  movq %%r10, %d(%%rax)\n"
.data
.LC302:
  .string "  popq %%r10\n"
.data
.LC301:
  .string "  movq %d(%%rsp), %%rax\n"
.data
.LC300:
  .string "  pushq %%rdi\n"
.data
.LC299:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC298:
  .string "  pushq %%rdi\n"
.data
.LC297:
  .string "  movw %%di, (%%rax)\n"
.data
.LC296:
  .string "  pushq %%rdi\n"
.data
.LC295:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC294:
  .string "  pushq %%rdi\n"
.data
.LC293:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC292:
  .string "  popq %%rax\n"
.data
.LC291:
  .string "  popq %%rdi\n"
.data
.LC290:
  .string "not implemented: size %d"
.data
.LC289:
  .string "  pushq %%rax\n"
.data
.LC288:
  .string "  movsbq (%%rax), %%rax\n"
.data
.LC287:
  .string "  pushq %%rax\n"
.data
.LC286:
  .string "  movzbl (%%rax), %%eax\n"
.data
.LC285:
  .string "  pushq %%rax\n"
.data
.LC284:
  .string "  movslq %%eax, %%rax\n"
.data
.LC283:
  .string "  movswl (%%rax), %%eax\n"
.data
.LC282:
  .string "  pushq %%rax\n"
.data
.LC281:
  .string "  movzwl (%%rax), %%eax\n"
.data
.LC280:
  .string "  pushq %%rax\n"
.data
.LC279:
  .string "  movslq (%%rax), %%rax\n"
.data
.LC278:
  .string "  pushq %%rax\n"
.data
.LC277:
  .string "  movl (%%rax), %%eax\n"
.data
.LC276:
  .string "  pushq %%r10\n"
.data
.LC275:
  .string "  movq %d(%%rax), %%r10\n"
.data
.LC274:
  .string "  popq %%rax\n"
.data
.LC273:
  .string "  pushq $%d\n"
.globl gen
.text
gen:
  pushq %rbp
  movq %rsp, %rbp
  subq $240, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $30, %rax
  je .Lcase5_30
  cmpq $27, %rax
  je .Lcase5_27
  cmpq $28, %rax
  je .Lcase5_28
  cmpq $31, %rax
  je .Lcase5_31
  cmpq $4, %rax
  je .Lcase5_4
  cmpq $5, %rax
  je .Lcase5_5
  cmpq $14, %rax
  je .Lcase5_14
  cmpq $15, %rax
  je .Lcase5_15
  cmpq $29, %rax
  je .Lcase5_29
  cmpq $32, %rax
  je .Lcase5_32
  cmpq $12, %rax
  je .Lcase5_12
  cmpq $13, %rax
  je .Lcase5_13
  cmpq $10, %rax
  je .Lcase5_10
  cmpq $11, %rax
  je .Lcase5_11
  jmp .Ldefault5
.Lcase5_30:
  leaq .LC273(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
.Lcase5_27:
.Lcase5_28:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_lval
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC274(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
.Lbegin349:
  pushq $8
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse350
  leaq .LC275(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
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
  leaq .LC276(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend350
.Lelse350:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse351
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse352
  leaq .LC277(%rip), %rax
  pushq %rax
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
  leaq .LC278(%rip), %rax
  pushq %rax
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
  jmp .Lend352
.Lelse352:
  leaq .LC279(%rip), %rax
  pushq %rax
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
  leaq .LC280(%rip), %rax
  pushq %rax
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
.Lend352:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend351
.Lelse351:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse353
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse354
  leaq .LC281(%rip), %rax
  pushq %rax
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
  leaq .LC282(%rip), %rax
  pushq %rax
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
  jmp .Lend354
.Lelse354:
  leaq .LC283(%rip), %rax
  pushq %rax
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
  leaq .LC284(%rip), %rax
  pushq %rax
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
  leaq .LC285(%rip), %rax
  pushq %rax
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
.Lend354:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend353
.Lelse353:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse355
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse356
  leaq .LC286(%rip), %rax
  pushq %rax
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
  leaq .LC287(%rip), %rax
  pushq %rax
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
  jmp .Lend356
.Lelse356:
  leaq .LC288(%rip), %rax
  pushq %rax
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
  leaq .LC289(%rip), %rax
  pushq %rax
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
.Lend356:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend355
.Lelse355:
  leaq .LC290(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
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
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend355:
.Lend353:
.Lend351:
.Lend350:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend357
  jmp .Lbreak52
.Lend357:
.Lcontinue47:
  jmp .Lbegin349
.Lend349:
.Lbreak52:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase5_31:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_lval
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase5_4:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_lval
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC291(%rip), %rax
  pushq %rax
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
  leaq .LC292(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend358
  leaq .LC293(%rip), %rax
  pushq %rax
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
  leaq .LC294(%rip), %rax
  pushq %rax
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
.Lend358:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend359
  leaq .LC295(%rip), %rax
  pushq %rax
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
  leaq .LC296(%rip), %rax
  pushq %rax
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
.Lend359:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend360
  leaq .LC297(%rip), %rax
  pushq %rax
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
  leaq .LC298(%rip), %rax
  pushq %rax
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
.Lend360:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend361
  leaq .LC299(%rip), %rax
  pushq %rax
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
  leaq .LC300(%rip), %rax
  pushq %rax
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
.Lend361:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
  movq 24(%rax), %r10
  pushq %r10
  movq 32(%rax), %r10
  pushq %r10
  movq 40(%rax), %r10
  pushq %r10
  movq 48(%rax), %r10
  pushq %r10
  movq 56(%rax), %r10
  pushq %r10
  movq 64(%rax), %r10
  pushq %r10
  movq 72(%rax), %r10
  pushq %r10
  movq 80(%rax), %r10
  pushq %r10
  movq 88(%rax), %r10
  pushq %r10
  movq 96(%rax), %r10
  pushq %r10
  movq 104(%rax), %r10
  pushq %r10
  movq 112(%rax), %r10
  pushq %r10
  movq 120(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin362:
.Lcontinue48:
  pushq $0
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend362
  pushq $8
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse363
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend363
.Lelse363:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse364
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend364
.Lelse364:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse365
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend365
.Lelse365:
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend365:
.Lend364:
.Lend363:
  jmp .Lbegin362
.Lend362:
.Lbreak53:
  leaq .LC301(%rip), %rax
  pushq %rax
  pushq $8
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  imulq %rdi, %rax
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
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin366:
  pushq $0
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend366
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC302(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse367
  leaq .LC303(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
  jmp .Lend367
.Lelse367:
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse368
  leaq .LC304(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
  jmp .Lend368
.Lelse368:
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse369
  leaq .LC305(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
  jmp .Lend369
.Lelse369:
  leaq .LC306(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
.Lend369:
.Lend368:
.Lend367:
.Lcontinue49:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin366
.Lend366:
.Lbreak54:
  leaq .LC307(%rip), %rax
  pushq %rax
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
  leaq .LC308(%rip), %rax
  pushq %rax
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
.Lcase5_5:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin370:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend370
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC309(%rip), %rax
  pushq %rax
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
.Lcontinue50:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin370
.Lend370:
.Lbreak55:
  leaq .LC310(%rip), %rax
  pushq %rax
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
.Lcase5_14:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_lval
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC311(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $8, %rax
  je .Lcase6_8
  cmpq $4, %rax
  je .Lcase6_4
  cmpq $2, %rax
  je .Lcase6_2
  cmpq $1, %rax
  je .Lcase6_1
.Lcase6_8:
  leaq .LC312(%rip), %rax
  pushq %rax
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
  leaq .LC313(%rip), %rax
  pushq %rax
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
  leaq .LC314(%rip), %rax
  pushq %rax
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
  leaq .LC315(%rip), %rax
  pushq %rax
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
  jmp .Lbreak56
.Lcase6_4:
  leaq .LC316(%rip), %rax
  pushq %rax
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
  leaq .LC317(%rip), %rax
  pushq %rax
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
  leaq .LC318(%rip), %rax
  pushq %rax
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
  leaq .LC319(%rip), %rax
  pushq %rax
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
  jmp .Lbreak56
.Lcase6_2:
  leaq .LC320(%rip), %rax
  pushq %rax
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
  leaq .LC321(%rip), %rax
  pushq %rax
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
  leaq .LC322(%rip), %rax
  pushq %rax
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
  leaq .LC323(%rip), %rax
  pushq %rax
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
  jmp .Lbreak56
.Lcase6_1:
  leaq .LC324(%rip), %rax
  pushq %rax
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
  leaq .LC325(%rip), %rax
  pushq %rax
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
  leaq .LC326(%rip), %rax
  pushq %rax
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
  leaq .LC327(%rip), %rax
  pushq %rax
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
  jmp .Lbreak56
.Lbreak56:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase5_15:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_lval
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC328(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $8, %rax
  je .Lcase7_8
  cmpq $4, %rax
  je .Lcase7_4
  cmpq $2, %rax
  je .Lcase7_2
  cmpq $1, %rax
  je .Lcase7_1
.Lcase7_8:
  leaq .LC329(%rip), %rax
  pushq %rax
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
  leaq .LC330(%rip), %rax
  pushq %rax
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
  leaq .LC331(%rip), %rax
  pushq %rax
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
  leaq .LC332(%rip), %rax
  pushq %rax
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
  jmp .Lbreak57
.Lcase7_4:
  leaq .LC333(%rip), %rax
  pushq %rax
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
  leaq .LC334(%rip), %rax
  pushq %rax
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
  leaq .LC335(%rip), %rax
  pushq %rax
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
  leaq .LC336(%rip), %rax
  pushq %rax
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
  jmp .Lbreak57
.Lcase7_2:
  leaq .LC337(%rip), %rax
  pushq %rax
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
  leaq .LC338(%rip), %rax
  pushq %rax
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
  leaq .LC339(%rip), %rax
  pushq %rax
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
  leaq .LC340(%rip), %rax
  pushq %rax
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
  jmp .Lbreak57
.Lcase7_1:
  leaq .LC341(%rip), %rax
  pushq %rax
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
  leaq .LC342(%rip), %rax
  pushq %rax
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
  leaq .LC343(%rip), %rax
  pushq %rax
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
  leaq .LC344(%rip), %rax
  pushq %rax
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
  jmp .Lbreak57
.Lbreak57:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase5_29:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin371:
.Lcontinue51:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend371
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  pushq $32
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend372
  leaq .LC345(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend372:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $7
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  cqto
  idivq %rdi
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin371
.Lend371:
.Lbreak58:
  pushq $6
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend373
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  leaq .LC346(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend373:
  movq %rbp, %rax
  subq $224, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC347(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $224, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC348(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $224, %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC349(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $224, %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC350(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $224, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC351(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $224, %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC352(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $228, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin374:
  pushq $0
  movq %rbp, %rax
  subq $228, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend374
  leaq .LC353(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $224, %rax
  pushq %rax
  movq %rbp, %rax
  subq $228, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
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
.Lcontinue52:
  movq %rbp, %rax
  subq $228, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin374
.Lend374:
.Lbreak59:
  leaq .LC354(%rip), %rax
  pushq %rax
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
  leaq .LC355(%rip), %rax
  pushq %rax
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
  leaq .LC356(%rip), %rax
  pushq %rax
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
  leaq .LC357(%rip), %rax
  pushq %rax
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
  leaq .LC358(%rip), %rax
  pushq %rax
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
  leaq .LC359(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
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
  leaq .LC360(%rip), %rax
  pushq %rax
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
  leaq .LC361(%rip), %rax
  pushq %rax
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
  leaq .LC362(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $7
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend375
  leaq .LC363(%rip), %rax
  pushq %rax
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
.Lend375:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Lcase8_1
  cmpq $4, %rax
  je .Lcase8_4
  cmpq $2, %rax
  je .Lcase8_2
  cmpq $8, %rax
  je .Lcase8_8
  jmp .Ldefault8
.Lcase8_1:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse376
  leaq .LC364(%rip), %rax
  pushq %rax
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
  leaq .LC365(%rip), %rax
  pushq %rax
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
  jmp .Lend376
.Lelse376:
  leaq .LC366(%rip), %rax
  pushq %rax
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
  leaq .LC367(%rip), %rax
  pushq %rax
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
.Lend376:
  jmp .Lbreak60
.Lcase8_4:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse377
  leaq .LC368(%rip), %rax
  pushq %rax
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
  leaq .LC369(%rip), %rax
  pushq %rax
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
  jmp .Lend377
.Lelse377:
  leaq .LC370(%rip), %rax
  pushq %rax
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
  leaq .LC371(%rip), %rax
  pushq %rax
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
.Lend377:
  jmp .Lbreak60
.Lcase8_2:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse378
  leaq .LC372(%rip), %rax
  pushq %rax
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
  leaq .LC373(%rip), %rax
  pushq %rax
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
  jmp .Lend378
.Lelse378:
  leaq .LC374(%rip), %rax
  pushq %rax
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
  leaq .LC375(%rip), %rax
  pushq %rax
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
.Lend378:
  jmp .Lbreak60
.Lcase8_8:
  leaq .LC376(%rip), %rax
  pushq %rax
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
  jmp .Lbreak60
.Ldefault8:
  leaq .LC377(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lbreak60:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase5_32:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $27
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call assert
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC378(%rip), %rax
  pushq %rax
  leaq arg_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
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
  leaq .LC379(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  leaq .LC380(%rip), %rax
  pushq %rax
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
  leaq .LC381(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  subq %rdi, %rax
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
  leaq .LC382(%rip), %rax
  pushq %rax
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
  leaq .LC383(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  subq %rdi, %rax
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
  leaq .LC384(%rip), %rax
  pushq %rax
  leaq current_stack_size(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $48
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
  leaq .LC385(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $28
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  subq %rdi, %rax
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
  leaq .LC386(%rip), %rax
  pushq %rax
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
.Lcase5_12:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_lval
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase5_13:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC387(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call sizeof_type
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
  pushq $8
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue380
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $6
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue381
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue381
  pushq $0
  jmp .Lend381
.Ltrue381:
  pushq $1
.Lend381:
  popq %rax
  cmpq $1, %rax
  je .Ltrue380
  pushq $0
  jmp .Lend380
.Ltrue380:
  pushq $1
.Lend380:
  popq %rax
  cmpq $0, %rax
  je .Lend379
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin382:
.Lcontinue53:
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend382
  movq %rbp, %rax
  subq $232, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  pushq $8
  movq %rbp, %rax
  subq $232, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse383
  leaq .LC388(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
  leaq .LC389(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend383
.Lelse383:
  pushq $4
  movq %rbp, %rax
  subq $232, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse384
  leaq .LC390(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
  leaq .LC391(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend384
.Lelse384:
  pushq $2
  movq %rbp, %rax
  subq $232, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse385
  leaq .LC392(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
  leaq .LC393(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend385
.Lelse385:
  leaq .LC394(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
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
  leaq .LC395(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  movq %rbp, %rax
  subq $172, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend385:
.Lend384:
.Lend383:
  jmp .Lbegin382
.Lend382:
.Lbreak61:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend379:
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend386
  leaq .LC396(%rip), %rax
  pushq %rax
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
  leaq .LC397(%rip), %rax
  pushq %rax
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
.Lend386:
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend387
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse388
  leaq .LC398(%rip), %rax
  pushq %rax
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
  jmp .Lend388
.Lelse388:
  leaq .LC399(%rip), %rax
  pushq %rax
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
.Lend388:
  leaq .LC400(%rip), %rax
  pushq %rax
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
.Lend387:
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend389
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse390
  leaq .LC401(%rip), %rax
  pushq %rax
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
  jmp .Lend390
.Lelse390:
  leaq .LC402(%rip), %rax
  pushq %rax
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
  leaq .LC403(%rip), %rax
  pushq %rax
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
.Lend390:
  leaq .LC404(%rip), %rax
  pushq %rax
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
.Lend389:
  movq %rbp, %rax
  subq $176, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend391
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse392
  leaq .LC405(%rip), %rax
  pushq %rax
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
  jmp .Lend392
.Lelse392:
  leaq .LC406(%rip), %rax
  pushq %rax
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
.Lend392:
  leaq .LC407(%rip), %rax
  pushq %rax
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
.Lend391:
  leaq .LC408(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lcase5_10:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  leaq label_count(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC409(%rip), %rax
  pushq %rax
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
  leaq .LC410(%rip), %rax
  pushq %rax
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
  leaq .LC411(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC412(%rip), %rax
  pushq %rax
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
  leaq .LC413(%rip), %rax
  pushq %rax
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
  leaq .LC414(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  leaq .LC415(%rip), %rax
  pushq %rax
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
  leaq .LC416(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  leaq .LC417(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  leaq .LC418(%rip), %rax
  pushq %rax
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
  leaq .LC419(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
.Lcase5_11:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  leaq label_count(%rip), %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC420(%rip), %rax
  pushq %rax
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
  leaq .LC421(%rip), %rax
  pushq %rax
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
  leaq .LC422(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC423(%rip), %rax
  pushq %rax
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
  leaq .LC424(%rip), %rax
  pushq %rax
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
  leaq .LC425(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  leaq .LC426(%rip), %rax
  pushq %rax
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
  leaq .LC427(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  leaq .LC428(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  leaq .LC429(%rip), %rax
  pushq %rax
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
  leaq .LC430(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
.Ldefault5:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  leaq .LC431(%rip), %rax
  pushq %rax
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
  leaq .LC432(%rip), %rax
  pushq %rax
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
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lcase9_0
  cmpq $1, %rax
  je .Lcase9_1
  cmpq $2, %rax
  je .Lcase9_2
  cmpq $3, %rax
  je .Lcase9_3
  cmpq $6, %rax
  je .Lcase9_6
  cmpq $7, %rax
  je .Lcase9_7
  cmpq $8, %rax
  je .Lcase9_8
  cmpq $9, %rax
  je .Lcase9_9
  jmp .Ldefault9
.Lcase9_0:
  leaq .LC433(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Lcase9_1:
  leaq .LC434(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Lcase9_2:
  leaq .LC435(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Lcase9_3:
  leaq .LC436(%rip), %rax
  pushq %rax
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
  leaq .LC437(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Lcase9_6:
  leaq .LC438(%rip), %rax
  pushq %rax
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
  leaq .LC439(%rip), %rax
  pushq %rax
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
  leaq .LC440(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Lcase9_7:
  leaq .LC441(%rip), %rax
  pushq %rax
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
  leaq .LC442(%rip), %rax
  pushq %rax
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
  leaq .LC443(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Lcase9_8:
  leaq .LC444(%rip), %rax
  pushq %rax
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
  leaq .LC445(%rip), %rax
  pushq %rax
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
  leaq .LC446(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Lcase9_9:
  leaq .LC447(%rip), %rax
  pushq %rax
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
  leaq .LC448(%rip), %rax
  pushq %rax
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
  leaq .LC449(%rip), %rax
  pushq %rax
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
  jmp .Lbreak62
.Ldefault9:
  leaq .LC450(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lbreak62:
  leaq .LC451(%rip), %rax
  pushq %rax
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
.Lbreak51:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC452:
  .string "\n"
.globl error
.text
error:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  pushq %r9
  pushq %r8
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %rdi
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
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
  call vfprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC452(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $1
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call exit
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC458:
  .string "\n"
.data
.LC457:
  .string "^ "
.data
.LC456:
  .string ""
.data
.LC455:
  .string "%*s"
.data
.LC454:
  .string "%.*s\n"
.data
.LC453:
  .string "%s:%d: "
.globl _error_at
.text
_error_at:
  pushq %rbp
  movq %rsp, %rbp
  subq $80, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rdx, -40(%rbp)
  movq %rcx, -32(%rbp)
  movq %r8, -24(%rbp)
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin393:
.Lcontinue54:
  leaq user_input(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse394
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $-1
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse394
  pushq $1
  jmp .Lend394
.Lfalse394:
  pushq $0
.Lend394:
  popq %rax
  cmpq $0, %rax
  je .Lend393
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  subq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin393
.Lend393:
.Lbreak63:
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin395:
.Lcontinue55:
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend395
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin395
.Lend395:
.Lbreak64:
  movq %rbp, %rax
  subq $60, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
  leaq user_input(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin396:
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend396
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend397
  movq %rbp, %rax
  subq $60, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
.Lend397:
.Lcontinue56:
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin396
.Lend396:
.Lbreak65:
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC453(%rip), %rax
  pushq %rax
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $60, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fprintf
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
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC454(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC455(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  leaq .LC456(%rip), %rax
  pushq %rax
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC457(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  pushq $0
  pushq $24
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call vfprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC458(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fprintf
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $1
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call exit
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.globl error_at
.text
error_at:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  pushq %r9
  pushq %r8
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %rdi
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
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
  call _error_at
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.globl error_at_token
.text
error_at_token:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  pushq %r9
  pushq %r8
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %rdi
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
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
  call _error_at
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.globl error_at_here
.text
error_at_here:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  pushq %r9
  pushq %r8
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %rdi
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
  popq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq 8(%rax), %r10
  pushq %r10
  movq 16(%rax), %r10
  pushq %r10
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
  call _error_at
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC459:
  .string "assertion failed"
.globl assert
.text
assert:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movb %dil, -1(%rbp)
  movq %rbp, %rax
  subq $1, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend398
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC459(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error_at_token
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend398:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC463:
  .string "%s: fseek"
.data
.LC462:
  .string "%s: fseek"
.data
.LC461:
  .string "cannot open %s"
.data
.LC460:
  .string "r"
.globl read_file
.text
read_file:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC460(%rip), %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fopen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend399
  leaq .LC461(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend399:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  pushq $2
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fseek
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $-1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend400
  leaq .LC462(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend400:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call ftell
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
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  pushq $0
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fseek
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $-1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend401
  leaq .LC463(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
.Lend401:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  pushq $1
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fread
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue403
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue403
  pushq $0
  jmp .Lend403
.Ltrue403:
  pushq $1
.Lend403:
  popq %rax
  cmpq $0, %rax
  je .Lend402
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $10
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
.Lend402:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call fclose
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC464:
  .string "."
.globl get_dir
.text
get_dir:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strlen
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  subq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin404:
  pushq $0
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend404
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $47
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend405
  jmp .Lbreak66
.Lend405:
.Lcontinue57:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin404
.Lend404:
.Lbreak66:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $-1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend406
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $2
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC464(%rip), %rax
  pushq %rax
  pushq $1
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncpy
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend406:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call calloc
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call strncpy
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC472:
  .string "  .zero %d\n"
.data
.LC471:
  .string "%.*s:\n"
.data
.LC470:
  .string ".data\n"
.data
.LC469:
  .string ".globl %.*s\n"
.data
.LC468:
  .string "  .zero 8\n"
.data
.LC467:
  .string "NULL:\n"
.data
.LC466:
  .string ".data\n"
.data
.LC465:
  .string "invalid argument"
.globl main
.text
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movl %edi, -4(%rbp)
  movq %rsi, -12(%rbp)
  movq %rbp, %rax
  subq $4, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend407
  leaq .LC465(%rip), %rax
  pushq %rax
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call error
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  pushq $1
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend407:
  leaq filename(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq dir_name(%rip), %rax
  pushq %rax
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call get_dir
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq user_input(%rip), %rax
  pushq %rax
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call read_file
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq token(%rip), %rax
  pushq %rax
  leaq user_input(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $1
  popq %rsi
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call tokenize
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq token(%rip), %rax
  pushq %rax
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call preprocess
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC466(%rip), %rax
  pushq %rax
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
  leaq .LC467(%rip), %rax
  pushq %rax
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
  leaq .LC468(%rip), %rax
  pushq %rax
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
.Lbegin408:
.Lcontinue58:
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call at_eof
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  movsbq %al, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend408
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call external
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lcase10_0
  cmpq $2, %rax
  je .Lcase10_2
  jmp .Ldefault10
.Lcase10_0:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  movq %rsp, %r10
  andq $-16, %rsp
  pushq %r10
  pushq $0
  movq $0, %rax
  call gen_function
  popq %rdi
  popq %rdi
  movq %rdi, %rsp
  pushq $0
  popq %rax
  jmp .Lbreak68
.Lcase10_2:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $69
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend409
  leaq .LC469(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
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
  leaq .LC470(%rip), %rax
  pushq %rax
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
  leaq .LC471(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %rdx
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
  leaq .LC472(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $48
  popq %rdi
  popq %rax
  addq %rdi, %rax
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
.Lend409:
  jmp .Lbreak68
.Ldefault10:
  jmp .Lbreak68
.Lbreak68:
  jmp .Lbegin408
.Lend408:
.Lbreak67:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
