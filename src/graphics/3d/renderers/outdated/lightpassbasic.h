#ifndef LIGHTPASSBASIC_H
#define LIGHTPASSBASIC_H

#include "lightpassuntextured.h"

namespace protoengine { namespace graphics {

    class LightPassBasic : RenderPass<EMPTY, EMPTY>
    {

    public:

        __attribute__((always_inline)) void init()
        {
            maths::mat4 projectionInverse;
            maths::mat4::M44TransformInverseSSE(projectionInverse, Camera::current_camera->projection);

            mShader->enable();
            mShader->setUniformMat4("projectionMatrix", Camera::current_camera->projection);
            mShader->setUniformMat4("projectionMatrixInverse", projectionInverse);
            mShader->setUniform1i("positionTexture", FBO_POSITION_BINDING);
            mShader->setUniform1i("albedoTexture", FBO_ALBEDO_SPECULAR_BINDING);
            mShader->setUniform1i("normalTexture", FBO_NORMAL_BINDING);
            mShader->setUniform1i("tangentTexture", FBO_TANGENT_BINDING);
            mShader->setUniform1i("biTangentTexture", FBO_BITANGENT_BINDING);
            mShader->setUniform1i("TSNormalTexture", FBO_TSNORMAL_BINDING);
            mShader->disable();
        }

        __attribute__((always_inline)) void draw()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mShader->enable();
            mShader->setUniform1f("numLights", dynamic_light_manager::gNumLights);
            mShader->setUniformMat4("viewMatrix", Camera::current_camera->getViewMatrix());
            mShader->setUniform3f("viewPos", Camera::current_camera->getPosition());

            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            glDispatchCompute((SCREEN_WIDTH / MAX_WORK_GROUP_SIZE), (SCREEN_HEIGHT / MAX_WORK_GROUP_SIZE), 1);
            mShader->disable();

            glFinish();
        }

        LightPassBasic()
        {
            makeComputeShader("src/shaders/tiledShading.glsl");
        }

    };

} }

#endif // LIGHTPASSBASIC_H
