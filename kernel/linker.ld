/* ==============================================================================
 * SAPHAL OS - Stage 2 Kernel Linker Script
 * Targets flat binary mapped at 0x8000 in x86 Protected Mode
 * ============================================================================== */

ENTRY(_start)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x8000;

    .text : ALIGN(4) {
        *(.text.boot)
        *(.text*)
    }

    .rodata : ALIGN(4) {
        *(.rodata*)
    }

    .data : ALIGN(4) {
        *(.data*)
    }

    .bss : ALIGN(4) {
        *(COMMON)
        *(.bss*)
    }

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
