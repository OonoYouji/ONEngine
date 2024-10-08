#include "BaseBuilding.h"

#include <ModelManager.h>

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
#include <Component/Collider/SphereCollider.h>
#include <Component/SplinePathRenderer/SplinePathRenderer.h>
#include <Component/Collider/BoxCollider.h>

#include <ImGuiManager.h>
#include"random/random.h"

//object
#include"Player/Player.h"


void BaseBuilding::Initialize() {

	Model* model=ModelManager::Load("TestObject");


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
	pivot_.quaternion = { 0,0,0,1 };
	pTransform_->position = { 0,0,-14 };
	pTransform_->rotate = { -1.5f,0,0 };
	behaviorRequest_ = Behavior::kRoot;
	////////////////////////////////////////////////////////////////////////////////////////////
    //  ペアレント
    ////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();
	UpdateMatrix();
	/*isActive = false;*/
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
	theta_ = distTheta(gen);
	phi_ = distPhi(gen);
	pTransform_->SetParent(player->GetTransform());
	
}
void BaseBuilding::ParentUpdate() {
	// 球面座標から位置を計算
	float radius = 8.0f;
	theta_ += 0.04f;
	phi_ += 0.04f;
	float x = radius * sin( theta_) * cos(phi_);
	float y = -2+radius * sin(theta_) * sin(phi_);
	/*float z = radius * cos(theta_);*/
	pTransform_->position = { x,y,-10 };
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
			ParentUpdate();
			break;

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

void BaseBuilding::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {
	
	if (dynamic_cast<Player*>(collision)&&!isCollisionTyphoon_) {
		isCollisionTyphoon_ = true;
		behaviorRequest_ = Behavior::kParent;
	}

}
