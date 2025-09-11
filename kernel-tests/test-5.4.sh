#!/bin/bash
# Test RTL8192EU driver with kernel 5.4

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/build-common.sh"

# Test with kernel 5.4.285
test_kernel "5.4.285"