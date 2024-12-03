#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerNormalEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerNormalEffect();
	~PlayerNormalEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_;

};
