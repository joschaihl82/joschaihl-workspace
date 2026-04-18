/* linker.ld - bare-metal linker script für x86-64 */

ENTRY(start_bios) /* der einstiegspunkt, der im C-Code definiert ist */

SECTIONS
{
    /* code und daten starten bei speicheradresse 0x0 */
    . = 0x0; 

    /* 1. .text (code) sektion */
    .text : ALIGN(4K)
    {
        *(.text)
        *(.text.*) 
    } 

    /* 2. .rodata (read-only data/konstanten) sektion */
    .rodata : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    /* 3. .data (initialisierte daten) sektion */
    .data : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    /* 4. .bss (uninitialisierte daten) sektion */
    .bss : ALIGN(4K)
    {
        *(.bss)
        *(.bss.*)
    }

    /* WICHTIG: Verwerfe alle vom Host-Linker hinzugefügten Dynamic-Linking-Sektionen */
    /DISCARD/ : 
    {
        *(.interp)
        *(.note*)
        *(.comment)
        *(.gnu*) /* Zielt auf .gnu.hash und andere .gnu-sektionen ab */
        *(.hash)
        *(.dyn*)
        *(.eh_frame) 
        *(.plt)
        *(.got)
        *(.stab)
        *(.stabstr)
    }
}