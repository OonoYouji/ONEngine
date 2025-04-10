#pragma once

/// std
#include <vector>
#include <span>

/// engine
#include "Engine/Entity/Interface/IEntity.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Graphics/Resource/ResourceData/Mesh.h"

/// ///////////////////////////////////////////////////
/// 地形のオブジェクトクラス
/// ///////////////////////////////////////////////////
class Terrain : public IEntity {
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

	std::vector<Mesh::VertexData> vertices_; ///< 頂点データ
	std::vector<uint32_t> indices_; ///< インデックスデータ
	
	std::span<std::span<Mesh::VertexData>> vertexSpan_; ///< 頂点データのスパン

	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ

public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	/// @brief 頂点を二次元配列化したものを取得する
	/// @return 二次元配列にした頂点データのスパン
	const std::span<std::span<Mesh::VertexData>>& GetVertexSpan() const { return vertexSpan_; }
};

