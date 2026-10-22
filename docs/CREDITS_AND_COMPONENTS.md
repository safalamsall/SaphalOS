# SAPHAL OS: Credits, Attributions, and Component Inventory

SAPHAL OS is an independent operating system project committed to total transparency. In accordance with project policy, all third-party software, specifications, compilers, firmware, and toolchains are explicitly credited below. No false claims of proprietary whole-cloth development are made.

---

## 1. Toolchains and Compilers
- **GNU Compiler Collection (GCC v14/v15)**
  - *Role*: Compiles freestanding C source files into 32-bit ELF object code (`-m32 -ffreestanding -nostdlib`).
  - *Origin*: Free Software Foundation (FSF).
  - *License*: GNU General Public License (GPLv3+).
- **Netwide Assembler (NASM v3.01)**
  - *Role*: Assembles Stage 1 MBR bootloader into flat binary (`-f bin`) and low-level interrupt/entry routines into ELF32 (`-f elf32`).
  - *Origin*: NASM Development Team.
  - *License*: BSD 2-Clause License.
- **GNU Binutils (ld, objdump, readelf)**
  - *Role*: Links object files into a contiguous protected-mode binary using custom linker scripts (`kernel/linker.ld`).
  - *Origin*: Free Software Foundation (FSF).
  - *License*: GNU GPLv3+.

---

## 2. Firmware and Emulation
- **QEMU (Quick Emulator v11.1.0)**
  - *Role*: Hardware platform emulator used for development, execution, and automated testing of x86-32 system images.
  - *Origin*: Fabrice Bellard and the QEMU Project contributors.
  - *License*: GNU GPLv2.
- **SeaBIOS (v1.17.0)**
  - *Role*: Open-source x86 legacy 16-bit BIOS implementation providing Power-On Self-Test (POST), INT 10h video services, and INT 13h disk read extensions.
  - *Origin*: Kevin O'Connor and SeaBIOS contributors.
  - *License*: GNU LGPLv3.

---

## 3. Hardware Specifications and Industry Standards
- **x86 Architecture Specification**: Intel 64 and IA-32 Architectures Software Developer's Manuals (Volume 1, 2, 3) for GDT, IDT, 8259 PIC remapping, and protected mode transitions.
- **VGA CRT Controller (CRTC)**: IBM VGA Hardware Registers standard (ports 0x3D4/0x3D5 and memory-mapped frame buffer at 0xB8000).
- **UART 16550 Serial Controller**: National Semiconductor PC16550D Universal Asynchronous Receiver/Transmitter protocol (ports 0x3F8-0x3FF).
- **Intel 8253/8254 PIT**: Programmable Interval Timer specification (ports 0x40-0x43).
- **Intel 8042 PS/2 Controller**: Keyboard scancode set 1 protocol (ports 0x60/0x64).
