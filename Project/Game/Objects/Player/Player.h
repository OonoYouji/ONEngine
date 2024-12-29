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

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// 
	PlayerMesh*                         playerMesh_   = nullptr;
	std::unique_ptr<PlayerStateManager> stateManager_ = nullptr;

	/// parameters
	Vec3  direction_ = { 0.0f, 0.0f, 0.0f };
	Vec3  lastDirection_;
	Vec3  velocity_;
	float moveSpeed_ = 12.0f;

	float gravityAccel_ = 1.0f;

	std::vector<Flag> flags_;


	/// 他クラスへのポインタ
	class GameCamera* pGameCamera_ = nullptr;

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
	

};
