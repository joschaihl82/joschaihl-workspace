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
  pushq $4
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  cmpq $7, %rax
  je .Lcase1_7
  cmpq $8, %rax
  je .Lcase1_8
.Lcase1_0:
  pushq $4
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_1:
  pushq $8
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_2:
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
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_3:
  pushq $1
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
  movl (%rax), %eax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_7:
  pushq $1
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase1_8:
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
  pushq %rax
  pushq $18
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
  movl (%rax), %eax
  pushq %rax
  pushq $16
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
  pushq %rax
  pushq $14
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
  subq $72, %rsp
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
  je .Lend54
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
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend54:
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
  je .Lend55
  pushq $3
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
.Lend55:
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
  je .Lend56
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
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend56:
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
  je .Lend57
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
.Lend57:
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
  je .Lend58
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $2
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
.Lend58:
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
  je .Lend59
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
  je .Lend60
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin61:
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
  je .Lend61
  movq %rbp, %rax
  subq $20, %rax
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
  je .Lend62
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
.Lend62:
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
  movq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  jmp .Lbegin61
.Lend61:
.Lbreak16:
.Lend60:
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
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend59:
  pushq $12
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
  je .Ltrue64
  pushq $13
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
  je .Ltrue64
  pushq $0
  jmp .Lend64
.Ltrue64:
  pushq $1
.Lend64:
  popq %rax
  cmpq $0, %rax
  je .Lend63
  movq %rbp, %rax
  subq $21, %rax
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
  je .Lelse65
  movq %rbp, %rax
  subq $21, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend65
.Lelse65:
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
.Lend65:
  movq %rbp, %rax
  subq $29, %rax
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
  je .Lelse66
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $0
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
  popq %rax
  cmpq $0, %rax
  je .Lend67
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
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
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend67:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $21, %rax
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
.Lbegin68:
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
  je .Lend68
  movq %rbp, %rax
  subq $37, %rax
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
  subq $37, %rax
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
  je .Lend69
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
.Lend69:
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
  movq %rbp, %rax
  subq $37, %rax
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
  subq $37, %rax
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
  jmp .Lbegin68
.Lend68:
.Lbreak17:
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend70
  movq %rbp, %rax
  subq $45, %rax
  pushq %rax
  movq %rbp, %rax
  subq $29, %rax
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
  subq $45, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse71
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
  movq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse72
  leaq .LC17(%rip), %rax
  pushq %rax
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
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movl (%rax), %eax
  pushq %rax
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
  jmp .Lend72
.Lelse72:
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
  subq $45, %rax
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
  subq $45, %rax
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
  movq (%rax), %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $45, %rax
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
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend72:
  jmp .Lend71
.Lelse71:
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
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend71:
.Lend70:
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
  jmp .Lend66
.Lelse66:
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
  je .Lend73
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
.Lend73:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $29, %rax
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
  subq $53, %rax
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
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse74
  movq %rbp, %rax
  subq $53, %rax
  pushq %rax
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
  subq $53, %rax
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
  subq $53, %rax
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
  jmp .Lend74
.Lelse74:
  movq %rbp, %rax
  subq $53, %rax
  pushq %rax
  pushq $4
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
  subq $53, %rax
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
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend74:
  movq %rbp, %rax
  subq $53, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend66:
.Lend63:
  movq %rbp, %rax
  subq $61, %rax
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
  subq $61, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend75
  movq %rbp, %rax
  subq $69, %rax
  pushq %rax
  movq %rbp, %rax
  subq $61, %rax
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
  subq $69, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend76
  movq %rbp, %rax
  subq $69, %rax
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
.Lend76:
  movq %rbp, %rax
  subq $61, %rax
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
.Lend75:
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
  je .Lelse77
.Lbegin78:
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
  je .Lend78
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin78
.Lend78:
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
  jmp .Lend77
.Lelse77:
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
  je .Lend79
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
.Lend79:
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
.Lend77:
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
  je .Lend80
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
.Lbegin81:
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
  je .Lend81
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
  je .Lend82
  jmp .Lbreak19
.Lend82:
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
  jmp .Lbegin81
.Lend81:
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
  pushq $6
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
.Lend80:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin83:
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
  je .Lend83
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
  pushq $1
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
  jmp .Lbegin83
.Lend83:
.Lbreak20:
.Lbegin84:
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
  je .Lend84
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
  movl (%rax), %eax
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
  je .Lend85
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
.Lend85:
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
  pushq $2
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
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin84
.Lend84:
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
  je .Lend86
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend86:
.Lbegin87:
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
  je .Lend87
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
  jmp .Lbegin87
.Lend87:
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
.Lbegin88:
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
  je .Lend88
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
  jmp .Lbegin88
.Lend88:
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
  je .Lend89
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
.Lend89:
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
  je .Lend90
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
.Lbegin91:
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
  je .Lend91
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
  jmp .Lbegin91
.Lend91:
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
.Lend90:
.Lbegin92:
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
  je .Lend92
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
  movl (%rax), %eax
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
  je .Lend93
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
.Lend93:
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
  pushq $2
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
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin92
.Lend92:
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
  je .Lend94
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend94:
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
  pushq $1
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
  movl (%rax), %eax
  pushq %rax
  pushq $1
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
  pushq $1
  pushq $3
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
  movl (%rax), %eax
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
  pushq $3
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
  je .Lend95
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
  pushq $1
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
.Lend95:
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
  je .Lend96
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
.Lend96:
  pushq $11
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
  je .Lend97
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
.Lend97:
  pushq $12
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
  je .Ltrue99
  pushq $13
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
  je .Ltrue99
  pushq $0
  jmp .Lend99
.Ltrue99:
  pushq $1
.Lend99:
  popq %rax
  cmpq $0, %rax
  je .Lend98
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
  je .Lend100
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
.Lend100:
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
.Lend98:
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
  je .Lend102
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
.Lend102:
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
.Lend101:
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
  je .Lend103
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
.Lend103:
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
  movl (%rax), %eax
  pushq %rax
  pushq $6
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse104
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
  je .Lend105
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
.Lend105:
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
  je .Lend106
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
  je .Lelse107
  movq %rbp, %rax
  subq $37, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend107
.Lelse107:
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
.Lend107:
.Lend106:
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
  je .Lfalse109
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
  je .Lfalse109
  pushq $1
  jmp .Lend109
.Lfalse109:
  pushq $0
.Lend109:
  popq %rax
  cmpq $0, %rax
  je .Lend108
.Lbegin110:
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
  je .Lend111
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
.Lend111:
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
  je .Lend112
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
.Lend112:
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
  je .Lend113
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
.Lend113:
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
  movl (%rax), %eax
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
  je .Lend114
  jmp .Lbreak27
.Lend114:
.Lcontinue25:
  jmp .Lbegin110
