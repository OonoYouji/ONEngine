#pragma once

/// std
#include <vector>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Terrain.h"

/// ///////////////////////////////////////////////////
/// 地形のコライダーコンポーネント
/// ///////////////////////////////////////////////////
class TerrainCollider : public IComponent {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainCollider();
	~TerrainCollider() override = default;

	/// @brief 地形の頂点情報をコピーする
	void CopyVertices();

private:
	/// =========================================
	/// private : objects
	/// =========================================

	Terrain* pTerrain_;

	std::vector<std::vector<TerrainVertex>> vertices_;

};

namespace COMP_DEBUG {
	void TerrainColliderDebug(TerrainCollider* _collider);
}

void from_json(const nlohmann::json& _j, TerrainCollider& _c);
void to_json(nlohmann::json& _j, const TerrainCollider& _c);