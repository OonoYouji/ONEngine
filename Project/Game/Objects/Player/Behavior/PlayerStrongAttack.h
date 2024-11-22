#pragma once

/// std
#include <functional>

/// base class
#include "IPlayerBehavior.h"

/// ===================================================
/// 強攻撃用 behavior
/// ===================================================
class PlayerStrongAttack : public IPlayerBehavior {

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

	PlayerStrongAttack(Player* _player);
	~PlayerStrongAttack() {}

	void Update() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int                                currentPhase_;
	float                              currentTime_ = 0.0f;
	std::array<ChargePhaseData, COUNT> chargePhaseDataArray_;

	bool isChargeMax_;


	float startLagTime_; /// 開始時のラグ
	float endLagTime_;   /// 終了時のラグ

};