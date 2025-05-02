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

	/// チャンクの生成
	Vector2 chunkSize = Vector2(
		terrainSize_.x / static_cast<float>(chunkCountX_),
		terrainSize_.y / static_cast<float>(chunkCountY_)
	);

	for (size_t row = 0; row < chunkCountX_; ++row) {
		for (size_t col = 0; col < chunkCountY_; ++col) {
			Vector3 chunkPosition = Vector3(
				static_cast<float>(row) * chunkSize.x,
				0.0f,
				static_cast<float>(col) * chunkSize.y
			);

			chunks_.emplace_back(this, chunkPosition, chunkSize);
		}
	}

	chunkSpan_ = std::span<std::span<TerrainChunk>>(reinterpret_cast<std::span<TerrainChunk>*>(chunks_.data()), chunkCountX_);



	/// カスタムメッシュで地形の描画を行う
	CustomMeshRenderer* meshRenderer = AddComponent<CustomMeshRenderer>();
	meshRenderer->SetVertices(vertices_);
	meshRenderer->SetIndices(indices_);
	meshRenderer->SetIsBufferRecreate(true);
}

void Terrain::Update() {
	for (auto& chunk : chunks_) {


		/// チャンクの描画
		Vector3 chunkSize3 = Vector3(chunk.GetChunkSize().x, 50.0f, chunk.GetChunkSize().y);
		Gizmo::DrawWireCube(
			chunk.GetPosition() + chunkSize3 * 0.5f,
			chunkSize3,
			Color::kRed
		);
	}

	if (CustomMeshRenderer* meshRenderer = GetComponent<CustomMeshRenderer>()) {
		meshRenderer->SetVertices(vertices_);
		//meshRenderer->SetIndices(indices_);
		meshRenderer->SetIsBufferRecreate(true);
	}
}