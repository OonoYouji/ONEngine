#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Math/Vector3.h"

#include "Node.h"

/// <summary>
/// 前方宣言
/// </summary>
class WorkStrongAttackAction;
class WorkRushAttackAction;

enum class EnemyAttackRangeType : int32_t{
	SHORT_RANGE,
	MIDDLE_RANGE,
	LONG_RANGE,
	COUNT // size 用
};
static std::unordered_map<EnemyAttackRangeType,std::string> rangeTypes = {
	{EnemyAttackRangeType::SHORT_RANGE,"SHORT_RANGE"},
	{EnemyAttackRangeType::MIDDLE_RANGE,"MIDDLE_RANGE"},
	{EnemyAttackRangeType::LONG_RANGE,"LONG_RANGE"},
};

namespace EnemyBehaviorTree{
	///====================================
	/// アニメーション を 更新するための Node
	///====================================
	class TransitionAnimation
		:public Action{
	public:
		TransitionAnimation(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop);
		~TransitionAnimation(){}

		Status tick()override;
	private:
		std::string animation_;
		float animationTotalTime_;
		bool isLoop_;
	};
	class TransitionAnimationWithWeapon
		:public Action{
	public:
		TransitionAnimationWithWeapon(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop);
		~TransitionAnimationWithWeapon(){}

		Status tick()override;
	private:
		std::string animation_[2];
		float animationTotalTime_;
		bool isLoop_;
	};
	///====================================
	/// 移動系
	///====================================
#pragma region"Move"

#pragma endregion

	///====================================
	/// 待機系
	///====================================
#pragma region"Idle"

#pragma endregion

	///====================================
	/// 攻撃系
	///====================================
#pragma region"Attack"
	class StrongAttack
		:public EnemyBehaviorTree::Action{
	public:
		StrongAttack(Enemy* enemy,WorkStrongAttackAction* worker);
		~StrongAttack()override{}

		Status tick()override;
	private:
		Status InitRotate();
		Status StartupUpdate();
		Status Attack();
		Status EndLagUpdate();
	private:
		std::function<Status()> currentUpdate_;
		WorkStrongAttackAction* workInBehavior_;

		float beforeRotateY_;
		float rotateP2E_;
		float currentTime_;
	};

	class RushAttack
		:public Action{
	public:
		RushAttack(Enemy* enemy,WorkRushAttackAction* worker);
		~RushAttack()override{}

		Status tick()override;
	private:
		Status InitRotate();
		Status StartupUpdate();
		Status Attack();
		Status EndLagUpdate();
	private:
		std::function<Status()> currentUpdate_;
		WorkRushAttackAction* workInBehavior_;

		float beforeRotateY_;
		float rotateP2E_;
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