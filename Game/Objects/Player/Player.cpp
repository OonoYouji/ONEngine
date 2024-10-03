#include "Player.h"


#include <ModelManager.h>

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
#include <Component/Collider/SphereCollider.h>
#include <Component/SplinePathRenderer/SplinePathRenderer.h>

#include <ImGuiManager.h>


void Player::Initialize() {
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("TestObject");
	//meshRenderer->SetMaterial("uvChecker");

	//auto particle = AddComponent<ParticleSystem>(12, "Sphere");
	//AddComponent<SphereCollider>(ModelManager::Load("Sphere"));

	pTranform_->scale = { 0.1f,0.1f,0.1f };
	baseTransform_.Initialize();
	SetPositionZ(-1.0f);
	UpdateMatrix();
}

void Player::Update() {

	/*Vec3 velocity{
		static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
		static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
		0.0f
	};
	Vec2 keyboardDir = { 0.0f, 0.0f };

	// キー入力に基づいて方向を取得
	if (Input::PressKey(KeyCode::A)) {
		keyboardDir.x -= 1.0f; // 左
	}
	if (Input::PressKey(KeyCode::W)) {
		keyboardDir.y -= 1.0f; // 前
	}
	if (Input::PressKey(KeyCode::D)) {
		keyboardDir.x += 1.0f; // 右
	}
	if (Input::PressKey(KeyCode::S)) {
		keyboardDir.y += 1.0f; // 下
	}

	float moveSpeed = 1.0f;
	
		// AまたはDが押されたときの回転
	Quaternion quaternionY = MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, keyboardDir.x * moveSpeed);
	
		// WまたはSが押されたときの回転
	Quaternion quaternionX = MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, keyboardDir.y * moveSpeed);
	
	pTranform_->quaternion *= quaternionX*quaternionY;
	pTranform_->quaternion.Normalize(pTranform_->quaternion);  // 正規化
	/*pTranform_->matTransform = Matrix4x4::MakeRotateQuaternion(pTranform_->quaternion.Normalize(pTranform_->quaternion));
	*/
	//SetPosition(GetPosition() + velocity * 0.25f);

}

void Player::Debug() {

}



Quaternion Player::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	// ���W�A���ɕϊ�
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sin(halfAngle);

	// ���K�����ꂽ���x�N�g��
	Vector3 normalizedAxis = axis.Normalize();

	// �N�H�[�^�j�I���̐�����v�Z
	float w = cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	// Vector4 �Ƃ��ĕԂ�
	return Quaternion(x, y, z, w);
}