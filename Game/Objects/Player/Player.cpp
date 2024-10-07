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
	//pivot_=AddComponent<Transform>();

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->Initialize();
	pivot_.Initialize();
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	pTransform_->position.z = -12;

	pTransform_->quaternion = { 0,0,0,1 };
	/*SetPositionZ(-1.0f);*/
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	pTransform_->SetParent(&pivot_);

	pivot_.UpdateMatrix();
	UpdateMatrix();
}

void Player::Update() {
	//入力
	velocity_ = {
		static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
		static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
		0.0f
	};

	velocity_ = velocity_.Normalize();

	/// 仮にvelocityをマイナスにして移動の違和感を消す
	velocity_ = -velocity_;

	rotateXAngle_ += velocity_.y * 0.1f;
	rotateYAngle_ += velocity_.x * 0.1f;


	//回転を適応
	rotateX_ = MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, rotateXAngle_);
	rotateY_ = MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, rotateYAngle_);
	pivot_.quaternion = rotateX_ * rotateY_;// 正規化

	// プレイヤーの向きの決定
	pTransform_->quaternion = MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, std::atan2(-velocity_.x, velocity_.y));


	pivot_.UpdateMatrix();

}

void Player::Debug() {
	if(ImGui::TreeNode("pivot")) {
		pivot_.Debug();
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateX_.x, rotateX_.y, rotateX_.z, rotateX_.w);
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateY_.x, rotateY_.y, rotateY_.z, rotateY_.w);
		ImGui::DragFloat3("velocity",&velocity_.x,0);

		ImGui::DragFloat("rotateXAngle", &rotateXAngle_, 0.01f);
		ImGui::DragFloat("rotateYAngle", &rotateYAngle_, 0.01f);

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