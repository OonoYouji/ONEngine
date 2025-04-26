#include "TerrainChunk.h"

/// std
#include <vector>

/// 
#include "../Terrain.h"

TerrainChunk::TerrainChunk(Terrain* _terrain, const Vector3& _position, const Vector2& _chunkSize)
	: pTerrain_(_terrain) {

	/// 引数のパラメータを保持  
	position_ = _position;
	chunkSize_ = _chunkSize;

	/// チャンク内の頂点データを計算  
	const int vertexCountX = static_cast<int>(chunkSize_.x);
	const int vertexCountY = static_cast<int>(chunkSize_.y);
	std::vector<Mesh::VertexData> vertices;

	for (int y = 0; y <= vertexCountY; ++y) {
		for (int x = 0; x <= vertexCountX; ++x) {
			Vector4 vertexPosition = Vector4(
				position_.x + x,
				position_.y,
				position_.z + y,
				1.0f
			);
			Mesh::VertexData vertex;
			vertex.position = vertexPosition;
			vertices.push_back(vertex);
		}
	}

	chunkVertices = std::span<Mesh::VertexData>(vertices);
}

