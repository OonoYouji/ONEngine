#pragma once

#include <functional>

#include "Game/Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Game/Objects/EnemyBullet/IEnemyBullet.h"
#include "GameObjectManager/BaseGameObject.h"
class Enemy;
class Player;

class MeshInstancingRenderer;
/// <summary>
/// Bullet を random で 生成する必要がある時に使う
/// </summary>
class EnemyBulletEmitter
	: public BaseGameObject{
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyBulletEmitter(Player* player,
					   Enemy* enemy,
					   float emitterActiveTime,
					   WorkEnemyAction* workRangedAttackAction_);
	~EnemyBulletEmitter();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================
	WorkEnemyAction* worker_;

	MeshInstancingRenderer* meshInstancingRenderer_;

	std::vector<IEnemyBullet*> drawBullets_;
	std::vector<Transform*> drawTransform_;

	Player* player_;
	Enemy* enemy_;

	float activeTime_;
	float leftActiveTime_;

	float leftCoolTime_;

	int32_t switchSound_;

	std::function<void()>spawnUpdate_;
	void SpawnRangedBullet();
	void SpawnLongRangeBullet();
};
