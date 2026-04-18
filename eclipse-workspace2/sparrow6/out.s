.section .rodata
.text
.globl show_whole
.type show_whole, @function
show_whole:
    push %rbp
    mov %rsp, %rbp
    # ident data (treated as null)
    call js_make_null_c
    push %rax
    # string literal id=0
    leaq .Lstr0(%rip), %rdi
    call js_make_string
    push %rax
    pop %rdi
    pop %rsi
    call printf
    # discard expr result
    call js_make_null_c
    pop %rbp
    ret
.globl show_fields
.type show_fields, @function
show_fields:
    push %rbp
    mov %rsp, %rbp
    # ident data_name (treated as null)
    call js_make_null_c
    push %rax
    # string literal id=1
    leaq .Lstr1(%rip), %rdi
    call js_make_string
    push %rax
    pop %rdi
    pop %rsi
    call printf
    # discard expr result
    # ident data_age (treated as null)
    call js_make_null_c
    push %rax
    # string literal id=2
    leaq .Lstr2(%rip), %rdi
    call js_make_string
    push %rax
    pop %rdi
    call js_print_value
    call js_make_null_c
    # discard expr result
    # ident data_active (treated as null)
    call js_make_null_c
    # if cond check (null/zero -> false)
    cmp $0, %rax
    je .Lelse160330
    # string literal id=3
    leaq .Lstr3(%rip), %rdi
    call js_make_string
    push %rax
    pop %rdi
    call printf
    # discard expr result
    jmp .Lend47260
.Lelse160330:
    # string literal id=4
    leaq .Lstr4(%rip), %rdi
    call js_make_string
    push %rax
    pop %rdi
    call printf
    # discard expr result
.Lend47260:
    # ident data_tags (treated as null)
    call js_make_null_c
    push %rax
    # string literal id=5
    leaq .Lstr5(%rip), %rdi
    call js_make_string
    push %rax
    pop %rdi
    pop %rsi
    call printf
    # discard expr result
    call js_make_null_c
    pop %rbp
    ret
.globl _start
_start:
    mov $60, %rax
    xor %rdi, %rdi
    syscall

/* --- runtime stubs (C-callable) --- */
.section .text
.globl js_make_null_c
js_make_null_c:
    mov $0, %rax
    ret

.globl js_make_string
js_make_string:
    /* rdi = pointer to C string */
    mov %rdi, %rax
    ret

.globl js_print_value
js_print_value:
    /* rdi = value (for demo, treat as C string pointer or 0) */
    test %rdi, %rdi
    je .print_null
    mov %rdi, %rsi
    lea .Lprint_fmt(%rip), %rdi
    xor %rax, %rax
    call printf
    ret
.print_null:
    lea .Lnull_str(%rip), %rdi
    xor %rax, %rax
    call puts
    ret

.section .rodata
.Lprint_fmt: .string "
/* --- runtime stubs (C-callable) --- */
.section .text
.globl js_make_null_c
js_make_null_c:
    mov $0, %rax
    ret

.globl js_make_string
js_make_string:
    /* rdi = pointer to C string */
    mov %rdi, %rax
    ret

.globl js_print_value
js_print_value:
    /* rdi = value (for demo, treat as C string pointer or 0) */
    test %rdi, %rdi
    je .print_null
    mov %rdi, %rsi
    lea .Lprint_fmt(%%rip), %%rdi
    xor %%rax, %%rax
    call printf
    ret
.print_null:
    lea .Lnull_str(%%rip), %%rdi
    xor %%rax, %%rax
    call puts
    ret

.section .rodata
.Lprint_fmt: .string "%s\n"
.Lnull_str: .string "null"
.section .text

.globl js_global_set
js_global_set:
    /* rdi = gid, rsi = value */
    /* no-op in demo */
    mov %rsi, %rax
    ret

.globl js_binary_op
js_binary_op:
    /* edi = opcode, operands expected in xmm0/xmm1? For demo, return null */
    call js_make_null_c
    ret
\n"
.Lnull_str: .string "null"
.section .text

.globl js_global_set
js_global_set:
    /* rdi = gid, rsi = value */
    /* no-op in demo */
    mov %rsi, %rax
    ret

.globl js_binary_op
js_binary_op:
    /* edi = opcode, operands expected in xmm0/xmm1? For demo, return null */
    call js_make_null_c
    ret
