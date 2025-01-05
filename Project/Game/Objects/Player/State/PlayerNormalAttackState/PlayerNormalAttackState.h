#pragma once

#include "../IPlayerState/IPlayerState.h"


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

	

};