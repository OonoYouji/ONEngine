#pragma once
#include "IEnemyState.h"

class EnemyIdleState :
	public IEnemyState{
public:
	EnemyIdleState(Enemy* enemy):IEnemyState(enemy){}
	~EnemyIdleState(){}

	void Initialize();
	void Update();
private:

};

class LongIdleAction
	:public EnemyBehaviorTree::Action{
public:
	LongIdleAction(Enemy* enemy,float _time);
	EnemyBehaviorTree::Status tick()override;
private:
	float time_;
};

class ShortIdleAction
	:public EnemyBehaviorTree::Action{
public:
	ShortIdleAction(Enemy* enemy,float _time);
	EnemyBehaviorTree::Status tick()override;
private:
	float time_;
};

class IdleLengthSelector
	:public EnemyBehaviorTree::ScoringSelectorNode{
public:
	IdleLengthSelector(Enemy* enemy);
};

class IdleStateTree :
	public EnemyBehaviorTree::Sequence{
public:
	IdleStateTree(Enemy* enemy): EnemyBehaviorTree::Sequence(enemy){
		// IdleSelector ノード（LongIdleかShortIdleのいずれかを実行）
		addChild(std::make_unique<IdleLengthSelector>(enemy));
	}
};