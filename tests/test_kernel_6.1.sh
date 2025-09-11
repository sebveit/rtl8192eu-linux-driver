#!/bin/bash
# Test RTL8192EU driver build with Linux kernel 6.1

set -e

# Source common functions
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
. "$SCRIPT_DIR/common.sh"

# Test kernel 6.1
test_kernel "6.1"