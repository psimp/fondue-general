#ifndef RSMPASS_H
#define RSMPASS_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

    class RSMPass : public RenderPass<EMPTY, EMPTY>
    {

        FrameBuffer mFrameBuffer;
        GLuint gDepth, gFlux, gPosition, gNormal;
        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    public:

        __attribute__((always_inline)) void init()
        {
            gPosition = mFrameBuffer.attachColorBuffer(SHADOW_WIDTH, SHADOW_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT, true);
            gNormal = mFrameBuffer.attachColorBuffer(SHADOW_WIDTH, SHADOW_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT, true);
            gFlux = mFrameBuffer.attachColorBuffer(SHADOW_WIDTH, SHADOW_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT, true);
            gDepth = mFrameBuffer.attachDepthBuffer(SHADOW_WIDTH, SHADOW_HEIGHT, GL_FLOAT);

            glActiveTexture(GL_TEXTURE0 + FBO_LIGHT_POS_BUFFER);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE0 + FBO_LIGHT_NORM_BUFFER);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE0 + FBO_LIGHT_FLUX_BUFFER);
            glBindTexture(GL_TEXTURE_2D, gFlux);
            glActiveTexture(GL_TEXTURE0 + FBO_LIGHT_DEPTH_BUFFER);
            glBindTexture(GL_TEXTURE_2D, gDepth);

            mFrameBuffer.initAttachedBuffers();
            mFrameBuffer.checkCompleteness();

            mShader->enable();
            mShader->setUniform1i("textures", TEX_ARRAY_C3);
            mShader->disable();
        }

        __attribute__((always_inline)) void draw(const maths::mat4& lightViewProj)
        {
            mShader->enable();
            mShader->setUniformMat4("PV", lightViewProj);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            mFrameBuffer.bind();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                for (TexturedBatch* batch : TexturedBatch::context_tex_batches)
                    batch->draw();
                Batch::unbind_all();

            mFrameBuffer.unbind();
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

            mShader->disable();
        }

        RSMPass()
        {
            makePipelineShader("src/shaders/rsm.vs", "src/shaders/rsm.fs");
        }

    };

} }

#endif // RSMPASS_H
