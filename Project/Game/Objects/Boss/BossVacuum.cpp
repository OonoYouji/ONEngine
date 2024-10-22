#include"BossVacuum.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <GraphicManager/Drawer/Material/Material.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/SphereCollider.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"
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

	bossHeadEr_ = new BossHeadEr();
	bossHeadEr_->Initialize();
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
		bossHeadEr_->RootInit();
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
		
		bossHeadEr_->AttackInit();
		pTransform_->position.z = 0.0f;
		pTransform_->position.x = 0.0f;
		pTransform_->position.y = 0.16f;
		pTransform_->SetParent(pBossTube_->GetTransform());
		isAttackInit_ = true;
		isRootinit_ = false;
	}
}
void BossHead::AttackUpdate() {
	bossHeadEr_->Update();
}

void BossHead::SetBossTube(BossTubu* bossTube) {
	pBossTube_ = bossTube;
	pTransform_->SetParent(pBossTube_->GetParent());
	bossHeadEr_->SetParent(pBossTube_->GetParent());
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

void BossHead::SetERRadius(float radius) {
	bossHeadEr_->SetERRadius(radius);
}

void  BossHead::LightFlashing() {
	bossHeadEr_->LightFlashing();
}

///////////////////////////////////////////////////////////////////////////////////////////
// BossER
////////////////////////////////////////////////////////////////////////////////////////////

void  BossHeadEr::Initialize() {

	er_ = AddComponent<EarthRenderer>();
	er_->SetRadius(0.0f);
	pTransform_->position.y = 2.6f;
	pTransform_->position.z = 2.6f;
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->rotateOrder = QUATERNION;
}
void  BossHeadEr::Update() {


}
void  BossHeadEr::Debug() {
	if (ImGui::TreeNode("Paramater")) {
		ImGui::DragFloat("radius", &radius_, 0.05f);
		/*ImGui::ColorEdit3("paint out color", &paintOutColor_.x);*/
		ImGui::TreePop();
	}
}

void BossHeadEr::RootInit() {
	er_->SetRadius(0.0f);
}
void BossHeadEr::AttackInit() {
	lightTime_ = 0.0f;
	er_->SetRadius(3.2f);
	paintOutColor_ = Vec4::kRed; 
	er_->SetColor(paintOutColor_);  // 赤に切り替え
}

void BossHeadEr::LightFlashing() {
	lightTime_ += Time::DeltaTime();
	er_->SetColor(paintOutColor_);  // 赤に切り替え

	if (lightTime_ >= 0.5f) {  // 0.5秒ごとに色を変える

		// 色の切り替え処理
		if (paintOutColor_==Vector4::kRed) {
			paintOutColor_=Vec4::kWhite;  // 白に切り替え
		}
		else {
			paintOutColor_ = Vec4::kRed; // 赤に切り替え
		}
		lightTime_ = 0.0f;  // 時間をリセット
	}

}

void BossHeadEr::SetERRadius(float radius) { er_->SetRadius(radius); }