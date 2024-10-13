#include "Boss.h"


#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

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
#undef max

void Boss::Initialize() {
	Model* model = ModelManager::Load("axis");
	//mesh
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.Initialize();
	ChangeState(std::make_unique<BossChasePlayer>(this));
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	pTransform_->quaternion = { 0,0,0,1 };
	SpeedParamater_ = 0.01f;
	pTransform_->position.z = -(Ground::groundScale_+1);
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
}

void Boss::Update() {
	//振る舞い更新
	behavior_->Update();

	pivot_.UpdateMatrix();
}

//通常更新
void Boss::RootInit() {

}
void Boss::RootUpdate() {

}
//ストーカー
void Boss::ChaseInit() {

}
void Boss::ChaseUpdate() {
	
}

//建物吸引
void Boss::SlurpInit() {

}
void Boss::SlurpUpdate() {
	// 一番近いビルを取得
	BaseBuilding* closestBuilding = FindClosestBuilding();
	if (closestBuilding && !closestBuilding->GetIsSlurped()) {  // すでに吸い込まれていないか確認
		// フラグを立てる
		closestBuilding->SetIsSlurped(true);
	}
}

void Boss::Debug() {
	if (ImGui::TreeNode("Paramater")) {
		ImGui::DragFloat("ChaseSpeedMax", &SpeedParamater_, 0.001f);
		ImGui::TreePop();
	}
}

void Boss::ChangeState(std::unique_ptr<BaseBossBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}

void Boss::SetPlayer(Player*player) {
	pPlayer_ = player;
}

void Boss::SetBuildingaManager(BuildingManager* buildingmanager) {
	pBuildingManager_ = buildingmanager;
}


// 一番近いビルを探す
BaseBuilding* Boss::FindClosestBuilding() {
	if (!pBuildingManager_) return nullptr; // BuildingManagerがセットされていない場合

	const std::list<BaseBuilding*>& buildings = pBuildingManager_->GetBuildings();
	BaseBuilding* closestBuilding = nullptr;
	float minDistance = std::numeric_limits<float>::max(); //初期値を最大に

	Vector3 bossPosition = GetPosition();
	float sphereRadius = Ground::groundScale_ + 1; // 地面の半径＋α

	for (BaseBuilding* building : buildings) {
		Vector3 buildingPosition = building->GetPosition();

		// 球面距離を計算
		auto [distance, direction] = CalculateDistanceAndDirection(bossPosition,buildingPosition, sphereRadius);

		if (distance < minDistance) {
			minDistance = distance;
			building->SetDirection(direction);
			closestBuilding = building;
		}
	}
	return closestBuilding;
}
std::pair<float, float> Boss::CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition, const float& radius) {
	// ボスとプレイヤーの位置を3次元座標から球面座標に変換
	float latitude1 = std::asin(bossPosition.y / Vector3::Length(bossPosition)); // ボスの緯度
	float longitude1 = std::atan2(bossPosition.z, bossPosition.x); // ボスの経度

	float latitude2 = std::asin(targetPos.y / Vector3::Length(targetPos)); // プレイヤーの緯度
	float longitude2 = std::atan2(targetPos.z, targetPos.x); // プレイヤーの経度

	// 中心角を計算
	float deltaSigma = std::acos(
		std::sin(latitude1) * std::sin(latitude2) +
		std::cos(latitude1) * std::cos(latitude2) * std::cos(longitude2 - longitude1)
	);

	// 球面上の距離を計算
	float sphereRadius =radius; // 半径
	float distance = sphereRadius * deltaSigma;

	// 方位角を計算
	float deltaLon = longitude2 - longitude1;
	float y = std::sin(deltaLon) * std::cos(latitude2);
	float x = std::cos(latitude1) * std::sin(latitude2) - std::sin(latitude1) * std::cos(latitude2) * std::cos(deltaLon);
	float direction = std::atan2(y, x);

	return { distance, direction }; // 距離と方位角を返す

}
