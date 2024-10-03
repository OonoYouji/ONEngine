#include "Scene_Game.h"

#include <ImGuiManager/ImGuiManager.h>

#include "GameCamera.h"
#include "Particle/ParticleSystem.h"
//class
#include"GameCamera.h"
#include "Player/Player.h"
#include"Ground/Ground.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Player* p1 = new Player;
	
	Ground* ground = new Ground;
	p1->Initialize();
	ground->Initialize();
	mainCamera_->Initialize();
	//ペアレント
	mainCamera_->SetParent(p1->GetPivot());
	
	mainCamera_->SetPosition({ 0.0f, -15, -30 });
	mainCamera_->SetRotate({ 45, 0, 0.0f });
	mainCamera_->UpdateMatrix();
	
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
}
