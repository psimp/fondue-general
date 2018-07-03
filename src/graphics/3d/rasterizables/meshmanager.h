#pragma once

#include <vector>
#include "../../../maths/maths.h"
#include "../../glwrappers/texturearray.h"
#include "../../shader.h"

namespace Fondue {

typedef unsigned short mesh_s;

class entity_manager
{

private:

    struct unpacked_pose
    {
        struct
        {
            float x;
            float y;
            float z;
            float scale;
        };
        maths::quat rot;

        __attribute__((always_inline)) void abs_rotate(float degrees, float x, float y, float z)
        {
            rot = maths::quat::unit_quat(maths::toRad(degrees), x, y, z) * rot;
        }

        __attribute__((always_inline)) void lcl_rotate(float degrees, float x, float y, float z)
        {
            rot = rot * maths::quat::unit_quat(maths::toRad(degrees), x, y, z);
        }

        __attribute__((always_inline)) void abs_shift(float dx, float dy, float dz)
        {
            x += dx;
            y += dy;
            z += dz;
        }
    };

private:

    GLuint mBBoxUBO;
    size_t cmem = 0;

    static inline std::vector<mesh_s> m_sub_meshes[MAX_GLOBAL_MESHES];
    static inline std::array<maths::mat4, MAX_GLOBAL_MESHES> m_positions;
    static inline int m_parents[MAX_GLOBAL_MESHES] = {-1};

public:

    static inline __attribute__((always_inline)) const maths::mat4& getAbsolutePosition(mesh_s mesh)
    { return m_positions[mesh]; }

    static inline __attribute__((always_inline)) const int getParent(mesh_s mesh)
    { return m_parents[mesh]; }

    static inline __attribute__((always_inline)) maths::mat4* getPackedPoses()
    { return &m_positions[0]; }

    static inline __attribute__((always_inline)) void abs_shift(mesh_s mesh, float x, float y, float z)
    {
        m_positions[mesh].elements[0 + 3 * 4] += x;
        m_positions[mesh].elements[1 + 3 * 4] += y;
        m_positions[mesh].elements[2 + 3 * 4] += z;

        for (mesh_s submesh : m_sub_meshes[mesh])
        {
            m_positions[submesh].elements[0 + 3 * 4] += x;
            m_positions[submesh].elements[1 + 3 * 4] += y;
            m_positions[submesh].elements[2 + 3 * 4] += z;
        }
    }

    static inline __attribute__((always_inline)) void shift(mesh_s mesh, float x, float y, float z)
    {
        maths::vec3 dir = maths::mat4::Mat4xVec3SSE(m_positions[mesh], {x, y, z});

        m_positions[mesh].elements[0 + 3 * 4] += dir.x;
        m_positions[mesh].elements[1 + 3 * 4] += dir.y;
        m_positions[mesh].elements[2 + 3 * 4] += dir.z;

        for (mesh_s submesh : m_sub_meshes[mesh])
        {
            m_positions[submesh].elements[0 + 3 * 4] += dir.x;
            m_positions[submesh].elements[1 + 3 * 4] += dir.y;
            m_positions[submesh].elements[2 + 3 * 4] += dir.z;
        }
    }

    static inline __attribute__((always_inline)) void rotate(mesh_s mesh, float degrees, float x, float y, float z)
    {
        static maths::mat4 temp;
        temp = maths::mat4::M44TransformInverseSSE(m_positions[mesh]);
        maths::mat4 ri = maths::mat4::rotation(degrees, maths::vec3(x, y, z));

        for (mesh_s submesh : m_sub_meshes[mesh])
        {
            m_positions[submesh] = m_positions[submesh] * temp;
            m_positions[submesh] = m_positions[submesh] * ri;
            m_positions[submesh] = m_positions[submesh] * m_positions[mesh];
        }

        m_positions[mesh] = ri * m_positions[mesh];
    }

    static inline __attribute__((always_inline)) void pose(mesh_s mesh, const maths::mat4& p)
    {
        static maths::mat4 temp;
        temp = maths::mat4::M44TransformInverseSSE(m_positions[mesh]);

        for (mesh_s submesh : m_sub_meshes[mesh])
        {
            m_positions[submesh] = m_positions[submesh] * temp;
            m_positions[submesh] *= p;
            m_positions[submesh] = m_positions[submesh] * m_positions[mesh];
        }

         m_positions[mesh] *= p;
    }

