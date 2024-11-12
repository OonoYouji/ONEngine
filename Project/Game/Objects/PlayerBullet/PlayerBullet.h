#pragma once

#include "GameObjectManager/GameObjectManager.h"

class PlayerBullet : public BaseGameObject {
public:

	PlayerBullet(class Player* _player, class DefeatedEnemy* _defeatedEnemy);
	~PlayerBullet();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void OnCollisionEnter(BaseGameObject* const collision) override;


	void SetVelocity(const Vec3& _velocity);
private:
	class MeshRenderer* meshRenderer_ = nullptr;

	Vec3 velocity_;

	float lifeTime_ = 3.0f;

	class Player* pPlayer_ = nullptr;
	class DefeatedEnemy* pDefeatedEnemy_ = nullptr;
};
