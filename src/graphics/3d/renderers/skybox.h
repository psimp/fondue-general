#ifndef SKYBOX_H
#define SKYBOX_H

#include "renderpass.h"
#include "../../glwrappers/texture.h"

namespace Fondue { namespace graphics {


    class SkyBoxRenderer : public RenderPass<EMPTY, EMPTY>
    {

    GLuint cubeVAO;

    public:

        __attribute__((always_inline)) void init(GLint skyTexBinding)
        {
            GLuint cubeVBO;

            float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                // front face
                -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,
                // left face
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                // right face
                 1.0f,  1.0f,  1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f,
                // bottom face
                -1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                // top face
                -1.0f,  1.0f, -1.0f,
                 1.0f,  1.0f , 1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f
            };

            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);

            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            mShader->enable();
            mShader->setUniform1i("cubeMap", skyTexBinding);
            mShader->disable();
        }

        __attribute__((always_inline)) void draw()
        {
            glDisable(GL_CULL_FACE);

            mShader->enable();
            mShader->setUniformMat4("proj", Camera::current_camera->getProjectionMatrix());
            mShader->setUniformMat4("view", Camera::current_camera->getViewMatrix());

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            mShader->disable();

            glEnable(GL_CULL_FACE);
        }

        SkyBoxRenderer()
        {
            makePipelineShader("src/shaders/skybox.vs", "src/shaders/skybox.fs");
        }

    };

} }

#endif // SKYBOX_H
