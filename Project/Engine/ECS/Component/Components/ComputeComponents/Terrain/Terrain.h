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

	/* ----- buffer ----- */
	StructuredBuffer<TerrainVertex> rwVertices_;
	StructuredBuffer<uint32_t> rwIndices_;
	bool isCreated_;

	/* ----- edit ----- */
	float brushRadius_;
	float brushStrength_;

	/* ----- terrain ----- */
	std::vector<uint32_t> indices_; ///< インデックスデータ

	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ
	uint32_t maxVertexNum_;
	uint32_t maxIndexNum_;

	/* ----- splatting ----- */
	std::array<std::string, SPLAT_TEX_COUNT> splattingTexPaths_;


	/* ----- flags ----- */

	bool isRenderingProcedural_;

public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	const std::vector<uint32_t>& GetIndices() const;

	const std::array<std::string, SPLAT_TEX_COUNT>& GetSplatTexPaths() const;

	/* ----- buffer ----- */

	StructuredBuffer<TerrainVertex>& GetRwVertices();
	StructuredBuffer<uint32_t>& GetRwIndices();

	void SetIsCreated(bool _isCreated);
	bool GetIsCreated() const;

	uint32_t GetMaxVertexNum();
	uint32_t GetMaxIndexNum();

	const Vector2& GetSize() const;

	/* ----- edit ----- */

	float GetBrushRadius() const;
	void SetBrushRadius(float _radius);

	float GetBrushStrength() const;
	void SetBrushStrength(float _strength);


	/* ----- flags ----- */
	bool GetIsRenderingProcedural() const;
	void SetIsRenderingProcedural(bool _isRenderingProcedural);

};


namespace COMP_DEBUG {
	void TerrainDebug(Terrain* _terrain);
} // namespace COMP_DEBUG



void from_json(const nlohmann::json& _j, Terrain& _t);
void to_json(nlohmann::json& _j, const Terrain& _t);
