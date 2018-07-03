#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../batches/textured.h"
#include "../../glwrappers/texturearray.h"
#include "../../shader.h"
#include "../../../maths/maths.h"
#include "../renderers/voxelizer.h"

#include <iostream>
#include <vector>
#include <cassert>

namespace Fondue {	namespace graphics {

    union TexLayer4
    {
        float data;
        struct
        {
            TextureLayer x, y, z, w;
        };
    };

    struct VertexBoneData
    {
        static const unsigned short NUM_BONES_PER_VEREX = 4;

        uint IDs[NUM_BONES_PER_VEREX];
        float Weights[NUM_BONES_PER_VEREX];

        void addBone(uint ID, float weight)
        {
            for (uint i = 0 ; i < NUM_BONES_PER_VEREX ; i++) {
                if (Weights[i] <= 0.001f) {
                    IDs[i] = ID;
                    Weights[i] = weight;
                    return;
                }
            }

            assert(0);
        }
    };

    struct BoneInfo
    {
        maths::mat4 offset;
        maths::mat4 final;
    };

    enum CHANNELS
    {
        T_R, T_RGB
    };

    static maths::mat4 toMat4(const aiMatrix4x4& m)
    {
        maths::mat4 res;

        for (unsigned char i = 0; i < 16; i++)
            res.elements[i] = *m[i];

        return res;
    }

    static maths::mat4 toMat4(const aiMatrix3x3& m)
    {
        maths::mat4 res;

        for (unsigned char i = 0; i < 3; i++)
            for (unsigned char j = 0; j < 3; j++)
                res.elements[j + i * 4] = *m[i + j * 3];

        return res;
    }

    class model_importer
    {
        private:

            typedef maths::mat4 BoneTransform;

            static inline aiString directory;
            static inline std::vector<aiString> mTextureSources;

            static inline mesh_s TEMP_meshOut;
            static inline TextureArray* TEMP_textureDestination_3C;
            static inline TextureArray* TEMP_textureDestination_1C;
            static inline Batch* TEMP_vertexDestination;
            static inline const maths::vec3* TEMP_offsetPos;
            static inline std::map<std::string, uint> TEMP_BoneMap;

            static void processNode(aiNode *node, const aiScene *scene, std::vector<unsigned int>& indices,
                                    std::vector<float>& vertexData, std::vector<float> &vertices, std::vector<maths::vec3> &tids, maths::vec3 &b0, maths::vec3 &b1);
            static void processMesh(aiMesh *mesh, const aiScene *scene, std::vector<unsigned int>& indices,
                                    std::vector<float>& vertexData, std::vector<float> &vertices, std::vector<maths::vec3> &tids, maths::vec3 &b0, maths::vec3 &b1);

            static void LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
            static maths::mat4 boneTransformer(const aiScene& scene, float TimeInSeconds, std::vector<maths::mat4>& Transforms,
                                               uint numBones, std::vector<BoneInfo> &BoneInfos);

            static TextureLayer loadMaterialTextures(const aiMaterial& mat, const aiTextureType& type, CHANNELS ch);

        public:          

            static void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const maths::mat4& ParentTransform,
                                          const aiScene& scene, std::vector<BoneInfo> &BoneInfos);

            static void load_model(const std::string& path, TextureArray *textureDestination, TextureArray *textureDestination1C,
                                        Batch *vertexDestination, Voxelizer *Voxelizer);

    private:
        model_importer() { }
    };

} }

#endif

