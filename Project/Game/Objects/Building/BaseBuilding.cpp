#include "BaseBuilding.h"

/// std
#include <numbers>

/// engine
#include "Input/Input.h"
#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "CustomComponents/EarthRenderer/EarthRenderer.h"

#include "ImGuiManager/ImGuiManager.h"
/// math
#include"Math/Random.h"
#include"FrameManager/Time.h"
/// std
#include<numbers>


/// objects
#include "Objects/Boss/Boss.h"
#include "Objects/Boss/BossVacuum.h"
#include "Objects/Ground/Ground.h"
#include "Objects/Boss/BossVacuum.h"

/// custom function
#include "Easing/EasingFunction.h"
#include "HormingFunction/Horming.h"



void BaseBuilding::Initialize() {

	/// 大きさごとのモデルを設定
	modelArray_ = {
		ModelManager::Load("TestObject"),
		ModelManager::Load("TestObject"),
		ModelManager::Load("TestObject")
	};

	auto mesh = AddComponent<MeshRenderer>();
	mesh->SetModel(modelArray_[0]);
	auto collider = AddComponent<BoxCollider>(modelArray_[0]);

	earthRenderer_ = AddComponent<EarthRenderer>();
	earthRenderer_->SetRadius(shadowRaidus_);

	particleSystem_ = AddComponent<ParticleSystem>(128, "Sphere");
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetEmittedParticleCount(2);
	particleSystem_->SetParticleEmitterFlags(PARTICLE_EMITTER_NOTIME);
	particleSystem_->SetBoxEmitterMinMax(-Vec3::kOne * 2.0f, Vec3::kOne * 2.0f);
	particleSystem_->SetParticleLifeTime(1.0f);

	particleSystem_->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();

		Vec3 scale = Vec3::kOne * 10.0f * particle->GetNormLifeTime();
		transform->scale = scale;

		Vec3 velocity = GetPosition().Normalize() * 5.0f;
		transform->position += velocity * Time::DeltaTime();
	});


	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////

	pivot_.Initialize();


	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////

	pivot_.quaternion = { 0,0,0,1 };//ピボット
	pTransform_->position = { 0,0,buildingSartZ };//ポジション
	pTransform_->rotate = { -1.5f,0,0 };//回転
	scaleMax_ = 1.0f;
	
	offsetPosition_ = pTransform_->position;

	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////

	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();

	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();

	
	/// 現在の状態
	currentScaleIndex_ = BUILDING_SCALE_SMALL;

	/// 影の色の配列
	shadowColorArray_ = {
		Vec4::kRed, 
		Vec4::kGreen, 
		Vec4::kBlue
	};
	
	/// 影の色の半径
	shadowRadiusArray_ = {
		0.75f, 1.3f, 2.2f
	};

	/// 各大きさのスケール
	buildingScaleArray_ = {
		0.5f, 1.0f, 1.5f
	};

	/// 次の大きさに行くための時間
	nextScalingTimeArray_ = {
		5.0f, 5.0f, 5.0f
	};

}

