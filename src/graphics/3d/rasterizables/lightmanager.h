#pragma once

#include <glad/glad.h>
#include "../../../maths/vec4.h"
#include "../../shader.h"

namespace Fondue {	namespace graphics {

typedef unsigned short light_s;

    struct dynamic_light_manager
	{

    struct Light
    {
        maths::vec4 position;
        maths::vec4 color3_rad1;
    };

    private:

        static inline GLuint mLightUBO;
        static inline Light* mLights;

    public:

        static inline unsigned short gNumLights = 0;

        static inline unsigned int reserved_size = MAX_LIGHTS * sizeof(Light);
        static inline unsigned int current_size = 0;

    public:

        static void init()
        {
            mLights = new Light[MAX_LIGHTS];

            glGenBuffers(1, &mLightUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, mLightUBO);
            glBufferData(GL_UNIFORM_BUFFER, reserved_size, NULL, GL_STATIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, mLightUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        static void update_buffer()
        {
            glBindBuffer(GL_UNIFORM_BUFFER, mLightUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, current_size, &mLights[0]);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        static void init_map()
        {
            glGenBuffers(1, &mLightUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, mLightUBO);
            glBufferStorage(GL_UNIFORM_BUFFER, reserved_size, 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, mLightUBO);

            mLights = reinterpret_cast<Light*>
                      (
                          glMapBufferRange(GL_UNIFORM_BUFFER, 0 , reserved_size,
                                           GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
                      );

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        static light_s add_light(float x, float y, float z, float r, float g, float b, float rad)
        {
            mLights[gNumLights] = {maths::vec4(x, y, z, 1), maths::vec4(r, g, b, rad)};

            gNumLights++;
            current_size += sizeof(Light);

            return (gNumLights - 1);
        }

        static light_s shift(light_s light, float x, float y, float z)
        {
            mLights[light].position.x += x;
            mLights[light].position.y += y;
            mLights[light].position.z += z;
        }

    private:
        dynamic_light_manager() { }
	};

} }
