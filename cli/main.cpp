// jpeg_edu — educational JPEG / BMP / PPM toolkit CLI
// Usage: jpeg_edu <command> [options]

#include "libjpeg_edu/bmp_io.h"
#include "libjpeg_edu/decoder.h"
#include "libjpeg_edu/encoder.h"
#include "libjpeg_edu/filters.h"
#include "libjpeg_edu/image_buffer.h"
#include "libjpeg_edu/octree.h"
#include "libjpeg_edu/ppm_io.h"

#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace libjpeg_edu;

namespace {

void usage() {
    std::cerr <<
        "jpeg_edu — educational image toolkit\n"
        "\n"
        "Commands:\n"
        "  encode  <input.bmp|ppm> <output.jpg> [-q QUALITY] [-gray] [-progressive]\n"
        "  decode  <input.jpg> <output.bmp|ppm>\n"
        "  info    <file.bmp|jpg>\n"
        "  filter  <input.bmp|ppm> <output.bmp|ppm> -f <blur|sharpen|edge|sobel|median>\n"
        "  quantize <input.bmp|ppm> <output.bmp|ppm> [-n COLORS]\n"
        "  bench   <input.bmp> [-q QUALITY] [-rounds N]\n"
        "\n"
        "Options:\n"
        "  -q        JPEG quality 1-100 (default 85)\n"
        "  -gray     Encode as grayscale\n"
        "  -progressive  Use progressive JPEG\n"
        "  -f        Filter type\n"
        "  -n        Max palette colors (default 256)\n"
        "  -rounds   Benchmark iterations (default 5)\n";
}

bool ends_with(const std::string& s, const std::string& suffix) {
    if (suffix.size() > s.size()) return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

ImageBuffer load_image(const std::string& path) {
    if (ends_with(path, ".bmp") || ends_with(path, ".BMP")) {
        return read_bmp(path);
    }
    if (ends_with(path, ".ppm") || ends_with(path, ".pgm") ||
        ends_with(path, ".PPM") || ends_with(path, ".PGM")) {
        return read_ppm(path);
    }
    if (ends_with(path, ".jpg") || ends_with(path, ".jpeg") ||
        ends_with(path, ".JPG") || ends_with(path, ".JPEG")) {
        return decode_jpeg(path);
    }
    throw std::runtime_error("Unknown input format: " + path);
}

void save_image(const std::string& path, const ImageBuffer& img,
                const EncodeOptions& opts = {}) {
    if (ends_with(path, ".bmp") || ends_with(path, ".BMP")) {
        write_bmp(path, img);
    } else if (ends_with(path, ".ppm") || ends_with(path, ".pgm") ||
               ends_with(path, ".PPM") || ends_with(path, ".PGM")) {
        write_ppm(path, img);
    } else if (ends_with(path, ".jpg") || ends_with(path, ".jpeg") ||
               ends_with(path, ".JPG") || ends_with(path, ".JPEG")) {
        encode_jpeg(path, img, opts);
    } else {
        throw std::runtime_error("Unknown output format: " + path);
    }
}

int cmd_encode(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: jpeg_edu encode <input> <output.jpg> [options]\n";
        return 1;
    }
    std::string input = argv[0];
    std::string output = argv[1];
    EncodeOptions opts;
    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            opts.quality = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "-gray") == 0) {
            opts.color = false;
        } else if (std::strcmp(argv[i], "-progressive") == 0) {
            opts.progressive = true;
        }
    }
    auto img = load_image(input);
    encode_jpeg(output, img, opts);
    std::cout << "Encoded " << input << " -> " << output
              << " (q=" << opts.quality
              << (opts.progressive ? ", progressive" : "")
              << (opts.color ? "" : ", grayscale") << ")\n";
    return 0;
}

int cmd_decode(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: jpeg_edu decode <input.jpg> <output.bmp|ppm>\n";
        return 1;
    }
    auto img = decode_jpeg(argv[0]);
    save_image(argv[1], img);
    std::cout << "Decoded " << argv[0] << " -> " << argv[1]
              << " (" << img.width << "x" << img.height << ")\n";
    return 0;
}

int cmd_info(int argc, char* argv[]) {
    if (argc < 1) {
        std::cerr << "Usage: jpeg_edu info <file>\n";
        return 1;
    }
    std::string path = argv[0];
    ImageInfo info;
    if (ends_with(path, ".bmp") || ends_with(path, ".BMP")) {
        info = bmp_info(path);
    } else if (ends_with(path, ".jpg") || ends_with(path, ".jpeg") ||
               ends_with(path, ".JPG") || ends_with(path, ".JPEG")) {
        info = jpeg_info(path);
    } else {
        std::cerr << "info supports .bmp and .jpg files\n";
        return 1;
    }
    std::cout << "Format:     " << info.format << "\n"
              << "Dimensions: " << info.width << " x " << info.height << "\n"
              << "Channels:   " << info.channels << "\n"
              << "Bit depth:  " << info.bit_depth << "\n"
              << "File size:  " << info.file_size << " bytes\n";
    return 0;
}

