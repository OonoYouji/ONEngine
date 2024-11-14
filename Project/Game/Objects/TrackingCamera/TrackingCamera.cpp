#define NOMINMAX
#include "TrackingCamera.h"

/// std
#include <format>
#include <numbers>

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"
#include "FrameManager/Time.h"

/// math
#include "Math/LerpShortAngle.h"

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

	lockOnLenghtScaleFactor_ = 10.0f;

	missTheTargetLenght_ = 5.0f;
	quaternionLerpSpeed_ = 10.0f;

	cameraOffsetDirection_ = { 0.0f, 0.0f, -1.0f};
	cameraOffsetLenght_ = 10.0f;

	cameraMoveSpeedVector_ = { 0.05f, 0.025f, 0.0f };

	toTargetLerpMaxTime_ = 0.5f;

	/// 値を外部保存の管理クラスに値を追加する
	AddVariables();
	ApplyVariables();
}

void TrackingCamera::Update() {
	ApplyVariables(); /// 値を適用

	prevIsLockOn_ = isLockOn_;

	/// オブジェクト間のベクトルを計算
	cameraToEnemyVector_  = pEnemy_->GetPosition() - pGameCamera_->GetPosition();
	playerToEnemyVector_  = pEnemy_->GetPosition() - pPlayer_->GetPosition();
	cameraToPlayerVector_ = pPlayer_->GetPosition() - pGameCamera_->GetPosition();

	/// ロックオンのフラグの更新
	LockOnUpdate();

	/// カメラの更新
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
			"camera offset rotate : {:0.2f}, {:0.2f}, {:0.2f}",
			cameraOffsetRotate_.x, cameraOffsetRotate_.y, cameraOffsetRotate_.z).c_str()
		);
		
		ImGui::Text(std::format(
			"camera target rotate : {:0.2f}, {:0.2f}, {:0.2f}",
			cameraTargetRotate_.x, cameraTargetRotate_.y, cameraTargetRotate_.z).c_str()
		);

		ImGui::Text(std::format(
			"target position : {:0.2f}, {:0.2f}, {:0.2f}",
			targetPosition_.x, targetPosition_.y, targetPosition_.z).c_str()
		);

			ImGui::Text(std::format(
			"player to enemy rotateY : {:0.2f}",
			playerToEnemyRotateY_).c_str()
		);

		


		ImGui::Text(std::format(
			"isLockOn : {}", isLockOn_).c_str()
		);

		ImGui::TreePop();
	}
}




