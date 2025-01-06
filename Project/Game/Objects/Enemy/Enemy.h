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

	Enemy(class Player* _player);
	~Enemy();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) override;
	void OnCollisionStay([[maybe_unused]] BaseGameObject* const collision) override;

	void RootUpdate();
	void AttackUpdate();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer*    meshRenderer_ = nullptr;
	class SphereCollider*  collider_     = nullptr;
	class EnemyHPRenderer* hpRenderer_   = nullptr;

	class Player* pPlayer_ = nullptr;

	/// parameters
	float hp_;
	float maxHP_;

	int stateIndex_;
	Vec3 direction_;

	float attackTime_;

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


	int GetStateIndex() const { return stateIndex_; }

};
