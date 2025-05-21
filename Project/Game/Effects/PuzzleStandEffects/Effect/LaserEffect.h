#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class LaserEffect : public IEntity {
public:
	LaserEffect() = default;
	~LaserEffect() override = default;

	void Initialize() override;
	void Update() override;
};

