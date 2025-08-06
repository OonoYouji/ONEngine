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
	friend class TerrainColliderVertexGenerator;
public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainCollider();
	~TerrainCollider() override = default;

	void AttachTerrain();

	/// @brief 地形の頂点情報をコピーする
	void CopyVertices(class DxManager* _dxManager);

private:
	/// =========================================
	/// private : objects
	/// =========================================

	Terrain* pTerrain_;

	std::vector<std::vector<TerrainVertex>> vertices_;

	bool isVertexGenerationRequested_;


public:
	/// =========================================
	/// public : accessor
	/// =========================================

	Terrain* GetTerrain() const;

	const std::vector<std::vector<TerrainVertex>>& GetVertices() const;
	std::vector<std::vector<TerrainVertex>>& GetVertices();


	void SetIsVertexGenerationRequested(bool _isRequested);

};

namespace COMP_DEBUG {
	void TerrainColliderDebug(TerrainCollider* _collider);
}

void from_json(const nlohmann::json& _j, TerrainCollider& _c);
void to_json(nlohmann::json& _j, const TerrainCollider& _c);