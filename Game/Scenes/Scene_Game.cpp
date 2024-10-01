#include "Scene_Game.h"

#include "GameCamera.h"
#include "Player/Player.h"
#include "Particle/ParticleSystem.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {
	
	(new Player)->Initialize();
	(new Player)->Initialize();
	//(new Player)->Initialize();

	/*for(uint32_t i = 0u; i < 1000u; ++i) {
		(new Player)->Initialize();
	}*/


	//mainCamera_->SetPosition({0.0f, 0.0f, -10.0f});
	//mainCamera_->UpdateMatrix();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