.Lend110:
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
.Lend108:
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
  je .Lend115
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
.Lend115:
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
  movl (%rax), %eax
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
  je .Lend116
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
.Lend116:
  jmp .Lend104
.Lelse104:
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
.Lend104:
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend117
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
  movl (%rax), %eax
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
.Lend117:
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
  pushq $21
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
  je .Lelse118
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
  movl (%rax), %eax
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
  je .Lend119
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
.Lend119:
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
  jmp .Lend118
.Lelse118:
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
  je .Lelse120
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
  jmp .Lend120
.Lelse120:
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
  je .Lelse121
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
  je .Lelse122
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
  jmp .Lend122
.Lelse122:
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
.Lend122:
  jmp .Lend121
.Lelse121:
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
  popq %rax
  cmpq $0, %rax
  je .Lelse123
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
  jmp .Lend123
.Lelse123:
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
  je .Lelse124
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
  jmp .Lend124
.Lelse124:
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
  je .Lelse125
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
  je .Lend126
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
.Lend126:
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
  je .Lend127
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
.Lend127:
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
  je .Lend128
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
.Lend128:
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
  jmp .Lend125
.Lelse125:
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
  je .Lelse129
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
.Lbegin130:
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
  je .Lend130
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
  jmp .Lbegin130
.Lend130:
.Lbreak28:
  jmp .Lend129
.Lelse129:
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
  je .Lelse131
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
  je .Lelse132
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
  jmp .Lend132
.Lelse132:
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
.Lend132:
  jmp .Lend131
.Lelse131:
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
  je .Lelse133
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
  jmp .Lend133
.Lelse133:
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
  je .Lelse134
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
  jmp .Lend134
.Lelse134:
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
.Lend134:
.Lend133:
.Lend131:
.Lend129:
.Lend125:
.Lend124:
.Lend123:
.Lend121:
.Lend120:
.Lend118:
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
  je .Lelse135
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
  jmp .Lend135
.Lelse135:
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
  je .Lelse136
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
  jmp .Lend136
.Lelse136:
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
  je .Lelse137
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
  jmp .Lend137
.Lelse137:
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
  je .Lelse138
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
  jmp .Lend138
.Lelse138:
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
  je .Lend139
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
.Lend139:
.Lend138:
.Lend137:
.Lend136:
.Lend135:
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
.Lbegin140:
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
  je .Lelse141
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
  jmp .Lend141
.Lelse141:
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
  je .Lelse142
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
  jmp .Lend142
.Lelse142:
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
.Lend142:
.Lend141:
.Lcontinue27:
  jmp .Lbegin140
.Lend140:
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
.Lbegin143:
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
  je .Lelse144
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
  jmp .Lend144
.Lelse144:
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
  je .Lelse145
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
  jmp .Lend145
.Lelse145:
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
.Lend145:
.Lend144:
.Lcontinue28:
  jmp .Lbegin143
.Lend143:
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
.Lbegin146:
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
  je .Lelse147
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
  jmp .Lend147
.Lelse147:
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
  je .Lelse148
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
  jmp .Lend148
.Lelse148:
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
  je .Lelse149
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
  jmp .Lend149
.Lelse149:
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
  je .Lelse150
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
  jmp .Lend150
.Lelse150:
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
.Lend150:
.Lend149:
.Lend148:
.Lend147:
.Lcontinue29:
  jmp .Lbegin146
.Lend146:
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
.Lbegin151:
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
  movl (%rax), %eax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue154
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
  movl (%rax), %eax
  pushq %rax
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue154
  pushq $0
  jmp .Lend154
.Ltrue154:
  pushq $1
.Lend154:
  popq %rax
  cmpq $0, %rax
  je .Lfalse153
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
  movl (%rax), %eax
  pushq %rax
  pushq $5
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse153
  pushq $1
  jmp .Lend153
.Lfalse153:
  pushq $0
.Lend153:
  popq %rax
  cmpq $0, %rax
  je .Lend152
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
  je .Lelse155
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
  movl (%rax), %eax
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
  je .Lend156
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
  pushq $1
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
.Lend156:
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
  jmp .Lend155
.Lelse155:
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
  je .Lend157
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
  movl (%rax), %eax
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
  je .Lend158
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
  pushq $1
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
.Lend158:
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
.Lend157:
.Lend155:
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
.Lend152:
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
  je .Lelse159
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
  movl (%rax), %eax
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
  je .Lfalse161
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
  movl (%rax), %eax
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
  je .Lfalse161
  pushq $1
  jmp .Lend161
.Lfalse161:
  pushq $0
.Lend161:
  popq %rax
  cmpq $0, %rax
  je .Lelse160
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  jmp .Lend160
.Lelse160:
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
.Lend160:
  jmp .Lend159
.Lelse159:
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
  je .Lelse162
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
  movl (%rax), %eax
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
  je .Lfalse164
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
  movl (%rax), %eax
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
  je .Lfalse164
  pushq $1
  jmp .Lend164
.Lfalse164:
  pushq $0
.Lend164:
  popq %rax
  cmpq $0, %rax
  je .Lelse163
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  jmp .Lend163
.Lelse163:
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
.Lend163:
  jmp .Lend162
.Lelse162:
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
.Lend162:
.Lend159:
.Lcontinue30:
  jmp .Lbegin151
.Lend151:
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
.Lbegin165:
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
  je .Lelse166
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  jmp .Lend167
.Lelse167:
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
.Lend167:
  jmp .Lend166
.Lelse166:
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
  je .Lelse169
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
  movl (%rax), %eax
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
  je .Lfalse171
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
  movl (%rax), %eax
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
  je .Lfalse171
  pushq $1
  jmp .Lend171
.Lfalse171:
  pushq $0
.Lend171:
  popq %rax
  cmpq $0, %rax
  je .Lelse170
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  jmp .Lend170
.Lelse170:
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
.Lend170:
  jmp .Lend169
.Lelse169:
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
.Lend169:
.Lend166:
.Lcontinue31:
  jmp .Lbegin165
.Lend165:
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
  je .Lelse172
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
  jmp .Lend172
.Lelse172:
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
  je .Lelse173
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  jmp .Lend174
.Lelse174:
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
.Lend174:
  jmp .Lend173
.Lelse173:
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
  je .Lelse175
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
  jmp .Lend175
.Lelse175:
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
  je .Lelse176
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
  movl (%rax), %eax
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
  je .Lfalse178
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
  movl (%rax), %eax
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
  je .Lfalse178
  pushq $1
  jmp .Lend178
.Lfalse178:
  pushq $0
.Lend178:
  popq %rax
  cmpq $0, %rax
  je .Lend177
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
.Lend177:
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
  movl (%rax), %eax
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
  je .Lend179
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
.Lend179:
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
  jmp .Lend176
.Lelse176:
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
  cmpq $0, %rax
  je .Lelse180
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
  je .Lend181
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
  je .Lend182
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
.Lend182:
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
.Lend181:
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
  jmp .Lend180
