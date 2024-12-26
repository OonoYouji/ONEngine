#pragma once

#include "../IPlayerState/IPlayerState.h"

/// ===================================================
/// 
/// ===================================================
class PlayerRootState : public IPlayerState {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerRootState(class Player* _player);
	~PlayerRootState();

	void Start()  override;
	void Update() override;
	void End()    override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};

