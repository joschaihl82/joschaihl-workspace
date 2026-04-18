.text
.globl parse
.type  parse, @function
parse:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $32, %rax
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
  lea global_scope(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea global_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $32, %rax
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
  mov $1, %rax
  push %rax
  lea global_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea global_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_scope(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.header.1:
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $64, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.1
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call external_declaration
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.1:
  jmp .L.for.header.1
.L.for.end.1:
  lea symbols(%rip), %rax
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
.globl external_declaration
.type  external_declaration, @function
external_declaration:
  push %rbp
  movq %rsp, %rbp
  sub $260, %rsp
  push %rdi
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -252(%rbp), %rax # symbol ref lvalue `in_compound_statement`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -248(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -240(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declaration_specifiers
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -236(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -228(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -220(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $64, %rdx
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
  lea -220(%rbp), %rax # symbol ref lvalue `initializer_head`
  push %rax
  lea -156(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_nop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -156(%rbp), %rax # symbol ref lvalue `initializers`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -156(%rbp), %rax # symbol ref lvalue `initializers`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.2
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declarator
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.2
.L.else.2:
.L.end.2:
  mov $0, %rax
  push %rax
  lea -148(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.3
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $5, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.4
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
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
  call make_symbol_function
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.4
.L.else.4:
  lea -252(%rbp), %rax # symbol ref lvalue `in_compound_statement`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.5
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.6
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_symbol_global
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.6
.L.else.6:
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $3, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.7
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -140(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -140(%rbp), %rax # symbol ref lvalue `alias`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -132(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -132(%rbp), %rax # symbol ref lvalue `name`
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
  call make_symbol_global
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -140(%rbp), %rax # symbol ref lvalue `alias`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.7
.L.else.7:
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $2, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.8
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
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
  call make_symbol_local
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.8
.L.else.8:
.L.end.8:
.L.end.7:
.L.end.6:
  jmp .L.end.5
.L.else.5:
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_symbol_global
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.5:
.L.end.4:
  jmp .L.end.3
.L.else.3:
.L.end.3:
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.9
  mov $1, %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $56, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.9
.L.else.9:
.L.end.9:
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $47, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.10
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $2, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.11
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.72(%rip), %rax
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
  jmp .L.end.11
.L.else.11:
.L.end.11:
  lea -252(%rbp), %rax # symbol ref lvalue `in_compound_statement`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.12
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $56, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.13
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.73(%rip), %rax
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
  jmp .L.end.13
.L.else.13:
.L.end.13:
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
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
  call parse_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -124(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.14
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -124(%rbp), %rax # symbol ref lvalue `init`
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
  call make_initialize
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -156(%rbp), %rax # symbol ref lvalue `initializers`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -156(%rbp), %rax # symbol ref lvalue `initializers`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.14
.L.else.14:
.L.end.14:
  jmp .L.end.12
.L.else.12:
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
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
  call parse_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -116(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -116(%rbp), %rax # symbol ref lvalue `init`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $72, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.12:
  jmp .L.end.10
.L.else.10:
.L.end.10:
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $2, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.15
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
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
  call make_typedef
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_type_symbol
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -108(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -108(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.15
.L.else.15:
.L.end.15:
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $32, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.16
  lea -252(%rbp), %rax # symbol ref lvalue `in_compound_statement`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.17
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.74(%rip), %rax
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
  jmp .L.end.17
.L.else.17:
.L.end.17:
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $2, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.18
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.75(%rip), %rax
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
  jmp .L.end.18
.L.else.18:
.L.end.18:
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_function(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $2, %rax
  push %rax
  lea current_function(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $56, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -100(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -96(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $64, %rdx
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
  lea -96(%rbp), %rax # symbol ref lvalue `parameter_inits`
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_scope
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea current_function(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.19:
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.19
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
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
  call make_symbol_local
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `arg`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `arg`
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
  call make_symbol_ref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -100(%rbp), %rax # symbol ref lvalue `arg_count`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_arg
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_assign
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_expr_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `parameter_cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `parameter_cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.19:
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.19
.L.for.end.19:
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call compound_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_scope
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `parameter_cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `parameter_inits`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_function(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $72, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea symbols(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_function(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea current_function(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea symbols(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -220(%rbp), %rax # symbol ref lvalue `initializer_head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.16
.L.else.16:
  lea -260(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.20
  mov $0, %rax
  push %rax
  lea -228(%rbp), %rax # symbol ref lvalue `decl`
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
  je .L.else.21
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $7, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.23
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $8, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.24
  lea -236(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $9, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.24:
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.23:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.22
  lea -248(%rbp), %rax # symbol ref lvalue `start_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.76(%rip), %rax
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
  jmp .L.end.22
.L.else.22:
.L.end.22:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.21
.L.else.21:
.L.end.21:
  lea -240(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $2, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.25
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.25
.L.else.25:
.L.end.25:
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.26
  lea symbols(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -148(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea symbols(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.26
.L.else.26:
.L.end.26:
  lea -220(%rbp), %rax # symbol ref lvalue `initializer_head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.20
.L.else.20:
.L.end.20:
.L.end.16:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.76:
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x6f
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x79
  .byte 0x74
  .byte 0x68
  .byte 0x69
  .byte 0x6e
  .byte 0x67
  .byte 0x00
.section .rodata
.L.str.75:
  .byte 0x66
  .byte 0x75
  .byte 0x6e
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x60
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x60
  .byte 0x00
.section .rodata
.L.str.74:
  .byte 0x66
  .byte 0x75
  .byte 0x6e
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x6c
  .byte 0x6c
  .byte 0x6f
  .byte 0x77
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x68
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.73:
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x62
  .byte 0x6c
  .byte 0x6f
  .byte 0x63
  .byte 0x6b
  .byte 0x20
  .byte 0x73
  .byte 0x63
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x68
  .byte 0x20
  .byte 0x6c
  .byte 0x69
  .byte 0x6e
  .byte 0x6b
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x63
  .byte 0x61
  .byte 0x6e
  .byte 0x27
  .byte 0x74
  .byte 0x20
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.72:
  .byte 0x69
  .byte 0x6c
  .byte 0x6c
  .byte 0x65
  .byte 0x67
  .byte 0x61
  .byte 0x6c
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x00
.text
.globl declaration_specifiers
.type  declaration_specifiers, @function
declaration_specifiers:
  push %rbp
  movq %rsp, %rbp
  sub $40, %rsp
  push %rdi
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.27:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.28
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.29
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.57(%rip), %rax
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
  jmp .L.end.29
.L.else.29:
.L.end.29:
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.28
.L.else.28:
.L.end.28:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.30
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.31
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.58(%rip), %rax
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
  jmp .L.end.31
.L.else.31:
.L.end.31:
  lea ty_char(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.30
.L.else.30:
.L.end.30:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $10, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.32
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.33
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.59(%rip), %rax
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
  jmp .L.end.33
.L.else.33:
.L.end.33:
  lea ty_bool(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.32
.L.else.32:
.L.end.32:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $23, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.34
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.35
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.60(%rip), %rax
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
  jmp .L.end.35
.L.else.35:
.L.end.35:
  lea ty_long(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.34
.L.else.34:
.L.end.34:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $13, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.36
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.37
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.61(%rip), %rax
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
  jmp .L.end.37
.L.else.37:
.L.end.37:
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.36
.L.else.36:
.L.end.36:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $22, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.38
  jmp .L.for.next.27
  jmp .L.end.38
.L.else.38:
.L.end.38:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $24, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.39
  jmp .L.for.next.27
  jmp .L.end.39
.L.else.39:
.L.end.39:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $25, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.40
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
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
  je .L.else.41
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.62(%rip), %rax
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
  jmp .L.end.41
.L.else.41:
.L.end.41:
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.42
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.63(%rip), %rax
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
  jmp .L.end.42
.L.else.42:
.L.end.42:
  mov $3, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.40
.L.else.40:
.L.end.40:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $14, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.43
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
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
  je .L.else.44
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.64(%rip), %rax
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
  jmp .L.end.44
.L.else.44:
.L.end.44:
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.45
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.65(%rip), %rax
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
  jmp .L.end.45
.L.else.45:
.L.end.45:
  mov $1, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.43
.L.else.43:
.L.end.43:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $15, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.46
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
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
  je .L.else.47
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.66(%rip), %rax
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
  jmp .L.end.47
.L.else.47:
.L.end.47:
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.48
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.67(%rip), %rax
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
  jmp .L.end.48
.L.else.48:
.L.end.48:
  mov $2, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `storage`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.27
  jmp .L.end.46
.L.else.46:
.L.end.46:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $16, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.50
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $17, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.50:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.49
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.51
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.68(%rip), %rax
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
  jmp .L.end.51
.L.else.51:
.L.end.51:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call struct_or_union_specifier
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.49
.L.else.49:
.L.end.49:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $21, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.52
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.53
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.69(%rip), %rax
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
  jmp .L.end.53
.L.else.53:
.L.end.53:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call enum_specifier
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.52
.L.else.52:
.L.end.52:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.54
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
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
  je .L.else.55
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call scope_lookup_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `type_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.56
  lea -8(%rbp), %rax # symbol ref lvalue `type_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.56
.L.else.56:
.L.end.56:
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.70(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.55
.L.else.55:
.L.end.55:
  jmp .L.end.54
.L.else.54:
.L.end.54:
  jmp .L.for.end.27
.L.for.next.27:
  jmp .L.for.header.27
.L.for.end.27:
.L.do.body.57:
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
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
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.71(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.58
.L.else.58:
.L.end.58:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.57
.L.do.end.57:
  lea -24(%rbp), %rax # symbol ref lvalue `type_spec`
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
.L.str.71:
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x20
  .byte 0x77
  .byte 0x61
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x75
  .byte 0x6c
  .byte 0x6c
  .byte 0x20
  .byte 0x61
  .byte 0x66
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x70
  .byte 0x61
  .byte 0x72
  .byte 0x73
  .byte 0x69
  .byte 0x6e
  .byte 0x67
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x00
.section .rodata
.L.str.70:
  .byte 0x75
  .byte 0x6e
  .byte 0x6b
  .byte 0x6e
  .byte 0x6f
  .byte 0x77
  .byte 0x6e
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x6e
  .byte 0x61
  .byte 0x6d
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x00
.section .rodata
.L.str.69:
  .byte 0x74
  .byte 0x77
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.68:
  .byte 0x74
  .byte 0x77
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.67:
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x61
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x6d
  .byte 0x69
  .byte 0x74
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.66:
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x6d
  .byte 0x69
  .byte 0x74
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x68
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.65:
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x61
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x6d
  .byte 0x69
  .byte 0x74
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.64:
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x6d
  .byte 0x69
  .byte 0x74
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x68
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.63:
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x61
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x6d
  .byte 0x69
  .byte 0x74
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.62:
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x6d
  .byte 0x69
  .byte 0x74
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x68
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.61:
  .byte 0x74
  .byte 0x77
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.60:
  .byte 0x74
  .byte 0x77
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.59:
  .byte 0x74
  .byte 0x77
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.58:
  .byte 0x74
  .byte 0x77
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.57:
  .byte 0x74
  .byte 0x77
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x00
.text
.globl enum_specifier
.type  enum_specifier, @function
enum_specifier:
  push %rbp
  movq %rsp, %rbp
  sub $96, %rsp
  push %rdi
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $21, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -88(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -80(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.59
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.59
.L.else.59:
.L.end.59:
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $32, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.60
  mov $0, %rax
  push %rax
  lea -72(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
.L.for.header.61:
  mov $0, %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.61
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -68(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -60(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $47, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.62
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $37, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -60(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.62
.L.else.62:
.L.end.62:
  mov $0, %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
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
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $58, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.63
.L.else.63:
.L.end.63:
  lea -60(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.64
  lea -60(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `iota`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.64
.L.else.64:
.L.end.64:
  lea -72(%rbp), %rax # symbol ref lvalue `iota`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  lea -52(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -68(%rbp), %rax # symbol ref lvalue `enumerator`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `enumerator`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -52(%rbp), %rax # symbol ref lvalue `enumerator_value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_symbol_constant
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -48(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.61:
  jmp .L.for.header.61
.L.for.end.61:
  lea -72(%rbp), %rax # symbol ref lvalue `iota`
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.65
  lea -88(%rbp), %rax # symbol ref lvalue `enum_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.55(%rip), %rax
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
  jmp .L.end.65
.L.else.65:
.L.end.65:
  lea -80(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_enum
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -80(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.66
  lea -80(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
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
  call make_type_symbol
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $1, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `sym`
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
  lea -32(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.66
.L.else.66:
.L.end.66:
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.60
.L.else.60:
  mov $0, %rax
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `name`
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
  je .L.else.67
  lea -88(%rbp), %rax # symbol ref lvalue `enum_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.56(%rip), %rax
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
  jmp .L.end.67
.L.else.67:
  lea -80(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $3, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call scope_lookup_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.68
  lea -24(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.68
.L.else.68:
.L.end.68:
  lea -80(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_enum
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -80(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
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
  call make_type_symbol
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `new_sym`
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
  lea -8(%rbp), %rax # symbol ref lvalue `new_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.end.67:
.L.end.60:
  leave
  ret
.section .rodata
.L.str.56:
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x79
  .byte 0x6d
  .byte 0x6f
  .byte 0x75
  .byte 0x73
  .byte 0x20
  .byte 0x65
  .byte 0x6e
  .byte 0x75
  .byte 0x6d
  .byte 0x20
  .byte 0x6d
  .byte 0x75
  .byte 0x73
  .byte 0x74
  .byte 0x20
  .byte 0x62
  .byte 0x65
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.55:
  .byte 0x65
  .byte 0x6d
  .byte 0x70
  .byte 0x74
  .byte 0x79
  .byte 0x20
  .byte 0x65
  .byte 0x6e
  .byte 0x75
  .byte 0x6d
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x64
  .byte 0x00
.text
.globl struct_or_union_specifier
.type  struct_or_union_specifier, @function
struct_or_union_specifier:
  push %rbp
  movq %rsp, %rbp
  sub $164, %rsp
  push %rdi
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $16, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.69
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $16, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -156(%rbp), %rax # symbol ref lvalue `struct_or_union_tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.69
.L.else.69:
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $17, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -156(%rbp), %rax # symbol ref lvalue `struct_or_union_tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.69:
  lea -156(%rbp), %rax # symbol ref lvalue `struct_or_union_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $17, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  lea -148(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -144(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.70
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.70
.L.else.70:
.L.end.70:
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $32, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.71
  lea -136(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $32, %rdx
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
  lea -136(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  lea -104(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -96(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -92(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $1, %rax
  push %rax
  lea -88(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
.L.for.header.72:
  mov $0, %rax
  push %rax
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.72
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declaration_specifiers
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -84(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declarator
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -76(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -164(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
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
  je .L.else.73
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.51(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.73
.L.else.73:
.L.end.73:
  lea -136(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -68(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.74:
  lea -68(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.74
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  je .L.and.76
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
.L.and.76:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.75
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.52(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.75
.L.else.75:
.L.end.75:
.L.for.next.74:
  lea -68(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.74
.L.for.end.74:
  lea -96(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $0, %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  and %rdi, %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -60(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -148(%rbp), %rax # symbol ref lvalue `is_union`
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
  je .L.else.77
  lea -96(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.77
.L.else.77:
.L.end.77:
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `max_member_size`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.78
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `max_member_size`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.78
.L.else.78:
.L.end.78:
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.79
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.79
.L.else.79:
.L.end.79:
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -60(%rbp), %rax # symbol ref lvalue `field_offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_field
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -104(%rbp), %rax # symbol ref lvalue `fields`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -104(%rbp), %rax # symbol ref lvalue `fields`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.72:
  jmp .L.for.header.72
.L.for.end.72:
  lea -148(%rbp), %rax # symbol ref lvalue `is_union`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.80
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -136(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -92(%rbp), %rax # symbol ref lvalue `max_member_size`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $0, %rax
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  and %rdi, %rax
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_union
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
  call make_type_symbol
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $28, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.80
.L.else.80:
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -136(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $0, %rax
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  and %rdi, %rax
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `max_align`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_struct
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
  call make_type_symbol
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.80:
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.81
  lea -148(%rbp), %rax # symbol ref lvalue `is_union`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.cond.false.82
  mov $2, %rax
  push %rax
  jmp .L.cond.end.83
.L.cond.false.82:
  mov $1, %rax
  push %rax
.L.cond.end.83:
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `ty_scope`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call scope_lookup_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -36(%rbp), %rax # symbol ref lvalue `existing_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.84
  lea -36(%rbp), %rax # symbol ref lvalue `existing_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.85
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.53(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.85
.L.else.85:
.L.end.85:
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `existing_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `existing_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.84
.L.else.84:
  lea -48(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.84:
  jmp .L.end.81
.L.else.81:
.L.end.81:
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.71
.L.else.71:
  mov $0, %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `name`
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
  je .L.else.86
  lea -156(%rbp), %rax # symbol ref lvalue `struct_or_union_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.54(%rip), %rax
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
  jmp .L.end.86
.L.else.86:
  lea -148(%rbp), %rax # symbol ref lvalue `is_union`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.cond.false.87
  mov $2, %rax
  push %rax
  jmp .L.cond.end.88
.L.cond.false.87:
  mov $1, %rax
  push %rax
.L.cond.end.88:
  lea -28(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `lookup_scope`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call scope_lookup_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.89
  lea -24(%rbp), %rax # symbol ref lvalue `ty_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.89
.L.else.89:
.L.end.89:
  lea -148(%rbp), %rax # symbol ref lvalue `is_union`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.90
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_union
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
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
  call make_type_symbol
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `new_sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `new_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $28, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.90
.L.else.90:
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_struct
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -144(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
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
  call make_type_symbol
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `new_sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `new_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.90:
  lea -8(%rbp), %rax # symbol ref lvalue `new_sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call define_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.end.86:
.L.end.71:
  leave
  ret
.section .rodata
.L.str.54:
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x79
  .byte 0x6d
  .byte 0x6f
  .byte 0x75
  .byte 0x73
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x75
  .byte 0x6e
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6d
  .byte 0x75
  .byte 0x73
  .byte 0x74
  .byte 0x20
  .byte 0x62
  .byte 0x65
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.53:
  .byte 0x72
  .byte 0x65
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x60
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x00
.section .rodata
.L.str.52:
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x75
  .byte 0x6e
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x64
  .byte 0x75
  .byte 0x70
  .byte 0x6c
  .byte 0x69
  .byte 0x63
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x6c
  .byte 0x64
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x00
.section .rodata
.L.str.51:
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x6c
  .byte 0x64
  .byte 0x20
  .byte 0x68
  .byte 0x61
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x63
  .byte 0x6f
  .byte 0x6d
  .byte 0x70
  .byte 0x6c
  .byte 0x65
  .byte 0x74
  .byte 0x65
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x00
.text
.globl declarator
.type  declarator, @function
declarator:
  push %rbp
  movq %rsp, %rbp
  sub $44, %rsp
  push %rdi
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.header.91:
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $44, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.91
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_pointer_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.91:
  jmp .L.for.header.91
.L.for.end.91:
  mov $0, %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.92
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.92
.L.else.92:
.L.end.92:
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -28(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.93:
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.94
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $5, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.95
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.50(%rip), %rax
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
  jmp .L.end.95
.L.else.95:
.L.end.95:
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  call parameter_list
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -20(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `params`
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
  call make_function_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.93
  jmp .L.end.94
.L.else.94:
.L.end.94:
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $34, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.96
  mov $0, %rax
  push %rax
  lea -12(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $35, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.97
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $37, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `len_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `array_len`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.97
.L.else.97:
.L.end.97:
  lea -44(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $35, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `array_len`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_array_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.93
  jmp .L.end.96
.L.else.96:
.L.end.96:
  jmp .L.for.end.93
.L.for.next.93:
  jmp .L.for.header.93
.L.for.end.93:
  lea -28(%rbp), %rax # symbol ref lvalue `ident`
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
.L.str.50:
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x66
  .byte 0x75
  .byte 0x6e
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x61
  .byte 0x74
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x74
  .byte 0x75
  .byte 0x72
  .byte 0x6e
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x66
  .byte 0x75
  .byte 0x6e
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x00
.text
.globl parameter_list
.type  parameter_list, @function
parameter_list:
  push %rbp
  movq %rsp, %rbp
  sub $80, %rsp
  push %rdi
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -72(%rbp), %rax # symbol ref lvalue `func_ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -64(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $24, %rdx
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
  lea -64(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %rax
  push %rax
  lea -28(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
.L.for.header.98:
  mov $0, %rax
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.98
  lea -28(%rbp), %rax # symbol ref lvalue `seen_void`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.99
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.47(%rip), %rax
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
  jmp .L.end.99
.L.else.99:
.L.end.99:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $61, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.100
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $61, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `seen_one_parameter`
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
  je .L.else.101
  lea -24(%rbp), %rax # symbol ref lvalue `ellipsis`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.48(%rip), %rax
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
  jmp .L.end.101
.L.else.101:
.L.end.101:
  mov $1, %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `func_ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -64(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.100
.L.else.100:
.L.end.100:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declaration_specifiers
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declarator
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `declspec`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_void(%rip), %rax
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
  je .L.else.102
  lea -8(%rbp), %rax # symbol ref lvalue `param_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.103
  lea -8(%rbp), %rax # symbol ref lvalue `param_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.49(%rip), %rax
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
  jmp .L.end.103
.L.else.103:
.L.end.103:
  mov $1, %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `seen_void`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.102
.L.else.102:
  lea -8(%rbp), %rax # symbol ref lvalue `param_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `declspec`
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
  call make_parameter
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `params`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `params`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.102:
  mov $0, %rax
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.104
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $58, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.104
.L.else.104:
.L.end.104:
  mov $1, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `seen_one_parameter`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.98:
  jmp .L.for.header.98
.L.for.end.98:
  lea -64(%rbp), %rax # symbol ref lvalue `head`
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
.L.str.49:
  .byte 0x61
  .byte 0x72
  .byte 0x67
  .byte 0x75
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x20
  .byte 0x6d
  .byte 0x61
  .byte 0x79
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x76
  .byte 0x6f
  .byte 0x69
  .byte 0x64
  .byte 0x60
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.48:
  .byte 0x49
  .byte 0x53
  .byte 0x4f
  .byte 0x20
  .byte 0x43
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x69
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x6e
  .byte 0x61
  .byte 0x6d
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x67
  .byte 0x75
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x20
  .byte 0x62
  .byte 0x65
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x2e
  .byte 0x2e
  .byte 0x2e
  .byte 0x60
  .byte 0x00
.section .rodata
.L.str.47:
  .byte 0x60
  .byte 0x76
  .byte 0x6f
  .byte 0x69
  .byte 0x64
  .byte 0x60
  .byte 0x20
  .byte 0x6d
  .byte 0x75
  .byte 0x73
  .byte 0x74
  .byte 0x20
  .byte 0x62
  .byte 0x65
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x72
  .byte 0x73
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x6f
  .byte 0x6e
  .byte 0x6c
  .byte 0x79
  .byte 0x20
  .byte 0x70
  .byte 0x61
  .byte 0x72
  .byte 0x61
  .byte 0x6d
  .byte 0x65
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x69
  .byte 0x66
  .byte 0x20
  .byte 0x73
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x64
  .byte 0x00
.text
.globl make_parameter
.type  make_parameter, @function
make_parameter:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $24, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $24, %rax
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
  lea -24(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `p`
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
  lea -8(%rbp), %rax # symbol ref lvalue `p`
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
.globl parse_initializer
.type  parse_initializer, @function
parse_initializer:
  push %rbp
  movq %rsp, %rbp
  sub $84, %rsp
  push %rdi
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $32, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.105
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_scalar_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.107
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $6, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.107:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.106
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.43(%rip), %rax
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
  jmp .L.end.106
.L.else.106:
.L.end.106:
  lea -60(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $24, %rdx
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
  lea -60(%rbp), %rax # symbol ref lvalue `init`
  push %rax
  lea -36(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -28(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $7, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.108
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `field_cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.108
.L.else.108:
.L.end.108:
.L.for.header.109:
  mov $0, %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.109
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $6, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.110
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `child_ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.110
.L.else.110:
.L.end.110:
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $7, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.111
  lea -24(%rbp), %rax # symbol ref lvalue `field_cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.112
  lea -24(%rbp), %rax # symbol ref lvalue `field_cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `child_ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `field_cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `field_cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.112
.L.else.112:
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.44(%rip), %rax
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
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `child_ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.112:
  jmp .L.end.111
.L.else.111:
.L.end.111:
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `child_ty`
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
  call parse_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `inits`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `inits`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.114
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $58, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.114:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.113
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $58, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.113
.L.else.113:
.L.end.113:
  lea -28(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  add $8, %rsp
.L.for.next.109:
  jmp .L.for.header.109
.L.for.end.109:
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $6, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.115
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.116
  lea -28(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.116
.L.else.116:
.L.end.116:
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setl %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.117
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.45(%rip), %rax
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
  jmp .L.end.117
.L.else.117:
.L.end.117:
  jmp .L.end.115
.L.else.115:
.L.end.115:
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_scalar_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.119
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $6, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.119:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.118
  lea -28(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setge %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.120
  lea -76(%rbp), %rax # symbol ref lvalue `decl`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.46(%rip), %rax
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
  jmp .L.end.120
.L.else.120:
.L.end.120:
  lea -60(%rbp), %rax # symbol ref lvalue `init`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.118
.L.else.118:
.L.end.118:
  lea -60(%rbp), %rax # symbol ref lvalue `init`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_initializer_aggregate
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.105
.L.else.105:
.L.end.105:
  lea -84(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call assignment_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `init_expr`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `initializing_type`
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
  call convert
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.46:
  .byte 0x65
  .byte 0x78
  .byte 0x63
  .byte 0x65
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x65
  .byte 0x6c
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.45:
  .byte 0x65
  .byte 0x78
  .byte 0x63
  .byte 0x65
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x65
  .byte 0x6c
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x72
  .byte 0x61
  .byte 0x79
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.44:
  .byte 0x65
  .byte 0x78
  .byte 0x63
  .byte 0x65
  .byte 0x73
  .byte 0x73
  .byte 0x20
  .byte 0x65
  .byte 0x6c
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.43:
  .byte 0x62
  .byte 0x72
  .byte 0x61
  .byte 0x63
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x6f
  .byte 0x75
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x00
.text
.globl string_literal
.type  string_literal, @function
string_literal:
  push %rbp
  movq %rsp, %rbp
  sub $2088, %rsp
  push %rdi
  lea -2088(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -2080(%rbp), %rax # symbol ref lvalue `strbuf`
  push %rax
  pop %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $2048, %rax
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
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -2088(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.121:
  lea -2088(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $29, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.121
  lea -2088(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $29, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `strlit`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strlen
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  mov $2048, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setge %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.122
  lea -24(%rbp), %rax # symbol ref lvalue `start`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.42(%rip), %rax
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
  jmp .L.end.122
.L.else.122:
.L.end.122:
  lea -2080(%rbp), %rax # symbol ref lvalue `strbuf`
  push %rax
  pop %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `count`
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
  add %rdi, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `strlit`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `len`
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
  call memcpy
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.121:
  jmp .L.for.header.121
.L.for.end.121:
  lea -24(%rbp), %rax # symbol ref lvalue `start`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -2080(%rbp), %rax # symbol ref lvalue `strbuf`
  push %rax
  pop %rax
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
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_string_literal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.42:
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x69
  .byte 0x6e
  .byte 0x67
  .byte 0x20
  .byte 0x6c
  .byte 0x69
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x6c
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x6f
  .byte 0x20
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x67
  .byte 0x65
  .byte 0x00
.text
.globl primary
.type  primary, @function
primary:
  push %rbp
  movq %rsp, %rbp
  sub $48, %rsp
  push %rdi
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.123
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `nested`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.123
.L.else.123:
.L.end.123:
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.124
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call scope_lookup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.125
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $4, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.126
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $72, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.126
.L.else.126:
.L.end.126:
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  call make_symbol_ref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.125
.L.else.125:
.L.end.125:
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_symbol_ref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.124
.L.else.124:
.L.end.124:
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $28, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.127
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $28, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `char_lit`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `char_lit`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.127
.L.else.127:
.L.end.127:
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $29, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.128
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call string_literal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.128
.L.else.128:
.L.end.128:
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $37, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `integer`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `integer`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl postfix_expr
.type  postfix_expr, @function
postfix_expr:
  push %rbp
  movq %rsp, %rbp
  sub $152, %rsp
  push %rdi
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call primary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.129:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -136(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $41, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.130
  mov $0, %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_lvalue
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.131
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.33(%rip), %rax
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
  jmp .L.end.131
.L.else.131:
.L.end.131:
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
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
  call make_postincrement
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.130
.L.else.130:
.L.end.130:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $43, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.132
  mov $0, %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_lvalue
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.133
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.34(%rip), %rax
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
  jmp .L.end.133
.L.else.133:
.L.end.133:
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
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
  call make_postdecrement
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.132
.L.else.132:
.L.end.132:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.134
  lea -128(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $64, %rdx
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
  lea -128(%rbp), %rax # symbol ref lvalue `arg_head`
  push %rax
  lea -64(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.135
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call assignment_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `args`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `args`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.header.136:
  mov $0, %rax
  push %rax
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.136
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $58, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call assignment_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `args`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `args`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.136:
  jmp .L.for.header.136
.L.for.end.136:
  jmp .L.end.135
.L.else.135:
.L.end.135:
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $4, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.137
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.35(%rip), %rax
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
  jmp .L.end.137
.L.else.137:
.L.end.137:
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
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
  je .L.else.138
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.138
.L.else.138:
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $5, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.139
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.36(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.139
.L.else.139:
.L.end.139:
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $40, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `ret_ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.138:
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -128(%rbp), %rax # symbol ref lvalue `arg_head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `ret_ty`
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
  call make_call
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.134
.L.else.134:
.L.end.134:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $34, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.140
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $35, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.143
  lea -40(%rbp), %rax # symbol ref lvalue `subscript`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  setne %al
  movzb %al, %eax
.L.and.143:
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.142
  lea -40(%rbp), %rax # symbol ref lvalue `subscript`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.and.144
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.144:
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.142:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.141
  lea -40(%rbp), %rax # symbol ref lvalue `subscript`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.146
  lea -40(%rbp), %rax # symbol ref lvalue `subscript`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  setne %al
  movzb %al, %eax
.L.and.146:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.145
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
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
  call make_deref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.145
.L.else.145:
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `subscript`
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
  call make_add_or_sub
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_deref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.145:
  jmp .L.for.next.129
  jmp .L.end.141
.L.else.141:
.L.end.141:
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.37(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `subscript`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
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
  jmp .L.end.140
.L.else.140:
.L.end.140:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $59, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.147
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $7, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.149
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $8, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.149:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.148
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.38(%rip), %rax
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
  jmp .L.end.148
.L.else.148:
.L.end.148:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
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
  call field_lookup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `member`
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
  je .L.else.150
  lea -32(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.39(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
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
  jmp .L.end.150
.L.else.150:
.L.end.150:
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `member`
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
  call make_member
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.129
  jmp .L.end.147
.L.else.147:
.L.end.147:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $60, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.151
  mov $0, %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
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
  je .L.else.152
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.40(%rip), %rax
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
  jmp .L.end.152
.L.else.152:
.L.end.152:
  lea -152(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
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
  call field_lookup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `member`
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
  je .L.else.153
  lea -16(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.41(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `field_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
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
  jmp .L.end.153
.L.else.153:
.L.end.153:
  lea -136(%rbp), %rax # symbol ref lvalue `candidate`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `member`
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
  call make_member_deref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.129
  jmp .L.end.151
.L.else.151:
.L.end.151:
  lea -144(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.for.next.129:
  jmp .L.for.header.129
.L.for.end.129:
  leave
  ret
.section .rodata
.L.str.41:
  .byte 0x6e
  .byte 0x6f
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x63
  .byte 0x68
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x6c
  .byte 0x64
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x00
.section .rodata
.L.str.40:
  .byte 0x6c
  .byte 0x65
  .byte 0x66
  .byte 0x74
  .byte 0x2d
  .byte 0x68
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x2d
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x6d
  .byte 0x65
  .byte 0x6d
  .byte 0x62
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x66
  .byte 0x20
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x73
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x70
  .byte 0x6f
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.39:
  .byte 0x6e
  .byte 0x6f
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x63
  .byte 0x68
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x6c
  .byte 0x64
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x00
.section .rodata
.L.str.38:
  .byte 0x6c
  .byte 0x65
  .byte 0x66
  .byte 0x74
  .byte 0x2d
  .byte 0x68
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x2d
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x6d
  .byte 0x65
  .byte 0x6d
  .byte 0x62
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x73
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x75
  .byte 0x6e
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.37:
  .byte 0x69
  .byte 0x6e
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x72
  .byte 0x61
  .byte 0x79
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x73
  .byte 0x63
  .byte 0x72
  .byte 0x69
  .byte 0x70
  .byte 0x74
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.36:
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x6c
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x6f
  .byte 0x62
  .byte 0x6a
  .byte 0x65
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x66
  .byte 0x75
  .byte 0x6e
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x73
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.35:
  .byte 0x6f
  .byte 0x6e
  .byte 0x6c
  .byte 0x79
  .byte 0x20
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x6c
  .byte 0x73
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x62
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x69
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x72
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x70
  .byte 0x70
  .byte 0x6f
  .byte 0x72
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.34:
  .byte 0x6c
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x69
  .byte 0x72
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x61
  .byte 0x73
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x72
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.33:
  .byte 0x6c
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x69
  .byte 0x72
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x61
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x63
  .byte 0x72
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x00
.text
.globl unary_expr
.type  unary_expr, @function
unary_expr:
  push %rbp
  movq %rsp, %rbp
  sub $72, %rsp
  push %rdi
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -64(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $44, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.154
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -56(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -56(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $3, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.155
  lea -56(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.28(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.155
.L.else.155:
.L.end.155:
  lea -56(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $4, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.156
  lea -56(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  jmp .L.end.156
.L.else.156:
.L.end.156:
  lea -64(%rbp), %rax # symbol ref lvalue `unop_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `base`
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
  call make_deref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.154
.L.else.154:
.L.end.154:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $54, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.157
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_lvalue
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.158
  lea -64(%rbp), %rax # symbol ref lvalue `unop_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  jmp .L.end.158
.L.else.158:
.L.end.158:
  lea -64(%rbp), %rax # symbol ref lvalue `unop_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `base`
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
  call make_addrof
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.157
.L.else.157:
.L.end.157:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $26, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.159
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call decl_type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -64(%rbp), %rax # symbol ref lvalue `unop_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_long(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.159
.L.else.159:
.L.end.159:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $9, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.160
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.161
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call can_start_type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.162
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call decl_type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -64(%rbp), %rax # symbol ref lvalue `unop_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.162
.L.else.162:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `sizeof_arg`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.162:
  jmp .L.end.161
.L.else.161:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `sizeof_arg`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.161:
  lea -64(%rbp), %rax # symbol ref lvalue `unop_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `sizeof_arg`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.160
.L.else.160:
.L.end.160:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $62, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.163
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_scalar_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.164
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.31(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.164
.L.else.164:
.L.end.164:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $5, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
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
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
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
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.163
.L.else.163:
.L.end.163:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $42, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.165
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_arithmetic_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.166
  lea -8(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.32(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.166
.L.else.166:
.L.end.166:
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
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
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `base`
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
  call make_add_or_sub
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.165
.L.else.165:
.L.end.165:
  lea -72(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call postfix_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.32:
  .byte 0x61
  .byte 0x72
  .byte 0x69
  .byte 0x74
  .byte 0x68
  .byte 0x6d
  .byte 0x65
  .byte 0x74
  .byte 0x69
  .byte 0x63
  .byte 0x20
  .byte 0x6e
  .byte 0x65
  .byte 0x67
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x69
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x69
  .byte 0x74
  .byte 0x68
  .byte 0x6d
  .byte 0x65
  .byte 0x74
  .byte 0x69
  .byte 0x63
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.31:
  .byte 0x6c
  .byte 0x6f
  .byte 0x67
  .byte 0x69
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x69
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.30:
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x6c
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x75
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.29:
  .byte 0x63
  .byte 0x61
  .byte 0x6e
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x66
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x6e
  .byte 0x63
  .byte 0x65
  .byte 0x20
  .byte 0x76
  .byte 0x6f
  .byte 0x69
  .byte 0x64
  .byte 0x20
  .byte 0x70
  .byte 0x6f
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.28:
  .byte 0x63
  .byte 0x61
  .byte 0x6e
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x66
  .byte 0x65
  .byte 0x72
  .byte 0x65
  .byte 0x6e
  .byte 0x63
  .byte 0x65
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x2d
  .byte 0x70
  .byte 0x6f
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x28
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.text
.globl is_lvalue
.type  is_lvalue, @function
is_lvalue:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $16, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.167
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_lvalue
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.167
.L.else.167:
.L.end.167:
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $4, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.168
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $6, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.169
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $17, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.169:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.168:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl cast_expr
.type  cast_expr, @function
cast_expr:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.170
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call can_start_type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.171
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call decl_type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `typename`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.173
  mov $0, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `typename`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_scalar_type
  mov %rbp, %rsp
  pop %rbp
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
.L.and.173:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.172
  lea -24(%rbp), %rax # symbol ref lvalue `start`
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
  call error_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.172
.L.else.172:
.L.end.172:
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call cast_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `start`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `typename`
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
  call make_conv
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.171
.L.else.171:
.L.end.171:
  lea -24(%rbp), %rax # symbol ref lvalue `start`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.170
.L.else.170:
.L.end.170:
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.27:
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x76
  .byte 0x65
  .byte 0x72
  .byte 0x73
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x2d
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x00
.text
.globl mul_expr
.type  mul_expr, @function
mul_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call cast_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.174:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $44, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.175
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $2, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.174
  jmp .L.end.175
.L.else.175:
.L.end.175:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $46, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.176
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $4, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.174
  jmp .L.end.176
.L.else.176:
.L.end.176:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $45, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.177
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $3, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call unary_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.174
  jmp .L.end.177
.L.else.177:
.L.end.177:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.for.next.174:
  jmp .L.for.header.174
.L.for.end.174:
  leave
  ret
.text
.globl add_expr
.type  add_expr, @function
add_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call mul_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.178:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $40, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.179
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call mul_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_add_or_sub
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.178
  jmp .L.end.179
.L.else.179:
.L.end.179:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $42, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.180
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call mul_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_add_or_sub
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.178
  jmp .L.end.180
.L.else.180:
.L.end.180:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.for.next.178:
  jmp .L.for.header.178
.L.for.end.178:
  leave
  ret
.text
.globl relational_expr
.type  relational_expr, @function
relational_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call add_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.181:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $48, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.182
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $5, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call add_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.181
  jmp .L.end.182
.L.else.182:
.L.end.182:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $49, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.183
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $6, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call add_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.181
  jmp .L.end.183
.L.else.183:
.L.end.183:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $50, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.184
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $7, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call add_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.181
  jmp .L.end.184
.L.else.184:
.L.end.184:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $51, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.185
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $8, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call add_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.181
  jmp .L.end.185
.L.else.185:
.L.end.185:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $52, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.186
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $9, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call add_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.181
  jmp .L.end.186
.L.else.186:
.L.end.186:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $53, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.187
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $10, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call add_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.181
  jmp .L.end.187
.L.else.187:
.L.end.187:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.for.next.181:
  jmp .L.for.header.181
.L.for.end.181:
  leave
  ret
.text
.globl and_expr
.type  and_expr, @function
and_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call relational_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.188:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $54, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.189
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $12, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call and_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_bitwise_op
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.188
  jmp .L.end.189
.L.else.189:
.L.end.189:
  jmp .L.for.end.188
.L.for.next.188:
  jmp .L.for.header.188
.L.for.end.188:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
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
.globl inclusive_or_expr
.type  inclusive_or_expr, @function
inclusive_or_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call and_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.190:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $56, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.191
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $11, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call inclusive_or_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_bitwise_op
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.190
  jmp .L.end.191
.L.else.191:
.L.end.191:
  jmp .L.for.end.190
.L.for.next.190:
  jmp .L.for.header.190
.L.for.end.190:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
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
.globl logical_and_expr
.type  logical_and_expr, @function
logical_and_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call inclusive_or_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.192:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $55, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.193
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call logical_and_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_logical_and
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.192
  jmp .L.end.193
.L.else.193:
.L.end.193:
  jmp .L.for.end.192
.L.for.next.192:
  jmp .L.for.header.192
.L.for.end.192:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
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
.globl logical_or_expr
.type  logical_or_expr, @function
logical_or_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call logical_and_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.194:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $57, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.195
  lea -8(%rbp), %rax # symbol ref lvalue `op_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call logical_or_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_logical_or
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.194
  jmp .L.end.195
.L.else.195:
.L.end.195:
  jmp .L.for.end.194
.L.for.next.194:
  jmp .L.for.header.194
.L.for.end.194:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
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
.globl conditional_expr
.type  conditional_expr, @function
conditional_expr:
  push %rbp
  movq %rsp, %rbp
  sub $40, %rsp
  push %rdi
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call logical_or_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.196:
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $63, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.197
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $39, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call conditional_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `question_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `true_expr`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `false_expr`
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
  call make_cond
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.196
  jmp .L.end.197
.L.else.197:
.L.end.197:
  jmp .L.for.end.196
.L.for.next.196:
  jmp .L.for.header.196
.L.for.end.196:
  lea -32(%rbp), %rax # symbol ref lvalue `base`
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
.globl assignment_expr
.type  assignment_expr, @function
assignment_expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call conditional_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.198:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $47, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.199
  lea -8(%rbp), %rax # symbol ref lvalue `eq_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call assignment_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_assign
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.198
  jmp .L.end.199
.L.else.199:
.L.end.199:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.for.next.198:
  jmp .L.for.header.198
.L.for.end.198:
  leave
  ret
.text
.globl expr
.type  expr, @function
expr:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call assignment_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.200:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $58, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.201
  lea -8(%rbp), %rax # symbol ref lvalue `comma`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_comma
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.200
  jmp .L.end.201
.L.else.201:
.L.end.201:
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
.L.for.next.200:
  jmp .L.for.header.200
.L.for.end.200:
  leave
  ret
.text
.globl do_stmt
.type  do_stmt, @function
do_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $4, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $8, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `do_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `cond`
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
  call make_do_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl while_stmt
.type  while_stmt, @function
while_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $8, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `while_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %r8
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_for_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl for_stmt
.type  for_stmt, @function
for_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $48, %rsp
  push %rdi
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $7, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_scope
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call can_start_type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.202
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declaration
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `initializer`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.202
.L.else.202:
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `initializer`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.end.202:
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.203
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `next`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.203
.L.else.203:
.L.end.203:
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -48(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_scope
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `for_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `initializer`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `next`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %r8
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_for_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl switch_stmt
.type  switch_stmt, @function
switch_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $64, %rsp
  push %rdi
  lea -64(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -64(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $18, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -56(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -64(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -64(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -64(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $32, %rdx
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
  lea -40(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_switch
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -64(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_switch
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -56(%rbp), %rax # symbol ref lvalue `switch_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `head`
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
  call make_switch
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl if_stmt
.type  if_stmt, @function
if_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $40, %rsp
  push %rdi
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $5, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $30, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $31, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $6, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.204
  lea -40(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `else_`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.204
.L.else.204:
.L.end.204:
  lea -32(%rbp), %rax # symbol ref lvalue `if_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `then`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `else_`
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
  call make_if_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl compound_stmt
.type  compound_stmt, @function
compound_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $96, %rsp
  push %rdi
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $32, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -88(%rbp), %rax
  mov %rax, %rdi
  mov $0, %rsi
  mov $64, %rdx
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
  lea -88(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_scope
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.header.205:
  mov $0, %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.205
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.206:
  lea -8(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.206
  lea -8(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `stmts`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `stmts`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.206:
  lea -8(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.206
.L.for.end.206:
.L.for.next.205:
  jmp .L.for.header.205
.L.for.end.205:
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_scope
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -88(%rbp), %rax # symbol ref lvalue `head`
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
.globl declaration
.type  declaration, @function
declaration:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call external_declaration
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl expr_stmt
.type  expr_stmt, @function
expr_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.207
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.207
.L.else.207:
.L.end.207:
  lea -16(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `e`
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
.globl return_stmt
.type  return_stmt, @function
return_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $36, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.208
  lea -16(%rbp), %rax # symbol ref lvalue `ret_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_return
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.208
.L.else.208:
.L.end.208:
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `ret_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `val`
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
  call make_return
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl stmt
.type  stmt, @function
stmt:
  push %rbp
  movq %rsp, %rbp
  sub $80, %rsp
  push %rdi
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $36, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.209
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call return_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.209
.L.else.209:
.L.end.209:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call can_start_type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.210
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declaration
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.210
.L.else.210:
.L.end.210:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $32, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.211
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call compound_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.211
.L.else.211:
.L.end.211:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $5, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.212
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call if_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.212
.L.else.212:
.L.end.212:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $7, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.213
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call for_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.213
.L.else.213:
.L.end.213:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $8, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.214
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call while_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.214
.L.else.214:
.L.end.214:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $4, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.215
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call do_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.215
.L.else.215:
.L.end.215:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $18, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.216
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call switch_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.216
.L.else.216:
.L.end.216:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $19, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.217
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $19, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -72(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call in_switch
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.218
  lea -72(%rbp), %rax # symbol ref lvalue `case_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.21(%rip), %rax
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
  jmp .L.end.218
.L.else.218:
.L.end.218:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call logical_or_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -64(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $39, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.219
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.22(%rip), %rax
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
  jmp .L.end.219
.L.else.219:
.L.end.219:
  lea -72(%rbp), %rax # symbol ref lvalue `case_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.23(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -56(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -72(%rbp), %rax # symbol ref lvalue `case_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `case_cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `label`
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
  call make_switch_case
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call switch_record_case
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -56(%rbp), %rax # symbol ref lvalue `label`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.217
.L.else.217:
.L.end.217:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $20, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.220
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $20, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call in_switch
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.221
  lea -48(%rbp), %rax # symbol ref lvalue `default_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.24(%rip), %rax
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
  jmp .L.end.221
.L.else.221:
.L.end.221:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $39, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $33, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.222
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.25(%rip), %rax
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
  jmp .L.end.222
.L.else.222:
.L.end.222:
  lea -48(%rbp), %rax # symbol ref lvalue `default_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.26(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -40(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -48(%rbp), %rax # symbol ref lvalue `default_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `label`
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
  call make_switch_case
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call switch_record_case
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `label`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.220
.L.else.220:
.L.end.220:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $11, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.223
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $11, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `break_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_break_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.223
.L.else.223:
.L.end.223:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $12, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.224
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $12, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `continue_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_continue_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.224
.L.else.224:
.L.end.224:
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -80(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $38, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `semi_tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `e`
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
  call make_expr_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.26:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x61
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.25:
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.24:
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x61
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.23:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x63
  .byte 0x61
  .byte 0x73
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.22:
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.21:
  .byte 0x63
  .byte 0x61
  .byte 0x73
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x6d
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x00
.text
.globl can_start_type_name
.type  can_start_type_name, @function
can_start_type_name:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $27, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.225
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call scope_lookup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.226
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.226
.L.else.226:
.L.end.226:
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call scope_lookup_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.227
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.227
.L.else.227:
.L.end.227:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.225
.L.else.225:
.L.end.225:
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $2, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.228
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.229
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $10, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.230
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $13, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.231
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $16, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.232
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $17, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.233
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $14, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.234
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $15, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.235
  lea -8(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $25, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call peek
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.235:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.234:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.233:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.232:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.231:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.230:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.229:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.228:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl decl_type_name
.type  decl_type_name, @function
decl_type_name:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call declaration_specifiers
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.236:
  lea -16(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $44, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call equal
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.236
  lea -8(%rbp), %rax # symbol ref lvalue `decltype`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_pointer_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `decltype`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.236:
  jmp .L.for.header.236
.L.for.end.236:
  lea -8(%rbp), %rax # symbol ref lvalue `decltype`
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
.globl convert
.type  convert, @function
convert:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `e`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -8(%rbp), %rax # symbol ref lvalue `target_ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `target_ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.237
  lea -16(%rbp), %rax # symbol ref lvalue `e`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.239
  lea -16(%rbp), %rax # symbol ref lvalue `e`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  setne %al
  movzb %al, %eax
.L.and.239:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.238
  lea -16(%rbp), %rax # symbol ref lvalue `e`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_pointer_type
  mov %rbp, %rsp
  pop %rbp
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
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.238
.L.else.238:
.L.end.238:
  lea -16(%rbp), %rax # symbol ref lvalue `e`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_arithmetic_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.240
  lea -16(%rbp), %rax # symbol ref lvalue `e`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.20(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `e`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `target_ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
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
  jmp .L.end.240
.L.else.240:
.L.end.240:
  jmp .L.end.237
.L.else.237:
.L.end.237:
  lea -16(%rbp), %rax # symbol ref lvalue `e`
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
.L.str.20:
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x76
  .byte 0x65
  .byte 0x72
  .byte 0x73
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x6d
  .byte 0x61
  .byte 0x6b
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x70
  .byte 0x6f
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x66
  .byte 0x72
  .byte 0x6f
  .byte 0x6d
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x67
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x68
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x63
  .byte 0x61
  .byte 0x73
  .byte 0x74
  .byte 0x00
.text
.globl equal
.type  equal, @function
equal:
  push %rbp
  movq %rsp, %rbp
  sub $20, %rsp
  push %rdi
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -12(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.241
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.241
.L.else.241:
.L.end.241:
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call eat
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl peek
.type  peek, @function
peek:
  push %rbp
  movq %rsp, %rbp
  sub $20, %rsp
  push %rdi
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -12(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `kind`
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
  leave
  ret
  leave
  ret
.text
.globl eat
.type  eat, @function
eat:
  push %rbp
  movq %rsp, %rbp
  sub $20, %rsp
  push %rdi
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -12(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -8(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `kind`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.242
  lea -8(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.19(%rip), %rax
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
  jmp .L.end.242
.L.else.242:
.L.end.242:
  lea -8(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `tok`
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
.L.str.19:
  .byte 0x75
  .byte 0x6e
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x6b
  .byte 0x65
  .byte 0x6e
  .byte 0x00
.text
.globl make_comma
.type  make_comma, @function
make_comma:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $18, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_cond
.type  make_cond, @function
make_cond:
  push %rbp
  movq %rsp, %rbp
  sub $40, %rsp
  push %rdi
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -24(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -16(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_scalar_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.243
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.17(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.243
.L.else.243:
.L.end.243:
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
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
  call is_valid_cond
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.244
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.18(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
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
  jmp .L.end.244
.L.else.244:
.L.end.244:
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $19, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.str.18:
  .byte 0x69
  .byte 0x6e
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x66
  .byte 0x61
  .byte 0x6c
  .byte 0x73
  .byte 0x65
  .byte 0x20
  .byte 0x62
  .byte 0x72
  .byte 0x61
  .byte 0x6e
  .byte 0x63
  .byte 0x68
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x64
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x61
  .byte 0x6c
  .byte 0x20
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x72
  .byte 0x65
  .byte 0x73
  .byte 0x73
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.17:
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x64
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x6e
  .byte 0x61
  .byte 0x72
  .byte 0x79
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x6d
  .byte 0x75
  .byte 0x73
  .byte 0x74
  .byte 0x20
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x73
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.text
.globl is_valid_cond
.type  is_valid_cond, @function
is_valid_cond:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_arithmetic_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.246
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_arithmetic_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.246:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.245
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.245
.L.else.245:
.L.end.245:
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $7, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.248
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $7, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.249
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
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
  setne %al
  movzb %al, %eax
.L.and.249:
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.248:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.247
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.247
.L.else.247:
.L.end.247:
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $8, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.251
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $8, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.252
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
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
  setne %al
  movzb %al, %eax
.L.and.252:
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.251:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.250
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.250
.L.else.250:
.L.end.250:
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_void(%rip), %rax
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
  je .L.and.254
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_void(%rip), %rax
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
  setne %al
  movzb %al, %eax
.L.and.254:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.253
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.253
.L.else.253:
.L.end.253:
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.256
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.256:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.255
  lea -16(%rbp), %rax # symbol ref lvalue `tru`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `fls`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
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
  call is_valid_cond
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.255
.L.else.255:
.L.end.255:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl make_initializer_aggregate
.type  make_initializer_aggregate, @function
make_initializer_aggregate:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `inits`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $24, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $24, %rax
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
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `inits`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_initializer
.type  make_initializer, @function
make_initializer:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `expr`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $24, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $24, %rax
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
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `expr`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_initialize
.type  make_initialize, @function
make_initialize:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `init`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $22, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `init`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_label
.type  make_label, @function
make_label:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $21, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_switch_case
.type  make_switch_case, @function
make_switch_case:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `label`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $32, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `sc`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $32, %rax
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sc`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sc`
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
  lea -16(%rbp), %rax # symbol ref lvalue `label`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sc`
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
  lea -8(%rbp), %rax # symbol ref lvalue `sc`
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
.globl make_switch
.type  make_switch, @function
make_switch:
  push %rbp
  movq %rsp, %rbp
  sub $40, %rsp
  push %rdi
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -24(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -16(%rbp), %rax # symbol ref lvalue `cases`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_arithmetic_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.257
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.16(%rip), %rax
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
  jmp .L.end.257
.L.else.257:
.L.end.257:
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $20, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `cases`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.str.16:
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x20
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x64
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6d
  .byte 0x75
  .byte 0x73
  .byte 0x74
  .byte 0x20
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x67
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x00
.text
.globl make_member_deref
.type  make_member_deref, @function
make_member_deref:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `field`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $17, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `field`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `field`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_member
.type  make_member, @function
make_member:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `field`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $16, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `field`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `field`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_postdecrement
.type  make_postdecrement, @function
make_postdecrement:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $15, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_postincrement
.type  make_postincrement, @function
make_postincrement:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $14, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_call
.type  make_call, @function
make_call:
  push %rbp
  movq %rsp, %rbp
  sub $40, %rsp
  push %rdi
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -24(%rbp), %rax # symbol ref lvalue `args`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -16(%rbp), %rax # symbol ref lvalue `ret_ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $7, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `ret_ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `args`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_addrof
.type  make_addrof, @function
make_addrof:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $5, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_pointer_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_deref
.type  make_deref, @function
make_deref:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $6, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_continue_stmt
.type  make_continue_stmt, @function
make_continue_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call in_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.258
  lea -16(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.15(%rip), %rax
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
  jmp .L.end.258
.L.else.258:
.L.end.258:
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $12, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.str.15:
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x74
  .byte 0x69
  .byte 0x6e
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x6c
  .byte 0x6f
  .byte 0x6f
  .byte 0x70
  .byte 0x00
.text
.globl make_break_stmt
.type  make_break_stmt, @function
make_break_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call in_loop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.260
  mov $0, %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call in_switch
  mov %rbp, %rsp
  pop %rbp
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
.L.and.260:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.259
  lea -16(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.14(%rip), %rax
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
  jmp .L.end.259
.L.else.259:
.L.end.259:
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $11, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.str.14:
  .byte 0x62
  .byte 0x72
  .byte 0x65
  .byte 0x61
  .byte 0x6b
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x6c
  .byte 0x6f
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x00
.text
.globl make_do_stmt
.type  make_do_stmt, @function
make_do_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $25, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_for_stmt
.type  make_for_stmt, @function
make_for_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $48, %rsp
  push %rdi
  lea -48(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -40(%rbp), %rax # symbol ref lvalue `initializer`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -24(%rbp), %rax # symbol ref lvalue `next`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %r8
  lea -16(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $24, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -48(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -40(%rbp), %rax # symbol ref lvalue `initializer`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `next`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `body`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_if_stmt
.type  make_if_stmt, @function
make_if_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $40, %rsp
  push %rdi
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -24(%rbp), %rax # symbol ref lvalue `then`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -16(%rbp), %rax # symbol ref lvalue `else_`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $23, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `cond`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `then`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `else_`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_expr_stmt
.type  make_expr_stmt, @function
make_expr_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $27, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_symbol_ref
.type  make_symbol_ref, @function
make_symbol_ref:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $4, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.261
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  jmp .L.end.261
.L.else.261:
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.end.261:
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_node_assign
.type  make_node_assign, @function
make_node_assign:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `lvalue`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `rvalue`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $2, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `rvalue`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `lvalue`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `rvalue`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `lvalue`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
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
  call convert
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_nop
.type  make_nop, @function
make_nop:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $3, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_return
.type  make_return, @function
make_return:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `val`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $26, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_void(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `val`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_logical_or
.type  make_logical_or, @function
make_logical_or:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $9, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_logical_and
.type  make_logical_and, @function
make_logical_and:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $8, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_add_or_sub
.type  make_add_or_sub, @function
make_add_or_sub:
  push %rbp
  movq %rsp, %rbp
  sub $44, %rsp
  push %rdi
  lea -44(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.do.body.262:
  mov $0, %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
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
  jne .L.or.264
  lea -44(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.264:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.263
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.8(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.263
.L.else.263:
.L.end.263:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.262
.L.do.end.262:
  lea -44(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.cond.false.265
  lea .L.str.9(%rip), %rax
  push %rax
  pop %rax
  push %rax
  jmp .L.cond.end.266
.L.cond.false.265:
  lea .L.str.10(%rip), %rax
  push %rax
  pop %rax
  push %rax
.L.cond.end.266:
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -44(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_arithmetic_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.268
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_arithmetic_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.268:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.267
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call usual_arithmetic_conversions
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.267
.L.else.267:
.L.end.267:
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.271
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  setne %al
  movzb %al, %eax
.L.and.271:
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.270
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.and.272
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.272:
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.270:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.269
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.cond.false.273
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  jmp .L.cond.end.274
.L.cond.false.273:
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
.L.cond.end.274:
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $2, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_int(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_const
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_node_binary
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.269
.L.else.269:
.L.end.269:
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.276
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.277
  lea -44(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
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
.L.and.277:
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.276:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.275
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.278
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.11(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `tok_str`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %r8
  pop %rcx
  pop %rdx
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
  jmp .L.end.278
.L.else.278:
.L.end.278:
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  jmp .L.end.275
.L.else.275:
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.279
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.12(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `tok_str`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
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
  jmp .L.end.279
.L.else.279:
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.else.280
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.13(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `tok_str`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rcx
  pop %rdx
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
  jmp .L.end.280
.L.else.280:
.L.end.280:
.L.end.279:
.L.end.275:
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -32(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.str.13:
  .byte 0x69
  .byte 0x6e
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.12:
  .byte 0x69
  .byte 0x6e
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.11:
  .byte 0x69
  .byte 0x6e
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.10:
  .byte 0x2d
  .byte 0x00
.section .rodata
.L.str.9:
  .byte 0x2b
  .byte 0x00
.section .rodata
.L.str.8:
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x00
.text
.globl usual_arithmetic_conversions
.type  usual_arithmetic_conversions, @function
usual_arithmetic_conversions:
  push %rbp
  movq %rsp, %rbp
  sub $36, %rsp
  push %rdi
  lea -36(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -28(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -36(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -28(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -20(%rbp), %rax # symbol ref lvalue `left_ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `right_ty`
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
  je .L.else.281
  leave
  ret
  jmp .L.end.281
.L.else.281:
.L.end.281:
  lea -20(%rbp), %rax # symbol ref lvalue `left_ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `right_ty`
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
  call integer_conversion_rank_compare
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -4(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -4(%rbp), %rax # symbol ref lvalue `cmp`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setl %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.282
  lea -36(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `right_ty`
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
  call make_conv
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.282
.L.else.282:
  lea -4(%rbp), %rax # symbol ref lvalue `cmp`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.283
  lea -28(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `left_ty`
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
  call make_conv
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.283
.L.else.283:
.L.end.283:
.L.end.282:
  leave
  ret
.text
.globl make_conv
.type  make_conv, @function
make_conv:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  mov $10, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_node_binary
.type  make_node_binary, @function
make_node_binary:
  push %rbp
  movq %rsp, %rbp
  sub $36, %rsp
  push %rdi
  lea -36(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -28(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -36(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.do.body.284:
  mov $0, %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
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
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.285
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.6(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.285
.L.else.285:
.L.end.285:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.284
.L.do.end.284:
.L.do.body.286:
  mov $0, %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.287
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.7(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.287
.L.else.287:
.L.end.287:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.286
.L.do.end.286:
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -28(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.str.7:
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x6c
  .byte 0x20
  .byte 0x6d
  .byte 0x61
  .byte 0x6b
  .byte 0x65
  .byte 0x5f
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x5f
  .byte 0x6f
  .byte 0x72
  .byte 0x5f
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x28
  .byte 0x29
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x73
  .byte 0x74
  .byte 0x65
  .byte 0x61
  .byte 0x64
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x73
  .byte 0x00
.section .rodata
.L.str.6:
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x6c
  .byte 0x20
  .byte 0x6d
  .byte 0x61
  .byte 0x6b
  .byte 0x65
  .byte 0x5f
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x5f
  .byte 0x6f
  .byte 0x72
  .byte 0x5f
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x28
  .byte 0x29
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x73
  .byte 0x74
  .byte 0x65
  .byte 0x61
  .byte 0x64
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x73
  .byte 0x00
.text
.globl make_bitwise_op
.type  make_bitwise_op, @function
make_bitwise_op:
  push %rbp
  movq %rsp, %rbp
  sub $36, %rsp
  push %rdi
  lea -36(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -28(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_integer_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.288
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.4(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.288
.L.else.288:
.L.end.288:
  mov $0, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call is_integer_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.289
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.5(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call type_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rdx
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
  jmp .L.end.289
.L.else.289:
.L.end.289:
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call usual_arithmetic_conversions
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -36(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -28(%rbp), %rax # symbol ref lvalue `op`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.L.str.5:
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x67
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x62
  .byte 0x69
  .byte 0x74
  .byte 0x77
  .byte 0x69
  .byte 0x73
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.4:
  .byte 0x65
  .byte 0x78
  .byte 0x70
  .byte 0x65
  .byte 0x63
  .byte 0x74
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x65
  .byte 0x67
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x62
  .byte 0x69
  .byte 0x74
  .byte 0x77
  .byte 0x69
  .byte 0x73
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x72
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x28
  .byte 0x68
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0x29
  .byte 0x00
.text
.globl make_string_literal
.type  make_string_literal, @function
make_string_literal:
  push %rbp
  movq %rsp, %rbp
  sub $4136, %rsp
  push %rdi
  lea -4136(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -4128(%rbp), %rax # symbol ref lvalue `contents`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -4120(%rbp), %rax # symbol ref lvalue `symbol_name_buf`
  push %rax
  pop %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $4096, %rax
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
  lea -4128(%rbp), %rax # symbol ref lvalue `contents`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strlen
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea ty_char(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `len`
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
  call make_array_type
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -4120(%rbp), %rax # symbol ref lvalue `symbol_name_buf`
  push %rax
  pop %rax
  push %rax
  mov $4096, %rax
  push %rax
  lea .L.str.3(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea counter.0(%rip), %rax
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call snprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -4136(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4120(%rbp), %rax # symbol ref lvalue `symbol_name_buf`
  push %rax
  pop %rax
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
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call make_symbol_global
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -4128(%rbp), %rax # symbol ref lvalue `contents`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $64, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea symbols(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea symbols(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -4136(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  call make_symbol_ref
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.3:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x2e
  .byte 0x25
  .byte 0x64
  .byte 0x00
.bss
.globl counter.0
counter.0:
  .zero 4
.text
.globl make_node_arg
.type  make_node_arg, @function
make_node_arg:
  push %rbp
  movq %rsp, %rbp
  sub $28, %rsp
  push %rdi
  lea -28(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -20(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -12(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $13, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -28(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -20(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -12(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_node_const
.type  make_node_const, @function
make_node_const:
  push %rbp
  movq %rsp, %rbp
  sub $28, %rsp
  push %rdi
  lea -28(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -20(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -12(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  mov $64, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $64, %rax
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
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -20(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -28(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -12(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
.globl make_symbol_constant
.type  make_symbol_constant, @function
make_symbol_constant:
  push %rbp
  movq %rsp, %rbp
  sub $36, %rsp
  push %rdi
  lea -36(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -28(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -20(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -12(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  mov $88, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $88, %rax
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
  lea -36(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $56, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -20(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $4, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -28(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $72, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
.globl make_symbol_global
.type  make_symbol_global, @function
make_symbol_global:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $88, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $88, %rax
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
  lea -32(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $2, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $56, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $2, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
.globl make_symbol_function
.type  make_symbol_function, @function
make_symbol_function:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $88, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $88, %rax
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $56, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $3, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
.globl make_symbol_local
.type  make_symbol_local, @function
make_symbol_local:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $88, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $88, %rax
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $56, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $48, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea current_function(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $72, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_function(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $72, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `s`
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
.globl make_type_symbol
.type  make_type_symbol, @function
make_type_symbol:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $40, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $40, %rax
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
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.290
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call strndup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.290
.L.else.290:
.L.end.290:
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
.globl in_loop
.type  in_loop, @function
in_loop:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea loop_depth(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl pop_loop
.type  pop_loop, @function
pop_loop:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
.L.do.body.291:
  mov $0, %rax
  push %rax
  lea loop_depth(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.292
  mov $0, %rax
  push %rax
  lea .L.str.2(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.292
.L.else.292:
.L.end.292:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.291
.L.do.end.291:
  lea loop_depth(%rip), %rax
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  sub $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.2:
  .byte 0x70
  .byte 0x6f
  .byte 0x70
  .byte 0x5f
  .byte 0x6c
  .byte 0x6f
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x6c
  .byte 0x6f
  .byte 0x6f
  .byte 0x70
  .byte 0x00
.text
.globl push_loop
.type  push_loop, @function
push_loop:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea loop_depth(%rip), %rax
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  add $8, %rsp
  leave
  ret
.bss
.globl loop_depth
loop_depth:
  .zero 4
.text
.globl in_switch
.type  in_switch, @function
in_switch:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea switch_depth(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl switch_record_case
.type  switch_record_case, @function
switch_record_case:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `case_`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `case_`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea switch_stack(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea switch_depth(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $8, %rax
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
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  lea switch_stack(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea switch_depth(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  mov $8, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  leave
  ret
.text
.globl pop_switch
.type  pop_switch, @function
pop_switch:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
.L.do.body.293:
  mov $0, %rax
  push %rax
  lea switch_depth(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.294
  mov $0, %rax
  push %rax
  lea .L.str.1(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.294
.L.else.294:
.L.end.294:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.293
.L.do.end.293:
  lea switch_depth(%rip), %rax
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  sub $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.1:
  .byte 0x70
  .byte 0x6f
  .byte 0x70
  .byte 0x5f
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x73
  .byte 0x69
  .byte 0x64
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x00
.text
.globl push_switch
.type  push_switch, @function
push_switch:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `head`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea switch_stack(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea switch_depth(%rip), %rax
  push %rax
  mov (%rsp), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  add $1, %rax
  pop %rdx
  pop %rsi
  push %rax
  push %rsi
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rdx
  mov $8, %rax
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  leave
  ret
.bss
.globl switch_stack
switch_stack:
  .zero 200
.bss
.globl switch_depth
switch_depth:
  .zero 4
.text
.globl scope_lookup
.type  scope_lookup, @function
scope_lookup:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.295:
  lea -24(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.295
  lea -24(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.296:
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.296
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.cond.false.297
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  jmp .L.cond.end.298
.L.cond.false.297:
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
.L.cond.end.298:
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  je .L.and.300
  lea -32(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym_name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strlen
  mov %rbp, %rsp
  pop %rbp
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
.L.and.300:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.299
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.299
.L.else.299:
.L.end.299:
.L.for.next.296:
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.296
.L.for.end.296:
.L.for.next.295:
  lea -24(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.295
.L.for.end.295:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl scope_lookup_type
.type  scope_lookup_type, @function
scope_lookup_type:
  push %rbp
  movq %rsp, %rbp
  sub $28, %rsp
  push %rdi
  lea -28(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -20(%rbp), %rax # symbol ref lvalue `tls`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.301:
  lea -16(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.301
  lea -16(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.302:
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.302
  lea -20(%rbp), %rax # symbol ref lvalue `tls`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.304
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  setne %al
  movzb %al, %eax
.L.and.304:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.303
  jmp .L.for.next.302
  jmp .L.end.303
.L.else.303:
.L.end.303:
  lea -20(%rbp), %rax # symbol ref lvalue `tls`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $2, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.306
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $28, %rax
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
  setne %al
  movzb %al, %eax
.L.and.306:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.305
  jmp .L.for.next.302
  jmp .L.end.305
.L.else.305:
.L.end.305:
  lea -20(%rbp), %rax # symbol ref lvalue `tls`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $3, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.308
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
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
  setne %al
  movzb %al, %eax
.L.and.308:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.307
  jmp .L.for.next.302
  jmp .L.end.307
.L.else.307:
.L.end.307:
  lea -20(%rbp), %rax # symbol ref lvalue `tls`
  push %rax
  pop %rax
  mov (%rax), %eax
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
  je .L.and.310
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.311
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $28, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.312
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.312:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.311:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.310:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.309
  jmp .L.for.next.302
  jmp .L.end.309
.L.else.309:
.L.end.309:
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
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
  je .L.else.313
  jmp .L.for.next.302
  jmp .L.end.313
.L.else.313:
.L.end.313:
  lea -28(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  je .L.and.315
  lea -28(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strlen
  mov %rbp, %rsp
  pop %rbp
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
.L.and.315:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.314
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.314
.L.else.314:
.L.end.314:
.L.for.next.302:
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.302
.L.for.end.302:
.L.for.next.301:
  lea -16(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.301
.L.for.end.301:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl define_type
.type  define_type, @function
define_type:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_scope(%rip), %rax
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
  leave
  ret
.text
.globl define
.type  define, @function
define:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.and.317
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  setne %al
  movzb %al, %eax
.L.and.317:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.316
  mov $0, %rax
  push %rax
  lea .L.str.0(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.316
.L.else.316:
.L.end.316:
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.0:
  .byte 0x69
  .byte 0x6e
  .byte 0x73
  .byte 0x65
  .byte 0x72
  .byte 0x74
  .byte 0x69
  .byte 0x6e
  .byte 0x67
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x6c
  .byte 0x6f
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x20
  .byte 0x76
  .byte 0x61
  .byte 0x72
  .byte 0x69
  .byte 0x61
  .byte 0x62
  .byte 0x6c
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x65
  .byte 0x20
  .byte 0x67
  .byte 0x6c
  .byte 0x6f
  .byte 0x62
  .byte 0x61
  .byte 0x6c
  .byte 0x20
  .byte 0x73
  .byte 0x63
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x00
.text
.globl pop_scope
.type  pop_scope, @function
pop_scope:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_scope(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  leave
  ret
.text
.globl push_scope
.type  push_scope, @function
push_scope:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  mov $32, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  mov $0, %rax
  push %rax
  mov $32, %rax
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
  lea current_scope(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `s`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea current_scope(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  leave
  ret
.bss
.globl global_scope
global_scope:
  .zero 8
.bss
.globl current_scope
current_scope:
  .zero 8
.bss
.globl symbols
symbols:
  .zero 8
.bss
.globl current_function
current_function:
  .zero 8
