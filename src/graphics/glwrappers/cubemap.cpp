#include "cubemap.h"

namespace Fondue { namespace graphics {

    CubeMap::CubeMap(bool mipmapped)
    {
        glGenTextures(1, &mTID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                         mWidth, mHeight, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        if (mipmapped) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    CubeMap::CubeMap(std::string filename, bool mipmapped)
    {
        glGenTextures(1, &mTID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            BYTE* pixels = import_image((filename + "_" + std::to_string(i)).c_str(), mWidth, mHeight);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                         mWidth, mHeight, 0, GL_RGB, GL_FLOAT, pixels);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        if (mipmapped) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    CubeMap::~CubeMap()
    {

    }

    void CubeMap::bind() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTID);
    }

    void CubeMap::unbind() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void CubeMap::save_binaries(const std::string& fileOut) const
    {
        size_t size = 3*4*mWidth*mHeight;

        for (unsigned int i = 0; i < 6; ++i)
        {
            BYTE* pixels = new BYTE[size];

            bind();
            glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, GL_FLOAT, pixels);
            unbind();

            FILE* fpd = fopen((fileOut + "_" + std::to_string(i) + ".bin").c_str(), "wb");
            fwrite(pixels, size, 1, fpd);
            fclose(fpd);

            FILE* fpi = fopen((fileOut + "_" + std::to_string(i) + ".binf").c_str(), "wb");
            size_t info2[3]= {mWidth, mHeight, size};
            fwrite(info2, 3 * sizeof(size_t), 1, fpi);
            fclose(fpi);

            delete pixels;
        }

    }

} }
