#pragma once

#include "../IPlayerState/IPlayerState.h"

#include "Math/Vector3.h"

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
	/// private : methods
	/// ===================================================

	void VelocitySetting();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class Transform* playerTransform_ = nullptr;

	float inputTimeLimit_;
	bool  canDoubleJump_;

};

