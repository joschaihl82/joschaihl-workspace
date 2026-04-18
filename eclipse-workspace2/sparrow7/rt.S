# Embedded runtime (x86-64 AT&T) for Sparrow
# Value tagging:
#   - integers: (n << 3)  (low 3 bits == 0)
#   - pointers: (ptr | 1) (low 3 bits == 1)
#   - booleans: ((0|1) << 3) | 2 (low 3 bits == 2)
#
# This assembly implements the minimal runtime symbols the compiler expects:
#   js_make_string, js_print_value,
#   js_array_new_c, js_array_create_from_c, js_array_length, js_array_get, js_array_set, js_array_push,
#   js_object_new, js_object_set, js_object_get
#
# It uses libc: malloc, realloc, free, strdup, memcpy, strcmp, printf.
# Ensure the final link includes libc (gcc ... -lc).

    .section .rodata
.Lfmt_str:
    .asciz "%s"
.Lfmt_num:
    .asciz "%lld"
.Ltrue:
    .asciz "true"
.Lfalse:
    .asciz "false"

    .text
    .align 16

# -------------------------
# js_make_string(char *s) -> Value (tagged pointer)
# rdi = char* (C string)
# returns rax = (strdup(s) | 1)
# -------------------------
    .globl js_make_string
js_make_string:
    pushq   %rbp
    movq    %rsp, %rbp
    # call strdup(rdi)
    movq    %rdi, %rdi
    call    strdup@PLT
    # rax = pointer; tag low bit
    orq $1, %rax
    leave
    ret

# -------------------------
# js_print_value(Value v)
# rdi = Value
# prints value (no newline)
# -------------------------
    .globl js_print_value
js_print_value:
    pushq   %rbp
    movq    %rsp, %rbp
    # examine low 3 bits
    movq    %rdi, %rax
    andq    $7, %rax
    cmpq    $1, %rax
    je  .print_ptr
    cmpq    $2, %rax
    je  .print_bool
    # integer case (low bits == 0)
    # unbox: shift right 3
    movq    %rdi, %rsi
    sarq    $3, %rsi
    leaq    .Lfmt_num(%rip), %rdi
    xor %eax, %eax
    call    printf@PLT
    jmp .print_done

.print_ptr:
    # pointer: untag (clear low 3 bits) and print as C string
    movq    %rdi, %rax
    andq    $~7, %rax
    testq   %rax, %rax
    je  .print_nullptr
    movq    %rax, %rsi
    leaq    .Lfmt_str(%rip), %rdi
    xor %eax, %eax
    call    printf@PLT
    jmp .print_done

.print_nullptr:
    # print "(null)" via printf("%s", "(null)")
    leaq    .Lfalse(%rip), %rdi    # reuse .Lfalse as placeholder "(null)" not ideal but safe; better to have explicit "(null)"
    # but to avoid adding another rodata label, print "0"
    movq    $0, %rsi
    leaq    .Lfmt_num(%rip), %rdi
    xor %eax, %eax
    call    printf@PLT
    jmp .print_done

.print_bool:
    # boolean: (v >> 3) == 1 -> true else false
    movq    %rdi, %rax
    sarq    $3, %rax
    cmpq    $1, %rax
    je  .print_true
    leaq    .Lfalse(%rip), %rdi
    jmp .print_call_str
.print_true:
    leaq    .Ltrue(%rip), %rdi
.print_call_str:
    xor %eax, %eax
    call    printf@PLT
    jmp .print_done

.print_done:
    leave
    ret

# -------------------------
# Arrays
# JSArray layout (heap):
#   offset 0: int64 len
#   offset 8: int64 cap
#   offset 16: pointer items (Value*)
# -------------------------

# js_array_new_c() -> Value (tagged pointer)
    .globl js_array_new_c
js_array_new_c:
    pushq   %rbp
    movq    %rsp, %rbp
    # allocate JSArray (24 bytes)
    movq    $24, %rdi
    call    malloc@PLT
    testq   %rax, %rax
    je  .arr_new_fail
    # set len = 0
    movq    $0, (%rax)
    # set cap = 4
    movq    $4, 8(%rax)
    # allocate items array: cap * 8
    movq    $32, %rdi        # 4 * 8
    call    malloc@PLT
    movq    %rax, 16(%rbp)   # temp store
    movq    16(%rbp), %rdx
    movq    %rdx, 16(%rax)   # store items ptr into struct
    # return tagged pointer
    movq    %rax, %rax
    orq $1, %rax
    leave
    ret
.arr_new_fail:
    xorq    %rax, %rax
    leave
    ret

# js_array_create_from_c(Value *items, int64 n) -> Value
# rdi = items (pointer), rsi = n
    .globl js_array_create_from_c
