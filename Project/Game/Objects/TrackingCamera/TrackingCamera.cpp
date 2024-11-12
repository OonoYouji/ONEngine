#include "TrackingCamera.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Player/Player.h"
#include "Objects/Enemy/Enemy.h"


TrackingCamera::TrackingCamera(GameCamera* _gameCamera, Player* _player, Enemy* _enemy) : pGameCamera_(_gameCamera), pPlayer_(_player), pEnemy_(_enemy) {
	CreateTag(this);
}

TrackingCamera::~TrackingCamera() {}

void TrackingCamera::Initialize() {

	/// 値を外部保存の管理クラスに値を追加する
	AddVariables();
	ApplyVariables();


	/// カメラの回転の計算方法
	pGameCamera_->GetTransform()->rotateOrder = QUATERNION;


}

void TrackingCamera::Update() {
	ApplyVariables();

	/// ボスへの方向ベクトル
	Vec3 cameraToEnemyDirection = pEnemy_->GetPosition() - (pPlayer_->GetPosition() + offsetPosition_);
	playerToEnemyVector_ = pEnemy_->GetPosition() - pPlayer_->GetPosition();

	/// offset position の inverse がカメラからプレイヤーへの方向ベクトル
	pGameCamera_->SetPosition(pPlayer_->GetPosition() + offsetPosition_);
	pGameCamera_->SetQuaternion(Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f }, 
		(-offsetPosition_.Normalize() + cameraToEnemyDirection.Normalize()).Normalize())
	);

}

void TrackingCamera::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(std::format(
			"player to enemy vector : {:0.2f}, {:0.2f}, {:0.2f}",
			playerToEnemyVector_.x, playerToEnemyVector_.y, playerToEnemyVector_.z).c_str()
		);

		ImGui::TreePop();
	}
}

void TrackingCamera::ApplyVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	
	offsetPosition_ = vm->GetValue<Vec3>(groupName, "offset position");

}

void TrackingCamera::AddVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "offset position", offsetPosition_);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

}

