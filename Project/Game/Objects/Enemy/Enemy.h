#pragma once

#include <memory>

#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "GameObjectManager/BaseGameObject.h"

namespace EnemyBehaviorTree{ class Node; }
class AnimationRenderer;
class IEnemyState;

class Enemy :
	public BaseGameObject{
public:
	Enemy();
	~Enemy();
	void Initialize()override;
	void Update()override;

	void Debug()override;
private:
	std::unique_ptr<IEnemyState> currentState_ = nullptr;
	AnimationRenderer* animationRender_ = nullptr;

	const float maxHp_;
	float hp_;
	const float maxStamina_;
	float stamina_;
	// 凶暴性 どれだけ攻撃をしてくるか
	float aggressive_;

	float speed_;
	Vector3 velocity_;
public:
	void SetAnimationRender(AnimationRenderer* next){ animationRender_ = next; }
	void TransitionState(IEnemyState* next);

	const float GetMaxHP()const{ return maxHp_; }
	float GetHP()const{ return hp_; }
	void  SetHP(float hp){ hp_ = hp; }

	float GetAggressive()const{ return aggressive_; }
	void  SetAggressive(float aggressive){ aggressive_ = aggressive; }

	const float GetMaxStamina()const{ return maxStamina_; }
	float GetStamina()const{ return stamina_; }
	void  SetStamina(float stamina){ stamina_ = stamina; }
};