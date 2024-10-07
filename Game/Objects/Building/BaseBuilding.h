#pragma once

#include <GameObjectManager.h>
#include <Component/Transform/Transform.h>
#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

class BaseBuilding : public BaseGameObject {
public:

	BaseBuilding() { CreateTag(this); }
	~BaseBuilding() {}

	virtual	void Initialize()override;
	virtual	void Update() override;

	virtual void Debug() override;

	//getter
	bool GetIsDeath()const { return isDeath_; }
	//setter
	void SetPivot(Transform pivot) { pivot_ = pivot; }
	void SetPivotQuaternion(Quaternion q) { pivot_.quaternion *= q; }
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);


private:
	//速度
	Vec3 velocity_;
	//クォータニオンRotate
	Quaternion rotateX_;
	Quaternion rotateY_;

	bool isDeath_;
	Transform pivot_;
	AudioSource* audioSource_ = nullptr;
};