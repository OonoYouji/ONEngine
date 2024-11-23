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
	WEAK_ATTACK_2,
	STRONG_ATTACK,
	TACKLE_ATTACK,
	CHASE,
	RANGED_ATTACK,
	LONGRANGE_ATTACK,
	JUMP_AWAY,
	COUNT // 要素数を把握するための要素
};

static std::unordered_map<ActionTypes,std::string> actionTypeWord = {
	{ActionTypes::IDLE			  ,"IDLE"},
	{ActionTypes::WEAK_ATTACK	  ,"WEAK_ATTACK"},
	{ActionTypes::WEAK_ATTACK_2	  ,"WEAK_ATTACK_2"},
	{ActionTypes::STRONG_ATTACK	  ,"STRONG_ATTACK"},
	{ActionTypes::TACKLE_ATTACK	  ,"TACKLE_ATTACK"},
	{ActionTypes::CHASE			  ,"CHASE"},
	{ActionTypes::RANGED_ATTACK	  ,"RANGED_ATTACK"},
	{ActionTypes::LONGRANGE_ATTACK,"LONGRANGE_ATTACK"},
	{ActionTypes::JUMP_AWAY       ,"JUMP_AWAY"}
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
	MotionTimes motionTimes_;
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

class WorkWeakAttack2Action
	:public WorkEnemyAction{
public:
	WorkWeakAttack2Action():WorkEnemyAction(ActionTypes::WEAK_ATTACK){
		collisionTime_      = 0;
		collisionStartTime_ = 0;
		collisionRadius_    = 0;
		damage_             = 0;
		maxRotateY2Player_  = 0;
	}
	~WorkWeakAttack2Action(){}

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

class WorkTackleAttackAction
	:public WorkEnemyAction{
public:
	WorkTackleAttackAction():WorkEnemyAction(ActionTypes::TACKLE_ATTACK){
		lockOnTime_        = 0.0f;
		collisionRadius_   = 0.0f;
		maxRotateSpeed_    = 0.0f;
		rotateSensitivity_ = 0.0f;
		damage_            = 0.0f;
		speed_             = 0.0f;
	}
	~WorkTackleAttackAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float lockOnTime_;
	float collisionRadius_;
	float maxRotateSpeed_;
	float rotateSensitivity_;
	float damage_;
	float speed_;
};

class WorkRangedAttackAction
	:public WorkEnemyAction{
public:
	WorkRangedAttackAction():WorkEnemyAction(ActionTypes::RANGED_ATTACK){
		attackForPlayerProbability_ = 0.0f;
		bulletScale_				= 0.0f;
		damage_ 					= 0.0f;
		spawnCoolTime_				= 0.0f;
		spawnPositionY_ 			= 0.0f;
		timeToLand_					= 0.0f;
		spawnRange_ 				= 0.0f;
		lifeTime_ 					= 0.0f;
	}
	~WorkRangedAttackAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float bulletScale_;
	float damage_;
	float timeToLand_;
	float spawnCoolTime_;
	float spawnRange_;
	float spawnPositionY_;
	float lifeTime_;
	// プレイヤーを 攻撃する 確率
	float attackForPlayerProbability_;
};

class WorkLongRangeAttackAction
	:public WorkEnemyAction{
public:
	WorkLongRangeAttackAction()
		:WorkEnemyAction(ActionTypes::LONGRANGE_ATTACK){
		damage_ 			 = 0.0f;
		bulletSpeed_ 		 = 0.0f;
		bulletScale_ 		 = 0.0f;
		bulletFireInterval_  = 0.0f;
		bulletSpawnOffsetZ_  = 0.0f;
		rotateMaxSpeed_ 	 = 0.0f;
	}
	~WorkLongRangeAttackAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float damage_;
	float bulletSpeed_;
	float bulletScale_;
	float bulletFireInterval_;
	float bulletSpawnOffsetZ_;
	float rotateMaxSpeed_;
};

class WorkJumpAwayAction
	:public WorkEnemyAction{
public:
	WorkJumpAwayAction()
		:WorkEnemyAction(ActionTypes::JUMP_AWAY){
		jumpSpeed_ = 0.0f;
		mass_ 	   = 0.0f;
		distance_  = 0.0f;
	}
	~WorkJumpAwayAction(){}

	void Debug()override;

	void Save(const std::string& name)override;
	void Load(const std::string& name)override;

	float jumpSpeed_;
	float mass_;
	// 中心からどれだけ離れたところに移動するか
	float distance_;
};

#pragma endregion