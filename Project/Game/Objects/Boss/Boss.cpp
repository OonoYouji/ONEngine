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
#undef max

Vec3 RotateVectorByQuaternion(const Vec3& vector, const Quaternion& rotation) {
	// クォータニオンによる回転を適用する
	Quaternion vectorAsQuaternion(0, vector.x, vector.y, vector.z); // ベクトルをクォータニオンに変換
	Quaternion rotatedVector = rotation * vectorAsQuaternion * rotation.Conjugate(); // クォータニオンの回転計算

	// 回転されたベクトルを返す
	return Vec3(rotatedVector.x, rotatedVector.y, rotatedVector.z);
}

void Boss::Initialize() {
	Model* model = ModelManager::Load("bossMainBody");
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);
	meshRenderer_->SetMaterial("uvChecker");
	auto collider = AddComponent<BoxCollider>(model);

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

void Boss::Update() {
	
	//振る舞い更新
	behavior_->Update();
	//敵のビルが一定数溜まったら
	if (pBuildingManager_->GetInBossBuilding().size() >= size_t(kBuildingNum_)&& !dynamic_cast<BossBulletShot*>(behavior_.get())) {
		ChangeState(std::make_unique<BossBulletShot>(this));
	}
	//ダメージ処理
	if (isHitBack_) {

		damageCoolTime_ -= Time::DeltaTime();

		// プレイヤーとボスの間の距離と方向を計算
		std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
			pPlayer_->GetPosition(), GetPosition(), Ground::groundScale_ + 1.0f);

		// ヒットバックの方向ベクトルを計算（プレイヤーからボスへの逆方向）
		Vec3 hitBackDirection = Vector3::Normalize(GetPosition() - pPlayer_->GetPosition());

		// ヒットバックのスピードを設定
		float hitBackSpeed = 0.5f * Time::DeltaTime(); // ヒットバックのスピードを調整

		// 現在の回転を取得
		Quaternion currentRotation = GetPivotQuaternion();

		// ヒットバック方向をローカル空間で計算（クォータニオンを使って方向を回転させる）
		Vec3 localHitBackDirection = RotateVectorByQuaternion(hitBackDirection, currentRotation);

		// ヒットバックの移動量を計算
		Vec3 moveAmount = localHitBackDirection * hitBackSpeed;

		// 移動量を基に新しいクォータニオンを生成（ここで回転を生成）
		Quaternion moveRotation = Quaternion::MakeFromAxis(Vector3::Normalize(localHitBackDirection), Vector3::Length(moveAmount));

		// ボスのクォータニオンを更新（移動量を反映）
		pivot_.quaternion *= moveRotation; // 回転の適用

		if (damageCoolTime_ <= 0) {
			meshRenderer_->SetColor(Vec4::kWhite);
			isHitBack_ = false;
		}
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
}

void Boss::AttackUpdate() {//超汚い
	if (!isAttackBack_) {

		attackEaseT_ += Time::DeltaTime();
		if (attackEaseT_ >= kAttackEaseT_) {

			if(attackCoolTime_ == 0.0f) {
				/// 振り下ろしたときの効果音再生
				audioSource_->PlayOneShot("bossAttackTubeDown.wav", 0.5f);
			}

			/// easeTをマックスに合わせる
			attackEaseT_     = kAttackEaseT_;
			attackCoolTime_ += Time::DeltaTime();

			/// 振り下ろし → TubeHeadを上げる動作に遷移
			if (attackCoolTime_ >= kAttackCoolTime_) {
				isAttackBack_ = true;
			}
		}
	}//戻る
	else if (isAttackBack_) {

		attackEaseT_ -= Time::DeltaTime();
		if (attackEaseT_ <= 0.0f) {
			attackEaseT_ = 0.0f;
			ChangeState(std::make_unique<BossChasePlayer>(this));
			isAttack_ = false;
		}
	}
	pBossTubu_->SetPositionY(EaseInBack(4.8f, 4.0f, attackEaseT_, kAttackEaseT_));
	pBossTubu_->SetPositionZ(EaseInBack(-1.0f, 2.7f, attackEaseT_, kAttackEaseT_));
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


void Boss::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {
	if (InTornadoBuilding* tornadoBuilding = dynamic_cast<InTornadoBuilding*>(collision)) {
		int scaleIndex = tornadoBuilding->GetScaleArrayIndex();

		const std::vector<float> damageValues = { 0.05f, 0.1f, 0.2f }; 

		if (scaleIndex >= 0 && scaleIndex < damageValues.size()) {
			isHitBack_ = true;
			damageCoolTime_ = kDamageCoolTime_;
			meshRenderer_->SetColor(Vec4(0.7f,0,0,1));
			DamageForPar(damageValues[scaleIndex]);
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
