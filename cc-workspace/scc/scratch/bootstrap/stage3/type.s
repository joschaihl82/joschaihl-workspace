.text
.globl is_scalar_type
.type  is_scalar_type, @function
is_scalar_type:
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
  jne .L.or.15
  lea -8(%rbp), %rax # symbol ref lvalue `ty`
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
.L.or.15:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.text
.globl is_arithmetic_type
.type  is_arithmetic_type, @function
is_arithmetic_type:
  push %rbp
  movq %rsp, %rbp
  sub $8, %rsp
  push %rdi
  lea -8(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.case.10
  cmp $1, %rax
  je .L.case.11
  cmp $2, %rax
  je .L.case.12
  cmp $9, %rax
  je .L.case.13
  jmp .L.default.14
.L.case.10:
.L.case.11:
.L.case.12:
.L.case.13:
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
.L.default.14:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
.L.switch.end.16:
  leave
  ret
.text
.globl is_integer_type
.type  is_integer_type, @function
is_integer_type:
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
  leave
  ret
  leave
  ret
.text
.globl integer_conversion_rank_compare
.type  integer_conversion_rank_compare, @function
integer_conversion_rank_compare:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -8(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `right`
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
  je .L.else.17
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.17
.L.else.17:
.L.end.17:
  lea -16(%rbp), %rax # symbol ref lvalue `left`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_bool(%rip), %rax
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
  je .L.else.18
  mov $0, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.18
.L.else.18:
.L.end.18:
  lea -8(%rbp), %rax # symbol ref lvalue `right`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea ty_bool(%rip), %rax
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
  je .L.else.19
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.19
.L.else.19:
.L.end.19:
  lea -16(%rbp), %rax # symbol ref lvalue `left`
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
  lea -8(%rbp), %rax # symbol ref lvalue `right`
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
  je .L.else.20
  mov $0, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.20
.L.else.20:
  mov $1, %rax
  push %rax
  pop %rax
  leave
  ret
.L.end.20:
  leave
  ret
.text
.globl type_name
.type  type_name, @function
type_name:
  push %rbp
  movq %rsp, %rbp
  sub $32, %rsp
  push %rdi
  lea -32(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
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
  lea -32(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  call type_name_to_stream
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
.text
.globl field_lookup
.type  field_lookup, @function
field_lookup:
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
.L.do.body.21:
  mov $0, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
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
  jne .L.or.23
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
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
.L.or.23:
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
  je .L.else.22
  lea -24(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.13(%rip), %rax
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
  jmp .L.end.22
.L.else.22:
.L.end.22:
  mov $0, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  jne .L.do.body.21
.L.do.end.21:
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.24:
  lea -8(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.24
  lea -24(%rbp), %rax # symbol ref lvalue `name`
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
  lea -8(%rbp), %rax # symbol ref lvalue `f`
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
  je .L.and.26
  lea -24(%rbp), %rax # symbol ref lvalue `name`
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
  lea -8(%rbp), %rax # symbol ref lvalue `f`
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
  lea -24(%rbp), %rax # symbol ref lvalue `name`
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
.L.and.26:
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.25
  lea -8(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  leave
  ret
  jmp .L.end.25
.L.else.25:
.L.end.25:
.L.for.next.24:
  lea -8(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `f`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.24
.L.for.end.24:
  mov $0, %rax
  push %rax
  pop %rax
  leave
  ret
  leave
  ret
.section .rodata
.L.str.13:
  .byte 0x66
  .byte 0x69
  .byte 0x65
  .byte 0x6c
  .byte 0x64
  .byte 0x5f
  .byte 0x6c
  .byte 0x6f
  .byte 0x6f
  .byte 0x6b
  .byte 0x75
  .byte 0x70
  .byte 0x20
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
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
  .byte 0x74
  .byte 0x79
  .byte 0x70
  .byte 0x65
  .byte 0x00
.text
.globl type_name_to_stream
.type  type_name_to_stream, @function
type_name_to_stream:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  cmp $0, %rax
  je .L.else.27
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.0(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call type_name_to_stream
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
  jmp .L.end.27
.L.else.27:
.L.end.27:
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rax
  cmp $4, %rax
  je .L.case.0
  cmp $0, %rax
  je .L.case.1
  cmp $1, %rax
  je .L.case.2
  cmp $2, %rax
  je .L.case.3
  cmp $5, %rax
  je .L.case.4
  cmp $6, %rax
  je .L.case.5
  cmp $3, %rax
  je .L.case.6
  cmp $7, %rax
  je .L.case.7
  cmp $8, %rax
  je .L.case.8
  cmp $9, %rax
  je .L.case.9
  jmp .L.switch.end.28
.L.case.0:
  lea .L.str.2(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call fputs
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.case.1:
  lea .L.str.3(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call fputs
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.case.2:
  lea .L.str.4(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call fputs
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.case.3:
  lea .L.str.5(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call fputs
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.case.4:
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call type_name_to_stream
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -8(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
.L.for.header.29:
  lea -8(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.for.end.29
  lea -8(%rbp), %rax # symbol ref lvalue `p`
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
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call type_name_to_stream
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -8(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.30
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.30
.L.else.30:
.L.end.30:
.L.for.next.29:
  lea -8(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `p`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.for.header.29
.L.for.end.29:
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.case.5:
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call type_name_to_stream
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.9(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
.L.case.6:
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  call type_name_to_stream
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $42, %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
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
  leave
  ret
.L.case.7:
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.10(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  pop %rax
  add $16, %rax
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
  add $40, %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.case.8:
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.11(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  pop %rax
  add $16, %rax
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
  add $40, %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.case.9:
  lea -16(%rbp), %rax # symbol ref lvalue `stream`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea .L.str.12(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -24(%rbp), %rax # symbol ref lvalue `ty`
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
  pop %rax
  add $16, %rax
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
  add $40, %rax
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
  call fprintf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.L.switch.end.28:
  leave
  ret
.section .rodata
.L.str.12:
  .byte 0x65
  .byte 0x6e
  .byte 0x75
  .byte 0x6d
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0x00
.section .rodata
.L.str.11:
  .byte 0x75
  .byte 0x6e
  .byte 0x69
  .byte 0x6f
  .byte 0x6e
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0x00
.section .rodata
.L.str.10:
  .byte 0x73
  .byte 0x74
  .byte 0x72
  .byte 0x75
  .byte 0x63
  .byte 0x74
  .byte 0x20
  .byte 0x25
  .byte 0x73
  .byte 0x00
.section .rodata
.L.str.9:
  .byte 0x5b
  .byte 0x25
  .byte 0x64
  .byte 0x5d
  .byte 0x00
.section .rodata
.L.str.8:
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.7:
  .byte 0x2c
  .byte 0x20
  .byte 0x00
.section .rodata
.L.str.6:
  .byte 0x28
  .byte 0x00
.section .rodata
.L.str.5:
  .byte 0x5f
  .byte 0x42
  .byte 0x6f
  .byte 0x6f
  .byte 0x6c
  .byte 0x00
.section .rodata
.L.str.4:
  .byte 0x63
  .byte 0x68
  .byte 0x61
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.3:
  .byte 0x69
  .byte 0x6e
  .byte 0x74
  .byte 0x00
.section .rodata
.L.str.2:
  .byte 0x76
  .byte 0x6f
  .byte 0x69
  .byte 0x64
  .byte 0x00
.section .rodata
.L.str.1:
  .byte 0x60
  .byte 0x29
  .byte 0x00
.section .rodata
.L.str.0:
  .byte 0x25
  .byte 0x73
  .byte 0x20
  .byte 0x28
  .byte 0x61
  .byte 0x6b
  .byte 0x61
  .byte 0x20
  .byte 0x60
  .byte 0x00
.text
.globl make_enum
.type  make_enum, @function
make_enum:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  pop %rax
  add $16, %rax
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
  add $16, %rax
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
  pop %rax
  add $20, %rax
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
  add $20, %rax
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
.globl make_union
.type  make_union, @function
make_union:
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
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `fields`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `size`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -12(%rbp), %rax # symbol ref lvalue `align`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `size`
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
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `align`
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
  add $20, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `name`
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
  add $40, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `fields`
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
.globl make_struct
.type  make_struct, @function
make_struct:
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
  push %rsi
  lea -24(%rbp), %rax # symbol ref lvalue `fields`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -16(%rbp), %rax # symbol ref lvalue `size`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rcx
  lea -12(%rbp), %rax # symbol ref lvalue `align`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `size`
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
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `align`
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
  add $20, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -32(%rbp), %rax # symbol ref lvalue `name`
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
  add $40, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `fields`
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
.globl make_field
.type  make_field, @function
make_field:
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
  lea -20(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rdx
  lea -12(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
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
  lea -8(%rbp), %rax # symbol ref lvalue `f`
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
  lea -28(%rbp), %rax # symbol ref lvalue `name`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `f`
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
  lea -20(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `f`
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
  lea -12(%rbp), %rax # symbol ref lvalue `offset`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `f`
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
  lea -8(%rbp), %rax # symbol ref lvalue `f`
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
.globl make_typedef
.type  make_typedef, @function
make_typedef:
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  call memcpy
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `ty`
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
  lea -24(%rbp), %rax # symbol ref lvalue `name`
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
  add $32, %rax
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
.globl make_function_type
.type  make_function_type, @function
make_function_type:
  push %rbp
  movq %rsp, %rbp
  sub $24, %rsp
  push %rdi
  lea -24(%rbp), %rax # symbol ref lvalue `ret`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -16(%rbp), %rax # symbol ref lvalue `params`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -24(%rbp), %rax # symbol ref lvalue `ret`
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
  add $40, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `params`
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
  mov $1, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
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
.globl make_array_type
.type  make_array_type, @function
make_array_type:
  push %rbp
  movq %rsp, %rbp
  sub $20, %rsp
  push %rdi
  lea -20(%rbp), %rax # symbol ref lvalue `base`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -12(%rbp), %rax # symbol ref lvalue `len`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -20(%rbp), %rax # symbol ref lvalue `base`
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
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -12(%rbp), %rax # symbol ref lvalue `len`
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
  add $40, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -20(%rbp), %rax # symbol ref lvalue `base`
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
  lea -12(%rbp), %rax # symbol ref lvalue `len`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  imul %rdi, %rax
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
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -20(%rbp), %rax # symbol ref lvalue `base`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
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
.globl make_pointer_type
.type  make_pointer_type, @function
make_pointer_type:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
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
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -16(%rbp), %rax # symbol ref lvalue `base`
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
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $8, %rax
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
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  mov $8, %rax
  push %rax
  lea -8(%rbp), %rax # symbol ref lvalue `t`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  add $20, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
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
.data
.globl ty_bool
ty_bool:
  .quad ty_bool_struct
.data
.globl ty_char
ty_char:
  .quad ty_char_struct
.data
.globl ty_void
ty_void:
  .quad ty_void_struct
.data
.globl ty_long
ty_long:
  .quad ty_long_struct
.data
.globl ty_int
ty_int:
  .quad ty_int_struct
.data
.globl ty_bool_struct
ty_bool_struct:
  .long 2
  .zero 4
  .quad 0
  .long 1
  .long 1
  .quad 0
  .quad 0
.data
.globl ty_char_struct
ty_char_struct:
  .long 1
  .zero 4
  .quad 0
  .long 1
  .long 1
  .quad 0
  .quad 0
.data
.globl ty_void_struct
ty_void_struct:
  .long 4
  .zero 4
  .quad 0
  .long 0
  .long 1
  .quad 0
  .quad 0
.data
.globl ty_long_struct
ty_long_struct:
  .long 0
  .zero 4
  .quad 0
  .long 8
  .long 8
  .quad 0
  .quad 0
.data
.globl ty_int_struct
ty_int_struct:
  .long 0
  .zero 4
  .quad 0
  .long 4
  .long 4
  .quad 0
  .quad 0
