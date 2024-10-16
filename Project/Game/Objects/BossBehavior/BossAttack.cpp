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
	//pBoss_->BossAttack();
	//値初期化
	easingTimeMax_ = 0.4f;
	MinDistance_ = 10.0f;
}

BossAttack ::~BossAttack() {

}

//更新
void BossAttack::Update() {
	
}
