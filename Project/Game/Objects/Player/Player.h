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
class Tornado;
class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;
	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;


	void Move();//移動

	//振る舞い関数
	void RootInit();
	void PowerUpInit();
	void PowerUpUpdate();
	void TutorialMove();

	/// <summary>
	/// time rateの更新処理
	/// パワーアップ時の演出でヒットストップを演出するための物
	/// </summary>
	void TimeRateUpdate();

	//ダメージ
	void DamageForBossHead();
	void DamageForBossBullet();

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

	/// <summary>
	/// ヒットストップの time rateを返す
	/// </summary>
	/// <returns></returns>
	float GetTimeRate() const { return timeRate_; }

	
	/// <summary>
	/// カメラの振る舞いが変化した瞬間を取得
	/// </summary>
	/// <returns> return: true 振る舞いが変化した、 false 振る舞いは変化してない </returns>
	bool GetIsCameraBehaviorChange() const { return cameraBehavior_ != preCameraBehavior_; }

	/// <summary>
	/// カメラの現在の振る舞いを得る
	/// </summary>
	/// <returns></returns>
	int  GetCameraBehavior() const { return cameraBehavior_; }

	/// <summary>
	/// カメラの振る舞いをセットする
	/// </summary>
	/// <param name="behavior"></param>
	void SetCameraBehavior(int behavior);

	void GaugeDownForPar(const float& par, const float& second);
	//ダメージ
	void DamageForPar(const float& par);
	//setter
	void PowerUpGaugeUp(float par);
	void SetTornado(Tornado* tornado);
	BasePlayerBehavior* GetBehavior()const { return behavior_.get(); }

	//状態変更
	void ChangeState(std::unique_ptr<BasePlayerBehavior>behavior);

	void SetBuildingManager(BuildingManager* buildingManager);


	void SetPlayerHP(class PlayerHP* _playerHP);

	bool GetIsAlive() const { return hp_ > 0; }

private:
	struct DamageParamater {
		bool isStop;
		float stopCollTime;
	    float kStopCollTime;
		float DamagePar;
	};
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

	/// grow size parameters

	bool  isPowerUp_;       /// パワーアップしているか
	float powerUpGaugeMax_;	/// パワーアップまでのゲージの最大
	float powerUpGauge_;	/// パワーアップまでのゲージの今
	float powerUpTimeMax_;	/// パワーアップ終了までの最大時間
	float powerUpTime_;		/// パワーアップ終了までのcurrent time

	float timeRate_;    /// ヒットストップのための time rate
	float hitStopTime_; /// ヒットストップする時間

	float moveSpeed_; /// 移動スピード

	/// camera param
	int cameraBehavior_;
	int preCameraBehavior_;

	//HP
	int hp_ = 5; /// int型のhp

	//ヒットバックパワー
	float hitBackPower_;
	Vec3 preInput_;

	DamageParamater damageForBossHead_;
	DamageParamater damageForBossBullet_;
	DamageParamater damageForBossBody_;

	/// パワーダウン
	float powerDownTime_;
	float powerDownInterval_;
	float powerDownPar_;

	//トルネード
	Tornado* pTornado_;

	class PlayerHP* pPlayerHP_ = nullptr;

};