    static inline __attribute__((always_inline)) void abs_rotate(mesh_s mesh, float degrees, float x, float y, float z)
    {
        maths::mat4 ri = maths::mat4::rotation(degrees, maths::vec3(x, y, z));

//        for (mesh_s submesh : m_sub_meshes[mesh])
//        {
//            m_positions[submesh] = m_positions[submesh] *  ri;
//        }

        m_positions[mesh] = ri * m_positions[mesh];
    }

    static inline __attribute__((always_inline)) void pack_pose(maths::mat4& res, unpacked_pose pose)
    {
        auto& rot = pose.rot;
        auto& scale = pose.scale;

        float t0 = 2 * (rot.x * rot.x);
        float t1 = 2 * (rot.w * rot.x);

        float t2 = 2 * (rot.x * rot.y);
        float t3 = 2 * (rot.y * rot.y);
        float t4 = 2 * (rot.w * rot.y);

        float t5 = 2 * (rot.x * rot.z);
        float t6 = 2 * (rot.y * rot.z);
        float t7 = 2 * (rot.z * rot.z);
        float t8 = 2 * (rot.w * rot.z);

        res.elements[0 + 0 * 4] = (1 - t3 - t7) * scale;
        res.elements[1 + 0 * 4] = (t2 + t8) * scale;
        res.elements[2 + 0 * 4] = (t5 - t4) * scale;

        res.elements[0 + 1 * 4] = (t2 - t8) * scale;
        res.elements[1 + 1 * 4] = (1 - t0 - t7) * scale;
        res.elements[2 + 1 * 4] = (t6 + t1) * scale;

        res.elements[0 + 2 * 4] = (t5 + t4) * scale;
        res.elements[1 + 2 * 4] = (t6 - t1) * scale;
        res.elements[2 + 2 * 4] = (1 - t0 - t3) * scale;

        res.elements[0 + 3 * 4] = pose.x;
        res.elements[1 + 3 * 4] = pose.y;
        res.elements[2 + 3 * 4] = pose.z;
        res.elements[3 + 3 * 4] = 1;
    }

    static inline void setSubmeshes(mesh_s n_mesh, mesh_s parent)
    {
        m_sub_meshes[parent].emplace_back(n_mesh);
        if (m_parents[parent] == -1) return;
        setSubmeshes(n_mesh, m_parents[parent]);
    }

    static inline __attribute__((always_inline)) void place_entity(const mesh_s thisMesh, float x, float y, float z, float scale)
    {
        unpacked_pose mpose;
        mpose.x = x;
        mpose.y = y;
        mpose.z = z;
        mpose.scale = scale;
        mpose.rot = maths::quat();
        pack_pose(m_positions[thisMesh], mpose);
    }


    static inline __attribute__((always_inline)) void place_entity(const mesh_s thisMesh, float x, float y, float z, float scale, mesh_s parent)
    {
        m_parents[thisMesh] = parent;
        setSubmeshes(thisMesh, parent);

        place_entity(thisMesh, x, y, z, scale);

        m_positions[thisMesh] = m_positions[thisMesh] * m_positions[parent];
    }

    static maths::vec3 getPos(mesh_s m)
    {
        return
        {
            m_positions[m].elements[0 + 3 * 4],
            m_positions[m].elements[1 + 3 * 4],
            m_positions[m].elements[2 + 3 * 4]
        };
    }

    static float getPosX(mesh_s m)
    {
        return m_positions[m].elements[0 + 3 * 4];
    }

    static float getPosY(mesh_s m)
    {
        return m_positions[m].elements[1 + 3 * 4];
    }

    static float getPosZ(mesh_s m)
    {
        return m_positions[m].elements[2 + 3 * 4];
    }

    static maths::vec3 getRelativePosition(mesh_s m1, mesh_s m2)
    {
        maths::vec3 v1 = getPos(m1);
        maths::vec3 v2 = getPos(m2);
        return {v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
    }

    static std::array<maths::mat4, MAX_GLOBAL_MESHES>& getPositions()
    {
        return m_positions;
    }

private:

    entity_manager() { }

};

}
