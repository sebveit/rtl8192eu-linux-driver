#!/bin/bash
# Common build functions for kernel testing - stateless version

set -e

# Base directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
DOWNLOADS_DIR="$ROOT_DIR/kernel-downloads"
OPENSSL_DIR="$ROOT_DIR/kernel-downloads/openssl-1.1"

# Cross-compilation settings for ARM64
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

# Create downloads directory
mkdir -p "$DOWNLOADS_DIR"

# Download and build OpenSSL 1.1 if needed (for older kernels)
setup_openssl() {
    local openssl_version="1.1.1w"
    local tarball="$DOWNLOADS_DIR/openssl-${openssl_version}.tar.gz"
    
    if [ -d "$OPENSSL_DIR" ] && [ -f "$OPENSSL_DIR/include/openssl/opensslconf.h" ]; then
        echo "OpenSSL 1.1 already built"
        return 0
    fi
    
    # Download if needed
    if [ ! -f "$tarball" ]; then
        echo "Downloading OpenSSL ${openssl_version}..."
        wget -q --show-progress -O "$tarball.tmp" \
            "https://www.openssl.org/source/openssl-${openssl_version}.tar.gz" || {
            echo "Failed to download OpenSSL"
            rm -f "$tarball.tmp"
            return 1
        }
        mv "$tarball.tmp" "$tarball"
    fi
    
    # Extract and build
    echo "Building OpenSSL ${openssl_version}..."
    local build_dir="/tmp/openssl-build-$$"
    mkdir -p "$build_dir"
    tar -xzf "$tarball" -C "$build_dir"
    
    cd "$build_dir/openssl-${openssl_version}"
    ./config --prefix="$OPENSSL_DIR" --openssldir="$OPENSSL_DIR" no-shared
    make -j$(nproc)
    make install_sw
    
    cd "$ROOT_DIR"
    rm -rf "$build_dir"
    
    echo "OpenSSL 1.1 built successfully"
    return 0
}

# Download kernel if archive doesn't exist
download_kernel() {
    local version="$1"
    local major_version="${version%%.*}"
    local tarball="$DOWNLOADS_DIR/linux-${version}.tar.xz"
    
    if [ -f "$tarball" ]; then
        echo "Kernel ${version} archive found in downloads"
        return 0
    fi
    
    echo "Downloading Linux kernel ${version}..."
    local url="https://cdn.kernel.org/pub/linux/kernel/v${major_version}.x/linux-${version}.tar.xz"
    
    if ! wget -q --show-progress -O "$tarball.tmp" "$url"; then
        echo "Failed to download kernel ${version}"
        rm -f "$tarball.tmp"
        return 1
    fi
    
    mv "$tarball.tmp" "$tarball"
    echo "Downloaded kernel ${version}"
    return 0
}

# Extract and prepare kernel (always fresh)
prepare_kernel() {
    local version="$1"
    local kernel_dir="$ROOT_DIR/linux-${version}"
    local tarball="$DOWNLOADS_DIR/linux-${version}.tar.xz"
    local major_version="${version%%.*}"
    local minor_version="${version#*.}"
    minor_version="${minor_version%%.*}"
    
    # Download if needed
    download_kernel "$version" || return 1
    
    # Setup OpenSSL 1.1 for kernels 5.4 and 5.10
    if [ "$major_version" -eq 5 ] && [ "$minor_version" -le 10 ]; then
        echo "Kernel ${version} requires OpenSSL 1.1"
        setup_openssl || return 1
        export HOSTCFLAGS="-I${OPENSSL_DIR}/include"
        export HOSTLDFLAGS="-L${OPENSSL_DIR}/lib"
    fi
    
    # Always remove and extract fresh
    echo "Extracting fresh kernel ${version}..."
    rm -rf "$kernel_dir"
    tar -xf "$tarball" -C "$ROOT_DIR"
    
    if [ ! -d "$kernel_dir" ]; then
        echo "Failed to extract kernel ${version}"
        return 1
    fi
    
    cd "$kernel_dir"
    
    # Configure for ARM64 cross-compilation
    echo "Configuring kernel ${version} for ARM64..."
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE defconfig
    
    # Enable required configs for the driver
    scripts/config --enable CONFIG_MODULES
    scripts/config --enable CONFIG_MODULE_UNLOAD
    scripts/config --enable CONFIG_USB_SUPPORT
    scripts/config --enable CONFIG_USB
    scripts/config --enable CONFIG_CFG80211
    scripts/config --enable CONFIG_WIRELESS
    scripts/config --enable CONFIG_WLAN
    
    # Update config
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE olddefconfig
    
    # Prepare for external module building
    echo "Preparing kernel ${version} for module building..."
    
    # Build what we need for module compilation
    # Create dummy extract-cert to avoid OpenSSL issues
    echo '#!/bin/sh' > scripts/extract-cert
    echo 'exit 0' >> scripts/extract-cert
    chmod +x scripts/extract-cert
    
    # Build tools and prepare kernel
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE scripts/basic
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE scripts/mod
    
    # Prepare with workaround for missing headers
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE prepare || true
    
    # Build modpost tool if not built
    if [ ! -f "scripts/mod/modpost" ]; then
        (cd scripts/mod && make modpost) || true
    fi
    
    # Create minimal asm-offsets.h if missing
    if [ ! -f "include/generated/asm-offsets.h" ]; then
        mkdir -p include/generated
        touch include/generated/asm-offsets.h
    fi
    
    # Create empty Module.symvers if missing
    touch Module.symvers
    
    cd "$ROOT_DIR"
    echo "Kernel ${version} prepared"
    return 0
}

# Build driver for kernel
build_driver() {
    local version="$1"
    local kernel_dir="$ROOT_DIR/linux-${version}"
    local module_name="8192eu-${version}.ko"
    
    # Always prepare fresh kernel
    prepare_kernel "$version" || return 1
    
    echo "Building driver for kernel ${version}..."
    cd "$ROOT_DIR"
    
    # Clean previous build
    make clean > /dev/null 2>&1 || true
    rm -f "$module_name"
    
    # Build the module with cross-compilation
    if make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE KSRC="$kernel_dir" -j$(nproc) modules; then
        if [ -f "8192eu.ko" ]; then
            cp "8192eu.ko" "$module_name"
            echo "Successfully built $module_name"
            ls -lh "$module_name"
            file "$module_name"
            return 0
        else
            echo "Build completed but module not found"
            return 1
        fi
    else
        echo "Build failed for kernel ${version}"
        return 1
    fi
}

# Test function
test_kernel() {
    local version="$1"
    
    echo ""
    echo "========================================"
    echo "Testing RTL8192EU with kernel ${version}"
    echo "========================================"
    echo "Cross-compiling for: ARM64"
    echo ""
    
    if build_driver "$version"; then
        echo ""
        echo "✓ Successfully built driver for kernel ${version}"
        return 0
    else
        echo ""
        echo "✗ Failed to build driver for kernel ${version}"
        return 1
    fi
}

# Clean function
clean_kernel() {
    local version="$1"
    echo "Cleaning kernel ${version} artifacts..."
    rm -rf "$ROOT_DIR/linux-${version}"
    rm -f "$ROOT_DIR/8192eu-${version}.ko"
    echo "Cleaned kernel ${version}"
}