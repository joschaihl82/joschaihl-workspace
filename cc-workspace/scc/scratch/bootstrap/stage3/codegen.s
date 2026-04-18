.text
.globl codegen
.type  codegen, @function
codegen:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `symbols`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `symbols`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.49:
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.49
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
  pop %rax
  cmp $3, %rax
  je .L.case.44
  cmp $2, %rax
  je .L.case.45
  cmp $1, %rax
  je .L.case.46
  cmp $4, %rax
  je .L.case.47
  cmp $0, %rax
  je .L.case.48
  jmp .L.switch.end.50
.L.case.44:
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_function
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.50
.L.case.45:
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_global
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.50
.L.case.46:
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  lea .L.str.236(%rip), %rax
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
  jmp .L.switch.end.50
.L.case.47:
.L.case.48:
  jmp .L.switch.end.50
.L.switch.end.50:
.L.for.next.49:
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
  jmp .L.for.header.49
.L.for.end.49:
  leave
  ret
.section .rodata
.L.str.236:
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
  .byte 0x20
  .byte 0x67
  .byte 0x6c
  .byte 0x6f
  .byte 0x62
  .byte 0x61
  .byte 0x6c
  .byte 0x20
  .byte 0x73
  .byte 0x79
  .byte 0x6d
  .byte 0x62
  .byte 0x6f
  .byte 0x6c
  .byte 0x73
  .byte 0x20
  .byte 0x6c
  .byte 0x69
  .byte 0x73
  .byte 0x74
  .byte 0x3f
  .byte 0x00
.text
.globl codegen_global
.type  codegen_global, @function
codegen_global:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  je .L.else.51
  leave
  ret
  jmp .L.end.51
.L.else.51:
.L.end.51:
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.52
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  lea .L.str.227(%rip), %rax
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
  jmp .L.end.52
