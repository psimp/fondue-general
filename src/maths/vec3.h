#pragma once

#include <math.h>

#include <iostream>

namespace Fondue { namespace maths {

    template<typename T>
    struct vec3_t
	{
		union 
		{
			struct
			{
                T x = 0.0f, y = 0.0f, z = 0.0f;
			};
            T elements[3];
		};

        vec3_t() = default;

        template<typename S>
        vec3_t(vec3_t<S> v) { x = T(v.x); y = T(v.y); z = T(v.z); }

        vec3_t(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        explicit vec3_t(T v)
        {
            this->x = v;
            this->y = v;
            this->z = v;
        }

        friend T dot(const vec3_t& lhs, const vec3_t& rhs)
        {
            return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
        }

        friend T magnitute(const vec3_t& v)
        {
            T r = v.x*v.x + v.y*v.y + v.z*v.z;
            return sqrt(r);
        }

        friend vec3_t normalize(const vec3_t& v)
        {
            return v / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        friend vec3_t abs(const vec3_t& v_)
        {
            vec3_t v = v_;

            v.x = std::abs(v.x);
            v.y = std::abs(v.y);
            v.z = std::abs(v.z);

            return v;
        }

        friend T hmax(const vec3_t& v)
        {
            return std::max(v.x, std::max(v.y, v.z));
        }

        friend T hmin(const vec3_t& v)
        {
            return std::min(v.x, std::min(v.y, v.z));
        }

        friend T length(const vec3_t& v)
        {
            return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
        }

        friend vec3_t max(const vec3_t& lhs, const vec3_t& rhs)
        {
            return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z) };
        }

        friend vec3_t min(const vec3_t& lhs, const vec3_t& rhs)
        {
            return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z) };
        }

        friend vec3_t floor(const vec3_t& v)
        {
            return { std::floor(v.x), std::floor(v.y), std::floor(v.z) };
        }

        friend vec3_t ceil(const vec3_t& v)
        {
            return { std::ceil(v.x), std::ceil(v.y), std::ceil(v.z) };
        }

        friend vec3_t operator+(const vec3_t& lhs, const vec3_t& rhs) {
            return vec3_t(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }

        friend vec3_t operator-(const vec3_t& lhs, const vec3_t& rhs) {
            return vec3_t(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
        }

        friend vec3_t operator*(const T scalar, const vec3_t vector)
        {
            return vec3_t(vector.x * scalar, vector.y * scalar, vector.z * scalar);
        }

        friend vec3_t operator*(const vec3_t vector, const T scalar)
        {
            return vec3_t(vector.x * scalar, vector.y * scalar, vector.z * scalar);
        }

        friend vec3_t operator/(const vec3_t vector, const T scalar)
        {
            return vector * (1/scalar);
        }

        friend vec3_t operator*(const vec3_t lhs, const vec3_t rhs)
        {
            return vec3_t(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
        }

        friend vec3_t operator/(const vec3_t lhs, const vec3_t rhs)
        {
            return vec3_t(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
        }

        friend vec3_t operator-(const vec3_t lhs, const T rhs)
        {
            return vec3_t(lhs.x * rhs, lhs.y - rhs, lhs.z - rhs);
        }

        friend vec3_t operator+(const vec3_t lhs, const T rhs)
        {
            return vec3_t(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
        }

        friend vec3_t operator-(const T lhs, const vec3_t rhs)
        {
            return vec3_t(lhs * rhs.x, lhs - rhs.y, lhs - rhs.z);
        }

        friend vec3_t operator+(const T lhs, const vec3_t rhs)
        {
            return vec3_t(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
        }

        vec3_t& operator+=(const vec3_t& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;

            return *this;
        }

        vec3_t& operator-=(const vec3_t& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return *this;
        }

        bool operator==(const vec3_t& other)
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const vec3_t& other)
        {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& stream, const vec3_t& vector)
        {
            stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
            return stream;
        }

	};

    typedef vec3_t<float> vec3;
    typedef vec3_t<int> vec3i;

}}
