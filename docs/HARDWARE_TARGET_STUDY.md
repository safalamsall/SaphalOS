# Xiaomi Redmi 9 Verified Hardware Platform Study (Codename: cattail)

## 1. Verified Device Profile (from Device Info 3.4.4.0)

| Parameter | Verified Value | Notes |
| :--- | :--- | :--- |
| **Market Name** | Xiaomi Redmi 9 (India Edition) | Model: `M2006C3MII` |
| **Device Codename** | **`cattail`** | Board: `cattail` |
| **SoC Platform** | **MediaTek Helio G35 (`MT6765G`)** | 12nm FinFET process |
| **CPU Architecture**| Octa-core ARM Cortex-A53 | 4x 2.3 GHz (Gold) + 4x 1.8 GHz (Silver) |
| **GPU Architecture**| **PowerVR Rogue GE8320** | Imagination Technologies (OpenGL ES 3.2) |
| **Display Panel** | **1600 x 720 (HD+)** | 6.53 inches, 269 ppi, 320 dpi (XHDPI) |
| **RAM** | 4 GB LPDDR4X (3780 MB) | 1600 MHz clock |
| **Internal Storage**| 64 GB eMMC 5.1 (59.60 GB usable) | Partition table: Dynamic Super |
| **Stock Kernel** | Linux 4.9.190 (`4.9.190-perf-g726745c074e7`) | Android 10 (API 29), MIUI 12 |
| **Accelerometer** | `sc7a20` sensor | Power: 0.001 mA |
| **Bootloader State**| **LOCKED** | Must be unlocked via Mi Unlock Tool |

---

## 2. Boot Chain & Security Analysis for MT6765

1. **BootROM**: MediaTek MT6765 BootROM initializes Cortex-A53 core 0 in AArch32 mode, reads security fuses, and loads preloader into SRAM.
2. **Preloader**: Located in eMMC hardware partition `boot0`/`boot1`. Initializes LPDDR4X memory timings specific to `MT6765G`.
3. **ARM Trusted Firmware (BL31)**: Switches processor to AArch64 EL3 runtime.
4. **LittleKernel (LK)**: Validates boot image header v2, parses cmdline, loads device tree, and jumps to kernel at `0x40080000`.
5. **Bootloader Lock Enforcement**: The stock LittleKernel will **refuse to boot any modified or custom kernel** until the user completes the official bootloader unlock process (`fastboot oem unlock` / Mi Unlock).

---

## 3. Strict Safety Policy

> [!CAUTION]
> 1. Do NOT flash images built for `lancelot` (Helio G80). They are hardware-incompatible with `cattail` (Helio G35).
> 2. Partitions `nvram`, `nvdata`, `protect_f`, and `protect_s` must NEVER be modified or erased.
> 3. First physical boot will be conducted strictly in RAM via `fastboot boot saphal_boot.img` once the bootloader is unlocked.
