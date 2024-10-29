#include "EnemyManager.h"

/// engine
#include <imgui.h>

/// objects
#include "Enemy.h"

EnemyManager::EnemyManager() {
	CreateTag(this);
}

EnemyManager::~EnemyManager() {}

void EnemyManager::Initialize() {
	prefabEnemy_ = new Enemy();
	prefabEnemy_->Initialize();
	prefabEnemy_->SetName("enemyPrefab");
}

void EnemyManager::Update() {

}

void EnemyManager::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SeparatorText("prefab enemy");

		ImGui::DragFloat("HP", &prefabEnemy_->hp_);


		ImGui::SeparatorText("tmp");
		
		if(ImGui::Button("create new enemy")) {
			CreateEnemy(prefabEnemy_, Vec3::kOne, {}, {});
		}


		ImGui::TreePop();
	}
}

void EnemyManager::CreateEnemy(Enemy* _prefab, const Vec3& _scale, const Vec3& _rotate, const Vec3& _position) const {
	Enemy* enemy = new Enemy();
	enemy->Initialize();

	enemy->SetScale(_scale);
	enemy->SetPosition(_position);
	enemy->SetRotate(_rotate);

	enemy->hp_ = _prefab->hp_;
}

