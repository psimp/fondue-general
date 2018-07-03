#ifndef BPRIMITIVES_H
#define BPRIMITIVES_H

#include <array>

namespace Fondue { namespace graphics {

    namespace TexturedBatchPrimitives
    {
        union TexLayer4
        {
            float data;
            struct
            {
                unsigned char x, y, z, w;
            };
        };

        static constexpr std::array<float, 36 * 10> cubeVertices(float x = 0, float y = 0, float z = 0, float s = 1)
        {
            TexLayer4 nullLayer{};
            nullLayer.x = 255;
            nullLayer.y = 255;
            nullLayer.z = 255;
            nullLayer.w = 255;

            std::array<float, 36 * 10> ret =
            {
            -1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    0.0f,  0.0f, -1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // bottom-left
             1.0f *s + x,  1.0f *s+ y, -1.0f *s+ z,    0.0f,  0.0f, -1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // top-right
             1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    0.0f,  0.0f, -1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // bottom-right
             1.0f *s + x,  1.0f *s+ y, -1.0f *s+ z,    0.0f,  0.0f, -1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // top-right
            -1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    0.0f,  0.0f, -1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // bottom-left
            -1.0f *s + x,  1.0f *s+ y, -1.0f *s+ z,    0.0f,  0.0f, -1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // top-left

             -1.0f*s  + x, -1.0f*s + y, 1.0f *s+ z,    0.0f,  0.0f,  1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // bottom-left
             1.0f *s + x, -1.0f *s+ y,  1.0f *s+ z,    0.0f,  0.0f,  1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // bottom-right
             1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    0.0f,  0.0f,  1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // top-right
             1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    0.0f,  0.0f,  1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // top-right
            -1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    0.0f,  0.0f,  1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // top-left
            -1.0f *s + x, -1.0f *s+ y,  1.0f *s+ z,    0.0f,  0.0f,  1.0f,     nullLayer.data,    nullLayer.data,     0.0f,  0.0f, // bottom-left

             -1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,   -1.0f,  0.0f,  0.0f,    nullLayer.data,    nullLayer.data,    -1.0f,  0.0f, // top-right
            -1.0f *s + x,  1.0f *s+ y, -1.0f *s+ z,    -1.0f,  0.0f,  0.0f,    nullLayer.data,    nullLayer.data,    -1.0f,  0.0f, // top-left
            -1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    -1.0f,  0.0f,  0.0f,    nullLayer.data,    nullLayer.data,    -1.0f,  0.0f, // bottom-left
            -1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    -1.0f,  0.0f,  0.0f,    nullLayer.data,    nullLayer.data,    -1.0f,  0.0f, // bottom-left
            -1.0f *s + x, -1.0f *s+ y,  1.0f *s+ z,    -1.0f,  0.0f,  0.0f,    nullLayer.data,    nullLayer.data,    -1.0f,  0.0f, // bottom-right
            -1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    -1.0f,  0.0f,  0.0f,    nullLayer.data,    nullLayer.data,    -1.0f,  0.0f, // top-right

             1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    1.0f,  0.0f,  0.0f,     nullLayer.data,    nullLayer.data,     1.0f,  0.0f, // top-left
             1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    1.0f,  0.0f,  0.0f,     nullLayer.data,    nullLayer.data,     1.0f,  0.0f, // bottom-right
             1.0f *s + x,  1.0f *s+ y, -1.0f *s+ z,    1.0f,  0.0f,  0.0f,     nullLayer.data,    nullLayer.data,     1.0f,  0.0f, // top-right
             1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    1.0f,  0.0f,  0.0f,     nullLayer.data,    nullLayer.data,     1.0f,  0.0f, // bottom-right
             1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    1.0f,  0.0f,  0.0f,     nullLayer.data,    nullLayer.data,     1.0f,  0.0f, // top-left
             1.0f *s + x, -1.0f *s+ y,  1.0f *s+ z,    1.0f,  0.0f,  0.0f,     nullLayer.data,    nullLayer.data,     1.0f,  0.0f, // bottom-left

             -1.0f*s  + x, -1.0f*s + y, -1.0f*s + z,   0.0f, -1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f, -1.0f, // top-right
             1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    0.0f, -1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f, -1.0f, // top-left
             1.0f *s + x, -1.0f *s+ y,  1.0f *s+ z,    0.0f, -1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f, -1.0f, // bottom-left
             1.0f *s + x, -1.0f *s+ y,  1.0f *s+ z,    0.0f, -1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f, -1.0f, // bottom-left
            -1.0f *s + x, -1.0f *s+ y,  1.0f *s+ z,    0.0f, -1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f, -1.0f, // bottom-right
            -1.0f *s + x, -1.0f *s+ y, -1.0f *s+ z,    0.0f, -1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f, -1.0f, // top-right

             -1.0f*s  + x,  1.0f*s + y, -1.0f*s + z,   0.0f,  1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f,  1.0f, // top-left
             1.0f *s + x,  1.0f *s+ y , 1.0f *s+ z,    0.0f,  1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f,  1.0f, // bottom-right
             1.0f *s + x,  1.0f *s+ y, -1.0f *s+ z,    0.0f,  1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f,  1.0f, // top-right
             1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    0.0f,  1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f,  1.0f, // bottom-right
            -1.0f *s + x,  1.0f *s+ y, -1.0f *s+ z,    0.0f,  1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f,  1.0f, // top-left
            -1.0f *s + x,  1.0f *s+ y,  1.0f *s+ z,    0.0f,  1.0f,  0.0f,     nullLayer.data,    nullLayer.data,     0.0f,  1.0f  // bottom-left
            };

            return ret;
        }

