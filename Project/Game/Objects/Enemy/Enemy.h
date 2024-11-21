#pragma once

#include <deque>
#include <memory>
#include <unordered_map>

#include "Game/Objects/Enemy/BehaviorTree/Node.h"
#include "GameObjectManager/BaseGameObject.h"

/// <summary>
/// 前方宣言
/// </summary>
class AnimationRenderer;
enum class ActionTypes;
enum class EnemyAttackRangeType : int32_t;
namespace EnemyBehaviorTree{
	class Action;
	class AttackAction;
	class AttackCombo;
	class IdleAction;
}
class IEnemyState;
class Player;
class EnemyAttackCollider;
class SphereCollider;
class WorkEnemyAction;

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

/// <summary>
/// モデルに対して Colliderを作るので，Collider 用の Model と Collider を 一緒に 管理する
/// </summary>
struct EnemyCollider{
	Model* model_;
	SphereCollider* sphereCollider_;
};

static std::string enemyJsonDirectory = "./Resources/Parameters/Objects/Enemy";
static std::string enemyComboDirectory = enemyJsonDirectory + "/Combo";
static std::string enemyActionDirectory = enemyJsonDirectory + "/Action";

enum class HpState{
	HP_HIGHTE,
	HP_NORMAL,
	HP_LOW,
	COUNT
};

static std::array <std::string,static_cast<int32_t>(HpState::COUNT)> wordByHpState = {
	"HP_HIGHTE",
	"HP_NORMAL",
	"HP_LOW",
};

class Enemy :
	public BaseGameObject{
public:
	Enemy(Player* player,EnemyAttackCollider* collider);
	~Enemy();
	void Initialize()override;
	void Update()override;

	void Debug()override;

	void SaveStatus();
	void SaveCombos();
	void SaveAllAction();
	void SaveCombo(const std::string& comboName,int32_t hpState);

	void LoadStatus();
	void LoadCombos();
	void LoadAllAction();
	void LoadCombo(const std::string& comboName,int32_t size,int32_t hpState);

	std::unique_ptr<EnemyBehaviorTree::Sequence> CreateAction(const std::string& actionName);
	std::unique_ptr<WorkEnemyAction> CreateWorker(ActionTypes type);

	void DecideNextNode();
private:
	Player* player_ = nullptr;
	EnemyAttackCollider* enemy_ = nullptr;

	std::unique_ptr<EnemyBehaviorTree::Node> rootNode_ = nullptr;
	AnimationRenderer* animationRender_ = nullptr;
	// 本体のコライダー これに当たるとダメージを受ける
	SphereCollider* enemyCollider_;

	float maxHp_;
	float hp_;
	std::array<float,static_cast<int32_t>(HpState::COUNT)> thresholdByHpState_;
	HpState currentHPState_;

	float speed_;

	std::string currentAction_;

	/// <summary>
	/// 攻撃を与えるCollider
	/// </summary>
	EnemyAttackCollider* attackCollider_;

	// 調整項目保存用
	using AttackActionName = std::string;
	std::unordered_map<AttackActionName,std::unique_ptr<WorkEnemyAction>> workEnemyActionVariables_;

	// RangeType別
	std::array<std::unordered_map<EnemyAttackRangeType,std::deque<std::string>>,static_cast<int32_t>(HpState::COUNT)> comboByRangeTypeByHpState_;
	std::unordered_map<EnemyAttackRangeType,float> distanceByRangeTypes_;

	std::array<std::unordered_map<std::string,ComboAttacks>,static_cast<int32_t>(HpState::COUNT)> editComboVariables_;
#ifdef _DEBUG
	HpState currentEditHpState_;
	bool isCreateWindowPop_;
	bool isComboCreateWindowPop_;
	// 編集されているもの 
	std::string currentEditActionName_;
	std::string currentEditComboName_;
	WorkEnemyAction* currentEditAction_;
	ComboAttacks* currentEditCombo_;

	std::string createObjectName_ = "NULL";
#endif // _DEBUG

public:
	void SetAnimationRender(const std::string& filePath);

	void Debug_SetCurrentAction(const std::string& action){ currentAction_ = action; }

	Player* GetPlayer()const;

	const float GetMaxHP()const{ return maxHp_; }
	float GetHP()const{ return hp_; }
	void  SetHP(float hp){ hp_ = hp; }
	HpState GetHpState()const{ return currentHPState_; }

	float GetSpeed()const{ return speed_; }

	const ComboAttacks& GetComboAttacks(int32_t hpState,const std::string& comboName)const;
	const std::deque<std::string>& GetComboList(HpState state,EnemyAttackRangeType rangeType)const;

	float GetDistanceByRangeTypes(EnemyAttackRangeType rangeType)const;

	void ActivateAttackCollider(ActionTypes offset,float radius);
	void TerminateAttackCollider();
};