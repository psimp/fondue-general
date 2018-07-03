#ifndef FORWARDPASS_H
#define FORWARDPASS_H

#include "renderpass.h"

namespace Fondue { namespace graphics {


    class ForwardPass : public RenderPass<EMPTY, EMPTY>
    {

    public:

        __attribute__((always_inline)) void init()
        {
            mShader->enable();

            mShader->setUniform1i("textures3C",                 TEX_ARRAY_C3);
            mShader->setUniform1i("textures1C",                 TEX_ARRAY_C1);
            mShader->setUniform1i("lightCullResults",           FBO_LIGHTS1);
            mShader->setUniform1i("irradianceMap",              CONVOLUTER_DIFFUSE_OUT);
            mShader->setUniform1i("prefilterMap",               CONVOLUTER_SPECULAR_OUT);
            mShader->setUniform1i("brdfLUT",                    BRDF_CONVOLUTION);

            mShader->setUniformMat4("projectionMatrix",   Camera::current_camera->getProjectionMatrix());

            mShader->disable();
        }

        __attribute__((always_inline)) void draw()
        {
            mShader->enable();

                mShader->setUniform1f("numLights",        float(dynamic_light_manager::gNumLights));
                mShader->setUniformMat4("viewMatrix",     Camera::current_camera->getViewMatrix());
                mShader->setUniform3f("viewPos",          Camera::current_camera->getPosition());
                mShader->setUniformMat4("ProjectionView", Camera::current_camera->getProjectionViewMatrix());

                for (TexturedBatch* batch : TexturedBatch::context_tex_batches)
                    batch->draw();

            mShader->disable();
        }

        ForwardPass()
        {
            makePipelineShader("src/shaders/forward.vs", "src/shaders/forward.fs");
        }

    };

} }

#endif // FORWARDPASS_H
