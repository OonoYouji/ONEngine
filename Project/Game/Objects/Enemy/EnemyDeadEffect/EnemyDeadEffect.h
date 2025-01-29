#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EnemyDeadEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : nest class
	/// ===================================================

	struct ParticleData {
		Vec3 velocity;
		Vec3 rotateValue;
		Vec3 scale;
	};


public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyDeadEffect();
	~EnemyDeadEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const uint32_t kMaxParticleCount_ = 128u;

	class ParticleSystem*     particleSystem_ = nullptr;
	std::vector<ParticleData> particles_;

	float  thisLifeTime_;
	bool   isDestroy_;
	size_t particleDeadCount_;

};