.L.else.52:
.L.end.52:
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $64, %rax
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
  je .L.else.53
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_global_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.53
.L.else.53:
.L.end.53:
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $64, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `data`
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
  je .L.else.54
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.228(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.229(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.230(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.231(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.54
.L.else.54:
.L.end.54:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.232(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.233(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.55:
  lea -8(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `data`
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
  setl %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.55
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.234(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `data`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `i`
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
  pop %rax
  movzbl (%rax), %eax
  cltq
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.55:
  lea -8(%rbp), %rax # symbol ref lvalue `i`
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
  jmp .L.for.header.55
.L.for.end.55:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.235(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.235:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x62
  .byte 0x79
  .byte 0x74
  .byte 0x65
  .byte 0x20
  .byte 0x30
  .byte 0x78
  .byte 0x30
  .byte 0x30
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.234:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x62
  .byte 0x79
  .byte 0x74
  .byte 0x65
  .byte 0x20
  .byte 0x30
  .byte 0x78
  .byte 0x25
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.233:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.232:
  .byte 0x2e
  .byte 0x73
  .byte 0x65
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x2e
  .byte 0x72
  .byte 0x6f
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.231:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x6f
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.230:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.229:
  .byte 0x2e
  .byte 0x67
  .byte 0x6c
  .byte 0x6f
  .byte 0x62
  .byte 0x6c
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.228:
  .byte 0x2e
  .byte 0x62
  .byte 0x73
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.227:
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x6b
  .byte 0x6e
  .byte 0x6f
  .byte 0x77
  .byte 0x6e
  .byte 0x00
.text
.globl codegen_global_initializer
.type  codegen_global_initializer, @function
codegen_global_initializer:
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
  mov $0, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.56
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.220(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.221(%rip), %rax
  push %rax
  pop %rax
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
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.222(%rip), %rax
  push %rax
  pop %rax
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
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.223(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.56
.L.else.56:
.L.end.56:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.224(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.225(%rip), %rax
  push %rax
  pop %rax
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
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.226(%rip), %rax
  push %rax
  pop %rax
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
  mov (%rax), %rax
  push %rax
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.226:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.225:
  .byte 0x2e
  .byte 0x67
  .byte 0x6c
  .byte 0x6f
  .byte 0x62
  .byte 0x6c
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.224:
  .byte 0x2e
  .byte 0x64
  .byte 0x61
  .byte 0x74
  .byte 0x61
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.223:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x6f
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.222:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.221:
  .byte 0x2e
  .byte 0x67
  .byte 0x6c
  .byte 0x6f
  .byte 0x62
  .byte 0x6c
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.220:
  .byte 0x2e
  .byte 0x62
  .byte 0x73
  .byte 0x73
  .byte 0xa
  .byte 0x00
.text
.globl codegen_initializer
.type  codegen_initializer, @function
codegen_initializer:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `init`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.else.57
  lea -16(%rbp), %rax # symbol ref lvalue `init`
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
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  call codegen_aggregate_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.57
.L.else.57:
  lea -16(%rbp), %rax # symbol ref lvalue `init`
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
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  call codegen_scalar_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.57:
  leave
  ret
.text
.globl codegen_aggregate_initializer
.type  codegen_aggregate_initializer, @function
codegen_aggregate_initializer:
  push %rbp
  movq %rsp, %rbp
  sub $48, %rsp
  push %rdi
  lea -48(%rbp), %rax # symbol ref lvalue `inits`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.else.58
  lea -48(%rbp), %rax # symbol ref lvalue `inits`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.59:
  lea -32(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.59
  lea -32(%rbp), %rax # symbol ref lvalue `i`
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
  call codegen_initializer
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.59:
  lea -32(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.59
.L.for.end.59:
  leave
  ret
  jmp .L.end.58
.L.else.58:
.L.end.58:
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.else.60
  mov $0, %rax
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -20(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -48(%rbp), %rax # symbol ref lvalue `inits`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.61:
  lea -12(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.61
  mov $0, %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
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
  je .L.else.62
  jmp .L.for.end.61
  jmp .L.end.62
.L.else.62:
.L.end.62:
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
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
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
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
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  lea -4(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -4(%rbp), %rax # symbol ref lvalue `padding`
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
  je .L.else.63
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.218(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `padding`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.63
.L.else.63:
.L.end.63:
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
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
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
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
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
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
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
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
  call codegen_initializer
  mov %rbp, %rsp
  pop %rbp
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
  lea -20(%rbp), %rax # symbol ref lvalue `cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.61:
  lea -12(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.61
.L.for.end.61:
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
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
  setl %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.64
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.219(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -40(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -24(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.64
.L.else.64:
.L.end.64:
  leave
  ret
  jmp .L.end.60
.L.else.60:
.L.end.60:
  leave
  ret
.section .rodata
.L.str.219:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x6f
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.218:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x7a
  .byte 0x65
  .byte 0x72
  .byte 0x6f
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.text
.globl codegen_scalar_initializer
.type  codegen_scalar_initializer, @function
codegen_scalar_initializer:
  push %rbp
  movq %rsp, %rbp
  sub $36, %rsp
  push %rdi
  lea -36(%rbp), %rax # symbol ref lvalue `value`
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
  lea -28(%rbp), %rax # symbol ref lvalue `ty`
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
  lea ty_char(%rip), %rax
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
  je .L.else.65
.L.do.body.66:
  mov $0, %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `value`
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
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.67
  lea -36(%rbp), %rax # symbol ref lvalue `value`
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
  lea .L.str.212(%rip), %rax
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
  jmp .L.end.67
.L.else.67:
.L.end.67:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.66
.L.do.end.66:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
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
  lea -20(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.213(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `strsym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.65
.L.else.65:
.L.end.65:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
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
  je .L.and.69
  lea -36(%rbp), %rax # symbol ref lvalue `value`
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
  setne %al
  movzb %al, %eax
.L.and.69:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.68
  lea -36(%rbp), %rax # symbol ref lvalue `value`
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
  add $32, %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.214(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.68
.L.else.68:
.L.end.68:
  lea -36(%rbp), %rax # symbol ref lvalue `value`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call consteval
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -4(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -28(%rbp), %rax # symbol ref lvalue `ty`
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
  pop %rax
  cmp $4, %rax
  je .L.case.41
  cmp $8, %rax
  je .L.case.42
  jmp .L.default.43
.L.case.41:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.215(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `scalar_value`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.70
.L.case.42:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.216(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `scalar_value`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.70
.L.default.43:
  mov $0, %rax
  push %rax
  lea .L.str.217(%rip), %rax
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
.L.switch.end.70:
  leave
  ret
.section .rodata
.L.str.217:
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
  .byte 0x73
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x67
  .byte 0x6c
  .byte 0x6f
  .byte 0x62
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
  .byte 0x00
.section .rodata
.L.str.216:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x71
  .byte 0x75
  .byte 0x61
  .byte 0x64
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.215:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x6c
  .byte 0x6f
  .byte 0x6e
  .byte 0x67
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.214:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x71
  .byte 0x75
  .byte 0x61
  .byte 0x64
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.213:
  .byte 0x20
  .byte 0x20
  .byte 0x2e
  .byte 0x71
  .byte 0x75
  .byte 0x61
  .byte 0x64
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.212:
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x2d
  .byte 0x73
  .byte 0x79
  .byte 0x6d
  .byte 0x62
  .byte 0x6f
  .byte 0x6c
  .byte 0x20
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
  .byte 0x72
  .byte 0x65
  .byte 0x66
  .byte 0x00
.text
.globl codegen_function
.type  codegen_function, @function
codegen_function:
  push %rbp
  movq %rsp, %rbp
  sub $12, %rsp
  push %rdi
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
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
  je .L.else.71
  leave
  ret
  jmp .L.end.71
.L.else.71:
.L.end.71:
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call calculate_frame_layout
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -4(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.203(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.204(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.205(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.206(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.207(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.208(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.209(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `offset`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `sym`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.210(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.211(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.211:
  .byte 0x20
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x74
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.210:
  .byte 0x20
  .byte 0x20
  .byte 0x6c
  .byte 0x65
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.209:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x73
  .byte 0x70
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.208:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x71
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x73
  .byte 0x70
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.207:
  .byte 0x20
  .byte 0x20
  .byte 0x70
  .byte 0x75
  .byte 0x73
  .byte 0x68
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.206:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.205:
  .byte 0x2e
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x20
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0x2c
  .byte 0x20
  .byte 0x40
  .byte 0x66
  .byte 0x75
  .byte 0x6e
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.204:
  .byte 0x2e
  .byte 0x67
  .byte 0x6c
  .byte 0x6f
  .byte 0x62
  .byte 0x6c
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.203:
  .byte 0x2e
  .byte 0x74
  .byte 0x65
  .byte 0x78
  .byte 0x74
  .byte 0xa
  .byte 0x00
.text
.globl calculate_frame_layout
.type  calculate_frame_layout, @function
calculate_frame_layout:
  push %rbp
  movq %rsp, %rbp
  sub $20, %rsp
  push %rdi
  lea -20(%rbp), %rax # symbol ref lvalue `func`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -12(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -20(%rbp), %rax # symbol ref lvalue `func`
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
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.72:
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.72
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.73
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  lea .L.str.202(%rip), %rax
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
  jmp .L.end.73
.L.else.73:
.L.end.73:
  lea -12(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
  add $16, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `sym`
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
.L.for.next.72:
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
  jmp .L.for.header.72
.L.for.end.72:
  mov $0, %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.202:
  .byte 0x73
  .byte 0x74
  .byte 0x6f
  .byte 0x72
  .byte 0x61
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x6b
  .byte 0x6e
  .byte 0x6f
  .byte 0x77
  .byte 0x6e
  .byte 0x00
.text
.globl codegen_stmt
.type  codegen_stmt, @function
codegen_stmt:
  push %rbp
  movq %rsp, %rbp
  sub $104, %rsp
  push %rdi
  lea -104(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -104(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -96(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.74:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.74
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  cmp $26, %rax
  je .L.case.29
  cmp $27, %rax
  je .L.case.30
  cmp $28, %rax
  je .L.case.31
  cmp $23, %rax
  je .L.case.32
  cmp $24, %rax
  je .L.case.33
  cmp $25, %rax
  je .L.case.34
  cmp $11, %rax
  je .L.case.35
  cmp $12, %rax
  je .L.case.36
  cmp $20, %rax
  je .L.case.37
  cmp $21, %rax
  je .L.case.38
  cmp $22, %rax
  je .L.case.39
  jmp .L.default.40
.L.case.29:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.76
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.164(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.76
.L.else.76:
.L.end.76:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.165(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.166(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.30:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_void
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.31:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  lea -88(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.77:
  lea -88(%rbp), %rax # symbol ref lvalue `stmt`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.77
  lea -88(%rbp), %rax # symbol ref lvalue `stmt`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.77:
  lea -88(%rbp), %rax # symbol ref lvalue `stmt`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -88(%rbp), %rax # symbol ref lvalue `stmt`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.77
.L.for.end.77:
  jmp .L.switch.end.75
.L.case.32:
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -80(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.167(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.168(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.169(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `label_count`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.170(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `label_count`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.171(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `label_count`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.78
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.78
.L.else.78:
.L.end.78:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.172(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -80(%rbp), %rax # symbol ref lvalue `label_count`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.33:
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -76(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea .L.str.173(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `label_count`
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
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -72(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea .L.str.174(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `label_count`
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
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -64(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea .L.str.175(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `label_count`
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
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -56(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.79
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.79
.L.else.79:
.L.end.79:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.176(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `header`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.80
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.177(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.178(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.179(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `end`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.80
.L.else.80:
.L.end.80:
  lea -56(%rbp), %rax # symbol ref lvalue `end`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `next`
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
  call push_loop_labels
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_loop_labels
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.180(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `next`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.81
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_void
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.81
.L.else.81:
.L.end.81:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.181(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `header`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.182(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `end`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.34:
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea .L.str.183(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `label_count`
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
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -44(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea .L.str.184(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `label_count`
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
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.185(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `header`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -36(%rbp), %rax # symbol ref lvalue `end`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `header`
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
  call push_loop_labels
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_loop_labels
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.186(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.187(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.188(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `label_count`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.189(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `end`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.35:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.190(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call break_target
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.36:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.191(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call continue_target
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.37:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.192(%rip), %rax
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
  lea -28(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -28(%rbp), %rax # symbol ref lvalue `end`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_break
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.193(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -20(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.82:
  lea -12(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.82
.L.do.body.83:
  mov $0, %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `c`
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
  add $8, %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $21, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
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
  je .L.else.84
  lea -12(%rbp), %rax # symbol ref lvalue `c`
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
  lea .L.str.194(%rip), %rax
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
  jmp .L.end.84
.L.else.84:
.L.end.84:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.83
.L.do.end.83:
  mov $0, %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `c`
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
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.85
  lea -20(%rbp), %rax # symbol ref lvalue `default_case`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.86
  lea -20(%rbp), %rax # symbol ref lvalue `default_case`
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
  lea .L.str.195(%rip), %rax
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
.L.end.86:
  lea -12(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `default_case`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.next.82
  jmp .L.end.85
.L.else.85:
.L.end.85:
  lea -12(%rbp), %rax # symbol ref lvalue `c`
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
  call consteval
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -4(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.196(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `cond_val`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.197(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `c`
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
  add $32, %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.82:
  lea -12(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `c`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.82
.L.for.end.82:
  lea -20(%rbp), %rax # symbol ref lvalue `default_case`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.87
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.198(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `default_case`
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
  add $32, %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.87
.L.else.87:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.199(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `end`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.87:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_stmt
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_break
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.200(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `end`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.38:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.201(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.case.39:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_local_initialization
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.75
.L.default.40:
  jmp .L.switch.end.75
.L.switch.end.75:
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call assert_stack_empty
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.74:
  lea -96(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -96(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.74
.L.for.end.74:
  leave
  ret
.section .rodata
.L.str.201:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.200:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.199:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.198:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.197:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.196:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.195:
  .byte 0x6d
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x69
  .byte 0x70
  .byte 0x6c
  .byte 0x65
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x66
  .byte 0x61
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x20
  .byte 0x63
  .byte 0x61
  .byte 0x73
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x00
.section .rodata
.L.str.194:
  .byte 0x63
  .byte 0x61
  .byte 0x73
  .byte 0x65
  .byte 0x20
  .byte 0x6c
  .byte 0x61
  .byte 0x62
  .byte 0x65
  .byte 0x6c
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
  .byte 0x6c
  .byte 0x61
  .byte 0x62
  .byte 0x65
  .byte 0x6c
  .byte 0x3f
  .byte 0x00
.section .rodata
.L.str.193:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.192:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x73
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x63
  .byte 0x68
  .byte 0x2e
  .byte 0x65
  .byte 0x6e
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.191:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.190:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.189:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.188:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x64
  .byte 0x6f
  .byte 0x2e
  .byte 0x62
  .byte 0x6f
  .byte 0x64
  .byte 0x79
  .byte 0x2e
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.187:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.186:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.185:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.184:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x64
  .byte 0x6f
  .byte 0x2e
  .byte 0x65
  .byte 0x6e
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.183:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x64
  .byte 0x6f
  .byte 0x2e
  .byte 0x62
  .byte 0x6f
  .byte 0x64
  .byte 0x79
  .byte 0x00
.section .rodata
.L.str.182:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.181:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.180:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.179:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.178:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.177:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.176:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.175:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x2e
  .byte 0x65
  .byte 0x6e
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.174:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x2e
  .byte 0x6e
  .byte 0x65
  .byte 0x78
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.173:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x2e
  .byte 0x68
  .byte 0x65
  .byte 0x61
  .byte 0x64
  .byte 0x65
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.172:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x65
  .byte 0x6e
  .byte 0x64
  .byte 0x2e
  .byte 0x25
  .byte 0x64
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.171:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x65
  .byte 0x6c
  .byte 0x73
  .byte 0x65
  .byte 0x2e
  .byte 0x25
  .byte 0x64
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.170:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x65
  .byte 0x6e
  .byte 0x64
  .byte 0x2e
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.169:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x65
  .byte 0x20
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x65
  .byte 0x6c
  .byte 0x73
  .byte 0x65
  .byte 0x2e
  .byte 0x25
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.168:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.167:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.166:
  .byte 0x20
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x74
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.165:
  .byte 0x20
  .byte 0x20
  .byte 0x6c
  .byte 0x65
  .byte 0x61
  .byte 0x76
  .byte 0x65
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.164:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.text
.globl codegen_expr
.type  codegen_expr, @function
codegen_expr:
  push %rbp
  movq %rsp, %rbp
  sub $68, %rsp
  push %rdi
  lea -68(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.88
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.88(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.89(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.88
.L.else.88:
.L.end.88:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.89
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.90(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.91(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.case.13
  cmp $1, %rax
  je .L.case.14
  cmp $2, %rax
  je .L.case.15
  cmp $3, %rax
  je .L.case.16
  cmp $4, %rax
  je .L.case.17
  cmp $11, %rax
  je .L.case.18
  cmp $12, %rax
  je .L.case.19
  cmp $5, %rax
  je .L.case.20
  cmp $6, %rax
  je .L.case.21
  cmp $7, %rax
  je .L.case.22
  cmp $8, %rax
  je .L.case.23
  cmp $9, %rax
  je .L.case.24
  cmp $10, %rax
  je .L.case.25
  jmp .L.switch.end.90
.L.case.13:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.92(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.93(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.90
.L.case.14:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.94(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.95(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.90
.L.case.15:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.96(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.97(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.90
.L.case.16:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.98(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.99(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.100(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.90
.L.case.17:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.101(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.102(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.103(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.90
.L.case.18:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.104(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.105(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.90
.L.case.19:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.106(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.107(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.90
.L.case.20:
.L.case.21:
.L.case.22:
.L.case.23:
.L.case.24:
.L.case.25:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.108(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.91
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.109(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.91
.L.else.91:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.92
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.110(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.92
.L.else.92:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.93
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.111(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.93
.L.else.93:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.94
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.112(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.94
.L.else.94:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $9, %rax
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
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.113(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.95
.L.else.95:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $10, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.96
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.114(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.96
.L.else.96:
.L.end.96:
.L.end.95:
.L.end.94:
.L.end.93:
.L.end.92:
.L.end.91:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.115(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.116(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.switch.end.90:
  jmp .L.end.89
.L.else.89:
.L.end.89:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.97
  mov $0, %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.98
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  lea .L.str.117(%rip), %rax
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
  jmp .L.end.98
.L.else.98:
.L.end.98:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_lvalue_addr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call load
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.118(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.97
.L.else.97:
.L.end.97:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.99
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_lvalue_addr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call store
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.119(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.99
.L.else.99:
.L.end.99:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.100
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_lvalue_addr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.100
.L.else.100:
.L.end.100:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.101
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call load
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.120(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.101
.L.else.101:
.L.end.101:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.102
  mov $0, %rax
  push %rax
  lea -60(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.103:
  lea -56(%rbp), %rax # symbol ref lvalue `arg`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.103
  lea -56(%rbp), %rax # symbol ref lvalue `arg`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -60(%rbp), %rax # symbol ref lvalue `num_args`
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
.L.for.next.103:
  lea -56(%rbp), %rax # symbol ref lvalue `arg`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `arg`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.103
.L.for.end.103:
  lea -60(%rbp), %rax # symbol ref lvalue `num_args`
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
  setg %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.104
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  lea .L.str.121(%rip), %rax
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
  jmp .L.end.104
.L.else.104:
.L.end.104:
.L.for.header.105:
  lea -60(%rbp), %rax # symbol ref lvalue `num_args`
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
  je .L.for.end.105
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -60(%rbp), %rax # symbol ref lvalue `num_args`
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
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.for.next.105:
  jmp .L.for.header.105
.L.for.end.105:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call emit_call
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.102
.L.else.102:
.L.end.102:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.106
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea .L.str.122(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `label_idx`
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
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -44(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.123(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.124(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.125(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.126(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.127(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.128(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.129(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.130(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.131(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.106
.L.else.106:
.L.end.106:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $9, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.107
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call gen_label
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -36(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea .L.str.132(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `label_idx`
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
  call gen_label_name
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -32(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.133(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.134(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.135(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.136(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.137(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.138(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.139(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.140(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.141(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.107
.L.else.107:
.L.end.107:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $10, %rax
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
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.142(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  cmp $1, %rax
  je .L.case.26
  cmp $4, %rax
  je .L.case.27
  jmp .L.default.28
.L.case.26:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.143(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.109
.L.case.27:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.144(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.109
.L.default.28:
  jmp .L.switch.end.109
.L.switch.end.109:
  lea .L.str.145(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.108
.L.else.108:
.L.end.108:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $13, %rax
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
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $5, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setg %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.111
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  lea .L.str.146(%rip), %rax
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
  jmp .L.end.111
.L.else.111:
.L.end.111:
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -24(%rbp), %rax # symbol ref lvalue `arg`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.110
.L.else.110:
.L.end.110:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $14, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.or.113
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $15, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
.L.or.113:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.112
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_lvalue_addr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stack_dup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call load
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.147(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $14, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.114
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.148(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.114
.L.else.114:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.149(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.114:
  lea .L.str.150(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.151(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.152(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.153(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call store
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.154(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.112
.L.else.112:
.L.end.112:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  jne .L.or.116
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
.L.or.116:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.115
  lea -68(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_lvalue_addr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call load
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.155(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.115
.L.else.115:
.L.end.115:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $19, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.117
  lea .L.str.156(%rip), %rax
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
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea .L.str.157(%rip), %rax
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
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.158(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.159(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.160(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `false_label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.161(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `end_label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.162(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `false_label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.163(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `end_label`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea depth(%rip), %rax
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
  jmp .L.end.117
.L.else.117:
.L.end.117:
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  mov $18, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.118
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_void
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.118
.L.else.118:
.L.end.118:
  leave
  ret
.section .rodata
.L.str.163:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.162:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.161:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.160:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.159:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.158:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.157:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x64
  .byte 0x2e
  .byte 0x65
  .byte 0x6e
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.156:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x64
  .byte 0x2e
  .byte 0x66
  .byte 0x61
  .byte 0x6c
  .byte 0x73
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.155:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.154:
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.153:
  .byte 0x72
  .byte 0x73
  .byte 0x69
  .byte 0x00
.section .rodata
.L.str.152:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.151:
  .byte 0x72
  .byte 0x73
  .byte 0x69
  .byte 0x00
.section .rodata
.L.str.150:
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.149:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x20
  .byte 0x24
  .byte 0x31
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.148:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x24
  .byte 0x31
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.147:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.146:
  .byte 0x67
  .byte 0x72
  .byte 0x65
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x72
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x61
  .byte 0x6e
  .byte 0x20
  .byte 0x36
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
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x63
  .byte 0x75
  .byte 0x72
  .byte 0x72
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x6c
  .byte 0x79
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
.L.str.145:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.144:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x74
  .byte 0x71
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.143:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x73
  .byte 0x78
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x65
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.142:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.141:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.140:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x7a
  .byte 0x62
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x65
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.139:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.138:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.137:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.136:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.135:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.134:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.133:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.132:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x6f
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.131:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.130:
  .byte 0x25
  .byte 0x73
  .byte 0x3a
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.129:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x7a
  .byte 0x62
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x65
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.128:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.127:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.126:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.125:
  .byte 0x20
  .byte 0x20
  .byte 0x6a
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.124:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.123:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.122:
  .byte 0x2e
  .byte 0x4c
  .byte 0x2e
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.121:
  .byte 0x66
  .byte 0x75
  .byte 0x6e
  .byte 0x63
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x73
  .byte 0x20
  .byte 0x77
  .byte 0x69
  .byte 0x74
  .byte 0x68
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x69
  .byte 0x74
  .byte 0x79
  .byte 0x20
  .byte 0x3e
  .byte 0x20
  .byte 0x36
  .byte 0x20
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
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
.L.str.120:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.119:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.118:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.117:
  .byte 0x75
  .byte 0x6e
  .byte 0x62
  .byte 0x6f
  .byte 0x75
  .byte 0x6e
  .byte 0x64
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
  .byte 0x00
.section .rodata
.L.str.116:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.115:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x7a
  .byte 0x62
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.114:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x67
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.113:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x67
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.112:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x6c
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.111:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x6c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.110:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x6e
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.109:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.108:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6d
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.107:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.106:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.105:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.104:
  .byte 0x20
  .byte 0x20
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.103:
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.102:
  .byte 0x20
  .byte 0x20
  .byte 0x69
  .byte 0x64
  .byte 0x69
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.101:
  .byte 0x20
  .byte 0x20
  .byte 0x78
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.100:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.99:
  .byte 0x20
  .byte 0x20
  .byte 0x69
  .byte 0x64
  .byte 0x69
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.98:
  .byte 0x20
  .byte 0x20
  .byte 0x78
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.97:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.96:
  .byte 0x20
  .byte 0x20
  .byte 0x69
  .byte 0x6d
  .byte 0x75
  .byte 0x6c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.95:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.94:
  .byte 0x20
  .byte 0x20
  .byte 0x73
  .byte 0x75
  .byte 0x62
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.93:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.92:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.91:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.90:
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x00
.section .rodata
.L.str.89:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.88:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.text
.globl codegen_lvalue_addr
.type  codegen_lvalue_addr, @function
codegen_lvalue_addr:
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
  pop %rax
  cmp $4, %rax
  je .L.case.8
  cmp $6, %rax
  je .L.case.9
  cmp $16, %rax
  je .L.case.10
  cmp $17, %rax
  je .L.case.11
  jmp .L.default.12
.L.case.8:
  mov $0, %rax
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
  je .L.else.120
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
  lea .L.str.77(%rip), %rax
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
  jmp .L.end.120
.L.else.120:
.L.end.120:
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
  pop %rax
  add $16, %rax
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
  je .L.else.121
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.78(%rip), %rax
  push %rax
  pop %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.121
.L.else.121:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.79(%rip), %rax
  push %rax
  pop %rax
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
  mov (%rax), %rax
  push %rax
  pop %rax
  add $72, %rax
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
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $24, %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.121:
  lea .L.str.80(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.119
.L.case.9:
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.119
.L.case.10:
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
  call codegen_lvalue_addr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
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
  je .L.else.122
  lea .L.str.81(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.82(%rip), %rax
  push %rax
  pop %rax
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
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.83(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.122
.L.else.122:
.L.end.122:
  jmp .L.switch.end.119
.L.case.11:
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
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
  je .L.else.123
  lea .L.str.84(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.85(%rip), %rax
  push %rax
  pop %rax
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
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.86(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.123
.L.else.123:
.L.end.123:
  jmp .L.switch.end.119
.L.default.12:
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
  lea .L.str.87(%rip), %rax
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
  jmp .L.switch.end.119
.L.switch.end.119:
  leave
  ret
.section .rodata
.L.str.87:
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
.L.str.86:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.85:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.84:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.83:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.82:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.81:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.80:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.79:
  .byte 0x20
  .byte 0x20
  .byte 0x6c
  .byte 0x65
  .byte 0x61
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x20
  .byte 0x23
  .byte 0x20
  .byte 0x73
  .byte 0x79
  .byte 0x6d
  .byte 0x62
  .byte 0x6f
  .byte 0x6c
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x66
  .byte 0x20
  .byte 0x6c
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x60
  .byte 0x25
  .byte 0x73
  .byte 0x60
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.78:
  .byte 0x20
  .byte 0x20
  .byte 0x6c
  .byte 0x65
  .byte 0x61
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x69
  .byte 0x70
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.77:
  .byte 0x75
  .byte 0x6e
  .byte 0x62
  .byte 0x6f
  .byte 0x75
  .byte 0x6e
  .byte 0x64
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
  .byte 0x00
.text
.globl codegen_local_initialization
.type  codegen_local_initialization, @function
codegen_local_initialization:
  push %rbp
  movq %rsp, %rbp
  sub $72, %rsp
  push %rdi
  lea -72(%rbp), %rax # symbol ref lvalue `n`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.do.body.124:
  mov $0, %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  mov $22, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
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
  je .L.else.125
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.125
.L.else.125:
.L.end.125:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.124
.L.do.end.124:
.L.do.body.126:
  mov $0, %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.127
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.127
.L.else.127:
.L.end.127:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.126
.L.do.end.126:
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  lea -64(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -64(%rbp), %rax # symbol ref lvalue `initialized_symbol`
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
  lea -56(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.and.129
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
.L.and.129:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.128
  mov $0, %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  je .L.else.130
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.51(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.52(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.130
.L.else.130:
.L.do.body.131:
  mov $0, %rax
  push %rax
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  mov $0, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
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
  je .L.else.132
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  lea .L.str.53(%rip), %rax
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
  jmp .L.end.132
.L.else.132:
.L.end.132:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.131
.L.do.end.131:
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.130:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.54(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `initialized_symbol`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.55(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  call store
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.128
.L.else.128:
.L.end.128:
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.else.133
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.56(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `initialized_symbol`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.57(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.58(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $1, %rax
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
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.59(%rip), %rax
  push %rax
  pop %rax
  push %rax
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
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $2, %rax
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
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.60(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call emit_call
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea -48(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -44(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.do.body.134:
  mov $0, %rax
  push %rax
  lea -44(%rbp), %rax # symbol ref lvalue `agg_init`
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
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.135
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.135
.L.else.135:
.L.end.135:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.134
.L.do.end.134:
  lea -44(%rbp), %rax # symbol ref lvalue `agg_init`
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
  lea -36(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.136:
  lea -36(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.136
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stack_dup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -36(%rbp), %rax # symbol ref lvalue `i`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stack_swap
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -48(%rbp), %rax # symbol ref lvalue `offset`
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
  je .L.else.137
  lea .L.str.62(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.63(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `offset`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.64(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.137
.L.else.137:
.L.end.137:
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
  call store
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -48(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
  pop %rdi
  pop %rax
  add %rdi, %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.136:
  lea -36(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.136
.L.for.end.136:
  lea .L.str.65(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.133
.L.else.133:
.L.end.133:
  lea -56(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.else.138
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.66(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -64(%rbp), %rax # symbol ref lvalue `initialized_symbol`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.67(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.68(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $1, %rax
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
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.69(%rip), %rax
  push %rax
  pop %rax
  push %rax
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
  lea argument_regs(%rip), %rax
  push %rax
  pop %rax
  push %rax
  mov $2, %rax
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
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.70(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call emit_call
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
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
  lea -28(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -20(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.do.body.139:
  mov $0, %rax
  push %rax
  lea -20(%rbp), %rax # symbol ref lvalue `agg_init`
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
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.140
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  jmp .L.end.140
.L.else.140:
.L.end.140:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.139
.L.do.end.139:
  lea -20(%rbp), %rax # symbol ref lvalue `agg_init`
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
.L.for.header.141:
  lea -12(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.141
  mov $0, %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `field_cursor`
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
  je .L.else.142
  jmp .L.for.next.141
  jmp .L.end.142
.L.else.142:
.L.end.142:
  lea -28(%rbp), %rax # symbol ref lvalue `field_cursor`
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
  lea -4(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stack_dup
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `i`
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
  call codegen_expr
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call stack_swap
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -4(%rbp), %rax # symbol ref lvalue `offset`
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
  je .L.else.143
  lea .L.str.72(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.73(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `offset`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.74(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.143
.L.else.143:
.L.end.143:
  lea -28(%rbp), %rax # symbol ref lvalue `field_cursor`
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
  call store
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -28(%rbp), %rax # symbol ref lvalue `field_cursor`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `field_cursor`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.for.next.141:
  lea -12(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.141
.L.for.end.141:
  lea .L.str.75(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.138
.L.else.138:
.L.end.138:
  lea -72(%rbp), %rax # symbol ref lvalue `n`
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.76:
  .byte 0x6e
  .byte 0x79
  .byte 0x69
  .byte 0x3a
  .byte 0x20
  .byte 0x73
  .byte 0x6f
  .byte 0x6d
  .byte 0x65
  .byte 0x20
  .byte 0x77
  .byte 0x65
  .byte 0x69
  .byte 0x72
  .byte 0x64
  .byte 0x20
  .byte 0x6b
  .byte 0x69
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x6f
  .byte 0x66
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
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.75:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.74:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.73:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.72:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.71:
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x2d
  .byte 0x61
  .byte 0x67
  .byte 0x67
  .byte 0x72
  .byte 0x65
  .byte 0x67
  .byte 0x61
  .byte 0x74
  .byte 0x65
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
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x61
  .byte 0x67
  .byte 0x67
  .byte 0x72
  .byte 0x65
  .byte 0x67
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.70:
  .byte 0x6d
  .byte 0x65
  .byte 0x6d
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.69:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.68:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.67:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.66:
  .byte 0x20
  .byte 0x20
  .byte 0x6c
  .byte 0x65
  .byte 0x61
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.65:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.64:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.63:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.62:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.61:
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x2d
  .byte 0x61
  .byte 0x67
  .byte 0x67
  .byte 0x72
  .byte 0x65
  .byte 0x67
  .byte 0x61
  .byte 0x74
  .byte 0x65
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
  .byte 0x6f
  .byte 0x66
  .byte 0x20
  .byte 0x61
  .byte 0x67
  .byte 0x67
  .byte 0x72
  .byte 0x65
  .byte 0x67
  .byte 0x61
  .byte 0x74
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.60:
  .byte 0x6d
  .byte 0x65
  .byte 0x6d
  .byte 0x73
  .byte 0x65
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.59:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x24
  .byte 0x25
  .byte 0x64
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.58:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.57:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.56:
  .byte 0x20
  .byte 0x20
  .byte 0x6c
  .byte 0x65
  .byte 0x61
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.55:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.54:
  .byte 0x20
  .byte 0x20
  .byte 0x6c
  .byte 0x65
  .byte 0x61
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.53:
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
  .byte 0x73
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.52:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.51:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.50:
  .byte 0x4e
  .byte 0x59
  .byte 0x49
  .byte 0x3a
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x2d
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
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x00
.section .rodata
.L.str.49:
  .byte 0x63
  .byte 0x6f
  .byte 0x64
  .byte 0x65
  .byte 0x67
  .byte 0x65
  .byte 0x6e
  .byte 0x5f
  .byte 0x6c
  .byte 0x6f
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x5f
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x69
  .byte 0x61
  .byte 0x6c
  .byte 0x69
  .byte 0x7a
  .byte 0x61
  .byte 0x74
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x6e
  .byte 0x2d
  .byte 0x69
  .byte 0x6e
  .byte 0x69
  .byte 0x74
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x64
  .byte 0x65
  .byte 0x00
.text
.globl consteval
.type  consteval, @function
consteval:
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
  je .L.else.144
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
  jmp .L.end.144
.L.else.144:
.L.end.144:
  lea -8(%rbp), %rax # symbol ref lvalue `n`
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
  leave
  ret
  leave
  ret
.section .rodata
.L.str.48:
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x61
  .byte 0x20
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x6e
  .byte 0x74
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
  .byte 0x61
  .byte 0x73
  .byte 0x20
  .byte 0x63
  .byte 0x75
  .byte 0x72
  .byte 0x72
  .byte 0x65
  .byte 0x6e
  .byte 0x74
  .byte 0x6c
  .byte 0x79
  .byte 0x20
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
  .byte 0x62
  .byte 0x79
  .byte 0x20
  .byte 0x74
  .byte 0x68
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x63
  .byte 0x6f
  .byte 0x6d
  .byte 0x70
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x72
  .byte 0x00
.text
.globl emit_call
.type  emit_call, @function
emit_call:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `function`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.41(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.44(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `function`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.46(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.47(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.47:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.46:
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0x00
.section .rodata
.L.str.45:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x73
  .byte 0x70
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.44:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x61
  .byte 0x6c
  .byte 0x6c
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.43:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x6e
  .byte 0x64
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x78
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x46
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x73
  .byte 0x70
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.42:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x73
  .byte 0x70
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.41:
  .byte 0x72
  .byte 0x62
  .byte 0x70
  .byte 0x00
.section .rodata
.L.str.40:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x24
  .byte 0x30
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0xa
  .byte 0x00
.text
.globl stack_swap
.type  stack_swap, @function
stack_swap:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea .L.str.36(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.37(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.38(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.39(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.39:
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x00
.section .rodata
.L.str.38:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.37:
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x00
.section .rodata
.L.str.36:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.text
.globl stack_dup
.type  stack_dup, @function
stack_dup:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea .L.str.35(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.35:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.34:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x73
  .byte 0x70
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.text
.globl store
.type  store, @function
store:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea .L.str.27(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  pop %rax
  cmp $1, %rax
  je .L.case.5
  cmp $4, %rax
  je .L.case.6
  jmp .L.default.7
.L.case.5:
  lea .L.str.28(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.145
.L.case.6:
  lea .L.str.30(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.31(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.145
.L.default.7:
  lea .L.str.32(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.145
.L.switch.end.145:
  leave
  ret
.section .rodata
.L.str.33:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x2c
  .byte 0x20
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x29
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.32:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.31:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x65
  .byte 0x61
  .byte 0x78
  .byte 0x2c
  .byte 0x20
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x29
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.30:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.29:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x62
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x61
  .byte 0x6c
  .byte 0x2c
  .byte 0x20
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x29
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.28:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.27:
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x00
.text
.globl load
.type  load, @function
load:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.else.146
  mov $0, %rax
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.146
.L.else.146:
.L.end.146:
  lea .L.str.20(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  je .L.else.147
  leave
  ret
  jmp .L.end.147
.L.else.147:
.L.end.147:
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  pop %rax
  cmp $1, %rax
  je .L.case.1
  cmp $4, %rax
  je .L.case.2
  cmp $8, %rax
  je .L.case.3
  jmp .L.default.4
.L.case.1:
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.148
.L.case.2:
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.23(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.148
.L.case.3:
  lea output_file(%rip), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.switch.end.148
.L.default.4:
  mov $0, %rax
  push %rax
  lea .L.str.26(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.switch.end.148:
  leave
  ret
.section .rodata
.L.str.26:
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
  .byte 0x73
  .byte 0x69
  .byte 0x7a
  .byte 0x65
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.25:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.24:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x74
  .byte 0x71
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.23:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x20
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x65
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.22:
  .byte 0x20
  .byte 0x20
  .byte 0x63
  .byte 0x6c
  .byte 0x74
  .byte 0x71
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.21:
  .byte 0x20
  .byte 0x20
  .byte 0x6d
  .byte 0x6f
  .byte 0x76
  .byte 0x7a
  .byte 0x62
  .byte 0x6c
  .byte 0x20
  .byte 0x28
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x29
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x65
  .byte 0x61
  .byte 0x78
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.20:
  .byte 0x72
  .byte 0x61
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.19:
  .byte 0x6e
  .byte 0x79
  .byte 0x69
  .byte 0x3a
  .byte 0x20
  .byte 0x6c
  .byte 0x6f
  .byte 0x61
  .byte 0x64
  .byte 0x20
  .byte 0x6f
  .byte 0x66
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
.globl assert_stack_empty
.type  assert_stack_empty, @function
assert_stack_empty:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea depth(%rip), %rax
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
  je .L.else.149
  mov $0, %rax
  push %rax
  lea .L.str.18(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea depth(%rip), %rax
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.149
.L.else.149:
.L.end.149:
  leave
  ret
.section .rodata
.L.str.18:
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x63
  .byte 0x6b
  .byte 0x20
  .byte 0x69
  .byte 0x73
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x65
  .byte 0x6d
  .byte 0x70
  .byte 0x74
  .byte 0x79
  .byte 0x20
  .byte 0x28
  .byte 0x64
  .byte 0x65
  .byte 0x70
  .byte 0x74
  .byte 0x68
  .byte 0x20
  .byte 0x25
  .byte 0x64
  .byte 0x29
  .byte 0x00
.text
.globl pop_void
.type  pop_void, @function
pop_void:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea depth(%rip), %rax
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
  je .L.else.150
  mov $0, %rax
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.150
.L.else.150:
.L.end.150:
  lea depth(%rip), %rax
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
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.17(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.17:
  .byte 0x20
  .byte 0x20
  .byte 0x61
  .byte 0x64
  .byte 0x64
  .byte 0x20
  .byte 0x24
  .byte 0x38
  .byte 0x2c
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x72
  .byte 0x73
  .byte 0x70
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.16:
  .byte 0x70
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x66
  .byte 0x72
  .byte 0x6f
  .byte 0x6d
  .byte 0x20
  .byte 0x65
  .byte 0x6d
  .byte 0x70
  .byte 0x74
  .byte 0x79
  .byte 0x20
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x63
  .byte 0x6b
  .byte 0x00
.text
.globl pop
.type  pop, @function
pop:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `reg`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea depth(%rip), %rax
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
  je .L.else.151
  mov $0, %rax
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
  call ice_at
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.151
.L.else.151:
.L.end.151:
  lea depth(%rip), %rax
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
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.15(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `reg`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.15:
  .byte 0x20
  .byte 0x20
  .byte 0x70
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.14:
  .byte 0x70
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x66
  .byte 0x72
  .byte 0x6f
  .byte 0x6d
  .byte 0x20
  .byte 0x65
  .byte 0x6d
  .byte 0x70
  .byte 0x74
  .byte 0x79
  .byte 0x20
  .byte 0x76
  .byte 0x61
  .byte 0x6c
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x63
  .byte 0x6b
  .byte 0x00
.text
.globl push
.type  push, @function
push:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `reg`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea depth(%rip), %rax
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
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.13(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `reg`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.13:
  .byte 0x20
  .byte 0x20
  .byte 0x70
  .byte 0x75
  .byte 0x73
  .byte 0x68
  .byte 0x20
  .byte 0x25
  .byte 0x25
  .byte 0x25
  .byte 0x73
  .byte 0xa
  .byte 0x00
.text
.globl gen_label_name
.type  gen_label_name, @function
gen_label_name:
  push %rbp
  movq %rsp, %rbp
  sub $36, %rsp
  push %rdi
  lea -36(%rbp), %rax # symbol ref lvalue `prefix`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -28(%rbp), %rax # symbol ref lvalue `count`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `name`
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
  lea -8(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.12(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -36(%rbp), %rax # symbol ref lvalue `prefix`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -28(%rbp), %rax # symbol ref lvalue `count`
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
  call fprintf
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
  lea -24(%rbp), %rax # symbol ref lvalue `name`
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
.L.str.12:
  .byte 0x25
  .byte 0x73
  .byte 0x2e
  .byte 0x25
  .byte 0x64
  .byte 0x00
.text
.globl gen_label
.type  gen_label, @function
gen_label:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea count.0(%rip), %rax
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
  pop %rax
  leave
  ret
  leave
  ret
.bss
.globl count.0
count.0:
  .zero 4
.text
.globl continue_target
.type  continue_target, @function
continue_target:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
.L.do.body.152:
  mov $0, %rax
  push %rax
  lea continue_stack_len(%rip), %rax
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
  je .L.else.153
  mov $0, %rax
  push %rax
  lea .L.str.11(%rip), %rax
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
  jmp .L.end.153
.L.else.153:
.L.end.153:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.152
.L.do.end.152:
  lea continue_stack(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea continue_stack_len(%rip), %rax
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
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.11:
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x6c
  .byte 0x6f
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x74
  .byte 0x69
  .byte 0x6e
  .byte 0x75
  .byte 0x65
  .byte 0x00
.text
.globl break_target
.type  break_target, @function
break_target:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
.L.do.body.154:
  mov $0, %rax
  push %rax
  lea break_stack_len(%rip), %rax
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
  je .L.else.155
  mov $0, %rax
  push %rax
  lea .L.str.10(%rip), %rax
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
  jmp .L.end.155
.L.else.155:
.L.end.155:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.154
.L.do.end.154:
  lea break_stack(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea break_stack_len(%rip), %rax
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
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.10:
  .byte 0x6e
  .byte 0x6f
  .byte 0x74
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x20
  .byte 0x6c
  .byte 0x6f
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x20
  .byte 0x62
  .byte 0x72
  .byte 0x65
  .byte 0x61
  .byte 0x6b
  .byte 0x00
.text
.globl pop_loop_labels
.type  pop_loop_labels, @function
pop_loop_labels:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
.L.do.body.156:
  mov $0, %rax
  push %rax
  lea continue_stack_len(%rip), %rax
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
  je .L.else.157
  mov $0, %rax
  push %rax
  lea .L.str.9(%rip), %rax
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
  jmp .L.end.157
.L.else.157:
.L.end.157:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.156
.L.do.end.156:
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pop_break
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea continue_stack_len(%rip), %rax
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
.L.str.9:
  .byte 0x70
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x66
  .byte 0x72
  .byte 0x6f
  .byte 0x6d
  .byte 0x20
  .byte 0x65
  .byte 0x6d
  .byte 0x70
  .byte 0x74
  .byte 0x79
  .byte 0x20
  .byte 0x62
  .byte 0x72
  .byte 0x65
  .byte 0x61
  .byte 0x6b
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x63
  .byte 0x6b
  .byte 0x00
.text
.globl push_loop_labels
.type  push_loop_labels, @function
push_loop_labels:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `break_label`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -8(%rbp), %rax # symbol ref lvalue `continue_label`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.do.body.158:
  mov $0, %rax
  push %rax
  lea continue_stack_len(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $25, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setl %al
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
  je .L.else.159
  mov $0, %rax
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
  jmp .L.end.159
.L.else.159:
.L.end.159:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.158
.L.do.end.158:
  lea -16(%rbp), %rax # symbol ref lvalue `break_label`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call push_break
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `continue_label`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea continue_stack(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea continue_stack_len(%rip), %rax
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
.section .rodata
.L.str.8:
  .byte 0x63
  .byte 0x6f
  .byte 0x6e
  .byte 0x74
  .byte 0x69
  .byte 0x6e
  .byte 0x75
  .byte 0x65
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x63
  .byte 0x6b
  .byte 0x20
  .byte 0x6f
  .byte 0x76
  .byte 0x65
  .byte 0x72
  .byte 0x66
  .byte 0x6c
  .byte 0x6f
  .byte 0x77
  .byte 0x00
.text
.globl pop_break
.type  pop_break, @function
pop_break:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
.L.do.body.160:
  mov $0, %rax
  push %rax
  lea break_stack_len(%rip), %rax
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
  je .L.else.161
  mov $0, %rax
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
  jmp .L.end.161
.L.else.161:
.L.end.161:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.160
.L.do.end.160:
  lea break_stack_len(%rip), %rax
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
.L.str.7:
  .byte 0x70
  .byte 0x6f
  .byte 0x70
  .byte 0x20
  .byte 0x66
  .byte 0x72
  .byte 0x6f
  .byte 0x6d
  .byte 0x20
  .byte 0x65
  .byte 0x6d
  .byte 0x70
  .byte 0x74
  .byte 0x79
  .byte 0x20
  .byte 0x62
  .byte 0x72
  .byte 0x65
  .byte 0x61
  .byte 0x6b
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x63
  .byte 0x6b
  .byte 0x00
.text
.globl push_break
.type  push_break, @function
push_break:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `break_label`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
.L.do.body.162:
  mov $0, %rax
  push %rax
  lea break_stack_len(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $25, %rax
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setl %al
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
  je .L.else.163
  mov $0, %rax
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
  jmp .L.end.163
.L.else.163:
.L.end.163:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.162
.L.do.end.162:
  lea -8(%rbp), %rax # symbol ref lvalue `break_label`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea break_stack(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea break_stack_len(%rip), %rax
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
.section .rodata
.L.str.6:
  .byte 0x62
  .byte 0x72
  .byte 0x65
  .byte 0x61
  .byte 0x6b
  .byte 0x20
  .byte 0x73
  .byte 0x74
  .byte 0x61
  .byte 0x63
  .byte 0x6b
  .byte 0x20
  .byte 0x6f
  .byte 0x76
  .byte 0x65
  .byte 0x72
  .byte 0x66
  .byte 0x6c
  .byte 0x6f
  .byte 0x77
  .byte 0x00
.bss
.globl continue_stack_len
continue_stack_len:
  .zero 4
.bss
.globl continue_stack
continue_stack:
  .zero 200
.bss
.globl break_stack_len
break_stack_len:
  .zero 4
.bss
.globl break_stack
break_stack:
  .zero 200
.bss
.globl depth
depth:
  .zero 4
.data
.globl argument_regs
argument_regs:
  .quad .L.str.0
  .quad .L.str.1
  .quad .L.str.2
  .quad .L.str.3
  .quad .L.str.4
  .quad .L.str.5
.section .rodata
.L.str.5:
  .byte 0x72
  .byte 0x39
  .byte 0x00
.section .rodata
.L.str.4:
  .byte 0x72
  .byte 0x38
  .byte 0x00
.section .rodata
.L.str.3:
  .byte 0x72
  .byte 0x63
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.2:
  .byte 0x72
  .byte 0x64
  .byte 0x78
  .byte 0x00
.section .rodata
.L.str.1:
  .byte 0x72
  .byte 0x73
  .byte 0x69
  .byte 0x00
.section .rodata
.L.str.0:
  .byte 0x72
  .byte 0x64
  .byte 0x69
  .byte 0x00
