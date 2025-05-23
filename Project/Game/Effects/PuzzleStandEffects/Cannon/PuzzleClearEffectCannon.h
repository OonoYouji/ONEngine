#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class PuzzleClearEffectCannon : public IEntity {
public:
	PuzzleClearEffectCannon() = default;
	~PuzzleClearEffectCannon() override = default;

	void Initialize() override;
	void Update() override;
};

