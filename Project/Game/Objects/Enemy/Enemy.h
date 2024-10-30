#pragma once

#include "GameObjectManager/GameObjectManager.h"

class Enemy : public BaseGameObject {
	friend class EnemyManager;
public:

	Enemy();
	~Enemy();

	void Initialize() override;
	void Update()     override;

private:
	class MeshRenderer* meshRenderer_ = nullptr;

	float hp_;           /// ただのHP
	float updateStartT_; /// プレイヤーのmoveTを参照する、プレイヤーがこれを超えたら更新処理を始める
};
