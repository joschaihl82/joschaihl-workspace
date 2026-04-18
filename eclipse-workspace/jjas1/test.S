    .text
    .global main

# main: basic prologue, symbol references, control flow, and SSE usage
main:
    pushq   %rbp
    movq    %rsp, %rbp

    # Load address of message into %rax (immediate symbol reference)
    movq    $msg, %rax

    # Use memory operand (disp(base)) and arithmetic: addq (mem), reg
    # (tests memory parsing and addq encoding)
    addq    (%rax), %rcx        # add first qword pointed by msg to %rcx

    # Call a function defined later (forward reference -> reloc)
    call    func_forward

    # Test conditional jump to forward label (reloc)
    cmpq    $0, %rcx
    je      done_forward

    # SSE: load 16 bytes from data into xmm0, do an addps with another memory operand
    movaps  (%rax), %xmm0       # load from memory at %rax into %xmm0
    addps   (%array), %xmm0     # add packed singles from array into xmm0

    # Store xmm0 back to memory (store form)
    movaps  %xmm0, (%rax)

    # Unconditional jump backward (label defined below)
    jmp     middle

# small function to be called (defined after use)
func_forward:
    pushq   %rbp
    movq    %rsp, %rbp
    # simple arithmetic and return
    movq    $1, %rax
    popq    %rbp
    ret

middle:
    # Demonstrate immediate to register mov and memory store
    movq    $0x1234, %r8       # immediate -> reg
    movq    %r8, (%rax)        # store r8 to memory at %rax (tests reg->mem)

    # Jump to label defined later (forward)
    jmp     done_forward

# data and symbols
    .data
    .align 8
msg:
    .ascii "TestMsg1234"       # string data (no trailing newline)
    .byte 0

    .align 16
array:
    .byte 0x00,0x00,0x80,0x3f  # 1.0f as float (little-endian)
    .byte 0x00,0x00,0x00,0x40  # 2.0f
    .byte 0x00,0x00,0x40,0x40  # 3.0f
    .byte 0x00,0x00,0x80,0x40  # 4.0f

    .bss
    .align 8
buffer:
    .skip 32                   # reserve 32 bytes in bss

done_forward:
    # Epilogue
    movq    %rbp, %rsp
    popq    %rbp
    ret
