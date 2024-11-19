#pragma once

#include <string>
#include <unordered_map>

#include "CustomMath/MotionTimes.h"

/// <summary>
/// 前方宣言
/// </summary>
enum class ActionTypes;

/// <summary>
/// 行動の一覧
/// </summary>
enum class ActionTypes{
	WEAK_ATTACK,
	STRONG_ATTACK,
	IDLE
};

static std::unordered_map<ActionTypes,std::string> actionTypeWord = {
	{ActionTypes::WEAK_ATTACK,"WEAK_ATTACK"},
	{ActionTypes::STRONG_ATTACK,"STRONG_ATTACK"},
	{ActionTypes::IDLE,"IDLE"}
};

/// <summary>
/// 継承のために class にしているが 基本扱いは struct と同じ
/// </summary>
class WorkEnemyAction{
public:
	WorkEnemyAction(ActionTypes type):type_(type),motionTimes_({0.0f,0.0f,0.0f}){}

	virtual ~WorkEnemyAction(){}

	virtual void Debug();
public:
	ActionTypes type_;
	std::string animationName_;
	MotionTimes motionTimes_;
public:
	static std::list<std::pair<std::string,std::string>> animationList;
};

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

	float collisionRadius_;
	//与えるダメージ
	float damage_;
};

class WorkStrongAttackAction
	:public WorkEnemyAction{
public:
	WorkStrongAttackAction():WorkEnemyAction(ActionTypes::WEAK_ATTACK){}
	~WorkStrongAttackAction(){}

	void Debug()override;

	float collisionTime_;

	float collisionRadius_;
	//与えるダメージ
	float damage_;

	// player への 振り向き方向
	float rotateY2Player;
	// player への 振り向き上限
	float maxRotateY2Player_;
};

#pragma endregion
