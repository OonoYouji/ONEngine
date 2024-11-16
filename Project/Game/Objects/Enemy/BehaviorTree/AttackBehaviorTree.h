#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>

#include "Node.h"

struct WorkAttackAction;

enum class EnemyAttackRangeType : int16_t{
	SHORT_RANGE,
	MIDDLE_RANGE,
	LONG_RANGE
};
// EnemyAttackRangeTypeと文字列の対応付け
std::unordered_map<EnemyAttackRangeType,std::string> rangeTypes = {
	{EnemyAttackRangeType::SHORT_RANGE,"SHORT_RANGE"},
	{EnemyAttackRangeType::MIDDLE_RANGE,"MIDDLE_RANGE"},
	{EnemyAttackRangeType::LONG_RANGE,"LONG_RANGE"},
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
		EnemyAttackRangeType rangeType_;
	};
}