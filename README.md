# SAPHAL OS
> **Security-Aware Platform for Hardened Applications and Layered OS**  
> *Resilient. Transparent. Hardened.*

SAPHAL OS is an independent operating system project engineered for security, verifiable system architecture, and deployment to physical smartphone hardware (Xiaomi Redmi 9 / MediaTek Helio G80).

SAPHAL OS explicitly rejects Android and AOSP as its foundation, providing its own boot identity, low-level drivers, memory protection models, direct Framebuffer GUI, and hardened userspace environment.

---

## 1. System Architecture Overview

```
                          +------------------------------------------+
                          |   SAPHAL OS Custom Mobile Userspace      |
                          |  - saphal-init (PID 1 Mounter & Reaper)  |
                          |  - saphal-secd (Security Supervisor)     |
                          |  - saphal-splash (1080x2340 FB Engine)   |
                          |  - saphal-sh (Smartphone CLI & Diag)     |
                          +------------------------------------------+
                                               |
                +------------------------------+------------------------------+
                |                                                             |
+--------------------------------+                          +-----------------------------------+
|   Tier 1: QEMU Micro-Core      |                          |   Tier 2: Smartphone Platform     |
|   - Custom MBR Bootloader      |                          |   - Hardened Linux Kernel Core    |
|   - x86-32 Protected Ring 0    |                          |   - ARM64 / MediaTek MT6769V DTB  |
|   - GDT/IDT/8259 PIC Remapper  |                          |   - Direct /dev/fb0 Mobile Engine |
|   - In-Memory Canaries         |                          |   - Android Boot Image v2 Header  |
+--------------------------------+                          +-----------------------------------+
```

---

## 2. Directory Layout

```
SaphalOs/
├── Makefile                          # Master build & test orchestration
├── README.md                         # Project documentation & execution guide
├── arch/
│   └── arm64/                        # ARM64 Architecture & MediaTek MT6769 Support
│       ├── boot.S                    # AArch64 entry point & 2KB Vector Table
│       ├── mtk_uart.c / mtk_uart.h   # MediaTek AP UART0 (0x11002000) driver
│       ├── gic.c / gic.h             # ARM GICv2/v3 base definitions
│       └── linker.ld                 # 0x40080000 DRAM kernel linker script
├── boot/
│   ├── boot.asm                      # Stage 1: 512-byte MBR bootloader (Fast A20, Real -> Protected)
│   └── stage2_entry.asm              # Stage 2: 32-bit assembly entry point
├── configs/
│   ├── redmi9_defconfig              # MediaTek Helio G80 kernel configuration
│   └── security_policy.conf          # Least-privilege capability & partition policy
├── drivers/
│   ├── io.h                          # Port I/O primitives
│   ├── vga.c / vga.h                 # VGA 80x25 terminal driver with SaphalOS visual branding
│   ├── serial.c / serial.h           # UART 16550 serial driver (COM1: 0x3F8)
│   ├── timer.c / timer.h             # PIT 8254 clock driver (100 Hz, 10ms ticks)
│   └── keyboard.c / keyboard.h       # PS/2 keyboard controller & input buffer
├── kernel/
│   ├── kernel.c / kernel.h           # Tier 1 microkernel orchestration
│   ├── idt.c / idt.h / idt_asm.asm   # IDT setup, CPU exception traps, INT 0x80 syscall gate
│   ├── pic.c / pic.h                 # 8259 PIC remapping (0x20..0x2F) & EOI management
│   └── linker.ld                     # Kernel linker script (0x8000 flat binary)
├── security/
│   ├── security.c / security.h       # Ring 0 audit & memory stack canaries (0xDEADBEEF)
│   └── shell.c / shell.h             # Early kernel shell
├── services/
│   ├── init.c / init.h               # Hardened PID 1 Init System (non-Android, non-systemd)
│   └── secd.c / secd.h               # Security Monitoring Daemon (mount audits, process isolation)
├── ui/
│   ├── framebuffer.c / framebuffer.h # Direct Linux Framebuffer (/dev/fb0) & virtual 1080x2340 engine
│   └── splash.c / splash.h           # Smartphone boot splash & lockscreen renderer
├── userspace/
│   ├── cli/
│   │   ├── saphal_sh.c / saphal_sh.h # Smartphone interactive shell
│   │   └── diagnostics.c / .h        # Hardware diagnostics (Battery, Display, CPU, Thermal)
├── scripts/
│   ├── build_initramfs.sh            # Packages hardened static rootfs into initramfs.cpio.gz
│   ├── generate_bootimg.py           # Android Boot Image v2 builder (Kernel + DTB + Ramdisk)
│   ├── test_boot.sh                  # Headless QEMU automated boot test harness
│   ├── test_userspace.sh             # Userspace integrity & boot.img verification harness
│   └── run_qemu.sh                   # Interactive emulator launcher
├── docs/
│   ├── ARCHITECTURE.md               # Technical specification & threat model
│   ├── CREDITS_AND_COMPONENTS.md     # Full third-party tool and component attribution
│   └── HARDWARE_TARGET_STUDY.md      # Xiaomi Redmi 9 hardware & boot-chain evaluation
└── tools/
    ├── qemu.sh                       # Standalone QEMU wrapper
    └── qemu/                         # Local emulator binaries & SeaBIOS firmware
```

---

## 3. How to Build & Verify Everything

### 1. Build & Run All Automated Tests:
```bash
make test-all
```
This automatically:
- Compiles the x86 Stage 1/Stage 2 microkernel
- Launches QEMU headlessly and verifies all boot assertion markers
- Compiles all 4 static userspace binaries (`init`, `saphal-secd`, `saphal-splash`, `saphal-sh`)
- Builds the hardened `initramfs.cpio.gz`
- Assembles the MediaTek Android Boot Image v2 (`saphal_boot.img`) and validates headers

### 2. Run Interactively in QEMU:
```bash
make run
```

### 3. Generate Standalone Smartphone Boot Image:
```bash
make bootimg
```

---

## 4. Hardware Safety Guarantees

- **No Device Flashing**: No physical device flashing will be performed during development.
- **Evidence-First**: No physical deployment until device board (`lancelot`/`galahad`), display IC, and bootloader state are queried non-destructively.
- **Protected Partitions**: `nvram`, `nvdata`, `protect_f`, and `protect_s` are strictly protected from all write operations.
