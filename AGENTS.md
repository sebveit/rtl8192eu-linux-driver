# Repo Guide for rtl8192eu-linux-driver

This repository contains the source code for the Realtek RTL8192EU wireless driver along with helper scripts.

## Coding Style
- Follow the Linux kernel coding style.
- Use tabs for indentation (equivalent to 8 spaces).
- Keep line length under 100 characters when possible.

## Commit Messages
- Begin the subject line with a short component if applicable, e.g. `core:` or `tests:`.
- Use the imperative mood ("Add support for ...").
- Include a short description in the body if the change is nontrivial.

## Testing
- Build the driver against all supported kernels with `make test`, or a single
  kernel/variant with `./kernel-tests/build-test.sh <version> [variant]`; see `TESTING.md`.
- If you modify any shell scripts, run `shellcheck` on them.
- Ensure the work tree is clean before committing.

