#ifndef DEPTHPASS_H
#define DEPTHPASS_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

    class DepthPass : public RenderPass<EMPTY, EMPTY>
    {

        unsigned int depthMapFBO, captureRBO, depthMap;
        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    public:

        __attribute__((always_inline)) GLuint init()
        {
             glGenFramebuffers(1, &depthMapFBO);

             glGenTextures(1, &depthMap);
             glBindTexture(GL_TEXTURE_2D, depthMap);
             glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
             float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
             glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

             glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
             glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
             glDrawBuffer(GL_NONE);
             glReadBuffer(GL_NONE);
             glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return depthMap;
        }

        __attribute__((always_inline)) void draw(const maths::mat4& lightViewProj)
        {
            mShader->enable();
            mShader->setUniformMat4("PV", lightViewProj);

            glCullFace(GL_FRONT);
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

                glClear(GL_DEPTH_BUFFER_BIT);

                for (TexturedBatch* batch : TexturedBatch::context_tex_batches)
                    batch->draw();
                Batch::unbind_all();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            glCullFace(GL_BACK);

            mShader->disable();

        }

        DepthPass()
        {
            makePipelineShader("src/shaders/depth.vs", "src/shaders/depth.fs");
        }

    };

} }

#endif // DEPTHPASS_H
