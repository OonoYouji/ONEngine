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


private:
	/// ===================================================
	/// private : objects
	/// ===================================================


};

