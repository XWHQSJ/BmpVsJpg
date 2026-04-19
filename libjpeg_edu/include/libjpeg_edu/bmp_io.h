#pragma once

#include "image_buffer.h"
#include <string>

namespace libjpeg_edu {

// Read a BMP file into an RGB (24-bit) ImageBuffer.
// Throws std::runtime_error on hard failures.
ImageBuffer read_bmp(const std::string& path);

// Write an ImageBuffer to a 24-bit BMP file.
// Throws std::runtime_error on hard failures.
void write_bmp(const std::string& path, const ImageBuffer& img);

// Query BMP file info without loading pixel data.
ImageInfo bmp_info(const std::string& path);

} // namespace libjpeg_edu
