#!/bin/bash
# Test RTL8192EU driver with all supported kernels

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "========================================="
echo "Testing RTL8192EU driver with all kernels"
echo "========================================="
echo "Cross-compiling for: ARM64"
echo ""

# Clean all artifacts first
echo "Cleaning all build artifacts..."
rm -rf "$ROOT_DIR"/linux-*
rm -f "$ROOT_DIR"/8192eu-*.ko
rm -f "$ROOT_DIR"/*.o "$ROOT_DIR"/*.ko "$ROOT_DIR"/*.mod* "$ROOT_DIR"/.*.cmd
rm -rf "$ROOT_DIR"/.tmp_versions
make -C "$ROOT_DIR" clean > /dev/null 2>&1 || true
echo ""

# Test each kernel version
KERNELS="5.4 5.10 5.15 6.1"
RESULTS=""

for version in $KERNELS; do
    echo "----------------------------------------"
    echo "Testing kernel $version"
    echo "----------------------------------------"
    
    if "$SCRIPT_DIR/test-${version}.sh"; then
        RESULTS="${RESULTS}Kernel ${version}: ✓ SUCCESS\n"
    else
        RESULTS="${RESULTS}Kernel ${version}: ✗ FAILED\n"
    fi
    echo ""
done

echo "========================================="
echo "Test Results Summary:"
echo "========================================="
echo -e "$RESULTS"

# List successfully built modules
echo "Built modules:"
ls -lh "$ROOT_DIR"/8192eu-*.ko 2>/dev/null || echo "No modules successfully built"