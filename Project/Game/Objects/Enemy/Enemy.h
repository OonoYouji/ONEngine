#pragma once

#include "GameObjectManager/GameObjectManager.h"

class Enemy : public BaseGameObject {
	friend class EnemyManager;
public:

	Enemy();
	~Enemy();

	void Initialize() override;
	void Update()     override;

	void OnCollisionEnter(BaseGameObject* const _collision) override;


	void SubHP(float _subValue);

	float GetHP() const { return hp_; }

private:
	class MeshRenderer* meshRenderer_ = nullptr;

	Vec3  direction_;	 /// 移動方向
	float speed_;        /// 移動スピード

	float hp_;           /// ただのHP
	float updateStartT_; /// プレイヤーのmoveTを参照する、プレイヤーがこれを超えたら更新処理を始める

	float lifeTime_ = 60.0f;
};
