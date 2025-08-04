#pragma once

/// std
#include <vector>
#include <array>
#include <span>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

#include "TerrainVertex.h"


/// ///////////////////////////////////////////////////
/// 地形のコンポーネント
/// ///////////////////////////////////////////////////
class Terrain : public IComponent {
public:
	/// =========================================
	/// public : sub class
	/// =========================================

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

	StructuredBuffer<TerrainVertex> rwVertices_;
	StructuredBuffer<uint32_t> rwIndices_;
	bool isCreated_;

	/* ----- terrain ----- */
	std::vector<TerrainVertex> vertices_; ///< 頂点データ
	std::vector<uint32_t> indices_; ///< インデックスデータ

	std::span<std::span<TerrainVertex>> vertexSpan_; ///< 頂点データのスパン

	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ

	uint32_t maxVertexNum_;
	uint32_t maxIndexNum_;

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

	/* ----- buffer ----- */

	StructuredBuffer<TerrainVertex>& GetRwVertices();
	StructuredBuffer<uint32_t>& GetRwIndices();

	void SetIsCreated(bool _isCreated);
	bool GetIsCreated() const;

	uint32_t GetMaxVertexNum();
	uint32_t GetMaxIndexNum();

	const Vector2& GetSize() const;

};


namespace COMP_DEBUG {
	void TerrainDebug(Terrain* _terrain);
} // namespace COMP_DEBUG



void from_json(const nlohmann::json& _j, Terrain& _t);
void to_json(nlohmann::json& _j, const Terrain& _t);
