#pragma once

/// edit target
#include "../Terrain.h"

/// /////////////////////////////////////////////////
/// Terrain のエディタクラス
/// /////////////////////////////////////////////////
class TerrainEditor final {
public:
	/// ==========================================
	/// public : methods
	/// ==========================================
	TerrainEditor(Terrain* _terrain, EntityComponentSystem* _ecs);
	~TerrainEditor();

	void Initialize();
	void Update();


private:
	/// ==========================================
	/// private : methods
	/// ==========================================

	Vector3 CalculateMouseRayDirection(const Vector2& _mousePosition) const;


private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	EntityComponentSystem* pECS_;
	Terrain* pTerrain_ = nullptr; ///< Terrainへのポインタ

	/* --- terrain --- */

	std::vector<Mesh::VertexData> points_; ///< Terrainの点のリスト

	/* --- input --- */
	Vector2 mousePosition_ = Vector2(0.0f, 0.0f); ///< マウスの位置
};

