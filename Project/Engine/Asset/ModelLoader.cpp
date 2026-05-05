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

        // 頂点データの抽出
        for (unsigned int v = 0; v < aiMeshPtr->mNumVertices; ++v) {
            Vertex vertex;
            
            // 位置
            vertex.position.x = aiMeshPtr->mVertices[v].x;
            vertex.position.y = aiMeshPtr->mVertices[v].y;
            vertex.position.z = aiMeshPtr->mVertices[v].z;

            // UV (存在する場合)
            if (aiMeshPtr->mTextureCoords[0]) {
                vertex.uv.x = aiMeshPtr->mTextureCoords[0][v].x;
                vertex.uv.y = aiMeshPtr->mTextureCoords[0][v].y;
            } else {
                vertex.uv = { 0.0f, 0.0f };
            }

            // 法線 (存在する場合)
            if (aiMeshPtr->HasNormals()) {
                vertex.normal.x = aiMeshPtr->mNormals[v].x;
                vertex.normal.y = aiMeshPtr->mNormals[v].y;
                vertex.normal.z = aiMeshPtr->mNormals[v].z;
            } else {
                vertex.normal = { 0, 1, 0 };
            }

            vertices.push_back(vertex);
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
        mesh->Create(device, vertices, indices);
        resultMeshes.push_back(std::move(mesh));
    }

    Engine::Console::Log(std::format("Loaded Model: {} (Meshes: {})", filePath, resultMeshes.size()));
    return resultMeshes;
}

} // namespace Engine::Asset
