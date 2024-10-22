#define NOMINMAX
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
#include"Easing/EasingFunction.h"
#include "Math/Random.h"
//obj
#include"Objects/Building/BuildingManager.h"
#include"Objects/Boss/BossVacuum.h"
#include"Objects/Tornado/Tornado.h"
#include"Objects/Ground/Ground.h"
#include"Objects/Boss/Boss.h"
#include "Objects/CameraState/GameCameraState.h"


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
	pivot_.quaternion       = { 0, 0, 0, 1 };
	transoform_.quaternion  = { 0, 0, 0, 1 };
	pTransform_->quaternion = { 0, 0, 0, 1 };
	pTransform_->position.z = -(Ground::groundScale_);
	transoform_.position.z  = -(Ground::groundScale_);

	pTransform_->scale = Vec3::kOne * 0.5f;

	powerUpGaugeMax_ = 100;
	powerUpTimeMax_ = 10.0f;//秒
	HP_ = HPMax_;
	//ヒットバック力
	hitBackPower_ = -0.05f;

	//ダメージ
	damageForBossHead_.kStopCollTime = 0.5f;
	damageForBossHead_.DamagePar = 0.05f;

	damageForBossBullet_.kStopCollTime = 0.5f;
	damageForBossBullet_.DamagePar = 0.2f;

	damageForBossBody_.kStopCollTime = 0.3f;
	damageForBossBody_.DamagePar = 0.0f;

	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////
	transoform_.rotateOrder  = QUATERNION;
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder       = QUATERNION;

	////ペアレント
	transoform_.SetParent(&pivot_);
	pTransform_->SetParent(&pivot_);

	//回転を適応
	rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, 0.0f);
	rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, 0.0f);

	pivot_.quaternion *= rotateX_ * rotateY_;// 正規化

	pivot_.UpdateMatrix();
	UpdateMatrix();


	/// grow size parameter
	timeRate_ = 1.0f; /// hit stopのtime rateの初期化

}

void Player::Update() {

	/// 前フレームのデータをコピー
	preCameraBehavior_ = cameraBehavior_; /// カメラの振る舞い


	float maxSpeed = 0.6f;
	float minSpeed = 0.5f;

	float t = (pTornado_->GetScaleScaler() - pTornado_->GetMinScale()) / (pTornado_->GetMaxScale() - pTornado_->GetMinScale());
	t = std::clamp(t, 0.0f, 1.0f);  // tを0.0～1.0に制限

	// speed_を計算
	speed_ = LerpE(maxSpeed, minSpeed, t);
	//振る舞い更新
	behavior_->Update();

	er_->SetRadius(radius_);
	er_->SetColor(paintOutColor_);
	//ストップしてない限り動ける
	if (!damageForBossHead_.isStop && !damageForBossBullet_.isStop&&!damageForBossBody_.isStop) {
		Move();//移動
	}

	//直接攻撃によるスタン
	damageForBossHead_.stopCollTime -= Time::TimeRateDeltaTime();
	if (damageForBossHead_.stopCollTime <= 0.0f) {
		damageForBossHead_.stopCollTime = 0.0f;
		damageForBossHead_.isStop = false;
	}

	//弾によるスタン
	damageForBossBullet_.stopCollTime -= Time::TimeRateDeltaTime();
	if (damageForBossBullet_.stopCollTime <= 0.0f) {
		damageForBossBullet_.stopCollTime = 0.0f;
		damageForBossBullet_.isStop = false;
	}

	//体によるスタン
	damageForBossBody_.stopCollTime -= Time::TimeRateDeltaTime();
	if (damageForBossBody_.stopCollTime <= 0.0f) {
		damageForBossBody_.stopCollTime = 0.0f;
		damageForBossBody_.isStop = false;
	}

	if (damageForBossBody_.isStop) {
		// ヒットバックの方向と強さを適用する
		velocity_ = preInput_.Normalize() * (hitBackPower_ * Time::TimeRateDeltaTime());

		// 回転を逆方向に適応させる
		float rotateXAngle_ = +velocity_.y;
		float rotateYAngle_ = -velocity_.x;

		Quaternion rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, rotateXAngle_);
		Quaternion rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, rotateYAngle_);

		// 回転を更新する
		pivot_.quaternion *= rotateX_ * rotateY_;
		pivot_.UpdateMatrix();
		transoform_.UpdateMatrix();
	}

	pivot_.UpdateMatrix();
	transoform_.UpdateMatrix();
}


/// <summary>
/// プレイヤーの移動
/// </summary>
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
	if (input_ != Vec3(0.0f, 0.0f, 0.0f)) {
		preInput_ = input_;
	}
	/// 移動の正規化
	input_ = input_.Normalize() * (speed_ * Time::TimeRateDeltaTime());
	velocity_ = Vec3::Lerp(velocity_, input_, 0.05f);
	
	Vec3 velocity = velocity_;
	if(isPowerUp_) {
		velocity *= 2.0f;
	}

	rotateXAngle_ = +velocity.y;
	rotateYAngle_ = -velocity.x;

	if (velocity != Vec3(0.0f, 0.0f, 0.0f)) {

		//回転を適応
		rotateX_ = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, rotateXAngle_);
		rotateY_ = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, rotateYAngle_);

		// プレイヤーの向きの決定

		Quaternion quaternionLocalZ = Quaternion::MakeFromAxis({ 0.0f, 0.0f, 1.0f }, std::atan2(velocity.x, velocity.y));

		pivot_.quaternion *= (rotateX_ * rotateY_);// 正規化
		pTransform_->quaternion = quaternionLocalZ.Conjugate();
	}
}



