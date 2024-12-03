#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerNormalEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerNormalEffect(class Player* _player);
	~PlayerNormalEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_;
	class Player*         pPlayer_ = nullptr;

};
