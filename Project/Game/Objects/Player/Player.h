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

	void OnCollisionEnter(BaseGameObject* const _collision) override;

	void AddVariables();
	void ApplyVariables();

	void Movement();

	void Fire();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer*   meshRenderer_ = nullptr;
	class SphereCollider* SphereCollider_  = nullptr;

	class PlayerBulletRenderer* bulletRenderer_ = nullptr;
	class PlayerNormalEffect*   normalEffect_   = nullptr;
	class TopDownCamera*        pTopDownCamera_ = nullptr;

	/// parameters

	Vec3 direction_;
	Vec3 velocity_;
	float movementSpeed_;

	float bulletSpeed_;
	float bulletLifeTime_;

	float colliderRadius_ = 2.0f;

	Vec3 currentDiff_, prevDiff_;

public:

	float GetColliderRadius() const { return colliderRadius_; }


	void SetTopDownCamera(class TopDownCamera* _camera);
};
