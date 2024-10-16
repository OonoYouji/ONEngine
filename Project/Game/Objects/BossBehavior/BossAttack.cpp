#include"BossAttack.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/boss/boss.h"
#include"FrameManager/Time.h"

//obj
#include"Objects/Ground/Ground.h"
#include"Objects/Player/Player.h"

#include"Easing/EasingFunction.h"
//function
#include"HormingFunction/Horming.h"
//初期化
BossAttack::BossAttack(Boss* boss)
	: BaseBossBehavior("BossAttack", boss) {
	pBoss_->AttackInit();
	//値初期化
	pBoss_->SetIsAttack(true);
	easingTimeMax_ = 0.4f;
	MinDistance_ = 10.0f;
}

BossAttack ::~BossAttack() {

}

//更新
void BossAttack::Update() {
	
	std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);
	
	// 現在の回転をオイラー角に変換
	Vec3 euler = QuaternionToEulerAngles(pBoss_->GetPivotQuaternion());

	// プレイヤーの方向を向くための回転を計算
	Quaternion inter = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

// 回転を更新
	pBoss_->SetPivotQuaternion(inter);
	

}
