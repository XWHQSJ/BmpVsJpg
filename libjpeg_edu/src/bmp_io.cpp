#include "libjpeg_edu/bmp_io.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>

namespace libjpeg_edu {

namespace {

#pragma pack(push, 1)
struct BmpFileHeader {
    uint16_t type;        // "BM"
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t off_bits;
};

struct BmpInfoHeader {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    int32_t  x_pels_per_meter;
    int32_t  y_pels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
};
#pragma pack(pop)

// BMP rows are padded to 4-byte boundaries
int bmp_row_stride(int width, int bit_count) {
    return ((width * bit_count + 31) / 32) * 4;
}

} // namespace

ImageBuffer read_bmp(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open BMP file: " + path);
    }

    BmpFileHeader fh{};
    file.read(reinterpret_cast<char*>(&fh), sizeof(fh));
    if (fh.type != 0x4D42) { // "BM"
        throw std::runtime_error("Not a BMP file: " + path);
    }

    BmpInfoHeader ih{};
    file.read(reinterpret_cast<char*>(&ih), sizeof(ih));
    if (ih.bit_count != 24) {
        throw std::runtime_error(
            "Only 24-bit BMP is supported, got " +
            std::to_string(ih.bit_count) + "-bit");
    }
    if (ih.compression != 0) {
        throw std::runtime_error("Compressed BMP not supported");
    }

    bool bottom_up = ih.height > 0;
    int width = ih.width;
    int height = bottom_up ? ih.height : -ih.height;

    int src_stride = bmp_row_stride(width, 24);

    ImageBuffer img;
    img.width = width;
    img.height = height;
    img.channels = 3;
    img.data.resize(static_cast<size_t>(width) * height * 3);

    file.seekg(fh.off_bits, std::ios::beg);

    std::vector<uint8_t> row_buf(src_stride);
    for (int y = 0; y < height; ++y) {
        file.read(reinterpret_cast<char*>(row_buf.data()), src_stride);
        // BMP stores BGR, bottom-up by default
        int dst_y = bottom_up ? (height - 1 - y) : y;
        uint8_t* dst = img.data.data() + static_cast<size_t>(dst_y) * width * 3;
        for (int x = 0; x < width; ++x) {
            dst[x * 3 + 0] = row_buf[x * 3 + 2]; // R
            dst[x * 3 + 1] = row_buf[x * 3 + 1]; // G
            dst[x * 3 + 2] = row_buf[x * 3 + 0]; // B
        }
    }

    return img;
}

void write_bmp(const std::string& path, const ImageBuffer& img) {
    if (img.empty()) {
        throw std::runtime_error("Cannot write empty image");
    }
    if (img.channels != 3 && img.channels != 1) {
        throw std::runtime_error("BMP writer supports 1 or 3 channel images");
    }

    int out_channels = 3; // always write 24-bit BMP
    int row_stride = bmp_row_stride(img.width, 24);
    uint32_t pixel_data_size = static_cast<uint32_t>(row_stride) * img.height;

    BmpFileHeader fh{};
    fh.type = 0x4D42;
    fh.off_bits = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);
    fh.size = fh.off_bits + pixel_data_size;
    fh.reserved1 = 0;
    fh.reserved2 = 0;

    BmpInfoHeader ih{};
    ih.size = sizeof(BmpInfoHeader);
    ih.width = img.width;
    ih.height = img.height; // bottom-up
    ih.planes = 1;
    ih.bit_count = 24;
    ih.compression = 0;
    ih.size_image = pixel_data_size;

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot create BMP file: " + path);
    }

    file.write(reinterpret_cast<const char*>(&fh), sizeof(fh));
    file.write(reinterpret_cast<const char*>(&ih), sizeof(ih));

    std::vector<uint8_t> row_buf(row_stride, 0);
    for (int y = img.height - 1; y >= 0; --y) { // bottom-up
        const uint8_t* src = img.data.data() + static_cast<size_t>(y) * img.width * img.channels;
        for (int x = 0; x < img.width; ++x) {
            if (img.channels == 3) {
                row_buf[x * 3 + 0] = src[x * 3 + 2]; // B
                row_buf[x * 3 + 1] = src[x * 3 + 1]; // G
                row_buf[x * 3 + 2] = src[x * 3 + 0]; // R
            } else {
                uint8_t g = src[x];
                row_buf[x * 3 + 0] = g;
                row_buf[x * 3 + 1] = g;
                row_buf[x * 3 + 2] = g;
            }
        }
        file.write(reinterpret_cast<const char*>(row_buf.data()), row_stride);
    }
}

ImageInfo bmp_info(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open BMP file: " + path);
    }

    BmpFileHeader fh{};
    file.read(reinterpret_cast<char*>(&fh), sizeof(fh));
    if (fh.type != 0x4D42) {
        throw std::runtime_error("Not a BMP file: " + path);
    }

    BmpInfoHeader ih{};
    file.read(reinterpret_cast<char*>(&ih), sizeof(ih));

    ImageInfo info;
    info.width = ih.width;
    info.height = ih.height > 0 ? ih.height : -ih.height;
    info.bit_depth = ih.bit_count;
    info.channels = ih.bit_count / 8;
    info.file_size = fh.size;
    info.format = "BMP";

    return info;
}

} // namespace libjpeg_edu
