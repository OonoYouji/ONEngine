#pragma once
#include "IEnemyState.h"

class EnemyAttackState :
	public IEnemyState{
public:
	EnemyAttackState(Enemy* enemy):IEnemyState(enemy){}
	~EnemyAttackState(){}

	void Initialize()override;
	void Update()override;
private:

public:
	void SetRootNode(std::unique_ptr<EnemyBehaviorTree::Node> rootNode){
		rootNode_ = std::move(rootNode);
	}
};

#pragma region"Nodes"
class ChaseAction
	:public EnemyBehaviorTree::Action{
public:
	ChaseAction(Enemy* enemy,float gaugeLength):EnemyBehaviorTree::Action(enemy),gaugeLength_(gaugeLength){}
	EnemyBehaviorTree::Status tick()override;
private:
	float gaugeLength_;
	float maxTime_ = 3.0f;
	float currentTime_ = 0.0f;
};

class WeakAttackAction
	:public EnemyBehaviorTree::Action{
public:
	WeakAttackAction(Enemy* enemy);
	EnemyBehaviorTree::Status tick()override;
};

class StrongAttackAction
	:public EnemyBehaviorTree::Action{
public:
	StrongAttackAction(Enemy* enemy);
	EnemyBehaviorTree::Status tick()override;
};

class ChaseAndAttack
	:public EnemyBehaviorTree::Sequence{
public:
	ChaseAndAttack(Enemy* enemy,float chaseGaugeLength,std::unique_ptr<EnemyBehaviorTree::Action> action);
};

class AttackSelector
	:public EnemyBehaviorTree::ScoringSelectorNode{
public:
	AttackSelector(Enemy* enemy);
};

class AttackContinueCondition
	: public EnemyBehaviorTree::Condition{
public:
	AttackContinueCondition(Enemy* enemy):EnemyBehaviorTree::Condition(enemy){}
	EnemyBehaviorTree::Status tick()override;
};

class ContinueAction
	: public EnemyBehaviorTree::Action{
public:
	ContinueAction(Enemy* enemy,EnemyAttackState* state);
	EnemyBehaviorTree::Status tick()override;
private:
	EnemyAttackState* state_;
};

class AttackStateTree
	: public EnemyBehaviorTree::Sequence{
public:
	AttackStateTree(Enemy* enemy,EnemyAttackState* state);
};
#pragma endregion