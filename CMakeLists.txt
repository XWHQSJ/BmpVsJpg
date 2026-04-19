cmake_minimum_required(VERSION 3.14)
project(BmpVsJpg VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- libjpeg dependency ----
find_package(JPEG REQUIRED)

# ---- Portable core library ----
add_library(libjpeg_edu STATIC
    libjpeg_edu/src/decoder.cpp
    libjpeg_edu/src/encoder.cpp
    libjpeg_edu/src/bmp_io.cpp
    libjpeg_edu/src/ppm_io.cpp
    libjpeg_edu/src/filters.cpp
    libjpeg_edu/src/octree.cpp
)
target_include_directories(libjpeg_edu PUBLIC
    libjpeg_edu/include
)
target_link_libraries(libjpeg_edu PUBLIC JPEG::JPEG)

# ---- CLI tool ----
add_executable(jpeg_edu cli/main.cpp)
target_link_libraries(jpeg_edu PRIVATE libjpeg_edu)

# ---- Tests (GoogleTest) ----
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(roundtrip_test tests/roundtrip_test.cpp)
target_link_libraries(roundtrip_test PRIVATE libjpeg_edu GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(roundtrip_test)

# ---- MFC targets (Windows only) ----
if(MSVC)
    message(STATUS "MSVC detected — MFC targets available but not built by default")
    # The original BmpVsJpg MFC dialog project lives in BmpVsJpg/
    # and can be opened with Visual Studio directly.
endif()
