#include "Player.h"

/// std
#include <numbers>

#include"Objects/Building/BaseBuilding.h"

#include "ONEngine/GraphicManager/ModelManager/ModelManager.h"

#include "Input/Input.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Collider/BoxCollider.h>
#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"

#include "ImGuiManager/ImGuiManager.h"
#include"FrameManager/time.h"
//obj
#include"Objects/Building/BuildingManager.h"
#include"Objects/Building/BuildingManager.h"
#include"Objects/Boss/BossVacuum.h"
#include"Objects/Ground/Ground.h"

void Player::Initialize() {
	Model* model = ModelManager::Load("playerInGame");
	//mesh
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	er_ = AddComponent<EarthRenderer>();
	er_->SetRadius(radius_);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	transoform_.Initialize();
	pivot_.Initialize();
	ChangeState(std::make_unique<PlayerRoot>(this));
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	speed_ = 1.0f;
	pivot_.quaternion = { 0,0,0,1 };
	transoform_.quaternion = { 0,0,0,1 };
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->position.z = -(Ground::groundScale_ + 3);
	transoform_.position.z = -(Ground::groundScale_ + 3);

	pTransform_->scale = Vec3::kOne * 0.5f;

	powerUpGaugeMax_ = 100;
	powerUpTimeMax_ = 5.0f;//秒
	HP_ = HPMax_;

	//ダメージ
	damageForBossHead_.kStopCollTime = 0.5f;
	damageForBossHead_.DamagePar = 0.05f;

	damageForBossBullet_.kStopCollTime = 0.5f;
	damageForBossBullet_.DamagePar = 0.2f;
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////
	transoform_.rotateOrder = QUATERNION;
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	transoform_.SetParent(&pivot_);
	pTransform_->SetParent(&pivot_);

	//回転を適応
	rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, 0.0f);
	rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, 0.0f);


	pivot_.quaternion *= rotateX_ * rotateY_;// 正規化


	pivot_.UpdateMatrix();
	UpdateMatrix();
}

void Player::Update() {

	behavior_->Update();

	er_->SetRadius(radius_);
	er_->SetColor(paintOutColor_);
	//ストップしてない限り動ける
	if (!damageForBossHead_.isStop&& !damageForBossBullet_.isStop) {
		Move();//移動
	}

	//直接攻撃によるスタン
	damageForBossHead_.stopCollTime -= Time::DeltaTime();
	if (damageForBossHead_.stopCollTime <= 0.0f) {
		damageForBossHead_.stopCollTime = 0.0f;
		damageForBossHead_.isStop = false;
	}

	//弾によるスタン
	damageForBossBullet_.stopCollTime -= Time::DeltaTime();
	if (damageForBossBullet_.stopCollTime <= 0.0f) {
		damageForBossBullet_.stopCollTime = 0.0f;
		damageForBossBullet_.isStop = false;
	}

	pivot_.UpdateMatrix();
	transoform_.UpdateMatrix();
}

void Player::Move() {
	//入力

	input_ = {};
	Vec2 gamePadInput = Input::GetLeftStick();

	/// game pad入力
	if (gamePadInput != Vec2(0.0f, 0.0f)) {
		input_ = { gamePadInput.x, gamePadInput.y, 0.0f };
	}

	if (Input::PressKey(KeyCode::W)) { input_.y = +1.0f; }
	if (Input::PressKey(KeyCode::A)) { input_.x = -1.0f; }
	if (Input::PressKey(KeyCode::S)) { input_.y = -1.0f; }
	if (Input::PressKey(KeyCode::D)) { input_.x = +1.0f; }

	/// 移動の正規化
	input_ = input_.Normalize() * (speed_ * Time::DeltaTime());
	velocity_ = Vec3::Lerp(velocity_, input_, 0.05f);

	rotateXAngle_ = +velocity_.y;
	rotateYAngle_ = -velocity_.x;

	if (velocity_ != Vec3(0.0f, 0.0f, 0.0f)) {

		//回転を適応
		rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, rotateXAngle_);
		rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, rotateYAngle_);

		// プレイヤーの向きの決定
		Quaternion quaternionLocalZ = Quaternion::MakeFromAxis({ 0.0f, 0.0f, 1.0f }, std::atan2(velocity_.x, velocity_.y));

		pivot_.quaternion *= rotateX_ * rotateY_;// 正規化
		pTransform_->quaternion = quaternionLocalZ.Conjugate();
	}
}

