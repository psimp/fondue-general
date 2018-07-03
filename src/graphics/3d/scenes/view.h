#pragma once

#include <vector>
#include <tuple>

#include "../batches/textured.h"

#include <glad/glad.h>

#include "../../../maths/maths.h"

namespace Fondue { namespace graphics {

// Responsible for viewspace culling / asserting viewspace random access resources
// Note: this does not necessarily mean these resources (meshes) are physically within the viewing frustum,
//       just that they contribute to the viewing image somehow (ie through a ray bounce)

#define VIEWSPACE_BBOX_UBO_BINDING          2
#define VIEWSPACE_ORIENTATION_UBO_BINDING   3

    class View
    {

        static inline GLuint mBBoxUBO, mOrientationsUBO;
        static inline uint mNumBBoxes;

    public :

        static int getNumBBoxes() { return mNumBBoxes; }

        static void init_viewspace_bbox_ubos()
        {
            glGenBuffers(1, &mBBoxUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, mBBoxUBO);
            glBufferData(GL_UNIFORM_BUFFER, MAX_VIEWSPACE_MESHES * sizeof(maths::vec4), NULL, GL_STATIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, VIEWSPACE_BBOX_UBO_BINDING, mBBoxUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glGenBuffers(1, &mOrientationsUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, mOrientationsUBO);
            glBufferData(GL_UNIFORM_BUFFER, MAX_VIEWSPACE_MESHES * 16 * sizeof(float), NULL, GL_STATIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, VIEWSPACE_ORIENTATION_UBO_BINDING, mOrientationsUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        static void updateViewSpace()
        {
            std::vector<maths::vec4> bounds;
            std::vector<maths::mat4> orients;

            for (auto batch : graphics::TexturedBatch::context_tex_batches)
            {
                auto[matrices, size] = batch->getOrientationMatricesMem();
                orients.insert(orients.begin(), &matrices[0], &matrices[size]);
                for (int i = 0; i < size; i++)
                    bounds.insert(bounds.end(), batch->getBoundingBoxes().begin(), batch->getBoundingBoxes().end());
            }

            glBindBuffer(GL_UNIFORM_BUFFER, mBBoxUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, bounds.size() * sizeof(maths::vec4), bounds.data());
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBuffer(GL_UNIFORM_BUFFER, mOrientationsUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, orients.size() * 16 * sizeof(float), orients.data());
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            mNumBBoxes = bounds.size();
        }

    private:

        View() { }

    };

} }
