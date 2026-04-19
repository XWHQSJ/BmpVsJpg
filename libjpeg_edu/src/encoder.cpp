#include "libjpeg_edu/encoder.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>

extern "C" {
#include <jpeglib.h>
}

#include <setjmp.h>

namespace libjpeg_edu {

namespace {

struct ErrorMgr {
    jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
    char message[JMSG_LENGTH_MAX];
};

void error_exit_handler(j_common_ptr cinfo) {
    auto* err = reinterpret_cast<ErrorMgr*>(cinfo->err);
    (*cinfo->err->format_message)(cinfo, err->message);
    longjmp(err->setjmp_buffer, 1);
}

} // namespace

void encode_jpeg(const std::string& path,
                 const ImageBuffer& img,
                 const EncodeOptions& opts) {
    if (img.empty()) {
        throw std::runtime_error("Cannot encode empty image");
    }
    if (img.channels != 1 && img.channels != 3) {
        throw std::runtime_error("Encoder supports 1 or 3 channel images");
    }

    FILE* outfile = std::fopen(path.c_str(), "wb");
    if (!outfile) {
        throw std::runtime_error("Cannot open output file: " + path);
    }

    jpeg_compress_struct cinfo{};
    ErrorMgr jerr{};

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit_handler;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        std::fclose(outfile);
        throw std::runtime_error(std::string("JPEG encode error: ") + jerr.message);
    }

    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = static_cast<JDIMENSION>(img.width);
    cinfo.image_height = static_cast<JDIMENSION>(img.height);

    // Prepare grayscale buffer if needed
    std::vector<uint8_t> gray_buf;
    const uint8_t* src_data = img.data.data();
    int src_channels = img.channels;

    if (!opts.color && img.channels == 3) {
        // Convert RGB to grayscale for output
        gray_buf.resize(static_cast<size_t>(img.width) * img.height);
        for (size_t i = 0; i < gray_buf.size(); ++i) {
            const uint8_t* px = img.data.data() + i * 3;
            gray_buf[i] = static_cast<uint8_t>(
                0.299 * px[0] + 0.587 * px[1] + 0.114 * px[2]);
        }
        src_data = gray_buf.data();
        src_channels = 1;
    }

    cinfo.input_components = src_channels;
    cinfo.in_color_space = (src_channels == 1) ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, opts.quality, TRUE);

    if (opts.progressive) {
        jpeg_simple_progression(&cinfo);
    }

    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = img.width * src_channels;
    while (cinfo.next_scanline < cinfo.image_height) {
        auto* row = const_cast<uint8_t*>(
            src_data + cinfo.next_scanline * row_stride);
        jpeg_write_scanlines(&cinfo, &row, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    std::fclose(outfile);
}

} // namespace libjpeg_edu
