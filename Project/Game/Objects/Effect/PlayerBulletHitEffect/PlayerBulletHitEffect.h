#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerBulletHitEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBulletHitEffect(class PlayerBullet* _playerBullet, class TopDownCamera* _topDownCamera);
	~PlayerBulletHitEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_ = nullptr;
	class MeshRenderer*   meshRenderer_   = nullptr;
	class MeshRenderer*   explosionRenderer_   = nullptr;
	class PlayerBullet*   pPlayerBullet_  = nullptr;

	float lifeTime_ = 1.0f;

	bool isAlive_ = true;

	class TopDownCamera* pTopDownCamera_ = nullptr;

public:

	bool GetIsAlive() const { return isAlive_; }
};
