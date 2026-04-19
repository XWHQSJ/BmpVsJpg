#pragma once

#include "libjpeg_edu/image_buffer.h"

#include <cstdint>
#include <vector>

namespace libjpeg_edu {

// Octree color quantization.
// Takes an RGB image and reduces it to at most `max_colors` distinct colors.
// Returns a palette as a vector of 0x00RRGGBB packed uint32_t values.
std::vector<uint32_t> octree_palette(const ImageBuffer& img,
                                     unsigned max_colors,
                                     unsigned color_bits = 6);

// Apply the given palette to produce a palettized image.
// Returns an ImageBuffer with channels=3 where each pixel is the nearest
// palette entry.
ImageBuffer apply_palette(const ImageBuffer& img,
                          const std::vector<uint32_t>& palette);

} // namespace libjpeg_edu
