#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitlePlayerAnimator final : public BaseGameObject {

	struct WindAnimationData {
		float time  = 0.0f;
		float speed = 1.0f;
	};

	struct FlyingObjectData {
		float time;
		float spped;
		float radius;
	};

public:

	TitlePlayerAnimator() { CreateTag(this); }
	~TitlePlayerAnimator() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void SetBasePosition(const Vec3& _basePosition);

private:

	Vec3 basePosition_;

	float animationTime_;
	float animationSpeed_;
	float animationAmplitude_;

	bool isSpin_     = false;
	float spinTime_  = 0.0f;
	float spinSpeed_ = 1.0f;

	std::vector<class Wind*>       windArray_;
	std::vector<WindAnimationData> windAnimationDataArray_;
	std::vector<FlyingObjectData> flyingObejctDataArray_;
};
