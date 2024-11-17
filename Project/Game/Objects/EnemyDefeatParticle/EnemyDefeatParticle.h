#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EnemyDefeatParticle : public BaseGameObject {

	struct ParticleData {
		Vec3 direction;
		float speed;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyDefeatParticle();
	~EnemyDefeatParticle();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	class ParticleSystem* particleSystem_ = nullptr;

	std::vector<ParticleData> particleDataArray_;

	float lifeTime_;
};
