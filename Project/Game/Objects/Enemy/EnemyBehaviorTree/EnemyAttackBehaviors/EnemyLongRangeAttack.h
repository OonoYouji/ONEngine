#pragma once

#include "../Node.h"

#include "Math/Vector3.h"

class WorkLongRangeAttackAction;

namespace EnemyBehaviorTree{

	class LongRangeAttackStartup
		:public EnemyBehaviorTree::Action{
	public:
		LongRangeAttackStartup(Enemy* enemy,
							   WorkLongRangeAttackAction* worker,
							   float startupTime,
							   float maxRotateYSpeed);
		~LongRangeAttackStartup()override{}

		Status tick()override;
	private:
		WorkLongRangeAttackAction* worker_;
		float leftTime_;

		float maxRotateYSpeed_;
	};

	class LongRangeAttackAction
		:public EnemyBehaviorTree::Action{
	public:
		LongRangeAttackAction(Enemy* enemy,WorkLongRangeAttackAction* worker);
		~LongRangeAttackAction()override{}

		Status tick()override;
	private:

		float leftTime_;
		float leftBulletSpawnTime_;

		WorkLongRangeAttackAction* worker_;
	};

	class LongRangeAttackEndLag
		:public EnemyBehaviorTree::Action{
	public:
		LongRangeAttackEndLag(Enemy* enemy,float endLagTime);
		~LongRangeAttackEndLag()override{}

		Status tick()override;
	private:
		float endLagTime_;
		float leftTime_;
	};

	class LongRangeAttack
		:public EnemyBehaviorTree::Sequence{
	public:
		LongRangeAttack(Enemy* enemy,WorkLongRangeAttackAction* worker);
		~LongRangeAttack()override{}
	};
}