.Lelse180:
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
  je .Lelse183
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
  jmp .Lend183
.Lelse183:
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
  je .Lelse184
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
  jmp .Lend184
.Lelse184:
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
  je .Lend185
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
.Lend185:
.Lend184:
.Lend183:
.Lend180:
.Lend176:
.Lend175:
.Lend173:
.Lend172:
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
.Lbegin186:
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
  je .Lend187
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
  movl (%rax), %eax
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
  je .Lend188
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
  pushq $1
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
.Lend188:
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
.Lend187:
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
  je .Lend189
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
  je .Lend190
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
.Lend190:
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
  movl (%rax), %eax
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
  je .Lend191
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
.Lend191:
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
  je .Lend192
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
.Lend192:
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
  movl (%rax), %eax
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
  pushq $1
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
.Lend189:
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
  movl (%rax), %eax
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
  je .Lend195
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
.Lend195:
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
  movl (%rax), %eax
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
  je .Lend196
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
.Lend196:
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
  je .Lend197
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
.Lend197:
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
  movl (%rax), %eax
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
  pushq $1
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
  je .Lend198
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
.Lend198:
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
  je .Lend199
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
.Lend199:
  jmp .Lbreak34
.Lcontinue32:
  jmp .Lbegin186
.Lend186:
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
  je .Lend200
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
.Lend200:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
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
  je .Lend201
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
  movl (%rax), %eax
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
.Lend201:
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
  je .Lend202
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
  je .Lend203
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
.Lend203:
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
  movl (%rax), %eax
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
  je .Lend204
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
  je .Lelse205
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
  movl (%rax), %eax
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
  je .Lend206
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
.Lend206:
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
.Lbegin207:
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
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setl %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend207
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
  pushq $1
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
  movl (%rax), %eax
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
  je .Lend208
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
.Lend208:
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
  jmp .Lbegin207
.Lend207:
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
  jmp .Lend205
.Lelse205:
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
.Lend205:
.Lend204:
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
.Lend202:
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
  je .Lend209
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
  je .Lelse210
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
  je .Lelse211
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
  jmp .Lend211
.Lelse211:
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
.Lend211:
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
  je .Lend212
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
.Lend212:
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
  je .Lend213
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
.Lbegin214:
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
  je .Lelse215
  jmp .Lbreak36
  jmp .Lend215
.Lelse215:
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
.Lend215:
.Lcontinue34:
  jmp .Lbegin214
.Lend214:
.Lbreak36:
  jmp .Lend210
.Lelse210:
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
  je .Lelse216
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
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend216
.Lelse216:
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
  je .Lelse217
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
  movl (%rax), %eax
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
  jmp .Lend217
.Lelse217:
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
  je .Lelse218
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
  jmp .Lend218
.Lelse218:
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
  movl (%rax), %eax
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
.Lend218:
.Lend217:
.Lend216:
.Lend210:
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
.Lend209:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $17
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
  je .Lend219
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
  je .Lend220
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
.Lend220:
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
.Lend219:
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
  je .Lfalse222
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
  je .Lfalse222
  pushq $1
  jmp .Lend222
.Lfalse222:
  pushq $0
.Lend222:
  popq %rax
  cmpq $1, %rax
  je .Ltrue221
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
  je .Lfalse224
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
  je .Lfalse224
  pushq $1
  jmp .Lend224
.Lfalse224:
  pushq $0
.Lend224:
  popq %rax
  cmpq $1, %rax
  je .Ltrue223
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
  je .Lfalse226
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
  je .Lfalse226
  pushq $1
  jmp .Lend226
.Lfalse226:
  pushq $0
.Lend226:
  popq %rax
  cmpq $1, %rax
  je .Ltrue225
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
  je .Ltrue225
  pushq $0
  jmp .Lend225
.Ltrue225:
  pushq $1
.Lend225:
  popq %rax
  cmpq $1, %rax
  je .Ltrue223
  pushq $0
  jmp .Lend223
.Ltrue223:
  pushq $1
.Lend223:
  popq %rax
  cmpq $1, %rax
  je .Ltrue221
  pushq $0
  jmp .Lend221
.Ltrue221:
  pushq $1
.Lend221:
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
.LC178:
  .string "tokenize failed"
.data
.LC177:
  .string "extern"
.data
.LC176:
  .string "_Bool"
.data
.LC175:
  .string "__builtin_va_list"
.data
.LC174:
  .string "void"
.data
.LC173:
  .string "continue"
.data
.LC172:
  .string "break"
.data
.LC171:
  .string "default"
.data
.LC170:
  .string "case"
.data
.LC169:
  .string "switch"
.data
.LC168:
  .string "typedef"
.data
.LC167:
  .string "union"
.data
.LC166:
  .string "struct"
.data
.LC165:
  .string "enum"
.data
.LC164:
  .string "char"
.data
.LC163:
  .string "sizeof"
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
.Lbegin227:
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
  je .Lend227
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
  je .Lend228
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
.Lend228:
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
  je .Lend229
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
.Lend229:
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
  je .Lend230
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
.Lbegin231:
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
  je .Lend231
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin231
.Lend231:
.Lbreak39:
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
.Lend230:
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
  je .Ltrue233
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
  je .Ltrue234
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
  je .Ltrue235
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
  je .Ltrue235
  pushq $0
  jmp .Lend235
.Ltrue235:
  pushq $1
.Lend235:
  popq %rax
  cmpq $1, %rax
  je .Ltrue234
  pushq $0
  jmp .Lend234
.Ltrue234:
  pushq $1
.Lend234:
  popq %rax
  cmpq $1, %rax
  je .Ltrue233
  pushq $0
  jmp .Lend233
.Ltrue233:
  pushq $1
.Lend233:
  popq %rax
  cmpq $0, %rax
  je .Lend232
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
.Lend232:
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
  je .Ltrue237
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
  je .Ltrue239
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
  je .Ltrue239
  pushq $0
  jmp .Lend239
.Ltrue239:
  pushq $1
.Lend239:
  popq %rax
  cmpq $0, %rax
  je .Lend238
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
.Lend238:
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
  je .Ltrue241
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
  je .Ltrue242
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
  je .Ltrue243
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
  je .Ltrue243
  pushq $0
  jmp .Lend243
.Ltrue243:
  pushq $1
.Lend243:
  popq %rax
  cmpq $1, %rax
  je .Ltrue242
  pushq $0
  jmp .Lend242
.Ltrue242:
  pushq $1
.Lend242:
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
  je .Lend244
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
.Lbegin245:
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
  je .Lend245
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin245
.Lend245:
.Lbreak40:
  jmp .Lcontinue35
.Lend244:
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
  je .Lend246
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
  je .Lend247
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
.Lend247:
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
.Lend246:
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
  je .Lend248
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
.Lend248:
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
  je .Lend249
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
.Lend249:
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
  je .Lend250
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
.Lend250:
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
  je .Lend251
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
.Lbegin252:
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
  je .Ltrue253
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
  je .Ltrue253
  pushq $0
  jmp .Lend253
