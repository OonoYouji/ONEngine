#pragma once
#include "GameObjectManager/BaseGameObject.h"

#include <array>
#include <memory>

class IPlayerBehavior;
class Player : public BaseGameObject{
public:

	Player(class GameCamera* _mainCamera);
	~Player();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:
	class MeshRenderer* meshRenderer_ = nullptr;

	std::unique_ptr<IPlayerBehavior> currentBehavior_;

	///===============================================
	/// status
	///===============================================
	float currentHP_;
	float maxHP_;

	float power_;

	// boss に与える ダメージ (攻撃していないときは ずっと0 になるようにする)
	float damage_;

	bool isInvisible_ = false;

	/// other class pointer
	class GameCamera* pGameCamera_ = nullptr;

public:
	///===============================================
	/// 各 Behavior で 使われる変数群 (基本調整可能なものだけ)
	///===============================================
	/// <summary>
	/// 各モーションを管理するための 時間変数群
	/// </summary>
	struct MotionTimes{
		float startupTime_;
		float activeTime_;
		float endLagTime_;
	};
	/// <summary>
	/// RootBehavior で使われる変数群
	/// </summary>
	struct WorkRootBehavior{
		float speed_;
		float rotateLerpSensitivity_;
	};
	/// <summary>
	/// AvoidanceBehavior で使われる変数群
	/// </summary>
	struct WorkAvoidanceBehavior{
		MotionTimes motionTimes_;

		float moveDistance_;
	};
	/// <summary>
	/// AvoidanceBehavior で使われる変数群
	/// </summary>
	struct WorkWeakAttackBehavior{
		MotionTimes motionTimes_;
		float damageFactor_;
	};
private:
	WorkRootBehavior workRootBehavior_;
	WorkAvoidanceBehavior workAvoidanceBehavior_;
	std::array<WorkWeakAttackBehavior,3> workWeakAttackBehavior_;
public:
	void TransitionBehavior(std::unique_ptr<IPlayerBehavior> next);

	float GetCurrentHP() const{ return currentHP_; }
	float GetMaxHP()     const{ return maxHP_; }

	float GetPower()const{ return power_; }
	void SetDamage(float damageFactor){ damage_ = power_ * damageFactor; }

	void SetIsInvisible(bool invisible){ isInvisible_ = invisible; }
	bool GetIsInvisible()const{ return isInvisible_; }

	const WorkRootBehavior& GetWorkRootBehavior()const{ return workRootBehavior_; }
	const WorkAvoidanceBehavior& GetWorkAvoidanceBehavior()const{ return workAvoidanceBehavior_; }
	const WorkWeakAttackBehavior& GetWorkWeakAttackBehavior(int32_t index){ return workWeakAttackBehavior_[index]; }
	int32_t GetWeakAttackComboMax()const{ return static_cast<int32_t>(workWeakAttackBehavior_.size()); }

	class GameCamera* GetCamera() const { return pGameCamera_; }

};