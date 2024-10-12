#include"BossChasePlayer.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/boss/boss.h"

//初期化
BossChasePlayer::BossChasePlayer(Boss* boss)
	: BaseBossBehavior("ChasePlayer", boss) {

	pBoss_->ChaseInit();
	//パラメータ初期化
	chaseSpeedMax_ = 5.0f;
	chaseSpeedNormal_ = 0.01f;
}

BossChasePlayer::~BossChasePlayer() {

}

//更新
void BossChasePlayer::Update() {
	chaseSpeedNormal_ = pBoss_->GetChaseSpeedParamater();
	//フラグでスピード変更出来るようにする
	pBoss_->ChaseUpdate(chaseSpeedNormal_);

	


}

