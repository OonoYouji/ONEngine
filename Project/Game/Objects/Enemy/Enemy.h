#pragma once

#include <memory>

#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "CustomMath/MotionTimes.h"
#include "Game/Objects/Enemy/BehaviorTree/Node.h"
#include "GameObjectManager/BaseGameObject.h"

class AnimationRenderer;
class Player;
class IEnemyState;

/// <summary>
/// IdleState variable
/// </summary>
struct WorkIdleAction{};

/// <summary>
/// AttackState variable
/// </summary>
struct WorkAttackAction{
	MotionTimes motionTimes_;
	// 与えるダメージ
	float damage_;
};

namespace EnemyBehaviorTree{
	class AttackAction;
	class AttackCombo;
}

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

	float speed_;

	std::string currentAction_;

	std::unordered_map<std::string,WorkAttackAction> workAttackVariables_;
	std::unordered_map<std::string,EnemyBehaviorTree::AttackCombo> attackCombo_;

#ifdef _DEBUG
	bool isCreateAttackAction_;
	bool isCreateAttackCombo_;

	// 編集されているもの 
	std::string* currentEditActionName_ = nullptr;
	WorkAttackAction* currentEditAction_;
	EnemyBehaviorTree::AttackCombo* currentEditCombo_;

	std::string createObjectName_ = "NULL";
#endif // _DEBUG

public:
	void SetAnimationRender(const std::string& filePath);
	void TransitionState(IEnemyState* next);

	void Debug_SetCurrentAction(const std::string& action){ currentAction_ = action; }

	Player* GetPlayer()const;

	const float GetMaxHP()const{ return maxHp_; }
	float GetHP()const{ return hp_; }
	void  SetHP(float hp){ hp_ = hp; }

	float GetSpeed()const{ return speed_; }

	const WorkAttackAction& GetWorkAttack(const std::string& attack)const;
};