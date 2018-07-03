#ifndef VOXELIZER_H
#define VOXELIZER_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

    struct c4f16
    {
        uint16_t x,y,z,w;
    };

    class Voxelizer : public RenderPass<EMPTY, EMPTY>
    {

        GLuint gPointBuffer, gPointMapTex, gVoxelMap, captureFBO, captureRBO, gTIDMapTex, gTIDBuffer, gIndexBuffer, gIndexMapTex, gHitMap;
        GLuint mFrameQuadVAO = rprims.getQuad();

    public:

        __attribute__((always_inline)) void genSceneBBoxSdf()
        {
            glActiveTexture(GL_TEXTURE0 + SCENE_SDF);
            glGenTextures(1, &gVoxelMap);
            glBindTexture(GL_TEXTURE_3D, gVoxelMap);
            glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, SCENE_GRID_SIZE, SCENE_GRID_SIZE, SCENE_GRID_SIZE, 0, GL_RED, GL_HALF_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glActiveTexture(GL_TEXTURE0 + SCENE_HITTRACKER);
            glGenTextures(1, &gHitMap);
            glBindTexture(GL_TEXTURE_3D, gHitMap);
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RG16F, SCENE_GRID_SIZE/8, SCENE_GRID_SIZE/8, SCENE_GRID_SIZE/8, 0, GL_RG, GL_HALF_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glGenFramebuffers(1, &captureFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, gVoxelMap, 0, 0);
            const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, buffers);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            mShader->enable();
            mShader->setUniformBlockBinding("BoundingBoxes", VIEWSPACE_BBOX_UBO_BINDING);
            mShader->setUniformBlockBinding("Orientations", VIEWSPACE_ORIENTATION_UBO_BINDING);
            mShader->setUniform1i("modelSdf",  VOXEL_MAP);
            mShader->setUniform1i("numBoundingBoxes",   View::getNumBBoxes());
            mShader->setUniform1i("hittracking",  0);
            mShader->setUniform1i("gridSize",  SCENE_GRID_SIZE);

            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glViewport(0, 0, SCENE_GRID_SIZE, SCENE_GRID_SIZE);
                glBindVertexArray(mFrameQuadVAO);

                for (unsigned int i = 0; i < SCENE_GRID_SIZE; ++i) {
                    mShader->setUniform1u("layer", i);
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gVoxelMap, 0, i);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                }

            mShader->setUniform1i("hittracking",  1);
            mShader->setUniform1i("gridSize",  SCENE_GRID_SIZE/8);

            glViewport(0, 0, SCENE_GRID_SIZE/8, SCENE_GRID_SIZE/8);

                for (unsigned int i = 0; i < SCENE_GRID_SIZE/8; ++i) {
                    mShader->setUniform1u("layer", i);
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gHitMap, 0, i);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                }

            glBindVertexArray(0);
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            mShader->disable();
        }

        __attribute__((always_inline)) void gensdf(const std::string& filename, std::vector<float> points, std::vector<uint> indices,
                                                     const std::vector<maths::vec3>& tids, const maths::vec3& bound,
                                                     const maths::vec3& shift, float scale)
        {
            const auto dims = ceil(bound);
            const size_t memsize = dims.x*dims.y*dims.z;

            glActiveTexture(GL_TEXTURE0 + VOXEL_MAP);
            glGenTextures(1, &gVoxelMap);
            glBindTexture(GL_TEXTURE_3D, gVoxelMap);

            FILE* fpd = fopen(("res/binaries/sdf/" + filename + ".bin").c_str(), "rb");
            if (fpd != nullptr)
            {
                c4f16* pixels = new c4f16[memsize];
                fread(pixels, sizeof(c4f16), memsize, fpd);
                glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, dims.x, dims.y, dims.z, 0, GL_RGBA, GL_HALF_FLOAT, pixels);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                fclose(fpd);
                delete [] pixels;
                return;
            }

            for (auto p = points.begin(); p < points.end(); p+=3) {
                *(p+0) *= scale;
                *(p+1) *= scale;
                *(p+2) *= scale;

                *(p+0) += shift.x;
                *(p+1) += shift.y;
                *(p+2) += shift.z;
            }

            glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, dims.x, dims.y, dims.z, 0, GL_RGBA, GL_HALF_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glGenBuffers(1, &gPointBuffer);
            glBindBuffer(GL_TEXTURE_BUFFER, gPointBuffer);
            glBufferData(GL_TEXTURE_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
            glGenTextures(1, &gPointMapTex);
            glActiveTexture(GL_TEXTURE0 + POINT_MAP);
            glBindTexture(GL_TEXTURE_BUFFER, gPointMapTex);
            glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, gPointBuffer);

            glGenBuffers(1, &gIndexBuffer);
            glBindBuffer(GL_TEXTURE_BUFFER, gIndexBuffer);
            glBufferData(GL_TEXTURE_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            glGenTextures(1, &gIndexMapTex);
            glActiveTexture(GL_TEXTURE0 + INDEX_MAP);
            glBindTexture(GL_TEXTURE_BUFFER, gIndexMapTex);
            glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, gIndexBuffer);

            glGenBuffers(1, &gTIDBuffer);
            glBindBuffer(GL_TEXTURE_BUFFER, gTIDBuffer);
            glBufferData(GL_TEXTURE_BUFFER, tids.size() * sizeof(maths::vec3), tids.data(), GL_STATIC_DRAW);
            glGenTextures(1, &gTIDMapTex);
            glActiveTexture(GL_TEXTURE0 + TID_MAP);
            glBindTexture(GL_TEXTURE_BUFFER, gTIDMapTex);
            glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, gTIDBuffer);

            glGenFramebuffers(1, &captureFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, gVoxelMap, 0, 0);
            const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, buffers);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            mShader->enable();
            mShader->setUniform1i("points", POINT_MAP);
            mShader->setUniform1i("tids", TID_MAP);
            mShader->setUniform1i("indices", INDEX_MAP);
            mShader->setUniform1i("gridSize", SCENE_GRID_SIZE);
            mShader->setUniform1i("numIndices", indices.size());

            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glViewport(0, 0, dims.x, dims.y);
                glBindVertexArray(mFrameQuadVAO);

                for (unsigned int i = 0; i < dims.z; ++i) {
                    mShader->setUniform1u("layer", i);
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gVoxelMap, 0, i);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                    if (i % 20 == 0) glFinish();
                }

                glBindVertexArray(0);
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            mShader->disable();

            glDeleteFramebuffers(1, &captureFBO);
            glDeleteBuffers(1, &gPointBuffer);

            c4f16* pixels = new c4f16[memsize];

            glActiveTexture(GL_TEXTURE0 + VOXEL_MAP);
            glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_HALF_FLOAT, pixels);

            FILE* fpd2 = fopen(("res/binaries/sdf/" + filename + ".bin").c_str(), "wb");
            fwrite(pixels, sizeof(c4f16), memsize, fpd2);
            fclose(fpd2);

            delete []pixels;

        }

        Voxelizer(const char* frag)
        {
            makePipelineShader("src/shaders/voxelizer.vs", frag);
        }


    };

} }

#endif // VOXELIZER_H
