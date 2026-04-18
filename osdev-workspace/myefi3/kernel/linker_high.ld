/* linker_high.ld - HighHalf VMA (use with HIGH=1) */
ENTRY(_start)

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
  /* Virtual base: KERNEL_VIRT_BASE + small offset */
  . = 0xFFFF800000010000;

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

