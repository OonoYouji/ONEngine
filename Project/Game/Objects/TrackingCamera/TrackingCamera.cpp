#include "TrackingCamera.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"
#include "FrameManager/Time.h"

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

	/// 親子付け
	pGameCamera_->SetParent(pTransform_);

	missTheTargetLenght_ = 5.0f;
	quaternionLerpSpeed_ = 10.0f;

	cameraMoveSpeedVector_ = { 0.05f, 0.025f, 0.0f };

	/// 値を外部保存の管理クラスに値を追加する
	AddVariables();
	ApplyVariables();
}

void TrackingCamera::Update() {
	ApplyVariables();

	prevIsLockOn_ = isLockOn_;

	/// ボスへの方向ベクトル
	cameraToEnemyVector_ = pEnemy_->GetPosition() - pGameCamera_->GetPosition();
	playerToEnemyVector_ = pEnemy_->GetPosition() - pPlayer_->GetPosition();
	cameraToPlayerVector_ = pPlayer_->GetPosition() - pGameCamera_->GetPosition();

	/// 方向ベクトルをquaternionに変える
	toPlayerQuaternion_ = Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f },
		cameraToPlayerVector_.Normalize()
	);

	toEnemyQuaternion_ = Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f },
		(cameraToPlayerVector_.Normalize() + cameraToEnemyVector_.Normalize()).Normalize()
	);


	/// カメラの回転
	Vec3 mouse = {
		Input::MouseVelocity().Normalize().y * -1.0f,
		Input::MouseVelocity().Normalize().x * +1.0f,
		0.0f
	};

	cameraOffsetRotate_ += cameraMoveSpeedVector_* mouse;
	cameraOffsetRotate_.x = std::clamp(cameraOffsetRotate_.x, 0.0f, 1.0f);
	//cameraOffsetRotate_.x = std::clamp(cameraOffsetRotate_.x, 0.0f, 1.0f);

	/// ロックオンのフラグの更新
	LockOnUpdate();

	quaternionLerpTime_ = std::clamp(quaternionLerpTime_ + quaternionLerpSpeed_ * Time::DeltaTime(), 0.0f, 1.0f);
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
			"offset rotate : {:0.2f}, {:0.2f}, {:0.2f}",
			cameraOffsetRotate_.x, cameraOffsetRotate_.y, cameraOffsetRotate_.z).c_str()
		);


		ImGui::Text(std::format(
			"isLockOn : {}", isLockOn_).c_str()
		);

		ImGui::TreePop();
	}
}




void TrackingCamera::LockOnToEnemy() {

	/// parentがplayerならキャンセルしてthisをparentする
	if(pGameCamera_->GetParent() == pPlayer_->GetTransform()) {
		pGameCamera_->ParentCancel(false);
		pGameCamera_->SetParent(pTransform_);
	}

	/// positionの更新、プレイヤーと敵の間に配置
	pTransform_->position = pPlayer_->GetPosition() + (playerToEnemyVector_ * 0.5f);

	/// offset position を rotate分回転させる
	pGameCamera_->SetPosition(Mat4::Transform(
		cameraOffsetPosition_ * playerToEnemyVector_.Len() / 10.0f, 
		Mat4::MakeRotate(cameraOffsetRotate_)));

	pGameCamera_->SetQuaternion(Quaternion::Lerp(
		toPlayerQuaternion_, toEnemyQuaternion_, 
		quaternionLerpTime_
	));
}

void TrackingCamera::LockOnToPlayer() {

	/// parentがthisならキャンセルしてplayerをparentする
	if(pGameCamera_->GetParent() == pTransform_) {
		pGameCamera_->ParentCancel(false);
		pGameCamera_->SetParent(pPlayer_->GetTransform());
	}

	/// offset position を rotate分回転させる
	pGameCamera_->SetPosition(Mat4::Transform(
		cameraOffsetPosition_, Mat4::MakeRotate(cameraOffsetRotate_)));

	pGameCamera_->SetQuaternion(Quaternion::Lerp(
		toEnemyQuaternion_, toPlayerQuaternion_, 
		quaternionLerpTime_
	));
}

void TrackingCamera::LockOnUpdate() {
	isLockOn_ = false;
	if(Input::PressKey(KeyCode::Enter) || Input::PressMouse(MouseCode::Right)) {

		/// プレイヤーと敵の距離が範囲内であれば
		if(missTheTargetLenght_ > playerToEnemyVector_.Len()) {
			isLockOn_ = true;
		}
	}

	/// ロックオンのフラグが切り替わった瞬間
	if((isLockOn_ && !prevIsLockOn_)
	   || (!isLockOn_ && prevIsLockOn_)) {
		quaternionLerpTime_ = 0.0f;
	}
}




void TrackingCamera::AddVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "offset position", cameraOffsetPosition_);
	vm->AddValue(groupName, "missTheTargetLenght", missTheTargetLenght_);
	vm->AddValue(groupName, "quaternionLerpSpeed", quaternionLerpSpeed_);
	vm->AddValue(groupName, "cameraMoveSpeedVector", cameraMoveSpeedVector_);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

}

void TrackingCamera::ApplyVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	
	cameraOffsetPosition_  = vm->GetValue<Vec3>(groupName, "offset position");
	missTheTargetLenght_   = vm->GetValue<float>(groupName, "missTheTargetLenght");
	quaternionLerpSpeed_   = vm->GetValue<float>(groupName, "quaternionLerpSpeed");
	cameraMoveSpeedVector_ = vm->GetValue<Vec3>(groupName, "cameraMoveSpeedVector");
}


