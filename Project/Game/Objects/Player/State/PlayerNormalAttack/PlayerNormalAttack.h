#pragma once

#include "../IPlayerState/IPlayerState.h"

/// ===================================================
/// プレイヤーの通常攻撃ステート
/// ===================================================
class PlayerNormalAttack : public IPlayerState {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerNormalAttack(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerNormalAttack();

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

