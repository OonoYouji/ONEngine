#pragma once

#include <string>
#include <unordered_map>

#include "CustomMath/MotionTimes.h"

/// <summary>
/// 行動の一覧
/// </summary>
enum class ActionTypes : int32_t{
	IDLE,
	WEAK_ATTACK,
	STRONG_ATTACK,
	RUSH_ATTACK,
	CHASE,
	COUNT // 要素数を把握するための要素
};

static std::unordered_map<ActionTypes,std::string> actionTypeWord = {
	{ActionTypes::IDLE,"IDLE"},
	{ActionTypes::WEAK_ATTACK,"WEAK_ATTACK"},
	{ActionTypes::STRONG_ATTACK,"STRONG_ATTACK"},
	{ActionTypes::RUSH_ATTACK,"RUSH_ATTACK"},
	{ActionTypes::CHASE,"CHASE"},
};

/// <summary>
/// 継承のために class にしているが 基本扱いは struct と同じ
/// </summary>
class WorkEnemyAction{
public:
	WorkEnemyAction(ActionTypes type):type_(type),motionTimes_({0.0f,0.0f,0.0f}){}

	virtual ~WorkEnemyAction(){}

	virtual void Debug();

	virtual void Save(const std::string& name);
	virtual void Load(const std::string& name);
public:
	ActionTypes type_;
	std::string animationName_;
	MotionTimes motionTimes_;
public:
	static std::list<std::pair<std::string,std::string>> animationList;
};

#pragma region"MoveWorker"
class WorkChaseAction
	:public WorkEnemyAction{
public:
	WorkChaseAction():WorkEnemyAction(ActionTypes::IDLE){
		speed_ = 0.0f;
		distanceToStopChasing_ = 0.0f;
	}
	~WorkChaseAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float speed_;
	/// <summary>
	/// どのくらい近づいたら 追いかけるのをやめるか
	/// </summary>
	float distanceToStopChasing_;
};
#pragma endregion

//待機系統の アクションで使われる変数群
#pragma region"IdleWorker"

class WorkIdleAction
	:public WorkEnemyAction{
public:
	WorkIdleAction():WorkEnemyAction(ActionTypes::IDLE){}
	~WorkIdleAction(){}

	void Debug()override;
};

#pragma endregion

//攻撃系統の アクションで使われる変数群
#pragma region"AttackWorker"

class WorkWeakAttackAction
	:public WorkEnemyAction{
public:
	WorkWeakAttackAction():WorkEnemyAction(ActionTypes::WEAK_ATTACK),collisionRadius_(0.0f),damage_(0.0f){}
	~WorkWeakAttackAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float collisionRadius_;
	//与えるダメージ
	float damage_;
};

class WorkStrongAttackAction
	:public WorkEnemyAction{
public:
	WorkStrongAttackAction():WorkEnemyAction(ActionTypes::WEAK_ATTACK){
		collisionTime_      = 0;
		collisionStartTime_ = 0;
		collisionRadius_    = 0;
		damage_             = 0;
		maxRotateY2Player_  = 0;
	}
	~WorkStrongAttackAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float collisionTime_;
	float collisionStartTime_;
	float collisionRadius_;
	//与えるダメージ
	float damage_;

	// player への 振り向き上限
	float maxRotateY2Player_;
};

class WorkRushAttackAction
	:public WorkEnemyAction{
public:
	WorkRushAttackAction():WorkEnemyAction(ActionTypes::RUSH_ATTACK){
		collisionRadius_   = 0.0f;
		maxRotateY2Player_ = 0.0f;
		damage_            = 0.0f;
		speed_             = 0.0f;
	}
	~WorkRushAttackAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float collisionRadius_;
	float maxRotateY2Player_;
	float damage_;
	float speed_;
};
#pragma endregion
