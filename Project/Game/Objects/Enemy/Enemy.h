#pragma once

#include <memory>

#include "GameObjectManager/BaseGameObject.h"

class Player;
class MeshRenderer;
class EnemyAI;

class Enemy :
	public BaseGameObject{
public:
	Enemy(Player* _palyer);
	~Enemy();
	void Initialize()override;
	void Update()override;

	void Debug()override;

private:
	Player* player_;

	std::unique_ptr<EnemyAI> ai_;

	MeshRenderer* meshRenderer_ = nullptr;

	float speed_;
	Vector3 velocity_;
};