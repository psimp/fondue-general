#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <glad/glad.h>

#include <string>

#include "FreeImage/FreeImage.h"
#include "../../utils/imageloader.h"

namespace Fondue { namespace graphics {

class CubeMap
{

private:

    GLuint mTID;
    size_t mWidth, mHeight;

public:

    CubeMap(bool mipmapped);
    CubeMap(std::string filename, bool mipmapped);
    ~CubeMap();
    void bind() const;
    void unbind() const;

    void save_binaries(const std::string &fileOut) const;

public:

    inline GLuint getTID() const { return mTID; }

};

} }

#endif // CUBEMAP_H
