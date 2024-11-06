#include "IEnemyState.h"
#include "Game/Objects/Enemy/Enemy.h"

#include "EnemyAttackState.h"
#include "EnemyIdleState.h"

IEnemyState::IEnemyState(Enemy* enemy): enemy_(enemy){}

EnemyBehaviorTree::Status TransitionNode::tick(){
	float shortIdlePoint = enemy_->GetShortIdlePoint();
	float longIdlePoint  = enemy_->GetLongIdlePoint();
	float weakAttackPoint = enemy_->GetWeakAttackPoint();
	float strongAttackPoint = enemy_->GetStrongAttackPoint();

	if((shortIdlePoint + longIdlePoint) * 0.5f > (weakAttackPoint + strongAttackPoint) * 0.5f){
		enemy_->TransitionState(new EnemyIdleState(enemy_));
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	enemy_->TransitionState(new EnemyAttackState(enemy_));
	return EnemyBehaviorTree::Status::SUCCESS;
}