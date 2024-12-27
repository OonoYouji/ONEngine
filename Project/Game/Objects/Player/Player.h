#pragma once

/// std
#include <memory>

/// engine
#include "GameObjectManager/BaseGameObject.h"

/// user
#include "PlayerMesh/PlayerMesh.h"
#include "State/Manager/PlayerStateManager.h"


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

	/// <summary>
	/// 入力からの更新
	/// </summary>
	void InputUpdate();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// 
	PlayerMesh*                         playerMesh_   = nullptr;
	std::unique_ptr<PlayerStateManager> stateManager_ = nullptr;

	/// parameters
	Vec3  direction_ = { 0.0f, 0.0f, 0.0f };
	float moveSpeed_ = 0.1f;

public:
	
	/// ===================================================
	/// public : accessor
	/// ===================================================

	const Vec3& GetDirection() const { return direction_; }

	float GetMoveSpeed() const { return moveSpeed_; }

};