        constexpr std::array<unsigned int, 36> cubeIndices =  {
            0
            ,1
            ,2
            ,3
            ,4
            ,5
            ,6
            ,7
            ,8
            ,9
            ,10
            ,11
            ,12
            ,13
            ,14
            ,15
            ,16
            ,17
            ,18
            ,19
            ,20
            ,21
            ,22
            ,23
            ,24
            ,25
            ,26
            ,27
            ,28
            ,29
            ,30
            ,31
            ,32
            ,33
            ,34
            ,35
        };

        static constexpr std::array<float, 6 * 10> quadVertices(float x = 0, float y = 0, float z = 0, float s = 1)
        {
            TexLayer4 nullLayer{};
            nullLayer.x = 255;
            nullLayer.y = 255;
            nullLayer.z = 255;
            nullLayer.w = 255;

            std::array<float, 6 * 10> ret =
            {
                 1.0f*s + x, 0.0f + y,  1.0f*s + z,      0.0f, 1.0f, 0.0f,   nullLayer.data,    nullLayer.data,   1.0f,  0.0f,
                -1.0f*s + x, 0.0f + y,  1.0f*s + z,      0.0f, 1.0f, 0.0f,   nullLayer.data,    nullLayer.data,   0.0f,  0.0f,
                -1.0f*s + x, 0.0f + y, -1.0f*s + z,      0.0f, 1.0f, 0.0f,   nullLayer.data,    nullLayer.data,   0.0f,  1.0f,

                 1.0f*s + x, 0.0f + y,  1.0f*s + z,      0.0f, 1.0f, 0.0f,   nullLayer.data,    nullLayer.data,   1.0f,  0.0f,
                -1.0f*s + x, 0.0f + y, -1.0f*s + z,      0.0f, 1.0f, 0.0f,   nullLayer.data,    nullLayer.data,   0.0f,  1.0f,
                 1.0f*s + x, 0.0f + y, -1.0f*s + z,      0.0f, 1.0f, 0.0f,   nullLayer.data,    nullLayer.data,   1.0f,  1.0f
            };

            return ret;
        }

        constexpr std::array<unsigned int, 6> quadIndices =  {
            5
            ,4
            ,3
            ,2
            ,1
            ,0
        };

    }

} }

#endif // BPRIMITIVES_H
