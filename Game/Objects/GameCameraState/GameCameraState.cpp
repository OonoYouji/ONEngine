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

	/// playerのquaternionを使う
	//pTransform_->quaternion = pPlayer_->GetQuaternion();

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
