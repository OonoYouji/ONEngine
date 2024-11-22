#pragma once

#include "../Node.h"

#include "Math/Vector3.h"

class WorkWeakAttackAction;

namespace EnemyBehaviorTree{
	class WeakAttackStartup
		:public EnemyBehaviorTree::Action{
	public:
		WeakAttackStartup(Enemy* enemy,float startupTime,float collisionRadius);
		~WeakAttackStartup()override{}

		Status tick()override;
	private:
		float leftTime_;

		float collisionRadius_;
	};

	class WeakAttackAction
		:public EnemyBehaviorTree::Action{
	public:
		WeakAttackAction(Enemy* enemy,float activeTime,float damage);
		~WeakAttackAction()override{}

		Status tick()override;
	private:
		float leftTime_;
		float damage_;
	};

	class WeakAttackEndLag
		:public EnemyBehaviorTree::Action{
	public:
		WeakAttackEndLag(Enemy* enemy,float endLagTime);
		~WeakAttackEndLag()override{}

		Status tick()override;
	private:
		float leftTime_;
	};

	class WeakAttack
		:public EnemyBehaviorTree::Sequence{
	public:
		WeakAttack(Enemy* enemy,WorkWeakAttackAction* worker);
		~WeakAttack()override{}
	};
}