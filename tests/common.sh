#!/bin/bash
# Common functions for kernel testing scripts

set -e

# Base directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
DOWNLOADS_DIR="$ROOT_DIR/downloads"
STATES_DIR="$ROOT_DIR/states"
BUILD_DEPS_DIR="$ROOT_DIR/build_dependencies"
OPENSSL_DIR="$BUILD_DEPS_DIR/openssl-1.1"

# Create necessary directories
mkdir -p "$DOWNLOADS_DIR" "$STATES_DIR" "$BUILD_DEPS_DIR"

# State management functions
check_state() {
    local kernel_version="$1"
    local state_name="$2"
    local state_file="$STATES_DIR/kernel-${kernel_version}-${state_name}.state"
    [ -f "$state_file" ]
}

set_state() {
    local kernel_version="$1"
    local state_name="$2"
    local state_file="$STATES_DIR/kernel-${kernel_version}-${state_name}.state"
    touch "$state_file"
}

clear_state() {
    local kernel_version="$1"
    local state_name="$2"
    local state_file="$STATES_DIR/kernel-${kernel_version}-${state_name}.state"
    rm -f "$state_file"
}

clear_all_states() {
    local kernel_version="$1"
    rm -f "$STATES_DIR/kernel-${kernel_version}-"*.state
}

# Download and build OpenSSL 1.1.x if needed
setup_openssl() {
    local openssl_version="1.1.1w"
    local tarball="$DOWNLOADS_DIR/openssl-${openssl_version}.tar.gz"
    local build_dir="/tmp/openssl-build-$$"
    
    if check_state "openssl" "built" && [ -d "$OPENSSL_DIR" ]; then
        echo "OpenSSL 1.1.x already built"
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
    
    # Extract to temp directory for building
    echo "Extracting OpenSSL ${openssl_version}..."
    mkdir -p "$build_dir"
    tar -xzf "$tarball" -C "$build_dir"
    
    # Build
    echo "Building OpenSSL ${openssl_version}..."
    cd "$build_dir/openssl-${openssl_version}"
    ./config --prefix="$OPENSSL_DIR" --openssldir="$OPENSSL_DIR" no-shared
    make -j$(nproc)
    make install_sw
    
    # Clean up build directory
    cd "$ROOT_DIR"
    rm -rf "$build_dir"
    
    set_state "openssl" "built"
    echo "OpenSSL 1.1.x built successfully"
    return 0
}

# Download kernel if needed
download_kernel() {
    local version="$1"
    local major_version="${version%%.*}"
    local tarball="$DOWNLOADS_DIR/linux-${version}.tar.xz"
    
    if [ -f "$tarball" ]; then
        echo "Kernel ${version} tarball already downloaded"
        return 0
    fi
    
    echo "Downloading Linux kernel ${version}..."
    mkdir -p "$DOWNLOADS_DIR"
    
    local url="https://cdn.kernel.org/pub/linux/kernel/v${major_version}.x/linux-${version}.tar.xz"
    
    if ! wget -q --show-progress -O "$tarball.tmp" "$url"; then
        echo "Failed to download kernel ${version}"
        rm -f "$tarball.tmp"
        return 1
    fi
    
    mv "$tarball.tmp" "$tarball"
    echo "Downloaded kernel ${version}"
    set_state "$version" "downloaded"
    return 0
}

# Extract kernel if needed
extract_kernel() {
    local version="$1"
    local kernel_dir="$ROOT_DIR/linux-${version}"
    local tarball="$DOWNLOADS_DIR/linux-${version}.tar.xz"
    
    if check_state "$version" "extracted" && [ -d "$kernel_dir" ]; then
        echo "Kernel ${version} already extracted"
        return 0
    fi
    
    # Download if needed
    if ! [ -f "$tarball" ]; then
        download_kernel "$version" || return 1
    fi
    
    echo "Extracting kernel ${version}..."
    rm -rf "$kernel_dir"
    tar -xf "$tarball" -C "$ROOT_DIR"
    
    if [ -d "$kernel_dir" ]; then
        set_state "$version" "extracted"
        echo "Extracted kernel ${version}"
        return 0
    else
        echo "Failed to extract kernel ${version}"
        return 1
    fi
}

