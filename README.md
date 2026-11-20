# Linux Driver for Realtek RTL8192EU

## Description
Linux driver for the Realtek RTL8192EU-VL-CG USB WiFi chip.

The code is based on Realtek's v5.6.3.1 driver as shipped in a
[ZIP archive](https://files.dlink.com.au/products/DWA-131/REV_E/Drivers/DWA-131_E1_Linux_v5.6.3.1/)
by D-Link. It has since been reduced to a Linux-only, RTL8192EU/USB-only
driver: code for other operating systems, other Realtek chips, other host
interfaces and never-enabled features has been removed, and a number of
security and correctness bugs have been fixed.

* Driver version: v5.6.3.1_34030.20190613_COEX20171113-0047
* Supported kernels: 5.4, 5.10, 5.15 and 6.1 (older kernels are rejected
  at build time)
* Module name: `8192eu`

## Building

```sh
make                                  # against the running kernel
make KSRC=/path/to/prepared/kernel    # against another kernel tree
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- KSRC=...   # cross build
```

Feature options are `?=` defaults at the top of the `Makefile` and can be
set on the command line, for example:

| Option | Default | Meaning |
|---|---|---|
| `CONFIG_RTW_DEBUG` | `y` | debug infrastructure, procfs debug files, log level parameter |
| `CONFIG_RTW_LOG_LEVEL` | `4` | default level for error/warning/print messages |
| `CONFIG_MP_INCLUDED` | `n` | manufacturing test mode (factory test ioctls) |
| `CONFIG_BT_COEXIST` | `y` | Bluetooth coexistence |
| `CONFIG_80211W` | `y` | management frame protection |
| `CONFIG_POWER_SAVING` | `n` | power saving |
| `CONFIG_WIFI_MONITOR` | `n` | monitor mode |
| `CONFIG_WOWLAN` | `n` | Wake on WLAN |

## Installing

```sh
sudo make install         # installs and runs depmod
sudo make uninstall
```

`DESTDIR` (or `INSTALL_MOD_PATH`) can be used for staged installs.

### DKMS

```sh
sudo make dkms_install    # copies the sources to /usr/src and registers them
sudo make dkms_remove
```

With DKMS the module is rebuilt automatically for new kernels.

## Runtime notes

* **Logging**: errors and warnings always go to the kernel log. The verbose
  `RTW_INFO`/`RTW_DBG` messages use dynamic debug and are off by default:
  `sudo modprobe 8192eu dyndbg=+p`, or
  `echo 'module 8192eu +p' | sudo tee /sys/kernel/debug/dynamic_debug/control`.
* **Files**: optional files (PHY parameter files, efuse map, MAC address
  file) are loaded through the kernel firmware loader and must be located
  below `/lib/firmware/` (default directory `/lib/firmware/rtl8192eu/`).
  The firmware itself is built into the driver.
* **Configuration interface**: the driver is configured through cfg80211
  (`iw`, NetworkManager, wpa_supplicant with `-Dnl80211`). Legacy
  wireless-extension tools are served by cfg80211's compatibility layer.

## Testing

See [TESTING.md](TESTING.md). `make test` builds the driver against all
supported kernels.

## License
This software is licensed under the GPLv2 only, as stated in the headers
of the source files.

## Copyright
The Realtek Corporation is the copyright holder of this software.
