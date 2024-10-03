#include "Scene_Game.h"


#include <ImGuiManager/ImGuiManager.h>

#include "GameCamera.h"
#include "Particle/ParticleSystem.h"
//class
#include"Player/PlayerPivot.h"
#include"GameCamera.h"
#include "Player/Player.h"
#include"Ground/Ground.h"



/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Player* p1 = new Player;
	PlayerPivot* playerPivot = new PlayerPivot;
	/*GameCamera* gameCamera = new GameCamera("PlayerCamear");*/
	Ground* ground = new Ground;
	playerPivot->Initialize();
	p1->Initialize();
	ground->Initialize();
	/*gameCamera->Initialize();*/
	

	p1->SetParent(playerPivot);
	/*mainCamera_->SetParent(p1);*/

	mainCamera_->SetPosition({ 0.0f, 0.0f, -6.49f * 4 });
	mainCamera_->SetRotate({ 0.066f, -0.258f, 0.0f });
	mainCamera_->UpdateMatrix();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