# Prepare kernel headers
prepare_kernel() {
    local version="$1"
    local kernel_dir="$ROOT_DIR/linux-${version}"
    
    if check_state "$version" "prepared"; then
        echo "Kernel ${version} already prepared"
        return 0
    fi
    
    # Ensure OpenSSL 1.1.x is available
    setup_openssl || {
        echo "Failed to setup OpenSSL 1.1.x"
        return 1
    }
    
    # Extract if needed
    if ! check_state "$version" "extracted" || ! [ -d "$kernel_dir" ]; then
        extract_kernel "$version" || return 1
    fi
    
    echo "Preparing kernel ${version} headers..."
    cd "$kernel_dir"
    
    # Configure for x86_64 with more complete config for testing
    make defconfig
    # Enable additional needed configs
    scripts/config --enable CONFIG_USB_SUPPORT
    scripts/config --enable CONFIG_USB
    scripts/config --enable CONFIG_USB_COMMON  
    scripts/config --enable CONFIG_USB_ARCH_HAS_HCD
    scripts/config --enable CONFIG_CFG80211
    scripts/config --enable CONFIG_WIRELESS
    scripts/config --enable CONFIG_WLAN
    scripts/config --enable CONFIG_NET
    scripts/config --enable CONFIG_NETDEVICES
    scripts/config --enable CONFIG_ETHERNET
    scripts/config --enable CONFIG_PROC_FS
    scripts/config --enable CONFIG_SYSFS
    scripts/config --enable CONFIG_CRYPTO
    scripts/config --enable CONFIG_CRYPTO_AES
    scripts/config --enable CONFIG_CRYPTO_CCM
    scripts/config --enable CONFIG_CRYPTO_GCM
    scripts/config --enable CONFIG_CRYPTO_CMAC
    scripts/config --enable CONFIG_CRC32
    # Update config after changes
    make olddefconfig
    
    # Set up environment to use our local OpenSSL 1.1.x
    export HOSTCFLAGS="-fcommon -I${OPENSSL_DIR}/include"
    export HOSTLDFLAGS="-L${OPENSSL_DIR}/lib"
    
    # Build scripts
    make HOSTCFLAGS="$HOSTCFLAGS" HOSTLDFLAGS="$HOSTLDFLAGS" scripts || true
    
    # Build modpost tool
    make HOSTCFLAGS="$HOSTCFLAGS" HOSTLDFLAGS="$HOSTLDFLAGS" scripts/mod/ || true
    
    # Prepare kernel - this generates asm-offsets.h
    make HOSTCFLAGS="$HOSTCFLAGS" HOSTLDFLAGS="$HOSTLDFLAGS" prepare || true
    
    # Additional preparation for modules
    make HOSTCFLAGS="$HOSTCFLAGS" HOSTLDFLAGS="$HOSTLDFLAGS" modules_prepare || true
    
    # Generate Module.symvers if missing
    if [ ! -f "Module.symvers" ]; then
        touch Module.symvers
    fi
    
    cd "$ROOT_DIR"
    set_state "$version" "prepared"
    echo "Prepared kernel ${version}"
    return 0
}

# Build driver for specific kernel
build_driver() {
    local version="$1"
    local kernel_dir="$ROOT_DIR/linux-${version}"
    local build_dir="$ROOT_DIR/build-${version}"
    local module_name="8192eu-${version}.ko"
    
    if check_state "$version" "built" && [ -f "$ROOT_DIR/$module_name" ]; then
        echo "Driver already built for kernel ${version}"
        return 0
    fi
    
    # Prepare kernel if needed
    if ! check_state "$version" "prepared"; then
        prepare_kernel "$version" || return 1
    fi
    
    echo "Building driver for kernel ${version}..."
    
    # Clean and prepare build directory
    rm -rf "$build_dir"
    mkdir -p "$build_dir"
    
    # Copy source files to build directory
    cp -r "$ROOT_DIR"/{core,hal,include,os_dep,platform} "$build_dir/"
    cp "$ROOT_DIR/Makefile" "$build_dir/"
    
    # Build the module
    cd "$build_dir"
    
    if make KSRC="$kernel_dir" -j$(nproc) modules; then
        
        # Copy module to root with version suffix
        if [ -f "8192eu.ko" ]; then
            cp "8192eu.ko" "$ROOT_DIR/$module_name"
            echo "Successfully built $module_name"
            cd "$ROOT_DIR"
            set_state "$version" "built"
            
            # Show module info
            ls -lh "$ROOT_DIR/$module_name"
            file "$ROOT_DIR/$module_name"
            return 0
        else
            echo "Build completed but module not found"
            cd "$ROOT_DIR"
            return 1
        fi
    else
        echo "Build failed for kernel ${version}"
        cd "$ROOT_DIR"
        return 1
    fi
}

# Clean function for specific kernel version
clean_kernel() {
    local version="$1"
    
    echo "Cleaning kernel ${version} artifacts..."
    
    # Remove build directory
    rm -rf "$ROOT_DIR/build-${version}"
    
    # Remove kernel source if requested
    if [ "${2:-}" = "all" ]; then
        rm -rf "$ROOT_DIR/linux-${version}"
        clear_state "$version" "extracted"
        clear_state "$version" "prepared"
    fi
    
    # Clear built state
    clear_state "$version" "built"
    
    echo "Cleaned kernel ${version}"
}

# Test function
test_kernel() {
    local version="$1"
    
    echo ""
    echo "========================================"
    echo "Testing RTL8192EU with kernel ${version}"
    echo "========================================"
    
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