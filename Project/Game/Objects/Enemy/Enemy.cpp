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
	ChangeState(std::make_unique<EnemyRoot>(this));
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////

	Quaternion initQuater = Quaternion::MakeFromAxis(Vec3::kRight, 2.0f);
	pivot_.quaternion = initQuater;
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->scale = { 2,2,2 };
	speedParamager_ = 0.5f;
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
}

void Enemy::Update() {
	
	//振る舞い更新
	behavior_->Update();
	////敵のビルが一定数溜まったら
	//if (pBuildingManager_->GetInBossBuilding().size() >= size_t(kBuildingNum_)&& !dynamic_cast<BossBulletShot*>(behavior_.get())) {
	//	ChangeState(std::make_unique<EnemyRoot>(this));
	//}


	pivot_.UpdateMatrix();
}

//通常更新
void Enemy::RootInit() {

}
void Enemy::RootUpdate() {

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
	if (InTornadoBuilding* tornadoBuilding = dynamic_cast<InTornadoBuilding*>(collision)) {
		int scaleIndex = tornadoBuilding->GetScaleArrayIndex();

		const std::vector<float> damageValues = { 0.05f, 0.1f, 0.2f }; 

		if (scaleIndex >= 0 && scaleIndex < damageValues.size()) {
		
		}
	}
}
