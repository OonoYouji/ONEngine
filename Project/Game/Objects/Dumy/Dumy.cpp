#include "Dumy.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/Collider/SphereCollider.h>

#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"
//obj
#include"Objects/Ground/Ground.h"
#include"Objects/Building/InTornadoBuilding.h"
//math
#include"Math/Random.h"
#include"FrameManager/Time.h"
//std
#include<numbers>
//function
#include"Easing/EasingFunction.h"

void Dumy::Initialize() {

	auto model = ModelManager::Load("PlayerInGame");
	mesh_ = AddComponent<MeshRenderer>();

	mesh_->SetModel(model);
	mesh_->SetMaterial("white2x2");
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
	pTransform_->position = { 0,0,Ground::groundScale_+0.4f };//ポジション
	pTransform_->rotate = { 0,0,0 };//回転
	pTransform_->scale = { 1.0f,1.0f,1.0f };//スケール
	hp_ = hpMax_;
	kEaseTime_ = 1.0f;
	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();
	pTransform_->rotateOrder = XYZ;
	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();

}

void Dumy::Update() {
	//ダミーの死亡
	if (isFall_) {

		easeTime_ += Time::DeltaTime();
		if (easeTime_ >= kEaseTime_) {
			easeTime_ = kEaseTime_;
			isDeath_ = true;
		}
		pTransform_->rotate.y = EaseOutSine<float>(0, 1.550f, easeTime_, kEaseTime_);	
	}
	//ピボット更新
	pivot_.UpdateMatrix();
}

void Dumy::Debug() {

}


void Dumy::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	//当たったら用済み
	if (dynamic_cast<InTornadoBuilding*>(collision)) {
		hp_--;
		if (hp_ <= 0) {
			isFall_ = true;
			mesh_->SetColor(Vec4::kRed);
		}
	}
}

