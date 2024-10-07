#include "GameCameraState.h"

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

	Transform* parent = pGameCamera_->GetParent();
	if(parent) {

		playerForward_ = Mat4::TransformNormal(Vec3::kFront, pPlayer_->GetMatTransform());
		Vec3 playerRightDir = Vec3::Cross(playerForward_, parent->position - pGameCamera_->GetPosition());
		playerRightDir      = playerRightDir.Normalize();
		float cameraRotateZ = Vec3::Dot(playerForward_, Vec3::Normalize(pGameCamera_->GetPosition()));

		pGameCamera_->UpdateMatrix();
		Quaternion quaternionXY = Quaternion::LockAt(pGameCamera_->GetPosition(), parent->position, Vec3::kUp);
		Quaternion quaternionZ  = Quaternion::MakeFromAxis(Vec3::Normalize(pGameCamera_->GetPosition()), playerRightDir.x / playerRightDir.y);


		pGameCamera_->SetQuaternion(quaternionXY * quaternionZ);
	}
}

void GameCameraState::LastUpdate() {
	//pGameCamera_->SetPosition(pTransform_->position);
	//pGameCamera_->SetQuaternion(pTransform_->quaternion);
}

void GameCameraState::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("player forward", &playerForward_.x, 0.0f);

		ImGui::TreePop();
	}
}


void GameCameraState::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
}

void GameCameraState::SetPlayer(Player* _player) {
	pPlayer_ = _player;
}
