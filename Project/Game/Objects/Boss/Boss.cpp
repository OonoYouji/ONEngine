#include "Boss.h"


#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"

#include <GraphicManager/Drawer/Material/Material.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"
#include "ComponentManager/AudioSource/AudioSource.h"

//std
#include <algorithm>
#include <numbers>
#include <limits>
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
//obj
#include"Objects/Player/Player.h"
#include"Objects/Building/BuildingManager.h"
#include"Objects/Ground/Ground.h"
//function
#include"Easing/EasingFunction.h"
#include"HormingFunction/Horming.h"
#include"Math/Random.h"
#undef max

void Boss::Initialize() {
	Model* model = ModelManager::Load("bossMainBodyGame");
	Model* collisionModel = ModelManager::Load("bossMainBodyCollisionBox");
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);
	meshRenderer_->SetMaterial("mainbody");
	auto collider = AddComponent<BoxCollider>(collisionModel);

	er_ = AddComponent<EarthRenderer>();
	er_->SetRadius(radius_);

	audioSource_ = AddComponent<AudioSource>();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.Initialize();
	ChangeState(std::make_unique<BossChasePlayer>(this));
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////

	Quaternion initQuater = Quaternion::MakeFromAxis(Vec3::kRight, 2.0f);
	pivot_.quaternion = initQuater;
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->scale = { 2,2,2 };
	SpeedParamater_ = 0.5f;
	pTransform_->position.z = -(Ground::groundScale_ + 1);
	HPMax_ = 100.0f;
	HP_ = HPMax_;
	nextDamageCollTime_ = 0.3f;

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
	isexp_ = false;
	const uint32_t kParticleMaxNum = 15u;

	//パーティクルAddComponent
	particleDataArray_.resize(kParticleMaxNum);
	particleSystem_ = AddComponent<ParticleSystem>(kParticleMaxNum, "bossAtackEffect");

	/// パーティクルの挙動
	particleSystem_->SetEmittedParticleCount(10);
	particleSystem_->SetEmitterFlags(PARTICLE_EMITTER_NOTIME);

	/// パーティクル初期化
	ParticleInit();
	/// パーティクル更新
	ParticleUpdate();
}


void Boss::Update() {

	er_->SetRadius(radius_);
	er_->SetColor(paintOutColor_);
	//振る舞い更新
	behavior_->Update();
	//敵のビルが一定数溜まったら
	if (pBuildingManager_->GetInBossBuilding().size() >= size_t(kBuildingNum_) && !dynamic_cast<BossBulletShot*>(behavior_.get())) {
		ChangeState(std::make_unique<BossBulletShot>(this));
	}
	//ダメージ処理
	if (isHitBack_) {
		damageCoolTime_ -= Time::TimeRateDeltaTime();

		// 距離と方向を計算
		std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
			pPlayer_->GetPosition(), GetPosition(), Ground::groundScale_ + 1.0f);

		// 現在の回転をオイラー角に変換
		Vec3 euler = QuaternionToEulerAngles(GetPivotQuaternion());
		// 現在の回転
		Quaternion currentRotation = GetPivotQuaternion();
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
			meshRenderer_->SetColor(Vec4::kWhite);
			isHitBack_ = false;
		}
	}
	//0より大きかったらnextDamageTimeの減算をする
	if (nextDamageTime_ >= 0) {
		nextDamageTime_ -= Time::TimeRateDeltaTime();
	}

	pivot_.UpdateMatrix();
}

//通常更新
void Boss::RootInit() {

}
void Boss::RootUpdate() {

}

void Boss::StopSound() {
	audioSource_->PlayOneShot("BossStop.wav", 0.5f);
}
//ストーカー
void Boss::ChaseInit() {

}
void Boss::ChaseUpdate() {

}

void Boss::KoteiSound() {
	audioSource_->PlayOneShot("bossAtack1Kotei.wav", 0.5f);
}


void Boss::ShotSound() {
	audioSource_->PlayOneShot("BossBulletShot.wav", 0.5f);
}

//void Boss::SlurpInit() {
//	audioSource_->PlayOneShot("bossAtack2kouho1.wav", 0.5f);
//}



//建物吸引
void Boss::SlurpInit() {
	audioSource_->PlayOneShot("bossAtack2kouho1.wav", 0.5f);
	isSlurping_ = false;
	slurpCooldownTimer_ = kSlurpCollTime_;
}

