/* ==============================================================================
 * SAPHAL OS - ARM64 (AArch64) Kernel Entry & Exception Vector Table
 * Target: MediaTek Helio G80 (MT6769V) / Xiaomi Redmi 9
 * ============================================================================== */

.section ".text.head", "ax"
.global _start_arm64
.global arm64_vector_table

_start_arm64:
    /* Standard ARM64 Linux Kernel Image Header */
    b       real_start              /* code0: Branch to start */
    .word   0                       /* code1: Reserved */
    .quad   0x80000                 /* text_offset */
    .quad   0x200000                /* image_size */
    .quad   0x00                    /* flags: Little-endian, 4K page */
    .quad   0                       /* res2 */
    .quad   0                       /* res3 */
    .quad   0                       /* res4 */
    .ascii  "ARM\x64"               /* magic: 0x644d5241 */
    .word   0                       /* res5 */

real_start:
    /* Save Device Tree pointer passed in X0 by bootloader / LittleKernel */
    mov     x21, x0

    /* Mask all DAIF interrupts (Debug, SError, IRQ, FIQ) */
    msr     daifset, #0xf

    /* Ensure we are executing at EL1 */
    mrs     x1, CurrentEL
    lsr     x1, x1, #2
    cmp     x1, #1
    b.eq    el1_entry

    /* If booted in EL2 (Hypervisor), drop to EL1 */
    cmp     x1, #2
    b.ne    hang_cpu
    mov     x2, #(1 << 31)          /* EL1 is AArch64 */
    msr     hcr_el2, x2
    mov     x2, #0x3c5              /* EL1h (SP_EL1) with interrupts masked */
    msr     spsr_el2, x2
    adr     x2, el1_entry
    msr     elr_el2, x2
    eret

el1_entry:
    /* Set Vector Base Address Register (VBAR_EL1) */
    adr     x0, arm64_vector_table
    msr     vbar_el1, x0
    isb

    /* Initialize EL1 Stack Pointer */
    adr     x0, arm64_stack_top
    mov     sp, x0

    /* Pass DTB pointer in X0 to C kernel entry point */
    mov     x0, x21
    bl      arm64_kmain

hang_cpu:
    wfe
    b       hang_cpu

/* ==============================================================================
 * AArch64 Exception Vector Table (Aligned to 2KB / 0x800)
 * ============================================================================== */
.align 11
arm64_vector_table:
    /* Current EL with SP0 */
    .align 7
    b       exc_sync_sp0
    .align 7
    b       exc_irq_sp0
    .align 7
    b       exc_fiq_sp0
    .align 7
    b       exc_serror_sp0

    /* Current EL with SPx */
    .align 7
    b       exc_sync_spx
    .align 7
    b       exc_irq_spx
    .align 7
    b       exc_fiq_spx
    .align 7
    b       exc_serror_spx

    /* Lower EL using AArch64 */
    .align 7
    b       exc_sync_lower64
    .align 7
    b       exc_irq_lower64
    .align 7
    b       exc_fiq_lower64
    .align 7
    b       exc_serror_lower64

    /* Lower EL using AArch32 */
    .align 7
    b       exc_sync_lower32
    .align 7
    b       exc_irq_lower32
    .align 7
    b       exc_fiq_lower32
    .align 7
    b       exc_serror_lower32

/* Stub Handlers */
exc_sync_sp0:
exc_irq_sp0:
exc_fiq_sp0:
exc_serror_sp0:
exc_sync_spx:
exc_irq_spx:
exc_fiq_spx:
exc_serror_spx:
exc_sync_lower64:
exc_irq_lower64:
exc_fiq_lower64:
exc_serror_lower64:
exc_sync_lower32:
exc_irq_lower32:
exc_fiq_lower32:
exc_serror_lower32:
    wfe
    b       .

.section ".bss"
.align 4
arm64_stack:
    .space 16384                    /* 16 KB Kernel Stack */
arm64_stack_top:
