#include "Dumy.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
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

	auto model = ModelManager::Load("TestObject");
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
	pTransform_->position = { 0,0,Ground::groundScale_+1 };//ポジション
	pTransform_->rotate = { -1.5f,0,0 };//回転
	pTransform_->scale = { 1.0f,0.1f,1.0f };//スケール
	hp_ = hpMax_;
	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();
}

void Dumy::Update() {
	//ダミーの死亡
	if (hp_ <= 0) {
		isBreak_ = true;
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
	}
}

