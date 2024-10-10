#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
//std
#include<optional>

class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void Move();//移動
	void BehaviorManagement();//振る舞い管理

	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;


	//振る舞い関数
	void RootInit();
	void RootUpdate();
	void PowerUpInit();
	void PowerUpUpdate();
	
	//getter
	Transform* GetPivot() { return &pivot_; }
	Transform* GetbaseTransform() { return &transoform_; }
	float GetPowerUpGauge() { return powerUpGauge_; }
	const Vec3& GetVelocity() const { return velocity_; }
	bool GetisPowerUp()const { return isPowerUp_; }
	//setter
	void PowerUpGaugeUp(float par);

private:
	enum class Behavior {
		kRoot,
		kPowerUp,
	};

private:
	
	//速度
	Vec3 input_;
	Vec3 velocity_;
	//クォータニオンRotate
	Quaternion rotateX_;
	Quaternion rotateY_;
	//ピボット
	Transform pivot_;
	Transform transoform_;
	//パワーアップゲージ
	float powerUpGaugeMax_;
	float powerUpGauge_;
	//パワーアップタイム
	float powerUpTimeMax_;
	float powerUpTime_;
	//パワーアップフラグ
	bool isPowerUp_;
	//// ふるまい
	Behavior behavior_;
	//振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	AudioSource* audioSource_ = nullptr;

	float rotateXAngle_;
	float rotateYAngle_;
	
};