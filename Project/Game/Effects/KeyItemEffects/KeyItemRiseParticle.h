#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class KeyItemRiseParticle : public IEntity {
public:
	KeyItemRiseParticle();
	~KeyItemRiseParticle();

	void Initialize() override;
	void Update() override;
};

