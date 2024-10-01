#include "Scene_Game.h"

#include "GameCamera.h"
#include "Player/Player.h"
#include "Particle/ParticleSystem.h"
#include"BasePanel/BasePanel.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {
	
	(new Player)->Initialize();
	(new BasePanel)->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


