#pragma once
#include "IEnemyBullet.h"

#include <functional>

class RangedAttackBullet :
	public IEnemyBullet{
public:
	RangedAttackBullet(float spawnRange,
					   float spawnPositionY,
					   float timeToLand,
					   float percentOfLockOnPlayer,
					   const Vector3& enemyPos,
					   const Vector3& playerPos);
	~RangedAttackBullet(){};

	void Initialize()override;
	void Update()override;

private:
	void Falling();
	void FadeOut();

	std::function<void()> currentUpdate_;

	float timeToLand_;
	float spawnPosY_;
	float currentTimeToLand_;
};
