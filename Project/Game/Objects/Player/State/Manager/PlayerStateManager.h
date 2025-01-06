#pragma once

/// std
#include <vector>
#include <memory>

/// user
#include "../IPlayerState/IPlayerState.h"

/// ===================================================
/// プレイヤーのステートをenumで表現
/// ===================================================
enum PlayerStateOrder {
	PlayerStateOrder_Root,
	PlayerStateOrder_Jump,
	PlayerStateOrder_Dush,
	PlayerStateOrder_Protection,
	PlayerStateOrder_NormalAttack,
	PlayerStateOrder_Max
};


/// ===================================================
/// プレイヤーのステートを管理するクラス
/// ===================================================
class PlayerStateManager final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerStateManager(class Player* _playerPtr);
	~PlayerStateManager();
	
	void Initialize();
	void Update();
	void Debug();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================
	
	class Player*       pPlayer_      = nullptr;
	class IPlayerState* currentState_ = nullptr;

	std::vector<std::unique_ptr<IPlayerState>> states_;

	int beforeState_ = 0;
	int currentStateIndex_ = 0;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	int GetBeforeState() const { return beforeState_; }
	int GetCurrentStateIndex() const { return currentStateIndex_; }

	IPlayerState* GetState(PlayerStateOrder _order) const {
		return states_[_order].get();
	}
};