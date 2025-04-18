#include "Terrain.h"

/// engine
#include "Engine/ECS/Component/Component.h"

Terrain::Terrain() {}
Terrain::~Terrain() {}

void Terrain::Initialize() {

	/// 頂点の生成
	const size_t terrainWidth = static_cast<size_t>(terrainSize_.x);
	const size_t terrainHeight = static_cast<size_t>(terrainSize_.y);
	vertices_.resize(terrainWidth * terrainHeight);

	for (size_t row = 0; row < terrainWidth; ++row) {
		for (size_t col = 0; col < terrainHeight; ++col) {

			size_t index = row * terrainWidth + col;
			vertices_[index].position = Vector4(static_cast<float>(row), 0.0f, static_cast<float>(col), 1.0f);
			vertices_[index].uv = Vector2(
				static_cast<float>(row) / static_cast<float>(terrainWidth),
				static_cast<float>(col) / static_cast<float>(terrainHeight) * -1.0f
			);
			vertices_[index].normal = Vector3(0.0f, 1.0f, 0.0f);
		}
	}


	/// インデックスの生成
	const size_t faceVerts = 6;
	indices_.resize((terrainWidth - 1) * (terrainHeight - 1) * faceVerts);

	for (size_t row = 0; row < terrainWidth - 1; ++row) {
		for (size_t col = 0; col < terrainHeight - 1; ++col) {

			size_t index = row * (terrainHeight - 1) * faceVerts + col * faceVerts;
			indices_[index + 0] = static_cast<uint32_t>(row * terrainWidth + col);
			indices_[index + 1] = static_cast<uint32_t>(row * terrainWidth + col + 1);
			indices_[index + 2] = static_cast<uint32_t>((row + 1) * terrainWidth + col);
			indices_[index + 3] = static_cast<uint32_t>((row + 1) * terrainWidth + col);
			indices_[index + 4] = static_cast<uint32_t>(row * terrainWidth + col + 1);
			indices_[index + 5] = static_cast<uint32_t>((row + 1) * terrainWidth + col + 1);
		}
	}


	/// spanに変換
	vertexSpan_ = std::span<std::span<Mesh::VertexData>>(reinterpret_cast<std::span<Mesh::VertexData>*>(vertices_.data()), terrainWidth);
	//for (size_t r = 0; r < terrainWidth; r++) {
	//	vertexSpan_[r] = std::span<Mesh::VertexData>(&vertices_[r * terrainHeight], terrainHeight);
	//}


	CustomMeshRenderer* meshRenderer = AddComponent<CustomMeshRenderer>();
	meshRenderer->SetVertices(vertices_);
	meshRenderer->SetIndices(indices_);
	meshRenderer->SetIsBufferRecreate(true);
}

void Terrain::Update() {

}