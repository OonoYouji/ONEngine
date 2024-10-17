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
	attackWaitTime_ = 0.0f;
	isSwingDown_ = false;
}

BossAttack ::~BossAttack() {

}

//更新
void BossAttack::Update() {
	if (!isSwingDown_) {
		attackWaitTime_ += Time::DeltaTime();

		std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);

		// 現在の回転をオイラー角に変換
		Vec3 euler = QuaternionToEulerAngles(pBoss_->GetPivotQuaternion());

		// プレイヤーの方向を向くための回転を計算
		 inter_ = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

		// 回転を更新
		pBoss_->SetPivotQuaternion(inter_);
		if (attackWaitTime_ >= kAttackWaitTime_) {
			isSwingDown_ = true;
		}

	}//振り下ろし
	else if (isSwingDown_) {
		pBoss_->SetPivotQuaternion(inter_);
		pBoss_->AttackUpdate();
	}
}
