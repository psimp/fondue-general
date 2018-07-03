#ifndef QUATERNION_H
#define QUATERNION_H

#pragma once

#define _USE_MATH_DEFINES
#include "maths.h"
#include "vec3.h"
#include <immintrin.h>

namespace Fondue {	namespace maths {

	struct quat
	{
        union
        {
            struct
            {
                float x = 0.0f;
                float y = 0.0f;
                float z = 0.0f;
                float w = 1.0f;
            };
            __m128 v;
        };

        operator __m128() const { return v; }

        quat() = default;

        static quat unit_quat(float theta, float x, float y, float z)
        {
            float s = sin(theta / 2);
            return quat(cos(theta / 2), x * s, y * s, z * s);
        }

        quat(float nw, float nx, float ny, float nz)
        {
            w = nw;
            x = nx;
            y = ny;
            z = nz;
        }

        quat(__m128 nv)
        {
            v = nv;
        }

        friend quat operator+(const quat& lhs, const quat& rhs)
        {
            return quat(lhs.w + rhs.w, lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }

        friend quat operator*(const quat& lhs, const quat& rhs)
        {
            __m128 xyzw = rhs.v;
            __m128 abcd = lhs.v;

            __m128 wzyx = SWIZZLE14(xyzw, 0,1,2,3);
            __m128 baba = SWIZZLE14(abcd, 0,1,0,1);
            __m128 dcdc = SWIZZLE14(abcd, 2,3,2,3);

            __m128 ZnXWY = _mm_hsub_ps(_mm_mul_ps(xyzw, baba), _mm_mul_ps(wzyx, dcdc));
            __m128 XZYnW = _mm_hadd_ps(_mm_mul_ps(xyzw, dcdc), _mm_mul_ps(wzyx, baba));

            __m128 XZWY = _mm_addsub_ps( SWIZZLE24(XZYnW, ZnXWY, 3,2,1,0),
                                         SWIZZLE24(ZnXWY, XZYnW, 2,3,0,1) );

            return SWIZZLE24(XZWY, XZWY, 2,1,3,0);

        }

        quat& operator*=(const quat& rhs)
        {
            __m128 xyzw = rhs.v;
            __m128 abcd = v;

            __m128 wzyx = SWIZZLE14(xyzw, 0,1,2,3);
            __m128 baba = SWIZZLE14(abcd, 0,1,0,1);
            __m128 dcdc = SWIZZLE14(abcd, 2,3,2,3);

            __m128 ZnXWY = _mm_hsub_ps(_mm_mul_ps(xyzw, baba), _mm_mul_ps(wzyx, dcdc));
            __m128 XZYnW = _mm_hadd_ps(_mm_mul_ps(xyzw, dcdc), _mm_mul_ps(wzyx, baba));

            __m128 XZWY = _mm_addsub_ps( SWIZZLE24(XZYnW, ZnXWY, 3,2,1,0),
                                         SWIZZLE24(ZnXWY, XZYnW, 2,3,0,1) );

            this->v =  SWIZZLE24(XZWY, XZWY, 2,1,3,0);

            return *this;
        }

        quat operator*(float scalar)
        {
            return quat(w * scalar, x * scalar, y * scalar, z * scalar);
        }

        quat conjugate()
        {
            return quat(w, -1 * x, -1 * y, -1 * z);
        }

        float magnitute()
        {
            return pow(pow(w, 2) + pow(x, 2) + pow(y, 2) + pow(z, 2), 0.5f);
        }

        quat get_unit()
        {
            float d = (1 / (*this).magnitute());

            float nx = this->x * d;
            float ny = this->y * d;
            float nz = this->z * d;
            float nw = this->w * d;

            return quat(nw, nx, ny, nz);
        }

        void normalize()
        {
            float d = (1 / (*this).magnitute());

            x *= d;
            y *= d;
            z *= d;
            w *= d;
        }

        vec3 toEuler()
        {
            float sinPitch = -2.0f * (y * z + w * x);
            if (fabs(sinPitch) > 0.9999f)
            {
                return vec3
                (
                    -atan2(-x * z - w * y, 0.5f - y * y - z * z),
                    -1.570796f * sinPitch,
                    0.0f
                );
            }
            return vec3
            (
                -atan2(x * z - w * y, 0.5f - x * x - y * y),
                -asin(sinPitch),
                -atan2(x * y - w * z, 0.5f - x * x - z * z)
            );
        }

        mat4 toMat4()
        {
            mat4 result(1.0f);

            result.elements[0 + 0 * 4] = 1 - 2 * (y * y) - 2 * (z * z);
            result.elements[1 + 0 * 4] = 2 * (x * y) + 2 * (w * z);
            result.elements[2 + 0 * 4] = 2 * (x * z) - 2 * (w * y);

            result.elements[0 + 1 * 4] = 2 * (x * y) - 2 * (w * z);
            result.elements[1 + 1 * 4] = 1 - 2 * (x * x) - 2 * (z * z);
            result.elements[2 + 1 * 4] = 2 * (y * z) + 2 * (w * x);

            result.elements[0 + 2 * 4] = 2 * (x * z) + 2 * (w * y);
            result.elements[1 + 2 * 4] = 2 * (y * z) - 2 * (w * x);
            result.elements[2 + 2 * 4] = 1 - 2 * (x * x) - 2 * (y * y);

            return result;
        }

        static vec3 rotateVectorByQuat(const vec3& v, const quat& q)
        {
            vec3 u(q.x, q.y, q.z);

            float s = q.w;

            return 2.0f * dot(u, v) * u + (s*s - dot(u, u)) * v + 2.0f * s * maths::cross(u, v);
        }

        // Taken from https://en.wikipedia.org/wiki/Slerp#Quaternion_Slerp
        static quat slerp(quat q0, quat q1, float t)
        {
            // Only unit quaternions are valid rotations.
            // Normalize to avoid undefined behavior.
            q0.normalize();
            q1.normalize();

            // Compute the cosine of the angle between the two vectors.
            float dot = (q0.x * q1.x + q0.y * q1.y + q0.z * q1.z);

            const float DOT_THRESHOLD = 0.9995;
            if (fabs(dot) > DOT_THRESHOLD) {
                // If the inputs are too close for comfort, linearly interpolate
                // and normalize the result.

                quat result = q0 + (q1 + q0.conjugate()) * t;
                result.get_unit();
                return result;
            }

            // If the dot product is negative, the quaternions
            // have opposite handed-ness and slerp won't take
            // the shorter path. Fix by reversing one quaternion.
            if (dot < 0.0f) {
                q1 = q1.conjugate();
                dot = -dot;
            }

            dot = clamp(dot, -1.0f, 1.0f);  // Robustness: Stay within domain of acos()
            float theta_0 = acos(dot);		// theta_0 = angle between input vectors
            float theta = theta_0 * t;		// theta = angle between v0 and result

            quat q2 = q1 + (q0.conjugate() * dot);
            q2.get_unit();					// { v0, v2 } is now an orthonormal basis

            return q0 * cos(theta) + q2 * sin(theta);
        }


        static inline __attribute__((always_inline)) void rotateVectorByQuatI(float& mx, float& my, float& mz, const quat& q)
        {
            float dotvq = 2.0f * (mx * q.x + my * q.y + mz * q.z);
            float dotqq = q.x * q.x + q.y * q.y + q.z * q.z;
            float qw2 = q.w * q.w;

            float nx = dotvq * q.x + (qw2 - dotqq) * mx + 2.0f * q.w * (q.y * mz - my * q.z);
            float ny = dotvq * q.y + (qw2 - dotqq) * my + 2.0f * q.w * (q.z * mx - mz * q.x);
            float nz = dotvq * q.z + (qw2 - dotqq) * mz + 2.0f * q.w * (q.x * my - mx * q.y);

            mx = nx;
            my = ny;
            mz = nz;
        }

        friend std::ostream& operator<<(std::ostream& stream, const quat& quat)
        {
            stream << "{ " << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << " }";
            return stream;
        }

    }__attribute__ ((aligned (16)));

} }

#endif
