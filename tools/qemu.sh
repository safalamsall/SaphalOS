#!/bin/bash
# Saphal OS - QEMU Wrapper Script
# Allows executing the self-contained QEMU installation seamlessly

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
QEMU_DIR="$PROJECT_ROOT/tools/qemu/root"

export LD_LIBRARY_PATH="$QEMU_DIR/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"

# Run system emulator with local BIOS & firmware paths and -net none by default
exec "$QEMU_DIR/usr/bin/qemu-system-x86_64" \
    -L "$QEMU_DIR/usr/share/seabios" \
    -L "$QEMU_DIR/usr/share/qemu" \
    -net none \
    "$@"
