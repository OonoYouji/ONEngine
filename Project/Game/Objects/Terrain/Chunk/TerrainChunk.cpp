#include "TerrainChunk.h"

/// std
#include <vector>

/// 
#include "Engine/ECS/Component/Component.h"
#include "../Terrain.h"

TerrainChunk::TerrainChunk(Terrain* _terrain, const Vector3& _position, const Vector2& _chunkSize)
	: pTerrain_(_terrain) {

	/// 引数のパラメータを保持  
	position_  = _position;
	chunkSize_ = _chunkSize;

	/*/// ここではチャンクの範囲内の地形の頂点を探索して自身のchunkVertices_にいれる
	for (auto& vertex : pTerrain_->GetVertices()) {
		/// チャンクの範囲内にある頂点を探索
		if (CollisionCheck::CubeVsSphere(
			position_, Vector3(chunkSize_.x, 1.0f, chunkSize_.y),
			Vector3(vertex.position.x, vertex.position.y, vertex.position.z), 0.5f)) {

			chunkVertices_.push_back(&vertex);
		}
	}*/


}

