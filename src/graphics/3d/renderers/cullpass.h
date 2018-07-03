#ifndef CULLPASS_H
#define CULLPASS_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

    class CullPass : public RenderPass<EMPTY, EMPTY>
    {
        GLuint mFrameQuadVAO;
        FrameBuffer mFrameBuffer;
        GLuint gLights1_4, gLights5_8, gLights9_11_length, gDepth;

    public:

        __attribute__((always_inline)) void init()
        {

            float quadVertices[] = {
                -1.0f,  1.0f, 0.0001f,
                -1.0f, -1.0f, 0.0001f,
                 1.0f,  1.0f, 0.0001f,
                 1.0f, -1.0f, 0.0001f
            };

            GLuint frameQuadVBO;
            glGenVertexArrays(1, &mFrameQuadVAO);
            glGenBuffers(1, &frameQuadVBO);
            glBindVertexArray(mFrameQuadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, frameQuadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            gLights1_4 = mFrameBuffer.attachColorBuffer(16, 16, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
            gLights5_8 = mFrameBuffer.attachColorBuffer(16, 16, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
            gLights9_11_length = mFrameBuffer.attachColorBuffer(16, 16, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
            gDepth = mFrameBuffer.attachDepthBuffer(16, 16, GL_FLOAT);

            mFrameBuffer.initAttachedBuffers();
            mFrameBuffer.checkCompleteness();

            auto projectionInverse = maths::mat4::M44TransformInverseSSE(Camera::current_camera->getProjectionMatrix());

            mShader->enable();
            mShader->setUniformMat4("projectionMatrix", Camera::current_camera->getProjectionMatrix());
            mShader->setUniformMat4("projectionMatrixInverse", projectionInverse);
            mShader->disable();

            glActiveTexture(GL_TEXTURE0 + FBO_LIGHTS1);
            glBindTexture(GL_TEXTURE_2D, gLights1_4);
            glActiveTexture(GL_TEXTURE0 + FBO_LIGHTS2);
            glBindTexture(GL_TEXTURE_2D, gLights5_8);
            glActiveTexture(GL_TEXTURE0 + FBO_LIGHTS3);
            glBindTexture(GL_TEXTURE_2D, gLights9_11_length);
        }

        __attribute__((always_inline)) void draw()
        {
            mShader->enable();
            mShader->setUniform1i("numLights", int(dynamic_light_manager::gNumLights));
            mShader->setUniformMat4("viewMatrix", Camera::current_camera->getViewMatrix());

            mFrameBuffer.bind();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glBindVertexArray(mFrameQuadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);

            mFrameBuffer.unbind();

            mShader->disable();

            glFinish();
        }

        CullPass()
        {
            makePipelineShader("src/shaders/cullPass.vs", "src/shaders/cullPass.fs");
        }

    };

} }

#endif // CULLPASS_H
