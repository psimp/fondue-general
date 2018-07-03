#ifndef GEOPASSBASIC_H
#define GEOPASSBASIC_H

#include "deferredpass.h"

namespace protoengine { namespace graphics {


    struct GeoPassBasicInitUniforms
    {

    };

    struct GeoPassBasicRunUniforms
    {

    };

    class GeoPassBasic : public RenderPass<EMPTY, EMPTY>, public DeferredPass
    {

        GLuint gPosition, gAlbedoSpec, gNormal, gTangent, gBiTangent, gTSNormal;

    public:

        __attribute__((always_inline)) void init(const TextureArray& textures)
        {
            mShader->enable();
            mShader->setTextureUnit("textures", textures.getTID(), 2);
            mShader->disable();

            // position color buffer
            gPosition = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
            // color + specular color buffer
            gAlbedoSpec = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
            // normal color buffer
            gNormal = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
            // post-lighting color buffer
            gFinal = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA32F, GL_RGBA, GL_FLOAT);
            // tangent color buffer
            gTangent = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
            // bitangent color buffer
            gBiTangent = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
            // tangent space normals color buffer
            gTSNormal = mFrameBuffer.attachColorBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
            // Depth buffer
            gDepth = mFrameBuffer.attachDepthBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_FLOAT);

            mFrameBuffer.initAttachedBuffers();
            mFrameBuffer.checkCompleteness();

            bindFinalTexture(FBO_FINAL_BINDING, GL_RGBA32F);
        }

        __attribute__((always_inline)) void draw(const TextureArray& textures)
        {
            mFrameBuffer.bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            textures.bind();

            std::array<maths::mat4, MAX_GLOBAL_MESHES>& positions = dynamic_mesh_manager::getPositions();

            mShader->enable();

            glActiveTexture(GL_TEXTURE0 + FBO_FINAL_BINDING);
            glBindTexture(GL_TEXTURE_2D, gFinal);
            glActiveTexture(GL_TEXTURE0 + FBO_DEPTH_BINDING);
            glBindTexture(GL_TEXTURE_2D, gDepth);
            glActiveTexture(GL_TEXTURE0 + FBO_POSITION_BINDING);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE0 + FBO_ALBEDO_SPECULAR_BINDING);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glActiveTexture(GL_TEXTURE0 + FBO_NORMAL_BINDING);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE0 + FBO_TANGENT_BINDING);
            glBindTexture(GL_TEXTURE_2D, gTangent);
            glActiveTexture(GL_TEXTURE0 + FBO_BITANGENT_BINDING);
            glBindTexture(GL_TEXTURE_2D, gBiTangent);
            glActiveTexture(GL_TEXTURE0 + FBO_TSNORMAL_BINDING);
            glBindTexture(GL_TEXTURE_2D, gTSNormal);

            mShader->setUniformMat4("ProjectionView", Camera::current_camera->projectionView);
            for (TexturedBatch* batch : TexturedBatch::context_tex_batches)
            {
                batch->update_positions(positions);
                batch->draw();
            }

            Batch::unbind_all();

            textures.unbind();

            mShader->disable();

            mFrameBuffer.unbind();
        }

        GeoPassBasic()
        {
            makePipelineShader("src/shaders/geometryPass.vs", "src/shaders/geometryPass.fs");
        }

    };

} }

#endif // GEOPASSBASIC_H
