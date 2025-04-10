#pragma once

/// std
#include <vector>
#include <span>

/// engine
#include "Engine/Entity/Interface/IEntity.h"
#include "Engine/Core/Utility/Math/Vector4.h"


/// ///////////////////////////////////////////////////
/// 地形のオブジェクトクラス
/// ///////////////////////////////////////////////////
class Terrain : public IEntity {
public:
	/// ===================================================
	/// public : sub class
	/// ===================================================
	
	struct Vertex {
		Vector4 position; ///< 頂点の位置
		Vector2 uv;       ///< uv座標
		Vector3 normal;   ///< 法線
	};
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

	std::vector<Vertex> vertices_; ///< 頂点データ
	std::vector<uint32_t> indices_; ///< インデックスデータ
	
	std::span<std::span<Vertex>> vertexSpan_; ///< 頂点データのスパン

	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ

public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	/// @brief 頂点を二次元配列化したものを取得する
	/// @return 二次元配列にした頂点データのスパン
	const std::span<std::span<Vertex>>& GetVertexSpan() const { return vertexSpan_; }
};

