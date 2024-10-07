#include "GameCameraState.h"

#include <algorithm>
#include <numbers>

#include "ImGuiManager.h"
#include "Player/Player.h"


void GameCameraState::Initialize() {

	/// transform initialize
	pTransform_->position    = { 0.0f, 0.0f, -50.0f };
	pTransform_->quaternion  = { 0.0f, 0.0f, 0.0f, 1.0f };
	pTransform_->rotateOrder = QUATERNION;
	pTransform_->Update();
}

void GameCameraState::Update() {

	Transform* parent = GetParent();
	if(parent) {

		/// playerの前方ベクトル計算
		playerForward_ = Mat4::TransformNormal(Vec3::kUp, pPlayer_->GetMatTransform());

		/// cameraのquaternion計算 その1
		pGameCamera_->UpdateMatrix();
		Quaternion quaternionXY = Quaternion::LockAt(pGameCamera_->GetPosition(), parent->position, Vec3::kUp);

		/// カメラの上方向のベクトル計算
		cameraUp_ = Quaternion::Transform(Vec3::kUp, quaternionXY);

		/// カメラのzRotateを計算
		cameraRotateZ_ = Vec3::Dot(playerForward_.Normalize(), cameraUp_.Normalize());
		cameraRotateZ_ = std::clamp(cameraRotateZ_ * std::numbers::pi_v<float>, -1.0f, 1.0f);
		cameraRotateZ_ = std::acos(cameraRotateZ_);

		/// cameraのquaternion計算 その2
		const Vec3& playerVelocity = pPlayer_->GetVelocity();
		Quaternion quaternionLocalZ = Quaternion::MakeFromAxis(
			{ 0.0f, 0.0f, 1.0f },
			std::atan2(-playerVelocity.x, playerVelocity.y)
		);

		/// cameraにセット
		pGameCamera_->SetQuaternion(quaternionXY);
		//pTransform_->quaternion = quaternionLocalZ;
	}
}

void GameCameraState::LastUpdate() {

}

void GameCameraState::Debug() {
	if(ImGui::TreeNodeEx("player var", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("forward", &playerForward_.x, 0.0f);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("camera var", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("up", &cameraUp_.x, 0.0f);
		ImGui::DragFloat("rotateZ", &cameraRotateZ_, 0.01f);

		ImGui::TreePop();
	}

}


void GameCameraState::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
	pGameCamera_->SetParent(pTransform_);
}

void GameCameraState::SetPlayer(Player* _player) {
	pPlayer_ = _player;
	SetParent(pPlayer_->GetPivot());
}
