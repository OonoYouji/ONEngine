#pragma once

#include "../IPlayerState/IPlayerState.h"

/// ===================================================
/// 
/// ===================================================
class PlayerJumpState : public IPlayerState {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerJumpState(class Player* _player);
	~PlayerJumpState();

	void Start()  override;
	void Update() override;
	void Exit()   override;

	bool IsEnd()  override;
	int NextStateIndex() override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};