//振る舞い関数
void Player::RootInit() {
	//パワーアップパラメータ初期化
	isPowerUp_ = false;
	powerUpGauge_ = 0;
}

void Player::PowerUpInit() {
	isPowerUp_ = true;
	powerUpTime_ = powerUpTimeMax_;
}

void Player::PowerUpUpdate() {
	powerUpTime_ -= Time::DeltaTime();//デルタタイムに直す	
}

void Player::PowerUpGaugeUp(float par) {

	float incrementGauge = powerUpGaugeMax_ * par;

	// 現在のゲージ値に増加分を追加s
	powerUpGauge_ += incrementGauge;

	// ゲージが最大値を超えないように制限
	if (powerUpGauge_ > powerUpGaugeMax_) {
		powerUpGauge_ = powerUpGaugeMax_;
		ChangeState(std::make_unique<PlayerPowerUp>(this));
	}
}

void Player::Debug() {

	if (ImGui::TreeNode("pivot")) {
		pivot_.Debug();
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateX_.x, rotateX_.y, rotateX_.z, rotateX_.w);
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateY_.x, rotateY_.y, rotateY_.z, rotateY_.w);
		ImGui::DragFloat3("velocity", &velocity_.x, 0);
		ImGui::DragFloat("speed", &speed_, 0.05f);
		ImGui::DragFloat("rotateXAngle", &rotateXAngle_, 0.01f);
		ImGui::DragFloat("rotateYAngle", &rotateYAngle_, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Parameter")) {
		ImGui::DragFloat("PowerUpGauge", &powerUpGauge_, 0.01f);
		ImGui::DragFloat("PowerUpGaugeMax", &powerUpGaugeMax_, 0.01f);
		ImGui::DragFloat("PowerUpTime", &powerUpTime_, 0.01f);
		ImGui::DragFloat("PowerUpTimeMax", &powerUpTimeMax_, 0.01f);
		ImGui::DragFloat("HP", &HP_, 0.01f);
		ImGui::DragFloat("DmageForBossHead", &damageForBossHead_.DamagePar, 0.01f);
		ImGui::DragFloat("DamageForBossBullet", &damageForBossBullet_.DamagePar, 0.01f);

		ImGui::Spacing();

		ImGui::DragFloat("radius", &radius_, 0.05f);
		ImGui::ColorEdit3("paint out color", &paintOutColor_.x);

		ImGui::TreePop();
	}
}

void Player::ChangeState(std::unique_ptr<BasePlayerBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}


void Player::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	if (dynamic_cast<BaseBuilding*>(collision) && !dynamic_cast<PlayerPowerUp*>(behavior_.get())) {
		PowerUpGaugeUp(0.05f);
	}
	//ボスの直接攻撃によるダメージ
	if (dynamic_cast<BossHead*>(collision) && !dynamic_cast<PlayerPowerUp*>(behavior_.get())) {
		if (!damageForBossHead_.isStop) {
			DamageForPar(damageForBossHead_.DamagePar);
			damageForBossHead_.isStop = true;
			damageForBossHead_.stopCollTime = damageForBossHead_.kStopCollTime;
			//指定の数分ビル破壊
			pBuindingManager_->SetDeathFlagInBuildings(5);
		}
	}

	//ボスの弾によるダメージ
	if (dynamic_cast<BossBulletLump*>(collision) && !dynamic_cast<PlayerPowerUp*>(behavior_.get())) {
		if (!damageForBossBullet_.isStop) {
			DamageForPar(damageForBossBullet_.DamagePar);
			damageForBossBullet_.isStop = true;
			damageForBossBullet_.stopCollTime = damageForBossBullet_.kStopCollTime;
			//指定の数分ビル破壊
			pBuindingManager_->SetDeathFlagInBuildings(10);
		}
	}
}
//割合によるダメージ
void Player::DamageForPar(const float& par) {

	//割合によるインクる面とする値を決める
	float decrementSize = HPMax_ * par;
	// HP減少
	HP_ -= decrementSize;
	//HPが0以下にならないように
	if (HP_ <= 0) {
		HP_ = 0.0f;
	}
}

//チュートリアルの挙動
void Player::TutorialMove() {
	pivot_.UpdateMatrix();
	transoform_.UpdateMatrix();
}

void  Player::SetBuildingManager(BuildingManager* buildingManager) {
	pBuindingManager_ = buildingManager;
}