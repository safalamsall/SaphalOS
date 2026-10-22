#!/usr/bin/env python3
"""
SAPHAL OS - Automated Headless QEMU Boot & Interactive Verification Harness
Executes kernel in QEMU, pipes commands into saphal-sh, and validates assertions.
"""

import subprocess
import time
import sys
import os

PROJECT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMG = os.path.join(PROJECT_DIR, "build", "saphal_os.img")
LOG_FILE = os.path.join(PROJECT_DIR, "build", "test_boot.log")

if not os.path.exists(IMG):
    print(f"[!] Target image {IMG} not found. Running make build...")
    subprocess.check_call(["make", "-C", PROJECT_DIR, "build"])

print("==========================================================")
print("  SAPHAL OS Boot Test: Automated QEMU Verification")
print("==========================================================")
print(f"[+] Target Image: {IMG}")
print(f"[+] Log File    : {LOG_FILE}")

qemu_cmd = [
    os.path.join(PROJECT_DIR, "tools", "qemu.sh"),
    "-drive", f"format=raw,file={IMG},index=0,media=disk",
    "-serial", "stdio",
    "-display", "none",
    "-no-reboot"
]

proc = subprocess.Popen(
    qemu_cmd,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    text=True
)

print(f"[+] QEMU process launched with PID: {proc.pid}")

# Wait for kernel boot sequence, then feed interactive commands
try:
    time.sleep(1.5)
    proc.stdin.write("owner\n")
    proc.stdin.flush()
    time.sleep(0.7)
    proc.stdin.write("test\n")
    proc.stdin.flush()
    time.sleep(0.7)
    proc.stdin.write("sysinfo\n")
    proc.stdin.flush()
    time.sleep(1.0)
except Exception as e:
    print(f"[!] Error piping commands: {e}")
finally:
    proc.terminate()
    output, _ = proc.communicate()

# Save log
os.makedirs(os.path.dirname(LOG_FILE), exist_ok=True)
with open(LOG_FILE, "w") as f:
    f.write(output)

print("[+] QEMU execution terminated.")
print("----------------------------------------------------------")
print("Recorded Serial Boot & Interactive Execution Log:")
print("----------------------------------------------------------")
print(output)
print("----------------------------------------------------------")

# Assert critical markers
assertions = [
    "[SAPHAL-BOOT] Stage 1 MBR Bootloader Active",
    "Switching to 32-bit Protected Mode",
    "SAPHAL OS Kernel v0.1.0-alpha Initializing...",
    "[SAPHAL-BOOT-VERIFY] Memory Canaries OK",
    "[SAPHAL-BOOT-VERIFY] Timer IRQ0 OK",
    "[SAPHAL-BOOT-VERIFY] Protected Mode Ring 0 OK",
    "[SAPHAL-SHELL] Shell Prompt Active",
    "DEVICE OWNER PROFILE: SAPHAL",
    "Registered Owner : Saphal",
    "Target Handset   : Xiaomi Redmi 9 India (cattail / MT6765G)",
    "Starting Kernel Subsystem Verification",
    "Memory Integrity & Canaries : PASSED (0xDEADBEEF verified)",
    "All Kernel Self-Tests PASSED",
]

passed = True
print("[+] Verifying Boot & Interactive Shell Execution Assertions:")
for marker in assertions:
    if marker in output:
        print(f"  [PASS] Assertion Met: '{marker}'")
    else:
        print(f"  [FAIL] Missing Marker: '{marker}'")
        passed = False

if "panic" in output.lower():
    print("[FAIL] Kernel Panic detected in log output!")
    passed = False

print("==========================================================")
if passed:
    print("  RESULT: ALL BOOT & INTERACTIVE TESTS PASSED (100% SUCCESS)")
    print("==========================================================")
    sys.exit(0)
else:
    print("  RESULT: TEST VERIFICATION FAILED")
    print("==========================================================")
    sys.exit(1)
