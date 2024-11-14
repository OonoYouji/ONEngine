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
	pTargetObject_ = pPlayer_;

	missTheTargetLenght_ = 5.0f;
	quaternionLerpSpeed_ = 10.0f;

	cameraMoveSpeedVector_ = { 0.05f, 0.025f, 0.0f };

	/// 値を外部保存の管理クラスに値を追加する
	AddVariables();
	ApplyVariables();
}

void TrackingCamera::Update() {
	ApplyVariables(); /// 値を適用

	prevIsLockOn_ = isLockOn_;

	/// ボスへの方向ベクトル
	cameraToEnemyVector_ = pEnemy_->GetPosition() - pGameCamera_->GetPosition();
	playerToEnemyVector_ = pEnemy_->GetPosition() - pPlayer_->GetPosition();
	cameraToPlayerVector_ = pPlayer_->GetPosition() - pGameCamera_->GetPosition();

	/// カメラの回転
	Vec3 cameraRotateValue = {
		static_cast<float>(Input::PressKey(KeyCode::UpArrow) - Input::PressKey(KeyCode::DownArrow)),
		static_cast<float>(Input::PressKey(KeyCode::RightArrow) - Input::PressKey(KeyCode::LeftArrow)),
		0.0f
	};


	if(Input::PressMouse(MouseCode::Left)) {
		cameraRotateValue += {
			3.0f * Input::MouseVelocity().Normalize().y,
			3.0f * Input::MouseVelocity().Normalize().x,
			0.0f
		};
	}


	cameraOffsetRotate_ += cameraMoveSpeedVector_* cameraRotateValue;
	cameraOffsetRotate_.x = std::clamp(cameraOffsetRotate_.x, 0.0f, 0.8f);

	/// ロックオンのフラグの更新
	LockOnUpdate();

	quaternionLerpTime_ = std::clamp(quaternionLerpTime_ + quaternionLerpSpeed_ * Time::DeltaTime(), 0.0f, 1.0f);
	if(isLockOn_) {
		LockOnToEnemy();
	} else {
		LockOnToPlayer();
	}

	pTransform_->Update();
	pGameCamera_->GetTransform()->Update();
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

	/// 敵をターゲットする
	pTargetObject_ = pEnemy_;


	/// positionの更新、プレイヤーと敵の間に配置
	Vec3 offsetPos = {};
	if(playerToEnemyVector_.Len() > 15.0f) {
		offsetPos = Mat4::Transform(
			cameraOffsetPosition_ * playerToEnemyVector_.Len() / 10.0f,
			Mat4::MakeRotate(cameraOffsetRotate_)
		);
	} else {
		offsetPos = Mat4::Transform(
			cameraOffsetPosition_,
			Mat4::MakeRotate(cameraOffsetRotate_)
		);
	}

	/// offset position を rotate分回転させる
	cameraNextPosition_ = pTargetObject_->GetPosition() + offsetPos;

	pGameCamera_->SetPosition(Vec3::Lerp(
		pGameCamera_->GetPosition(), cameraNextPosition_,
		0.5f
	));

	/// カメラの行列更新
	pGameCamera_->UpdateMatrix();
	cameraToPlayerVector_ = pPlayer_->GetPosition() - pGameCamera_->GetPosition();
	cameraToEnemyVector_ = pEnemy_->GetPosition() - pGameCamera_->GetPosition();

	/// カメラから敵とプレイヤーの間への回転角を計算
	cameraToEnemyQuaternion_ = Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f },
		(cameraToPlayerVector_.Normalize() + cameraToEnemyVector_.Normalize()).Normalize()
	);

	pGameCamera_->SetQuaternion(Quaternion::Lerp(
		cameraToPlayerQuaternion_, cameraToEnemyQuaternion_, 
		quaternionLerpTime_
	));
}

void TrackingCamera::LockOnToPlayer() {

	/// プレイヤーをターゲットする
	pTargetObject_ = pPlayer_;



	/// offset position を rotate分回転させる
	cameraNextPosition_ = pTargetObject_->GetPosition() + Mat4::Transform(
		cameraOffsetPosition_, Mat4::MakeRotate(cameraOffsetRotate_)
	);

	pGameCamera_->SetPosition(Vec3::Lerp(
		pGameCamera_->GetPosition(), cameraNextPosition_,
		0.5f
	));



	/// カメラの行列更新
	pGameCamera_->UpdateMatrix();
	cameraToPlayerVector_ = pPlayer_->GetPosition() - pGameCamera_->GetPosition();

	/// 方向ベクトルをquaternionに変える
	cameraToPlayerQuaternion_ = Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f },
		cameraToPlayerVector_.Normalize()
	);

	pGameCamera_->SetQuaternion(Quaternion::Lerp(
		cameraToEnemyQuaternion_, cameraToPlayerQuaternion_, 
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


