Testing Instructions
====================

Before running any of the kernel build tests, make sure the following packages are installed:

- build-essential (includes `gcc` and `make`)
- `flex`
- `bison`
- `bc`
- `wget`
- `tar`
- `xz-utils`
- the `aarch64-linux-gnu` cross‑compiler toolchain
- any other dependencies required for building a Linux kernel (for example, `libssl-dev` and `libelf-dev`)

These packages must be installed before running `tests/test_kernel_5.4.sh`.

Once these packages are available, you can execute the test script for building against Linux 5.4:

```sh
$ ./tests/test_kernel_5.4.sh
```

The script downloads and prepares the Linux 5.4 sources if needed and then builds the driver modules using the AArch64 cross‑compiler.
