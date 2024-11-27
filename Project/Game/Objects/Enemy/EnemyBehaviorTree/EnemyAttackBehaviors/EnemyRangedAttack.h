#pragma once

#include "../Node.h"

#include "Math/Vector3.h"

class WorkRangedAttackAction;

namespace EnemyBehaviorTree{

	class RangedAttackStartup
		:public EnemyBehaviorTree::Action{
	public:
		RangedAttackStartup(Enemy* enemy,
							WorkRangedAttackAction* worker);
		~RangedAttackStartup()override{}

		Status tick()override;
	private:
		float startupTime_;
		float currentTime_;

		WorkRangedAttackAction* worker_;
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
		float endLagTime_;
		float leftTime_;
	};

	class RangedAttack
		:public EnemyBehaviorTree::Sequence{
	public:
		RangedAttack(Enemy* enemy,WorkRangedAttackAction* worker);
		~RangedAttack()override;
	};
}