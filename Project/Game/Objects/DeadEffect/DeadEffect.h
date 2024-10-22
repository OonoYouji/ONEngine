#pragma once

#include "GameObjectManager/GameObjectManager.h"

class DeadEffect : public BaseGameObject {

	struct ParticleData {
		Vec3 moveDirection;
		float speed;
	};


public:

	DeadEffect();
	~DeadEffect();

	void Initialize() override;
	void Update()     override;

private:
	class ParticleSystem* particleSyste_;
};