void TrackingCamera::LockOnToEnemy() {


	/// カメラの回転 keyboard
	Vec3 cameraRotateValue = {
		static_cast<float>(Input::PressKey(KeyCode::UpArrow) - Input::PressKey(KeyCode::DownArrow)),
		static_cast<float>(Input::PressKey(KeyCode::RightArrow) - Input::PressKey(KeyCode::LeftArrow)),
		0.0f
	};

	/// カメラの回転 mouse
	if(Input::PressMouse(MouseCode::Left)) {
		cameraRotateValue += {
			1.5f * Input::MouseVelocity().Normalize().y,
			1.5f * Input::MouseVelocity().Normalize().x,
			0.0f
		};
	}



	/// ---------------------------------------------------
	/// 
	/// ---------------------------------------------------

	Vec3 normP2EVec = playerToEnemyVector_.Normalize();
	playerToEnemyRotateY_ = std::atan2(normP2EVec.x, normP2EVec.z);

	/// ターゲット開始した
	if(isTargetingActive_) {
		toTargetLerpTime_ += Time::DeltaTime();
		float lerpT = std::min(toTargetLerpTime_ / toTargetLerpMaxTime_, 1.0f);

		Vec3 diff = Vec3(0.0f, playerToEnemyRotateY_, 0.0f) - saveCameraOffsetRotate_ ;
		bool isInverse = false; /// diffを反転したか

		/// マイナス値をプラス値に直す
		if(diff.y < 0.0f) {
			diff.y *= -1.0f;
			isInverse = true;
		}


		//cameraTargetRotate_ = Vec3::Lerp(saveCameraTargetRotate_, { 0.0f, playerToEnemyRotateY_, 0.0f }, lerpT);

		if(diff.y > std::numbers::pi_v<float>) {
			if(isInverse) {
				cameraOffsetRotate_ = Vec3::Lerp(saveCameraOffsetRotate_, { 0.0f, 2.0f * std::numbers::pi_v<float>, 0.0f }, lerpT);
			} else {
				cameraOffsetRotate_ = Vec3::Lerp(saveCameraOffsetRotate_, { 0.0f, -2.0f * std::numbers::pi_v<float>, 0.0f }, lerpT);
			}
		} else {
			cameraOffsetRotate_ = Vec3::Lerp(saveCameraOffsetRotate_, {}, lerpT);
		}

		cameraTargetRotate_ = Vec3::Lerp(saveCameraTargetRotate_, { 0.0f, playerToEnemyRotateY_, 0.0f }, lerpT);

		targetPosition_     = Vec3::Lerp(saveTargetPosition_, pTargetObject_->GetPosition() - (playerToEnemyVector_ * 0.5f), lerpT);

		/// 終了した
		if(lerpT == 1.0f) {
			isTargetingActive_ = false;
		}
	} else {

		targetPosition_ = pTargetObject_->GetPosition() - (playerToEnemyVector_ * 0.5f);
		
		cameraTargetRotate_.y = playerToEnemyRotateY_;

		CameraOffsetRotateUpdate(cameraRotateValue);

	}



	/// ---------------------------------------------------
	/// 座標の計算
	/// ---------------------------------------------------

	/// positionの更新、プレイヤーと敵の間に配置
	float scaleFactor = playerToEnemyVector_.Len() / lockOnLenghtScaleFactor_;
	Vec3  offsetPos   = cameraOffsetDirection_ * cameraOffsetLenght_ * scaleFactor;

	/// 近づき過ぎたら倍率で値を変えていたのをやめる
	if(playerToEnemyVector_.Len() < 15.0f) {
		offsetPos = Mat4::Transform(
			Vec3::Lerp(offsetPos, cameraOffsetDirection_, 0.5f),
			Mat4::MakeRotate(cameraTargetRotate_ + cameraOffsetRotate_)
		);
	} else {
		offsetPos = Mat4::Transform(
			Vec3::Lerp(cameraOffsetDirection_, offsetPos, 0.5f),
			Mat4::MakeRotate(cameraTargetRotate_ + cameraOffsetRotate_)
		);
	}


	/// offset position を rotate分回転させる
	cameraNextPosition_ = targetPosition_ + offsetPos;

	pGameCamera_->SetPosition(Vec3::Lerp(
		pGameCamera_->GetPosition(), cameraNextPosition_,
		0.5f
	));


	
	/// ---------------------------------------------------
	/// 回転角の計算
	/// ---------------------------------------------------

	/// カメラの行列更新
	pGameCamera_->UpdateMatrix();
	cameraToPlayerVector_ = pPlayer_->GetPosition() - pGameCamera_->GetPosition();
	cameraToEnemyVector_  = pEnemy_->GetPosition() - pGameCamera_->GetPosition();

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


	/// ---------------------------------------------------
	/// 
	/// ---------------------------------------------------

	/// ターゲットをはずした
	if(isTargetLost_) {

		toTargetLerpTime_ += Time::DeltaTime();
		float lerpT = std::min(toTargetLerpTime_ / 0.5f, 1.0f);

		cameraOffsetRotate_ = saveCameraOffsetRotate_ + saveCameraTargetRotate_;
		
		targetPosition_ = Vec3::Lerp(saveTargetPosition_, pTargetObject_->GetPosition(), lerpT);
		currentDirection_ = Vec3::Lerp(
			(cameraToPlayerVector_.Normalize() + cameraToEnemyVector_.Normalize()).Normalize(),
			cameraToPlayerVector_.Normalize(),
			lerpT
		);

		if(lerpT == 1.0f) {
			isTargetLost_ = false;
		}

	} else {

		targetPosition_ = pTargetObject_->GetPosition();

		/// 
		CameraOffsetRotateUpdate(cameraRotateValue);
	}



	/// ---------------------------------------------------
	/// 位置の計算
	/// ---------------------------------------------------

	/// offset position を rotate分回転させる
	cameraNextPosition_ = targetPosition_ + Mat4::Transform(
		cameraOffsetDirection_ * cameraOffsetLenght_, Mat4::MakeRotate(cameraOffsetRotate_)
	);

	pGameCamera_->SetPosition(Vec3::Lerp(
		pGameCamera_->GetPosition(), cameraNextPosition_,
		0.5f
	));



	/// ---------------------------------------------------
	/// 回転の計算
	/// ---------------------------------------------------

	/// カメラの行列更新
	pGameCamera_->UpdateMatrix();
	cameraToPlayerVector_ = pPlayer_->GetPosition() - pGameCamera_->GetPosition();

	if(!isTargetLost_) {
		currentDirection_ = cameraToPlayerVector_.Normalize();
	}

	/// 方向ベクトルをquaternionに変える
	cameraToPlayerQuaternion_ = Quaternion::LockAt(
		{ 0.0f, 0.0f, 0.0f },
		currentDirection_.Normalize()
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

	/// ロックオンを外した瞬間
	if(!isLockOn_ && prevIsLockOn_) {
		quaternionLerpTime_ = 0.0f;

		isTargetLost_ = true;
		toTargetLerpTime_ = 0.0f;

		saveCameraTargetRotate_ = cameraTargetRotate_;
		saveCameraOffsetRotate_ = cameraOffsetRotate_;

		cameraTargetRotate_ = {};

		/// プレイヤーをターゲットする
		pTargetObject_ = pPlayer_;
		saveTargetPosition_ = targetPosition_;

		return;
	}

	/// ロックオンをした瞬間
	if(isLockOn_ && !prevIsLockOn_) {
		quaternionLerpTime_ = 0.0f;


		isTargetingActive_ = true;
		toTargetLerpTime_ = 0.0f;

		saveCameraTargetRotate_ = cameraTargetRotate_;
		saveCameraOffsetRotate_ = cameraOffsetRotate_;


		/// 敵をターゲットする
		pTargetObject_ = pEnemy_;
		saveTargetPosition_ = targetPosition_;


		Vec3 normP2EVec = playerToEnemyVector_.Normalize();
		playerToEnemyRotateY_ = std::atan2(normP2EVec.z, normP2EVec.x);

		return;
	}

}

void TrackingCamera::CameraOffsetRotateUpdate(const Vec3& _rotateValue) {
	cameraOffsetRotate_ += cameraMoveSpeedVector_ * _rotateValue;
	cameraOffsetRotate_.x = std::clamp(cameraOffsetRotate_.x, -0.3f, 0.8f);
	cameraOffsetRotate_.y = std::fmod(cameraOffsetRotate_.y, 2.0f*  std::numbers::pi_v<float>);
}




void TrackingCamera::AddVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "missTheTargetLenght", missTheTargetLenght_);
	vm->AddValue(groupName, "quaternionLerpSpeed", quaternionLerpSpeed_);
	vm->AddValue(groupName, "cameraMoveSpeedVector", cameraMoveSpeedVector_);
	vm->AddValue(groupName, "lockOnLenghtScaleFactor", lockOnLenghtScaleFactor_);
	vm->AddValue(groupName, "toTargetLerpMaxTime", toTargetLerpMaxTime_);
	vm->AddValue(groupName, "cameraOffsetDirection", cameraOffsetDirection_);
	vm->AddValue(groupName, "cameraOffsetLenght", cameraOffsetLenght_);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

}

void TrackingCamera::ApplyVariables() {
	VariableManager*   vm        = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	
	missTheTargetLenght_     = vm->GetValue<float>(groupName, "missTheTargetLenght");
	quaternionLerpSpeed_     = vm->GetValue<float>(groupName, "quaternionLerpSpeed");
	cameraMoveSpeedVector_   = vm->GetValue<Vec3>(groupName,  "cameraMoveSpeedVector");
	lockOnLenghtScaleFactor_ = vm->GetValue<float>(groupName, "lockOnLenghtScaleFactor");
	toTargetLerpMaxTime_     = vm->GetValue<float>(groupName, "toTargetLerpMaxTime");
	cameraOffsetDirection_   = vm->GetValue<Vec3>(groupName,  "cameraOffsetDirection");
	cameraOffsetLenght_      = vm->GetValue<float>(groupName, "cameraOffsetLenght");

	cameraOffsetDirection_ = cameraOffsetDirection_.Normalize();
	vm->SetValue(groupName, "cameraOffsetDirection", cameraOffsetDirection_);
}


