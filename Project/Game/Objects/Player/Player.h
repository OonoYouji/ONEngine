#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Player : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Player();
	~Player();

	void Initialize() override;
	void Update()     override;


	void AddVariables();
	void ApplyVariables();

	void Movement();

	void Fire();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

	class PlayerBulletRenderer* bulletRenderer_ = nullptr;
	class PlayerNormalEffect*   normalEffect_   = nullptr;


	/// parameters

	Vec3 direction_;
	Vec3 velocity_;
	float movementSpeed_;

	float bulletSpeed_;
	float bulletLifeTime_;
};
