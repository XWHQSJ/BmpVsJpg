// libFuzzer target for JPEG decoder.
// Feeds arbitrary byte sequences into decode_jpeg_mem to find crashes,
// undefined behavior, and memory errors in the decode path.

#include "libjpeg_edu/decoder.h"

#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Skip trivially small inputs — libjpeg needs at least a few bytes
    // for SOI marker (0xFFD8) before it can do anything meaningful.
    if (size < 2) {
        return 0;
    }

    try {
        auto img = libjpeg_edu::decode_jpeg_mem(data, size);
        // If decode succeeded, do a basic sanity check to exercise
        // the returned buffer (prevents optimizer from eliding the call).
        if (!img.empty()) {
            volatile uint8_t sink = img.data[0];
            (void)sink;
        }
    } catch (const std::exception&) {
        // Expected — most fuzz inputs are not valid JPEGs.
    } catch (...) {
        // Catch everything to prevent fuzzer from treating unknown
        // exceptions as crashes.
    }

    return 0;
}
