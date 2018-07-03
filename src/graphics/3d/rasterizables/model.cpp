#include "model.h"

namespace Fondue { namespace graphics {

    void model_importer::load_model(const std::string& path, TextureArray* textureDestination3C, TextureArray* textureDestination1C,
                                      Batch* vertexDestination, Voxelizer* voxelizer)
    {
        TEMP_textureDestination_3C = textureDestination3C;
        TEMP_textureDestination_1C = textureDestination1C;

        TEMP_vertexDestination = vertexDestination;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_ValidateDataStructure );

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;

        auto bsp = path.find_last_of('/');
        auto fsp = path.find_last_of('.');
        directory = path.substr(0, bsp);
        auto filename = path.substr(bsp, fsp);

        std::vector<unsigned int> indices;
        std::vector<float> vertexData;
        std::vector<float> vertices;
        std::vector<maths::vec3> tids;
        maths::vec3 boundaryMin, boundaryMax;
            processNode(scene->mRootNode, scene, indices, vertexData, vertices, tids, boundaryMin, boundaryMax);

        const auto shift = -1.0 * boundaryMin;
        const auto scale = BOUNDARY_MAX_DIM / (hmax(boundaryMax+shift));
        const auto bound = (boundaryMax + shift) * scale;

        auto file = path.substr(bsp+1, path.find_last_of('.') - (bsp+1));

        if (voxelizer) voxelizer->gensdf(file, vertices, indices, tids, bound, shift * (scale * 1.07), scale * 0.7);
        if(TEMP_vertexDestination) TEMP_vertexDestination->fill_with(vertexData, indices, bound, vertexData.size());

        FILE* infs = fopen((filename + ".binf").c_str(), "wb");
        if (infs != NULL)
        {
            size_t mems[4] = { vertices.size(), indices.size(), tids.size(), vertexData.size() };
            fwrite(mems, 4 * sizeof(size_t), 1, infs);
            fclose(infs);

            bins = fopen((filename + "_vertices" + ".bin").c_str(), "wb");
            fwrite(vertices.data(), mems[0]*sizeof(uint), 1, bins);
            fclose(bins);

            FILE* bins = fopen((filename + "_indices" + ".bin").c_str(), "wb");
\           fwrite(indices.data(), mems[1]*sizeof(float), 1, bins);
            fclose(bins);

            bins = fopen((filename + "_tids" + ".bin").c_str(), "wb");
            fwrite(tids.data(), mems[2]*sizeof(float), 1, bins);
            fclose(bins);

            bins = fopen((filename + "_vertexdata" + ".bin").c_str(), "wb");
            fwrite(vertexData.data(), mems[3]*sizeof(maths::vec3), 1, bins);
            fclose(bins);

            bins = fopen((filename + "_bounds" + ".bin").c_str(), "wb");
            fwrite(bounds, size, 1, bins);
            fclose(bins);
        }
    }

    void model_importer::processNode(aiNode* node, const aiScene* scene, std::vector<unsigned int>& indices,
                                     std::vector<float>& vertexData, std::vector<float> &vertices, std::vector<maths::vec3> &tids,
                                     maths::vec3 &b0, maths::vec3 &b1)
    {
        for(unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, indices, vertexData, vertices, tids, b0, b1);

        for(unsigned int i = 0; i < node->mNumMeshes; i++)
            processMesh(scene->mMeshes[node->mMeshes[i]], scene, indices, vertexData, vertices, tids, b0, b1);
    }

