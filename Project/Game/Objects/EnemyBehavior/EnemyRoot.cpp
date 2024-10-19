#include"EnemyRoot.h"
//behavior
#include"objects/EnemyBehavior/EnemyChasePlayer.h"
//obj
#include"Objects/Enemy/Enemy.h"
#include"Objects/Ground/Ground.h"
#include"Objects/boss/boss.h"
#include"Objects/Player/Player.h"
//std
#include<numbers>
//function
#include"HormingFunction/Horming.h"
#include"FrameManager/Time.h"
//初期化

EnemyRoot::EnemyRoot(Enemy* boss)
	: BaseEnemyBehavior("Root", boss) {
	
}

EnemyRoot::~EnemyRoot() {

}

//更新
void EnemyRoot::Update() {

}


