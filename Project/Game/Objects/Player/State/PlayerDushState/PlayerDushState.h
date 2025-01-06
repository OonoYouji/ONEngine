#pragma once

#include "../IPlayerState/IPlayerState.h"

/// ===================================================
/// プレイヤーのダッシュステート
/// ===================================================
class PlayerDushState : public IPlayerState {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerDushState(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerDushState();

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

	float dushStartSpeed_;
	float dushNormalSpeed_;

	float dushTransitionTime_;
	float currentTime_;

	bool isAccelerateEnded_ = false;

};

