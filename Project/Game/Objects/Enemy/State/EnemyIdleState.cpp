#include "EnemyIdleState.h"

#include "FrameManager/Time.h"
#include "Game/Objects/Enemy/BehaviorTree/Node.h"
#include "Game/Objects/Enemy/Enemy.h"

void EnemyIdleState::Initialize(){
	float idlePoint = 0.0f;

	rootNode_.reset(new IdleStateTree(enemy_));
}

void EnemyIdleState::Update(){
	rootNode_->tick();
}

LongIdleAction::LongIdleAction(Enemy* enemy,float _time):EnemyBehaviorTree::Action(enemy),time_(_time){
	//	enemy_->SetAnimationRender("Kari_Boss_LongWait");
}

EnemyBehaviorTree::Status LongIdleAction::tick(){
	time_ -= Time::DeltaTime();
	enemy_->Debug_SetCurrentAction("LongIdle");
	if(time_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}

	return EnemyBehaviorTree::Status::RUNNING;
}

ShortIdleAction::ShortIdleAction(Enemy* enemy,float _time):EnemyBehaviorTree::Action(enemy),time_(_time){
	// enemy_->SetAnimationRender("Kari_Boss_Wait");
}

EnemyBehaviorTree::Status ShortIdleAction::tick(){
	time_ -= Time::DeltaTime();
	enemy_->Debug_SetCurrentAction("ShortIdle");
	if(time_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

IdleLengthSelector::IdleLengthSelector(Enemy* enemy):EnemyBehaviorTree::ScoringSelectorNode(enemy){
	addChild(std::make_unique<ShortIdleAction>(enemy,1.0f),[](){return 10.0f; });
	addChild(std::make_unique<LongIdleAction>(enemy,2.0f),[](){return 0.0f; });
}