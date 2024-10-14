#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>

//std
#include<memory>
//behavior
#include"Objects/BossBehavior/BaseBossBehavior.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/BossBehavior/BossChasePlayer.h"

class Player;
class BuildingManager;
class BaseBuilding;
class Boss : public BaseGameObject {
public:

	Boss() { CreateTag(this); }
	~Boss() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	//通常更新
	void RootInit();
	void RootUpdate();
	//ストーカー
	void ChaseInit();
	void ChaseUpdate();
	//建物吸引
	void SlurpInit();
	void SlurpUpdate();
	
	std::pair<float, float> CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition, const float& radius);
	//プレイヤーセット
	void SetPlayer(Player*player);
	void SetBuildingaManager(BuildingManager* player);
	BaseBuilding* FindClosestBuilding();
	//状態変更
	void ChangeState(std::unique_ptr<BaseBossBehavior>behavior);
	//getter
	bool GetIsSlurping()const { return isSlurping_; }
	float GetChaseSpeedParamater()const {return SpeedParamater_; }
	Quaternion GetPivotQuaternion()const { return pivot_.quaternion; }
	Player* GetPlayer() { return pPlayer_; }

	//setter
	void SetIsSlurping(bool is) { isSlurping_ = is; }
	void SetSlurpingCoolTimer() { slurpCooldownTimer_ = kSlurpCollTime_; }
	void SetPivotQuaternion(Quaternion pivot) { pivot_.quaternion = pivot; }
	void SetPivotSubtraction(Quaternion pivot) { pivot_.quaternion *= pivot; }
	
private:
	
private:
	//プレイヤーポインタ
	Player* pPlayer_=nullptr;
	BuildingManager* pBuildingManager_ = nullptr;
	////状態
	std::unique_ptr<BaseBossBehavior>behavior_=nullptr;
	//ピボット
	Transform pivot_;
	//パラメータ調節用追従すピーⅮ
	float SpeedParamater_;
	//吸い込み関連
	bool isSlurping_ = false;  // 吸い込み中かどうか
	float slurpCooldownTimer_ = 0.0f;  // クールダウン用のタイマー
	const float kSlurpCollTime_= 1.0f;  // 吸い込み完了後のクールダウン時間（秒）
};