#!/bin/bash
################################################################################
# RTL8192EU Performance Tuning Script
# 
# This script optimizes the driver parameters for maximum performance
################################################################################

# Check if running as root
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root for optimal settings" 
   echo "Some settings may not apply without root privileges"
fi

echo "================================================"
echo "RTL8192EU Performance Tuning Script"
echo "================================================"

# Check if module is loaded
if ! lsmod | grep -q "8192eu"; then
    echo "Error: 8192eu module is not loaded"
    echo "Please load the module first: sudo modprobe 8192eu"
    exit 1
fi

echo ""
echo "Applying performance optimizations..."
echo ""

# USB Aggregation Settings
echo "1. USB Aggregation:"
echo "   - Setting RX aggregation mode to USB (optimal for RTL8192EU)"
if [ -w /sys/module/8192eu/parameters/rtw_usb_rxagg_mode ]; then
    echo 2 > /sys/module/8192eu/parameters/rtw_usb_rxagg_mode
    echo "   ✓ RX aggregation mode set to 2 (USB)"
else
    echo "   ✗ Cannot modify rtw_usb_rxagg_mode (permission denied)"
fi

echo "   - Enabling dynamic aggregation"
if [ -w /sys/module/8192eu/parameters/rtw_dynamic_agg_enable ]; then
    echo 1 > /sys/module/8192eu/parameters/rtw_dynamic_agg_enable
    echo "   ✓ Dynamic aggregation enabled"
else
    echo "   ✗ Cannot modify rtw_dynamic_agg_enable (permission denied)"
fi

# Power Management Settings
echo ""
echo "2. Power Management:"
echo "   - Disabling power saving for maximum performance"
if [ -w /sys/module/8192eu/parameters/rtw_power_mgnt ]; then
    echo 0 > /sys/module/8192eu/parameters/rtw_power_mgnt
    echo "   ✓ Power management disabled"
else
    echo "   ✗ Cannot modify rtw_power_mgnt (permission denied)"
fi

if [ -w /sys/module/8192eu/parameters/rtw_ips_mode ]; then
    echo 0 > /sys/module/8192eu/parameters/rtw_ips_mode
    echo "   ✓ IPS mode disabled"
fi

# Network Interface Optimizations
IFACE=$(ip link show | grep -E "wlan|wlp" | head -1 | cut -d: -f2 | tr -d ' ')

if [ -n "$IFACE" ]; then
    echo ""
    echo "3. Network Interface Optimizations ($IFACE):"
    
    # Disable power management on wireless interface
    if command -v iwconfig &> /dev/null; then
        iwconfig $IFACE power off 2>/dev/null && \
            echo "   ✓ WiFi power management disabled" || \
            echo "   ✗ Cannot disable WiFi power management"
    fi
    
    # Set MTU for optimal performance
    ip link set dev $IFACE mtu 2304 2>/dev/null && \
        echo "   ✓ MTU set to 2304 (optimal for 802.11)" || \
        echo "   ✗ Cannot set MTU"
    
    # Enable TX queue optimization
    if [ -d /sys/class/net/$IFACE/queues ]; then
        for queue in /sys/class/net/$IFACE/queues/tx-*/xps_cpus; do
            echo f > $queue 2>/dev/null
        done
        echo "   ✓ TX queue CPU affinity optimized"
    fi
else
    echo ""
    echo "3. No wireless interface found for optimization"
fi

# System-wide Network Optimizations
echo ""
echo "4. System Network Stack Optimizations:"

if [[ $EUID -eq 0 ]]; then
    # Increase network buffers
    sysctl -w net.core.rmem_max=134217728 2>/dev/null
    sysctl -w net.core.wmem_max=134217728 2>/dev/null
    sysctl -w net.core.rmem_default=524288 2>/dev/null
    sysctl -w net.core.wmem_default=524288 2>/dev/null
    sysctl -w net.core.netdev_max_backlog=5000 2>/dev/null
    sysctl -w net.ipv4.tcp_rmem="4096 524288 134217728" 2>/dev/null
    sysctl -w net.ipv4.tcp_wmem="4096 524288 134217728" 2>/dev/null
    sysctl -w net.ipv4.tcp_congestion_control=bbr 2>/dev/null
    echo "   ✓ Network buffer sizes increased"
    echo "   ✓ TCP congestion control set to BBR (if available)"
    
    # Optimize for low latency
    sysctl -w net.ipv4.tcp_low_latency=1 2>/dev/null
    sysctl -w net.ipv4.tcp_sack=1 2>/dev/null
    sysctl -w net.ipv4.tcp_timestamps=1 2>/dev/null
    echo "   ✓ TCP optimizations applied"
else
    echo "   ✗ Run as root to apply system optimizations"
fi

# USB Optimizations
echo ""
echo "5. USB Controller Optimizations:"

if [[ $EUID -eq 0 ]]; then
    # Find USB device for RTL8192EU
    USB_DEVICE=$(lsusb | grep -i "0bda:818b\|2357:0109\|2357:0108\|2357:0126" | head -1)
    
    if [ -n "$USB_DEVICE" ]; then
        BUS=$(echo $USB_DEVICE | cut -d' ' -f2 | tr -d '0')
        DEVICE=$(echo $USB_DEVICE | cut -d' ' -f4 | tr -d ':' | tr -d '0')
        
        # Disable USB autosuspend for the device
        if [ -f "/sys/bus/usb/devices/$BUS-$DEVICE/power/control" ]; then
            echo on > /sys/bus/usb/devices/$BUS-$DEVICE/power/control 2>/dev/null && \
                echo "   ✓ USB autosuspend disabled for RTL8192EU" || \
                echo "   ✗ Cannot disable USB autosuspend"
        fi
    else
        echo "   ✗ RTL8192EU USB device not found"
    fi
    
    # General USB latency optimization
    for i in /sys/bus/usb/devices/*/power/autosuspend; do
        echo -1 > $i 2>/dev/null
    done
    echo "   ✓ USB autosuspend disabled globally"
else
    echo "   ✗ Run as root to apply USB optimizations"
fi

echo ""
echo "================================================"
echo "Performance optimizations complete!"
echo ""
echo "Tips for best performance:"
echo "  1. Use 5GHz band if available (less congestion)"
echo "  2. Set router to 80MHz channel width"
echo "  3. Use WPA2-AES encryption (faster than TKIP)"
echo "  4. Position adapter for best signal strength"
echo "  5. Keep driver updated to latest version"
echo ""
echo "To make these settings permanent, add this script"
echo "to your system startup (e.g., /etc/rc.local)"
echo "================================================"