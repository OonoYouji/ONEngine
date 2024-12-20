#include "EnemyManager.h"

#include "../Enemy/Enemy.h"

EnemyManager::EnemyManager() {
	CreateTag(this);
}

EnemyManager::~EnemyManager() {}

void EnemyManager::Initialize() {

}

void EnemyManager::Update() {

}

void EnemyManager::GenerateEnemy(const Vec3& _position) {
	Enemy* enemy = new Enemy();
	enemy->Initialize();
	enemy->SetPosition(_position);

	enemy->UpdateMatrix();

	enemyList_.push_back(enemy);
}

