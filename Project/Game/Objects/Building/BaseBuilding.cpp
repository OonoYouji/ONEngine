#include "BaseBuilding.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"
#include"Math/Random.h"

//object
#include"FrameManager/Time.h"


void BaseBuilding::Initialize() {

	Model* model = ModelManager::Load("TestObject");


	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);
	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->Initialize();
	pivot_.Initialize();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };//ピボット
	pTransform_->position = { 0,0,-10.8f };//ポジション
	pTransform_->rotate = { -1.5f,0,0 };//回転
	pTransform_->scale = { 1.0f,0.1f,1.0f };//スケール
	ofsetX = -0.14f;
	ofsetY = -1.10f;
	scaleMax_ = 1.0f;
	behaviorRequest_ = Behavior::kRoot;
	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();

	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();

}

void BaseBuilding::Update() {

	//タイフーンと合体
	if (isCollisionTyphoon_ && behavior_ != Behavior::kParent) {
		behaviorRequest_ = Behavior::kParent;
	}
	//ピボット更新
	pivot_.UpdateMatrix();
}

void BaseBuilding::Debug() {
	if (ImGui::TreeNode("pivot")) {
		ImGui::DragFloat("rotateXAngle", &ofsetX, 0.01f);
		ImGui::DragFloat("rotateYAngle", &ofsetY, 0.01f);
		ImGui::TreePop();
	}
}

//振る舞い関数
void BaseBuilding::RootInit() {
	
}
void BaseBuilding::RootUpdate() {
	growTime_ += Time::DeltaTime();
	GrowForTime(0.2f, 2.0f);
}

void BaseBuilding::ParentInit(Tornado* tornade) {
	speed_ = Random::Float(4.0f, 5.0f);//回転スピード
	radius_ = Random::Float(-0.5f, 0.5f);//半径
	pTransform_->SetParent(tornade->GetParent());//取るね――ドペアレント
	pTransform_->scale = { 0.2f,0.2f,0.2f };

}
void BaseBuilding::ParentUpdate(Tornado* tornade) {
	//回転速度を加算
	theta_ += (speed_/ tornade->GetTransform()->scale.x) * Time::DeltaTime();
	phi_ += (speed_/ tornade->GetTransform()->scale.x) * Time::DeltaTime();
	//球面座標を計算
	float x = (tornade->GetTransform()->scale.x + radius_) * sin(theta_) ;
	float y = (tornade->GetTransform()->scale.y + radius_) * cos(theta_) ;
	/*float z = radius * cos(theta_);*/
	pTransform_->position = { x,y,-2 };
}

void BaseBuilding::GrowForTime(const float& par,const float&second ) {

	//割合によるインクる面とする値を決める
	float incrementSize = scaleMax_ * par;

	if (growTime_ >= second) {//毎秒
		// 現在のスケール値に増加分を追加s
		pTransform_->scale.y += incrementSize;
	// スケールが最大値を超えないように制限
		if (pTransform_->scale.y > scaleMax_) {
			pTransform_->scale.y =scaleMax_;
		}
		growTime_ = 0.0f;
	}
}

//振る舞い管理
void BaseBuilding::BehaviorManagement(Tornado* tornado) {
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			RootInit();
			break;
		case Behavior::kParent:
			ParentInit(tornado);
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
	case Behavior::kParent:
		ParentUpdate(tornado);
		break;
	}
}

void BaseBuilding::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	if (dynamic_cast<Player*>(collision) && !isCollisionTyphoon_) {
		isCollisionTyphoon_ = true;
		behaviorRequest_ = Behavior::kParent;
	}
}

////トルネードセット
//void BaseBuilding::SetTornado(Tornado* tornado) {
//	pTornado_ = tornado;
//}
