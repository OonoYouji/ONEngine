#pragma once

#include "Game/Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "GameObjectManager/BaseGameObject.h"

class Enemy;
class Player;

/// <summary>
/// Bullet を random で 生成する必要がある時に使う
/// </summary>
class EnemyBulletEmitter
	: public BaseGameObject{
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyBulletEmitter(Player* player,Enemy* enemy,float emitterActiveTime,WorkRangedAttackAction* workRangedAttackAction_);
	~EnemyBulletEmitter();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================
	WorkRangedAttackAction worker_;

	Player* player_;
	Enemy* enemy_;

	float activeTime_;
	float leftActiveTime_;

	float leftCoolTime_;
};
