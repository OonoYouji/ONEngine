#include "BaseBuilding.h"

#include <ModelManager.h>

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
#include <Component/Collider/SphereCollider.h>
#include <Component/SplinePathRenderer/SplinePathRenderer.h>

#include <ImGuiManager.h>


void BaseBuilding::Initialize() {

	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel("TestObject");

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pTranform_->Initialize();
	pivot_.Initialize();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	pTranform_->position = { 0,0,-14 };
	pTranform_->rotate = { -1.5f,0,0 };
	////////////////////////////////////////////////////////////////////////////////////////////
    //  ペアレント
    ////////////////////////////////////////////////////////////////////////////////////////////
	/*pTranform_->rotateOrder = QUATERNION;*/
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	pTranform_->SetParent(&pivot_);

	pivot_.UpdateMatrix();
	UpdateMatrix();
}

void BaseBuilding::Update() {

	//タイフーンと合体
	if (isCollisionTyphoon_&&behavior_!=Behavior::kParent) {
		behaviorRequest_ = Behavior::kParent;
	}
	//ピボット更新
	pivot_.UpdateMatrix();

}

void BaseBuilding::Debug() {

}

//振る舞い関数
void BaseBuilding::RootInit() {

}
void BaseBuilding::RootUpdate() {

}
void BaseBuilding::ParentInit(Player* player) {
	pTranform_->SetParent(player->GetPivot());
}
void BaseBuilding::ParentUpdate() {
	pTranform_->rotate.x += 0.01f;
}

//振る舞い管理
void BaseBuilding::BehaviorManagement(Player* player) {
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
			ParentInit(player);
			break;
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
			ParentUpdate();
			break;

		}
	}
}

Quaternion BaseBuilding::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	//    W A   ɕϊ 
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sin(halfAngle);

	//    K     ꂽ   x N g  
	Vector3 normalizedAxis = axis.Normalize();

	//  N H [ ^ j I   ̐     v Z
	float w = cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	// Vector4  Ƃ  ĕԂ 
	return Quaternion(x, y, z, w);
}