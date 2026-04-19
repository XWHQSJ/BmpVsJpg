# BmpVsJpg

[![CI](https://github.com/XWHQSJ/BmpVsJpg/actions/workflows/ci.yml/badge.svg)](https://github.com/XWHQSJ/BmpVsJpg/actions/workflows/ci.yml)
[![Release](https://github.com/XWHQSJ/BmpVsJpg/actions/workflows/release.yml/badge.svg)](https://github.com/XWHQSJ/BmpVsJpg/releases)
[![CodeQL](https://github.com/XWHQSJ/BmpVsJpg/actions/workflows/codeql.yml/badge.svg)](https://github.com/XWHQSJ/BmpVsJpg/actions/workflows/codeql.yml)
![C++](https://img.shields.io/badge/language-C%2B%2B17-blue)
![License: MIT](https://img.shields.io/badge/license-MIT-green)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey)

Educational JPEG/BMP image toolkit. Includes a portable C++ core library, a cross-platform CLI tool, and a Windows MFC GUI. Based on the book *"Visual C++ MPEG-JPEG"*.

JPEG/BMP 教学工具集。包含跨平台 C++ 核心库、命令行工具和 Windows MFC GUI。

## Install from Release

Download prebuilt binaries from the [Releases](https://github.com/XWHQSJ/BmpVsJpg/releases) page:

```bash
# macOS (Apple Silicon)
curl -LO https://github.com/XWHQSJ/BmpVsJpg/releases/latest/download/jpeg_edu-macos-arm64-v1.0.0.tar.gz
tar xzf jpeg_edu-macos-arm64-v1.0.0.tar.gz

# Linux (x86_64)
curl -LO https://github.com/XWHQSJ/BmpVsJpg/releases/latest/download/jpeg_edu-linux-x86_64-v1.0.0.tar.gz
tar xzf jpeg_edu-linux-x86_64-v1.0.0.tar.gz
```

Requires libjpeg-turbo at runtime (`brew install jpeg-turbo` / `apt install libjpeg-turbo8`).

## Three Consumption Modes

### 1. CLI Tool (`jpeg_edu`)

```bash
# Build
cmake -B build -S .
cmake --build build

# Encode BMP -> JPEG
./build/jpeg_edu encode photo.bmp photo.jpg -q 85 -progressive

# Decode JPEG -> BMP
./build/jpeg_edu decode photo.jpg output.bmp

# Image info
./build/jpeg_edu info photo.jpg

# Apply filter
./build/jpeg_edu filter input.bmp edges.bmp -f sobel

# Quantize colors (octree)
./build/jpeg_edu quantize input.bmp reduced.bmp -n 32

# Benchmark encode/decode
./build/jpeg_edu bench input.bmp -q 85 -rounds 10
```

### 2. C++ Library (`libjpeg_edu`)

```cpp
#include "libjpeg_edu/decoder.h"
#include "libjpeg_edu/encoder.h"
#include "libjpeg_edu/filters.h"

using namespace libjpeg_edu;

auto img = decode_jpeg("photo.jpg");
auto edges = apply_filter(img, FilterType::kSobel);
encode_jpeg("edges.jpg", edges, {.quality = 90, .progressive = true});
```

Link against `libjpeg_edu` in your CMakeLists.txt:
```cmake
add_subdirectory(path/to/BmpVsJpg)
target_link_libraries(your_target PRIVATE libjpeg_edu)
```

### 3. MFC GUI (Windows only)

Open `BmpVsJpg/BmpVsJpg.sln` in Visual Studio with MFC support. The GUI provides BMP-to-JPEG and JPEG-to-BMP conversion with quality control and image processing filters.

## Build Requirements

| Platform | Requirements |
|----------|-------------|
| macOS/Linux | CMake 3.14+, C++17, libjpeg or libjpeg-turbo |
| Windows | Visual Studio 2019+, CMake, or direct `.sln` for MFC |

```bash
# macOS
brew install jpeg-turbo cmake
cmake -B build -S . && cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

## Project Structure

```
BmpVsJpg/
  libjpeg_edu/             Portable C++ core library (no Windows deps)
    include/libjpeg_edu/   Public headers
    src/                   Implementation
  cli/                     Cross-platform CLI tool
  tests/                   GoogleTest roundtrip + PSNR tests
  fuzz/                    libFuzzer targets + seed corpus
  docs/                    Quality comparison + filter showcase HTML
  qa/                      Asset generation scripts
  .github/workflows/       CI (sanitizers + fuzz + CodeQL + release)
  BmpVsJpg/                Original MFC GUI (Windows only)
  Change_Bmp_Jpg/          Alternative MFC implementation
  CMakeLists.txt           Top-level build
```

## Library Modules

| Module | Description |
|--------|-------------|
| `decoder.h` | JPEG decode via libjpeg, metadata query |
| `encoder.h` | JPEG encode with quality, grayscale, progressive |
| `bmp_io.h` | 24-bit BMP read/write with BGR/bottom-up handling |
| `ppm_io.h` | PPM (P6) and PGM (P5) read/write |
| `filters.h` | Blur, sharpen, edge, Sobel, median convolution filters |
| `octree.h` | Octree color quantization |

## Tests

14 GoogleTest cases covering JPEG roundtrip PSNR (>35 dB @ q95, >25 dB @ q50), BMP/PPM lossless roundtrip, filter output, octree quantization, and error handling.

CI runs all tests under AddressSanitizer and UndefinedBehaviorSanitizer on both Ubuntu and macOS.

## Visual Demos

- [Quality Comparison](docs/quality_comparison.html) -- the same image at quality 10/25/50/75/95/100 with file sizes and PSNR
- [Filter Showcase](docs/filter_showcase.html) -- blur, sharpen, edge, Sobel, and median filter effects with kernel visualizations

Regenerate assets: `qa/generate_comparison.sh` (requires a build of `jpeg_edu`).

## Fuzzing

libFuzzer targets for the JPEG decoder and BMP reader live in `fuzz/`. CI runs each for 5 minutes per push with ASan+UBSan enabled.

```bash
# Build and run locally (requires LLVM clang, not Apple clang)
cmake -B build-fuzz -DENABLE_FUZZ=ON \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ .
cmake --build build-fuzz --target fuzz_decoder
./build-fuzz/fuzz/fuzz_decoder fuzz/corpus/ -max_total_time=60
```

Seed corpus in `fuzz/corpus/` contains JPEGs at various quality levels plus a BMP.

## Cleanup Notes

The duplicate `BmpToJpg/` and `JpgToBmp/` directories have been removed (they were identical copies). Bug fixes applied to `Jpeg.cpp` (tb1/tb2 pointer check) and `BmpVsJpgDlg.cpp` (hardcoded quality).

## References

- Reference book: *Visual C++ MPEG-JPEG*
- Book PDF and source code: [Baidu Netdisk](http://pan.baidu.com/s/1pLEpIzT) password: dgpt
- Additional source code and libraries: [Baidu Netdisk](http://pan.baidu.com/s/1nv0MGhr) password: yw2v

## License

[MIT](LICENSE)

## Security

See [SECURITY.md](SECURITY.md) for vulnerability reporting and security measures.
