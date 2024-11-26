#pragma once

#include <functional>

#include "../Node.h"

#include "Math/Vector3.h"

class WorkStrongAttackAction;
namespace EnemyBehaviorTree{

	class StrongAttackStartup
		:public EnemyBehaviorTree::Action{
	public:
		StrongAttackStartup(Enemy* enemy,float startupTime,float maxRotateY);
		~StrongAttackStartup()override{}

		Status tick()override;
	private:
		std::function<Status()> currentUpdate_;
		Status InitRotate();
		Status Update();
	private:
		float currentTime_;
		float startupTime_;

		float beforeRotateY_;
		float rotateP2E_;
		float maxRotateY_;
	};

	class StrongAttackAction
		:public EnemyBehaviorTree::Action{
	public:
		StrongAttackAction(Enemy* enemy,
						   float activeTime,
						   float collisionStartTime,
						   float collisionTime,
						   float damage);
		~StrongAttackAction()override{}

		Status tick()override;
	private:
		float activeTime_;
		float currentTime_;
		float collisionStartTime_;
		float collisionTime_;

		float damage_;
	};

	class StrongAttackEndLag
		:public EnemyBehaviorTree::Action{
	public:
		StrongAttackEndLag(Enemy* enemy,float endLagTime);
		~StrongAttackEndLag()override{}

		Status tick()override;
	private:
		float currentTime_;
		float endLagTime_;
	};

	class StrongAttack
		:public EnemyBehaviorTree::Sequence{
	public:
		StrongAttack(Enemy* enemy,WorkStrongAttackAction* worker);
		~StrongAttack()override{}
	};
}