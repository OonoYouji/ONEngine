#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;

	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

	//getter
	Transform* GetPivot() { return &pivot_; }

	const Vec3& GetVelocity() const { return velocity_; }

private:
	//速度
	Vec3 velocity_;
	//クォータニオンRotate
	Quaternion rotateX_;
	Quaternion rotateY_;
	//ピボット
	Transform pivot_;
	
	AudioSource* audioSource_ = nullptr;

	float rotateXAngle_;
	float rotateYAngle_;
	//float rotationSpeed = 0.01f;  // 回転速度
	//float moveSpeed = 0.05f;  // 移動速度


};