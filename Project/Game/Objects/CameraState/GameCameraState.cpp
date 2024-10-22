#include "GameCameraState.h"

#include <algorithm>
#include <numbers>

#include "ImGuiManager/ImGuiManager.h"
#include "Objects/Player/Player.h"
#include "GraphicManager/Light/DirectionalLight.h"

//function
#include"Easing/EasingFunction.h"
#include"FrameManager/Time.h"
#include "Math/Easing.h"



void GameCameraState::Initialize() {

	/// transform initialize
	pTransform_->position    = { 0.0f, 0.0f, -40.0f };
	pTransform_->quaternion  = { 0.0f, 0.0f, 0.0f, 1.0f };
	pTransform_->rotateOrder = QUATERNION;
	pTransform_->Update();

	easeTMax_   = 0.7f;
	zoomOutMax_ = -70.0f;
	zoomRoot_   = -40.0f;
	zoomInMax_  = -20.0f;
}

void GameCameraState::Update() {

	prevBehavior_ = currentBehavior_;
	currentBehavior_ = pPlayer_->GetCameraBehavior();

	/// プレイヤーからカメラの振る舞いが変わったか得る
	if(prevBehavior_ != currentBehavior_) {
		CameraBehavior behavior = static_cast<CameraBehavior>(currentBehavior_);

		/// 対応したbehaviorに設定する
		switch(behavior) {
		case CameraBehavior::kRoot:
			SetBehaviorZoomRoot();
			break;
		case CameraBehavior::kZoomIn:
			SetBehaviorZoomIn();
			break;
		case CameraBehavior::kZoomOut:
			SetBehaviorZoomOut();
			break;
		}
	}


	//プレイヤーのゲージMaxでカメラズームアウト
	//if (pPlayer_->GetisPowerUp()) {
	//	SetBehaviorZoomIn();
	//}
	////ズームイン
	//else if (!pPlayer_->GetisPowerUp()) {
	//	SetBehaviorZoomRoot();
	//}
	//振る舞い管理
	BehaviorManagement();

	pLight_->SetDirection(-pGameCamera_->GetPosition().Normalize());
}

void GameCameraState::LastUpdate() {

}

//振る舞い関数
void GameCameraState::RootInit() {
	currentZoomValue_ = pTransform_->position.z;
	easeT_ = 0.0f;
}


void GameCameraState::RootUpdate() {
	easeT_ += Time::TimeRateDeltaTime();
	if(easeT_ >= easeTMax_) {
		easeT_ = easeTMax_;
	}


	pTransform_->position.z = EaseOutBack<float>(currentZoomValue_, zoomRoot_, easeT_, easeTMax_);

	//pTransform_->position.z = std::lerp(
	//	currentZoomValue_, zoomRoot_,
	//	(easeT_ / easeTMax_)
	//);
}

void  GameCameraState::ZoomInInit() {
	//ズームアウトのカメラの位置になるよう初期化
	currentZoomValue_ = pTransform_->position.z;
	//pTransform_->position.z = zoomOutMax_;
	easeT_ = 0.0f;
}
void  GameCameraState::ZoomInUpdate() {
	easeT_ += Time::DeltaTime();//デルタタイムにしたい
	if (easeT_ >= easeTMax_) {
		easeT_ = easeTMax_;
	}
	//カメラを引く
	pTransform_->position.z = EaseOutBack<float>(currentZoomValue_, zoomInMax_, easeT_, easeTMax_);
}

void  GameCameraState::ZoomOutInit() {
	//通常状態のカメラの位置になるよう初期化
	currentZoomValue_ = pTransform_->position.z;
	//pTransform_->position.z = zoomInMax_;
	easeT_ = 0.0f;
}

void  GameCameraState::ZoomOutUpdate() {
	easeT_ += Time::TimeRateDeltaTime();//デルタタイムにしたい
	if (easeT_ >= easeTMax_) {
		easeT_ = easeTMax_;
	}
	//カメラを引く
	pTransform_->position.z = EaseOutBack<float>(currentZoomValue_, zoomOutMax_, easeT_, easeTMax_);
}


void GameCameraState::Debug() {
	
	if(ImGui::TreeNodeEx("player var", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("camera var", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("zoomOutMax", &zoomOutMax_);
		ImGui::DragFloat("zoomInMax", &zoomInMax_);

		ImGui::TreePop();
	}

}


//振る舞い管理
void  GameCameraState::BehaviorManagement() {
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case CameraBehavior::kRoot:
		default:
			RootInit();
			break;
		case CameraBehavior::kZoomIn:
			ZoomInInit();
			break;
		case CameraBehavior::kZoomOut:
			ZoomOutInit();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	// 振る舞い更新
	switch (behavior_) {
	case CameraBehavior::kRoot:
	default:
		RootUpdate();
		break;
	case CameraBehavior::kZoomIn:
		ZoomInUpdate();
		break;
	case CameraBehavior::kZoomOut:
		ZoomOutUpdate();
		break;

	}

}

//ズームアウトにセット
void GameCameraState::SetBehaviorZoomOut() {
	if (behavior_ != CameraBehavior::kZoomOut) {
		behaviorRequest_ = CameraBehavior::kZoomOut;
	}
}

void GameCameraState::SetBehaviorZoomRoot() {
	if(behavior_ != CameraBehavior::kRoot) {
		behaviorRequest_ = CameraBehavior::kRoot;
	}
}

//ズームインにセット
void GameCameraState::SetBehaviorZoomIn() {
	//ズームアウトの時にInに戻す
	if (behavior_ != CameraBehavior::kZoomIn) {
		behaviorRequest_ = CameraBehavior::kZoomIn;
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
