#pragma once

#include "Engine/ECS/System/Interface/ECSISystem.h"

/// ///////////////////////////////////////////////
/// 地形との当たり判定、押し戻しをするクラス
/// ///////////////////////////////////////////////
class TerrainCollisionSystem : public ECSISystem {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainCollisionSystem() = default;
	~TerrainCollisionSystem() = default;

	void Update(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class Terrain* pTerrain_ = nullptr;
};



