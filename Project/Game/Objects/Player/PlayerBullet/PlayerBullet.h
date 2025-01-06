#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerBullet : public BaseGameObject {
public:

	enum State {
		State_Idel,
		State_Move,
		State_Max
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerBullet();
	~PlayerBullet();

	void Initialize() override;
	void Update()     override;

	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) override;


	void Idel();
	void Move();

	void Fire(class Enemy* _target);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer*   meshRenderer_ = nullptr;
	class MeshRenderer*   locusRenderer_ = nullptr;
	class SphereCollider* collider_ = nullptr;

	int stateIndex_;

	class Enemy* targetEnemy_ = nullptr;
	Vec3 direction_ = {};

	bool isHitEnemy_ = false;

	Vec3 idelPosition_  = {};

	float lifeTime_ = 5.0f;

public:

	bool GetIsHitEnemy() const { return isHitEnemy_; }

	void SetIdelPosition(const Vec3& _position) { idelPosition_ = _position; }

};
