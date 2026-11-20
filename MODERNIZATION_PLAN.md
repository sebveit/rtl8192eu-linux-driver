# Modernization Status

This document summarizes how the driver differs from the original Realtek
v5.6.3.1 code drop and what is still open.

## Scope

* Linux only, RTL8192EU over USB only.
* Kernels 5.4, 5.10, 5.15 and 6.1; older kernels fail with `#error`.

## Done

### Code reduction
* Removed Windows/WinCE/XP, AP-platform and IoT code paths (phydm
  `ODM_WIN`/`ODM_AP`/`ODM_IOT`, `PLATFORM_*`), and the corresponding files.
* Removed code for all other Realtek chips and for PCIe/SDIO/GSPI.
* Removed never-enabled features: mesh, MCC, WAPI, Intel WiDi, Android
  vendor HAL, repeater SON, IPCAM, PNO.
* Removed the driver's own standard wireless-extension handlers; cfg80211's
  WEXT compatibility layer serves legacy tools.
* Removed unused functions, variables, labels and dead preprocessor blocks.
* Replaced Windows-style types and macros (`IN`, `OUT`, `VOID`, `PVOID`,
  `UCHAR`, `NDIS_*`, ...).

### Build system
* Lean Kbuild Makefile with `ccflags-y`, command-line overridable options,
  endianness from the kernel config, Kbuild `clean`, `DESTDIR` installs.
* DKMS support (`dkms.conf`, `make dkms_install`).
* Build tests for all supported kernels plus x86_64, big-endian arm64 and
  clang, with a check for unresolved symbols; GitHub Actions workflow.
* Manufacturing (MP) test mode is off by default.

### Security and correctness fixes
* Bounds checks in the wpa_supplicant/hostapd ioctls (key index, TKIP key
  length, WPA IE length, minimum request size, station data size).
* Bounded argument parsing and result formatting in the private ioctl
  dispatcher; fixed-size buffer for Android private commands.
* procfs write handlers: bounded, NUL-terminated user input.
* MP ioctl handlers no longer use 2 KiB stack buffers.
* Fixed NULL dereferences, out-of-bounds reads (u8 read as enum, AES-CMAC
  vector), uninitialised variables, wrong logic operators, fall-through bugs
  and a broken TDLS build.

### Kernel integration
* Logging through `pr_*`/dynamic debug with the module name as prefix.
* Runtime files are loaded through the firmware loader instead of opening
  files from kernel context.

## Open

* Hardware testing (see [TESTING.md](TESTING.md)).
* Building with W=1 still reports ~80 set-but-unused variables and ~25
  missing prototypes.
* Several optional features (power saving, WoWLAN, TDLS, BT coexistence
  disabled) build but emit unused-variable warnings.
* Logging still goes through driver macros rather than `netdev_*`/`dev_*`.
