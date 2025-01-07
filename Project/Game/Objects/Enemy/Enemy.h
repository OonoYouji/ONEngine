#pragma once

#include "GameObjectManager/BaseGameObject.h"

/// ===================================================
/// Enemyのクラス
/// ===================================================
class Enemy : public BaseGameObject {
public:

	enum State {
		State_Root,
		State_Attack,
		State_Blowing,
		State_Max
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Enemy(class Player* _player, class EnemyManager* _manager, class TrackingCamera* _camera);
	~Enemy();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) override;
	void OnCollisionStay([[maybe_unused]] BaseGameObject* const collision) override;

	void RootUpdate();
	void AttackUpdate();
	void BlowingUpdate();

	void Destroy();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer*    meshRenderer_ = nullptr;
	class SphereCollider*  collider_     = nullptr;
	class EnemyHPRenderer* hpRenderer_   = nullptr;
	class EntityShadow*    shadow_       = nullptr;

	class Player* pPlayer_ = nullptr;
	class EnemyManager* pManager_ = nullptr;
	class TrackingCamera* pTrackingCamera_ = nullptr;

	/// parameters
	float hp_;
	float maxHP_;

	int stateIndex_;
	Vec3 direction_;

	float attackTime_;

	Vec3 blowingDirection_;
	float blowingTime_;

	float hittedTime_;

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
