#!/bin/bash

echo "================================================"
echo "Testing RTL8192EU driver with multiple kernels"
echo "================================================"
echo ""

# Track results
PASSED=0
FAILED=0
RESULTS=""

# Test each kernel version
for VERSION in 5.4 5.10 5.15 6.1; do
    echo "----------------------------------------"
    echo "Testing kernel $VERSION..."
    echo "----------------------------------------"
    
    if ./kernel-tests/simple-test-${VERSION}.sh; then
        echo "✓ Kernel $VERSION: PASSED"
        PASSED=$((PASSED + 1))
        RESULTS="${RESULTS}✓ Kernel ${VERSION}: PASSED\n"
    else
        echo "✗ Kernel $VERSION: FAILED"
        FAILED=$((FAILED + 1))
        RESULTS="${RESULTS}✗ Kernel ${VERSION}: FAILED\n"
    fi
    echo ""
done

echo "================================================"
echo "Test Summary"
echo "================================================"
echo -e "$RESULTS"
echo "Total: $((PASSED + FAILED)) tests, $PASSED passed, $FAILED failed"

if [ $FAILED -eq 0 ]; then
    echo "✓ All tests passed!"
    exit 0
else
    echo "✗ Some tests failed"
    exit 1
fi