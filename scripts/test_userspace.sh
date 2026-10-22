#!/usr/bin/env bash
# ==============================================================================
# SAPHAL OS - Userspace & Packaging Test Verification Suite
# ==============================================================================

set -eo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

echo "=========================================================="
echo "  Testing SAPHAL OS Userspace & Packaging Pipeline"
echo "=========================================================="

# 1. Build initramfs
"$PROJECT_DIR/scripts/build_initramfs.sh"

INITRAMFS="$BUILD_DIR/initramfs.cpio.gz"
if [ ! -f "$INITRAMFS" ]; then
    echo "[!] Error: $INITRAMFS was not generated!"
    exit 1
fi

echo "[+] Verifying Initramfs Archive Integrity:"
gzip -dc "$INITRAMFS" | cpio -it | grep -E "init|saphal-sh|saphal-secd|saphal-splash|saphal-launcher"

# 2. Compile Device Tree Blob for cattail
echo "[+] Compiling MediaTek MT6769/MT6765 Device Tree (cattail):"
DTB_OUT="$BUILD_DIR/cattail.dtb"
"$PROJECT_DIR/tools/dtc/usr/bin/dtc" -I dts -O dtb -o "$DTB_OUT" "$PROJECT_DIR/arch/arm64/boot/dts/cattail.dts"

# 3. Test boot.img generation
echo "[+] Generating MediaTek Boot Image with cattail DTB:"
MOCK_KERNEL="$BUILD_DIR/kernel.bin"
BOOTIMG_OUT="$BUILD_DIR/saphal_boot.img"

python3 "$PROJECT_DIR/scripts/generate_bootimg.py" \
    "$MOCK_KERNEL" \
    "$INITRAMFS" \
    "$BOOTIMG_OUT" \
    "$DTB_OUT" \
    "console=tty0 boot=saphal-init enforcing=1"

if [ ! -f "$BOOTIMG_OUT" ]; then
    echo "[!] Error: Boot image generation failed!"
    exit 1
fi

# Verify MAGIC header
HEADER_MAGIC=$(head -c 8 "$BOOTIMG_OUT")
if [ "$HEADER_MAGIC" = "ANDROID!" ]; then
    echo "[PASS] Verified Boot Image Magic Header: '$HEADER_MAGIC'"
else
    echo "[FAIL] Unexpected Boot Image Header: '$HEADER_MAGIC'"
    exit 1
fi

echo "=========================================================="
echo "  ALL USERSPACE & PACKAGING TESTS PASSED (100% SUCCESS)"
echo "=========================================================="
