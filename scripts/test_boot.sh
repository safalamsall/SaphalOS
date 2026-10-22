#!/usr/bin/env bash
# ==============================================================================
# SAPHAL OS - Automated Headless QEMU Boot & Interactive Verification Harness
# ==============================================================================

set -eo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
python3 "$PROJECT_DIR/scripts/test_boot.py"
