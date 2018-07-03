#ifndef LIGHTPASSUNTEXURED_H
#define LIGHTPASSUNTEXURED_H

#include "renderpass.h"

namespace protoengine { namespace graphics {

    struct LightPassUntexturedInitUniforms
    {

    };

    struct LightPassUntexturedRunUniforms
    {
        maths::mat4 viewMatrix;
        maths::vec4 viewPos_nLights;

        void make()
        {
            maths::vec3 viewPos = Camera::current_camera->getPosition();

            viewMatrix = Camera::current_camera->getViewMatrix();
            viewPos_nLights.x = viewPos.x;
            viewPos_nLights.y = viewPos.y;
            viewPos_nLights.z = viewPos.z;
            viewPos_nLights.w = dynamic_light_manager::gNumLights;
        }
    };

    class LightPassUntextured : RenderPass<EMPTY, LightPassUntexturedRunUniforms>
    {

    public:

        __attribute__((always_inline)) void init()
        {
            maths::mat4 projectionInverse;
            maths::mat4::M44TransformInverseSSE(projectionInverse, Camera::current_camera->projection);

            mShader->enable();
            mShader->setUniformMat4("projectionMatrix", Camera::current_camera->projection);
            mShader->setUniformMat4("projectionMatrixInverse", projectionInverse);
            mShader->setUniform1i("positionTexture", FBO_POSITION_BINDING_UT);
            mShader->setUniform1i("albedoTexture", FBO_ALBEDO_SPECULAR_BINDING_UT);
            mShader->setUniform1i("normalTexture", FBO_NORMAL_BINDING_UT);
            mShader->disable();
        }

        __attribute__((always_inline)) void draw()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mShader->RunUniformStruct->make();
            mShader->enable();
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            glDispatchCompute((SCREEN_WIDTH / MAX_WORK_GROUP_SIZE), (SCREEN_HEIGHT / MAX_WORK_GROUP_SIZE), 1);
            mShader->disable();

            glFinish();
        }

        LightPassUntextured()
        {
            makeComputeShader("src/shaders/tiledShadingUntextured.glsl");
        }

    };

} }

#endif // LIGHTPASSUNTEXURED_H
