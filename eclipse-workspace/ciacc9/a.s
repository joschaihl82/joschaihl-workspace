.data
NULL:
  .zero 8
.globl new_string
.text
new_string:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %%rdi, -8(%rbp)
  movl %%esi, -12(%rbp)
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $16
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
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
  je .Lfalse0
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call memcmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse0
  pushq $1
  jmp .Lend0
.Lfalse0:
  pushq $0
.Lend0:
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
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strlen
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse1
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call memcmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse1
  pushq $1
  jmp .Lend1
.Lfalse1:
  pushq $0
.Lend1:
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.globl new_type
.text
new_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movl %%edi, -4(%rbp)
  movq %%rsi, -12(%rbp)
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movq %%rdi, -8(%rbp)
  movb %%sil, -9(%rbp)
  movq %rbp, %rax
  subq $17, %rax
  pushq %rax
  pushq $4
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movl %%edi, -4(%rbp)
  movl %%esi, -8(%rbp)
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
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %%rdx, -24(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse2
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend2
.Lelse2:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend2:
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
  je .Lend3
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
.Lend3:
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
  je .Lelse4
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  je .Lend5
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend5:
  jmp .Lend4
.Lelse4:
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
  je .Lelse6
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call align
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call align
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend6
.Lelse6:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call align
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend6:
.Lend4:
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
  movq %%rdi, -8(%rbp)
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  movslq (%rax), %rax
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC1:
  .string "\n"
.globl cerror
.text
cerror:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  pushq %%r9
  pushq %%r8
  pushq %%rcx
  pushq %%rdx
  pushq %%rsi
  pushq %%rdi
  movq %%rdi, -8(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  pushq %rax
  popq %rax
  movl $8, %eax
  movl %eax, -32(%rbp)
  movl $48, %eax
  movl %eax, -28(%rbp)
  leaq 16(%rbp), %rax
  movq %rax, -24(%rbp)
  leaq -88(%rbp), %rax
  movq %rax, -16(%rbp)
  pushq $0
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
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call vfprintf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC1(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fprintf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $1
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call exit
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC7:
  .string "\n"
.data
.LC6:
  .string "^ "
.data
.LC5:
  .string ""
.data
.LC4:
  .string "%*s"
.data
.LC3:
  .string "%.*s\n"
.data
.LC2:
  .string "%s:%d: "
.globl _error_at
.text
_error_at:
  pushq %rbp
  movq %rsp, %rbp
  subq $80, %rsp
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %%rdx, -40(%rbp)
  movq %%rcx, -32(%rbp)
  movq %%r8, -24(%rbp)
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
.Lbegin7:
.Lcontinue1:
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
  je .Lfalse8
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
  je .Lfalse8
  pushq $1
  jmp .Lend8
.Lfalse8:
  pushq $0
.Lend8:
  popq %rax
  cmpq $0, %rax
  je .Lend7
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  subq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin7
.Lend7:
.Lbreak2:
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
.Lbegin9:
.Lcontinue2:
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
  je .Lend9
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin9
.Lend9:
.Lbreak3:
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
.Lbegin10:
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
  je .Lend10
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
  je .Lend11
  movq %rbp, %rax
  subq $60, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
.Lend11:
.Lcontinue3:
  movq %rbp, %rax
  subq $68, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin10
.Lend10:
.Lbreak4:
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC2(%rip), %rax
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fprintf
  addq $8, %rsp
  popq %rsp
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
  leaq .LC3(%rip), %rax
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fprintf
  addq $8, %rsp
  popq %rsp
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
  leaq .LC4(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  leaq .LC5(%rip), %rax
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fprintf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC6(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fprintf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call vfprintf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq stderr(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC7(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fprintf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $1
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call exit
  addq $8, %rsp
  popq %rsp
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
  pushq %%r9
  pushq %%r8
  pushq %%rcx
  pushq %%rdx
  pushq %%rsi
  pushq %%rdi
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  pushq %rax
  popq %rax
  movl $16, %eax
  movl %eax, -40(%rbp)
  movl $48, %eax
  movl %eax, -36(%rbp)
  leaq 16(%rbp), %rax
  movq %rax, -32(%rbp)
  leaq -96(%rbp), %rax
  movq %rax, -24(%rbp)
  pushq $0
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
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call _error_at
  addq $8, %rsp
  popq %rsp
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
  pushq %%r9
  pushq %%r8
  pushq %%rcx
  pushq %%rdx
  pushq %%rsi
  pushq %%rdi
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  popq %rax
  pushq %rax
  popq %rax
  movl $16, %eax
  movl %eax, -40(%rbp)
  movl $48, %eax
  movl %eax, -36(%rbp)
  leaq 16(%rbp), %rax
  movq %rax, -32(%rbp)
  leaq -96(%rbp), %rax
  movq %rax, -24(%rbp)
  pushq $0
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
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call _error_at
  addq $8, %rsp
  popq %rsp
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
  pushq %%r9
  pushq %%r8
  pushq %%rcx
  pushq %%rdx
  pushq %%rsi
  pushq %%rdi
  movq %%rdi, -8(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  pushq %rax
  popq %rax
  movl $8, %eax
  movl %eax, -32(%rbp)
  movl $48, %eax
  movl %eax, -28(%rbp)
  leaq 16(%rbp), %rax
  movq %rax, -24(%rbp)
  leaq -88(%rbp), %rax
  movq %rax, -16(%rbp)
  pushq $0
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
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call _error_at
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC8:
  .string "assertion failed"
.globl assert
.text
assert:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movb %%dil, -1(%rbp)
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
  je .Lend12
  leaq token(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC8(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend12:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC12:
  .string "%s: fseek"
.data
.LC11:
  .string "%s: fseek"
.data
.LC10:
  .string "cannot open %s"
.data
.LC9:
  .string "r"
.globl read_file
.text
read_file:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %%rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC9(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fopen
  addq $8, %rsp
  popq %rsp
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
  je .Lend13
  leaq .LC10(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend13:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $0
  pushq $2
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fseek
  addq $8, %rsp
  popq %rsp
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
  je .Lend14
  leaq .LC11(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend14:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call ftell
  addq $8, %rsp
  popq %rsp
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fseek
  addq $8, %rsp
  popq %rsp
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
  je .Lend15
  leaq .LC12(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend15:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fread
  addq $8, %rsp
  popq %rsp
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
  je .Ltrue17
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
  je .Ltrue17
  pushq $0
  jmp .Lend17
.Ltrue17:
  pushq $1
.Lend17:
  popq %rax
  cmpq $0, %rax
  je .Lend16
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
.Lend16:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call fclose
  addq $8, %rsp
  popq %rsp
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
.LC13:
  .string "."
.globl get_dir
.text
get_dir:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %%rdi, -8(%rbp)
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strlen
  addq $8, %rsp
  popq %rsp
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
.Lbegin18:
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
  je .Lend18
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
  je .Lend19
  jmp .Lbreak5
.Lend19:
.Lcontinue4:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin18
.Lend18:
.Lbreak5:
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
  je .Lend20
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $2
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  leaq .LC13(%rip), %rax
  pushq %rax
  pushq $1
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncpy
  addq $8, %rsp
  popq %rsp
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
.Lend20:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncpy
  addq $8, %rsp
  popq %rsp
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
  movl %%edi, -4(%rbp)
  movq %%rsi, -12(%rbp)
  movq %%rdx, -20(%rbp)
  movl %%ecx, -24(%rbp)
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_string
  addq $8, %rsp
  popq %rsp
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
  movb %%dil, -1(%rbp)
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
  je .Lfalse22
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
  je .Lfalse22
  pushq $1
  jmp .Lend22
.Lfalse22:
  pushq $0
.Lend22:
  popq %rax
  cmpq $1, %rax
  je .Ltrue21
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
  je .Lfalse24
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
  je .Lfalse24
  pushq $1
  jmp .Lend24
.Lfalse24:
  pushq $0
.Lend24:
  popq %rax
  cmpq $1, %rax
  je .Ltrue23
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
  je .Lfalse26
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
  je .Lfalse26
  pushq $1
  jmp .Lend26
.Lfalse26:
  pushq $0
.Lend26:
  popq %rax
  cmpq $1, %rax
  je .Ltrue25
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
  je .Ltrue25
  pushq $0
  jmp .Lend25
.Ltrue25:
  pushq $1
.Lend25:
  popq %rax
  cmpq $1, %rax
  je .Ltrue23
  pushq $0
  jmp .Lend23
.Ltrue23:
  pushq $1
.Lend23:
  popq %rax
  cmpq $1, %rax
  je .Ltrue21
  pushq $0
  jmp .Lend21
.Ltrue21:
  pushq $1
.Lend21:
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
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strlen
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call memcmp
  addq $8, %rsp
  popq %rsp
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
.LC55:
  .string "tokenize failed"
.data
.LC54:
  .string "extern"
.data
.LC53:
  .string "_Bool"
.data
.LC52:
  .string "__builtin_va_list"
.data
.LC51:
  .string "void"
.data
.LC50:
  .string "continue"
.data
.LC49:
  .string "break"
.data
.LC48:
  .string "default"
.data
.LC47:
  .string "case"
.data
.LC46:
  .string "switch"
.data
.LC45:
  .string "typedef"
.data
.LC44:
  .string "union"
.data
.LC43:
  .string "struct"
.data
.LC42:
  .string "enum"
.data
.LC41:
  .string "char"
.data
.LC40:
  .string "sizeof"
.data
.LC39:
  .string "int"
.data
.LC38:
  .string "for"
.data
.LC37:
  .string "while"
.data
.LC36:
  .string "else"
.data
.LC35:
  .string "if"
.data
.LC34:
  .string "return"
.data
.LC33:
  .string "+-*/()<>:;={},&[].!"
.data
.LC32:
  .string "..."
.data
.LC31:
  .string "->"
.data
.LC30:
  .string "tokenize failed: \"*/\" not found"
.data
.LC29:
  .string "*/"
.data
.LC28:
  .string "/*"
.data
.LC27:
  .string "//"
.data
.LC26:
  .string "/="
.data
.LC25:
  .string "*="
.data
.LC24:
  .string "-="
.data
.LC23:
  .string "+="
.data
.LC22:
  .string "--"
.data
.LC21:
  .string "++"
.data
.LC20:
  .string "&&"
.data
.LC19:
  .string "||"
.data
.LC18:
  .string ">="
.data
.LC17:
  .string "<="
.data
.LC16:
  .string "!="
.data
.LC15:
  .string "=="
.data
.LC14:
  .string "#"
.globl tokenize
.text
tokenize:
  pushq %rbp
  movq %rsp, %rbp
  subq $80, %rsp
  movq %%rdi, -8(%rbp)
  movb %%sil, -9(%rbp)
  movq %rbp, %rax
  subq $41, %rax
  pushq %rax
  popq %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $41, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
  movq %rbp, %rax
  subq $41, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin27:
.Lcontinue5:
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
  je .Lend27
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
  je .Lend28
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue5
.Lend28:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call isspace
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend29
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lcontinue5
.Lend29:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC14(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend30
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
.Lbegin31:
.Lcontinue6:
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call isspace
  addq $8, %rsp
  popq %rsp
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
  je .Lend31
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin31
.Lend31:
.Lbreak7:
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend30:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC15(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue33
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC16(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue34
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC17(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue35
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC18(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue35
  pushq $0
  jmp .Lend35
.Ltrue35:
  pushq $1
.Lend35:
  popq %rax
  cmpq $1, %rax
  je .Ltrue34
  pushq $0
  jmp .Lend34
.Ltrue34:
  pushq $1
.Lend34:
  popq %rax
  cmpq $1, %rax
  je .Ltrue33
  pushq $0
  jmp .Lend33
.Ltrue33:
  pushq $1
.Lend33:
  popq %rax
  cmpq $0, %rax
  je .Lend32
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend32:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC19(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue37
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC20(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue37
  pushq $0
  jmp .Lend37
.Ltrue37:
  pushq $1
.Lend37:
  popq %rax
  cmpq $0, %rax
  je .Lend36
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend36:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC21(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue39
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC22(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue39
  pushq $0
  jmp .Lend39
.Ltrue39:
  pushq $1
.Lend39:
  popq %rax
  cmpq $0, %rax
  je .Lend38
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend38:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC23(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue41
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC24(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue42
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC25(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue43
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC26(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue43
  pushq $0
  jmp .Lend43
.Ltrue43:
  pushq $1
.Lend43:
  popq %rax
  cmpq $1, %rax
  je .Ltrue42
  pushq $0
  jmp .Lend42
.Ltrue42:
  pushq $1
.Lend42:
  popq %rax
  cmpq $1, %rax
  je .Ltrue41
  pushq $0
  jmp .Lend41
.Ltrue41:
  pushq $1
.Lend41:
  popq %rax
  cmpq $0, %rax
  je .Lend40
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend40:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC27(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend44
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
.Lbegin45:
.Lcontinue7:
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
  je .Lend45
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin45
.Lend45:
.Lbreak8:
  jmp .Lcontinue5
.Lend44:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC28(%rip), %rax
  pushq %rax
  pushq $2
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lend46
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
  leaq .LC29(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strstr
  addq $8, %rsp
  popq %rsp
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
  je .Lend47
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC30(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend47:
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
  jmp .Lcontinue5
.Lend46:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC31(%rip), %rax
  pushq %rax
  pushq $2
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lend48
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend48:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC32(%rip), %rax
  pushq %rax
  pushq $3
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lend49
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend49:
  leaq .LC33(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strchr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend50
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue5
.Lend50:
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
  je .Lend51
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin52:
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
  je .Ltrue53
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
  je .Ltrue53
  pushq $0
  jmp .Lend53
.Ltrue53:
  pushq $1
.Lend53:
  popq %rax
  cmpq $0, %rax
  je .Lend52
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
.Lcontinue8:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin52
.Lend52:
.Lbreak9:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lcontinue5
.Lend51:
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
  je .Lend54
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin55:
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
  je .Lfalse57
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
  je .Lfalse57
  pushq $1
  jmp .Lend57
.Lfalse57:
  pushq $0
.Lend57:
  popq %rax
  cmpq $1, %rax
  je .Ltrue56
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
  je .Ltrue56
  pushq $0
  jmp .Lend56
.Ltrue56:
  pushq $1
.Lend56:
  popq %rax
  cmpq $0, %rax
  je .Lend55
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
.Lcontinue9:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin55
.Lend55:
.Lbreak10:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lcontinue5
.Lend54:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call isdigit
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend58
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strtol
  addq $8, %rsp
  popq %rsp
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend58:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC34(%rip), %rax
  pushq %rax
  pushq $6
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse60
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse60
  pushq $1
  jmp .Lend60
.Lfalse60:
  pushq $0
.Lend60:
  popq %rax
  cmpq $0, %rax
  je .Lend59
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend59:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC35(%rip), %rax
  pushq %rax
  pushq $2
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse62
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse62
  pushq $1
  jmp .Lend62
.Lfalse62:
  pushq $0
.Lend62:
  popq %rax
  cmpq $0, %rax
  je .Lend61
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend61:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC36(%rip), %rax
  pushq %rax
  pushq $4
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse64
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse64
  pushq $1
  jmp .Lend64
.Lfalse64:
  pushq $0
.Lend64:
  popq %rax
  cmpq $0, %rax
  je .Lend63
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend63:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC37(%rip), %rax
  pushq %rax
  pushq $5
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse66
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse66
  pushq $1
  jmp .Lend66
.Lfalse66:
  pushq $0
.Lend66:
  popq %rax
  cmpq $0, %rax
  je .Lend65
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend65:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC38(%rip), %rax
  pushq %rax
  pushq $3
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse68
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse68
  pushq $1
  jmp .Lend68
.Lfalse68:
  pushq $0
.Lend68:
  popq %rax
  cmpq $0, %rax
  je .Lend67
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend67:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC39(%rip), %rax
  pushq %rax
  pushq $3
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse70
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse70
  pushq $1
  jmp .Lend70
.Lfalse70:
  pushq $0
.Lend70:
  popq %rax
  cmpq $0, %rax
  je .Lend69
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend69:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC40(%rip), %rax
  pushq %rax
  pushq $6
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse72
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse72
  pushq $1
  jmp .Lend72
.Lfalse72:
  pushq $0
.Lend72:
  popq %rax
  cmpq $0, %rax
  je .Lend71
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend71:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC41(%rip), %rax
  pushq %rax
  pushq $4
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse74
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse74
  pushq $1
  jmp .Lend74
.Lfalse74:
  pushq $0
.Lend74:
  popq %rax
  cmpq $0, %rax
  je .Lend73
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend73:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC42(%rip), %rax
  pushq %rax
  pushq $4
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse76
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse76
  pushq $1
  jmp .Lend76
.Lfalse76:
  pushq $0
.Lend76:
  popq %rax
  cmpq $0, %rax
  je .Lend75
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend75:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC43(%rip), %rax
  pushq %rax
  pushq $6
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse78
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse78
  pushq $1
  jmp .Lend78
.Lfalse78:
  pushq $0
.Lend78:
  popq %rax
  cmpq $0, %rax
  je .Lend77
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend77:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC44(%rip), %rax
  pushq %rax
  pushq $5
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse80
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse80
  pushq $1
  jmp .Lend80
.Lfalse80:
  pushq $0
.Lend80:
  popq %rax
  cmpq $0, %rax
  je .Lend79
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend79:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC45(%rip), %rax
  pushq %rax
  pushq $7
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse82
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse82
  pushq $1
  jmp .Lend82
.Lfalse82:
  pushq $0
.Lend82:
  popq %rax
  cmpq $0, %rax
  je .Lend81
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend81:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC46(%rip), %rax
  pushq %rax
  pushq $6
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse84
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse84
  pushq $1
  jmp .Lend84
.Lfalse84:
  pushq $0
.Lend84:
  popq %rax
  cmpq $0, %rax
  je .Lend83
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend83:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC47(%rip), %rax
  pushq %rax
  pushq $4
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse86
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse86
  pushq $1
  jmp .Lend86
.Lfalse86:
  pushq $0
.Lend86:
  popq %rax
  cmpq $0, %rax
  je .Lend85
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend85:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC48(%rip), %rax
  pushq %rax
  pushq $7
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse88
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse88
  pushq $1
  jmp .Lend88
.Lfalse88:
  pushq $0
.Lend88:
  popq %rax
  cmpq $0, %rax
  je .Lend87
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend87:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC49(%rip), %rax
  pushq %rax
  pushq $5
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse90
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse90
  pushq $1
  jmp .Lend90
.Lfalse90:
  pushq $0
.Lend90:
  popq %rax
  cmpq $0, %rax
  je .Lend89
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend89:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC50(%rip), %rax
  pushq %rax
  pushq $8
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse92
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse92
  pushq $1
  jmp .Lend92
.Lfalse92:
  pushq $0
.Lend92:
  popq %rax
  cmpq $0, %rax
  je .Lend91
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend91:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC51(%rip), %rax
  pushq %rax
  pushq $4
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse94
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse94
  pushq $1
  jmp .Lend94
.Lfalse94:
  pushq $0
.Lend94:
  popq %rax
  cmpq $0, %rax
  je .Lend93
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend93:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC52(%rip), %rax
  pushq %rax
  pushq $17
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse96
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse96
  pushq $1
  jmp .Lend96
.Lfalse96:
  pushq $0
.Lend96:
  popq %rax
  cmpq $0, %rax
  je .Lend95
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend95:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC53(%rip), %rax
  pushq %rax
  pushq $5
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse98
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse98
  pushq $1
  jmp .Lend98
.Lfalse98:
  pushq $0
.Lend98:
  popq %rax
  cmpq $0, %rax
  je .Lend97
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend97:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC54(%rip), %rax
  pushq %rax
  pushq $6
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncmp
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse100
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call is_alnum
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse100
  pushq $1
  jmp .Lend100
.Lfalse100:
  pushq $0
.Lend100:
  popq %rax
  cmpq $0, %rax
  je .Lend99
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend99:
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
  je .Lfalse103
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
  je .Lfalse103
  pushq $1
  jmp .Lend103
.Lfalse103:
  pushq $0
.Lend103:
  popq %rax
  cmpq $1, %rax
  je .Ltrue102
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
  je .Lfalse105
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
  je .Lfalse105
  pushq $1
  jmp .Lend105
.Lfalse105:
  pushq $0
.Lend105:
  popq %rax
  cmpq $1, %rax
  je .Ltrue104
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
  je .Ltrue104
  pushq $0
  jmp .Lend104
.Ltrue104:
  pushq $1
.Lend104:
  popq %rax
  cmpq $1, %rax
  je .Ltrue102
  pushq $0
  jmp .Lend102
.Ltrue102:
  pushq $1
.Lend102:
  popq %rax
  cmpq $0, %rax
  je .Lend101
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
.Lbegin106:
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
  je .Lfalse108
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
  je .Lfalse108
  pushq $1
  jmp .Lend108
.Lfalse108:
  pushq $0
.Lend108:
  popq %rax
  cmpq $1, %rax
  je .Ltrue107
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
  je .Lfalse110
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
  je .Lfalse110
  pushq $1
  jmp .Lend110
.Lfalse110:
  pushq $0
.Lend110:
  popq %rax
  cmpq $1, %rax
  je .Ltrue109
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
  je .Lfalse112
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
  je .Lfalse112
  pushq $1
  jmp .Lend112
.Lfalse112:
  pushq $0
.Lend112:
  popq %rax
  cmpq $1, %rax
  je .Ltrue111
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
  je .Ltrue111
  pushq $0
  jmp .Lend111
.Ltrue111:
  pushq $1
.Lend111:
  popq %rax
  cmpq $1, %rax
  je .Ltrue109
  pushq $0
  jmp .Lend109
.Ltrue109:
  pushq $1
.Lend109:
  popq %rax
  cmpq $1, %rax
  je .Ltrue107
  pushq $0
  jmp .Lend107
.Ltrue107:
  pushq $1
.Lend107:
  popq %rax
  cmpq $0, %rax
  je .Lend106
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
.Lcontinue10:
  movq %rbp, %rax
  subq $57, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rdi
  pushq %rdi
  addq $1, %rdi
  movq %rdi, (%rax)
  popq %rax
  jmp .Lbegin106
.Lend106:
.Lbreak11:
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
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
  jmp .Lcontinue5
.Lend101:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC55(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lbegin27
.Lend27:
.Lbreak6:
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend113
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_token
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
.Lend113:
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
.globl once_file
.data
once_file:
  .zero 800
.globl macros
.data
macros:
  .zero 800
.globl mi
.data
mi:
  .zero 4
.globl find_macro
.text
find_macro:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %%rdi, -8(%rbp)
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin114:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  leaq mi(%rip), %rax
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
  je .Lend114
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq macros(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend115
  leaq macros(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
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
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend115:
.Lcontinue11:
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin114
.Lend114:
.Lbreak12:
  leaq NULL(%rip), %rax
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
.globl remove_newline
.text
remove_newline:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %%rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
.Lbegin116:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend116
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
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse117
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse118
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
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend118
.Lelse118:
  movq %rbp, %rax
  subq $8, %rax
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
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend118:
  jmp .Lend117
.Lelse117:
  movq %rbp, %rax
  subq $16, %rax
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
.Lend117:
.Lcontinue12:
  movq %rbp, %rax
  subq $24, %rax
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
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin116
.Lend116:
.Lbreak13:
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
.LC66:
  .string "#define"
.data
.LC65:
  .string "once"
.data
.LC64:
  .string "#pragma"
.data
.LC63:
  .string "#endif"
.data
.LC62:
  .string "#endif"
.data
.LC61:
  .string "#else"
.data
.LC60:
  .string "#endif"
.data
.LC59:
  .string "#else"
.data
.LC58:
  .string "#ifdef"
.data
.LC57:
  .string "%s/%s"
.data
.LC56:
  .string "#include"
.globl preprocess
.text
preprocess:
  pushq %rbp
  movq %rsp, %rbp
  subq $112, %rsp
  movq %%rdi, -8(%rbp)
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
  leaq NULL(%rip), %rax
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
.Lbegin119:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend119
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
  pushq $18
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse120
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
  leaq .LC56(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend121
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
  leaq filename(%rip), %rax
  pushq %rax
  pushq $1
  pushq $100
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  pushq $1
  pushq $200
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  pushq $16
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq filename(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strncpy
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $200
  leaq .LC57(%rip), %rax
  pushq %rax
  leaq dir_name(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%r8
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call snprintf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call read_file
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
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
  pushq $0
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call tokenize
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call preprocess
  addq $8, %rsp
  popq %rsp
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
  subq $64, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend122
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin123:
.Lcontinue14:
  movq %rbp, %rax
  subq $72, %rax
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
  je .Lend123
  movq %rbp, %rax
  subq $72, %rax
  pushq %rax
  movq %rbp, %rax
  subq $72, %rax
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
  jmp .Lbegin123
.Lend123:
.Lbreak15:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse124
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
  subq $64, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend124
.Lelse124:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend124:
  movq %rbp, %rax
  subq $16, %rax
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
.Lend122:
.Lend121:
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
  leaq .LC58(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend125
.Lbegin126:
.Lcontinue15:
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
  leaq .LC59(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue127
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
  leaq .LC60(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue127
  pushq $0
  jmp .Lend127
.Ltrue127:
  pushq $1
.Lend127:
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
  jmp .Lbegin126
.Lend126:
.Lbreak16:
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
  leaq .LC61(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse128
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
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse129
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
  jmp .Lend129
.Lelse129:
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
.Lend129:
.Lbegin130:
.Lcontinue16:
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
  leaq .LC62(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
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
  jmp .Lbegin130
.Lend130:
.Lbreak17:
  jmp .Lend128
.Lelse128:
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
  leaq .LC63(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend128:
.Lend125:
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
  leaq .LC64(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend131
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
  leaq .LC65(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_chr_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend132
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin133:
.Lcontinue17:
  leaq once_file(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $76, %rax
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
  popq %rax
  cmpq $0, %rax
  je .Lend133
  leaq once_file(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $76, %rax
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
  leaq filename(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call strcmp
  addq $8, %rsp
  popq %rsp
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
  je .Lend134
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend134:
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin133
.Lend133:
.Lbreak18:
  leaq once_file(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $76, %rax
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
.Lend132:
.Lend131:
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
  leaq .LC66(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call startswith
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend135
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  pushq $1
  pushq $16
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  subq $84, %rax
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
.Lbegin136:
.Lcontinue18:
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
  je .Lend136
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
  jmp .Lbegin136
.Lend136:
.Lbreak19:
  movq %rbp, %rax
  subq $92, %rax
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
  subq $92, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq macros(%rip), %rax
  pushq %rax
  leaq mi(%rip), %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  pushq $8
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend135:
  jmp .Lend120
.Lelse120:
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
  je .Lend137
  movq %rbp, %rax
  subq $84, %rax
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_macro
  addq $8, %rsp
  popq %rsp
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
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend138
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse139
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
  jmp .Lend139
.Lelse139:
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
.Lend139:
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
  jmp .Lcontinue13
.Lend138:
  movq %rbp, %rax
  subq $100, %rax
  pushq %rax
  movq %rbp, %rax
  subq $84, %rax
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
.Lbegin140:
.Lcontinue19:
  movq %rbp, %rax
  subq $100, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend140
  movq %rbp, %rax
  subq $108, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $108, %rax
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
  subq $100, %rax
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
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $108, %rax
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
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $108, %rax
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
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse141
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
  subq $108, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend141
.Lelse141:
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
.Lend141:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $108, %rax
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
  subq $100, %rax
  pushq %rax
  movq %rbp, %rax
  subq $100, %rax
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
  jmp .Lbegin140
.Lend140:
.Lbreak20:
  jmp .Lcontinue13
.Lend137:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lelse142
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
  jmp .Lend142
.Lelse142:
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
.Lend142:
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
.Lend120:
.Lcontinue13:
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
  jmp .Lbegin119
.Lend119:
.Lbreak14:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call remove_newline
  addq $8, %rsp
  popq %rsp
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
  movq %%rdi, -8(%rbp)
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
.Lbegin143:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend143
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend144
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
.Lend144:
.Lcontinue20:
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
  jmp .Lbegin143
.Lend143:
.Lbreak21:
  leaq NULL(%rip), %rax
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
.globl new_lvar
.text
new_lvar:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $32
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  je .Lelse145
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend145
.Lelse145:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend145:
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
  movq %%rdi, -8(%rbp)
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
.Lbegin146:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend146
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend147
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
.Lend147:
.Lcontinue21:
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
  jmp .Lbegin146
.Lend146:
.Lbreak22:
  leaq NULL(%rip), %rax
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
.globl new_enum_val
.text
new_enum_val:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %%rdi, -8(%rbp)
  movl %%esi, -12(%rbp)
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movq %%rdi, -8(%rbp)
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
.Lbegin148:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend148
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend149
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
.Lend149:
.Lcontinue22:
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
  jmp .Lbegin148
.Lend148:
.Lbreak23:
  leaq NULL(%rip), %rax
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
.globl find_struct_field
.text
find_struct_field:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
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
.Lbegin150:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend150
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend151
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
.Lend151:
.Lcontinue23:
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
  jmp .Lbegin150
.Lend150:
.Lbreak24:
  leaq NULL(%rip), %rax
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
.globl new_gvar
.text
new_gvar:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movq %%rdi, -8(%rbp)
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
.Lbegin152:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend152
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend153
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
.Lend153:
.Lcontinue24:
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
  jmp .Lbegin152
.Lend152:
.Lbreak25:
  leaq NULL(%rip), %rax
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
.globl find_typedef
.text
find_typedef:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %%rdi, -8(%rbp)
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
.Lbegin154:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend154
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend155
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
.Lend155:
.Lcontinue25:
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
  jmp .Lbegin154
.Lend154:
.Lbreak26:
  leaq NULL(%rip), %rax
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
.globl new_typedef
.text
new_typedef:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movq %%rdi, -8(%rbp)
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
.Lbegin156:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend156
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend157
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
.Lend157:
.Lcontinue26:
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
  jmp .Lbegin156
.Lend156:
.Lbreak27:
  leaq NULL(%rip), %rax
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
.globl new_func
.text
new_func:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %%rdi, -8(%rbp)
  movq %%rsi, -16(%rbp)
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movq %%rdi, -8(%rbp)
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
  je .Ltrue159
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_chr_equals
  addq $8, %rsp
  popq %rsp
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
  je .Ltrue159
  pushq $0
  jmp .Lend159
.Ltrue159:
  pushq $1
.Lend159:
  popq %rax
  cmpq $0, %rax
  je .Lend158
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend158:
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
  movl %%edi, -4(%rbp)
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
  je .Lend160
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
.Lend160:
  leaq NULL(%rip), %rax
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
  je .Lend161
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend161:
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
  movl %%edi, -4(%rbp)
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
  movq %%rdi, -8(%rbp)
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
.LC67:
  .string "token mismatch: expected %s"
.globl expect
.text
expect:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %%rdi, -8(%rbp)
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
  je .Ltrue163
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_chr_equals
  addq $8, %rsp
  popq %rsp
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
  je .Ltrue163
  pushq $0
  jmp .Lend163
.Ltrue163:
  pushq $1
.Lend163:
  popq %rax
  cmpq $0, %rax
  je .Lend162
  leaq .LC67(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend162:
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
.LC68:
  .string "token mismatch"
.globl expect_kind
.text
expect_kind:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movl %%edi, -4(%rbp)
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
  je .Lend164
  leaq .LC68(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend164:
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
.LC69:
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
  je .Lend165
  leaq .LC69(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend165:
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
.LC79:
  .string "expected struct name"
.data
.LC78:
  .string "struct %.*s is already defined"
.data
.LC77:
  .string ";"
.data
.LC76:
  .string "expected type"
.data
.LC75:
  .string "}"
.data
.LC74:
  .string "{"
.data
.LC73:
  .string ","
.data
.LC72:
  .string "expected enum name"
.data
.LC71:
  .string "}"
.data
.LC70:
  .string "{"
.globl consume_type_name
.text
consume_type_name:
  pushq %rbp
  movq %rsp, %rbp
  subq $72, %rsp
  pushq $9
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend166
  pushq $0
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend166:
  pushq $10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend167
  pushq $3
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend167:
  pushq $25
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend168
  pushq $5
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend168:
  pushq $26
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend169
  pushq $7
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend169:
  pushq $27
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend170
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $2
  pushq $8
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
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
.Lend170:
  pushq $11
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend171
  pushq $2
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  leaq .LC70(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend172
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin173:
.Lcontinue27:
  leaq .LC71(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lend173
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_ident
  addq $8, %rsp
  popq %rsp
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
  je .Lend174
  leaq .LC72(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend174:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_enum_val
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  leaq .LC73(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  jmp .Lbegin173
.Lend173:
.Lbreak28:
.Lend172:
  pushq $0
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend171:
  pushq $12
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call check_kind
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue176
  pushq $13
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call check_kind
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue176
  pushq $0
  jmp .Lend176
.Ltrue176:
  pushq $1
.Lend176:
  popq %rax
  cmpq $0, %rax
  je .Lend175
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse177
  movq %rbp, %rax
  subq $21, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend177
.Lelse177:
  pushq $12
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
.Lend177:
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  pushq $2
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC74(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse178
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend179
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
.Lend179:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_struct_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin180:
.Lcontinue28:
  leaq .LC75(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lend180
  movq %rbp, %rax
  subq $37, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type
  addq $8, %rsp
  popq %rsp
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
  je .Lend181
  leaq .LC76(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend181:
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call add_field
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC77(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lbegin180
.Lend180:
.Lbreak29:
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend182
  movq %rbp, %rax
  subq $45, %rax
  pushq %rax
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_struct
  addq $8, %rsp
  popq %rsp
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
  je .Lelse183
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
  je .Lelse184
  leaq .LC78(%rip), %rax
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
  movslq (%rax), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lend184
.Lelse184:
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
  movslq (%rax), %rax
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
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lend184:
  jmp .Lend183
.Lelse183:
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
.Lend183:
.Lend182:
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
  je .Lend185
  leaq .LC79(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend185:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $29, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_struct
  addq $8, %rsp
  popq %rsp
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
  je .Lelse186
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_struct_type
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend186
.Lelse186:
  movq %rbp, %rax
  subq $53, %rax
  pushq %rax
  pushq $4
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
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
.Lend186:
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
.Lend178:
.Lend175:
  movq %rbp, %rax
  subq $61, %rax
  pushq %rax
  pushq $2
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
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
  je .Lend187
  movq %rbp, %rax
  subq $69, %rax
  pushq %rax
  movq %rbp, %rax
  subq $61, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_typedef
  addq $8, %rsp
  popq %rsp
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
  je .Lend188
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
.Lend188:
  movq %rbp, %rax
  subq $61, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call go_to
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend187:
  leaq NULL(%rip), %rax
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
.LC90:
  .string "]"
.data
.LC89:
  .string "expected constant expression"
.data
.LC88:
  .string "["
.data
.LC87:
  .string ")"
.data
.LC86:
  .string "..."
.data
.LC85:
  .string ","
.data
.LC84:
  .string "("
.data
.LC83:
  .string "expected identifier"
.data
.LC82:
  .string ")"
.data
.LC81:
  .string "*"
.data
.LC80:
  .string "("
.globl expect_nested_type
.text
expect_nested_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  movq %%rdi, -8(%rbp)
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
  leaq .LC80(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse189
.Lbegin190:
.Lcontinue29:
  leaq .LC81(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend190
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin190
.Lend190:
.Lbreak30:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect_nested_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC82(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lend189
.Lelse189:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_ident
  addq $8, %rsp
  popq %rsp
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
  je .Lend191
  leaq .LC83(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend191:
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  pushq $1
  pushq $16
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
.Lend189:
  leaq .LC84(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend192
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_noident_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_ident
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
.Lbegin193:
.Lcontinue30:
  leaq .LC85(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend193
  leaq .LC86(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend194
  jmp .Lbreak31
.Lend194:
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_noident_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_ident
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  jmp .Lbegin193
.Lend193:
.Lbreak31:
  leaq .LC87(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend192:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin195:
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
  je .Lend195
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lcontinue31:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin195
.Lend195:
.Lbreak32:
.Lbegin196:
.Lcontinue32:
  leaq .LC88(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend196
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
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
  je .Lend197
  leaq .LC89(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend197:
  leaq .LC90(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
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
  jmp .Lbegin196
.Lend196:
.Lbreak33:
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
.LC91:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type_name
  addq $8, %rsp
  popq %rsp
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
  je .Lend198
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend198:
.Lbegin199:
.Lcontinue33:
  leaq .LC91(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend199
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin199
.Lend199:
.Lbreak34:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect_nested_type
  addq $8, %rsp
  popq %rsp
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
.LC100:
  .string "]"
.data
.LC99:
  .string "expected constant expression"
.data
.LC98:
  .string "["
.data
.LC97:
  .string ")"
.data
.LC96:
  .string ","
.data
.LC95:
  .string "("
.data
.LC94:
  .string ")"
.data
.LC93:
  .string "("
.data
.LC92:
  .string "*"
.globl expect_noident_type
.text
expect_noident_type:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %%rdi, -8(%rbp)
.Lbegin200:
.Lcontinue34:
  leaq .LC92(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend200
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbegin200
.Lend200:
.Lbreak35:
  leaq .LC93(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend201
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect_noident_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC94(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend201:
  leaq .LC95(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend202
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_noident_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
.Lbegin203:
.Lcontinue35:
  leaq .LC96(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend203
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_noident_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  jmp .Lbegin203
.Lend203:
.Lbreak36:
  leaq .LC97(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $6
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend202:
.Lbegin204:
.Lcontinue36:
  leaq .LC98(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend204
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
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
  je .Lend205
  leaq .LC99(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend205:
  leaq .LC100(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
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
  jmp .Lbegin204
.Lend204:
.Lbreak37:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type_name
  addq $8, %rsp
  popq %rsp
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
  je .Lend206
  leaq NULL(%rip), %rax
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
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect_noident_type
  addq $8, %rsp
  popq %rsp
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
.LC101:
  .string "internal error"
.globl new_node
.text
new_node:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movl %%edi, -4(%rbp)
  movq %%rsi, -12(%rbp)
  movq %%rdx, -20(%rbp)
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
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
  jmp .Lbreak38
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak38
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak38
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
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
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
  jmp .Lbreak38
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak38
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lbreak38
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
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
  jmp .Lbreak38
.Lcase2_27:
.Lcase2_28:
  leaq .LC101(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lbreak38
.Ldefault2:
  jmp .Lbreak38
.Lbreak38:
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
  movl %%edi, -4(%rbp)
  movq %%rsi, -12(%rbp)
  movq %%rdx, -20(%rbp)
  movq %%rcx, -28(%rbp)
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movl %%edi, -4(%rbp)
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  movb %%dil, -1(%rbp)
  movq %rbp, %rax
  subq $9, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
.LC118:
  .string ";"
.data
.LC117:
  .string "expected block"
.data
.LC116:
  .string ";"
.data
.LC115:
  .string ")"
.data
.LC114:
  .string ","
.data
.LC113:
  .string "failed to parse argument"
.data
.LC112:
  .string "..."
.data
.LC111:
  .string ")"
.data
.LC110:
  .string ")"
.data
.LC109:
  .string "("
.data
.LC108:
  .string "invalid type"
.data
.LC107:
  .string ";"
.data
.LC106:
  .string "expected type"
.data
.LC105:
  .string ";"
.data
.LC104:
  .string "expected struct or union"
.data
.LC103:
  .string ";"
.data
.LC102:
  .string "NULL"
.globl external
.text
external:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  leaq locals(%rip), %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
  leaq .LC102(%rip), %rax
  pushq %rax
  pushq $4
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_string
  addq $8, %rsp
  popq %rsp
  pushq %rax
  pushq $1
  pushq $5
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_gvar
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend207:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $1
  pushq $72
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend208
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
.Lend208:
  pushq $11
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call check_kind
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend209
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type_name
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  leaq .LC103(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
.Lend209:
  pushq $12
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call check_kind
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue211
  pushq $13
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call check_kind
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue211
  pushq $0
  jmp .Lend211
.Ltrue211:
  pushq $1
.Lend211:
  popq %rax
  cmpq $0, %rax
  je .Lend210
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type_name
  addq $8, %rsp
  popq %rsp
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
  je .Lend212
  leaq .LC104(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend212:
  leaq .LC105(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
.Lend210:
  pushq $19
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend213
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type
  addq $8, %rsp
  popq %rsp
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
  je .Lend214
  leaq .LC106(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend214:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typedef
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  leaq .LC107(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
.Lend213:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type
  addq $8, %rsp
  popq %rsp
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
  je .Lend215
  leaq .LC108(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend215:
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
  pushq $6
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse216
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_func
  addq $8, %rsp
  popq %rsp
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
  je .Lend217
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_func
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
.Lend217:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call go_to
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call next
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC109(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend218
  leaq .LC110(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse219
  movq %rbp, %rax
  subq $37, %rax
  pushq %rax
  pushq $1
  popq %rdi
  popq %rax
  movb %dil, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend219
.Lelse219:
  movq %rbp, %rax
  subq $36, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call go_to
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend219:
.Lend218:
  leaq .LC111(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse221
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
  je .Lfalse221
  pushq $1
  jmp .Lend221
.Lfalse221:
  pushq $0
.Lend221:
  popq %rax
  cmpq $0, %rax
  je .Lend220
.Lbegin222:
  leaq .LC112(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend223
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
  jmp .Lbreak39
.Lend223:
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type
  addq $8, %rsp
  popq %rsp
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
  je .Lend224
  leaq .LC113(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend224:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_lvar
  addq $8, %rsp
  popq %rsp
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
  je .Lend225
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_lvar
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend225:
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
  leaq .LC114(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lend226
  jmp .Lbreak39
.Lend226:
.Lcontinue37:
  jmp .Lbegin222
.Lend222:
.Lbreak39:
  leaq .LC115(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend220:
  leaq .LC116(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
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
.Lend227:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call stmt
  addq $8, %rsp
  popq %rsp
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
  je .Lend228
  leaq .LC117(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend228:
  jmp .Lend216
.Lelse216:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_gvar
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC118(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend216:
  leaq locals(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend229
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
.Lend229:
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
.LC141:
  .string ";"
.data
.LC140:
  .string ";"
.data
.LC139:
  .string ";"
.data
.LC138:
  .string ";"
.data
.LC137:
  .string ";"
.data
.LC136:
  .string "}"
.data
.LC135:
  .string "{"
.data
.LC134:
  .string ")"
.data
.LC133:
  .string ")"
.data
.LC132:
  .string ";"
.data
.LC131:
  .string ";"
.data
.LC130:
  .string ";"
.data
.LC129:
  .string ";"
.data
.LC128:
  .string "("
.data
.LC127:
  .string ")"
.data
.LC126:
  .string "("
.data
.LC125:
  .string ")"
.data
.LC124:
  .string "("
.data
.LC123:
  .string ")"
.data
.LC122:
  .string "("
.data
.LC121:
  .string ":"
.data
.LC120:
  .string ":"
.data
.LC119:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse230
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
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
  je .Lend231
  leaq .LC119(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend231:
  leaq .LC120(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend230
.Lelse230:
  pushq $22
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse232
  leaq .LC121(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $21
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend232
.Lelse232:
  pushq $5
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse233
  leaq .LC122(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC123(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call stmt
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $6
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse234
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call stmt
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend234
.Lelse234:
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend234:
  jmp .Lend233
.Lelse233:
  pushq $20
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse235
  leaq .LC124(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC125(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call stmt
  addq $8, %rsp
  popq %rsp
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend235
.Lelse235:
  pushq $7
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse236
  leaq .LC126(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC127(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call stmt
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend236
.Lelse236:
  pushq $8
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse237
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
  subq $48, %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
  subq $56, %rax
  pushq %rax
  leaq NULL(%rip), %rax
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
  subq $64, %rax
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC128(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC129(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lend238
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC130(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend238:
  leaq .LC131(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lend239
  movq %rbp, %rax
  subq $48, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC132(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend239:
  leaq .LC133(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lend240
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC134(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend240:
  movq %rbp, %rax
  subq $64, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call stmt
  addq $8, %rsp
  popq %rsp
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend237
.Lelse237:
  leaq .LC135(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse241
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $26
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
.Lbegin242:
  leaq .LC136(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
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
  je .Lend242
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call stmt
  addq $8, %rsp
  popq %rsp
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lcontinue38:
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
  jmp .Lbegin242
.Lend242:
.Lbreak40:
  jmp .Lend241
.Lelse241:
  pushq $4
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse243
  leaq .LC137(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse244
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $16
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend244
.Lelse244:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $16
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC138(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend244:
  jmp .Lend243
.Lelse243:
  pushq $23
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse245
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $22
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC139(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lend245
.Lelse245:
  pushq $24
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse246
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $23
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC140(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lend246
.Lelse246:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC141(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend246:
.Lend245:
.Lend243:
.Lend241:
.Lend237:
.Lend236:
.Lend235:
.Lend233:
.Lend232:
.Lend230:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC146:
  .string "/="
.data
.LC145:
  .string "*="
.data
.LC144:
  .string "-="
.data
.LC143:
  .string "+="
.data
.LC142:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call logical
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC142(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse247
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend247
.Lelse247:
  leaq .LC143(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse248
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend248
.Lelse248:
  leaq .LC144(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse249
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend249
.Lelse249:
  leaq .LC145(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse250
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend250
.Lelse250:
  leaq .LC146(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend251
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend251:
.Lend250:
.Lend249:
.Lend248:
.Lend247:
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
.LC148:
  .string "||"
.data
.LC147:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call equality
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin252:
  leaq .LC147(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse253
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call logical
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend253
.Lelse253:
  leaq .LC148(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse254
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call logical
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend254
.Lelse254:
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
.Lend254:
.Lend253:
.Lcontinue39:
  jmp .Lbegin252
.Lend252:
.Lbreak41:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC150:
  .string "!="
.data
.LC149:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call relational
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin255:
  leaq .LC149(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse256
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call relational
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend256
.Lelse256:
  leaq .LC150(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse257
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call relational
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend257
.Lelse257:
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
.Lend257:
.Lend256:
.Lcontinue40:
  jmp .Lbegin255
.Lend255:
.Lbreak42:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC154:
  .string ">"
.data
.LC153:
  .string ">="
.data
.LC152:
  .string "<"
.data
.LC151:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call add
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin258:
  leaq .LC151(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse259
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call add
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend259
.Lelse259:
  leaq .LC152(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse260
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call add
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend260
.Lelse260:
  leaq .LC153(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse261
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $9
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call add
  addq $8, %rsp
  popq %rsp
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend261
.Lelse261:
  leaq .LC154(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse262
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $8
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call add
  addq $8, %rsp
  popq %rsp
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend262
.Lelse262:
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
.Lend262:
.Lend261:
.Lend260:
.Lend259:
.Lcontinue41:
  jmp .Lbegin258
.Lend258:
.Lbreak43:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC158:
  .string "-"
.data
.LC157:
  .string "+"
.data
.LC156:
  .string "-"
.data
.LC155:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call mul
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin263:
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
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue266
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue266
  pushq $0
  jmp .Lend266
.Ltrue266:
  pushq $1
.Lend266:
  popq %rax
  cmpq $0, %rax
  je .Lfalse265
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
  pushq $5
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse265
  pushq $1
  jmp .Lend265
.Lfalse265:
  pushq $0
.Lend265:
  popq %rax
  cmpq $0, %rax
  je .Lend264
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC155(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse267
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend268
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
  leaq NULL(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend268:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call mul
  addq $8, %rsp
  popq %rsp
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend267
.Lelse267:
  leaq .LC156(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend269
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend270
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
  leaq NULL(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend270:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call mul
  addq $8, %rsp
  popq %rsp
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend269:
.Lend267:
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
.Lend264:
  leaq .LC157(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse271
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call mul
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse273
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
  je .Lfalse273
  pushq $1
  jmp .Lend273
.Lfalse273:
  pushq $0
.Lend273:
  popq %rax
  cmpq $0, %rax
  je .Lelse272
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
  jmp .Lend272
.Lelse272:
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend272:
  jmp .Lend271
.Lelse271:
  leaq .LC158(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse274
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call mul
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse276
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
  je .Lfalse276
  pushq $1
  jmp .Lend276
.Lfalse276:
  pushq $0
.Lend276:
  popq %rax
  cmpq $0, %rax
  je .Lelse275
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
  jmp .Lend275
.Lelse275:
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend275:
  jmp .Lend274
.Lelse274:
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
.Lend274:
.Lend271:
.Lcontinue42:
  jmp .Lbegin263
.Lend263:
.Lbreak44:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC160:
  .string "/"
.data
.LC159:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call unary
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin277:
  leaq .LC159(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse278
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call unary
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse280
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
  je .Lfalse280
  pushq $1
  jmp .Lend280
.Lfalse280:
  pushq $0
.Lend280:
  popq %rax
  cmpq $0, %rax
  je .Lelse279
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
  jmp .Lend279
.Lelse279:
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend279:
  jmp .Lend278
.Lelse278:
  leaq .LC160(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse281
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call unary
  addq $8, %rsp
  popq %rsp
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
  je .Lfalse283
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
  je .Lfalse283
  pushq $1
  jmp .Lend283
.Lfalse283:
  pushq $0
.Lend283:
  popq %rax
  cmpq $0, %rax
  je .Lelse282
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
  jmp .Lend282
.Lelse282:
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend282:
  jmp .Lend281
.Lelse281:
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
.Lend281:
.Lend278:
.Lcontinue43:
  jmp .Lbegin277
.Lend277:
.Lbreak45:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC170:
  .string "--"
.data
.LC169:
  .string "++"
.data
.LC168:
  .string "!"
.data
.LC167:
  .string ")"
.data
.LC166:
  .string "("
.data
.LC165:
  .string "dereference failed: not a pointer"
.data
.LC164:
  .string "*"
.data
.LC163:
  .string "&"
.data
.LC162:
  .string "-"
.data
.LC161:
  .string "+"
.globl unary
.text
unary:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  leaq .LC161(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse284
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call postfix
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend284
.Lelse284:
  leaq .LC162(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse285
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call postfix
  addq $8, %rsp
  popq %rsp
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
  je .Lelse286
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
  jmp .Lend286
.Lelse286:
  pushq $1
  pushq $0
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend286:
  jmp .Lend285
.Lelse285:
  leaq .LC163(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse287
  pushq $12
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call postfix
  addq $8, %rsp
  popq %rsp
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend287
.Lelse287:
  leaq .LC164(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse288
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call primary
  addq $8, %rsp
  popq %rsp
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
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse290
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
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
  leaq .LC165(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend289:
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend291
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend291:
  pushq $13
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend288
.Lelse288:
  pushq $15
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse292
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
  leaq .LC166(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend293
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_noident_type
  addq $8, %rsp
  popq %rsp
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
  je .Lend294
  leaq .LC167(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend294:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call go_to
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend293:
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call unary
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend292
.Lelse292:
  leaq .LC168(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse295
  pushq $6
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call postfix
  addq $8, %rsp
  popq %rsp
  pushq %rax
  pushq $0
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend295
.Lelse295:
  leaq .LC169(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse296
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call postfix
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  jmp .Lend296
.Lelse296:
  leaq .LC170(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend297
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call postfix
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend297:
.Lend296:
.Lend295:
.Lend292:
.Lend288:
.Lend287:
.Lend285:
.Lend284:
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call postfix
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC183:
  .string "--"
.data
.LC182:
  .string "++"
.data
.LC181:
  .string "no such field"
.data
.LC180:
  .string "expected struct type"
.data
.LC179:
  .string "expected pointer type"
.data
.LC178:
  .string "expected identifier after '->'"
.data
.LC177:
  .string "->"
.data
.LC176:
  .string "no such field"
.data
.LC175:
  .string "expected struct type"
.data
.LC174:
  .string "expected identifier after '.'"
.data
.LC173:
  .string "."
.data
.LC172:
  .string "]"
.data
.LC171:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call primary
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin298:
  leaq .LC171(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend299
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend300
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
  leaq NULL(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend300:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC172(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lcontinue44
.Lend299:
  leaq .LC173(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend301
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_ident
  addq $8, %rsp
  popq %rsp
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
  je .Lend302
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC174(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend302:
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
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend303
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC175(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend303:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_struct_field
  addq $8, %rsp
  popq %rsp
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
  je .Lend304
  leaq .LC176(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend304:
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue44
.Lend301:
  leaq .LC177(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend305
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_ident
  addq $8, %rsp
  popq %rsp
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
  je .Lend306
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC178(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend306:
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
  pushq $1
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend307
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC179(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend307:
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
  pushq $4
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend308
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC180(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend308:
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_struct_field
  addq $8, %rsp
  popq %rsp
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
  je .Lend309
  leaq .LC181(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_here
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend309:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue44
.Lend305:
  leaq .LC182(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend310
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue44
.Lend310:
  leaq .LC183(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend311
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lcontinue44
.Lend311:
  jmp .Lbreak46
.Lcontinue44:
  jmp .Lbegin298
.Lend298:
.Lbreak46:
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
.LC197:
  .string "unknown escape sequence"
.data
.LC196:
  .string "%.*s is not defined"
.data
.LC195:
  .string ","
.data
.LC194:
  .string ")"
.data
.LC193:
  .string ")"
.data
.LC192:
  .string "va_start"
.data
.LC191:
  .string "("
.data
.LC190:
  .string "}"
.data
.LC189:
  .string ","
.data
.LC188:
  .string "type mismatch"
.data
.LC187:
  .string "{"
.data
.LC186:
  .string "="
.data
.LC185:
  .string ")"
.data
.LC184:
  .string "("
.globl primary
.text
primary:
  pushq %rbp
  movq %rsp, %rbp
  subq $104, %rsp
  leaq .LC184(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend312
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC185(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
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
.Lend312:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $16
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
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
  je .Lend313
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  pushq $1
  pushq $24
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
.Lend313:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_type
  addq $8, %rsp
  popq %rsp
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
  je .Lend314
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_lvar
  addq $8, %rsp
  popq %rsp
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
  je .Lend315
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_lvar
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend315:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $27
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
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
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
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
  leaq .LC186(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend316
  leaq .LC187(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse317
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setne %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend318
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC188(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend318:
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
  leaq NULL(%rip), %rax
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
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
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
.Lbegin319:
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
  je .Lend319
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rdi
  popq %rax
  imulq %rdi, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  je .Lend320
  leaq .LC189(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend320:
.Lcontinue45:
  movq %rbp, %rax
  subq $52, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin319
.Lend319:
.Lbreak47:
  leaq .LC190(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lend317
.Lelse317:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assign
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend317:
.Lend316:
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
.Lend314:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_ident
  addq $8, %rsp
  popq %rsp
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
  je .Lend321
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  leaq .LC191(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse322
  movq %rbp, %rax
  subq $76, %rax
  pushq %rax
  pushq $1
  pushq $48
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call calloc
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_func
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse323
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_func
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend323
.Lelse323:
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  pushq $0
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_type
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
.Lend323:
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
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
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
  leaq .LC192(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call str_chr_equals
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend324
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
.Lend324:
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
  leaq .LC193(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
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
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend325:
.Lbegin326:
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
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expr
  addq $8, %rsp
  popq %rsp
  pushq %rax
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node
  addq $8, %rsp
  popq %rsp
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
  leaq .LC194(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume
  addq $8, %rsp
  popq %rsp
  movsbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse327
  jmp .Lbreak48
  jmp .Lend327
.Lelse327:
  leaq .LC195(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend327:
.Lcontinue46:
  jmp .Lbegin326
.Lend326:
.Lbreak48:
  jmp .Lend322
.Lelse322:
  movq %rbp, %rax
  subq $40, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_lvar
  addq $8, %rsp
  popq %rsp
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
  je .Lelse328
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $27
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
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
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  popq %rax
  movq (%rax), %rax
  pushq %rax
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend328
.Lelse328:
  movq %rbp, %rax
  subq $92, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_enum_val
  addq $8, %rsp
  popq %rsp
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
  je .Lelse329
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend329
.Lelse329:
  movq %rbp, %rax
  subq $100, %rax
  pushq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call find_gvar
  addq $8, %rsp
  popq %rsp
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
  je .Lelse330
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  pushq $28
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq NULL(%rip), %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_typed_node
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend330
.Lelse330:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC196(%rip), %rax
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
  popq %%rcx
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend330:
.Lend329:
.Lend328:
.Lend322:
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
.Lend321:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  pushq $17
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call consume_kind
  addq $8, %rsp
  popq %rsp
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
  je .Lend331
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
  je .Lend332
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
  jmp .Lbreak49
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
  jmp .Lbreak49
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
  jmp .Lbreak49
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
  jmp .Lbreak49
.Ldefault3:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  leaq .LC197(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at_token
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lbreak49:
.Lend332:
  movq %rbp, %rax
  subq $8, %rax
  pushq %rax
  movq %rbp, %rax
  subq $101, %rax
  pushq %rax
  popq %rax
  movsbq (%rax), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_char
  addq $8, %rsp
  popq %rsp
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
.Lend331:
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call expect_number
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call new_node_num
  addq $8, %rsp
  popq %rsp
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
.globl arg_count
.data
arg_count:
  .zero 4
.globl current_stack_size
.data
current_stack_size:
  .zero 4
.data
.LC200:
  .string "  .string \"%.*s\"\n"
.data
.LC199:
  .string ".LC%d:\n"
.data
.LC198:
  .string ".data\n"
.globl gen_string_literal
.text
gen_string_literal:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq %%rdi, -8(%rbp)
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
.Lbegin333:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend333
  leaq .LC198(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC199(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC200(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lcontinue47:
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
  jmp .Lbegin333
.Lend333:
.Lbreak50:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC208:
  .string "left value of assignment must be variable: found %d\n"
.data
.LC207:
  .string "  pushq %%rax\n"
.data
.LC206:
  .string "  leaq .LC%d(%%rip), %%rax\n"
.data
.LC205:
  .string "  pushq %%rax\n"
.data
.LC204:
  .string "  leaq %.*s(%%rip), %%rax\n"
.data
.LC203:
  .string "  pushq %%rax\n"
.data
.LC202:
  .string "  subq $%d, %%rax\n"
.data
.LC201:
  .string "  movq %%rbp, %%rax\n"
.globl gen_lval
.text
gen_lval:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %%rdi, -8(%rbp)
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
  je .Lelse334
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lend334
.Lelse334:
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
  je .Lelse335
  leaq .LC201(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC202(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC203(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lend335
.Lelse335:
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
  je .Lelse336
  leaq .LC204(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC205(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lend336
.Lelse336:
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
  je .Lelse337
  leaq .LC206(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC207(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lend337
.Lelse337:
  leaq .LC208(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend337:
.Lend336:
.Lend335:
.Lend334:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC249:
  .string "  popq %%rax\n"
.data
.LC248:
  .string "  jmp .Lcontinue%d\n"
.data
.LC247:
  .string "  jmp .Lbreak%d\n"
.data
.LC246:
  .string "  ret\n"
.data
.LC245:
  .string "  popq %%rbp\n"
.data
.LC244:
  .string "  movq %%rbp, %%rsp\n"
.data
.LC243:
  .string "  popq %%rax\n"
.data
.LC242:
  .string "  pushq $0\n"
.data
.LC241:
  .string ".Lbreak%d:\n"
.data
.LC240:
  .string ".Lend%d:\n"
.data
.LC239:
  .string "  jmp .Lbegin%d\n"
.data
.LC238:
  .string ".Lcontinue%d:\n"
.data
.LC237:
  .string "  je .Lend%d\n"
.data
.LC236:
  .string "  cmpq $0, %%rax\n"
.data
.LC235:
  .string "  popq %%rax\n"
.data
.LC234:
  .string ".Lbegin%d:\n"
.data
.LC233:
  .string ".Lbreak%d:\n"
.data
.LC232:
  .string ".Lend%d:\n"
.data
.LC231:
  .string "  jmp .Lbegin%d\n"
.data
.LC230:
  .string "  je .Lend%d\n"
.data
.LC229:
  .string "  cmpq $0, %%rax\n"
.data
.LC228:
  .string "  popq %%rax\n"
.data
.LC227:
  .string ".Lcontinue%d:\n"
.data
.LC226:
  .string ".Lbegin%d:\n"
.data
.LC225:
  .string ".Ldefault%d:\n"
.data
.LC224:
  .string ".Lcase%d_%d:\n"
.data
.LC223:
  .string ".Lbreak%d:\n"
.data
.LC222:
  .string "  jmp .Ldefault%d\n"
.data
.LC221:
  .string "  je .Lcase%d_%d\n"
.data
.LC220:
  .string "  cmpq $%d, %%rax\n"
.data
.LC219:
  .string "  popq %%rax\n"
.data
.LC218:
  .string ".Lend%d:\n"
.data
.LC217:
  .string ".Lelse%d:\n"
.data
.LC216:
  .string "  jmp .Lend%d\n"
.data
.LC215:
  .string "  je .Lelse%d\n"
.data
.LC214:
  .string "  cmpq $0, %%rax\n"
.data
.LC213:
  .string "  popq %%rax\n"
.data
.LC212:
  .string ".Lend%d:\n"
.data
.LC211:
  .string "  je .Lend%d\n"
.data
.LC210:
  .string "  cmpq $0, %%rax\n"
.data
.LC209:
  .string "  popq %%rax\n"
.globl gen_stmt
.text
gen_stmt:
  pushq %rbp
  movq %rsp, %rbp
  subq $40, %rsp
  movq %%rdi, -8(%rbp)
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
.Lbegin338:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend338
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lcontinue48:
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
  jmp .Lbegin338
.Lend338:
.Lbreak52:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC209(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC210(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC213(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC214(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC215(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC216(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC217(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC218(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC219(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  subq $32, %rax
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
  subq $28, %rax
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
.Lbegin339:
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend339
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
  je .Lfalse341
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
  je .Lfalse341
  pushq $1
  jmp .Lend341
.Lfalse341:
  pushq $0
.Lend341:
  popq %rax
  cmpq $0, %rax
  je .Lend340
  leaq .LC220(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC221(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend340:
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
  je .Lfalse343
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
  je .Lfalse343
  pushq $1
  jmp .Lend343
.Lfalse343:
  pushq $0
.Lend343:
  popq %rax
  cmpq $0, %rax
  je .Lend342
  leaq .LC222(%rip), %rax
  pushq %rax
  leaq switch_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend342:
.Lcontinue49:
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
  jmp .Lbegin339
.Lend339:
.Lbreak53:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC223(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  subq $28, %rax
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC224(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_21:
  leaq .LC225(%rip), %rax
  pushq %rax
  leaq switch_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  subq $28, %rax
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
  subq $32, %rax
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
  leaq .LC226(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC227(%rip), %rax
  pushq %rax
  leaq continue_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC228(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC229(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC230(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC231(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC232(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC233(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  subq $28, %rax
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
  subq $28, %rax
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
  subq $32, %rax
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
  je .Lend344
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend344:
  leaq .LC234(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  je .Lend345
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC235(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC236(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC237(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend345:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC238(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  je .Lend346
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend346:
  leaq .LC239(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC240(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC241(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  je .Lelse347
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lend347
.Lelse347:
  leaq .LC242(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend347:
  leaq .LC243(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC244(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC245(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC246(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_22:
  leaq .LC247(%rip), %rax
  pushq %rax
  leaq break_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lcase4_23:
  leaq .LC248(%rip), %rax
  pushq %rax
  leaq continue_count(%rip), %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC249(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lbreak51:
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC281:
  .string "  ret\n"
.data
.LC280:
  .string "  popq %%rbp\n"
.data
.LC279:
  .string "  movq %%rbp, %%rsp\n"
.data
.LC278:
  .string "invalid size"
.data
.LC277:
  .string "  movb %s, -%d(%%rbp)\n"
.data
.LC276:
  .string "  movl %s, -%d(%%rbp)\n"
.data
.LC275:
  .string "  movq %s, -%d(%%rbp)\n"
.data
.LC274:
  .string "  pushq %s\n"
.data
.LC273:
  .string "  subq $%d, %%rsp\n"
.data
.LC272:
  .string "  movq %%rsp, %%rbp\n"
.data
.LC271:
  .string "  pushq %%rbp\n"
.data
.LC270:
  .string "%.*s:\n"
.data
.LC269:
  .string ".text\n"
.data
.LC268:
  .string ".globl %.*s\n"
.data
.LC267:
  .string "%%r9"
.data
.LC266:
  .string "%%r8"
.data
.LC265:
  .string "%%rcx"
.data
.LC264:
  .string "%%rdx"
.data
.LC263:
  .string "%%rsi"
.data
.LC262:
  .string "%%rdi"
.data
.LC261:
  .string "%%r9d"
.data
.LC260:
  .string "%%r8d"
.data
.LC259:
  .string "%%ecx"
.data
.LC258:
  .string "%%edx"
.data
.LC257:
  .string "%%esi"
.data
.LC256:
  .string "%%edi"
.data
.LC255:
  .string "%%r9b"
.data
.LC254:
  .string "%%r8b"
.data
.LC253:
  .string "%%cl"
.data
.LC252:
  .string "%%dl"
.data
.LC251:
  .string "%%sil"
.data
.LC250:
  .string "%%dil"
.globl gen_function
.text
gen_function:
  pushq %rbp
  movq %rsp, %rbp
  subq $176, %rsp
  movq %%rdi, -8(%rbp)
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $0
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
  subq $56, %rax
  pushq %rax
  pushq $8
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
  movq %rbp, %rax
  subq $56, %rax
  pushq %rax
  pushq $16
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
  subq $56, %rax
  pushq %rax
  pushq $24
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
  subq $56, %rax
  pushq %rax
  pushq $32
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
  subq $56, %rax
  pushq %rax
  pushq $40
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
  leaq .LC256(%rip), %rax
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
  leaq .LC257(%rip), %rax
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
  leaq .LC258(%rip), %rax
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
  leaq .LC259(%rip), %rax
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
  leaq .LC260(%rip), %rax
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
  leaq .LC261(%rip), %rax
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
  leaq .LC262(%rip), %rax
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
  leaq .LC263(%rip), %rax
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
  leaq .LC264(%rip), %rax
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
  leaq .LC265(%rip), %rax
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
  leaq .LC266(%rip), %rax
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
  leaq .LC267(%rip), %rax
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_string_literal
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC268(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC269(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC270(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC271(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC272(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC273(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  je .Lend348
  movq %rbp, %rax
  subq $156, %rax
  pushq %rax
  pushq $5
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin349:
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
  je .Lend349
  leaq .LC274(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lcontinue50:
  movq %rbp, %rax
  subq $156, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin349
.Lend349:
.Lbreak54:
.Lend348:
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
.Lbegin350:
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
  je .Lfalse351
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
  movslq (%rax), %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lfalse351
  pushq $1
  jmp .Lend351
.Lfalse351:
  pushq $0
.Lend351:
  popq %rax
  cmpq $0, %rax
  je .Lend350
  movq %rbp, %rax
  subq $164, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
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
  je .Lelse352
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
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
  jmp .Lend352
.Lelse352:
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
.Lend352:
.Lbegin353:
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
  je .Lend353
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
  je .Lelse354
  leaq .LC275(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend354
.Lelse354:
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
  je .Lelse355
  leaq .LC276(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend355
.Lelse355:
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
  je .Lelse356
  leaq .LC277(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  jmp .Lend356
.Lelse356:
  leaq .LC278(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend356:
.Lend355:
.Lend354:
.Lcontinue52:
  movq %rbp, %rax
  subq $160, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin353
.Lend353:
.Lbreak56:
.Lcontinue51:
  movq %rbp, %rax
  subq $156, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin350
.Lend350:
.Lbreak55:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_stmt
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC279(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC280(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC281(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.data
.LC407:
  .string "  pushq %%rax\n"
.data
.LC406:
  .string "unreachable"
.data
.LC405:
  .string "  movzbq %%al, %%rax\n"
.data
.LC404:
  .string "  setle %%al\n"
.data
.LC403:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC402:
  .string "  movzbq %%al, %%rax\n"
.data
.LC401:
  .string "  setl %%al\n"
.data
.LC400:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC399:
  .string "  movzbq %%al, %%rax\n"
.data
.LC398:
  .string "  setne %%al\n"
.data
.LC397:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC396:
  .string "  movzbq %%al, %%rax\n"
.data
.LC395:
  .string "  sete %%al\n"
.data
.LC394:
  .string "  cmpq %%rdi, %%rax\n"
.data
.LC393:
  .string "  idivq %%rdi\n"
.data
.LC392:
  .string "  cqto\n"
.data
.LC391:
  .string "  imulq %%rdi, %%rax\n"
.data
.LC390:
  .string "  subq %%rdi, %%rax\n"
.data
.LC389:
  .string "  addq %%rdi, %%rax\n"
.data
.LC388:
  .string "  popq %%rax\n"
.data
.LC387:
  .string "  popq %%rdi\n"
.data
.LC386:
  .string ".Lend%d:\n"
.data
.LC385:
  .string "  pushq $1\n"
.data
.LC384:
  .string ".Ltrue%d:\n"
.data
.LC383:
  .string "  jmp .Lend%d\n"
.data
.LC382:
  .string "  pushq $0\n"
.data
.LC381:
  .string "  je .Ltrue%d\n"
.data
.LC380:
  .string "  cmpq $1, %%rax\n"
.data
.LC379:
  .string "  popq %%rax\n"
.data
.LC378:
  .string "  je .Ltrue%d\n"
.data
.LC377:
  .string "  cmpq $1, %%rax\n"
.data
.LC376:
  .string "  popq %%rax\n"
.data
.LC375:
  .string ".Lend%d:\n"
.data
.LC374:
  .string "  pushq $0\n"
.data
.LC373:
  .string ".Lfalse%d:\n"
.data
.LC372:
  .string "  jmp .Lend%d\n"
.data
.LC371:
  .string "  pushq $1\n"
.data
.LC370:
  .string "  je .Lfalse%d\n"
.data
.LC369:
  .string "  cmpq $0, %%rax\n"
.data
.LC368:
  .string "  popq %%rax\n"
.data
.LC367:
  .string "  je .Lfalse%d\n"
.data
.LC366:
  .string "  cmpq $0, %%rax\n"
.data
.LC365:
  .string "  popq %%rax\n"
.data
.LC364:
  .string "  pushq %%rax\n"
.data
.LC363:
  .string "unexpected type"
.data
.LC362:
  .string "  movsbq (%%rax), %%rax\n"
.data
.LC361:
  .string "  movq (%%rax), %%rax\n"
.data
.LC360:
  .string "  movslq (%%rax), %%rax\n"
.data
.LC359:
  .string "  popq %%rax\n"
.data
.LC358:
  .string "  pushq $0\n"
.data
.LC357:
  .string "  movq %%rax, -%d(%%rbp)\n"
.data
.LC356:
  .string "  leaq -%d(%%rbp), %%rax\n"
.data
.LC355:
  .string "  movq %%rax, -%d(%%rbp)\n"
.data
.LC354:
  .string "  leaq 16(%%rbp), %%rax\n"
.data
.LC353:
  .string "  movl %%eax, -%d(%%rbp)\n"
.data
.LC352:
  .string "  movl $48, %%eax\n"
.data
.LC351:
  .string "  movl %%eax, -%d(%%rbp)\n"
.data
.LC350:
  .string "  movl $%d, %%eax\n"
.data
.LC349:
  .string "not implemented: return value"
.data
.LC348:
  .string "  pushq %%rax\n"
.data
.LC347:
  .string "  pushq %%rax\n"
.data
.LC346:
  .string "  movslq %%eax, %%rax\n"
.data
.LC345:
  .string "  pushq %%rax\n"
.data
.LC344:
  .string "  movsbq %%al, %%rax\n"
.data
.LC343:
  .string "  pushq $0\n"
.data
.LC342:
  .string "  popq %%rsp\n"
.data
.LC341:
  .string "  addq $8, %%rsp\n"
.data
.LC340:
  .string "  call %.*s\n"
.data
.LC339:
  .string "  movq $0, %%rax\n"
.data
.LC338:
  .string "  subq $8, %%rsp\n"
.data
.LC337:
  .string "  pushq %%rax\n"
.data
.LC336:
  .string "  andq $-16, %%rsp\n"
.data
.LC335:
  .string "  movq %%rsp, %%rax\n"
.data
.LC334:
  .string "  popq %s\n"
.data
.LC333:
  .string "%%r9"
.data
.LC332:
  .string "%%r8"
.data
.LC331:
  .string "%%rcx"
.data
.LC330:
  .string "%%rdx"
.data
.LC329:
  .string "%%rsi"
.data
.LC328:
  .string "%%rdi"
.data
.LC327:
  .string "too many arguments"
.data
.LC326:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC325:
  .string "  addb $-1, %%dil\n"
.data
.LC324:
  .string "  pushq %%rdi\n"
.data
.LC323:
  .string "  movb (%%rax), %%dil\n"
.data
.LC322:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC321:
  .string "  subl $1, %%edi\n"
.data
.LC320:
  .string "  pushq %%rdi\n"
.data
.LC319:
  .string "  movl (%%rax), %%edi\n"
.data
.LC318:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC317:
  .string "  subq $1, %%rdi\n"
.data
.LC316:
  .string "  pushq %%rdi\n"
.data
.LC315:
  .string "  movq (%%rax), %%rdi\n"
.data
.LC314:
  .string "  popq %%rax\n"
.data
.LC313:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC312:
  .string "  addb $1, %%dil\n"
.data
.LC311:
  .string "  pushq %%rdi\n"
.data
.LC310:
  .string "  movb (%%rax), %%dil\n"
.data
.LC309:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC308:
  .string "  addl $1, %%edi\n"
.data
.LC307:
  .string "  pushq %%rdi\n"
.data
.LC306:
  .string "  movl (%%rax), %%edi\n"
.data
.LC305:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC304:
  .string "  addq $1, %%rdi\n"
.data
.LC303:
  .string "  pushq %%rdi\n"
.data
.LC302:
  .string "  movq (%%rax), %%rdi\n"
.data
.LC301:
  .string "  popq %%rax\n"
.data
.LC300:
  .string "  pushq $0\n"
.data
.LC299:
  .string "  popq %%rax\n"
.data
.LC298:
  .string "  pushq %%rdi\n"
.data
.LC297:
  .string "failed to assign"
.data
.LC296:
  .string "  movq %%rdi, (%%rax)\n"
.data
.LC295:
  .string "  movb %%dil, (%%rax)\n"
.data
.LC294:
  .string "  movl %%edi, (%%rax)\n"
.data
.LC293:
  .string "  popq %%rax\n"
.data
.LC292:
  .string "  popq %%rdi\n"
.data
.LC291:
  .string "not implemented: size %d"
.data
.LC290:
  .string "  pushq %%rax\n"
.data
.LC289:
  .string "  movsbq (%%rax), %%rax\n"
.data
.LC288:
  .string "  pushq %%rax\n"
.data
.LC287:
  .string "  movslq (%%rax), %%rax\n"
.data
.LC286:
  .string "  pushq %%r10\n"
.data
.LC285:
  .string "  movq %d(%%rax), %%r10\n"
.data
.LC284:
  .string "  pushq %%rax\n"
.data
.LC283:
  .string "  popq %%rax\n"
.data
.LC282:
  .string "  pushq $%d\n"
.globl gen
.text
gen:
  pushq %rbp
  movq %rsp, %rbp
  subq $88, %rsp
  movq %%rdi, -8(%rbp)
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
  leaq .LC282(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_lval
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC283(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  pushq $2
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $1, %rax
  je .Ltrue358
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
  je .Ltrue358
  pushq $0
  jmp .Lend358
.Ltrue358:
  pushq $1
.Lend358:
  popq %rax
  cmpq $0, %rax
  je .Lend357
  leaq .LC284(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend357:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movl %edi, (%rax)
  pushq %rdi
  popq %rax
.Lbegin359:
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
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lelse360
  leaq .LC285(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $28, %rax
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
  subq %rdi, %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC286(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
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
  jmp .Lend360
.Lelse360:
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
  je .Lelse361
  leaq .LC287(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC288(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
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
  jmp .Lend361
.Lelse361:
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
  je .Lelse362
  leaq .LC289(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC290(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
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
  jmp .Lend362
.Lelse362:
  leaq .LC291(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend362:
.Lend361:
.Lend360:
  movq %rbp, %rax
  subq $32, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  setle %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend363
  jmp .Lbreak58
.Lend363:
.Lcontinue53:
  jmp .Lbegin359
.Lend359:
.Lbreak58:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_lval
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_lval
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC292(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC293(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  leaq .LC294(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak59
.Lcase6_1:
  leaq .LC295(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak59
.Lcase6_8:
  leaq .LC296(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak59
.Ldefault6:
  leaq .LC297(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lbreak59:
  leaq .LC298(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
.Lbegin364:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend364
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC299(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lcontinue54:
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
  jmp .Lbegin364
.Lend364:
.Lbreak60:
  leaq .LC300(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_lval
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC301(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  leaq .LC302(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC303(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC304(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC305(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak61
.Lcase7_4:
  leaq .LC306(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC307(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC308(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC309(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak61
.Lcase7_1:
  leaq .LC310(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC311(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC312(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC313(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak61
.Lbreak61:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_lval
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC314(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  leaq .LC315(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC316(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC317(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC318(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak62
.Lcase8_4:
  leaq .LC319(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC320(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC321(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC322(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak62
.Lcase8_1:
  leaq .LC323(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC324(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC325(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC326(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak62
.Lbreak62:
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
.Lbegin365:
.Lcontinue55:
  movq %rbp, %rax
  subq $24, %rax
  pushq %rax
  popq %rax
  movq 0(%rax), %r10
  pushq %r10
  popq %rax
  cmpq $0, %rax
  je .Lend365
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $16, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  addl $1, %edi
  movl %edi, (%rax)
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
  jmp .Lbegin365
.Lend365:
.Lbreak63:
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
  je .Lend366
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
  leaq .LC327(%rip), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call error_at
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lend366:
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  pushq $0
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC328(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  pushq $8
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC329(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  pushq $16
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC330(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  pushq $24
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC331(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  pushq $32
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC332(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  pushq $40
  popq %rdi
  popq %rax
  addq %rdi, %rax
  pushq %rax
  leaq .LC333(%rip), %rax
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rax
  subq $84, %rax
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
.Lbegin367:
  pushq $0
  movq %rbp, %rax
  subq $84, %rax
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
  je .Lend367
  leaq .LC334(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $80, %rax
  pushq %rax
  movq %rbp, %rax
  subq $84, %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lcontinue56:
  movq %rbp, %rax
  subq $84, %rax
  pushq %rax
  popq %rax
  movl (%rax), %edi
  pushq %rdi
  subl $1, %edi
  movl %edi, (%rax)
  popq %rax
  jmp .Lbegin367
.Lend367:
.Lbreak64:
  leaq .LC335(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC336(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC337(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC338(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC339(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC340(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC341(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC342(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  pushq $5
  popq %rdi
  popq %rax
  cmpq %rdi, %rax
  sete %al
  movzbq %al, %rax
  pushq %rax
  popq %rax
  cmpq $0, %rax
  je .Lend368
  leaq .LC343(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend368:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  leaq .LC344(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC345(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak65
.Lcase9_4:
  leaq .LC346(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC347(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak65
.Lcase9_8:
  leaq .LC348(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak65
.Ldefault9:
  leaq .LC349(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lbreak65:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call assert
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC350(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC351(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC352(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC353(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC354(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC355(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC356(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC357(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC358(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_lval
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC359(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call sizeof_type
  addq $8, %rsp
  popq %rsp
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
  leaq .LC360(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak66
.Lcase10_8:
  leaq .LC361(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak66
.Lcase10_1:
  leaq .LC362(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak66
.Ldefault10:
  leaq .LC363(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
.Lbreak66:
  leaq .LC364(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC365(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC366(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC367(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC368(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC369(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC370(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC371(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC372(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC373(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC374(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC375(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC376(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC377(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC378(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC379(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC380(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC381(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC382(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC383(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC384(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC385(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC386(%rip), %rax
  pushq %rax
  movq %rbp, %rax
  subq $12, %rax
  pushq %rax
  popq %rax
  movslq (%rax), %rax
  pushq %rax
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  leaq .LC387(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC388(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
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
  leaq .LC389(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Lcase11_1:
  leaq .LC390(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Lcase11_2:
  leaq .LC391(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Lcase11_3:
  leaq .LC392(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC393(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Lcase11_6:
  leaq .LC394(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC395(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC396(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Lcase11_7:
  leaq .LC397(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC398(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC399(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Lcase11_8:
  leaq .LC400(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC401(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC402(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Lcase11_9:
  leaq .LC403(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC404(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC405(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  jmp .Lbreak67
.Ldefault11:
  leaq .LC406(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lbreak67:
  leaq .LC407(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lbreak57:
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
.data
.LC415:
  .string "  .zero %d\n"
.data
.LC414:
  .string "%.*s:\n"
.data
.LC413:
  .string ".data\n"
.data
.LC412:
  .string ".globl %.*s\n"
.data
.LC411:
  .string "  .zero 8\n"
.data
.LC410:
  .string "NULL:\n"
.data
.LC409:
  .string ".data\n"
.data
.LC408:
  .string "invalid argument"
.globl main
.text
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movl %%edi, -4(%rbp)
  movq %%rsi, -12(%rbp)
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
  je .Lend369
  leaq .LC408(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call cerror
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  pushq $1
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
.Lend369:
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call get_dir
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call read_file
  addq $8, %rsp
  popq %rsp
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call tokenize
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call preprocess
  addq $8, %rsp
  popq %rsp
  pushq %rax
  popq %rdi
  popq %rax
  movq %rdi, (%rax)
  pushq %rdi
  popq %rax
  leaq .LC409(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC410(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC411(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lbegin370:
.Lcontinue57:
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call at_eof
  addq $8, %rsp
  popq %rsp
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
  je .Lend370
  movq %rbp, %rax
  subq $20, %rax
  pushq %rax
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call external
  addq $8, %rsp
  popq %rsp
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
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call gen_function
  addq $8, %rsp
  popq %rsp
  pushq $0
  popq %rax
  jmp .Lbreak69
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
  je .Lend371
  leaq .LC412(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC413(%rip), %rax
  pushq %rax
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC414(%rip), %rax
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
  popq %%rdx
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
  leaq .LC415(%rip), %rax
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
  popq %%rsi
  popq %%rdi
  movq %rsp, %rax
  andq $-16, %rsp
  pushq %rax
  subq $8, %rsp
  movq $0, %rax
  call printf
  addq $8, %rsp
  popq %rsp
  movslq %eax, %rax
  pushq %rax
  popq %rax
.Lend371:
  jmp .Lbreak69
.Ldefault12:
  jmp .Lbreak69
.Lbreak69:
  jmp .Lbegin370
.Lend370:
.Lbreak68:
  pushq $0
  popq %rax
  movq %rbp, %rsp
  popq %rbp
  ret
  movq %rbp, %rsp
  popq %rbp
  ret
