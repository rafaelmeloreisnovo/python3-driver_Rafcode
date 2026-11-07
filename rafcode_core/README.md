# RAFCODEΦ independent freestanding byte core (candidate)

Scope: an original C11 bounded byte cursor. This is NOT a replacement for
the Python driver, libfbclient, or the Firebird client API. No Firebird
protocol compatibility, device speedup, or live database operation is claimed.

## Provenance / license boundary

- Upstream: FirebirdSQL/python3-driver, root LICENSE = MIT, unchanged.
- Original files in rafcode_core/: SPDX LicenseRef-RAFCODEPHI-Authorial-1.0.
- Terms: LICENSES/RAFCODEPHI-AUTHORIAL-1.0.txt; non-OSI custom grant.
- No existing upstream source was modified, moved or relicensed.
- Packaging: existing Hatchling wheel includes src/firebird only; this
  candidate core is not silently shipped with the Python wheel.

## Real, limited dependency boundary

- Runtime: no libc, malloc, Python, OS syscalls, dynamic loader calls,
  POSIX headers, or other external runtime libraries in the core.
- Compilation: requires a compiler supplying GCC/Clang integer-type macros
  and an object linker for the chosen target; these are BUILD tools.
- The host test executable has normal OS start-up. This does NOT make
  the separate core object dependent on a host runtime.
- ARMv7/AArch64 are compile targets, not claims of device execution.
- OS syscalls, if ever needed, belong in a separate audited platform adapter.

## Contract

Reader/writer cursors use length and offset. Every read/write validates
bounds with subtraction before pointer access. On errors, output objects,
buffer bytes and cursor offsets remain unchanged. No aliasing-dependent
word loads; byte operations avoid unaligned-access faults. A frame is a
generic 4-byte big-endian length prefix with explicit max payload, not
the Firebird wire format.

## Reproduce on a GCC/Clang host

    cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -ffreestanding -fno-builtin \
      -fno-stack-protector -Irafcode_core/include \
      -c rafcode_core/src/rafcode_cursor.c -o /tmp/rafcode_cursor.o
    nm -u /tmp/rafcode_cursor.o
    cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic \
      -Irafcode_core/include rafcode_core/src/rafcode_cursor.c \
      rafcode_core/tests/cursor_selftest.c -o /tmp/rafcode_selftest
    /tmp/rafcode_selftest

The nm -u output must be empty. Nonzero test exit is a failure. The CI
also compiles objects with -nostdinc for ARMv7 and AArch64 without SDK
headers. These tests do not require a Firebird server.

## Optimization boundary

Comments and whitespace do NOT directly command the C compiler or set
binary alignment. Flags, recognized attributes, linker scripts and actual
preprocessor directives do. This revision does not parse comment hints.
Use measured compiler output, alignment constraints, disassembly and
cross-architecture parity tests BEFORE adding inline assembly or hex
opcodes. Do not claim speedups from fewer source loops or symbols.

## Gates / rollback

State: independent candidate; no Python integration; merge HOLD.
Next: parity on ARM32 hardware + Firebird API compatibility design with
independent provenance and permissions. If gates fail, leave master intact
and supersede this branch through a reviewed successor; never rewrite the
upstream original or discard its license.
