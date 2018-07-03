#ifndef QUADPASS_H
#define QUADPASS_H

#include "geopassbasic.h"
#include "geopassuntextured.h"
#include "lightpassbasic.h"
#include "lightpassuntextured.h"

namespace protoengine { namespace graphics {

    struct QuadPassInitUniforms
    {

    };

    struct QuadPassRunUniforms
    {

    };

    class DeferredPassCombiner : public RenderPass<EMPTY, EMPTY>
    {

        GLuint mFrameQuadVAO;

    public:

        __attribute__((always_inline)) void init(GLuint displayTexture)
        {
            float quadVertices[] = {
                -1.0f,  1.0f, 0.0001f,     0.0f, 1.0f,
                -1.0f, -1.0f, 0.0001f,     0.0f, 0.0f,
                 1.0f,  1.0f, 0.0001f,     1.0f, 1.0f,
                 1.0f, -1.0f, 0.0001f,     1.0f, 0.0f
            };

            GLuint frameQuadVBO;
            glGenVertexArrays(1, &mFrameQuadVAO);
            glGenBuffers(1, &frameQuadVBO);
            glBindVertexArray(mFrameQuadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, frameQuadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            mShader->enable();
            mShader->setUniform1i("image", displayTexture);
            //mShader->setUniform1i("gFinal2", FBO_LIGHTS1);
            //mShader->setUniform1i("gDepth1", FBO_DEPTH_BINDING);
            //mShader->setUniform1i("gDepth2", FBO_DEPTH_BINDING_UT);
            mShader->disable();
        }

        __attribute__((always_inline)) void draw()
        {
            mShader->enable();
            glBindVertexArray(mFrameQuadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
            mShader->disable();
        }

        DeferredPassCombiner()
        {
            makePipelineShader("src/shaders/quad.vs", "src/shaders/quad.fs");
        }

    };

} }


#endif // QUADPASS_H
