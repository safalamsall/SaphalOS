#!/usr/bin/env python3
"""
SAPHAL OS - Android Boot Image Header v2 Generator
Creates standard MediaTek-compatible boot.img structures
"""

import sys
import os
import struct
import hashlib

BOOT_MAGIC = b"ANDROID!"
BOOT_PAGE_SIZE = 2048

def pad_to_page(data, page_size=BOOT_PAGE_SIZE):
    pad_len = (page_size - (len(data) % page_size)) % page_size
    return data + (b"\x00" * pad_len)

def create_boot_image(kernel_path, ramdisk_path, dtb_path, output_path, cmdline=""):
    print("[+] Reading input components:")
    with open(kernel_path, "rb") as f:
        kernel_data = f.read()
    print(f"  [*] Kernel  : {kernel_path} ({len(kernel_data)} bytes)")

    with open(ramdisk_path, "rb") as f:
        ramdisk_data = f.read()
    print(f"  [*] Ramdisk : {ramdisk_path} ({len(ramdisk_data)} bytes)")

    dtb_data = b""
    if dtb_path and os.path.exists(dtb_path):
        with open(dtb_path, "rb") as f:
            dtb_data = f.read()
        print(f"  [*] DTB     : {dtb_path} ({len(dtb_data)} bytes)")
    else:
        print("  [*] DTB     : None provided (empty placeholder for testing)")

    kernel_size = len(kernel_data)
    ramdisk_size = len(ramdisk_data)
    dtb_size = len(dtb_data)

    kernel_addr = 0x40080000
    ramdisk_addr = 0x44000000
    second_size = 0
    second_addr = 0x40F00000
    tags_addr = 0x40000100
    page_size = BOOT_PAGE_SIZE
    header_version = 2
    os_version = (11 << 25) | (10 << 21) # Android 11 / Oct 2026

    name = b"SAPHAL_OS_v0.1\x00".ljust(16, b"\x00")
    cmdline_bytes = cmdline.encode("utf-8").ljust(512, b"\x00")

    # Calculate SHA hash over kernel, ramdisk, dtb
    sha = hashlib.sha1()
    sha.update(kernel_data)
    sha.update(ramdisk_data)
    if dtb_data:
        sha.update(dtb_data)
    id_hash = sha.digest().ljust(32, b"\x00")

    extra_cmdline = b"".ljust(1024, b"\x00")
    recovery_dtbo_size = 0
    recovery_dtbo_offset = 0
    header_size = 1660

    dtb_addr = 0x47000000

    # Pack header v2
    header = struct.pack(
        "<8s10I16s512s32s1024s4IQI",
        BOOT_MAGIC,
        kernel_size, kernel_addr,
        ramdisk_size, ramdisk_addr,
        second_size, second_addr,
        tags_addr, page_size,
        header_version, os_version,
        name,
        cmdline_bytes,
        id_hash,
        extra_cmdline,
        recovery_dtbo_size,
        0, 0, # offset low/high
        header_size,
        dtb_size,
        dtb_addr
    )

    header_padded = pad_to_page(header)
    kernel_padded = pad_to_page(kernel_data)
    ramdisk_padded = pad_to_page(ramdisk_data)
    dtb_padded = pad_to_page(dtb_data) if dtb_data else b""

    with open(output_path, "wb") as f:
        f.write(header_padded)
        f.write(kernel_padded)
        f.write(ramdisk_padded)
        if dtb_padded:
            f.write(dtb_padded)

    total_size = os.path.getsize(output_path)
    print(f"[+] Successfully generated Android Boot Image: {output_path}")
    print(f"  [*] Total Size: {total_size} bytes ({total_size / 1024 / 1024:.2f} MB)")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print(f"Usage: {sys.argv[0]} <kernel> <ramdisk> <output_boot.img> [dtb] [cmdline]")
        sys.exit(1)

    k = sys.argv[1]
    r = sys.argv[2]
    out = sys.argv[3]
    d = sys.argv[4] if len(sys.argv) > 4 else None
    c = sys.argv[5] if len(sys.argv) > 5 else "boot=saphal-init console=tty0 enforcing=1"

    create_boot_image(k, r, d, out, c)
