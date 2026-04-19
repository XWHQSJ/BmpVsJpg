#pragma once

#include "image_buffer.h"
#include <string>

namespace libjpeg_edu {

// Read a PPM (P6 binary) or PGM (P5 binary) file.
// Throws std::runtime_error on failure.
ImageBuffer read_ppm(const std::string& path);

// Write an ImageBuffer to PPM (P6) or PGM (P5) depending on channels.
// Throws std::runtime_error on failure.
void write_ppm(const std::string& path, const ImageBuffer& img);

} // namespace libjpeg_edu
