#ifndef BATCH_H
#define BATCH_H

#include <glad/glad.h>
#include <vector>
#include <iostream>

#include "../../../maths/maths.h"
#include "../../shader.h"
#include "../rasterizables/meshmanager.h"

// This class is responsible for allocating opengl storage and producing valid mesh slots for use
//  in the mesh manager

namespace Fondue { namespace graphics {

#define NULL_TEXTURE 255    // Indicates that T_LAYERS will be interpreted as an rgb value

#define MAX_BATCH_SIZE			1000
#define MAX_OBJ_VERTICES		100000
#define MAX_OBJ_INDICES 		100000
#define MAX_VERTICES            MAX_BATCH_SIZE * MAX_OBJ_VERTICES
#define INDICES_SIZE			MAX_BATCH_SIZE * MAX_OBJ_INDICES * sizeof(unsigned int)

#define VERTEX_ATTRIB_INDEX                 0
#define NORMALS_ATTRIB_INDEX                1
#define T_LAYERS_ATTRIB_INDEX1              2
#define T_LAYERS_ATTRIB_INDEX2              3
#define TEXTURE_ATTRIB_INDEX                4
#define TANGENT_ATTRIB_INDEX                5
#define MODEL_MATRIX_ATTRIB_INDEX0          6

static inline const unsigned int BOUNDARY_MAX_DIM = 256;

    class Batch
    {

    public:

        // Pre-render methods

        Batch() { }
        virtual ~Batch() { }

        virtual void fill_with(std::vector<float>& vertexData, std::vector<unsigned int>& indices, const maths::vec3& bounding, const unsigned int numVertices) = 0;

        virtual void init_main_vbo() = 0;

        void __attribute__((always_inline)) init_ibo()
        {
            glGenBuffers(1, &mIBO);

            glBindVertexArray(mVAO);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
            //glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, TEMP_index_data.size() * sizeof(unsigned int), TEMP_index_data.data(), 0);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, TEMP_index_data.size() * sizeof(unsigned int), TEMP_index_data.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            glBindVertexArray(0);

            TEMP_index_data.clear();
            TEMP_index_data.shrink_to_fit();
        }

        void __attribute__((always_inline)) init_matrix_vbo()
        {                    
            glGenBuffers(1, &mMatricesVBO);

            glBindVertexArray(mVAO);
            glBindBuffer(GL_ARRAY_BUFFER, mMatricesVBO);

            glBufferData(GL_ARRAY_BUFFER, mNumMeshes * 16 * sizeof(float), NULL, GL_STATIC_DRAW);

            glVertexAttribPointer(MODEL_MATRIX_ATTRIB_INDEX0 + 0, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(0 * sizeof(float)));
            glVertexAttribPointer(MODEL_MATRIX_ATTRIB_INDEX0 + 1, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));
            glVertexAttribPointer(MODEL_MATRIX_ATTRIB_INDEX0 + 2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
            glVertexAttribPointer(MODEL_MATRIX_ATTRIB_INDEX0 + 3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));

            glEnableVertexAttribArray(MODEL_MATRIX_ATTRIB_INDEX0 + 0);
            glEnableVertexAttribArray(MODEL_MATRIX_ATTRIB_INDEX0 + 1);
            glEnableVertexAttribArray(MODEL_MATRIX_ATTRIB_INDEX0 + 2);
            glEnableVertexAttribArray(MODEL_MATRIX_ATTRIB_INDEX0 + 3);

            glVertexAttribDivisor(MODEL_MATRIX_ATTRIB_INDEX0 + 0, 1);
            glVertexAttribDivisor(MODEL_MATRIX_ATTRIB_INDEX0 + 1, 1);
            glVertexAttribDivisor(MODEL_MATRIX_ATTRIB_INDEX0 + 2, 1);
            glVertexAttribDivisor(MODEL_MATRIX_ATTRIB_INDEX0 + 3, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

    public:

        // Render-time methods

        const std::vector<maths::vec4>& getBoundingBoxes()
        {
            return m_bbox_data;
        }

        const auto getOrientationMatricesMem()
        {
            return std::tuple(&entity_manager::getPositions()[mSlot], mNumMeshes);
        }

        void  draw()
        {
            glBindVertexArray(mVAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
            glDrawElementsInstanced(GL_TRIANGLES, mTotalNumIndices, GL_UNSIGNED_INT, NULL, mNumMeshes);
        }

        void __attribute__((always_inline)) update_positions()
        {
            glBindBuffer(GL_ARRAY_BUFFER, mMatricesVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, mNumMeshes * 16 * sizeof(float), &entity_manager::getPositions()[mSlot]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        static void __attribute__((always_inline)) unbind_all()
        {
            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        static void __attribute__((always_inline)) draw_all()
        {
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, drawParameters.data(), drawParameters.size(), 0);
        }

        GLuint getBBoxUBO() const           { return mBBoxUBO;            }
        unsigned int getNumIndices() const  { return mTotalNumIndices;    }
        unsigned int getNumMeshes() const   { return mNumMeshes;          }
        unsigned int getMeshIndex() const   { return occupied_mesh_slots; }

    protected:

        typedef struct
        {
            uint  count;
            uint  instanceCount;
            uint  firstIndex;
            uint  baseVertex;
            uint  baseInstance;
        } DrawElementsIndirectCommand;

        static inline std::vector<DrawElementsIndirectCommand> drawParameters;

        static inline unsigned int occupied_mesh_slots = 0;

        GLuint mVBO, mVAO, mIBO, mMatricesVBO, mBBoxUBO;

        unsigned int mInstanceCapacity = 0, mTotalNumIndices = 0, mTotalNumVertices = 0;

        mesh_s mSlot = 0, mNumMeshes = 0;

        std::vector<float> TEMP_vertex_data;
        std::vector<unsigned int> TEMP_index_data;
        std::vector<maths::vec4> m_bbox_data;

    private:

        virtual void make_VAO() { }

    };

} }

#endif //BATCH_H