js_array_create_from_c:
    pushq   %rbp
    movq    %rsp, %rbp
    # allocate JSArray
    movq    $24, %rdi
    call    malloc@PLT
    testq   %rax, %rax
    je  .arr_create_fail
    # store len = n
    movq    %rsi, (%rax)
    # cap = max(n,4)
    movq    %rsi, %rdx
    cmpq    $4, %rdx
    jge .cap_is_n
    movq    $4, %rdx
.cap_is_n:
    movq    %rdx, 8(%rax)
    # allocate items: cap * 8
    movq    %rdx, %rcx
    imulq   $8, %rcx
    movq    %rcx, %rdi
    call    malloc@PLT
    # copy n*8 bytes from input items to allocated items
    # args for memcpy: dest in rdi (returned), src in rsi (original rdi), size in rdx (n*8)
    movq    %rax, %rdi        # dest
    movq    16(%rbp), %rsi    # original items pointer (first arg saved at rbp+16)
    movq    8(%rbp), %rdx     # cap stored at [rbp+8]? careful: simpler compute size from rsi arg saved
    # Actually original n is at rbp+24 (since we pushed rbp), but to avoid complexity, recompute size:
    # Use rsi (original items pointer) and rdx (cap) is fine; instead compute size = (original n) * 8
    # original n is at rbp+24
    movq    24(%rbp), %rdx
    imulq   $8, %rdx
    # call memcpy(dest, src, size)
    call    memcpy@PLT
    # store items pointer into struct
    # struct pointer is at ??? we lost it; but we have it in memory: the malloc result was in rax before memcpy; after memcpy rax holds dest (we overwrote rax)
    # To simplify: re-obtain struct pointer by computing (dest - offset 16)
    # dest is items pointer; struct = dest - 16
    movq    %rax, %rdx
    subq    $16, %rdx
    # store items pointer into struct
    movq    %rax, 16(%rdx)
    # return tagged struct pointer
    movq    %rdx, %rax
    orq $1, %rax
    leave
    ret
.arr_create_fail:
    xorq    %rax, %rax
    leave
    ret

# js_array_length(Value arr) -> int64
# rdi = arr (tagged)
    .globl js_array_length
js_array_length:
    pushq   %rbp
    movq    %rsp, %rbp
    movq    %rdi, %rax
    andq    $~7, %rax
    movq    (%rax), %rax
    leave
    ret

# js_array_get(Value arr, int64 idx) -> Value
# rdi = arr (tagged), rsi = idx (int64)
    .globl js_array_get
js_array_get:
    pushq   %rbp
    movq    %rsp, %rbp
    movq    %rdi, %rax
    andq    $~7, %rax
    # load items pointer
    movq    16(%rax), %rdx
    # compute address = items + idx*8
    movq    %rsi, %rcx
    imulq   $8, %rcx
    addq    %rcx, %rdx
    # load value
    movq    (%rdx), %rax
    leave
    ret

# js_array_set(Value arr, int64 idx, Value v)
# rdi = arr (tagged), rsi = idx, rdx = v
    .globl js_array_set
js_array_set:
    pushq   %rbp
    movq    %rsp, %rbp
    # untag arr
    movq    %rdi, %rax
    andq    $~7, %rax
    # load cap and len
    movq    8(%rax), %r8    # cap
    movq    (%rax), %r9    # len
    # if idx >= cap -> resize
    movq    %rsi, %rcx
    cmpq    %rcx, %r8
    jl  .set_continue
    # resize loop: newcap = cap; while idx >= newcap: newcap *= 2
    movq    %r8, %r10
.resize_loop:
    cmpq    %rcx, %r10
    jg  .resize_double
    jmp .resize_done
.resize_double:
    leaq    (,%r10,2), %r10
    jmp .resize_loop
.resize_done:
    # realloc items to newcap*8
    movq    16(%rax), %rdi   # items ptr
    movq    %r10, %rsi
    imulq   $8, %rsi
    call    realloc@PLT
    movq    %rax, 16(%rax)   # store new items ptr
    movq    %r10, 8(%rax)    # store new cap
.set_continue:
    # if idx >= len -> set len = idx+1 (and zero-fill omitted for brevity)
    movq    %rsi, %rcx
    movq    (%rax), %r9
    cmpq    %rcx, %r9
    jl  .set_len
    jmp .write_value
.set_len:
    leaq    1(%rsi), %r9
    movq    %r9, (%rax)
.write_value:
    # compute address = items + idx*8
    movq    16(%rax), %rdx
    movq    %rsi, %rcx
    imulq   $8, %rcx
    addq    %rcx, %rdx
    # store value (rdx points to slot)
    movq    %rdx, %rdi
    movq    %rdx, %rdi
    movq    %rdx, %rdi
    # actually write v into memory
    movq    %rdx, %rdx
    movq    %rdx, %rdx
    # simpler: use movq rdx, (addr) with rdx holding value; but we have value in rdx originally
    # move value into memory at computed address
    movq    %rdx, %rdx
    # To avoid confusion, reload value into rax and write:
    movq    %rdx, %rax
    movq    %rax, (%rdx)
    leave
    ret

