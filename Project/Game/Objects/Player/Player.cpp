#include "Player.h"


#include "GraphicManager/ModelManager/ModelManager.h"

#include <Input/Input.h>
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/ParticleSystem/ParticleSystem.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"


void Player::Initialize() {
	Model* model = ModelManager::Load("axis");
	//mesh
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	transoform_.Initialize();
	pivot_.Initialize();
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	transoform_.quaternion = { 0,0,0,1 };
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->position.z = -12;
	transoform_.position.z=-12;
	/*SetPositionZ(-1.0f);*/
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////
	transoform_.rotateOrder = QUATERNION;
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	transoform_.SetParent(&pivot_);
	pTransform_->SetParent(&pivot_);

	pivot_.UpdateMatrix();
	UpdateMatrix();
}

void Player::Update() {
	//入力
	input_ = {
		static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
		static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
		0.0f
	};

	/// 移動の正規化
	input_    = input_.Normalize() * 0.1f;
	velocity_ = Vec3::Lerp(velocity_, input_, 0.1f);

	rotateXAngle_ = +velocity_.y;
	rotateYAngle_ = -velocity_.x;

	if(velocity_ != Vec3(0.0f, 0.0f, 0.0f)) {

		//回転を適応
		rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, rotateXAngle_);
		rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, rotateYAngle_);

		// プレイヤーの向きの決定
		Quaternion quaternionLocalZ = Quaternion::MakeFromAxis({ 0.0f, 0.0f, 1.0f }, std::atan2(velocity_.x, velocity_.y));

		pivot_.quaternion *= rotateX_ * rotateY_;// 正規化
		pTransform_->quaternion = quaternionLocalZ.Conjugate();

		pivot_.Update();
		transoform_.Update();
	}


}

void Player::Debug() {
	if(ImGui::TreeNode("pivot")) {
		pivot_.Debug();
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateX_.x, rotateX_.y, rotateX_.z, rotateX_.w);
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateY_.x, rotateY_.y, rotateY_.z, rotateY_.w);
		ImGui::DragFloat3("velocity", &velocity_.x, 0);

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