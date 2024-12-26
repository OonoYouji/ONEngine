#pragma once

#include "GameObjectManager/BaseGameObject.h"

/// ===================================================
/// Enemyのクラス
/// ===================================================
class Enemy : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Enemy();
	~Enemy();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer*    meshRenderer_ = nullptr;
	class EnemyHPRenderer* hpRenderer_ = nullptr;

	/// parameters
	float hp_;
	float maxHP_;


public:


	/// <summary>
	/// 現在のHPを取得
	/// </summary>
	/// <returns></returns>
	float GetHP() const { return hp_; }

	/// <summary>
	/// HPの最大値を取得
	/// </summary>
	/// <returns></returns>
	float GetMaxHP() const { return maxHP_; }

};
