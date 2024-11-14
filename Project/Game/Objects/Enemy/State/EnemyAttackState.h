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
#pragma endregion