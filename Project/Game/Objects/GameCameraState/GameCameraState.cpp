#include "GameCameraState.h"

#include <algorithm>
#include <numbers>

#include "ImGuiManager/ImGuiManager.h"
#include "Objects/Player/Player.h"
#include "GraphicManager/Light/DirectionalLight.h"


void GameCameraState::Initialize() {

	/// transform initialize
	pTransform_->position    = { 0.0f, 0.0f, -50.0f };
	pTransform_->quaternion  = { 0.0f, 0.0f, 0.0f, 1.0f };
	pTransform_->rotateOrder = QUATERNION;
	pTransform_->Update();
}

void GameCameraState::Update() {


	pLight_->SetDirection(-pGameCamera_->GetPosition().Normalize());
}

void GameCameraState::LastUpdate() {

}

void GameCameraState::Debug() {
	if(ImGui::TreeNodeEx("player var", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("camera var", ImGuiTreeNodeFlags_DefaultOpen)) {


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

void GameCameraState::SetDirectionalLight(DirectionalLight* _directionalLight) {
	pLight_ = _directionalLight;
}
