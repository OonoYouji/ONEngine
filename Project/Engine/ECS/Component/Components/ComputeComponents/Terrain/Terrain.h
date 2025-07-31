#pragma once

/// std
#include <vector>
#include <array>
#include <span>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Utility.h"

#include "TerrainVertex.h"


/// ///////////////////////////////////////////////////
/// 地形のコンポーネント
/// ///////////////////////////////////////////////////
class Terrain : public IComponent {
public:
	/// =========================================
	/// public : sub class
	/// =========================================

	struct Triangle {
		uint32_t i0 : 10;
		uint32_t i1 : 10;
		uint32_t i2 : 10;
		uint32_t padding : 2; ///< パディング
	};

	enum SPLAT_TEX {
		GRASS,
		DIRT,
		ROCK,
		SNOW,
		SPLAT_TEX_COUNT
	};


public:
	/// =========================================
	/// public : methods
	/// =========================================

	Terrain();
	~Terrain() override;

private:
	/// =========================================
	/// private : objects
	/// =========================================
	/// ===================================================
	/// private : objects
	/// ===================================================

	/* ----- terrain ----- */
	std::vector<TerrainVertex> vertices_; ///< 頂点データ
	std::vector<uint32_t> indices_; ///< インデックスデータ

	std::span<std::span<TerrainVertex>> vertexSpan_; ///< 頂点データのスパン

	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ


	/* ----- Octree ----- */

	//std::unique_ptr<TerrainQuadTree> octree_; ///< Octreeのポインタ


	/* ----- edit ----- */
	std::vector<std::pair<size_t, TerrainVertex*>> editVertices_;



	/* ----- splatting ----- */

	std::array<std::string, SPLAT_TEX_COUNT> splattingTexPaths_;


public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	/// @brief 頂点を二次元配列化したものを取得する
	/// @return 二次元配列にした頂点データのスパン
	const std::span<std::span<TerrainVertex>>& GetVertexSpan() const;

	const std::vector<TerrainVertex>& GetVertices() const;
	std::vector<TerrainVertex>& GetVertices();
	const std::vector<uint32_t>& GetIndices() const;

	const std::vector<std::pair<size_t, TerrainVertex*>>& GetEditVertices();

	const std::array<std::string, SPLAT_TEX_COUNT>& GetSplatTexPaths() const;

	//TerrainQuadTree* GetQuadTree();
};


namespace COMP_DEBUG {
	void TerrainDebug(Terrain* _terrain);
} // namespace COMP_DEBUG
