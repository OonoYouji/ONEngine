#include "ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"

#ifdef _DEBUG
#pragma comment(lib, "Externals/assimp/lib/Debug/assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "Externals/assimp/lib/Release/assimp-vc143-mt.lib")
#endif

namespace Engine::Asset {

namespace {
    Engine::Math::Matrix4x4 aiMatrixToEngine(const aiMatrix4x4& m) {
        // Assimp の行列を分解し、OldEngine 方式（X反転）で再構築する
        aiVector3D   scale;
        aiQuaternion rotate;
        aiVector3D   position;
        m.Decompose(scale, rotate, position);

        // X軸を反転させる変換 (OldEngine 方式)
        return Engine::Math::Matrix4x4::MakeScale({ scale.x, scale.y, scale.z })
            * Engine::Math::Matrix4x4::MakeRotate(Engine::Math::Quaternion(rotate.x, -rotate.y, -rotate.z, rotate.w))
            * Engine::Math::Matrix4x4::MakeTranslate({ -position.x, position.y, position.z });
    }
}

std::shared_ptr<Model> ModelLoader::LoadModel(Graphics::RenderDevice* device, const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        Engine::Console::LogError(std::format("Model file not found: {}", filePath));
        return nullptr;
    }

    Assimp::Importer importer;
    // OldEngine と同じフラグ構成にする
    uint32_t flags = aiProcess_Triangulate | 
                     aiProcess_FlipWindingOrder | 
                     aiProcess_FlipUVs |
                     aiProcess_JoinIdenticalVertices |
                     aiProcess_GenSmoothNormals |
                     aiProcess_CalcTangentSpace |
                     aiProcess_LimitBoneWeights;

    const aiScene* scene = importer.ReadFile(filePath, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Engine::Console::LogError(std::format("Assimp Error: {}", importer.GetErrorString()));
        return nullptr;
    }

    auto model = std::make_shared<Model>();

    // 1. ノード階層の解析とボーンの対応付け
    std::vector<Mesh::Bone> globalBones;
    std::unordered_map<std::string, uint32_t> boneNameToIndex;
    std::unordered_map<std::string, aiBone*> boneInfoMap;

