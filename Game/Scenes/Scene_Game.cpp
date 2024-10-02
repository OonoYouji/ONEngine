#include "Scene_Game.h"

#include "GameCamera.h"
#include "Player/Player.h"
#include "Particle/ParticleSystem.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {
	
	(new Player)->Initialize();


	mainCamera_->SetPosition({ 0.0f, 0.0f, -6.49f * 2 });
	mainCamera_->SetRotate({ 0.066f, -0.258f, 0.0f });
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


