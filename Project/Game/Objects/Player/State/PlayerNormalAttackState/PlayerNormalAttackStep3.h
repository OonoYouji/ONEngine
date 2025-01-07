#pragma once

/// engine
#include "Math/Vector3.h"

/// user
#include "../IPlayerState/IPlayerState.h"

class PlayerNormalAttackStep3 : public IPlayerState {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerNormalAttackStep3(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerNormalAttackStep3();

	void Start() override;
	void Update() override;
	void Exit() override;
	
	bool IsEnd() override;
	int NextStateIndex() override;

	void Debug() override;
	void Save() override;
	void Load() override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	float maxTime_;
	float currentTime_;

	bool isEnd_;

};