    void model_importer::processMesh(aiMesh* mesh, const aiScene* scene, std::vector<unsigned int>& indices,
                                     std::vector<float>& vertexData, std::vector<float>& vertices, std::vector<maths::vec3> &tids,
                                     maths::vec3 &b0, maths::vec3 &b1)
    {
        aiMaterial& material = *scene->mMaterials[mesh->mMaterialIndex];
        TextureLayer diffuseMap;
        TextureLayer normalMap;
        TextureLayer roughnessMap;
        TextureLayer metalicMap;
        TextureLayer aoMap;

        auto cNumIndices = indices.size();

        // DO NOT CHANGE ORDER ***************
        diffuseMap = loadMaterialTextures(material, aiTextureType_DIFFUSE, T_RGB);
        normalMap = loadMaterialTextures(material, aiTextureType_AMBIENT, T_RGB);
        roughnessMap = loadMaterialTextures(material, aiTextureType_SPECULAR, T_R);
        metalicMap = loadMaterialTextures(material, aiTextureType_HEIGHT, T_R);
        aoMap = loadMaterialTextures(material, aiTextureType_OPACITY, T_R);
        // ***********************************

        TexLayer4 layers1;
        layers1.x= diffuseMap;
        layers1.y= normalMap;
        layers1.z= roughnessMap;
        layers1.w= metalicMap;

        TexLayer4 layers2;
        layers2.x= aoMap;
        layers2.y= 0;
        layers2.z= 0;
        layers2.w= 0;

        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {               
            vertexData.emplace_back(mesh->mVertices[v].x);
            vertexData.emplace_back(mesh->mVertices[v].y);
            vertexData.emplace_back(mesh->mVertices[v].z);

            vertices.emplace_back(mesh->mVertices[v].x);
            vertices.emplace_back(mesh->mVertices[v].y);
            vertices.emplace_back(mesh->mVertices[v].z);

            b0 = min(b0, {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z});
            b1 = max(b1, {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z});

            vertexData.emplace_back(mesh->mNormals[v].x);
            vertexData.emplace_back(mesh->mNormals[v].y);
            vertexData.emplace_back(mesh->mNormals[v].z);

            vertexData.emplace_back(layers1.data);
            vertexData.emplace_back(layers2.data);

            if(mesh->mTextureCoords[0])
            {
                vertexData.emplace_back(mesh->mTextureCoords[0][v].x);
                vertexData.emplace_back(mesh->mTextureCoords[0][v].y);
                tids.push_back( {mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y, float(layers1.x) } );
            }
            else {
                vertexData.emplace_back(0);
                vertexData.emplace_back(0);
                tids.push_back( {0, 0, 255 } );
            }
        }

        for(unsigned int f = 0; f < mesh->mNumFaces; f++)
            for(unsigned int i = 0; i < mesh->mFaces[f].mNumIndices; i++)
                indices.emplace_back(mesh->mFaces[f].mIndices[i] + cNumIndices);
    }

    TextureLayer model_importer::loadMaterialTextures(const aiMaterial& mat, const aiTextureType& type, CHANNELS ch)
    {
        TextureLayer layer;
        if(mat.GetTextureCount(type) == 1)
        {
            aiString path;
            mat.GetTexture(type, 0, &path);
            aiString td = directory;
            td.Append("/");
            td.Append(path.C_Str());

            if(TEMP_textureDestination_3C) layer = ch == T_RGB ? TEMP_textureDestination_3C->addToBack(td.C_Str())
                                                                 : TEMP_textureDestination_1C->addToBack(td.C_Str());

            mTextureSources.emplace_back(path);
        }
        else
        {
            std::cerr << std::string("Texture file missing from mesh in: ") + directory.C_Str() << std::endl;

            layer = TextureLayer(255);
        }
        return layer;
    }

