#include "BaseBuilding.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/Collider/SphereCollider.h>

#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"
#include"Math/Random.h"

//object
#include"FrameManager/Time.h"


void BaseBuilding::Initialize() {

	 model_= ModelManager::Load("TestObject");

	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(model_);
	 auto collider = AddComponent<BoxCollider>(model_);
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
	scaleMax_ = 1.0f;
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();

	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();

}

void BaseBuilding::Update() {
	//成長
	growTime_ += Time::DeltaTime();
	GrowForTime(0.2f, 2.0f);

	//吸われる処理
	if (isSlurp_) {

	}
	
	//ピボット更新
	pivot_.UpdateMatrix();
}

void BaseBuilding::Debug() {
	
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



void BaseBuilding::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {
	//当たったら用済み
	if (dynamic_cast<Player*>(collision)) {
		isDeath_ = true;
		
	}
}
