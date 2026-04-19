#include "libjpeg_edu/filters.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <vector>

namespace libjpeg_edu {

bool parse_filter_type(const std::string& name, FilterType& out) {
    if (name == "blur")    { out = FilterType::kBlur;    return true; }
    if (name == "sharpen") { out = FilterType::kSharpen; return true; }
    if (name == "edge")    { out = FilterType::kEdge;    return true; }
    if (name == "sobel")   { out = FilterType::kSobel;   return true; }
    if (name == "median")  { out = FilterType::kMedian;  return true; }
    return false;
}

namespace {

// Convert RGB to single-channel grayscale
std::vector<uint8_t> to_gray(const ImageBuffer& img) {
    if (img.channels == 1) {
        return img.data;
    }
    std::vector<uint8_t> gray(img.pixel_count());
    for (size_t i = 0; i < gray.size(); ++i) {
        const uint8_t* px = img.data.data() + i * 3;
        gray[i] = static_cast<uint8_t>(
            0.299 * px[0] + 0.587 * px[1] + 0.114 * px[2]);
    }
    return gray;
}

// Clamp an int to [0, 255]
uint8_t clamp8(int v) {
    return static_cast<uint8_t>(std::max(0, std::min(255, v)));
}

using Kernel3x3 = int[9];

std::vector<uint8_t> convolve(const std::vector<uint8_t>& gray,
                               int w, int h,
                               const Kernel3x3& kernel,
                               int divisor) {
    std::vector<uint8_t> out(gray.size(), 0);
    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            int sum = 0;
            int ki = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    sum += kernel[ki++] * gray[(y + dy) * w + (x + dx)];
                }
            }
            if (divisor != 1) sum /= divisor;
            out[y * w + x] = clamp8(std::abs(sum));
        }
    }
    return out;
}

std::vector<uint8_t> median_filter(const std::vector<uint8_t>& gray,
                                    int w, int h) {
    std::vector<uint8_t> out(gray.size(), 0);
    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            uint8_t window[9];
            int ki = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    window[ki++] = gray[(y + dy) * w + (x + dx)];
                }
            }
            std::sort(window, window + 9);
            out[y * w + x] = window[4]; // median
        }
    }
    return out;
}

// Sobel magnitude = sqrt(Gx^2 + Gy^2)
std::vector<uint8_t> sobel_filter(const std::vector<uint8_t>& gray,
                                   int w, int h) {
    const Kernel3x3 gx = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    const Kernel3x3 gy = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    std::vector<uint8_t> out(gray.size(), 0);
    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            int sx = 0, sy = 0;
            int ki = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int val = gray[(y + dy) * w + (x + dx)];
                    sx += gx[ki] * val;
                    sy += gy[ki] * val;
                    ++ki;
                }
            }
            double mag = std::sqrt(sx * sx + sy * sy);
            out[y * w + x] = clamp8(static_cast<int>(mag));
        }
    }
    return out;
}

ImageBuffer gray_to_image(const std::vector<uint8_t>& gray, int w, int h) {
    ImageBuffer out;
    out.width = w;
    out.height = h;
    out.channels = 1;
    out.data = gray;
    return out;
}

} // namespace

ImageBuffer apply_filter(const ImageBuffer& img, FilterType type) {
    if (img.empty()) {
        throw std::runtime_error("Cannot filter empty image");
    }

    auto gray = to_gray(img);
    int w = img.width;
    int h = img.height;

    std::vector<uint8_t> result;

    switch (type) {
    case FilterType::kBlur: {
        const Kernel3x3 k = {1, 2, 1, 2, 4, 2, 1, 2, 1};
        result = convolve(gray, w, h, k, 16);
        break;
    }
    case FilterType::kSharpen: {
        const Kernel3x3 k = {0, -1, 0, -1, 5, -1, 0, -1, 0};
        result = convolve(gray, w, h, k, 1);
        break;
    }
    case FilterType::kEdge: {
        const Kernel3x3 k = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
        result = convolve(gray, w, h, k, 1);
        break;
    }
    case FilterType::kSobel:
        result = sobel_filter(gray, w, h);
        break;
    case FilterType::kMedian:
        result = median_filter(gray, w, h);
        break;
    }

    return gray_to_image(result, w, h);
}

} // namespace libjpeg_edu
