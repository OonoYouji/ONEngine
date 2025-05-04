#pragma once

/// std
#include <span>

/// engine
#include "Engine/Graphics/Resource/ResourceData/Mesh.h"

/// ///////////////////////////////////////////////////
/// 地形のチャンククラス
/// ///////////////////////////////////////////////////
class TerrainChunk {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	TerrainChunk(class Terrain* _terrain, const Vector3& _position, const Vector2& _chunkSize);
	~TerrainChunk() = default;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class Terrain* pTerrain_ = nullptr; ///< 地形
	Vector3 position_;
	Vector2 chunkSize_;

	std::vector<Mesh::VertexData*> chunkVertices_; ///< 頂点データのpointer


public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	const Vector3& GetPosition() const { return position_; }

	const Vector2& GetChunkSize() const { return chunkSize_; }

	const std::vector<Mesh::VertexData*>& GetChunkVertices() const { return chunkVertices_; }

};

