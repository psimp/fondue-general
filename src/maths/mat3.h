#pragma once

#include "vec3.h"
#include "mat4.h"

namespace Fondue { namespace maths {

    struct mat3
    {
        union
        {
            float elements[3 * 3];
            vec3 rows[3];
        };

        mat3()
        {
            for (float& f : elements)
                f = 0.0f;
        }

        mat3(const mat4& M)
        {
            int i = 0;
            for (float* f : elements)
            {
                *(f+0) = M.elements[i++];
                *(f+1) = M.elements[i++];
                *(f+2) = M.elements[i++];
                i++; // Skip 4th row element
            }
        }

        static mat3 lookTowards(const vec3& _up, const vec3& focusPosition, const vec3& eyePosition)
        {
            vec3 direction = normalize(focusPosition - eyePosition);
            vec3 right = normalize(cross(_up, direction));
            vec3 up = normalize(cross(direction, right));

            mat3 viewSpace(1.0f);

            viewSpace.elements[0 + 0 * 4] = right.x;
            viewSpace.elements[1 + 0 * 4] = up.x;
            viewSpace.elements[2 + 0 * 4] = direction.x;

            viewSpace.elements[0 + 1 * 4] = right.y;
            viewSpace.elements[1 + 1 * 4] = up.y;
            viewSpace.elements[2 + 1 * 4] = direction.y;

            viewSpace.elements[0 + 2 * 4] = right.z;
            viewSpace.elements[1 + 2 * 4] = up.z;
            viewSpace.elements[2 + 2 * 4] = direction.z;

            return viewSpace;
        }

    };

} }
