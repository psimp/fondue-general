#ifndef RAYMARCH_H
#define RAYMARCH_H

#include "renderpass.h"

namespace Fondue { namespace graphics {

#define HEMISPHERE_SAMPLES_BINDING 4

    class RayMarch : public RenderPass<EMPTY, EMPTY>
    {

        GLuint gVoxelMap, gAccumulator, captureFBO, captureRBO;
        GLuint mFrameQuadVAO = rprims.getQuad();

    public:

        __attribute__((always_inline)) void init()
        {
            int nx = 3, ny = 2;
            maths::vec2 samples[ny * nx];
            for (int x = 0; x < nx; x++)
            {
                float lon = ((x+0.5) / nx);
                for (int y = 0; y < ny; y++)
                {
                    float lat = asin(2*((y+0.5)/ny-0.5));

                    samples[y*x].x = lon;
                    samples[y*x].y = lat;
                }
            }

            GLuint sampleBuffer;
            glGenBuffers(1, &sampleBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, sampleBuffer);
            glBufferData(GL_UNIFORM_BUFFER, ny * nx * sizeof(maths::vec2), samples, GL_STATIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, HEMISPHERE_SAMPLES_BINDING, sampleBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glFinish();

            mShader->enable();
            mShader->setUniform1i("sdf", VOXEL_MAP);
            mShader->setUniform1i("sdfScene", SCENE_SDF);
            mShader->setUniform1i("sdfSceneHits", SCENE_HITTRACKER);
            mShader->setUniform1i("noisemap",  NOISE_MAP);
            mShader->setUniform1i("sceneGridSize", SCENE_GRID_SIZE);
            mShader->setUniformBlockBinding("BoundingBoxes", VIEWSPACE_BBOX_UBO_BINDING);
            mShader->setUniformBlockBinding("Orientations", VIEWSPACE_ORIENTATION_UBO_BINDING);
            mShader->setUniformBlockBinding("SampleHemisphere", HEMISPHERE_SAMPLES_BINDING);
            mShader->disable();

        }

        __attribute__((always_inline)) void draw(int cFrame)
        {
            mShader->enable();
            mShader->setUniformMat4("viewMatrix",       Camera::current_camera->getViewMatrix());
            mShader->setUniform3f("viewPos",            Camera::current_camera->getPosition());
//            mShader->setUniform3f("sunDir",             Camera::current_camera->getPosition());
            mShader->setUniform1i("iFrame",             cFrame);
            mShader->setUniform1i("numBoundingBoxes",   View::getNumBBoxes());

            glBindVertexArray(mFrameQuadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);

            mShader->disable();
        }

        RayMarch()
        {
            makePipelineShader("src/shaders/raymarch.vs", "src/shaders/raytest.fs");
        }

    };

} }

#endif // RAYMARCH_H
