#pragma once

#include <functional>
#include <stdint.h>
#include <string>
#include <unordered_map>

#include "Node.h"

/// <summary>
/// 前方宣言
/// </summary>
class WorkIdleAction;
class WorkWeakAttackAction;
class WorkStrongAttackAction;

enum class EnemyAttackRangeType : int32_t{
	SHORT_RANGE,
	MIDDLE_RANGE,
	LONG_RANGE
};

namespace EnemyBehaviorTree{
	///====================================
	/// アニメーション を 更新するための Node
	///====================================
	class TransitionAnimation
		:public Action{
	public:
		TransitionAnimation(Enemy* enemy,const std::string& animation);
		~TransitionAnimation(){}

		Status tick()override;
	private:
		std::string animation_;
	};

	///====================================
	/// 待機系
	///====================================
#pragma region"Idle"
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
#pragma endregion

	///====================================
	/// 攻撃系
	///====================================
#pragma region"Attack"
	class WeakAttack
		:public EnemyBehaviorTree::Action{
	public:
		WeakAttack(Enemy* enemy,WorkWeakAttackAction* worker);
		~WeakAttack()override{}

		Status tick()override;
	private:
		Status StartupUpdate();
		Status Attack();
		Status EndLagUpdate();
	private:
		std::function<Status()> currentUpdate_;
		WorkWeakAttackAction* workInBehavior_;
		float currentTime_;
	};

	class StrongAttack
		:public EnemyBehaviorTree::Action{
	public:
		StrongAttack(Enemy* enemy,WorkStrongAttackAction* worker);
		~StrongAttack()override{}

		Status tick()override;
	private:
		Status StartupUpdate();
		Status Attack();
		Status EndLagUpdate();
	private:
		std::function<Status()> currentUpdate_;
		WorkStrongAttackAction* workInBehavior_;
		float currentTime_;
	};

#pragma endregion

	///====================================
	/// 連続攻撃 (単発を含む)
	///====================================
#pragma region"Combo"

	class AttackCombo
		:public EnemyBehaviorTree::Sequence{
	public:
		AttackCombo(Enemy* enemy,const std::string& comboName);
	private:
		EnemyAttackRangeType rangeType_;
	};
#pragma endregion
}