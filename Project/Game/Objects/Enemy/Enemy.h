#pragma once

#include <memory>

#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "CustomMath/MotionTimes.h"
#include "Game/Objects/Enemy/BehaviorTree/Node.h"
#include "GameObjectManager/BaseGameObject.h"

/// <summary>
/// 前方宣言
/// </summary>
class AnimationRenderer;
enum class EnemyAttackRangeType : int16_t;
namespace EnemyBehaviorTree{
	class AttackAction;
	class AttackCombo;
}
class IEnemyState;
class Player;

/// <summary>
/// IdleState variable
/// </summary>
struct WorkIdleAction{
	std::string animationName_;
	MotionTimes motionTimes_;
};

/// <summary>
/// AttackState variable
/// </summary>
struct WorkAttackAction{
	std::string animationName_;
	MotionTimes motionTimes_;
	//与えるダメージ
	float damage_;
};

/// <summary>
/// コンボで使用される Attack の 情報
/// </summary>
struct ComboAttack{
	std::string attackName_;
	int32_t index_;

	ComboAttack(const std::string& name,int32_t i):attackName_(name),index_(i){}
};

struct ComboAttacks{
	EnemyAttackRangeType rangeType_;
	std::list<ComboAttack> comboAttacks_;
};

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

	// 調整項目保存用
	using AttackActionName = std::string;
	std::unordered_map<AttackActionName,WorkAttackAction> workAttackVariables_;

	// RangeType別
	std::unordered_map<EnemyAttackRangeType,std::list<std::string>> comboByRangeType_;

#ifdef _DEBUG

	std::unordered_map<std::string,ComboAttacks> editComboVariables_;
	bool isCreateWindowPop_;

	// 編集されているもの 
	std::string* currentEditActionName_ = nullptr;
	std::string* currentEditComboName_ = nullptr;

	WorkAttackAction* currentEditAction_;
	ComboAttacks* currentEditCombo_;

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
	const ComboAttacks& GetComboAttacks(const std::string& comboName)const;
	const std::list<std::string>& GetComboList(EnemyAttackRangeType rangeType)const;
};