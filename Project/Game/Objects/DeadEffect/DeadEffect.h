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
	void Debug()      override;

	void SetIsStart(bool _isStart);

	bool IsFinished() const { return endTime_ <= 0.0f; }

private:
	class ParticleSystem*     particleSyste_;
	std::vector<ParticleData> particleDataArray_;

	bool isStart_ = false;
	float endTime_ = 3.0f;
};
