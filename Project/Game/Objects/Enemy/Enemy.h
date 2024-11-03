#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Player;
class MeshRenderer;
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

	MeshRenderer* meshRenderer_ = nullptr;

	Vector2 direction_;
	float speed_;
public:

};