.intel_syntax noprefix
.data
NULL:
  .zero 8
.globl new_type
.text
new_type:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 4], edi
  mov [rbp - 12], rsi
  mov rax, rbp
  sub rax, 20
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_struct_type
.text
new_struct_type:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov [rbp - 9], sil
  mov rax, rbp
  sub rax, 17
  push rax
  push 4
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 17
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 17
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 17
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 9
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 17
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl align
.text
align:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 4], edi
  mov [rbp - 8], esi
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl add_field
.text
add_field:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov [rbp - 24], rdx
  mov rax, rbp
  sub rax, 32
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse0
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend0
.Lelse0:
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend0:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend1:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend3
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend3:
  jmp .Lend2
.Lelse2:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse4
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call align
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call align
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend4
.Lelse4:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call align
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend4:
.Lend2:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC0:
  .string "sizeof_type: unknown type"
.globl sizeof_type
.text
sizeof_type:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lcase1_0
  cmp rax, 1
  je .Lcase1_1
  cmp rax, 2
  je .Lcase1_2
  cmp rax, 3
  je .Lcase1_3
  cmp rax, 4
  je .Lcase1_4
  cmp rax, 7
  je .Lcase1_7
  cmp rax, 8
  je .Lcase1_8
.Lcase1_0:
  push 4
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1_1:
  push 8
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1_2:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1_3:
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1_4:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1_7:
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1_8:
  push 24
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lbreak1:
  lea rax, .LC0[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
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
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov [rbp - 12], esi
  mov rax, rbp
  sub rax, 20
  push rax
  push 1
  push 16
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl str_equals
.text
str_equals:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse5
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call memcmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse5
  push 1
  jmp .Lend5
.Lfalse5:
  push 0
.Lend5:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl str_chr_equals
.text
str_chr_equals:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strlen
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse6
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call memcmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse6
  push 1
  jmp .Lend6
.Lfalse6:
  push 0
.Lend6:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
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
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 12
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin7:
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  lea rax, mi[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend7
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, macros[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend8
  lea rax, macros[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend8:
.Lcontinue1:
  mov rax, rbp
  sub rax, 12
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin7
.Lend7:
.Lbreak2:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl remove_newline
.text
remove_newline:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin9:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend9
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse10
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse11
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend11
.Lelse11:
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend11:
  jmp .Lend10
.Lelse10:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend10:
.Lcontinue2:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin9
.Lend9:
.Lbreak3:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC11:
  .string "#define"
.data
.LC10:
  .string "once"
.data
.LC9:
  .string "#pragma"
.data
.LC8:
  .string "#endif"
.data
.LC7:
  .string "#endif"
.data
.LC6:
  .string "#else"
.data
.LC5:
  .string "#endif"
.data
.LC4:
  .string "#else"
.data
.LC3:
  .string "#ifdef"
.data
.LC2:
  .string "%s/%s"
.data
.LC1:
  .string "#include"
.globl preprocess
.text
preprocess:
  push rbp
  mov rbp, rsp
  sub rsp, 112
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin12:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend12
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 18
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse13
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC1[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend14
  mov rax, rbp
  sub rax, 40
  push rax
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, filename[rip]
  push rax
  push 1
  push 100
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 48
  push rax
  push 1
  push 200
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncpy
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 200
  lea rax, .LC2[rip]
  push rax
  lea rax, dir_name[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call snprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call read_file
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call tokenize
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call preprocess
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, filename[rip]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend15
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin16:
.Lcontinue4:
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend16
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin16
.Lend16:
.Lbreak5:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse17
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend17
.Lelse17:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend17:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend15:
.Lend14:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC3[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend18
.Lbegin19:
.Lcontinue5:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC4[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue20
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC5[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue20
  push 0
  jmp .Lend20
.Ltrue20:
  push 1
.Lend20:
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend19
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin19
.Lend19:
.Lbreak6:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC6[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse21
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse22
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend22
.Lelse22:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend22:
.Lbegin23:
.Lcontinue6:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC7[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend23
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin23
.Lend23:
.Lbreak7:
  jmp .Lend21
.Lelse21:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC8[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend21:
.Lend18:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC9[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend24
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC10[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_chr_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend25
  mov rax, rbp
  sub rax, 76
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin26:
.Lcontinue7:
  lea rax, once_file[rip]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend26
  lea rax, once_file[rip]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strcmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend27
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend27:
  mov rax, rbp
  sub rax, 76
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin26
.Lend26:
.Lbreak8:
  lea rax, once_file[rip]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend25:
.Lend24:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC11[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend28
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 84
  push rax
  push 1
  push 16
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin29:
.Lcontinue8:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend29
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin29
.Lend29:
.Lbreak9:
  mov rax, rbp
  sub rax, 92
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, macros[rip]
  push rax
  lea rax, mi[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend28:
  jmp .Lend13
.Lelse13:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend30
  mov rax, rbp
  sub rax, 84
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_macro
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend31
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse32
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend32
.Lelse32:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend32:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue3
.Lend31:
  mov rax, rbp
  sub rax, 100
  push rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin33:
.Lcontinue9:
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend33
  mov rax, rbp
  sub rax, 108
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse34
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend34
.Lelse34:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend34:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 100
  push rax
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin33
.Lend33:
.Lbreak10:
  jmp .Lcontinue3
.Lend30:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse35
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend35
.Lelse35:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend35:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend13:
.Lcontinue3:
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin12
.Lend12:
.Lbreak4:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call remove_newline
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
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
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  lea rax, locals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin36:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend36
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend37
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend37:
.Lcontinue10:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin36
.Lend36:
.Lbreak11:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_lvar
.text
new_lvar:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, locals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, locals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse38
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, locals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend38
.Lelse38:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend38:
  lea rax, locals[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl find_enum_val
.text
find_enum_val:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  lea rax, enumVals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin39:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend39
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend40
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend40:
.Lcontinue11:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin39
.Lend39:
.Lbreak12:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_enum_val
.text
new_enum_val:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov [rbp - 12], esi
  mov rax, rbp
  sub rax, 20
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, enumVals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, enumVals[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl find_struct
.text
find_struct:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  lea rax, structs[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin41:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend41
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend42
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend42:
.Lcontinue12:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin41
.Lend41:
.Lbreak13:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl find_struct_field
.text
find_struct_field:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin43:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend43
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend44
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend44:
.Lcontinue13:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin43
.Lend43:
.Lbreak14:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_gvar
.text
new_gvar:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, globals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, globals[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.globl find_gvar
.text
find_gvar:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  lea rax, globals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin45:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend45
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend46
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend46:
.Lcontinue14:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin45
.Lend45:
.Lbreak15:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl find_typedef
.text
find_typedef:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  lea rax, typedefs[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin47:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend47
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend48
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend48:
.Lcontinue15:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin47
.Lend47:
.Lbreak16:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_typedef
.text
new_typedef:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, typedefs[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, typedefs[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl find_func
.text
find_func:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  lea rax, funcs[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin49:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend49
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend50
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend50:
.Lcontinue16:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin49
.Lend49:
.Lbreak17:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_func
.text
new_func:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, funcs[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, funcs[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl consume
.text
consume:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 8], rdi
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue52
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_chr_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue52
  push 0
  jmp .Lend52
.Ltrue52:
  push 1
.Lend52:
  pop rax
  cmp rax, 0
  je .Lend51
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend51:
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl consume_kind
.text
consume_kind:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 4], edi
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend53
  mov rax, rbp
  sub rax, 12
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend53:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl consume_ident
.text
consume_ident:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend54
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend54:
  mov rax, rbp
  sub rax, 8
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl check_kind
.text
check_kind:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp - 4], edi
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl go_to
.text
go_to:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 8], rdi
  lea rax, token[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC12:
  .string "token mismatch: expected %s"
.globl expect
.text
expect:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 8], rdi
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue56
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_chr_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue56
  push 0
  jmp .Lend56
.Ltrue56:
  push 1
.Lend56:
  pop rax
  cmp rax, 0
  je .Lend55
  lea rax, .LC12[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend55:
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC13:
  .string "token mismatch"
.globl expect_kind
.text
expect_kind:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp - 4], edi
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend57
  lea rax, .LC13[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend57:
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC14:
  .string "token mismatch: expected number"
.globl expect_number
.text
expect_number:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 3
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend58
  lea rax, .LC14[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend58:
  mov rax, rbp
  sub rax, 4
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl at_eof
.text
at_eof:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 14
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC24:
  .string "expected struct name"
.data
.LC23:
  .string "struct %.*s is already defined"
.data
.LC22:
  .string ";"
.data
.LC21:
  .string "expected type"
.data
.LC20:
  .string "}"
.data
.LC19:
  .string "{"
.data
.LC18:
  .string ","
.data
.LC17:
  .string "expected enum name"
.data
.LC16:
  .string "}"
.data
.LC15:
  .string "{"
.globl consume_type_name
.text
consume_type_name:
  push rbp
  mov rbp, rsp
  sub rsp, 72
  push 9
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend59
  push 0
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend59:
  push 10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend60
  push 3
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend60:
  push 25
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend61
  push 5
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend61:
  push 26
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend62
  push 7
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend62:
  push 27
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend63
  mov rax, rbp
  sub rax, 8
  push rax
  push 2
  push 8
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend63:
  push 11
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend64
  push 2
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  lea rax, .LC15[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend65
  mov rax, rbp
  sub rax, 12
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin66:
.Lcontinue17:
  lea rax, .LC16[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend66
  mov rax, rbp
  sub rax, 20
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_ident
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend67
  lea rax, .LC17[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend67:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_enum_val
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  lea rax, .LC18[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  jmp .Lbegin66
.Lend66:
.Lbreak18:
.Lend65:
  push 0
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend64:
  push 12
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call check_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue69
  push 13
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call check_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue69
  push 0
  jmp .Lend69
.Ltrue69:
  push 1
.Lend69:
  pop rax
  cmp rax, 0
  je .Lend68
  mov rax, rbp
  sub rax, 21
  push rax
  push 0
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  push 13
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse70
  mov rax, rbp
  sub rax, 21
  push rax
  push 1
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lend70
.Lelse70:
  push 12
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
.Lend70:
  mov rax, rbp
  sub rax, 29
  push rax
  push 2
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC19[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse71
  mov rax, rbp
  sub rax, 20
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend72
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend72:
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 21
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_struct_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin73:
.Lcontinue18:
  lea rax, .LC20[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend73
  mov rax, rbp
  sub rax, 37
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 37
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend74
  lea rax, .LC21[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend74:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 37
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 37
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call add_field
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC22[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lbegin73
.Lend73:
.Lbreak19:
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend75
  mov rax, rbp
  sub rax, 45
  push rax
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_struct
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse76
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse77
  lea rax, .LC23[rip]
  push rax
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lend77
.Lelse77:
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend77:
  jmp .Lend76
.Lelse76:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, structs[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, structs[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend76:
.Lend75:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend71
.Lelse71:
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend78
  lea rax, .LC24[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend78:
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_struct
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 53
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse79
  mov rax, rbp
  sub rax, 53
  push rax
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 21
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_struct_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 53
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, structs[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, structs[rip]
  push rax
  mov rax, rbp
  sub rax, 53
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend79
.Lelse79:
  mov rax, rbp
  sub rax, 53
  push rax
  push 4
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 53
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend79:
  mov rax, rbp
  sub rax, 53
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend71:
.Lend68:
  mov rax, rbp
  sub rax, 61
  push rax
  push 2
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 61
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend80
  mov rax, rbp
  sub rax, 69
  push rax
  mov rax, rbp
  sub rax, 61
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_typedef
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 69
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend81
  mov rax, rbp
  sub rax, 69
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend81:
  mov rax, rbp
  sub rax, 61
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call go_to
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend80:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC35:
  .string "]"
.data
.LC34:
  .string "expected constant expression"
.data
.LC33:
  .string "["
.data
.LC32:
  .string ")"
.data
.LC31:
  .string "..."
.data
.LC30:
  .string ","
.data
.LC29:
  .string "("
.data
.LC28:
  .string "expected identifier"
.data
.LC27:
  .string ")"
.data
.LC26:
  .string "*"
.data
.LC25:
  .string "("
.globl expect_nested_type
.text
expect_nested_type:
  push rbp
  mov rbp, rsp
  sub rsp, 48
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 12
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  lea rax, .LC25[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse82
.Lbegin83:
.Lcontinue19:
  lea rax, .LC26[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend83
  mov rax, rbp
  sub rax, 12
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin83
.Lend83:
.Lbreak20:
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect_nested_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC27[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lend82
.Lelse82:
  mov rax, rbp
  sub rax, 28
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_ident
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend84
  lea rax, .LC28[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend84:
  mov rax, rbp
  sub rax, 20
  push rax
  push 1
  push 16
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend82:
  lea rax, .LC29[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend85
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_noident_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_ident
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
.Lbegin86:
.Lcontinue20:
  lea rax, .LC30[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend86
  lea rax, .LC31[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend87
  jmp .Lbreak21
.Lend87:
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_noident_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_ident
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  jmp .Lbegin86
.Lend86:
.Lbreak21:
  lea rax, .LC32[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 6
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend85:
  mov rax, rbp
  sub rax, 32
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin88:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend88
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lcontinue21:
  mov rax, rbp
  sub rax, 32
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin88
.Lend88:
.Lbreak22:
.Lbegin89:
.Lcontinue22:
  lea rax, .LC33[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend89
  mov rax, rbp
  sub rax, 40
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend90
  lea rax, .LC34[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend90:
  lea rax, .LC35[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lbegin89
.Lend89:
.Lbreak23:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC36:
  .string "*"
.globl consume_type
.text
consume_type:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type_name
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend91
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend91:
.Lbegin92:
.Lcontinue23:
  lea rax, .LC36[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend92
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin92
.Lend92:
.Lbreak24:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect_nested_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC45:
  .string "]"
.data
.LC44:
  .string "expected constant expression"
.data
.LC43:
  .string "["
.data
.LC42:
  .string ")"
.data
.LC41:
  .string ","
.data
.LC40:
  .string "("
.data
.LC39:
  .string ")"
.data
.LC38:
  .string "("
.data
.LC37:
  .string "*"
.globl expect_noident_type
.text
expect_noident_type:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
.Lbegin93:
.Lcontinue24:
  lea rax, .LC37[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend93
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin93
.Lend93:
.Lbreak25:
  lea rax, .LC38[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend94
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect_noident_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC39[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend94:
  lea rax, .LC40[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend95
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_noident_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
.Lbegin96:
.Lcontinue25:
  lea rax, .LC41[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend96
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_noident_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  jmp .Lbegin96
.Lend96:
.Lbreak26:
  lea rax, .LC42[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 6
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend95:
.Lbegin97:
.Lcontinue26:
  lea rax, .LC43[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend97
  mov rax, rbp
  sub rax, 16
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend98
  lea rax, .LC44[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend98:
  lea rax, .LC45[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lbegin97
.Lend97:
.Lbreak27:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl consume_noident_type
.text
consume_noident_type:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type_name
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend99
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend99:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect_noident_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl next
.text
next:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC46:
  .string "internal error"
.globl new_node
.text
new_node:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 4], edi
  mov [rbp - 12], rsi
  mov [rbp - 20], rdx
  mov rax, rbp
  sub rax, 28
  push rax
  push 1
  push 48
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lcase2_0
  cmp rax, 1
  je .Lcase2_1
  cmp rax, 2
  je .Lcase2_2
  cmp rax, 3
  je .Lcase2_3
  cmp rax, 4
  je .Lcase2_4
  cmp rax, 6
  je .Lcase2_6
  cmp rax, 7
  je .Lcase2_7
  cmp rax, 8
  je .Lcase2_8
  cmp rax, 9
  je .Lcase2_9
  cmp rax, 10
  je .Lcase2_10
  cmp rax, 11
  je .Lcase2_11
  cmp rax, 12
  je .Lcase2_12
  cmp rax, 13
  je .Lcase2_13
  cmp rax, 31
  je .Lcase2_31
  cmp rax, 29
  je .Lcase2_29
  cmp rax, 14
  je .Lcase2_14
  cmp rax, 15
  je .Lcase2_15
  cmp rax, 27
  je .Lcase2_27
  cmp rax, 28
  je .Lcase2_28
  jmp .Ldefault2
.Lcase2_0:
.Lcase2_1:
.Lcase2_2:
.Lcase2_3:
.Lcase2_4:
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbreak28
.Lcase2_6:
.Lcase2_7:
.Lcase2_8:
.Lcase2_9:
.Lcase2_10:
.Lcase2_11:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbreak28
.Lcase2_12:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbreak28
.Lcase2_13:
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbreak28
.Lcase2_31:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  push 3
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbreak28
.Lcase2_29:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbreak28
.Lcase2_14:
.Lcase2_15:
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbreak28
.Lcase2_27:
.Lcase2_28:
  lea rax, .LC46[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lbreak28
.Ldefault2:
  jmp .Lbreak28
.Lbreak28:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_typed_node
.text
new_typed_node:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  mov [rbp - 4], edi
  mov [rbp - 12], rsi
  mov [rbp - 20], rdx
  mov [rbp - 28], rcx
  mov rax, rbp
  sub rax, 36
  push rax
  push 1
  push 48
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_node_num
.text
new_node_num:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 4], edi
  mov rax, rbp
  sub rax, 12
  push rax
  push 1
  push 48
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 30
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl new_node_char
.text
new_node_char:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 1], dil
  mov rax, rbp
  sub rax, 9
  push rax
  push 1
  push 48
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 9
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 30
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 9
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 17
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 17
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 3
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 9
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 17
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 9
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
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
.LC63:
  .string ";"
.data
.LC62:
  .string "expected block"
.data
.LC61:
  .string ";"
.data
.LC60:
  .string ")"
.data
.LC59:
  .string ","
.data
.LC58:
  .string "failed to parse argument"
.data
.LC57:
  .string "..."
.data
.LC56:
  .string ")"
.data
.LC55:
  .string ")"
.data
.LC54:
  .string "("
.data
.LC53:
  .string "invalid type"
.data
.LC52:
  .string ";"
.data
.LC51:
  .string "expected type"
.data
.LC50:
  .string ";"
.data
.LC49:
  .string "expected struct or union"
.data
.LC48:
  .string ";"
.data
.LC47:
  .string "NULL"
.globl external
.text
external:
  push rbp
  mov rbp, rsp
  sub rsp, 48
  lea rax, locals[rip]
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, globals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend100
  lea rax, .LC47[rip]
  push rax
  push 4
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_string
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 1
  push 5
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_gvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend100:
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  push 72
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, ext[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  push 28
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend101
  lea rax, ext[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 69
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
.Lend101:
  push 11
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call check_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend102
  lea rax, ext[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 3
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type_name
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  lea rax, .LC48[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend102:
  push 12
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call check_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue104
  push 13
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call check_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue104
  push 0
  jmp .Lend104
.Ltrue104:
  push 1
.Lend104:
  pop rax
  cmp rax, 0
  je .Lend103
  lea rax, ext[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 4
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type_name
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend105
  lea rax, .LC49[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend105:
  lea rax, .LC50[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend103:
  push 19
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend106
  lea rax, ext[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 5
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend107
  lea rax, .LC51[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend107:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typedef
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  lea rax, .LC52[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend106:
  mov rax, rbp
  sub rax, 28
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend108
  lea rax, .LC53[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend108:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 6
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse109
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_func
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend110
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_func
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
.Lend110:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call go_to
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call next
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC54[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 37
  push rax
  push 0
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  push 25
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend111
  lea rax, .LC55[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse112
  mov rax, rbp
  sub rax, 37
  push rax
  push 1
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lend112
.Lelse112:
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call go_to
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend112:
.Lend111:
  lea rax, .LC56[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse114
  mov rax, rbp
  sub rax, 37
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse114
  push 1
  jmp .Lend114
.Lfalse114:
  push 0
.Lend114:
  pop rax
  cmp rax, 0
  je .Lend113
.Lbegin115:
  lea rax, .LC57[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend116
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 68
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lbreak29
.Lend116:
  mov rax, rbp
  sub rax, 28
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend117
  lea rax, .LC58[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend117:
  mov rax, rbp
  sub rax, 45
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_lvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend118
  mov rax, rbp
  sub rax, 45
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_lvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend118:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 45
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, .LC59[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend119
  jmp .Lbreak29
.Lend119:
.Lcontinue27:
  jmp .Lbegin115
.Lend115:
.Lbreak29:
  lea rax, .LC60[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend113:
  lea rax, .LC61[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend120
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend120:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 26
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend121
  lea rax, .LC62[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend121:
  jmp .Lend109
.Lelse109:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 2
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_gvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, .LC63[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend109:
  lea rax, locals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend122
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, locals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend122:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC86:
  .string ";"
.data
.LC85:
  .string ";"
.data
.LC84:
  .string ";"
.data
.LC83:
  .string ";"
.data
.LC82:
  .string ";"
.data
.LC81:
  .string "}"
.data
.LC80:
  .string "{"
.data
.LC79:
  .string ")"
.data
.LC78:
  .string ")"
.data
.LC77:
  .string ";"
.data
.LC76:
  .string ";"
.data
.LC75:
  .string ";"
.data
.LC74:
  .string ";"
.data
.LC73:
  .string "("
.data
.LC72:
  .string ")"
.data
.LC71:
  .string "("
.data
.LC70:
  .string ")"
.data
.LC69:
  .string "("
.data
.LC68:
  .string ")"
.data
.LC67:
  .string "("
.data
.LC66:
  .string ":"
.data
.LC65:
  .string ":"
.data
.LC64:
  .string "expected constant expression"
.globl stmt
.text
stmt:
  push rbp
  mov rbp, rsp
  sub rsp, 80
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  push 21
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse123
  mov rax, rbp
  sub rax, 16
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend124
  lea rax, .LC64[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend124:
  lea rax, .LC65[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 20
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend123
.Lelse123:
  push 22
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse125
  lea rax, .LC66[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 21
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend125
.Lelse125:
  push 5
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse126
  lea rax, .LC67[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC68[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 6
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse127
  mov rax, rbp
  sub rax, 8
  push rax
  push 18
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 33
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend127
.Lelse127:
  mov rax, rbp
  sub rax, 8
  push rax
  push 17
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend127:
  jmp .Lend126
.Lelse126:
  push 20
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse128
  lea rax, .LC69[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC70[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 19
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend128
.Lelse128:
  push 7
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse129
  lea rax, .LC71[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC72[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 24
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend129
.Lelse129:
  push 8
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse130
  mov rax, rbp
  sub rax, 40
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 48
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 64
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC73[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC74[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend131
  mov rax, rbp
  sub rax, 40
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC75[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend131:
  lea rax, .LC76[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend132
  mov rax, rbp
  sub rax, 48
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC77[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend132:
  lea rax, .LC78[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend133
  mov rax, rbp
  sub rax, 56
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC79[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend133:
  mov rax, rbp
  sub rax, 64
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  push 33
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  push 33
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 25
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend130
.Lelse130:
  lea rax, .LC80[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse134
  mov rax, rbp
  sub rax, 8
  push rax
  push 26
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin135:
  lea rax, .LC81[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend135
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 33
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lcontinue28:
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin135
.Lend135:
.Lbreak30:
  jmp .Lend134
.Lelse134:
  push 4
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse136
  lea rax, .LC82[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse137
  mov rax, rbp
  sub rax, 8
  push rax
  push 16
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend137
.Lelse137:
  mov rax, rbp
  sub rax, 8
  push rax
  push 16
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC83[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend137:
  jmp .Lend136
.Lelse136:
  push 23
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse138
  mov rax, rbp
  sub rax, 8
  push rax
  push 22
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC84[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lend138
.Lelse138:
  push 24
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse139
  mov rax, rbp
  sub rax, 8
  push rax
  push 23
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC85[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lend139
.Lelse139:
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC86[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend139:
.Lend138:
.Lend136:
.Lend134:
.Lend130:
.Lend129:
.Lend128:
.Lend126:
.Lend125:
.Lend123:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl expr
.text
expr:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC91:
  .string "/="
.data
.LC90:
  .string "*="
.data
.LC89:
  .string "-="
.data
.LC88:
  .string "+="
.data
.LC87:
  .string "="
.globl assign
.text
assign:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call logical
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC87[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse140
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend140
.Lelse140:
  lea rax, .LC88[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse141
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend141
.Lelse141:
  lea rax, .LC89[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse142
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend142
.Lelse142:
  lea rax, .LC90[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse143
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend143
.Lelse143:
  lea rax, .LC91[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend144
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend144:
.Lend143:
.Lend142:
.Lend141:
.Lend140:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC93:
  .string "||"
.data
.LC92:
  .string "&&"
.globl logical
.text
logical:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call equality
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin145:
  lea rax, .LC92[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse146
  mov rax, rbp
  sub rax, 8
  push rax
  push 10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call logical
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend146
.Lelse146:
  lea rax, .LC93[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse147
  mov rax, rbp
  sub rax, 8
  push rax
  push 11
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call logical
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend147
.Lelse147:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend147:
.Lend146:
.Lcontinue29:
  jmp .Lbegin145
.Lend145:
.Lbreak31:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC95:
  .string "!="
.data
.LC94:
  .string "=="
.globl equality
.text
equality:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call relational
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin148:
  lea rax, .LC94[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse149
  mov rax, rbp
  sub rax, 8
  push rax
  push 6
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call relational
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend149
.Lelse149:
  lea rax, .LC95[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse150
  mov rax, rbp
  sub rax, 8
  push rax
  push 7
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call relational
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend150
.Lelse150:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend150:
.Lend149:
.Lcontinue30:
  jmp .Lbegin148
.Lend148:
.Lbreak32:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC99:
  .string ">"
.data
.LC98:
  .string ">="
.data
.LC97:
  .string "<"
.data
.LC96:
  .string "<="
.globl relational
.text
relational:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call add
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin151:
  lea rax, .LC96[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse152
  mov rax, rbp
  sub rax, 8
  push rax
  push 9
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call add
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend152
.Lelse152:
  lea rax, .LC97[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse153
  mov rax, rbp
  sub rax, 8
  push rax
  push 8
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call add
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend153
.Lelse153:
  lea rax, .LC98[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse154
  mov rax, rbp
  sub rax, 8
  push rax
  push 9
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call add
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend154
.Lelse154:
  lea rax, .LC99[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse155
  mov rax, rbp
  sub rax, 8
  push rax
  push 8
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call add
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend155
.Lelse155:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend155:
.Lend154:
.Lend153:
.Lend152:
.Lcontinue31:
  jmp .Lbegin151
.Lend151:
.Lbreak33:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC103:
  .string "-"
.data
.LC102:
  .string "+"
.data
.LC101:
  .string "-"
.data
.LC100:
  .string "+"
.globl add
.text
add:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call mul
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin156:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue159
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue159
  push 0
  jmp .Lend159
.Ltrue159:
  push 1
.Lend159:
  pop rax
  cmp rax, 0
  je .Lfalse158
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse158
  push 1
  jmp .Lend158
.Lfalse158:
  push 0
.Lend158:
  pop rax
  cmp rax, 0
  je .Lend157
  mov rax, rbp
  sub rax, 12
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, .LC100[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse160
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend161
  mov rax, rbp
  sub rax, 8
  push rax
  push 12
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend161:
  mov rax, rbp
  sub rax, 8
  push rax
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call mul
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend160
.Lelse160:
  lea rax, .LC101[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend162
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend163
  mov rax, rbp
  sub rax, 8
  push rax
  push 12
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend163:
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call mul
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend162:
.Lend160:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend157:
  lea rax, .LC102[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse164
  mov rax, rbp
  sub rax, 20
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call mul
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse166
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse166
  push 1
  jmp .Lend166
.Lfalse166:
  push 0
.Lend166:
  pop rax
  cmp rax, 0
  je .Lelse165
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend165
.Lelse165:
  mov rax, rbp
  sub rax, 8
  push rax
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend165:
  jmp .Lend164
.Lelse164:
  lea rax, .LC103[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse167
  mov rax, rbp
  sub rax, 20
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call mul
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse169
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse169
  push 1
  jmp .Lend169
.Lfalse169:
  push 0
.Lend169:
  pop rax
  cmp rax, 0
  je .Lelse168
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend168
.Lelse168:
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend168:
  jmp .Lend167
.Lelse167:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend167:
.Lend164:
.Lcontinue32:
  jmp .Lbegin156
.Lend156:
.Lbreak34:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC105:
  .string "/"
.data
.LC104:
  .string "*"
.globl mul
.text
mul:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call unary
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin170:
  lea rax, .LC104[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse171
  mov rax, rbp
  sub rax, 16
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call unary
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse173
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse173
  push 1
  jmp .Lend173
.Lfalse173:
  push 0
.Lend173:
  pop rax
  cmp rax, 0
  je .Lelse172
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend172
.Lelse172:
  mov rax, rbp
  sub rax, 8
  push rax
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend172:
  jmp .Lend171
.Lelse171:
  lea rax, .LC105[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse174
  mov rax, rbp
  sub rax, 16
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call unary
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse176
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse176
  push 1
  jmp .Lend176
.Lfalse176:
  push 0
.Lend176:
  pop rax
  cmp rax, 0
  je .Lelse175
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend175
.Lelse175:
  mov rax, rbp
  sub rax, 8
  push rax
  push 3
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend175:
  jmp .Lend174
.Lelse174:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend174:
.Lend171:
.Lcontinue33:
  jmp .Lbegin170
.Lend170:
.Lbreak35:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC115:
  .string "--"
.data
.LC114:
  .string "++"
.data
.LC113:
  .string "!"
.data
.LC112:
  .string ")"
.data
.LC111:
  .string "("
.data
.LC110:
  .string "dereference failed: not a pointer"
.data
.LC109:
  .string "*"
.data
.LC108:
  .string "&"
.data
.LC107:
  .string "-"
.data
.LC106:
  .string "+"
.globl unary
.text
unary:
  push rbp
  mov rbp, rsp
  sub rsp, 48
  lea rax, .LC106[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse177
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call postfix
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend177
.Lelse177:
  lea rax, .LC107[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse178
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call postfix
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse179
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend179
.Lelse179:
  push 1
  push 0
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend179:
  jmp .Lend178
.Lelse178:
  lea rax, .LC108[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse180
  push 12
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call postfix
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend180
.Lelse180:
  lea rax, .LC109[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse181
  mov rax, rbp
  sub rax, 16
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call primary
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse183
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse183
  push 1
  jmp .Lend183
.Lfalse183:
  push 0
.Lend183:
  pop rax
  cmp rax, 0
  je .Lend182
  lea rax, .LC110[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend182:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend184
  mov rax, rbp
  sub rax, 16
  push rax
  push 12
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend184:
  push 13
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend181
.Lelse181:
  push 15
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse185
  mov rax, rbp
  sub rax, 24
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC111[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend186
  mov rax, rbp
  sub rax, 32
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_noident_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend187
  lea rax, .LC112[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend187:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call go_to
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend186:
  mov rax, rbp
  sub rax, 40
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call unary
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend185
.Lelse185:
  lea rax, .LC113[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse188
  push 6
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call postfix
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend188
.Lelse188:
  lea rax, .LC114[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse189
  mov rax, rbp
  sub rax, 40
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call postfix
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 4
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend189
.Lelse189:
  lea rax, .LC115[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend190
  mov rax, rbp
  sub rax, 40
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call postfix
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 4
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend190:
.Lend189:
.Lend188:
.Lend185:
.Lend181:
.Lend180:
.Lend178:
.Lend177:
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call postfix
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC128:
  .string "--"
.data
.LC127:
  .string "++"
.data
.LC126:
  .string "no such field"
.data
.LC125:
  .string "expected struct type"
.data
.LC124:
  .string "expected pointer type"
.data
.LC123:
  .string "expected identifier after '->'"
.data
.LC122:
  .string "->"
.data
.LC121:
  .string "no such field"
.data
.LC120:
  .string "expected struct type"
.data
.LC119:
  .string "expected identifier after '.'"
.data
.LC118:
  .string "."
.data
.LC117:
  .string "]"
.data
.LC116:
  .string "["
.globl postfix
.text
postfix:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call primary
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin191:
  lea rax, .LC116[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend192
  mov rax, rbp
  sub rax, 16
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend193
  mov rax, rbp
  sub rax, 8
  push rax
  push 12
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend193:
  mov rax, rbp
  sub rax, 8
  push rax
  push 13
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC117[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lcontinue34
.Lend192:
  lea rax, .LC118[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend194
  mov rax, rbp
  sub rax, 28
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_ident
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend195
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC119[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend195:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend196
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC120[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend196:
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_struct_field
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend197
  lea rax, .LC121[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend197:
  mov rax, rbp
  sub rax, 8
  push rax
  push 12
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 13
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue34
.Lend194:
  lea rax, .LC122[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend198
  mov rax, rbp
  sub rax, 28
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_ident
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend199
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC123[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend199:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend200
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC124[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend200:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend201
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC125[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend201:
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_struct_field
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend202
  lea rax, .LC126[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_here
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend202:
  mov rax, rbp
  sub rax, 8
  push rax
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 13
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue34
.Lend198:
  lea rax, .LC127[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend203
  mov rax, rbp
  sub rax, 8
  push rax
  push 14
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue34
.Lend203:
  lea rax, .LC128[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend204
  mov rax, rbp
  sub rax, 8
  push rax
  push 15
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue34
.Lend204:
  jmp .Lbreak36
.Lcontinue34:
  jmp .Lbegin191
.Lend191:
.Lbreak36:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC142:
  .string "unknown escape sequence"
.data
.LC141:
  .string "%.*s is not defined"
.data
.LC140:
  .string ","
.data
.LC139:
  .string ")"
.data
.LC138:
  .string ")"
.data
.LC137:
  .string "va_start"
.data
.LC136:
  .string "("
.data
.LC135:
  .string "}"
.data
.LC134:
  .string ","
.data
.LC133:
  .string "type mismatch"
.data
.LC132:
  .string "{"
.data
.LC131:
  .string "="
.data
.LC130:
  .string ")"
.data
.LC129:
  .string "("
.globl primary
.text
primary:
  push rbp
  mov rbp, rsp
  sub rsp, 104
  lea rax, .LC129[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend205
  mov rax, rbp
  sub rax, 8
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC130[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend205:
  mov rax, rbp
  sub rax, 16
  push rax
  push 16
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend206
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, ext[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, ext[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 31
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, literal_count[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend206:
  mov rax, rbp
  sub rax, 32
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend207
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_lvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend208
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_lvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend208:
  mov rax, rbp
  sub rax, 8
  push rax
  push 27
  push 0
  push 0
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, .LC131[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend209
  lea rax, .LC132[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse210
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend211
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC133[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend211:
  mov rax, rbp
  sub rax, 8
  push rax
  push 5
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 52
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin212:
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend212
  mov rax, rbp
  sub rax, 60
  push rax
  push 0
  push 12
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 1
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 68
  push rax
  push 13
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 33
  push 4
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend213
  lea rax, .LC134[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend213:
.Lcontinue35:
  mov rax, rbp
  sub rax, 52
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin212
.Lend212:
.Lbreak37:
  lea rax, .LC135[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lend210
.Lelse210:
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assign
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend210:
.Lend209:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend207:
  mov rax, rbp
  sub rax, 16
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_ident
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend214
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  lea rax, .LC136[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse215
  mov rax, rbp
  sub rax, 76
  push rax
  push 1
  push 48
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_func
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lelse216
  mov rax, rbp
  sub rax, 84
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_func
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend216
.Lelse216:
  mov rax, rbp
  sub rax, 84
  push rax
  push 0
  push 0
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_type
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend216:
  mov rax, rbp
  sub rax, 8
  push rax
  push 29
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC137[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call str_chr_equals
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend217
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 32
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend217:
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC138[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend218
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend218:
.Lbegin219:
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 33
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC139[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse220
  jmp .Lbreak38
  jmp .Lend220
.Lelse220:
  lea rax, .LC140[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend220:
.Lcontinue36:
  jmp .Lbegin219
.Lend219:
.Lbreak38:
  jmp .Lend215
.Lelse215:
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_lvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse221
  mov rax, rbp
  sub rax, 8
  push rax
  push 27
  push 0
  push 0
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend221
.Lelse221:
  mov rax, rbp
  sub rax, 92
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_enum_val
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse222
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend222
.Lelse222:
  mov rax, rbp
  sub rax, 100
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call find_gvar
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse223
  mov rax, rbp
  sub rax, 8
  push rax
  push 28
  push 0
  push 0
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_typed_node
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend223
.Lelse223:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC141[rip]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend223:
.Lend222:
.Lend221:
.Lend215:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend214:
  mov rax, rbp
  sub rax, 16
  push rax
  push 17
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call consume_kind
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend224
  mov rax, rbp
  sub rax, 101
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 101
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 92
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend225
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 92
  je .Lcase3_92
  cmp rax, 39
  je .Lcase3_39
  cmp rax, 110
  je .Lcase3_110
  cmp rax, 48
  je .Lcase3_48
  jmp .Ldefault3
.Lcase3_92:
  mov rax, rbp
  sub rax, 101
  push rax
  push 92
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lbreak39
.Lcase3_39:
  mov rax, rbp
  sub rax, 101
  push rax
  push 39
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lbreak39
.Lcase3_110:
  mov rax, rbp
  sub rax, 101
  push rax
  push 10
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lbreak39
.Lcase3_48:
  mov rax, rbp
  sub rax, 101
  push rax
  push 0
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lbreak39
.Ldefault3:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC142[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lbreak39:
.Lend225:
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 101
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_char
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend224:
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call expect_number
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_node_num
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl token
.data
token:
  .zero 8
.globl new_token
.text
new_token:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  mov [rbp - 4], edi
  mov [rbp - 12], rsi
  mov [rbp - 20], rdx
  mov [rbp - 24], ecx
  mov rax, rbp
  sub rax, 32
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_string
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl is_alnum
.text
is_alnum:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp - 1], dil
  push 97
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse227
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 122
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse227
  push 1
  jmp .Lend227
.Lfalse227:
  push 0
.Lend227:
  pop rax
  cmp rax, 1
  je .Ltrue226
  push 65
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse229
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 90
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse229
  push 1
  jmp .Lend229
.Lfalse229:
  push 0
.Lend229:
  pop rax
  cmp rax, 1
  je .Ltrue228
  push 48
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse231
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 57
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse231
  push 1
  jmp .Lend231
.Lfalse231:
  push 0
.Lend231:
  pop rax
  cmp rax, 1
  je .Ltrue230
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 95
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue230
  push 0
  jmp .Lend230
.Ltrue230:
  push 1
.Lend230:
  pop rax
  cmp rax, 1
  je .Ltrue228
  push 0
  jmp .Lend228
.Ltrue228:
  push 1
.Lend228:
  pop rax
  cmp rax, 1
  je .Ltrue226
  push 0
  jmp .Lend226
.Ltrue226:
  push 1
.Lend226:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.globl startswith
.text
startswith:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strlen
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call memcmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC184:
  .string "tokenize failed"
.data
.LC183:
  .string "extern"
.data
.LC182:
  .string "_Bool"
.data
.LC181:
  .string "__builtin_va_list"
.data
.LC180:
  .string "void"
.data
.LC179:
  .string "continue"
.data
.LC178:
  .string "break"
.data
.LC177:
  .string "default"
.data
.LC176:
  .string "case"
.data
.LC175:
  .string "switch"
.data
.LC174:
  .string "typedef"
.data
.LC173:
  .string "union"
.data
.LC172:
  .string "struct"
.data
.LC171:
  .string "enum"
.data
.LC170:
  .string "char"
.data
.LC169:
  .string "sizeof"
.data
.LC168:
  .string "int"
.data
.LC167:
  .string "for"
.data
.LC166:
  .string "while"
.data
.LC165:
  .string "else"
.data
.LC164:
  .string "if"
.data
.LC163:
  .string "return"
.data
.LC162:
  .string "+-*/()<>:;={},&[].!"
.data
.LC161:
  .string "..."
.data
.LC160:
  .string "->"
.data
.LC159:
  .string "tokenize failed: \"*/\" not found"
.data
.LC158:
  .string "*/"
.data
.LC157:
  .string "/*"
.data
.LC156:
  .string "//"
.data
.LC155:
  .string "/="
.data
.LC154:
  .string "*="
.data
.LC153:
  .string "-="
.data
.LC152:
  .string "+="
.data
.LC151:
  .string "--"
.data
.LC150:
  .string "++"
.data
.LC149:
  .string "&&"
.data
.LC148:
  .string "||"
.data
.LC147:
  .string ">="
.data
.LC146:
  .string "<="
.data
.LC145:
  .string "!="
.data
.LC144:
  .string "=="
.data
.LC143:
  .string "#"
.globl tokenize
.text
tokenize:
  push rbp
  mov rbp, rsp
  sub rsp, 80
  mov [rbp - 8], rdi
  mov [rbp - 9], sil
  mov rax, rbp
  sub rax, 41
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  mov r10, [rax + 24]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 41
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 49
  push rax
  mov rax, rbp
  sub rax, 41
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin232:
.Lcontinue37:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend232
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend233
  mov rax, rbp
  sub rax, 49
  push rax
  push 1
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  push 1
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend233:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call isspace
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  cmp rax, 0
  je .Lend234
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lcontinue37
.Lend234:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC143[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend235
  mov rax, rbp
  sub rax, 57
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin236:
.Lcontinue38:
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call isspace
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend236
  mov rax, rbp
  sub rax, 57
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin236
.Lend236:
.Lbreak41:
  mov rax, rbp
  sub rax, 49
  push rax
  push 18
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend235:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC144[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue238
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC145[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue239
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC146[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue240
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC147[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue240
  push 0
  jmp .Lend240
.Ltrue240:
  push 1
.Lend240:
  pop rax
  cmp rax, 1
  je .Ltrue239
  push 0
  jmp .Lend239
.Ltrue239:
  push 1
.Lend239:
  pop rax
  cmp rax, 1
  je .Ltrue238
  push 0
  jmp .Lend238
.Ltrue238:
  push 1
.Lend238:
  pop rax
  cmp rax, 0
  je .Lend237
  mov rax, rbp
  sub rax, 49
  push rax
  push 0
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend237:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC148[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue242
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC149[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue242
  push 0
  jmp .Lend242
.Ltrue242:
  push 1
.Lend242:
  pop rax
  cmp rax, 0
  je .Lend241
  mov rax, rbp
  sub rax, 49
  push rax
  push 0
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend241:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC150[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue244
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC151[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue244
  push 0
  jmp .Lend244
.Ltrue244:
  push 1
.Lend244:
  pop rax
  cmp rax, 0
  je .Lend243
  mov rax, rbp
  sub rax, 49
  push rax
  push 0
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend243:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC152[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue246
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC153[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue247
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC154[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue248
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC155[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue248
  push 0
  jmp .Lend248
.Ltrue248:
  push 1
.Lend248:
  pop rax
  cmp rax, 1
  je .Ltrue247
  push 0
  jmp .Lend247
.Ltrue247:
  push 1
.Lend247:
  pop rax
  cmp rax, 1
  je .Ltrue246
  push 0
  jmp .Lend246
.Ltrue246:
  push 1
.Lend246:
  pop rax
  cmp rax, 0
  je .Lend245
  mov rax, rbp
  sub rax, 49
  push rax
  push 0
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend245:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC156[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend249
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin250:
.Lcontinue39:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend250
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin250
.Lend250:
.Lbreak42:
  jmp .Lcontinue37
.Lend249:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC157[rip]
  push rax
  push 2
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend251
  mov rax, rbp
  sub rax, 57
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC158[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strstr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend252
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC159[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend252:
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend251:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC160[rip]
  push rax
  push 2
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend253
  mov rax, rbp
  sub rax, 49
  push rax
  push 0
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend253:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC161[rip]
  push rax
  push 3
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend254
  mov rax, rbp
  sub rax, 49
  push rax
  push 0
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend254:
  lea rax, .LC162[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strchr
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
  cmp rax, 0
  je .Lend255
  mov rax, rbp
  sub rax, 49
  push rax
  push 0
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  push 1
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend255:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 34
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend256
  mov rax, rbp
  sub rax, 49
  push rax
  push 16
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 0
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin257:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 92
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue258
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 34
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue258
  push 0
  jmp .Lend258
.Ltrue258:
  push 1
.Lend258:
  pop rax
  cmp rax, 0
  je .Lend257
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
.Lcontinue40:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin257
.Lend257:
.Lbreak43:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lcontinue37
.Lend256:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 39
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend259
  mov rax, rbp
  sub rax, 49
  push rax
  push 17
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 0
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin260:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 92
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse262
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 92
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse262
  push 1
  jmp .Lend262
.Lfalse262:
  push 0
.Lend262:
  pop rax
  cmp rax, 1
  je .Ltrue261
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 39
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue261
  push 0
  jmp .Lend261
.Ltrue261:
  push 1
.Lend261:
  pop rax
  cmp rax, 0
  je .Lend260
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
.Lcontinue41:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin260
.Lend260:
.Lbreak44:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lcontinue37
.Lend259:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call isdigit
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  cmp rax, 0
  je .Lend263
  mov rax, rbp
  sub rax, 65
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 69
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 65
  push rax
  push 10
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strtol
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 73
  push rax
  mov rax, rbp
  sub rax, 65
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 65
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 49
  push rax
  push 3
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 73
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 69
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend263:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC163[rip]
  push rax
  push 6
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse265
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse265
  push 1
  jmp .Lend265
.Lfalse265:
  push 0
.Lend265:
  pop rax
  cmp rax, 0
  je .Lend264
  mov rax, rbp
  sub rax, 49
  push rax
  push 4
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend264:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC164[rip]
  push rax
  push 2
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse267
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse267
  push 1
  jmp .Lend267
.Lfalse267:
  push 0
.Lend267:
  pop rax
  cmp rax, 0
  je .Lend266
  mov rax, rbp
  sub rax, 49
  push rax
  push 5
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend266:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC165[rip]
  push rax
  push 4
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse269
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse269
  push 1
  jmp .Lend269
.Lfalse269:
  push 0
.Lend269:
  pop rax
  cmp rax, 0
  je .Lend268
  mov rax, rbp
  sub rax, 49
  push rax
  push 6
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend268:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC166[rip]
  push rax
  push 5
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse271
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse271
  push 1
  jmp .Lend271
.Lfalse271:
  push 0
.Lend271:
  pop rax
  cmp rax, 0
  je .Lend270
  mov rax, rbp
  sub rax, 49
  push rax
  push 7
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend270:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC167[rip]
  push rax
  push 3
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse273
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse273
  push 1
  jmp .Lend273
.Lfalse273:
  push 0
.Lend273:
  pop rax
  cmp rax, 0
  je .Lend272
  mov rax, rbp
  sub rax, 49
  push rax
  push 8
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend272:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC168[rip]
  push rax
  push 3
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse275
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse275
  push 1
  jmp .Lend275
.Lfalse275:
  push 0
.Lend275:
  pop rax
  cmp rax, 0
  je .Lend274
  mov rax, rbp
  sub rax, 49
  push rax
  push 9
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 3
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend274:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC169[rip]
  push rax
  push 6
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse277
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse277
  push 1
  jmp .Lend277
.Lfalse277:
  push 0
.Lend277:
  pop rax
  cmp rax, 0
  je .Lend276
  mov rax, rbp
  sub rax, 49
  push rax
  push 15
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend276:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC170[rip]
  push rax
  push 4
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse279
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse279
  push 1
  jmp .Lend279
.Lfalse279:
  push 0
.Lend279:
  pop rax
  cmp rax, 0
  je .Lend278
  mov rax, rbp
  sub rax, 49
  push rax
  push 10
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend278:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC171[rip]
  push rax
  push 4
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse281
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse281
  push 1
  jmp .Lend281
.Lfalse281:
  push 0
.Lend281:
  pop rax
  cmp rax, 0
  je .Lend280
  mov rax, rbp
  sub rax, 49
  push rax
  push 11
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend280:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC172[rip]
  push rax
  push 6
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse283
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse283
  push 1
  jmp .Lend283
.Lfalse283:
  push 0
.Lend283:
  pop rax
  cmp rax, 0
  je .Lend282
  mov rax, rbp
  sub rax, 49
  push rax
  push 12
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend282:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC173[rip]
  push rax
  push 5
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse285
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse285
  push 1
  jmp .Lend285
.Lfalse285:
  push 0
.Lend285:
  pop rax
  cmp rax, 0
  je .Lend284
  mov rax, rbp
  sub rax, 49
  push rax
  push 13
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend284:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC174[rip]
  push rax
  push 7
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse287
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 7
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse287
  push 1
  jmp .Lend287
.Lfalse287:
  push 0
.Lend287:
  pop rax
  cmp rax, 0
  je .Lend286
  mov rax, rbp
  sub rax, 49
  push rax
  push 19
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 7
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 7
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend286:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC175[rip]
  push rax
  push 6
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse289
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse289
  push 1
  jmp .Lend289
.Lfalse289:
  push 0
.Lend289:
  pop rax
  cmp rax, 0
  je .Lend288
  mov rax, rbp
  sub rax, 49
  push rax
  push 20
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend288:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC176[rip]
  push rax
  push 4
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse291
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse291
  push 1
  jmp .Lend291
.Lfalse291:
  push 0
.Lend291:
  pop rax
  cmp rax, 0
  je .Lend290
  mov rax, rbp
  sub rax, 49
  push rax
  push 21
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend290:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC177[rip]
  push rax
  push 7
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse293
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 7
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse293
  push 1
  jmp .Lend293
.Lfalse293:
  push 0
.Lend293:
  pop rax
  cmp rax, 0
  je .Lend292
  mov rax, rbp
  sub rax, 49
  push rax
  push 22
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 7
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 7
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend292:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC178[rip]
  push rax
  push 5
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse295
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse295
  push 1
  jmp .Lend295
.Lfalse295:
  push 0
.Lend295:
  pop rax
  cmp rax, 0
  je .Lend294
  mov rax, rbp
  sub rax, 49
  push rax
  push 23
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend294:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC179[rip]
  push rax
  push 8
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse297
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse297
  push 1
  jmp .Lend297
.Lfalse297:
  push 0
.Lend297:
  pop rax
  cmp rax, 0
  je .Lend296
  mov rax, rbp
  sub rax, 49
  push rax
  push 24
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend296:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC180[rip]
  push rax
  push 4
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse299
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse299
  push 1
  jmp .Lend299
.Lfalse299:
  push 0
.Lend299:
  pop rax
  cmp rax, 0
  je .Lend298
  mov rax, rbp
  sub rax, 49
  push rax
  push 25
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend298:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC181[rip]
  push rax
  push 17
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse301
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 17
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse301
  push 1
  jmp .Lend301
.Lfalse301:
  push 0
.Lend301:
  pop rax
  cmp rax, 0
  je .Lend300
  mov rax, rbp
  sub rax, 49
  push rax
  push 27
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 17
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 17
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend300:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC182[rip]
  push rax
  push 5
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse303
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse303
  push 1
  jmp .Lend303
.Lfalse303:
  push 0
.Lend303:
  pop rax
  cmp rax, 0
  je .Lend302
  mov rax, rbp
  sub rax, 49
  push rax
  push 26
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 5
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend302:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC183[rip]
  push rax
  push 6
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncmp
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse305
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call is_alnum
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse305
  push 1
  jmp .Lend305
.Lfalse305:
  push 0
.Lend305:
  pop rax
  cmp rax, 0
  je .Lend304
  mov rax, rbp
  sub rax, 49
  push rax
  push 28
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 6
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend304:
  push 97
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse308
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 122
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse308
  push 1
  jmp .Lend308
.Lfalse308:
  push 0
.Lend308:
  pop rax
  cmp rax, 1
  je .Ltrue307
  push 65
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse310
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 90
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse310
  push 1
  jmp .Lend310
.Lfalse310:
  push 0
.Lend310:
  pop rax
  cmp rax, 1
  je .Ltrue309
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 95
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue309
  push 0
  jmp .Lend309
.Ltrue309:
  push 1
.Lend309:
  pop rax
  cmp rax, 1
  je .Ltrue307
  push 0
  jmp .Lend307
.Ltrue307:
  push 1
.Lend307:
  pop rax
  cmp rax, 0
  je .Lend306
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 57
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin311:
  push 97
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse313
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 122
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse313
  push 1
  jmp .Lend313
.Lfalse313:
  push 0
.Lend313:
  pop rax
  cmp rax, 1
  je .Ltrue312
  push 65
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse315
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 90
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse315
  push 1
  jmp .Lend315
.Lfalse315:
  push 0
.Lend315:
  pop rax
  cmp rax, 1
  je .Ltrue314
  push 48
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse317
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 57
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse317
  push 1
  jmp .Lend317
.Lfalse317:
  push 0
.Lend317:
  pop rax
  cmp rax, 1
  je .Ltrue316
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 95
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue316
  push 0
  jmp .Lend316
.Ltrue316:
  push 1
.Lend316:
  pop rax
  cmp rax, 1
  je .Ltrue314
  push 0
  jmp .Lend314
.Ltrue314:
  push 1
.Lend314:
  pop rax
  cmp rax, 1
  je .Ltrue312
  push 0
  jmp .Lend312
.Ltrue312:
  push 1
.Lend312:
  pop rax
  cmp rax, 0
  je .Lend311
  mov rax, rbp
  sub rax, 57
  push rax
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lcontinue42:
  mov rax, rbp
  sub rax, 57
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin311
.Lend311:
.Lbreak45:
  mov rax, rbp
  sub rax, 49
  push rax
  push 2
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 57
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lcontinue37
.Lend306:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC184[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lbegin232
.Lend232:
.Lbreak40:
  mov rax, rbp
  sub rax, 9
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend318
  push 14
  mov rax, rbp
  sub rax, 49
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call new_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rax
.Lend318:
  mov rax, rbp
  sub rax, 41
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
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
.LC187:
  .string "  .string \"%.*s\"\n"
.data
.LC186:
  .string ".LC%d:\n"
.data
.LC185:
  .string ".data\n"
.globl gen_string_literal
.text
gen_string_literal:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin319:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend319
  lea rax, .LC185[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC186[rip]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC187[rip]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lcontinue43:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin319
.Lend319:
.Lbreak46:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC195:
  .string "left value of assignment must be variable: found %d\n"
.data
.LC194:
  .string "  push rax\n"
.data
.LC193:
  .string "  lea rax, .LC%d[rip]\n"
.data
.LC192:
  .string "  push rax\n"
.data
.LC191:
  .string "  lea rax, %.*s[rip]\n"
.data
.LC190:
  .string "  push rax\n"
.data
.LC189:
  .string "  sub rax, %d\n"
.data
.LC188:
  .string "  mov rax, rbp\n"
.globl gen_lval
.text
gen_lval:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 13
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse320
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lend320
.Lelse320:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 27
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse321
  lea rax, .LC188[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC189[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC190[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lend321
.Lelse321:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 28
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse322
  lea rax, .LC191[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC192[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lend322
.Lelse322:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 31
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse323
  lea rax, .LC193[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC194[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lend323
.Lelse323:
  lea rax, .LC195[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend323:
.Lend322:
.Lend321:
.Lend320:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC236:
  .string "  pop rax\n"
.data
.LC235:
  .string "  jmp .Lcontinue%d\n"
.data
.LC234:
  .string "  jmp .Lbreak%d\n"
.data
.LC233:
  .string "  ret\n"
.data
.LC232:
  .string "  pop rbp\n"
.data
.LC231:
  .string "  mov rsp, rbp\n"
.data
.LC230:
  .string "  pop rax\n"
.data
.LC229:
  .string "  push 0\n"
.data
.LC228:
  .string ".Lbreak%d:\n"
.data
.LC227:
  .string ".Lend%d:\n"
.data
.LC226:
  .string "  jmp .Lbegin%d\n"
.data
.LC225:
  .string ".Lcontinue%d:\n"
.data
.LC224:
  .string "  je .Lend%d\n"
.data
.LC223:
  .string "  cmp rax, 0\n"
.data
.LC222:
  .string "  pop rax\n"
.data
.LC221:
  .string ".Lbegin%d:\n"
.data
.LC220:
  .string ".Lbreak%d:\n"
.data
.LC219:
  .string ".Lend%d:\n"
.data
.LC218:
  .string "  jmp .Lbegin%d\n"
.data
.LC217:
  .string "  je .Lend%d\n"
.data
.LC216:
  .string "  cmp rax, 0\n"
.data
.LC215:
  .string "  pop rax\n"
.data
.LC214:
  .string ".Lcontinue%d:\n"
.data
.LC213:
  .string ".Lbegin%d:\n"
.data
.LC212:
  .string ".Ldefault%d:\n"
.data
.LC211:
  .string ".Lcase%d_%d:\n"
.data
.LC210:
  .string ".Lbreak%d:\n"
.data
.LC209:
  .string "  jmp .Ldefault%d\n"
.data
.LC208:
  .string "  je .Lcase%d_%d\n"
.data
.LC207:
  .string "  cmp rax, %d\n"
.data
.LC206:
  .string "  pop rax\n"
.data
.LC205:
  .string ".Lend%d:\n"
.data
.LC204:
  .string ".Lelse%d:\n"
.data
.LC203:
  .string "  jmp .Lend%d\n"
.data
.LC202:
  .string "  je .Lelse%d\n"
.data
.LC201:
  .string "  cmp rax, 0\n"
.data
.LC200:
  .string "  pop rax\n"
.data
.LC199:
  .string ".Lend%d:\n"
.data
.LC198:
  .string "  je .Lend%d\n"
.data
.LC197:
  .string "  cmp rax, 0\n"
.data
.LC196:
  .string "  pop rax\n"
.globl gen_stmt
.text
gen_stmt:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rax
  cmp rax, 26
  je .Lcase4_26
  cmp rax, 17
  je .Lcase4_17
  cmp rax, 18
  je .Lcase4_18
  cmp rax, 19
  je .Lcase4_19
  cmp rax, 20
  je .Lcase4_20
  cmp rax, 21
  je .Lcase4_21
  cmp rax, 24
  je .Lcase4_24
  cmp rax, 25
  je .Lcase4_25
  cmp rax, 16
  je .Lcase4_16
  cmp rax, 22
  je .Lcase4_22
  cmp rax, 23
  je .Lcase4_23
  jmp .Ldefault4
.Lcase4_26:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin324:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend324
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lcontinue44:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin324
.Lend324:
.Lbreak48:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_17:
  mov rax, rbp
  sub rax, 20
  push rax
  lea rax, label_count[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC196[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC197[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC198[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC199[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_18:
  mov rax, rbp
  sub rax, 20
  push rax
  lea rax, label_count[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC200[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC201[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC202[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC203[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC204[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC205[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_19:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 26
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC206[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  lea rax, break_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  lea rax, break_count[rip]
  push rax
  lea rax, max_break_count[rip]
  push rax
  lea rax, max_break_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  lea rax, switch_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, switch_count[rip]
  push rax
  lea rax, max_switch_count[rip]
  push rax
  lea rax, max_switch_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin325:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend325
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse327
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 20
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse327
  push 1
  jmp .Lend327
.Lfalse327:
  push 0
.Lend327:
  pop rax
  cmp rax, 0
  je .Lend326
  lea rax, .LC207[rip]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC208[rip]
  push rax
  lea rax, switch_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lend326:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse329
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 21
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse329
  push 1
  jmp .Lend329
.Lfalse329:
  push 0
.Lend329:
  pop rax
  cmp rax, 0
  je .Lend328
  lea rax, .LC209[rip]
  push rax
  lea rax, switch_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lend328:
.Lcontinue45:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin325
.Lend325:
.Lbreak49:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC210[rip]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, break_count[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, switch_count[rip]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_20:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 30
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC211[rip]
  push rax
  lea rax, switch_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_21:
  lea rax, .LC212[rip]
  push rax
  lea rax, switch_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_24:
  mov rax, rbp
  sub rax, 20
  push rax
  lea rax, label_count[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  lea rax, continue_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, continue_count[rip]
  push rax
  lea rax, max_continue_count[rip]
  push rax
  lea rax, max_continue_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  lea rax, break_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  lea rax, break_count[rip]
  push rax
  lea rax, max_break_count[rip]
  push rax
  lea rax, max_break_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, .LC213[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC214[rip]
  push rax
  lea rax, continue_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC215[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC216[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC217[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC218[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC219[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC220[rip]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, continue_count[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, break_count[rip]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_25:
  mov rax, rbp
  sub rax, 20
  push rax
  lea rax, label_count[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  lea rax, continue_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 32
  push rax
  lea rax, continue_count[rip]
  push rax
  lea rax, max_continue_count[rip]
  push rax
  lea rax, max_continue_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 36
  push rax
  lea rax, break_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  lea rax, break_count[rip]
  push rax
  lea rax, max_break_count[rip]
  push rax
  lea rax, max_break_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend330
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend330:
  lea rax, .LC221[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend331
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC222[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC223[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC224[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lend331:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC225[rip]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend332
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend332:
  lea rax, .LC226[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC227[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC228[rip]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, continue_count[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, break_count[rip]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_16:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse333
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lend333
.Lelse333:
  lea rax, .LC229[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lend333:
  lea rax, .LC230[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC231[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC232[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC233[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_22:
  lea rax, .LC234[rip]
  push rax
  lea rax, break_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase4_23:
  lea rax, .LC235[rip]
  push rax
  lea rax, continue_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Ldefault4:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC236[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lbreak47:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC268:
  .string "  ret\n"
.data
.LC267:
  .string "  pop rbp\n"
.data
.LC266:
  .string "  mov rsp, rbp\n"
.data
.LC265:
  .string "invalid size"
.data
.LC264:
  .string "  mov [rbp - %d], %s\n"
.data
.LC263:
  .string "  mov [rbp - %d], %s\n"
.data
.LC262:
  .string "  mov [rbp - %d], %s\n"
.data
.LC261:
  .string "  push %s\n"
.data
.LC260:
  .string "  sub rsp, %d\n"
.data
.LC259:
  .string "  mov rbp, rsp\n"
.data
.LC258:
  .string "  push rbp\n"
.data
.LC257:
  .string "%.*s:\n"
.data
.LC256:
  .string ".text\n"
.data
.LC255:
  .string ".globl %.*s\n"
.data
.LC254:
  .string "r9"
.data
.LC253:
  .string "r8"
.data
.LC252:
  .string "rcx"
.data
.LC251:
  .string "rdx"
.data
.LC250:
  .string "rsi"
.data
.LC249:
  .string "rdi"
.data
.LC248:
  .string "r9d"
.data
.LC247:
  .string "r8d"
.data
.LC246:
  .string "ecx"
.data
.LC245:
  .string "edx"
.data
.LC244:
  .string "esi"
.data
.LC243:
  .string "edi"
.data
.LC242:
  .string "r9b"
.data
.LC241:
  .string "r8b"
.data
.LC240:
  .string "cl"
.data
.LC239:
  .string "dl"
.data
.LC238:
  .string "sil"
.data
.LC237:
  .string "dil"
.globl gen_function
.text
gen_function:
  push rbp
  mov rbp, rsp
  sub rsp, 176
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 56
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC237[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC238[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC239[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC240[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC241[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC242[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 0
  pop rax
  mov rax, rbp
  sub rax, 104
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC243[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 104
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC244[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 104
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC245[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 104
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC246[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 104
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC247[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 104
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC248[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 0
  pop rax
  mov rax, rbp
  sub rax, 152
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC249[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 152
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC250[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 152
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC251[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 152
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC252[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 152
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC253[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 152
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC254[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_string_literal
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC255[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC256[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC257[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC258[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC259[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC260[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 68
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend334
  mov rax, rbp
  sub rax, 156
  push rax
  push 5
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin335:
  push 0
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend335
  lea rax, .LC261[rip]
  push rax
  mov rax, rbp
  sub rax, 152
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lcontinue46:
  mov rax, rbp
  sub rax, 156
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  sub edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin335
.Lend335:
.Lbreak50:
.Lend334:
  mov rax, rbp
  sub rax, 156
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 160
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin336:
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 6
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse337
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse337
  push 1
  jmp .Lend337
.Lfalse337:
  push 0
.Lend337:
  pop rax
  cmp rax, 0
  je .Lend336
  mov rax, rbp
  sub rax, 164
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 168
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse338
  mov rax, rbp
  sub rax, 164
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend338
.Lelse338:
  mov rax, rbp
  sub rax, 164
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lend338:
.Lbegin339:
  mov rax, rbp
  sub rax, 164
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend339
  push 8
  mov rax, rbp
  sub rax, 164
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse340
  lea rax, .LC262[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 152
  push rax
  mov rax, rbp
  sub rax, 160
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 168
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend340
.Lelse340:
  mov rax, rbp
  sub rax, 164
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse341
  lea rax, .LC263[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 104
  push rax
  mov rax, rbp
  sub rax, 160
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 168
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend341
.Lelse341:
  mov rax, rbp
  sub rax, 164
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse342
  lea rax, .LC264[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rbp
  sub rax, 160
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 168
  push rax
  mov rax, rbp
  sub rax, 168
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend342
.Lelse342:
  lea rax, .LC265[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend342:
.Lend341:
.Lend340:
.Lcontinue48:
  mov rax, rbp
  sub rax, 160
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin339
.Lend339:
.Lbreak52:
.Lcontinue47:
  mov rax, rbp
  sub rax, 156
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin336
.Lend336:
.Lbreak51:
  lea rax, arg_count[rip]
  push rax
  mov rax, rbp
  sub rax, 156
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, current_stack_size[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_stmt
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC266[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC267[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC268[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC395:
  .string "  push rax\n"
.data
.LC394:
  .string "unreachable"
.data
.LC393:
  .string "  movzb rax, al\n"
.data
.LC392:
  .string "  setle al\n"
.data
.LC391:
  .string "  cmp rax, rdi\n"
.data
.LC390:
  .string "  movzb rax, al\n"
.data
.LC389:
  .string "  setl al\n"
.data
.LC388:
  .string "  cmp rax, rdi\n"
.data
.LC387:
  .string "  movzb rax, al\n"
.data
.LC386:
  .string "  setne al\n"
.data
.LC385:
  .string "  cmp rax, rdi\n"
.data
.LC384:
  .string "  movzb rax, al\n"
.data
.LC383:
  .string "  sete al\n"
.data
.LC382:
  .string "  cmp rax, rdi\n"
.data
.LC381:
  .string "  idiv rdi\n"
.data
.LC380:
  .string "  cqo\n"
.data
.LC379:
  .string "  imul rax, rdi\n"
.data
.LC378:
  .string "  sub rax, rdi\n"
.data
.LC377:
  .string "  add rax, rdi\n"
.data
.LC376:
  .string "  pop rax\n"
.data
.LC375:
  .string "  pop rdi\n"
.data
.LC374:
  .string ".Lend%d:\n"
.data
.LC373:
  .string "  push 1\n"
.data
.LC372:
  .string ".Ltrue%d:\n"
.data
.LC371:
  .string "  jmp .Lend%d\n"
.data
.LC370:
  .string "  push 0\n"
.data
.LC369:
  .string "  je .Ltrue%d\n"
.data
.LC368:
  .string "  cmp rax, 1\n"
.data
.LC367:
  .string "  pop rax\n"
.data
.LC366:
  .string "  je .Ltrue%d\n"
.data
.LC365:
  .string "  cmp rax, 1\n"
.data
.LC364:
  .string "  pop rax\n"
.data
.LC363:
  .string ".Lend%d:\n"
.data
.LC362:
  .string "  push 0\n"
.data
.LC361:
  .string ".Lfalse%d:\n"
.data
.LC360:
  .string "  jmp .Lend%d\n"
.data
.LC359:
  .string "  push 1\n"
.data
.LC358:
  .string "  je .Lfalse%d\n"
.data
.LC357:
  .string "  cmp rax, 0\n"
.data
.LC356:
  .string "  pop rax\n"
.data
.LC355:
  .string "  je .Lfalse%d\n"
.data
.LC354:
  .string "  cmp rax, 0\n"
.data
.LC353:
  .string "  pop rax\n"
.data
.LC352:
  .string "  push rax\n"
.data
.LC351:
  .string "unexpected type"
.data
.LC350:
  .string "  movsx rax, BYTE PTR [rax]\n"
.data
.LC349:
  .string "  mov rax, [rax]\n"
.data
.LC348:
  .string "  mov eax, [rax]\n"
.data
.LC347:
  .string "  pop rax\n"
.data
.LC346:
  .string "  push 0\n"
.data
.LC345:
  .string "  mov [rbp - %d], rax\n"
.data
.LC344:
  .string "  lea rax, [rbp - %d]\n"
.data
.LC343:
  .string "  mov [rbp - %d], rax\n"
.data
.LC342:
  .string "  lea rax, [rbp + 16]\n"
.data
.LC341:
  .string "  mov [rbp - %d], eax\n"
.data
.LC340:
  .string "  mov eax, 48\n"
.data
.LC339:
  .string "  mov [rbp - %d], eax\n"
.data
.LC338:
  .string "  mov eax, %d\n"
.data
.LC337:
  .string "not implemented: return value"
.data
.LC336:
  .string "  push rax\n"
.data
.LC335:
  .string "  push rax\n"
.data
.LC334:
  .string "  movsx rax, eax\n"
.data
.LC333:
  .string "  push rax\n"
.data
.LC332:
  .string "  movsx rax, al\n"
.data
.LC331:
  .string "  push 0\n"
.data
.LC330:
  .string "  mov rsp, rdi\n"
.data
.LC329:
  .string "  pop rdi\n"
.data
.LC328:
  .string "  pop rdi\n"
.data
.LC327:
  .string "  call %.*s\n"
.data
.LC326:
  .string "  mov rax, 0\n"
.data
.LC325:
  .string "  push 0\n"
.data
.LC324:
  .string "  push r10\n"
.data
.LC323:
  .string "  and rsp, 0xfffffffffffffff0\n"
.data
.LC322:
  .string "  mov r10, rsp\n"
.data
.LC321:
  .string "  pop %s\n"
.data
.LC320:
  .string "r9"
.data
.LC319:
  .string "r8"
.data
.LC318:
  .string "rcx"
.data
.LC317:
  .string "rdx"
.data
.LC316:
  .string "rsi"
.data
.LC315:
  .string "rdi"
.data
.LC314:
  .string "too many arguments"
.data
.LC313:
  .string "not implemented: too big object"
.data
.LC312:
  .string "  mov [rax], dil\n"
.data
.LC311:
  .string "  sub dil, 1\n"
.data
.LC310:
  .string "  push rdi\n"
.data
.LC309:
  .string "  mov dil, [rax]\n"
.data
.LC308:
  .string "  mov [rax], edi\n"
.data
.LC307:
  .string "  sub edi, 1\n"
.data
.LC306:
  .string "  push rdi\n"
.data
.LC305:
  .string "  mov edi, [rax]\n"
.data
.LC304:
  .string "  mov [rax], rdi\n"
.data
.LC303:
  .string "  sub rdi, 1\n"
.data
.LC302:
  .string "  push rdi\n"
.data
.LC301:
  .string "  mov rdi, [rax]\n"
.data
.LC300:
  .string " pop rax\n"
.data
.LC299:
  .string "  mov [rax], dil\n"
.data
.LC298:
  .string "  add dil, 1\n"
.data
.LC297:
  .string "  push rdi\n"
.data
.LC296:
  .string "  mov dil, [rax]\n"
.data
.LC295:
  .string "  mov [rax], edi\n"
.data
.LC294:
  .string "  add edi, 1\n"
.data
.LC293:
  .string "  push rdi\n"
.data
.LC292:
  .string "  mov edi, [rax]\n"
.data
.LC291:
  .string "  mov [rax], rdi\n"
.data
.LC290:
  .string "  add rdi, 1\n"
.data
.LC289:
  .string "  push rdi\n"
.data
.LC288:
  .string "  mov rdi, [rax]\n"
.data
.LC287:
  .string " pop rax\n"
.data
.LC286:
  .string "  push 0\n"
.data
.LC285:
  .string "  pop rax\n"
.data
.LC284:
  .string "  push rdi\n"
.data
.LC283:
  .string "failed to assign"
.data
.LC282:
  .string "  mov [rax], rdi\n"
.data
.LC281:
  .string "  mov BYTE PTR [rax], dil\n"
.data
.LC280:
  .string "  mov DWORD PTR [rax], edi\n"
.data
.LC279:
  .string "  pop rax\n"
.data
.LC278:
  .string "  pop rdi\n"
.data
.LC277:
  .string "not implemented: size %d"
.data
.LC276:
  .string "  push rax\n"
.data
.LC275:
  .string "  movsx rax, BYTE PTR [rax]\n"
.data
.LC274:
  .string "  push rax\n"
.data
.LC273:
  .string "  movsxd rax, [rax]\n"
.data
.LC272:
  .string "  push r10\n"
.data
.LC271:
  .string "  mov r10, [rax + %d]\n"
.data
.LC270:
  .string "  pop rax\n"
.data
.LC269:
  .string "  push %d\n"
.globl gen
.text
gen:
  push rbp
  mov rbp, rsp
  sub rsp, 88
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rax
  cmp rax, 30
  je .Lcase5_30
  cmp rax, 27
  je .Lcase5_27
  cmp rax, 28
  je .Lcase5_28
  cmp rax, 31
  je .Lcase5_31
  cmp rax, 4
  je .Lcase5_4
  cmp rax, 5
  je .Lcase5_5
  cmp rax, 14
  je .Lcase5_14
  cmp rax, 15
  je .Lcase5_15
  cmp rax, 29
  je .Lcase5_29
  cmp rax, 32
  je .Lcase5_32
  cmp rax, 12
  je .Lcase5_12
  cmp rax, 13
  je .Lcase5_13
  cmp rax, 10
  je .Lcase5_10
  cmp rax, 11
  je .Lcase5_11
  jmp .Ldefault5
.Lcase5_30:
  lea rax, .LC269[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_27:
.Lcase5_28:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_lval
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC270[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin343:
  push 8
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse344
  lea rax, .LC271[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC272[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend344
.Lelse344:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse345
  lea rax, .LC273[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC274[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend345
.Lelse345:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse346
  lea rax, .LC275[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC276[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  jmp .Lend346
.Lelse346:
  lea rax, .LC277[rip]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend346:
.Lend345:
.Lend344:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend347
  jmp .Lbreak54
.Lend347:
.Lcontinue49:
  jmp .Lbegin343
.Lend343:
.Lbreak54:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_31:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_lval
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_4:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_lval
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC278[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC279[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  cmp rax, 4
  je .Lcase6_4
  cmp rax, 1
  je .Lcase6_1
  cmp rax, 8
  je .Lcase6_8
  jmp .Ldefault6
.Lcase6_4:
  lea rax, .LC280[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak55
.Lcase6_1:
  lea rax, .LC281[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak55
.Lcase6_8:
  lea rax, .LC282[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak55
.Ldefault6:
  lea rax, .LC283[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lbreak55:
  lea rax, .LC284[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_5:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin348:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend348
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC285[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lcontinue50:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin348
.Lend348:
.Lbreak56:
  lea rax, .LC286[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_14:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_lval
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC287[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  cmp rax, 8
  je .Lcase7_8
  cmp rax, 4
  je .Lcase7_4
  cmp rax, 1
  je .Lcase7_1
.Lcase7_8:
  lea rax, .LC288[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC289[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC290[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC291[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak57
.Lcase7_4:
  lea rax, .LC292[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC293[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC294[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC295[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak57
.Lcase7_1:
  lea rax, .LC296[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC297[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC298[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC299[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak57
.Lbreak57:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_15:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_lval
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC300[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  cmp rax, 8
  je .Lcase8_8
  cmp rax, 4
  je .Lcase8_4
  cmp rax, 1
  je .Lcase8_1
.Lcase8_8:
  lea rax, .LC301[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC302[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC303[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC304[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak58
.Lcase8_4:
  lea rax, .LC305[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC306[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC307[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC308[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak58
.Lcase8_1:
  lea rax, .LC309[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC310[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC311[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC312[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak58
.Lbreak58:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_29:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin349:
.Lcontinue51:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend349
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  push 32
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend350
  lea rax, .LC313[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend350:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 7
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 8
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lbegin349
.Lend349:
.Lbreak59:
  push 6
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend351
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, .LC314[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend351:
  mov rax, rbp
  sub rax, 76
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC315[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 76
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC316[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 76
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC317[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 76
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC318[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 76
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC319[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 76
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC320[rip]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 0
  pop rax
  mov rax, rbp
  sub rax, 80
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin352:
  push 0
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend352
  lea rax, .LC321[rip]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lcontinue52:
  mov rax, rbp
  sub rax, 80
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  sub edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin352
.Lend352:
.Lbreak60:
  lea rax, .LC322[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC323[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC324[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC325[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC326[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC327[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC328[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC329[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC330[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend353
  lea rax, .LC331[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend353:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  cmp rax, 1
  je .Lcase9_1
  cmp rax, 4
  je .Lcase9_4
  cmp rax, 8
  je .Lcase9_8
  jmp .Ldefault9
.Lcase9_1:
  lea rax, .LC332[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC333[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak61
.Lcase9_4:
  lea rax, .LC334[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC335[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak61
.Lcase9_8:
  lea rax, .LC336[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak61
.Ldefault9:
  lea rax, .LC337[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lbreak61:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_32:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 27
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call assert
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC338[rip]
  push rax
  lea rax, arg_count[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC339[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC340[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC341[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 4
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC342[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC343[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC344[rip]
  push rax
  lea rax, current_stack_size[rip]
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC345[rip]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  push 16
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC346[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_12:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_lval
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_13:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC347[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call sizeof_type
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  cmp rax, 4
  je .Lcase10_4
  cmp rax, 8
  je .Lcase10_8
  cmp rax, 1
  je .Lcase10_1
  jmp .Ldefault10
.Lcase10_4:
  lea rax, .LC348[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak62
.Lcase10_8:
  lea rax, .LC349[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak62
.Lcase10_1:
  lea rax, .LC350[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak62
.Ldefault10:
  lea rax, .LC351[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lbreak62:
  lea rax, .LC352[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_10:
  mov rax, rbp
  sub rax, 12
  push rax
  lea rax, label_count[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC353[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC354[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC355[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC356[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC357[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC358[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC359[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC360[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC361[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC362[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC363[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase5_11:
  mov rax, rbp
  sub rax, 12
  push rax
  lea rax, label_count[rip]
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC364[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC365[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC366[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC367[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC368[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC369[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC370[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC371[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC372[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC373[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC374[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Ldefault5:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  lea rax, .LC375[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC376[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lcase11_0
  cmp rax, 1
  je .Lcase11_1
  cmp rax, 2
  je .Lcase11_2
  cmp rax, 3
  je .Lcase11_3
  cmp rax, 6
  je .Lcase11_6
  cmp rax, 7
  je .Lcase11_7
  cmp rax, 8
  je .Lcase11_8
  cmp rax, 9
  je .Lcase11_9
  jmp .Ldefault11
.Lcase11_0:
  lea rax, .LC377[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Lcase11_1:
  lea rax, .LC378[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Lcase11_2:
  lea rax, .LC379[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Lcase11_3:
  lea rax, .LC380[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC381[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Lcase11_6:
  lea rax, .LC382[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC383[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC384[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Lcase11_7:
  lea rax, .LC385[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC386[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC387[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Lcase11_8:
  lea rax, .LC388[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC389[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC390[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Lcase11_9:
  lea rax, .LC391[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC392[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC393[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  jmp .Lbreak63
.Ldefault11:
  lea rax, .LC394[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lbreak63:
  lea rax, .LC395[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lbreak53:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC396:
  .string "\n"
.globl error
.text
error:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  push r9
  push r8
  push rcx
  push rdx
  push rsi
  push rdi
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop rax
  mov eax, 8
  mov [rbp - 32], eax
  mov eax, 48
  mov [rbp - 28], eax
  lea rax, [rbp + 16]
  mov [rbp - 24], rax
  lea rax, [rbp - 88]
  mov [rbp - 16], rax
  push 0
  pop rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call vfprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC396[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 1
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call exit
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC402:
  .string "\n"
.data
.LC401:
  .string "^ "
.data
.LC400:
  .string ""
.data
.LC399:
  .string "%*s"
.data
.LC398:
  .string "%.*s\n"
.data
.LC397:
  .string "%s:%d: "
.globl _error_at
.text
_error_at:
  push rbp
  mov rbp, rsp
  sub rsp, 80
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov [rbp - 40], rdx
  mov [rbp - 32], rcx
  mov [rbp - 24], r8
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin354:
.Lcontinue53:
  lea rax, user_input[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse355
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push -1
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse355
  push 1
  jmp .Lend355
.Lfalse355:
  push 0
.Lend355:
  pop rax
  cmp rax, 0
  je .Lend354
  mov rax, rbp
  sub rax, 48
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  sub rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin354
.Lend354:
.Lbreak64:
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin356:
.Lcontinue54:
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend356
  mov rax, rbp
  sub rax, 56
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin356
.Lend356:
.Lbreak65:
  mov rax, rbp
  sub rax, 60
  push rax
  push 1
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 68
  push rax
  lea rax, user_input[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin357:
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend357
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend358
  mov rax, rbp
  sub rax, 60
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
.Lend358:
.Lcontinue55:
  mov rax, rbp
  sub rax, 68
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin357
.Lend357:
.Lbreak66:
  mov rax, rbp
  sub rax, 72
  push rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC397[rip]
  push rax
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC398[rip]
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 76
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC399[rip]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  lea rax, .LC400[rip]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC401[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 40
  push rax
  push 0
  push 24
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call vfprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC402[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fprintf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  push 1
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call exit
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.globl error_at
.text
error_at:
  push rbp
  mov rbp, rsp
  sub rsp, 48
  push r9
  push r8
  push rcx
  push rdx
  push rsi
  push rdi
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop rax
  mov eax, 16
  mov [rbp - 40], eax
  mov eax, 48
  mov [rbp - 36], eax
  lea rax, [rbp + 16]
  mov [rbp - 32], rax
  lea rax, [rbp - 96]
  mov [rbp - 24], rax
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call _error_at
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.globl error_at_token
.text
error_at_token:
  push rbp
  mov rbp, rsp
  sub rsp, 48
  push r9
  push r8
  push rcx
  push rdx
  push rsi
  push rdi
  mov [rbp - 8], rdi
  mov [rbp - 16], rsi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop rax
  mov eax, 16
  mov [rbp - 40], eax
  mov eax, 48
  mov [rbp - 36], eax
  lea rax, [rbp + 16]
  mov [rbp - 32], rax
  lea rax, [rbp - 96]
  mov [rbp - 24], rax
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call _error_at
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.globl error_at_here
.text
error_at_here:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  push r9
  push r8
  push rcx
  push rdx
  push rsi
  push rdi
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop rax
  mov eax, 8
  mov [rbp - 32], eax
  mov eax, 48
  mov [rbp - 28], eax
  lea rax, [rbp + 16]
  mov [rbp - 24], rax
  lea rax, [rbp - 88]
  mov [rbp - 16], rax
  push 0
  pop rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov r10, [rax + 8]
  push r10
  mov r10, [rax + 16]
  push r10
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call _error_at
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.data
.LC403:
  .string "assertion failed"
.globl assert
.text
assert:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp - 1], dil
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend359
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC403[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error_at_token
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend359:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC407:
  .string "%s: fseek"
.data
.LC406:
  .string "%s: fseek"
.data
.LC405:
  .string "cannot open %s"
.data
.LC404:
  .string "r"
.globl read_file
.text
read_file:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC404[rip]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fopen
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend360
  lea rax, .LC405[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend360:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  push 2
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fseek
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push -1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend361
  lea rax, .LC406[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend361:
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call ftell
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fseek
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push -1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend362
  lea rax, .LC407[rip]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend362:
  mov rax, rbp
  sub rax, 28
  push rax
  push 1
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fread
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue364
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue364
  push 0
  jmp .Lend364
.Ltrue364:
  push 1
.Lend364:
  pop rax
  cmp rax, 0
  je .Lend363
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 20
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 10
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
.Lend363:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call fclose
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC408:
  .string "."
.globl get_dir
.text
get_dir:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strlen
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin365:
  push 0
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend365
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 47
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend366
  jmp .Lbreak67
.Lend366:
.Lcontinue56:
  mov rax, rbp
  sub rax, 12
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  sub edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin365
.Lend365:
.Lbreak67:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push -1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend367
  mov rax, rbp
  sub rax, 20
  push rax
  push 1
  push 2
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC408[rip]
  push rax
  push 1
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncpy
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend367:
  mov rax, rbp
  sub rax, 20
  push rax
  push 1
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call calloc
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strncpy
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.data
.LC417:
  .string "  .zero %d\n"
.data
.LC416:
  .string "%.*s:\n"
.data
.LC415:
  .string ".data\n"
.data
.LC414:
  .string ".globl %.*s\n"
.data
.LC413:
  .string "  .zero 8\n"
.data
.LC412:
  .string "NULL:\n"
.data
.LC411:
  .string ".data\n"
.data
.LC410:
  .string ".intel_syntax noprefix\n"
.data
.LC409:
  .string "invalid argument"
.globl main
.text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 4], edi
  mov [rbp - 12], rsi
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend368
  lea rax, .LC409[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call error
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend368:
  lea rax, filename[rip]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, dir_name[rip]
  push rax
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call get_dir
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, user_input[rip]
  push rax
  lea rax, filename[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call read_file
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, token[rip]
  push rax
  lea rax, user_input[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 1
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call tokenize
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, token[rip]
  push rax
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call preprocess
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, .LC410[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC411[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC412[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC413[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lbegin369:
.Lcontinue57:
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call at_eof
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend369
  mov rax, rbp
  sub rax, 20
  push rax
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call external
  pop rdi
  pop rdi
  mov rsp, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lcase12_0
  cmp rax, 2
  je .Lcase12_2
  jmp .Ldefault12
.Lcase12_0:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call gen_function
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
  jmp .Lbreak69
.Lcase12_2:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 69
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend370
  lea rax, .LC414[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC415[rip]
  push rax
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC416[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  lea rax, .LC417[rip]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov eax, [rax]
  push rax
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call printf
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
.Lend370:
  jmp .Lbreak69
.Ldefault12:
  jmp .Lbreak69
.Lbreak69:
  jmp .Lbegin369
.Lend369:
.Lbreak68:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
