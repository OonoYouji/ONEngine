#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerBulletHitEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBulletHitEffect();
	~PlayerBulletHitEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_;

};
