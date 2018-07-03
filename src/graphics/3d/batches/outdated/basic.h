#ifndef BASICBATCH_H
#define BASICBATCH_H

#include "batch.h"

namespace protoengine { namespace graphics {

#define VBO_STRIDE_BASIC        7 * sizeof(float)
#define BASIC_BUFFER_SIZE       VBO_STRIDE_BASIC * MAX_VERTICES

    class UntexturedBatch : public Batch
    {

    public:

        static inline std::vector<UntexturedBatch *> context_untex_batches;

        UntexturedBatch(unsigned int renderableCapacity);
        ~UntexturedBatch();

        slot_mesh add(std::vector<float>& vertexData, std::vector<unsigned int>& indices,
                      const unsigned int numVertices, const unsigned int subBatchStride) override;

    public:

        void init_main_vbo() override;

    public:

        slot_mesh addInstance();

    };

} }

#endif // BASICBATCH_H
