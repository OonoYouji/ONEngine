#include"BossVacuum.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <GraphicManager/Drawer/Material/Material.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>
#include"FrameManager/Time.h"
#include "ImGuiManager/ImGuiManager.h"
//obj
#include "Objects/Boss/Boss.h"
#include"Easing/EasingFunction.h"
//std
#include<numbers>

void BossTubu::Initialize() {
	Model* model = ModelManager::Load("bossTubu");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);
	//浮遊サイクル
	ParamaterInit();
}
void BossTubu::Update() {
	if (!pBoss_->GetIsAttack()) {
		// 1フレームでのパラメータ加算値
		const float step = 2.0f * std::numbers::pi_v<float> / floatingCycle_;
		// パラメータを1ステップ分加算
		floatingParameter_ += step * Time::DeltaTime() * 60;
		floatingParameter_ = std::fmod(floatingParameter_, 2.0f * std::numbers::pi_v<float>);
		// 浮遊の振幅＜m＞
		/*floatingAmplitude_ = 0.2f;*/
		// 浮遊を座標に反映
		pTransform_->position.z = std::sin(floatingParameter_) * floatingAmplitude_;
	}
}
void BossTubu::Debug() {
	
}

void BossTubu::SetBoss(Boss* boss) {
	pBoss_ = boss;
	pTransform_->SetParent(pBoss_->GetTransform());
}

void BossTubu::ParamaterInit() {
	floatingCycle_ = 80.0f;
	floatingAmplitude_ = 0.5f;
	pTransform_->position.z = -1.0f;
	pTransform_->position.y = 4.8f;
	pTransform_->rotate.x = -0.9f;
}
///////////////////////////////////////////////////////////////////////////////////////////
// Bosshead
////////////////////////////////////////////////////////////////////////////////////////////

void BossHead::Initialize() {
	Model* model = ModelManager::Load("bossHead");
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	ParamaterInit();
}
void BossHead::Update() {
	if (!pBossTube_->GetBoss()->GetIsAttack()) {
		RootInit();
		// 1フレームでのパラメータ加算値
		const float step = 2.0f * std::numbers::pi_v<float> / floatingCycle_;
		// パラメータを1ステップ分加算
		floatingParameter_ += step * Time::DeltaTime() * 60;
		floatingParameter_ = std::fmod(floatingParameter_, 2.0f * std::numbers::pi_v<float>);
		// 浮遊を座標に反映
		pTransform_->position.z = std::sin(floatingParameter_) * floatingAmplitude_;
	}
	else {
		AttackInit();
	}
}
void BossHead::Debug() {

}

void BossHead::RootInit() {
	if (!isRootinit_) {
		floatingCycle_ = 65.0f;
		floatingAmplitude_ = 0.3f;
		pTransform_->position.z = 0.0f;
		pTransform_->position.x = 0.1f;
		pTransform_->position.y = 4.5f;
		isAttackInit_ = false;
		isRootinit_ = true;
		pTransform_->SetParent(pBossTube_->GetParent());
	}
}
void BossHead::AttackInit() {
	if (!isAttackInit_) {

		pTransform_->position.z = 0.0f;
		pTransform_->position.x = 0.0f;
		pTransform_->position.y = 0.16f;
		pTransform_->SetParent(pBossTube_->GetTransform());
		isAttackInit_ = true;
		isRootinit_ = false;
	}
}


void BossHead::SetBossTube(BossTubu* bossTube) {
	pBossTube_ = bossTube;
	pTransform_->SetParent(pBossTube_->GetParent());
}

void BossHead::ParentBoss() {
	pTransform_->SetParent(pBossTube_->GetParent());
}
void BossHead::ParentTubu() {
	pTransform_->SetParent(pBossTube_->GetTransform());
}

void BossHead::ParamaterInit() {
	//浮遊サイクル
	floatingCycle_ = 65.0f;
	floatingAmplitude_ = 0.3f;
	pTransform_->position.z = 0.0f;
	pTransform_->position.x = 0.1f;
	pTransform_->position.y = 4.5f;
}