#pragma once

/// std
#include <vector>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

class KeyItemCylinderEffect : public IEntity {
public:
	KeyItemCylinderEffect();
	~KeyItemCylinderEffect();

	void Initialize() override;
	void Update()     override;

private:
	struct ParticleElement {
		float rotateSpeed;
	};

	std::vector<ParticleElement> particleElements_;

};

