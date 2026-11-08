// Minimal Rust -> C++ smoke executable.
#include <cstdint>
#include <cstdlib>
#include <ffi/lob_ffi.h>
#include <iostream>

int main() {
    const std::uint32_t version = lob_ffi_abi_version();
    std::cout << "lob_ffi ABI version: " << version << '\n';
    return version == LOB_FFI_ABI_VERSION ? EXIT_SUCCESS : EXIT_FAILURE;
}
