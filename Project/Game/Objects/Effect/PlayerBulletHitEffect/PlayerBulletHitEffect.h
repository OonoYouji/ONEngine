#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerBulletHitEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBulletHitEffect(class PlayerBullet* _playerBullet);
	~PlayerBulletHitEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_ = nullptr;
	class MeshRenderer*   meshRenderer_   = nullptr;
	class PlayerBullet*   pPlayerBullet_  = nullptr;

	float lifeTime_ = 1.0f;

	bool isAlive_ = true;

public:

	bool GetIsAlive() const { return isAlive_; }
};
