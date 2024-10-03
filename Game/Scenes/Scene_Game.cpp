#include "Scene_Game.h"


#include <ImGuiManager/ImGuiManager.h>

#include "GameCamera.h"
#include "Particle/ParticleSystem.h"

#include"Player/PlayerPivot.h"
#include "Player/Player.h"
#include"Ground/Ground.h"



/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Player* p1 = new Player;
	PlayerPivot* playerPivot = new PlayerPivot;
	Ground* ground = new Ground;
	playerPivot->Initialize();
	p1->Initialize();
	ground->Initialize();
	

	p1->SetParent(playerPivot);

	mainCamera_->SetPosition({ 0.0f, 0.0f, -6.49f * 4 });
	mainCamera_->SetRotate({ 0.066f, -0.258f, 0.0f });
	mainCamera_->UpdateMatrix();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


