#pragma once
#include "GameObjectManager/BaseGameObject.h"

#include <memory>

class IPlayerBehavior;
class Player :
	public BaseGameObject{
public:
	Player();
	~Player();
	void Initialize()override;
	void Update()override;

	void Debug()override;

private:
	class MeshRenderer* meshRenderer_ = nullptr;

	std::unique_ptr<IPlayerBehavior> currentBehavior_;

	///===============================================
	/// status
	///===============================================
	float currentHP_;
	float maxHP_;

	bool isInvisible_ = false;
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
private:
	WorkRootBehavior workRootBehavior_;
	WorkAvoidanceBehavior workAvoidanceBehavior_;
public:
	void TransitionBehavior(std::unique_ptr<IPlayerBehavior> next);

	float GetCurrentHP() const{ return currentHP_; }
	float GetMaxHP()     const{ return maxHP_; }

	void SetIsInvisible(bool invisible){ isInvisible_ = invisible; }
	bool GetIsInvisible()const{ return isInvisible_; }

	const WorkRootBehavior& GetWorkRootBehavior()const{ return workRootBehavior_; }
	const WorkAvoidanceBehavior& GetWorkAvoidanceBehavior()const{ return workAvoidanceBehavior_; }
};