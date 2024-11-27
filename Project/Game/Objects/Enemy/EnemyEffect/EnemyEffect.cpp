#include "EnemyEffect.h"

#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

EnemyEffect::EnemyEffect(){
	CreateTag(this);
	effect_ = AddComponent<AnimationRenderer>("Effect5");
	effect_->isActive = false;
}

EnemyEffect::~EnemyEffect(){}

void EnemyEffect::Initialize(){}

void EnemyEffect::Update(){}

void EnemyEffect::SetIsActive(bool isActive){
	isActive = isActive;
	effect_->isActive = isActive;
}

void EnemyEffect::SetEffectAnimationRender(const std::string& filePath){
	this->isActive = true;
	effect_->isActive = true;
	effect_->ChangeAnimation(filePath);
}

void EnemyEffect::SetEffectAnimationTotalTime(float _totalTime){
	effect_->SetTotalTime(_totalTime,
						  effect_->GetCurrentNodeAnimationKey());
}

void EnemyEffect::ResetAnimationTotal(){
	effect_->SetTotalTime(
		effect_->GetDuration(effect_->GetCurrentNodeAnimationKey()),
		effect_->GetCurrentNodeAnimationKey()
	);
}

void EnemyEffect::SetEffectAnimationFlags(int _flags,bool _isResetTime){
	effect_->SetAnimationFlags(_flags);
	if(_isResetTime){
		ResetAnimationTotal();
	}
}