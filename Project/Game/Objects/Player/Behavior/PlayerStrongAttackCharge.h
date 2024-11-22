#pragma once

/// std
#include <functional>

/// base class
#include "IPlayerBehavior.h"

/// ===================================================
/// 強攻撃用 behavior
/// ===================================================
class PlayerStrongAttackCharge : public IPlayerBehavior {

	/// ===================================================
	/// 次のbehavior
	/// ===================================================
	enum NEXT_BEHAVIOR {
		ROOT,          /// 通常に戻る
		NEXT_CHARGE,   /// 次のチャージ
		STRONG_ATTACK, /// 攻撃する
		AVOIDANCE,     /// 回避攻撃
	};


	/// ===================================================
	/// 現在の段階を表すenum
	/// ===================================================
	enum PHASE {
		NONE,    /// 最初のなにもないとき
		FIRST,   /// 一段階目
		SECOND,  /// 二段階目
		THIRD,   /// 三段階目
		RELEASE, /// 離した時
		COUNT,   /// カウント用
	};


	/// ===================================================
	/// 各段階のデータ
	/// ===================================================
	struct ChargePhaseData {
		float damage; /// 定数
		float time;   /// 絶対的な時間
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerStrongAttackCharge(Player* _player, int _phase, float _nextChargeTime);
	~PlayerStrongAttackCharge() {}

	void Update() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int   currentPhase_;
	float currentTime_ = 0.0f;

	bool isChargeMax_;

	int   nextBehavior_;
	float nextChargeTime_; /// 次のチャージまでの時間

	bool isFinish_; /// 入力をやめて攻撃または回避したとき


	float startLagTime_; /// 開始時のラグ
	float endLagTime_;   /// 終了時のラグ

};
