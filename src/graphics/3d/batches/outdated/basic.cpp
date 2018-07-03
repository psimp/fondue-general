#include "basic.h"

namespace protoengine { namespace graphics {

    UntexturedBatch::UntexturedBatch(unsigned int renderableCapacity)
    {
        context_untex_batches.emplace_back(this);

        mRenderableCapacity = renderableCapacity;
        mSlot = occupied_mesh_slots;
        occupied_mesh_slots += renderableCapacity;
        mStrideDepth = 0;

        glGenVertexArrays(1, &mVAO);
    }

    UntexturedBatch::~UntexturedBatch()
    {
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mIBO);
    }

    slot_mesh UntexturedBatch::add(std::vector<float>& vertexData, std::vector<unsigned int>& indices, const unsigned int numVertices, const unsigned int subBatchStride)
    {
        if (mStrideDepth == 0) mStrideDepth = subBatchStride;

        // This fills the VAO with meshes to be instanced
        if (mNumMeshes < mRenderableCapacity)
        {
            int cNumMeshes = mSlot + mNumMeshes;

            // Populate the index array with the mesh's index data; Apply necessary offsets
            for (unsigned int& mi : indices)
                mi += mTotalNumIndices;

            TEMP_vertex_data.insert(TEMP_vertex_data.end(), vertexData.begin(), vertexData.end());
            TEMP_index_data.insert(TEMP_index_data.end(), indices.begin(), indices.end());

            mTotalNumIndices += indices.size();
            mTotalNumVertices += numVertices;

            if (mStrideDepth == 1)
                mNumMeshes++;

            if (mStrideDepth != 0)
                mStrideDepth--;
            else
                mNumMeshes++;

            return cNumMeshes;
        }

        return 0;
    }

    void UntexturedBatch::init_main_vbo()
    {
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        glBufferStorage(GL_ARRAY_BUFFER, TEMP_vertex_data.size() * sizeof(float), TEMP_vertex_data.data(), 0);

        glVertexAttribPointer(  VERTEX_ATTRIB_INDEX,              3, GL_FLOAT,          GL_FALSE,  VBO_STRIDE_BASIC, (void*)(0 * sizeof(float)) );
        glVertexAttribPointer(  NORMALS_ATTRIB_INDEX,             3, GL_FLOAT,          GL_TRUE,   VBO_STRIDE_BASIC, (void*)(3 * sizeof(float)) );
        glVertexAttribPointer(  T_LAYERS_ATTRIB_INDEX,            4, GL_UNSIGNED_BYTE,  GL_FALSE,  VBO_STRIDE_BASIC, (void*)(6 * sizeof(float)) );

        glEnableVertexAttribArray(VERTEX_ATTRIB_INDEX);
        glEnableVertexAttribArray(NORMALS_ATTRIB_INDEX);
        glEnableVertexAttribArray(T_LAYERS_ATTRIB_INDEX);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        TEMP_vertex_data.clear();
        TEMP_vertex_data.shrink_to_fit();

        drawParameters.emplace_back(DrawElementsIndirectCommand{mTotalNumIndices, mNumMeshes, 0, 0, 0});
    }

    slot_mesh UntexturedBatch::addInstance()
    {
        if (mNumMeshes < mRenderableCapacity)
        {
            int cNumMeshes = mSlot + mNumMeshes;
            mNumMeshes++;
            return cNumMeshes;
        }

        return 0;
    }

} }