/// ==============================================================================================================
///			behavior
/// ==============================================================================================================

/// 振る舞い関数
void Player::RootInit() {
	//パワーアップパラメータ初期化
	isPowerUp_ = false;
	powerUpGauge_ = 0;
}

void Player::PowerUpInit() {
	isPowerUp_   = true;
	powerUpTime_ = powerUpTimeMax_;
	timeRate_    = 0.005f;
	hitStopTime_ = 0.5f;
}

/// <summary>
/// パワーアップしているときに呼び出される
/// </summary>
void Player::PowerUpUpdate() {

	/// 残り時間の減少
	powerUpTime_ -= Time::TimeRateDeltaTime();

}

void Player::PowerUpGaugeUp(float par) {

	float incrementGauge = powerUpGaugeMax_ * par;

	// 現在のゲージ値に増加分を追加s
	powerUpGauge_ += incrementGauge;

	// ゲージが最大値を超えないように制限
	if (powerUpGauge_ > powerUpGaugeMax_) {
		powerUpGauge_ = powerUpGaugeMax_;

		/// 現在のステートがpower up state出なければ
		PlayerPowerUp* powerUpBehavior = dynamic_cast<PlayerPowerUp*>(behavior_.get());
		if(!powerUpBehavior) {
			/// カメラの振る舞いを変化させる
			cameraBehavior_ = static_cast<int>(CameraBehavior::kZoomIn);

			ChangeState(std::make_unique<PlayerPowerUp>(this));
		}
	}
}



void Player::ChangeState(std::unique_ptr<BasePlayerBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}


void Player::SetCameraBehavior(int behavior) {
	cameraBehavior_ = behavior;
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

void Player::TimeRateUpdate() {

	hitStopTime_ -= Time::DeltaTime();
	if(hitStopTime_ > 0.0f) {
		//timeRate_ = Random::Int(0, 1) + 1 * 0.1f;
	} else {
		timeRate_ += Time::DeltaTime();
		timeRate_ = std::min(timeRate_, 1.0f);
	}

	Time::SetTimeRate(timeRate_);
}

void  Player::SetBuildingManager(BuildingManager* buildingManager) {
	pBuindingManager_ = buildingManager;
}

void Player::SetTornado(Tornado* tornado) {
	pTornado_ = tornado;
}

void Player::DamageForBossHead() {
	if (!damageForBossHead_.isStop) {
		DamageForPar(damageForBossHead_.DamagePar);
		damageForBossHead_.isStop = true;
		damageForBossHead_.stopCollTime = damageForBossHead_.kStopCollTime;
		//指定の数分ビル破壊
		pBuindingManager_->SetDeathFlagInBuildings(5);
	}
}
void Player::DamageForBossBullet() {
	if (!damageForBossBullet_.isStop) {
		DamageForPar(damageForBossBullet_.DamagePar);
		damageForBossBullet_.isStop = true;
		damageForBossBullet_.stopCollTime = damageForBossBullet_.kStopCollTime;
		//指定の数分ビル破壊
		pBuindingManager_->SetDeathFlagInBuildings(10);
	}
}

//ボスと当たった時のダメ―じ
void Player::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	if (dynamic_cast<Boss*>(collision) && !damageForBossBody_.isStop) {
		DamageForPar(damageForBossBullet_.DamagePar);
		damageForBossBody_.isStop = true;
		damageForBossBody_.stopCollTime = damageForBossBody_.kStopCollTime;
	}
}

/// ==============================================================================================================
///			debug
/// ==============================================================================================================


void Player::Debug() {

	if(ImGui::TreeNode("pivot")) {
		pivot_.Debug();
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateX_.x, rotateX_.y, rotateX_.z, rotateX_.w);
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateY_.x, rotateY_.y, rotateY_.z, rotateY_.w);
		ImGui::DragFloat3("velocity", &velocity_.x, 0);
		ImGui::DragFloat("speed", &speed_, 0.05f);
		ImGui::DragFloat("rotateXAngle", &rotateXAngle_, 0.01f);
		ImGui::DragFloat("rotateYAngle", &rotateYAngle_, 0.01f);
		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Parameter")) {

		ImGui::SeparatorText("power up");

		ImGui::DragFloat("PowerUpGauge", &powerUpGauge_);
		ImGui::DragFloat("PowerUpGaugeMax", &powerUpGaugeMax_, 0.01f);
		ImGui::DragFloat("PowerUpTime", &powerUpTime_, 0.01f);
		ImGui::DragFloat("PowerUpTimeMax", &powerUpTimeMax_, 0.01f);

		ImGui::DragFloat("hit stop time rate", &timeRate_);


		ImGui::SeparatorText("status");

		ImGui::DragFloat("HP", &HP_, 0.01f);
		ImGui::DragFloat("DmageForBossHead", &damageForBossHead_.DamagePar, 0.01f);
		ImGui::DragFloat("DamageForBossBullet", &damageForBossBullet_.DamagePar, 0.01f);

		ImGui::Spacing();

		ImGui::DragFloat("radius", &radius_, 0.05f);
		ImGui::ColorEdit3("paint out color", &paintOutColor_.x);

		ImGui::TreePop();
	}
}
