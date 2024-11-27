#include "RangedAttackBullet.h"

#include <algorithm>

#include "FrameManager/Time.h"
#include "Math/Easing.h"
#include "Math/Random.h"

RangedAttackBullet::RangedAttackBullet(float spawnRange,
									   float spawnPositionY,
									   float timeToLand,
									   float percentOfLockOnPlayer,
									   const Vector3& enemyPos,
									   const Vector3& playerPos)
	:IEnemyBullet(BulletType::RANGED_ATTACK_BULLET){
	CreateTag(this);

	{// Times Initialize
		timeToLand_ = timeToLand;
		currentTimeToLand_ = 0.0f;
	}

	{// Position Initialize
		float luck = Random::Float(0.0f,100.0f);
		if(luck - percentOfLockOnPlayer < 0.0f){
			pTransform_->position = {playerPos.x,spawnPositionY,playerPos.z};
		} else{
			Vector2 positionXZ = (Random::Vec2({-1.0f,-1.0f},{1.0f,1.0f})).Normalize() * spawnRange;
			pTransform_->position = {enemyPos.x + positionXZ.x,spawnPositionY,enemyPos.z + positionXZ.y};
		}

		spawnPosY_ = spawnPositionY;
	}

}

void RangedAttackBullet::Initialize(){
	IEnemyBullet::Initialize();
}

void RangedAttackBullet::Update(){
	currentTimeToLand_+= Time::DeltaTime();
	float t = currentTimeToLand_ / timeToLand_;
	pTransform_->position.y = (std::lerp)(spawnPosY_,-0.5f,Ease::In::Back(t));

	if(t >= 1.0f){
		isActive = false;
	}
}