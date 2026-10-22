#!/usr/bin/env bash
# ==============================================================================
# SAPHAL OS - Hardened Initramfs RootFS Builder
# Project: SAPHAL OS (Security-Aware Platform for Hardened Applications and Layered OS)
# ==============================================================================

set -eo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
ROOTFS_DIR="$BUILD_DIR/rootfs"
INITRAMFS_OUT="$BUILD_DIR/initramfs.cpio.gz"

echo "=========================================================="
echo "  Building SAPHAL OS Hardened Userspace RootFS"
echo "=========================================================="

rm -rf "$ROOTFS_DIR"
mkdir -p "$ROOTFS_DIR"/{bin,sbin,etc/saphal,proc,sys,dev,tmp,run,mnt,root}

echo "[1/4] Compiling static system binaries..."
gcc -static -O2 -Wall -Wextra "$PROJECT_DIR/services/init.c" -o "$ROOTFS_DIR/init"
gcc -static -O2 -Wall -Wextra "$PROJECT_DIR/services/secd.c" -o "$ROOTFS_DIR/bin/saphal-secd"
gcc -static -O2 -Wall -Wextra "$PROJECT_DIR/ui/framebuffer.c" "$PROJECT_DIR/ui/splash.c" -o "$ROOTFS_DIR/bin/saphal-splash"
gcc -static -O2 -Wall -Wextra "$PROJECT_DIR/userspace/diagnostics.c" "$PROJECT_DIR/userspace/cli/saphal_sh.c" -o "$ROOTFS_DIR/bin/saphal-sh"

# Copy owner avatar into rootfs
if [ -f "$PROJECT_DIR/assets/owner_avatar.ppm" ]; then
    cp "$PROJECT_DIR/assets/owner_avatar.ppm" "$ROOTFS_DIR/etc/saphal/owner_avatar.ppm"
fi

# Create standard symlinks
ln -sf /bin/saphal-sh "$ROOTFS_DIR/bin/sh"
ln -sf /bin/saphal-sh "$ROOTFS_DIR/bin/bash"

echo "[2/4] Generating system configuration files..."
cat << 'EOF' > "$ROOTFS_DIR/etc/os-release"
NAME="SAPHAL OS"
VERSION="0.1.0-alpha"
ID=saphalos
PRETTY_NAME="SAPHAL OS v0.1.0-alpha (Hardened Non-Android Mobile)"
TAGLINE="Resilient, Transparent, Hardened"
HOME_URL="https://github.com/safalamsall/SaphalOS"
EOF

cat << 'EOF' > "$ROOTFS_DIR/etc/hostname"
saphal-os
EOF

echo "[3/4] Packaging CPIO archive..."
(
    cd "$ROOTFS_DIR"
    find . -mindepth 1 | cpio -o -H newc | gzip -9 > "$INITRAMFS_OUT"
)

echo "[4/4] Verified output:"
echo "  [*] Target Archive: $INITRAMFS_OUT"
echo "  [*] Archive Size  : $(du -h "$INITRAMFS_OUT" | cut -f1)"
echo "=========================================================="
echo "  RootFS Build Successful!"
echo "=========================================================="
