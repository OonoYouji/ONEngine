#include "IdleState.h"

#include "FrameManager/Time.h"
#include "Game/Objects/Enemy/BehaviorTree/Node.h"
#include "Game/Objects/Enemy/Enemy.h"

void IdleState::Initialize(){
	float idlePoint = 0.0f;

	rootNode_.reset(new IdleStateTree(enemy_));
}

void IdleState::Update(){
	rootNode_->tick();
}

EnemyBehaviorTree::Status LongIdleAction::tick(){
	time_ -= Time::DeltaTime();
	if(time_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}

	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status ShortIdleAction::tick(){
	time_ -= Time::DeltaTime();
	if(time_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

IdleCondition::IdleCondition(Enemy* enemy):Condition(enemy),
maxStamina(enemy_->GetMaxStamina()),
currentStamina(enemy_->GetStamina()),
maxHP(enemy_->GetMaxHP()),
currentHP(enemy_->GetHP()){
}

EnemyBehaviorTree::Status IdleCondition::tick(){
	float idlePoint = 0.0f;
	idlePoint += (1.0f - (currentStamina / maxStamina)) * staminaWeight_;
	idlePoint += (currentHP / maxHP) * hpWeight_;
	// ロング
	if(idlePoint >= judgePointLongOrShort_){
		return EnemyBehaviorTree::Status::SUCCESS;
	} else{ // ショート
		return EnemyBehaviorTree::Status::FAILURE;
	}
}

EnemyBehaviorTree::Status TransitionNode::tick(){
	enemy_->TransitionState(new IdleState(enemy_));
	return EnemyBehaviorTree::Status::SUCCESS;
}