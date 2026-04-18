	.text
	.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	leaq .Lstr8(%rip), %rax
	pushq %rax
	leaq .Lstr7(%rip), %rax
	pushq %rax
	call printf@PLT
	addq $16, %rsp
	movq $0, %rax
	call show_fields
	call show_whole
	leaq .Lstr9(%rip), %rax
	pushq %rax
	leaq .Lstr7(%rip), %rax
	pushq %rax
	call printf@PLT
	addq $16, %rsp
	movq $0, %rax
	movl $0, %eax
	leave
	ret
	.text
	.globl show_whole
show_whole:
	pushq %rbp
	movq %rsp, %rbp
	movq data(%rip), %rax
	pushq %rax
	leaq .Lstr10(%rip), %rax
	pushq %rax
	call printf
	addq $16, %rsp
	leave
	ret
	.text
	.globl show_fields
show_fields:
	pushq %rbp
	movq %rsp, %rbp
	movq data_name(%rip), %rax
	pushq %rax
	leaq .Lstr11(%rip), %rax
	pushq %rax
	call printf
	addq $16, %rsp
	movq data_age(%rip), %rax
	pushq %rax
	leaq .Lstr13(%rip), %rax
	pushq %rax
	leaq .Lstr12(%rip), %rax
	pushq %rax
	call printf@PLT
	addq $24, %rsp
	movq $0, %rax
	movq data_active(%rip), %rax
	cmpq $0, %rax
	je .Lelse300313
	leaq .Lstr14(%rip), %rax
	pushq %rax
	call printf
	addq $8, %rsp
	jmp .Lend94874
.Lelse300313:
	leaq .Lstr15(%rip), %rax
	pushq %rax
	call printf
	addq $8, %rsp
.Lend94874:
	movq data_tags(%rip), %rax
	pushq %rax
	leaq .Lstr16(%rip), %rax
	pushq %rax
	call printf
	addq $16, %rsp
	leave
	ret
	.section .rodata
.Lstr16:
	.asciz "Tags JSON: %s\n"
.Lstr15:
	.asciz "Active: false\n"
.Lstr14:
	.asciz "Active: true\n"
.Lstr13:
	.asciz "Age:"
.Lstr12:
	.asciz "%s %g\\n"
.Lstr11:
	.asciz "Name: %s\n"
.Lstr10:
	.asciz "Full JSON: %s\n"
.Lstr9:
	.asciz "Demo end"
.Lstr8:
	.asciz "Demo start"
.Lstr7:
	.asciz "%s\\n"
.Lstr1:
	.asciz "Josh"
.Lstr0:
	.asciz "{\"name\":\"Josh\",\"age\":34,\"active\":null,\"tags\":[\"dev\",\"c\",\"sparrow\"]}"
	.data
data_tags:
	.quad .Lstr0
data_active:
	.quad 0
data_age:
	.quad 34
data_name:
	.quad .Lstr1
data:
	.quad .Lstr0
