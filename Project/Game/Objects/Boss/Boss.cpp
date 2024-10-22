#include "Boss.h"


#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"

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
#include"Math/Random.h"
#undef max

void Boss::Initialize() {
	Model* model = ModelManager::Load("bossMainBody");
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);
	meshRenderer_->SetMaterial("mainbody");
	auto collider = AddComponent<BoxCollider>(model);

	er_ = AddComponent<EarthRenderer>();
	er_->SetRadius(radius_);

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
	nextDamageCollTime_ = 1.0f;

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

	const uint32_t kParticleMaxNum = 12u;

	particleDataArray_.resize(kParticleMaxNum);
	ParticleSystem* particleSystem = AddComponent<ParticleSystem>(kParticleMaxNum, "axis");


	/// パーティクルデータの初期化
	for (auto& data : particleDataArray_) {
		/*	data.pivot.Initialize();*/
		data.rotateSpeed = Random::Float(5.0f, 10.0f);
		data.transform.Initialize();
		data.transform.position = { 0,0,-12 };
	}

	/// パーティクルの挙動
	particleSystem->SetParticleLifeTime(3.0f);
	particleSystem->SetEmittedParticleCount(10);
	particleSystem->SetParticleRespawnTime(0.6f);
	particleSystem->SetUseBillboard(false);

	particleSystem->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();
		ParticleData& data = particleDataArray_[particle->GetID()];

		// 回転処理
		data.transform.rotate.z += data.rotateSpeed * Time::DeltaTime();
		data.velocity.z += (-kGravity_ * Time::DeltaTime());

		//変位
		data.transform.position += (data.velocity / 0.0166f) * Time::DeltaTime();/// 0.0166f

		// 反発する
		if (data.transform.position.z > -(Ground::groundScale_-1)) {
			data.transform.position.z = -(Ground::groundScale_-1);
			// 反発係数により反発する
			data.velocity.z *= reboundFactor_;
			data.rotateSpeed *= reboundFactor_;
		}

		transform->position += GetPosition();

		/*	data.time += Time::DeltaTime();
			transform->rotate = data.rotate;
			transform->scale = data.scale;

			transform->position = {
				std::cos(data.time * data.speed) * data.radius,
				std::sin(data.time * data.speed) * data.radius,
				particle->GetNormLifeTime() * data.maxPosY
			};

			transform->position = Mat4::Transform(transform->position, matRotate_);
			transform->position += GetPosition();*/
		});
}

void Boss::Update() {

	er_->SetRadius(radius_);
	er_->SetColor(paintOutColor_);
	//振る舞い更新
	behavior_->Update();
	//敵のビルが一定数溜まったら
	if (pBuildingManager_->GetInBossBuilding().size() >= size_t(kBuildingNum_)&& !dynamic_cast<BossBulletShot*>(behavior_.get())) {
		ChangeState(std::make_unique<BossBulletShot>(this));
	}
	//ダメージ処理
	if (isHitBack_) {
		damageCoolTime_ -= Time::DeltaTime();

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
		float rotationSpeed = 20.0f; // 回転速度、必要に応じて調整
		Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::DeltaTime());

		// ホーミング移動のスピードを設定
		Quaternion move = ToQuaternion({ -0.6f * Time::DeltaTime(), 0, 0 });

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
	if ( nextDamageTime_ >= 0) {
		nextDamageTime_ -= Time::DeltaTime();
	}	

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
	isSlurping_ = false;
	slurpCooldownTimer_ = kSlurpCollTime_;
}

void Boss::SlurpUpdate() {
	// 一番近いビルを取得
	BaseBuilding* closestBuilding = FindClosestBuilding();
	if (closestBuilding && !isSlurping_&&slurpCooldownTimer_<=0.0f) {  // すでに吸い込まれていないか確認
		// フラグを立てる
		closestBuilding->SetSlurpPos(pBossHead_->GetPosition());
		closestBuilding->SetIsSlurped(true);
		isSlurping_ = true;
	}
	//吸い込みしてないとき
	if (!isSlurping_) {
		slurpCooldownTimer_ -= Time::DeltaTime();
		if (slurpCooldownTimer_ <= 0.0f) {//minを使うべき
			slurpCooldownTimer_ = 0.0f;
		}
	}
}

void Boss::BulletShotInit() {

}
void Boss::BulletShotUpdate() {

}

void Boss::AttackInit() {
	
	attackEaseT_ = 0.0f;
	attackCoolTime_ = 0.0f;
	pBossTubu_->ParamaterInit();
	isAttackBack_ = false;
	isAttack_ = true;
	pBossHead_->SetIsAttackCollision(false);
}

void Boss::AttackUpdate() {//超汚い

	pBossHead_->AttackUpdate();

	if (!isAttackBack_) {
		attackEaseT_ += Time::DeltaTime();
		if (attackEaseT_ >= kAttackEaseT_-0.1f) {
			pBossHead_->SetIsAttackCollision(true);
		}
		if (attackEaseT_ >= kAttackEaseT_) {
			pBossHead_->SetERRadius(0.0f);
			attackEaseT_ = kAttackEaseT_;
			attackCoolTime_ += Time::DeltaTime();
			if (attackCoolTime_ >= kAttackCoolTime_) {
				isAttackBack_ = true;
			}
		}
	}//戻る
	else if (isAttackBack_) {
		attackEaseT_ -= Time::DeltaTime();
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
	pBossTubu_->SetPositionY(EaseInBack(4.8f, 3.2f, attackEaseT_, kAttackEaseT_));
	pBossTubu_->SetPositionZ(EaseInBack(-1.0f, 2.0f, attackEaseT_, kAttackEaseT_));
}

void  Boss::AttackChaseUpdate() {
	pBossHead_->LightFlashing();
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
	if (dynamic_cast<Tornado*>(collision)&& nextDamageTime_ <=0&& !isHitBack_) {
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
			if (totalDamage>0) {
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
		HP_ = 0.0f;
	}
}

