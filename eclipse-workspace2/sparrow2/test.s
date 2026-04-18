	.text
	.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	# number 10
	movq $10, %rax
	movq %rax, x(%rip)
	call hello
	movl $0, %eax
	leave
	ret
	.text
	.globl hello
hello:
	pushq %rbp
	movq %rsp, %rbp
	# number 123
	movq $123, %rax
	pushq %rax
	leaq .Lstr1(%rip), %rax
	pushq %rax
	leaq .Lstr2(%rip), %rax
	pushq %rax
	leaq .Lstr0(%rip), %rax
	pushq %rax
	call printf@PLT
	addq $32, %rsp
	movq $0, %rax
	leave
	ret
	.section .rodata
.Lstr2:
	.asciz "Hello"
.Lstr1:
	.asciz "world"
.Lstr0:
	.asciz "%s %s %g\\n"
	.data
x:
	.quad 0
