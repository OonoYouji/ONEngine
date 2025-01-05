#pragma once

/// std
#include <memory>
#include <vector>

/// engine
#include "GameObjectManager/BaseGameObject.h"

/// user
#include "PlayerMesh/PlayerMesh.h"
#include "State/Manager/PlayerStateManager.h"
#include "CustomMath/Flag/Flag.h"

enum PlayerFlag {
	PlayerFlag_IsDush,
	PlayerFlag_IsJump,
	PlayerFlag_IsAttack,
	PlayerFlag_IsProtection,
	PlayerFlag_IsTargetButtonPressed,
	PlayerFlag_IsTarget,
	PlayerFlag_Max
};


/// ===================================================
/// プレイヤーのクラス
/// ===================================================
class Player : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Player(class GameCamera* _gameCameraPtr);
	~Player();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	/// <summary>
	/// 入力からの更新
	/// </summary>
	void InputUpdate();

	void ApplyGravity();

	void MeshRotateUpdate();

	/// <summary>
	/// デバッグ用のオブジェクトの初期化
	/// </summary>
	void DebugObjectInitialize();

	
	void UpdateTargetEnemy();
	void UpdateNearEnemy();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// 
	std::unique_ptr<PlayerStateManager> stateManager_ = nullptr;
	PlayerMesh*                         playerMesh_   = nullptr;
	class PlayerProtectionMesh*         protectionMesh_ = nullptr;
	class TargetSpriteRender*           targetSpriteRender_ = nullptr;

	/// parameters
	Vec3  direction_ = { 0.0f, 0.0f, 0.0f };
	Vec3  lastDirection_;
	Vec3  velocity_;
	float moveSpeed_ = 12.0f;

	float gravityAccel_ = 1.0f;

	std::vector<Flag> flags_;


	/// 他クラスへのポインタ
	class GameCamera*   pGameCamera_   = nullptr;
	class EnemyManager* pEnemyManager_ = nullptr;

	class Enemy* nearEnemy_   = nullptr;
	class Enemy* targetEnemy_ = nullptr;

	std::list<class Enemy*> pEnemyList_;
	std::list<class Enemy*> pForwardEnemyList_;


#ifdef _DEBUG


#endif // _DEBUG
public:
	
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ---------------------------------------------------
	/// Direction
	const Vec3& GetDirection() const { return direction_; }

	const Vec3& GetLastDirection() const { return lastDirection_; }
	void SetLastDirection(const Vec3& _direction) { lastDirection_ = _direction; }


	/// ---------------------------------------------------
	/// Velocity
	const Vec3& GetVelocity() const { return velocity_; }
	void SetVelocity(const Vec3& _velocity) { velocity_ = _velocity; }


	float GetMoveSpeed() const { return moveSpeed_; }

	const Flag& GetFlag(PlayerFlag _flag) { return flags_[_flag]; }


	/// ---------------------------------------------------
	/// Mesh
	PlayerMesh* GetMesh() const { return playerMesh_; }

	void SetMeshRotate(const Vec3& _rotate) { playerMesh_->SetRotate(_rotate); }
	

	PlayerStateManager* GetStateManager() const { return stateManager_.get(); }


	/// ---------------------------------------------------
	/// other class
	void SetEnemyManager(class EnemyManager* _enemyManager) { pEnemyManager_ = _enemyManager; }

};
