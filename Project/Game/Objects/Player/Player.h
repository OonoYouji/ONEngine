#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"
#include "ComponentManager/AudioSource/AudioSource.h"


class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

	//getter
	Transform* GetPivot() { return &pivot_; }
	Transform* GetbaseTransform() { return &transoform_; }

	const Vec3& GetVelocity() const { return velocity_; }

private:
	//速度
	Vec3 input_;
	Vec3 velocity_;
	//クォータニオンRotate
	Quaternion rotateX_;
	Quaternion rotateY_;
	//ピボット
	Transform pivot_;
	Transform transoform_;

	AudioSource* audioSource_ = nullptr;

	float rotateXAngle_;
	float rotateYAngle_;
	
};