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
	void End()    override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};

