#pragma once

#include "GameObjectManager/BaseGameObject.h"

class RotationCube : public BaseGameObject {

	struct ParticleData {
		Vec3  offset = Vec3(0,0,0);
		float phase;
		float angle;
		float rotationSpeed;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	RotationCube();
	~RotationCube();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_ = nullptr;
	std::vector<ParticleData> particleDatas_;

	float animationTime_ = 0.0f;
};
