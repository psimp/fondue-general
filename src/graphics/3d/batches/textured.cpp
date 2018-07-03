#include "textured.h"

namespace Fondue { namespace graphics {

    TexturedBatch::TexturedBatch(unsigned int renderableCapacity)
    {
        context_tex_batches.emplace_back(this);

        mInstanceCapacity = renderableCapacity;
        mSlot = occupied_mesh_slots;
        occupied_mesh_slots += renderableCapacity;

        glGenVertexArrays(1, &mVAO);
    }

    TexturedBatch::~TexturedBatch()
    {
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mIBO);
    }

    void TexturedBatch::fill_with(std::vector<float> &vertexData, std::vector<unsigned int>& indices, const maths::vec3& bounding, const unsigned int numVertices)
    {
        if (mTotalNumIndices != 0)
            for (unsigned int& mi : indices)
                mi += mTotalNumIndices;

        TEMP_vertex_data.insert(TEMP_vertex_data.end(), vertexData.begin(), vertexData.end());
        TEMP_index_data.insert(TEMP_index_data.end(), indices.begin(), indices.end());
        m_bbox_data.emplace_back( maths::vec4(bounding.x, bounding.y, bounding.z, 1.0f) );

        mTotalNumIndices += indices.size();
        mTotalNumVertices += numVertices;
    }

    void TexturedBatch::add_cube()
    {
        auto indices = TexturedBatchPrimitives::cubeIndices;
        if (mTotalNumIndices != 0)
            for (unsigned int& mi : indices)
                mi += mTotalNumIndices;

        auto vertices = TexturedBatchPrimitives::cubeVertices();
        TEMP_vertex_data.insert(TEMP_vertex_data.end(), vertices.begin(), vertices.end());
        TEMP_index_data.insert(TEMP_index_data.end(), indices.begin(), indices.end());
        m_bbox_data.push_back( {BOUNDARY_MAX_DIM,BOUNDARY_MAX_DIM,BOUNDARY_MAX_DIM, 1.0f} );

        mTotalNumIndices += 36;
        mTotalNumVertices += 36;
    }

    void TexturedBatch::add_plane()
    {
        auto indices = TexturedBatchPrimitives::quadIndices;
        if (mTotalNumIndices != 0)
            for (unsigned int& mi : indices)
                mi += mTotalNumIndices;

        auto vertices = TexturedBatchPrimitives::quadVertices();
        TEMP_vertex_data.insert(TEMP_vertex_data.end(), vertices.begin(), vertices.end());
        TEMP_index_data.insert(TEMP_index_data.end(), indices.begin(), indices.end());
        m_bbox_data.push_back( {BOUNDARY_MAX_DIM,1,BOUNDARY_MAX_DIM, 1.0f} );

        mTotalNumIndices += 6;
        mTotalNumVertices += 6;
    }

    void TexturedBatch::init_main_vbo()
    {
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        //glBufferStorage(GL_ARRAY_BUFFER, TEMP_vertex_data.size() * sizeof(float), TEMP_vertex_data.data(), 0);
        glBufferData(GL_ARRAY_BUFFER, TEMP_vertex_data.size() * sizeof(float), TEMP_vertex_data.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(  VERTEX_ATTRIB_INDEX,              3, GL_FLOAT,          GL_FALSE,  VBO_STRIDE_TEXTURED, (void*)(0 * sizeof(float)) );
        glVertexAttribPointer(  NORMALS_ATTRIB_INDEX,             3, GL_FLOAT,          GL_TRUE,   VBO_STRIDE_TEXTURED, (void*)(3 * sizeof(float)) );
        glVertexAttribPointer(  T_LAYERS_ATTRIB_INDEX1,           4, GL_UNSIGNED_BYTE,  GL_FALSE,  VBO_STRIDE_TEXTURED, (void*)(6 * sizeof(float)) );
        glVertexAttribPointer(  T_LAYERS_ATTRIB_INDEX2,           4, GL_UNSIGNED_BYTE,  GL_FALSE,  VBO_STRIDE_TEXTURED, (void*)(7 * sizeof(float)) );
        glVertexAttribPointer(  TEXTURE_ATTRIB_INDEX,             2, GL_FLOAT,          GL_FALSE,  VBO_STRIDE_TEXTURED, (void*)(8 * sizeof(float)) );
        //glVertexAttribPointer(  TANGENT_ATTRIB_INDEX,             3, GL_FLOAT,          GL_FALSE,  VBO_STRIDE_TEXTURED, (void*)(10 * sizeof(float)) );

        glEnableVertexAttribArray(VERTEX_ATTRIB_INDEX);
        glEnableVertexAttribArray(NORMALS_ATTRIB_INDEX);
        glEnableVertexAttribArray(TEXTURE_ATTRIB_INDEX);
        glEnableVertexAttribArray(T_LAYERS_ATTRIB_INDEX1);
        glEnableVertexAttribArray(T_LAYERS_ATTRIB_INDEX2);
        //glEnableVertexAttribArray(TANGENT_ATTRIB_INDEX);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        TEMP_vertex_data.clear();
        TEMP_vertex_data.shrink_to_fit();

        drawParameters.emplace_back(DrawElementsIndirectCommand{mTotalNumIndices, mNumMeshes, 0, 0, 0});
    }

    mesh_s TexturedBatch::add_instance()
    {
        if (mNumMeshes < mInstanceCapacity)
        {
            int cNumMeshes = mSlot + mNumMeshes;
            mNumMeshes++;
            return cNumMeshes;
        }

        throw std::runtime_error("Mesh batch over capacity!");
    }

} }
