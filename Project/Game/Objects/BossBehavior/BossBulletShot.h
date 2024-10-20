#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"

class BossBulletShot : public BaseBossBehavior {
public:
	//コンストラクタ
	BossBulletShot(Boss* boss);
	~BossBulletShot();
	void Update()override;

private:
	//
	float easingTime_;
	float easingTimeMax_;
	float easeDirection_;
	float MinDistance_;
	//弾発射
	bool isAnticipationed_;
	float anticipationTime_;
	const float kAnticipationTime_=2.0f;//秒
	//スタン時間
	float stopTime_;
	const float kStopTime_ = 1.0f;
	bool isStop_;
	
	// プレイヤーの方向を向くための回転を計算
	Quaternion inter_;
	Quaternion move_;
	class BossBulletLump* pBossbulletLump_ = nullptr;
};
