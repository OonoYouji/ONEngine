#include"BossSlurp.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/boss/boss.h"

//初期化
BossSlurp::BossSlurp(Boss* boss)
	: BaseBossBehavior("BossSlurp", boss) {
	pBoss_->SlurpInit();
	
}

BossSlurp ::~BossSlurp() {

}

//更新
void BossSlurp::Update() {
	
	pBoss_->SlurpUpdate();
}

