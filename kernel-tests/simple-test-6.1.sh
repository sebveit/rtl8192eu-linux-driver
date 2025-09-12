#!/bin/bash
set -e

echo "Testing kernel 6.1 build (simple version)"

ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-
KSRC=./linux-6.1

# Clean any previous build artifacts
make clean

# Download and prepare kernel if needed
if [ ! -d "$KSRC" ]; then
    echo "Downloading Linux kernel 6.1..."
    TARBALL="linux-6.1.tar.xz"
    URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/$TARBALL"
    wget -O "$TARBALL" "$URL"
    tar -xf "$TARBALL"
    rm "$TARBALL"
    
    echo "Preparing kernel 6.1..."
    cd "$KSRC"
    # Use HOSTCFLAGS="-fcommon" to fix dtc build issue with newer gcc
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE HOSTCFLAGS="-fcommon" defconfig
    
    # Fix extract-cert.c to compile without OpenSSL
    cat > scripts/extract-cert.c << 'EOF'
#include <stdio.h>
int main(int argc, char **argv) { return 0; }
EOF
    
    # Continue with modules_prepare - this will now compile cleanly
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE HOSTCFLAGS="-fcommon" modules_prepare
    cd ..
fi

echo "Building driver..."
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE KSRC=$KSRC modules

if [ -f "8192eu.ko" ]; then
    echo "✓ Successfully built 8192eu.ko"
    ls -lh 8192eu.ko
else
    echo "✗ Build failed - module not found"
    exit 1
fi