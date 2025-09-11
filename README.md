# Linux Driver for Realtek RTL8192EU

## Description
This is yet another repository which contains the source code of the Linux driver
for the Realtek RTL8192EU-VL-CG chip.

The source code of this driver has been taken from a [ZIP archive](https://files.dlink.com.au/products/DWA-131/REV_E/Drivers/DWA-131_E1_Linux_v5.6.3.1/)
which is provided by the D-Link Corporation.

## Motivation
This repository serves as a personal training ground for understanding how
Linux kernel modules interact with device drivers. While keeping the original
code base intact, the goal of this fork is to update the driver for modern
kernel releases and to fix issues that surface during the porting process.

## Features
* Driver Version: v5.6.3.1_34030.20190613_COEX20171113-0047
* Release Date: 2019-09-20
* Supported Linux Kernel Version: 5.4 and newer

## License
This software is licensed under the GPLv2 only,
as you can see in the various header files of the source code.

## Copyright
The Realtek Corporation is the copyright holder of this software.

## Testing

See [TESTING.md](TESTING.md) for instructions on building the driver against a
cross‑compiled Linux kernel. The required packages listed there must be
installed before running `tests/test_kernel_5.4.sh`.
