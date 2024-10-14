#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitlePlayerAnimator final : public BaseGameObject {
public:

	TitlePlayerAnimator() { CreateTag(this); }
	~TitlePlayerAnimator() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	Vec3 basePosition_;

	float animationTime_;
	float animationSpeed_;
	float animationAmplitude_;

};
