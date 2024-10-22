#pragma once

#include "GameObjectManager/GameObjectManager.h"

class PlayerBullet : public BaseGameObject {
public:

	PlayerBullet();
	~PlayerBullet();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetVelocity(const Vec3& _velocity);
private:
	class MeshRenderer* meshRenderer_ = nullptr;

	Vec3 velocity_;

	float lifeTime_ = 10.0f;
};
