#ifndef TEXTUREARRAY_H
#define TEXTUREARRAY_H

#include <string>
#include <glad/glad.h>
#include <unordered_map>

#include <FreeImage/FreeImage.h>
#include "framebuffer.h"

#include "../../utils/imageloader.h"

typedef unsigned char TextureLayer;

namespace Fondue {	namespace graphics {

    const std::string path_not_found = "res/models/NULL";

    class TextureArray
    {

    private:

        GLuint mTID;
        GLuint mBinding;
        GLenum mFormat;
        GLint mInternalFormat;

        std::unordered_map<std::string, TextureLayer> mFileNames;
        const unsigned int mMaxLayers = 10;
        unsigned int mLayers = 0;
        const int mWidth, mHeight;

    public:

        TextureArray(int width, int height, GLint internalFormat, GLenum format, GLuint binding);
        ~TextureArray();
        void generateMipMaps() const;
        void bind(GLuint binding);
        void unbind() const;

        void attachLayerToFBO(FrameBuffer fbo, unsigned int layer) const;

    public:

        TextureLayer addToBack(std::string filename);

        inline GLuint getTID() const { return mTID; }

    };

}}


#endif // TEXTUREARRAY_H
