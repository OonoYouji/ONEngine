#pragma once

#include "GameObjectManager/BaseGameObject.h"

#include <string>

#include "ComponentManager/Collider/SphereCollider.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

enum class BulletType{
	RANGED_ATTACK_BULLET,
	LONG_RANGE_BULLET,

	COUNT
};

class IEnemyBullet : public BaseGameObject{
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	IEnemyBullet(BulletType type);
	~IEnemyBullet();

	void Initialize() override;
	void Update()     override;

	void OnCollisionEnter(BaseGameObject* const _collision);
protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================
	SphereCollider* sphereCollider_;
	AnimationRenderer* render_;

	BulletType type_;

	float lifeLeftTime_;

	float damage_;
public:
	float GetLifeLeftTime()const{ return lifeLeftTime_; }
	void SetLifeTime(float lifeTime){ lifeLeftTime_ = lifeTime; }
	void SetDamage(float damage){ damage_ = damage; }
};
