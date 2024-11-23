#pragma once

/// std
#include <functional>

/// base class
#include "IPlayerBehavior.h"
#include <Objects/Player/Player.h>

/// ===================================================
/// 強攻撃用 behavior
/// ===================================================
class PlayerStrongAttack : public IPlayerBehavior {

	/// ===================================================
	/// 次のbehavior
	/// ===================================================
	enum NEXT_BEHAVIOR {
		ROOT,               /// 通常に戻る
		NEXT_STRONG_ATTACK, /// 次のチャージ
		LAST_STRONG_ATTACK, /// 攻撃する
		AVOIDANCE,          /// 回避攻撃
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerStrongAttack(Player* _player, int _phase);
	~PlayerStrongAttack() {}

	void Update() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int   phase_;
	int   nextBehavior_;

	bool isFinish_; /// 入力をやめて攻撃または回避したとき

	float currentTime_, maxTime_;

	float damage_;

};
