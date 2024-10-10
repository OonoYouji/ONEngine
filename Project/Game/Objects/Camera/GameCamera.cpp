#include "GameCamera.h"

#include"ImGuiManager/ImGuiManager.h"
#include"Math/Matrix4x4.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Transform/Transform.h>

//function
#include"Easing/EasingFunction.h"
#include"FrameManager/Time.h"

GameCamera::GameCamera(const std::string& name) {
	CreateTag(this);
	SetName(name);
	CameraManager::GetInstance()->AddCamera(GetName(), this);
}

GameCamera::~GameCamera() {}


/// ===================================================
/// 初期化
/// ===================================================
void GameCamera::Initialize() {
	//auto meshRenderer = AddComponent<MeshRenderer>();
	//meshRenderer->SetModel("TestObject");
	//transform_.position = { 0.0f, 3.8f, -6.49f * 2 };
	//transform_.rotate.x = 0.26f;
	//パラメータセット
	easeTMax_ = 0.7f;
	zoomOutMax_ = -30.0f;
	zoomInMax_ = 0.0f;

	pTransform_->quaternion  = { 0, 0, 0, 1 };
	pTransform_->rotateOrder = QUATERNION;
	BaseCamera::BaseInitialize();

}

/// ===================================================
/// 更新処理
/// ===================================================
void GameCamera::Update() {
	//振る舞い管理
	BehaviorManagement();
	//更新
	BaseUpdate();
}

//振る舞い関数
void GameCamera::RootInit() {

}
void GameCamera::RootUpdate() {

}

void GameCamera::ZoomInInit() {
	//ズームアウトのカメラの位置になるよう初期化
	pTransform_->position.z = zoomOutMax_;
	easeT_ = 0.0f;
}
void GameCamera::ZoomInUpdate() {
	easeT_ +=Time::DeltaTime();//デルタタイムにしたい
	if (easeT_ >= easeTMax_) {
		easeT_ = easeTMax_;
	}
	//カメラを引く
	pTransform_->position.z = EaseOutBack<float>(zoomOutMax_, zoomInMax_, easeT_, easeTMax_);
}

void GameCamera::ZoomOutInit() {
	//通常状態のカメラの位置になるよう初期化
	pTransform_->position.z =zoomInMax_;
	easeT_ = 0.0f;
}

void GameCamera::ZoomOutUpdate() {
	easeT_ += Time::DeltaTime();//デルタタイムにしたい
	if (easeT_ >= easeTMax_) {
		easeT_ = easeTMax_;
	}
	//カメラを引く
	pTransform_->position.z = EaseOutBack<float>(zoomInMax_, zoomOutMax_, easeT_, easeTMax_);
}



void GameCamera::Debug() {

	if (ImGui::TreeNode("ZoomParamater")) {
		ImGui::DragFloat("zoomOutMax", &zoomOutMax_);
		ImGui::DragFloat("zoomInMax", &zoomInMax_);
		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("debyg", ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec3 pos = GetPosition();
		ImGui::DragFloat3("world pos", &pos.x);
		ImGui::Text("m[0][0]:%f, m[0][1]:%f, m[0][2]:%f ,m[0][3]:%f", pTransform_->matTransform.m[0][0], pTransform_->matTransform.m[0][1], pTransform_->matTransform.m[0][2], pTransform_->matTransform.m[0][3]);
		ImGui::Text("m[1][0]:%f, m[1][1]:%f, m[1][2]:%f ,m[1][3]:%f", pTransform_->matTransform.m[1][0], pTransform_->matTransform.m[1][1], pTransform_->matTransform.m[1][2], pTransform_->matTransform.m[1][3]);
		ImGui::Text("m[2][0]:%f, m[2][1]:%f, m[2][2]:%f ,m[2][3]:%f", pTransform_->matTransform.m[2][0], pTransform_->matTransform.m[2][1], pTransform_->matTransform.m[2][2], pTransform_->matTransform.m[2][3]);
		ImGui::Text("m[3][0]:%f, m[3][1]:%f, m[3][2]:%f ,m[3][3]:%f", pTransform_->matTransform.m[3][0], pTransform_->matTransform.m[3][1], pTransform_->matTransform.m[3][2], pTransform_->matTransform.m[3][3]);

		ImGui::TreePop();
	}
}
//ズームアウトにセット
void GameCamera::SetBehaviorZoomOut() {
	if (behavior_ != Behavior::kZoomOut) {
		behaviorRequest_ = Behavior::kZoomOut;
	}
}

//ズームインにセット
void GameCamera::SetBehaviorZoomIn() {
	//ズームアウトの時にInに戻す
	if (behavior_ == Behavior::kZoomOut) {
		behaviorRequest_ = Behavior::kZoomIn;
	}
}


//振る舞い管理
void GameCamera::BehaviorManagement() {
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