// Roundtrip PSNR tests for libjpeg_edu
// Verifies encode -> decode fidelity at various quality levels.

#include "libjpeg_edu/bmp_io.h"
#include "libjpeg_edu/decoder.h"
#include "libjpeg_edu/encoder.h"
#include "libjpeg_edu/filters.h"
#include "libjpeg_edu/image_buffer.h"
#include "libjpeg_edu/octree.h"
#include "libjpeg_edu/ppm_io.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

using namespace libjpeg_edu;

namespace {

// Generate a 32x32 RGB gradient test image
ImageBuffer make_gradient(int w = 32, int h = 32) {
    ImageBuffer img;
    img.width = w;
    img.height = h;
    img.channels = 3;
    img.data.resize(static_cast<size_t>(w) * h * 3);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            size_t off = (static_cast<size_t>(y) * w + x) * 3;
            img.data[off + 0] = static_cast<uint8_t>(x * 255 / (w - 1));   // R
            img.data[off + 1] = static_cast<uint8_t>(y * 255 / (h - 1));   // G
            img.data[off + 2] = static_cast<uint8_t>(128);                   // B
        }
    }
    return img;
}

double compute_psnr(const ImageBuffer& a, const ImageBuffer& b) {
    EXPECT_EQ(a.data.size(), b.data.size());
    if (a.data.size() != b.data.size()) return 0.0;
    double mse = 0.0;
    for (size_t i = 0; i < a.data.size(); ++i) {
        double d = static_cast<double>(a.data[i]) - b.data[i];
        mse += d * d;
    }
    mse /= static_cast<double>(a.data.size());
    if (mse < 1e-10) return 99.0;
    return 10.0 * std::log10(255.0 * 255.0 / mse);
}

std::string tmp_path(const std::string& name) {
    return std::string("/tmp/jpeg_edu_test_") + name;
}

} // namespace

// --- JPEG roundtrip tests ---

TEST(RoundtripTest, Q95_PSNR_Above35dB) {
    auto original = make_gradient(64, 64);
    std::string jpg = tmp_path("q95.jpg");

    EncodeOptions opts;
    opts.quality = 95;
    encode_jpeg(jpg, original, opts);
    auto decoded = decode_jpeg(jpg);

    double psnr = compute_psnr(original, decoded);
    EXPECT_GT(psnr, 35.0) << "PSNR at q95 should be > 35 dB, got " << psnr;
    std::remove(jpg.c_str());
}

TEST(RoundtripTest, Q50_PSNR_Above25dB) {
    auto original = make_gradient(64, 64);
    std::string jpg = tmp_path("q50.jpg");

    EncodeOptions opts;
    opts.quality = 50;
    encode_jpeg(jpg, original, opts);
    auto decoded = decode_jpeg(jpg);

    double psnr = compute_psnr(original, decoded);
    EXPECT_GT(psnr, 25.0) << "PSNR at q50 should be > 25 dB, got " << psnr;
    std::remove(jpg.c_str());
}

TEST(RoundtripTest, ProgressiveJPEG) {
    auto original = make_gradient(32, 32);
    std::string jpg = tmp_path("progressive.jpg");

    EncodeOptions opts;
    opts.quality = 90;
    opts.progressive = true;
    encode_jpeg(jpg, original, opts);
    auto decoded = decode_jpeg(jpg);

    EXPECT_EQ(decoded.width, original.width);
    EXPECT_EQ(decoded.height, original.height);
    double psnr = compute_psnr(original, decoded);
    EXPECT_GT(psnr, 30.0);
    std::remove(jpg.c_str());
}

TEST(RoundtripTest, GrayscaleEncode) {
    auto original = make_gradient(32, 32);
    std::string jpg = tmp_path("gray.jpg");

    EncodeOptions opts;
    opts.quality = 90;
    opts.color = false;
    encode_jpeg(jpg, original, opts);

    auto info = jpeg_info(jpg);
    EXPECT_EQ(info.channels, 1);

    auto decoded = decode_jpeg(jpg); // always returns RGB
    EXPECT_EQ(decoded.channels, 3);
    EXPECT_EQ(decoded.width, original.width);
    std::remove(jpg.c_str());
}

// --- BMP roundtrip ---

TEST(BmpTest, Roundtrip) {
    auto original = make_gradient(48, 48);
    std::string bmp = tmp_path("rt.bmp");

    write_bmp(bmp, original);
    auto loaded = read_bmp(bmp);

    EXPECT_EQ(loaded.width, original.width);
    EXPECT_EQ(loaded.height, original.height);
    EXPECT_EQ(loaded.channels, original.channels);
    EXPECT_EQ(loaded.data, original.data); // BMP is lossless
    std::remove(bmp.c_str());
}

