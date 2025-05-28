#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class PuzzleClearEffectCannonStand : public IEntity {
public:
	PuzzleClearEffectCannonStand() = default;
	~PuzzleClearEffectCannonStand() override = default;

	void Initialize() override;
	void Update() override;
};

