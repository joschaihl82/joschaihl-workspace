/* linker_low.ld - Identity/Low VMA for quick testing */
ENTRY(_start)

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
  . = 0x00100000; /* 1 MiB */

  .text : {
    *(.text._start)
    *(.text*)
  }

  .rodata : { *(.rodata*) }

  .data : { *(.data*) }

  .bss : {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  }

  /DISCARD/ : { *(.eh_frame) }
}

