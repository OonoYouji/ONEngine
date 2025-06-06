#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class LaserHitEffect : public IEntity {
public:
	LaserHitEffect() = default;
	~LaserHitEffect() override = default;

	void Initialize() override;
	void Update() override;
};

class LaserExplosionEffect : public IEntity {
public:
	LaserExplosionEffect() = default;
	~LaserExplosionEffect() override = default;

	void Initialize() override;
	void Update() override;
};