#include "libjpeg_edu/ppm_io.h"

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

namespace libjpeg_edu {

namespace {

// Skip whitespace and comments in PPM header
void skip_ws_comments(std::ifstream& f) {
    while (true) {
        int c = f.peek();
        if (c == '#') {
            std::string line;
            std::getline(f, line);
        } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            f.get();
        } else {
            break;
        }
    }
}

int read_int(std::ifstream& f) {
    skip_ws_comments(f);
    int val = 0;
    f >> val;
    return val;
}

} // namespace

ImageBuffer read_ppm(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open PPM/PGM file: " + path);
    }

    char magic[3] = {};
    file.read(magic, 2);

    bool is_pgm = (magic[0] == 'P' && magic[1] == '5');
    bool is_ppm = (magic[0] == 'P' && magic[1] == '6');
    if (!is_pgm && !is_ppm) {
        throw std::runtime_error("Unsupported format (expected P5 or P6): " + path);
    }

    int width = read_int(file);
    int height = read_int(file);
    int maxval = read_int(file);
    file.get(); // consume single whitespace after maxval

    if (maxval != 255) {
        throw std::runtime_error("Only maxval=255 supported, got " + std::to_string(maxval));
    }

    ImageBuffer img;
    img.width = width;
    img.height = height;
    img.channels = is_ppm ? 3 : 1;
    img.data.resize(static_cast<size_t>(width) * height * img.channels);

    file.read(reinterpret_cast<char*>(img.data.data()),
              static_cast<std::streamsize>(img.data.size()));

    return img;
}

void write_ppm(const std::string& path, const ImageBuffer& img) {
    if (img.empty()) {
        throw std::runtime_error("Cannot write empty image");
    }

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot create PPM/PGM file: " + path);
    }

    if (img.channels == 1) {
        file << "P5\n";
    } else if (img.channels == 3) {
        file << "P6\n";
    } else {
        throw std::runtime_error("PPM writer supports 1 or 3 channel images");
    }

    file << img.width << " " << img.height << "\n255\n";
    file.write(reinterpret_cast<const char*>(img.data.data()),
               static_cast<std::streamsize>(img.data.size()));
}

} // namespace libjpeg_edu
