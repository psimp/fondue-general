#include "texture.h"

namespace Fondue { namespace graphics {

    Texture::Texture(const BYTE* data, size_t width, size_t height, GLuint binding)
        : mWidth(width), mHeight(height), mBinding(binding)
    {
        glActiveTexture(GL_TEXTURE0 + mBinding);
        glGenTextures(1, &mTID);
        glBindTexture(GL_TEXTURE_2D, mTID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Texture::Texture(const std::string& filename, GLuint binding)
        : mBinding(binding)
    {
        FREE_IMAGE_FORMAT format;
        BYTE* pixels = import_image(filename.c_str(), mWidth, mHeight, &format);

        glActiveTexture(GL_TEXTURE0 + mBinding);
        glGenTextures(1, &mTID);
        glBindTexture(GL_TEXTURE_2D, mTID);

        if(format == FIF_JPEG) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        else if(format == FIF_PNG) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        else if(format == FIF_HDR) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, pixels);
        else throw std::runtime_error("Image format not recognized!");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Texture::~Texture()
    {

    }

    void Texture::bind(GLuint binding)
    {
        mBinding = binding;
        glActiveTexture(GL_TEXTURE0 + mBinding);
        glBindTexture(GL_TEXTURE_2D, mTID);
    }

    void Texture::unbind() const
    {
        glActiveTexture(GL_TEXTURE0 + mBinding);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::save_binaries(const std::string& fileOut) const
    {
        size_t size = 3*4*mWidth*mHeight;

        BYTE* pixels = new BYTE[size];

        glActiveTexture(GL_TEXTURE0 + mBinding);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);
        glActiveTexture(GL_TEXTURE0);

        FILE* fpd = fopen((fileOut + ".bin").c_str(), "wb");
        fwrite(pixels, size, 1, fpd);
        fclose(fpd);

        FILE* fpi = fopen((fileOut + ".binf").c_str(), "wb");
        size_t info2[3]= {mWidth, mHeight, size};
        fwrite(info2, 3 * sizeof(size_t), 1, fpi);
        fclose(fpi);

        delete []pixels;
    }

} }
