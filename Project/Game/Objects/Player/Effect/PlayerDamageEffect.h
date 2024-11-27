#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerDamageEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerDamageEffect();
	~PlayerDamageEffect();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void AddVariables();
	void ApplyVariables();

	void Start(float _time, float _damage);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

	float alpha_ = 1.0f;
	float maxAlpha_;

	float maxDamageEffect_;
	float minDamageEffect_;

	float maxTime_;
	float currentTime_;

};
