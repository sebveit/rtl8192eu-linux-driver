#!/bin/bash
# Build the driver against one supported kernel version.
#
# Usage: kernel-tests/build-test.sh <version>    e.g. 5.4, 5.10, 5.15, 6.1
#
# The kernel tree is expected in ./linux-<version>. If it is missing, it is
# extracted from kernel-downloads/linux-<version>.*.tar.xz (newest point
# release) or downloaded from kernel.org, then configured and prepared.
#
# Environment overrides: ARCH (default arm64), CROSS_COMPILE
# (default aarch64-linux-gnu-), JOBS (default: number of CPUs).
set -euo pipefail

VERSION=${1:?usage: $0 <kernel version>}
ARCH=${ARCH:-arm64}
CROSS_COMPILE=${CROSS_COMPILE-aarch64-linux-gnu-}
JOBS=${JOBS:-$(nproc)}

cd "$(dirname "$0")/.."
KSRC=./linux-$VERSION
MAKE_ARGS=(ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE")

echo "Testing kernel $VERSION build"

prepare_kernel() {
	local tarball
	mkdir -p kernel-downloads
	tarball=$(ls kernel-downloads/linux-"$VERSION".*.tar.xz 2>/dev/null | sort -V | tail -n1 || true)
	if [ -z "$tarball" ]; then
		tarball=kernel-downloads/linux-$VERSION.tar.xz
		echo "Downloading Linux $VERSION..."
		wget -O "$tarball" \
			"https://cdn.kernel.org/pub/linux/kernel/v${VERSION%%.*}.x/linux-$VERSION.tar.xz"
	fi

	echo "Extracting $tarball..."
	mkdir "$KSRC"
	tar -xf "$tarball" -C "$KSRC" --strip-components=1

	echo "Preparing kernel $VERSION..."
	# -fcommon: older dtc sources fail to link with GCC >= 10
	make -C "$KSRC" "${MAKE_ARGS[@]}" HOSTCFLAGS="-fcommon" defconfig
	# Distribution kernels enable wireless extensions; defconfig does not.
	# USB_ZD1201 selects WIRELESS_EXT and WEXT_PRIV.
	"$KSRC"/scripts/config --file "$KSRC/.config" -e CFG80211_WEXT \
		-e WLAN_VENDOR_ZYDAS -m USB_ZD1201
	make -C "$KSRC" "${MAKE_ARGS[@]}" HOSTCFLAGS="-fcommon" olddefconfig
	# extract-cert needs OpenSSL headers that may be missing; it is not
	# needed for building external modules.
	printf '#include <stdio.h>\nint main(void) { return 0; }\n' > "$KSRC/scripts/extract-cert.c"
	make -C "$KSRC" "${MAKE_ARGS[@]}" HOSTCFLAGS="-fcommon" -j"$JOBS" modules_prepare
}

[ -d "$KSRC" ] || prepare_kernel

make "${MAKE_ARGS[@]}" KSRC="$KSRC" clean >/dev/null
make "${MAKE_ARGS[@]}" KSRC="$KSRC" -j"$JOBS" modules

if [ -f 8192eu.ko ]; then
	echo "✓ Successfully built 8192eu.ko for $VERSION"
	ls -lh 8192eu.ko
else
	echo "✗ Build failed - module not found"
	exit 1
fi
