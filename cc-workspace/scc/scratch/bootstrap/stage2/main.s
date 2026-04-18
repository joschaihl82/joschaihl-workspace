.text
.globl main
.type  main, @function
main:
  push %rbp
  movq %rsp, %rbp
  sub $36, %rsp
  push %rdi
  lea -36(%rbp), %rax # symbol ref lvalue `argc`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -32(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -36(%rbp), %rax # symbol ref lvalue `argc`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -32(%rbp), %rax # symbol ref lvalue `argv`
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
  call parse_options
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea input_file(%rip), %rax
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
  je .L.else.0
  lea .L.str.20(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call printf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.0
.L.else.0:
.L.end.0:
  mov $0, %rax
  push %rax
  lea output_file(%rip), %rax
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
  je .L.else.1
  lea stdout(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea output_file(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.1
.L.else.1:
.L.end.1:
  lea input_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call setup_preprocessor
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -24(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -32(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  lea -24(%rbp), %rax # symbol ref lvalue `stream`
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
  call load_file
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call tokenize
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -16(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `tok`
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call parse
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
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call codegen
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.20:
  .byte 0x65
  .byte 0x72
  .byte 0x72
  .byte 0x6f
  .byte 0x72
  .byte 0x3a
  .byte 0x20
  .byte 0x6e
  .byte 0x6f
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x70
  .byte 0x75
  .byte 0x74
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x6c
  .byte 0x75
  .byte 0xa
  .byte 0x00
.text
.globl setup_preprocessor
.type  setup_preprocessor, @function
setup_preprocessor:
  push %rbp
  movq %rsp, %rbp
  sub $76, %rsp
  push %rdi
  lea -76(%rbp), %rax # symbol ref lvalue `filename`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `pipefd`
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call pipe
  mov %rbp, %rsp
  pop %rbp
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
  je .L.else.2
  lea .L.str.10(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call perror
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.2
.L.else.2:
.L.end.2:
  lea -76(%rbp), %rax # symbol ref lvalue `filename`
  push %rax
  pop %rax
  mov (%rax), %rax
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
  call fopen
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -60(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov $0, %rax
  push %rax
  lea -60(%rbp), %rax # symbol ref lvalue `f`
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
  je .L.else.3
  lea .L.str.12(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call perror
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.3
.L.else.3:
.L.end.3:
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call fork
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea -52(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -52(%rbp), %rax # symbol ref lvalue `child_pid`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  mov $0, %rax
  push %rax
  mov $1, %rax
  push %rax
  pop %rdi
  pop %rax
  sub %rdi, %rax
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
  lea .L.str.13(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call perror
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.4
.L.else.4:
.L.end.4:
  lea -52(%rbp), %rax # symbol ref lvalue `child_pid`
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
  je .L.else.5
  lea -68(%rbp), %rax # symbol ref lvalue `pipefd`
  push %rax
  pop %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call close
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `pipefd`
  push %rax
  pop %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $4, %rax
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
  mov (%rax), %eax
  cltq
  push %rax
  mov $1, %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call dup2
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -68(%rbp), %rax # symbol ref lvalue `pipefd`
  push %rax
  pop %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $4, %rax
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
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call close
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -48(%rbp), %rax
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
  lea .L.str.14(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rax
  pop %rdi
  push %rax
  push %rdi
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov (%rsp), %rax
  push %rax
  lea .L.str.15(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rax
  pop %rdi
  push %rax
  push %rdi
  pop %rax
  add $8, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov (%rsp), %rax
  push %rax
  lea .L.str.16(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rax
  pop %rdi
  push %rax
  push %rdi
  pop %rax
  add $16, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov (%rsp), %rax
  push %rax
  lea .L.str.17(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rax
  pop %rdi
  push %rax
  push %rdi
  pop %rax
  add $24, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov (%rsp), %rax
  push %rax
  lea -76(%rbp), %rax # symbol ref lvalue `filename`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  pop %rdi
  push %rax
  push %rdi
  pop %rax
  add $32, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  mov (%rsp), %rax
  push %rax
  mov $0, %rax
  push %rax
  pop %rax
  pop %rdi
  push %rax
  push %rdi
  pop %rax
  add $40, %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  pop %rax
  lea .L.str.18(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -48(%rbp), %rax # symbol ref lvalue `args`
  push %rax
  pop %rax
  push %rax
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call execv
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.5
.L.else.5:
  lea -68(%rbp), %rax # symbol ref lvalue `pipefd`
  push %rax
  pop %rax
  push %rax
  mov $1, %rax
  push %rax
  mov $4, %rax
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
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call close
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  lea -60(%rbp), %rax # symbol ref lvalue `f`
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
  lea -68(%rbp), %rax # symbol ref lvalue `pipefd`
  push %rax
  pop %rax
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
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
  call fdopen
  mov %rbp, %rsp
  pop %rbp
  push %rax
  pop %rax
  leave
  ret
.L.end.5:
  leave
  ret
.section .rodata
.L.str.19:
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.18:
  .byte 0x2f
  .byte 0x75
  .byte 0x73
  .byte 0x72
  .byte 0x2f
  .byte 0x62
  .byte 0x69
  .byte 0x6e
  .byte 0x2f
  .byte 0x67
  .byte 0x63
  .byte 0x63
  .byte 0x00
.section .rodata
.L.str.17:
  .byte 0x2d
  .byte 0x44
  .byte 0x5f
  .byte 0x5f
  .byte 0x53
  .byte 0x43
  .byte 0x43
  .byte 0x5f
  .byte 0x5f
  .byte 0x3d
  .byte 0x31
  .byte 0x00
.section .rodata
.L.str.16:
  .byte 0x2d
  .byte 0x50
  .byte 0x00
.section .rodata
.L.str.15:
  .byte 0x2d
  .byte 0x45
  .byte 0x00
.section .rodata
.L.str.14:
  .byte 0x2f
  .byte 0x75
  .byte 0x73
  .byte 0x72
  .byte 0x2f
  .byte 0x62
  .byte 0x69
  .byte 0x6e
  .byte 0x2f
  .byte 0x67
  .byte 0x63
  .byte 0x63
  .byte 0x00
.section .rodata
.L.str.13:
  .byte 0x66
  .byte 0x61
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x66
  .byte 0x6f
  .byte 0x72
  .byte 0x6b
  .byte 0x00
.section .rodata
.L.str.12:
  .byte 0x66
  .byte 0x61
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x6e
  .byte 0x20
  .byte 0x73
  .byte 0x6f
  .byte 0x75
  .byte 0x72
  .byte 0x63
  .byte 0x65
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.11:
  .byte 0x72
  .byte 0x00
.section .rodata
.L.str.10:
  .byte 0x66
  .byte 0x61
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x6e
  .byte 0x20
  .byte 0x70
  .byte 0x69
  .byte 0x70
  .byte 0x65
  .byte 0x00
.text
.globl parse_options
.type  parse_options, @function
parse_options:
  push %rbp
  movq %rsp, %rbp
  sub $16, %rsp
  push %rdi
  lea -16(%rbp), %rax # symbol ref lvalue `argc`
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  push %rsi
  lea -12(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea enable_warnings(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  lea -4(%rbp), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  lea -16(%rbp), %rax # symbol ref lvalue `argc`
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
  setl %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.6
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call usage
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.6
.L.else.6:
.L.end.6:
.L.for.header.7:
  lea -4(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `argc`
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
  je .L.for.end.7
  lea .L.str.1(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strcmp
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
  jne .L.or.9
  lea .L.str.2(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strcmp
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
.L.or.9:
  setne %al
  movzb %al, %eax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.8
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call usage
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.8
.L.else.8:
.L.end.8:
  lea .L.str.3(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strcmp
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
  je .L.else.10
  mov $0, %rax
  push %rax
  lea enable_warnings(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %eax, (%rdi)
  push %rax
  add $8, %rsp
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  jmp .L.end.10
.L.else.10:
.L.end.10:
  lea .L.str.4(%rip), %rax
  push %rax
  pop %rax
  push %rax
  lea -12(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  pop %rsi
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call strcmp
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
  je .L.else.11
  lea output_file(%rip), %rax
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.12
  lea .L.str.5(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call printf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.12
.L.else.12:
.L.end.12:
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  lea -4(%rbp), %rax # symbol ref lvalue `i`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  lea -16(%rbp), %rax # symbol ref lvalue `argc`
  push %rax
  pop %rax
  mov (%rax), %eax
  cltq
  push %rax
  pop %rdi
  pop %rax
  cmp %rdi, %rax
  setge %al
  movzb %al, %rax
  push %rax
  pop %rax
  cmp $0, %rax
  je .L.else.13
  lea .L.str.6(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call printf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.13
.L.else.13:
.L.end.13:
  lea -12(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  call fopen
  mov %rbp, %rsp
  pop %rbp
  push %rax
  lea output_file(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  mov $0, %rax
  push %rax
  lea output_file(%rip), %rax
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
  je .L.else.14
  lea .L.str.8(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call perror
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  jmp .L.end.14
.L.else.14:
.L.end.14:
  jmp .L.for.next.7
  jmp .L.end.11
.L.else.11:
.L.end.11:
  mov $0, %rax
  push %rax
  lea input_file(%rip), %rax
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
  je .L.else.15
  lea -12(%rbp), %rax # symbol ref lvalue `argv`
  push %rax
  pop %rax
  mov (%rax), %rax
  push %rax
  lea -4(%rbp), %rax # symbol ref lvalue `i`
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
  pop %rax
  mov (%rax), %rax
  push %rax
  lea input_file(%rip), %rax
  push %rax
  pop %rdi
  pop %rax
  mov %rax, (%rdi)
  push %rax
  add $8, %rsp
  jmp .L.end.15
.L.else.15:
  lea .L.str.9(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call printf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
.L.end.15:
.L.for.next.7:
  jmp .L.for.header.7
.L.for.end.7:
  leave
  ret
.section .rodata
.L.str.9:
  .byte 0x65
  .byte 0x72
  .byte 0x72
  .byte 0x6f
  .byte 0x72
  .byte 0x3a
  .byte 0x20
  .byte 0x6d
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x69
  .byte 0x70
  .byte 0x6c
  .byte 0x65
  .byte 0x20
  .byte 0x69
  .byte 0x6e
  .byte 0x70
  .byte 0x75
  .byte 0x74
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.8:
  .byte 0x65
  .byte 0x72
  .byte 0x72
  .byte 0x6f
  .byte 0x72
  .byte 0x3a
  .byte 0x20
  .byte 0x66
  .byte 0x61
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x64
  .byte 0x20
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x70
  .byte 0x65
  .byte 0x6e
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x70
  .byte 0x75
  .byte 0x74
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x00
.section .rodata
.L.str.7:
  .byte 0x77
  .byte 0x00
.section .rodata
.L.str.6:
  .byte 0x65
  .byte 0x72
  .byte 0x72
  .byte 0x6f
  .byte 0x72
  .byte 0x3a
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
  .byte 0x74
  .byte 0x6f
  .byte 0x20
  .byte 0x2d
  .byte 0x6f
  .byte 0x20
  .byte 0x72
  .byte 0x65
  .byte 0x71
  .byte 0x75
  .byte 0x69
  .byte 0x72
  .byte 0x65
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.5:
  .byte 0x65
  .byte 0x72
  .byte 0x72
  .byte 0x6f
  .byte 0x72
  .byte 0x3a
  .byte 0x20
  .byte 0x6d
  .byte 0x75
  .byte 0x6c
  .byte 0x74
  .byte 0x69
  .byte 0x70
  .byte 0x6c
  .byte 0x65
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x70
  .byte 0x75
  .byte 0x74
  .byte 0x20
  .byte 0x66
  .byte 0x69
  .byte 0x6c
  .byte 0x65
  .byte 0x73
  .byte 0x20
  .byte 0x64
  .byte 0x65
  .byte 0x63
  .byte 0x6c
  .byte 0x61
  .byte 0x72
  .byte 0x65
  .byte 0x64
  .byte 0xa
  .byte 0x00
.section .rodata
.L.str.4:
  .byte 0x2d
  .byte 0x6f
  .byte 0x00
.section .rodata
.L.str.3:
  .byte 0x2d
  .byte 0x77
  .byte 0x00
.section .rodata
.L.str.2:
  .byte 0x2d
  .byte 0x2d
  .byte 0x68
  .byte 0x65
  .byte 0x6c
  .byte 0x70
  .byte 0x00
.section .rodata
.L.str.1:
  .byte 0x2d
  .byte 0x68
  .byte 0x00
.text
.globl usage
.type  usage, @function
usage:
  push %rbp
  movq %rsp, %rbp
  sub $0, %rsp
  lea .L.str.0(%rip), %rax
  push %rax
  pop %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call printf
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  mov $1, %rax
  push %rax
  pop %rdi
  mov $0, %al
  push %rbp
  mov %rsp, %rbp
  and $0xFFFFFFFFFFFFFFF0, %rsp
  call exit
  mov %rbp, %rsp
  pop %rbp
  push %rax
  add $8, %rsp
  leave
  ret
.section .rodata
.L.str.0:
  .byte 0x73
  .byte 0x63
  .byte 0x63
  .byte 0x20
  .byte 0x3c
  .byte 0x69
  .byte 0x6e
  .byte 0x70
  .byte 0x75
  .byte 0x74
  .byte 0x2e
  .byte 0x63
  .byte 0x3e
  .byte 0x20
  .byte 0x5b
  .byte 0x2d
  .byte 0x6f
  .byte 0x20
  .byte 0x6f
  .byte 0x75
  .byte 0x74
  .byte 0x5d
  .byte 0xa
  .byte 0x00
.bss
.globl enable_warnings
enable_warnings:
  .zero 4
.bss
.globl input_file
input_file:
  .zero 8
.bss
.globl output_file
output_file:
  .zero 8
