#include "GameCameraZoomInOut.h"

#include <algorithm>
#include <numbers>

#include "ImGuiManager/ImGuiManager.h"
#include "Objects/Player/Player.h"
#include "GraphicManager/Light/DirectionalLight.h"

//function
#include"Easing/EasingFunction.h"
#include"FrameManager/Time.h"

void GameCameraZoomInOut::Initialize() {

	/// transform initialize
	pTransform_->position    = { 0.0f, 0.0f, -50.0f };
	pTransform_->quaternion  = { 0.0f, 0.0f, 0.0f, 1.0f };
	pTransform_->rotateOrder = QUATERNION;
	pTransform_->Update();

	easeTMax_ = 0.7f;
	zoomOutMax_ = -70.0f;
	zoomInMax_ = -50.0f;
}

void GameCameraZoomInOut::Update() {
	//プレイヤーのゲージMaxでカメラズームアウト
	if (pPlayer_->GetisPowerUp()) {
		SetBehaviorZoomOut();
	}
	//ズームイン
	else if (!pPlayer_->GetisPowerUp()) {
		SetBehaviorZoomIn();
	}
	//振る舞い管理
	BehaviorManagement();
}

void GameCameraZoomInOut::LastUpdate() {

}

//振る舞い関数
void GameCameraZoomInOut::RootInit() {

}
void GameCameraZoomInOut::RootUpdate() {

}

void  GameCameraZoomInOut::ZoomInInit() {
	//ズームアウトのカメラの位置になるよう初期化
	pTransform_->position.z = zoomOutMax_;
	easeT_ = 0.0f;
}
void  GameCameraZoomInOut::ZoomInUpdate() {
	easeT_ += Time::DeltaTime();//デルタタイムにしたい
	if (easeT_ >= easeTMax_) {
		easeT_ = easeTMax_;
	}
	//カメラを引く
	pTransform_->position.z = EaseOutBack<float>(zoomOutMax_, zoomInMax_, easeT_, easeTMax_);
}

void  GameCameraZoomInOut::ZoomOutInit() {
	//通常状態のカメラの位置になるよう初期化
	pTransform_->position.z = zoomInMax_;
	easeT_ = 0.0f;
}

void  GameCameraZoomInOut::ZoomOutUpdate() {
	easeT_ += Time::DeltaTime();//デルタタイムにしたい
	if (easeT_ >= easeTMax_) {
		easeT_ = easeTMax_;
	}
	//カメラを引く
	pTransform_->position.z = EaseOutBack<float>(zoomInMax_, zoomOutMax_, easeT_, easeTMax_);
}


void GameCameraZoomInOut::Debug() {
	
	if(ImGui::TreeNodeEx("player var", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("camera var", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("zoomOutMax", &zoomOutMax_);
		ImGui::DragFloat("zoomInMax", &zoomInMax_);

		ImGui::TreePop();
	}

}


void GameCameraZoomInOut::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
	pGameCamera_->SetParent(pTransform_);
}

void GameCameraZoomInOut::SetPlayer(Player* _player) {
	pPlayer_ = _player;
	SetParent(pPlayer_->GetPivot());
}


//振る舞い管理
void  GameCameraZoomInOut::BehaviorManagement() {
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			RootInit();
			break;
		case Behavior::kZoomIn:
			ZoomInInit();
			break;
		case Behavior::kZoomOut:
			ZoomOutInit();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	// 振る舞い更新
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		RootUpdate();
		break;
	case Behavior::kZoomIn:
		ZoomInUpdate();
		break;
	case Behavior::kZoomOut:
		ZoomOutUpdate();
		break;

	}

}

//ズームアウトにセット
void GameCameraZoomInOut::SetBehaviorZoomOut() {
	if (behavior_ != Behavior::kZoomOut) {
		behaviorRequest_ = Behavior::kZoomOut;
	}
}

//ズームインにセット
void GameCameraZoomInOut::SetBehaviorZoomIn() {
	//ズームアウトの時にInに戻す
	if (behavior_ == Behavior::kZoomOut) {
		behaviorRequest_ = Behavior::kZoomIn;
	}
}

