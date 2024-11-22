#pragma once

#include "../Node.h"

class WorkIdleAction;

namespace EnemyBehaviorTree{
	class IdleAction
		:public Action{
	public:
		IdleAction(Enemy* enemy,WorkIdleAction* worker);
		~IdleAction()override{}

		Status tick()override;
	private:
		WorkIdleAction* workInBehavior_;
		float currentTime_;
	};
	class Idle
		:public Sequence{
	public:
		Idle(Enemy* enemy,WorkIdleAction* worker);
		~Idle()override{}
	};
}
