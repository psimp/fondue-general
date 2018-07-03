#pragma once

#include <FreeImage/FreeImage.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <fstream>

namespace Fondue{

    static std::string get_file_handle(const std::string& s)
    {
        auto start = s.find_last_of('/');
        auto end = s.find_last_of('\.');
        std::string last_word = s.substr(start + 1, end - start);
        return last_word;
    }

    static BYTE* import_image(const char* filename, size_t& width, size_t& height, FREE_IMAGE_FORMAT* format = nullptr,
                              bool rescale = false, int rWidth = 0, int rHeight = 0)
	{

        auto ts = std::string(filename);
        std::replace( ts.begin(), ts.end(), '\\', '/');
        filename = ts.c_str();

        // First check if binary file exists
        auto binfile = "res/binaries/" + get_file_handle(filename);

        FILE* infs = fopen((binfile + "binf").c_str(), "rb");
        if (infs != NULL) {
            size_t inf[3];
            fread(inf, 3 * sizeof(size_t), 1, infs);
            fclose(infs);

            width = inf[0];
            height = inf[1];
            auto size = inf[2];

            FILE* bins = fopen((binfile + "bin").c_str(), "rb");
            BYTE* bytes = new BYTE[size];
            fread(bytes, size, 1, bins);
            fclose(bins);

            return bytes;
        }

        // Else use FreeImage
        FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;
        FIBITMAP *image_bitmap = nullptr;

        image_format = FreeImage_GetFileType(filename, 0);
        if (image_format == FIF_UNKNOWN)
        {
            image_format = FreeImage_GetFIFFromFilename(filename);
            if (image_format == FIF_UNKNOWN)
                throw std::runtime_error(std::string("Image ") + filename + " format unknown!");
        }

        if (FreeImage_FIFSupportsReading(image_format))
            image_bitmap = FreeImage_Load(image_format, filename);
        if (!image_bitmap)
            throw std::runtime_error(std::string("Image ") + filename + " failed to load!");

        if (rescale && (rWidth / rHeight) % (FreeImage_GetWidth(image_bitmap) / FreeImage_GetHeight(image_bitmap)) != 0)
            std::cout << "TEXTURE: " << filename << " will appear streched. " << "STORED AS: "
                      << rWidth << "x" << rHeight << std::endl;

        if (rescale) image_bitmap = FreeImage_Rescale(image_bitmap, rWidth, rHeight, FILTER_BILINEAR);

        BYTE* pixels = FreeImage_GetBits(image_bitmap);
        width = FreeImage_GetWidth(image_bitmap);
        height = FreeImage_GetHeight(image_bitmap);

        unsigned num_bytes = FreeImage_GetBPP(image_bitmap) / 8;
        size_t size = width * height * num_bytes;

        if (format) *format = image_format;

        BYTE* bytes = new BYTE[size];
        std::memcpy(bytes, pixels, size);
        FreeImage_Unload(image_bitmap);
        return bytes;
	}

}
