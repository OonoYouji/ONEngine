#pragma once

#include "../IPlayerState/IPlayerState.h"

/// ===================================================
/// 
/// ===================================================
class PlayerDushState : public IPlayerState {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerDushState(class Player* _player);
	~PlayerDushState();

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

