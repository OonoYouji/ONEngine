#define NOMINMAX
#include "BossBulletLump.h"

/// std
#include <numbers>

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include <Input/Input.h>

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

/// objects
#include "Objects/Boss/Boss.h"


void BossBulletLump::Initialize() {
	Model* model = ModelManager::Load("bossMainBody");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);
	/// transform initialize
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;
	/// action param initialize
	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.Initialize();
	pTransform_->SetParent(&pivot_);
	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };//ピボット
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->position.z = -12;
	pTransform_->scale = { 2,2,2 };
	isDeath_ = false;
}

void BossBulletLump::Update() {
	//移動
	Quaternion moveQuaternion = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, 0.02f);
	pivot_.quaternion *= moveQuaternion;
	pivot_.UpdateMatrix();

	//無敵時間
	invincibleTime_ += Time::DeltaTime();
}

void BossBulletLump::Debug() {

	if(ImGui::TreeNode("this param")) {
		ImGui::DragFloat4("pivot", &pivot_.quaternion.x);
		ImGui::TreePop();
	}
}

void BossBulletLump::SetDirection(Quaternion direction) {
	pivot_.quaternion = direction; 
	pivot_.UpdateMatrix();
}

void BossBulletLump::SetBoss(Boss* boss) {
	/*pBoss_ = boss;*/
	/*pivot_.SetParent(pBoss_->GetTransform());*/
}


void BossBulletLump::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	if (dynamic_cast<Boss*>(collision)&&!isDeath_) {
		if (invincibleTime_ >= kInvincibleTime_) {
			isDeath_ = true;
		}
	}
}