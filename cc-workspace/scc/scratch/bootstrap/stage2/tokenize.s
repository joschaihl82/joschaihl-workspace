.text
.globl tokenize
.type  tokenize, @function
tokenize:
  push %rbp
  movq %rsp, %rbp
  sub $148, %rsp
  lea -148(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $48, %rdx
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call memset
  mov %rbp, %rsp
  pop %rbp
  push %rax
  mov (%rsp), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rax
  pop %rdi
  push %rax
  push %rdi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  pop %rax
  lea -148(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  lea -100(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea source(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -92(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.52:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.52
.L.for.header.53:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call isspace
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.53
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.next.53:
  jmp .L.for.header.53
.L.for.end.53:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.54
  jmp .L.for.end.52
  jmp .L.end.54
.L.else.54:
.L.end.54:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $40, %rax
  je .L.case.0
  cmp $41, %rax
  je .L.case.1
  cmp $123, %rax
  je .L.case.2
  cmp $125, %rax
  je .L.case.3
  cmp $91, %rax
  je .L.case.4
  cmp $93, %rax
  je .L.case.5
  cmp $59, %rax
  je .L.case.6
  cmp $58, %rax
  je .L.case.7
  cmp $43, %rax
  je .L.case.8
  cmp $45, %rax
  je .L.case.9
  cmp $42, %rax
  je .L.case.10
  cmp $47, %rax
  je .L.case.11
  cmp $37, %rax
  je .L.case.12
  cmp $61, %rax
  je .L.case.13
  cmp $33, %rax
  je .L.case.14
  cmp $60, %rax
  je .L.case.15
  cmp $62, %rax
  je .L.case.16
  cmp $38, %rax
  je .L.case.17
  cmp $124, %rax
  je .L.case.18
  cmp $44, %rax
  je .L.case.19
  cmp $46, %rax
  je .L.case.20
  cmp $63, %rax
  je .L.case.21
  jmp .L.default.22
.L.case.0:
  mov $30, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.1:
  mov $31, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.2:
  mov $32, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.3:
  mov $33, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.4:
  mov $34, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.5:
  mov $35, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.6:
  mov $38, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.7:
  mov $39, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.8:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $43, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.56
  mov $41, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.56
.L.else.56:
  mov $40, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.56:
  jmp .L.switch.end.55
.L.case.9:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $45, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.57
  mov $43, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.57
.L.else.57:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $62, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.58
  mov $60, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.58
.L.else.58:
  mov $42, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.58:
.L.end.57:
  jmp .L.switch.end.55
.L.case.10:
  mov $44, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.11:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $42, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.59
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.header.60:
  mov $1, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.60
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $42, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.61
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $47, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.62
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.for.end.60
  jmp .L.end.62
.L.else.62:
.L.end.62:
  jmp .L.end.61
.L.else.61:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.end.61:
.L.for.next.60:
  jmp .L.for.header.60
.L.for.end.60:
  jmp .L.for.next.52
  jmp .L.end.59
.L.else.59:
.L.end.59:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $47, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.63
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.header.64:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $10, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.64
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.next.64:
  jmp .L.for.header.64
.L.for.end.64:
  jmp .L.for.next.52
  jmp .L.end.63
.L.else.63:
.L.end.63:
  mov $45, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.12:
  mov $46, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.13:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $61, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.65
  mov $48, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.65
.L.else.65:
  mov $47, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.65:
  jmp .L.switch.end.55
.L.case.14:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $61, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.66
  mov $49, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.66
.L.else.66:
  mov $62, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.66:
  jmp .L.switch.end.55
.L.case.15:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $61, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.67
  mov $51, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.67
.L.else.67:
  mov $50, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.67:
  jmp .L.switch.end.55
.L.case.16:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $61, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.68
  mov $53, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.68
.L.else.68:
  mov $52, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.68:
  jmp .L.switch.end.55
.L.case.17:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $38, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.69
  mov $55, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.69
.L.else.69:
  mov $54, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.69:
  jmp .L.switch.end.55
.L.case.18:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $124, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.70
  mov $57, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.70
.L.else.70:
  mov $56, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.70:
  jmp .L.switch.end.55
.L.case.19:
  mov $58, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.case.20:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $46, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.71
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $46, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.72
  mov $61, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $2, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $3, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.end.72
.L.else.72:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  sub $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  mov $59, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.72:
  jmp .L.end.71
.L.else.71:
  mov $59, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.71:
  jmp .L.switch.end.55
.L.case.21:
  mov $63, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.switch.end.55
.L.default.22:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call isalpha
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.74
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $95, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.74:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.73
  mov $0, %rax
  push %rax
  lea -84(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.75:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call isalnum
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.76
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $95, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.76:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.75
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.next.75:
  jmp .L.for.header.75
.L.for.end.75:
  mov $0, %rax
  push %rax
  lea -76(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -72(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.77:
  lea -72(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $648, %rax
  push %rax
  mov $24, %rax
  push %rax
  pop %rdi
  pop %rax
  xor %rdx, %rdx
  idiv %rdi
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setl %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.77
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea keywords(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $24, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.79
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  lea keywords(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $24, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strncmp
  mov %rbp, %rsp
  pop %rbp
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.79:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.78
  lea keywords(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $24, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `was_keyword`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.78
.L.else.78:
.L.end.78:
.L.for.next.77:
  lea -72(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  jmp .L.for.header.77
.L.for.end.77:
  mov $0, %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `was_keyword`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.80
  mov $27, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.80
.L.else.80:
.L.end.80:
  jmp .L.end.73
.L.else.73:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call isdigit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.81
  mov $0, %rax
  push %rax
  lea -64(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -56(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
.L.for.header.82:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call isdigit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.82
  lea -56(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $10, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rax
  cltq
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  mov $48, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -64(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.next.82:
  jmp .L.for.header.82
.L.for.end.82:
  mov $37, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -56(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.81
.L.else.81:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $39, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.83
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -52(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -44(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -36(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
.L.for.header.84:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $39, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.84
  lea -44(%rbp), %rax # symbol ref lvalue `seen_char_already`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.85
  mov $0, %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `warned`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.86
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.27(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call warn_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `warned`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.86
.L.else.86:
.L.end.86:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $256, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.85
.L.else.85:
.L.end.85:
  mov $1, %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `seen_char_already`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $92, %rax
  je .L.case.23
  jmp .L.default.37
.L.case.23:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $39, %rax
  je .L.case.24
  cmp $34, %rax
  je .L.case.25
  cmp $92, %rax
  je .L.case.26
  cmp $63, %rax
  je .L.case.27
  cmp $97, %rax
  je .L.case.28
  cmp $98, %rax
  je .L.case.29
  cmp $102, %rax
  je .L.case.30
  cmp $114, %rax
  je .L.case.31
  cmp $116, %rax
  je .L.case.32
  cmp $118, %rax
  je .L.case.33
  cmp $110, %rax
  je .L.case.34
  cmp $48, %rax
  je .L.case.35
  jmp .L.default.36
.L.case.24:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $39, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.25:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $34, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.26:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $92, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.27:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $63, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.28:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $7, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.29:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $8, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.30:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $12, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.31:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $13, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.32:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $9, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.33:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $11, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.34:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $10, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.case.35:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.default.36:
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.28(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call error_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.88
.L.switch.end.88:
  jmp .L.switch.end.87
.L.default.37:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rax
  cltq
  push %rax
  pop %rdi
  pop %rax
  or %rdi, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.switch.end.87
.L.switch.end.87:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.next.84:
  jmp .L.for.header.84
.L.for.end.84:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  mov $28, %rax
  push %rax
  lea -52(%rbp), %rax # symbol ref lvalue `pos`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -52(%rbp), %rax # symbol ref lvalue `pos`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.83
.L.else.83:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $34, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.89
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `contents`
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call open_memstream
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.90:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $34, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.90
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $92, %rax
  je .L.case.38
  jmp .L.default.51
.L.case.38:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $39, %rax
  je .L.case.39
  cmp $34, %rax
  je .L.case.40
  cmp $63, %rax
  je .L.case.41
  cmp $92, %rax
  je .L.case.42
  cmp $97, %rax
  je .L.case.43
  cmp $98, %rax
  je .L.case.44
  cmp $102, %rax
  je .L.case.45
  cmp $110, %rax
  je .L.case.46
  cmp $114, %rax
  je .L.case.47
  cmp $116, %rax
  je .L.case.48
  cmp $118, %rax
  je .L.case.49
  jmp .L.default.50
.L.case.39:
  mov $39, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.40:
  mov $34, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.41:
  mov $63, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.42:
  mov $92, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.43:
  mov $7, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.44:
  mov $8, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.45:
  mov $12, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.46:
  mov $10, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.47:
  mov $13, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.48:
  mov $9, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.case.49:
  mov $11, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.92
.L.default.50:
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.29(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call error_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.switch.end.92:
  jmp .L.switch.end.91
.L.default.51:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.91
.L.switch.end.91:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.next.90:
  jmp .L.for.header.90
.L.for.end.90:
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rdx
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fflush
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fclose
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $29, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `pos`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `pos`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `contents`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.89
.L.else.89:
  mov $0, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea .L.str.30(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call error_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.89:
.L.end.83:
.L.end.81:
.L.end.73:
  jmp .L.switch.end.55
.L.switch.end.55:
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.52:
  jmp .L.for.header.52
.L.for.end.52:
  mov $64, %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_token
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -148(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.30:
  .byte 0x75
  .byte 0x6e
  .byte 0x72
  .byte 0x65
  .byte 0x63
  .byte 0x6f
  .byte 0x67
  .byte 0x6e
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x63
  .byte 0x68
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.29:
  .byte 0x75
  .byte 0x6e
  .byte 0x72
  .byte 0x65
  .byte 0x63
  .byte 0x6f
  .byte 0x67
  .byte 0x6e
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x65
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x65
  .byte 0x6e
  .byte 0x63
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.28:
  .byte 0x75
  .byte 0x6e
  .byte 0x72
  .byte 0x65
  .byte 0x63
  .byte 0x6f
  .byte 0x67
  .byte 0x6e
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x65
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x65
  .byte 0x6e
  .byte 0x63
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.27:
  .byte 0x6d
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x69
  .byte 0x2d
  .byte 0x63
  .byte 0x68
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x63
  .byte 0x68
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x6e
  .byte 0x74
  .byte 0x00
.text
.globl make_token
.type  make_token, @function
make_token:
  push %rbp
  movq %rsp, %rbp
  sub $28, %rsp
  push %rdi
  lea -28(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `pos`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $48, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call malloc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $48, %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call memset
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -28(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `pos`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl load_file
.type  load_file, @function
load_file:
  push %rbp
  movq %rsp, %rbp
  sub $4128, %rsp
  push %rdi
  lea -4128(%rbp), %rax # symbol ref lvalue `filename`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -4120(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -4128(%rbp), %rax # symbol ref lvalue `filename`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strdup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea source_name(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea source(%rip), %rax
  push %rax
  lea source_len(%rip), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call open_memstream
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -4112(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.93:
  lea -4104(%rbp), %rax # symbol ref lvalue `read_buf`
  push %rax
  pop %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $4096, %rax
  push %rax
  lea -4120(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fread
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `nbytes`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.94
  jmp .L.for.end.93
  jmp .L.end.94
.L.else.94:
.L.end.94:
  lea -4104(%rbp), %rax # symbol ref lvalue `read_buf`
  push %rax
  pop %rax
  push %rax
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `nbytes`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4112(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fwrite
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.93:
  jmp .L.for.header.93
.L.for.end.93:
  lea -4120(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fclose
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -4112(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fflush
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea source_len(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.96
  lea source(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea source_len(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  mov $10, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.96:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.95
  mov $10, %rax
  push %rax
  lea -4112(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.95
.L.else.95:
.L.end.95:
  mov $0, %rax
  push %rax
  lea -4112(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fputc
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -4112(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fclose
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.data
.globl keywords
keywords:
  .quad .L.str.0
  .long 1
  .zero 4
  .quad 3
  .quad .L.str.1
  .long 36
  .zero 4
  .quad 6
  .quad .L.str.2
  .long 5
  .zero 4
  .quad 2
  .quad .L.str.3
  .long 6
  .zero 4
  .quad 4
  .quad .L.str.4
  .long 7
  .zero 4
  .quad 3
  .quad .L.str.5
  .long 8
  .zero 4
  .quad 5
  .quad .L.str.6
  .long 9
  .zero 4
  .quad 6
  .quad .L.str.7
  .long 2
  .zero 4
  .quad 4
  .quad .L.str.8
  .long 4
  .zero 4
  .quad 2
  .quad .L.str.9
  .long 10
  .zero 4
  .quad 5
  .quad .L.str.10
  .long 13
  .zero 4
  .quad 4
  .quad .L.str.11
  .long 11
  .zero 4
  .quad 5
  .quad .L.str.12
  .long 14
  .zero 4
  .quad 6
  .quad .L.str.13
  .long 12
  .zero 4
  .quad 8
  .quad .L.str.14
  .long 15
  .zero 4
  .quad 7
  .quad .L.str.15
  .long 16
  .zero 4
  .quad 6
  .quad .L.str.16
  .long 17
  .zero 4
  .quad 5
  .quad .L.str.17
  .long 18
  .zero 4
  .quad 6
  .quad .L.str.18
  .long 19
  .zero 4
  .quad 4
  .quad .L.str.19
  .long 20
  .zero 4
  .quad 7
  .quad .L.str.20
  .long 21
  .zero 4
  .quad 4
  .quad .L.str.21
  .long 22
  .zero 4
  .quad 8
  .quad .L.str.22
  .long 23
  .zero 4
  .quad 4
  .quad .L.str.23
  .long 24
  .zero 4
  .quad 5
  .quad .L.str.24
  .long 25
  .zero 4
  .quad 6
  .quad .L.str.25
  .long 26
  .zero 4
  .quad 8
  .quad .L.str.26
  .long 26
  .zero 4
  .quad 11
.section .rodata
.L.str.26:
  .byte 0x5f
  .byte 0x5f
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x67
  .byte 0x6e
  .byte 0x6f
  .byte 0x66
  .byte 0x5f
  .byte 0x5f
  .byte 0x00
.section .rodata
.L.str.25:
  .byte 0x5f
  .byte 0x41
  .byte 0x6c
  .byte 0x69
  .byte 0x67
  .byte 0x6e
  .byte 0x6f
  .byte 0x66
  .byte 0x00
.section .rodata
.L.str.24:
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x63
  .byte 0x00
.section .rodata
.L.str.23:
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x73
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.22:
  .byte 0x6c
  .byte 0x6f
  .byte 0x6e
  .byte 0x67
  .byte 0x00
.section .rodata
.L.str.21:
  .byte 0x75
  .byte 0x6e
  .byte 0x73
  .byte 0x69
  .byte 0x67
  .byte 0x6e
  .byte 0x65
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.20:
  .byte 0x65
  .byte 0x6e
  .byte 0x75
  .byte 0x6d
  .byte 0x00
.section .rodata
.L.str.19:
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x61
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.18:
  .byte 0x63
  .byte 0x61
  .byte 0x73
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.17:
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x00
.section .rodata
.L.str.16:
  .byte 0x75
  .byte 0x6e
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.15:
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.14:
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x00
.section .rodata
.L.str.13:
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x74
  .byte 0x69
  .byte 0x6e
  .byte 0x75
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.12:
  .byte 0x65
  .byte 0x78
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.11:
  .byte 0x62
  .byte 0x72
  .byte 0x65
  .byte 0x61
  .byte 0x6b
  .byte 0x00
.section .rodata
.L.str.10:
  .byte 0x76
  .byte 0x6f
  .byte 0x69
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.9:
  .byte 0x5f
  .byte 0x42
  .byte 0x6f
  .byte 0x6f
  .byte 0x6c
  .byte 0x00
.section .rodata
.L.str.8:
  .byte 0x64
  .byte 0x6f
  .byte 0x00
.section .rodata
.L.str.7:
  .byte 0x63
  .byte 0x68
  .byte 0x61
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.6:
  .byte 0x73
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x6f
  .byte 0x66
  .byte 0x00
.section .rodata
.L.str.5:
  .byte 0x77
  .byte 0x68
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.4:
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.3:
  .byte 0x65
  .byte 0x6c
  .byte 0x73
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.2:
  .byte 0x69
  .byte 0x66
  .byte 0x00
.section .rodata
.L.str.1:
  .byte 0x72
  .byte 0x65
  .byte 0x74
  .byte 0x75
  .byte 0x72
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.0:
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x00
.bss
.globl source_len
source_len:
  .zero 8
.bss
.globl source
source:
  .zero 8
.bss
.globl source_name
source_name:
  .zero 8
