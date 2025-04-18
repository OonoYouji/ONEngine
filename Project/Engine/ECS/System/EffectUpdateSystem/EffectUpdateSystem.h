#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// /////////////////////////////////////////////////
/// EffectUpdateSystem(エフェクトの更新システム)
/// /////////////////////////////////////////////////
class EffectUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EffectUpdateSystem() = default;
	~EffectUpdateSystem() = default;

	void Update(EntityComponentSystem* _pEntityComponentSystem) override;

private:
	/// ==================================================
	/// private : objects
	/// ==================================================



};

