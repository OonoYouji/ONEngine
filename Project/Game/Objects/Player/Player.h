#pragma once

/// std
#include <vector>
#include <array>

/// base
#include "GameObjectManager/GameObjectManager.h"

/// objects
#include "Objects/ShootingCourse/ShootingCourse.h"


/// 前方宣言
class Reticle;


/// ===================================================
/// プレイヤーが操作するエンティティのクラス
/// ===================================================
class Player : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void SetReticle(Reticle* _reticle);

	/// <summary>
	/// 弾の発射に必要なエネルギーのゲッタ
	/// </summary>
	/// <returns></returns>
	float GetFiringGauge() const { return firingGauge_; }


	void AddScore(uint32_t _value);

	void SetScoreObj(class Score* _score);
	void SetDefeatedEnemy(class DefeatedEnemy* _defeatedEnemy);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Reticle* pReticle_ = nullptr;

	/// bullet param
	float bulletSpeed_;

	float firingGauge_ = 100.0f;
	float firingSubValue_ = 5.0f;
	float firingAddValue_ = 0.3f; /// 打っていないときは常に追加される

	class Score* pScore_ = nullptr;
	class DefeatedEnemy* pDefeatedEnemy_ = nullptr;
};