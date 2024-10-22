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
	attackFixationTime_ = 0.0f;
	isSwingDown_ = false;
	isFixation_ = false;
}

BossAttack ::~BossAttack() {

}

//更新
void BossAttack::Update() {

	if (!isFixation_) {

		attackWaitTime_ += Time::DeltaTime();

		std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);

		// 現在の回転をオイラー角に変換
		Vec3 euler = QuaternionToEulerAngles(pBoss_->GetPivotQuaternion());

		// プレイヤーの方向を向くための回転を計算
		Quaternion targetRotation = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

		// 現在の回転
		Quaternion currentRotation = pBoss_->GetPivotQuaternion();

		// 回転をスムーズに補間 (Slerpを使用)
		float rotationSpeed = 5.0f; // 回転速度、必要に応じて調整
		inter_ = Slerp(currentRotation, targetRotation, rotationSpeed * Time::DeltaTime());

		// 回転を更新
		pBoss_->SetPivotQuaternion(inter_);
		if (attackWaitTime_ >= kAttackWaitTime_) {
			isFixation_ = true;
		}

	}/// 振り下ろし位置固定
	else if (isFixation_) {
		pBoss_->AttackFixationUpdate();
		attackFixationTime_ += Time::DeltaTime();
		if (attackFixationTime_ >= kAttackFixationTime_) {
			isSwingDown_ = true;
		}
	}/// 振り下ろし
	if (isSwingDown_) {
		pBoss_->AttackUpdate();
	}
}


