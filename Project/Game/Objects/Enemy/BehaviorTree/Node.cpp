#include "Node.h"

#include "../Enemy.h"
#include "../State/IEnemyState.h"

EnemyBehaviorTree::Status EnemyBehaviorTree::EnemyTransitionSequence::tick(){
	if(EnemyBehaviorTree::Sequence::tick() != EnemyBehaviorTree::Status::SUCCESS){
		return EnemyBehaviorTree::Sequence::tick();
	}
	enemy_->TransitionState(nextState_);
    return EnemyBehaviorTree::Status::SUCCESS;
}
