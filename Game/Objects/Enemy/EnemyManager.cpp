#include "EnemyManager.h"

#include <GameObjectManager.h>
#include <ImGuiManager.h>
#include <Input.h>
#include "Enemy/Enemy.h"
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"
#include "GameManager/GameManager.h"


void EnemyManager::Initialize() {


}

void EnemyManager::Update() {

	if(Input::GetInsatnce()->TriggerPadButton(PadCode::RightShoulder)) {
		Enemy* newEnemy = new Enemy();
		newEnemy->SetWave(dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer")));
		newEnemy->Initialize();

		/// 操作方法を書いてあるバインダーが前に出たときに
		/// 動きを止めるリストに追加
		pGameManager_->AddPausedObject(newEnemy);

	}

}

void EnemyManager::Debug() {
	if(ImGui::TreeNodeEx("EnemyManager", ImGuiTreeNodeFlags_DefaultOpen)) {
		if(ImGui::Button("Enemy Pop")) {
			Enemy* newEnemy = new Enemy();
			newEnemy->SetWave(dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer")));
			newEnemy->Initialize();
		}

		ImGui::TreePop();
	}
}

void EnemyManager::SetGameManager(GameManager* gameManager) {
	pGameManager_ = gameManager;
}
