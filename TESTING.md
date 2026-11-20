# Testing

## Build tests

`kernel-tests/build-test.sh <version> [variant]` builds the driver against a
prepared kernel tree and fails on compiler errors or on symbols the kernel
does not export.

| Variant | Target | Kernel tree |
|---|---|---|
| `arm64` (default) | arm64, GCC cross compiler | `./linux-<version>` |
| `arm64be` | big-endian arm64 | `./linux-<version>-arm64be` |
| `x86_64` | x86_64, host GCC | `./linux-<version>-x86_64` |
| `x86_64-clang` | x86_64, `CC=clang` | `./linux-<version>-x86_64-clang` |

If the kernel tree is missing it is extracted from
`kernel-downloads/linux-<version>.*.tar.xz` (or downloaded from kernel.org),
configured with `defconfig` plus wireless extensions (`CFG80211_WEXT`,
`WIRELESS_EXT`, `WEXT_PRIV`, as on distribution kernels) and prepared with
`modules_prepare`.

Driver options can be passed via `DRIVER_ARGS`:

```sh
./kernel-tests/build-test.sh 6.1
./kernel-tests/build-test.sh 6.1 x86_64-clang
DRIVER_ARGS="CONFIG_MP_INCLUDED=y CONFIG_WOWLAN=y" ./kernel-tests/build-test.sh 6.1
```

`make test` (or `kernel-tests/simple-test-all.sh`) builds against 5.4, 5.10,
5.15 and 6.1 for arm64. The scripts run `make clean` in the source tree, so
run them one at a time.

The GitHub Actions workflow in `.github/workflows/build.yml` runs the full
matrix (all kernels, all variants, and the optional features).

## Required packages

GCC and binutils for the host and for `aarch64-linux-gnu`, clang (for the
clang variant), `make`, `bc`, `bison`, `flex`, `libelf` and OpenSSL
development files, `wget`, `tar` and `xz`.

## Hardware testing

Build tests do not exercise the driver. Before a release, test with a real
RTL8192EU adapter: load/unload, scan, connect (WPA2/WPA3), throughput
(e.g. iperf3), AP mode, suspend/resume and hot-unplug, and watch the kernel
log for warnings.
