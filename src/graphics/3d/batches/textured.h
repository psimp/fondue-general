#ifndef INSTANCED_H
#define INSTANCED_H

#include "batch.h"
#include "bprimitives.h"

namespace Fondue { namespace graphics {

#define VBO_STRIDE_TEXTURED     10 * sizeof(float)
#define TEXTURED_BUFFER_SIZE    VBO_STRIDE_TEXTURED * MAX_VERTICES

    class TexturedBatch : public Batch
    {

    public:

        static inline std::vector<TexturedBatch *> context_tex_batches;

        TexturedBatch(unsigned int renderableCapacity);
        ~TexturedBatch();

        void fill_with(std::vector<float>& vertexData, std::vector<unsigned int>& indices, const maths::vec3& bounding, unsigned int numVertices) override;
        void add_cube();
        void add_plane();

    public:

        void init_main_vbo() override;

    public:

        mesh_s add_instance();

    };

} }

#endif // INSTANCED_H
