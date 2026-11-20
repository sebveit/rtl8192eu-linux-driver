# Yocto Recipe Configuration for RTL8192EU Driver

## Essential Makefile Variables for Optimal Performance

### Core Performance Settings
```makefile
CONFIG_POWER_SAVING = n          # Disable power saving for performance
CONFIG_IPS = n                   # Disable Inactive Power Save
CONFIG_LPS = n                   # Disable Leisure Power Save
CONFIG_USB_AUTOSUSPEND = n       # Disable USB auto-suspend
```

### Multiple BSSID & Virtual Interface Support
```makefile
CONFIG_IFACE_NUMBER = 3          # Enable 3 interfaces (already set)
CONFIG_CONCURRENT_MODE = y       # Enable concurrent mode (already set)
CONFIG_MI_WITH_MBSSID_CAM = y    # Multiple interface MBSSID CAM (auto-enabled)
CONFIG_MBSSID_CAM = y            # MBSSID CAM support (auto-enabled)
CONFIG_AP_MODE = y               # AP mode support
CONFIG_P2P = y                   # P2P support for virtual interfaces
```

### Fast Transition & Performance
```makefile
CONFIG_RTW_80211R = y            # 802.11r Fast Transition
CONFIG_RTW_WNM = y               # Wireless Network Management
CONFIG_WAPI_SUPPORT = n          # Disable WAPI (unless needed in China)
CONFIG_EFUSE_CONFIG_FILE = n     # Use hardware EFUSE (faster)
CONFIG_RTW_DEBUG = n             # Disable debug for production
CONFIG_PROC_DEBUG = n            # Disable proc debug
```

### Network Optimization
```makefile
CONFIG_TCP_CSUM_OFFLOAD_TX = y   # Enable TX checksum offload
CONFIG_SKB_COPY = n              # Avoid SKB copying overhead
CONFIG_USE_USB_BUFFER_ALLOC_RX = y  # Efficient USB RX allocation
CONFIG_PREALLOC_RECV_SKB = y     # Pre-allocate receive SKBs
CONFIG_FIX_NR_BULKIN_BUFFER = y  # Fixed bulk-in buffer count
```

### USB Performance
```makefile
CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC = y  # Pre-allocate vendor buffers
CONFIG_USB_VENDOR_REQ_MUTEX = y             # USB vendor request mutex
CONFIG_USB_TX_AGGREGATION = y               # Enable TX aggregation
CONFIG_USB_RX_AGGREGATION = y               # Enable RX aggregation
```

### Security & Features
```makefile
CONFIG_80211W = y                # Management Frame Protection
CONFIG_IEEE80211W = y            # 802.11w PMF
CONFIG_RTW_MFP = y              # Management Frame Protection
CONFIG_WOWLAN = n               # Wake on WLAN (disable for AP use)
```

## Yocto Recipe Template

```bitbake
SUMMARY = "Realtek RTL8192EU WiFi Driver"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://Makefile;md5=..."

SRC_URI = "git://your-repo/rtl8192eu.git;branch=main;protocol=https"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

inherit module

MAKE_TARGETS = "modules"
MODULES_INSTALL_TARGET = "install"

EXTRA_OEMAKE = " \
    KSRC=${STAGING_KERNEL_DIR} \
    CROSS_COMPILE=${TARGET_PREFIX} \
    ARCH=${ARCH} \
    CONFIG_POWER_SAVING=n \
    CONFIG_USB_AUTOSUSPEND=n \
    CONFIG_RTW_DEBUG=n \
"

KERNEL_MODULE_AUTOLOAD += "8192eu"

RPROVIDES_${PN} += "kernel-module-8192eu"
```

## Key Considerations

1. **Power Management**: Disable all power saving for AP/infrastructure use
2. **Buffer Management**: Enable pre-allocation and efficient USB buffering
3. **Interface Limits**: 3 interfaces max with current MBSSID CAM (8 entries)
4. **Platform**: Set appropriate platform flags (ARM_RPI for Raspberry Pi)
5. **Debug**: Disable all debug options for production builds
6. **Aggregation**: Enable USB TX/RX aggregation for better throughput
7. **Fast Transition**: Enable 802.11r/WNM for enterprise environments

## Runtime Configuration

For optimal virtual interface setup:
```bash
# Create virtual AP interface
iw phy phy0 interface add wlan1 type __ap

# Create virtual station interface  
iw phy phy0 interface add wlan2 type station

# Configure different channels (if hardware supports)
iw dev wlan0 set channel 6
iw dev wlan1 set channel 11
```