; ==============================================================================
; SAPHAL OS - Stage 2 Kernel Assembly Entry Point
; Links at physical address 0x8000 (.text.boot)
; ==============================================================================

[bits 32]
section .text.boot

global _start
extern kmain

_start:
    ; Ensure interrupts remain disabled until IDT and PIC are initialized
    cli

    ; Call kernel main C entry
    call kmain

    ; Kernel should never return. If it does, safely halt CPU
    cli
.hang:
    hlt
    jmp .hang
