#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerStrongAttackChargeEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerStrongAttackChargeEffect();
	~PlayerStrongAttackChargeEffect();

	void Initialize() override;
	void Update()     override;

	void SetTimeRate(float _timeRate);
	void SetAnimationActive(bool _isActive);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class AnimationRenderer* animationRenderer_ = nullptr;

};
