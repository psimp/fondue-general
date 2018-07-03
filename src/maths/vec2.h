#pragma once

#include <iostream>

namespace Fondue { namespace maths {

	struct vec2
	{
		union
		{
			struct
			{
				float x, y;
			};
			float elements[2];
		};

        vec2() = default;

        vec2(const float& x, const float &y)
        {
            this->y = y;
            this->x = x;
        }

        vec2& add(const vec2& other)
        {
            x += other.x;
            y += other.y;

            return *this;
        }

        vec2& subtract(const vec2& other)
        {
            x -= other.x;
            y -= other.y;

            return *this;
        }

        vec2 operator+(const vec2& v) {
            return vec2(x + v.x, y + v.y);
        }

        vec2 operator-(const vec2& v) {
            return vec2(x - v.x, y - v.y);
        }

        vec2& operator+=(const vec2& other)
        {
            return add(other);
        }

        vec2& operator-=(const vec2& other)
        {
            return subtract(other);
        }

        bool operator==(const vec2& other)
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const vec2& other)
        {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& stream, const vec2& vector)
        {
            stream << "(" << vector.x << ", " << vector.y << ")";
            return stream;
        }
	};

}}
