#include "EnemyBulletEmitter.h"

#include "../EnemyBullet/RangedAttackBullet.h"
#include "Game/Objects/Enemy/Enemy.h"
#include "Game/Objects/Player/Player.h"

#include "FrameManager/Time.h"

EnemyBulletEmitter::EnemyBulletEmitter(Player* player,
									   Enemy* enemy,
									   float emitterActiveTime,
									   WorkRangedAttackAction* workRangedAttackAction_)
	:BaseGameObject(){
	CreateTag(this);

	player_ = player;
	enemy_ 	= enemy;

	activeTime_ 	= emitterActiveTime;
	leftActiveTime_ = activeTime_;
	leftCoolTime_ = worker_.spawnCoolTime_;

	worker_ = *workRangedAttackAction_;
}

EnemyBulletEmitter::~EnemyBulletEmitter(){}

void EnemyBulletEmitter::Initialize(){}

void EnemyBulletEmitter::Update(){
	leftActiveTime_ -= Time::DeltaTime();
	leftCoolTime_ -= Time::DeltaTime();

	if(leftCoolTime_ <= 0.0f){
		// Spawn
		RangedAttackBullet* newBullet = new RangedAttackBullet(worker_.spawnRange_,
															   worker_.spawnPositionY_,
															   worker_.timeToLand_,
															   worker_.attackForPlayerProbability_,
															   enemy_->GetPosition(),
															   player_->GetPosition());
		newBullet->Initialize();
		newBullet->SetDamage(worker_.damage_);
		newBullet->SetLifeTime(worker_.lifeTime_);
		newBullet->SetScale({worker_.bulletScale_,worker_.bulletScale_,worker_.bulletScale_});

		leftCoolTime_ = worker_.spawnCoolTime_;
	}
	if(leftActiveTime_ <= 0.0f){
		Destory();
	}
}