cmake_minimum_required(VERSION 3.25)
# Keep VERSION in sync with [workspace.package] version in Cargo.toml.
project(lob_lab VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LOB_BUILD_TESTS "Build C++ tests" ON)
option(LOB_WERROR "Treat compiler warnings as errors" OFF)
set(LOB_SANITIZERS "" CACHE STRING "Comma-separated -fsanitize= list, e.g. address,undefined")

# ---- Sanitizers (C/C++ side; the Rust staticlib is not instrumented) --------
if(LOB_SANITIZERS)
  add_compile_options(-fsanitize=${LOB_SANITIZERS} -fno-omit-frame-pointer -fno-sanitize-recover=all)
  add_link_options(-fsanitize=${LOB_SANITIZERS})
endif()

# ---- Shared warning flags ----------------------------------------------------
add_library(lob_warnings INTERFACE)
target_compile_options(lob_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion
  -Wold-style-cast -Wnon-virtual-dtor -Woverloaded-virtual
  $<$<BOOL:${LOB_WERROR}>:-Werror>)

include(FetchContent)

# ---- Rust: build the Cargo workspace and import lob_ffi as a static lib ------
# Corrosion maps CMAKE_BUILD_TYPE to the Cargo profile and adds the native
# system libraries a Rust staticlib needs (pthread, dl, m, ...).
FetchContent_Declare(Corrosion
  GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
  GIT_TAG v0.5.1
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(Corrosion)

corrosion_import_crate(
  MANIFEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml
  CRATES lob-ffi
  CRATE_TYPES staticlib)
target_include_directories(lob_ffi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---- Executables ---------------------------------------------------------------
add_executable(lob_hello apps/hello_ffi.cpp)
target_link_libraries(lob_hello PRIVATE lob_ffi lob_warnings)

# ---- Tests -------------------------------------------------------------------
if(LOB_BUILD_TESTS)
  enable_testing()
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_subdirectory(tests)
  add_test(NAME lob_hello COMMAND lob_hello)
endif()
