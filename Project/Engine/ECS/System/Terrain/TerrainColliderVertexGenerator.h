#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// 地形コライダーの頂点生成システム
/// ///////////////////////////////////////////////////
class TerrainColliderVertexGenerator : public ECSISystem {
public:
	/// ========================================
	/// public : methods
	/// ========================================

	TerrainColliderVertexGenerator(class DxManager* _dxManager);
	~TerrainColliderVertexGenerator() override = default;

	void OutsideOfRuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class GameEntity*>& _entities) override;
	void RuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class GameEntity*>& _entities) override;

private:
	/// ========================================
	/// private : objects
	/// ========================================

	class DxManager* pDxManager_;

};

