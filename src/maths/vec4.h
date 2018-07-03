#pragma once

#include <iostream>
#include <immintrin.h>

namespace Fondue { namespace maths {

    struct vec4
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			float elements[4];
			__m128 SSEVec4;
		};

		vec4() = default;

        vec4(const float& x, const float &y, const float &z, const float &w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        vec4& add(const vec4& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this;
        }

        vec4& subtract(const vec4& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;

            return *this;
        }

        vec4 operator+(const vec4& v) {
            return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
        }

        vec4 operator-(const vec4& v) {
            return vec4(x - v.x, y - v.y, z - v.x, w - v.w);
        }

        vec4& operator+=(const vec4& other)
        {
            return add(other);
        }

        vec4& operator-=(const vec4& other)
        {
            return subtract(other);
        }

        bool operator==(const vec4& other)
        {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const vec4& other)
        {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& stream, const vec4& vector)
        {
            stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
            return stream;
        }

    }__attribute__ ((aligned (16)));

}}
