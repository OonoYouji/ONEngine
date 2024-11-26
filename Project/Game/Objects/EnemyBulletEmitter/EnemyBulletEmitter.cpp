#include "EnemyBulletEmitter.h"

#include "../EnemyBullet/RangedAttackBullet.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "Game/Objects/Enemy/Enemy.h"
#include "Game/Objects/EnemyBullet/LongRangeBullet.h"
#include "Game/Objects/EnemyBullet/RangedAttackBullet.h"
#include "Game/Objects/Player/Player.h"

#include "FrameManager/Time.h"

EnemyBulletEmitter::EnemyBulletEmitter(Player* player,
									   Enemy* enemy,
									   float emitterActiveTime,
									   WorkEnemyAction* worker)
	:BaseGameObject(){
	CreateTag(this);

	player_ = player;
	enemy_ 	= enemy;

	if(worker->type_ == ActionTypes::RANGED_ATTACK){
		spawnUpdate_ = [this](){SpawnRangedBullet(); };
	} else if(worker->type_ == ActionTypes::LONGRANGE_ATTACK){
		spawnUpdate_ = [this](){SpawnLongRangeBullet(); };
	} else{
		assert(0); // 玉を使わないクラスで 生成されたら エラー
	}

	activeTime_ 	= emitterActiveTime;
	leftActiveTime_ = activeTime_;

	worker_ = worker;

	meshInstancingRenderer_ = AddComponent<MeshInstancingRenderer>(50);
	meshInstancingRenderer_->SetModel("Wrasse");
	drawBullets_.reserve(50);
	drawTransform_.reserve(50);
}

EnemyBulletEmitter::~EnemyBulletEmitter(){}

void EnemyBulletEmitter::Initialize(){}

void EnemyBulletEmitter::Update(){
	leftActiveTime_ -= Time::DeltaTime();
	leftCoolTime_ -= Time::DeltaTime();

	// spawn
	spawnUpdate_();

	std::erase_if(drawBullets_,[](IEnemyBullet* bullet){
		if(bullet->GetLifeLeftTime() > 0.0f){
			return false;
		}
		bullet->Destory();
		return true;
				  });

	meshInstancingRenderer_->ResetTransformArray();
	drawTransform_.clear();
	for(auto bullet : drawBullets_){
		drawTransform_.push_back(bullet->GetTransform());
	}

	meshInstancingRenderer_->SetTransformArray(drawTransform_);

	if(leftActiveTime_ <= 0.0f && drawBullets_.empty()){
		drawBullets_.clear();
		drawTransform_.clear();
		Destory();
	}
}

void EnemyBulletEmitter::SpawnRangedBullet(){

	if(leftCoolTime_ <= 0.0f && leftActiveTime_ > 0.0f){
		WorkRangedAttackAction* rangedWorker = reinterpret_cast<WorkRangedAttackAction*>(worker_);
		// Spawn
		RangedAttackBullet* newBullet = new RangedAttackBullet(rangedWorker->spawnRange_,
															   rangedWorker->spawnPositionY_,
															   rangedWorker->timeToLand_,
															   rangedWorker->attackForPlayerProbability_,
															   enemy_->GetPosition(),
															   player_->GetPosition());
		newBullet->Initialize();
		newBullet->SetDamage(rangedWorker->damage_);
		newBullet->SetLifeTime(rangedWorker->lifeTime_);
		newBullet->SetScale({rangedWorker->bulletScale_,
							rangedWorker->bulletScale_,
							rangedWorker->bulletScale_});

		drawBullets_.push_back(newBullet);

		leftCoolTime_ = rangedWorker->spawnCoolTime_;
	}
}

void EnemyBulletEmitter::SpawnLongRangeBullet(){
	if(leftCoolTime_ <= 0.0f && leftActiveTime_ > 0.0f){
		WorkLongRangeAttackAction* longWorker = reinterpret_cast<WorkLongRangeAttackAction*>(worker_);
		// SpawnBullet
		LongRangeBullet* newBullet = new LongRangeBullet();
		newBullet->Initialize();
		Matrix4x4 rotateY = Matrix4x4::MakeRotateY(enemy_->GetRotate().y);
		Vector3 enemyDir = Matrix4x4::Transform(
			{0.0f,3.5f,1.0f},
			rotateY
		);
		newBullet->SetDamage(longWorker->damage_);
		newBullet->SetLifeTime(10.0f);
		newBullet->SetScale({longWorker->bulletScale_,
							longWorker->bulletScale_,
							longWorker->bulletScale_});

		newBullet->SetPosition(enemy_->GetPosition() + (enemyDir * longWorker->bulletSpawnOffsetZ_));
		newBullet->SetRotateY(atan2(enemyDir.x,enemyDir.z));
		newBullet->SetVelocityXZ(Vector2(enemyDir.x,enemyDir.z).Normalize() * longWorker->bulletSpeed_);

		newBullet->UpdateMatrix();

		drawBullets_.push_back(newBullet);

		leftCoolTime_ = longWorker->bulletFireInterval_;
	}
}
