#include "PlayerPivot.h"

#include <ModelManager.h>

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
#include <Component/Collider/SphereCollider.h>
#include <Component/SplinePathRenderer/SplinePathRenderer.h>

#include <ImGuiManager.h>


void PlayerPivot::Initialize() {
	
	pTranform_->rotateOrder = QUATERNION;
	pTranform_->quaternion = { 1,0,0,0 };
	

	//auto particle = AddComponent<ParticleSystem>(12, "Sphere");
	//AddComponent<SphereCollider>(ModelManager::Load("Sphere"));
	pTranform_->scale = { 11,11,11 };
	SetPositionZ(10.0f);
	UpdateMatrix();
}

void PlayerPivot::Update() {

	velocity = {
				static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
				static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
				0.0f
	};

	rotateXAngle += velocity.y * 0.01f;
	rotateYAngle += velocity.x * 0.01f;

	rotateX = MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -velocity.y * 0.1f);
	rotateY = MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, velocity.x * 0.1f);
	pTranform_->quaternion *= rotateX * rotateY;// 正規化

}



void PlayerPivot::Debug() {

}

Quaternion PlayerPivot::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	// ラジアンに変換
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sin(halfAngle);

	// 正規化された軸ベクトル
	Vector3 normalizedAxis = axis.Normalize();

	// クォータニオンの成分を計算
	float w = cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	// Vector4 として返す
	return Quaternion(x, y, z, w);
}