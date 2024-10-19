#include "Enemy.h"


#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <GraphicManager/Drawer/Material/Material.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"
//std
#include <algorithm>
#include<numbers>
#include <limits>
#include "ImGuiManager/ImGuiManager.h"
#include"FrameManager/Time.h"
//obj
#include"Objects/Player/Player.h"
#include"Objects/Building/BuildingManager.h"
#include"Objects/Ground/Ground.h"
#include"Objects/Tornado/Tornado.h"
//function
#include"Easing/EasingFunction.h"
#include"HormingFunction/Horming.h"
#undef max

void Enemy::Initialize() {
	Model* model = ModelManager::Load("Enemy");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	er_ = AddComponent<EarthRenderer>();
	er_->SetRadius(radius_);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.Initialize();

	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////

	Quaternion initQuater = Quaternion::MakeFromAxis(Vec3::kRight, 2.0f);
	pivot_.quaternion = initQuater;
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->scale = { 1,1,1 };
	speedParamager_ = 0.2f;
	pTransform_->position.z = -(Ground::groundScale_ + 1);
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////	
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;
	///////////////////////////////////////////////////////////////////////////////////////////
	//	ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////	
	pTransform_->SetParent(&pivot_);
	//行列更新
	pivot_.UpdateMatrix();
	UpdateMatrix();

	ChangeState(std::make_unique<EnemyRoot>(this));
}

void Enemy::Update() {
	
	//振る舞い更新
	behavior_->Update();


	pivot_.UpdateMatrix();
}

//通常更新
void Enemy::RootInit() {
	easeT_ = 0.0f;
	easeTimeMax_ = 1.0f;
	amplitude_ = 0.2f;
	period_ = 0.1f;
	perceptionDistance_ = 6.0f;
	isChase_ = false;

}
void Enemy::RootUpdate() {
	// 距離と方向を計算
	std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
	GetPlayer()->GetPosition(), GetPosition(), Ground::groundScale_ + 1.0f);

	if (distanceAndDirection.first <= perceptionDistance_ && !isChase_) {
		isChase_ = true;
	}
	if (isChase_) {//チェイスモードに以降

		// 現在の回転をオイラー角に変換
		Vec3 euler = QuaternionToEulerAngles(GetPivotQuaternion());

		// プレイヤーの方向を向くための回転を計算
		Quaternion targetRotation = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

		// 現在の回転
		Quaternion currentRotation = GetPivotQuaternion();

		// 回転をスムーズに補間 (Slerpを使用)
		float rotationSpeed = 10.0f; // 回転速度、必要に応じて調整
		Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::DeltaTime());

		// 回転を更新
	SetPivotQuaternion(interpolatedRotation);

		easeT_ += Time::DeltaTime();
		if (easeT_ >= easeTimeMax_) {
			easeT_ = easeTimeMax_;
			ChangeState(std::make_unique<EnemyChasePlayer>(this));
		}
		SetScale(EaseAmplitudeScale(Vec3(1.0f, 1.0f, 1.0f), easeT_, easeTimeMax_, amplitude_, period_));

	}
}
//ストーカー
void Enemy::ChaseInit() {

}
void Enemy::ChaseUpdate() {

}

void Enemy::Debug() {
	if (ImGui::TreeNode("Paramater")) {
		ImGui::DragFloat("ChaseSpeedMax", &speedParamager_, 0.001f);
		ImGui::DragFloat("radius", &radius_, 0.05f);
		ImGui::ColorEdit3("paint out color", &paintOutColor_.x);
		ImGui::TreePop();
	}
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}


void Enemy::SetPlayer(Player* player) {
	pPlayer_ = player;
}

void Enemy::SetBuildingaManager(BuildingManager* buildingmanager) {
	pBuildingManager_ = buildingmanager;
}


void Enemy::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {
	if ( dynamic_cast<Tornado*>(collision)) {
		isInTornado_ = true;
	}
}
