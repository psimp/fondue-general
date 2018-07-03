#ifndef TEXTUREARRAY_CPP
#define TEXTUREARRAY_CPP

#include "texturearray.h"

namespace Fondue {	namespace graphics {

    TextureArray::TextureArray(int width, int height, GLint internalFormat, GLenum format, GLuint binding)
        : mFormat(format), mInternalFormat(internalFormat), mWidth(width), mHeight(height), mBinding(binding)
    {
        glActiveTexture(GL_TEXTURE0 + mBinding);
        glGenTextures(1, &mTID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mTID);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, mWidth, mHeight, mMaxLayers);
        glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, internalFormat, mWidth, mHeight, mMaxLayers, 0, format, GL_UNSIGNED_BYTE, 0);
        glActiveTexture(GL_TEXTURE0);
    }

    TextureArray::~TextureArray() { }

    void TextureArray::bind(GLuint binding)
    {
        mBinding = binding;
        glActiveTexture(GL_TEXTURE0 + mBinding);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mTID);
    }

    void TextureArray::unbind() const
    {
        glActiveTexture(GL_TEXTURE0 + mBinding);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    void TextureArray::generateMipMaps() const
    {
        glActiveTexture(GL_TEXTURE0 + mBinding);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    TextureLayer TextureArray::addToBack(std::string filename)
    {
        if (filename == path_not_found)
            return 255;

        if (mLayers >= mMaxLayers)
            throw std::runtime_error(filename + " added to a full TextureArray!");

        std::unordered_map<std::string, TextureLayer>::const_iterator got = mFileNames.find(filename);
        if (got != mFileNames.end()) return mFileNames.at(filename);

        mFileNames.emplace(std::make_pair(filename, mLayers));
        size_t width, height;
        BYTE* pixels = import_image(filename.c_str(), width, height, nullptr, TRUE, mWidth, mHeight);

        glActiveTexture(GL_TEXTURE0 + mBinding);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, mLayers++, mWidth, mHeight, 1, mFormat, GL_UNSIGNED_BYTE, pixels);

        delete [] pixels;

        return mLayers - 1;
    }

} }


#endif // TEXTUREARRAY_CPP
