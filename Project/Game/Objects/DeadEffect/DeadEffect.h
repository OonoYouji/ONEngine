#pragma once

/// std
#include <vector>

/// base class
#include "GameObjectManager/GameObjectManager.h"



class DeadEffect : public BaseGameObject {

	struct ParticleData {
		Vec3 position;
		float scaleScaler;
	};


public:

	DeadEffect();
	~DeadEffect();

	void Initialize() override;
	void Update()     override;

private:
	class ParticleSystem*     particleSyste_;
	std::vector<ParticleData> particleDataArray_;
};