int cmd_filter(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: jpeg_edu filter <input> <output> -f <type>\n";
        return 1;
    }
    std::string input = argv[0];
    std::string output = argv[1];
    FilterType ftype = FilterType::kBlur;
    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            if (!parse_filter_type(argv[++i], ftype)) {
                std::cerr << "Unknown filter: " << argv[i] << "\n";
                return 1;
            }
        }
    }
    auto img = load_image(input);
    auto result = apply_filter(img, ftype);
    save_image(output, result);
    std::cout << "Filtered " << input << " -> " << output << "\n";
    return 0;
}

int cmd_quantize(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: jpeg_edu quantize <input> <output> [-n COLORS]\n";
        return 1;
    }
    std::string input = argv[0];
    std::string output = argv[1];
    unsigned max_colors = 256;
    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            max_colors = static_cast<unsigned>(std::atoi(argv[++i]));
        }
    }
    auto img = load_image(input);
    auto palette = octree_palette(img, max_colors);
    auto result = apply_palette(img, palette);
    save_image(output, result);
    std::cout << "Quantized " << input << " -> " << output
              << " (" << palette.size() << " colors)\n";
    return 0;
}

double compute_psnr(const ImageBuffer& a, const ImageBuffer& b) {
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

int cmd_bench(int argc, char* argv[]) {
    if (argc < 1) {
        std::cerr << "Usage: jpeg_edu bench <input.bmp> [-q QUALITY] [-rounds N]\n";
        return 1;
    }
    std::string input = argv[0];
    int quality = 85;
    int rounds = 5;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            quality = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "-rounds") == 0 && i + 1 < argc) {
            rounds = std::atoi(argv[++i]);
        }
    }

    auto img = load_image(input);
    size_t raw_bytes = img.data.size();
    double raw_mb = static_cast<double>(raw_bytes) / (1024.0 * 1024.0);

    std::string tmp_jpg = "/tmp/jpeg_edu_bench.jpg";

    EncodeOptions opts;
    opts.quality = quality;

    // Warm up
    encode_jpeg(tmp_jpg, img, opts);
    decode_jpeg(tmp_jpg);

    // Encode benchmark
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int r = 0; r < rounds; ++r) {
        encode_jpeg(tmp_jpg, img, opts);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double enc_ms = std::chrono::duration<double, std::milli>(t1 - t0).count() / rounds;

    // Decode benchmark
    t0 = std::chrono::high_resolution_clock::now();
    ImageBuffer decoded;
    for (int r = 0; r < rounds; ++r) {
        decoded = decode_jpeg(tmp_jpg);
    }
    t1 = std::chrono::high_resolution_clock::now();
    double dec_ms = std::chrono::duration<double, std::milli>(t1 - t0).count() / rounds;

    double psnr = compute_psnr(img, decoded);

    std::cout << "Benchmark: " << input << " (" << img.width << "x"
              << img.height << ", q=" << quality << ")\n"
              << "  Encode:  " << enc_ms << " ms  ("
              << (raw_mb / (enc_ms / 1000.0)) << " MB/s)\n"
              << "  Decode:  " << dec_ms << " ms  ("
              << (raw_mb / (dec_ms / 1000.0)) << " MB/s)\n"
              << "  PSNR:    " << psnr << " dB\n"
              << "  Rounds:  " << rounds << "\n";
    return 0;
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 1;
    }

    std::string cmd = argv[1];
    int sub_argc = argc - 2;
    char** sub_argv = argv + 2;

    try {
        if (cmd == "encode")   return cmd_encode(sub_argc, sub_argv);
        if (cmd == "decode")   return cmd_decode(sub_argc, sub_argv);
        if (cmd == "info")     return cmd_info(sub_argc, sub_argv);
        if (cmd == "filter")   return cmd_filter(sub_argc, sub_argv);
        if (cmd == "quantize") return cmd_quantize(sub_argc, sub_argv);
        if (cmd == "bench")    return cmd_bench(sub_argc, sub_argv);
        if (cmd == "--help" || cmd == "-h") {
            usage();
            return 0;
        }
        std::cerr << "Unknown command: " << cmd << "\n";
        usage();
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
