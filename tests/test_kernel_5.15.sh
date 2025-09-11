#!/bin/bash
# Test RTL8192EU driver build with Linux kernel 5.15

set -e

# Source common functions
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
. "$SCRIPT_DIR/common.sh"

# Test kernel 5.15
test_kernel "5.15"