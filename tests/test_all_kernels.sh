#!/bin/sh
# Test all supported kernel versions
# Each test builds in an isolated directory to avoid conflicts

set -e

echo "Testing RTL8192EU driver with multiple kernel versions"
echo "======================================================="
echo ""

SCRIPT_DIR=$(dirname "$0")
cd "$SCRIPT_DIR/.."

# Clean up any previous module builds (but keep tarballs and kernel sources)
echo "Cleaning previous builds..."
rm -f 8192eu-*.ko
rm -rf build-*

# Test each kernel version
RESULTS=""
FAILED=""
SUCCESS=""

for KVER in 5.4 5.10 5.15 6.1; do
    echo ""
    echo "========================================" 
    echo "Testing kernel $KVER..."
    echo "========================================"
    
    if ./tests/test_kernel_$KVER.sh; then
        if [ -f "8192eu-$KVER.ko" ]; then
            SIZE=$(ls -lh 8192eu-$KVER.ko | awk '{print $5}')
            RESULTS="$RESULTS\n✓ Kernel $KVER: SUCCESS (Module size: $SIZE)"
            SUCCESS="$SUCCESS $KVER"
        else
            RESULTS="$RESULTS\n✗ Kernel $KVER: FAILED (Module not found)"
            FAILED="$FAILED $KVER"
        fi
    else
        RESULTS="$RESULTS\n✗ Kernel $KVER: FAILED (Build error)"
        FAILED="$FAILED $KVER"
    fi
done

# Summary
echo ""
echo "========================================"
echo "Build Results Summary"
echo "========================================"
echo -e "$RESULTS"
echo ""

if [ -n "$SUCCESS" ]; then
    echo "Successfully built for:$SUCCESS"
fi

if [ -n "$FAILED" ]; then
    echo "Failed for:$FAILED"
    echo ""
    echo "For failed builds, check:"
    echo "  - Cross-compiler is installed (aarch64-linux-gnu-gcc)"
    echo "  - OpenSSL development headers are available"
    echo "  - Sufficient disk space"
fi

echo ""
echo "Built modules:"
ls -lh 8192eu-*.ko 2>/dev/null || echo "No modules built successfully"

# Clean up build directories (keep kernels and tarballs for reuse)
echo ""
echo "Cleaning up build directories..."
rm -rf build-*
echo "Done! (Kernel sources and tarballs preserved for future use)"