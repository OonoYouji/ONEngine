#pragma once

/// std
#include <string>
#include <vector>

#include "../IPlayerState/IPlayerState.h"

#include "Math/Vector3.h"

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

	std::vector<Vec3> bouncePositions_; /// バウンドする位置
	
	float hitBoxRadius_;

	bool isEnded_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	const std::vector<Vec3>& GetBouncePositions() { return bouncePositions_; }

};

