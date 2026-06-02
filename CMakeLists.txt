cmake_minimum_required(VERSION 3.16)
project(cpp-snippet-demo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan integration — finds packages declared in conanfile.txt
# Run: conan install . --build=missing  before cmake
if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
endif()

find_package(OpenSSL QUIET)
find_package(ZLIB QUIET)
find_package(CURL QUIET)

# Enable all warnings to highlight real code quality issues
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files — each intentionally contains snippets from known open-source origins
set(SOURCES
    src/main.cpp
    src/checksum/md5.cpp
    src/checksum/crc32.cpp
    src/encoding/base64.cpp
    src/compression/lz_compress.cpp
    src/utils/string_utils.cpp
)

add_executable(snippet-demo ${SOURCES})

target_include_directories(snippet-demo PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Install target
install(TARGETS snippet-demo DESTINATION bin)

message(STATUS "=============================================================")
message(STATUS "  JFrog Snippet Detection Demo — C++ File Utility Tool")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  After building, run snippet detection with:")
message(STATUS "  $ jf audit --sca --sbom --static-sca --snippet --watches=<watch>")
message(STATUS "=============================================================")
