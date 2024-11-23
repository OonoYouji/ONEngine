#pragma once

#include "../Node.h"

#include "Math/Vector3.h"

#include <functional>

#include "../Node.h"

#include "Math/Vector3.h"

class WorkWeakAttack2Action;
namespace EnemyBehaviorTree{

	class WeakAttack2Startup
		:public EnemyBehaviorTree::Action{
	public:
		WeakAttack2Startup(Enemy* enemy,float startupTime,float maxRotateY);
		~WeakAttack2Startup()override{}

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

	class WeakAttack2Action
		:public EnemyBehaviorTree::Action{
	public:
		WeakAttack2Action(Enemy* enemy,float activeTime,float collisionStartTime,float collisionTime,float collisionRadius,float damage);
		~WeakAttack2Action()override{}

		Status tick()override;
	private:
		float activeTime_;
		float currentTime_;
		float collisionStartTime_;
		float collisionTime_;
		float collisionRadius_;

		float damage_;
	};

	class WeakAttack2EndLag
		:public EnemyBehaviorTree::Action{
	public:
		WeakAttack2EndLag(Enemy* enemy,float endLagTime);
		~WeakAttack2EndLag()override{}

		Status tick()override;
	private:
		float currentTime_;
	};

	class WeakAttack2
		:public EnemyBehaviorTree::Sequence{
	public:
		WeakAttack2(Enemy* enemy,WorkWeakAttack2Action* worker);
		~WeakAttack2()override{}
	};
}