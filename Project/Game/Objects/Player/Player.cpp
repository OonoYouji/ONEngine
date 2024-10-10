#include "Player.h"
#include"Building/BaseBuilding.h"

#include <ModelManager.h>

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
#include <Component/Collider/SphereCollider.h>
#include <Component/Collider/BoxCollider.h>
#include <Component/SplinePathRenderer/SplinePathRenderer.h>

#include <ImGuiManager.h>
#include<Time/Time.h>

void Player::Initialize() {
	Model* model = ModelManager::Load("axis");
	//mesh
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model);
	auto collider = AddComponent<BoxCollider>(model);

	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	transoform_.Initialize();
	pivot_.Initialize();
	///////////////////////////////////////////////////////////////////////////////////////////
	// 値セット
	////////////////////////////////////////////////////////////////////////////////////////////
	pivot_.quaternion = { 0,0,0,1 };
	transoform_.quaternion = { 0,0,0,1 };
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->position.z = -12;
	transoform_.position.z=-12;
	powerUpGaugeMax_ = 100;
	powerUpTimeMax_ = 5.0f;//秒
	/*SetPositionZ(-1.0f);*/
	///////////////////////////////////////////////////////////////////////////////////////////
	// 回転モード
	////////////////////////////////////////////////////////////////////////////////////////////
	transoform_.rotateOrder = QUATERNION;
	pTransform_->rotateOrder = QUATERNION;
	pivot_.rotateOrder = QUATERNION;

	////ペアレント
	transoform_.SetParent(&pivot_);
	pTransform_->SetParent(&pivot_);

	pivot_.UpdateMatrix();
	UpdateMatrix();
}

void Player::Update() {
	BehaviorManagement();//振る舞い管理
	Move();//移動

	pivot_.Update();
	transoform_.Update();
}

void Player::Move() {
	//入力
	input_ = {
		static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
		static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
		0.0f
	};

	/// 移動の正規化
	input_ = input_.Normalize() * 0.1f;
	velocity_ = Vec3::Lerp(velocity_, input_, 0.1f);

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
void Player::RootUpdate() {
	
}
void Player::PowerUpInit() {
	isPowerUp_ = true;
	powerUpTime_ = powerUpTimeMax_;
}
void Player::PowerUpUpdate() {
	powerUpTime_-=Time::DeltaTime();//デルタタイムに直す
	if (powerUpTime_ <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::PowerUpGaugeUp(float par) {

	float incrementGauge = powerUpGaugeMax_ * par;

	// 現在のゲージ値に増加分を追加s
	powerUpGauge_ += incrementGauge;

	// ゲージが最大値を超えないように制限
	if (powerUpGauge_ > powerUpGaugeMax_) {
		behaviorRequest_ = Behavior::kPowerUp;
		
	}
}

void Player::Debug() {
	if(ImGui::TreeNode("pivot")) {
		pivot_.Debug();
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateX_.x, rotateX_.y, rotateX_.z, rotateX_.w);
		ImGui::Text("X:%f Y:%f Z:%f W:%f", rotateY_.x, rotateY_.y, rotateY_.z, rotateY_.w);
		ImGui::DragFloat3("velocity", &velocity_.x, 0);
		ImGui::DragFloat("rotateXAngle", &rotateXAngle_, 0.01f);
		ImGui::DragFloat("rotateYAngle", &rotateYAngle_, 0.01f);
			ImGui::TreePop();
	}
	if (ImGui::TreeNode("Parameter")) {
		ImGui::DragFloat("PowerUpGauge", &powerUpGauge_, 0.01f);
		ImGui::DragFloat("PowerUpGaugeMax", &powerUpGaugeMax_, 0.01f);
		ImGui::DragFloat("PowerUpTime", &powerUpTime_, 0.01f);
		ImGui::DragFloat("PowerUpTimeMax", &powerUpTimeMax_, 0.01f);

		ImGui::TreePop();
	}
}

//振る舞い管理
void Player::BehaviorManagement() {
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			RootInit();
			break;
		case Behavior::kPowerUp:
			PowerUpInit();
			break;
		
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	// 振る舞い更新
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		RootUpdate();
		break;
	case Behavior::kPowerUp:
		PowerUpUpdate();
		break;
	
	}

}

void Player::OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {

	if (dynamic_cast<BaseBuilding*>(collision)) {
		PowerUpGaugeUp(0.3f);
	}

}
