#!/bin/bash
# Clean all kernel build artifacts

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Cleaning all kernel build artifacts..."

# Remove kernel source directories
rm -rf "$ROOT_DIR"/linux-*

# Remove built modules
rm -f "$ROOT_DIR"/8192eu-*.ko

# Clean driver build artifacts
rm -f "$ROOT_DIR"/*.o "$ROOT_DIR"/*.ko "$ROOT_DIR"/*.mod* "$ROOT_DIR"/.*.cmd
rm -rf "$ROOT_DIR"/.tmp_versions
rm -rf "$ROOT_DIR"/Module.symvers "$ROOT_DIR"/modules.order

# Run make clean
make -C "$ROOT_DIR" clean > /dev/null 2>&1 || true

echo "All artifacts cleaned (downloads preserved in kernel-downloads/)"