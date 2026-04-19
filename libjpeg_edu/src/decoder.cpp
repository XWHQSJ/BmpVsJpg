#include "libjpeg_edu/decoder.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>

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

ImageBuffer decode_jpeg(const std::string& path) {
    FILE* infile = std::fopen(path.c_str(), "rb");
    if (!infile) {
        throw std::runtime_error("Cannot open JPEG file: " + path);
    }

    jpeg_decompress_struct cinfo{};
    ErrorMgr jerr{};

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit_handler;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        std::fclose(infile);
        throw std::runtime_error(std::string("JPEG decode error: ") + jerr.message);
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&cinfo);

    ImageBuffer img;
    img.width = static_cast<int>(cinfo.output_width);
    img.height = static_cast<int>(cinfo.output_height);
    img.channels = 3; // we force RGB output
    img.data.resize(static_cast<size_t>(img.width) * img.height * img.channels);

    int row_stride = img.width * img.channels;
    while (cinfo.output_scanline < cinfo.output_height) {
        uint8_t* row = img.data.data() + cinfo.output_scanline * row_stride;
        jpeg_read_scanlines(&cinfo, &row, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    std::fclose(infile);

    return img;
}

ImageInfo jpeg_info(const std::string& path) {
    FILE* infile = std::fopen(path.c_str(), "rb");
    if (!infile) {
        throw std::runtime_error("Cannot open JPEG file: " + path);
    }

    jpeg_decompress_struct cinfo{};
    ErrorMgr jerr{};

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit_handler;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        std::fclose(infile);
        throw std::runtime_error(std::string("JPEG info error: ") + jerr.message);
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    ImageInfo info;
    info.width = static_cast<int>(cinfo.image_width);
    info.height = static_cast<int>(cinfo.image_height);
    info.channels = cinfo.num_components;
    info.bit_depth = 8;
    info.format = "JPEG";

    // Get file size
    std::fseek(infile, 0, SEEK_END);
    info.file_size = static_cast<size_t>(std::ftell(infile));

    jpeg_destroy_decompress(&cinfo);
    std::fclose(infile);

    return info;
}

} // namespace libjpeg_edu
