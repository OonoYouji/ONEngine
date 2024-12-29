#include "EnemyManager.h"

/// external
#include <imgui.h>

#include "../Enemy/Enemy.h"

EnemyManager::EnemyManager() {
	CreateTag(this);
}

EnemyManager::~EnemyManager() {}

void EnemyManager::Initialize() {

}

void EnemyManager::Update() {

}

void EnemyManager::Debug() {

}

void EnemyManager::EmitterEdit() {

	if(ImGui::TreeNode("emitter edit")) {

		if(ImGui::Button("add emitter")) {
			EnemyEmitter* emitter = new EnemyEmitter();
			emitter->Initialize();
			enemyEmitterList_.push_back(emitter);
		}


		/// ---------------------------------------------------
		/// 
		/// ---------------------------------------------------
		if(ImGui::BeginChild("space"), ImVec2(), true, ImGuiWindowFlags_AlwaysVerticalScrollbar) {

			for(EnemyEmitter* emitter : enemyEmitterList_) {
				emitter->Debug();
			}

			ImGui::EndChild();
		}



		ImGui::TreePop();
	}

}

void EnemyManager::GenerateEnemy(const Vec3& _position) {
	Enemy* enemy = new Enemy();
	enemy->Initialize();
	enemy->SetPosition(_position);

	enemy->UpdateMatrix();

	enemyList_.push_back(enemy);
}

