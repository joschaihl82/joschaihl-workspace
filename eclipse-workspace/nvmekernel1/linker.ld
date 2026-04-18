OUTPUT_FORMAT(pe-x86-64)
ENTRY(efi_main)

SECTIONS
{
  . = 0x100000;
  .text : { *(.text*) *(.rodata*) }
  .data : { *(.data*) }
  .bss  : { *(.bss*) *(COMMON) }
}