.Ltrue253:
  pushq $1
.Lend253:
  popq %rax
  cmpq $0, %rax
  je .Lend252
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
  jmp .Lbegin252
.Lend252:
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
.Lend251:
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
  je .Lend254
  movq %rbp, %rax
  subq $49, %rax
  pushq %rax
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
.Lbegin255:
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
  je .Lfalse257
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
  je .Lfalse257
  pushq $1
  jmp .Lend257
.Lfalse257:
  pushq $0
.Lend257:
  popq %rax
  cmpq $1, %rax
  je .Ltrue256
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
  je .Ltrue256
  pushq $0
  jmp .Lend256
.Ltrue256:
  pushq $1
.Lend256:
  popq %rax
  cmpq $0, %rax
  je .Lend255
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
  jmp .Lbegin255
.Lend255:
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
  je .Lend258
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
.Lend258:
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
  je .Lfalse260
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
  je .Lfalse260
  pushq $1
  jmp .Lend260
.Lfalse260:
  pushq $0
.Lend260:
  popq %rax
  cmpq $0, %rax
  je .Lend259
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
.Lend259:
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
  je .Lfalse262
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
  je .Lfalse262
  pushq $1
  jmp .Lend262
.Lfalse262:
  pushq $0
.Lend262:
  popq %rax
  cmpq $0, %rax
  je .Lend261
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
.Lend261:
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
  je .Lfalse264
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
.Lend263:
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
  je .Lfalse266
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
.Lend265:
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
  je .Lfalse268
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
.Lend267:
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
  je .Lfalse270
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
.Lend269:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC163(%rip), %rax
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
  je .Lfalse272
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
.Lend271:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC164(%rip), %rax
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
  je .Lfalse274
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
.Lend273:
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
  je .Lfalse276
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
.Lend275:
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
  je .Lfalse278
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
.Lend277:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC167(%rip), %rax
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
  je .Lfalse280
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
.Lend279:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC168(%rip), %rax
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
  je .Lfalse282
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
.Lend281:
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
  je .Lfalse284
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
.Lend283:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC170(%rip), %rax
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
  je .Lfalse288
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
.Lend287:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC172(%rip), %rax
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
  leaq .LC173(%rip), %rax
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
  je .Lfalse292
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
.Lend291:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC174(%rip), %rax
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
  je .Lfalse294
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
.Lend293:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC175(%rip), %rax
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
  je .Lfalse296
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
.Lend295:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC176(%rip), %rax
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
  je .Lfalse298
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
.Lend297:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC177(%rip), %rax
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
  je .Lfalse300
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
.Lend299:
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
  je .Lfalse303
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
  je .Lfalse303
  pushq $1
  jmp .Lend303
.Lfalse303:
  pushq $0
.Lend303:
  popq %rax
  cmpq $1, %rax
  je .Ltrue302
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
  je .Lfalse305
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
  je .Lfalse305
  pushq $1
  jmp .Lend305
.Lfalse305:
  pushq $0
.Lend305:
  popq %rax
  cmpq $1, %rax
  je .Ltrue304
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
  je .Ltrue304
  pushq $0
  jmp .Lend304
.Ltrue304:
  pushq $1
.Lend304:
  popq %rax
  cmpq $1, %rax
  je .Ltrue302
  pushq $0
  jmp .Lend302
.Ltrue302:
  pushq $1
.Lend302:
  popq %rax
  cmpq $0, %rax
  je .Lend301
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
.Lbegin306:
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
  je .Lfalse308
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
  je .Lfalse308
  pushq $1
  jmp .Lend308
.Lfalse308:
  pushq $0
.Lend308:
  popq %rax
  cmpq $1, %rax
  je .Ltrue307
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
  je .Lfalse310
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
  je .Lfalse310
  pushq $1
  jmp .Lend310
.Lfalse310:
  pushq $0
.Lend310:
  popq %rax
  cmpq $1, %rax
  je .Ltrue309
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
  je .Lfalse312
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
  je .Lfalse312
  pushq $1
  jmp .Lend312
.Lfalse312:
  pushq $0
.Lend312:
  popq %rax
  cmpq $1, %rax
  je .Ltrue311
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
  je .Ltrue311
  pushq $0
  jmp .Lend311
.Ltrue311:
  pushq $1
.Lend311:
  popq %rax
  cmpq $1, %rax
  je .Ltrue309
  pushq $0
  jmp .Lend309
.Ltrue309:
  pushq $1
.Lend309:
  popq %rax
  cmpq $1, %rax
  je .Ltrue307
  pushq $0
  jmp .Lend307
.Ltrue307:
  pushq $1
.Lend307:
  popq %rax
  cmpq $0, %rax
  je .Lend306
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
  jmp .Lbegin306
.Lend306:
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
.Lend301:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC178(%rip), %rax
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
  jmp .Lbegin227
.Lend227:
.Lbreak38:
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend313
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
.Lend313:
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
.LC181:
  .string "  .string \"%.*s\"\n"
.data
.LC180:
  .string ".LC%d:\n"
