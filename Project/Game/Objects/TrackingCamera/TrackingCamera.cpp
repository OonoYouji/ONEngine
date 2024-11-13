#include "TrackingCamera.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Player/Player.h"
#include "Objects/Enemy/Enemy.h"


TrackingCamera::TrackingCamera(GameCamera* _gameCamera, Player* _player, Enemy* _enemy) : pGameCamera_(_gameCamera), pPlayer_(_player), pEnemy_(_enemy) {
	CreateTag(this);
}

TrackingCamera::~TrackingCamera() {}

void TrackingCamera::Initialize() {

	/// カメラの回転の計算方法
	pGameCamera_->GetTransform()->rotateOrder = QUATERNION;

	missTheTargetLenght_ = 5.0f;


	/// 値を外部保存の管理クラスに値を追加する
	AddVariables();
	ApplyVariables();

}

void TrackingCamera::Update() {
	ApplyVariables();

	/// ボスへの方向ベクトル
	cameraToEnemyVector_ = pEnemy_->GetPosition() - (pPlayer_->GetPosition() + cameraOffsetPosition_);
	playerToEnemyVector_ = pEnemy_->GetPosition() - pPlayer_->GetPosition();

	
	isLockOn_ = false;
	if(Input::PressKey(KeyCode::Enter)) {

		/// プレイヤーと敵の距離が範囲内であれば
		if(missTheTargetLenght_ > playerToEnemyVector_.Len()) {
			isLockOn_ = true;
		}
	}


	if(isLockOn_) {
		LockOnToEnemy();
	} else {
		LockOnToPlayer();
	}

}

void TrackingCamera::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SeparatorText("parameters");
		ImGui::Text(std::format(
			"player to enemy vector : {:0.2f}, {:0.2f}, {:0.2f}",
			playerToEnemyVector_.x, playerToEnemyVector_.y, playerToEnemyVector_.z).c_str()
		);
		
		ImGui::Text(std::format(
			"camera to enemy vector : {:0.2f}, {:0.2f}, {:0.2f}",
			cameraToEnemyVector_.x, cameraToEnemyVector_.y, cameraToEnemyVector_.z).c_str()
		);


		ImGui::Text(std::format(
			"isLockOn : {}", isLockOn_).c_str()
		);

		ImGui::TreePop();
	}
}




void TrackingCamera::LockOnToEnemy() {
	/// offset position の inverse がカメラからプレイヤーへの方向ベクトル
	pGameCamera_->SetPosition(pPlayer_->GetPosition() + cameraOffsetPosition_);
	pGameCamera_->SetQuaternion(Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f },
		(-cameraOffsetPosition_.Normalize() + cameraToEnemyVector_.Normalize()).Normalize()
	));
}

void TrackingCamera::LockOnToPlayer() {
	/// offset position の inverse がカメラからプレイヤーへの方向ベクトル
	pGameCamera_->SetPosition(pPlayer_->GetPosition() + cameraOffsetPosition_);
	pGameCamera_->SetQuaternion(Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f },
		-cameraOffsetPosition_.Normalize()
	));
}




void TrackingCamera::AddVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "offset position", cameraOffsetPosition_);
	vm->AddValue(groupName, "missTheTargetLenght", missTheTargetLenght_);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

}

void TrackingCamera::ApplyVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	
	cameraOffsetPosition_ = vm->GetValue<Vec3>(groupName, "offset position");
	missTheTargetLenght_  = vm->GetValue<float>(groupName, "missTheTargetLenght");

}


