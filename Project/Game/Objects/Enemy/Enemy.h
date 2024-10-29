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

	float hp_;

	float moveStartPoint_;
};
