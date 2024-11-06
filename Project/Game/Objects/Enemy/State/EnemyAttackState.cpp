#include "EnemyAttackState.h"
#include "../Enemy.h"

#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"
#include "Objects/Player/Player.h"

void EnemyAttackState::Initialize(){
	rootNode_ = std::make_unique<AttackStateTree>(enemy_,this);
}

void EnemyAttackState::Update(){
	rootNode_->tick();
}

WeakAttackAction::WeakAttackAction(Enemy* enemy):Action(enemy){
	// enemy_->SetAnimationRender("Kari_Boss_WeakAttack");
	enemy_->SetStamina(enemy_->GetStamina() - enemy_->GetWorkWeakAttack().staminaConsumed_);
}

EnemyBehaviorTree::Status WeakAttackAction::tick(){
	enemy_->Debug_SetCurrentAction("WeakAttack");
	return EnemyBehaviorTree::Status::SUCCESS;
}

StrongAttackAction::StrongAttackAction(Enemy* enemy):Action(enemy){
	// enemy_->SetAnimationRender("Kari_Boss_StrongAttack");
	enemy_->SetStamina(enemy_->GetStamina() - enemy_->GetWorkStrongAttack().staminaConsumed_);
}

EnemyBehaviorTree::Status StrongAttackAction::tick(){
	enemy_->Debug_SetCurrentAction("StrongAttack");
	return EnemyBehaviorTree::Status::SUCCESS;
}

EnemyBehaviorTree::Status AttackContinueCondition::tick(){
	float e2pLength = enemy_->GetPosition().Len() - enemy_->GetPlayer()->GetPosition().Len();
	if(e2pLength <= 0.1f){
		float shortIdlePoint = enemy_->GetShortIdlePoint();
		float longIdlePoint  = enemy_->GetLongIdlePoint();
		float weakAttackPoint = enemy_->GetWeakAttackPoint();
		float strongAttackPoint = enemy_->GetStrongAttackPoint();

		if((shortIdlePoint + longIdlePoint) * 0.5f < (weakAttackPoint + strongAttackPoint) * 0.5f){
			return EnemyBehaviorTree::Status::SUCCESS;
		}
	}
	return EnemyBehaviorTree::Status::FAILURE;
}

EnemyBehaviorTree::Status ChaseAction::tick(){
	enemy_->Debug_SetCurrentAction("Chase");


	Vector3 e2pDiff = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
	if(e2pDiff.Len() <= gaugeLength_){
		return EnemyBehaviorTree::Status::SUCCESS;
	}

	Vector3 currentRotate = enemy_->GetRotate();
	// Rotate Update
	{
		e2pDiff = e2pDiff.Normalize();
		currentRotate.y = LerpShortAngle(currentRotate.y,atan2(e2pDiff.x,e2pDiff.y),0.1f);
		enemy_->SetRotate(currentRotate);
	}

	// Postion Update
	{
		Vector3 velocity = {0.0f,0.0f,enemy_->GetSpeed() * Time::DeltaTime()};
		velocity = Mat4::Transform(velocity,Mat4::MakeRotate(currentRotate));
		enemy_->SetPosition(enemy_->GetPosition() + velocity);
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

ChaseAndAttack::ChaseAndAttack(Enemy* enemy,float chaseGaugeLength,std::unique_ptr<EnemyBehaviorTree::Action> action):EnemyBehaviorTree::Sequence(enemy){
	addChild(std::make_unique<ChaseAction>(enemy,chaseGaugeLength));
	addChild(std::move(action));
}

AttackSelector::AttackSelector(Enemy* enemy):EnemyBehaviorTree::ScoringSelectorNode(enemy){
	addChild(std::make_unique<ChaseAndAttack>(enemy_,enemy_->GetWorkStrongAttack().activationDistance_,std::make_unique<StrongAttackAction>(enemy)),[this](){return enemy_->GetStrongAttackPoint(); });
	addChild(std::make_unique<ChaseAndAttack>(enemy_,enemy_->GetWorkStrongAttack().activationDistance_,std::make_unique<WeakAttackAction>(enemy)),[this](){return enemy_->GetWeakAttackPoint(); });
}

ContinueAction::ContinueAction(Enemy* enemy,EnemyAttackState* state):EnemyBehaviorTree::Action(enemy),state_(state){}

EnemyBehaviorTree::Status ContinueAction::tick(){
	state_->SetRootNode(std::make_unique<AttackStateTree>(enemy_,state_));
	return EnemyBehaviorTree::Status::SUCCESS;
}

AttackStateTree::AttackStateTree(Enemy* enemy,EnemyAttackState* state): EnemyBehaviorTree::Sequence(enemy){
	addChild(std::make_unique<AttackSelector>(enemy));

	// ContinueConditionSelector: 攻撃を続けるかどうかを決定する
	auto continueConditionSelector = std::make_unique<EnemyBehaviorTree::Selector>(enemy);
	continueConditionSelector->addChild(std::make_unique<AttackContinueCondition>(enemy));
	continueConditionSelector->addChild(std::make_unique<TransitionNode>(enemy));  // AttackContinueConditionが失敗の場合に実行される

	addChild(std::move(continueConditionSelector));

	// ContinueAction: 攻撃を続ける場合の動作
	addChild(std::make_unique<ContinueAction>(enemy,state));
}
