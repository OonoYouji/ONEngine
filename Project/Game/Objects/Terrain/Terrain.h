#pragma once

/// std
#include <vector>
#include <span>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Graphics/Resource/ResourceData/Mesh.h"

#include "Chunk/TerrainChunk.h"

/// ///////////////////////////////////////////////////
/// 地形のオブジェクトクラス
/// ///////////////////////////////////////////////////
class Terrain : public IEntity {
	friend class TerrainEditor;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Terrain();
	~Terrain();

	void Initialize() override;
	void Update()     override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/* ----- terrain ----- */
	std::vector<Mesh::VertexData> vertices_; ///< 頂点データ
	std::vector<uint32_t> indices_; ///< インデックスデータ
	
	std::span<std::span<Mesh::VertexData>> vertexSpan_; ///< 頂点データのスパン

	Vector2 terrainSize_ = Vector2(10, 10); ///< 地形のサイズ


	/* ----- chunk ----- */
	std::vector<TerrainChunk> chunks_; ///< チャンクの配列（チャンクの数は地形のサイズによって変化する）
	std::span<std::span<TerrainChunk>> chunkSpan_; ///< チャンクの頂点データのスパン

	size_t chunkCountX_ = 10; ///< チャンクの数（X軸方向）
	size_t chunkCountY_ = 10; ///< チャンクの数（Y軸方向）

public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	/// @brief 頂点を二次元配列化したものを取得する
	/// @return 二次元配列にした頂点データのスパン
	const std::span<std::span<Mesh::VertexData>>& GetVertexSpan() const { return vertexSpan_; }
};

