#pragma once

#include "image_buffer.h"
#include <string>

namespace libjpeg_edu {

enum class FilterType {
    kBlur,
    kSharpen,
    kEdge,
    kSobel,
    kMedian,
};

// Parse a filter name string. Returns false if unknown.
bool parse_filter_type(const std::string& name, FilterType& out);

// Apply a filter to an ImageBuffer (operates on grayscale internally,
// converts multi-channel to grayscale first if needed).
// Returns a new ImageBuffer with the result.
ImageBuffer apply_filter(const ImageBuffer& img, FilterType type);

} // namespace libjpeg_edu
