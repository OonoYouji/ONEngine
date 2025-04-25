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
	TerrainEditor(Terrain* _terrain);
	~TerrainEditor();

	void Initialize();
	void Update();

private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	Terrain* terrain_ = nullptr; ///< Terrainへのポインタ

	std::vector<Mesh::VertexData> points_; ///< Terrainの点のリスト

};

