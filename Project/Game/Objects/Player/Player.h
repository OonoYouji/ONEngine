#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
//behavior
#include"Objects/PlayerBehavior/BasePlayerBehavior.h"
#include"Objects/PlayerBehavior/PlayerRoot.h"
#include"Objects/PlayerBehavior/PlayerPowerUp.h"

//std
#include<optional>
#include<memory>

class BuildingManager;
class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void Move();//移動


	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;


	//振る舞い関数
	void RootInit();
	void PowerUpInit();
	void PowerUpUpdate();
	void TutorialMove();

	//getter
	Transform* GetPivot() { return &pivot_; }
	Transform* GetbaseTransform() { return &transoform_; }
	//パワーアップゲージ
	float GetPowerUpGauge() { return powerUpGauge_; }
	//速度
	const Vec3& GetVelocity() const { return velocity_; }
	//パワーアップフラグ
	bool GetisPowerUp()const { return isPowerUp_; }
	//パワーアップタイム
	float GetPowerUpTime()const { return powerUpTime_; }
	//setter
	void PowerUpGaugeUp(float par);

	//状態変更
	void ChangeState(std::unique_ptr<BasePlayerBehavior>behavior);

	void SetBuildingManager(BuildingManager* buildingManager);

private:
	BuildingManager* pBuindingManager_;
	//速度
	Vec3 input_;
	Vec3 velocity_;
	float speed_;
	//クォータニオンRotate
	Quaternion rotateX_;
	Quaternion rotateY_;
	//ピボット
	Transform pivot_;
	Transform transoform_;

	AudioSource* audioSource_ = nullptr;
	//状態
	std::unique_ptr<BasePlayerBehavior>behavior_;

	float rotateXAngle_;
	float rotateYAngle_;

	class EarthRenderer* er_ = nullptr;
	float radius_ = 1.0f;
	Vec4  paintOutColor_ = { 1,1,1,1 };

	////////////////////////////////////////////////////////////////////////////////////////////
	//  パラメータ
	////////////////////////////////////////////////////////////////////////////////////////////
	//パワーアップゲージ
	float powerUpGaugeMax_;
	float powerUpGauge_;
	//パワーアップタイム
	float powerUpTimeMax_;
	float powerUpTime_;
	//移動スピード
	float moveSpeed_;
	//パワーアップフラグ
	bool isPowerUp_;

	//HP
	float HP_;
	const	float HPMax_ = 100.0f;

	//スタン
	bool isStop_;
	float stopCollTime_;
	const float kStopCollTime_=0.5f;
};