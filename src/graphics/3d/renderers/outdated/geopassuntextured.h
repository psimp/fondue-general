#ifndef GEOPASSUNTEXTURED_H
#define GEOPASSUNTEXTURED_H

#include "renderpass.h"
#include "deferredpass.h"

namespace protoengine { namespace graphics {

    struct GeoPassUntexturedInitUniforms
    {

    };

    struct GeoPassUntexturedRunUniforms
    {
        maths::mat4 projectionView;
    };

    class GeoPassUntextured : public RenderPass<EMPTY, GeoPassUntexturedRunUniforms>, public DeferredPass
    {

        GLuint gPosition, gAlbedoSpec, gNormal;

    public:

        __attribute__((always_inline)) void init()
        {
            // position color buffer
            gPosition = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
            // color + specular color buffer
            gAlbedoSpec = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
            // normal color buffer
            gNormal = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
            // post-lighting color buffer
            gFinal = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT);
            // Depth buffer
            gDepth = mFrameBuffer.attachDepthBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_FLOAT);

            mFrameBuffer.initAttachedBuffers();
            mFrameBuffer.checkCompleteness();

            bindFinalTexture(FBO_FINAL_BINDING_UT, GL_RGBA16F);
        }

        __attribute__((always_inline)) void draw()
        {
            mFrameBuffer.bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            std::array<maths::mat4, MAX_GLOBAL_MESHES>& positions = dynamic_mesh_manager::getPositions();

            mShader->enable();

            glActiveTexture(GL_TEXTURE0 + FBO_FINAL_BINDING_UT);
            glBindTexture(GL_TEXTURE_2D, gFinal);
            glActiveTexture(GL_TEXTURE0 + FBO_DEPTH_BINDING_UT);
            glBindTexture(GL_TEXTURE_2D, gDepth);
            glActiveTexture(GL_TEXTURE0 + FBO_POSITION_BINDING_UT);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE0 + FBO_ALBEDO_SPECULAR_BINDING_UT);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glActiveTexture(GL_TEXTURE0 + FBO_NORMAL_BINDING_UT);
            glBindTexture(GL_TEXTURE_2D, gNormal);

            mShader->RunUniformStruct->projectionView = Camera::current_camera->projectionView;
            for (UntexturedBatch* batch : UntexturedBatch::context_untex_batches)
            {
                batch->update_positions(positions);
                batch->draw();
            }

            Batch::unbind_all();
            mShader->disable();

            mFrameBuffer.unbind();

            glFinish();
        }

        GeoPassUntextured()
        {
            makePipelineShader("src/shaders/geometryPassUntextured.vs", "src/shaders/geometryPassUntextured.fs");
        }

    };

} }

#endif // GEOPASSUNTEXTURED_H
