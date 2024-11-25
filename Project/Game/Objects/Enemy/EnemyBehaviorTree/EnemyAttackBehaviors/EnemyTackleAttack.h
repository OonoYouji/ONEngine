#pragma once

#include <functional>

#include "../Node.h"

#include "Math/Vector3.h"

class WorkTackleAttackAction;

namespace EnemyBehaviorTree{
	class TackleAttackStartup
		:public EnemyBehaviorTree::Action{
	public:
		TackleAttackStartup(Enemy* enemy,
							float startupTime,
							float lockOnTime);
		~TackleAttackStartup()override{}

		Status tick()override;
	private:
		float startupTime_;
		float currentTime_;
		float lockOnTime_;
	};

	class TackleAttackAction
		:public EnemyBehaviorTree::Action{
	public:
		TackleAttackAction(Enemy* enemy,
						   float activeTime,
						   float speed,
						   float damage);
		~TackleAttackAction()override{}

		Status tick()override;
	private:
		float activeTime_;
		float currentTime_;
		float speed_;
		float damage_;
	};

	class TackleAttackEndLag
		:public EnemyBehaviorTree::Action{
	public:
		TackleAttackEndLag(Enemy* enemy,float endLagTime);
		~TackleAttackEndLag()override{}

		Status tick()override;
	private:
		float leftTime_;
	};

	class TackleAttack
		:public EnemyBehaviorTree::Sequence{
	public:
		TackleAttack(Enemy* enemy,WorkTackleAttackAction* worker);
		~TackleAttack()override{}
	};
}