#pragma once

#include "../Node.h"

#include "Math/Vector3.h"

class WorkChaseAction;

namespace EnemyBehaviorTree{
	class ChaseAction
		:public Action{
	public:
		ChaseAction(Enemy* enemy,WorkChaseAction* worker);
		~ChaseAction()override{}

		Status tick()override;
	private:
		WorkChaseAction* workInBehavior_;
		Vector3 velocity_;
		float currentTime_;
	};
	class Chase
		:public Sequence{
	public:
		Chase(Enemy* enemy,WorkChaseAction* worker);
		~Chase()override{}
	};
}