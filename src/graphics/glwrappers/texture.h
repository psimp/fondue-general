#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <string>

#include "FreeImage/FreeImage.h"
#include "../../utils/imageloader.h"

namespace Fondue { namespace graphics {

    class Texture
    {

    private:

        GLuint mTID, mBinding;
        size_t mWidth, mHeight;

    public:

        Texture(const BYTE* data, size_t width, size_t height, GLuint binding);
        Texture(const std::string& filename, GLuint binding);
        ~Texture();
        void bind(GLuint binding);
        void unbind() const;

        void save_binaries(const std::string &fileOut) const;

    public:

        inline GLuint getTID() const { return mTID; }

    };

} }

#endif // TEXTURE_H
