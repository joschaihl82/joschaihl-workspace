/* linker.ld - minimal PE/COFF layout for x86_64 UEFI binary
   Compatible with x86_64-w64-mingw32-ld when invoked via gcc driver.
*/
OUTPUT_FORMAT("pei-x86-64")
ENTRY(efi_main)

SECTIONS
{
  /* align start to 4K */
  . = 0x1000;

  /* code and read-only data */
  .text : {
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    *(.rdata)
    *(.rdata.*)
    *(.pdata)
    *(.xdata)
    *(.eh_frame)
  }

  /* initialized data */
  .data : {
    *(.data)
    *(.data.*)
    *(.sdata)
    *(.ctors)
    *(.dtors)
  }

  /* relocations (if any) */
  .reloc : {
    *(.reloc)
  }

  /* uninitialized data */
  .bss : {
    __bss_start = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    __bss_end = .;
  }

  /* Provide placeholder symbols for optional binary blobs */
  PROVIDE(_binary_init_sh_start = 0);
  PROVIDE(_binary_init_sh_end   = 0);
  PROVIDE(_binary_init_sh_size  = 0);

  /* align end */
  . = ALIGN(0x1000);
}