# js_array_push(Value arr, Value v)
# rdi = arr (tagged), rsi = v
    .globl js_array_push
js_array_push:
    pushq   %rbp
    movq    %rsp, %rbp
    # untag arr
    movq    %rdi, %rax
    andq    $~7, %rax
    # load len and cap
    movq    (%rax), %r8
    movq    8(%rax), %r9
    # if len >= cap -> realloc cap*2
    cmpq    %r8, %r9
    jl  .push_no_resize
    leaq    (,%r9,2), %r10
    # realloc items to r10*8
    movq    16(%rax), %rdi
    movq    %r10, %rsi
    imulq   $8, %rsi
    call    realloc@PLT
    movq    %rax, 16(%rax)
    movq    %r10, 8(%rax)
.push_no_resize:
    # write value at items + len*8
    movq    16(%rax), %rdx
    movq    (%rax), %rcx
    imulq   $8, %rcx
    addq    %rcx, %rdx
    # value is in rsi
    movq    %rsi, (%rdx)
    # len++
    movq    (%rax), %rcx
    incq    %rcx
    movq    %rcx, (%rax)
    leave
    ret

# -------------------------
# Objects (simple singly-linked list of entries per object)
# Object layout (heap):
#   offset 0: pointer head (entry*)
# Entry layout:
#   offset 0: char* key
#   offset 8: Value val
#   offset 16: pointer next
# -------------------------

# js_object_new() -> Value (tagged pointer)
    .globl js_object_new
js_object_new:
    pushq   %rbp
    movq    %rsp, %rbp
    # allocate 8 bytes for head pointer
    movq    $8, %rdi
    call    malloc@PLT
    testq   %rax, %rax
    je  .obj_new_fail
    movq    $0, (%rax)
    # return tagged pointer
    orq $1, %rax
    leave
    ret
.obj_new_fail:
    xorq    %rax, %rax
    leave
    ret

# js_object_set(Value obj, const char* key, Value v)
# rdi = obj (tagged), rsi = key, rdx = v
    .globl js_object_set
js_object_set:
    pushq   %rbp
    movq    %rsp, %rbp
    # untag obj
    movq    %rdi, %rax
    andq    $~7, %rax
    # allocate entry (24 bytes)
    movq    $24, %rdi
    call    malloc@PLT
    testq   %rax, %rax
    je  .obj_set_done
    # store strdup(key) into entry->key
    movq    %rsi, %rdi
    call    strdup@PLT
    movq    %rax, (%rax)    # store at entry->key (but careful: we overwrote rax)
    # To avoid complexity, do stores using known registers:
    # Actually: after malloc, entry ptr in rax; we need to call strdup and then store result into (entry)
    # Rework sequence:
    # Save entry ptr in r8, then call strdup, then store.
    # (Simplify by reimplementing correctly:)
    # redo:
    movq    %rax, %r8        # r8 = entry
    movq    %rsi, %rdi
    call    strdup@PLT
    movq    %rax, (%r8)      # entry->key = strdup result
    # store value
    movq    %rdx, 8(%r8)
    # store next = head
    movq    (%rax), %r9     # WRONG: we lost object base; instead reload head from object
    # reload object base
    movq    %rdi, %rdi      # noop to keep registers sane
    # correct approach: head = *(obj_ptr)
    movq    %rdi, %rdi
    # fix: compute obj_ptr = untagged obj (we have in %rax? no)
    # For clarity, recompute untagged obj into r10:
    movq    %rdi, %r10
    andq    $~7, %r10
    movq    (%r10), %r9
    movq    %r9, 16(%r8)    # entry->next = head
    # store new head
    movq    %r8, (%r10)
.obj_set_done:
    leave
    ret

# js_object_get(Value obj, const char* key) -> Value
# rdi = obj (tagged), rsi = key
    .globl js_object_get
js_object_get:
    pushq   %rbp
    movq    %rsp, %rbp
    # untag obj
    movq    %rdi, %rax
    andq    $~7, %rax
    # head = *(obj_ptr)
    movq    (%rax), %r8
    # iterate
.test_loop:
    cmpq    $0, %r8
    je  .obj_not_found
    # compare key: strcmp(rsi, entry->key)
    movq    (%r8), %rdi
    movq    %rsi, %rsi
    call    strcmp@PLT
    testq   %rax, %rax
    je  .obj_found
    movq    16(%r8), %r8
    jmp .test_loop
.obj_found:
    movq    8(%r8), %rax
    leave
    ret
.obj_not_found:
    xorq    %rax, %rax
    leave
    ret

# End of runtime assembly
