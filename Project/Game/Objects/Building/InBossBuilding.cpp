#include"InBossBuilding.h"
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

//初期化
void InBossBuilding::Initialize() {
	speed_ = Random::Float(4.0f, 5.0f);//回転スピード
	radius_ = Random::Float(-0.5f, 0.5f);//半径
	pTransform_->scale = { 0.2f,0.2f,0.2f };
}
//更新
void InBossBuilding::Update() {
	//回転速度を加算
	theta_ += speed_ * Time::DeltaTime();
	phi_ += speed_ * Time::DeltaTime();
	//球面座標を計算
	float x = radius_ * sin(theta_);
	float y = radius_ * cos(theta_);
	pTransform_->position = { x,y,-5 };

	if (!isDeath_) {
		if (pBoss_->GetIsBuildingKill()) {
			isDeath_ = true;
			pTransform_->parent_ = nullptr;
			
		}
	}
}
//デバッグ
void InBossBuilding::Debug() {
	if (ImGui::TreeNode("name")) {
	
		ImGui::TreePop();
	}
}

void InBossBuilding::SetModel(Model* model) {
	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(model);
}

void InBossBuilding::SetBoss(Boss* boss) {
	pBoss_ = boss;
	pTransform_->SetParent(boss->GetTransform());//取るね――ドペアレント
}
