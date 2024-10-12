#include"BossChasePlayer.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/boss/boss.h"

//初期化
BossChasePlayer::BossChasePlayer(Boss* boss)
	: BaseBossBehavior("ChasePlayer", boss) {

	pBoss_->ChaseInit();
}

BossChasePlayer::~BossChasePlayer() {

}

//更新
void BossChasePlayer::Update() {

	pBoss_->ChaseUpdate();
}

