add_executable(ffi_smoke_test ffi_smoke_test.cpp)
target_link_libraries(ffi_smoke_test PRIVATE lob_ffi lob_warnings GTest::gtest_main)

# Header must stay valid C11.
add_library(ffi_header_c_check OBJECT ffi_header_c_check.c)
target_link_libraries(ffi_header_c_check PRIVATE lob_ffi)
set_target_properties(ffi_header_c_check PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
target_compile_options(ffi_header_c_check PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${LOB_WERROR}>:-Werror>)

include(GoogleTest)
gtest_discover_tests(ffi_smoke_test)
