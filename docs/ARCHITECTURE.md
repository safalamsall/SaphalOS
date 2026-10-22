# SAPHAL OS: System Architecture & Technical Specification

## 1. Executive Summary & Identity
- **OS Name**: SAPHAL OS (*Security-Aware Platform for Hardened Applications and Layered OS*)
- **Tagline**: Resilient, Transparent, Hardened
- **Philosophy**: Zero-telemetry, strict privilege separation, minimal attack surface, verifiable source tree.
- **Architectural Policy**: Independent system architecture. Free of Android framework, AOSP, Google Play Services, and Binder IPC bloatware.

---

## 2. Evaluation of Architectural Options

The user prompt requires a rigorous comparative analysis across three development paths:

| Criterion | Option A: Independent ARM64 Kernel | Option B: Linux Kernel + Custom SAPHAL Userspace | Option C: Hybrid Architecture (Recommended) |
| :--- | :--- | :--- | :--- |
| **Independence** | Complete (100% custom from entry point) | Userspace & security independent; relies on upstream Linux kernel | Independent microkernel core for research + Linux smartphone platform |
| **Development Complexity** | Extremely high (~10,000+ hours to write modern peripheral drivers) | Manageable; focus on security, init, shell, services, GUI | Structured into clear, realistic phases |
| **Display & MIPI DSI** | Requires reverse-engineering MediaTek display controller registers | Supported via Linux DRM/KMS or fbdev driver stack | Researched on QEMU, deployed via DRM |
| **GPU Acceleration** | Nearly impossible (Mali-G52 Valhall firmware is undocumented) | Supported via Panfrost open-source driver or vendor blob | Software rasterizer in QEMU; Panfrost on hardware |
| **Touchscreen Input** | Custom I2C bus driver + Goodix/Focaltech protocol driver required | Standard Linux `evdev` interface | PS/2 in QEMU; `evdev` on physical device |
| **Cellular & Baseband** | Impossible without proprietary MTK CCCI (Cross Core Comm Interface) | Handled via RIL daemon or `Ofono` over CCCI driver | Mocked in QEMU; driver abstraction on hardware |
| **Maintenance Effort** | Prohibitive for a solo developer | Sustainable long-term | Cleanly decoupled and scalable |
| **Redmi 9 Feasibility** | Extremely low probability of full phone boot | High probability (similar to postmarketOS / Linux phone ports) | Immediate runnable code today + real phone path |

### Recommendation: Option C (Hybrid Architecture)
1. **Tier 1 (Microkernel Research Core)**: The current bootable x86-32/ARM64 educational core implemented in [`boot/`](file:///home/Saphal/SaphalOs/boot) and [`kernel/`](file:///home/Saphal/SaphalOs/kernel). It boots directly in QEMU in under 1 second, allows rapid testing of low-level OS algorithms (memory allocators, GDT/IDT tables, capability masks, system call dispatchers, interactive shells), and has zero third-party OS dependencies.
2. **Tier 2 (Physical Smartphone Platform)**: When deploying to the physical Xiaomi Redmi 9, we pair the hardened Linux kernel (which encapsulates the proprietary MediaTek MT6769V device tree, PMIC, and display drivers) with **100% custom SAPHAL OS userspace** (custom `saphal-init`, security supervisor `saphal-secd`, shell `saphal-sh`, and custom GUI stack). Android/AOSP is completely excluded.

---

## 3. Tier 1 Prototype Memory Map & Execution Flow

```
+-------------------------------------------------------+ 0x100000 (1 MB)
| Extended Memory (Paging / Heap / Buffers)             |
+-------------------------------------------------------+ 0x090000
| Protected Mode Stack (grows downwards)                |
+-------------------------------------------------------+ 0x080000
| Unused Conventional Memory Space                      |
+-------------------------------------------------------+ 0x010000 (64 KB)
| Stage 2 Kernel Binary (.text, .rodata, .data, .bss)   |
| Linked at 0x8000, max size 32 KB loaded by MBR        |
+-------------------------------------------------------+ 0x008000
| Stage 1 MBR Boot Sector (512 bytes)                   |
+-------------------------------------------------------+ 0x007C00
| Real Mode Stack & BIOS Data Area (BDA)                |
+-------------------------------------------------------+ 0x000400
| Interrupt Vector Table (IVT - Real Mode)              |
+-------------------------------------------------------+ 0x000000
```

---

## 4. Security Architecture & Threat Model

1. **Privilege Separation (Ring 0 vs Ring 3)**:
   - Kernel code and hardware drivers execute strictly in Ring 0.
   - Applications and userspace processes execute in Ring 3.
   - Transition between Ring 3 and Ring 0 occurs only through hardware-controlled Trap Gates (INT 0x80).
2. **Memory Stack Canaries**:
   - Critical kernel memory structures and stack boundaries are guarded with canary words (`0xDEADBEEF`).
   - The security subsystem continuously audits canary integrity (`sec_verify_integrity()`). Any memory overrun triggers an immediate kernel alert.
3. **Least Privilege & Zero Telemetry**:
   - The operating system collects zero behavioral telemetry, phone identifiers, or usage tracking.
   - The shell (`saphal-sh`) enforces role-based command authorization with audit logs emitted directly to the serial console.
