#!/bin/bash
# Test RTL8192EU driver with kernel 6.1

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/build-common.sh"

# Test with kernel 6.1.119
test_kernel "6.1.119"