.data
.LC179:
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
.Lbegin314:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend314
  leaq .LC179(%rip), %rax
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
  leaq .LC180(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC181(%rip), %rax
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
  movl (%rax), %eax
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
  jmp .Lbegin314
.Lend314:
.Lbreak44:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC189:
  .string "left value of assignment must be variable: found %d\n"
.data
.LC188:
  .string "  pushq %%rax\n"
.data
.LC187:
  .string "  leaq .LC%d(%%rip), %%rax\n"
.data
.LC186:
  .string "  pushq %%rax\n"
.data
.LC185:
  .string "  leaq %.*s(%%rip), %%rax\n"
.data
.LC184:
  .string "  pushq %%rax\n"
.data
.LC183:
  .string "  subq $%d, %%rax\n"
.data
.LC182:
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
  movl (%rax), %eax
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
  je .Lelse315
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
  jmp .Lend315
.Lelse315:
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
  movl (%rax), %eax
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
  je .Lelse316
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
  movl (%rax), %eax
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
  jmp .Lend316
.Lelse316:
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
  movl (%rax), %eax
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
  je .Lelse317
  leaq .LC185(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC186(%rip), %rax
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
  jmp .Lend317
.Lelse317:
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
  movl (%rax), %eax
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
  je .Lelse318
  leaq .LC187(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC188(%rip), %rax
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
  jmp .Lend318
.Lelse318:
  leaq .LC189(%rip), %rax
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
  movl (%rax), %eax
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
.Lend318:
.Lend317:
.Lend316:
.Lend315:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC230:
  .string "  popq %%rax\n"
.data
.LC229:
  .string "  jmp .Lcontinue%d\n"
.data
.LC228:
  .string "  jmp .Lbreak%d\n"
.data
.LC227:
  .string "  ret\n"
.data
.LC226:
  .string "  popq %%rbp\n"
.data
.LC225:
  .string "  movq %%rbp, %%rsp\n"
.data
.LC224:
  .string "  popq %%rax\n"
.data
.LC223:
  .string "  pushq $0\n"
.data
.LC222:
  .string ".Lbreak%d:\n"
.data
.LC221:
  .string ".Lend%d:\n"
.data
.LC220:
  .string "  jmp .Lbegin%d\n"
.data
.LC219:
  .string ".Lcontinue%d:\n"
.data
.LC218:
  .string "  je .Lend%d\n"
.data
.LC217:
  .string "  cmpq $0, %%rax\n"
.data
.LC216:
  .string "  popq %%rax\n"
.data
.LC215:
  .string ".Lbegin%d:\n"
.data
.LC214:
  .string ".Lbreak%d:\n"
.data
.LC213:
  .string ".Lend%d:\n"
.data
.LC212:
  .string "  jmp .Lbegin%d\n"
.data
.LC211:
  .string "  je .Lend%d\n"
.data
.LC210:
  .string "  cmpq $0, %%rax\n"
.data
.LC209:
  .string "  popq %%rax\n"
.data
.LC208:
  .string ".Lcontinue%d:\n"
.data
.LC207:
  .string ".Lbegin%d:\n"
.data
.LC206:
  .string ".Ldefault%d:\n"
.data
.LC205:
  .string ".Lcase%d_%d:\n"
.data
.LC204:
  .string ".Lbreak%d:\n"
.data
.LC203:
  .string "  jmp .Ldefault%d\n"
.data
.LC202:
  .string "  je .Lcase%d_%d\n"
.data
.LC201:
  .string "  cmpq $%d, %%rax\n"
.data
.LC200:
  .string "  popq %%rax\n"
.data
.LC199:
  .string ".Lend%d:\n"
.data
.LC198:
  .string ".Lelse%d:\n"
.data
.LC197:
  .string "  jmp .Lend%d\n"
.data
.LC196:
  .string "  je .Lelse%d\n"
.data
.LC195:
  .string "  cmpq $0, %%rax\n"
.data
.LC194:
  .string "  popq %%rax\n"
.data
.LC193:
  .string ".Lend%d:\n"
.data
.LC192:
  .string "  je .Lend%d\n"
.data
.LC191:
  .string "  cmpq $0, %%rax\n"
.data
.LC190:
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
  movl (%rax), %eax
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
.Lbegin319:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend319
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
  jmp .Lbegin319
.Lend319:
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
  leaq .LC190(%rip), %rax
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
  leaq .LC192(%rip), %rax
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
  leaq .LC193(%rip), %rax
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
  leaq .LC197(%rip), %rax
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
  leaq .LC198(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC200(%rip), %rax
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
.Lbegin320:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend320
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
  je .Lfalse322
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
  movl (%rax), %eax
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
  je .Lfalse322
  pushq $1
  jmp .Lend322
.Lfalse322:
  pushq $0
.Lend322:
  popq %rax
  cmpq $0, %rax
  je .Lend321
  leaq .LC201(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC202(%rip), %rax
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
  movl (%rax), %eax
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
.Lend321:
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
  je .Lfalse324
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
  movl (%rax), %eax
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
  je .Lfalse324
  pushq $1
  jmp .Lend324
.Lfalse324:
  pushq $0
.Lend324:
  popq %rax
  cmpq $0, %rax
  je .Lend323
  leaq .LC203(%rip), %rax
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
.Lend323:
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
  jmp .Lbegin320
.Lend320:
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
  leaq .LC204(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC205(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC207(%rip), %rax
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
  leaq .LC208(%rip), %rax
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
  leaq .LC209(%rip), %rax
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
  leaq .LC210(%rip), %rax
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
  leaq .LC211(%rip), %rax
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
  leaq .LC212(%rip), %rax
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
  leaq .LC213(%rip), %rax
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
  leaq .LC214(%rip), %rax
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
  je .Lend325
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
.Lend325:
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
  je .Lend326
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
  leaq .LC216(%rip), %rax
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
  leaq .LC217(%rip), %rax
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
.Lend326:
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
  leaq .LC219(%rip), %rax
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
  je .Lend327
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
.Lend327:
  leaq .LC220(%rip), %rax
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
  leaq .LC222(%rip), %rax
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
  je .Lelse328
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
  jmp .Lend328
.Lelse328:
  leaq .LC223(%rip), %rax
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
.Lend328:
  leaq .LC224(%rip), %rax
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
  leaq .LC225(%rip), %rax
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
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_22:
  leaq .LC228(%rip), %rax
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
  leaq .LC229(%rip), %rax
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
.Lbreak45:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC262:
  .string "  ret\n"
.data
.LC261:
  .string "  popq %%rbp\n"
.data
.LC260:
  .string "  movq %%rbp, %%rsp\n"
.data
.LC259:
  .string "invalid size"
.data
.LC258:
  .string "  movb %s, -%d(%%rbp)\n"
.data
.LC257:
  .string "  movl %s, -%d(%%rbp)\n"
.data
.LC256:
  .string "  movq %s, -%d(%%rbp)\n"
.data
.LC255:
  .string "  pushq %s\n"
.data
.LC254:
  .string "  subq $%d, %%rsp\n"
.data
.LC253:
  .string "  movq %%rsp, %%rbp\n"
.data
.LC252:
  .string "  pushq %%rbp\n"
.data
.LC251:
  .string "%.*s:\n"
.data
.LC250:
  .string ".text\n"
.data
.LC249:
  .string ".globl %.*s\n"
.data
.LC248:
  .string "%r9"
.data
.LC247:
  .string "%r8"
.data
.LC246:
  .string "%rcx"
.data
.LC245:
  .string "%rdx"
.data
.LC244:
  .string "%rsi"
.data
.LC243:
  .string "%rdi"
.data
.LC242:
  .string "%r9d"
.data
.LC241:
  .string "%r8d"
.data
.LC240:
  .string "%ecx"
.data
.LC239:
  .string "%edx"
.data
.LC238:
  .string "%esi"
.data
.LC237:
  .string "%edi"
.data
.LC236:
  .string "%r9b"
.data
.LC235:
  .string "%r8b"
.data
.LC234:
  .string "%cl"
.data
.LC233:
  .string "%dl"
.data
.LC232:
  .string "%sil"
.data
.LC231:
  .string "%dil"
.globl gen_function
.text
gen_function:
  pushq %rbp
  movq %rsp, %rbp
  subq $176, %rsp
  movq %rdi, -8(%rbp)
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC231(%rip), %rax
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
  leaq .LC232(%rip), %rax
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
  leaq .LC233(%rip), %rax
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
  pushq $32
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
  pushq $40
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
  leaq .LC237(%rip), %rax
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
  leaq .LC238(%rip), %rax
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
  leaq .LC239(%rip), %rax
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
  pushq $32
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
  pushq $40
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
  leaq .LC243(%rip), %rax
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
  leaq .LC244(%rip), %rax
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
  leaq .LC245(%rip), %rax
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
  pushq $32
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
  pushq $40
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
  leaq .LC249(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC250(%rip), %rax
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
  leaq .LC251(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC252(%rip), %rax
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
  leaq .LC253(%rip), %rax
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
  leaq .LC254(%rip), %rax
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
  movl (%rax), %eax
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
  je .Lend329
  movq %rbp, %rax
  subq $156, %rax
  pushq %rax
  pushq $5
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin330:
  pushq $0
  movq %rbp, %rax
  subq $156, %rax
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
  je .Lend330
  leaq .LC255(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  movq %rbp, %rax
  subq $156, %rax
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
  subq $156, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin330
.Lend330:
.Lbreak48:
.Lend329:
  movq %rbp, %rax
  subq $156, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $160, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin331:
  movq %rbp, %rax
  subq $156, %rax
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
  je .Lfalse332
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
  subq $156, %rax
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
  movl (%rax), %eax
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
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $156, %rax
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
  je .Lelse333
  movq %rbp, %rax
  subq $164, %rax
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
  subq $156, %rax
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
  movl (%rax), %eax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend333
.Lelse333:
  movq %rbp, %rax
  subq $164, %rax
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
  subq $156, %rax
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
  movl (%rax), %eax
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
  subq $156, %rax
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
  movl (%rax), %eax
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
.Lend333:
  movq %rbp, %rax
  subq $168, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin334:
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  je .Lend334
  pushq $8
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  je .Lelse335
  leaq .LC256(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $152, %rax
  pushq %rax
  movq %rbp, %rax
  subq $160, %rax
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
  subq $156, %rax
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
  movl (%rax), %eax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  subq $168, %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  jmp .Lend335
.Lelse335:
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  je .Lelse336
  leaq .LC257(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $104, %rax
  pushq %rax
  movq %rbp, %rax
  subq $160, %rax
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
  subq $156, %rax
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
  movl (%rax), %eax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  subq $168, %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  jmp .Lend336
.Lelse336:
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  je .Lelse337
  leaq .LC258(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  movq %rbp, %rax
  subq $160, %rax
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
  subq $156, %rax
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
  movl (%rax), %eax
  pushq %rax
  movq %rbp, %rax
  subq $168, %rax
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
  subq $168, %rax
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
  addq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend337
.Lelse337:
  leaq .LC259(%rip), %rax
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
.Lend337:
.Lend336:
.Lend335:
.Lcontinue46:
  movq %rbp, %rax
  subq $160, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin334
.Lend334:
.Lbreak50:
.Lcontinue45:
  movq %rbp, %rax
  subq $156, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin331
.Lend331:
.Lbreak49:
  leaq arg_count(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $156, %rax
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
  movl (%rax), %eax
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
  leaq .LC260(%rip), %rax
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
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC389:
  .string "  pushq %%rax\n"
.data
.LC388:
  .string "unreachable"
.data
.LC387:
  .string "  movzbq %%al, %%rax\n"
.data
.LC386:
  .string "  setle %%al\n"
.data
.LC385:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC384:
  .string "  movzbq %%al, %%rax\n"
.data
.LC383:
  .string "  setl %%al\n"
.data
.LC382:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC381:
  .string "  movzbq %%al, %%rax\n"
.data
.LC380:
  .string "  setne %%al\n"
.data
.LC379:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC378:
  .string "  movzbq %%al, %%rax\n"
.data
.LC377:
  .string "  sete %%al\n"
.data
.LC376:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC375:
  .string "  idivq %%rdi\n"
.data
.LC374:
  .string "  cqto\n"
.data
.LC373:
  .string "  imulq %%rdi, %%rax\n"
.data
.LC372:
  .string "  subq %%rdi, %%rax\n"
.data
.LC371:
  .string "  addq %%rdi, %%rax\n"
.data
.LC370:
  .string "  popq %%rax\n"
.data
.LC369:
  .string "  popq %%rdi\n"
.data
.LC368:
  .string ".Lend%d:\n"
.data
.LC367:
  .string "  pushq $1\n"
.data
.LC366:
  .string ".Ltrue%d:\n"
.data
.LC365:
  .string "  jmp .Lend%d\n"
.data
.LC364:
  .string "  pushq $0\n"
.data
.LC363:
  .string "  je .Ltrue%d\n"
.data
.LC362:
  .string "  cmpq $1, %%rax\n"
.data
.LC361:
  .string "  popq %%rax\n"
.data
.LC360:
  .string "  je .Ltrue%d\n"
.data
.LC359:
  .string "  cmpq $1, %%rax\n"
.data
.LC358:
  .string "  popq %%rax\n"
.data
.LC357:
  .string ".Lend%d:\n"
.data
.LC356:
  .string "  pushq $0\n"
.data
.LC355:
  .string ".Lfalse%d:\n"
.data
.LC354:
  .string "  jmp .Lend%d\n"
.data
.LC353:
  .string "  pushq $1\n"
.data
.LC352:
  .string "  je .Lfalse%d\n"
.data
.LC351:
  .string "  cmpq $0, %%rax\n"
.data
.LC350:
  .string "  popq %%rax\n"
.data
.LC349:
  .string "  je .Lfalse%d\n"
.data
.LC348:
  .string "  cmpq $0, %%rax\n"
.data
.LC347:
  .string "  popq %%rax\n"
.data
.LC346:
  .string "  pushq %%rax\n"
.data
.LC345:
  .string "unexpected type"
.data
.LC344:
  .string "  movsbq (%%rax), %%rax\n"
.data
.LC343:
  .string "  movq (%%rax), %%rax\n"
.data
.LC342:
  .string "  movl (%%rax), %%eax\n"
.data
.LC341:
  .string "  popq %%rax\n"
.data
.LC340:
  .string "  pushq $0\n"
.data
.LC339:
  .string "  movq %%rax, -%d(%%rbp)\n"
.data
.LC338:
  .string "  leaq -%d(%%rbp), %%rax\n"
.data
.LC337:
  .string "  movq %%rax, -%d(%%rbp)\n"
.data
.LC336:
  .string "  leaq 16(%%rbp), %%rax\n"
.data
.LC335:
  .string "  movl %%eax, -%d(%%rbp)\n"
.data
.LC334:
  .string "  movl $48, %%eax\n"
.data
.LC333:
  .string "  movl %%eax, -%d(%%rbp)\n"
.data
.LC332:
  .string "  movl $%d, %%eax\n"
.data
.LC331:
  .string "not implemented: return value"
.data
.LC330:
  .string "  pushq %%rax\n"
.data
.LC329:
  .string "  pushq %%rax\n"
.data
.LC328:
  .string "  movslq %%eax, %%rax\n"
.data
.LC327:
  .string "  pushq %%rax\n"
.data
.LC326:
  .string "  movsbq %%al, %%rax\n"
.data
.LC325:
  .string "  pushq $0\n"
.data
.LC324:
  .string "  movq %%rdi, %%rsp\n"
.data
.LC323:
  .string "  popq %%rdi\n"
.data
.LC322:
  .string "  popq %%rdi\n"
.data
.LC321:
  .string "  call %.*s\n"
.data
.LC320:
  .string "  movq $0, %%rax\n"
.data
.LC319:
  .string "  pushq $0\n"
.data
.LC318:
  .string "  pushq %%r10\n"
.data
.LC317:
  .string "  andq $-16, %%rsp\n"
.data
.LC316:
  .string "  movq %%rsp, %%r10\n"
.data
.LC315:
  .string "  popq %s\n"
.data
.LC314:
  .string "%r9"
.data
.LC313:
  .string "%r8"
.data
.LC312:
  .string "%rcx"
.data
.LC311:
  .string "%rdx"
.data
.LC310:
  .string "%rsi"
.data
.LC309:
  .string "%rdi"
.data
.LC308:
  .string "too many arguments"
.data
.LC307:
  .string "not implemented: too big object"
.data
.LC306:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC305:
  .string "  subb $1, %%dil\n"
.data
.LC304:
  .string "  pushq %%rdi\n"
.data
.LC303:
  .string "  movb (%%rax), %%dil\n"
.data
.LC302:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC301:
  .string "  subl $1, %%edi\n"
.data
.LC300:
  .string "  pushq %%rdi\n"
.data
.LC299:
  .string "  movl (%%rax), %%edi\n"
.data
.LC298:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC297:
  .string "  subq $1, %%rdi\n"
.data
.LC296:
  .string "  pushq %%rdi\n"
.data
.LC295:
  .string "  movq (%%rax), %%rdi\n"
.data
.LC294:
  .string " popq %%rax\n"
.data
.LC293:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC292:
  .string "  addb $1, %%dil\n"
.data
.LC291:
  .string "  pushq %%rdi\n"
.data
.LC290:
  .string "  movb (%%rax), %%dil\n"
.data
.LC289:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC288:
  .string "  addl $1, %%edi\n"
.data
.LC287:
  .string "  pushq %%rdi\n"
.data
.LC286:
  .string "  movl (%%rax), %%edi\n"
.data
.LC285:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC284:
  .string "  addq $1, %%rdi\n"
.data
.LC283:
  .string "  pushq %%rdi\n"
.data
.LC282:
  .string "  movq (%%rax), %%rdi\n"
.data
.LC281:
  .string " popq %%rax\n"
.data
.LC280:
  .string "  pushq $0\n"
.data
.LC279:
  .string "  popq %%rax\n"
.data
.LC278:
  .string "  pushq %%rdi\n"
.data
.LC277:
  .string "failed to assign"
.data
.LC276:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC275:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC274:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC273:
  .string "  popq %%rax\n"
.data
.LC272:
  .string "  popq %%rdi\n"
.data
.LC271:
  .string "not implemented: size %d"
.data
.LC270:
  .string "  pushq %%rax\n"
.data
.LC269:
  .string "  movsbq (%%rax), %%rax\n"
.data
.LC268:
  .string "  pushq %%rax\n"
.data
.LC267:
  .string "  movslq (%%rax), %%rax\n"
.data
.LC266:
  .string "  pushq %%r10\n"
.data
.LC265:
  .string "  movq %d(%%rax), %%r10\n"
.data
.LC264:
  .string "  popq %%rax\n"
.data
.LC263:
  .string "  pushq $%d\n"
.globl gen
.text
gen:
  pushq %rbp
  movq %rsp, %rbp
  subq $88, %rsp
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
  movl (%rax), %eax
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
  leaq .LC263(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC264(%rip), %rax
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
.Lbegin338:
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
  je .Lelse339
  leaq .LC265(%rip), %rax
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
  leaq .LC266(%rip), %rax
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
  jmp .Lend339
.Lelse339:
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
  je .Lelse340
  leaq .LC267(%rip), %rax
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
  leaq .LC268(%rip), %rax
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
  jmp .Lend340
.Lelse340:
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
  je .Lelse341
  leaq .LC269(%rip), %rax
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
  jmp .Lend341
.Lelse341:
  leaq .LC271(%rip), %rax
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
.Lend341:
.Lend340:
.Lend339:
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
  je .Lend342
  jmp .Lbreak52
.Lend342:
.Lcontinue47:
  jmp .Lbegin338
.Lend338:
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
  leaq .LC273(%rip), %rax
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
  cmpq $4, %rax
  je .Lcase6_4
  cmpq $1, %rax
  je .Lcase6_1
  cmpq $8, %rax
  je .Lcase6_8
  jmp .Ldefault6
.Lcase6_4:
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
  jmp .Lbreak53
.Lcase6_1:
  leaq .LC275(%rip), %rax
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
  jmp .Lbreak53
.Lcase6_8:
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
  jmp .Lbreak53
.Ldefault6:
  leaq .LC277(%rip), %rax
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
.Lbreak53:
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
.Lbegin343:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend343
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
.Lcontinue48:
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
  jmp .Lbegin343
.Lend343:
.Lbreak54:
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
  cmpq $1, %rax
  je .Lcase7_1
.Lcase7_8:
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
  jmp .Lbreak55
.Lcase7_4:
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
  jmp .Lbreak55
.Lcase7_1:
  leaq .LC290(%rip), %rax
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
  jmp .Lbreak55
.Lbreak55:
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
  je .Lcase8_8
  cmpq $4, %rax
  je .Lcase8_4
  cmpq $1, %rax
  je .Lcase8_1
.Lcase8_8:
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
  jmp .Lbreak56
.Lcase8_4:
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
  leaq .LC301(%rip), %rax
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
  jmp .Lbreak56
.Lcase8_1:
  leaq .LC303(%rip), %rax
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
  leaq .LC304(%rip), %rax
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
  leaq .LC305(%rip), %rax
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
  leaq .LC306(%rip), %rax
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
.Lbegin344:
.Lcontinue49:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend344
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
  je .Lend345
  leaq .LC307(%rip), %rax
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
.Lend345:
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
  jmp .Lbegin344
.Lend344:
.Lbreak57:
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
  je .Lend346
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
  leaq .LC308(%rip), %rax
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
.Lend346:
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC309(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC310(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC311(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC312(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC313(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC314(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $80, %rax
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
.Lbegin347:
  pushq $0
  movq %rbp, %rax
  subq $80, %rax
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
  je .Lend347
  leaq .LC315(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  movq %rbp, %rax
  subq $80, %rax
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
.Lcontinue50:
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin347
.Lend347:
.Lbreak58:
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
  movl (%rax), %eax
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
  movl (%rax), %eax
  pushq %rax
  pushq $5
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend348
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
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend348:
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
  je .Lcase9_1
  cmpq $4, %rax
  je .Lcase9_4
  cmpq $8, %rax
  je .Lcase9_8
  jmp .Ldefault9
.Lcase9_1:
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
  jmp .Lbreak59
.Lcase9_4:
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
  jmp .Lbreak59
.Lcase9_8:
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
  jmp .Lbreak59
.Ldefault9:
  leaq .LC331(%rip), %rax
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
.Lbreak59:
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
  movl (%rax), %eax
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
  leaq .LC332(%rip), %rax
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
  leaq .LC333(%rip), %rax
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
  movl (%rax), %eax
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
  movl (%rax), %eax
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
  leaq .LC337(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC338(%rip), %rax
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
  leaq .LC339(%rip), %rax
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
  movl (%rax), %eax
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
  cmpq $4, %rax
  je .Lcase10_4
  cmpq $8, %rax
  je .Lcase10_8
  cmpq $1, %rax
  je .Lcase10_1
  jmp .Ldefault10
.Lcase10_4:
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
  jmp .Lbreak60
.Lcase10_8:
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
  jmp .Lbreak60
.Lcase10_1:
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
  jmp .Lbreak60
.Ldefault10:
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
.Lbreak60:
  leaq .LC346(%rip), %rax
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
  leaq .LC347(%rip), %rax
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
  leaq .LC348(%rip), %rax
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
  leaq .LC349(%rip), %rax
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
  leaq .LC350(%rip), %rax
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
  leaq .LC351(%rip), %rax
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
  leaq .LC352(%rip), %rax
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
  leaq .LC353(%rip), %rax
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
  leaq .LC354(%rip), %rax
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
  leaq .LC355(%rip), %rax
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
  leaq .LC363(%rip), %rax
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
  leaq .LC366(%rip), %rax
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
  leaq .LC368(%rip), %rax
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
  movl (%rax), %eax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lcase11_0
  cmpq $1, %rax
  je .Lcase11_1
  cmpq $2, %rax
  je .Lcase11_2
  cmpq $3, %rax
  je .Lcase11_3
  cmpq $6, %rax
  je .Lcase11_6
  cmpq $7, %rax
  je .Lcase11_7
  cmpq $8, %rax
  je .Lcase11_8
  cmpq $9, %rax
  je .Lcase11_9
  jmp .Ldefault11
.Lcase11_0:
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
  jmp .Lbreak61
.Lcase11_1:
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
  jmp .Lbreak61
.Lcase11_2:
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
  jmp .Lbreak61
.Lcase11_3:
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
  jmp .Lbreak61
.Lcase11_6:
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
  leaq .LC377(%rip), %rax
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
  leaq .LC378(%rip), %rax
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
  jmp .Lbreak61
.Lcase11_7:
  leaq .LC379(%rip), %rax
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
  jmp .Lbreak61
.Lcase11_8:
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
  jmp .Lbreak61
.Lcase11_9:
  leaq .LC385(%rip), %rax
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
  jmp .Lbreak61
.Ldefault11:
  leaq .LC388(%rip), %rax
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
.Lbreak61:
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
.Lbreak51:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC390:
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
  leaq .LC390(%rip), %rax
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
.LC396:
  .string "\n"
.data
.LC395:
  .string "^ "
.data
.LC394:
  .string ""
.data
.LC393:
  .string "%*s"
.data
.LC392:
  .string "%.*s\n"
.data
.LC391:
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
.Lbegin349:
.Lcontinue51:
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
  je .Lfalse350
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
  je .Lfalse350
  pushq $1
  jmp .Lend350
.Lfalse350:
  pushq $0
.Lend350:
  popq %rax
  cmpq $0, %rax
  je .Lend349
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  subq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin349
.Lend349:
.Lbreak62:
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
.Lbegin351:
.Lcontinue52:
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
  je .Lend351
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin351
.Lend351:
.Lbreak63:
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
.Lbegin352:
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
  je .Lend352
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
  je .Lend353
  movq %rbp, %rax
  subq $60, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
.Lend353:
.Lcontinue53:
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
 popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin352
.Lend352:
.Lbreak64:
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC391(%rip), %rax
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
  leaq .LC392(%rip), %rax
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
  leaq .LC393(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  leaq .LC394(%rip), %rax
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
  leaq .LC395(%rip), %rax
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
  leaq .LC396(%rip), %rax
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
.LC397:
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
  je .Lend354
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC397(%rip), %rax
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
.Lend354:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC401:
  .string "%s: fseek"
.data
.LC400:
  .string "%s: fseek"
.data
.LC399:
  .string "cannot open %s"
.data
.LC398:
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
  leaq .LC398(%rip), %rax
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
  je .Lend355
  leaq .LC399(%rip), %rax
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
.Lend355:
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
  je .Lend356
  leaq .LC400(%rip), %rax
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
.Lend356:
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
  je .Lend357
  leaq .LC401(%rip), %rax
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
.Lend357:
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
  je .Ltrue359
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
  je .Ltrue359
  pushq $0
  jmp .Lend359
.Ltrue359:
  pushq $1
.Lend359:
  popq %rax
  cmpq $0, %rax
  je .Lend358
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
.Lend358:
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
.LC402:
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
.Lbegin360:
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
  je .Lend360
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
  je .Lend361
  jmp .Lbreak65
.Lend361:
.Lcontinue54:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
 popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin360
.Lend360:
.Lbreak65:
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
  je .Lend362
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
  leaq .LC402(%rip), %rax
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
.Lend362:
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
.LC410:
  .string "  .zero %d\n"
.data
.LC409:
  .string "%.*s:\n"
.data
.LC408:
  .string ".data\n"
.data
.LC407:
  .string ".globl %.*s\n"
.data
.LC406:
  .string "  .zero 8\n"
.data
.LC405:
  .string "NULL:\n"
.data
.LC404:
  .string ".data\n"
.data
.LC403:
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
  je .Lend363
  leaq .LC403(%rip), %rax
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
.Lend363:
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
.Lbegin364:
.Lcontinue55:
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
  je .Lend364
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
  movl (%rax), %eax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lcase12_0
  cmpq $2, %rax
  je .Lcase12_2
  jmp .Ldefault12
.Lcase12_0:
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
  jmp .Lbreak67
.Lcase12_2:
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
  je .Lend365
  leaq .LC407(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC408(%rip), %rax
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
  leaq .LC409(%rip), %rax
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
  movl (%rax), %eax
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
  leaq .LC410(%rip), %rax
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
  movl (%rax), %eax
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
.Lend365:
  jmp .Lbreak67
.Ldefault12:
  jmp .Lbreak67
.Lbreak67:
  jmp .Lbegin364
.Lend364:
.Lbreak66:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
