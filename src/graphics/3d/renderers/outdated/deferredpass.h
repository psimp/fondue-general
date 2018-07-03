#ifndef DEFERREDPASS_H
#define DEFERREDPASS_H

#include "renderpass.h"

namespace protoengine { namespace graphics {

    class DeferredPass
    {

    protected:

        FrameBuffer mFrameBuffer;
        GLuint gFinal, gDepth;
        GLuint mFinalBinding;

        void bindFinalTexture(GLuint binding, GLenum format)
        {
            mFinalBinding = binding;
            glBindImageTexture(binding, gFinal, 0, GL_FALSE, 0, GL_WRITE_ONLY, format);
        }

    public:

        GLuint getFinalBuffer()  { return gFinal; }
        GLuint getDepthBuffer()  { return gDepth; }

        GLuint getFinalBinding()  { return mFinalBinding; }

    };

} }


#endif // DEFERREDPASS_H
