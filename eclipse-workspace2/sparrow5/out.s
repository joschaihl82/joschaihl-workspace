	.text
	.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	# object literal
	call js_object_new@PLT
	movq %rax, %rbx
	leaq .Lstr0(%rip), %rax
	movq %rbx, %rdi
	leaq .Lstr1(%rip), %rsi
	movq %rax, %rdx
	call js_object_set@PLT
	# number 34
	movq $34, %rax
	shlq $3, %rax
	movq %rbx, %rdi
	leaq .Lstr2(%rip), %rsi
	movq %rax, %rdx
	call js_object_set@PLT
	movq true(%rip), %rax
	movq %rbx, %rdi
	leaq .Lstr3(%rip), %rsi
	movq %rax, %rdx
	call js_object_set@PLT
	# array literal
	call js_array_new_c@PLT
	movq %rax, %rbx
	leaq .Lstr4(%rip), %rax
	movq %rbx, %rdi
	movq %rax, %rsi
	call js_array_push@PLT
	leaq .Lstr5(%rip), %rax
	movq %rbx, %rdi
	movq %rax, %rsi
	call js_array_push@PLT
	leaq .Lstr6(%rip), %rax
	movq %rbx, %rdi
	movq %rax, %rsi
	call js_array_push@PLT
	movq %rbx, %rax
	movq %rbx, %rdi
	leaq .Lstr7(%rip), %rsi
	movq %rax, %rdx
	call js_object_set@PLT
	movq %rbx, %rax
	movq %rax, data(%rip)
	leaq .Lstr8(%rip), %rax
	movq %rax, %rdi
	call js_print_value@PLT
	movl $10, %edi
	call putchar@PLT
	movq $0, %rax
	call show_fields
	call show_whole
	leaq .Lstr9(%rip), %rax
	movq %rax, %rdi
	call js_print_value@PLT
	movl $10, %edi
	call putchar@PLT
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
	leaq .Lstr12(%rip), %rax
	movq %rax, %rdi
	call js_print_value@PLT
	movl $32, %edi
	call putchar@PLT
	movq data_age(%rip), %rax
	movq %rax, %rdi
	call js_print_value@PLT
	movl $10, %edi
	call putchar@PLT
	movq $0, %rax
	movq data_active(%rip), %rax
	cmpq $0, %rax
	je .Lelse21915
	leaq .Lstr13(%rip), %rax
	pushq %rax
	call printf
	addq $8, %rsp
	jmp .Lend399994
.Lelse21915:
	leaq .Lstr14(%rip), %rax
	pushq %rax
	call printf
	addq $8, %rsp
.Lend399994:
	movq data_tags(%rip), %rax
	pushq %rax
	leaq .Lstr15(%rip), %rax
	pushq %rax
	call printf
	addq $16, %rsp
	leave
	ret
	.section .rodata
.Lstr15:
	.asciz "Tags JSON: %s\n"
.Lstr14:
	.asciz "Active: false\n"
.Lstr13:
	.asciz "Active: true\n"
.Lstr12:
	.asciz "Age:"
.Lstr11:
	.asciz "Name: %s\n"
.Lstr10:
	.asciz "Full JSON: %s\n"
.Lstr9:
	.asciz "Demo end"
.Lstr8:
	.asciz "Demo start"
.Lstr7:
	.asciz "tags"
.Lstr6:
	.asciz "sparrow"
.Lstr5:
	.asciz "c"
.Lstr4:
	.asciz "dev"
.Lstr3:
	.asciz "active"
.Lstr2:
	.asciz "age"
.Lstr1:
	.asciz "name"
.Lstr0:
	.asciz "Josh"
	.data
data_tags:
	.quad 0
data_active:
	.quad 0
data_age:
	.quad 0
data_name:
	.quad 0
true:
	.quad 0
data:
	.quad 0

	.section .rodata
.Ljs_fmt_str:
	.asciz "%s"
.Ljs_fmt_num:
	.asciz "%lld"

	.text
	.globl js_array_create_from_c
js_array_create_from_c:
	# stub: return 0
	xorq %rax, %rax
	ret

	.globl js_array_new_c
js_array_new_c:
	xorq %rax, %rax
	ret

	.globl js_array_length
js_array_length:
	xorq %rax, %rax
	ret

	.globl js_array_get
js_array_get:
	xorq %rax, %rax
	ret

	.globl js_array_set
js_array_set:
	ret

	.globl js_array_push
js_array_push:
	ret

	.globl js_object_new
js_object_new:
	xorq %rax, %rax
	ret

	.globl js_object_set
js_object_set:
	ret

	.globl js_object_get
js_object_get:
	xorq %rax, %rax
	ret

	.globl js_value_to_cstr
js_value_to_cstr:
	pushq %rbp
	movq %rsp, %rbp
	movq %rdi, %rax
	andq $7, %rax
	cmpq $0, %rax
	jne .Ljs_v2c_ptr
	# integer case: return NULL (simplified)
	xorq %rax, %rax
	leave
	ret
.Ljs_v2c_ptr:
	movq %rdi, %rax
	leave
	ret

	.globl js_print_value
js_print_value:
	pushq %rbp
	movq %rsp, %rbp
	movq %rdi, %rax
	andq $7, %rax
	cmpq $0, %rax
	je .Ljs_print_int
	# treat rdi as C string pointer
	movq %rdi, %rsi
	leaq .Ljs_fmt_str(%rip), %rdi
	xor %eax, %eax
	call printf@PLT
	jmp .Ljs_print_done
.Ljs_print_int:
	# unbox integer
	movq %rdi, %rax
	shrq $3, %rax
	movq %rax, %rsi
	leaq .Ljs_fmt_num(%rip), %rdi
	xor %eax, %eax
	call printf@PLT
.Ljs_print_done:
	leave
	ret
