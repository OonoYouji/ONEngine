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
	//mesh
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("TestObject");

}

void Player::Update() {
	//入力
	velocity_ = {
			static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
			static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
			0.0f
	};
	////
	//rotateXAngle_ += velocity_.y * 0.01f;
	//rotateYAngle_ += velocity_.x * 0.01f;

	//回転を適応
	rotateX_ = MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -velocity_.y * 0.1f);
	rotateY_ = MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, velocity_.x * 0.1f);
	pivot_.quaternion *= rotateX_ * rotateY_;// 正規化

	pivot_.UpdateMatrix();

}

void Player::Debug() {
	if(ImGui::TreeNode("pivot")) {
		pivot_.Debug();
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateX_.x, rotateX_.y, rotateX_.z, rotateX_.w);
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateY_.x, rotateY_.y, rotateY_.z, rotateY_.w);
		ImGui::DragFloat3("velocity",&velocity_.x,0);
		ImGui::TreePop();
	}
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