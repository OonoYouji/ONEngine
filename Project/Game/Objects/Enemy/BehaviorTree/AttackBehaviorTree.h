#pragma once

#include <string>
#include <unordered_map>

#include "Node.h"

struct WorkAttackAction;

namespace EnemyBehaviorTree{
	class AttackAction
		:public Action{
	public:
		AttackAction(Enemy* enemy,const std::string& attackName);
		~AttackAction();
		Status tick()override;
	private:
		const WorkAttackAction& workInBehavior_;
		float currentTime_;

		std::function<Status()> currentUpdate_;
		Status StartupUpdate();
		Status Attack();
		Status EndLagUpdate();

	};

	class AttackCombo
		:public Sequence{
	public:
		AttackCombo(Enemy* enemy,const std::string& );
	};
}