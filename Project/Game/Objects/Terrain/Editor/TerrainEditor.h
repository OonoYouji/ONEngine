#pragma once

/// std
#include <vector>
#include <functional>

/// edit target
#include "../Terrain.h"
#include "../TerrainVertex.h"

/// /////////////////////////////////////////////////
/// Terrain のエディタクラス
/// /////////////////////////////////////////////////
class TerrainEditor final {
public:

	enum EDIT_MODE {
		EDIT_MODE_NONE,
		EDIT_MODE_TERRAIN_VERTEX,
		EDIT_MODE_SPLAT_BLEND,
	};

	enum SPLAT_BLEND_MODE {
		BLEND_MODE_GRASS,
		BLEND_MODE_DIRT,
		BLEND_MODE_ROCK,
		BLEND_MODE_SNOW,
	};


public:
	/// ==========================================
	/// public : methods
	/// ==========================================
	TerrainEditor(Terrain* _terrain, EntityComponentSystem* _ecs);
	~TerrainEditor();

	void Initialize();
	void Update();

	void RecalculateNormal();

	void OutputVertices(const std::string& _filePath);

	void EditTerrainVertex();
	void EditSplatBlend();

private:
	/// ==========================================
	/// private : methods
	/// ==========================================

	Vector3 CalculateMouseRayDirection(const Vector2& _mousePosition) const;

	Vector3 CalculateMouseNearPoint(const Vector2& _mousePosition) const;
	Vector3 CalculateMouseFarPoint(const Vector2& _mousePosition) const;

private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	EntityComponentSystem* pECS_;
	Terrain* pTerrain_ = nullptr; ///< Terrainへのポインタ

	int editMode_;
	std::vector<std::function<void()>> editFunctions_; ///< 編集モードの関数リスト
	int editSplatBlendMode_ = 0; ///< 編集モードのインデックス


	/* --- terrain --- */

	std::vector<TerrainVertex> points_; ///< Terrainの点のリスト
	std::vector<std::pair<size_t, TerrainVertex*>> editedVertices_; ///< 選択された点のリスト

	/* --- input --- */
	Vector2 mousePosition_ = Vector2(0.0f, 0.0f); ///< マウスの位置
	Vector3 mouseNearPos_ = Vector3(0.0f, 0.0f, 0.0f); ///< マウスの近点
	Vector3 mouseFarPos_ = Vector3(0.0f, 0.0f, 0.0f); ///< マウスの遠点
	
	/* --- edit param --- */
	float editRadius_ = 1.0f; ///< 編集する半径

};

