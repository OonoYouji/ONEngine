#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// 地形との衝突判定、押し戻しのシステム
/// ///////////////////////////////////////////////////
class TerrainCollision : public ECSISystem {
public:
	/// ========================================
	/// public : methods
	/// ========================================

	TerrainCollision() = default;
	~TerrainCollision() override = default;

	void OutsideOfRuntimeUpdate(class EntityComponentSystem* _ecs) override;
	void RuntimeUpdate(class EntityComponentSystem* _ecs) override;

private:
	/// ========================================
	/// private : objects
	/// ========================================

};

