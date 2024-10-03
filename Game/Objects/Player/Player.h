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
private:
	Transform baseTransform_;
	float moveSpeed = 0.05f;  // 移動速度
	float rotationSpeed = 0.01f;  // 回転速度
	AudioSource* audioSource_ = nullptr;

};