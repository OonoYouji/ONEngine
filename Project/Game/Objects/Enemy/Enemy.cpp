#include "Enemy.h"
#include "State/IEnemyState.h"
#include "State/IdleState.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Enemy::Enemy():BaseGameObject(),maxHp_(0.0f),maxStamina_(0.0f){
	CreateTag(this);
}

Enemy::~Enemy(){}

void Enemy::Initialize(){
	animationRender_ = AddComponent<AnimationRenderer>("Kari_Boss_Wait");

	hp_ = maxHp_;
	stamina_ = maxStamina_;

	currentState_.reset(new IdleState(this));
	currentState_->Initialize();
}

void Enemy::Update(){
	currentState_->Update();
}

void Enemy::Debug(){

}

void Enemy::TransitionState(IEnemyState* next){
	currentState_.reset(next);
	currentState_->Initialize();
}
