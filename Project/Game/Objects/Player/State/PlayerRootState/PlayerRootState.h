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

	PlayerRootState(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerRootState();

	void Start()  override;
	void Update() override;
	void Exit()   override;

	bool IsEnd()  override;
	int NextStateIndex() override;

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	void MoveUpdate();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================


};

