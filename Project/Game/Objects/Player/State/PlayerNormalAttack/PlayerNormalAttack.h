#pragma once

/// std
#include <string>

#include "../IPlayerState/IPlayerState.h"

/// ===================================================
/// プレイヤーの通常攻撃ステート
/// ===================================================
class PlayerNormalAttack : public IPlayerState {

	struct AttackData {
		int   hitCount;
		float hitInterval;
		float damage;
	};


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

	void Debug() override;
	void Save()  override;
	void Load()  override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	float time_;
	float maxTime_;

	AttackData sourceAttackData_ = {
		.hitCount = 1,
		.hitInterval = 0.2f,
		.damage = 10.0f
	};

	AttackData attackData_;
	float hitBoxRadius_;

	bool isEnded_;


	const std::string filePath_ = "./Resources/Parameters/Objects/PlayerNormalAttack.json";
	const std::string groupName_ = "PlayerNormalAttack";
};

