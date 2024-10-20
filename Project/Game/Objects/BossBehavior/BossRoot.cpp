#include"BossRoot.h"
#include"Objects/BossBehavior/BossChasePlayer.h"
#include"Objects/boss/boss.h"

//初期化
BossRoot::BossRoot(Boss* boss)
	: BaseBossBehavior("BossRoot", boss) {

	pBoss_->RootInit();
}

BossRoot::~BossRoot() {

}

//更新
void BossRoot::Update() {
	pBoss_->RootUpdate();

}

