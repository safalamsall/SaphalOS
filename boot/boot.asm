; ==============================================================================
; SAPHAL OS - Stage 1 MBR Bootloader (x86 16-bit Real Mode -> 32-bit Protected Mode)
; Project: SAPHAL OS (Security-Aware Platform for Hardened Applications and Layered OS)
; Strict 512-byte MBR constraint
; ==============================================================================

[org 0x7C00]
[bits 16]

start:
    ; 1. Set up segment registers and stack below MBR
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x7C00
    cld
    sti

    mov [BOOT_DRIVE], dl    ; Save BIOS boot drive

    ; 2. Initialize Serial COM1 (0x3F8) for early headless diagnostic logging
    call init_serial_early

    ; 3. Print bootloader active banner
    mov si, msg_boot
    call print_both_16

    ; 4. Read Stage-2 Kernel from disk into memory address 0x0000:0x8000
    ; Read 64 sectors (32 KB) starting from Sector 2 (LBA 1)
    mov si, msg_load
    call print_both_16

    mov ah, 0x02            ; BIOS INT 13h: Read Sectors
    mov al, 64              ; 64 sectors (32 KB)
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Sector 2 (1-based, Sector 1 is MBR)
    mov dh, 0               ; Head 0
    mov dl, [BOOT_DRIVE]    ; Boot drive
    mov bx, 0x8000          ; Target destination: ES:BX = 0x0000:0x8000
    int 0x13
    jc disk_error

    mov si, msg_pm
    call print_both_16

    ; 5. Enable A20 Line (Fast A20 Gate)
    in al, 0x92
    or al, 0x02
    out 0x92, al

    ; 6. Enter 32-bit Protected Mode
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:protected_mode_entry

disk_error:
    mov si, msg_err
    call print_both_16
.halt_loop:
    hlt
    jmp .halt_loop

; ------------------------------------------------------------------------------
; Helper Functions
; ------------------------------------------------------------------------------
print_both_16:
    pusha
.loop:
    lodsb
    or al, al
    jz .done
    ; Screen output (BIOS Teletype)
    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    ; Serial output (COM1: 0x3F8)
    mov dx, 0x3FD
.swait:
    in al, dx
    test al, 0x20
    jz .swait
    mov dx, 0x3F8
    mov al, [si - 1]
    out dx, al
    jmp .loop
.done:
    popa
    ret

init_serial_early:
    pusha
    mov dx, 0x3F9           ; Disable interrupts
    xor al, al
    out dx, al

    mov dx, 0x3FB           ; Enable DLAB
    mov al, 0x80
    out dx, al

    mov dx, 0x3F8           ; Divisor low (115200 baud)
    mov al, 0x01
    out dx, al
    mov dx, 0x3F9           ; Divisor high
    xor al, al
    out dx, al

    mov dx, 0x3FB           ; 8N1
    mov al, 0x03
    out dx, al

    mov dx, 0x3FA           ; Enable FIFO, clear buffers
    mov al, 0xC7
    out dx, al

    mov dx, 0x3FC           ; DTR & RTS
    mov al, 0x03
    out dx, al
    popa
    ret

; ------------------------------------------------------------------------------
; Global Descriptor Table (GDT)
; ------------------------------------------------------------------------------
align 4
gdt_start:
    ; 0x00: Null descriptor
    dd 0, 0

    ; 0x08: Kernel Code (Ring 0, Base 0, Limit 4GB)
    dw 0xFFFF, 0x0000
    db 0x00, 10011010b, 11001111b, 0x00

    ; 0x10: Kernel Data (Ring 0, Base 0, Limit 4GB)
    dw 0xFFFF, 0x0000
    db 0x00, 10010010b, 11001111b, 0x00

    ; 0x18: User Code (Ring 3, Base 0, Limit 4GB)
    dw 0xFFFF, 0x0000
    db 0x00, 11111010b, 11001111b, 0x00

    ; 0x20: User Data (Ring 3, Base 0, Limit 4GB)
    dw 0xFFFF, 0x0000
    db 0x00, 11110010b, 11001111b, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10

; ------------------------------------------------------------------------------
; 32-bit Protected Mode Entry Point
; ------------------------------------------------------------------------------
[bits 32]
protected_mode_entry:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000        ; Set stack pointer
    jmp 0x8000              ; Jump to kernel entry

; ------------------------------------------------------------------------------
; Data Strings & MBR Signature
; ------------------------------------------------------------------------------
BOOT_DRIVE: db 0
msg_boot: db 13, 10, "[SAPHAL-BOOT] Stage 1 MBR Bootloader Active", 13, 10, 0
msg_load: db "[SAPHAL-BOOT] Loading Stage-2 Kernel from disk...", 13, 10, 0
msg_pm:   db "[SAPHAL-BOOT] Switching to 32-bit Protected Mode...", 13, 10, 0
msg_err:  db 13, 10, "[SAPHAL-BOOT] Disk read error! Halted.", 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xAA55