void Boss::SlurpUpdate() {
	//if (!audioSource_->IsPlayingAudio()) {
	//	audioSource_->PlayOneShot("fanfare", 0.5f);//吸い込んだ時
	//}
	// 一番近いビルを取得
	BaseBuilding* closestBuilding = FindClosestBuilding();
	if (closestBuilding && !isSlurping_ && slurpCooldownTimer_ <= 0.0f) {  // すでに吸い込まれていないか確認
		// フラグを立てる
		closestBuilding->SetSlurpPos(pBossHead_->GetPosition());
		closestBuilding->SetIsSlurped(true);
		isSlurping_ = true;
	}
	//吸い込みしてないとき
	if (!isSlurping_) {
		slurpCooldownTimer_ -= Time::TimeRateDeltaTime();
		if (slurpCooldownTimer_ <= 0.0f) {//minを使うべき
			slurpCooldownTimer_ = 0.0f;
		}
	}
}

void Boss::BulletShotInit() {}

void Boss::BulletShotUpdate() {
	

}

/// パーティクル初期化
void Boss::ParticleInit() {
	/// パーティクルデータの初期化
	for (auto& data : particleDataArray_) {
		data.reflectionCount = 0;
		data.rotateSpeed = Random::Float(5.0f, 10.0f);/// 回転スピード
		data.transform.Initialize();	/// Transform初期化
		data.pivot.Initialize();//pivot初期化
		data.pivot = pivot_;//pivot代入
		data.transform.position = { 0,Random::Float(6,7) ,-(Ground::groundScale_ + 1) };
		data.velocity = { Random::Float(-1.0f,1.0f),Random::Float(-1.0f,1.0f),-5 };/// 速度
	}
}

/// <summary>
///  パーティクル更新
/// </summary>
void Boss::ParticleUpdate() {

	particleSystem_->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();
		ParticleData& data = particleDataArray_[particle->GetID()];


		transform->SetParent(&data.pivot);
		// 回転処理
		data.transform.rotate.z += data.rotateSpeed * Time::DeltaTime();
		data.velocity.z += (kGravity_ * Time::DeltaTime());

		// 変位
		data.transform.position += (data.velocity) * Time::DeltaTime();

		// 反発する
		if (data.transform.position.z > -(Ground::groundScale_ - positionOfset_)) {
			data.transform.position.z = -(Ground::groundScale_ - positionOfset_);
			// 反発係数により反発する
			data.velocity.z *= reboundFactor_;
			data.rotateSpeed *= reboundFactor_;
			data.reflectionCount++;  // 反発カウントインクリメント
		}

		// 反発カウントが2回以上になったら止める
		if (data.reflectionCount >= 2) {
			data.velocity.x = 0.0f;
			data.velocity.z = 0.0f;
		}

		// 反発カウントが最大に達したらy軸の速度を止める
		if (data.reflectionCount >= reflectionCountMax_) {
			data.velocity.y = 0;
			data.transform.position.z = -(Ground::groundScale_ - positionOfset_);
		}

		// 回転の適用
		float rotateXAngle_ = +data.velocity.y;
		float rotateYAngle_ = -data.velocity.x;

		if (data.velocity != Vec3(0.0f, 0.0f, 0.0f)) {
			// 回転を適応
			Quaternion rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, rotateXAngle_);
			Quaternion rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, rotateYAngle_);

			data.pivot.quaternion *= (rotateX_ * rotateY_);  // 回転の適用と正規化
		}

		// transformの更新
		transform->position = data.transform.position;
		transform->rotate = data.rotate;
		transform->scale = { 0.2f, 0.2f, 0.2f };

		});
}
void Boss::AttackInit() {

	attackEaseT_ = 0.0f;
	attackCoolTime_ = 0.0f;
	pBossTubu_->ParamaterInit();
	isAttackBack_ = false;
	isAttack_ = true;
	isParticle_ = false;
	particleSystem_->SetBurst(false, 0, 0);
	pBossHead_->SetIsAttackCollision(false);

}

void Boss::AttackUpdate() {/// 超汚い

	pBossHead_->AttackUpdate();

	if (!isAttackBack_) {
		attackEaseT_ += Time::TimeRateDeltaTime();
		if (attackEaseT_ >= kAttackEaseT_ - 0.1f) {
			pBossHead_->SetIsAttackCollision(true);
			//emetter
			if (!isParticle_) {
				ParticleInit();
				particleSystem_->SetBurst(true, 0, 0);
				isParticle_ = true;
			}
		}
		if (attackEaseT_ >= kAttackEaseT_) {
			if (attackCoolTime_ == 0.0f) {
				/// 振り下ろしたときの効果音再生
				audioSource_->PlayOneShot("bossTubeDown.wav", 0.5f);
			}
			pBossHead_->SetERRadius(0.0f);
			attackEaseT_ = kAttackEaseT_;
			attackCoolTime_ += Time::TimeRateDeltaTime();

			/// 振り下ろし → TubeHeadを上げる動作に遷移
			if (attackCoolTime_ >= kAttackCoolTime_) {
				isAttackBack_ = true;
			}
		}
	}//戻る
	else if (isAttackBack_) {
		attackEaseT_ -= Time::TimeRateDeltaTime();
		if (attackEaseT_ <= 0.3f) {
			pBossHead_->SetIsAttackCollision(false);

		}
		if (attackEaseT_ <= 0.0f) {

			attackEaseT_ = 0.0f;
			ChangeState(std::make_unique<BossChasePlayer>(this));
			isAttack_ = false;
			pBossHead_->SetIsAttackCollision(false);
		}
	}
	pBossTubu_->SetPositionY(EaseInBack(1.5f, 1.5f, attackEaseT_, kAttackEaseT_));
	pBossTubu_->SetPositionZ(EaseInBack(-0.5f, 0.5f, attackEaseT_, kAttackEaseT_));
}

