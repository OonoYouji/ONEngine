#include "Scene_Game.h"

#include "GameCamera.h"
#include "Player/Player.h"
#include "Particle/ParticleSystem.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {
	
	(new Player)->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


