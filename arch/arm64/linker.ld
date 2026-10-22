/* ==============================================================================
 * SAPHAL OS - ARM64 Linker Script
 * Target: MediaTek MT6769 Memory Map (DRAM base 0x40000000, Kernel at 0x40080000)
 * ============================================================================== */

OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start_arm64)

SECTIONS
{
    . = 0x40080000;

    .text.head : {
        *(.text.head)
    }

    .text : ALIGN(4096) {
        *(.text*)
    }

    .rodata : ALIGN(4096) {
        *(.rodata*)
    }

    .data : ALIGN(4096) {
        *(.data*)
    }

    .bss : ALIGN(4096) {
        *(.bss*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
