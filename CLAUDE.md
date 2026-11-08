# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

LOB-Lab is a learning/portfolio project: Rust market-data ingestion → C ABI → C++20 order-book replay → Python ML research. `overview.md` is the authoritative spec (scope, milestones M0–M7, testing and benchmarking methodology). Read it before starting any milestone work.

## Collaboration rules (most important)

The author is building Rust/C++ expertise. **Claude must not implement core logic unless the author explicitly asks for a full implementation.**

- **Author-owned:** the Rust parser/codec core, the Rust/C++ FFI design (event schema, batch API, ownership/lifetime contract), the C++ order-book data structures and event application, performance optimizations, concurrency, and interpreting benchmarks.
- **Claude may implement:** Cargo/CMake infrastructure, CI, formatting/linting, test scaffolding and fixtures, routine Python utilities, docs, and visualization.
- For author-owned work: explain the requirements, offer design alternatives, ask for the author's proposal, review their code, and give hints that get progressively more specific. Don't silently rewrite their core code.

## Commands

```sh
# Rust (toolchain pinned in rust-toolchain.toml)
cargo fmt --all --check
cargo clippy --workspace --all-targets -- -D warnings
cargo test --workspace
cargo test -p lob-ffi <test_name_substring>      # single Rust test

# C++ — presets: debug | release (RelWithDebInfo) | asan (Debug + ASan/UBSan)
cmake --preset debug && cmake --build --preset debug && ctest --preset debug
ctest --preset debug -R <regex>                  # single CTest test
./build/debug/tests/ffi_smoke_test --gtest_filter='Suite.Name'
./build/debug/lob_hello

# Formatting check for C/C++ (CI runs this)
git ls-files '*.c' '*.h' '*.cpp' '*.hpp' | xargs clang-format --dry-run --Werror
```

All presets set `LOB_WERROR=ON`, so C++ warnings (`-Wconversion`, `-Wsign-conversion`, `-Wold-style-cast`, ...) fail the build. The first configure needs network access because FetchContent downloads Corrosion and GoogleTest. On this macOS machine, use the Homebrew clang-tidy (`/opt/homebrew/opt/llvm/bin/clang-tidy -p build/debug ...`); the one in `/usr/local/bin` can't find the SDK headers.

## Architecture

- **Rust → C++ linkage:** CMake uses Corrosion to build the Cargo workspace. `corrosion_import_crate` imports the `lob-ffi` crate (`rust/ffi`) as the static-library target `lob_ffi`, which also exports `include/` as its include directory. C++ targets just link `lob_ffi`. Corrosion maps `CMAKE_BUILD_TYPE` to the Cargo profile.
- **The ABI is defined twice and must stay in sync:** every `extern "C"` item in `rust/ffi/src/lib.rs` has a hand-written declaration in `include/ffi/lob_ffi.h` (no cbindgen). Drift is caught by tests:
  - `LOB_FFI_ABI_VERSION` (header) must equal the Rust `ABI_VERSION`; `tests/ffi_smoke_test.cpp` asserts this. Bump both when the ABI changes.
  - `tests/ffi_header_c_check.c` compiles the header as strict C11. ABI struct layout checks (`_Static_assert` on `sizeof`/`offsetof`/`alignof`) go there, mirrored by `size_of`/`align_of` assertions on the Rust side.
- Only fixed-width C types may cross the boundary — never Rust types or C++ standard-library containers. Rust panics and C++ exceptions must not unwind across it.
- The `asan` preset instruments only the C/C++ side; the Rust staticlib isn't sanitized.
- `release` keeps line tables (CMake `RelWithDebInfo`, Cargo `debug = "line-tables-only"`) so profiling can attribute samples to source lines.
- Planned layout (create directories only when needed): `rust/{market_data,event_codec,ffi}`, `src/{book,replay,features,inference}`, `include/{lob,ffi}`, `benchmarks/`, `python/`, `docs/{architecture,experiments,performance}`.

## Domain rules

- Prices are integer ticks and quantities are integers; never compare float prices for equality.
- Historical **reconstruction** applies recorded feed events and must not invent matching-engine behavior. Simulated matching/fills (M7) is a separate model whose assumptions are documented.
- Correctness comes first: build a deterministic, single-threaded version, and add concurrency only when measurements justify it.
- Don't claim zero-copy unless the code actually avoids copies and guarantees layout, alignment, and lifetimes.
- The target platform is Linux x86-64 (CI uses ubuntu-24.04 with gcc and clang). Local development is on macOS arm64.
- Never commit datasets (e.g., LOBSTER samples) or large binaries. `/data/` and `*.bin` are gitignored.

## Commits and releases

- Use Conventional Commits: `type(scope): summary`, with types `feat`, `fix`, `perf`, `refactor`, `test`, `docs`, `style`, `build`, `ci`, `chore`, `revert`. Scopes name subsystems (`ffi`, `codec`, `parser`, `book`, `replay`, `features`, `ml`, `bench`, …). Mark breaking changes with `!`, e.g. any ABI change that bumps `LOB_FFI_ABI_VERSION`.
- git-cliff (`cliff.toml`) builds CHANGELOG.md from these prefixes, and commits without a prefix are left out. PRs are squash-merged, so **the PR title must also be a Conventional Commit**.
- The version is set in two places that must match: `[workspace.package] version` in `Cargo.toml` and `project(lob_lab VERSION …)` in `CMakeLists.txt`. `release.yml` checks both against the tag. Releases go through a release PR (bump versions → `uvx git-cliff --tag vX.Y.Z -o CHANGELOG.md` → `chore(release): vX.Y.Z` PR → tag after merge); the full steps are in CONTRIBUTING.md.

## Current status

M0 (toolchain) is complete: the ABI exposes only `lob_ffi_abi_version()`. Next is M1: the author designs the canonical event schema and the buffer-ownership contract, and Claude reviews it.
