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
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  lea rax, macros_list[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin7:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend7
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
.Lend8:
.Lcontinue1:
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
  sub rsp, 40
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
  mov rax, rbp
  sub rax, 32
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
.Lbegin9:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend9
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
  jmp .Lend11
.Lelse11:
  mov rax, rbp
  sub rax, 24
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
.Lend11:
  jmp .Lend10
.Lelse10:
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
.Lend10:
.Lcontinue2:
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
  jmp .Lbegin9
.Lend9:
.Lbreak3:
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
  push rbp
  mov rbp, rsp
  sub rsp, 136
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
  mov rax, rbp
  sub rax, 48
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
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call strndup
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 56
  push rax
  push 1
  push 512
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
  sub rax, 56
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 512
  lea rax, .LC2[rip]
  push rax
  lea rax, dir_name[rip]
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
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 56
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
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 64
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
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 72
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
  sub rax, 72
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend15
  mov rax, rbp
  sub rax, 80
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
.Lbegin16:
.Lcontinue4:
  mov rax, rbp
  sub rax, 80
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
  sub rax, 80
  push rax
  mov rax, rbp
  sub rax, 80
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
  jmp .Lend17
.Lelse17:
  mov rax, rbp
  sub rax, 24
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
.Lend17:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 80
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
  jmp .Lcontinue3
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
  lea rax, .LC4[rip]
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
  je .Lend19
  mov rax, rbp
  sub rax, 88
  push rax
  lea rax, once_files_list[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lbegin20:
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend20
  mov rax, rbp
  sub rax, 88
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
  je .Lend21
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend21:
.Lcontinue5:
  mov rax, rbp
  sub rax, 88
  push rax
  mov rax, rbp
  sub rax, 88
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
  jmp .Lbegin20
.Lend20:
.Lbreak6:
  mov rax, rbp
  sub rax, 96
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
  sub rax, 96
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 0
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
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, once_files_list[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, once_files_list[rip]
  push rax
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend19:
  jmp .Lcontinue3
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
  cmp rax, 0
  je .Lend22
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
  sub rax, 104
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
  sub rax, 104
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
  sub rax, 104
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
.Lbegin23:
.Lcontinue6:
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
  pop rax
  cmp rax, 0
  je .Lfalse24
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
  je .Lfalse24
  push 1
  jmp .Lend24
.Lfalse24:
  push 0
.Lend24:
  pop rax
  cmp rax, 0
  je .Lend23
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
  mov rax, rbp
  sub rax, 112
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
  sub rax, 112
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
  mov rax, rbp
  sub rax, 104
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, macros_list[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, macros_list[rip]
  push rax
  mov rax, rbp
  sub rax, 104
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
.Lend22:
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
  je .Lend25
  mov rax, rbp
  sub rax, 104
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
  sub rax, 104
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
  je .Lend26
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse27
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
  jmp .Lend27
.Lelse27:
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
.Lend27:
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
.Lend26:
  mov rax, rbp
  sub rax, 120
  push rax
  mov rax, rbp
  sub rax, 104
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
.Lbegin28:
.Lcontinue7:
  mov rax, rbp
  sub rax, 120
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend28
  mov rax, rbp
  sub rax, 128
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
  sub rax, 128
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  mov rax, rbp
  sub rax, 120
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  push 32
  pop rdx
  pop rsi
  pop rdi
  mov r10, rsp
  and rsp, 0xfffffffffffffff0
  push r10
  push 0
  mov rax, 0
  call memcpy
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, eax
  push rax
  pop rax
  mov rax, rbp
  sub rax, 128
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
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse29
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
  sub rax, 128
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .Lend29
.Lelse29:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 128
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.Lend29:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 128
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
  sub rax, 120
  push rax
  mov rax, rbp
  sub rax, 120
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
  jmp .Lbegin28
.Lend28:
.Lbreak8:
  jmp .Lcontinue3
.Lend25:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lelse30
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
  jmp .Lend30
.Lelse30:
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
.Lend30:
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
.Lbegin31:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend31
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
  je .Lend32
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
.Lend32:
.Lcontinue8:
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
  jmp .Lbegin31
.Lend31:
.Lbreak9:
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
  je .Lelse33
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
  jmp .Lend33
.Lelse33:
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
.Lend33:
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
.Lbegin34:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend34
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
  je .Lend35
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
.Lend35:
.Lcontinue9:
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
  jmp .Lbegin34
.Lend34:
.Lbreak10:
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
.Lbegin38:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend38
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
  je .Lend39
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
.Lend39:
.Lcontinue11:
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
  jmp .Lbegin38
.Lend38:
.Lbreak12:
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
.Lbegin40:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend40
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
  je .Lend41
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
.Lend41:
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
  jmp .Lbegin40
.Lend40:
.Lbreak13:
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
.Lbegin42:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend42
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
  je .Lend43
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
.Lend43:
.Lcontinue13:
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
  jmp .Lbegin42
.Lend42:
.Lbreak14:
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
.Lbegin44:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend44
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
  je .Lend45
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
.Lend45:
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
  jmp .Lbegin44
.Lend44:
.Lbreak15:
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
  je .Ltrue47
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
  je .Ltrue47
  push 0
  jmp .Lend47
.Ltrue47:
  push 1
.Lend47:
  pop rax
  cmp rax, 0
  je .Lend46
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend46:
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
  je .Lend48
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
.Lend48:
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
  je .Lend49
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend49:
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
.LC6:
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
  je .Ltrue51
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
  je .Ltrue51
  push 0
  jmp .Lend51
.Ltrue51:
  push 1
.Lend51:
  pop rax
  cmp rax, 0
  je .Lend50
  lea rax, .LC6[rip]
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
.Lend50:
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
.LC7:
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
  je .Lend52
  lea rax, .LC7[rip]
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
.Lend52:
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
.LC8:
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
  je .Lend53
  lea rax, .LC8[rip]
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
.Lend53:
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
  je .Lend54
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
.Lend54:
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
  je .Lend55
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
.Lend55:
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
  je .Lend56
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
.Lend56:
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
  je .Lend57
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
.Lend57:
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
  je .Lend58
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
.Lend58:
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
  je .Lend59
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
  lea rax, .LC9[rip]
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
  je .Lend60
  mov rax, rbp
  sub rax, 12
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin61:
.Lcontinue15:
  lea rax, .LC10[rip]
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
  je .Lend61
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
  je .Lend62
  lea rax, .LC11[rip]
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
.Lend62:
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
  lea rax, .LC12[rip]
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
  jmp .Lbegin61
.Lend61:
.Lbreak16:
.Lend60:
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
  je .Ltrue64
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
  je .Ltrue64
  push 0
  jmp .Lend64
.Ltrue64:
  push 1
.Lend64:
  pop rax
  cmp rax, 0
  je .Lend63
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
  je .Lelse65
  mov rax, rbp
  sub rax, 21
  push rax
  push 1
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lend65
.Lelse65:
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
.Lend65:
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
  lea rax, .LC13[rip]
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
  je .Lelse66
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
  je .Lend67
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
.Lend67:
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
.Lbegin68:
.Lcontinue16:
  lea rax, .LC14[rip]
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
  je .Lend68
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
  je .Lend69
  lea rax, .LC15[rip]
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
.Lend69:
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
  lea rax, .LC16[rip]
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
  jmp .Lbegin68
.Lend68:
.Lbreak17:
  mov rax, rbp
  sub rax, 29
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend70
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
  je .Lelse71
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
  je .Lelse72
  lea rax, .LC17[rip]
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
  jmp .Lend72
.Lelse72:
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
.Lend72:
  jmp .Lend71
.Lelse71:
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
.Lend71:
.Lend70:
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
  jmp .Lend66
.Lelse66:
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
  je .Lend73
  lea rax, .LC18[rip]
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
.Lend73:
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
  je .Lelse74
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
  jmp .Lend74
.Lelse74:
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
.Lend74:
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
.Lend66:
.Lend63:
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
  je .Lend75
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
  je .Lend76
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
.Lend76:
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
.Lend75:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
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
  je .Lelse77
.Lbegin78:
.Lcontinue17:
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
  pop rax
  cmp rax, 0
  je .Lend78
  mov rax, rbp
  sub rax, 12
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin78
.Lend78:
.Lbreak18:
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
  lea rax, .LC21[rip]
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
  jmp .Lend77
.Lelse77:
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
  je .Lend79
  lea rax, .LC22[rip]
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
.Lend79:
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
.Lend77:
  lea rax, .LC23[rip]
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
  je .Lend80
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
.Lbegin81:
.Lcontinue18:
  lea rax, .LC24[rip]
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
  je .Lend81
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
  je .Lend82
  jmp .Lbreak19
.Lend82:
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
  jmp .Lbegin81
.Lend81:
.Lbreak19:
  lea rax, .LC26[rip]
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
.Lend80:
  mov rax, rbp
  sub rax, 32
  push rax
  push 0
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin83:
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
  je .Lend83
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
.Lcontinue19:
  mov rax, rbp
  sub rax, 32
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
.Lbegin84:
.Lcontinue20:
  lea rax, .LC27[rip]
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
  je .Lend84
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
  je .Lend85
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
.Lend85:
  lea rax, .LC29[rip]
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
  jmp .Lbegin84
.Lend84:
.Lbreak21:
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
.LC30:
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
  je .Lend86
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend86:
.Lbegin87:
.Lcontinue21:
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
  je .Lend87
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
  jmp .Lbegin87
.Lend87:
.Lbreak22:
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
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp - 8], rdi
.Lbegin88:
.Lcontinue22:
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
  je .Lend88
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
  jmp .Lbegin88
.Lend88:
.Lbreak23:
  lea rax, .LC32[rip]
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
  lea rax, .LC33[rip]
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
.Lend89:
  lea rax, .LC34[rip]
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
  je .Lend90
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
.Lbegin91:
.Lcontinue23:
  lea rax, .LC35[rip]
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
  je .Lend91
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
  jmp .Lbegin91
.Lend91:
.Lbreak24:
  lea rax, .LC36[rip]
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
.Lend90:
.Lbegin92:
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
  je .Lend92
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
  je .Lend93
  lea rax, .LC38[rip]
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
.Lend93:
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
  jmp .Lbegin92
.Lend92:
.Lbreak25:
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
  je .Lend94
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend94:
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
.LC40:
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
  jmp .Lbreak26
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
  jmp .Lbreak26
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
  jmp .Lbreak26
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
  jmp .Lbreak26
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
  jmp .Lbreak26
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
  jmp .Lbreak26
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
  jmp .Lbreak26
.Lcase2_27:
.Lcase2_28:
  lea rax, .LC40[rip]
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
  jmp .Lbreak26
.Ldefault2:
  jmp .Lbreak26
.Lbreak26:
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
  je .Lend95
  lea rax, .LC41[rip]
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
.Lend95:
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
  je .Lend96
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
.Lend96:
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
  je .Lend97
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
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend97:
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
  je .Ltrue99
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
  je .Ltrue99
  push 0
  jmp .Lend99
.Ltrue99:
  push 1
.Lend99:
  pop rax
  cmp rax, 0
  je .Lend98
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
  je .Lend100
  lea rax, .LC43[rip]
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
.Lend100:
  lea rax, .LC44[rip]
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
.Lend98:
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
  je .Lend101
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
  je .Lend102
  lea rax, .LC45[rip]
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
.Lend102:
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
  lea rax, .LC46[rip]
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
.Lend101:
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
  je .Lend103
  lea rax, .LC47[rip]
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
.Lend103:
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
  je .Lelse104
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
  je .Lend105
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
.Lend105:
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
  je .Lend106
  lea rax, .LC49[rip]
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
  je .Lelse107
  mov rax, rbp
  sub rax, 37
  push rax
  push 1
  pop rdi
  pop rax
  mov BYTE PTR [rax], dil
  push rdi
  pop rax
  jmp .Lend107
.Lelse107:
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
.Lend107:
.Lend106:
  lea rax, .LC50[rip]
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
  je .Lfalse109
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
  je .Lfalse109
  push 1
  jmp .Lend109
.Lfalse109:
  push 0
.Lend109:
  pop rax
  cmp rax, 0
  je .Lend108
.Lbegin110:
  lea rax, .LC51[rip]
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
  je .Lend111
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
  jmp .Lbreak27
.Lend111:
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
  je .Lend112
  lea rax, .LC52[rip]
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
.Lend112:
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
  je .Lend113
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
.Lend113:
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
  lea rax, .LC53[rip]
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
  je .Lend114
  jmp .Lbreak27
.Lend114:
.Lcontinue25:
  jmp .Lbegin110
.Lend110:
.Lbreak27:
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
.Lend108:
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
  je .Lend115
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
.Lend115:
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
  je .Lend116
  lea rax, .LC56[rip]
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
.Lend116:
  jmp .Lend104
.Lelse104:
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
  lea rax, .LC57[rip]
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
.Lend104:
  lea rax, locals[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend117
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
.Lend117:
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
  je .Lelse118
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
  je .Lend119
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
.Lend119:
  lea rax, .LC59[rip]
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
  jmp .Lend118
.Lelse118:
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
  je .Lelse120
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
  jmp .Lend120
.Lelse120:
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
  je .Lelse121
  lea rax, .LC61[rip]
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
  lea rax, .LC62[rip]
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
  je .Lelse122
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
  jmp .Lend122
.Lelse122:
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
.Lend122:
  jmp .Lend121
.Lelse121:
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
  je .Lelse123
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
  lea rax, .LC64[rip]
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
  jmp .Lend123
.Lelse123:
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
  je .Lelse124
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
  jmp .Lend124
.Lelse124:
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
  je .Lelse125
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
  lea rax, .LC68[rip]
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
  je .Lend126
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
.Lend126:
  lea rax, .LC70[rip]
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
  je .Lend127
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
.Lend127:
  lea rax, .LC72[rip]
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
  je .Lend128
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
.Lend128:
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
  jmp .Lend125
.Lelse125:
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
  pop rax
  cmp rax, 0
  je .Lelse129
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
.Lbegin130:
  lea rax, .LC75[rip]
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
  je .Lend130
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
.Lcontinue26:
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
  jmp .Lbegin130
.Lend130:
.Lbreak28:
  jmp .Lend129
.Lelse129:
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
  je .Lelse131
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
  pop rax
  cmp rax, 0
  je .Lelse132
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
  jmp .Lend132
.Lelse132:
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
  jmp .Lend131
.Lelse131:
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
  je .Lelse133
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
  lea rax, .LC78[rip]
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
  jmp .Lend133
.Lelse133:
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
  je .Lelse134
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
  jmp .Lend134
.Lelse134:
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
  lea rax, .LC80[rip]
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
  pop rax
  cmp rax, 0
  je .Lelse135
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
  jmp .Lend135
.Lelse135:
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
  je .Lelse136
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
  jmp .Lend136
.Lelse136:
  lea rax, .LC83[rip]
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
  jmp .Lend137
.Lelse137:
  lea rax, .LC84[rip]
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
  je .Lelse138
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
  jmp .Lend138
.Lelse138:
  lea rax, .LC85[rip]
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
  je .Lend139
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
.Lend139:
.Lend138:
.Lend137:
.Lend136:
.Lend135:
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
.LC87:
  .string "||"
.data
.LC86:
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
.Lbegin140:
  lea rax, .LC86[rip]
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
  jmp .Lend141
.Lelse141:
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
  je .Lelse142
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
  jmp .Lend142
.Lelse142:
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
.Lend142:
.Lend141:
.Lcontinue27:
  jmp .Lbegin140
.Lend140:
.Lbreak29:
  mov rsp, rbp
  pop rbp
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
.Lbegin143:
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
  je .Lelse144
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
  jmp .Lend144
.Lelse144:
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
  je .Lelse145
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
  jmp .Lend145
.Lelse145:
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
.Lend145:
.Lend144:
.Lcontinue28:
  jmp .Lbegin143
.Lend143:
.Lbreak30:
  mov rsp, rbp
  pop rbp
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
.Lbegin146:
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
  je .Lelse147
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
  jmp .Lend147
.Lelse147:
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
  je .Lelse148
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
  jmp .Lend148
.Lelse148:
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
  je .Lelse149
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
  jmp .Lend149
.Lelse149:
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
  je .Lelse150
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
.Lend148:
.Lend147:
.Lcontinue29:
  jmp .Lbegin146
.Lend146:
.Lbreak31:
  mov rsp, rbp
  pop rbp
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
.Lbegin151:
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
  je .Ltrue154
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
  je .Ltrue154
  push 0
  jmp .Lend154
.Ltrue154:
  push 1
.Lend154:
  pop rax
  cmp rax, 0
  je .Lfalse153
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
  je .Lfalse153
  push 1
  jmp .Lend153
.Lfalse153:
  push 0
.Lend153:
  pop rax
  cmp rax, 0
  je .Lend152
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
  je .Lelse155
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
  je .Lend156
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
.Lend156:
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
  jmp .Lend155
.Lelse155:
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
  je .Lend157
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
  je .Lend158
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
.Lend158:
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
.Lend157:
.Lend155:
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
.Lend152:
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
  je .Lelse159
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
  je .Lfalse161
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
  je .Lfalse161
  push 1
  jmp .Lend161
.Lfalse161:
  push 0
.Lend161:
  pop rax
  cmp rax, 0
  je .Lelse160
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
  jmp .Lend160
.Lelse160:
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
.Lend160:
  jmp .Lend159
.Lelse159:
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
  je .Lelse162
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
  je .Lfalse164
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
  je .Lfalse164
  push 1
  jmp .Lend164
.Lfalse164:
  push 0
.Lend164:
  pop rax
  cmp rax, 0
  je .Lelse163
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
  jmp .Lend163
.Lelse163:
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
.Lend163:
  jmp .Lend162
.Lelse162:
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
.Lend162:
.Lend159:
.Lcontinue30:
  jmp .Lbegin151
.Lend151:
.Lbreak32:
  mov rsp, rbp
  pop rbp
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
.Lbegin165:
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
  je .Lelse166
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
  je .Lfalse168
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
  je .Lfalse168
  push 1
  jmp .Lend168
.Lfalse168:
  push 0
.Lend168:
  pop rax
  cmp rax, 0
  je .Lelse167
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
  jmp .Lend167
.Lelse167:
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
.Lend167:
  jmp .Lend166
.Lelse166:
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
  je .Lelse169
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
  je .Lfalse171
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
  je .Lfalse171
  push 1
  jmp .Lend171
.Lfalse171:
  push 0
.Lend171:
  pop rax
  cmp rax, 0
  je .Lelse170
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
  jmp .Lend170
.Lelse170:
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
.Lend170:
  jmp .Lend169
.Lelse169:
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
.Lend169:
.Lend166:
.Lcontinue31:
  jmp .Lbegin165
.Lend165:
.Lbreak33:
  mov rsp, rbp
  pop rbp
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
  push rbp
  mov rbp, rsp
  sub rsp, 48
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
  je .Lelse172
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
  jmp .Lend172
.Lelse172:
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
  je .Lelse173
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
  je .Lelse174
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
  jmp .Lend174
.Lelse174:
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
.Lend174:
  jmp .Lend173
.Lelse173:
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
  je .Lelse175
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
  jmp .Lend175
.Lelse175:
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
  je .Lelse176
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
  je .Lfalse178
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
  je .Lfalse178
  push 1
  jmp .Lend178
.Lfalse178:
  push 0
.Lend178:
  pop rax
  cmp rax, 0
  je .Lend177
  lea rax, .LC104[rip]
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
.Lend177:
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
  je .Lend179
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
.Lend179:
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
  jmp .Lend176
.Lelse176:
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
  je .Lelse180
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
  je .Lend181
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
  je .Lend182
  lea rax, .LC106[rip]
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
.Lend182:
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
.Lend181:
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
  jmp .Lend180
.Lelse180:
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
  je .Lelse183
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
  jmp .Lend183
.Lelse183:
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
  je .Lelse184
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
  jmp .Lend184
.Lelse184:
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
  je .Lend185
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
.Lend185:
.Lend184:
.Lend183:
.Lend180:
.Lend176:
.Lend175:
.Lend173:
.Lend172:
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
.Lbegin186:
  lea rax, .LC110[rip]
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
  je .Lend187
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
  je .Lend188
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
.Lend188:
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
  lea rax, .LC111[rip]
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
  jmp .Lcontinue32
.Lend187:
  lea rax, .LC112[rip]
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
  je .Lend189
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
  je .Lend190
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC113[rip]
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
.Lend190:
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
  je .Lend191
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC114[rip]
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
.Lend191:
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
  je .Lend192
  lea rax, .LC115[rip]
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
.Lend192:
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
  jmp .Lcontinue32
.Lend189:
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
  je .Lend193
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
  je .Lend194
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC117[rip]
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
.Lend194:
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
  je .Lend195
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC118[rip]
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
  je .Lend196
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
  je .Lend197
  lea rax, .LC120[rip]
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
  jmp .Lcontinue32
.Lend193:
  lea rax, .LC121[rip]
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
  jmp .Lcontinue32
.Lend198:
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
  je .Lend199
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
  jmp .Lcontinue32
.Lend199:
  jmp .Lbreak34
.Lcontinue32:
  jmp .Lbegin186
.Lend186:
.Lbreak34:
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
  push rbp
  mov rbp, rsp
  sub rsp, 104
  lea rax, .LC123[rip]
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
  je .Lend200
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
  lea rax, .LC124[rip]
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
.Lend200:
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
  je .Lend201
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
.Lend201:
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
  je .Lend202
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
  je .Lend203
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
.Lend203:
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
  lea rax, .LC125[rip]
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
  lea rax, .LC126[rip]
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
  je .Lelse205
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
  je .Lend206
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC127[rip]
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
.Lend206:
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
.Lbegin207:
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
  je .Lend207
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
  je .Lend208
  lea rax, .LC128[rip]
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
.Lend208:
.Lcontinue33:
  mov rax, rbp
  sub rax, 52
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin207
.Lend207:
.Lbreak35:
  lea rax, .LC129[rip]
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
  jmp .Lend205
.Lelse205:
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
.Lend205:
.Lend204:
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
.Lend202:
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
  je .Lend209
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  lea rax, .LC130[rip]
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
  je .Lelse211
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
  jmp .Lend211
.Lelse211:
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
.Lend211:
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
  lea rax, .LC131[rip]
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
  je .Lend212
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
.Lend212:
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
  je .Lend213
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
.Lend213:
.Lbegin214:
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
  lea rax, .LC133[rip]
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
  jmp .Lbreak36
  jmp .Lend215
.Lelse215:
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
.Lend215:
.Lcontinue34:
  jmp .Lbegin214
.Lend214:
.Lbreak36:
  jmp .Lend210
.Lelse210:
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
  je .Lelse216
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
  jmp .Lend216
.Lelse216:
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
  je .Lelse217
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
  jmp .Lend217
.Lelse217:
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
  je .Lelse218
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
  jmp .Lend218
.Lelse218:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC135[rip]
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
.Lend218:
.Lend217:
.Lend216:
.Lend210:
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
.Lend209:
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
  je .Lend219
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
  je .Lend220
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
  jmp .Lbreak37
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
  jmp .Lbreak37
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
  jmp .Lbreak37
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
  jmp .Lbreak37
.Ldefault3:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC136[rip]
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
.Lbreak37:
.Lend220:
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
.Lend219:
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
  je .Lfalse222
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
  je .Lfalse222
  push 1
  jmp .Lend222
.Lfalse222:
  push 0
.Lend222:
  pop rax
  cmp rax, 1
  je .Ltrue221
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
  je .Lfalse224
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
  je .Lfalse224
  push 1
  jmp .Lend224
.Lfalse224:
  push 0
.Lend224:
  pop rax
  cmp rax, 1
  je .Ltrue223
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
  je .Lfalse226
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
  je .Lfalse226
  push 1
  jmp .Lend226
.Lfalse226:
  push 0
.Lend226:
  pop rax
  cmp rax, 1
  je .Ltrue225
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
  je .Ltrue225
  push 0
  jmp .Lend225
.Ltrue225:
  push 1
.Lend225:
  pop rax
  cmp rax, 1
  je .Ltrue223
  push 0
  jmp .Lend223
.Ltrue223:
  push 1
.Lend223:
  pop rax
  cmp rax, 1
  je .Ltrue221
  push 0
  jmp .Lend221
.Ltrue221:
  push 1
.Lend221:
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
.Lbegin227:
.Lcontinue35:
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
  je .Lend227
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
  je .Lend228
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
  jmp .Lcontinue35
.Lend228:
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
  je .Lend229
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lcontinue35
.Lend229:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC137[rip]
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
  je .Lend230
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
.Lbegin231:
.Lcontinue36:
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
  je .Lend231
  mov rax, rbp
  sub rax, 57
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin231
.Lend231:
.Lbreak39:
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
  jmp .Lcontinue35
.Lend230:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC138[rip]
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
  je .Ltrue233
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC139[rip]
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
  je .Ltrue234
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC140[rip]
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
  je .Ltrue235
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC141[rip]
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
  je .Ltrue235
  push 0
  jmp .Lend235
.Ltrue235:
  push 1
.Lend235:
  pop rax
  cmp rax, 1
  je .Ltrue234
  push 0
  jmp .Lend234
.Ltrue234:
  push 1
.Lend234:
  pop rax
  cmp rax, 1
  je .Ltrue233
  push 0
  jmp .Lend233
.Ltrue233:
  push 1
.Lend233:
  pop rax
  cmp rax, 0
  je .Lend232
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
  jmp .Lcontinue35
.Lend232:
  mov rax, rbp
  sub rax, 8
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
  call startswith
  pop rdi
  pop rdi
  mov rsp, rdi
  movsx rax, al
  push rax
  pop rax
  cmp rax, 1
  je .Ltrue237
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
  cmp rax, 1
  je .Ltrue237
  push 0
  jmp .Lend237
.Ltrue237:
  push 1
.Lend237:
  pop rax
  cmp rax, 0
  je .Lend236
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
  jmp .Lcontinue35
.Lend236:
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
  je .Ltrue239
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
  push 0
  jmp .Lend239
.Ltrue239:
  push 1
.Lend239:
  pop rax
  cmp rax, 0
  je .Lend238
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
  jmp .Lcontinue35
.Lend238:
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
  je .Ltrue241
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
  je .Ltrue242
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
  je .Ltrue243
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
  je .Ltrue243
  push 0
  jmp .Lend243
.Ltrue243:
  push 1
.Lend243:
  pop rax
  cmp rax, 1
  je .Ltrue242
  push 0
  jmp .Lend242
.Ltrue242:
  push 1
.Lend242:
  pop rax
  cmp rax, 1
  je .Ltrue241
  push 0
  jmp .Lend241
.Ltrue241:
  push 1
.Lend241:
  pop rax
  cmp rax, 0
  je .Lend240
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
  jmp .Lcontinue35
.Lend240:
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
  cmp rax, 0
  je .Lend244
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
.Lbegin245:
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
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend245
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin245
.Lend245:
.Lbreak40:
  jmp .Lcontinue35
.Lend244:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC151[rip]
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
  je .Lend246
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
  lea rax, .LC152[rip]
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
  je .Lend247
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
  call error_at
  pop rdi
  pop rdi
  mov rsp, rdi
  push 0
  pop rax
.Lend247:
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
  jmp .Lcontinue35
.Lend246:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC154[rip]
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
  je .Lend248
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
  jmp .Lcontinue35
.Lend248:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC155[rip]
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
  je .Lend249
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
  jmp .Lcontinue35
.Lend249:
  lea rax, .LC156[rip]
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
  je .Lend250
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
  jmp .Lcontinue35
.Lend250:
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
  je .Lend251
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
.Lbegin252:
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
  je .Ltrue253
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
  je .Ltrue253
  push 0
  jmp .Lend253
.Ltrue253:
  push 1
.Lend253:
  pop rax
  cmp rax, 0
  je .Lend252
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
.Lcontinue38:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin252
.Lend252:
.Lbreak41:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lcontinue35
.Lend251:
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
  je .Lend254
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
.Lbegin255:
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
  je .Lfalse257
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
  je .Lfalse257
  push 1
  jmp .Lend257
.Lfalse257:
  push 0
.Lend257:
  pop rax
  cmp rax, 1
  je .Ltrue256
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
  je .Ltrue256
  push 0
  jmp .Lend256
.Ltrue256:
  push 1
.Lend256:
  pop rax
  cmp rax, 0
  je .Lend255
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
.Lcontinue39:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin255
.Lend255:
.Lbreak42:
  mov rax, rbp
  sub rax, 8
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lcontinue35
.Lend254:
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
  je .Lend258
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
  jmp .Lcontinue35
.Lend258:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC157[rip]
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
  je .Lfalse260
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
  je .Lfalse260
  push 1
  jmp .Lend260
.Lfalse260:
  push 0
.Lend260:
  pop rax
  cmp rax, 0
  je .Lend259
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
  jmp .Lcontinue35
.Lend259:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC158[rip]
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
  je .Lfalse262
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
  je .Lfalse262
  push 1
  jmp .Lend262
.Lfalse262:
  push 0
.Lend262:
  pop rax
  cmp rax, 0
  je .Lend261
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
  jmp .Lcontinue35
.Lend261:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC159[rip]
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
  je .Lfalse264
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
  je .Lfalse264
  push 1
  jmp .Lend264
.Lfalse264:
  push 0
.Lend264:
  pop rax
  cmp rax, 0
  je .Lend263
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
  jmp .Lcontinue35
.Lend263:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC160[rip]
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
  je .Lfalse266
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
  je .Lfalse266
  push 1
  jmp .Lend266
.Lfalse266:
  push 0
.Lend266:
  pop rax
  cmp rax, 0
  je .Lend265
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
  jmp .Lcontinue35
.Lend265:
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
  je .Lfalse268
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
  je .Lfalse268
  push 1
  jmp .Lend268
.Lfalse268:
  push 0
.Lend268:
  pop rax
  cmp rax, 0
  je .Lend267
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
  jmp .Lcontinue35
.Lend267:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC162[rip]
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
  je .Lfalse270
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
  je .Lfalse270
  push 1
  jmp .Lend270
.Lfalse270:
  push 0
.Lend270:
  pop rax
  cmp rax, 0
  je .Lend269
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
  jmp .Lcontinue35
.Lend269:
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
  je .Lfalse272
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
  je .Lfalse272
  push 1
  jmp .Lend272
.Lfalse272:
  push 0
.Lend272:
  pop rax
  cmp rax, 0
  je .Lend271
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
  jmp .Lcontinue35
.Lend271:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC164[rip]
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
  je .Lfalse274
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
  je .Lfalse274
  push 1
  jmp .Lend274
.Lfalse274:
  push 0
.Lend274:
  pop rax
  cmp rax, 0
  je .Lend273
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
  jmp .Lcontinue35
.Lend273:
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
  je .Lfalse276
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
  je .Lfalse276
  push 1
  jmp .Lend276
.Lfalse276:
  push 0
.Lend276:
  pop rax
  cmp rax, 0
  je .Lend275
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
  jmp .Lcontinue35
.Lend275:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC166[rip]
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
  je .Lfalse278
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
  je .Lfalse278
  push 1
  jmp .Lend278
.Lfalse278:
  push 0
.Lend278:
  pop rax
  cmp rax, 0
  je .Lend277
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
  jmp .Lcontinue35
.Lend277:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC167[rip]
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
  je .Lfalse280
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
  je .Lfalse280
  push 1
  jmp .Lend280
.Lfalse280:
  push 0
.Lend280:
  pop rax
  cmp rax, 0
  je .Lend279
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
  jmp .Lcontinue35
.Lend279:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC168[rip]
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
  je .Lfalse282
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
  je .Lfalse282
  push 1
  jmp .Lend282
.Lfalse282:
  push 0
.Lend282:
  pop rax
  cmp rax, 0
  je .Lend281
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
  jmp .Lcontinue35
.Lend281:
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
  je .Lfalse284
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
  je .Lfalse284
  push 1
  jmp .Lend284
.Lfalse284:
  push 0
.Lend284:
  pop rax
  cmp rax, 0
  je .Lend283
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
  jmp .Lcontinue35
.Lend283:
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
  je .Lfalse286
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
  je .Lfalse286
  push 1
  jmp .Lend286
.Lfalse286:
  push 0
.Lend286:
  pop rax
  cmp rax, 0
  je .Lend285
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
  jmp .Lcontinue35
.Lend285:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC171[rip]
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
  je .Lfalse288
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
  je .Lfalse288
  push 1
  jmp .Lend288
.Lfalse288:
  push 0
.Lend288:
  pop rax
  cmp rax, 0
  je .Lend287
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
  jmp .Lcontinue35
.Lend287:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC172[rip]
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
  je .Lfalse290
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
  je .Lfalse290
  push 1
  jmp .Lend290
.Lfalse290:
  push 0
.Lend290:
  pop rax
  cmp rax, 0
  je .Lend289
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
  jmp .Lcontinue35
.Lend289:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC173[rip]
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
  je .Lfalse292
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
  je .Lfalse292
  push 1
  jmp .Lend292
.Lfalse292:
  push 0
.Lend292:
  pop rax
  cmp rax, 0
  je .Lend291
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
  jmp .Lcontinue35
.Lend291:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC174[rip]
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
  je .Lfalse294
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
  je .Lfalse294
  push 1
  jmp .Lend294
.Lfalse294:
  push 0
.Lend294:
  pop rax
  cmp rax, 0
  je .Lend293
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
  jmp .Lcontinue35
.Lend293:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC175[rip]
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
  je .Lfalse296
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
  je .Lfalse296
  push 1
  jmp .Lend296
.Lfalse296:
  push 0
.Lend296:
  pop rax
  cmp rax, 0
  je .Lend295
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
  jmp .Lcontinue35
.Lend295:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC176[rip]
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
  je .Lfalse298
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
  je .Lfalse298
  push 1
  jmp .Lend298
.Lfalse298:
  push 0
.Lend298:
  pop rax
  cmp rax, 0
  je .Lend297
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
  jmp .Lcontinue35
.Lend297:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC177[rip]
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
  je .Lfalse300
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
  je .Lfalse300
  push 1
  jmp .Lend300
.Lfalse300:
  push 0
.Lend300:
  pop rax
  cmp rax, 0
  je .Lend299
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
  jmp .Lcontinue35
.Lend299:
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
  je .Lfalse303
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
  je .Lfalse303
  push 1
  jmp .Lend303
.Lfalse303:
  push 0
.Lend303:
  pop rax
  cmp rax, 1
  je .Ltrue302
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
  je .Lfalse305
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
  je .Lfalse305
  push 1
  jmp .Lend305
.Lfalse305:
  push 0
.Lend305:
  pop rax
  cmp rax, 1
  je .Ltrue304
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
  je .Ltrue304
  push 0
  jmp .Lend304
.Ltrue304:
  push 1
.Lend304:
  pop rax
  cmp rax, 1
  je .Ltrue302
  push 0
  jmp .Lend302
.Ltrue302:
  push 1
.Lend302:
  pop rax
  cmp rax, 0
  je .Lend301
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
.Lbegin306:
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
  je .Lfalse308
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
  je .Lfalse310
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
  je .Lfalse310
  push 1
  jmp .Lend310
.Lfalse310:
  push 0
.Lend310:
  pop rax
  cmp rax, 1
  je .Ltrue309
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
  je .Lfalse312
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
  je .Lfalse312
  push 1
  jmp .Lend312
.Lfalse312:
  push 0
.Lend312:
  pop rax
  cmp rax, 1
  je .Ltrue311
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
  je .Ltrue311
  push 0
  jmp .Lend311
.Ltrue311:
  push 1
.Lend311:
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
.Lcontinue40:
  mov rax, rbp
  sub rax, 57
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin306
.Lend306:
.Lbreak43:
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
  jmp .Lcontinue35
.Lend301:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC178[rip]
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
  jmp .Lbegin227
.Lend227:
.Lbreak38:
  mov rax, rbp
  sub rax, 9
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend313
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
.Lend313:
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
.Lbegin314:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend314
  lea rax, .LC179[rip]
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
  lea rax, .LC180[rip]
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
  lea rax, .LC181[rip]
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
.Lcontinue41:
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
  jmp .Lbegin314
.Lend314:
.Lbreak44:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC189:
  .string "left value of assignment must be variable: found %d\n"
.data
.LC188:
  .string "  push rax\n"
.data
.LC187:
  .string "  lea rax, .LC%d[rip]\n"
.data
.LC186:
  .string "  push rax\n"
.data
.LC185:
  .string "  lea rax, %.*s[rip]\n"
.data
.LC184:
  .string "  push rax\n"
.data
.LC183:
  .string "  sub rax, %d\n"
.data
.LC182:
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
  je .Lelse315
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
  jmp .Lend315
.Lelse315:
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
  je .Lelse316
  lea rax, .LC182[rip]
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
  lea rax, .LC183[rip]
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
  lea rax, .LC184[rip]
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
  jmp .Lend316
.Lelse316:
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
  je .Lelse317
  lea rax, .LC185[rip]
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
  lea rax, .LC186[rip]
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
  jmp .Lend317
.Lelse317:
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
  je .Lelse318
  lea rax, .LC187[rip]
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
  jmp .Lend318
.Lelse318:
  lea rax, .LC189[rip]
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
.Lend318:
.Lend317:
.Lend316:
.Lend315:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC230:
  .string "  pop rax\n"
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
  .string "  pop rbp\n"
.data
.LC225:
  .string "  mov rsp, rbp\n"
.data
.LC224:
  .string "  pop rax\n"
.data
.LC223:
  .string "  push 0\n"
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
  .string "  cmp rax, 0\n"
.data
.LC216:
  .string "  pop rax\n"
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
  .string "  cmp rax, 0\n"
.data
.LC209:
  .string "  pop rax\n"
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
  .string "  cmp rax, %d\n"
.data
.LC200:
  .string "  pop rax\n"
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
  .string "  cmp rax, 0\n"
.data
.LC194:
  .string "  pop rax\n"
.data
.LC193:
  .string ".Lend%d:\n"
.data
.LC192:
  .string "  je .Lend%d\n"
.data
.LC191:
  .string "  cmp rax, 0\n"
.data
.LC190:
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
.Lcontinue42:
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
  jmp .Lbegin319
.Lend319:
.Lbreak46:
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
  lea rax, .LC191[rip]
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
  lea rax, .LC192[rip]
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
  lea rax, .LC193[rip]
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
  lea rax, .LC195[rip]
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
  lea rax, .LC196[rip]
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
  lea rax, .LC197[rip]
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
.Lbegin320:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend320
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
  je .Lfalse322
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
  je .Lfalse322
  push 1
  jmp .Lend322
.Lfalse322:
  push 0
.Lend322:
  pop rax
  cmp rax, 0
  je .Lend321
  lea rax, .LC201[rip]
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
  lea rax, .LC202[rip]
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
.Lend321:
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
  je .Lfalse324
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
  je .Lfalse324
  push 1
  jmp .Lend324
.Lfalse324:
  push 0
.Lend324:
  pop rax
  cmp rax, 0
  je .Lend323
  lea rax, .LC203[rip]
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
.Lend323:
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
  jmp .Lbegin320
.Lend320:
.Lbreak47:
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
  lea rax, .LC204[rip]
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
  lea rax, .LC205[rip]
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
  lea rax, .LC206[rip]
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
  lea rax, .LC207[rip]
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
  lea rax, .LC208[rip]
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
  lea rax, .LC209[rip]
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
  lea rax, .LC210[rip]
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
  lea rax, .LC211[rip]
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
  lea rax, .LC212[rip]
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
  je .Lend325
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
.Lend325:
  lea rax, .LC215[rip]
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
  je .Lend326
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
.Lend326:
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
  lea rax, .LC219[rip]
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
  je .Lend327
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
.Lend327:
  lea rax, .LC220[rip]
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
  lea rax, .LC222[rip]
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
  je .Lelse328
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
  jmp .Lend328
.Lelse328:
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
.Lend328:
  lea rax, .LC224[rip]
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
  lea rax, .LC225[rip]
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
  lea rax, .LC226[rip]
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
  lea rax, .LC227[rip]
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
  lea rax, .LC228[rip]
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
  lea rax, .LC229[rip]
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
.Lbreak45:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC262:
  .string "  ret\n"
.data
.LC261:
  .string "  pop rbp\n"
.data
.LC260:
  .string "  mov rsp, rbp\n"
.data
.LC259:
  .string "invalid size"
.data
.LC258:
  .string "  mov [rbp - %d], %s\n"
.data
.LC257:
  .string "  mov [rbp - %d], %s\n"
.data
.LC256:
  .string "  mov [rbp - %d], %s\n"
.data
.LC255:
  .string "  push %s\n"
.data
.LC254:
  .string "  sub rsp, %d\n"
.data
.LC253:
  .string "  mov rbp, rsp\n"
.data
.LC252:
  .string "  push rbp\n"
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
  .string "r9"
.data
.LC247:
  .string "r8"
.data
.LC246:
  .string "rcx"
.data
.LC245:
  .string "rdx"
.data
.LC244:
  .string "rsi"
.data
.LC243:
  .string "rdi"
.data
.LC242:
  .string "r9d"
.data
.LC241:
  .string "r8d"
.data
.LC240:
  .string "ecx"
.data
.LC239:
  .string "edx"
.data
.LC238:
  .string "esi"
.data
.LC237:
  .string "edi"
.data
.LC236:
  .string "r9b"
.data
.LC235:
  .string "r8b"
.data
.LC234:
  .string "cl"
.data
.LC233:
  .string "dl"
.data
.LC232:
  .string "sil"
.data
.LC231:
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
  lea rax, .LC231[rip]
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
  lea rax, .LC232[rip]
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
  lea rax, .LC233[rip]
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
  lea rax, .LC234[rip]
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
  lea rax, .LC235[rip]
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
  lea rax, .LC236[rip]
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
  lea rax, .LC237[rip]
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
  lea rax, .LC238[rip]
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
  lea rax, .LC239[rip]
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
  lea rax, .LC240[rip]
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
  lea rax, .LC241[rip]
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
  sub rax, 152
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
  sub rax, 152
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
  sub rax, 152
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
  sub rax, 152
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
  sub rax, 152
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
  sub rax, 152
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
  lea rax, .LC249[rip]
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
  lea rax, .LC250[rip]
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
  lea rax, .LC251[rip]
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
  lea rax, .LC252[rip]
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
  lea rax, .LC253[rip]
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
  lea rax, .LC254[rip]
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
  je .Lend329
  mov rax, rbp
  sub rax, 156
  push rax
  push 5
  pop rdi
  pop rax
  mov DWORD PTR [rax], edi
  push rdi
  pop rax
.Lbegin330:
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
  je .Lend330
  lea rax, .LC255[rip]
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
.Lcontinue44:
  mov rax, rbp
  sub rax, 156
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  sub edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin330
.Lend330:
.Lbreak48:
.Lend329:
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
.Lbegin331:
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
  je .Lfalse332
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
  je .Lfalse332
  push 1
  jmp .Lend332
.Lfalse332:
  push 0
.Lend332:
  pop rax
  cmp rax, 0
  je .Lend331
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
  je .Lelse333
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
  jmp .Lend333
.Lelse333:
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
.Lend333:
.Lbegin334:
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
  je .Lend334
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
  je .Lelse335
  lea rax, .LC256[rip]
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
  jmp .Lend335
.Lelse335:
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
  je .Lelse336
  lea rax, .LC257[rip]
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
  jmp .Lend336
.Lelse336:
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
  je .Lelse337
  lea rax, .LC258[rip]
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
  jmp .Lend337
.Lelse337:
  lea rax, .LC259[rip]
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
.Lend337:
.Lend336:
.Lend335:
.Lcontinue46:
  mov rax, rbp
  sub rax, 160
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin334
.Lend334:
.Lbreak50:
.Lcontinue45:
  mov rax, rbp
  sub rax, 156
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin331
.Lend331:
.Lbreak49:
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
  lea rax, .LC260[rip]
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
  lea rax, .LC261[rip]
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
  lea rax, .LC262[rip]
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
.LC389:
  .string "  push rax\n"
.data
.LC388:
  .string "unreachable"
.data
.LC387:
  .string "  movzb rax, al\n"
.data
.LC386:
  .string "  setle al\n"
.data
.LC385:
  .string "  cmp rax, rdi\n"
.data
.LC384:
  .string "  movzb rax, al\n"
.data
.LC383:
  .string "  setl al\n"
.data
.LC382:
  .string "  cmp rax, rdi\n"
.data
.LC381:
  .string "  movzb rax, al\n"
.data
.LC380:
  .string "  setne al\n"
.data
.LC379:
  .string "  cmp rax, rdi\n"
.data
.LC378:
  .string "  movzb rax, al\n"
.data
.LC377:
  .string "  sete al\n"
.data
.LC376:
  .string "  cmp rax, rdi\n"
.data
.LC375:
  .string "  idiv rdi\n"
.data
.LC374:
  .string "  cqo\n"
.data
.LC373:
  .string "  imul rax, rdi\n"
.data
.LC372:
  .string "  sub rax, rdi\n"
.data
.LC371:
  .string "  add rax, rdi\n"
.data
.LC370:
  .string "  pop rax\n"
.data
.LC369:
  .string "  pop rdi\n"
.data
.LC368:
  .string ".Lend%d:\n"
.data
.LC367:
  .string "  push 1\n"
.data
.LC366:
  .string ".Ltrue%d:\n"
.data
.LC365:
  .string "  jmp .Lend%d\n"
.data
.LC364:
  .string "  push 0\n"
.data
.LC363:
  .string "  je .Ltrue%d\n"
.data
.LC362:
  .string "  cmp rax, 1\n"
.data
.LC361:
  .string "  pop rax\n"
.data
.LC360:
  .string "  je .Ltrue%d\n"
.data
.LC359:
  .string "  cmp rax, 1\n"
.data
.LC358:
  .string "  pop rax\n"
.data
.LC357:
  .string ".Lend%d:\n"
.data
.LC356:
  .string "  push 0\n"
.data
.LC355:
  .string ".Lfalse%d:\n"
.data
.LC354:
  .string "  jmp .Lend%d\n"
.data
.LC353:
  .string "  push 1\n"
.data
.LC352:
  .string "  je .Lfalse%d\n"
.data
.LC351:
  .string "  cmp rax, 0\n"
.data
.LC350:
  .string "  pop rax\n"
.data
.LC349:
  .string "  je .Lfalse%d\n"
.data
.LC348:
  .string "  cmp rax, 0\n"
.data
.LC347:
  .string "  pop rax\n"
.data
.LC346:
  .string "  push rax\n"
.data
.LC345:
  .string "unexpected type"
.data
.LC344:
  .string "  movsx rax, BYTE PTR [rax]\n"
.data
.LC343:
  .string "  mov rax, [rax]\n"
.data
.LC342:
  .string "  mov eax, [rax]\n"
.data
.LC341:
  .string "  pop rax\n"
.data
.LC340:
  .string "  push 0\n"
.data
.LC339:
  .string "  mov [rbp - %d], rax\n"
.data
.LC338:
  .string "  lea rax, [rbp - %d]\n"
.data
.LC337:
  .string "  mov [rbp - %d], rax\n"
.data
.LC336:
  .string "  lea rax, [rbp + 16]\n"
.data
.LC335:
  .string "  mov [rbp - %d], eax\n"
.data
.LC334:
  .string "  mov eax, 48\n"
.data
.LC333:
  .string "  mov [rbp - %d], eax\n"
.data
.LC332:
  .string "  mov eax, %d\n"
.data
.LC331:
  .string "not implemented: return value"
.data
.LC330:
  .string "  push rax\n"
.data
.LC329:
  .string "  push rax\n"
.data
.LC328:
  .string "  movsx rax, eax\n"
.data
.LC327:
  .string "  push rax\n"
.data
.LC326:
  .string "  movsx rax, al\n"
.data
.LC325:
  .string "  push 0\n"
.data
.LC324:
  .string "  mov rsp, rdi\n"
.data
.LC323:
  .string "  pop rdi\n"
.data
.LC322:
  .string "  pop rdi\n"
.data
.LC321:
  .string "  call %.*s\n"
.data
.LC320:
  .string "  mov rax, 0\n"
.data
.LC319:
  .string "  push 0\n"
.data
.LC318:
  .string "  push r10\n"
.data
.LC317:
  .string "  and rsp, 0xfffffffffffffff0\n"
.data
.LC316:
  .string "  mov r10, rsp\n"
.data
.LC315:
  .string "  pop %s\n"
.data
.LC314:
  .string "r9"
.data
.LC313:
  .string "r8"
.data
.LC312:
  .string "rcx"
.data
.LC311:
  .string "rdx"
.data
.LC310:
  .string "rsi"
.data
.LC309:
  .string "rdi"
.data
.LC308:
  .string "too many arguments"
.data
.LC307:
  .string "not implemented: too big object"
.data
.LC306:
  .string "  mov [rax], dil\n"
.data
.LC305:
  .string "  sub dil, 1\n"
.data
.LC304:
  .string "  push rdi\n"
.data
.LC303:
  .string "  mov dil, [rax]\n"
.data
.LC302:
  .string "  mov [rax], edi\n"
.data
.LC301:
  .string "  sub edi, 1\n"
.data
.LC300:
  .string "  push rdi\n"
.data
.LC299:
  .string "  mov edi, [rax]\n"
.data
.LC298:
  .string "  mov [rax], rdi\n"
.data
.LC297:
  .string "  sub rdi, 1\n"
.data
.LC296:
  .string "  push rdi\n"
.data
.LC295:
  .string "  mov rdi, [rax]\n"
.data
.LC294:
  .string " pop rax\n"
.data
.LC293:
  .string "  mov [rax], dil\n"
.data
.LC292:
  .string "  add dil, 1\n"
.data
.LC291:
  .string "  push rdi\n"
.data
.LC290:
  .string "  mov dil, [rax]\n"
.data
.LC289:
  .string "  mov [rax], edi\n"
.data
.LC288:
  .string "  add edi, 1\n"
.data
.LC287:
  .string "  push rdi\n"
.data
.LC286:
  .string "  mov edi, [rax]\n"
.data
.LC285:
  .string "  mov [rax], rdi\n"
.data
.LC284:
  .string "  add rdi, 1\n"
.data
.LC283:
  .string "  push rdi\n"
.data
.LC282:
  .string "  mov rdi, [rax]\n"
.data
.LC281:
  .string " pop rax\n"
.data
.LC280:
  .string "  push 0\n"
.data
.LC279:
  .string "  pop rax\n"
.data
.LC278:
  .string "  push rdi\n"
.data
.LC277:
  .string "failed to assign"
.data
.LC276:
  .string "  mov [rax], rdi\n"
.data
.LC275:
  .string "  mov BYTE PTR [rax], dil\n"
.data
.LC274:
  .string "  mov DWORD PTR [rax], edi\n"
.data
.LC273:
  .string "  pop rax\n"
.data
.LC272:
  .string "  pop rdi\n"
.data
.LC271:
  .string "not implemented: size %d"
.data
.LC270:
  .string "  push rax\n"
.data
.LC269:
  .string "  movsx rax, BYTE PTR [rax]\n"
.data
.LC268:
  .string "  push rax\n"
.data
.LC267:
  .string "  movsxd rax, [rax]\n"
.data
.LC266:
  .string "  push r10\n"
.data
.LC265:
  .string "  mov r10, [rax + %d]\n"
.data
.LC264:
  .string "  pop rax\n"
.data
.LC263:
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
  lea rax, .LC264[rip]
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
.Lbegin338:
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
  je .Lelse339
  lea rax, .LC265[rip]
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
  jmp .Lend339
.Lelse339:
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
  je .Lelse340
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
  jmp .Lend340
.Lelse340:
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
  je .Lelse341
  lea rax, .LC269[rip]
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
  jmp .Lend341
.Lelse341:
  lea rax, .LC271[rip]
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
.Lend341:
.Lend340:
.Lend339:
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
  je .Lend342
  jmp .Lbreak52
.Lend342:
.Lcontinue47:
  jmp .Lbegin338
.Lend338:
.Lbreak52:
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
  jmp .Lbreak53
.Lcase6_1:
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
  jmp .Lbreak53
.Lcase6_8:
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
  jmp .Lbreak53
.Ldefault6:
  lea rax, .LC277[rip]
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
.Lbreak53:
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
.Lbegin343:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend343
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
.Lcontinue48:
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
  jmp .Lbegin343
.Lend343:
.Lbreak54:
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
  lea rax, .LC283[rip]
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
  jmp .Lbreak55
.Lcase7_4:
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
  jmp .Lbreak55
.Lcase7_1:
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
  jmp .Lbreak55
.Lbreak55:
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
  jmp .Lbreak56
.Lcase8_4:
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
  jmp .Lbreak56
.Lcase8_1:
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
  jmp .Lbreak56
.Lbreak56:
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
.Lbegin344:
.Lcontinue49:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  pop rax
  cmp rax, 0
  je .Lend344
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
  je .Lend345
  lea rax, .LC307[rip]
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
.Lend345:
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
  jmp .Lbegin344
.Lend344:
.Lbreak57:
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
  je .Lend346
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
  lea rax, .LC308[rip]
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
.Lend346:
  mov rax, rbp
  sub rax, 76
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  lea rax, .LC309[rip]
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
  lea rax, .LC310[rip]
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
  lea rax, .LC311[rip]
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
  lea rax, .LC312[rip]
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
  lea rax, .LC313[rip]
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
  lea rax, .LC314[rip]
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
.Lbegin347:
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
  je .Lend347
  lea rax, .LC315[rip]
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
.Lcontinue50:
  mov rax, rbp
  sub rax, 80
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  sub edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin347
.Lend347:
.Lbreak58:
  lea rax, .LC316[rip]
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
  lea rax, .LC317[rip]
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
  lea rax, .LC318[rip]
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
  lea rax, .LC319[rip]
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
  lea rax, .LC320[rip]
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
  lea rax, .LC321[rip]
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
  je .Lend348
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
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend348:
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
  jmp .Lbreak59
.Lcase9_4:
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
  jmp .Lbreak59
.Lcase9_8:
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
  jmp .Lbreak59
.Ldefault9:
  lea rax, .LC331[rip]
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
.Lbreak59:
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
  lea rax, .LC332[rip]
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
  lea rax, .LC333[rip]
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
  lea rax, .LC337[rip]
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
  lea rax, .LC338[rip]
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
  lea rax, .LC341[rip]
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
  jmp .Lbreak60
.Lcase10_8:
  lea rax, .LC343[rip]
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
  jmp .Lbreak60
.Lcase10_1:
  lea rax, .LC344[rip]
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
  jmp .Lbreak60
.Ldefault10:
  lea rax, .LC345[rip]
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
.Lbreak60:
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
  lea rax, .LC349[rip]
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
  lea rax, .LC351[rip]
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
  lea rax, .LC352[rip]
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
  lea rax, .LC358[rip]
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
  lea rax, .LC361[rip]
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
  lea rax, .LC369[rip]
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
  lea rax, .LC371[rip]
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
.Lcase11_1:
  lea rax, .LC372[rip]
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
.Lcase11_2:
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
  jmp .Lbreak61
.Lcase11_3:
  lea rax, .LC374[rip]
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
  jmp .Lbreak61
.Lcase11_6:
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
  jmp .Lbreak61
.Lcase11_7:
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
  jmp .Lbreak61
.Lcase11_8:
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
  jmp .Lbreak61
.Lcase11_9:
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
  jmp .Lbreak61
.Ldefault11:
  lea rax, .LC388[rip]
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
.Lbreak61:
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
.Lbreak51:
  mov rsp, rbp
  pop rbp
  ret
.data
.LC390:
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
  lea rax, .LC390[rip]
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
.Lbegin349:
.Lcontinue51:
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
  je .Lfalse350
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
  je .Lfalse350
  push 1
  jmp .Lend350
.Lfalse350:
  push 0
.Lend350:
  pop rax
  cmp rax, 0
  je .Lend349
  mov rax, rbp
  sub rax, 48
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  sub rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin349
.Lend349:
.Lbreak62:
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
.Lbegin351:
.Lcontinue52:
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
  je .Lend351
  mov rax, rbp
  sub rax, 56
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin351
.Lend351:
.Lbreak63:
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
.Lbegin352:
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
  je .Lend352
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
  je .Lend353
  mov rax, rbp
  sub rax, 60
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  add edi, 1
  mov [rax], edi
  pop rax
.Lend353:
.Lcontinue53:
  mov rax, rbp
  sub rax, 68
  push rax
 pop rax
  mov rdi, [rax]
  push rdi
  add rdi, 1
  mov [rax], rdi
  pop rax
  jmp .Lbegin352
.Lend352:
.Lbreak64:
  mov rax, rbp
  sub rax, 72
  push rax
  lea rax, stderr[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC391[rip]
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
  lea rax, .LC392[rip]
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
  lea rax, .LC393[rip]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, [rax]
  push rax
  lea rax, .LC394[rip]
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
  lea rax, .LC395[rip]
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
.LC397:
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
  je .Lend354
  lea rax, token[rip]
  push rax
  pop rax
  mov r10, [rax + 0]
  push r10
  lea rax, .LC397[rip]
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
.Lend354:
  mov rsp, rbp
  pop rbp
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
  lea rax, .LC398[rip]
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
  je .Lend355
  lea rax, .LC399[rip]
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
.Lend355:
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
  je .Lend356
  lea rax, .LC400[rip]
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
.Lend356:
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
  je .Lend357
  lea rax, .LC401[rip]
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
.Lend357:
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
  je .Ltrue359
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
  je .Ltrue359
  push 0
  jmp .Lend359
.Ltrue359:
  push 1
.Lend359:
  pop rax
  cmp rax, 0
  je .Lend358
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
.Lend358:
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
.LC402:
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
.Lbegin360:
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
  je .Lend360
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
  je .Lend361
  jmp .Lbreak65
.Lend361:
.Lcontinue54:
  mov rax, rbp
  sub rax, 12
  push rax
 pop rax
  mov edi, [rax]
  push rdi
  sub edi, 1
  mov [rax], edi
  pop rax
  jmp .Lbegin360
.Lend360:
.Lbreak65:
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
  je .Lend362
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
  lea rax, .LC402[rip]
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
.Lend362:
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
.LC411:
  .string "  .zero %d\n"
.data
.LC410:
  .string "%.*s:\n"
.data
.LC409:
  .string ".data\n"
.data
.LC408:
  .string ".globl %.*s\n"
.data
.LC407:
  .string "  .zero 8\n"
.data
.LC406:
  .string "NULL:\n"
.data
.LC405:
  .string ".data\n"
.data
.LC404:
  .string ".intel_syntax noprefix\n"
.data
.LC403:
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
  je .Lend363
  lea rax, .LC403[rip]
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
.Lend363:
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
  lea rax, .LC404[rip]
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
  lea rax, .LC405[rip]
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
  lea rax, .LC406[rip]
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
  lea rax, .LC407[rip]
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
.Lbegin364:
.Lcontinue55:
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
  je .Lend364
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
  jmp .Lbreak67
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
  je .Lend365
  lea rax, .LC408[rip]
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
  lea rax, .LC409[rip]
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
  lea rax, .LC410[rip]
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
  lea rax, .LC411[rip]
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
.Lend365:
  jmp .Lbreak67
.Ldefault12:
  jmp .Lbreak67
.Lbreak67:
  jmp .Lbegin364
.Lend364:
.Lbreak66:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
