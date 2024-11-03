#include "Enemy.h"

#include "../Player/Player.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Enemy::Enemy(Player* _palyer):BaseGameObject(){
	CreateTag(this);
	player_ = _palyer;
}

Enemy::~Enemy(){}

void Enemy::Initialize(){
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Sphere");
}

void Enemy::Update(){}

void Enemy::Debug(){}
