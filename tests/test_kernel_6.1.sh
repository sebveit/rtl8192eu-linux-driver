#!/bin/sh
set -e
ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-
KSRC=./linux-6.1
if [ ! -d "$KSRC" ]; then
    TARBALL="linux-6.1.tar.xz"
    URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/$TARBALL"
    wget -O "$TARBALL" "$URL"
    tar -xf "$TARBALL"
    rm "$TARBALL"
    (cd "$KSRC" && make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE HOSTCFLAGS="-fcommon" defconfig && \
     make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE HOSTCFLAGS="-fcommon" modules_prepare)
fi
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE KSRC=$KSRC modules
