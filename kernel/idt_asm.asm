; ==============================================================================
; SAPHAL OS - Low-Level Interrupt Service Routine Stubs
; ==============================================================================

[bits 32]
section .text

extern isr_handler
extern irq_handler

; Macro for ISRs without error code (pushes dummy 0 error code)
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0        ; Dummy error code
    push dword %1       ; Interrupt number
    jmp isr_common_stub
%endmacro

; Macro for ISRs with error code pushed by CPU
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push dword %1       ; Interrupt number (error code already on stack)
    jmp isr_common_stub
%endmacro

; Macro for Hardware IRQs
%macro IRQ 2
global irq%1
irq%1:
    push dword 0        ; Dummy error code
    push dword %2       ; Interrupt number
    jmp irq_common_stub
%endmacro

; Define CPU Exception Handlers (0 - 31)
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

; Define Hardware IRQs (32 - 47)
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Define Syscall Handler (INT 0x80 / 128)
ISR_NOERRCODE 128

isr_common_stub:
    pusha               ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

    mov ax, ds          ; Lower 16-bits of eax = ds
    push eax            ; Save the data segment descriptor

    mov ax, 0x10        ; Load kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp            ; Pass pointer to registers_t
    call isr_handler
    add esp, 4          ; Clean up parameter

    pop eax             ; Reload original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                ; Pops edi, esi, ebp...
    add esp, 8          ; Clean up error code and interrupt number
    iret                ; Return from interrupt

irq_common_stub:
    pusha

    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    iret

global idt_flush
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret
