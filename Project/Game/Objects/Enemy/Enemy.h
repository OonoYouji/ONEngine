#pragma once

#include <memory>

#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "GameObjectManager/BaseGameObject.h"

namespace EnemyBehaviorTree{ class Node; }
class AnimationRenderer;
class Player;
class IEnemyState;

class Enemy :
	public BaseGameObject{
public:
	Enemy(Player* player);
	~Enemy();
	void Initialize()override;
	void Update()override;

	void Debug()override;
private:
	Player* player_ = nullptr;

	std::unique_ptr<IEnemyState> currentState_ = nullptr;
	AnimationRenderer* animationRender_ = nullptr;

	const float maxHp_;
	float hp_;
	const float maxStamina_;
	float stamina_;

	float speed_;

	std::string currentAction_;

	/// <summary>
	/// IdleState variable
	/// </summary>
	struct WorkIdleAction{
		float hpWeight_;
		float staminaWeight_;

		float healingStamina_;
	};
	WorkIdleAction workShortIdle_;
	WorkIdleAction workLongIdle_;

	/// <summary>
	/// AttackState variable
	/// </summary>
	struct WorkAttackAction{
		float hpWeight_;
		float staminaWeight_;

		float activationDistance_;

		float damage_;
		float staminaConsumed_;
	};
	WorkAttackAction workWeakAttack_;
	WorkAttackAction workStrongAttack_;
public:
	void SetAnimationRender(const std::string& filePath);
	void TransitionState(IEnemyState* next);

	void Debug_SetCurrentAction(const std::string& action){ currentAction_ = action; }

	Player* GetPlayer()const;

	const float GetMaxHP()const{ return maxHp_; }
	float GetHP()const{ return hp_; }
	void  SetHP(float hp){ hp_ = hp; }

	const float GetMaxStamina()const{ return maxStamina_; }
	float GetStamina()const{ return stamina_; }
	void  SetStamina(float stamina){ stamina_ = stamina; }

	float GetSpeed()const{ return speed_; }

	const WorkIdleAction& GetWorkShortIdle()const{ return workShortIdle_; }
	const WorkIdleAction& GetWorkLongIdle()const{ return workLongIdle_; }

	const WorkAttackAction& GetWorkWeakAttack()const{ return workWeakAttack_; }
	const WorkAttackAction& GetWorkStrongAttack()const{ return workStrongAttack_; }

	float GetShortIdlePoint()const;
	float GetLongIdlePoint()const;
	float GetWeakAttackPoint()const;
	float GetStrongAttackPoint()const;
};