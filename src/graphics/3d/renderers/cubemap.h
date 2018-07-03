#ifndef CUBEMAPPER_H
#define CUBEMAPPER_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

#define CUBEMAP_MIPS 4

    class CubeMapper : public RenderPass<EMPTY, EMPTY>
    {

        bool mipMapSampler, roughnessFactor;

        unsigned short mSize;

        GLuint mCubeVAO = rprims.getCube();

        GLuint cubeFBO, cubeRBO, cubeMapOut;

        maths::mat4 captureProjection = maths::mat4::perspective(90.0f, 1.0f, 0.1f, 10.0f);
        maths::mat4 captureViewProjection[6] =
        {
           maths::mat4::lookTowards({0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f, 0.0f} ) * captureProjection,
           maths::mat4::lookTowards({0.0f, -1.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f, 0.0f} ) * captureProjection,
           maths::mat4::lookTowards({0.0f,  0.0f,  -1.0f}, { 0.0f,  -1.0f,  0.0f}, {0.0f, 0.0f, 0.0f} ) * captureProjection,
           maths::mat4::lookTowards({0.0f,  0.0f, 1.0f}, { 0.0f, 1.0f,  0.0f}, {0.0f, 0.0f, 0.0f} ) * captureProjection,
           maths::mat4::lookTowards({0.0f, -1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f, 0.0f} ) * captureProjection,
           maths::mat4::lookTowards({0.0f, -1.0f,  0.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f, 0.0f} ) * captureProjection
        };

    public:

        __attribute__((always_inline)) GLuint init(GLint inputTexBinding)
        {
            mShader->enable();
            mShader->setUniform1i("envMap", inputTexBinding);
            mShader->disable();

            glGenFramebuffers(1, &cubeFBO);
            glGenRenderbuffers(1, &cubeRBO);

            glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
            glBindRenderbuffer(GL_RENDERBUFFER, cubeRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mSize, mSize);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cubeRBO);

            glGenTextures(1, &cubeMapOut);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapOut);
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                             mSize, mSize, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (roughnessFactor || mipMapSampler) glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            else glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            if (roughnessFactor) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return cubeMapOut;
        }

        __attribute__((always_inline)) void generate_maps(const std::string& binhandle)
        {
            size_t memsize = 3 * mSize * mSize;

            glDisable(GL_CULL_FACE);
            mShader->enable();

            glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);
            if (!roughnessFactor)
            {
                glViewport(0, 0, mSize, mSize);
                for (unsigned int i = 0; i < 6; ++i)
                {
                    mShader->setUniformMat4("PV", captureViewProjection[i]);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapOut, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glBindVertexArray(mCubeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    glBindVertexArray(0);
                }

                glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapOut);
                if (mipMapSampler)
                    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
            else {
                for (unsigned int mip = 0; mip < CUBEMAP_MIPS; ++mip)
                {
                    // reisze framebuffer according to mip-level size.
                    int mipWidth  = mSize * pow(0.5, mip);
                    int mipHeight = mSize * pow(0.5, mip);
                    glBindRenderbuffer(GL_RENDERBUFFER, cubeRBO);
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
                    glViewport(0, 0, mipWidth, mipHeight);

                    float roughness = (float)mip / (float)(CUBEMAP_MIPS - 1);
                    mShader->setUniform1f("roughness", roughness);
                    for (unsigned int i = 0; i < 6; ++i)
                    {
                        mShader->setUniformMat4("PV", captureViewProjection[i]);
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapOut, mip);

                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        glBindVertexArray(mCubeVAO);
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        glBindVertexArray(0);
                    }
                }
            }

            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            mShader->disable();
            glEnable(GL_CULL_FACE);

            FILE* fpi = fopen(("res/binaries/env/" + binhandle + ".binf").c_str(), "wb");
            size_t info2[3]= {mSize, mSize, memsize};
            fwrite(info2, 3 * sizeof(size_t), 1, fpi);
            fclose(fpi);

            if (!roughnessFactor)
            {
                for (unsigned int i = 0; i < 6; ++i)
                {
                    uint16_t* pixelsf16 = new uint16_t[memsize];

                    glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, GL_HALF_FLOAT, pixelsf16);

                    FILE* fpd = fopen(("res/binaries/env/" + binhandle + "_" + std::to_string(i) + ".bin").c_str(), "wb");
                    fwrite(pixelsf16, memsize * sizeof(uint16_t), 1, fpd);
                    fclose(fpd);

                    delete []pixelsf16;
                }
            }
            else
            {
                for (unsigned int mip = 0; mip < CUBEMAP_MIPS; ++mip)
                {
                    int mipWidth  = mSize * pow(0.5, mip);
                    int mipHeight = mSize * pow(0.5, mip);
                    int memsizemip = 3 * mipWidth * mipHeight;
                    for (unsigned int i = 0; i < 6; ++i)
                    {
                        uint16_t* pixelsf16 = new uint16_t[memsizemip];

                        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB, GL_HALF_FLOAT, pixelsf16);

                        FILE* fpd = fopen(("res/binaries/env/" + binhandle + "_" + std::to_string(i) + "_" + std::to_string(mip) + ".bin").c_str(), "wb");
                        fwrite(pixelsf16, memsizemip * sizeof(uint16_t), 1, fpd);
                        fclose(fpd);

                        delete []pixelsf16;
                    }
                }
            }
        }

        CubeMapper(const char* s1, const char* s2, unsigned short size, bool useMipMapSampler, bool roughnessFactor)
            : mSize(size), mipMapSampler(useMipMapSampler), roughnessFactor(roughnessFactor)
        {
            makePipelineShader(s1, s2);
        }

    };

} }


#endif // CUBEMAPPER_H
