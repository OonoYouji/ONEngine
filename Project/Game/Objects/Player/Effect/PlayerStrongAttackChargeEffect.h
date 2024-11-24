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

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_ = nullptr;

};
