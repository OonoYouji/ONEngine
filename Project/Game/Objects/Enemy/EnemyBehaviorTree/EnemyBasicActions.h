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
class WorkTackleAttackAction;

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

	class TransitionAnimationWithWeaponAndSub
		:public Action{
	public:
		TransitionAnimationWithWeaponAndSub(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop);
		~TransitionAnimationWithWeaponAndSub(){}

		Status tick()override;
	private:
		std::string animation_[3];
		float animationTotalTime_;
		bool isLoop_;
	};

	class TransitionEffectAnimation
		:public Action{
	public:
		TransitionEffectAnimation(Enemy* enemy,
								  const std::string& animation,
								  float animationTotalTime,
								  Vector3 effectPos = {0.0f,0.0f,0.0f},
								  bool isLoop = false);
		~TransitionEffectAnimation(){}

		Status tick()override;
	private:
		std::string animation_;
		Vector3 effectPos_;
		float animationTotalTime_;
		bool isLoop_;
	};

	class TransitionEffectAnimationWithSub
		:public Action{
	public:
		TransitionEffectAnimationWithSub(Enemy* enemy,
										 const std::string& animation,
										 const std::string& subAnimation,
										 float animationTotalTime,
										 Vector3 effectPos = {0.0f,0.0f,0.0f},
										 Vector3 effect2Pos = {0.0f,0.0f,0.0f},
										 bool isLoop = false);
		~TransitionEffectAnimationWithSub(){}

		Status tick()override;
	private:
		std::string animation_[2];
		Vector3 effectPos_ = {0.0f,0.0f,0.0f};
		Vector3 effect2Pos_ = {0.0f,0.0f,0.0f};
		float animationTotalTime_;
		bool isLoop_;
	};

	class PlaySe
		:public Action{
	public:
		PlaySe(Enemy* enemy, const std::string& seName);
		~PlaySe(){}

		Status tick();
	private:
		std::string seName_;
	};

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