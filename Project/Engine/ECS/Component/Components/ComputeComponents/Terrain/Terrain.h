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
#include "River/River.h"


/// COMP_DEBUGで使用するための前方宣言
class Terrain;
class EntityComponentSystem;
class AssetCollection;


static const uint32_t kMaxTerrainTextureNum = 4u;



namespace COMP_DEBUG {
	void TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, AssetCollection* _assetCollection);
	
	/// テクスチャモードの編集
	bool TerrainTextureEditModeDebug(std::array<std::string, kMaxTerrainTextureNum>* _texturePaths, int32_t _usedTextureIndex, AssetCollection* _assetCollection);

} // namespace COMP_DEBUG


/// ///////////////////////////////////////////////////
/// 地形のコンポーネント
/// ///////////////////////////////////////////////////
class Terrain : public IComponent {
	friend void COMP_DEBUG::TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, AssetCollection* _assetCollection);
	friend void from_json(const nlohmann::json& _j, Terrain& _t);
	friend void to_json(nlohmann::json& _j, const Terrain& _t);
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


	enum class EditMode : int32_t {
		None,    /// 操作なし
		Vertex,	 /// 勾配の操作
		Texture, /// テクスチャの操作
		Count
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

	/// ----- buffer ----- ///
	StructuredBuffer<TerrainVertex> rwVertices_;
	StructuredBuffer<uint32_t> rwIndices_;
	bool isCreated_;

	/// ----- edit ----- ///
	float brushRadius_;
	float brushStrength_;
	int32_t editMode_;
	int32_t usedTextureIndex_;

	/// ----- terrain ----- ///
	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ
	uint32_t maxVertexNum_;
	uint32_t maxIndexNum_;

	/// ----- river ----- ///
	River river_;

	/// ----- splatting ----- ///
	std::array<std::string, kMaxTerrainTextureNum> splattingTexPaths_;

	/// ----- flags ----- ///
	bool isRenderingProcedural_;


public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	const std::array<std::string, kMaxTerrainTextureNum>& GetSplatTexPaths() const;

	/// ----- buffer ----- ///
	StructuredBuffer<TerrainVertex>& GetRwVertices();
	StructuredBuffer<uint32_t>& GetRwIndices();

	void SetIsCreated(bool _isCreated);
	bool GetIsCreated() const;

	uint32_t GetMaxVertexNum();
	uint32_t GetMaxIndexNum();

	const Vector2& GetSize() const;

	/// ----- edit ----- ///
	float GetBrushRadius() const;
	void SetBrushRadius(float _radius);

	float GetBrushStrength() const;
	void SetBrushStrength(float _strength);


	/// ----- river ----- ///
	River* GetRiver();

	/// ----- flags ----- ///
	bool GetIsRenderingProcedural() const;
	void SetIsRenderingProcedural(bool _isRenderingProcedural);

};



void from_json(const nlohmann::json& _j, Terrain& _t);
void to_json(nlohmann::json& _j, const Terrain& _t);
