#include"BossSlurp.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/boss/boss.h"
#include"FrameManager/Time.h"

#include"Easing/EasingFunction.h"
//初期化
BossSlurp::BossSlurp(Boss* boss)
	: BaseBossBehavior("BossSlurp", boss) {
	pBoss_->SlurpInit();
	easingTimeMax_ = 0.4f;
}

BossSlurp ::~BossSlurp() {

}

//更新
void BossSlurp::Update() {
	
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
	pBoss_->SetScale(EaseInCubic<Vec3>(Vec3(1.0f,1.0f,1.0f), Vec3(1.4f, 1.4f, 1.4f),easingTime_,easingTimeMax_));


}

