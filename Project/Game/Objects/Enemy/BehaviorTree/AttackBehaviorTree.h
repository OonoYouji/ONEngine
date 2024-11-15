#pragma once

#include <string>
#include <unordered_map>

#include "Node.h"

struct WorkAttackAction;

enum class EnemyAttackRangeType{
	SHORT_RANGEE,
	MIDDLE_RANGE,
	LONG_RANGE
};


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
		AttackCombo(Enemy* enemy,const std::string& comboName);
	private:
	};
}