    void model_importer::LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData> &Bones)
    {
        uint uniqueBones = 0;
        std::vector<BoneInfo> BoneInfos;

        for (uint i = 0 ; i < pMesh->mNumBones ; i++)
        {
            uint BoneIndex = 0;
            std::string BoneName(pMesh->mBones[i]->mName.data);

            auto lb = TEMP_BoneMap.lower_bound(BoneName);
            if(lb != TEMP_BoneMap.end() && !(TEMP_BoneMap.key_comp()(BoneName, lb->first))) {
                BoneIndex = lb->second;
            }
            else {
                BoneIndex = uniqueBones++;
                BoneInfos.push_back({});
                TEMP_BoneMap.insert(lb, {BoneName, BoneIndex});
            }

            BoneInfos[BoneIndex].offset = toMat4(pMesh->mBones[i]->mOffsetMatrix);

            for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
                uint vertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
                float weight = pMesh->mBones[i]->mWeights[j].mWeight;
                Bones[vertexID].addBone(BoneIndex, weight);
            }
        }
    }

    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumRotationKeys > 0);

        for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++)
            if (AnimationTime < float(pNodeAnim->mRotationKeys[i + 1].mTime))
                return i;

        assert(0);
    }

    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumPositionKeys > 0);

        for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++)
            if (AnimationTime < float(pNodeAnim->mPositionKeys[i + 1].mTime))
                return i;

        assert(0);
    }

    uint FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumScalingKeys > 0);

        for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++)
            if (AnimationTime < float(pNodeAnim->mScalingKeys[i + 1].mTime))
                return i;

        assert(0);
    }

    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        // we need at least two values to interpolate...
        if (pNodeAnim->mNumRotationKeys == 1) {
            Out = pNodeAnim->mRotationKeys[0].mValue;
            return;
        }

        uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
        uint NextRotationIndex = (RotationIndex + 1);
        assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
        float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
        float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = Out.Normalize();
    }

    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        // we need at least two values to interpolate...
        if (pNodeAnim->mNumPositionKeys == 1) {
            Out = pNodeAnim->mPositionKeys[0].mValue;
            return;
        }

        uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
        uint NextPositionIndex = (PositionIndex + 1);
        assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
        auto DeltaTime = float(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
        auto Factor = (AnimationTime - float(pNodeAnim->mPositionKeys[PositionIndex].mTime)) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& end = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        auto da = end - start;
        Out = start + Factor * da;
    }

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        // we need at least two values to interpolate...
        if (pNodeAnim->mNumScalingKeys == 1) {
            Out = pNodeAnim->mScalingKeys[0].mValue;
            return;
        }

        uint ScalingIndex = FindScale(AnimationTime, pNodeAnim);
        uint NextScaleIndex = (ScalingIndex + 1);
        assert(NextScaleIndex < pNodeAnim->mNumScalingKeys);
        auto DeltaTime = float(pNodeAnim->mScalingKeys[NextScaleIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
        auto Factor = (AnimationTime - float(pNodeAnim->mScalingKeys[ScalingIndex].mTime)) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& end = pNodeAnim->mScalingKeys[NextScaleIndex].mValue;
        auto da = end - start;
        Out = start + Factor * da;
    }

    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName)
    {
        for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
            const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

            if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
                return pNodeAnim;
            }
        }

        return NULL;
    }

    void model_importer::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const maths::mat4& ParentTransform, const aiScene& scene, std::vector<BoneInfo>& BoneInfos)
    {
        std::string NodeName(pNode->mName.data);

        const aiAnimation* pAnimation = scene.mAnimations[0];

        maths::mat4 NodeTransformation(toMat4(pNode->mTransformation));

        const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

        if (pNodeAnim) {
            // Interpolate scaling and generate scaling transformation matrix
            aiVector3D Scaling;
            CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
            auto ScalingM = maths::mat4::scale({Scaling.x, Scaling.y, Scaling.z});

            // Interpolate rotation and generate rotation transformation matrix
            aiQuaternion RotationQ;
            CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
            auto RotationM = toMat4(RotationQ.GetMatrix());

            // Interpolate translation and generate translation transformation matrix
            aiVector3D Translation;
            CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
            auto TranslationM = maths::mat4::translation({Translation.x, Translation.y, Translation.z});

            // Combine the above transformations
            NodeTransformation = TranslationM * RotationM * ScalingM;
        }

        maths::mat4 currentTransformation = ParentTransform * NodeTransformation;

        if (TEMP_BoneMap.find(NodeName) != TEMP_BoneMap.end()) {
            uint BoneIndex = TEMP_BoneMap[NodeName];
            BoneInfos[BoneIndex].final = currentTransformation * BoneInfos[BoneIndex].offset;
        }

        for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
            ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], currentTransformation, scene, BoneInfos);
        }
    }

    maths::mat4 model_importer::boneTransformer(const aiScene& scene, float TimeInSeconds, std::vector<maths::mat4>& Transforms, uint numBones, std::vector<BoneInfo>& BoneInfos)
    {
        auto Identity = maths::mat4::identity();

        float TicksPerSecond = float(scene.mAnimations[0]->mTicksPerSecond) <= 0.00001f ? float(scene.mAnimations[0]->mTicksPerSecond) : 25.f;

        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float AnimationTime = fmod(TimeInTicks, float(scene.mAnimations[0]->mDuration));

        ReadNodeHeirarchy(AnimationTime, scene.mRootNode, Identity, scene, BoneInfos);

        Transforms.resize(numBones);

        for (uint i = 0 ; i < numBones ; i++)
        {
            Transforms[i] = BoneInfos[i].final;
        }
    }

} }



























