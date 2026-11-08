//! C ABI surface for LOB-Lab.
//!
//! M0 placeholder: exposes only an ABI version so the Rust -> C++ build and
//! link path can be verified. The real event/batch interface is designed in M1.
//!
//! Keep `include/ffi/lob_ffi.h` in sync with every `extern "C"` item here.

/// ABI version reported to C++. `0` means "pre-schema placeholder".
///
/// Must equal `LOB_FFI_ABI_VERSION` in `include/ffi/lob_ffi.h`; the C++ smoke
/// test checks this so header/library drift fails CI.
pub const ABI_VERSION: u32 = 0;

/// Returns the ABI version implemented by this library.
#[unsafe(no_mangle)]
pub extern "C" fn lob_ffi_abi_version() -> u32 {
    ABI_VERSION
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn abi_version_matches_constant() {
        assert_eq!(lob_ffi_abi_version(), ABI_VERSION);
    }
}