    // まず全メッシュからボーン名とオフセット行列を収集
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aiMeshPtr = scene->mMeshes[i];
        for (unsigned int b = 0; b < aiMeshPtr->mNumBones; ++b) {
            boneInfoMap[aiMeshPtr->mBones[b]->mName.C_Str()] = aiMeshPtr->mBones[b];
        }
    }

    // 再帰的にノードを走査して階層構造を構築
    std::function<void(aiNode*, int32_t)> processNode = [&](aiNode* node, int32_t parentIdx) {
        uint32_t currentIdx = static_cast<uint32_t>(globalBones.size());
        std::string name = node->mName.C_Str();
        boneNameToIndex[name] = currentIdx;

        Mesh::Bone bone;
        bone.name = name;
        bone.parentIndex = parentIdx;
        bone.localMatrix = aiMatrixToEngine(node->mTransformation);
        
        // ボーン情報（オフセット行列）があれば取得
        if (boneInfoMap.count(name)) {
            // offsetMatrix も aiMatrixToEngine で座標変換して取得
            bone.offsetMatrix = aiMatrixToEngine(boneInfoMap[name]->mOffsetMatrix);
        } else {
            bone.offsetMatrix = Engine::Math::Matrix4x4::kIdentity;
        }

        globalBones.push_back(bone);

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], (int32_t)currentIdx);
        }
    };

    processNode(scene->mRootNode, -1);

    // 2. アニメーションのロード
    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        aiAnimation* aiAnim = scene->mAnimations[i];
        Animation anim;
        anim.name = aiAnim->mName.C_Str();
        anim.duration = static_cast<float>(aiAnim->mDuration / (aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0));

        for (unsigned int c = 0; c < aiAnim->mNumChannels; ++c) {
            aiNodeAnim* aiChannel = aiAnim->mChannels[c];
            NodeAnimation nodeAnim;

            for (unsigned int k = 0; k < aiChannel->mNumPositionKeys; ++k) {
                aiVectorKey& key = aiChannel->mPositionKeys[k];
                nodeAnim.translate.push_back({
                    static_cast<float>(key.mTime / (aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0)),
                    { -key.mValue.x, key.mValue.y, key.mValue.z } // X反転
                });
            }

            for (unsigned int k = 0; k < aiChannel->mNumRotationKeys; ++k) {
                aiQuatKey& key = aiChannel->mRotationKeys[k];
                nodeAnim.rotate.push_back({
                    static_cast<float>(key.mTime / (aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0)),
                    { key.mValue.x, -key.mValue.y, -key.mValue.z, key.mValue.w } // X反転相当 (Y,Z反転)
                });
            }

            for (unsigned int k = 0; k < aiChannel->mNumScalingKeys; ++k) {
                aiVectorKey& key = aiChannel->mScalingKeys[k];
                nodeAnim.scale.push_back({
                    static_cast<float>(key.mTime / (aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0)),
                    { key.mValue.x, key.mValue.y, key.mValue.z }
                });
            }

            anim.nodeAnimations[aiChannel->mNodeName.C_Str()] = std::move(nodeAnim);
        }
        model->AddAnimation(std::move(anim));
    }

    std::vector<std::unique_ptr<Mesh>> resultMeshes;

    // 3. 各サブメッシュの処理
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aiMeshPtr = scene->mMeshes[i];
        
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Math::Vector3 minP = { FLT_MAX, FLT_MAX, FLT_MAX };
        Math::Vector3 maxP = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (unsigned int v = 0; v < aiMeshPtr->mNumVertices; ++v) {
            Vertex vertex;
            // X座標と法線のX成分を反転 (OldEngine 方式)
            vertex.position = { -aiMeshPtr->mVertices[v].x, aiMeshPtr->mVertices[v].y, aiMeshPtr->mVertices[v].z, 1.0f };
            minP.x = std::min(minP.x, vertex.position.x);
            minP.y = std::min(minP.y, vertex.position.y);
            minP.z = std::min(minP.z, vertex.position.z);
            maxP.x = std::max(maxP.x, vertex.position.x);
            maxP.y = std::max(maxP.y, vertex.position.y);
            maxP.z = std::max(maxP.z, vertex.position.z);

            if (aiMeshPtr->HasNormals()) {
                vertex.normal = { -aiMeshPtr->mNormals[v].x, aiMeshPtr->mNormals[v].y, aiMeshPtr->mNormals[v].z, 0.0f };
            } else {
                vertex.normal = { 0, 1, 0, 0 };
            }

            if (aiMeshPtr->mTextureCoords[0]) {
                vertex.uv = { aiMeshPtr->mTextureCoords[0][v].x, aiMeshPtr->mTextureCoords[0][v].y };
            } else {
                vertex.uv = { 0.0f, 0.0f };
            }
            vertex._pad[0] = 0; vertex._pad[1] = 0;
            vertices.push_back(vertex);
        }

        std::vector<GeneratedSchema::BoneWeightData> boneWeights;
        if (aiMeshPtr->HasBones()) {
            boneWeights.resize(aiMeshPtr->mNumVertices);
            for (auto& w : boneWeights) {
                for (int b = 0; b < 4; ++b) w.boneIndices[b] = 0;
                w.boneWeights = { 0, 0, 0, 0 };
            }

            std::vector<int> weightCountPerVertex(aiMeshPtr->mNumVertices, 0);
            for (unsigned int b = 0; b < aiMeshPtr->mNumBones; ++b) {
                aiBone* aiBonePtr = aiMeshPtr->mBones[b];
                uint32_t globalBoneIdx = boneNameToIndex[aiBonePtr->mName.C_Str()];

                for (unsigned int w = 0; w < aiBonePtr->mNumWeights; ++w) {
                    uint32_t vIdx = aiBonePtr->mWeights[w].mVertexId;
                    float weight = aiBonePtr->mWeights[w].mWeight;

                    if (weightCountPerVertex[vIdx] < 4) {
                        int slot = weightCountPerVertex[vIdx];
                        boneWeights[vIdx].boneIndices[slot] = globalBoneIdx;
                        if (slot == 0) boneWeights[vIdx].boneWeights.x = weight;
                        else if (slot == 1) boneWeights[vIdx].boneWeights.y = weight;
                        else if (slot == 2) boneWeights[vIdx].boneWeights.z = weight;
                        else if (slot == 3) boneWeights[vIdx].boneWeights.w = weight;
                        weightCountPerVertex[vIdx]++;
                    }
                }
            }
        }

        for (unsigned int f = 0; f < aiMeshPtr->mNumFaces; ++f) {
            aiFace face = aiMeshPtr->mFaces[f];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx) {
                indices.push_back(face.mIndices[idx]);
            }
        }

        auto mesh = std::make_unique<Mesh>();
        mesh->SetAABB(minP, maxP);
        if (!boneWeights.empty()) {
            mesh->SetSkeleton(globalBones, std::move(boneWeights));
        }
        mesh->Create(device, vertices, indices);
        Engine::Console::Log(std::format("ModelLoader: Submesh {} loaded. Vertices={}, Indices={}", i, vertices.size(), indices.size()));
        resultMeshes.push_back(std::move(mesh));
    }

    model->SetMeshes(std::move(resultMeshes));
    Engine::Console::Log(std::format("Loaded Model: {} (Meshes: {}, Unified Bones: {}, Animations: {})", filePath, model->GetMeshes().size(), globalBones.size(), model->GetAnimations().size()));
    return model;
}

} // namespace Engine::Asset