void BaseBuilding::Update() {

	// 吸われる処理
	if(isSlurp_) {
		//建物を浮かせるイージング
		floatBuildingEaseTime_ += Time::DeltaTime();
		if(floatBuildingEaseTime_ >= floatBuildingEaseTimeMax_) {
			floatBuildingEaseTime_ = floatBuildingEaseTimeMax_;
		}
		pTransform_->rotate.x = EaseOutQuint(-1.5f, 0.4f, floatBuildingEaseTime_, floatBuildingEaseTimeMax_);
		offsetPosition_.z = EaseInSine(buildingSartZ, -14.5f, floatBuildingEaseTime_, floatBuildingEaseTimeMax_);


		// 球面距離を計算
		auto [distance, direction] = CalculateDistanceAndDirection(slurpPos_, GetPosition(), Ground::groundScale_ + 1);

		Vec3 euler = QuaternionToEulerAngles(pivot_.quaternion);

		// プレイヤーの方向を向くための回転を計算
		Quaternion inter = ToQuaternion({ euler.x, euler.y, -direction });

		// ホーミング移動のスピードを設定
		Quaternion move = ToQuaternion({ 0.5f*Time::DeltaTime(), 0, 0});

		// 回転を更新
		pivot_.quaternion = inter;
		pivot_.quaternion *= move;

		//ある程度近づいたら
		if(distance <= 2.0f) {//パラメータ化するかも
			isTaken_ = true;
		}
	} else {

		/// 吸われない場合の処理を以下に記載
		
		Animation(); /// ぴょこぴょこ

		/// 成長
		growTime_ += Time::DeltaTime();

		/// 次の大きさに変わるための処理

		if(currentScaleIndex_ < BUILDING_SCALE_BIG) {
			nextScalingTimeArray_[currentScaleIndex_] -= Time::DeltaTime();
			if(nextScalingTimeArray_[currentScaleIndex_] < 0.0f) {
				currentScaleIndex_++;
				hp_ = hp_ + 1.0f;

				/// 中から大に変わるときの設定
				if(currentScaleIndex_ == BUILDING_SCALE_BIG) {
					particleSystem_->SetEmittedParticleCount(3); /// 一回のemitで出現するパーティクルの数
				}

				/// SetBurst(bool, float 1, float 2);
				/// float 1 -> バーストの全体時間           0.75秒かけてバーストする
				/// float 2 -> 一回バーストするのにかかる時間 0.05秒に一回バーストする
				particleSystem_->SetBurst(true, 0.75f, 0.05f);
			}
		}

		// 回転を適用
		Quaternion rotateX = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, pos_.first);
		Quaternion rotateY = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, pos_.second);
		pivot_.quaternion = (rotateX * rotateY);

		shake_ = shake_ * 0.1f;
		pTransform_->position = offsetPosition_ + shake_;

	}

	
	/// 影の色と大きさを設定
	earthRenderer_->SetRadius(shadowRadiusArray_[currentScaleIndex_]);
	earthRenderer_->SetColor(shadowColorArray_[currentScaleIndex_]);


	/// ピボット更新
	pivot_.UpdateMatrix();
}

void BaseBuilding::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		/// ---------------------------------------------------
		/// 今の状態
		/// ---------------------------------------------------
		ImGui::SeparatorText("parameter");
		ImGui::SliderInt("current building scale index", &currentScaleIndex_, 0, BUILDING_SCALE_COUNT - 1);

		ImGui::DragFloat("phi", &pos_.first, 0.01f);
		ImGui::DragFloat("theta", &pos_.second, 0.01f);

		/// ---------------------------------------------------
		/// shadowのデバッグ
		/// ---------------------------------------------------
		if(ImGui::TreeNodeEx("shadow", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat("radius", &shadowRaidus_, 0.1f);
		
			ImGui::Spacing();

			for(size_t i = 0; i < BUILDING_SCALE_COUNT; ++i) {
				std::string colorLabel  = std::string("color_") + std::to_string(i);
				std::string radiusLabel = std::string("radius") + std::to_string(i);
				ImGui::ColorEdit4(colorLabel.c_str(), &shadowColorArray_[i].x, ImGuiColorEditFlags_AlphaBar);
				ImGui::DragFloat(radiusLabel.c_str(), &shadowRadiusArray_[i], 0.05f);
			}

			ImGui::TreePop();
		}

		/// ---------------------------------------------------
		/// animationのデバッグ
		/// ---------------------------------------------------
		if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat("time",  &animationTime_,  0.01f);
			ImGui::DragFloat("speed", &animationSpeed_, 0.01f);


			ImGui::TreePop();
		}

		

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

void BaseBuilding::Animation() {
	animationTime_ += Time::DeltaTime()*2;

	float sinValue = std::sin(animationTime_ * animationSpeed_) * 0.5f + 0.5f;
	float scaleXZ = buildingScaleArray_[currentScaleIndex_] + 0.25f * -sinValue;
	float scaleY = buildingScaleArray_[currentScaleIndex_] + 0.25f * +sinValue;

	pTransform_->scale.x = scaleXZ;
	pTransform_->scale.y = scaleY;
	pTransform_->scale.z = scaleXZ;
}

void BaseBuilding::SubHP(float _subValue) {
	hp_ -= _subValue;

	/// トルネードに吸い込まれる
	if(hp_ <= 0.0f) {
		isInTornado_ = true;
	}
}

void BaseBuilding::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	//当たったら用済み
	//if(dynamic_cast<Tornado*>(collision) && !isSlurp_) {
	//	isInTornado_ = true;
	//}

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

void BaseBuilding::SetShake(const Vec3& _shaleValue) {
	shake_ = _shaleValue;
}
