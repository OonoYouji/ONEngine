#pragma once

#include <deque>
#include <list>
#include <memory>
#include <unordered_map>

#include "EnemyBehaviorTree/Node.h"
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
class EnemyEffect;
class AudioSource;

/// <summary>
/// コンボで使用される Attack の 情報
/// </summary>
struct ComboAttack{
	const std::string& attackName_;
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

	void LoadAllAnimation();

	std::unique_ptr<EnemyBehaviorTree::Sequence> CreateAction(const std::string& actionName);
	std::unique_ptr<WorkEnemyAction> CreateWorker(ActionTypes type);

	void DecideNextNode();
private:
	Player* player_ = nullptr;

	class EntityShadow* entityShadow_ = nullptr;

	std::unique_ptr<EnemyBehaviorTree::Node> rootNode_ = nullptr;
	AnimationRenderer* bodyAnimationRenderer_ 	   	   = nullptr;
	AnimationRenderer* weaponAnimationRenderer_    	   = nullptr;
	AnimationRenderer* subWeaponAnimationRenderer_ 	   = nullptr;
	AnimationRenderer* effectAnimationRenderer_    	   = nullptr;

	AudioSource* se_;

	EnemyEffect* effect1_;
	EnemyEffect* effect2_;
	EnemyEffect* passiveEffect_;

	std::list<EnemyEffect*> damageEffects_;

	// 武器がスポーンする とき の uv 座標
	float spawnWeaponUvEndPosY_;
	float spawnWeaponUvStartPosY_;

	float spawnSubWeaponUvEndPosY_;
	float spawnSubWeaponUvStartPosY_;

	/// <summary>
	/// 本体のコライダー これに当たるとダメージを受ける
	/// </summary>
	SphereCollider* hitCollider_;
	/// <summary>
	/// 攻撃を与えるCollider
	/// </summary>
	EnemyAttackCollider* attackCollider_;
	float colliderRadius_ = 4.0f; /// 仮の適当な値

	bool isInvisible_;
	float maxHp_;
	float hp_;

	bool outOfStage_;
	bool preOutOfStage_;

	// 各 アクションで セットするので 基本は 0
	float currentDamage_ = 0.0f;
	HpState currentHpState_;
	HpState preHpState_;
	std::array<float,static_cast<int32_t>(HpState::COUNT)> thresholdByHpState_;

	// 調整項目保存用
	using AttackActionName = std::string;
	std::unordered_map<AttackActionName,std::unique_ptr<WorkEnemyAction>> workEnemyActionVariables_;

	// RangeType別
	std::array<std::unordered_map<EnemyAttackRangeType,std::deque<std::string>>,static_cast<int32_t>(HpState::COUNT)> comboByRangeTypeByHpState_;
	std::unordered_map<EnemyAttackRangeType,float> distanceByRangeTypes_;

	std::array<std::unordered_map<std::string,ComboAttacks>,static_cast<int32_t>(HpState::COUNT)> editComboVariables_;

	std::string preComboName_;
#ifdef _DEBUG
	bool actionIsActive_;
	HpState currentEditHpState_;
	bool isCreateWindowPop_;
	bool isComboCreateWindowPop_;
	// 編集されているもの 

	std::string dointCombo_;

	std::string* currentEditActionName_;
	std::string* currentEditComboName_;
	std::string actionNameBeforeNameChange_;
	std::string comboNameBeforeNameChange_;

	WorkEnemyAction* currentEditAction_;
	ComboAttacks* currentEditCombo_;

	Vector2 actionListWindowSize_ 	= {100.0f,75.0f};
	Vector2 combosActionWindowSize_ = {100.0f,75.0f};

	std::string createObjectName_ = "NULL";
#endif // _DEBUG
public:
	EnemyEffect* GetEnemy1Effect();
	EnemyEffect* GetEnemy2Effect();
	void StartPassiveEffect();

	bool GetIsInvisible()const{ return isInvisible_; }
	void SetIsInvisible(bool isInvisible){ isInvisible_ = isInvisible; }

	Vector3 GetCollisionOffset(ActionTypes type);

	void SetAnimationRender(const std::string& filePath);

	void SetAnimationRender(const std::string& filePath,
							const std::string& weaponFilePath);

	void SetAnimationRender(const std::string& filePath,
							const std::string& weaponFilePath,
							const std::string& subWeapon);

	void SetAnimationRender(const std::string& filePath,
							const std::string& weaponFilePath,
							const std::string& subWeapon,
							const std::string& effect);

	void PlaySE(const std::string& se);

	void SpawnDamageEffect(float damage);

	bool GetOutOfStage()const{ return outOfStage_; }
	bool GetTriggerOutOfStage()const{ return outOfStage_ && !preOutOfStage_; }

	void SpawnWeapon(float t);
	void SpawnSubWeapon(float t);

	void SetAnimationTotalTime(float _totalTime);

	void SetEffectAnimationTotalTime(float _totalTime);

	float GetBodyAnimationTotalTime()const;

	void ResetAnimationTotal();
	void ResetEffectAnimationTotal();

	void SetAnimationFlags(int _flags,bool _isResetTime = true);
	void SetEffectAnimationFlags(int _flags,bool _isResetTime = true);

	Player* GetPlayer()const;

	const float GetMaxHP()const{ return maxHp_; }
	float GetHP()const{ return hp_; }
	void  SetHP(float hp){ hp_ = hp; }
	HpState GetHpState()const{ return currentHpState_; }

	float GetDamage()const{ return currentDamage_; }
	void  SetDamage(float damage){ currentDamage_  = damage; }

	const ComboAttacks& GetComboAttacks(int32_t hpState,const std::string& comboName)const;
	const std::deque<std::string>& GetComboList(HpState state,EnemyAttackRangeType rangeType)const;

	float GetDistanceByRangeTypes(EnemyAttackRangeType rangeType)const;

	void ActivateAttackCollider(ActionTypes offset);
	void TerminateAttackCollider();

	float GetColliderRadius(){ return colliderRadius_; }
};