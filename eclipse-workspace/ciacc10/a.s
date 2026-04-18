.intel_syntax noprefix
.data
NULL:
  .zero 8
.globl main
.text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
