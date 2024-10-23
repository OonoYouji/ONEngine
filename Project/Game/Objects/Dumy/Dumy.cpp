#include "Dumy.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include <ComponentManager/Collider/SphereCollider.h>

#include <ComponentManager/SplinePathRenderer/SplinePathRenderer.h>

#include "ImGuiManager/ImGuiManager.h"
//obj
#include"Objects/Ground/Ground.h"
#include"Objects/Building/InTornadoBuilding.h"
//math
#include"Math/Random.h"
#include"FrameManager/Time.h"
//std
#include<numbers>
//function
#include"Easing/EasingFunction.h"
#include"HormingFunction/Horming.h"

void Dumy::Initialize() {

	auto model = ModelManager::Load("bossMainBodyGame");
	mesh_ = AddComponent<MeshRenderer>();

	mesh_->SetModel(model);
	mesh_->SetMaterial("mainbody");
	auto collider = AddComponent<BoxCollider>(model);


	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->Initialize();
	pivot_.Initialize();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };//ピボット
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->position = { 0,0,Ground::groundScale_ + 0.4f };//ポジション
	pTransform_->rotate = { 0,03.3f,0 };//回転
	pTransform_->scale = { 1.0f,1.0f,1.0f };//スケール
	hp_ = hpMax_;
	kEaseTime_ = 1.0f;

	////////////////////////////////////////////////////////////////////////////////////////////
	//  ペアレント
	////////////////////////////////////////////////////////////////////////////////////////////
	pTransform_->SetParent(&pivot_);
	pivot_.UpdateMatrix();
	pTransform_->rotateOrder = XYZ;
	pivot_.rotateOrder = QUATERNION;
	UpdateMatrix();

	audioSource_ = AddComponent<AudioSource>();
	nextDamageCollTime_ = 0.3f;
}

void Dumy::Update() {
	//ダミーの死亡
	if (isFall_) {

		easeTime_ += Time::TimeRateDeltaTime();
		if (easeTime_ >= kEaseTime_) {
			easeTime_ = kEaseTime_;
			isDeath_ = true;
		}
		pTransform_->rotate.y = EaseOutSine<float>(3.3f, 1.550f, easeTime_, kEaseTime_);
	}
	else {
		//ダメージ処理
		if (isHitBack_) {
			damageCoolTime_ -= Time::TimeRateDeltaTime();

			// 距離と方向を計算
			std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
				pPlayer_->GetPosition(), GetPosition(), Ground::groundScale_ + 1.0f);

			// 現在の回転をオイラー角に変換
			Vec3 euler = QuaternionToEulerAngles(pivot_.quaternion);
			// 現在の回転
			Quaternion currentRotation = pivot_.quaternion;
			// プレイヤーの方向を向くための回転を計算
			Quaternion targetRotation = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });
			// 回転をスムーズに補間 (Slerpを使用)
			float rotationSpeed = 10.0f; // 回転速度、必要に応じて調整
			Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::TimeRateDeltaTime());

			// ホーミング移動のスピードを設定
			Quaternion move = ToQuaternion({ -0.6f * Time::TimeRateDeltaTime(), 0, 0 });

			pivot_.quaternion = interpolatedRotation;
			pivot_.quaternion *= (move); // 移動もスムーズに

			// クールダウン処理
			if (damageCoolTime_ <= 0) {
				nextDamageTime_ = nextDamageCollTime_;//次にダメージを受けるまでのクールタイムを設定
				mesh_->SetColor(Vec4::kWhite);
				isHitBack_ = false;
			}
		}
		//0より大きかったらnextDamageTimeの減算をする
		if (nextDamageTime_ >= 0) {
			nextDamageTime_ -= Time::TimeRateDeltaTime();
		}

	}
	//ピボット更新
	pivot_.UpdateMatrix();
}

void Dumy::Debug() {

}


//ボスコリジョン(トルネード)
void Dumy::OnCollisionStay([[maybe_unused]] BaseGameObject* const collision) {
	if (dynamic_cast<Tornado*>(collision) && nextDamageTime_ <= 0 && !isHitBack_&&!isFall_) {


		audioSource_->PlayOneShot("BossDamage.wav", 0.5f);//ダメージ受けた時の効果音
		// 合計ダメージを適用
		isHitBack_ = true;
		damageCoolTime_ = kDamageCoolTime_;
		mesh_->SetColor(Vec4(0.7f, 0, 0, 1));
		hp_--;
		if (hp_ <= 0) {
			isFall_ = true;
			mesh_->SetColor(Vec4::kRed);
		}

	}
}

void Dumy::SetPlyer(Player* player) {
	pPlayer_ = player;
}

//void Dumy::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {
//
//	//当たったら用済み
//	if (dynamic_cast<Tornado*>(collision)) {
//		hp_--;
//		if (hp_ <= 0) {
//			isFall_ = true;
//			mesh_->SetColor(Vec4::kRed);
//		}
//	}
//}

