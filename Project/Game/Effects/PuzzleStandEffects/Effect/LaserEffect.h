#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class LaserEffect : public IEntity {
public:
	LaserEffect() = default;
	~LaserEffect() override = default;

	void Initialize() override;
	void Update() override;
};

class LaserShootEffect : public IEntity {
public:
	LaserShootEffect() = default;
	~LaserShootEffect() override = default;
	
	void Initialize() override;
	void Update() override;
};