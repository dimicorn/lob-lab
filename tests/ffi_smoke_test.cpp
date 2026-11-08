#include <ffi/lob_ffi.h>
#include <gtest/gtest.h>

TEST(FfiSmoke, AbiVersionMatchesHeader) {
    EXPECT_EQ(lob_ffi_abi_version(), LOB_FFI_ABI_VERSION);
}
