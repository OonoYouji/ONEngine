#include "GraphicsResourceLoader.h"

/// externals
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/DebugTools/Assert.h"


GraphicsResourceLoader::GraphicsResourceLoader(DxManager* _dxManager) 
	: dxManager_(_dxManager) {}

GraphicsResourceLoader::~GraphicsResourceLoader() {}



void GraphicsResourceLoader::Initialize() {

	/// assimpの読み込みフラグ
	assimpLoadFlags_ = aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

}

void GraphicsResourceLoader::LoadTexture([[maybe_unused]] const std::string& _filePath) {}

size_t GraphicsResourceLoader::LoadModelObj([[maybe_unused]] const std::string& _filePath) {

	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(_filePath, assimpLoadFlags_);

	if (!scene) {
		return 0; ///< 読み込み失敗
	}

	/// mesh 解析
	for (uint32_t meshIndex = 0u; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		Assert(mesh->HasNormals(), "mesh has no normals");
		Assert(mesh->HasTextureCoords(0), "mesh has no texture coords");

		/// sceneのデータを使ってMeshを作成する
		std::vector<Mesh::VertexData> vertices;
		std::vector<uint32_t>         indices;

		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);


		/// vertex 解析
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
			Mesh::VertexData&& vertex = {
				Vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f),
				Vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z),
				Vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
			};

			vertices.push_back(vertex);
		}


		/// index 解析
		for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			Assert(face.mNumIndices == 3, "not triangles");

			for (uint32_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}


		/// mesh dataを作成
		std::unique_ptr<Mesh> meshData = std::make_unique<Mesh>();
		meshData->SetVertices(vertices);
		meshData->SetIndices(indices);

		/// bufferの作成
		meshData->CreateBuffer(dxManager_->GetDxDevice());

		meshes_.push_back(std::move(meshData));
	}

	return meshes_.size() - 1; ///< 読み込み成功, meshのindexを返す
}
