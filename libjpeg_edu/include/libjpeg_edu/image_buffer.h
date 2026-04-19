#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace libjpeg_edu {

struct ImageBuffer {
    std::vector<uint8_t> data;
    int width = 0;
    int height = 0;
    int channels = 0; // 1 = grayscale, 3 = RGB

    bool empty() const { return data.empty(); }
    size_t row_stride() const { return static_cast<size_t>(width) * channels; }
    size_t pixel_count() const { return static_cast<size_t>(width) * height; }
};

struct ImageInfo {
    int width = 0;
    int height = 0;
    int channels = 0;
    int bit_depth = 0;
    size_t file_size = 0;
    std::string format;
};

} // namespace libjpeg_edu
