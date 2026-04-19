#pragma once

#include "image_buffer.h"
#include <string>

namespace libjpeg_edu {

struct EncodeOptions {
    int quality = 85;         // 1..100
    bool color = true;        // false = grayscale output
    bool progressive = false; // progressive JPEG encoding
};

// Encode an ImageBuffer to a JPEG file using libjpeg.
// Throws std::runtime_error on hard failures.
void encode_jpeg(const std::string& path,
                 const ImageBuffer& img,
                 const EncodeOptions& opts = {});

} // namespace libjpeg_edu
