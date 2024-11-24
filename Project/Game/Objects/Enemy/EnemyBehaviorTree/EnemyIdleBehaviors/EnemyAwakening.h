#pragma once

#include "../Node.h"

namespace EnemyBehaviorTree{
	class EnemyAwakeningAction
		:public Action{
	public:
		EnemyAwakeningAction(Enemy* enemy);
		~EnemyAwakeningAction(){}

		Status tick()override;
	private:
		float leftTime_;
	};

	class EnemyAwakening
		:public Sequence{
	public:
		EnemyAwakening(Enemy* enemy);
		~EnemyAwakening()override{}
	};

}