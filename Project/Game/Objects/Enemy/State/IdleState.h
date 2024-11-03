#pragma once
#include "IEnemyState.h"

class IdleState :
	public IEnemyState{
public:
	IdleState(Enemy* enemy):IEnemyState(enemy){}
	~IdleState(){}

	void Initialize();
	void Update();
private:

};

namespace EnemyBehaviorTree{
	class Node;
	class Action;
	class Condition;
	class Sequence;
	class Selector;
}

class LongIdleAction
	:public EnemyBehaviorTree::Action{
public:
	LongIdleAction(Enemy* enemy,float _time):EnemyBehaviorTree::Action(enemy),time_(_time){}
	EnemyBehaviorTree::Status tick()override;
private:
	float time_;
};

class ShortIdleAction
	:public EnemyBehaviorTree::Action{
public:
	ShortIdleAction(Enemy* enemy,float _time):EnemyBehaviorTree::Action(enemy),time_(_time){}
	EnemyBehaviorTree::Status tick()override;
private:
	float time_;
};

class IdleCondition
	: public EnemyBehaviorTree::Condition{
public:
	IdleCondition(Enemy* enemy);
	EnemyBehaviorTree::Status tick()override;
private:
	float judgePointLongOrShort_;
	float staminaWeight_;
	float hpWeight_;
	const float maxStamina;
	const float currentStamina;
	const float maxHP;
	const float currentHP;
};

class IdleLengthSelector
	:public EnemyBehaviorTree::Selector{
public:
	IdleLengthSelector(Enemy* enemy):EnemyBehaviorTree::Selector(enemy){
		addChild(std::make_unique<IdleCondition>(enemy));
		addChild(std::make_unique<LongIdleAction>(enemy,2.0f));
		addChild(std::make_unique<ShortIdleAction>(enemy,1.0f));
	}
private:
};

class TransitionNode
	: public EnemyBehaviorTree::Action{
public:
	TransitionNode(Enemy* enemy): Action(enemy){}

	EnemyBehaviorTree::Status tick() override;
};

class IdleStateTree :
	public EnemyBehaviorTree::Sequence{
public:
	IdleStateTree(Enemy* enemy): EnemyBehaviorTree::Sequence(enemy){
		// IdleSelector ノード（LongIdleかShortIdleのいずれかを実行）
		addChild(std::make_unique<IdleLengthSelector>(enemy));
		// TransitionNode ノード（次のステートを決定）
		addChild(std::make_unique<TransitionNode>(enemy));
	}
};