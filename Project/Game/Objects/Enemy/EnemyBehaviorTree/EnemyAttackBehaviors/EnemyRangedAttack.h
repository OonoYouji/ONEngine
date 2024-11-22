#pragma once

#include "../Node.h"

#include "Math/Vector3.h"

class WorkRangedAttackAction;

namespace EnemyBehaviorTree{

	class RangedAttackStartup
		:public EnemyBehaviorTree::Action{
	public:
		RangedAttackStartup(Enemy* enemy,
							float startupTime,
							float downSpeed,
							float bulletDamage,
							float bulletSpawnValue_,
							float spawnRange,
							float spawnPositionY,
							float lifeTime,
							float attackForPlayerProbability);
		~RangedAttackStartup()override{}

		Status tick()override;
	private:
		float leftTime_;

		float bulletDownSpeed_;
		float bulletDamage_;
		float bulletSpawnRange_;
		float bulletSpawnValue_;
		float bulletSpawnPositionY_;
		float bulletLifeTime_;
		float bulletAttackForPlayerProbability_;
	};

	class RangedAttackAction
		:public EnemyBehaviorTree::Action{
	public:
		RangedAttackAction(Enemy* enemy,float activeTime);
		~RangedAttackAction()override{}

		Status tick()override;
	private:
		float leftTime_;
	};

	class RangedAttackEndLag
		:public EnemyBehaviorTree::Action{
	public:
		RangedAttackEndLag(Enemy* enemy,float endLagTime);
		~RangedAttackEndLag()override{}

		Status tick()override;
	private:
		float leftTime_;
	};

	class RangedAttack
		:public EnemyBehaviorTree::Sequence{
	public:
		RangedAttack(Enemy* enemy,WorkRangedAttackAction* worker);
		~RangedAttack()override{}
	};
}