/*
 * Compiles the FFI header as strict C so C++-only constructs cannot creep in.
 * Layout checks (_Static_assert on sizeof/offsetof/alignof) for ABI structs
 * belong here once they exist.
 */
#include <ffi/lob_ffi.h>

uint32_t lob_ffi_header_c_check(void) {
    return lob_ffi_abi_version();
}
