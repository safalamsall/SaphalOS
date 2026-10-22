# SAPHAL OS
> **Security-Aware Platform for Hardened Applications and Layered OS**  
> *Resilient. Transparent. Hardened.*

SAPHAL OS is an independent operating system project engineered for security, verifiable system architecture, and eventual deployment to physical smartphone hardware (Xiaomi Redmi 9).

SAPHAL OS explicitly rejects Android and AOSP as its foundation, providing its own boot identity, low-level drivers, memory protection models, and userspace environment.

---

## 1. Current State & Prototype Status (v0.1.0-alpha)

- **Execution Environment**: Tested and verified under QEMU system emulator on Kali Linux (WSL 2).
- **Core Architecture**: x86 32-bit Protected Mode micro-core (Tier 1 Research Core) with dual VGA and UART 16550 serial console output.
- **Bootloader**: Custom 512-byte Stage 1 MBR bootloader transitioning from 16-bit Real Mode to 32-bit Protected Mode with Fast A20 gate activation and GDT initialization.
- **Kernel Subsystems**:
  - GDT (Ring 0 Kernel Code/Data & Ring 3 User Code/Data descriptors)
  - 8259 Programmable Interrupt Controller (PIC) remapped to vectors `0x20..0x2F`
  - 256-entry Interrupt Descriptor Table (IDT) with CPU exception traps & INT 0x80 syscall gate
  - PIT 8254 Timer driver running at 100 Hz (10ms tick resolution)
  - PS/2 Keyboard driver with scancode-to-ASCII translation and ring buffer
  - Security Subsystem with memory stack canary verification (`0xDEADBEEF`)
  - Interactive Security Shell (`saphal-sh`) supporting `help`, `about`, `secstatus`, `sysinfo`, `test`, `clear`, and `reboot`

---

## 2. Directory Layout

```
SaphalOs/
├── Makefile                 # Master build orchestration
├── README.md                # System documentation
├── boot/
│   ├── boot.asm             # Stage 1: MBR 512-byte bootloader (Real Mode -> Protected Mode)
│   └── stage2_entry.asm     # Stage 2: 32-bit assembly entry point
├── kernel/
│   ├── kernel.c             # Kernel orchestration & initialization flow
│   ├── kernel.h
│   ├── idt.c                # Interrupt Descriptor Table & exception handling
│   ├── idt.h
│   ├── idt_asm.asm          # Low-level ISR & IRQ assembly stubs
│   ├── linker.ld            # Kernel linker script (0x8000 flat binary)
│   ├── pic.c                # 8259 PIC remapping & EOI management
│   └── pic.h
├── drivers/
│   ├── io.h                 # Port I/O inline functions (inb, outb, io_wait)
│   ├── vga.c                # VGA text buffer driver (0xB8000) with custom palette
│   ├── vga.h
│   ├── serial.c             # UART 16550 serial driver (COM1: 0x3F8)
│   ├── serial.h
│   ├── timer.c              # PIT 8254 clock driver (100 Hz)
│   ├── timer.h
│   ├── keyboard.c           # PS/2 keyboard controller & input buffer
│   └── keyboard.h
├── security/
│   ├── security.c           # Memory integrity checks & stack canary auditing
│   └── security.h
├── userspace/
│   ├── shell.c              # SaphalOS interactive security shell (saphal-sh)
│   └── shell.h
├── scripts/
│   ├── run_qemu.sh          # Interactive QEMU runner
│   └── test_boot.sh         # Headless automated boot test harness
├── docs/
│   ├── ARCHITECTURE.md      # Detailed system architecture and threat model
│   ├── CREDITS_AND_COMPONENTS.md # Explicit third-party attribution and tool inventory
│   └── HARDWARE_TARGET_STUDY.md  # Xiaomi Redmi 9 hardware & boot-chain evaluation
└── tools/
    ├── qemu.sh              # Standalone QEMU wrapper
    └── qemu/                # Local emulator binaries & SeaBIOS firmware
```

---

## 3. How to Build & Test

### Build the Bootable Disk Image:
```bash
make clean && make build
```
This produces `build/saphal_os.img` (10 MB raw disk image).

### Run the Automated Boot Verification Test:
```bash
./scripts/test_boot.sh
```
This launches QEMU headlessly, records serial diagnostic logs, asserts that all kernel markers and memory canaries pass, and terminates cleanly.

### Run Interactively in QEMU:
```bash
./scripts/run_qemu.sh
```
Or via make:
```bash
make run
```

---

## 4. Hardware Safety & Redmi 9 Porting Rules

- **Zero-Touch Policy**: The physical Redmi 9 will NEVER be flashed, partitioned, or modified during early development.
- **Evidence-First**: No physical deployment will occur until exact device codename, display IC, and bootloader lock state are verified non-destructively.
- **Attribution Transparency**: All third-party toolchains (GCC, NASM, GNU Binutils), firmware (SeaBIOS), and emulator tools (QEMU) are fully credited in [`docs/CREDITS_AND_COMPONENTS.md`](file:///home/Saphal/SaphalOs/docs/CREDITS_AND_COMPONENTS.md).
