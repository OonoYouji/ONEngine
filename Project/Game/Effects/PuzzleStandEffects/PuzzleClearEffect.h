#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// //////////////////////////////////////////////////
/// パズルのクリアエフェクト
/// //////////////////////////////////////////////////
class PuzzleClearEffect : public IEntity {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PuzzleClearEffect();
	~PuzzleClearEffect() override;

	void Initialize() override;
	void Update() override;

	void Reset();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class PuzzleClearEffectCannon* cannon_;
	class LaserEffect* laser_;

	class IEntity* hitEffects_[2];

};

