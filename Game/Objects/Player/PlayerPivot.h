#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

#include"Player/Player.h"

class PlayerPivot : public BaseGameObject {
public:

	PlayerPivot() { CreateTag(this); }
	~PlayerPivot() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;



	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

private:
	Vec3 velocity;
	Quaternion rotateX;
	Quaternion rotateY;
	float rotateXAngle;
	float rotateYAngle;
	AudioSource* audioSource_ = nullptr;
};