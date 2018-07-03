#ifndef QUADPASS_H
#define QUADPASS_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

    class RenderToScreen : public RenderPass<EMPTY, EMPTY>
    {

    GLuint mFrameQuadVAO = rprims.getQuad();

    public:

        __attribute__((always_inline)) void init(GLuint displayTexture)
        {
            mShader->enable();
            mShader->setUniform1i("image", displayTexture);
            mShader->disable();
        }

        __attribute__((always_inline)) void draw(int cFrame)
        {
            mShader->enable();
            mShader->setUniform1i("iFrame",             cFrame);

                glBindVertexArray(mFrameQuadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);

            mShader->disable();
        }

        RenderToScreen(const char* fragmentShader)
        {
            makePipelineShader("src/shaders/quad.vs", fragmentShader);
        }

    };

} }


#endif // QUADPASS_H
