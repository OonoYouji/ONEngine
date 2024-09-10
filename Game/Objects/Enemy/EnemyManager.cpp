#include "EnemyManager.h"

#include <GameObjectManager.h>
#include <ImGuiManager.h>
#include <Input.h>
#include "Enemy/Enemy.h"
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"



void EnemyManager::Initialize()
{
}

void EnemyManager::Update() {}

void EnemyManager::Debug()
{
	if (ImGui::TreeNodeEx("EnemyManager", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Button("Enemy Pop"))
		{
			Enemy* newEnemy = new Enemy();
			newEnemy->SetWave(dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer")));
			newEnemy->Initialize();
		}

		ImGui::TreePop();
	}
}
