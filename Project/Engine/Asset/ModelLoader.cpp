#include "ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
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
        return Engine::Math::Matrix4x4(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );
    }
}

std::vector<std::unique_ptr<Mesh>> ModelLoader::LoadModel(Graphics::RenderDevice* device, const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        Engine::Console::LogError(std::format("Model file not found: {}", filePath));
        return {};
    }

    Assimp::Importer importer;
    
    // 座標系の変換 (左手系へ) や、三角形化、頂点の統合などを行う
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

    std::vector<std::unique_ptr<Mesh>> resultMeshes;

    // 全メッシュを走査
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aiMeshPtr = scene->mMeshes[i];
        
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Math::Vector3 minP = { FLT_MAX, FLT_MAX, FLT_MAX };
        Math::Vector3 maxP = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        // 頂点データの抽出 (Vertex構造体は変更せず、位置/法線/UVのみ)
        for (unsigned int v = 0; v < aiMeshPtr->mNumVertices; ++v) {
            Vertex vertex;
            
            // 位置 (w=1.0)
            vertex.position.x = aiMeshPtr->mVertices[v].x;
            vertex.position.y = aiMeshPtr->mVertices[v].y;
            vertex.position.z = aiMeshPtr->mVertices[v].z;
            vertex.position.w = 1.0f;

            minP.x = std::min(minP.x, vertex.position.x);
            minP.y = std::min(minP.y, vertex.position.y);
            minP.z = std::min(minP.z, vertex.position.z);
            maxP.x = std::max(maxP.x, vertex.position.x);
            maxP.y = std::max(maxP.y, vertex.position.y);
            maxP.z = std::max(maxP.z, vertex.position.z);

            // 法線 (w=0.0)
            if (aiMeshPtr->HasNormals()) {
                vertex.normal.x = aiMeshPtr->mNormals[v].x;
                vertex.normal.y = aiMeshPtr->mNormals[v].y;
                vertex.normal.z = aiMeshPtr->mNormals[v].z;
            } else {
                vertex.normal = { 0, 1, 0, 0 };
            }
            vertex.normal.w = 0.0f;

            // UV
            if (aiMeshPtr->mTextureCoords[0]) {
                vertex.uv.x = aiMeshPtr->mTextureCoords[0][v].x;
                vertex.uv.y = aiMeshPtr->mTextureCoords[0][v].y;
            } else {
                vertex.uv = { 0.0f, 0.0f };
            }

            vertex._pad[0] = 0;
            vertex._pad[1] = 0;

            vertices.push_back(vertex);
        }

        // ボーン情報の抽出 (Additive: 既存のVertexは壊さない)
        std::vector<Mesh::Bone> meshBones;
        std::vector<GeneratedSchema::BoneWeightData> boneWeights;
        
        if (aiMeshPtr->HasBones()) {
            boneWeights.resize(aiMeshPtr->mNumVertices);
            // ゼロ初期化
            for (auto& w : boneWeights) {
                for (int b = 0; b < 4; ++b) w.boneIndices[b] = 0;
                w.boneWeights = { 0, 0, 0, 0 };
            }

            std::vector<int> weightCountPerVertex(aiMeshPtr->mNumVertices, 0);

            for (unsigned int b = 0; b < aiMeshPtr->mNumBones; ++b) {
                aiBone* aiBonePtr = aiMeshPtr->mBones[b];
                
                Mesh::Bone bone;
                bone.name = aiBonePtr->mName.C_Str();
                bone.offsetMatrix = aiMatrixToEngine(aiBonePtr->mOffsetMatrix);
                meshBones.push_back(bone);

                uint32_t boneIndex = b; 
                for (unsigned int w = 0; w < aiBonePtr->mNumWeights; ++w) {
                    uint32_t vIdx = aiBonePtr->mWeights[w].mVertexId;
                    float weight = aiBonePtr->mWeights[w].mWeight;

                    if (weightCountPerVertex[vIdx] < 4) {
                        boneWeights[vIdx].boneIndices[weightCountPerVertex[vIdx]] = boneIndex;
                        // GeneratedSchema::BoneWeightData の fields 名に合わせてアクセス
                        if (weightCountPerVertex[vIdx] == 0) boneWeights[vIdx].boneWeights.x = weight;
                        else if (weightCountPerVertex[vIdx] == 1) boneWeights[vIdx].boneWeights.y = weight;
                        else if (weightCountPerVertex[vIdx] == 2) boneWeights[vIdx].boneWeights.z = weight;
                        else if (weightCountPerVertex[vIdx] == 3) boneWeights[vIdx].boneWeights.w = weight;
                        
                        weightCountPerVertex[vIdx]++;
                    }
                }
            }
        }

        // インデックスデータの抽出
        for (unsigned int f = 0; f < aiMeshPtr->mNumFaces; ++f) {
            aiFace face = aiMeshPtr->mFaces[f];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx) {
                indices.push_back(face.mIndices[idx]);
            }
        }

        // Engine::Mesh オブジェクトの作成
        auto mesh = std::make_unique<Mesh>();
        mesh->SetAABB(minP, maxP);
        mesh->Create(device, vertices, indices);
        if (!boneWeights.empty()) {
            mesh->SetSkeleton(std::move(meshBones), std::move(boneWeights));
        }
        resultMeshes.push_back(std::move(mesh));
    }

    Engine::Console::Log(std::format("Loaded Model: {} (Meshes: {})", filePath, resultMeshes.size()));
    return resultMeshes;
}

} // namespace Engine::Asset
