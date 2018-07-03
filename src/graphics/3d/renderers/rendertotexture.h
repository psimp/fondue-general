#ifndef RENDERTOTEXTURE_H
#define RENDERTOTEXTURE_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

    class TextureRenderer : public RenderPass<EMPTY, EMPTY>
    {

        int mSize;

        GLuint mFrameQuadVAO = rprims.getQuad();
        GLuint outputTexture;

        GLuint captureFBO, captureRBO;

    public:

        __attribute__((always_inline)) GLuint init()
        {
            glGenTextures(1, &outputTexture);
            glBindTexture(GL_TEXTURE_2D, outputTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, mSize, mSize, 0, GL_RG, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenFramebuffers(1, &captureFBO);
            glGenRenderbuffers(1, &captureRBO);
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mSize, mSize);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return outputTexture;
        }

        __attribute__((always_inline)) void generate_tex(const std::string& binhandle)
        {
            mShader->enable();

            glViewport(0, 0, mSize, mSize);
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glBindVertexArray(mFrameQuadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

            mShader->disable();
            if (binhandle == "\0") return;

            size_t size = 2*mSize*mSize;

            uint16_t* pixels = new uint16_t[size];

            glBindTexture(GL_TEXTURE_2D, outputTexture);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_HALF_FLOAT, pixels);

            FILE* fpd = fopen(("res/binaries/env/" + binhandle + ".bin").c_str(), "wb");
            fwrite(pixels, size * sizeof(uint16_t), 1, fpd);
            fclose(fpd);

            FILE* fpi = fopen(("res/binaries/env/" + binhandle + ".binf").c_str(), "wb");
            size_t info2[3]= {mSize, mSize, size};
            fwrite(info2, 3 * sizeof(size_t), 1, fpi);
            fclose(fpi);

            delete []pixels;
        }

        TextureRenderer(const char* s, int size) : mSize(size)
        {
            makePipelineShader("src/shaders/textureRender.vs", s);
        }

    };

} }

#endif // RENDERTOTEXTURE_H