void  Boss::AttackFixationUpdate() {
	pBossHead_->LightFlashing();
}

void Boss::BulletHitBack() {
	// ホーミング移動のスピードを設定
	Quaternion move = ToQuaternion({ 6.0f * Time::TimeRateDeltaTime(), 0, 0 });
	pivot_.quaternion*=(move); // 移動もスムーズに
}

void Boss::Debug() {
	if (ImGui::TreeNode("Paramater")) {
		ImGui::DragFloat("ChaseSpeedMax", &SpeedParamater_, 0.001f);
		ImGui::DragFloat("radius", &radius_, 0.05f);
		ImGui::DragFloat("HP", &HP_, 0.05f);
		ImGui::DragFloat("HPMax", &HPMax_, 0.05f);
		ImGui::ColorEdit3("paint out color", &paintOutColor_.x);
		ImGui::TreePop();
	}
}

void Boss::ChangeState(std::unique_ptr<BaseBossBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}


// 一番近いビルを探す
BaseBuilding* Boss::FindClosestBuilding() {

	if (!pBuildingManager_) return nullptr; // BuildingManagerがセットされていない場合

	const std::list<BaseBuilding*>& buildings = pBuildingManager_->GetBuildings();
	BaseBuilding* closestBuilding = nullptr;
	float minDistance = std::numeric_limits<float>::max(); //初期値を最大に

	Vector3 bossPosition = pBossHead_->GetPosition();
	float sphereRadius = Ground::groundScale_ + 1; // 地面の半径＋α

	for (BaseBuilding* building : buildings) {
		Vector3 buildingPosition = building->GetPosition();

		// 球面距離を計算
		auto [distance, direction] = CalculateDistanceAndDirection(bossPosition, buildingPosition, sphereRadius);

		if (distance < minDistance) {
			minDistance = distance;
			closestBuilding = building;
		}
	}
	return closestBuilding;
}

void Boss::SetHead(BossHead* bossHead) {
	pBossHead_ = bossHead;
}

void Boss::SetTubu(BossTubu* bossHead) {
	pBossTubu_ = bossHead;
}


void Boss::SetPlayer(Player* player) {
	pPlayer_ = player;
}

void Boss::SetBuildingaManager(BuildingManager* buildingmanager) {
	pBuildingManager_ = buildingmanager;
}

//ボスコリジョン(トルネード)
void Boss::OnCollisionStay([[maybe_unused]] BaseGameObject* const collision) {
	if (dynamic_cast<Tornado*>(collision) && nextDamageTime_ <= 0 && !isHitBack_) {
		if (pBuildingManager_) {

			float totalDamage = 0.0f;
			const std::vector<float> damageValues = { 0.05f, 0.1f, 0.2f };//各ダメージパラメータ

			//巻きこまれてるビルから、ダメージ計算
			for (auto& inTornadoBuilding : pBuildingManager_->GetInTornadoBuildingss()) {
				int scaleIndex = inTornadoBuilding->GetScaleArrayIndex();
				if (scaleIndex >= 0 && scaleIndex < damageValues.size()) {
					totalDamage += damageValues[scaleIndex];//合計値を足していく
				}
			}
			//ダメージが0以上
			if (totalDamage > 0) {
				audioSource_->PlayOneShot("BossDamage.wav", 0.5f);//ダメージ受けた時の効果音
				// 合計ダメージを適用
				isHitBack_ = true;
				damageCoolTime_ = kDamageCoolTime_;
				meshRenderer_->SetColor(Vec4(0.7f, 0, 0, 1));
				DamageForPar(totalDamage);  // 合計ダメージを適用
			}
		}
	}
}

//割合によるダメージ
void Boss::DamageForPar(const float& par) {

	//割合によるインクる面とする値を決める
	float decrementSize = HPMax_ * par;
	// HP減少
	HP_ -= decrementSize;
	//HPが0以下にならないように
	if (HP_ <= 0) {
		if (!isexp_) {
			audioSource_->PlayOneShot("exp.wav", 0.5f);
			isexp_ = true;
		}
		HP_ = 0.0f;
	}
}