TEST(BmpTest, Info) {
    auto img = make_gradient(16, 16);
    std::string bmp = tmp_path("info.bmp");
    write_bmp(bmp, img);

    auto info = bmp_info(bmp);
    EXPECT_EQ(info.width, 16);
    EXPECT_EQ(info.height, 16);
    EXPECT_EQ(info.format, "BMP");
    EXPECT_EQ(info.bit_depth, 24);
    std::remove(bmp.c_str());
}

// --- PPM roundtrip ---

TEST(PpmTest, RoundtripRGB) {
    auto original = make_gradient(24, 24);
    std::string ppm = tmp_path("rt.ppm");

    write_ppm(ppm, original);
    auto loaded = read_ppm(ppm);

    EXPECT_EQ(loaded.width, original.width);
    EXPECT_EQ(loaded.height, original.height);
    EXPECT_EQ(loaded.data, original.data);
    std::remove(ppm.c_str());
}

TEST(PpmTest, GrayscaleRoundtrip) {
    ImageBuffer gray;
    gray.width = 16;
    gray.height = 16;
    gray.channels = 1;
    gray.data.resize(256);
    for (int i = 0; i < 256; ++i) gray.data[i] = static_cast<uint8_t>(i);

    std::string pgm = tmp_path("rt.pgm");
    write_ppm(pgm, gray);
    auto loaded = read_ppm(pgm);

    EXPECT_EQ(loaded.channels, 1);
    EXPECT_EQ(loaded.data, gray.data);
    std::remove(pgm.c_str());
}

// --- Filter tests ---

TEST(FilterTest, AllFiltersProduceOutput) {
    auto img = make_gradient(32, 32);
    FilterType types[] = {FilterType::kBlur, FilterType::kSharpen,
                          FilterType::kEdge, FilterType::kSobel,
                          FilterType::kMedian};
    for (auto t : types) {
        auto result = apply_filter(img, t);
        EXPECT_EQ(result.width, img.width);
        EXPECT_EQ(result.height, img.height);
        EXPECT_EQ(result.channels, 1); // filters output grayscale
        EXPECT_FALSE(result.empty());
    }
}

TEST(FilterTest, ParseFilterType) {
    FilterType t;
    EXPECT_TRUE(parse_filter_type("blur", t));
    EXPECT_EQ(t, FilterType::kBlur);
    EXPECT_TRUE(parse_filter_type("sobel", t));
    EXPECT_EQ(t, FilterType::kSobel);
    EXPECT_FALSE(parse_filter_type("nonexistent", t));
}

// --- Octree tests ---

TEST(OctreeTest, ReduceColors) {
    auto img = make_gradient(32, 32);
    auto palette = octree_palette(img, 16, 6);
    EXPECT_LE(palette.size(), 16u);
    EXPECT_GE(palette.size(), 2u);
}

TEST(OctreeTest, ApplyPalette) {
    auto img = make_gradient(16, 16);
    auto palette = octree_palette(img, 8);
    auto result = apply_palette(img, palette);
    EXPECT_EQ(result.width, img.width);
    EXPECT_EQ(result.height, img.height);
    EXPECT_EQ(result.channels, 3);

    // Every pixel in result should match a palette entry
    for (size_t i = 0; i < result.pixel_count(); ++i) {
        uint32_t px = (static_cast<uint32_t>(result.data[i * 3]) << 16) |
                      (static_cast<uint32_t>(result.data[i * 3 + 1]) << 8) |
                      static_cast<uint32_t>(result.data[i * 3 + 2]);
        bool found = false;
        for (uint32_t c : palette) {
            if (c == px) { found = true; break; }
        }
        EXPECT_TRUE(found) << "Pixel " << i << " not in palette";
    }
}

// --- Edge cases ---

TEST(EdgeCase, EmptyImageThrows) {
    ImageBuffer empty;
    EXPECT_THROW(apply_filter(empty, FilterType::kBlur), std::runtime_error);
    EXPECT_THROW(encode_jpeg("/tmp/x.jpg", empty, {}), std::runtime_error);
    EXPECT_THROW(write_bmp("/tmp/x.bmp", empty), std::runtime_error);
    EXPECT_THROW(write_ppm("/tmp/x.ppm", empty), std::runtime_error);
}

TEST(EdgeCase, BadFileThrows) {
    EXPECT_THROW(decode_jpeg("/nonexistent.jpg"), std::runtime_error);
    EXPECT_THROW(read_bmp("/nonexistent.bmp"), std::runtime_error);
    EXPECT_THROW(read_ppm("/nonexistent.ppm"), std::runtime_error);
}
