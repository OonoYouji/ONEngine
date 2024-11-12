#pragma once

/// std
#include <functional>
#include <array>

#include "GameObjectManager/GameObjectManager.h"

enum ENEMY_MOVE_STATE {
	ENEMY_MOVE_STATE_MOVE_FORWARD,  /// 前進する
	ENEMY_MOVE_STATE_ROTATION,	    /// クルクル回転する
	ENEMY_MOVE_STATE_MOVE_UP,       /// 上に進む
	ENEMY_MOVE_STATE_COUNT          /// カウント用
};


class Enemy : public BaseGameObject {
	friend class EnemyManager;
public:

	Enemy();
	~Enemy();

	void Initialize() override;
	void Update()     override;

	void OnCollisionEnter(BaseGameObject* const _collision) override;


	void StateMoveForward();
	void StateRatotion();
	void StateMoveUp();


	void SubHP(float _subValue);
	float GetHP() const { return hp_; }

private:

	static std::array<std::function<void(void)>, ENEMY_MOVE_STATE_COUNT> stateUpdateFunctionArray_;


	class MeshRenderer* meshRenderer_ = nullptr;

	int moveState_;

	Vec3  direction_;	 /// 移動方向
	float speed_;        /// 移動スピード

	float hp_;           /// ただのHP
	float updateStartT_; /// プレイヤーのmoveTを参照する、プレイヤーがこれを超えたら更新処理を始める

	float lifeTime_ = 60.0f;

	Vec4 constantColor_ = { 1.0f, 1.0f, 1.0f, 0.5f };
	Vec4 damagedColor_  = { 1.0f, 0.0f, 0.0f, 0.5f };
};
