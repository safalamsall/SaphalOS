#!/usr/bin/env bash
# ==============================================================================
# SAPHAL OS - Automated Headless QEMU Boot Verification Harness
# Project: SAPHAL OS (Security-Aware Platform for Hardened Applications and Layered OS)
# ==============================================================================

set -eo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMG="$PROJECT_DIR/build/saphal_os.img"
LOG_FILE="$PROJECT_DIR/build/test_boot.log"
TIMEOUT_SEC=4

if [ ! -f "$IMG" ]; then
    echo "[!] Error: Disk image $IMG not found. Building now..."
    make -C "$PROJECT_DIR" build
fi

echo "=========================================================="
echo "  SAPHAL OS Boot Test: Automated QEMU Verification"
echo "=========================================================="
echo "[+] Target Image: $IMG"
echo "[+] Log File: $LOG_FILE"

# Launch QEMU headlessly in the background
"$PROJECT_DIR/tools/qemu.sh" \
    -drive format=raw,file="$IMG",index=0,media=disk \
    -serial file:"$LOG_FILE" \
    -display none \
    -no-reboot &

QEMU_PID=$!
echo "[+] QEMU process launched with PID: $QEMU_PID"

# Wait for kernel boot sequence
sleep "$TIMEOUT_SEC"

# Safely kill QEMU
if kill -0 "$QEMU_PID" 2>/dev/null; then
    kill "$QEMU_PID" 2>/dev/null || true
    wait "$QEMU_PID" 2>/dev/null || true
fi

echo "[+] QEMU execution terminated."
echo "----------------------------------------------------------"
echo "Recorded Serial Boot Log:"
echo "----------------------------------------------------------"
cat "$LOG_FILE"
echo "----------------------------------------------------------"

# Assert critical markers
PASS=1

assert_marker() {
    local marker="$1"
    if grep -Fq "$marker" "$LOG_FILE"; then
        echo -e "  [PASS] Assertion Met: '$marker'"
    else
        echo -e "  [FAIL] Missing Marker: '$marker'"
        PASS=0
    fi
}

echo "[+] Verifying Boot Assertions:"
assert_marker "[SAPHAL-BOOT] Stage 1 MBR Bootloader Active"
assert_marker "Switching to 32-bit Protected Mode"
assert_marker "SAPHAL OS Kernel v0.1.0-alpha Initializing..."
assert_marker "[SAPHAL-BOOT-VERIFY] Memory Canaries OK"
assert_marker "[SAPHAL-BOOT-VERIFY] Timer IRQ0 OK"
assert_marker "[SAPHAL-BOOT-VERIFY] Protected Mode Ring 0 OK"
assert_marker "[SAPHAL-SHELL] Shell Prompt Active"

if grep -iq "panic" "$LOG_FILE"; then
    echo "[FAIL] Kernel Panic detected in log output!"
    PASS=0
fi

echo "=========================================================="
if [ "$PASS" -eq 1 ]; then
    echo "  RESULT: ALL BOOT ASSERTIONS PASSED (100% SUCCESS)"
    echo "=========================================================="
    exit 0
else
    echo "  RESULT: BOOT VERIFICATION FAILED"
    echo "=========================================================="
    exit 1
fi
