/*
 * LOB-Lab C ABI between the Rust ingestion layer and the C++ engine.
 *
 * Rules (to be expanded in M1):
 *   - Plain C only: fixed-width integers, no Rust or C++ standard-library types.
 *   - Every function must be callable from C and C++.
 *   - Keep in sync with rust/ffi/src/lib.rs.
 */
#ifndef LOB_FFI_H
#define LOB_FFI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Expected ABI version. Must match lob_ffi_abi_version(). */
#define LOB_FFI_ABI_VERSION 0u /* NOLINT(cppcoreguidelines-macro-usage): C header */

/* Returns the ABI version implemented by the linked Rust library. */
uint32_t lob_ffi_abi_version(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LOB_FFI_H */
