#include"BossSlurp.h"
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
BossSlurp::BossSlurp(Boss* boss)
	: BaseBossBehavior("BossSlurp", boss) {
	pBoss_->SlurpInit();
	//値初期化
	easingTimeMax_ = 0.4f;
	MinDistance_ = 10.0f;
}

BossSlurp ::~BossSlurp() {

}

//更新
void BossSlurp::Update() {

	//吸い込み処理更新
	pBoss_->SlurpUpdate();

	// イージングタイムを更新
	easingTime_ += Time::DeltaTime() * easeDirection_; // 方向に応じて時間を増減

	// タイムが1を超えたら逆方向に、0未満になったら進む方向に変更
	if (easingTime_ >= easingTimeMax_) {
		easingTime_ = easingTimeMax_;
		easeDirection_ = -1.0f; // 逆方向に切り替え
	}
	else if (easingTime_ <= 0.0f) {
		easingTime_ = 0.0f;
		easeDirection_ = 1.0f; // 進む方向に切り替え
	}
	//ボスのスケール切り替え
	pBoss_->SetScale(EaseInCubic<Vec3>(Vec3(2.0f,2.0f,2.0f), Vec3(2.4f, 2.4f, 2.4f),easingTime_,easingTimeMax_));

	//一定距離で追従に戻す
	std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);
	if (distanceAndDirection.first <= MinDistance_) {
		if (!pBoss_->GetIsSlurping()) {
			pBoss_->ChangeState(std::make_unique<BossChasePlayer>(pBoss_));
		}
	}
}
