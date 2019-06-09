
#include "libpng_ifstream.h"

#ifndef PNG_HEADER_BYTES
#define PNG_HEADER_BYTES 8
#endif

void ReadData(png_structp png_ptr, png_bytep data, png_size_t length) {

        ((std::istream*)png_get_io_ptr(png_ptr))->read((char*)data, length);
}

char* LoadPng(const std::string& filename, int& width, int& height, int& channels) {

        png_bytep* row_ptrs = nullptr;
        char* data = nullptr;
        unsigned int stride;

        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) {
                return Error("Could not open file.");
        }

        // validate png header bytes
        png_byte header[PNG_HEADER_BYTES];
        in.read((char*) header, PNG_HEADER_BYTES);
        // 0 indicates successful validation
        if (png_sig_cmp(header, 0, PNG_HEADER_BYTES)) {
                return Error("PNG Header could not be validated.");
        }

        // create the png read struct
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr) {
                return Error("Couldn't create png read struct.");
        }

        // create png info struct
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
                // free up read struct
                png_destroy_read_struct(&png_ptr, nullptr, nullptr);
                return Error("Couldn't create png info struct.");
        }

        // point to the reader function, set header offset, and read info
        png_set_read_fn(png_ptr, (png_voidp) &in, ReadData);
        png_set_sig_bytes(png_ptr, PNG_HEADER_BYTES);
        png_read_info(png_ptr, info_ptr);

        png_uint_32 img_width = png_get_image_width(png_ptr, info_ptr);
        png_uint_32 img_height = png_get_image_height(png_ptr, info_ptr);
        png_uint_32 bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        png_uint_32 num_channels = png_get_channels(png_ptr, info_ptr);
        png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

        // set up rows and strides (width * bits * channels)
        row_ptrs = new png_bytep[img_height];
        stride = img_width * bit_depth * num_channels / PNG_HEADER_BYTES;
        data = new char[img_height * stride];

        // set row pointers
        for (size_t i = 0; i < img_height; i++) {
                png_uint_32 offset = i * stride;
                row_ptrs[i] = (png_bytep) data + offset;
        }

        // call the read function on 
        png_read_image(png_ptr, row_ptrs);

        // row pointers and read structs are no longer needed
        delete[] (png_bytep)row_ptrs;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) 0);

        // assign references and return buffer
        width = img_width;
        height = img_height;
        channels = num_channels;

        return data;
}

char* Error(const std::string& message) {
        std::cout << message << std::endl;
        return nullptr;
}

