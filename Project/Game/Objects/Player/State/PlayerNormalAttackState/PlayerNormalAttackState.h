#pragma once

#include "../IPlayerState/IPlayerState.h"

#include "Math/Vector3.h"

class PlayerNormalAttackState : public IPlayerState {
public:

	PlayerNormalAttackState(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerNormalAttackState();

	void Start()  override;
	void Update() override;
	void Exit()   override;

	bool IsEnd() override;
	int NextStateIndex() override;
	
	void Debug() override;

private:

	Vec3 direction_;

	float maxTime_;
	float currentTime_;

	Vec3 startPosition_;

	Vec3 colliderPosition_;
	Vec3 colliderRotate_;
	Vec3 colliderSize_;

	bool isInputNextAttack_;

};