#!/usr/bin/env bash
# ==============================================================================
# SAPHAL OS - Interactive QEMU Launcher
# ==============================================================================

set -eo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMG="$PROJECT_DIR/build/saphal_os.img"

if [ ! -f "$IMG" ]; then
    echo "[+] Building SAPHAL OS image..."
    make -C "$PROJECT_DIR" build
fi

echo "=========================================================="
echo "  Launching SAPHAL OS v0.1.0 in QEMU"
echo "  Console Mode: Dual (VGA display + Serial stdio)"
echo "  Press Ctrl+C or close QEMU window to exit."
echo "=========================================================="

"$PROJECT_DIR/tools/qemu.sh" \
    -drive format=raw,file="$IMG",index=0,media=disk \
    -serial stdio \
    -m 128M \
    "$@"
