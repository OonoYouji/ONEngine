#pragma once

#include "GameObjectManager/BaseGameObject.h"

#include <array>
#include <memory>

#include "CustomMath/MotionTimes.h"

class IPlayerBehavior;
class Player : public BaseGameObject {
public:

	///===============================================
	/// public : sub class, struct
	///===============================================


	/// ---------------------------------------------------
	/// 各 Behavior で 使われる変数群 (基本調整可能なものだけ)
	/// ---------------------------------------------------

	/// <summary>
	/// RootBehavior で使われる変数群
	/// </summary>
	struct WorkRootBehavior {
		float speed_;
		float rotateLerpSensitivity_;
	};

	/// <summary>
	/// AvoidanceBehavior で使われる変数群
	/// </summary>
	struct WorkAvoidanceBehavior {
		MotionTimes motionTimes_;
		float moveDistance_;
	};

	/// <summary>
	/// AvoidanceBehavior で使われる変数群
	/// </summary>
	struct WorkWeakAttackBehavior {
		MotionTimes motionTimes_;
		float damageFactor_;
	};


	/// <summary>
	/// StrongAttackで使用する変数
	/// </summary>
	struct StrongAttackBehavior {
		const std::string    name_ = "StrongAttackBehavior";
		std::array<float, 3> damages_;        /// 各段階ごとのダメージ
		std::array<float, 3> nextChargeTime_; /// 各段階ごとの遷移までの時間
		float                actionTime_;
	};

public:

	Player();
	~Player();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void AddVariables();
	void LoadVariables();
	void ApplyVariables();

	/// <summary>
	/// Enemyの当たり判定に食い込まないように押し戻す
	/// </summary>
	void PushBack();

	void SpawnWeapon();

	void ClampStage();

	void PlayAudio(const std::string& _filePath, float _volume);

private:

	class SphereCollider* sphereCollider_ = nullptr;

	class AnimationRenderer* bodyAnimationRenderer_   = nullptr;
	class AnimationRenderer* weaponAnimationRenderer_ = nullptr;
	std::unique_ptr<IPlayerBehavior> currentBehavior_;

	class AudioSource* audioSource_ = nullptr;

	/// ---------------------------------------------------
	/// status
	/// ---------------------------------------------------

	float currentHP_;
	float maxHP_;

	// boss に与える ダメージ (攻撃していないときは ずっと0 になるようにする)
	float damage_;

	bool isInvisible_ = false;

	float colliderRadius_;
	
	float weaponSpawnTime_, weaponSpawnMaxTime_;
	float startPosY_, endPosY_;
	

	float stageRange_ = 50.0f;;

	/// ---------------------------------------------------
	/// 
	/// ---------------------------------------------------

	Vec2 lastDirection_; /// 最後に向いている方向


	/// ---------------------------------------------------
	/// sub class, struct
	/// ---------------------------------------------------

	WorkRootBehavior                      workRootBehavior_;
	WorkAvoidanceBehavior                 workAvoidanceBehavior_;
	std::array<WorkWeakAttackBehavior, 3> workWeakAttackBehavior_;
	StrongAttackBehavior                  strongAttackBehavior_;

	/// ---------------------------------------------------
	/// other class 
	/// ---------------------------------------------------

	class TrackingCamera*       pTrackingCamera_ = nullptr;
	class Enemy*                pEnemy_          = nullptr;
	class PlayerAttackCollider* attackCollider_  = nullptr;
	class EntityShadow*         entityShadow_    = nullptr;
	class PlayerStrongAttackChargeEffect* strongAttackChargeEffect_ = nullptr;

public:

	/// ---------------------------------------------------
	/// public : method accessor
	/// ---------------------------------------------------

	void TransitionBehavior(std::unique_ptr<IPlayerBehavior> next);

	float GetCurrentHP() const { return currentHP_; }
	float GetMaxHP()     const { return maxHP_; }

	float GetDamage() const { return damage_; }
	void SetDamage(float _damage) { damage_ = _damage; }

	void SetIsInvisible(bool invisible) { isInvisible_ = invisible; }
	bool GetIsInvisible()const { return isInvisible_; }

	const WorkRootBehavior& GetWorkRootBehavior() const { return workRootBehavior_; }
	const WorkAvoidanceBehavior& GetWorkAvoidanceBehavior() const { return workAvoidanceBehavior_; }
	const WorkWeakAttackBehavior& GetWorkWeakAttackBehavior(int32_t index) { return workWeakAttackBehavior_[index]; }
	const StrongAttackBehavior& GetStrongAttackBehavior() const { return strongAttackBehavior_; }

	int32_t GetWeakAttackComboMax() const { return static_cast<int32_t>(workWeakAttackBehavior_.size() - 1); }
	

	void SetEnemy(class Enemy* _enemy);
	void SetTrackingCamera(class TrackingCamera* _trackingCamera);

	class TrackingCamera* GetTrackingCamera() const { return pTrackingCamera_; }
	class Enemy* GetEnemy() const { return pEnemy_; }
	class PlayerAttackCollider* GetAttackCollider() const { return attackCollider_; }
	class PlayerStrongAttackChargeEffect* GetPlayerStrongAttackChargeEffect() const { return strongAttackChargeEffect_; }
	
	void SetAnimationModel(const std::string& _filePath);
	void SetAnimationModel(const std::string& _bodyModelFilePath, const std::string& _weaponModelFilePath);
	void SetAnimationTotalTime(float _totalTime);
	void ResetAnimationTotal();

	void SetAnimationFlags(int _flags, bool _isResetTime = true);

	float GetAnimationDuration();

	const Vec2& GetLastDirection() const { return lastDirection_; }
	void SetLastDirection(const Vec2& _lastDirection) { lastDirection_ = _lastDirection; }

	void SetIsActiveWeapon(bool _isActive);

	float GetColliderRadius() const { return colliderRadius_; }


	void SetAttackMode(int _mode);

};