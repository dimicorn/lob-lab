# LOB-Lab

Mixed-language market-data replay, limit-order-book reconstruction, and ML research platform:
**Rust** ingestion → **C ABI** → **C++20** replay engine → **Python** research.

See [overview.md](overview.md) for scope, milestones, and collaboration rules.

**Status:** M0 (toolchain). The Rust library exposes only `lob_ffi_abi_version()`.

## Prerequisites

- Rust (version pinned in `rust-toolchain.toml`; `rustup` installs it automatically)
- CMake ≥ 3.25 and a C++20 compiler (GCC ≥ 13 or Clang ≥ 17)
- Network access on first configure (CMake fetches Corrosion and GoogleTest)

## Build and test

```sh
# Rust
cargo fmt --all --check
cargo clippy --workspace --all-targets -- -D warnings
cargo test --workspace

# C++ (presets: debug | release | asan)
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
./build/debug/lob_hello
```

CMake builds the Cargo workspace through [Corrosion](https://github.com/corrosion-rs/corrosion) and links
the `lob_ffi` static library into the C++ targets. `release` is `RelWithDebInfo`, so profilers can map
samples to source lines. `asan` instruments the C/C++ code with ASan and UBSan; the Rust static library
is not instrumented.

## Layout

```text
Cargo.toml              Cargo workspace
rust/ffi/               lob-ffi crate (staticlib + rlib): C ABI exports
include/ffi/lob_ffi.h   C header for the ABI (keep in sync with rust/ffi/src/lib.rs)
apps/                   C++ executables
tests/                  GoogleTest suites + C11 header compile check
```
