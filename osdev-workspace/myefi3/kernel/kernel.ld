/* kernel/linker_high.ld - HighHalf VMA */
ENTRY(_start)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
  . = 0xFFFF800000010000;

  .text ALIGN(0x1000) : {
    *(.text._start)
    *(.text*)
  }

  .rodata ALIGN(0x1000) : { *(.rodata*) }

  .data ALIGN(0x1000) : { *(.data*) }

  .bss ALIGN(0x1000) : {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  }

  /* Reserve a small stack region */
  .stack ALIGN(0x1000) : {
    __stack_start = .;
    . = . + 0x4000;
    __stack_end = .;
  }

  /DISCARD/ : { *(.eh_frame) }
}

