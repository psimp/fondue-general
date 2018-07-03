#ifndef RPRIMITIVES_H
#define RPRIMITIVES_H

#include <glad/glad.h>

namespace Fondue { namespace graphics {

class primitives
{
    GLuint cubeVAO = UINT32_MAX;
    GLuint quadVAO = UINT32_MAX;

    void gen_cube_obj()
    {
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

        GLuint cubeVBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void gen_quad_obj()
    {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0001f,     0.0f, 1.0f,
            -1.0f, -1.0f, 0.0001f,     0.0f, 0.0f,
             1.0f,  1.0f, 0.0001f,     1.0f, 1.0f,
             1.0f, -1.0f, 0.0001f,     1.0f, 0.0f
        };

        GLuint frameQuadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &frameQuadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, frameQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }

public:

    primitives() { }

    auto getCube()
    {
        if(cubeVAO == UINT32_MAX) gen_cube_obj();
        return cubeVAO;
    }

    auto getQuad()
    {
        if(quadVAO == UINT32_MAX) gen_quad_obj();
        return quadVAO;
    }

};

} }

#endif // RPRIMITIVES_H
