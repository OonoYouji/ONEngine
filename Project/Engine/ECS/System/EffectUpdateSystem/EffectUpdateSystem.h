#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"

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

	void Update(class EntityComponentSystem* _pEntityComponentSystem) override;

private:
	/// ==================================================
	/// private : objects
	/// ==================================================


private:
	/// ==================================================
	/// private : methods
	/// ==================================================

	/// @brief エフェクトの要素を更新する
	void UpdateElement(Effect* _effect, Effect::Element* _element);


};

