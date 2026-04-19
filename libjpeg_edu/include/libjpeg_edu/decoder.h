#pragma once

#include "image_buffer.h"
#include <string>

namespace libjpeg_edu {

// Decode a JPEG file into an RGB ImageBuffer using libjpeg.
// Throws std::runtime_error on hard failures.
ImageBuffer decode_jpeg(const std::string& path);

// Decode JPEG data from a memory buffer into an RGB ImageBuffer.
// Throws std::runtime_error on hard failures.
ImageBuffer decode_jpeg_mem(const uint8_t* data, size_t size);

// Query JPEG file info without full decode.
ImageInfo jpeg_info(const std::string& path);

} // namespace libjpeg_edu
