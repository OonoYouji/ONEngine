#include "ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <unordered_map>
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
        // Assimp (Row-Major) -> Engine (Row-Major) そのままコピー
        return Engine::Math::Matrix4x4(
            m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4
        );
    }
}

std::vector<std::unique_ptr<Mesh>> ModelLoader::LoadModel(Graphics::RenderDevice* device, const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        Engine::Console::LogError(std::format("Model file not found: {}", filePath));
        return {};
    }

    Assimp::Importer importer;
    uint32_t flags = aiProcess_Triangulate | 
                     aiProcess_ConvertToLeftHanded | 
                     aiProcess_JoinIdenticalVertices |
                     aiProcess_GenSmoothNormals |
                     aiProcess_CalcTangentSpace;

    const aiScene* scene = importer.ReadFile(filePath, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Engine::Console::LogError(std::format("Assimp Error: {}", importer.GetErrorString()));
        return {};
    }

    // 1. モデル全体のユニークなボーンセットを構築 (グローバルボーンパレット)
    std::vector<Mesh::Bone> globalBones;
    std::unordered_map<std::string, uint32_t> boneNameToIndex;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aiMeshPtr = scene->mMeshes[i];
        for (unsigned int b = 0; b < aiMeshPtr->mNumBones; ++b) {
            std::string name = aiMeshPtr->mBones[b]->mName.C_Str();
            if (boneNameToIndex.find(name) == boneNameToIndex.end()) {
                uint32_t newIdx = static_cast<uint32_t>(globalBones.size());
                boneNameToIndex[name] = newIdx;
                
                Mesh::Bone bone;
                bone.name = name;
                bone.offsetMatrix = aiMatrixToEngine(aiMeshPtr->mBones[b]->mOffsetMatrix);
                globalBones.push_back(bone);
            }
        }
    }

    std::vector<std::unique_ptr<Mesh>> resultMeshes;

    // 2. 各サブメッシュの処理
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aiMeshPtr = scene->mMeshes[i];
        
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Math::Vector3 minP = { FLT_MAX, FLT_MAX, FLT_MAX };
        Math::Vector3 maxP = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (unsigned int v = 0; v < aiMeshPtr->mNumVertices; ++v) {
            Vertex vertex;
            vertex.position = { aiMeshPtr->mVertices[v].x, aiMeshPtr->mVertices[v].y, aiMeshPtr->mVertices[v].z, 1.0f };
            minP.x = std::min(minP.x, vertex.position.x);
            minP.y = std::min(minP.y, vertex.position.y);
            minP.z = std::min(minP.z, vertex.position.z);
            maxP.x = std::max(maxP.x, vertex.position.x);
            maxP.y = std::max(maxP.y, vertex.position.y);
            maxP.z = std::max(maxP.z, vertex.position.z);

            if (aiMeshPtr->HasNormals()) {
                vertex.normal = { aiMeshPtr->mNormals[v].x, aiMeshPtr->mNormals[v].y, aiMeshPtr->mNormals[v].z, 0.0f };
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

        // ウェイト情報をグローバルインデックスにリマップして抽出
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
            // Createの前にウェイトをセット！
            mesh->SetSkeleton(globalBones, std::move(boneWeights));
        }
        mesh->Create(device, vertices, indices);
        resultMeshes.push_back(std::move(mesh));
    }

    Engine::Console::Log(std::format("Loaded Model: {} (Meshes: {}, Unified Bones: {})", filePath, resultMeshes.size(), globalBones.size()));
    return resultMeshes;
}

} // namespace Engine::Asset
