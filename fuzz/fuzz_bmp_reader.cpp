// libFuzzer target for BMP reader.
// Feeds arbitrary byte sequences to read_bmp via a temporary file
// to find crashes in the BMP parsing path.

#include "libjpeg_edu/bmp_io.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 14) {  // BMP file header is 14 bytes minimum
        return 0;
    }

    // Write fuzz input to a temp file since read_bmp expects a path
    std::string tmp = "/tmp/fuzz_bmp_input.bmp";
    FILE* f = std::fopen(tmp.c_str(), "wb");
    if (!f) return 0;
    std::fwrite(data, 1, size, f);
    std::fclose(f);

    try {
        auto img = libjpeg_edu::read_bmp(tmp);
        if (!img.empty()) {
            volatile uint8_t sink = img.data[0];
            (void)sink;
        }
    } catch (const std::exception&) {
        // Expected for malformed inputs
    } catch (...) {
    }

    std::remove(tmp.c_str());
    return 0;
}
