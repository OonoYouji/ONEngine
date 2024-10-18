#include "BaseBuilding.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include "CustomComponents/EarthRenderer/EarthRenderer.h"

#include "ImGuiManager/ImGuiManager.h"
//math
#include"Math/Random.h"
#include"FrameManager/Time.h"
//std
#include<numbers>
//object
#include"Objects/Ground/Ground.h"
#include"Objects/Boss/BossVacuum.h"
#include"Objects/Boss/Boss.h"
//function
#include"Easing/EasingFunction.h"
#include"HormingFunction/Horming.h"

void BaseBuilding::Initialize() {

	model_ = ModelManager::Load("TestObject");

	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(model_);
	auto collider = AddComponent<BoxCollider>(model_);

	earthRenderer_ = AddComponent<EarthRenderer>();
	earthRenderer_->SetRadius(shadowRaidus_);
	earthRenderer_->SetColor(shadowColor_);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->Initialize();
	pivot_.Initialize();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };//ピボット
	pTransform_->position = { 0,0,buildingSartZ };//ポジション
	pTransform_->rotate = { -1.5f,0,0 };//回転
	pTransform_->scale = { 1.0f,0.1f,1.0f };//スケール
	scaleMax_ = 1.0f;

	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();

	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();
}

void BaseBuilding::Update() {
	

	earthRenderer_->SetRadius(shadowRaidus_);
	earthRenderer_->SetColor(shadowColor_);

	//吸われる処理
	if(isSlurp_) {
		//建物を浮かせるイージング
		floatBuildingEaseTime_ += Time::DeltaTime();
		if(floatBuildingEaseTime_ >= floatBuildingEaseTimeMax_) {
			floatBuildingEaseTime_ = floatBuildingEaseTimeMax_;
		}
		pTransform_->rotate.x = EaseOutQuint(-1.5f, 0.4f, floatBuildingEaseTime_, floatBuildingEaseTimeMax_);
		pTransform_->position.z = EaseInSine(buildingSartZ, -14.5f, floatBuildingEaseTime_, floatBuildingEaseTimeMax_);

		// 球面距離を計算
		auto [distance, direction] = CalculateDistanceAndDirection(slurpPos_, GetPosition(), Ground::groundScale_ + 1);

		Vec3 euler = QuaternionToEulerAngles(pivot_.quaternion);

		// プレイヤーの方向を向くための回転を計算
		Quaternion inter = ToQuaternion({ euler.x, euler.y, -direction });

		// ホーミング移動のスピードを設定
		Quaternion move = ToQuaternion({ 0.01f, 0, 0 });

		// 回転を更新
		pivot_.quaternion = inter;
		pivot_.quaternion *= move;

		//ある程度近づいたら
		if(distance <= 2.0f) {//パラメータ化するかも
			isTaken_ = true;
		}
	} else {
		//成長
		growTime_ += Time::DeltaTime();
		GrowForTime(0.2f, 2.0f);
		// 回転を適用
		Quaternion rotateX = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, pos_.second);
		Quaternion rotateY = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, pos_.first);
		pivot_.quaternion = (rotateX * rotateY);
	}

	//ピボット更新
	pivot_.UpdateMatrix();
}

void BaseBuilding::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit4("color", &shadowColor_.x);
		ImGui::DragFloat("radius", &shadowRaidus_, 0.1f);
		ImGui::DragFloat("phi", &pos_.first, 0.01f);
		ImGui::DragFloat("theta", &pos_.second, 0.01f);

		ImGui::TreePop();
	}
}

void BaseBuilding::GrowForTime(const float& par, const float& second) {

	//割合によるインクる面とする値を決める
	float incrementSize = scaleMax_ * par;

	if(growTime_ >= second) {//毎秒
		// 現在のスケール値に増加分を追加s
		pTransform_->scale.y += incrementSize;
		// スケールが最大値を超えないように制限
		if(pTransform_->scale.y > scaleMax_) {
			pTransform_->scale.y = scaleMax_;
		}
		growTime_ = 0.0f;
	}
}

void BaseBuilding::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {
	//当たったら用済み
	if(dynamic_cast<Player*>(collision) && !isSlurp_) {
		isInTornado_ = true;
	}

	//当たったら用済み
	if(dynamic_cast<Boss*>(collision) && isSlurp_) {
		isTaken_ = true;
	}

	//当たったら用済み
	if(dynamic_cast<BossHead*>(collision) &&pBoss_->GetIsAttack()) {
		isBreak_ = true;
	}
}

void BaseBuilding::SetBoss(Boss*boss) {
	pBoss_ = boss;
}